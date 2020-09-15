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

#include "resources/FontManager.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "graphics/Graphics.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/ImageManager.h"

#include "graphics/FontData.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFontManager::cFontManager(cGraphics* apGraphics,cGui *apGui,cResources *apResources)
		: iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
							apResources->GetLowLevelSystem())
	{
		mpGraphics = apGraphics;
		mpResources = apResources;
		mpGui = apGui;
	}

	cFontManager::~cFontManager()
	{
		DestroyAll();
		Log(" Done with fonts\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iFontData* cFontManager::CreateFontData(const tString& asName, int alSize,unsigned short alFirstChar,
											unsigned short alLastChar)
	{
		tWString sPath;
		iFontData* pFont;
		tString asNewName = cString::ToLowerCase(asName);

		BeginLoad(asName);
		
		//asNewName = cString::SetFileExt(asName,"ttf");

		pFont = static_cast<iFontData*>(this->FindLoadedResource(asNewName,sPath));

		if(pFont==NULL && sPath!=_W(""))
		{
			pFont = mpGraphics->GetLowLevel()->CreateFontData(asNewName);
			pFont->SetUp(mpResources,mpGui);
			
			tString sExt = cString::ToLowerCase(cString::GetFileExt(asName));

			//True Type Font
			if(sExt == "ttf")
			{
				if(pFont->CreateFromFontFile(sPath,alSize,alFirstChar,alLastChar)==false){
					hplDelete(pFont);
					EndLoad();
					return NULL;
				}
			}
			//Angel code font type
			else if(sExt == "fnt")
			{
				if(pFont->CreateFromBitmapFile(sPath)==false){
					hplDelete(pFont);
					EndLoad();
					return NULL;
				}
			}
			else
			{
				Error("Font '%s' has an unkown extension!\n",asName.c_str());
				hplDelete(pFont);
				EndLoad();
				return NULL;
			}
			
			//mpResources->GetImageManager()->FlushAll();
			AddResource(pFont);
		}

		if(pFont)pFont->IncUserCount();
		else Error("Couldn't create font '%s'\n",asNewName.c_str());
		
		EndLoad();
		return pFont;
	}

	//-----------------------------------------------------------------------

	void cFontManager::Unload(iResourceBase* apResource)
	{

	}
	//-----------------------------------------------------------------------

	void cFontManager::Destroy(iResourceBase* apResource)
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


	//-----------------------------------------------------------------------
}
