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

#include "resources/MeshLoaderHandler.h"

#include "resources/MeshLoader.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "scene/Scene.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cMeshLoaderHandler::cMeshLoaderHandler(cResources* apResources, cScene *apScene)
	{
		mpResources = apResources;
		mpScene = apScene;
	}
	
	//-----------------------------------------------------------------------

	cMeshLoaderHandler::~cMeshLoaderHandler()
	{
		
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cMesh* cMeshLoaderHandler::LoadMesh(const tWString& asFile,tMeshLoadFlag aFlags)
	{
		iMeshLoader *pMeshLoader = static_cast<iMeshLoader*>(GetLoaderForFile(asFile));

		if(pMeshLoader)
		{
				return pMeshLoader->LoadMesh(asFile,aFlags);
		}
		return NULL;
	}

	//-----------------------------------------------------------------------

	bool cMeshLoaderHandler::SaveMesh(cMesh* apMesh,const tWString& asFile)
	{
		iMeshLoader *pMeshLoader = static_cast<iMeshLoader*>(GetLoaderForFile(asFile));

		if(pMeshLoader)
		{
				return pMeshLoader->SaveMesh(apMesh,asFile);
		}
		return false;
	}
	
	//-----------------------------------------------------------------------
	cAnimation* cMeshLoaderHandler::LoadAnimation(const tWString& asFile)
	{
		iMeshLoader *pMeshLoader = static_cast<iMeshLoader*>(GetLoaderForFile(asFile));

		if(pMeshLoader)
		{
				return pMeshLoader->LoadAnimation(asFile);
		}
		else
		{
			return NULL;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	
	void cMeshLoaderHandler::SetupLoader(iResourceLoader *apLoader)
	{
		iMeshLoader *pMeshLoader = static_cast<iMeshLoader*>(apLoader);

		pMeshLoader->mpMaterialManager = mpResources->GetMaterialManager();
		pMeshLoader->mpMeshManager = mpResources->GetMeshManager();
		pMeshLoader->mpAnimationManager = mpResources->GetAnimationManager();
	}

	//-----------------------------------------------------------------------
}
