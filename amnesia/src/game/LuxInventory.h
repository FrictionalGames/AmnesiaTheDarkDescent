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

#ifndef LUX_INVENTORY_H
#define LUX_INVENTORY_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class iLuxItemType;
class cLuxCombineItemCallback;

//----------------------------------------------

enum eLuxInventoryFader
{
	eLuxInventoryFader_Journal,
	eLuxInventoryFader_Sanity,
	eLuxInventoryFader_Health,
	eLuxInventoryFader_Tinderbox,
	eLuxInventoryFader_Oil,

	eLuxInventoryFader_LastEnum
};

//----------------------------------------------

class cLuxInventory_Item 
{
public:
	cLuxInventory_Item(const tString& asName, eLuxItemType aType, const tString& asSubType, cLuxInventory *apInventory);
	~cLuxInventory_Item();

	const tString& GetName(){ return msName; }
	eLuxItemType GetType() { return mType;}
	const tString& GetSubType(){ return msSubType; }

	void SetImage(cGuiGfxElement *apImage){ mpImage = apImage;}
	cGuiGfxElement* GetImage(){ return mpImage;}

	void SetImageName(const tString& asName){ msImageName = asName;}
	const tString& GetImageName(){ return msImageName;}

	void SetStringVal(const tString& asVal){ msVal = asVal;}
	const tString& GetStringVal(){ return msVal;}

	void SetExtraStringVal(const tString& asExtraVal){ msExtraVal = asExtraVal;}
	const tString& GetExtraStringVal(){ return msExtraVal;}

	tString msImageFile;

	float GetAmount(){ return mfAmount;}
	int GetCount(){ return mlCount;}
	const tString& GetGameNameEntry(){ return msGameNameEntry;}
	const tString& GetGameDescEntry(){ return msGameDescEntry;}

	void SetAmount(float afX){ mfAmount = afX;}
	void SetCount(int alX){ mlCount = alX; }
	void AddCount(int alX){ mlCount += alX; }
	void SetGameNameEntry(const tString& asEntry){ msGameNameEntry =  asEntry;}
	void SetGameDescEntry(const tString& asEntry){ msGameDescEntry =  asEntry;}

private:
	cLuxInventory *mpInventory;

	tString msName;	
	eLuxItemType mType;
	tString msSubType;
	tString msVal;
	tString msExtraVal;

	cGuiGfxElement *mpImage;
	tString msImageName;
	
	int mlCount;
	float mfAmount;
	tString msGameNameEntry;
	tString msGameDescEntry;
};

//----------------------------------------------

class cLuxInventory_Slot
{
	friend class cLuxInventory;
public:
	cLuxInventory_Slot(cLuxInventory *apInventory, cWidgetImage* apImage, int alIdx);
	~cLuxInventory_Slot();

	cWidgetImage* GetImageWidget() { return mpImage; }

private:
	////////////////////////
	//Helper
    cLuxInventory_Item* GetItem();

	////////////////////////
	//Callbacks
	bool OnMouseDown(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnMouseDown);

	bool OnMouseUp(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnMouseUp);

	bool OnMouseDoubleClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnMouseDoubleClick);

	bool OnMouseEnter(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnMouseEnter);

	bool OnMouseLeave(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnMouseLeave);

	bool OnGetUINavFocus(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnGetUINavFocus);

	bool OnLoseUINavFocus(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnLoseUINavFocus);

	bool OnUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnUIButtonPress);

	bool OnUIButtonRelease(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnUIButtonRelease);

	bool OnUIButtonDoublePress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnUIButtonDoublePress);
	

	bool OnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnUpdate);

	bool OnDraw(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnDraw);

	////////////////////////
	//Data
	cLuxInventory *mpInventory;
	cWidgetImage *mpImage;

	cGuiGfxElement *mpGlowGfx;

	int mlIdx;

	////////////////////////
	//Variables
	float mfGlowAlpha;
};

