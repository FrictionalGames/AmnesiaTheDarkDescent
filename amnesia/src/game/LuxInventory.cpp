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

#include "LuxInventory.h"

#include "LuxPlayer.h"
#include "LuxInputHandler.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxItemType.h"
#include "LuxEntity.h"
#include "LuxMessageHandler.h"
#include "LuxHintHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxEffectHandler.h"
#include "LuxJournal.h"
#include "LuxGlobalDataHandler.h"
#include "LuxAchievementHandler.h"


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInventory::cLuxInventory() : iLuxUpdateable("LuxInventory")
{
	///////////////////////////////
	//Get Engine
	mpGui = gpBase->mpEngine->GetGui();
	mpScene = gpBase->mpEngine->GetScene();
	mpGraphics = gpBase->mpEngine->GetGraphics();
	
	///////////////////////////////
	//Setup GUI stuff
	mpGuiSkin = mpGui->CreateSkin("gui_main_menu.skin");
	mpGuiSet = mpGui->CreateSet("Inventory",mpGuiSkin);
	mpGuiSet->SetFocusDrawCallback(this, kGuiCallback(InventoryDrawFocus));
	AppDeviceWasPlugged(); // Sets up focus draw

	mvGuiSetCenterSize = cVector2f(800, 600);
	LuxCalcGuiSetScreenOffset(mvGuiSetCenterSize, mvGuiSetSize, mvGuiSetOffset);
	mvGuiSetStartPos = cVector3f(-mvGuiSetOffset.x,-mvGuiSetOffset.y,0);

	mpGuiSet->SetVirtualSize(mvGuiSetSize, -1000,1000, mvGuiSetOffset);
	mpGuiSet->SetActive(false);
	mpGuiSet->SetDrawMouse(false);//Init
	
	///////////////////////////////
	//Load settings
	mvScreenSize = gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetScreenSizeFloat();

	mfFadeInTime = gpBase->mpMenuCfg->GetFloat("Inventory","FadeInTime", 10);
	mfFadeOutTime = gpBase->mpMenuCfg->GetFloat("Inventory","FadeOutTime",10);

	///////////////////////////////
	//Create Viewport
	mpViewport = mpScene->CreateViewport();
	mpViewport->SetActive(false);
	mpViewport->SetVisible(false);

	mpViewport->AddGuiSet(mpGuiSet);

	///////////////////////////////
	//Add item types
	mvItemTypes.resize(eLuxItemType_LastEnum);

	mvItemTypes[eLuxItemType_Puzzle] = hplNew( cLuxItemType_Puzzle, ());
	mvItemTypes[eLuxItemType_HandObject] = hplNew( cLuxItemType_HandObject, ());
	mvItemTypes[eLuxItemType_Note] = hplNew( cLuxItemType_Note, ());
	mvItemTypes[eLuxItemType_Diary] = hplNew( cLuxItemType_Diary, ());
	mvItemTypes[eLuxItemType_Coins] = hplNew( cLuxItemType_Coins, ());
	mvItemTypes[eLuxItemType_Lantern] = hplNew( cLuxItemType_Lantern, ());
	mvItemTypes[eLuxItemType_Health] = hplNew( cLuxItemType_Health, ());
	mvItemTypes[eLuxItemType_Sanity] = hplNew( cLuxItemType_Sanity, ());
	mvItemTypes[eLuxItemType_LampOil] = hplNew( cLuxItemType_LampOil, ());
	mvItemTypes[eLuxItemType_Tinderbox] = hplNew( cLuxItemType_Tinderbox, ());

	mfPickedUpAt = -1;
	mbPickedObjectMoved = false;

	
	///////////////////////////////
	//Set up variables
	for(int i=0; i<eLuxInventoryFader_LastEnum; ++i)
	{
		mvAlphaFader[i].mfFadeInSpeed = 1.0f;
		mvAlphaFader[i].mfFadeOutSpeed = 1.0f;
	}

	///////////////////////////////
	//Load Data

	/////////////////
	// Sanity frame
	// Corners
	mpFrameSanityCorners[0] = mpGui->CreateGfxImage("inventory_frame_sanity_corner_lu.tga", eGuiMaterial_Alpha);
	mpFrameSanityCorners[1] = mpGui->CreateGfxImage("inventory_frame_sanity_corner_ru.tga", eGuiMaterial_Alpha);
	mpFrameSanityCorners[2] = mpGui->CreateGfxImage("inventory_frame_sanity_corner_rd.tga", eGuiMaterial_Alpha);
	mpFrameSanityCorners[3] = mpGui->CreateGfxImage("inventory_frame_sanity_corner_ld.tga", eGuiMaterial_Alpha);

	// Borders
	mpFrameSanityBorders[0] = mpGui->CreateGfxImage("inventory_frame_sanity_border_u.tga", eGuiMaterial_Alpha);
	mpFrameSanityBorders[1] = mpGui->CreateGfxImage("inventory_frame_sanity_border_r.tga", eGuiMaterial_Alpha);
	mpFrameSanityBorders[2] = mpGui->CreateGfxImage("inventory_frame_sanity_border_d.tga", eGuiMaterial_Alpha);
	mpFrameSanityBorders[3] = mpGui->CreateGfxImage("inventory_frame_sanity_border_l.tga", eGuiMaterial_Alpha);

	/////////////////
	// Health frame
	// Corners
	mpFrameHealthCorners[0] = mpGui->CreateGfxImage("inventory_frame_health_corner_lu.tga", eGuiMaterial_Alpha);
	mpFrameHealthCorners[1] = mpGui->CreateGfxImage("inventory_frame_health_corner_ru.tga", eGuiMaterial_Alpha);
	mpFrameHealthCorners[2] = mpGui->CreateGfxImage("inventory_frame_health_corner_rd.tga", eGuiMaterial_Alpha);
	mpFrameHealthCorners[3] = mpGui->CreateGfxImage("inventory_frame_health_corner_ld.tga", eGuiMaterial_Alpha);

	// Borders
	mpFrameHealthBorders[0] = mpGui->CreateGfxImage("inventory_frame_health_border_u.tga", eGuiMaterial_Alpha);
	mpFrameHealthBorders[1] = mpGui->CreateGfxImage("inventory_frame_health_border_r.tga", eGuiMaterial_Alpha);
	mpFrameHealthBorders[2] = mpGui->CreateGfxImage("inventory_frame_health_border_d.tga", eGuiMaterial_Alpha);
	mpFrameHealthBorders[3] = mpGui->CreateGfxImage("inventory_frame_health_border_l.tga", eGuiMaterial_Alpha);

	//////////////////
	// Generic frame
	// Corners
	mpFrameGenericCorners[0] = mpGui->CreateGfxImage("inventory_frame_generic_corner_lu.tga", eGuiMaterial_Alpha);
	mpFrameGenericCorners[1] = mpGui->CreateGfxImage("inventory_frame_generic_corner_ru.tga", eGuiMaterial_Alpha);
	mpFrameGenericCorners[2] = mpGui->CreateGfxImage("inventory_frame_generic_corner_rd.tga", eGuiMaterial_Alpha);
	mpFrameGenericCorners[3] = mpGui->CreateGfxImage("inventory_frame_generic_corner_ld.tga", eGuiMaterial_Alpha);

	// Borders
	mpFrameGenericBorders[0] = mpGui->CreateGfxImage("inventory_frame_generic_border_u.tga", eGuiMaterial_Alpha);
	mpFrameGenericBorders[1] = mpGui->CreateGfxImage("inventory_frame_generic_border_r.tga", eGuiMaterial_Alpha);
	mpFrameGenericBorders[2] = mpGui->CreateGfxImage("inventory_frame_generic_border_d.tga", eGuiMaterial_Alpha);
	mpFrameGenericBorders[3] = mpGui->CreateGfxImage("inventory_frame_generic_border_l.tga", eGuiMaterial_Alpha);

	mpWhiteGfx = mpGui->CreateGfxFilledRect(cColor(1,1), eGuiMaterial_Alpha);
 
	cParserVarContainer programVars;
	mpEffectProgram = mpGraphics->CreateGpuProgramFromShaders("InventoryEffect","inventory_screen_effect_vtx.glsl",
																"inventory_screen_effect_frag.glsl", &programVars);

	mpFontDefault = NULL;
	mpFontHeader = NULL;

	mpScript = NULL;

	CreateGui();

	mfTimer = 0;
}

//-----------------------------------------------------------------------

cLuxInventory::~cLuxInventory()
{
	STLDeleteAll(mlstCombineCallbacks);

	STLDeleteAll(mvSlots);
	STLDeleteAll(mvItems);
	STLDeleteAll(mvItemTypes);

	if(mpScript)
	{
		gpBase->mpEngine->GetResources()->GetScriptManager()->Destroy(mpScript);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// ITEM
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInventory_Item::cLuxInventory_Item(const tString& asName, eLuxItemType aType, const tString& asSubType, cLuxInventory *apInventory)
{
	mpInventory = apInventory;

	msName = asName;
	mType = aType;
	msSubType = asSubType;

	mlCount = 1;
	mfAmount = 1;

	mpImage = NULL;
}

cLuxInventory_Item::~cLuxInventory_Item()
{
	if(mpImage){
		mpInventory->mpGui->DestroyGfx(mpImage);
	}

}



//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SLOT
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInventory_Slot::cLuxInventory_Slot(cLuxInventory *apInventory, cWidgetImage* apImage, int alIdx)
{
	mpInventory = apInventory;
	cGui* pGui = mpInventory->mpGui;

	cGuiGfxElement* pGfx = NULL;
	
	if(mpInventory->msLayout_SlotGfx!="")
		pGui->CreateGfxImage(mpInventory->msLayout_SlotGfx, eGuiMaterial_Alpha);


	if(pGfx==NULL)
		pGfx = pGui->CreateGfxFilledRect(cColor(1), eGuiMaterial_Alpha);
	pGfx->SetColor(mpInventory->mLayout_SlotColor);

	mpImage = apImage;
	mpImage->SetImage(pGfx);

	if(mpInventory->mvLayout_SlotSize==cVector2f(-1))
		mpInventory->mvLayout_SlotSize = mpImage->GetSize();
	else
		mpImage->SetSize(mpInventory->mvLayout_SlotSize);

	mpImage->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(OnMouseDown));
	mpImage->AddCallback(eGuiMessage_MouseUp,this, kGuiCallback(OnMouseUp));
	mpImage->AddCallback(eGuiMessage_MouseDoubleClick,this, kGuiCallback(OnMouseDoubleClick));
	mpImage->AddCallback(eGuiMessage_MouseEnter,this, kGuiCallback(OnMouseEnter));
	mpImage->AddCallback(eGuiMessage_MouseLeave,this, kGuiCallback(OnMouseLeave));
	mpImage->AddCallback(eGuiMessage_OnUpdate,this, kGuiCallback(OnUpdate));
	mpImage->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(OnDraw));
	mpImage->AddCallback(eGuiMessage_GetUINavFocus, this, kGuiCallback(OnGetUINavFocus));
	mpImage->AddCallback(eGuiMessage_LoseUINavFocus, this, kGuiCallback(OnLoseUINavFocus));
	mpImage->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(OnUIButtonPress));
	mpImage->AddCallback(eGuiMessage_UIButtonDoublePress, this, kGuiCallback(OnUIButtonDoublePress));
	mpImage->AddCallback(eGuiMessage_UIButtonRelease, this, kGuiCallback(OnUIButtonRelease));

	mpGlowGfx = NULL;

	if(mpInventory->msLayout_SlotMouseOverGfx!="")
		mpGlowGfx = mpInventory->mpGui->CreateGfxImage(mpInventory->msLayout_SlotMouseOverGfx,eGuiMaterial_Alpha);

	if(mpGlowGfx==NULL)
	{
		mpGlowGfx = pGui->CreateGfxFilledRect(cColor(1), eGuiMaterial_Alpha);
		mpGlowGfx->SetActiveSize(mpInventory->mvLayout_SlotSize);
	}
	mpGlowGfx->SetColor(mpInventory->mLayout_SlotMouseOverColor);

	mlIdx = alIdx;

	mfGlowAlpha = 0;
}

cLuxInventory_Slot::~cLuxInventory_Slot()
{

}

//-----------------------------------------------------------------------

cLuxInventory_Item* cLuxInventory_Slot::GetItem()
{
	if((int)mpInventory->mvItems.size() <= mlIdx) 
		return NULL;
	else
		return mpInventory->mvItems[mlIdx];

}

//-----------------------------------------------------------------------

bool cLuxInventory_Slot::OnMouseDown(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(GetItem())
	{
		const cVector2f &vSize = apWidget->GetSize();
		cVector3f vItemPos = apWidget->GetGlobalPosition() + cVector3f((vSize - GetItem()->GetImage()->GetImageSize()))*0.5f;

		cVector3f vPos = apWidget->GetGlobalPosition();
		cVector2f vOffset = cVector2f(vItemPos.x, vItemPos.y) - mpInventory->mpGuiSet->GetMousePos();
		mpInventory->SetPickedItem(GetItem(), vOffset);
	}
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnMouseDown);

//-----------------------------------------------------------------------

