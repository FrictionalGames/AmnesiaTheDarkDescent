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

#ifndef HPL_GUI_SET_H
#define HPL_GUI_SET_H

#include <list>
#include "gui/GuiTypes.h"
#include "graphics/GraphicsTypes.h"


namespace hpl {

	//---------------------------------------------

	class cResources;
	class cGraphics;
	class cSound;
	class cScene;

	class cFrustum;
	class iFontData;
	
	class cGui;
	class cGuiSkin;
	class iGuiMaterial;
	class iGuiPopUp;
	class iWidget;
	
	class cWidgetWindow;
	class cWidgetFrame;
	class cWidgetButton;
	class cWidgetLabel;
	class cWidgetSlider;
	class cWidgetTextBox;
	class cWidgetCheckBox;
	class cWidgetImage;
	class cWidgetListBox;
	class cWidgetMultiPropertyListBox;
	class cWidgetComboBox;
	class cWidgetMenuItem;
	class cWidgetContextMenu;
	class cWidgetMainMenu;
	class cWidgetTab;
	class cWidgetTabLabel;
	class cWidgetTabFrame;
	class cWidgetGroup;
	class cWidgetDummy;

	class cGuiPopUpMessageBox;
	class cGuiPopUpFilePicker;
	class cGuiPopUpColorPicker;	
	class cGuiPopUpUIKeyboard;

	//---------------------------------------------
	
	typedef std::list<iGuiPopUp*> tGuiPopUpList;
	typedef tGuiPopUpList::iterator tGuiPopUpListIt;
    
	//---------------------------------------------
	
	class cGuiClipRegion;
	class cGuiRenderObject
	{
	public:
		cGuiRenderObject() : mpGfx(NULL), mvPos(0), mvSize(0), mColor(0,0), mpCustomMaterial(NULL), mpClipRegion(NULL), mbRotated(false), mfAngle(0.0f), mvPivot(0.0f)
		{}

		cGuiGfxElement *mpGfx;
		cVector3f mvPos;
		cVector2f mvSize;
		cColor mColor;
		iGuiMaterial *mpCustomMaterial;
		cGuiClipRegion *mpClipRegion;

		bool mbRotated;
		float mfAngle;
		cVector3f mvPivot;
	};

	class cGuiRenderObjectCompare
	{
	public:
		bool operator()(const cGuiRenderObject& aObjectA, const cGuiRenderObject& aObjectB);
	};

	typedef std::multiset<cGuiRenderObject,cGuiRenderObjectCompare> tGuiRenderObjectSet;
	typedef tGuiRenderObjectSet::iterator tGuiRenderObjectSetIt;

	//-----------------------------------------------
		
	typedef std::list<cGuiClipRegion*> tGuiClipRegionList;
	typedef tGuiClipRegionList::iterator tGuiClipRegionListIt;
	
	class cGuiClipRegion
	{
	public:
		cGuiClipRegion() : mRect(0,0,-1,-1){}
		~cGuiClipRegion();

		void Clear();
		cGuiClipRegion* CreateChild(const cVector3f &avPos, const cVector2f &avSize);

		//tGuiRenderObjectSet m_setObjects;
		cRect2f mRect;
		
		tGuiClipRegionList mlstChildren;
	};

	//-----------------------------------------------

	class cGuiGlobalShortcut
	{
	public:
		cGuiGlobalShortcut(int alKeyModifiers, eKey aKey, iWidget* apWidget, eGuiMessage aMessage, bool abBypassVisiblity, bool abBypassEnabled);

		const cKeyPress& GetKey() { return mKey; }

		bool IsEnabled() { return mbEnabled; }
		void SetEnabled(bool abX) { mbEnabled = abX; }

		bool ProcessKeyPress(const cKeyPress& aKey);
		bool Exec();

		tString ToString();
	protected:
		bool mbEnabled;
		bool mbBypassVisibility;
		bool mbBypassEnabled;

		cKeyPress mKey;
		iWidget* mpWidget;
		eGuiMessage mMessage;
	};

	typedef std::vector<cGuiGlobalShortcut*> tShortcutVec;
	typedef tShortcutVec::iterator tShortcutVecIt;

	typedef std::list<cGuiGlobalShortcut*> tShortcutList;
	typedef tShortcutList::iterator tShortcutListIt;

	//-----------------------------------------------
	
