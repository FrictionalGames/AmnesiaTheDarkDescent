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

#ifndef HPL_WIDGET_H
#define HPL_WIDGET_H

#include <list>
#include <vector>
#include "gui/GuiTypes.h"
#include "system/LowLevelSystem.h"

namespace hpl {

	class cGui;
	class cGuiSet;
	class cGuiSkin;
	class cGuiSkinFont;

	class cGuiGfxElement;
	class cGuiClipRegion;

	class cGuiGlobalShortcut;

	class iFontData;

	//--------------------------------

	struct cWidgetCallback
	{
		cWidgetCallback(void *apObject, tGuiCallbackFunc apFunc)
		{
			mpObject = apObject;
            mpFunc = apFunc;
		}

		void *mpObject;
		tGuiCallbackFunc mpFunc;
	};

	typedef std::list<cWidgetCallback> tWidgetCallbackList;
	typedef tWidgetCallbackList::iterator tWidgetCallbackListIt;

	//--------------------------------
	
	class iWidget
	{
	friend class cGuiSet;
	public:
		iWidget(eWidgetType aType,cGuiSet *apSet, cGuiSkin *apSkin);
		virtual ~iWidget();
	
		/////////////////////////
		//General
		void Update(float afTimeStep);

		void Draw(float afTimeStep, cGuiClipRegion *apClipRegion);

		bool ProcessMessage(eGuiMessage aMessage, const cGuiMessageData& aData, bool abSkipVisCheck=false, bool abSkipEnabledCheck=false);

		void AddCallback(eGuiMessage aMessage,void *apObject,tGuiCallbackFunc apFunc);

		eWidgetType GetType(){ return mType;}
		
		void Init();

		virtual cGuiGlobalShortcut* AddShortcut(int alKeyModifiers, eKey aKey, eGuiMessage aMsg=eGuiMessage_ButtonPressed, bool abBypassVisibility=true, bool abBypassEnabled=true);

		/////////////////////////
		// ToolTip functions
		void SetToolTip(const tWString& asToolTip);
		const tWString& GetToolTip() { return msToolTip; }
		void SetToolTipEnabled(bool abX) { mbToolTipEnabled = abX; }
		bool IsToolTipEnabled() { return mbToolTipEnabled; }

        /////////////////////////
		// Public Helper functions
		virtual bool PointIsInside(const cVector2f& avPoint, bool abOnlyClipped);
		
		/////////////////////////
		//Hierarchy
		virtual void AttachChild(iWidget *apChild);
		virtual void RemoveChild(iWidget *apChild);
		tWidgetList& GetChildren() { return mlstChildren; }

		virtual void OnAttachChild(iWidget* apChild){}
		virtual void OnRemoveChild(iWidget* apChild){}

		/////////////////////////
		//Properties
		cGuiSet* GetSet(){ return mpSet; }

		iWidget *GetParent(){ return mpParent;}

        void SetEnabled(bool abX);
		bool IsEnabled();
		void SetVisible(bool abX);
		bool IsVisible();

		bool HasFocus();

		void SetName(const tString& asName){ msName = asName;}
		const tString& GetName(){ return msName;}

		void SetText(const tWString& asText);
		const tWString& GetText()const{ return msText; }

		iFontData *GetDefaultFontType(){ return mpDefaultFontType;}
		virtual void SetDefaultFontType(iFontData *apFont){ mpDefaultFontType = apFont;}

		const cColor& GetDefaultFontColor(){ return mDefaultFontColor;}
		virtual void SetDefaultFontColor(const cColor& aColor){ mDefaultFontColor = aColor;}
		
		const cVector2f& GetDefaultFontSize(){ return mvDefaultFontSize;}
		virtual void SetDefaultFontSize(const cVector2f& avSize){ mvDefaultFontSize = avSize;}

		void SetClipActive(bool abX){ mbClipsGraphics = abX;}
		bool GetClipActive(){ return mbClipsGraphics;}
		
