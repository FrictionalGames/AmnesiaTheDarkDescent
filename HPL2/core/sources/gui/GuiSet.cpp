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

#include "gui/GuiSet.h"

#include "math/Math.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/Graphics.h"
#include "graphics/FontData.h"

#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/ImageManager.h"
#include "graphics/FrameSubImage.h"
#include "graphics/FrameBitmap.h"
#include "resources/FileSearcher.h"

#include "scene/Scene.h"
#include "scene/Camera.h"

#include "input/Input.h"
#include "input/Keyboard.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/Widget.h"
#include "gui/GuiMaterial.h"
#include "gui/GuiGfxElement.h"
#include "gui/GuiPopUp.h"

#include "gui/GuiPopUpMessageBox.h"
#include "gui/GuiPopUpFilePicker.h"
#include "gui/GuiPopUpColorPicker.h"
#include "gui/GuiPopUpUIKeyboard.h"

#include "gui/WidgetWindow.h"
#include "gui/WidgetFrame.h"
#include "gui/WidgetButton.h"
#include "gui/WidgetLabel.h"
#include "gui/WidgetSlider.h"
#include "gui/WidgetTextBox.h"
#include "gui/WidgetCheckBox.h"
#include "gui/WidgetImage.h"
#include "gui/WidgetListBox.h"
#include "gui/WidgetMultiPropertyListBox.h"
#include "gui/WidgetComboBox.h"
#include "gui/WidgetMenuItem.h"
#include "gui/WidgetContextMenu.h"
#include "gui/WidgetMainMenu.h"
#include "gui/WidgetTabFrame.h"
#include "gui/WidgetGroup.h"
#include "gui/WidgetDummy.h"

#include <stdarg.h>
#include <stdlib.h>

#include <algorithm>

namespace hpl {

	//-----------------------------------------------------------------------

	// This is temporary, but works now. Used for sorting widgets Z-wise before sending input

	static bool SortWidget_Z (const iWidget* apWidgetA, const iWidget* apWidgetB)
	{
		float fAZ, fBZ;
				
		fAZ = ((iWidget*)apWidgetA)->GetGlobalPosition().z;
		fBZ = ((iWidget*)apWidgetB)->GetGlobalPosition().z;
		return (fAZ > fBZ);
	}

	//-----------------------------------------------------------------------
	

	//////////////////////////////////////////////////////////////////////////
	// RENDER OBJECT
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cGuiRenderObjectCompare::operator()(	const cGuiRenderObject& aObjectA, 
												const cGuiRenderObject& aObjectB)
	{
		//Z
		float fZA = aObjectA.mvPos.z;
		float fZB = aObjectB.mvPos.z;
		if(fZA != fZB)
		{
			return fZA < fZB;
		}

		//Clip Region
		cGuiClipRegion *pClipA = aObjectA.mpClipRegion;
		cGuiClipRegion *pClipB = aObjectB.mpClipRegion;
		if(pClipA != pClipB)
		{
			return pClipA > pClipB;
		}
		
		//Material
		iGuiMaterial *pMaterialA = aObjectA.mpCustomMaterial ? aObjectA.mpCustomMaterial : aObjectA.mpGfx->mpMaterial;
		iGuiMaterial *pMaterialB = aObjectB.mpCustomMaterial ? aObjectB.mpCustomMaterial : aObjectB.mpGfx->mpMaterial;
		if(pMaterialA != pMaterialB)
		{
			return pMaterialA > pMaterialB;
		}
		
		//Texture
		iTexture *pTextureA = aObjectA.mpGfx->mvTextures[0];
		iTexture *pTextureB = aObjectB.mpGfx->mvTextures[0];
		if(pTextureA != pTextureB)
		{
			return pTextureA > pTextureB;
		}
		
		//Equal
		return false;

	}

	//////////////////////////////////////////////////////////////////////////
	// CLIP REGION
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	cGuiClipRegion::~cGuiClipRegion()
	{
		Clear();
	}

	void cGuiClipRegion::Clear()
	{
		STLDeleteAll(mlstChildren);
	}