	class cGuiSet
	{
	friend class iGuiPopUp;
	public:
		cGuiSet(const tString &asName, cGui *apGui,cGuiSkin *apSkin,
				cResources *apResources, cGraphics* apGraphics,
				cSound *apSound, cScene *apScene);
		~cGuiSet();

		////////////////////////////////////
		// General

		void Update(float afTimeStep);

		void DrawAll(float afTimeStep);

		bool SendMessage(eGuiMessage aMessage, const cGuiMessageData& aData);

		cGuiGlobalShortcut* AddGlobalShortcut(int alKeyModifiers, eKey aKey,  iWidget* apWidget, eGuiMessage aMessage, bool abBypassVisibility=true, bool abBypassEnabled=true);
		void RemoveGlobalShortcut(cGuiGlobalShortcut* apShortcut);

		void ClearGlobalShortcuts();

		void DestroyAllWidgets();

		void ResetMouseOver();

		////////////////////////////////////
		// Rendering
		void Render(cFrustum *apFrustum);

		void ClearRenderObjects();

		void SetDrawOffset(const cVector3f& avOffset){ mvDrawOffset = avOffset;}
		void SetCurrentClipRegion(cGuiClipRegion *apRegion){ mpCurrentClipRegion = apRegion;}
		cGuiClipRegion* GetCurrentClipRegion(){ return mpCurrentClipRegion;}
		cGuiClipRegion* GetBaseClipRegion(){ return &mBaseClipRegion;}
		
		void DrawGfx(	cGuiGfxElement* apGfx, 
						const cVector3f &avPos, 
						const cVector2f &avSize=-1,
						const cColor& aColor=cColor(1,1),
						eGuiMaterial aMaterial = eGuiMaterial_LastEnum,
						float afRotationAngle=0, 
						bool abUseCustomPivot=false, const cVector3f& avCustomPivot=0);
		
		void DrawFont (	const tWString &asText, 
						iFontData *apFont, const cVector3f &avPos,
						const cVector2f &avSize, const cColor& aColor,
						eFontAlign aAlign = eFontAlign_Left, 
						eGuiMaterial aMaterial = eGuiMaterial_FontNormal);

		void DrawFont (	iFontData *apFont, const cVector3f &avPos,
						const cVector2f &avSize, const cColor& aColor,
						eFontAlign aAlign,eGuiMaterial aMaterial,
						const wchar_t* fmt,...);

		void DrawFont (	iFontData *apFont, const cVector3f &avPos,
						const cVector2f &avSize, const cColor& aColor,
						const wchar_t* fmt,...);
		
		////////////////////////////////////
		// Widget Creation
		cWidgetWindow* CreateWidgetWindow(	tWidgetWindowButtonFlag alFlags=eWidgetWindowButtonFlag_None,
											const cVector3f &avLocalPos=0,
											const cVector2f &avSize=0,
											const tWString &asText=_W(""),
											iWidget *apParent=NULL,
											const tString& asName = "" );

		cWidgetFrame* CreateWidgetFrame(	const cVector3f &avLocalPos=0,
											const cVector2f &avSize=0,
											bool abDrawFrame=false,
											iWidget *apParent=NULL,
											bool abHScrollBar=false,
											bool abVScrollBar=false,
											const tString& asName = "" );

		cWidgetButton* CreateWidgetButton(	const cVector3f &avLocalPos=0,
											const cVector2f &avSize=0,
											const tWString &asText=_W(""),
											iWidget *apParent=NULL,
											bool abToggleable=false,
											const tString& asName = "");

		cWidgetLabel* CreateWidgetLabel(	const cVector3f &avLocalPos=0,
											const cVector2f &avSize=0,
											const tWString &asText=_W(""),
											iWidget *apParent=NULL,
											const tString& asName = "" );

		cWidgetSlider* CreateWidgetSlider(	eWidgetSliderOrientation aOrientation,
											const cVector3f &avLocalPos=0,
											const cVector2f &avSize=0,
											int alMaxValue=10,
											iWidget *apParent=NULL,
											const tString& asName = "" );

		cWidgetTextBox* CreateWidgetTextBox(const cVector3f &avLocalPos=0,
											const cVector2f &avSize=0,
											const tWString &asText=_W(""),
											iWidget *apParent=NULL,
											eWidgetTextBoxInputType aType = eWidgetTextBoxInputType_Normal,
											float afNumericAdd=1.0f,
											bool abShowButtons=true,
											const tString& asName = "" );