bool cLuxInventory_Slot::OnMouseUp(iWidget* apWidget, const cGuiMessageData& aData)
{
	cLuxInventory_Item *pPickedItem = mpInventory->GetPickedItem();
	cLuxInventory_Item *pItem = GetItem();

	if(pItem && pPickedItem && pPickedItem != pItem)
	{
		if(mpInventory->CheckSpecialCombineAction(pItem, pPickedItem, mlIdx)==false)
		{
			cLuxCombineItemsCallback *pComb = mpInventory->GetCombineCallback(pItem->GetName(), pPickedItem->GetName());
			if(pComb)
			{
				bool bAutoDestroy = pComb->mbAutoDestroy;
				tString sCombName = pComb->msName;
				mpInventory->RunScript(pComb->msFunction+ "(\"" + pComb->msItemA + "\", \"" + pComb->msItemB + "\")" );
				
				if(bAutoDestroy) 
				{
					tLuxCombineItemsCallbackListIt combIt = mpInventory->mlstCombineCallbacks.begin();
                    for(; combIt != mpInventory->mlstCombineCallbacks.end(); ++combIt)
					{
						cLuxCombineItemsCallback *pCombCallback = *combIt;
						if(pCombCallback == pComb && sCombName == pCombCallback->msName)
						{
							mpInventory->mlstCombineCallbacks.erase(combIt);
							hplDelete(pComb);
							break;
						}
					}
				}
			}
			else
			{
				mpInventory->SetMessageText(kTranslate("Inventory","CombinationDoesNotWork"),0);
			}
		}

		////////////////////////////////
		// Set back the normal description,
		// Need to get item again in case there have been changes.
		iLuxItemType *pItemType = mpInventory->GetItemTypeData(pItem->GetType());
		pItem = GetItem();
		mpInventory->SetDescTextFromItem(pItem);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnMouseUp);


//-----------------------------------------------------------------------

bool cLuxInventory_Slot::OnMouseDoubleClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	cLuxInventory_Item *pItem = GetItem();
	
	if(pItem)
	{
		iLuxItemType *pItemType = mpInventory->mvItemTypes[pItem->GetType()];

		pItemType->OnUse(pItem, mlIdx);

		//Update the pointer as item might be removed at usage
		pItem = GetItem();
		if(pItem)
			mpInventory->SetDescTextFromItem(pItem);
		else
			mpInventory->SetDescText(_W(""),_W(""));
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnMouseDoubleClick);

//-----------------------------------------------------------------------


bool cLuxInventory_Slot::OnMouseEnter(iWidget* apWidget, const cGuiMessageData& aData)
{
	cLuxInventory_Item *pPickedItem = mpInventory->GetPickedItem();
	cLuxInventory_Item *pItem = GetItem();

	mpInventory->SetCurrentWidget(apWidget);

	if(pItem && mpInventory->mbMessageActive==false)
	{
		///////////////////////////
		//Display combine info
		if(pPickedItem && pPickedItem != pItem)
		{
			tWString sCombine = kTranslate("Inventory", "Combine") + _W(" ") + kTranslate("Inventory",pPickedItem->GetGameNameEntry())  + _W(" ") +
								kTranslate("Inventory", "with") + _W(" ") + kTranslate("Inventory",pItem->GetGameNameEntry()) + _W(".");
			mpInventory->SetDescText(_W(""), sCombine);
		}
		///////////////////////////
		//Display item info
		else
		{
			mpInventory->SetDescTextFromItem(pItem);
		}
	}
	else if(pPickedItem==NULL)
	{
		mpInventory->SetDescText(_W(""), _W(""));
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnMouseEnter);

//-----------------------------------------------------------------------

bool cLuxInventory_Slot::OnMouseLeave(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mpInventory->GetCurrentWidget() == apWidget)
		mpInventory->SetCurrentWidget(NULL);

	cLuxInventory_Item *pItem = GetItem();
	if(pItem==NULL) return true;

	if(mpInventory->GetCurrentWidget() == NULL)
	{
		mpInventory->SetDescText(_W(""), _W(""));
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnMouseLeave);

//-----------------------------------------------------------------------

bool cLuxInventory_Slot::OnGetUINavFocus(iWidget* apWidget, const cGuiMessageData& aData)
{
	OnMouseEnter(apWidget, aData);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnGetUINavFocus);

bool cLuxInventory_Slot::OnLoseUINavFocus(iWidget* apWidget, const cGuiMessageData& aData)
{
	OnMouseLeave(apWidget, aData);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnLoseUINavFocus);

//-----------------------------------------------------------------------

bool cLuxInventory_Slot::OnUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if((aData.mlVal & eUIButton_Primary) ==0)
		return false;

	if(GetItem())
	{
		const cVector2f &vSize = apWidget->GetSize();
		cVector3f vItemPos = apWidget->GetGlobalPosition() + cVector3f((vSize - GetItem()->GetImage()->GetImageSize()))*0.5f;

		cVector3f vPos = apWidget->GetGlobalPosition();
		cVector3f vTargetPos = vPos + cVector3f(37, 50, 0);
		cVector2f vOffset = cVector2f(vItemPos.x, vItemPos.y) - cVector2f(vTargetPos.x, vTargetPos.y);
		mpInventory->SetPickedItem(GetItem(), vOffset);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnUIButtonPress);

bool cLuxInventory_Slot::OnUIButtonDoublePress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if((aData.mlVal & eUIButton_Primary) ==0)
			return false;

	return OnMouseDoubleClick(apWidget, aData);
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnUIButtonDoublePress);


bool cLuxInventory_Slot::OnUIButtonRelease(iWidget* apWidget, const cGuiMessageData& aData)
{
	if((aData.mlVal & eUIButton_Primary)==0)
		return true;

	cLuxInventory_Item *pPickedItem = mpInventory->GetPickedItem();
	cLuxInventory_Item *pItem = GetItem();

	if(pPickedItem && pItem && pItem == pPickedItem)
	{
		if(mpInventory->GetItemPickedAt() + 0.5f > gpBase->mpEngine->GetGameTime() && mpInventory->GetPickedItemMoved() == false)
		{
			return OnMouseDoubleClick(apWidget, aData);
		}
	}
	return OnMouseUp(apWidget, aData);
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnUIButtonRelease);

//-----------------------------------------------------------------------

bool cLuxInventory_Slot::OnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	float fTimeStep = aData.mfVal;

	if(mpInventory->GetCurrentWidget()==mpImage && mpInventory->mbMessageActive==false)
	{
		mfGlowAlpha += fTimeStep * 2.0f;
		if(mfGlowAlpha > 1.0f) mfGlowAlpha = 1.0f;
	}
	else
	{
		mfGlowAlpha -= fTimeStep * 1.5f;
		if(mfGlowAlpha < 0.0f) mfGlowAlpha = 0.0f;
	}


	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnUpdate);

//-----------------------------------------------------------------------

bool cLuxInventory_Slot::OnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	float fTimeStep = aData.mfVal;

	const cVector2f &vSize = apWidget->GetSize();

	///////////////////////////////////
	//Glow
	/*if(mfGlowAlpha >0)
	{
		mpInventory->mpGuiSet->DrawGfx(	mpGlowGfx, apWidget->GetGlobalPosition()+mpInventory->mvLayout_SlotMouseOverOffset,
										mpGlowGfx->GetActiveSize(),	cColor(mfGlowAlpha * mpInventory->mfAlpha,1) );
	}*/

	//////////////////////////////////
	//Item
	cLuxInventory_Item *pItem = GetItem();
	if(pItem && pItem != mpInventory->GetPickedItem())
	{
		cVector3f vItemPos = apWidget->GetGlobalPosition() + cVector3f((vSize-pItem->GetImage()->GetImageSize()))*0.5f;
		vItemPos.z += 0.1f;

		mpInventory->mpGuiSet->DrawGfx(	pItem->GetImage(), vItemPos,-1,cColor(1,mpInventory->mfAlpha) );

		/////////////////////////////////////////////////////
		// Draw "outline" - TODO: this looks really bad right now
		if(mfGlowAlpha > 0)
		{
			float fMin = mpInventory->mfLayout_SlotPulsatingMin;
			float fAmp = mpInventory->mfLayout_SlotPulsatingAmp;
			float fFreq = mpInventory->mfLayout_SlotPulsatingFreq;

			float fPulseAlpha = fMin + (1.0f-fMin)*(cos(mpInventory->mfTimer*fFreq)*0.5+0.5f);
			
			for(int i=0; i<2; ++i)
				mpInventory->mpGuiSet->DrawGfx(pItem->GetImage(), vItemPos+cVector3f(0,0,0.1f),-1, cColor(fPulseAlpha*mfGlowAlpha*mpInventory->mfAlpha, 1), eGuiMaterial_Additive);
		}

		///////////////
		//Draw count
		iLuxItemType *pType = mpInventory->mvItemTypes[pItem->GetType()];
		if(pType->HasCount())
		{
			cVector3f vPos = apWidget->GetGlobalPosition() + cVector3f(vSize.x,0,0) + mpInventory->mvLayout_ItemCountOffset;

			mpInventory->mpGuiSet->DrawFont(mpInventory->mpFontDefault, vPos,mpInventory->mvLayout_FontSize_ItemCount,
											cColor(1,mpInventory->mfAlpha),eFontAlign_Center, eGuiMaterial_FontNormal,
											_W("x%d"), pItem->GetCount());
		}
	}
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory_Slot, OnDraw);

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxInventory::OnClearFonts()
{

}

void cLuxInventory::LoadFonts()
{
	tString sFontDefaultFile = gpBase->mpMenuCfg->GetString("Inventory","DefaultFont","");
	mpFontDefault = LoadFont(sFontDefaultFile);

	tString sFontHeaderFile = gpBase->mpMenuCfg->GetString("Inventory","HeaderFont","");
	mpFontHeader = LoadFont(sFontHeaderFile);
}

void cLuxInventory::OnStart()
{
	
}

//-----------------------------------------------------------------------

void cLuxInventory::Reset()
{
	///////////////////////////////
	//Reset variables
	mfAlpha =0;	

	mbActive = false;
	mbDisabled = false;

	mbExitToJournal = false;
	mbEnterFromJournal = false;

	mbMessageActive = false;

	mpEquippedItem = NULL;
	
	ResetSessionVars();

	///////////////////////////////
	//Reset data
	if(mpScript) gpBase->mpEngine->GetResources()->GetScriptManager()->Destroy(mpScript);
	mpScript = NULL;

	STLDeleteAll(mvItems);
	STLDeleteAll(mlstCombineCallbacks);
}

//-----------------------------------------------------------------------

void cLuxInventory::OnGameStart()
{
	LoadScript();
	
	RunScript("OnGameStart()");
}

//-----------------------------------------------------------------------

void cLuxInventory::Update(float afTimeStep)
{
	/////////////////////////////
	// Update Picked item
	// Simple way to make sure the picked item is dropped.
	if(gpBase->mpEngine->GetInput()->IsTriggerd(eLuxAction_LeftClick)==false && 
		gpBase->mpEngine->GetInput()->IsTriggerd(eLuxAction_UIPrimary)==false)
	{
		mpPickedItem = NULL;
	}

	/////////////////////////////
	/// Update main alpha
	if(mbActive)
	{
		if(mfAlpha < 1)
		{
			mfAlpha += (1.0f/mfFadeInTime) * afTimeStep;			
			if(mfAlpha > 1)
			{
				mfAlpha =1;
				mbEnterFromJournal = false;
			}
		}
	}
	else
	{
		mfAlpha -= (1.0f/mfFadeOutTime) * afTimeStep;
		if(mfAlpha < 0)
		{
			mfAlpha =0;
			if(mbExitToJournal)
			{
				gpBase->mpJournal->SetOpenedFromInventory(true);
				gpBase->mpEngine->GetUpdater()->SetContainer("Journal");
			}
			else
			{
				gpBase->mpEngine->GetUpdater()->SetContainer("Default");
				gpBase->mpInputHandler->ChangeState(eLuxInputState_Game);
			}
		}
	}

	/////////////////////////////
	/// Update misc alpha faders
	for(int i=0; i<eLuxInventoryFader_LastEnum; ++i)
	{
		mvAlphaFader[i].Update(afTimeStep);
	}

	mfTimer += afTimeStep;

	/////////////////////////////
	/// Update old desc text alpha
	if(mfOldDescTextAlpha > 0)
	{
		mfOldDescTextAlpha -= afTimeStep * 2.2f;
		if(mfOldDescTextAlpha < 0)
		{
			mfOldDescTextAlpha =0;
		}
	}

	/////////////////////////////
	/// Update message text life and alpha
	if(mbMessageActive) // mfMessageTextLife>0
	{
		mfMessageTextAlpha += afTimeStep * 3.5f;
		if(mfMessageTextAlpha > 1) mfMessageTextAlpha =1;

		//mfMessageTextLife -= afTimeStep;
	}
	if(mbMessageActive==false && mfMessageTextAlpha > 0) // mfMessageTextLife <=0
	{
		mfMessageTextAlpha -= afTimeStep * 1.5f;
		if(mfMessageTextAlpha < 0) mfMessageTextAlpha =0;
	}
	


	/////////////////////////////
	/// Update image widget alpha
	for(size_t i=0; i<mvImageWidgets.size(); ++i)
	{
		cColor col = mvImageWidgets[i]->GetColorMul();
		col.a = mfAlpha;
        mvImageWidgets[i]->SetColorMul(col);
	}

	////////////////////////
	//Update hints and effects
	gpBase->mpHintHandler->UpdateHintText(afTimeStep);
	gpBase->mpEffectHandler->GetSanityGainFlash()->Update(afTimeStep);
}



