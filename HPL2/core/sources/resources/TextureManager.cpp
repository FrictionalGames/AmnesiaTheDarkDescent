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

#include "resources/TextureManager.h"
#include "system/String.h"
#include "graphics/Graphics.h"
#include "resources/Resources.h"
#include "graphics/Texture.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/LowLevelResources.h"
#include "system/LowLevelSystem.h"
#include "resources/FileSearcher.h"
#include "graphics/Bitmap.h"
#include "resources/BitmapLoaderHandler.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cTextureManager::cTextureManager(cGraphics* apGraphics,cResources *apResources)
		: iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
							apResources->GetLowLevelSystem())
	{
		mpGraphics = apGraphics;
		mpResources = apResources;
		
		mpBitmapLoaderHandler = mpResources->GetBitmapLoaderHandler();

		mlMemoryUsage =0;
		
		mvCubeSideSuffixes.push_back("_pos_x");
		mvCubeSideSuffixes.push_back("_neg_x");
		mvCubeSideSuffixes.push_back("_pos_y");
		mvCubeSideSuffixes.push_back("_neg_y");
		mvCubeSideSuffixes.push_back("_pos_z");
		mvCubeSideSuffixes.push_back("_neg_z");
	}

	cTextureManager::~cTextureManager()
	{
		STLMapDeleteAll(m_mapAttenuationTextures);
		DestroyAll();
		Log(" Destroyed all textures\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::Create1D(const tString& asName,bool abUseMipMaps, 
										eTextureUsage aUsage, unsigned int alTextureSizeLevel)
	{
		return CreateSimpleTexture(asName,abUseMipMaps,aUsage, eTextureType_1D,alTextureSizeLevel);
	}

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::Create2D(const tString& asName,bool abUseMipMaps, eTextureType aType,
										eTextureUsage aUsage, unsigned int alTextureSizeLevel)
	{
		return CreateSimpleTexture(asName,abUseMipMaps,aUsage, aType,alTextureSizeLevel);
	}

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::Create3D(const tString& asName,bool abUseMipMaps, eTextureUsage aUsage,
										unsigned int alTextureSizeLevel)
	{
		return CreateSimpleTexture(asName,abUseMipMaps,aUsage, eTextureType_3D,alTextureSizeLevel);
	}

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::CreateAnim(const tString& asFirstFrameName,bool abUseMipMaps,eTextureType aType,
											eTextureUsage aUsage, unsigned int alTextureSizeLevel)
	{
		BeginLoad(asFirstFrameName);
		
		///////////////////////////
		//Check the base name
		int lPos = cString::GetFirstStringPos(asFirstFrameName, "01");
		if(lPos <0)
		{
			Error("First frame of animation '%s' must contain '01'!\n", asFirstFrameName.c_str());
			return NULL;
		}

		//Remove 01 in the string
		tString sSub1 = cString::Sub(asFirstFrameName, 0,lPos);
		tString sSub2 = cString::Sub(asFirstFrameName, lPos+2);
		tString sBaseName = sSub1 + sSub2;

		if(sSub2.size()==0 || sSub2[0]!='.')
		{
			Error("First frame of animation '%s' must contain '01' before extension!\n", asFirstFrameName.c_str());
			return NULL;
		}

		///////////////////////////
		//Check if texture exists
		
		//Create a fake full path.
		tWString sFirstFramePath = mpFileSearcher->GetFilePath(asFirstFrameName);
		if(sFirstFramePath == _W(""))
		{
			Error("First frame of animation '%s' could not be found!\n", asFirstFrameName.c_str());
			return NULL;
		}
		tWString sFakeFullPath = cString::GetFilePathW(sFirstFramePath) + cString::To16Char(cString::GetFileName(sBaseName));
		
        iTexture* pTexture = static_cast<iTexture*>(GetResource(sFakeFullPath));
		
		///////////////////////////
		//Check if texture exists
		if(pTexture==NULL)
		{
			tString sFileExt = cString::GetFileExt(sBaseName);
			tString sFileName = cString::SetFileExt(cString::GetFileName(sBaseName),"");

			tStringVec mvFileNames;

			tString sTest = sFileName + "01."+sFileExt;
			int lNum = 2;
			tWStringVec vPaths;

			while(true)
			{
				tWString sPath = mpFileSearcher->GetFilePath(sTest);
                
				if(sPath == _W(""))
				{
					break;
				}
				else
				{
					vPaths.push_back(sPath);
					if(lNum<10)
						sTest = sFileName + "0"+cString::ToString(lNum)+"."+sFileExt;
					else
						sTest = sFileName + cString::ToString(lNum)+"."+sFileExt;
					
					++lNum;
				}
			}

			if(vPaths.empty()) 
			{
				Error("No textures found for animation %s\n",sBaseName.c_str());
				EndLoad();
				return NULL;
			}
			
			std::vector<cBitmap*> vBitmaps;
			for(size_t i =0; i< vPaths.size(); ++i)
			{
				cBitmap* pBmp = mpBitmapLoaderHandler->LoadBitmap(vPaths[i],0);
				if(pBmp==NULL){
					Error("Couldn't load bitmap '%s'!\n",cString::To8Char(vPaths[i]).c_str());
					
					for(int j=0;j<(int)vBitmaps.size();j++) hplDelete(vBitmaps[j]);
					
					EndLoad();
					return NULL;
				}

				vBitmaps.push_back(pBmp);
			}
			
			//Create the animated texture
			pTexture = mpGraphics->GetLowLevel()->CreateTexture(sBaseName, aType, aUsage);
			pTexture->SetFullPath(sFakeFullPath);
			
			pTexture->SetSizeDownScaleLevel(alTextureSizeLevel);

			if(pTexture->CreateAnimFromBitmapVec(&vBitmaps)==false)
			{
				Error("Couldn't create animated texture '%s'!\n", sBaseName.c_str());
				hplDelete(pTexture);
				for(int j=0;j<(int)vBitmaps.size();j++) hplDelete(vBitmaps[j]);
				EndLoad();
				return NULL;
			}

			//Bitmaps no longer needed.
			for(int j=0;j<(int)vBitmaps.size();j++) hplDelete(vBitmaps[j]);
			
			mlMemoryUsage += pTexture->GetMemorySize();
			AddResource(pTexture);
		}

		if(pTexture)pTexture->IncUserCount();
		else Error("Couldn't texture '%s'\n",asFirstFrameName.c_str());
		
		EndLoad();
		return pTexture;
	}

	//-----------------------------------------------------------------------
	
	iTexture* cTextureManager::CreateCubeMap(const tString& asPathName,bool abUseMipMaps, 
											eTextureUsage aUsage,
											unsigned int alTextureSizeLevel)
	{
		tString sExt = cString::ToLowerCase(cString::GetFileExt(asPathName));

		/////////////////////////////////////////////////////////
		// Load Cubemap from single file
		if(sExt == "dds")
		{
			return CreateSimpleTexture(asPathName,abUseMipMaps,aUsage,eTextureType_CubeMap,alTextureSizeLevel);	
		}
		/////////////////////////////////////////////////////////
		// Load Cubemap from multiple files
		else
		{
			tString sName = cString::SetFileExt(asPathName,"");

			tWString sFakeFullPath = cString::To16Char(sName);
			iTexture* pTexture = static_cast<iTexture*>(GetResource(sFakeFullPath));

			BeginLoad(asPathName);
					
			if(pTexture==NULL)
			{
				//See if files for all faces exist
				tWStringVec vPaths;
				tWString sPath=_W("");
				for(int i=0;i <6 ;i++)
				{
					tStringVec *apFileFormatsVec = mpBitmapLoaderHandler->GetSupportedTypes();
					for(tStringVecIt it = apFileFormatsVec->begin();it!=apFileFormatsVec->end();++it)
					{
						tString sNewName = sName + mvCubeSideSuffixes[i] + "." + *it;
						sPath = mpFileSearcher->GetFilePath(sNewName);
		               
						if(sPath!=_W(""))break;
					}
					
					if(sPath==_W(""))
					{
						tString sNewName = sName + mvCubeSideSuffixes[i];
						Error("Couldn't find %d-face '%s', for cubemap '%s' in path: '%s'\n",i,sNewName.c_str(),sName.c_str(), asPathName.c_str());
						return NULL;
					}

					vPaths.push_back(sPath);
				}
				
				//Load bitmaps for all faces
				std::vector<cBitmap*> vBitmaps;
				for(int i=0;i<6; i++)
				{
					cBitmap* pBmp = mpBitmapLoaderHandler->LoadBitmap(vPaths[i],0);
					if(pBmp==NULL){
						Error("Couldn't load bitmap '%s'!\n",cString::To8Char(vPaths[i]).c_str());
						for(int j=0;j<(int)vBitmaps.size();j++) hplDelete(vBitmaps[j]);
						EndLoad();
						return NULL;
					}
					
					vBitmaps.push_back(pBmp);
				}

				//Create the cubemap
				pTexture = mpGraphics->GetLowLevel()->CreateTexture(sName,eTextureType_CubeMap, aUsage);
				pTexture->SetFullPath(sFakeFullPath);
				
				pTexture->SetUseMipMaps(abUseMipMaps);
				pTexture->SetSizeDownScaleLevel(alTextureSizeLevel);

				if(pTexture->CreateCubeFromBitmapVec(&vBitmaps)==false)
				{
					Error("Couldn't create cubemap '%s'!\n", sName.c_str());
					hplDelete(pTexture);
					for(int j=0;j<(int)vBitmaps.size();j++) hplDelete(vBitmaps[j]);
					EndLoad();
					return NULL;
				}

				//Bitmaps no longer needed.
				for(int j=0;j<(int)vBitmaps.size();j++)	hplDelete(vBitmaps[j]);
				
				mlMemoryUsage += pTexture->GetMemorySize();
				AddResource(pTexture);
			}

			if(pTexture)pTexture->IncUserCount();
			else Error("Couldn't texture '%s'\n",sName.c_str());
			
			EndLoad();
			return pTexture;
		}
	}

	//-----------------------------------------------------------------------

	void cTextureManager::Unload(iResourceBase* apResource)
	{

	}
	//-----------------------------------------------------------------------

	void cTextureManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();

		if(apResource->HasUsers()==false)
		{
			mlMemoryUsage -= static_cast<iTexture*>(apResource)->GetMemorySize();

			RemoveResource(apResource);
			hplDelete(apResource);
		}
	}

	//-----------------------------------------------------------------------

	void cTextureManager::Update(float afTimeStep)
	{
		tResourceBaseMapIt it = m_mapResources.begin();
		for(; it != m_mapResources.end(); ++it)
		{
			iResourceBase *pBase = it->second;
			iTexture *pTexture = static_cast<iTexture*>(pBase);

			pTexture->Update(afTimeStep);
		}
	}

	//-----------------------------------------------------------------------

	
	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::CreateSimpleTexture(	const tString& asName,bool abUseMipMaps, 
													eTextureUsage aUsage, eTextureType aType,
													unsigned int alTextureSizeLevel)
	{
		tWString sPath;
		iTexture* pTexture;
		
		BeginLoad(asName);

		pTexture = FindTexture2D(asName,sPath);

		if(pTexture==NULL && sPath!=_W(""))
		{
			//Load the bitmap
			cBitmap *pBmp;
			pBmp = mpBitmapLoaderHandler->LoadBitmap(sPath,0);
			if(pBmp==NULL)
			{
				Error("Texture manager Couldn't load bitmap '%s'\n", cString::To8Char(sPath).c_str());
				EndLoad();
				return NULL;
			}

			//Create the texture and load from bitmap
			pTexture = mpGraphics->GetLowLevel()->CreateTexture(asName,aType,aUsage);
			pTexture->SetFullPath(sPath);
			
			pTexture->SetUseMipMaps(abUseMipMaps);
			pTexture->SetSizeDownScaleLevel(alTextureSizeLevel);
			
			if(pTexture->CreateFromBitmap(pBmp)==false)
			{
				hplDelete(pTexture);
				hplDelete(pBmp);
				EndLoad();
				return NULL;
			}

			//Bitmap is no longer needed so delete it.
			hplDelete(pBmp);
			
			mlMemoryUsage += pTexture->GetMemorySize();
			AddResource(pTexture);
		}

		if(pTexture)pTexture->IncUserCount();
		else Error("Couldn't texture '%s'\n",asName.c_str());
		
		EndLoad();
		return pTexture;
	}

	//-----------------------------------------------------------------------
	
	iTexture* cTextureManager::FindTexture2D(const tString &asName, tWString &asFilePath)
	{
		iTexture *pTexture=NULL;

		if(cString::GetFileExt(asName)=="")
		{
			int lMaxCount =-1;

			///////////////////////
			//Iterate the different formats
			tStringVec *apFileFormatsVec = mpBitmapLoaderHandler->GetSupportedTypes();
			for(tStringVecIt it = apFileFormatsVec->begin();it!= apFileFormatsVec->end();++it)
			{
				tWString sTempPath = _W("");
				iTexture *pTempTex=NULL;
				int lCount=0;

				tString sNewName = cString::SetFileExt(asName,*it);
				pTempTex = static_cast<iTexture*> (FindLoadedResource(sNewName, sTempPath, &lCount));

				///////////////////////
				//Check if the image exists and then check if it has the hightest equal count.
				if((pTempTex==NULL && sTempPath!=_W("")) || pTempTex!=NULL)
				{
					if(lCount > lMaxCount)
					{
						lMaxCount = lCount;
						asFilePath = sTempPath;
						pTexture = pTempTex;
					}
				}
			}
		}
		else
		{
			pTexture = static_cast<iTexture*> (FindLoadedResource(asName, asFilePath));
		}

		return pTexture;
	}


	//-----------------------------------------------------------------------
}
