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

#include "resources/SoundEntityManager.h"

#include "system/String.h"
#include "sound/Sound.h"
#include "resources/Resources.h"
#include "sound/SoundEntityData.h"
#include "system/LowLevelSystem.h"
#include "sound/SoundHandler.h"
#include "sound/SoundChannel.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSoundEntityManager::cSoundEntityManager(cSound* apSound,cResources *apResources)
		: iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
							apResources->GetLowLevelSystem())
	{
		mpSound = apSound;
		mpResources = apResources;
	}

	cSoundEntityManager::~cSoundEntityManager()
	{
        DestroyAll();

		Log(" Done with sound entities\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cSoundEntityManager::Preload(const tString& asFile)
	{
		cSoundEntityData *pData = CreateSoundEntity(asFile);
		if(pData == NULL) {
			Warning("Couldn't preload sound entity '%s'\n",asFile.c_str());
			return;
		}

		pData->PreloadSounds();
	}

	//-----------------------------------------------------------------------

	cSoundEntityData* cSoundEntityManager::CreateSoundEntity(const tString& asName)
	{
		tWString sPath;
		cSoundEntityData* pSoundEntity;
		tString asNewName;
		
		BeginLoad(asName);

		asNewName = cString::SetFileExt(asName,"snt");
		
		pSoundEntity = static_cast<cSoundEntityData*>(this->FindLoadedResource(asNewName,sPath));

		if(pSoundEntity==NULL && sPath!=_W(""))
		{
			pSoundEntity = hplNew( cSoundEntityData, (asNewName, mpResources,mpSound) );

			if(pSoundEntity->CreateFromFile(sPath))
			{
				AddResource(pSoundEntity);
			}
			else
			{
				hplDelete(pSoundEntity);
				pSoundEntity =NULL;
			}
		}
		
		if(pSoundEntity)pSoundEntity->IncUserCount();
		else Error("Couldn't create SoundEntity '%s'\n",asNewName.c_str());
		
		EndLoad();
		return pSoundEntity;
	}

	//-----------------------------------------------------------------------

	void cSoundEntityManager::Unload(iResourceBase* apResource)
	{

	}
	//-----------------------------------------------------------------------

	void cSoundEntityManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();

		if(apResource->HasUsers()==false){
			RemoveResource(apResource);
			hplDelete(apResource);
		}
	}

	//-----------------------------------------------------------------------
}