//-----------------------------------------------------------------------

void cLuxInventory::OnEnterContainer(const tString& asOldContainer)
{
	////////////////////////
	//If player has use item state, change that
	if(gpBase->mpPlayer->GetCurrentState() == eLuxPlayerState_UseItem)
	{	
		gpBase->mpPlayer->ChangeState(eLuxPlayerState_Normal);
	}

	////////////////////////
	//Create the background
	CreateBackground();
	
	////////////////////////////
	//Set up states and viewport
	mbActive = true;

	gpBase->mpInputHandler->ChangeState(eLuxInputState_Inventory);
	
	mpViewport->SetActive(true);
	mpViewport->SetVisible(true);
	mpGuiSet->SetActive(true);

#ifdef USE_GAMEPAD
	if(gpBase->mpInputHandler->IsGamepadPresent() == false)
	{
		mpGuiSet->SetDrawMouse(true);
		mpGuiSet->SetMouseMovementEnabled(true);
	}
	else if(gpBase->mpInputHandler->IsGamepadPresent())
	{
		mpGuiSet->SetDrawMouse(false);
		mpGuiSet->SetMouseMovementEnabled(false);
	}
		mpGuiSet->SetDrawFocus(gpBase->mpInputHandler->IsGamepadPresent());
#else
	mpGuiSet->SetDrawMouse(true);
	
#endif


	mpGuiSet->ResetMouseOver();
	mpGui->SetFocus(mpGuiSet);

	gpBase->mpMapHandler->ResumeSoundsAndMusic();

	for(int i=0; i<eLuxInventoryFader_LastEnum; ++i) mvAlphaFader[i].Reset();


	/////////////////////
	//Effects
	gpBase->mpMapHandler->GetCurrentMap()->GetPhysicsWorld()->FadeoutAllLoopSounds(1.0f);
	gpBase->mpPlayer->StopTerrorSound();

	///////////////////////////
	//Pause voices and turn down volume on world sounds
	if(mbEnterFromJournal == false)
	{
		gpBase->mpEffectHandler->GetPlayVoice()->PauseCurrentVoices();

		cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
		pSoundHandler->FadeGlobalVolume(0.5f, 0.5f,eSoundEntryType_World,eLuxGlobalVolumeType_GameMenu, false);
	}
	
	/////////////////////
	//Reset variables
	ResetSessionVars();

	if(mpLastCurrentWidget)
	{
		SetCurrentWidget(mpLastCurrentWidget);
	}
	else
	{
		mpLastCurrentWidget = mpHealthWidget;
		SetCurrentWidget(mpLastCurrentWidget);
	}
#if USE_GAMEPAD
	if(mpLastCurrentWidget && gpBase->mpInputHandler->IsGamepadPresent())
	{
		mpLastCurrentWidget->ProcessMessage(eGuiMessage_GetUINavFocus, cGuiMessageData());
	}
#endif
}

void cLuxInventory::OnLeaveContainer(const tString& asNewContainer)
{
	///////////////////////////
	//Pause voices and turn down volume on world sounds
	if(mbExitToJournal == false)
	{
		gpBase->mpEffectHandler->GetPlayVoice()->UnpauseCurrentVoices();

		cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
		pSoundHandler->FadeGlobalVolume(1.0f, 0.5f,eSoundEntryType_World,eLuxGlobalVolumeType_GameMenu, false);
	}

	////////////////////////////
	//Set up states
	mpViewport->SetActive(false);
	mpViewport->SetVisible(false);
	mpGuiSet->SetActive(false);

	mbExitToJournal = false;
	mbEnterFromJournal = false;

	mpLastCurrentWidget = mpCurrentWidget;

	DestroyBackground();
}

//-----------------------------------------------------------------------

static int StatusToIndex(float afX)
{
	if(afX < 25.0f) return 0;
	if(afX < 50.0f) return 1;
	if(afX < 75.0f) return 2;
	return 3;
}

void cLuxInventory::OnDraw(float afFrameTime)
{
	////////////////////////
	//Draw background
	if(mpScreenGfx && mfAlpha<1) 
		mpGuiSet->DrawGfx(mpScreenGfx,mvGuiSetStartPos+cVector3f(0,0,0),mvGuiSetSize);

	if(mpScreenBgGfx)
		mpGuiSet->DrawGfx(mpScreenBgGfx,mvGuiSetStartPos+cVector3f(0,0,0.2f),mvGuiSetSize,cColor(1, mfAlpha));

	//////////////////////////////////
	//Fade
	if(mbExitToJournal || mbEnterFromJournal)
	{
		mpGuiSet->DrawGfx(mpWhiteGfx,mvGuiSetStartPos+cVector3f(0,0,20),mvGuiSetSize,cColor(0, 1.0f - mfAlpha*mfAlpha));
	}

	////////////////////////
	//Picked item
	if(mpPickedItem)
	{
		cVector3f vPickedItemPos = mvPickedItemOffset;
		if(gpBase->mpEngine->GetInput()->IsTriggerd(eLuxAction_LeftClick))
			vPickedItemPos += mpGuiSet->GetMousePos();
		else
			vPickedItemPos += mpCurrentWidget->GetGlobalPosition() + mpCurrentWidget->GetSize()*0.5f;

		vPickedItemPos.z = 40.0f;		
		mpGuiSet->DrawGfx(mpPickedItem->GetImage(),vPickedItemPos,-1,cColor(1, mfAlpha*0.8f));
	}

	////////////////////////
	//Player properties
	/*
	cVector3f vPlayerDataStart = cVector3f(10,300,1);
	float fPlayerDataXAdd = 140;
    
	mpGuiSet->DrawFont(	mpFontDefault, vPlayerDataStart,20,cColor(1,mfAlpha),_W("%ls"), kTranslate("Inventory", "LampOil:").c_str());
	mpGuiSet->DrawFont(	mpFontDefault, vPlayerDataStart+cVector3f(fPlayerDataXAdd,0,0),20,cColor(1,mfAlpha),_W("%.0f%%"), gpBase->mpPlayer->GetLampOil());
	vPlayerDataStart.y+= 22;

	mpGuiSet->DrawFont(	mpFontDefault, vPlayerDataStart,20,cColor(1,mfAlpha),_W("%ls"), kTranslate("Inventory", "Thalers:").c_str());
	mpGuiSet->DrawFont(	mpFontDefault, vPlayerDataStart+cVector3f(fPlayerDataXAdd,0,0),20,cColor(1,mfAlpha),_W("%d"), gpBase->mpPlayer->GetCoins());
	vPlayerDataStart.y+= 22;

	mpGuiSet->DrawFont(	mpFontDefault, vPlayerDataStart,20,cColor(1,mfAlpha),_W("%ls"), kTranslate("Inventory", "Tinderboxes:").c_str());
	mpGuiSet->DrawFont(	mpFontDefault, vPlayerDataStart+cVector3f(fPlayerDataXAdd,0,0),20,cColor(1,mfAlpha),_W("%d"), gpBase->mpPlayer->GetTinderboxes());
	vPlayerDataStart.y+= 22;
	*/

		
	////////////////////////
	//Draw Message text
	if(mfMessageTextAlpha > 0)
	{
		if(msMessageText != _W(""))
		{
			float fAlpha = mfMessageTextAlpha*mfAlpha;
			mpGuiSet->DrawFont(msMessageText, mpFontDefault, mvLayout_PopUpMessageCenter,mvLayout_FontSize_Message, 
								cColor(1,fAlpha),eFontAlign_Center);
			
			mpGuiSet->DrawGfx(	mpWhiteGfx, cVector3f(mvGuiSetStartPos.x, mvLayout_PopUpMessageCenter.y - 20, mvLayout_PopUpMessageCenter.z-0.1f),
								cVector2f(mvGuiSetSize.x, mvLayout_FontSize_Message.y+20*2),cColor(1.0f - fAlpha*0.8f), eGuiMaterial_Modulative );
		}
	}				

	////////////////////////
	//Draw text
	const cVector3f& vHeaderCenter = mvLayout_ItemHeaderCenter;
    if(msHeaderText != _W(""))
		mpGuiSet->DrawFont(msHeaderText, mpFontHeader, vHeaderCenter,mvLayout_FontSize_Header,cColor(1,mfAlpha),eFontAlign_Center);
	
	if(msNormalText != _W(""))
		DrawDescTextRow(msNormalText, cColor(1,1));

	////////////////////////
	//Draw frame around text
	{
		// Set frame start pos using corner size, padding and frame size
		cVector3f vTextFramePos = vHeaderCenter - 
			cVector3f(mvLayout_ItemTextFrameSize.x, 0, 0)*0.5f - 
			cVector3f(mpFrameGenericCorners[0]->GetActiveSize()) -
			cVector3f(mvLayout_ItemTextFrameHPadding.x, mvLayout_ItemTextFrameVPadding.y, 0) + 
			mvLayout_ItemTextFrameOffset;

		DrawFrame(mpFrameGenericCorners, mpFrameGenericBorders, 
					vTextFramePos, mvLayout_ItemTextFrameSize);
	}

    
	////////////////////////
	//Draw old text
	if(mfOldDescTextAlpha > 0)
	{
		if(msOldHeaderText != _W(""))
			mpGuiSet->DrawFont(msOldHeaderText, mpFontHeader, vHeaderCenter,mvLayout_FontSize_Header,cColor(1,mfAlpha*mfOldDescTextAlpha),eFontAlign_Center);

		if(msOldNormalText != _W(""))
			DrawDescTextRow(msOldNormalText, cColor(1,mfOldDescTextAlpha));
	}	
	
	////////////////////////
	//Draw extra effects and hints
	gpBase->mpHintHandler->DrawHintText(afFrameTime, mpGuiSet);
	gpBase->mpEffectHandler->GetSanityGainFlash()->DrawFlash(mpGuiSet, afFrameTime);
}

//-----------------------------------------------------------------------

void cLuxInventory::ExitPressed()
{
	if(mbActive==false) return;

	//////////////////////////////////
	// Message is active, remove it
	if(mbMessageActive)
	{
		mbMessageActive = false;
	}
	///////////////////////////
	// Exit the inventory
	else
	{
		gpBase->mpInputHandler->ChangeState(eLuxInputState_Null);

		mpGuiSet->SetDrawMouse(false);

		mpGui->SetFocus(NULL);
		
		mbActive = false;
		mbEnterFromJournal = false;
	}
}

//-----------------------------------------------------------------------

void cLuxInventory::LoadScript()
{
	/////////////////////
	// Destroy old
	if(mpScript)
	{
		gpBase->mpEngine->GetResources()->GetScriptManager()->Destroy(mpScript);
		mpScript = NULL;
	}

	/////////////////////
	// Load script
	tString sFile = gpBase->mpMapHandler->GetMapFolder() + "inventory.hps";
	mpScript  = gpBase->mpEngine->GetResources()->GetScriptManager()->CreateScript(sFile);
	if(mpScript==NULL)
	{
		Error("Inventory script '%s' not found!\n", sFile.c_str());
	}
}

//-----------------------------------------------------------------------


cLuxInventory_Item * cLuxInventory::AddItem(const tString& asName, eLuxItemType aType,
											const tString& asSubTypeName, const tString& asImageName,
											float afAmount, const tString& asVal, const tString& asExtraVal,
											bool* apRemoveItemProp)
{
	////////////
	// First Init
	iLuxItemType *pItemType = mvItemTypes[aType];

	if(apRemoveItemProp) *apRemoveItemProp = false;

	/////////////
	// Tinderbox achievemtn
	if (aType == eLuxItemType_Tinderbox)
	{
		cLuxScriptVar* pVar = gpBase->mpGlobalDataHandler->GetVar("TinderboxesCollected");
		if (pVar == NULL)
		{
			Error("Couldn't find Global var '%s'\n", asName.c_str());
			//return;
		}
		tString sVal = pVar->msVal;
		int intVal = 0;
		if (sVal != "")
		{
			intVal = cString::ToInt(sVal.c_str(), 0);
			intVal += 1;
		}

		pVar->msVal = cString::ToString(intVal);

		if (intVal >= 150)
		{
			gpBase->mpAchievementHandler->UnlockAchievement(eLuxAchievement_Illuminatus);
		}
	}

	////////////
	// Check if has max amount
	if(pItemType->GetHasMaxAmount())
	{
		gpBase->mpMessageHandler->SetMessage(kTranslate("Inventory", "TooManyItemsOfSort"),0); 
		return NULL;
	}

	////////////
	// If item type has count then check for 
	if(pItemType->HasCount())
	{
		cLuxInventory_Item *pItem = GetItemFromSubType(asSubTypeName);
		if(pItem)
		{
			/////////////////////////////////
			//Check if there are too many
			if(pItem->GetCount() >= pItemType->GetMaxCount())
			{
				gpBase->mpMessageHandler->SetMessage(kTranslate("Inventory", "TooManyItemsOfSort"),0); 
				return NULL;
			}
			else
			{
				pItem->AddCount(1);
				
				if(apRemoveItemProp) *apRemoveItemProp = true;
				return pItem;
			}
		}
	}
	
	////////////
	// Create item and its data
	cLuxInventory_Item *pItem = hplNew( cLuxInventory_Item, (asName,aType, asSubTypeName,this) );
	
	pItem->SetGameNameEntry("ItemName_"+asSubTypeName);
	pItem->SetGameDescEntry("ItemDesc_"+asSubTypeName);
	pItem->SetAmount(afAmount);

	tString sFullImageName = "graphics/item/"+asImageName;
	pItem->SetImageName(sFullImageName);
	pItem->SetStringVal(asVal);
	pItem->SetExtraStringVal(asExtraVal);

	if (asVal == "L02_Shipment")
	{
		gpBase->mpAchievementHandler->UnlockAchievement(eLuxAchievement_StillAlive);
	}	

	//Log("Loaded image %d for '%s'\n", pImage, asSubTypeName.c_str());
	//Log("  Image: %d Offset: (%s) Size: (%s)\n", pImage->GetImage(0), pImage->GetOffset().ToString().c_str(), 
	//												pImage->GetActiveSize().ToString().c_str());

	//////////////////////////
	//Check if something else should be done with the item (like adding to notebook).
	if(pItemType->BeforeAddItem(pItem))
	{
		if(apRemoveItemProp) *apRemoveItemProp = true;
		hplDelete(pItem);
		return NULL;
	}

	////////////////////////
	//Load graphics for item
	cGuiGfxElement *pImage = mpGui->CreateGfxImage(sFullImageName,eGuiMaterial_Alpha);
	if(pImage ==NULL)
	{
		Error("Could not create image '%s'! Not adding item '%s'!\n", asImageName.c_str(), asName.c_str());
		hplDelete(pItem);
		return NULL;
	}
	pItem->SetImage(pImage);

	////////////
	// Add item to inventory
	if(apRemoveItemProp) *apRemoveItemProp = true;
	mvItems.push_back(pItem);  
	return pItem;
}

