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

#ifndef HPL_FONT_MANAGER_H
#define HPL_FONT_MANAGER_H

#include "resources/ResourceManager.h"

namespace hpl {
	
	class cGraphics;
	class cResources;
	class cGui;
	class iFontData;

	class cFontManager : public iResourceManager
	{
	public:
		cFontManager(cGraphics* apGraphics,cGui *apGui,cResources *apResources);
		~cFontManager();

		/**
		 * Create a new font
		 * \param asName name of the font
		 * \param alSize size the characters are rendered in
		 * \param alFirstChar first ASCII character to be rendered
		 * \param alLastChar last ASCII character to be rendered
		 * \return 
		 */
		iFontData* CreateFontData(const tString& asName, int alSize=16,unsigned short alFirstChar=32,
								unsigned short alLastChar=255);

		void Destroy(iResourceBase* apResource);
		void Unload(iResourceBase* apResource);

	private:
		cGraphics* mpGraphics;
		cResources *mpResources;
		cGui *mpGui;
	};

};
#endif // HPL_FONT_MANAGER_H