//----------------------------------------------

class cLuxInventory : public iLuxUpdateable
{
friend class cLuxInventory_SaveData;
friend class cLuxInventory_Slot;
friend class cLuxInventory_Item;
public:	
	cLuxInventory();
	~cLuxInventory();
	
	////////////////////////
	// General
	void OnClearFonts();
	void LoadFonts();

	void OnStart();
	void Reset();
	
	void OnGameStart();

	void Update(float afTimeStep);

	void OnEnterContainer(const tString& asOldContainer);
	void OnLeaveContainer(const tString& asNewContainer);

	void OnDraw(float afFrameTime);

	cGuiSet* GetSet() { return mpGuiSet; }

	void ExitPressed();

	////////////////////////
	// Actions
	void LoadScript();

	cLuxInventory_Item* AddItem(const tString& asName, eLuxItemType aType,
								const tString& asSubTypeName, const tString& asImageName,
								float afAmount, const tString & asVal,const tString& asExtraVal,
								bool* apRemoveItemProp=NULL);
	void RemoveItem(const tString& asName);
	void RemoveItem(cLuxInventory_Item *apItem);
	void RemoveItemOfType(eLuxItemType aType);
	int GetItemIndex(cLuxInventory_Item *apItem);
	bool ItemExists(cLuxInventory_Item *apItem){ return GetItemIndex(apItem) >= 0; }
	cLuxInventory_Item* GetItem(const tString& asName);
	cLuxInventory_Item* GetItemFromSubType(const tString& asSubType);
	bool HasItemOfType(eLuxItemType aType);

	void AddCombineCallback(const tString& asName,
							const tString& asItemA, const tString& asItemB,
							const tString& asFunction, bool abAutoDestroy);
	void RemoveCombineCallback(const tString& asName);
	cLuxCombineItemsCallback* GetCombineCallback(const tString& asItemA, const tString& asItemB);

	void RunScript(const tString& asCommand);
	bool RecompileScript(tString *apOutput);

	void SetDescTextFromItem(cLuxInventory_Item *apItem);

	void OpenJournal();

	////////////////////////
	// Properties
	void SetDisabled(bool abX){ mbDisabled = abX;}
	bool GetDisabled(){ return mbDisabled;}

	void SetDescText(const tWString &asHeader, const tWString &asNormal);
	/**
	 * if life time is <=0 then the life time is calculated based on string length.
	 */
	void SetMessageText(const tWString &asText, float afLifeTime);
	bool GetMessageActive(){ return mbMessageActive;}

	void SetPickedItem(cLuxInventory_Item *apItem, const cVector2f& avOffset);
	cLuxInventory_Item* GetPickedItem(){ return mpPickedItem;}

	void SetCurrentWidget(iWidget *apWidget);
	iWidget* GetCurrentWidget(){ return mpCurrentWidget;}

	void SetEnterFromJournal(bool abX){ mbEnterFromJournal = abX;}

	cLuxInventory_Item* GetItem(int alIdx){ return mvItems[alIdx];}
	int GetItemNum(){ return (int)mvItems.size();}
	
	void SetEquippedHandItem(cLuxInventory_Item *apItem);
	cLuxInventory_Item* GetEquippedHandItem(){ return mpEquippedItem; }

	eLuxItemType GetItemTypeFromString(const tString& asType);
	iLuxItemType * GetItemTypeData(eLuxItemType aType){ return mvItemTypes[aType];}


	bool GetPickedItemMoved() { return mbPickedObjectMoved; }
	double GetItemPickedAt() { return mfPickedUpAt; }

#ifdef USE_GAMEPAD
	void AppDeviceWasPlugged();
	void AppDeviceWasRemoved();
#endif

private:
	///////////////////////
	// Helper methods
	void DrawDescTextRow(const tWString& asText, const cColor& aCol);

	void ResetSessionVars();

	bool CheckSpecialCombineAction(cLuxInventory_Item *apItemA, cLuxInventory_Item *apItemB, int alSlotIndex);