		void SetPosition(const cVector3f &avPos);
		void SetGlobalPosition(const cVector3f &avPos);
		const cVector3f& GetLocalPosition();
		const cVector3f& GetGlobalPosition();

		void SetChildrenOffset(const cVector3f&);
		const cVector3f& GetChildrenOffset() { return mvChildrenOffset; }

		void SetAffectedByScroll(bool abX);
		void SetScrollAmount(const cVector3f& avX);
		const cVector3f& GetScrollAmount() { return mvScrollAmount; }

		void CenterGlobalPositionInSet();

		void SetSize(const cVector2f &avSize);
		cVector2f GetSize(){ return mvSize;}
		
		void SetColorMul(const cColor &aColor){ mColorMul = aColor;}
		const cColor& GetColorMul(){ return mColorMul;}

		bool ClipsGraphics();

		bool GetMouseIsOver(){ return mbMouseIsOver;}

		bool IsConnectedTo(iWidget *apWidget, bool abIsStartWidget=true);
		bool IsConnectedToChildren(){ return mbConnectedToChildren;}
		void SetConnectedToChildren(bool abX){ mbConnectedToChildren = abX;}

		cGuiGfxElement* GetPointerGfx();

		void SetGlobalKeyPressListener(bool abX) { mbGlobalKeyPressListener = abX; }
		bool IsGlobalKeyPressListener() { return mbGlobalKeyPressListener; }

		void SetUserData(void *apData) { mpUserData = apData;}
		void * GetUserData() { return mpUserData;}

		void SetUserValue(int alX) { mlUserValue = alX;}
		int GetUserValue() { return mlUserValue;}

		void SetCallbacksDisabled(bool abX) { mbCallbacksDisabled = abX; }
		bool GetCallbacksDisabled() { return mbCallbacksDisabled; }

		void SetFocusNavigation(eUIArrow aDir, iWidget* apWidget);
		iWidget* GetFocusNavigation(eUIArrow aDir);

		bool HasFocusNavigation();

		void SetGlobalUIInputListener(bool abX) { mbGlobalUIInputListener = abX; }
		bool IsGlobalUIInputListener()			{ return mbGlobalUIInputListener; }

	protected:
		/////////////////////////
		// Upper Widget functions
		virtual void OnLoadGraphics(){}
		
		virtual void OnChangeSize(){}
		virtual void OnChangePosition(){}
		virtual void OnChangeText(){}

		virtual void OnChildUpdate(iWidget* apChild){}
		
		virtual void OnInit(){}

		virtual void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion){}
		virtual void OnDrawAfterClip(float afTimeStep, cGuiClipRegion *apClipRegion){}

		virtual void OnUpdate(float afTimeStep){}

		virtual bool OnMessage(eGuiMessage aMessage, const cGuiMessageData& aData){return false;}
		virtual bool OnMouseMove(const cGuiMessageData& aData){return false;}
		virtual bool OnMouseDown(const cGuiMessageData& aData){return false;}
		virtual bool OnMouseUp(const cGuiMessageData& aData){return false;}
		virtual bool OnMouseDoubleClick(const cGuiMessageData& aData){return false;}
		virtual bool OnMouseEnter(const cGuiMessageData& aData){return false;}
		virtual bool OnMouseLeave(const cGuiMessageData& aData){return false;}

		virtual bool GetFocus(const cGuiMessageData& aData);
		virtual bool OnGotFocus(const cGuiMessageData& aData);
		virtual void OnChildGotFocus(iWidget* apChild, const cGuiMessageData& aData);

		virtual bool GetTabFocus(const cGuiMessageData& aData);
		virtual bool OnGotTabFocus(const cGuiMessageData& aData);
		virtual void OnChildGotTabFocus(iWidget* apChild, const cGuiMessageData& aData);

		virtual bool LoseFocus(const cGuiMessageData& aData);
		virtual bool OnLostFocus(const cGuiMessageData& aData){return false;}
		virtual void OnChildLostFocus(iWidget* apChild, const cGuiMessageData& aData);

