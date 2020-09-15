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

#ifndef HPL_GUI_POP_UP_H
#define HPL_GUI_POP_UP_H

#include "gui/GuiTypes.h"

namespace hpl {

	class cGuiSet;
	class cGuiSkin;

	class cWidgetWindow;

	class iGuiPopUp 
	{
	public:
		iGuiPopUp(cGuiSet *apSet, bool abAddCloseButton, const cVector2f& avPopUpSize);
		virtual ~iGuiPopUp();

		void AddOnDestroyCallback(void *apCallbackObject, tGuiCallbackFunc apCallback);

		void SetKillOnEscapeKey(bool abX);
		bool GetKillOnEscapeKey();

		void SetNextFocusWidget(iWidget *apNewFocused)  { mpNewFocused = apNewFocused; mbFocusChanged = apNewFocused != NULL; }
		cGuiSet* GetGuiSet() { return mpSet; }

	protected:
		bool Window_OnClose(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Window_OnClose);

		virtual void OnCloseSpecific() {}

		void SelfDestruct();

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Use this func to safely execute callbacks (makes sure right attention and focus widgets are set after destroying popup)
		bool RunCallback(void* apObj, tGuiCallbackFunc apCallback, 
							iWidget* apWidget, const cGuiMessageData& aData, bool abRunFocusChangeChecks);

		void SetUpDefaultFocus(iWidget* apWidget);

		cGuiSet *mpSet;
		cGuiSkin *mpSkin;

		cWidgetWindow* mpWindow;

		void* mpDestroyCallbackObject;
		tGuiCallbackFunc mpDestroyCallback;

		bool mbAttChanged;
		iWidget* mpNewAttention;
		bool mbFocusChanged;
		iWidget* mpNewFocused;
		bool mbDefaultUIFocusChanged;
		iWidget* mpNewDefaultUIFocus;
	};

};
#endif // HPL_GUI_POP_UP_H