		cWidgetCheckBox* CreateWidgetCheckBox(const cVector3f &avLocalPos=0,
											const cVector2f &avSize=0,
											const tWString &asText=_W(""),
											iWidget *apParent=NULL,
											const tString& asName = "" );

		cWidgetImage* CreateWidgetImage(const tString &asFile="",
										const cVector3f &avLocalPos=0,
										const cVector2f &avSize=-1,
										eGuiMaterial aMaterial = eGuiMaterial_Alpha,
										bool abAnimate=false,
										iWidget *apParent=NULL,
										const tString& asName = "" );

		cWidgetListBox* CreateWidgetListBox(const cVector3f &avLocalPos=0,
											const cVector2f &avSize=0,
											iWidget *apParent=NULL,
											const tString& asName = "" );

		cWidgetMultiPropertyListBox* CreateWidgetMultiPropertyListBox(	const cVector3f& avLocalPos=0,
																		const cVector2f& avSize=0,
																		iWidget* apParent=NULL,
																		const tString& asName = "");

		cWidgetComboBox* CreateWidgetComboBox(const cVector3f &avLocalPos=0,
												const cVector2f &avSize=0,
												const tWString &asText=_W(""),
												iWidget *apParent=NULL,
												const tString& asName = "");

		cWidgetMenuItem* CreateWidgetMenuItem(const cVector3f &avLocalPos=0,
												const cVector2f &avSize=0,
												const tWString &asText=_W(""),
												iWidget *apParent=NULL,
												const tString& asName = "");

		cWidgetContextMenu* CreateWidgetContextMenu(const cVector3f &avLocalPos=0,
												const cVector2f &avSize=0,
												const tWString &asText=_W(""),
												iWidget *apParent=NULL,
												const tString& asName = "");

		cWidgetMainMenu* CreateWidgetMainMenu(iWidget *apParent=NULL,
												const tString& asName = "");

		cWidgetTabLabel* CreateWidgetTabLabel(const cVector3f &avLocalPos=0,
												const cVector2f &avSize=0,
												const tWString &asText=_W(""),
												iWidget *apParent=NULL,
												const tString& asName = "");
		
		cWidgetTab* CreateWidgetTab(const cVector3f &avLocalPos=0,
												const cVector2f &avSize=0,
												const tWString &asText=_W(""),
												iWidget *apParent=NULL,
												const tString& asName = "");

		cWidgetTabFrame* CreateWidgetTabFrame(const cVector3f &avLocalPos=0,
												const cVector2f &avSize=0,
												const tWString &asText=_W(""),
												iWidget *apParent=NULL,
												bool abAllowHScroll = true,
												bool abAllowVScroll = true,
												const tString& asName = "");

		cWidgetGroup* CreateWidgetGroup(const cVector3f &avLocalPos=0,
												const cVector2f &avSize=0,
												const tWString &asText=_W(""),
												iWidget *apParent=NULL,
												const tString& asName = "");

		cWidgetDummy* CreateWidgetDummy(const cVector3f &avLocalPos=0,
												iWidget *apParent=NULL,
												const tString& asName = "");


		iWidget *GetWidgetFromName(const tString& asName);

		void DestroyWidget(iWidget *apWidget, bool abDestroyChildren=false);

		bool IsValidWidget(iWidget *apWidget);

		////////////////////////////////////
		// Popup
		cGuiPopUpMessageBox* CreatePopUpMessageBox(	const tWString& asLabel, const tWString& asText,
													const tWString& asButton1, const tWString& asButton2,
													void *apCallbackObject, tGuiCallbackFunc apCallback);

		cGuiPopUpFilePicker* CreatePopUpSaveFilePicker( tWString &asFileName, const tWString &asCategory, 
															 const tWString &asFilter, const tWString &asStartPath, bool abShowHidden,
															void *apCallbackObject, tGuiCallbackFunc apCallback, const tWString& asStartFilename=_W(""));

		cGuiPopUpFilePicker* CreatePopUpLoadFilePicker( tWStringVec &avFileList, bool abAddAllFilesFilter, 
															 const tWString &asStartPath, bool abShowHidden, 
															 void *apCallbackObject, tGuiCallbackFunc apCallback);

		cGuiPopUpColorPicker* CreatePopUpColorPicker(cColor* apDestColor, const cVector3f& avPos, void *apCallbackObject, tGuiCallbackFunc apCallback);

		cGuiPopUpUIKeyboard* CreatePopUpUIKeyboard(cWidgetTextBox* apTarget);