		virtual bool OnKeyPress(const cGuiMessageData& aData){return false;}

		virtual bool OnGamepadButtonPress(const cGuiMessageData& aData){return false;}

		virtual bool OnUIArrowPress(const cGuiMessageData& aData){return false;}
		virtual bool OnUIArrowRelease(const cGuiMessageData& aData){return false;}
		virtual bool OnUIButtonPress(const cGuiMessageData& aData){return false;}
		virtual bool OnUIButtonRelease(const cGuiMessageData& aData){return false;}
		virtual bool OnUIButtonDoublePress(const cGuiMessageData& aData){return false;}
		
		virtual bool OnGetUINavFocus(const cGuiMessageData& aData){return false;}
		virtual bool OnLoseUINavFocus(const cGuiMessageData& aData){return false;}
	
		
		/////////////////////////
		// Private Helper functions
		bool CheckPointInsideClippingParent(const cVector2f& avPoint);
		cVector3f WorldToLocalPosition(const cVector3f &avPos);
		cVector2f GetPosRelativeToMouse(const cGuiMessageData& aData);

		//The order must be like this:
		//Borders: Right, Left, Up and Down
		//Corners: LEftUp, RightUp, RightDown and LEftDown.
		void DrawBordersAndCorners(cGuiGfxElement *apBackground,
									cGuiGfxElement **apBorderVec,cGuiGfxElement **apCornerVec,
									const cVector3f &avPosition, const cVector2f &avSize, bool abBordersOutside=false);

		void DrawSkinText(const tWString& asText,eGuiSkinFont aFont,const cVector3f& avPosition,
							eFontAlign aAlign = eFontAlign_Left);

		void DrawDefaultText(	const tWString& asText,
								const cVector3f& avPosition,eFontAlign aAlign);

		void DrawDefaultText(	const tWString& asText,
								const cVector3f& avPosition,eFontAlign aAlign, const cColor& aCol);

		void DrawDefaultTextHighlight(	const tWString& asText,
										const cVector3f& avPosition, eFontAlign aAlign); //, int alFirstVisibleChar, int alHighlightStart, int alHighlightSize);
		
		void SetPositionUpdated();

		void LoadGraphics();

		/////////////////////////
		// Variables
		cGuiSet *mpSet;
		cGuiSkin *mpSkin;
		cGui *mpGui;

		tWString msText;
		cVector3f mvPosition;
		cVector3f mvGlobalPosition;
		cVector2f mvSize;
		cVector3f mvChildrenOffset;
		cVector3f mvScrollAmount;
		bool mbAffectedByScroll;

		tString msName;

		eWidgetType mType;

        int mlPositionCount;

		cGuiSkinFont *mpDefaultFont;
		iFontData *mpDefaultFontType;
		cColor mDefaultFontColor;
		cColor mDefaultFontHighlightColor;
		cVector2f mvDefaultFontSize;

		iWidget *mpParent;

		tWidgetList mlstChildren;
		
		bool mbEnabled;
		bool mbVisible;

		cColor mColorMul;

		bool mbMouseIsOver;

		bool mbClipsGraphics;

		cGuiGfxElement* mpPointerGfx;

        bool mbConnectedToChildren;

		bool mbToolTipEnabled;
		tWString msToolTip;

		std::vector<cGuiGlobalShortcut*> mvShortcuts;

		bool mbGlobalKeyPressListener;
		bool mbGlobalUIInputListener;

		bool mbTextChanged;

		bool mbCallbacksDisabled;
	private:
		void SetMouseIsOver(bool abX){ mbMouseIsOver = abX;}        
		bool ProcessCallbacks(eGuiMessage aMessage, const cGuiMessageData& aData);

		int UIArrowToArrayPos(eUIArrow aDir);

        std::vector<tWidgetCallbackList> mvCallbackLists;

		bool mbPositionIsUpdated;

		void *mpUserData;
		int mlUserValue;

		std::vector<iWidget*>			mvFocusNavWidgets;
	};

};
#endif // HPL_WIDGET_H