	cGuiClipRegion* cGuiClipRegion::CreateChild(const cVector3f &avPos, const cVector2f &avSize)
	{
		cGuiClipRegion *pRegion = hplNew( cGuiClipRegion, () );
		
		if(mRect.w <0)
		{
			pRegion->mRect = cRect2f(cVector2f(avPos.x, avPos.y),avSize);	
		}
		else
		{
			cRect2f t = cRect2f(cVector2f(avPos.x, avPos.y),avSize);
			pRegion->mRect = cMath::GetClipRect(t, mRect);
			if(pRegion->mRect.w < 0 ) pRegion->mRect.w = 0;
			if(pRegion->mRect.h < 0 ) pRegion->mRect.h = 0;
		}
			

		mlstChildren.push_back(pRegion);

		return pRegion;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGuiGlobalShortcut::cGuiGlobalShortcut(int alKeyModifiers, eKey aKey, iWidget* apWidget, eGuiMessage aMessage, bool abBypassVisibility, bool abBypassEnabled)
	{
		mbEnabled = true;
		mbBypassVisibility = abBypassVisibility;
		mbBypassEnabled = abBypassEnabled;

		mKey = cKeyPress(aKey, 0, alKeyModifiers);
		mpWidget = apWidget;
		mMessage = aMessage;
	}

	//-----------------------------------------------------------------------

	bool cGuiGlobalShortcut::ProcessKeyPress(const cKeyPress& aKey)
	{
		if(IsEnabled()==false ||
			mKey.mlModifier != aKey.mlModifier ||
			mKey.mKey != aKey.mKey)
			return false;

		return Exec();
	}

	//-----------------------------------------------------------------------

	bool cGuiGlobalShortcut::Exec()
	{
		if(mpWidget)
			return mpWidget->ProcessMessage(mMessage, cGuiMessageData(), mbBypassVisibility, mbBypassEnabled);

		return false;
	}

	//-----------------------------------------------------------------------

	tString cGuiGlobalShortcut::ToString()
	{
		tString sText;

		if(mpWidget)
		{
			iKeyboard* pKB = mpWidget->GetSet()->GetGui()->GetInput()->GetKeyboard();
			for(int i=0;i<eKeyModifier_LastEnum;++i)
			{
				eKeyModifier mod = (eKeyModifier) cMath::Pow2(i);
				if(mKey.mlModifier & mod)
				{
					if(sText!="")
						sText+= "+";
					sText += pKB->ModifierKeyToString(mod);
				}
			}
			if(mKey.mKey!=eKey_None)
			{
				if(sText!="")
					sText += "+";
				sText += pKB->KeyToString(mKey.mKey);
			}
		}

		return sText;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGuiSet::cGuiSet(	const tString &asName, cGui *apGui, cGuiSkin *apSkin, 
						cResources *apResources, cGraphics* apGraphics,
						cSound *apSound, cScene *apScene)
	{
		mpGui = apGui;
		mpSkin = NULL;
		
		msName = asName;
        
		mpResources = apResources;
		mpGraphics = apGraphics;
		mpSound = apSound;
		mpScene = apScene;

		mpGfxCurrentPointer = NULL;

		mpFocusedWidget = NULL;

		mpAttentionWidget = NULL;

		mlPopupCount =0;
		mfLastPopUpZ = 20;

		mvDrawOffset =0;

		mfContextMenuZ = 500;

		mvVirtualSize = mpGraphics->GetLowLevel()->GetScreenSizeFloat();
		mfVirtualMinZ = -1000;
		mfVirtualMaxZ = 1000;
		mvVirtualSizeOffset = cVector2f(0);

		mbActive = true;
		mbDrawMouse = true;
		mfMouseZ =mfVirtualMaxZ;

		mbIs3D = false;
		mbRendersBeforePostEffects = false;
		mv3DSize = 1;
		m_mtx3DTransform = cMatrixf::Identity;
		mbCullBackface = false;

		mpWidgetRoot = hplNew( iWidget, (eWidgetType_Root,this,mpSkin) );
		mpWidgetRoot->AddCallback(eGuiMessage_OnDraw,this,kGuiCallback(DrawMouse));
		mpWidgetRoot->AddCallback(eGuiMessage_OnDraw,this,kGuiCallback(DrawFocus));

		mpCurrentClipRegion = &mBaseClipRegion;

		mbDestroyingSet = false;

		mlDrawPrio = 0;
		
		// 9 mouse buttons defined in InputTypes and GuiTypes
		mvMouseDown.resize(9);
		for(int i=0; i<(int)mvMouseDown.size(); ++i) mvMouseDown[i] = false;
		mbMouseMovementEnabled = true;

		SetSkin(apSkin);

		mfToolTipTimer = 0;
		mfToolTipTimeToPopUp = 0.5f;
		mpCurrentToolTipWidget = NULL;

		mpFrameToolTip = NULL;
		mpFrameBGToolTip = NULL;
		mpLabelToolTip = NULL;

		mpTabOrderWidget = NULL;
		mfWindowZ = -1;

		if(mpSkin)
		{
			CreateToolTipWidgets();
		}

		mpTopMostDestroyingWidget = NULL;

		mpDefaultFocusNavWidget = NULL;
		mbDrawFocus = false;
		mpFocusDrawObject = NULL;
		mpFocusDrawCallback = NULL;

		mbSortWidgets = false;
	}

	//-----------------------------------------------------------------------

	cGuiSet::~cGuiSet()
	{
		mbDestroyingSet = true;
		{
			STLDeleteAll(mlstPopUps);
			STLDeleteAll(mlstWidgets);
			hplDelete(mpWidgetRoot);
		}
		mbDestroyingSet = false;

		ClearGlobalShortcuts();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGuiSet::Update(float afTimeStep)
	{
		/////////////////////////////
		// Popups
		if(mlstPopUps.empty()==false)
		{
			STLDeleteAll(mlstPopUps);
		}

		/////////////////////////////
		// Update widgets
		tWidgetListIt it = mlstWidgets.begin();
		for(; it != mlstWidgets.end(); ++it)
		{
			iWidget *pWidget = *it;
			pWidget->Update(afTimeStep);       
		}

		/////////////////////////////
		// Update ToolTip
		UpdateToolTip(afTimeStep);
	}

	//-----------------------------------------------------------------------

	void cGuiSet::DrawAll(float afTimeStep)
	{
		if(mbActive==false) return;

		///////////////////////////////
		//Draw all widgets
		SetCurrentClipRegion(&mBaseClipRegion);
		mpWidgetRoot->Draw(afTimeStep, &mBaseClipRegion);
		
		SetCurrentClipRegion(&mBaseClipRegion);
		
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::SendMessage(eGuiMessage aMessage, const cGuiMessageData& aData)
	{
		// Sort widgets so that highest Z value mean first on the list (if needed)
		if(mbSortWidgets)
		{
			mbSortWidgets = false;
			mlstWidgets.sort(SortWidget_Z);
		}

		switch(aMessage)
		{
			case eGuiMessage_MouseMove:				return OnMouseMove(aData);
			case eGuiMessage_MouseDown:				return OnMouseDown(aData);
			case eGuiMessage_MouseUp:				return OnMouseUp(aData);
			case eGuiMessage_MouseDoubleClick:		return OnMouseDoubleClick(aData);

			case eGuiMessage_KeyPress:				return OnKeyPress(aData);
			case eGuiMessage_KeyRelease:			return OnKeyRelease(aData);

			case eGuiMessage_GamepadInput:			return OnGamepadInput(aData);

			case eGuiMessage_UIArrowPress:			return OnUIArrowPress(aData);
			case eGuiMessage_UIArrowRelease:		return OnUIArrowRelease(aData);
			case eGuiMessage_UIButtonPress:			return OnUIButtonPress(aData);
			case eGuiMessage_UIButtonRelease:		return OnUIButtonRelease(aData);
			case eGuiMessage_UIButtonDoublePress:	return OnUIButtonDoublePress(aData);
		}
		
		return false;
	}

	//-----------------------------------------------------------------------

	cGuiGlobalShortcut* cGuiSet::AddGlobalShortcut(int alKeyModifiers, eKey aKey, iWidget* apWidget, eGuiMessage aMessage, bool abBypassVisibility, bool abBypassEnabled)
	{
		cGuiGlobalShortcut* pShortcut = hplNew(cGuiGlobalShortcut, (alKeyModifiers, aKey, 
																	apWidget, aMessage, 
																	abBypassVisibility, abBypassEnabled));

		mlstShortcuts.push_back(pShortcut);

		return pShortcut;
	}

	//-----------------------------------------------------------------------

	void cGuiSet::RemoveGlobalShortcut(cGuiGlobalShortcut* apShortcut)
	{
		if(apShortcut==NULL)
			return;

		tShortcutListIt it = find(mlstShortcuts.begin(), mlstShortcuts.end(), apShortcut);
		if(it!=mlstShortcuts.end())
		{
			cGuiGlobalShortcut* pShortcut = *it;
			mlstShortcuts.erase(it);

			hplDelete(pShortcut);
		}
	}


	//-----------------------------------------------------------------------

	void cGuiSet::ClearGlobalShortcuts()
	{
		STLDeleteAll(mlstShortcuts);
	}

	//-----------------------------------------------------------------------
	
	void cGuiSet::DestroyAllWidgets()
	{
		mbDestroyingSet = true;

		STLDeleteAll(mlstPopUps);
		STLDeleteAll(mlstWidgets);
		mpWidgetRoot->mlstChildren.clear();

		mpFocusedWidget = NULL;
		mpAttentionWidget = NULL;

		mbDestroyingSet = false;

		mpCurrentToolTipWidget = NULL;

		CreateToolTipWidgets();
	}

	//-----------------------------------------------------------------------

	void cGuiSet::ResetMouseOver()
	{
		tWidgetListIt it = mlstWidgets.begin();
		for(; it != mlstWidgets.end(); ++it)
		{
			iWidget *pWidget = *it;
			pWidget->SetMouseIsOver(false);
		}
	}

	//-----------------------------------------------------------------------

	//TODO: Support multi textures
	void cGuiSet::Render(cFrustum *apFrustum)
	{
		iLowLevelGraphics *pLowLevelGraphics = mpGraphics->GetLowLevel();
	
		///////////////////////////////////
		// Init rendering

		//3D projection
		if(mbIs3D)
		{
			pLowLevelGraphics->SetDepthTestActive(true);
			pLowLevelGraphics->SetDepthWriteActive(false);

			//Invert the y coordinate: = -y, this also get the gui into the correct position.
			//Also scale to size
			cVector3f vPreScale = cVector3f(mv3DSize.x / mvVirtualSize.x,
											-mv3DSize.y / mvVirtualSize.y,
											mv3DSize.z / (mfVirtualMaxZ - mfVirtualMinZ));
			cMatrixf mtxPreMul = cMath::MatrixScale(vPreScale);
			//note: Offset needs to be converted to shape coords (done by multiplying with pre scale)
			mtxPreMul.SetTranslation(cVector3f(mvVirtualSizeOffset.x*vPreScale.x, mvVirtualSizeOffset.y*vPreScale.y, 0));
			
			//Create the final model matrix
			cMatrixf mtxModel = cMath::MatrixMul(m_mtx3DTransform, mtxPreMul);
			mtxModel = cMath::MatrixMul(apFrustum->GetViewMatrix(), mtxModel);
			
			pLowLevelGraphics->SetMatrix(eMatrix_ModelView, mtxModel);

			//No need for projection matrix, should be setup, right? :)

			pLowLevelGraphics->SetCullActive(mbCullBackface);
		}
		//Screen projection
		else
		{
			pLowLevelGraphics->SetDepthTestActive(false);
			pLowLevelGraphics->SetDepthWriteActive(false);
			pLowLevelGraphics->SetIdentityMatrix(eMatrix_ModelView);

			//Set up min and max for orth projection
			cVector3f vProjMin(-mvVirtualSizeOffset.x, -mvVirtualSizeOffset.y, mfVirtualMinZ);
			cVector3f vProjMax(mvVirtualSize.x-mvVirtualSizeOffset.x, mvVirtualSize.y-mvVirtualSizeOffset.y, mfVirtualMaxZ);

			pLowLevelGraphics->SetOrthoProjection(vProjMin,vProjMax);
		}
		
		///////////////////////////////
		// Render all clip regions
		
		RenderClipRegion();

		///////////////////////////////
		//Clear the render object set
		mBaseClipRegion.Clear();

		if(mbIs3D)
		{
			if(mbCullBackface==false) pLowLevelGraphics->SetCullActive(true);
		}
	}

	//-----------------------------------------------------------------------

	void cGuiSet::ClearRenderObjects()
	{
		m_setRenderObjects.clear();
	}

	//-----------------------------------------------------------------------
	
	void cGuiSet::DrawGfx(	cGuiGfxElement* apGfx, const cVector3f &avPos, const cVector2f &avSize,
							const cColor& aColor,eGuiMaterial aMaterial,
							float afRotationAngle, 
							bool abUseCustomPivot, const cVector3f& avCustomPivot)
	{
		if(mpCurrentClipRegion==NULL) return;
		if(mpCurrentClipRegion->mRect.w ==0 || mpCurrentClipRegion->mRect.h==0) return;

		//Log("Bug:Drawing gfx: %p\n", apGfx);

		cVector3f vAbsPos =  avPos + apGfx->GetOffset() + mvDrawOffset;
		if(mpCurrentClipRegion->mRect.w >0)
		{
			cRect2f gfxRect;
			gfxRect.x = vAbsPos.x;
			gfxRect.y = vAbsPos.y;
			if(avSize.x < 0)
			{
				gfxRect.w = apGfx->GetImageSize().x;
				gfxRect.h = apGfx->GetImageSize().y;
			}
			else
			{
				gfxRect.w = avSize.x;
				gfxRect.h = avSize.y;
			}
			
			if(cMath::CheckRectIntersection(mpCurrentClipRegion->mRect,gfxRect)==false) return;
		}
		
		apGfx->Flush();

		cGuiRenderObject object;

		//Log("Clip: %f %f\n",mpCurrentClipRegion->mRect.w,mpCurrentClipRegion->mRect.h);

		//////////////////////////
		//Set Graphics
		object.mpGfx = apGfx;
		object.mpClipRegion = mpCurrentClipRegion;

		///////////////////////////
		//Position, size and color
		object.mvPos = vAbsPos;
		if(avSize.x < 0)	object.mvSize = apGfx->GetImageSize();
		else				object.mvSize = avSize;
		object.mColor = aColor;
		
		///////////////////////////
		//Material
		if(aMaterial != eGuiMaterial_LastEnum)	object.mpCustomMaterial = mpGui->GetMaterial(aMaterial);
		else									object.mpCustomMaterial = NULL;	

		///////////////////////////
		//Rotation
		if(afRotationAngle !=0)
		{
			object.mbRotated = true;
			object.mfAngle = afRotationAngle;
			
			if(abUseCustomPivot)
				object.mvPivot = avCustomPivot;
			else
				object.mvPivot = object.mvSize*0.5f;
		}
		else
		{
			object.mbRotated = false;
		}

		m_setRenderObjects.insert(object);
	}

	//-----------------------------------------------------------------------

	void cGuiSet::DrawFont(	const tWString &asText,
							iFontData *apFont, const cVector3f &avPos,
							const cVector2f &avSize, const cColor& aColor,
							eFontAlign aAlign, eGuiMaterial aMaterial)
	{
		DrawTextFromCharArry(asText.c_str(), apFont,avSize,avPos,aColor,aMaterial,aAlign);		
	}

	//-----------------------------------------------------------------------
	
	static wchar_t gsTempTextArray[1024];
	void cGuiSet::DrawFont (iFontData *apFont, const cVector3f &avPos,
							const cVector2f &avSize, const cColor& aColor,
							eFontAlign aAlign,eGuiMaterial aMaterial,
							const wchar_t* fmt,...)
	{
		va_list ap;	
		if (fmt == NULL) return;	
		va_start(ap, fmt);
		vswprintf(gsTempTextArray, 1023, fmt, ap);
		va_end(ap);

		DrawTextFromCharArry(gsTempTextArray, apFont,avSize,avPos,aColor,aMaterial,aAlign);
	}
	
	void cGuiSet::DrawFont (	iFontData *apFont, const cVector3f &avPos,
								const cVector2f &avSize, const cColor& aColor,
								const wchar_t* fmt,...)
	{
		va_list ap;	
		if (fmt == NULL) return;	
		va_start(ap, fmt);
		vswprintf(gsTempTextArray, 1023, fmt, ap);
		va_end(ap);

		DrawTextFromCharArry(gsTempTextArray, apFont,avSize,avPos,aColor,eGuiMaterial_FontNormal,eFontAlign_Left);
	}

	
	//-----------------------------------------------------------------------

	cWidgetWindow* cGuiSet::CreateWidgetWindow(	tWidgetWindowButtonFlag alFlags,
												const cVector3f &avLocalPos, 
												const cVector2f &avSize,
												const tWString &asText,
												iWidget *apParent,
												const tString& asName)
	{
		cWidgetWindow *pWindow = hplNew( cWidgetWindow, (this,mpSkin, alFlags) );
		pWindow->SetPosition(avLocalPos);
		pWindow->SetSize(avSize);
		pWindow->SetText(asText);
		pWindow->SetName(asName);
		AddWidget(pWindow, apParent);

		mlstWindows.push_back(pWindow);

		return pWindow;
	}

	cWidgetFrame* cGuiSet::CreateWidgetFrame(	const cVector3f &avLocalPos,
												const cVector2f &avSize,
												bool abDrawFrame,
												iWidget *apParent,
												bool abHScrollBar, bool abVScrollBar,
												const tString& asName)
	{
		cWidgetFrame *pFrame = hplNew( cWidgetFrame, (this,mpSkin, abHScrollBar, abVScrollBar) );
		pFrame->SetPosition(avLocalPos);
		pFrame->SetSize(avSize);
		pFrame->SetDrawFrame(abDrawFrame);
		pFrame->SetName(asName);
		AddWidget(pFrame,apParent);
		return pFrame;
	}

	cWidgetButton* cGuiSet::CreateWidgetButton(	const cVector3f &avLocalPos,
												const cVector2f &avSize,
												const tWString &asText,
												iWidget *apParent,
												bool abToggleable,
												const tString& asName)
	{
		cWidgetButton *pButton = hplNew( cWidgetButton, (this,mpSkin) );
		pButton->SetPosition(avLocalPos);
		pButton->SetSize(avSize);
		pButton->SetText(asText);
		pButton->SetName(asName);
		pButton->SetToggleable(abToggleable);
		AddWidget(pButton,apParent);
		return pButton;
	}

	cWidgetLabel* cGuiSet::CreateWidgetLabel(	const cVector3f &avLocalPos,
												const cVector2f &avSize,
												const tWString &asText,
												iWidget *apParent,
												const tString& asName)
	{
		cWidgetLabel *pLabel = hplNew( cWidgetLabel, (this,mpSkin) );
		pLabel->SetPosition(avLocalPos);
		pLabel->SetSize(avSize);
		pLabel->SetText(asText);
		pLabel->SetName(asName);
		
		if(avSize == -1)
		{
			pLabel->SetAutogenerateSize(true);
			pLabel->SetAutogenerateSize(false);
		}
		
		AddWidget(pLabel,apParent);
		return pLabel;
	}

	cWidgetSlider* cGuiSet::CreateWidgetSlider(	eWidgetSliderOrientation aOrientation,
												const cVector3f &avLocalPos,
												const cVector2f &avSize,
												int alMaxValue,
                                                iWidget *apParent,
												const tString& asName)
	{
		cWidgetSlider *pSlider = hplNew( cWidgetSlider, (this,mpSkin,aOrientation) );
		pSlider->SetPosition(avLocalPos);
		pSlider->SetSize(avSize);
		pSlider->SetMaxValue(alMaxValue);
		pSlider->SetName(asName);
		AddWidget(pSlider,apParent);
		return pSlider;
	}

	cWidgetTextBox* cGuiSet::CreateWidgetTextBox( const cVector3f &avLocalPos,
												  const cVector2f &avSize,
												  const tWString &asText,
												  iWidget *apParent,
												  eWidgetTextBoxInputType aType,
												  float afNumericAdd,
												  bool abShowButtons,
												  const tString& asName)
	{
		cWidgetTextBox *pTextBox = hplNew( cWidgetTextBox, (this,mpSkin,aType) );
		pTextBox->SetPosition(avLocalPos);
		pTextBox->SetSize(avSize);
		pTextBox->SetShowButtons(abShowButtons);
		pTextBox->SetNumericAdd(afNumericAdd);
		pTextBox->SetText(asText);
		pTextBox->SetName(asName);
		AddWidget(pTextBox,apParent);
		return pTextBox;
	}

	cWidgetCheckBox* cGuiSet::CreateWidgetCheckBox(	const cVector3f &avLocalPos,
													const cVector2f &avSize,
													const tWString &asText,
													iWidget *apParent,
													const tString& asName)
	{
		cWidgetCheckBox *pCheckBox = hplNew( cWidgetCheckBox, (this,mpSkin) );
		pCheckBox->SetPosition(avLocalPos);
		pCheckBox->SetSize(avSize);
		pCheckBox->SetText(asText);
		pCheckBox->SetName(asName);
		AddWidget(pCheckBox,apParent);
		return pCheckBox;
	}

	cWidgetImage* cGuiSet::CreateWidgetImage(const tString &asFile,
											const cVector3f &avLocalPos,
											const cVector2f &avSize,
											eGuiMaterial aMaterial,
											bool abAnimate,
											iWidget *apParent,
											const tString& asName)
	{
		cWidgetImage *pImage = hplNew( cWidgetImage, (this,mpSkin) );
		cGuiGfxElement *pGfx = NULL;
		if(asFile != "")
		{
			if(abAnimate)
			{
				pGfx = mpGui->CreateGfxImageBuffer(asFile,aMaterial,true);
			}
			else
			{
				pGfx = mpGui->CreateGfxImage(asFile,aMaterial);
			}
		}
		pImage->SetPosition(avLocalPos);

		if(pGfx && avSize.x <0) 
		{
			pImage->SetSize(pGfx->GetImageSize());
		}
		else
		{
			pImage->SetSize(avSize);
		}

		pImage->SetImage(pGfx);

		pImage->SetName(asName);

		AddWidget(pImage,apParent);
		return pImage;
	}

	cWidgetListBox* cGuiSet::CreateWidgetListBox(const cVector3f &avLocalPos,
												const cVector2f &avSize,
												iWidget *apParent,
												const tString& asName)
	{
		cWidgetListBox *pListBox = hplNew( cWidgetListBox,(this,mpSkin) );
		pListBox->SetPosition(avLocalPos);
		pListBox->SetSize(avSize);
		pListBox->SetName(asName);
		AddWidget(pListBox,apParent);
		return pListBox;
	}

	cWidgetMultiPropertyListBox* cGuiSet::CreateWidgetMultiPropertyListBox(	const cVector3f& avLocalPos,
																		const cVector2f& avSize,
																		iWidget* apParent,
																		const tString& asName)
	{
		cWidgetMultiPropertyListBox* pListBox = hplNew( cWidgetMultiPropertyListBox, (this,mpSkin));
		pListBox->SetPosition(avLocalPos);
		pListBox->SetSize(avSize);
		pListBox->SetName(asName);
		AddWidget(pListBox,apParent);
		return pListBox;
	}

	cWidgetComboBox* cGuiSet::CreateWidgetComboBox(	const cVector3f &avLocalPos,
													const cVector2f &avSize,
													const tWString &asText,
													iWidget *apParent,
													const tString& asName)
	{
		cWidgetComboBox *pComboBox = hplNew( cWidgetComboBox, (this,mpSkin) );
		pComboBox->SetPosition(avLocalPos);
		pComboBox->SetSize(avSize);
		pComboBox->SetText(asText);
		pComboBox->SetName(asName);
		AddWidget(pComboBox,apParent);
		return pComboBox;
	}

	cWidgetMenuItem* cGuiSet::CreateWidgetMenuItem(	const cVector3f &avLocalPos,
													const cVector2f &avSize,
													const tWString &asText,
													iWidget *apParent,
													const tString& asName)
	{
		cWidgetMenuItem *pItem = hplNew( cWidgetMenuItem, ((iWidgetMenu*)apParent) );
		pItem->SetPosition(avLocalPos);
		pItem->SetSize(avSize);
		pItem->SetText(asText);
		pItem->SetName(asName);
		AddWidget(pItem,apParent);
		return pItem;
	}

	cWidgetContextMenu* cGuiSet::CreateWidgetContextMenu(	const cVector3f &avLocalPos,
													const cVector2f &avSize,
													const tWString &asText,
													iWidget *apParent,
													const tString& asName)
	{
		cWidgetContextMenu *pMenu = hplNew( cWidgetContextMenu, (this,mpSkin) );
		pMenu->SetPosition(avLocalPos);
		pMenu->SetSize(avSize);
		pMenu->SetText(asText);
		pMenu->SetName(asName);
		AddWidget(pMenu,apParent);
		return pMenu;
	}

	cWidgetMainMenu* cGuiSet::CreateWidgetMainMenu(iWidget *apParent,
													const tString& asName)
	{
		cWidgetMainMenu *pMenu = hplNew( cWidgetMainMenu, (this,mpSkin) );
		pMenu->SetName(asName);
		AddWidget(pMenu,apParent);
		return pMenu;
	}

	cWidgetTabLabel* cGuiSet::CreateWidgetTabLabel(	const cVector3f &avLocalPos,
													const cVector2f &avSize,
													const tWString &asText,
													iWidget *apParent,
													const tString& asName)
	{
		cWidgetTabLabel *pLabel = hplNew( cWidgetTabLabel, (this,mpSkin) );
		pLabel->SetPosition(avLocalPos);
		pLabel->SetSize(avSize);
		pLabel->SetText(asText);
		pLabel->SetName(asName);
		AddWidget(pLabel,apParent);
		return pLabel;
	}

	cWidgetTab* cGuiSet::CreateWidgetTab(	const cVector3f &avLocalPos,
													const cVector2f &avSize,
													const tWString &asText,
													iWidget *apParent,
													const tString& asName)
	{
		cWidgetTab *pTab = hplNew( cWidgetTab, ((cWidgetTabFrame*)apParent) );
        pTab->SetPosition(avLocalPos);
		pTab->SetSize(avSize);
		pTab->SetText(asText);
		pTab->SetName(asName);

		AddWidget(pTab,apParent);
		return pTab;
	}

	cWidgetTabFrame* cGuiSet::CreateWidgetTabFrame(	const cVector3f &avLocalPos,
													const cVector2f &avSize,
													const tWString &asText,
													iWidget *apParent,
													bool abAllowHScroll,
													bool abAllowVScroll,
													const tString& asName)
	{
		cWidgetTabFrame *pFrame = hplNew( cWidgetTabFrame, (this,mpSkin) );
        pFrame->SetPosition(avLocalPos);
		pFrame->SetSize(avSize);
		pFrame->SetText(asText);
		pFrame->SetName(asName);
		pFrame->SetHorizontalScrollEnabled(abAllowHScroll);
		pFrame->SetVerticalScrollEnabled(abAllowVScroll);
		AddWidget(pFrame,apParent);
		return pFrame;
	}

	cWidgetGroup* cGuiSet::CreateWidgetGroup(const cVector3f &avLocalPos,
												const cVector2f &avSize,
												const tWString &asText,
												iWidget *apParent,
												const tString& asName)
	{
		cWidgetGroup* pGroup = hplNew( cWidgetGroup, (this,mpSkin) );
		pGroup->SetPosition(avLocalPos);
		pGroup->SetSize(avSize);
		pGroup->SetText(asText);
		pGroup->SetName(asName);
		AddWidget(pGroup,apParent);
		return pGroup;
	}

	cWidgetDummy* cGuiSet::CreateWidgetDummy(const cVector3f &avLocalPos,
												iWidget* apParent,
												const tString &asName)
	{
		cWidgetDummy* pDummy = hplNew( cWidgetDummy, (this,mpSkin) );
		pDummy->SetPosition(avLocalPos);
		pDummy->SetName(asName);
		AddWidget(pDummy,apParent);
		return pDummy;	
	}

	//-----------------------------------------------------------------------

	iWidget * cGuiSet::GetWidgetFromName(const tString& asName)
	{
		return (iWidget*)STLFindByName(mlstWidgets, asName);
	}	

	//-----------------------------------------------------------------------

	void cGuiSet::DestroyWidget(iWidget *apWidget, bool abDestroyChildren)
	{
		if(apWidget == mpFocusedWidget) mpFocusedWidget = NULL;
		mlstTabOrderWidgets.remove(apWidget);
		mpTabOrderWidget = NULL;
		
		if(apWidget==mpCurrentToolTipWidget)
			SetToolTipWidget(NULL);

		if(apWidget==mpDefaultFocusNavWidget)
			mpDefaultFocusNavWidget = NULL;

		tWidgetList& lstChildren = apWidget->GetChildren();
		if(abDestroyChildren && lstChildren.empty()==false)
		{
			if(mpTopMostDestroyingWidget==NULL)
			{
				mpTopMostDestroyingWidget = apWidget;
				mbOldDestroyingSet = mbDestroyingSet;
				mbDestroyingSet = true;
			}

			tWidgetList& lstChildren = apWidget->GetChildren();
			tWidgetListIt it = lstChildren.begin();
			while(it!=lstChildren.end())
			{
				iWidget* pWidget = *it;
				DestroyWidget(pWidget, true);

				it = lstChildren.begin();
			}
		}

		STLFindAndDelete(mlstWidgets, apWidget);

		if(mpTopMostDestroyingWidget==apWidget)
		{
			mpTopMostDestroyingWidget = NULL;
			mbDestroyingSet = mbOldDestroyingSet;
		}
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::IsValidWidget(iWidget* apWidget)
	{
		if(apWidget==NULL)
			return true;

		tWidgetListIt it = find(mlstWidgets.begin(), mlstWidgets.end(), apWidget);
		return (it!=mlstWidgets.end());
	}

	//-----------------------------------------------------------------------

	cGuiPopUpMessageBox* cGuiSet::CreatePopUpMessageBox(	const tWString& asLabel, const tWString& asText,
								const tWString& asButton1, const tWString& asButton2,
								void *apCallbackObject, tGuiCallbackFunc apCallback)
	{
		cGuiPopUpMessageBox* pMessageBox = hplNew( cGuiPopUpMessageBox, (this, asLabel,asText,
																asButton1,asButton2, 
																apCallbackObject,apCallback) );
		
		return pMessageBox;
	}

	//-----------------------------------------------------------------------

	cGuiPopUpFilePicker* cGuiSet::CreatePopUpSaveFilePicker( tWString &asFileName, const tWString &asCategory, 
															 const tWString &asFilter, const tWString &asStartPath, bool abShowHidden,
															void *apCallbackObject, tGuiCallbackFunc apCallback, const tWString& asStartFilename)
	{
		cGuiPopUpFilePicker* pPicker = hplNew( cGuiPopUpFilePicker, (this, mpSkin, eFilePickerType_Save, asStartPath, abShowHidden, apCallbackObject, apCallback, asStartFilename) );
		pPicker->Init();
		pPicker->SetSaveFileDest( asFileName );

		pPicker->AddCategory( asCategory, asFilter );

		return pPicker;
	}

	//-----------------------------------------------------------------------

	/**	Returns a handle to the Load file popup, so new filters can be added outside. This handle doesn't need to be destroyed.
	 *
	 * \param &avFileList Reference to the destination file name string list
	 * \param abAddAllFilesFilter If the "all files" (*.*) filter should be added
	 * \param &asStartPath Where the popup should start browsing
	 * \param *apCallbackObject 
	 * \param apCallback 
	 * \return 
	 */
	cGuiPopUpFilePicker* cGuiSet::CreatePopUpLoadFilePicker( tWStringVec &avFileList, bool abAddAllFilesFilter, 
															 const tWString &asStartPath, bool abShowHidden,
															 void *apCallbackObject, tGuiCallbackFunc apCallback)
	{
		cGuiPopUpFilePicker* pPicker = hplNew( cGuiPopUpFilePicker, (this, mpSkin, eFilePickerType_Load, asStartPath, abShowHidden, apCallbackObject, apCallback) );
		pPicker->Init();
		pPicker->SetLoadFileListDest( avFileList );

		if(abAddAllFilesFilter)
			pPicker->AddCategory(_W("All files"), _W("*.*"));

		return pPicker;
	}

	//-----------------------------------------------------------------------

	cGuiPopUpColorPicker* cGuiSet::CreatePopUpColorPicker( cColor* apDestColor, const cVector3f& avPos,  void *apCallbackObject, tGuiCallbackFunc apCallback)
	{
		cGuiPopUpColorPicker* pPicker = hplNew( cGuiPopUpColorPicker,(this, mpSkin, avPos, apDestColor, apCallbackObject, apCallback));
		
		return pPicker;
	}
	
	//-----------------------------------------------------------------------

	cGuiPopUpUIKeyboard* cGuiSet::CreatePopUpUIKeyboard(cWidgetTextBox* apTarget)
	{
		cGuiPopUpUIKeyboard* pKB = hplNew(cGuiPopUpUIKeyboard,(apTarget, NULL, NULL));
		
		return pKB;
	}
	
	//-----------------------------------------------------------------------


	void cGuiSet::DestroyPopUp(iGuiPopUp *apPopUp)
	{
		//mlstPopUps.push_back(apPopUp);
		mlstPopUps.push_front(apPopUp);
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::PopUpIsActive()
	{
		return mlPopupCount > 0;
	}

	//-----------------------------------------------------------------------

	void cGuiSet::ShowContextMenu( cWidgetContextMenu* apMenu, const cVector3f& avPosition )
	{
		if(apMenu==NULL) return;

		cVector3f vPos = avPosition;

		if(avPosition.x + apMenu->GetSize().x > mvVirtualSize.x)
			vPos.x = mvVirtualSize.x - apMenu->GetSize().x;
		if(avPosition.y + apMenu->GetSize().y > mvVirtualSize.y)
			vPos.y = mvVirtualSize.y - apMenu->GetSize().y;
	
		apMenu->SetVisible(true);
		apMenu->SetEnabled(true);

		apMenu->SetHighlightedItem(NULL);

		if(	apMenu->GetParentItem()==NULL || 
			apMenu->GetParentMenu()->GetType()==eWidgetType_MainMenu)
		{
			vPos.z = mfContextMenuZ;
			IncContextMenuZ();
		}
		else
			vPos.z = apMenu->GetParentMenu()->GetGlobalPosition().z;


		SetAttentionWidget(apMenu->GetTopMostMenu());

		apMenu->SetGlobalPosition(vPos);
		SetFocusedWidget(apMenu);
	}

	//-----------------------------------------------------------------------

	void cGuiSet::RemoveWindow(cWidgetWindow* apWin)
	{
		mlstWindows.remove(apWin);
	}

	void cGuiSet::SetLastWindowZ(float afX)
	{
		mfWindowZ = afX; 
	}

	void cGuiSet::SetWindowOnTop(cWidgetWindow* apWin)
	{
		tWidgetListIt it = find(mlstWindows.begin(), mlstWindows.end(), apWin);
		if(it==mlstWindows.end())
			return;
		RemoveWindow(apWin);
		mlstWindows.push_back(apWin);

		it = mlstWindows.begin();
		float fHeight = 20.0f;
		for(;it!=mlstWindows.end();++it)
		{
			iWidget* pWin = *it;
			cVector3f vPos = pWin->GetGlobalPosition();
			vPos.z = fHeight;

			pWin->SetGlobalPosition(vPos);
			fHeight+=10.0f;
		}
	}

	//-----------------------------------------------------------------------

	void cGuiSet::SetActive(bool abX)
	{
		if(mbActive == abX) return;

		mbActive = abX;
	}

	//-----------------------------------------------------------------------

	void cGuiSet::SetDrawMouse(bool abX)
	{
		if(mbDrawMouse == abX) return;

		mbDrawMouse = abX;
	}

	//-----------------------------------------------------------------------

	void cGuiSet::SetRootWidgetClips(bool abX)
	{
		mpWidgetRoot->SetClipActive(abX);
		if(abX)
			mpWidgetRoot->SetSize(mvVirtualSize);
		else
			mpWidgetRoot->SetSize(0);
	}
	
	bool cGuiSet::GetRootWidgetClips()
	{
		return mpWidgetRoot->GetClipActive();
	}

	//-----------------------------------------------------------------------

	void cGuiSet::SetVirtualSize(const cVector2f& avSize, float afMinZ, float afMaxZ, const cVector2f& avOffset)
	{
		mvVirtualSize = avSize;
		mfVirtualMinZ = afMinZ;
		mfVirtualMaxZ = afMaxZ;
		mvVirtualSizeOffset = avOffset;
	}

	//-----------------------------------------------------------------------

	void cGuiSet::SetFocusedWidget(iWidget* apWidget, bool abCheckForValidity)
	{
		if(mpFocusedWidget==apWidget)
			return;
		
		if(abCheckForValidity && IsValidWidget(apWidget)==false)
			apWidget = NULL;

		iWidget* pOldFocus = mpFocusedWidget;
		cGuiMessageData data = cGuiMessageData(mvMousePos,0);

		mpFocusedWidget = apWidget;

		if(mpFocusedWidget)
			mpFocusedWidget->GetFocus(data);
		
		if(pOldFocus)
			pOldFocus->ProcessMessage(eGuiMessage_LostFocus, data);
	}

	//-----------------------------------------------------------------------

	void cGuiSet::PushFocusedWidget()
	{
		mlstFocusedStack.push_back(mpFocusedWidget);
	}

	void cGuiSet::PopFocusedWidget()
	{
		if(mlstFocusedStack.empty()) return;

		SetFocusedWidget(mlstFocusedStack.back(), true);
		mlstFocusedStack.pop_back();
	}

	//-----------------------------------------------------------------------

	void cGuiSet::SetAttentionWidget(iWidget *apWidget, bool abClearFocus, bool abCheckForValidity)
	{
		if(mpAttentionWidget == apWidget) return;

		if(abCheckForValidity && IsValidWidget(apWidget)==false)
			apWidget = NULL;

		mpAttentionWidget = apWidget;
		cGuiMessageData data = cGuiMessageData(mvMousePos, 0);

		//Log("Sett attn: %d\n",mpAttentionWidget);
		iWidget* pOldFocus = mpFocusedWidget;
		
		if(mpFocusedWidget && mpFocusedWidget->IsConnectedTo(mpAttentionWidget)==false)
		{
			//Log("Lost focus %d\n",mpFocusedWidget);
			
			if(mpAttentionWidget!= NULL || abClearFocus)
				mpFocusedWidget = NULL;
		}

		if(mpAttentionWidget && mpFocusedWidget == NULL)
		{
			//Log("Got focus %d\n",apWidget);
			mpFocusedWidget = apWidget;
			if(mpFocusedWidget) mpFocusedWidget->ProcessMessage(eGuiMessage_GotFocus, data);
		}

		if(pOldFocus && pOldFocus != mpFocusedWidget)
			pOldFocus->ProcessMessage(eGuiMessage_LostFocus, data);
	}

	//-----------------------------------------------------------------------

	void cGuiSet::PushAttentionWidget()
	{
		mlstAttentionStack.push_back(mpAttentionWidget);
	}

	void cGuiSet::PopAttentionWidget(bool abClearFocus)
	{
		if(mlstAttentionStack.empty()) return;

		SetAttentionWidget(mlstAttentionStack.back(), abClearFocus, true);

		mlstAttentionStack.pop_back();
	}

	void cGuiSet::SetRendersBeforePostEffects(bool abX)
	{
		mbRendersBeforePostEffects = abX;
	}

	//-----------------------------------------------------------------------

	void cGuiSet::SetIs3D(bool abX)
	{
		mbIs3D = abX;
	}

	void cGuiSet::Set3DSize(const cVector3f& avSize)
	{
		mv3DSize = avSize;
	}

	
	void cGuiSet::Set3DTransform(const cMatrixf& a_mtxTransform)
	{
		m_mtx3DTransform = a_mtxTransform;
	}

	//-----------------------------------------------------------------------

	void cGuiSet::SetCurrentPointer(cGuiGfxElement *apGfx)
	{
		mpGfxCurrentPointer = apGfx;
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::HasFocus()
	{
		return mpGui->GetFocusedSet() == this;
	}

	//-----------------------------------------------------------------------

	void cGuiSet::PositionWidgetInsideBounds(iWidget* apWidget)
	{
		cVector3f vPos = apWidget->GetGlobalPosition();
		const cVector2f& vSize = apWidget->GetSize();
		const cVector2f& vSetSize = GetVirtualSize();

		for(int i=0; i<2; ++i)
		{
			if(vPos.v[i] < 0)
				vPos.v[i]=0;
			if(vPos.v[i]+vSize.v[i] > vSetSize.v[i])
				vPos.v[i] = vSetSize.v[i]-vSize.v[i];
		}

		apWidget->SetGlobalPosition(vPos);
	}

	//-----------------------------------------------------------------------

	void cGuiSet::SetSkin(cGuiSkin* apSkin)
	{
		//if(mpSkin == apSkin) return; Remove til there is a real skin

		mpSkin = apSkin;
		
		if(mpSkin)
		{
			mpGfxCurrentPointer = mpSkin->GetGfx(eGuiSkinGfx_PointerNormal);
		}
		else
		{
			mpGfxCurrentPointer = NULL;
		}
		
	}

	void cGuiSet::IncPopUpZ()
	{
		mfLastPopUpZ += 5.0f; 
		if(mfLastPopUpZ>=500)
			mfLastPopUpZ = 20;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//--------------------------------------------------------------

	void cGuiSet::DrawTextFromCharArry(	const wchar_t* apString, iFontData *apFont,
										const cVector2f& avSize, const cVector3f& avPosition,
										const cColor& aColor, eGuiMaterial aMaterial,
										eFontAlign aAlign)
	{
		int lCount =0;
		cVector3f vPos = avPosition;

		//////////////////////////////////////////////////////
		// Change position depending on the alignment
		if(aAlign == eFontAlign_Center){
			vPos.x -= apFont->GetLength(avSize, apString)/2;
		}
		else if(aAlign == eFontAlign_Right)
		{
			vPos.x -= apFont->GetLength(avSize, apString);
		}

		//////////////////////////////////////////////////////
		// Iterate the characters in string until NULL is found
		while(apString[lCount] != 0)
		{
			wchar_t lGlyphNum = ((wchar_t)apString[lCount]);
			
			//Check if the glyph is valid (in range)
			if(	lGlyphNum < apFont->GetFirstChar() || 
				lGlyphNum > apFont->GetLastChar())
			{
				lCount++;
				continue;
			}
			//Get actual number of the glyph in the font.
			lGlyphNum -= apFont->GetFirstChar();
			
			//Get glyph data and draw.
			cGlyph *pGlyph = apFont->GetGlyph(lGlyphNum);
			if(pGlyph)
			{
				cVector2f vOffset(pGlyph->mvOffset * avSize);
				cVector2f vSize(pGlyph->mvSize * avSize);// *apFont->GetSizeRatio());

				DrawGfx(pGlyph->mpGuiGfx,vPos + vOffset,vSize,aColor,aMaterial);

				vPos.x += pGlyph->mfAdvance*avSize.x; 
			}
			lCount++;
		}
	}

	//--------------------------------------------------------------
	
#define kLogRender (false)
	
	static void SetClipArea(iLowLevelGraphics *pLowLevelGraphics, cGuiClipRegion *apRegion)
	{
		cRect2f& clipRect =apRegion->mRect;

		//////////////////////////////////
		// Set up clip area
		if(apRegion->mRect.w >0)
		{
			cPlanef plane;

			//Bottom
			plane.FromNormalPoint(cVector3f(0,-1,0),cVector3f(0,clipRect.y+clipRect.h,0));
			pLowLevelGraphics->SetClipPlane(0, plane);
			pLowLevelGraphics->SetClipPlaneActive(0, true);

			//Top
			plane.FromNormalPoint(cVector3f(0,1,0),cVector3f(0,clipRect.y,0));
			pLowLevelGraphics->SetClipPlane(1, plane);
			pLowLevelGraphics->SetClipPlaneActive(1, true);

			//Right
			plane.FromNormalPoint(cVector3f(1,0,0),cVector3f(clipRect.x,0,0));
			pLowLevelGraphics->SetClipPlane(2, plane);
			pLowLevelGraphics->SetClipPlaneActive(2, true);

			//Left
			plane.FromNormalPoint(cVector3f(-1,0,0),cVector3f(clipRect.x+clipRect.w,0,0));
			pLowLevelGraphics->SetClipPlane(3, plane);
			pLowLevelGraphics->SetClipPlaneActive(3, true);

			if(kLogRender) Log("-- Clip region: %d Clipping: x %f y %f w %f h %f\n",apRegion,
				apRegion->mRect.x,apRegion->mRect.y,
				apRegion->mRect.w,apRegion->mRect.h);
		}
		else
		{
			if(kLogRender)Log("-- Clip region: %d No clipping!\n",apRegion);
		}
	}

	//-----------------------------------------------------------------------

	void cGuiSet::RenderClipRegion()
	{
		iLowLevelGraphics *pLowLevelGraphics = mpGraphics->GetLowLevel();

		if(kLogRender)Log("-------------------\n");

		///////////////////////////////////////
		//See if there is anything to draw
		tGuiRenderObjectSet &setRenderObjects = m_setRenderObjects;
		if(setRenderObjects.empty())
		{
			if(kLogRender) Log("------------------------\n");
			return;
		}
		
		//////////////////////////////////
		// Graphics setup
		pLowLevelGraphics->SetTexture(0,NULL);

		//////////////////////////////////
		// Set up variables
		
		tGuiRenderObjectSetIt it = setRenderObjects.begin();
		
		iGuiMaterial *pLastMaterial = NULL;
		iTexture *pLastTexture = NULL;
		cGuiClipRegion *pLastClipRegion = NULL;

		cGuiGfxElement *pGfx = it->mpGfx;
		iGuiMaterial *pMaterial = it->mpCustomMaterial ? it->mpCustomMaterial : pGfx->mpMaterial;
		iTexture *pTexture = pGfx->mvTextures[0];
		cGuiClipRegion *pClipRegion = it->mpClipRegion;

		int lIdxAdd=0;

		//Log("bug:Rendering objects!\n");

		///////////////////////////////////
		// Iterate objects
		while(it != setRenderObjects.end())
		{
			///////////////////////////////
			//Start rendering
			if(pLastMaterial != pMaterial){
				pMaterial->BeforeRender();
				if(kLogRender)Log("Material %s before\n",pMaterial->GetName().c_str());
			}

			////////////////////////////
			// SetClip area
			if(pLastClipRegion != pClipRegion)
			{
				SetClipArea(pLowLevelGraphics,pClipRegion);
			}
			
			pLowLevelGraphics->SetTexture(0,pTexture);
			if(kLogRender)Log("Texture %d\n",pTexture);

			//////////////////////////
			//Iterate for all with same texture and material
			do 
			{
				const cGuiRenderObject &object = *it;
				cGuiGfxElement *pGfx = object.mpGfx;

				//Log("bug: gfx: %p\n",pGfx);

				if(kLogRender)
				{
					if(pGfx->mvImages[0])
						Log(" gfx: %d '%s'\n",pGfx,pGfx->mvImages[0]->GetName().c_str());
					else
						Log(" gfx: %d 'null'\n");
				}

				//DEBUG!
				/*if(pGfx->GetImage(0) && pGfx->GetImage(0)->GetName()=="_temp_hand.tga")
				{
					Log("Drawing: %d (%s):(%s) %d (%s)\n", object.mpGfx, object.mvPos.ToString().c_str(), object.mvSize.ToString().c_str(),
															object.mpClipRegion, object.mColor.ToString().c_str());
				}*/

				///////////////////////////
				// Add object to batch
				if(object.mbRotated)
				{
					for(int i=0; i<4; ++i)
					{
						cVertex &vtx = pGfx->mvVtx[i];
						cVector3f vVtxPos = vtx.pos;
						const cVector3f& vPos = object.mvPos;

						//Scale
						vVtxPos.x *= object.mvSize.x;
						vVtxPos.y *= object.mvSize.y;

						//Rotate
						vVtxPos.x -= object.mvPivot.x;
						vVtxPos.y -= object.mvPivot.y;
						vVtxPos = cMath::MatrixMul(cMath::MatrixRotateZ(object.mfAngle), vVtxPos); 
						vVtxPos.x += object.mvPivot.x;
						vVtxPos.y += object.mvPivot.y;
						
						pLowLevelGraphics->AddVertexToBatch_Raw(
							cVector3f(	vVtxPos.x + vPos.x,
										vVtxPos.y + vPos.y,
										vPos.z),
							vtx.col * object.mColor,
							vtx.tex);
					}
				}
				else
				{
					for(int i=0; i<4; ++i)
					{
						cVertex &vtx = pGfx->mvVtx[i];
						cVector3f& vVtxPos = vtx.pos;
						const cVector3f& vPos = object.mvPos;
						pLowLevelGraphics->AddVertexToBatch_Raw(
							cVector3f(	vVtxPos.x * object.mvSize.x + vPos.x,
										vVtxPos.y * object.mvSize.y + vPos.y,
										vPos.z),
							vtx.col * object.mColor,
							vtx.tex);
					}
				}

				for(int i=0;i<4;i++)
					pLowLevelGraphics->AddIndexToBatch(lIdxAdd + i);

				lIdxAdd += 4;

				///////////////////////////
				//Set last texture
				pLastMaterial =  pMaterial;
				pLastTexture =   pTexture;
				pLastClipRegion = pClipRegion;

				/////////////////////////////
				//Get next object
				++it; if(it == setRenderObjects.end()) break;

				pGfx = it->mpGfx;
				pMaterial = it->mpCustomMaterial ? it->mpCustomMaterial : pGfx->mpMaterial;
				pTexture = it->mpGfx->mvTextures[0];
				pClipRegion = it->mpClipRegion;
			} 
			while(	pTexture == pLastTexture &&
					pMaterial == pLastMaterial &&
					pClipRegion == pLastClipRegion);

			//////////////////////////////
			// Render batch
			pLowLevelGraphics->FlushQuadBatch(	eVtxBatchFlag_Position | eVtxBatchFlag_Texture0 | 
												eVtxBatchFlag_Color0,false);
			pLowLevelGraphics->ClearBatch();
			lIdxAdd=0;

			/////////////////////////////////
			//Clip region end
			if(pLastClipRegion  != pClipRegion  || it == setRenderObjects.end())
			{
				if(pLastClipRegion->mRect.w >0)
				{
					for(int i=0; i<4; ++i) pLowLevelGraphics->SetClipPlaneActive(i, false);
				}
			}
			
			/////////////////////////////////
			//Material end
			if(pLastMaterial != pMaterial || it == setRenderObjects.end())
			{
				pLastMaterial->AfterRender();
				if(kLogRender)Log("Material %d '%s' after. new: %d '%s'\n",	pLastMaterial,pLastMaterial->GetName().c_str(),
																		pMaterial,pMaterial->GetName().c_str());
			}
		}
		
		if(kLogRender)Log("---------- END %d -----------\n");
	}
	//-----------------------------------------------------------------------

	void cGuiSet::AddWidget(iWidget *apWidget,iWidget *apParent)
	{
		mlstWidgets.push_front(apWidget);

		if(apParent)
			apParent->AttachChild(apWidget);
		else
			mpWidgetRoot->AttachChild(apWidget);

		apWidget->Init();
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::OnMouseMove(const cGuiMessageData &aData)
	{
		if(GetMouseMovementEnabled()==false)
			return false;

		///////////////////////////
		//Set up variables
		mvMousePos = aData.mvPos;
		
		iWidget* pOldToolTipWidget = mpCurrentToolTipWidget;
		iWidget* pNewToolTipWidget = NULL;
	
		cGuiMessageData tData = aData;
		tData.mlVal = 0;
		if(mvMouseDown[0]) tData.mlVal |= eGuiMouseButton_Left;
		if(mvMouseDown[1]) tData.mlVal |= eGuiMouseButton_Middle;
		if(mvMouseDown[2]) tData.mlVal |= eGuiMouseButton_Right;
		if(mvMouseDown[3]) tData.mlVal |= eGuiMouseButton_WheelUp;
		if(mvMouseDown[4]) tData.mlVal |= eGuiMouseButton_WheelDown;

		///////////////////////////
		//Call widgets
		bool bRet = false;
		bool bPointerSet = false;
		bool bToolTipWidgetSet = false;
		bool bToolTipWidgetLeft = false;
		iWidget* pWidgetUnderMouse=NULL;

		///////////////////////////
		// Widgets should be sorted by z value, so the first check must be the widget that is right under the mouse
		tWidgetListIt it = mlstWidgets.begin();
		for(; it != mlstWidgets.end(); ++it)
		{
			iWidget *pWidget = *it;
			if(pWidget->IsVisible()==false)
				continue;	
			
			if(mpAttentionWidget && pWidget->IsConnectedTo(mpAttentionWidget)==false)
				continue;

			if(pWidget->PointIsInside(mvMousePos,false))
			{
				if(pWidgetUnderMouse==NULL || 
					pWidgetUnderMouse->IsConnectedTo(pWidget) || 
					pWidget->IsConnectedTo(pWidgetUnderMouse))
				{
					if(pWidgetUnderMouse==NULL)
						pWidgetUnderMouse=pWidget;

					////////////////////////////
					//Mouse enter event
					if(pWidget->GetMouseIsOver()==false)
					{
						pWidget->SetMouseIsOver(true);
						if(pWidget->ProcessMessage(eGuiMessage_MouseEnter, tData))
						{
							bRet = true;
						}

						////////////////////////////
						//Set up tooltips on mouse enter		
						if(pWidget->IsToolTipEnabled())
						{
							if(bToolTipWidgetSet==false)
							{
								bToolTipWidgetSet = true;
								pNewToolTipWidget = pWidget;

								if(pNewToolTipWidget!=pOldToolTipWidget)
									mfToolTipTimer = 0;
							}
						}

						if(pWidget->HasFocusNavigation() && pWidget->IsVisible() && pWidget->IsEnabled())
						{
							iWidget *pOldFocus = mpFocusedWidget;

							if(pOldFocus==NULL || (pOldFocus->GetParent() == pWidget->GetParent()
													&& pOldFocus->GetSet() == pWidget->GetSet()))
							{
								SetFocusedWidget(pWidget);
							}
						}
					}

					////////////////////////////
					//Set pointer
					if(bPointerSet==false && pWidget->GetPointerGfx())
					{
						if(	mpAttentionWidget &&
							pWidget->IsConnectedTo(mpAttentionWidget)==false)
						{
						}
						else
						{
							if(pWidget->IsEnabled())
							{
								if(mpGfxCurrentPointer != pWidget->GetPointerGfx())
									SetCurrentPointer(pWidget->GetPointerGfx());							
							}
							else
							{
								SetCurrentPointer(mpSkin->GetGfx(eGuiSkinGfx_PointerNormal));
							}
							bPointerSet = true;
						}
					}
				}
			}
			else
			{
				////////////////////////////
				//Mouse leave event
				if(pWidget->GetMouseIsOver())
				{
					pWidget->SetMouseIsOver(false);
					pWidget->ProcessMessage(eGuiMessage_MouseLeave, tData);

					//In case the widget is moved under the mouse again, check:
					if(mpFocusedWidget == pWidget && pWidget->PointIsInside(mvMousePos, false))
					{
						pWidget->SetMouseIsOver(true);
						if(pWidget->ProcessMessage(eGuiMessage_MouseEnter, tData)) bRet = true;
					}
					else
					{
						if(pOldToolTipWidget==pWidget)
							bToolTipWidgetLeft = true;
					}
				}
			}

			////////////////////////////
			//Mouse move event
			if(pWidget->GetMouseIsOver() || mpFocusedWidget == pWidget)
			{
				if(pWidget->ProcessMessage(eGuiMessage_MouseMove, tData)) bRet = true;
			}
		}

		if(bToolTipWidgetSet)
		{
			mpCurrentToolTipWidget = pNewToolTipWidget;
		}
		else if(bToolTipWidgetLeft)
		{
			mpCurrentToolTipWidget=NULL;
		}
		
		return bRet;
	}
	
	//-----------------------------------------------------------------------

	bool cGuiSet::OnMouseDown(const cGuiMessageData& aData)
	{
		///////////////////////////
		//Set up variables
		mvMouseDown[cMath::Log2ToInt(aData.mlVal)] = true;

		cGuiMessageData tData = aData;
		tData.mvPos = mvMousePos;

		iWidget *pOldFocus = mpFocusedWidget;

		///////////////////////////
		//Call widgets
		bool bRet = false;

		tWidgetListIt it = mlstWidgets.begin();
		for(; it != mlstWidgets.end(); ++it)
		{
			iWidget *pWidget = *it;

			//If widget is not visible, skip it
			if(pWidget->IsVisible()==false)
				continue;

			//If there is an attention set, do not send clicks to any other widgets
			if(mpAttentionWidget && pWidget->IsConnectedTo(mpAttentionWidget)==false) 
			{
				continue;
			}

			if(pWidget->GetMouseIsOver())
			{
				if(mpFocusedWidget != pWidget)
				{
					if(pWidget->GetFocus(tData))
					{
						mpFocusedWidget = pWidget;
					}
				}
				//else
				//{
				//	mpFocusedWidget = pWidget;
				//}

				//Log("Got focus %d\n",pWidget);

				if(pWidget->ProcessMessage(eGuiMessage_MouseDown, tData))
				{
					bRet = true;
					break;
				}
			}
		}

		//Se if anything was clicked
		if(bRet == false)
			mpFocusedWidget = NULL;
		
		//Lost focus callback
		if(mpFocusedWidget != pOldFocus)
		{
			//Log("Lost focus %d\n",pOldFocus);
			if(pOldFocus && IsValidWidget(pOldFocus)) pOldFocus->ProcessMessage(eGuiMessage_LostFocus, tData, true, true);			
		}

		return bRet;
	}
	
	//-----------------------------------------------------------------------
	
	bool cGuiSet::OnMouseUp(const cGuiMessageData& aData)
	{
		///////////////////////////
		//Set up variables
		mvMouseDown[cMath::Log2ToInt(aData.mlVal)] = false;

		cGuiMessageData tData = aData;
		tData.mvPos = mvMousePos;

		//mlstWidgets.sort(SortWidget_Z);
		
		///////////////////////////
		//Call widgets
		bool bRet = false;
		
		if(mpFocusedWidget)
		{
			bRet = mpFocusedWidget->ProcessMessage(eGuiMessage_MouseUp, tData);
		}

		if(bRet == false)
		{
			tWidgetListIt it = mlstWidgets.begin();
			for(; it != mlstWidgets.end(); ++it)
			{
				iWidget *pWidget = *it;

				//If these is an attention set, do send clicks to any other widgets
				if(mpAttentionWidget && pWidget->IsConnectedTo(mpAttentionWidget)==false) 
				{
					continue;
				}

				if(pWidget != mpFocusedWidget && pWidget->GetMouseIsOver())
				{
					if(pWidget->ProcessMessage(eGuiMessage_MouseUp, tData))
					{
						bRet = true;
						break;
					}
				}
			}
		}

		return bRet;
	}
	
	//-----------------------------------------------------------------------

	bool cGuiSet::OnMouseDoubleClick(const cGuiMessageData& aData)
	{
		///////////////////////////
		//Set up variables
		cGuiMessageData tData = aData;
		tData.mvPos = mvMousePos;
		
		///////////////////////////
		//Call widgets
        bool bRet = false;
		tWidgetListIt it = mlstWidgets.begin();
		for(; it != mlstWidgets.end(); ++it)
		{
			iWidget *pWidget = *it;

			//If these is an attention set, do send clicks to any other widgets
			if(mpAttentionWidget && pWidget->IsConnectedTo(mpAttentionWidget)==false) 
			{
				continue;
			}

			if(pWidget->GetMouseIsOver())
			{
				if(pWidget->ProcessMessage(eGuiMessage_MouseDoubleClick, tData))
				{
					bRet = true;
					break;
				}
			}
		}

		return bRet;
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::OnKeyPress(const cGuiMessageData& aData)
	{
		///////////////////////////
		//Set up variables
		cGuiMessageData tData = aData;
		tData.mvPos = mvMousePos;

		///////////////////////////
		//Call widgets
		bool bRet = false;

		///////////////////////////
		// Check tab order (temporary - this should be handled by iWidget, for widgets capable of containing other widgets)
		if(bRet==false && tData.mKeyPress.mKey==eKey_Tab && mlstTabOrderWidgets.empty()==false)
		{
			iWidget* pOldWidgetToFocus = mpTabOrderWidget;
			iWidget* pNewWidgetToFocus=NULL;

			tWidgetListIt it = find(mlstTabOrderWidgets.begin(), mlstTabOrderWidgets.end(), mpFocusedWidget);
			if(it==mlstTabOrderWidgets.end())
				it = find(mlstTabOrderWidgets.begin(), mlstTabOrderWidgets.end(), pOldWidgetToFocus);

			if(it!=mlstTabOrderWidgets.end())
			{
				do
				{
					if(tData.mKeyPress.mlModifier==eKeyModifier_Shift) 
					{
						if(it==mlstTabOrderWidgets.begin())
							it=mlstTabOrderWidgets.end();
						--it;
					}
					else if(tData.mKeyPress.mlModifier==eKeyModifier_None)
					{
						++it;
						if(it==mlstTabOrderWidgets.end())
							it=mlstTabOrderWidgets.begin();
					}

					pNewWidgetToFocus = *it;
				}
				while(pNewWidgetToFocus->IsVisible()==false && pNewWidgetToFocus->IsEnabled()==false);
			}
			
			if(pNewWidgetToFocus && pNewWidgetToFocus->IsVisible() && pNewWidgetToFocus->IsEnabled())
			{
				mpTabOrderWidget = pNewWidgetToFocus;
				SetFocusedWidget(mpTabOrderWidget);
				mpTabOrderWidget->OnGotTabFocus(tData);
				bRet = true;
			}
		}
		

		if(bRet==false)
		{
			if(mpFocusedWidget) 
				bRet = mpFocusedWidget->ProcessMessage(eGuiMessage_KeyPress, tData);

			if(bRet==false)
			{
				eKey key = tData.mKeyPress.mKey;
				eUIArrow dir = TranslateKeyToUIArrow(key);

				if(dir!=eUIArrow_LastEnum)
					bRet = SendMessage(eGuiMessage_UIArrowPress, dir);
			}
		}

		

		if(bRet==false)
		{
			// Steal focus from current widget
			iWidget* pFocusedWidget = mpFocusedWidget;
			SetFocusedWidget(NULL);

			bRet = ExecShortcut(tData.mKeyPress);

			// Restore focus if shortcut did not set own
			if(mpFocusedWidget==NULL)
                SetFocusedWidget(pFocusedWidget);
		}
		
		if(bRet==false)
		{
			tWidgetListIt it = mlstWidgets.begin();
			for(; it != mlstWidgets.end(); ++it)
			{
				iWidget *pWidget = *it;

				//If these is an attention set, do send clicks to any other widgets
				if(mpAttentionWidget && pWidget->IsConnectedTo(mpAttentionWidget)==false) 
				{
					continue;
				}

				if(pWidget->GetMouseIsOver() || pWidget->IsGlobalKeyPressListener() && mpFocusedWidget != pWidget)
				{
					if(pWidget->ProcessMessage(eGuiMessage_KeyPress, tData))
					{
						bRet = true;
						break;
					}
				}
			}
		}

		return bRet;
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::OnKeyRelease(const cGuiMessageData& aData)
	{
		///////////////////////////
		//Set up variables
		cGuiMessageData tData = aData;
		tData.mvPos = mvMousePos;

		///////////////////////////
		//Call widgets
		bool bRet = false;

		if(mpFocusedWidget)
		{
			bRet = mpFocusedWidget->ProcessMessage(eGuiMessage_KeyRelease, tData);
			if(bRet==false)
			{
				eKey key = tData.mKeyPress.mKey;
				eUIArrow dir = TranslateKeyToUIArrow(key);

				if(dir!=eUIArrow_LastEnum)
					bRet = SendMessage(eGuiMessage_UIArrowRelease, dir);
			}
		}
		
		if(bRet==false)
		{
			tWidgetListIt it = mlstWidgets.begin();
			for(; it != mlstWidgets.end(); ++it)
			{
				iWidget *pWidget = *it;

				//If these is an attention set, do send clicks to any other widgets
				if(mpAttentionWidget && pWidget->IsConnectedTo(mpAttentionWidget)==false) 
				{
					continue;
				}

				if(pWidget->GetMouseIsOver() && mpFocusedWidget != pWidget)
				{
					if(pWidget->ProcessMessage(eGuiMessage_KeyRelease, tData))
					{
						bRet = true;
						break;
					}
				}
			}
		}

		return bRet;
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::OnGamepadInput(const cGuiMessageData& aData)
	{
		///////////////////////////
		//Set up variables
		cGuiMessageData tData = aData;
		
		///////////////////////////
		//Call widgets
		bool bRet = false;

		if(mpFocusedWidget)
		{
			bRet = mpFocusedWidget->ProcessMessage(eGuiMessage_GamepadInput, tData);
		}
		
		if(bRet==false)
		{
			tWidgetListIt it = mlstWidgets.begin();
			for(; it != mlstWidgets.end(); ++it)
			{
				iWidget *pWidget = *it;

				//If these is an attention set, do send clicks to any other widgets
				if(mpAttentionWidget && pWidget->IsConnectedTo(mpAttentionWidget)==false) 
				{
					continue;
				}

				if(pWidget->GetMouseIsOver() && mpFocusedWidget != pWidget)
				{
					if(pWidget->ProcessMessage(eGuiMessage_GamepadInput, tData))
					{
						bRet = true;
						break;
					}
				}
			}
		}

		return bRet;
	}
		
	//-----------------------------------------------------------------------

	bool cGuiSet::OnUIArrowPress(const cGuiMessageData& aData)
	{
		///////////////////////////////////////////////////////////////////////////////////////////////
		// First check if we have a proper start point, if we are not and cannot set one, just skip
		if(mpFocusedWidget==NULL || 
			mpFocusedWidget->HasFocusNavigation()==false && mpFocusedWidget!=mpDefaultFocusNavWidget)
		{
			if(mpDefaultFocusNavWidget==NULL) 
			{
				return false;
			}
			else
			{
				if(mpAttentionWidget==NULL || mpDefaultFocusNavWidget->IsConnectedTo(mpAttentionWidget))
				{
					SetFocusedWidget(mpDefaultFocusNavWidget);
					mpFocusedWidget->ProcessMessage(eGuiMessage_GetUINavFocus, cGuiMessageData());

					return true;
				}
				return false;
			}
		}

		bool bRet = false;

		///////////////////////////////////////////////////////////////////////////////////////////////
		// Check if widget likes the arrow press for anything else other than moving away,
		// if not the case, just get next widget and check for validity
		if(mpFocusedWidget) bRet = mpFocusedWidget->ProcessMessage(eGuiMessage_UIArrowPress, aData);

		if(bRet==false)
		{
			iWidget* pNextWidget = mpFocusedWidget->GetFocusNavigation((eUIArrow)aData.mlVal);

			if(pNextWidget && 
				pNextWidget->IsEnabled() && 
				pNextWidget->IsVisible() &&
				(mpAttentionWidget==NULL || pNextWidget->IsConnectedTo(mpAttentionWidget)))
			{
				iWidget *pOldFocus = mpFocusedWidget;
				SetFocusedWidget(pNextWidget);

				// Do whatever stuff the widget wants to do when getting the focus through arrows
				mpFocusedWidget->ProcessMessage(eGuiMessage_GetUINavFocus, cGuiMessageData());
				pOldFocus->ProcessMessage(eGuiMessage_LoseUINavFocus, cGuiMessageData());
				bRet = true;
			}
		}

		///////////////////////////////////////////////////////////////////////////
		// If we still haven't consumed the input, check for global listeners
		if(bRet==false)
		{
			tWidgetListIt it = mlstWidgets.begin();
			for(; it != mlstWidgets.end(); ++it)
			{
				iWidget *pWidget = *it;

				// If widget is not eligible for input check, skip it (or if it's the focused one, we already checked that)
				if(pWidget->IsGlobalUIInputListener()==false || pWidget==mpFocusedWidget || pWidget->IsVisible()==false)
					continue;

				//If there is an attention set, do not send clicks to any other widgets
				if(mpAttentionWidget && pWidget->IsConnectedTo(mpAttentionWidget)==false) 
				{
					continue;
				}

				bRet = pWidget->ProcessMessage(eGuiMessage_UIArrowPress, aData);
				if(bRet)
					break;
			}
		}


		return bRet;
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::OnUIArrowRelease(const cGuiMessageData& aData)
	{
		if(mpFocusedWidget==NULL)
		{
			return false;
		}

		bool bRet = mpFocusedWidget->ProcessMessage(eGuiMessage_UIArrowRelease, aData);

		return bRet;
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::OnUIButtonPress(const cGuiMessageData& aData)
	{
		///////////////////////////
		//Call widgets
		bool bRet = false;
		if(mpFocusedWidget)
			bRet = mpFocusedWidget->ProcessMessage(eGuiMessage_UIButtonPress, aData);

		if(bRet==false)
		{
			tWidgetListIt it = mlstWidgets.begin();
			for(; it != mlstWidgets.end(); ++it)
			{
				iWidget *pWidget = *it;

				// If widget is not eligible for input check, skip it (or if it's the focused one, we already checked that)
				if(pWidget->IsGlobalUIInputListener()==false || pWidget==mpFocusedWidget || pWidget->IsVisible()==false)
					continue;

				//If there is an attention set, do not send clicks to any other widgets
				if(mpAttentionWidget && pWidget->IsConnectedTo(mpAttentionWidget)==false) 
				{
					continue;
				}

				bRet = pWidget->ProcessMessage(eGuiMessage_UIButtonPress, aData);
				if(bRet)
					break;
			}
		}

		return bRet;
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::OnUIButtonRelease(const cGuiMessageData& aData)
	{
		///////////////////////////
		//Call widgets
		bool bRet = false;
		if(mpFocusedWidget)
			bRet = mpFocusedWidget->ProcessMessage(eGuiMessage_UIButtonRelease, aData);

		if(bRet==false)
		{
			tWidgetListIt it = mlstWidgets.begin();
			for(; it != mlstWidgets.end(); ++it)
			{
				iWidget *pWidget = *it;

				// If widget is not eligible for input check, skip it (or if it's the focused one, we already checked that)
				if(pWidget->IsGlobalUIInputListener()==false || pWidget==mpFocusedWidget || pWidget->IsVisible()==false)
					continue;

				//If there is an attention set, do not send clicks to any other widgets
				if(mpAttentionWidget && pWidget->IsConnectedTo(mpAttentionWidget)==false) 
				{
					continue;
				}

				bRet = pWidget->ProcessMessage(eGuiMessage_UIButtonRelease, aData);
				if(bRet)
					break;
			}
		}

		return bRet;
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::OnUIButtonDoublePress(const cGuiMessageData& aData)
	{
		if(mpFocusedWidget==NULL) return false;

		return mpFocusedWidget->ProcessMessage(eGuiMessage_UIButtonDoublePress, aData);
	}

	//-----------------------------------------------------------------------

	bool cGuiSet::DrawMouse(iWidget* apWidget, const cGuiMessageData& aData)
	{	
		if(HasFocus() && mbDrawMouse && mpGfxCurrentPointer)
		{
			DrawGfx(mpGfxCurrentPointer,cVector3f(mvMousePos.x,mvMousePos.y, mfMouseZ),
				mpGfxCurrentPointer->GetImageSize(),cColor(1,1));
		}
		
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiSet,DrawMouse)

	//-----------------------------------------------------------------------

	bool cGuiSet::DrawFocus(iWidget* apWidget, const cGuiMessageData& aData)
	{	
		if(HasFocus() && mpFocusedWidget && mbDrawFocus && 
			(mpFocusedWidget==mpDefaultFocusNavWidget || mpFocusedWidget->HasFocusNavigation()))
		{
			if(mpFocusDrawObject && mpFocusDrawCallback)
				mpFocusDrawCallback(mpFocusDrawObject, mpFocusedWidget, aData);
			else
			{
				cVector3f vPos = mpFocusedWidget->GetGlobalPosition();
				vPos.z = mfMouseZ-1.0f;
				DrawGfx(cGui::mpGfxRect, vPos, mpFocusedWidget->GetSize(), cColor(0.8f,0.2f));
			}
		}
		
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiSet,DrawFocus)

	//-----------------------------------------------------------------------

	bool cGuiSet::ExecShortcut(const cKeyPress& aKeyPress)
	{
		bool bRet = false;
		tShortcutListIt it = mlstShortcuts.begin();
		for(;it!=mlstShortcuts.end();++it)
		{
			cGuiGlobalShortcut* pShortcut = *it;
			bRet = pShortcut->ProcessKeyPress(aKeyPress);
			if(bRet)
				break;
		}

		return bRet;
	}

	//-----------------------------------------------------------------------

	void cGuiSet::UpdateToolTip(float afTimeStep)
	{
		if(mpSkin==NULL || mpFrameToolTip==NULL) return;

		if(mpCurrentToolTipWidget==NULL || mpCurrentToolTipWidget->IsVisible()==false)
		{
			if(mpFrameToolTip->IsVisible())
			{
				mpFrameToolTip->SetEnabled(false);
				mpFrameToolTip->SetVisible(false);
			}
		}
		else
		{
			if(mfToolTipTimer>=mfToolTipTimeToPopUp)
			{
				if(mpFrameToolTip->IsVisible()==false)
				{
					const tWString& sTipText = mpCurrentToolTipWidget->GetToolTip().c_str();
					const cVector2f& mvFontSize = mpLabelToolTip->GetDefaultFontSize();

					float fTextLength = mpLabelToolTip->GetDefaultFontType()->GetLength(mvFontSize, 
																						sTipText.c_str()) + 3 + 3;
					float fMaxTextLength = GetVirtualSize().x*0.4f;
					iFontData* pFont = mpLabelToolTip->GetDefaultFontType();

					tWStringVec vRows;
					pFont->GetWordWrapRows(fMaxTextLength, mvFontSize.y+2, mvFontSize, sTipText, &vRows);
					int lRows = (int)vRows.size();

					cVector3f vPos = mvMousePos + mpGfxCurrentPointer->GetImageSize();
					vPos.z = mfMouseZ - 2;

					cVector2f vToolTipSize = cVector2f( cMath::Min(fTextLength, fMaxTextLength), 
														lRows*(mvFontSize.y+2) -2 +3 +3);

					mpLabelToolTip->SetText(sTipText);
					mpLabelToolTip->SetSize(vToolTipSize);

					if(vPos.x + vToolTipSize.x > mvVirtualSize.x)
						vPos.x = mvVirtualSize.x-vToolTipSize.x;
					if(vPos.y + vToolTipSize.y > mvVirtualSize.y)
						vPos.y = mvVirtualSize.y-vToolTipSize.y;

					mpFrameToolTip->SetGlobalPosition(vPos);
					mpFrameToolTip->SetSize(vToolTipSize);

					mpFrameBGToolTip->SetSize(vToolTipSize-2);

					mpFrameToolTip->SetEnabled(true);
					mpFrameToolTip->SetVisible(true);
				}
			}
			else
			{
				mfToolTipTimer+=afTimeStep;

				if(mpFrameToolTip->IsVisible())
				{
					mpFrameToolTip->SetEnabled(false);
					mpFrameToolTip->SetVisible(false);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	void cGuiSet::CreateToolTipWidgets()
	{
		mpFrameToolTip = CreateWidgetFrame(0,0,false);
		mpFrameToolTip->SetBackGroundColor(cColor(0,1));
		mpFrameToolTip->SetDrawBackground(true);
		mpFrameToolTip->SetEnabled(false);
		mpFrameToolTip->SetVisible(false);

		mpFrameBGToolTip = CreateWidgetFrame(cVector3f(1,1,0.1f),0,false,mpFrameToolTip);
		mpFrameBGToolTip->SetBackGroundColor(cColor(1,1,0.882f,1));
		mpFrameBGToolTip->SetDrawBackground(true);

		mpLabelToolTip = CreateWidgetLabel(cVector3f(3,2,0.1f),0,_W(""), mpFrameToolTip);
		mpLabelToolTip->SetDefaultFontSize(12);
		mpLabelToolTip->SetWordWrap(true);
	}

	//-----------------------------------------------------------------------

	eUIArrow cGuiSet::TranslateKeyToUIArrow(eKey aKey)
	{
		switch(aKey)
		{
		case eKey_Up:			return eUIArrow_Up;					
		case eKey_Right:		return eUIArrow_Right;	
		case eKey_Down:			return eUIArrow_Down;
		case eKey_Left:			return eUIArrow_Left;		
		default:				return eUIArrow_LastEnum;
		}
	}

	//-----------------------------------------------------------------------

	void cGuiSet::AddToTabOrder(iWidget* apWidget)
	{
		if(apWidget && find(mlstTabOrderWidgets.begin(), mlstTabOrderWidgets.end(), apWidget)==mlstTabOrderWidgets.end())
			mlstTabOrderWidgets.push_back(apWidget);

		mpTabOrderWidget = mlstTabOrderWidgets.front();
	}

	//-----------------------------------------------------------------------

	void cGuiSet::ClearTabOrder()
	{
		mlstTabOrderWidgets.clear();

		mpTabOrderWidget = NULL;
	}

	//-----------------------------------------------------------------------

	void cGuiSet::SetDefaultFocusNavWidget(iWidget* apWidget, bool abCheckForValidity)
	{
		if(mpDefaultFocusNavWidget==apWidget) return;

		if(abCheckForValidity && IsValidWidget(apWidget)==false)
			apWidget = NULL;

		mpDefaultFocusNavWidget = apWidget;
	}

	//-----------------------------------------------------------------------

	void cGuiSet::PushDefaultFocusNavWidget()
	{
		mlstDefaultFocusStack.push_back(mpDefaultFocusNavWidget);
	}

	void cGuiSet::PopDefaultFocusNavWidget()
	{
		if(mlstDefaultFocusStack.empty()) return;

		SetDefaultFocusNavWidget(mlstDefaultFocusStack.back(), true);

		mlstDefaultFocusStack.pop_back();
	}

	//-----------------------------------------------------------------------

}
