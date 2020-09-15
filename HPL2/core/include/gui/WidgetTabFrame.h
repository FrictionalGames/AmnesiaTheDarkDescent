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

#ifndef HPL_WIDGET_TAB_FRAME
#define HPL_WIDGET_TAB_FRAME

#include "gui/Widget.h"

using namespace std;

namespace hpl
{
	class cGuiSet;
	class cGuiSkin;

	class cWidgetFrame;
	class cWidgetButton;
	
	class cWidgetTab;
	class cWidgetTabLabel;
	class cWidgetTabFrame;

	class cGuiGfxElement;

	typedef vector<cWidgetTab*>			tWidgetTabVector;
	typedef tWidgetTabVector::iterator	tWidgetTabVectorIt;


	//////////////////////////////////////////////////////////////////
	// Tab Label
	//////////////////////////////////////////////////////////////////

	class cWidgetTabLabel : public iWidget
	{
	friend class cWidgetTab;
	friend class cWidgetTabFrame;
	public:
		cWidgetTabLabel(cGuiSet* apSet, cGuiSkin* apSkin);
		virtual ~cWidgetTabLabel();

		cGuiGfxElement* GetIcon() { return mpGfxIcon; }
	protected:
		///////////////////////////
		// Implemented functions
		bool OnMouseDown(const cGuiMessageData& aData);

		void OnChangeText();

		bool OnGetUINavFocus(const cGuiMessageData& aData);

		void OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion);
		void OnLoadGraphics();

		///////////////////////////
		// Data
		cWidgetTab* mpParentTab;

		

		cGuiGfxElement* mvGfxBorders[4];
		cGuiGfxElement* mvGfxCorners[4];
		cGuiGfxElement* mpGfxBackground;

		cGuiGfxElement* mpGfxIcon;
	};

	//////////////////////////////////////////////////////////////////
	// Tab
	//////////////////////////////////////////////////////////////////

	class cWidgetTab : public iWidget
	{
    friend class cWidgetTabFrame;
	friend class cWidgetTabLabel;
	public:
		cWidgetTab(cWidgetTabFrame* apParent);
		virtual ~cWidgetTab();

		int GetIndex() { return mlIndex; }

		void AttachChild(iWidget* apChild);
		void RemoveChild(iWidget* apChild);

		void SetLabelPosX(float afPosX);

		void SetIcon(cGuiGfxElement* apIcon);

		cWidgetTabFrame* GetParentTabFrame() { return mpParentTabFrame; }
		cWidgetTabLabel* GetTabLabel() { return mpTabLabel; }

		void SetOnTop(bool abX);
		bool IsOnTop() { return mbOnTop; }

        bool PointIsInside(const cVector2f& avPoint, bool abOnlyClipped);

	protected:
		////////////////////////////
		// Implemented functions
        void OnChangeText();
		void OnChangeSize();

		void OnInit();

		void OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion);

		void OnLoadGraphics();

		////////////////////////////
		// Data
		int mlIndex;

		bool mbOnTop;

		cWidgetTabFrame* mpParentTabFrame;

		cWidgetTabLabel* mpTabLabel;
		cWidgetFrame*	mpFrame;
	};

	//////////////////////////////////////////////////////////////////
	// Tab Frame
	//////////////////////////////////////////////////////////////////

	class cWidgetTabFrame : public iWidget
	{
	friend class cWidgetTab;
	friend class cWidgetTabLabel;
	public:
		cWidgetTabFrame(cGuiSet* apSet, cGuiSkin* apSkin);
		virtual ~cWidgetTabFrame();

		cWidgetTab* AddTab(const tWString& asTabCaption);
		void ClearTabs();

		int GetTabNum();
		cWidgetTab* GetTab(int alIdx);
		cWidgetTab* GetTab(const tWString& asTabCaption);

		cWidgetTab* GetTabOnTop() { return mpTopTab; }

		void SetBackgroundZ(float afZ) { mfBackgroundZ = afZ; }
		float GetBackgroundZ() { return mfBackgroundZ; }

		void SetForegroundZ(float afZ) { mfForegroundZ = afZ; }
		float GetForegroundZ() { return mfForegroundZ; }

		int GetTabOnTopIndex();
		void SetTabOnTop(cWidgetTab* apTab);
		void SetTabOnTopByIndex(int alIdx);

		void SetHorizontalScrollEnabled(bool abX);
		void SetVerticalScrollEnabled(bool abX);

	protected:
		bool OnUIButtonPress(const cGuiMessageData& aData);

		/////////////////////////////
		// Own fuctions
		bool ArrowButton_Pressed(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(ArrowButton_Pressed);

		void SetScrollButtonsHidden(bool abX);
		void UpdateTabVisibility();

		/////////////////////////////
		// Implemented functions
		void OnInit();
		void OnChangeSize();

		void OnLoadGraphics();
		
		/////////////////////////////
		// Data
		tWidgetTabVector mvTabs;
		cWidgetTab*		mpTopTab;

		cWidgetButton* mvArrowButtons[2];

		float mfBackgroundZ;
		float mfForegroundZ;

		float mfButtonSize;
		float mfFirstTabOffset;
		float mfTabSeparation;

		int mlFirstVisibleTab;
		int mlLastVisibleTab;

		bool mbTabOnTopChanged;

		float mfTabInactiveOffset;
		float mfTabActiveOffset;

		cGuiGfxElement* mvGfxArrows[2];
		cGuiGfxElement* mvGfxDisabledArrows[2];

		float mfTextLeftPadding;
		float mfTextRightPadding;
		float mfIconPadding;

		cGuiGfxElement* mvGfxTabLabelBorders[4];
		cGuiGfxElement* mvGfxTabLabelCorners[4];
		cGuiGfxElement* mpGfxTabLabelBG;
		cGuiGfxElement* mpGfxTabLabelBGActive;


		cGuiGfxElement* mvGfxTabBorders[4];
		cGuiGfxElement* mvGfxTabCorners[4];
		cGuiGfxElement* mpGfxTabBG;

		bool mbHoriScrollEnabled;
		bool mbVertScrollEnabled;
	};
};

#endif	// HPL_WIDGET_TAB_FRAME