		void DestroyPopUp(iGuiPopUp *apPopUp);

		bool PopUpIsActive();

		////////////////////////////////////
		// Context Menu
		void ShowContextMenu( cWidgetContextMenu* apMenu, const cVector3f& avPosition );

		////////////////////////////////////
		// Windows
		void RemoveWindow(cWidgetWindow* apWin);
		void SetLastWindowZ(float afX);
		void SetWindowOnTop(cWidgetWindow* apWin);

		////////////////////////////////////
		// Properties
		void SetActive(bool abX);
		bool IsActive(){return mbActive;}

		const tString& GetName() { return msName;}

		cGui* GetGui(){ return mpGui;}

		void SetDrawMouse(bool abX);
		bool GetDrawMouse(){ return mbDrawMouse;}

		void SetMouseZ(float afZ){mfMouseZ = afZ;}
		float GetMouseZ(){ return mfMouseZ;}

		void SetMouseMovementEnabled(bool abX) { mbMouseMovementEnabled = abX; }
		bool GetMouseMovementEnabled() { return mbMouseMovementEnabled; }

		const cVector2f& GetMousePos(){ return mvMousePos;}

		void SetRootWidgetClips(bool abX);
		bool GetRootWidgetClips();

		void SetVirtualSize(const cVector2f& avSize, float afMinZ, float afMaxZ, const cVector2f& avOffset=0);
		const cVector2f& GetVirtualSize(){return mvVirtualSize;}
		const cVector2f& GetVirtualSizeOffset(){return mvVirtualSizeOffset;}

		void SetFocusedWidget(iWidget* apWidget, bool abCheckForValidity=false);
		iWidget* GetFocusedWidget(){return mpFocusedWidget;}

		void PushFocusedWidget();
		void PopFocusedWidget();

		void SetAttentionWidget(iWidget *apWidget, bool abClearFocus = true, bool abCheckForValidity = false);
		iWidget* GetAttentionWidget(){ return mpAttentionWidget;}

		void PushAttentionWidget();
		void PopAttentionWidget(bool abClearFocus=true);
		
		void SetIs3D(bool abX);
		bool Is3D(){ return mbIs3D;}

		void SetRendersBeforePostEffects(bool abX);
		bool RendersBeforePostEffects() { return mbRendersBeforePostEffects; }

		int GetDrawPriority(){ return mlDrawPrio;}
		void SetDrawPriority(int alPrio){ mlDrawPrio = alPrio;}

		void SetCurrentPointer(cGuiGfxElement *apGfx);
		cGuiGfxElement* GetCurrentPointer(){ return mpGfxCurrentPointer;}

		void Set3DSize(const cVector3f& avSize);
		const cVector3f& Get3DSize(){return mv3DSize;}

		void SetCullBackface(bool abX){ mbCullBackface = abX;}
		bool GetCullBackface(){ return mbCullBackface;}

		void Set3DTransform(const cMatrixf& a_mtxTransform);
		const cMatrixf& Get3DTransform(){ return m_mtx3DTransform;}

		bool HasFocus();

		void PositionWidgetInsideBounds(iWidget* apWidget);


		void SetSkin(cGuiSkin* apSkin);
		cGuiSkin* GetSkin(){ return mpSkin;}

		cResources* GetResources(){ return mpResources;}

		bool IsDestroyingSet(){ return mbDestroyingSet;}

		void IncContextMenuZ() { mfContextMenuZ+=5; }
		void DecContextMenuZ() { mfContextMenuZ-=5; }

		float GetPopUpZ() { return mfLastPopUpZ; }
		void IncPopUpZ();

		void AddToTabOrder(iWidget* apWidget);
		void ClearTabOrder();

		iWidget* GetToolTipWidget() { return mpCurrentToolTipWidget; }
		void SetToolTipWidget(iWidget* apWidget) { mpCurrentToolTipWidget = apWidget; }

		void SetWidgetsNeedSorting() { mbSortWidgets = true; }

		void SetDefaultFocusNavWidget(iWidget* apWidget, bool abCheckForValidity=false);
		iWidget* GetDefaultFocusNavWidget() { return mpDefaultFocusNavWidget; }

		void PushDefaultFocusNavWidget();
		void PopDefaultFocusNavWidget();

		void SetDrawFocus(bool abX)												{ mbDrawFocus = abX; }
		void SetFocusDrawCallback(void* apObject, tGuiCallbackFunc apCallback)	{ mpFocusDrawObject = apObject; mpFocusDrawCallback = apCallback; }
		
