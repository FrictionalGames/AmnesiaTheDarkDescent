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

#include "gui/Gui.h"

#include "graphics/Graphics.h"
#include "graphics/LowLevelGraphics.h"

#include "sound/Sound.h"

#include "gui/GuiSet.h"
#include "gui/GuiSkin.h"
#include "gui/GuiGfxElement.h"

#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/ImageManager.h"
#include "graphics/FrameSubImage.h"
#include "graphics/FrameBitmap.h"
#include "resources/FileSearcher.h"

#include "system/String.h"

#include "input/Keyboard.h"


#include "gui/GuiMaterialBasicTypes.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	cGuiGfxElement* cGui::mpGfxRect = NULL;

	//-----------------------------------------------------------------------

	cGui::cGui()  : iUpdateable("HPL_Gui")
	{
		mpSetInFocus = NULL;

		mlLastRenderTime = 0;
	}

	//-----------------------------------------------------------------------

	cGui::~cGui()
	{
		Log("Exiting Gui Module\n");
		Log("--------------------------------------------------------\n");
		
		Log(" Deleting all sets\n");
		STLMapDeleteAll(m_mapSets);
		Log(" Deleting all skins\n");
		STLMapDeleteAll(m_mapSkins);
		
		Log(" Deleting all gfx elements\n");
		//STLDeleteAll(mlstGfxElements);
		for(tGuiGfxElementListIt it = mlstGfxElements.begin(); it != mlstGfxElements.end(); ++it)
		{
			cGuiGfxElement *pGfx = *it;
			//Log("Removing %d image: %d\n", pGfx, pGfx->GetImage(0));
			hplDelete(pGfx);
		}
		STLDeleteAll(mlstToBeDestroyedGfxElements);

		Log(" Deleting all materials\n");
		for(int i=0; i< eGuiMaterial_LastEnum; ++i) 
		{
			if(mvMaterials[i]) hplDelete(mvMaterials[i]);
		}
		
		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGui::Init(cResources *apResources, cGraphics* apGraphics,
						cSound *apSound, cScene *apScene, cInput* apInput)
	{
		mpResources = apResources;
		mpGraphics = apGraphics;
		mpSound = apSound;
		mpScene = apScene;
		mpInput = apInput;

		//////////////////////////////
		// Create materials
		for(int i=0; i< eGuiMaterial_LastEnum; ++i) mvMaterials[i] = NULL;

		mvMaterials[eGuiMaterial_Diffuse] = hplNew( cGuiMaterial_Diffuse,(mpGraphics->GetLowLevel()) );
		mvMaterials[eGuiMaterial_Alpha] = hplNew( cGuiMaterial_Alpha,(mpGraphics->GetLowLevel()) );
		mvMaterials[eGuiMaterial_FontNormal] = hplNew( cGuiMaterial_FontNormal,(mpGraphics->GetLowLevel()) );
		mvMaterials[eGuiMaterial_Additive] = hplNew( cGuiMaterial_Additive,(mpGraphics->GetLowLevel()) );
		mvMaterials[eGuiMaterial_Modulative] = hplNew( cGuiMaterial_Modulative,(mpGraphics->GetLowLevel()) );
		mvMaterials[eGuiMaterial_PremulAlpha] = hplNew( cGuiMaterial_PremulAlpha,(mpGraphics->GetLowLevel()) );


		//////////////////////////////
		// Skin setup
		GenerateSkinTypeStrings();


		//////////////////////////////
		// Set up global gfx
		if(mpGfxRect==NULL)
			mpGfxRect = CreateGfxFilledRect(cColor(1,1), eGuiMaterial_Alpha);
	}

	//-----------------------------------------------------------------------

	void cGui::Update(float afTimeStep)
	{
		////////////////////////////////////
		// Update Sets
		tGuiSetMapIt it = m_mapSets.begin();
		for(; it != m_mapSets.end(); ++it)
		{
			cGuiSet *pSet = it->second;
			if(pSet->IsActive()) pSet->Update(afTimeStep);
		}

		/////////////////////////////
		// Update gfx elements
		tGuiGfxElementListIt gfxIt = mlstGfxElements.begin();
		for(; gfxIt != mlstGfxElements.end(); ++gfxIt)
		{
			cGuiGfxElement *pGfx = *gfxIt;
			pGfx->Update(afTimeStep);
		}
	}

	//-----------------------------------------------------------------------

	void cGui::OnDraw(float afFrameTime)
	{
		tGuiSetMapIt setIt = m_mapSets.begin();
		for(; setIt != m_mapSets.end(); ++setIt)
		{
			cGuiSet *pSet = setIt->second;
			pSet->DrawAll(afFrameTime);
		}
	}

	//-----------------------------------------------------------------------

	void cGui::OnPostBufferSwap()
	{
		//Log("Bug:Clearing render objects\n");

		//////////////////////////////////
		// Clear render sets in all sets
		tGuiSetMapIt setIt = m_mapSets.begin();
		for(; setIt != m_mapSets.end(); ++setIt)
		{
			cGuiSet *pSet = setIt->second;
			pSet->ClearRenderObjects();
		}

		//Delete all gfx elements to be deleted (this way, all pointers in set stay valid!)
		STLDeleteAll(mlstToBeDestroyedGfxElements);
	}
	
	//-----------------------------------------------------------------------

	iGuiMaterial* cGui::GetMaterial(eGuiMaterial aType)
	{
		return mvMaterials[aType];
	}

	//-----------------------------------------------------------------------

	cGuiSkin* cGui::CreateSkin(const tString& asFile)
	{
		tString sName = cString::SetFileExt(cString::GetFileName(asFile),"");
		
		//See if skin exists
		tGuiSkinMapIt it = m_mapSkins.find(sName);
		if(it != m_mapSkins.end())
		{
			return it->second;
		}
		
		//Create new skin.
		cGuiSkin *pSkin = hplNew( cGuiSkin, (sName, this) );
		tWString sPath = mpResources->GetFileSearcher()->GetFilePath(asFile);

		if(pSkin->LoadFromFile(sPath)==false)
		{
			hplDelete(pSkin);
			return NULL;
		}

		m_mapSkins.insert(tGuiSkinMap::value_type(sName,pSkin));
		return pSkin;
	}

	//-----------------------------------------------------------------------

	eGuiSkinGfx cGui::GetSkinGfxFromString(const tString& asType)
	{
		tGuiSkinGfxMapIt it = m_mapSkinGfxStrings.find(asType);
		if(it == m_mapSkinGfxStrings.end())
		{
			Warning("Skin gfx type '%s' does not exist!\n",asType.c_str());
			return eGuiSkinGfx_LastEnum;
		}

		return it->second;
	}

	eGuiSkinFont cGui::GetSkinFontFromString(const tString& asType)
	{
		tGuiSkinFontMapIt it = m_mapSkinFontStrings.find(asType);
		if(it == m_mapSkinFontStrings.end())
		{
			Warning("Skin Font type '%s' does not exist!\n",asType.c_str());
			return eGuiSkinFont_LastEnum;
		}

		return it->second;
	}

	eGuiSkinAttribute cGui::GetSkinAttributeFromString(const tString& asType)
	{
		tGuiSkinAttributeMapIt it = m_mapSkinAttributeStrings.find(asType);
		if(it == m_mapSkinAttributeStrings.end())
		{
			Warning("Skin Attribute type '%s' does not exist!\n",asType.c_str());
			return eGuiSkinAttribute_LastEnum;
		}

		return it->second;
	}

	//-----------------------------------------------------------------------

	tString cGui::GetSkinGfxString(eGuiSkinGfx aType)
	{
		tGuiSkinGfxMapIt it = m_mapSkinGfxStrings.begin();
		for(; it != m_mapSkinGfxStrings.end(); ++it)
		{
			if(it->second == aType) return it->first;
		}

		return "Unknown";
	}
	
	tString cGui::GetSkinFontString(eGuiSkinFont aType)
	{
		tGuiSkinFontMapIt it = m_mapSkinFontStrings.begin();
		for(; it != m_mapSkinFontStrings.end(); ++it)
		{
			if(it->second == aType) return it->first;
		}

		return "Unknown";
	}
	
	tString cGui::GetSkinAttributeString(eGuiSkinAttribute aType)
	{
		tGuiSkinAttributeMapIt it = m_mapSkinAttributeStrings.begin();
		for(; it != m_mapSkinAttributeStrings.end(); ++it)
		{
			if(it->second == aType) return it->first;
		}

		return "Unknown";
	}

	//-----------------------------------------------------------------------

	cGuiSet* cGui::CreateSet(const tString& asName, cGuiSkin *apSkin)
	{
		cGuiSet *pSet = hplNew( cGuiSet, (asName,this,apSkin,mpResources, mpGraphics, mpSound,mpScene) );

		m_mapSets.insert(tGuiSetMap::value_type(asName, pSet));
        
		return pSet;
	}
	
	cGuiSet* cGui::GetSetFromName(const tString& asName)
	{
		tGuiSetMapIt it = m_mapSets.find(asName);
		if(it == m_mapSets.end()) return NULL;

		return it->second;
	}
	
	void cGui::SetFocus(cGuiSet* apSet)
	{
		if(mpSetInFocus == apSet) return;

		//TODO: Call lost focus or stuff

		mpSetInFocus = apSet;
	}
	
	void cGui::SetFocusByName(const tString& asSetName)
	{
		cGuiSet *pSet = GetSetFromName(asSetName);
		if(pSet) SetFocus(pSet);
	}
	
	void cGui::DestroySet(cGuiSet *apSet)
	{
		if(apSet==NULL) return;

		tGuiSetMapIt it = m_mapSets.begin();
		for(; it != m_mapSets.end(); ++it)
		{
			if(apSet == it->second)
			{
				m_mapSets.erase(it);
				hplDelete( apSet);
				break;
			}
		}
	}

	//-----------------------------------------------------------------------

	cGuiGfxElement* cGui::CreateGfxFilledRect(const cColor& aColor, eGuiMaterial aMaterial, bool abAddToList)
	{
		cGuiGfxElement *pGfxElem = hplNew( cGuiGfxElement, (this) );

		pGfxElem->SetColor(aColor);
		pGfxElem->SetMaterial(GetMaterial(aMaterial));

		if(abAddToList) mlstGfxElements.push_back(pGfxElem);

		return pGfxElem;
	}

	//-----------------------------------------------------------------------

	cGuiGfxElement* cGui::CreateGfxImage(	const tString &asFile, eGuiMaterial aMaterial,
		const cColor& aColor, bool abAddToList)
	{
		////////////////////////////
		// Load image
		cFrameSubImage *pImage = mpResources->GetImageManager()->CreateImage(asFile);
		if(pImage==NULL){
			Error("Could not load image '%s'!\n",asFile.c_str());
			return NULL;
		}

		/////////////////////////////
		// Create element
		cGuiGfxElement *pGfxElem = hplNew( cGuiGfxElement, (this) );

		pGfxElem->SetColor(aColor);
		pGfxElem->SetMaterial(GetMaterial(aMaterial));
		pGfxElem->AddImage(pImage);

		if(abAddToList) mlstGfxElements.push_back(pGfxElem);

		//if(abAddToList) Log("Create gfx '%s': %d %d\n", asFile.c_str(), pGfxElem, pImage);

		return pGfxElem;
	}

	//-----------------------------------------------------------------------

	cGuiGfxElement* cGui::CreateGfxTexture(	const tString &asFile,
											eGuiMaterial aMaterial,
											eTextureType aTextureType,
											const cColor& aColor, bool abMipMaps, 
											bool abAddToList)
	{
		///////////////////
		// Error check
		if( aTextureType != eTextureType_2D && aTextureType != eTextureType_Rect )
		{
			Error("Texture %s could not be loaded because texture type param was not valid!\n",asFile.c_str());
			return NULL;
		}

		///////////////////
		// Load texture
		iTexture *pTexture = mpResources->GetTextureManager()->Create2D(asFile,abMipMaps,aTextureType);
		if(pTexture==NULL)
		{
			Error("Could not load texture '%s'!\n",asFile.c_str());
			return NULL;
		}
		
		/////////////////////////////
		// Create element
		cGuiGfxElement *pGfxElem = CreateGfxTexture(pTexture,true,aMaterial,aColor,abAddToList);

		return pGfxElem;
	}

	//-----------------------------------------------------------------------

	cGuiGfxElement* cGui::CreateGfxTexture(iTexture *apTexture, bool abAutoDestroyTexture, 
											eGuiMaterial aMaterial,
											const cColor& aColor,
											bool abAddToList,
											const cVector2f& avStartUV,
											const cVector2f& avEndUV)
	{
		cGuiGfxElement *pGfxElem = hplNew( cGuiGfxElement, (this) );

		if(apTexture->GetUsage() == eTextureUsage_RenderTarget) pGfxElem->SetFlipUvYAxis(true);

		pGfxElem->SetColor(aColor);
		pGfxElem->SetMaterial(GetMaterial(aMaterial));
		pGfxElem->AddTexture(apTexture, avStartUV, avEndUV);
		pGfxElem->SetDestroyTexture(abAutoDestroyTexture);

		if(abAddToList) mlstGfxElements.push_back(pGfxElem);

		return pGfxElem;
	}

	//-----------------------------------------------------------------------

	cGuiGfxElement* cGui::CreateGfxImageBuffer(	const tString &asFile,eGuiMaterial aMaterial,
		bool abCreateAnimation,
		const cColor& aColor, bool abAddToList)
	{
		////////////////////////////
		// Load images
		tString sName = cString::SetFileExt(asFile,"");
		tString sExt = cString::GetFileExt(asFile);

		std::vector<cFrameSubImage*> vImages;

		int lFileNum =0;
		while(true)
		{
			tString sNum = lFileNum<=9 ? "0"+cString::ToString(lFileNum) : cString::ToString(lFileNum);
			tString sFile = sName + sNum+"."+sExt;

			if(mpResources->GetFileSearcher()->GetFilePath(sFile)==_W("")) break;

			cFrameSubImage *pImage = mpResources->GetImageManager()->CreateImage(sFile);
			vImages.push_back(pImage);
			++lFileNum;
		}

		if(vImages.empty())
		{
			Error("Could not load any images with '%s' as base!\n",asFile.c_str());
			return NULL;
		}

		/////////////////////////////
		// Create element
		cGuiGfxElement *pGfxElem = hplNew( cGuiGfxElement, (this) );

		pGfxElem->SetColor(aColor);
		pGfxElem->SetMaterial(GetMaterial(aMaterial));

		for(size_t i=0; i< vImages.size(); ++i)
		{
			if(i==0) pGfxElem->AddImage(vImages[i]);
			pGfxElem->AddImageToBuffer(vImages[i]);
		}

		if(abAddToList) mlstGfxElements.push_back(pGfxElem);

		///////////////////////////////
		// Create animation
		if(abCreateAnimation)
		{
			cGuiGfxAnimation *pAnim = pGfxElem->CreateAnimtion("Default");
			for(size_t i=0; i<vImages.size(); ++i)
			{
				pAnim->AddFrame((int)i);
			}
		}

		return pGfxElem;
	}


	//-----------------------------------------------------------------------

	void cGui::DestroyGfx(cGuiGfxElement* apGfx)
	{
		//STLFindAndDelete(mlstGfxElements,apGfx);

		tGuiGfxElementListIt it = mlstGfxElements.begin();
		for(; it != mlstGfxElements.end(); ++it)
		{
			cGuiGfxElement*pGfx = *it;
			if(pGfx == apGfx)
			{
				mlstToBeDestroyedGfxElements.push_back(pGfx);
				mlstGfxElements.erase(it);
				break;
			}
		}
	}

	//-----------------------------------------------------------------------

	
	bool cGui::SendMousePos(const cVector2l &avPos, const cVector2l &avRel)
	{
		if(mpSetInFocus==NULL) return false;

		cVector2f vVirtualPos = cVector2f( (float)avPos.x, (float)avPos.y) / mpGraphics->GetLowLevel()->GetScreenSizeFloat();
		cVector2f vVirtualRel = cVector2f( (float)avRel.x, (float)avRel.y) / mpGraphics->GetLowLevel()->GetScreenSizeFloat();
		
        vVirtualPos *= mpSetInFocus->GetVirtualSize();
		vVirtualPos -= mpSetInFocus->GetVirtualSizeOffset();
		
		vVirtualRel *= mpSetInFocus->GetVirtualSize();

		return mpSetInFocus->SendMessage(eGuiMessage_MouseMove, cGuiMessageData(vVirtualPos, vVirtualRel));
	}
	
	bool cGui::SendMouseClickDown(eGuiMouseButton aButton, int alKeyModifiers)
	{
		if(mpSetInFocus==NULL)return false;

		cGuiMessageData data;
		data.mlVal = aButton;
		data.mKeyPress.mlModifier = alKeyModifiers;

		return mpSetInFocus->SendMessage(eGuiMessage_MouseDown,data);
	}
	
	bool cGui::SendMouseClickUp(eGuiMouseButton aButton, int alKeyModifiers)
	{
		if(mpSetInFocus==NULL)return false;

		cGuiMessageData data;
		data.mlVal = aButton;
		data.mKeyPress.mlModifier = alKeyModifiers;

		return mpSetInFocus->SendMessage(eGuiMessage_MouseUp, data);
	}

	bool cGui::SendMouseDoubleClick(eGuiMouseButton aButton, int alKeyModifiers)
	{
		if(mpSetInFocus==NULL)return false;

		cGuiMessageData data;
		data.mlVal = aButton;
		data.mKeyPress.mlModifier = alKeyModifiers;

		return mpSetInFocus->SendMessage(eGuiMessage_MouseDoubleClick,data);
	}

	bool cGui::SendKeyPress(const cKeyPress& keyPress)
	{
		if(mpSetInFocus==NULL)return false;

		return mpSetInFocus->SendMessage(eGuiMessage_KeyPress, cGuiMessageData(keyPress));
	}

	bool cGui::SendKeyRelease(const cKeyPress& aKeyPress)
	{
		if(mpSetInFocus==NULL)return false;

		return mpSetInFocus->SendMessage(eGuiMessage_KeyRelease, cGuiMessageData(aKeyPress));
	}

	//-----------------------------------------------------------------------

	bool cGui::SendGamepadInput(const cGamepadInputData& aInput)
	{
		if(mpSetInFocus==NULL)return false;

		return mpSetInFocus->SendMessage(eGuiMessage_GamepadInput, cGuiMessageData(aInput));
	}

	//-----------------------------------------------------------------------

	bool cGui::SendUIArrowPress(eUIArrow aX)
	{
		if(mpSetInFocus==NULL) return false;

		return mpSetInFocus->SendMessage(eGuiMessage_UIArrowPress, cGuiMessageData(aX));
	}

	bool cGui::SendUIArrowRelease(eUIArrow aX)
	{
		if(mpSetInFocus==NULL) return false;

		return mpSetInFocus->SendMessage(eGuiMessage_UIArrowRelease, cGuiMessageData(aX));
	}

	bool cGui::SendUIButtonPress(eUIButton aX)
	{
		if(mpSetInFocus==NULL) return false;

		return mpSetInFocus->SendMessage(eGuiMessage_UIButtonPress, cGuiMessageData(aX));
	}
	
	bool cGui::SendUIButtonRelease(eUIButton aX)
	{
		if(mpSetInFocus==NULL) return false;

		return mpSetInFocus->SendMessage(eGuiMessage_UIButtonRelease, cGuiMessageData(aX));
	}

	bool cGui::SendUIButtonDoublePress(eUIButton aX)
	{
		if(mpSetInFocus==NULL) return false;

		return mpSetInFocus->SendMessage(eGuiMessage_UIButtonDoublePress, cGuiMessageData(aX));
	}
	
	//-----------------------------------------------------------------------
	
	eMouseButton cGui::TranslateGuiMouseToMouse(eGuiMouseButton aButton)
	{
		switch(aButton)
		{
		case eGuiMouseButton_Left:
			return eMouseButton_Left;
		case eGuiMouseButton_Middle:
			return eMouseButton_Middle;
		case eGuiMouseButton_Right:
			return eMouseButton_Right;
		case eGuiMouseButton_WheelUp:
			return eMouseButton_WheelUp;
		case eGuiMouseButton_WheelDown:
			return eMouseButton_WheelDown;
		case eGuiMouseButton_Button6:
			return eMouseButton_Button6;
		case eGuiMouseButton_Button7:
			return eMouseButton_Button7;
		case eGuiMouseButton_Button8:
			return eMouseButton_Button8;
		case eGuiMouseButton_Button9:
			return eMouseButton_Button9;
		}
		return eMouseButton_LastEnum;
	}

	//-----------------------------------------------------------------------

	eGuiMouseButton cGui::TranslateMouseToGuiMouse(eMouseButton aButton)
	{
		switch(aButton)
		{
		case eMouseButton_Left:
			return eGuiMouseButton_Left;
		case eMouseButton_Middle:
			return eGuiMouseButton_Middle;
		case eMouseButton_Right:
			return eGuiMouseButton_Right;
		case eMouseButton_WheelUp:
			return eGuiMouseButton_WheelUp;
		case eMouseButton_WheelDown:
			return eGuiMouseButton_WheelDown;
		case eMouseButton_Button6:
			return eGuiMouseButton_Button6;
		case eMouseButton_Button7:
			return eGuiMouseButton_Button7;
		case eMouseButton_Button8:
			return eGuiMouseButton_Button8;
		case eMouseButton_Button9:
			return eGuiMouseButton_Button9;
		}
		return eGuiMouseButton_LastEnum;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

#define AddFont(name) m_mapSkinFontStrings.insert(tGuiSkinFontMap::value_type(#name,eGuiSkinFont_##name))
#define AddAttribute(name) m_mapSkinAttributeStrings.insert(tGuiSkinAttributeMap::value_type(#name,eGuiSkinAttribute_##name))
#define AddGfx(name) m_mapSkinGfxStrings.insert(tGuiSkinGfxMap::value_type(#name,eGuiSkinGfx_##name))
	void cGui::GenerateSkinTypeStrings()
	{
		///////////////////////
		// Fonts
        AddFont(Default);
		AddFont(Disabled);
		AddFont(WindowLabel);

		///////////////////////
		// Attributes
		AddAttribute(WindowLabelTextOffset);
		AddAttribute(WindowButtonSize);
		AddAttribute(WindowBordersOutside);

		AddAttribute(ButtonPressedContentOffset);

		AddAttribute(SliderButtonSize);

		AddAttribute(ListBoxSliderWidth);

		AddAttribute(ComboBoxButtonWidth);
		AddAttribute(ComboBoxSliderWidth);

		AddAttribute(ContextMenuItemArrowSize);
		AddAttribute(ContextMenuItemCheckSize);
		AddAttribute(ContextMenuItemHPadding);
		AddAttribute(ContextMenuItemVPadding);
		AddAttribute(ContextMenuItemSeparation);
		AddAttribute(ContextMenuItemTextLeftPadding);
		AddAttribute(ContextMenuItemTextRightPadding);
		AddAttribute(ContextMenuSeparatorPadding);
		
		AddAttribute(MainMenuItemTextPadding);
		AddAttribute(MainMenuItemVPadding);
		AddAttribute(MainMenuItemSeparation);

		AddAttribute(MenuSeparatorPadding);
		AddAttribute(MenuSeparatorHeight);

		AddAttribute(TabFrameButtonSize);
		AddAttribute(TabFrameIconPadding);
		AddAttribute(TabFrameTextLeftPadding);
		AddAttribute(TabFrameTextRightPadding);
		AddAttribute(TabFrameLabelInactiveOffset);
		AddAttribute(TabFrameLabelActiveOffset);
		AddAttribute(FirstTabOffset);
		AddAttribute(TabSeparation);

		AddAttribute(GroupHeaderOffset);
		AddAttribute(GroupHeaderOverUpperBorder);

		AddAttribute(DefaultFontHighlightColor);

		AddAttribute(TabFrameButtonSize);

		///////////////////////
		// Pointer Graphics
		AddGfx(PointerNormal);
		AddGfx(PointerText);
		
		///////////////////////
		// Window Graphics
		AddGfx(WindowBorderRight);
		AddGfx(WindowBorderLeft);
		AddGfx(WindowBorderUp);
		AddGfx(WindowBorderDown);

		AddGfx(WindowCornerLU);
		AddGfx(WindowCornerRU);
		AddGfx(WindowCornerRD);
		AddGfx(WindowCornerLD);

		AddGfx(WindowLabel);
		AddGfx(WindowBackground);

		AddGfx(WindowButtonCross);

		///////////////////////////////////
		// Frame Graphics
		AddGfx(FrameBorderRight);
		AddGfx(FrameBorderLeft);
		AddGfx(FrameBorderUp);
		AddGfx(FrameBorderDown);

		AddGfx(FrameCornerLU);
		AddGfx(FrameCornerRU);
		AddGfx(FrameCornerRD);
		AddGfx(FrameCornerLD);

		AddGfx(FrameBackground);

		///////////////////////////////////
		// Check box Graphics
		AddGfx(CheckBoxEnabledUnchecked);
		AddGfx(CheckBoxEnabledChecked);
		AddGfx(CheckBoxDisabledUnchecked);
		AddGfx(CheckBoxDisabledChecked);


		///////////////////////////////////
		// Text box Graphics
		AddGfx(TextBoxBackground);
		AddGfx(TextBoxSelectedTextBack);
		AddGfx(TextBoxMarker);
		
		///////////////////////////////////
		// List box Graphics
		AddGfx(ListBoxBackground);

		///////////////////////////////////
		// List box Graphics
		AddGfx(ComboBoxButtonIcon);

		AddGfx(ComboBoxBorderRight);
		AddGfx(ComboBoxBorderLeft);
		AddGfx(ComboBoxBorderUp);
		AddGfx(ComboBoxBorderDown);

		AddGfx(ComboBoxCornerLU);
		AddGfx(ComboBoxCornerRU);
		AddGfx(ComboBoxCornerRD);
		AddGfx(ComboBoxCornerLD);

		AddGfx(ComboBoxBackground);

		///////////////////////////////////
		// Slider Graphics
		AddGfx(SliderHandleBorderRight);
		AddGfx(SliderHandleBorderLeft);
		AddGfx(SliderHandleBorderUp);
		AddGfx(SliderHandleBorderDown);

		AddGfx(SliderHandleCornerLU);
		AddGfx(SliderHandleCornerRU);
		AddGfx(SliderHandleCornerRD);
		AddGfx(SliderHandleCornerLD);

		AddGfx(SliderHandleBackground);

		AddGfx(SliderVertArrowUp);
		AddGfx(SliderVertArrowDown);
		AddGfx(SliderVertBackground);

		AddGfx(SliderHoriArrowUp);
		AddGfx(SliderHoriArrowDown);
		AddGfx(SliderHoriBackground);


		///////////////////////
		//Button Graphics
		AddGfx(ButtonUpBorderRight);
		AddGfx(ButtonUpBorderLeft);
		AddGfx(ButtonUpBorderUp);
		AddGfx(ButtonUpBorderDown);

		AddGfx(ButtonUpCornerLU);
		AddGfx(ButtonUpCornerRU);
		AddGfx(ButtonUpCornerRD);
		AddGfx(ButtonUpCornerLD);

		AddGfx(ButtonUpBackground);

		AddGfx(ButtonDownBorderRight);
		AddGfx(ButtonDownBorderLeft);
		AddGfx(ButtonDownBorderUp);
		AddGfx(ButtonDownBorderDown);

		AddGfx(ButtonDownCornerLU);
		AddGfx(ButtonDownCornerRU);
		AddGfx(ButtonDownCornerRD);
		AddGfx(ButtonDownCornerLD);

		AddGfx(ButtonDownBackground);

		//////////////////////////
		//Menu Graphics
		AddGfx(MenuEnabledChecked);
		AddGfx(MenuDisabledChecked);
		AddGfx(MenuSeparatorLine);
		AddGfx(MenuItemArrow);

		//////////////////////////
		//Tab Graphics
		AddGfx(TabLabelBorderRight);
		AddGfx(TabLabelBorderLeft);
		AddGfx(TabLabelBorderUp);
		AddGfx(TabLabelBorderDown);

		AddGfx(TabLabelCornerLU);
		AddGfx(TabLabelCornerRU);
		AddGfx(TabLabelCornerRD);
		AddGfx(TabLabelCornerLD);

		AddGfx(TabBorderRight);
		AddGfx(TabBorderLeft);
		AddGfx(TabBorderUp);
		AddGfx(TabBorderDown);

		AddGfx(TabCornerLU);
		AddGfx(TabCornerRU);
		AddGfx(TabCornerRD);
		AddGfx(TabCornerLD);

		AddGfx(TabLabelBackground);
		AddGfx(TabLabelBackgroundActive);
		AddGfx(TabBackground);

		AddGfx(TabFrameArrowLeft);
		AddGfx(TabFrameArrowRight);

		//////////////////////////
		//Group Graphics
		AddGfx(GroupBorderLeft);
		AddGfx(GroupBorderRight);
		AddGfx(GroupBorderUp);
		AddGfx(GroupBorderDown);

		AddGfx(GroupCornerLU);
		AddGfx(GroupCornerRU);
		AddGfx(GroupCornerRD);
		AddGfx(GroupCornerLD);

		AddGfx(GroupBackground);

		//////////////////////////
		//File Picker Graphics
		AddGfx(FilePickerUpButton);

		AddGfx(FilePickerIconGraphics);
		AddGfx(FilePickerIconModels);
		AddGfx(FilePickerIconSounds);
		AddGfx(FilePickerIconText);
		AddGfx(FilePickerIconOther);
		AddGfx(FilePickerIconFolder);		
	}
	//-----------------------------------------------------------------------

}