//-----------------------------------------------------------------------

void cLuxInventory::RemoveItem(const tString& asName)
{
	std::vector<cLuxInventory_Item*>::iterator it = mvItems.begin();
	for(; it != mvItems.end(); ++it)
	{
		cLuxInventory_Item *pItem = *it;
		if(pItem->GetName() == asName)
		{
			if(mpPickedItem == pItem) mpPickedItem = NULL;
			if(mpEquippedItem == pItem) mpEquippedItem = NULL;

			hplDelete(pItem);
			mvItems.erase(it);
			return;
		}

	}
	Warning("Item '%s' could not be found when trying to remove it!\n", asName.c_str());
}

void cLuxInventory::RemoveItem(cLuxInventory_Item *apItem)
{
	iLuxItemType *pType = mvItemTypes[apItem->GetType()];

    if(pType->HasCount())
	{
		apItem->AddCount(-1);
		if(apItem->GetCount()>0) return;
	}

	if(mpPickedItem == apItem) mpPickedItem = NULL;
	if(mpEquippedItem == apItem) mpEquippedItem = NULL;

	std::vector<cLuxInventory_Item*>::iterator it = mvItems.begin();
	for(; it != mvItems.end(); ++it)
	{
		cLuxInventory_Item *pItem = *it;
		if(pItem == apItem)
		{
			hplDelete(pItem);
			mvItems.erase(it);
			return;
		}
	}
}

//-----------------------------------------------------------------------

void cLuxInventory::RemoveItemOfType(eLuxItemType aType)
{
	for(size_t i=0; i<mvItems.size(); ++i)
	{
		cLuxInventory_Item *pItem = mvItems[i];
        if(pItem->GetType() == aType)
		{
			RemoveItem(pItem);
			return;
		}
	}
}

//-----------------------------------------------------------------------

int cLuxInventory::GetItemIndex(cLuxInventory_Item *apItem)
{
	for(size_t i=0; i<mvItems.size(); ++i)
	{
		if(mvItems[i] == apItem) return (int)i;
	}
	return -1;
}

//-----------------------------------------------------------------------

cLuxInventory_Item* cLuxInventory::GetItem(const tString& asName)
{
	std::vector<cLuxInventory_Item*>::iterator it = mvItems.begin();
	for(; it != mvItems.end(); ++it)
	{
		cLuxInventory_Item *pItem = *it;
		if(pItem->GetName() == asName) return pItem;
	}
	return NULL;
}

//-----------------------------------------------------------------------

cLuxInventory_Item* cLuxInventory::GetItemFromSubType(const tString& asSubType)
{
	std::vector<cLuxInventory_Item*>::iterator it = mvItems.begin();
	for(; it != mvItems.end(); ++it)
	{
		cLuxInventory_Item *pItem = *it;
		if(pItem->GetSubType() == asSubType) return pItem;
	}
	return NULL;
}

//-----------------------------------------------------------------------

bool cLuxInventory::HasItemOfType(eLuxItemType aType)
{
	std::vector<cLuxInventory_Item*>::iterator it = mvItems.begin();
	for(; it != mvItems.end(); ++it)
	{
		cLuxInventory_Item *pItem = *it;
		if(pItem->GetType() == aType) return true;
	}
	return false;
}

//-----------------------------------------------------------------------

void cLuxInventory::AddCombineCallback(	const tString& asName,
										const tString& asItemA, const tString& asItemB,
										const tString& asFunction, bool abAutoDestroy)
{
	cLuxCombineItemsCallback *pCombCallback = hplNew( cLuxCombineItemsCallback, () );
    
	pCombCallback->msName = asName;
	pCombCallback->msItemA = asItemA;
	pCombCallback->msItemB = asItemB;
	pCombCallback->msFunction = asFunction;
	pCombCallback->mbAutoDestroy = abAutoDestroy;

	mlstCombineCallbacks.push_back(pCombCallback);
}

void cLuxInventory::RemoveCombineCallback(const tString& asName)
{
	tLuxCombineItemsCallbackListIt it = mlstCombineCallbacks.begin(); 
	for(; it != mlstCombineCallbacks.begin(); ++it)
	{
		cLuxCombineItemsCallback *pCombCallback = *it;
        if(pCombCallback->msName == asName)
		{
			hplDelete(pCombCallback);
			mlstCombineCallbacks.erase(it);
			return;
		}
	}

	Warning("Inventory combination callback '%s' could not be found!\n", asName.c_str());
}