		bool GetDrawFocus() { return mbDrawFocus; }

	private:
		void DrawTextFromCharArry(	const wchar_t* apString, iFontData *apFont,
									const cVector2f& avSize, const cVector3f& avPosition,
									const cColor& aColor, eGuiMaterial aMaterial,
									eFontAlign aAlign);
								

		void RenderClipRegion();

		void AddWidget(iWidget *apWidget,iWidget *apParent);

		bool OnMouseMove(const cGuiMessageData &aData);
		bool OnMouseDown(const cGuiMessageData& aData);
		bool OnMouseUp(const cGuiMessageData& aData);
		bool OnMouseDoubleClick(const cGuiMessageData& aData);

		bool OnKeyPress(const cGuiMessageData& aData);
		bool OnKeyRelease(const cGuiMessageData& aData);

		bool OnGamepadInput(const cGuiMessageData& aData);

		bool OnUIArrowPress(const cGuiMessageData& aData);
		bool OnUIArrowRelease(const cGuiMessageData& aData);
		bool OnUIButtonPress(const cGuiMessageData& aData);
		bool OnUIButtonRelease(const cGuiMessageData& aData);
		bool OnUIButtonDoublePress(const cGuiMessageData& aData);

		bool OnGetUINavFocus(const cGuiMessageData& aData);
		bool OnLoseUINavFocus(const cGuiMessageData& aData);

		bool DrawMouse(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(DrawMouse);

		bool DrawFocus(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(DrawFocus);

		bool ExecShortcut(const cKeyPress& aKeyPress);

		void UpdateToolTip(float afTimeStep);

		void CreateToolTipWidgets();

		eUIArrow TranslateKeyToUIArrow(eKey aKey);

		cGui *mpGui;
		cGuiSkin *mpSkin;

		tString msName;

		cResources *mpResources;
		cGraphics* mpGraphics;
		cSound *mpSound;
		cScene *mpScene;

		iWidget *mpAttentionWidget;
		tWidgetList mlstAttentionStack;

		iWidget *mpFocusedWidget;
		tWidgetList mlstFocusedStack;

		iWidget* mpWidgetRoot;
		tWidgetList mlstWidgets;

		tGuiRenderObjectSet m_setRenderObjects;

		int mlPopupCount;
		float mfLastPopUpZ;

		float mfWindowZ;

		float mfContextMenuZ;

		cVector2f mvVirtualSize;
		cVector2f mvVirtualSizeOffset;
		float mfVirtualMinZ;
		float mfVirtualMaxZ;

		cVector3f mvDrawOffset;

		bool mbCullBackface;
		bool mbIs3D;
		bool mbRendersBeforePostEffects;
		cVector3f mv3DSize;
		cMatrixf m_mtx3DTransform;
		int mlDrawPrio;
		
		bool mbActive;
		bool mbDrawMouse;
		float mfMouseZ;
		cGuiGfxElement *mpGfxCurrentPointer;
				
		tBoolVec mvMouseDown;
		cVector2f mvMousePos;
		bool mbMouseMovementEnabled;

		tGuiPopUpList mlstPopUps;

		cGuiClipRegion mBaseClipRegion;
		cGuiClipRegion* mpCurrentClipRegion;

		tShortcutList mlstShortcuts;

		tWidgetList mlstWindows;

		bool mbSortWidgets;

		///////////////////////////////////
		// Tool tips
		float mfToolTipTimer;
		float mfToolTipTimeToPopUp;
		iWidget* mpCurrentToolTipWidget;
		bool mbToolTipUpdated;

		cWidgetFrame* mpFrameToolTip;
		cWidgetFrame* mpFrameBGToolTip;
		cWidgetLabel* mpLabelToolTip;

		////////////////////////////////////
		// Tab Order
		tWidgetList mlstTabOrderWidgets;
		iWidget* mpTabOrderWidget;

		bool mbDestroyingSet;

		bool mbOldDestroyingSet;
		iWidget* mpTopMostDestroyingWidget;

		iWidget*	mpDefaultFocusNavWidget;
		tWidgetList	mlstDefaultFocusStack;

		bool				mbDrawFocus;
		void*				mpFocusDrawObject;
		tGuiCallbackFunc	mpFocusDrawCallback;
	};

};
#endif // HPL_GUI_SET_H
