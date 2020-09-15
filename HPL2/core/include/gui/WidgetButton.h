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

#ifndef HPL_WIDGET_BUTTON_H
#define HPL_WIDGET_BUTTON_H

#include "gui/Widget.h"

namespace hpl {

	class cGuiSkinFont;

	class cWidgetButton : public iWidget
	{
	public:
		cWidgetButton(cGuiSet *apSet, cGuiSkin *apSkin);
		virtual ~cWidgetButton();

		void SetImage(cGuiGfxElement *apImage, bool abDestroyImage=true);
		cGuiGfxElement *GetImage(){return mpImage;}

		void SetRepeatActive(bool abX) { mbRepeatActive = abX; }
		bool GetRepeatActive() { return mbRepeatActive; }
		
		void SetRepeatFreq(float afFreq);
		
		void SetToggleable(bool abX) { mbToggleable = abX; }
		bool IsToggleable() { return mbToggleable; }

		void SetPressed(bool abX, bool abGenCallback=true);
		bool IsPressed() { return mbPressed; }
		
	protected:
		/////////////////////////
		// Implemented functions
		void OnLoadGraphics();
		
		void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);

		void OnUpdate(float afTimeStep);

		bool OnMouseMove(const cGuiMessageData& aData);
		bool OnMouseDown(const cGuiMessageData& aData);
		bool OnMouseUp(const cGuiMessageData& aData);
		bool OnMouseEnter(const cGuiMessageData& aData);
		bool OnMouseLeave(const cGuiMessageData& aData);

		bool OnMouseDoubleClick(const cGuiMessageData& aData) { return true; }

		bool OnLostFocus(const cGuiMessageData& aData);

		bool OnUIButtonPress(const cGuiMessageData& aData);
		bool OnUIButtonRelease(const cGuiMessageData& aData);

		/////////////////////////
		// Data
		bool mbPressed;

		bool mbToggleable;

		cGuiGfxElement *mpImage;
		bool mbDestroyImage;

		float mfTimer;
		bool mbRepeatActive;
		float mfRepeatPeriod;
		int mlRepeatTimes;
		cGuiMessageData mPressedData;

		cGuiGfxElement *mpGfxBackgroundUp;
		cGuiGfxElement *mvGfxBordersUp[4];
		cGuiGfxElement *mvGfxCornersUp[4];

		cGuiGfxElement *mpGfxBackgroundDown;
		cGuiGfxElement *mvGfxBordersDown[4];
		cGuiGfxElement *mvGfxCornersDown[4];
	};

};
#endif // HPL_WIDGET_BUTTON_H
