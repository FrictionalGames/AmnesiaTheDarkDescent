/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "resources/GpuShaderManager.h"

#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "system/PreprocessParser.h"
#include "system/Platform.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/GPUShader.h"

#include "resources/FileSearcher.h"

#ifdef WIN32
#include <io.h>
#endif

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGpuShaderManager::cGpuShaderManager(cFileSearcher *apFileSearcher,iLowLevelGraphics *apLowLevelGraphics, 
		iLowLevelResources *apLowLevelResources,iLowLevelSystem *apLowLevelSystem)
		: iResourceManager(apFileSearcher, apLowLevelResources,apLowLevelSystem)
	{
		mpLowLevelGraphics = apLowLevelGraphics;

		mpPreprocessParser = hplNew(cPreprocessParser, () );

		mpPreprocessParser->GetEnvVarContainer()->Add("ScreenWidth",mpLowLevelGraphics->GetScreenSizeInt().x);
		mpPreprocessParser->GetEnvVarContainer()->Add("ScreenHeigth",mpLowLevelGraphics->GetScreenSizeInt().y);

		#ifdef WIN32
			mpPreprocessParser->GetEnvVarContainer()->Add("OS_Windows");
		#elif defined(__APPLE__)
			mpPreprocessParser->GetEnvVarContainer()->Add("OS_OSX");
		#elif defined(__linux__)
			mpPreprocessParser->GetEnvVarContainer()->Add("OS_Linux");
		#endif
	}

	cGpuShaderManager::~cGpuShaderManager()
	{
		hplDelete(mpPreprocessParser);

		DestroyAll();

		Log(" Done with Gpu programs\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGpuShaderManager::CheckFeatureSupport()
	{
		////////////////////////////
		//Shader model variables
		if(mpLowLevelGraphics->GetCaps(eGraphicCaps_ShaderModel_2))		mpPreprocessParser->GetEnvVarContainer()->Add("ShaderModel_2");
		if(mpLowLevelGraphics->GetCaps(eGraphicCaps_ShaderModel_3))		mpPreprocessParser->GetEnvVarContainer()->Add("ShaderModel_3");
		if(mpLowLevelGraphics->GetCaps(eGraphicCaps_ShaderModel_4))		mpPreprocessParser->GetEnvVarContainer()->Add("ShaderModel_4");

		/////////////////////////
		// Test Feature support
		if(IsShaderSupported("_test_array_support_frag.glsl", eGpuShaderType_Fragment)==false)
		{
			Log("ATTENTION: System does not support const arrays in glsl!\n");
			mpPreprocessParser->GetEnvVarContainer()->Add("FeatureNotSupported_ConstArray");
		}
	}

	//-----------------------------------------------------------------------

	iGpuShader* cGpuShaderManager::CreateShader(const tString& asName, eGpuShaderType aType,
												cParserVarContainer *apVarContainer)
	{
		iGpuShader* pShader;

		BeginLoad(asName);

		/////////////////////////////////////////
        // If we have a variable container do NOT add the shader as a resource!
		if(apVarContainer)
		{
			tString sFileData;
			tString sParsedOutput;
		
			/////////////////////////////////
			//Get file from file searcher
			tWString sPath = mpFileSearcher->GetFilePath(asName);
			if(sPath==_W("")){
				Error("Couldn't find file '%s' in resources\n",asName.c_str());
				EndLoad();
				return NULL;
			}

			/////////////////////////////////
			//Load data
			unsigned int lFileSize = cPlatform::GetFileSize(sPath);

			sFileData.resize(lFileSize);
			cPlatform::CopyFileToBuffer(sPath,&sFileData[0],lFileSize);

			/////////////////////////////////
			//Parse file
			mpPreprocessParser->Parse(&sFileData, &sParsedOutput,apVarContainer,cString::GetFilePathW(sPath));
			
			/////////////////////////////////
			//Compile
			pShader = mpLowLevelGraphics->CreateGpuShader(asName, aType);
			pShader->SetFullPath(sPath);
			
			if(pShader->CreateFromString(sParsedOutput.c_str())==false)
			{
				Error("Couldn't create program '%s'\n",asName.c_str());
				hplDelete(pShader);
				EndLoad();
				return NULL;
			}

			/////////////////////////////////
			//Sampler to texture units setup, if needed
			if(aType == eGpuShaderType_Fragment && pShader->SamplerNeedsTextureUnitSetup())
			{
				tParseVarMap *pVarMap = mpPreprocessParser->GetParsingVarContainer()->GetMapPtr();
				tParseVarMapIt varIt = pVarMap->begin();
				for(; varIt != pVarMap->end(); ++varIt)
				{
					const tString& sVarName = varIt->first;
					const tString& sVarVal = varIt->second;
					if(sVarName == "") continue;
                    
					tStringVec vStrings;
					tString sSepp = "_";
					cString::GetStringVec(sVarName,vStrings,&sSepp);
					if(vStrings.size()>=2 && vStrings[0]=="sampler")
					{
						int lUnit = cString::ToInt(sVarVal.c_str(), 0);
						
						pShader->AddSamplerUnit(vStrings[1], lUnit);
					}
					
				}
			}
		}
		/////////////////////////////////////////
		// Normal resource load
		else
		{
			tWString sPath;
			pShader = static_cast<iGpuShader*>(FindLoadedResource(asName,sPath));

			if(pShader==NULL && sPath!=_W(""))
			{
				pShader = mpLowLevelGraphics->CreateGpuShader(asName, aType);

				if(pShader->CreateFromFile(sPath)==false)
				{
					Error("Couldn't create program '%s'\n",asName.c_str());
					hplDelete(pShader);
					EndLoad();
					return NULL;
				}

				AddResource(pShader);
			}

			if(pShader)pShader->IncUserCount();
			else Error("Couldn't load program '%s'\n",asName.c_str());
		}
		
		
		EndLoad();
		return pShader;
     }

	//-----------------------------------------------------------------------

	void cGpuShaderManager::Unload(iResourceBase* apResource)
	{

	}
	//-----------------------------------------------------------------------

	void cGpuShaderManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();

		if(apResource->HasUsers()==false){
			RemoveResource(apResource);
			hplDelete(apResource);
		}
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cGpuShaderManager::IsShaderSupported(const tString& asName, eGpuShaderType aType)
	{
		/////////////////////////////////
		//Get file from file searcher
		tWString sPath = mpFileSearcher->GetFilePath(asName);
		if(sPath==_W("")){
			Error("Couldn't find test file '%s' in resources\n",asName.c_str());
			return false;
		}

		/////////////////////////////////
		//Compile
		iGpuShader* pShader = mpLowLevelGraphics->CreateGpuShader(asName, aType);
		
		bool bRet = pShader->CreateFromFile(sPath, "main", false);
		hplDelete(pShader);
		
		return bRet;
	}
	
	//-----------------------------------------------------------------------
}