cLuxCombineItemsCallback*  cLuxInventory::GetCombineCallback(const tString& asItemA, const tString& asItemB)
{
	tLuxCombineItemsCallbackListIt it = mlstCombineCallbacks.begin(); 
	for(; it != mlstCombineCallbacks.end(); ++it)
	{
		cLuxCombineItemsCallback *pCombCallback = *it;
		
		if( (pCombCallback->msItemA == asItemA && pCombCallback->msItemB == asItemB) ||
			(pCombCallback->msItemA == asItemB && pCombCallback->msItemB == asItemA))
		{
			return pCombCallback;
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------

void cLuxInventory::RunScript(const tString& asCommand)
{
	if(mpScript==NULL) return;

    mpScript->Run(asCommand);
}

bool cLuxInventory::RecompileScript(tString *apOutput)
{
	if(mpScript)
		gpBase->mpEngine->GetResources()->GetScriptManager()->Destroy(mpScript);
	
	tString sFile = gpBase->mpMapHandler->GetMapFolder() + "inventory.hps";
	mpScript = gpBase->mpEngine->GetResources()->GetScriptManager()->CreateScript(sFile, apOutput);

	return mpScript != NULL;
}

//-----------------------------------------------------------------------

void cLuxInventory::SetDescTextFromItem(cLuxInventory_Item *apItem)
{
	if(apItem)
	{
		iLuxItemType *pItemType = GetItemTypeData(apItem->GetType());

		SetDescText(kTranslate("Inventory",apItem->GetGameNameEntry())+pItemType->GetDisplayedNameAdd(apItem),
					kTranslate("Inventory",apItem->GetGameDescEntry()) );
	}
	else
	{
		SetDescText(_W(""), _W(""));
	}
}

//-----------------------------------------------------------------------

void cLuxInventory::SetDescText(const tWString &asHeader, const tWString &asNormal)
{
	if(asHeader == _W("") && asNormal == _W(""))
	{
		msOldHeaderText = msHeaderText;
		msOldNormalText = msNormalText;
		mfOldDescTextAlpha = 1.0f;
	}
	else
	{
		mfOldDescTextAlpha = 0.0f;
	}
#ifdef USE_GAMEPAD
	msHeaderText = ParseStringForGamepadIconsRemoval(gpBase->mpHelpFuncs->ParseString(asHeader));
	msNormalText = ParseStringForGamepadIconsRemoval(gpBase->mpHelpFuncs->ParseString(asNormal));
#else
	msHeaderText = gpBase->mpHelpFuncs->ParseString(asHeader);
	msNormalText = gpBase->mpHelpFuncs->ParseString(asNormal);
#endif
}

void cLuxInventory::OpenJournal()
{
	if(mbActive==false) return;

	ExitPressed();
	mbExitToJournal = true;
}

//-----------------------------------------------------------------------

void cLuxInventory::SetMessageText(const tWString &asText, float afLifeTime)
{
	msMessageText = gpBase->mpHelpFuncs->ParseString(asText);
	mfMessageTextLife = afLifeTime <=0 ? gpBase->mpHelpFuncs->GetStringDuration(msMessageText) : afLifeTime;
	mfMessageTextAlpha = 0.0f;
	mbMessageActive = true;
}

//-----------------------------------------------------------------------

void cLuxInventory::SetPickedItem(cLuxInventory_Item *apItem, const cVector2f& avOffset)
{ 
	mfPickedUpAt = gpBase->mpEngine->GetGameTime();
	mbPickedObjectMoved = false;
	mpPickedItem = apItem;
	mvPickedItemOffset = avOffset;
}

//-----------------------------------------------------------------------

void cLuxInventory::SetCurrentWidget(iWidget *apWidget)
{ 
	if(mfPickedUpAt + 1.0f/60.0f < gpBase->mpEngine->GetGameTime())
	{
		mbPickedObjectMoved = true;
	}

	mpCurrentWidget = apWidget;
	mpGuiSet->SetFocusedWidget(apWidget);
}

//-----------------------------------------------------------------------

void cLuxInventory::SetEquippedHandItem(cLuxInventory_Item *apItem)
{
	if(mpEquippedItem == apItem) return;

	mpEquippedItem = apItem;
}

//-----------------------------------------------------------------------

eLuxItemType cLuxInventory::GetItemTypeFromString(const tString& asType)
{
	tString sLowType = cString::ToLowerCase(asType);

	for(size_t i=0; i<mvItemTypes.size(); ++i)
	{
		iLuxItemType *pItemType = mvItemTypes[i];
        if(cString::ToLowerCase(pItemType->GetName()) == sLowType)
		{
			return pItemType->GetType();
		}
	}

	Error("Item type '%s' does not exist!\n", asType.c_str());

    return eLuxItemType_Puzzle;
}

//-----------------------------------------------------------------------
#ifdef USE_GAMEPAD
void cLuxInventory::AppDeviceWasPlugged()
{
	mpGuiSet->SetDrawFocus(gpBase->mpInputHandler->IsGamepadPresent());
}

//-----------------------------------------------------------------------

void cLuxInventory::AppDeviceWasRemoved()
{
	AppDeviceWasPlugged();
}
#endif
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxInventory::DrawDescTextRow(const tWString& asText, const cColor& aCol)
{
	cColor finalCol = aCol;
	finalCol.a *= mfAlpha;

	const cVector3f& vMessageBodyCenter = mvLayout_ItemDescCenter;
	tWStringVec vRows;
	float fStartY = vMessageBodyCenter.y+30;
	float fHalfWidth = mfLayout_ItemDescTextWidth*0.5f;
	cVector2f vFontSize = mvLayout_FontSize_Description;
	mpFontDefault->GetWordWrapRows(mfLayout_ItemDescTextWidth,vFontSize.y+1,vFontSize.y,asText, &vRows);
	for(size_t i=0; i<vRows.size(); ++i)
	{
		float fY = fStartY + (float)i * (vFontSize.y+1);
		//mpGuiSet->DrawFont(vRows[i], mpFontDefault, cVector3f(vMessageBodyCenter.x,fY ,1),vFontSize.y,finalCol,eFontAlign_Center);
		mpGuiSet->DrawFont(vRows[i], mpFontDefault, cVector3f(vMessageBodyCenter.x-fHalfWidth, fY, 1), vFontSize.y, finalCol, eFontAlign_Left);
	}
}

//-----------------------------------------------------------------------

void cLuxInventory::ResetSessionVars()
{
	mfOldDescTextAlpha =0;

	mbMessageActive = false;

	mfOilMovementT =0;

	msHeaderText = _W("");
	msNormalText = _W("");

	mpPickedItem = NULL;
	mpCurrentWidget = NULL;

	msMessageText = _W("");
	mfMessageTextLife =0;
	mfMessageTextAlpha =0;
}

static bool CheckItemTypeCombo(cLuxInventory_Item *apItemA, cLuxInventory_Item *apItemB, eLuxItemType aTypeA, eLuxItemType aTypeB)
{
	return	(apItemA->GetType() == aTypeA && apItemB->GetType() == aTypeB) ||
			(apItemA->GetType() == aTypeB && apItemB->GetType() == aTypeA);
}

bool cLuxInventory::CheckSpecialCombineAction(cLuxInventory_Item *apItemA, cLuxInventory_Item *apItemB, int alSlotIndex)
{
	/////////////////////////////
	//Combine lantern and oil
	if(CheckItemTypeCombo(apItemA, apItemB, eLuxItemType_Lantern, eLuxItemType_LampOil))
	{
		cLuxInventory_Item *pOilItem = apItemA->GetType()==eLuxItemType_LampOil ? apItemA : apItemB;
        
		GetItemTypeData(eLuxItemType_LampOil)->OnUse(pOilItem, alSlotIndex);
		return true;
	}

	
	return false;
}

//-----------------------------------------------------------------------

void cLuxInventory::CreateBackground()
{
	CreateScreenTextures();
	RenderBackgroundImage();
}

//-----------------------------------------------------------------------

void cLuxInventory::CreateScreenTextures()
{
	iLowLevelGraphics *pLowGfx = mpGraphics->GetLowLevel();
	cVector3l vTexSize = pLowGfx->GetScreenSizeInt();
	vTexSize.z = 0;

	mpScreenTexture = mpGraphics->CreateTexture("Screen",eTextureType_Rect,eTextureUsage_RenderTarget);
	mpScreenTexture->CreateFromRawData(vTexSize,ePixelFormat_RGBA,NULL);
	mpScreenTexture->SetWrapSTR(eTextureWrap_ClampToEdge);

	mpScreenBgTexture = mpGraphics->CreateTexture("ScreenBlur",eTextureType_Rect,eTextureUsage_RenderTarget);
	mpScreenBgTexture->CreateFromRawData(vTexSize,ePixelFormat_RGBA,NULL);

	mpScreenGfx = mpGui->CreateGfxTexture(mpScreenTexture,false,eGuiMaterial_Diffuse);
	mpScreenBgGfx = mpGui->CreateGfxTexture(mpScreenBgTexture,false,eGuiMaterial_Alpha);
}

//-----------------------------------------------------------------------

void cLuxInventory::RenderBackgroundImage()
{
	iLowLevelGraphics *pLowGfx = mpGraphics->GetLowLevel();

	//////////////////////////////
	// Create frame buffers
	iFrameBuffer *pEffectBuffer  = mpGraphics->CreateFrameBuffer("InventoryEffectbuffer");
	pEffectBuffer->SetTexture2D(0,mpScreenBgTexture);
	pEffectBuffer->CompileAndValidate();
	
	//////////////////////////////
	// Render

	//Render scene again without gui.
	gpBase->mpHelpFuncs->RenderBackgroundScreen(false);
	
	//Set up main states
	pLowGfx->SetBlendActive(false);
	pLowGfx->SetDepthTestActive(false);
	pLowGfx->SetDepthWriteActive(false);

	pLowGfx->SetOrthoProjection(mvScreenSize,-1000,1000);
	pLowGfx->SetIdentityMatrix(eMatrix_ModelView);

	//Copy screen to screen texture
	pLowGfx->CopyFrameBufferToTexure(mpScreenTexture,0,pLowGfx->GetScreenSizeInt(),0);

	//Bind shader and draw
	mpEffectProgram->Bind();
	pLowGfx->SetCurrentFrameBuffer(pEffectBuffer);

	pLowGfx->SetTexture(0,mpScreenTexture);

	pLowGfx->DrawQuad(0,mvScreenSize,cVector2f(0, mvScreenSize.y),cVector2f(mvScreenSize.x,0),cColor(1,1));
	mpEffectProgram->UnBind();

	//Copy a copy of the full gui with all HUD!
	pLowGfx->SetCurrentFrameBuffer(NULL);
	pLowGfx->SetTexture(0,NULL);

	gpBase->mpHelpFuncs->RenderBackgroundScreen(true);
	pLowGfx->CopyFrameBufferToTexure(mpScreenTexture,0,pLowGfx->GetScreenSizeInt(),0);

	
	///////////////////////
	// Exit

	//Render states
	pLowGfx->SetTexture(0,NULL);
	pLowGfx->SetCurrentFrameBuffer(NULL);
	pLowGfx->SetDepthTestActive(true);

	//Flush the rendering
	pLowGfx->FlushRendering();
	pLowGfx->WaitAndFinishRendering();

	//Destroy data
	mpGraphics->DestroyFrameBuffer(pEffectBuffer);
}	

//-----------------------------------------------------------------------

void cLuxInventory::DestroyBackground()
{
	if(mpScreenGfx) mpGui->DestroyGfx(mpScreenGfx);
	if(mpScreenTexture) mpGraphics->DestroyTexture(mpScreenTexture);
	if(mpScreenBgGfx) mpGui->DestroyGfx(mpScreenBgGfx);
	if(mpScreenBgTexture) mpGraphics->DestroyTexture(mpScreenBgTexture);

	mpScreenGfx = NULL;
	mpScreenTexture = NULL;
	mpScreenBgTexture = NULL;
	mpScreenBgGfx = NULL;
}

//-----------------------------------------------------------------------

void cLuxInventory::CreateGui()
{
	cGui *pGui = gpBase->mpEngine->GetGui();
	cWidgetImage *pImage = NULL;

	cConfigFile *pMenuCfg = gpBase->mpMenuCfg;

	/////////////////////////////////
	// Load settings
	mvLayout_PopUpMessageCenter = pMenuCfg->GetVector3f("Inventory", "PopUpMessageCenter", 0);

	// Journal
	mvLayout_JournalCenter = pMenuCfg->GetVector3f("Inventory", "JournalCenter", 0);
	mvLayout_JournalLabelCenter = pMenuCfg->GetVector3f("Inventory", "JournalLabelCenter", 0);
	mvLayout_JournalFrameSize = pMenuCfg->GetVector2f("Inventory", "JournalFrameSize", -1);
	mvLayout_JournalFrameHPadding = pMenuCfg->GetVector2f("Inventory", "JournalFrameHPadding", 0);
	mvLayout_JournalFrameVPadding = pMenuCfg->GetVector2f("Inventory", "JournalFrameVPadding", 0);
	mvLayout_JournalFrameOffset = pMenuCfg->GetVector3f("Inventory", "JournalFrameOffset", 0);

	// Item slot
	msLayout_SlotGfx = pMenuCfg->GetString("Inventory", "SlotImage", "");
	mvLayout_SlotSize = pMenuCfg->GetVector2f("Inventory", "SlotSize", -1);
	mLayout_SlotColor = pMenuCfg->GetColor("Inventory", "SlotColor", cColor(1));

	msLayout_SlotMouseOverGfx = pMenuCfg->GetString("Inventory", "SlotMouseOverImage", "");
	mLayout_SlotMouseOverColor = pMenuCfg->GetColor("Inventory", "SlotMouseOverColor", cColor(1,1));
	mvLayout_SlotMouseOverOffset = pMenuCfg->GetVector3f("Inventory", "SlotMouseOverOffset", 0);

	mfLayout_SlotPulsatingMin = pMenuCfg->GetFloat("Inventory", "SlotPulsatingMin", 1.25f);
	mfLayout_SlotPulsatingAmp = pMenuCfg->GetFloat("Inventory", "SlotPulsatingAmp", 0.25f);
	mfLayout_SlotPulsatingFreq = pMenuCfg->GetFloat("Inventory", "SlotPulsatingFreq", 2);

	// Item slots
	mlLayout_SlotsRows = pMenuCfg->GetInt("Inventory", "SlotsRows", 3);
	mlLayout_SlotsColumns = pMenuCfg->GetInt("Inventory", "SlotsColumns", 6);
	mLayout_SlotsGridColor = pMenuCfg->GetColor("Inventory", "SlotsGridColor", cColor(0.3f,0.5f));
	mvLayout_SlotsStart = pMenuCfg->GetVector3f("Inventory", "SlotsStart", 0);
	mvLayout_SlotsSeparation = pMenuCfg->GetVector3f("Inventory", "SlotsSeparation", 0);
	mvLayout_SlotFrameSize = pMenuCfg->GetVector2f("Inventory", "SlotsFrameSize", -1);
	mvLayout_SlotFrameHPadding = pMenuCfg->GetVector2f("Inventory", "SlotsFrameHPadding", 0);
	mvLayout_SlotFrameVPadding = pMenuCfg->GetVector2f("Inventory", "SlotsFrameVPadding", 0);
	mvLayout_SlotFrameOffset = pMenuCfg->GetVector3f("Inventory", "SlotsFrameOffset", 0);
	mbLayout_DrawSlotsGrid = pMenuCfg->GetBool("Inventory", "SlotsDrawGrid", true);
	mbLayout_DrawSlotsFrame = pMenuCfg->GetBool("Inventory", "SlotsDrawFrame", true);
	mvLayout_SlotGridHExtension = pMenuCfg->GetVector2f("Inventory", "SlotsGridHExtension", 0);
	mvLayout_SlotGridVExtension = pMenuCfg->GetVector2f("Inventory", "SlotsGridVExtension", 0);
	mvLayout_SlotsGridOffset = pMenuCfg->GetVector3f("Inventory", "SlotsGridOffset", 0);
	
	// Health
	mvLayout_HealthCenter = pMenuCfg->GetVector3f("Inventory", "HealthCenter", 0);
	mvLayout_HealthLabelCenter = pMenuCfg->GetVector3f("Inventory", "HealthLabelCenter", 0);
	mvLayout_HealthFrameSize = pMenuCfg->GetVector2f("Inventory", "HealthFrameSize", -1);
	mvLayout_HealthFrameHPadding = pMenuCfg->GetVector2f("Inventory", "HealthFrameHPadding", 0);
	mvLayout_HealthFrameVPadding = pMenuCfg->GetVector2f("Inventory", "HealthFrameVPadding", 0);
	mvLayout_HealthFrameOffset = pMenuCfg->GetVector3f("Inventory", "HealthFrameOffset", 0);

	// Sanity
	mvLayout_SanityCenter = pMenuCfg->GetVector3f("Inventory", "SanityCenter", 0);
	mvLayout_SanityLabelCenter = pMenuCfg->GetVector3f("Inventory", "SanityLabelCenter", 0);
	mvLayout_SanityFrameSize = pMenuCfg->GetVector2f("Inventory", "SanityFrameSize", -1);
	mvLayout_SanityFrameHPadding = pMenuCfg->GetVector2f("Inventory", "SanityFrameHPadding", 0);
	mvLayout_SanityFrameVPadding = pMenuCfg->GetVector2f("Inventory", "SanityFrameVPadding", 0);
	mvLayout_SanityFrameOffset = pMenuCfg->GetVector3f("Inventory", "SanityFrameOffset", 0);

	// Tinderboxes
	mvLayout_TinderboxesCenter = pMenuCfg->GetVector3f("Inventory", "TinderboxesCenter", 0);
	mvLayout_TinderboxesLabelCenter = pMenuCfg->GetVector3f("Inventory", "TinderboxesLabelCenter", 0);
	mvLayout_TinderboxesFrameSize = pMenuCfg->GetVector2f("Inventory", "TinderboxesFrameSize", -1);
	mvLayout_TinderboxesFrameHPadding = pMenuCfg->GetVector2f("Inventory", "TinderboxesFrameHPadding", 0);
	mvLayout_TinderboxesFrameVPadding = pMenuCfg->GetVector2f("Inventory", "TinderboxesFrameVPadding", 0);
	mvLayout_TinderboxesFrameOffset = pMenuCfg->GetVector3f("Inventory", "TinderboxesFrameOffset", 0);

	// Oil
	mvLayout_OilCenter = pMenuCfg->GetVector3f("Inventory", "OilCenter", 0);
	mvLayout_OilLabelCenter = pMenuCfg->GetVector3f("Inventory", "OilLabelCenter", 0);
	mvLayout_OilFrameSize = pMenuCfg->GetVector2f("Inventory", "OilFrameSize", -1);
	mvLayout_OilFrameHPadding = pMenuCfg->GetVector2f("Inventory", "OilFrameHPadding", 0);
	mvLayout_OilFrameVPadding = pMenuCfg->GetVector2f("Inventory", "OilFrameVPadding", 0);
	mvLayout_OilFrameOffset = pMenuCfg->GetVector3f("Inventory", "OilFrameOffset", 0);

	for(int i=0; i<3; ++i)
	{
		mvOilBubbleSpeed[i] = 40.0f;
		mvOilBubblePos[i] = pMenuCfg->GetVector3f("Inventory", "OilCenter", 0) +	cVector3f(0,0,0.7f) + 
																					cMath::RandRectVector3f(cVector3f(-10,-10, 0), cVector3f(10,10, 0));
	}

	// Item text
	mvLayout_ItemHeaderCenter = pMenuCfg->GetVector3f("Inventory", "ItemHeaderCenter", 0);
	mvLayout_ItemDescCenter = pMenuCfg->GetVector3f("Inventory", "ItemDescCenter", 0);
	mvLayout_ItemTextFrameSize = pMenuCfg->GetVector2f("Inventory", "ItemTextFrameSize", -1);
	mvLayout_ItemTextFrameHPadding = pMenuCfg->GetVector2f("Inventory", "ItemTextFrameHPadding", 0);
	mvLayout_ItemTextFrameVPadding = pMenuCfg->GetVector2f("Inventory", "ItemTextFrameVPadding", 0);
	mvLayout_ItemTextFrameOffset = pMenuCfg->GetVector3f("Inventory", "ItemTextFrameOffset", 0);

	mfLayout_ItemDescTextWidth = pMenuCfg->GetFloat("Inventory", "ItemDescTextWidth", 400);

	if(mvLayout_ItemTextFrameSize==cVector2f(-1))
		mvLayout_ItemTextFrameSize = cVector2f(mfLayout_ItemDescTextWidth, mfLayout_ItemDescTextWidth*0.25f);

	mvLayout_ItemCountOffset = pMenuCfg->GetVector3f("Inventory", "ItemCountOffset", 0);

	// Font size
	mvLayout_FontSize_ItemCount = pMenuCfg->GetVector2f("Inventory", "FontSize_ItemCount", 0);
	mvLayout_FontSize_Label = pMenuCfg->GetVector2f("Inventory", "FontSize_Label", 0);
	mvLayout_FontSize_Header = pMenuCfg->GetVector2f("Inventory", "FontSize_Header", 0);
	mvLayout_FontSize_Description = pMenuCfg->GetVector2f("Inventory", "FontSize_Description", 0);
	mvLayout_FontSize_Message = pMenuCfg->GetVector2f("Inventory", "FontSize_Message", 0);
	

	/////////////////////////////////
	// Create Sanity and Health status
	tString sStatusSuffix[4] = {"25", "50", "75", "100"};
	float fGlowPulseMiddle[4] = { 0.5f, 0.55f, 0.65f, 0.7f };
	float fGlowPulseFreq[4] = { 1.5f, 1.4f, 1.2f, 1.0f };

    for(int i=0; i<4; ++i)
	{
		mpSanityStatus[i] = pGui->CreateGfxImage("inventory/inventory_sanity_"+sStatusSuffix[i]+".tga", eGuiMaterial_Alpha);
		mpSanityGlow[i] = pGui->CreateGfxImage("inventory/inventory_sanity_glow_"+sStatusSuffix[i]+".tga", eGuiMaterial_Alpha);

		mpHealthStatus[i] = pGui->CreateGfxImage("inventory/inventory_health_"+sStatusSuffix[i]+".tga", eGuiMaterial_Alpha);
		mpHealthGlow[i] = pGui->CreateGfxImage("inventory/inventory_health_glow_"+sStatusSuffix[i]+".tga", eGuiMaterial_Alpha);

		mvPulseMiddle.push_back(fGlowPulseMiddle[i]);
		mvPulseFreq.push_back(fGlowPulseFreq[i]);
	}

	/////////////////////////////////
	// Create Sanity widget
	pImage = mpGuiSet->CreateWidgetImage("", 0, -1, eGuiMaterial_Alpha, false);
	pImage->SetUserValue(eLuxInventoryFader_Sanity);
	pImage->AddCallback(eGuiMessage_OnDraw, this, kGuiCallback(SanityOnDraw));
	pImage->AddCallback(eGuiMessage_OnUpdate,this, kGuiCallback(SanityOnUpdate));
	pImage->AddCallback(eGuiMessage_MouseEnter,this, kGuiCallback(InventoryWidgetMouseEnter));
	pImage->AddCallback(eGuiMessage_MouseLeave,this, kGuiCallback(InventoryWidgetMouseEnter));
	pImage->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(UIButtonPress));
	pImage->SetGlobalUIInputListener(true);
	mvImageWidgets.push_back(pImage);
	iWidget* pSanity = mpSanityWidget = pImage;

	/////////////////////////////////
	// Create Health widget
	pImage = mpGuiSet->CreateWidgetImage("", 0, -1, eGuiMaterial_Alpha, false);
	pImage->SetUserValue(eLuxInventoryFader_Health);
	pImage->AddCallback(eGuiMessage_OnDraw, this, kGuiCallback(HealthOnDraw));
	pImage->AddCallback(eGuiMessage_OnUpdate,this, kGuiCallback(HealthOnUpdate));
	pImage->AddCallback(eGuiMessage_MouseEnter,this, kGuiCallback(InventoryWidgetMouseEnter));
	pImage->AddCallback(eGuiMessage_MouseLeave,this, kGuiCallback(InventoryWidgetMouseEnter));
	mvImageWidgets.push_back(pImage);
	iWidget* pHealth = mpHealthWidget = pImage;

	/////////////////////////////////
	// Create remaining oil display
	mpRemainingOilFG = pGui->CreateGfxImage("inventory/inventory_oil_fg.tga", eGuiMaterial_Alpha);
	for(int i=0; i<3; ++i)
		mpOilBubble[i] = pGui->CreateGfxImage("inventory/inventory_oil_bubble_"+cString::ToString(i+1)+".tga", eGuiMaterial_Alpha);
	mpOilLiquid = pGui->CreateGfxImage("inventory/inventory_oil_liquid.tga", eGuiMaterial_Alpha);

	pImage = mpGuiSet->CreateWidgetImage("inventory/inventory_oil_bg.tga", eGuiMaterial_Alpha);
	pImage->SetPosition(mvLayout_OilCenter-cVector2f(pImage->GetSize())*0.5f);
	pImage->SetUserValue(eLuxInventoryFader_Oil);
	pImage->AddCallback(eGuiMessage_OnDraw, this, kGuiCallback(OilOnDraw));
	pImage->AddCallback(eGuiMessage_OnUpdate,this, kGuiCallback(OilOnUpdate));
	pImage->AddCallback(eGuiMessage_MouseEnter,this, kGuiCallback(InventoryWidgetMouseEnter));
	pImage->AddCallback(eGuiMessage_MouseLeave,this, kGuiCallback(InventoryWidgetMouseEnter));
	pImage->AddCallback(eGuiMessage_MouseUp,this, kGuiCallback(OilOnMouseUp));
	mvImageWidgets.push_back(pImage);

	iWidget* pOil = mpOilWidget = pImage;

	/////////////////////////////////
	// Create tinderbox icon
	pImage = mpGuiSet->CreateWidgetImage("inventory/inventory_tinderboxes.tga", 0, -1, eGuiMaterial_Alpha, false);
	pImage->SetUserValue(eLuxInventoryFader_Tinderbox);
	pImage->SetPosition(mvLayout_TinderboxesCenter-cVector2f(pImage->GetSize())*0.5f);
	pImage->AddCallback(eGuiMessage_OnDraw, this, kGuiCallback(TinderboxOnDraw));
	pImage->AddCallback(eGuiMessage_OnUpdate,this, kGuiCallback(TinderboxOnUpdate));
	pImage->AddCallback(eGuiMessage_MouseEnter,this, kGuiCallback(InventoryWidgetMouseEnter));
	pImage->AddCallback(eGuiMessage_MouseLeave,this, kGuiCallback(InventoryWidgetMouseEnter));
	mvImageWidgets.push_back(pImage);

	iWidget* pTinderbox = mpTinderboxWidget= pImage;

	/////////////////////////////////
	// Equip slot
	/*pImage = mpGuiSet->CreateWidgetImage("inventory/inventory_equip_slot.tga", 0,-1,eGuiMaterial_Alpha,false,pSlotGroup);
	pImage->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(EquipSlotOnDraw));
	pImage->AddCallback(eGuiMessage_MouseUp,this, kGuiCallback(EquipSlotMouseUp));
	mvImageWidgets.push_back(pImage);*/
	
	/////////////////////////////////
	// Journal
	mpJournalMouseOverGfx = pGui->CreateGfxImage("inventory/inventory_journal_mouse_over.tga",eGuiMaterial_Alpha);

	pImage = mpGuiSet->CreateWidgetImage("inventory/inventory_journal.tga", 0,-1,eGuiMaterial_Alpha,false);
	pImage->SetPosition(mvLayout_JournalCenter-cVector2f(pImage->GetSize())*0.5f);
	pImage->SetUserValue(eLuxInventoryFader_Journal);
	pImage->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(JournalOnDraw));
	pImage->AddCallback(eGuiMessage_OnUpdate,this, kGuiCallback(JournalOnUpdate));
	pImage->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(JournalMouseDown));
	pImage->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(JournalUIButtonPress));
	pImage->AddCallback(eGuiMessage_MouseEnter,this, kGuiCallback(InventoryWidgetMouseEnter));
	pImage->AddCallback(eGuiMessage_MouseLeave,this, kGuiCallback(InventoryWidgetMouseEnter));
	mvImageWidgets.push_back(pImage);

	

	for(size_t i=0; i<mvImageWidgets.size(); ++i)
	{
		iWidget* pWidget = mvImageWidgets[i];

		
		pWidget->AddCallback(eGuiMessage_GetUINavFocus, this, kGuiCallback(InventoryWidgetOnGetUINavFocus));
		pWidget->AddCallback(eGuiMessage_LoseUINavFocus, this, kGuiCallback(InventoryWidgetOnLoseUINavFocus));
	}

	iWidget* pJournal = mpJournalWidget = pImage;

	/////////////////////////////////
	// Create Slot group
	const cVector3f& vSlotStart = mvLayout_SlotsStart;
    cWidgetDummy *pSlotGroup = mpGuiSet->CreateWidgetDummy(vSlotStart);
	pSlotGroup->AddCallback(eGuiMessage_OnDraw, this, kGuiCallback(ItemSlotsOnDraw));

	/////////////////////////////////
	// Create slots

	const cVector3f& vSlotSeparation = mvLayout_SlotsSeparation;
	for(int y=0; y<mlLayout_SlotsRows; ++y)
	for(int x=0; x<mlLayout_SlotsColumns; ++x)
	{
		pImage = mpGuiSet->CreateWidgetImage("", 0, -1,eGuiMaterial_Alpha,false,pSlotGroup);

        cLuxInventory_Slot *pSlot = hplNew( cLuxInventory_Slot, (this, pImage, (int)mvSlots.size()));

		cVector3f vPos = cVector3f( (float)x * (pImage->GetSize().x + vSlotSeparation.x), 
									(float)y * (pImage->GetSize().y + vSlotSeparation.y), 
									0);
		pImage->SetPosition(vPos);

		mvSlots.push_back(pSlot);
		mvImageWidgets.push_back(pImage);
	}

	mpLastCurrentWidget = mvSlots[0]->GetImageWidget();
	
	//////////////////////////////////////////////////////////////
	// Set up navigation
	mpGuiSet->SetDefaultFocusNavWidget(pHealth);
	pHealth->SetFocusNavigation(eUIArrow_Down, pSanity);
	pHealth->SetFocusNavigation(eUIArrow_Right, mvSlots[0+0]->GetImageWidget());
	pSanity->SetFocusNavigation(eUIArrow_Up, pHealth);
	pSanity->SetFocusNavigation(eUIArrow_Right, mvSlots[2*mlLayout_SlotsColumns+0]->GetImageWidget());
	pTinderbox->SetFocusNavigation(eUIArrow_Left, mvSlots[0+mlLayout_SlotsColumns-1]->GetImageWidget());
	pTinderbox->SetFocusNavigation(eUIArrow_Down, pOil);
	pOil->SetFocusNavigation(eUIArrow_Up, pTinderbox);
	pOil->SetFocusNavigation(eUIArrow_Left, mvSlots[1*mlLayout_SlotsColumns+mlLayout_SlotsColumns-1]->GetImageWidget());
	pOil->SetFocusNavigation(eUIArrow_Down, pJournal);
	pJournal->SetFocusNavigation(eUIArrow_Up, pOil);
	pJournal->SetFocusNavigation(eUIArrow_Left, mvSlots[2*mlLayout_SlotsColumns+mlLayout_SlotsColumns-1]->GetImageWidget());


	for(int y=0; y<mlLayout_SlotsRows; ++y)
	for(int x=0; x<mlLayout_SlotsColumns; ++x)
	{
		cLuxInventory_Slot *pSlot = mvSlots[y*mlLayout_SlotsColumns + x];
		iWidget* pSlotWidget = pSlot->GetImageWidget();

		iWidget* pWidgetAtLeft = NULL;
		iWidget* pWidgetAtRight = NULL;
		iWidget* pWidgetAbove = NULL;
		iWidget* pWidgetBelow = NULL;
		
		// Check boundaries for validity of adjacent widgets
		if(x-1>=0)
			pWidgetAtLeft = mvSlots[y*mlLayout_SlotsColumns + x-1]->GetImageWidget();

		if(x+1<mlLayout_SlotsColumns)
			pWidgetAtRight = mvSlots[y*mlLayout_SlotsColumns + x+1]->GetImageWidget();

		if(y-1>=0)
			pWidgetAbove = mvSlots[(y-1)*mlLayout_SlotsColumns + x]->GetImageWidget();

		if(y+1<mlLayout_SlotsRows)
			pWidgetBelow = mvSlots[(y+1)*mlLayout_SlotsColumns + x]->GetImageWidget();


		////////////////////////////////////////////////
		// Set up navigation for current slot
		pSlotWidget->SetFocusNavigation(eUIArrow_Up, pWidgetAbove);
		pSlotWidget->SetFocusNavigation(eUIArrow_Right, pWidgetAtRight);
		pSlotWidget->SetFocusNavigation(eUIArrow_Down, pWidgetBelow);
		pSlotWidget->SetFocusNavigation(eUIArrow_Left, pWidgetAtLeft);
	}

	mvSlots[0+0]->GetImageWidget()->SetFocusNavigation(eUIArrow_Left, pHealth);
	mvSlots[mlLayout_SlotsColumns+0]->GetImageWidget()->SetFocusNavigation(eUIArrow_Left, pHealth);
	mvSlots[2*mlLayout_SlotsColumns+0]->GetImageWidget()->SetFocusNavigation(eUIArrow_Left, pSanity);

	mvSlots[0+mlLayout_SlotsColumns-1]->GetImageWidget()->SetFocusNavigation(eUIArrow_Right, pTinderbox);
	mvSlots[mlLayout_SlotsColumns+mlLayout_SlotsColumns-1]->GetImageWidget()->SetFocusNavigation(eUIArrow_Right, pOil);
	mvSlots[2*mlLayout_SlotsColumns+mlLayout_SlotsColumns-1]->GetImageWidget()->SetFocusNavigation(eUIArrow_Right, pOil);
}

