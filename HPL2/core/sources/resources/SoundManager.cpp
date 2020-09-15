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

#include "resources/SoundManager.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "sound/Sound.h"
#include "sound/SoundData.h"
#include "sound/LowLevelSound.h"
#include "resources/FileSearcher.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSoundManager::cSoundManager(cSound* apSound,cResources *apResources)
		: iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
							apResources->GetLowLevelSystem())
	{
		mpSound = apSound;
		mpResources = apResources;

		mpSound->GetLowLevel()->GetSupportedFormats(mlstFileFormats);
	}

	cSoundManager::~cSoundManager()
	{
		DestroyAll();
		Log(" Done with sounds\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iSoundData* cSoundManager::CreateSoundData(const tString& asName, bool abStream,bool abLoopStream)
	{
		tWString sPath;
		iSoundData* pSound=NULL;

		BeginLoad(asName);

		///////////////////////////
		// Load Stream
		if(abStream)
		{
			FindStreamPath(asName, sPath);
			if(sPath!=_W(""))
			{
				pSound = mpSound->GetLowLevel()->LoadSoundData(	asName,sPath,"",abStream, abLoopStream);
				if(pSound)
				{
					AddResource(pSound, true, false);
					mlstStreamData.push_back(pSound);
					pSound->SetSoundManager(mpResources->GetSoundManager());
				}
			}
		}
		///////////////////////////
		// Load Normal Sample
		else
		{
			pSound = FindSampleData(asName, sPath);
		
			if(pSound==NULL && sPath!=_W(""))
			{
				pSound = mpSound->GetLowLevel()->LoadSoundData(	asName,sPath,"",abStream, abLoopStream);
				if(pSound)
				{
					AddResource(pSound);
					pSound->SetSoundManager(mpResources->GetSoundManager());
				}
			}
		}
				
		//Do _not_ increase user count here since that is used when creating channel

		//if(!pSound) Error("Couldn't load sound data '%s'\n",asName.c_str());
		EndLoad();
		return pSound;
	}

	//-----------------------------------------------------------------------

	void cSoundManager::Unload(iResourceBase* apResource)
	{

	}
	//-----------------------------------------------------------------------

	void cSoundManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();
			
		iSoundData *pData = static_cast<iSoundData *>(apResource);
		if(pData->IsStream() && pData->HasUsers()==false)
		{
			STLFindAndDelete(mlstStreamData, pData);
		}
	}

	//-----------------------------------------------------------------------

	void cSoundManager::DestroyAll()
	{
		/////////////////////
		// Streams
		for(tSoundDataListIt streamIt=mlstStreamData.begin(); streamIt != mlstStreamData.end(); ++streamIt)
		{
			iSoundData *pStream = *streamIt;
			
			Log("  destroying %p\n",pStream);
			Log("   file '%s'\n", cString::To8Char(pStream->GetFullPath()).c_str());

			hplDelete(pStream);
		}
		mlstStreamData.clear();
		
		/////////////////////
		// Normal samples
		tResourceBaseMapIt it = m_mapResources.begin();
		while(it != m_mapResources.end())
		{
			iResourceBase* pData = it->second;
			RemoveResource(pData);
			hplDelete(pData);
			it= m_mapResources.begin();
		}
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	iSoundData *cSoundManager::FindSampleData(const tString &asName, tWString &asFilePath)
	{
		iSoundData *pData=NULL;

		if(cString::GetFileExt(asName)=="")
		{
			for(tStringListIt it = mlstFileFormats.begin();it!=mlstFileFormats.end();++it)
			{
				tString sNewName = cString::SetFileExt(asName,*it);
				
				pData = static_cast<iSoundData*> (FindLoadedResource(sNewName, asFilePath));
				if((pData==NULL && asFilePath!=_W("")) || pData!=NULL)break;
			}
		}
		else
		{
			pData = static_cast<iSoundData*> (FindLoadedResource(asName, asFilePath));
		}

		return pData;
	}

	//-----------------------------------------------------------------------

	void cSoundManager::FindStreamPath(const tString &asName, tWString &asFilePath)
	{
		iSoundData *pData=NULL;

		if(cString::GetFileExt(asName)=="")
		{
			for(tStringListIt it = mlstFileFormats.begin();it!=mlstFileFormats.end();++it)
			{
				tString sNewName = cString::SetFileExt(asName,*it);

				asFilePath = mpFileSearcher->GetFilePath(sNewName);
				if(asFilePath != _W("")) break;
			}
		}
		else
		{
			asFilePath = mpFileSearcher->GetFilePath(asName);
		}
	}

	//-----------------------------------------------------------------------
}
