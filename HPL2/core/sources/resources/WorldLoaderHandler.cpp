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

#include "resources/WorldLoaderHandler.h"

#include "resources/WorldLoader.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"

#include "scene/Scene.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cWorldLoaderHandler::cWorldLoaderHandler(cResources* apResources,cGraphics *apGraphics, cScene *apScene, cPhysics *apPhysics)
	{
		mpResources = apResources;
		mpScene = apScene;
		mpGraphics = apGraphics;
		mpPhysics = apPhysics;
	}
	
	//-----------------------------------------------------------------------

	cWorldLoaderHandler::~cWorldLoaderHandler()
	{
		
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cWorld* cWorldLoaderHandler::LoadWorld(const tWString& asFile,tWorldLoadFlag aFlags)
	{
		iWorldLoader *pWorldLoader = static_cast<iWorldLoader*>(GetLoaderForFile(asFile));

		if(pWorldLoader)
		{
				return pWorldLoader->LoadWorld(asFile,aFlags);
		}
		
		return NULL;
	}

	//-----------------------------------------------------------------------

	
	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	
	void cWorldLoaderHandler::SetupLoader(iResourceLoader *apLoader)
	{
		iWorldLoader *pWorldLoader = static_cast<iWorldLoader*>(apLoader);
		
		pWorldLoader->mpResources = mpResources;
		pWorldLoader->mpGraphics = mpGraphics;
		pWorldLoader->mpScene = mpScene;
		pWorldLoader->mpPhysics = mpPhysics;
	}

	//-----------------------------------------------------------------------
}