	void CreateBackground();
	void RenderBackgroundImage();
	void CreateScreenTextures();
	void DestroyBackground();

	void CreateGui();

	void DrawElementAtCenter(const cVector3f& avCenter, cGuiGfxElement* apGfx, float afAlpha, const cVector2f& avSize=-1, bool abMulColWithAlpha=false);

#ifdef USE_GAMEPAD
	tWString ParseStringForGamepadIconsRemoval(const tWString & asCurrentText);
	tWString AddGamepadTextAtPosition(const tWString& asCommand, int alPosition);
#endif

	///////////////////////
	// Gui callbacks

	// Sanity
	bool SanityOnDraw(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SanityOnDraw);

	bool SanityOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SanityOnUpdate);

	// Health
	bool HealthOnDraw(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(HealthOnDraw);

	bool HealthOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(HealthOnUpdate);

	// Oil
	bool OilOnDraw(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OilOnDraw);

	bool OilOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OilOnUpdate);

	bool OilOnMouseUp(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OilOnMouseUp);

	// Tinderbox
	bool TinderboxOnDraw(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(TinderboxOnDraw);

	bool TinderboxOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(TinderboxOnUpdate);

	// Journal
	bool JournalOnDraw(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(JournalOnDraw);

	bool JournalOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(JournalOnUpdate);

	bool JournalMouseDown(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(JournalMouseDown);

	bool JournalUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(JournalUIButtonPress);

	// Items
	bool ItemSlotsOnDraw(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ItemSlotsOnDraw);

	bool ItemSlotsOnFocusNav(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ItemSlotsOnFocusNav);

	// Global
	bool UIButtonPress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIButtonPress);

	void StatusWidgetUpdate(iWidget* apWidget, int alStatusIdx, cGuiGfxElement** avStatusGfx, const cVector3f &avPos);
	void StatusWidgetDraw(const cVector3f& vPos, int alStatusIdx, cGuiGfxElement** avStatusGlowGfx);

	void MouseOverWidgetUpdate(iWidget* apWidget, const tWString& asDescHeader, const tWString& asDescMsg);

	bool InventoryWidgetMouseEnter(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InventoryWidgetMouseEnter);

	bool InventoryWidgetMouseLeave(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InventoryWidgetMouseLeave);

	bool InventoryWidgetOnGetUINavFocus(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InventoryWidgetOnGetUINavFocus);

	bool InventoryWidgetOnLoseUINavFocus(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InventoryWidgetOnLoseUINavFocus);

	
	bool InventoryDrawFocus(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InventoryDrawFocus);

	/**
	 * Draws a frame around a given widget using some given graphics
	 * \param apGfxCorners Pointer to an array of 4 cGuiGfxElement objects for the frame corners
	 * \param apGfxBorders Pointer to an array of 4 cGuiGfxElement objects for the frame borders
	 * \param apWidget Pointer to a Widget to wrap the frame around
	 * \param avHPadding Vector2f that holds the horizontal padding for the frame (x: left padding, y: right padding)
	 * \param avVPadding Vector2f that holds the vertical padding for the frame (x: upper padding, y: lower padding)
	 * \param avOffset Vector3f holding an offset value (useful when wanting to adjust frame z-value for example)
	 * \param avSize Vector2f that states size for the base rectangle(-1 in will use widget size)
	 */
	void DrawFrameAroundWidget(cGuiGfxElement** apGfxCorners, cGuiGfxElement** apGfxBorders, 
									iWidget* apWidget, 
									const cVector2f& avHPadding=0, const cVector2f& avVPadding=0, 
									const cVector3f& avOffset=0, const cVector2f& avSize=-1);

	/**
	 * Draws a frame around a rectangle given by a position and size
	 */
	void DrawFrame(cGuiGfxElement** apGfxFrameCorners, cGuiGfxElement** apGfxFrameBorders, 
					const cVector3f& avPosition, const cVector2f& avSize);

	bool OnDrawInvWidget(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnDrawInvWidget);

	///////////////////////
	// Settings
	float mfFadeInTime;
	float mfFadeOutTime;
	
	///////////////////////
	// Data
	cGui *mpGui;
	cScene *mpScene;
	cGraphics *mpGraphics;
	
	cGuiSkin *mpGuiSkin;
	cGuiSet *mpGuiSet;

	iScript *mpScript;

	iFontData *mpFontDefault;
	iFontData *mpFontHeader;

	cViewport *mpViewport;

	std::vector<iLuxItemType*> mvItemTypes;

	std::vector<cLuxInventory_Slot*> mvSlots;
	std::vector<cLuxInventory_Item*> mvItems;
	
	std::vector<cWidgetImage*> mvImageWidgets;
	
	cVector2f mvScreenSize;
	cVector2f mvGuiSetCenterSize;
	cVector2f mvGuiSetSize;
	cVector2f mvGuiSetOffset;
	cVector3f mvGuiSetStartPos;

	iTexture *mpScreenTexture;
	cGuiGfxElement *mpScreenGfx;
	iTexture *mpScreenBgTexture;
	cGuiGfxElement *mpScreenBgGfx;

	cGuiGfxElement* mpFrameHealthCorners[4];
	cGuiGfxElement* mpFrameHealthBorders[4];

	cGuiGfxElement* mpFrameSanityCorners[4];
	cGuiGfxElement* mpFrameSanityBorders[4];

	cGuiGfxElement* mpFrameGenericCorners[4];
	cGuiGfxElement* mpFrameGenericBorders[4];

	cGuiGfxElement *mpSanityStatus[4];
	cGuiGfxElement *mpSanityGlow[4];
	cGuiGfxElement *mpHealthStatus[4];
	cGuiGfxElement *mpHealthGlow[4];

	cGuiGfxElement *mpRemainingOilFG;
	cGuiGfxElement *mpRemainingOilBG;
	cGuiGfxElement* mpOilBubble[3];
	cGuiGfxElement* mpOilLiquid;
	cVector3f mvOilBubblePos[3];
	float mvOilBubbleSpeed[3];
	float mfOilMovementT;

	cGuiGfxElement *mpTinderbox;

	cGuiGfxElement *mpJournalMouseOverGfx;

	cGuiGfxElement *mpWhiteGfx;

	iGpuProgram *mpEffectProgram;

	iWidget * mpHealthWidget;
	iWidget * mpSanityWidget;
	iWidget * mpTinderboxWidget;
	iWidget * mpOilWidget;
	iWidget * mpJournalWidget;

	////////////////////////
	// Layout properties
	cVector3f mvLayout_PopUpMessageCenter;

	// Journal
	cVector3f mvLayout_JournalCenter;
	cVector3f mvLayout_JournalLabelCenter;
	cVector2f mvLayout_JournalFrameSize;
	cVector2f mvLayout_JournalFrameHPadding;
	cVector2f mvLayout_JournalFrameVPadding;
	cVector3f mvLayout_JournalFrameOffset;

	// Item Slot
	tString msLayout_SlotGfx;
	cVector2f mvLayout_SlotSize;
	cColor mLayout_SlotColor;

	tString msLayout_SlotMouseOverGfx;
	cColor mLayout_SlotMouseOverColor;
	cVector3f mvLayout_SlotMouseOverOffset;

	float mfLayout_SlotPulsatingMin;
	float mfLayout_SlotPulsatingAmp;
	float mfLayout_SlotPulsatingFreq;

	// Slots group
	int mlLayout_SlotsRows;
	int mlLayout_SlotsColumns;
	cColor mLayout_SlotsGridColor;
	bool mbLayout_DrawSlotsGrid;
	cVector3f mvLayout_SlotsStart;
	cVector3f mvLayout_SlotsSeparation;
	bool mbLayout_DrawSlotsFrame;
	cVector2f mvLayout_SlotFrameSize;
	cVector2f mvLayout_SlotFrameHPadding;
	cVector2f mvLayout_SlotFrameVPadding;
	cVector3f mvLayout_SlotFrameOffset;
	cVector2f mvLayout_SlotGridHExtension;
	cVector2f mvLayout_SlotGridVExtension;
	cVector3f mvLayout_SlotsGridOffset;

	// Sanity
	cVector3f mvLayout_SanityCenter;
	cVector3f mvLayout_SanityLabelCenter;
	cVector2f mvLayout_SanityFrameSize;
	cVector2f mvLayout_SanityFrameHPadding;
	cVector2f mvLayout_SanityFrameVPadding;
	cVector3f mvLayout_SanityFrameOffset;

	// Health
	cVector3f mvLayout_HealthCenter;
	cVector3f mvLayout_HealthLabelCenter;
	cVector2f mvLayout_HealthFrameSize;
	cVector2f mvLayout_HealthFrameHPadding;
	cVector2f mvLayout_HealthFrameVPadding;
	cVector3f mvLayout_HealthFrameOffset;

	// Tinderboxes
	cVector3f mvLayout_TinderboxesCenter;
	cVector3f mvLayout_TinderboxesLabelCenter;
	cVector2f mvLayout_TinderboxesFrameSize;
	cVector2f mvLayout_TinderboxesFrameHPadding;
	cVector2f mvLayout_TinderboxesFrameVPadding;
	cVector3f mvLayout_TinderboxesFrameOffset;

	// Oil
	cVector3f mvLayout_OilCenter;
	cVector3f mvLayout_OilLabelCenter;
	cVector2f mvLayout_OilFrameSize;
	cVector2f mvLayout_OilFrameHPadding;
	cVector2f mvLayout_OilFrameVPadding;
	cVector3f mvLayout_OilFrameOffset;

	// Item text
	cVector3f mvLayout_ItemHeaderCenter;
	cVector3f mvLayout_ItemDescCenter;
	cVector2f mvLayout_ItemTextFrameSize;
	cVector2f mvLayout_ItemTextFrameHPadding;
	cVector2f mvLayout_ItemTextFrameVPadding;
	cVector3f mvLayout_ItemTextFrameOffset;
	float mfLayout_ItemDescTextWidth;

	cVector3f mvLayout_ItemCountOffset;

	cVector2f mvLayout_FontSize_ItemCount;
	cVector2f mvLayout_FontSize_Label;
	cVector2f mvLayout_FontSize_Header;
	cVector2f mvLayout_FontSize_Description;
	cVector2f mvLayout_FontSize_Message;

	///////////////////////
	// Variables
	float mfAlpha;
	bool mbActive;
	bool mbDisabled;
	bool mbExitToJournal;
	bool mbEnterFromJournal;

	cLuxAlphaFader mvAlphaFader[eLuxInventoryFader_LastEnum];

	tLuxCombineItemsCallbackList mlstCombineCallbacks;
	
	tWString msHeaderText;
	tWString msNormalText;

	tWString msOldHeaderText;
	tWString msOldNormalText;
	float mfOldDescTextAlpha;

	tWString msMessageText;
	float mfMessageTextLife;
	float mfMessageTextAlpha;
	bool mbMessageActive;
	
	double mfPickedUpAt;
	bool mbPickedObjectMoved;
	cLuxInventory_Item *mpPickedItem;
	cVector2f mvPickedItemOffset;
	iWidget *mpCurrentWidget;
	iWidget *mpLastCurrentWidget;

	cLuxInventory_Item *mpEquippedItem;

	std::vector<cGuiGfxElement*> mvGridLines;

	float mfTimer;
	tFloatVec mvPulseMiddle;
	tFloatVec mvPulseFreq;
};

//----------------------------------------------


#endif // LUX_INVENTORY_H
