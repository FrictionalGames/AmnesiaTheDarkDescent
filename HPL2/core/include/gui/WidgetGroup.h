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

#ifndef HPL_WIDGET_GROUP_H
#define HPL_WIDGET_GROUP_H

#include "gui/Widget.h"


namespace hpl {

	class cWidgetLabel;
	class cGuiGfxElement;

	class cWidgetGroup : public iWidget
	{
	public:
		cWidgetGroup(cGuiSet* apSet, cGuiSkin* apSkin);
		~cWidgetGroup();

		void SetHeaderText(const tWString& asText);
		const tWString& GetHeaderText();

		void SetDefaultFontSize(const cVector2f& avSize);
	protected:
		/////////////////////////////
		// Implemented functions
		void OnInit();
		void OnLoadGraphics();

		void OnChangeText();

		void OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion);
		void OnDrawAfterClip(float afTimeStep, cGuiClipRegion *apClipRegion);

		/////////////////////////////
		// Data

		cWidgetLabel *mpHeader;
		cVector2f mvHeaderSize;

		cGuiGfxElement *mpGfxBackground;
		
		cGuiGfxElement *mvGfxBorders[4];
		cGuiGfxElement *mvGfxCorners[4];

		float mfHeaderOffset;
		bool mbHeaderBreaksUpperBorder;
	};
};


#endif // HPL_WIDGET_GROUP_H
