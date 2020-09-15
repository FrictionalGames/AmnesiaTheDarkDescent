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

#include "resources/BitmapLoaderHandler.h"

#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "graphics/Graphics.h"

#include "graphics/Bitmap.h"
#include "resources/BitmapLoader.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cBitmapLoaderHandler::cBitmapLoaderHandler(cResources* apResources, cGraphics* apGraphics)
	{
		mpResources = apResources;
		mpGraphics = apGraphics;
	}
	
	//-----------------------------------------------------------------------

	cBitmapLoaderHandler::~cBitmapLoaderHandler()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cBitmap* cBitmapLoaderHandler::LoadBitmap(const tWString& asFile, tBitmapLoadFlag aFlags)
	{
		iBitmapLoader *pBitmapLoader = static_cast<iBitmapLoader*>(GetLoaderForFile(asFile));

		if(pBitmapLoader)
		{
			cBitmap* pBitmap = pBitmapLoader->LoadBitmap(asFile, aFlags);

			//Set name of the file loaded.
			if(pBitmap) pBitmap->SetFileName(cString::GetFileNameW(asFile));

			return pBitmap;
		}
		else
		{
			return NULL;
		}
	}

	//-----------------------------------------------------------------------

	bool cBitmapLoaderHandler::SaveBitmap(cBitmap* apBitmap, const tWString& asFile, tBitmapSaveFlag aFlags)
	{
		iBitmapLoader *pBitmapLoader = static_cast<iBitmapLoader*>(GetLoaderForFile(asFile));
		
		if(pBitmapLoader)
		{
			return pBitmapLoader->SaveBitmap(apBitmap,asFile,aFlags);
		}
		return false;
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cBitmapLoaderHandler::SetupLoader(iResourceLoader *apLoader)
	{
		iBitmapLoader *pBitmapLoader = static_cast<iBitmapLoader*>(apLoader);

		pBitmapLoader->mpLowLevelGraphics = mpGraphics->GetLowLevel();
	}
	
	//-----------------------------------------------------------------------
}