//-----------------------------------------------------------------------

void cLuxInventory::DrawElementAtCenter(const cVector3f& avCenter, cGuiGfxElement* apGfx, float afAlpha, const cVector2f& avSize, bool abMulColWithAlpha)
{
	cVector2f vSize = avSize;
	if(vSize==-1)
		vSize = apGfx->GetImageSize();

	cColor col = abMulColWithAlpha ? cColor(afAlpha, afAlpha) : cColor(1, afAlpha);
	mpGuiSet->DrawGfx(apGfx, avCenter-vSize*0.5f, vSize,  col);
}

//-----------------------------------------------------------------------

bool cLuxInventory::SanityOnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lSanityStatus = StatusToIndex(gpBase->mpPlayer->GetSanity());
	const cVector3f& vSanityPos = mvLayout_SanityCenter;

	/////////////////////////////////////////
	// Draw glow
	StatusWidgetDraw(vSanityPos, lSanityStatus, mpSanityGlow); 
	
	//mpGuiSet->DrawFont(	mpFontDefault, mvLayout_SanityLabelCenter,
	//					mvLayout_FontSize_Label, cColor(1,mfAlpha), eFontAlign_Center, eGuiMaterial_FontNormal, 
	//					_W("%ls"), kTranslate("Inventory", "Sanity").c_str());

	/////////////////////////////////////////
	// Draw frame
	DrawFrameAroundWidget(mpFrameSanityCorners, mpFrameSanityBorders, apWidget, 
							mvLayout_SanityFrameHPadding, mvLayout_SanityFrameVPadding,
							mvLayout_SanityFrameOffset, mvLayout_SanityFrameSize);
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, SanityOnDraw);

