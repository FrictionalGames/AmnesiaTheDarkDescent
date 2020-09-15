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

#include "graphics/ProgramComboManager.h"

#include "system/LowLevelSystem.h"
#include "system/PreprocessParser.h"

#include "graphics/Graphics.h"
#include "graphics/GPUShader.h"
#include "graphics/GPUProgram.h"
#include "graphics/LowLevelGraphics.h"

#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/GpuShaderManager.h"

#include "math/Math.h"

#include "system/String.h"

namespace hpl {

	const static bool gbLogStuff = false;

	//////////////////////////////////////////////////////////////////////////
	// PROGRAM
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cProgramComboProgram::DestroyProgram()
	{
		if(mpProgram==NULL) return;

		hplDelete(mpProgram);
		mpProgram = NULL;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cProgramComboManager::cProgramComboManager(const tString& asName,cGraphics *apGraphics, cResources *apResources, int alNumOfMainModes)
	{
		mpGraphics = apGraphics;
		mpResources = apResources;

		msName = asName;

		mlNumOfMainModes = alNumOfMainModes;
		
		mvCombinationNum.resize(mlNumOfMainModes);
		
		mvProgramSets.resize(mlNumOfMainModes);
		mvVtxShaderSets.resize(mlNumOfMainModes);
		mvFragShaderSets.resize(mlNumOfMainModes);

		mvVarLists.resize(mlNumOfMainModes);
		mvSettings.resize(mlNumOfMainModes);
	}

	cProgramComboManager::~cProgramComboManager()
	{
		DestroyShadersAndPrograms();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//--------------------------------------------------------------------------

	iGpuProgram* cProgramComboManager::GenerateProgram(int alMainMode, int alFlags)
	{
		cProgramComboProgram *pProgData = NULL;

		tProgramComboProgramMapIt it = mvProgramSets[alMainMode].find(alFlags);

		////////////////////////////////////
		// Create program
		if(it == mvProgramSets[alMainMode].end())
		{
			// Create data and add to map
			pProgData = hplNew(cProgramComboProgram, () );
			mvProgramSets[alMainMode].insert(tProgramComboProgramMap::value_type(alFlags, pProgData));
			
			cProgramComboSettings& comboSettings = mvSettings[alMainMode];
			tString sProgramName = GenerateProgramName(alMainMode, alFlags);
			
			////////////////////////////////////
			// Create (or get) shaders
			iGpuShader *pVtxShader = GetShaderForCombo(alMainMode, alFlags, comboSettings.msVtxShader, kPC_VertexBit);			
			iGpuShader *pFragShader = GetShaderForCombo(alMainMode, alFlags, comboSettings.msFragShader, kPC_FragmentBit);

			////////////////////////////////////
			// Create program
			iGpuProgram* pProgram = NULL;
			
			if(pVtxShader && pFragShader)
				pProgram = CreateProgramFromShaders(sProgramName, pVtxShader, pFragShader,false);
			
			pProgData->mpProgram = pProgram;
						
			////////////////////////////////////
			// Init variables
			if(pProgram)
			{
				pProgram->SetUserId(alFlags);

				tProgramComboVariableListIt it = mvVarLists[alMainMode].begin();
				tProgramComboVariableListIt endIt = mvVarLists[alMainMode].end();
				for(; it != endIt; ++it)
				{
					cProgramComboVariable &var = *it;
					pProgram->GetVariableAsId(var.msVarName,var.mlId);		                
				}

				//Log(" -- Creating program '%s' / %d\n", sProgramName.c_str(), pProgram);
			}
		}
		//////////////////////////////////////
		// Program exists, get from list
		else
		{
			pProgData = it->second;
		}

		//////////////////////////////////////
		// Increase user count
		pProgData->mlUserCount++;
		
		return pProgData->mpProgram;
	}

	//--------------------------------------------------------------------------

	
	void cProgramComboManager::SetupGenerateProgramData(int alMainMode, const tString &asModeName,
														const tString &asVtxShaderName, const tString &asFragShaderName,
														cProgramComboFeature *apFeatures, int alFeatureNum, 
														cParserVarContainer &avDefaultVars)
	{
		////////////////////////////////
		//Set up variables and containers
		int lFeatureNum = alFeatureNum;
		int lCombinations = (int)(pow(2.0,(double)lFeatureNum)+0.49);
		mvCombinationNum[alMainMode] = lCombinations;
				
		////////////////////////////////
		//Save settings
		cProgramComboSettings& comboSettings = mvSettings[alMainMode];
		comboSettings.msName = asModeName;
		comboSettings.msVtxShader = asVtxShaderName;
		comboSettings.msFragShader = asFragShaderName;
		
		comboSettings.mvFeatures.resize(alFeatureNum);
		for(int i=0;i<alFeatureNum; ++i)
			comboSettings.mvFeatures[i] = apFeatures[i];		
		
		tParseVarMap* pVarMap = avDefaultVars.GetMapPtr();
		comboSettings.mvDefaultVars.reserve(pVarMap->size());

		tParseVarMapIt it = pVarMap->begin();
		for(; it != pVarMap->end(); ++it)
		{
			comboSettings.mvDefaultVars.push_back(cProgramComboSettingsVar(it->first, it->second));
		}
	}
	
	//-----------------------------------------------------------------------

	void cProgramComboManager::AddGenerateProgramVariableId(const tString& asVarName, int alId, int alMainMode)
	{
		mvVarLists[alMainMode].push_back(cProgramComboVariable(asVarName, alId));
	}

	//-----------------------------------------------------------------------

	void cProgramComboManager::DestroyGeneratedProgram(int alMainMode, iGpuProgram* apProgram)
	{
		tProgramComboProgramMapIt it = mvProgramSets[alMainMode].find(apProgram->GetUserId());
		if(it == mvProgramSets[alMainMode].end())
		{
			Error("Could not find and destroy generated program: %s\n", apProgram->GetName().c_str());
			return;
		}

		cProgramComboProgram *pProgData = it->second;
		iGpuProgram *pProgram = pProgData->mpProgram;
		
		/////////////////////////
		//Dec user count and destroy if 0
		pProgData->mlUserCount--;
		if(pProgData->mlUserCount <= 0)
		{
			iGpuShader *pVtxShader = pProgram->GetShader(eGpuShaderType_Vertex);
			iGpuShader *pFragShader = pProgram->GetShader(eGpuShaderType_Fragment);
			
			/////////////////////////
			//Destroy program
			//Log("    Destroying program '%s'/%d id: %d\n", pProgram->GetName().c_str(),pProgram, pProgram->GetUserId());
			mvProgramSets[alMainMode].erase(it);
			if(pProgram) hplDelete(pProgram);
			hplDelete(pProgData);

			/////////////////////////
			//Destroy shaders (must be done after program deletion!)
			if(pVtxShader)
				DestroyGeneratedShader(alMainMode,pVtxShader , eGpuShaderType_Vertex);
			if(pFragShader)
				DestroyGeneratedShader(alMainMode,pFragShader , eGpuShaderType_Fragment);
		}
	}

	//-----------------------------------------------------------------------

	void cProgramComboManager::DestroyGeneratedShader(int alMainMode, iGpuShader* apShader, eGpuShaderType aType)
	{
		tProgramComboShaderMap* pShaderSet = aType== eGpuShaderType_Vertex ? &mvVtxShaderSets[alMainMode] : &mvFragShaderSets[alMainMode];
		
		/////////////////////////
		//Search for shader using id (which is the saved bitflags)
		tProgramComboShaderMapIt it = pShaderSet->find(apShader->GetUserId());
		if(it == pShaderSet->end())
		{
			Error("Could not find and destroy generated shader: '%s' with id %d in combo manager '%s'\n", apShader->GetName().c_str(), apShader->GetUserId(), msName.c_str());
			return;
		}

		cProgramComboShader *pShaderData = it->second;
		iGpuShader *pShader = pShaderData->mpShader;

		/////////////////////////
		//Dec user count and destroy if 0
		pShaderData->mlUserCount--;
		if(pShaderData->mlUserCount <=0)
		{
			//Log(" Destroying shader '%s' id: %d\n", pShader->GetName().c_str(), pShader->GetUserId());

			pShaderSet->erase(it);
			if(pShader) mpResources->GetGpuShaderManager()->Destroy(pShader);
			hplDelete(pShaderData);
		}

	}

	//-----------------------------------------------------------------------

	iGpuProgram *cProgramComboManager::CreateProgram(const tString& asName, bool abAddtoList)
	{
		iGpuProgram *pProgram = mpGraphics->GetLowLevel()->CreateGpuProgram(asName);
		pProgram->SetAutoDestroyShaders(false);
		if(abAddtoList)
		{
			mlstExtraPrograms.push_back(pProgram);
		}

		return pProgram;
	}

	//-----------------------------------------------------------------------

	iGpuShader *cProgramComboManager::CreateShader(const tString& asName,eGpuShaderType aType, cParserVarContainer *apVars, bool abAddtoList)
	{
		iGpuShader *pShader = mpResources->GetGpuShaderManager()->CreateShader(asName,aType,apVars);
		if(pShader == NULL)
		{
			Error("Could not load material '%s' shader '%s'\n",msName.c_str(),asName.c_str());
			return NULL;
		}

		if(pShader && abAddtoList) 
		{
			mlstExtraShaders.push_back(pShader);
		}

		return pShader;
	}

	//-----------------------------------------------------------------------

	void cProgramComboManager::DestroyShader(iGpuShader * apShader)
	{
		if(apShader == NULL) return;

		STLFindAndRemove(mlstExtraShaders, apShader);
		
		mpResources->GetGpuShaderManager()->Destroy(apShader);
	}

	//-----------------------------------------------------------------------

	void cProgramComboManager::DestroyProgram(iGpuProgram * apProgram)
	{
		STLFindAndDelete(mlstExtraPrograms, apProgram);
	}

	//-----------------------------------------------------------------------

	iGpuProgram* cProgramComboManager::CreateProgramFromShaders(const tString &asProgramName,
																const tString &asVtxShaderName, 
																const tString &asFragShaderName,
																cParserVarContainer *apVars,
																bool abAddtoList)
	{
		iGpuProgram *pProgram;
		iGpuShader *pVtxShader, *pFragShader;

		pVtxShader = CreateShader(asVtxShaderName,eGpuShaderType_Vertex,apVars, abAddtoList);
		if(pVtxShader == NULL) return NULL;

		pFragShader = CreateShader(asFragShaderName,eGpuShaderType_Fragment,apVars, abAddtoList);
		if(pFragShader == NULL){
			DestroyShader(pVtxShader);
			return NULL;
		}

		pProgram = CreateProgram(asProgramName, abAddtoList);
		pProgram->SetShader(eGpuShaderType_Vertex, pVtxShader);
		pProgram->SetShader(eGpuShaderType_Fragment, pFragShader);

		if(pProgram->Link()==false)
		{
			Error("Could not link material '%s' program %s!\n",msName.c_str(),asProgramName.c_str());
			//////////////////
			// Remove from list and destroy
			if(abAddtoList)
			{
				DestroyShader(pVtxShader);
				DestroyShader(pFragShader);
				DestroyProgram(pProgram);
			}
			//////////////////
			// Just destroy
			else
			{
				mpResources->GetGpuShaderManager()->Destroy(pVtxShader);
				mpResources->GetGpuShaderManager()->Destroy(pFragShader);
				hplDelete(pProgram);		
			}

			return NULL;
		}

		if(abAddtoList==false)
		{
			pProgram->SetAutoDestroyShaders(true);
			pProgram->SetResources(mpResources);
		}

		return pProgram;
	}

	//-----------------------------------------------------------------------

	iGpuProgram* cProgramComboManager::CreateProgramFromShaders(const tString &asProgramName, iGpuShader *apVtxShader,iGpuShader *apFragShader, 
																bool abAddtoList)
	{
		iGpuProgram *pProgram;

		pProgram = CreateProgram(asProgramName, abAddtoList);
		pProgram->SetShader(eGpuShaderType_Vertex, apVtxShader);
		pProgram->SetShader(eGpuShaderType_Fragment, apFragShader);

		if(pProgram->Link()==false)
		{
			Error("Could not link program '%s' in ProgramComboManager!\n",asProgramName.c_str());
			if(abAddtoList)
				DestroyProgram(pProgram);
			else
				hplDelete(pProgram);
			return NULL;
		}

		return pProgram;
	}

	
	//-----------------------------------------------------------------------

	void cProgramComboManager::DestroyShadersAndPrograms()
	{
		for(int rmode =0; rmode < mlNumOfMainModes; ++rmode)
		{
			///////////////////////////////
			// Destroy Generated Content

			//Programs
			for(tProgramComboProgramMapIt it = mvProgramSets[rmode].begin(); it != mvProgramSets[rmode].end(); ++it)
			{
				cProgramComboProgram *pProgramData = it->second;
				pProgramData->DestroyProgram();
			}
			STLMapDeleteAll(mvProgramSets[rmode]);

			//Shaders (must be after programs!)
			for(tProgramComboShaderMapIt it = mvVtxShaderSets[rmode].begin(); it != mvVtxShaderSets[rmode].end(); ++it)
				if(it->second->mpShader)	mpResources->GetGpuShaderManager()->Destroy(it->second->mpShader);

			for(tProgramComboShaderMapIt it = mvFragShaderSets[rmode].begin(); it != mvFragShaderSets[rmode].end(); ++it)
				if(it->second->mpShader)	mpResources->GetGpuShaderManager()->Destroy(it->second->mpShader);

			STLMapDeleteAll(mvVtxShaderSets[rmode]);
			STLMapDeleteAll(mvFragShaderSets[rmode]);

			mvVarLists[rmode].clear();

			///////////////////////////////
			// Destroy Extra Content
			
			//Programs
			STLDeleteAll(mlstExtraPrograms);

			//Shaders (must be after programs!)
			for(tGpuShaderListIt it = mlstExtraShaders.begin(); it!= mlstExtraShaders.end(); ++it)
			{
				mpResources->GetGpuShaderManager()->Destroy(*it);	
			}
			mlstExtraShaders.clear();
		}
	}

	//--------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//--------------------------------------------------------------------------


	tString cProgramComboManager::GenerateProgramName(int alMainMode, int alBitFlags)
	{
		cProgramComboSettings& settings = mvSettings[alMainMode];

		tString sName = msName + "_" + settings.msName;
		for(size_t i=0; i<settings.mvFeatures.size(); ++i)
		{
			if(alBitFlags & cMath::GetFlagBit((int)i)) 
			{
				sName += "_" + settings.mvFeatures[i].msVariable;
			}
		}

		return sName;
	}

	//--------------------------------------------------------------------------


	iGpuShader* cProgramComboManager::GetShaderForCombo(int alMainMode, int alBitFlags, const tString& asShaderName, tFlag aShaderType)
	{
		cProgramComboSettings &comboSettings = mvSettings[alMainMode];

		//////////////////////////////
		// Calculate the valid bit flags for current shader type
		// Valid means that only bits use the correct shader type is part of bit string.
		int lValidBits =0;
		for(size_t i=0; i<comboSettings.mvFeatures.size(); ++i)
		{
			int lCurrentBit = cMath::GetFlagBit((int)i);
			if( (alBitFlags & lCurrentBit) &&
				(comboSettings.mvFeatures[i].mlShaders & aShaderType) )
			{
				lValidBits |= lCurrentBit; 
			}
		}
		
		//////////////////////////////
		// Check if program exists and if so return it
		tProgramComboShaderMap* pShaderSet = aShaderType== kPC_VertexBit ? &mvVtxShaderSets[alMainMode] : &mvFragShaderSets[alMainMode];
		tProgramComboShaderMapIt it = pShaderSet->find(lValidBits);
		if(it != pShaderSet->end())
		{
			cProgramComboShader *pShaderData = it->second;
			
			pShaderData->mlUserCount++;
			return pShaderData->mpShader;
		}

		//////////////////////////////
		// Create program, add to set and return it
		cProgramComboShader *pShaderData = hplNew(cProgramComboShader , ());
		iGpuShader* pShader = CreateShaderFromFeatures(	asShaderName, aShaderType, alBitFlags, 
														&comboSettings.mvFeatures[0], (int)comboSettings.mvFeatures.size(), 
														&comboSettings.mvDefaultVars[0], (int)comboSettings.mvDefaultVars.size());
		pShaderData->mpShader = pShader;
		pShaderData->mlUserCount++;
		pShaderSet->insert(tProgramComboShaderMap::value_type(lValidBits, pShaderData));

		if(pShader) pShader->SetUserId(lValidBits);

		//tString sTempName = asShaderName +"_"+GenerateProgramName(alMainMode, lValidBits);
		//Log("Created shader '%s' id: %d\n", pShader->GetName().c_str(), pShader->GetUserId());

		return pShader;
	}

	//--------------------------------------------------------------------

	iGpuShader* cProgramComboManager::CreateShaderFromFeatures(	const tString& asShaderFile, tFlag aShaderType,
																int alBitFlags, cProgramComboFeature* apFeatures, int alFeatureNum, 
																cProgramComboSettingsVar *apDefaultVars, int alDefaultVarsNum)
	{
		cParserVarContainer vars;

		///////////////////////////
		//Add variables to variable container
		for(int lFeature=0; lFeature<alFeatureNum; ++lFeature)
		{
			//This check if feature j in in current flag
			if( alBitFlags & cMath::GetFlagBit(lFeature))
			{
				if(apFeatures[lFeature].mlShaders & aShaderType)
				{
					vars.Add(apFeatures[lFeature].msVariable);
				}
			}
		}
		
		/////////////////////////
		//Add the default variables,
		for(int i=0; i<alDefaultVarsNum; ++i)
		{
			vars.Add(apDefaultVars[i].msName, apDefaultVars[i].msValue);
		}

		/////////////////////////
		//Create the shader
		eGpuShaderType shaderType = aShaderType == kPC_VertexBit ? eGpuShaderType_Vertex : eGpuShaderType_Fragment;
		return CreateShader(asShaderFile,shaderType,&vars,false);
	}

	//--------------------------------------------------------------------------

	
	

}