//-----------------------------------------------------------------------

bool cLuxInventory::SanityOnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lSanityStatus = StatusToIndex(gpBase->mpPlayer->GetSanity());
	StatusWidgetUpdate(apWidget, lSanityStatus, mpSanityStatus, mvLayout_SanityCenter);
	
	tString sSanityDesc = "SanityDesc"+cString::ToString(lSanityStatus);
	MouseOverWidgetUpdate(apWidget, kTranslate("Inventory", "Sanity"), kTranslate("Inventory", sSanityDesc));
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, SanityOnUpdate);

//-----------------------------------------------------------------------

bool cLuxInventory::HealthOnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lHealthStatus = StatusToIndex(gpBase->mpPlayer->GetHealth());
	const cVector3f& vHealthPos = mvLayout_HealthCenter;

	/////////////////////////////////////////
	// Draw glow
	StatusWidgetDraw(vHealthPos, lHealthStatus, mpHealthGlow); 
	
	//mpGuiSet->DrawFont(	mpFontDefault, mvLayout_HealthLabelCenter,
	//					mvLayout_FontSize_Label, cColor(1,mfAlpha), eFontAlign_Center, eGuiMaterial_FontNormal, 
	//					_W("%ls"), kTranslate("Inventory", "Health").c_str());

	/////////////////////////////////////////
	// Draw frame
	DrawFrameAroundWidget(mpFrameHealthCorners, mpFrameHealthBorders, apWidget, 
							mvLayout_HealthFrameHPadding, mvLayout_HealthFrameVPadding,
							mvLayout_HealthFrameOffset, mvLayout_HealthFrameSize);
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, HealthOnDraw);

//-----------------------------------------------------------------------

bool cLuxInventory::HealthOnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lHealthStatus = StatusToIndex(gpBase->mpPlayer->GetHealth());
	StatusWidgetUpdate(apWidget, lHealthStatus, mpHealthStatus, mvLayout_HealthCenter);
	
	tString sHealthDesc = "HealthDesc"+cString::ToString(lHealthStatus);
	MouseOverWidgetUpdate(apWidget, kTranslate("Inventory", "Health"), kTranslate("Inventory", sHealthDesc));
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, HealthOnUpdate);

//-----------------------------------------------------------------------

bool cLuxInventory::OilOnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	const cVector3f& vOilCenter = mvLayout_OilCenter;

	////////////////////////
	//Oil container with remaining oil
	const cVector2f& vFullOilSize = apWidget->GetSize();
	float fRemainingOilHeight = gpBase->mpPlayer->GetLampOil()*0.01f * vFullOilSize.y;

	cVector3f vOilStart = vOilCenter-cVector3f(vFullOilSize)*0.5f;
	cVector2f vSize = cVector2f(vFullOilSize.x, fRemainingOilHeight);

	////////////////////////////////////
	// Set clip region so nothing is drawn outside the oil height
	cGuiClipRegion* pOldClipRegion = mpGuiSet->GetCurrentClipRegion();
	cGuiClipRegion* pChildClipRegion = pOldClipRegion->CreateChild(vOilStart + cVector3f(0,vFullOilSize.y-fRemainingOilHeight, 0.6f),
																	cVector2f(vFullOilSize.x,fRemainingOilHeight));
	
	mpGuiSet->SetCurrentClipRegion(pChildClipRegion);

	//////////////////////////////
	// Draw oil liquid
	cVector2f vLiguidSize = apWidget->GetSize()*2;
	cVector3f vLiquidPos = apWidget->GetGlobalPosition() - cVector3f(vLiguidSize.x*0.25f, vLiguidSize.y*0.25f, 0);

	cVector3f vLiquidPosAdd1 = cVector3f(	sin(mfOilMovementT*0.3f) * apWidget->GetSize().x*0.5, 
											cos(mfOilMovementT*0.2f) * apWidget->GetSize().y*0.2, 0.6f);
	cVector3f vLiquidPosAdd2 = cVector3f(  cos(mfOilMovementT*0.2f) * apWidget->GetSize().x*0.3, 
											sin(mfOilMovementT*0.15f) * apWidget->GetSize().y*0.15, 0.6f);
	
	mpGuiSet->DrawGfx(mpOilLiquid, vLiquidPos + vLiquidPosAdd1, vLiguidSize, cColor(1,0.75f*mfAlpha));
	mpGuiSet->DrawGfx(mpOilLiquid, vLiquidPos + vLiquidPosAdd2, vLiguidSize, cColor(1,0.25f*mfAlpha));

	//////////////////////////////
	// Draw bubbles
	for(int i=0; i<3; ++i)
		mpGuiSet->DrawGfx(mpOilBubble[i], mvOilBubblePos[i], mpOilBubble[i]->GetActiveSize(), cColor(1, mfAlpha));


	///////////////////////////////
	// Return to normal
	mpGuiSet->SetCurrentClipRegion(pOldClipRegion);
	
	///////////////////////////////
	// Draw frame
	DrawElementAtCenter(vOilCenter+cVector3f(0,0,1), mpRemainingOilFG, mfAlpha);
	
	//mpGuiSet->DrawFont(	mpFontDefault, mvLayout_OilLabelCenter,
	//					mvLayout_FontSize_Label, cColor(1,mfAlpha), eFontAlign_Center, eGuiMaterial_FontNormal, 
	//					_W("%ls"), kTranslate("Inventory", "LampOil").c_str());

	DrawFrameAroundWidget(mpFrameGenericCorners, mpFrameGenericBorders, apWidget, 
							mvLayout_OilFrameHPadding, mvLayout_OilFrameVPadding,
							mvLayout_OilFrameOffset, mvLayout_OilFrameSize);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, OilOnDraw);

//-----------------------------------------------------------------------


bool cLuxInventory::OilOnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	MouseOverWidgetUpdate(apWidget, kTranslate("Inventory", "LampOil"), kTranslate("Inventory", "LampOilDesc"));

	const cVector2f& vFullOilSize = apWidget->GetSize();
	float fRemainingOilHeight = gpBase->mpPlayer->GetLampOil()*0.01f * vFullOilSize.y;

	if(fRemainingOilHeight<kEpsilonf)
		return true;


	///////////////////////////////////
	// Update a var used to move oil
	mfOilMovementT += aData.mfVal;

	///////////////////////////////////
	// Update bubble position
	int lOutsideCount = 0; //The number of bubbles that reached the top
	const cVector3f& vOilPos=  apWidget->GetGlobalPosition();
	for(int i=0; i<3; ++i)
	{
		if(mvOilBubblePos[i].y < vOilPos.y-30) 
		{
			lOutsideCount++;
			continue;
		}
		
		mvOilBubblePos[i].y -= aData.mfVal*mvOilBubbleSpeed[i];
	}
	
	//If all outside, start over.
	if(lOutsideCount==3)
	{
		for(int i=0; i<3; ++i)
		{
			mvOilBubbleSpeed[i] = vFullOilSize.y*0.5f * cMath::RandRectf(0.5f, 1.25f);

			mvOilBubblePos[i].y = vOilPos.y+vFullOilSize.y;
			mvOilBubblePos[i].x = cMath::RandRectf(vOilPos.x, vOilPos.x+vFullOilSize.x);
		}
	}
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, OilOnUpdate);

//-----------------------------------------------------------------------

bool cLuxInventory::OilOnMouseUp(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mpPickedItem && mpPickedItem->GetType()==eLuxItemType_LampOil)
	{
		GetItemTypeData(eLuxItemType_LampOil)->OnUse(mpPickedItem, 0);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, OilOnMouseUp);

//-----------------------------------------------------------------------

bool cLuxInventory::TinderboxOnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	cVector2f vCountFontSize(mvLayout_FontSize_Label);

	const cVector3f& vTinderboxCenter = mvLayout_TinderboxesCenter;
	mpGuiSet->DrawFont( mpFontDefault, vTinderboxCenter+cVector3f(0,-vCountFontSize.y*0.5f,1), 
						vCountFontSize, cColor(1,mfAlpha), eFontAlign_Center, eGuiMaterial_FontNormal, 
						_W("x %d"), gpBase->mpPlayer->GetTinderboxes());

	//mpGuiSet->DrawFont(	mpFontDefault, mvLayout_TinderboxesLabelCenter,
	//					mvLayout_FontSize_Label, cColor(1,mfAlpha), eFontAlign_Center, eGuiMaterial_FontNormal,
	//					_W("%ls"),kTranslate("Inventory", "Tinderboxes").c_str());

	DrawFrameAroundWidget(mpFrameGenericCorners, mpFrameGenericBorders, apWidget, 
							mvLayout_TinderboxesFrameHPadding, mvLayout_TinderboxesFrameVPadding,
							mvLayout_TinderboxesFrameOffset, mvLayout_TinderboxesFrameSize);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, TinderboxOnDraw);

//-----------------------------------------------------------------------


bool cLuxInventory::TinderboxOnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	MouseOverWidgetUpdate(apWidget, kTranslate("Inventory", "Tinderboxes"), kTranslate("Inventory", "TinderboxesDesc"));
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, TinderboxOnUpdate);

//-----------------------------------------------------------------------

bool cLuxInventory::JournalOnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	float fAlpha = mvAlphaFader[eLuxInventoryFader_Journal].mfAlpha * mfAlpha;

	cVector2f vSizeAdd = (mpJournalMouseOverGfx->GetActiveSize() - apWidget->GetSize()) * 0.5f;

	mpGuiSet->DrawGfx(mpJournalMouseOverGfx, apWidget->GetGlobalPosition()+cVector3f(vSizeAdd.x,vSizeAdd.y,0.5f),-1, cColor(1, fAlpha));

	//mpGuiSet->DrawFont(	mpFontDefault, mvLayout_JournalLabelCenter,
	//							mvLayout_FontSize_Label, cColor(1,mfAlpha), eFontAlign_Center, eGuiMaterial_FontNormal,
	//							_W("%ls"),kTranslate("Inventory", "Journal").c_str());

	/////////////////////////////////////////
	// Draw frame
	DrawFrameAroundWidget(mpFrameGenericCorners, mpFrameGenericBorders, apWidget, 
							mvLayout_JournalFrameHPadding, mvLayout_JournalFrameVPadding,
							mvLayout_JournalFrameOffset, mvLayout_JournalFrameSize);
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, JournalOnDraw);

//-----------------------------------------------------------------------

bool cLuxInventory::JournalOnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	MouseOverWidgetUpdate(apWidget, kTranslate("Inventory", "Journal"), kTranslate("Inventory", "JournalDesc"));
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, JournalOnUpdate);

//-----------------------------------------------------------------------

bool cLuxInventory::JournalMouseDown(iWidget* apWidget, const cGuiMessageData& aData)
{
	OpenJournal();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, JournalMouseDown);

//-----------------------------------------------------------------------

bool cLuxInventory::JournalUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==eUIButton_Primary)
		OpenJournal();
	else
		return false;

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, JournalUIButtonPress);

//-----------------------------------------------------------------------


bool cLuxInventory::ItemSlotsOnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbLayout_DrawSlotsFrame)
	{
		DrawFrameAroundWidget(mpFrameGenericCorners, mpFrameGenericBorders, apWidget, 
								mvLayout_SlotFrameHPadding, mvLayout_SlotFrameVPadding, 
								mvLayout_SlotFrameOffset, mvLayout_SlotFrameSize);
	}

	if(mbLayout_DrawSlotsGrid)
	{
		// Get grid lines size
		cVector2f vHoriSize = cVector2f(apWidget->GetSize().x + mvLayout_SlotGridHExtension.x + mvLayout_SlotGridHExtension.y, 
										mvLayout_SlotsSeparation.y);
		cVector2f vVertSize = cVector2f(mvLayout_SlotsSeparation.x, 
										apWidget->GetSize().y + mvLayout_SlotGridVExtension.x + mvLayout_SlotGridVExtension.y);

		// Get grid lines separation
		cVector2f vLineSeparation = mvLayout_SlotSize + 
									cVector2f(mvLayout_SlotsSeparation.x, mvLayout_SlotsSeparation.y);

		// Get grid start point
		cVector3f vPos = mvLayout_SlotsStart - 
							cVector3f(mvLayout_SlotFrameHPadding.x, mvLayout_SlotFrameVPadding.x, 0) +
							mvLayout_SlotFrameOffset + mvLayout_SlotsGridOffset;

		// Get line color
		cColor lineColor = mLayout_SlotsGridColor;
		lineColor.a *= mfAlpha;

		// Draw crossed lines
		for(int i=1;i<mlLayout_SlotsRows;++i)
			mpGuiSet->DrawGfx(mpWhiteGfx, vPos + cVector3f(-mvLayout_SlotGridHExtension.x,vLineSeparation.y*i,0), vHoriSize, lineColor);

		for(int i=1;i<mlLayout_SlotsColumns;++i)
			mpGuiSet->DrawGfx(mpWhiteGfx, vPos + cVector3f(vLineSeparation.x*i,-mvLayout_SlotGridVExtension.x,0), vVertSize, lineColor);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, ItemSlotsOnDraw);

//-----------------------------------------------------------------------

bool cLuxInventory::ItemSlotsOnFocusNav(iWidget* apWidget, const cGuiMessageData& aData)
{
	SetCurrentWidget(apWidget);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, ItemSlotsOnFocusNav);

//-----------------------------------------------------------------------

bool cLuxInventory::UIButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	iWidget* pWidget = mpGuiSet->GetFocusedWidget();

	////////////////
	// Switch category based on which category we are in
	int lUserValue = pWidget == NULL ? 0 : pWidget->GetUserValue();

	if(lUserValue == 0) return false;

	switch(aData.mlVal)
	{
	case eUIButton_NextPage:
		switch(lUserValue)
		{
		case 1:
		case 2: SetCurrentWidget(mpTinderboxWidget); return true;
		case 3: SetCurrentWidget(mpOilWidget); return true;
		case 4: SetCurrentWidget(mpJournalWidget); return true;
		default: SetCurrentWidget(mpTinderboxWidget); return true;
		}
		break;
	case eUIButton_PrevPage:
		switch(lUserValue)
		{
		case 2: SetCurrentWidget(mpSanityWidget); return true;
		case 3: 
		case 4: 
		case 5: SetCurrentWidget(mpHealthWidget); return true;
		default: SetCurrentWidget(mpHealthWidget); return true;
		}
		break;
	}

	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, UIButtonPress);


//-----------------------------------------------------------------------

void cLuxInventory::StatusWidgetUpdate(iWidget* apWidget, int alStatusIdx, cGuiGfxElement** avStatusGfx, const cVector3f &avPos)
{
	////////////////////////////////////////////////////////////
	// Helper for updating status widgets

	cWidgetImage* pImage = (cWidgetImage*)apWidget;
	cGuiGfxElement* pOldGfx = pImage->GetImage();
	cGuiGfxElement* pNewGfx = avStatusGfx[alStatusIdx];
	if(pNewGfx!=pOldGfx)
	{
		pImage->SetImage(pNewGfx);
		pImage->SetPosition(avPos-cVector3f(pNewGfx->GetImageSize())*0.5f);
		pImage->SetSize(pNewGfx->GetImageSize());
	}
}

void cLuxInventory::StatusWidgetDraw(const cVector3f& avPos, int alStatusIdx, cGuiGfxElement** apStatusGlowGfx)
{
	cGuiGfxElement* pGlow = apStatusGlowGfx[alStatusIdx];

	//////////////////////////////////////////////////////////////////////////////////////
	// Get an alpha value oscillating between fPulseMid-fPulseAmp and fPulseMid+fPulseAmp, 
	// with fPulseAmp being the distance between fPulseMid and 1

	float fPulseMid = mvPulseMiddle[alStatusIdx];
	float fPulseAmp = 1.0f - fPulseMid;
	float fGlowAlpha = fPulseMid + fPulseAmp*(cos(mfTimer*mvPulseFreq[alStatusIdx]));

	DrawElementAtCenter(avPos+cVector3f(0,0,0.1f), pGlow, mfAlpha*fGlowAlpha, pGlow->GetActiveSize(), false);
}

//-----------------------------------------------------------------------

void cLuxInventory::MouseOverWidgetUpdate(iWidget* apWidget, const tWString& asDescHeader, const tWString& asDescMsg)
{
	int lFaderID = apWidget->GetUserValue();
	if (lFaderID >= eLuxInventoryFader_LastEnum) {
		Warning("Invalid InventoryFader %d\n", lFaderID);
		return;
	}
	if(mbMessageActive==false && apWidget==mpCurrentWidget && mvAlphaFader[lFaderID].mbActive==false)
	{
		mvAlphaFader[lFaderID].mbActive=true;
		SetDescText(asDescHeader, asDescMsg);
	}
	
	if( (apWidget!=mpCurrentWidget || mbMessageActive) && mvAlphaFader[lFaderID].mbActive)
	{
		mvAlphaFader[lFaderID].mbActive=false;
		if(msHeaderText==asDescHeader && msNormalText==asDescMsg)
			SetDescText(_W(""), _W(""));
	}
}

//-----------------------------------------------------------------------


bool cLuxInventory::InventoryWidgetMouseEnter(iWidget* apWidget, const cGuiMessageData& aData)
{
	return InventoryWidgetOnGetUINavFocus(apWidget, aData);
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, InventoryWidgetMouseEnter);


bool cLuxInventory::InventoryWidgetMouseLeave(iWidget* apWidget, const cGuiMessageData& aData)
{
	return InventoryWidgetOnLoseUINavFocus(apWidget, aData);
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, InventoryWidgetMouseLeave);

bool cLuxInventory::InventoryWidgetOnGetUINavFocus(iWidget* apWidget, const cGuiMessageData& aData)
{
	SetCurrentWidget(apWidget);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, InventoryWidgetOnGetUINavFocus);


bool cLuxInventory::InventoryWidgetOnLoseUINavFocus(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(GetCurrentWidget()==apWidget)
		SetCurrentWidget(NULL);
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, InventoryWidgetOnLoseUINavFocus);

//-----------------------------------------------------------------------

bool cLuxInventory::InventoryDrawFocus(iWidget* apWidget, const cGuiMessageData& aData)
{
	////////////////////////
	//Picked item
	if(mpCurrentWidget)
	{
		cVector3f vCursorPos = mpCurrentWidget->GetGlobalPosition() + mpCurrentWidget->GetSize()*0.5f;
		vCursorPos.z = mpGuiSet->GetMouseZ()-1.0f;
		mpGuiSet->DrawGfx(mpGuiSet->GetCurrentPointer(), vCursorPos, -1, cColor(1.0f, mfAlpha));
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxInventory, InventoryDrawFocus);

//-----------------------------------------------------------------------

void cLuxInventory::DrawFrameAroundWidget(cGuiGfxElement** apGfxCorners, cGuiGfxElement** apGfxBorders,
										  iWidget* apWidget, 
										  const cVector2f& avHPadding, const cVector2f& avVPadding, 
										  const cVector3f& avOffset, const cVector2f& avSize)
{
	// Get Base Rectangle
	const cVector3f& vPosition = apWidget->GetGlobalPosition();
	cVector2f vWidgetSize = apWidget->GetSize();
	cVector2f vSize = cVector2f( avSize.x==-1? vWidgetSize.x : avSize.x, 
								 avSize.y==-1? vWidgetSize.y : avSize.y);

	// Wrap frame around rectangle with added padding 
	DrawFrame(apGfxCorners, apGfxBorders, 
				vPosition-cVector3f(apGfxCorners[0]->GetActiveSize().x+avHPadding.x, apGfxCorners[0]->GetActiveSize().y+avVPadding.x, 0) + avOffset, 
				vSize + cVector2f(avHPadding.x+avHPadding.y, avVPadding.x+avVPadding.y));
}


//-----------------------------------------------------------------------

void cLuxInventory::DrawFrame(cGuiGfxElement** apGfxCorners, cGuiGfxElement** apGfxBorders,
								const cVector3f& avPosition, const cVector2f& avSize)
{
	/////////////////////////////////////////
	// Draw frame
	cVector3f vPos = avPosition;

	// Left part
	{
		// Draw Upper left corner
		mpGuiSet->DrawGfx(apGfxCorners[0], 
							vPos, -1, cColor(1,mfAlpha));

		// Draw Left border
		mpGuiSet->DrawGfx(apGfxBorders[3], 
							vPos + cVector3f(0, apGfxCorners[0]->GetActiveSize().y, 0), 
							cVector2f( apGfxCorners[0]->GetActiveSize().x, avSize.y),
							cColor(1,mfAlpha));

		// Draw Lower left corner
		mpGuiSet->DrawGfx(apGfxCorners[3],
							vPos + cVector3f(0, apGfxCorners[0]->GetActiveSize().y+ avSize.y, 0),
							-1, cColor(1,mfAlpha));

	}

	// Move vPos to the right
	vPos.x += apGfxCorners[0]->GetActiveSize().x;

	// Middle part
	{
		// Draw Upper border
		mpGuiSet->DrawGfx(apGfxBorders[0], 
							vPos, 
							cVector2f(avSize.x, 
										apGfxCorners[0]->GetActiveSize().y),
							cColor(1,mfAlpha));

		// Draw Lower border
		mpGuiSet->DrawGfx(apGfxBorders[2], 
							vPos+cVector3f(0, apGfxCorners[0]->GetActiveSize().y + avSize.y,0), 
							cVector2f(avSize.x, apGfxCorners[3]->GetActiveSize().y),
							cColor(1,mfAlpha));
	}

	// Move vPos to the right
	vPos.x += avSize.x;

	// Right part
	{
		// Draw Upper right corner
		mpGuiSet->DrawGfx(apGfxCorners[1],
							vPos,
							-1,cColor(1,mfAlpha));

		// Draw Right border
		mpGuiSet->DrawGfx(apGfxBorders[1], 
							vPos + cVector3f(0, apGfxCorners[1]->GetActiveSize().y, 0), 
							cVector2f( apGfxCorners[1]->GetActiveSize().x, avSize.y),
							cColor(1,mfAlpha));

		// Draw Lower right corner
		mpGuiSet->DrawGfx(apGfxCorners[2],
							vPos + cVector3f(0, apGfxCorners[1]->GetActiveSize().y + avSize.y, 0),
							-1, cColor(1,mfAlpha));
	}

}


//-----------------------------------------------------------------------



#ifdef USE_GAMEPAD
tWString cLuxInventory::ParseStringForGamepadIconsRemoval(const tWString & asCurrentText)
{
	tWString sOutput=_W("");
	tWString sCommand =_W("");
	bool bParseVar = false;
	
	int lPosition = 0;
 
	/////////////////
	// Find all the icons
    for(size_t i=0; i<asCurrentText.size(); ++i)
	{
		wchar_t lChar = asCurrentText[i];
		
		////////////////////////
		// Parse variable
		if(bParseVar)
		{
			// Add character to command
			if( lChar != '$' )
			{
				sCommand += lChar;
			}
			//Parse command
			else
			{
				///////////
				// Create the icon and get the replacement text for buttons that have no icon
				tWString sReplace = AddGamepadTextAtPosition(sCommand, lPosition);

				/////////
				// Make room for the icon
				sOutput += sReplace;
				lPosition += sReplace.size();

				bParseVar = false;
			}

		}
		////////////////////////
		// Normal parsing
		else
		{
			if(lChar == '$')
			{
                bParseVar = true;
				sCommand = _W("");
			}
			else
			{
				lPosition++;
				sOutput += lChar;
			}
		}
	}

	return sOutput;
}

//-----------------------------------------------------------------------

tWString cLuxInventory::AddGamepadTextAtPosition(const tWString& asCommand, int alPosition)
{
	////////////
	// Convert the command from wString to tString
	tString sCommand;
	sCommand.resize(asCommand.size());
	std::copy(asCommand.begin(), asCommand.end(), sCommand.begin());

	////////////////
	// Get icon from command string
	tString sSep = ".";
	tStringVec vInputParts;
	cString::GetStringVec(sCommand, vInputParts, &sSep);

	cLuxHintIcon hintIcon;
	hintIcon.mlCharacterPosition = alPosition;

	if(vInputParts[0] == "GamepadButton")
	{
		/////////////
		// Return the translated command for buttons with no icon
		return kTranslate("ButtonNames", vInputParts[1]);
	}
	else
	{
		/////////////
		// Return the translated command for buttons with no icon
		return kTranslate("ButtonNames", vInputParts[1] +"."+ vInputParts[2]);
	}

	return _W(" ");
}
#endif
