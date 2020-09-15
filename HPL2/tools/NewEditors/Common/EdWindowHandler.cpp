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

#include "../Common/EdWindowHandler.h"

#include "../Common/EdMenu.h"
#include "../Common/Editor.h"
#include "../Common/EdWorld.h"
#include "../Common/EdSurfacePicker.h"

#include "../Common/EdPopUpTextureBrowser.h"
#include "../Common/EdPopUpSoundBrowser.h"

#include "../Common/EdWindowFind.h"


//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// WINDOW - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

cGuiSet* iEdWindow::mpSet = NULL;

//---------------------------------------------------------------------

iEdWindow::iEdWindow(iEditor* apEditor, const tWString& asName) : iEdModule(apEditor, asName)
{
	if(mpSet==NULL) mpSet = apEditor->GetGuiSet();

	mpBGWidget = NULL;
	mpLayoutBlock = NULL;
	mpMenu = NULL;

	mbUpdated = false;

	mbDestroyOnSetInactive = false;

	mbLayoutCreated = false; 

	//Log("Creating window %ls\n", GetName().c_str());
}

iEdWindow::~iEdWindow()
{
	//Log("Deleting window %ls\n", GetName().c_str());
	STLDeleteAll(mlstInputs);

	if(mpMenu)
		hplDelete(mpMenu);

	if(mpBGWidget && mpSet->IsDestroyingSet()==false)
		mpSet->DestroyWidget(mpBGWidget, true);
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// WINDOW - PUBLIC METHODS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

void iEdWindow::SetDestroyOnSetInactive(bool abX)
{
	mbDestroyOnSetInactive = abX;
}

//---------------------------------------------------------------------

float iEdWindow::GetMenuHeight()
{
	if(mpMenu) return mpMenu->GetMenuHeight();
	else return 0;
}

void iEdWindow::SetMenu(iEdMenu* apMenu)
{
	mpMenu = apMenu;
	if(mpMenu) mpMenu->SetWindow(this);
}

//---------------------------------------------------------------------

void iEdWindow::SetLayoutBlock(const cLayoutBlock* apBlock)
{
	mpLayoutBlock = apBlock;
}

//---------------------------------------------------------------------

bool iEdWindow::IsActive()
{
	return mpBGWidget && 
		mpBGWidget->IsEnabled() && mpBGWidget->IsVisible();
}

void iEdWindow::SetActive(bool abX)
{
	////////////////////////////////////////////
	// Set background active status
	mpBGWidget->SetEnabled(abX);
	mpBGWidget->SetVisible(abX);

	////////////////////////////////////////////
	// Set shortcuts status
	for(size_t i=0; i<mvShortcuts.size(); ++i)
	{
		cGuiGlobalShortcut* pShortcut = mvShortcuts[i];

		pShortcut->SetEnabled(abX);
	}

	if(mbLayoutCreated==false) return;

	OnSetActive(abX);

	if(abX)	SetUpdated();
	else
	{
		if(mbDestroyOnSetInactive)
			mpEditor->GetWindowHandler()->DestroyWindow(this);
	}
}

//---------------------------------------------------------------------

void iEdWindow::SetPosition(const cVector3f& avX)
{
	if(mpBGWidget->GetLocalPosition()==avX)
		return;

	mpBGWidget->SetPosition(avX);
	OnSetPosition(avX);
}

//---------------------------------------------------------------------

void iEdWindow::SetSize(const cVector2f& avX)
{
	if(mpBGWidget->GetSize()==avX)
		return;

	mpBGWidget->SetSize(avX);
	OnSetSize(avX);
}

//---------------------------------------------------------------------

void iEdWindow::SetWidth(float afX)
{
	const cVector2f vSize = mpBGWidget->GetSize();
	if(vSize.x == afX)
		return;

	if(afX==-1)
		afX = mpSet->GetVirtualSize().x - mpBGWidget->GetGlobalPosition().x;

	mpBGWidget->SetSize(cVector2f(afX, vSize.y));
	OnSetSize(mpBGWidget->GetSize());
}

//---------------------------------------------------------------------

void iEdWindow::SetHeight(float afX)
{
	const cVector2f vSize = mpBGWidget->GetSize();
	if(vSize.y == afX)
		return;

	if(afX==-1)
		afX = mpSet->GetVirtualSize().y - mpBGWidget->GetGlobalPosition().y;

	mpBGWidget->SetSize(cVector2f(vSize.x, afX));
	OnSetSize(mpBGWidget->GetSize());
}

//---------------------------------------------------------------------

void iEdWindow::SetUpdated()
{
	//if(mbUpdated) return;

	//mbUpdated = true;
	mpEditor->GetWindowHandler()->AddWindowToUpdateList(this);	
}

//---------------------------------------------------------------------

bool iEdWindow::EdInputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	return WindowSpecificInputCallback(static_cast<iEdInput*>(apWidget->GetUserData()));
}
kGuiCallbackDeclaredFuncEnd(iEdWindow, EdInputCallback);


//---------------------------------------------------------------------

cEdInputBool* iEdWindow::CreateInputBool(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent)
{
	cEdInputBool* pInput = hplNew(cEdInputBool, (this, asLabel, apParent));
	SetUpInput(pInput, avPos);

	return pInput;
}

//---------------------------------------------------------------------

cEdInputEnum* iEdWindow::CreateInputEnum(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, float afBoxWidth)
{
	cEdInputEnum* pInput = hplNew(cEdInputEnum,(this, asLabel, afBoxWidth, apParent));
	SetUpInput(pInput, avPos);

	return pInput;
}

//---------------------------------------------------------------------

cEdInputText* iEdWindow::CreateInputString(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, float afBoxWidth)
{
	cEdInputText* pInput = hplNew(cEdInputText, (this, asLabel, afBoxWidth, 1, false, false, apParent));
	SetUpInput(pInput, avPos, eEdInpStyle_ColumnLabelOnTop);

	return pInput;
}

//---------------------------------------------------------------------

cEdInputFile* iEdWindow::CreateInputFile(const cVector3f& avPos, const tWString& asLabel, const tWString& asCategory, 
										 iWidget* apParent, float afBoxWidth, const tWString& asStartPath)
{
	cEdInputFile* pInput = hplNew(cEdInputFile,(this, asLabel, afBoxWidth, asCategory, apParent));
	SetUpFileInput(pInput, avPos, eEdInpStyle_ColumnLabelOnTop, asStartPath);

	return pInput;
}

//---------------------------------------------------------------------

cEdInputFile* iEdWindow::CreateInputMesh(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, 
										 float afBoxWidth, const tWString& asStartPath)
{
	cEdInputFile* pInput = hplNew(cEdInputModel,(this, asLabel, false, afBoxWidth, apParent));
	SetUpFileInput(pInput, avPos, eEdInpStyle_ColumnLabelOnTop, asStartPath);

	return pInput;
}

//---------------------------------------------------------------------

cEdInputFile* iEdWindow::CreateInputAnim(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, 
										 float afBoxWidth, const tWString& asStartPath)
{
	cEdInputFile* pInput = hplNew(cEdInputModel,(this, asLabel, true, afBoxWidth, apParent));
	SetUpFileInput(pInput, avPos, eEdInpStyle_ColumnLabelOnTop, asStartPath);

	return pInput;
}

//---------------------------------------------------------------------

cEdInputFile* iEdWindow::CreateInputEntFile(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, 
											float afBoxWidth, const tWString& asStartPath)
{
	cEdInputFile* pInput = hplNew(cEdInputEntity,(this, asLabel, afBoxWidth, apParent));
	SetUpFileInput(pInput, avPos, eEdInpStyle_ColumnLabelOnTop, mpEditor->GetMainLookUpDir(0));

	return pInput;
}

//---------------------------------------------------------------------

cEdInputFile* iEdWindow::CreateInputParticleSystem(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, 
												   float afBoxWidth, const tWString& asStartPath)
{
	cEdInputFile* pInput = hplNew(cEdInputParticleSystem,(this, asLabel, afBoxWidth, apParent));
	SetUpFileInput(pInput, avPos, eEdInpStyle_ColumnLabelOnTop, mpEditor->GetMainLookUpDir(eDir_Particles));

	return pInput;
}

//---------------------------------------------------------------------

cEdInputTexture* iEdWindow::CreateInputMaterial(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, float afBoxWidth, const tWString& asStartPath)
{
	cEdInputTexture* pInput = hplNew(cEdInputTexture,(this, asLabel, afBoxWidth, true, eEdTexture_LastEnum, apParent));
	SetUpFileInput(pInput, avPos, eEdInpStyle_ColumnLabelOnTop, asStartPath);

	return pInput;
}

//---------------------------------------------------------------------

cEdInputTexture* iEdWindow::CreateInputTexture(const cVector3f& avPos, const tWString& asLabel, eEdTexture aType, iWidget* apParent,
												   float afBoxWidth, const tWString& asStartPath)
{
	cEdInputTexture* pInput = hplNew(cEdInputTexture,(this, asLabel, afBoxWidth, false, aType, apParent));
	SetUpFileInput(pInput, avPos, eEdInpStyle_ColumnLabelOnTop, asStartPath);

	return pInput;
}

cEdInputTexture* iEdWindow::CreateInputTexture1D(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, 
												   float afBoxWidth, const tWString& asStartPath)
{
	return CreateInputTexture(avPos, asLabel, eEdTexture_1D, apParent, afBoxWidth, asStartPath);
}

cEdInputTexture* iEdWindow::CreateInputTexture2D(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, 
												   float afBoxWidth, const tWString& asStartPath)
{
	return CreateInputTexture(avPos, asLabel, eEdTexture_2D, apParent, afBoxWidth, asStartPath);
}

cEdInputTexture* iEdWindow::CreateInputCubeMap(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, 
												   float afBoxWidth, const tWString& asStartPath)
{
	return CreateInputTexture(avPos, asLabel, eEdTexture_CubeMap, apParent, afBoxWidth, asStartPath);
}

//---------------------------------------------------------------------

cEdInputSound* iEdWindow::CreateInputSound(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, 
												   float afBoxWidth, const tWString& asStartPath)
{
	cEdInputSound* pInput = hplNew(cEdInputSound,(this, asLabel, afBoxWidth, apParent));
	SetUpFileInput(pInput, avPos, eEdInpStyle_ColumnLabelOnTop, mpEditor->GetMainLookUpDir(eDir_Sounds));

	return pInput;
}

//---------------------------------------------------------------------

cEdInputNumber* iEdWindow::CreateInputNumber(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, 
											 float afBoxWidth, float afNumericAdd)
{
	cEdInputNumber* pInput = hplNew(cEdInputNumber,(this, asLabel, afBoxWidth, apParent, afNumericAdd));
	SetUpInput(pInput, avPos, eEdInpStyle_ColumnLabelOnTop);

	return pInput;
}

//---------------------------------------------------------------------

cEdInputVec2* iEdWindow::CreateInputVec2(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, 
										 float afBoxWidth, const tWStringList& alstLabels, eEdInpStyle aStyle, float afNumericAdd)
{
	cEdInputVec2* pInput = hplNew(cEdInputVec2,(this, asLabel, afBoxWidth, alstLabels, apParent, afNumericAdd));
	SetUpInput(pInput, avPos, aStyle);

	return pInput;
}

//---------------------------------------------------------------------

cEdInputVec3* iEdWindow::CreateInputVec3(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent, 
										 float afBoxWidth, const tWStringList& alstLabels, eEdInpStyle aStyle, float afNumericAdd)
{
	cEdInputVec3* pInput = hplNew(cEdInputVec3,(this, asLabel, afBoxWidth, alstLabels, apParent,afNumericAdd));
	SetUpInput(pInput, avPos, aStyle);

	return pInput;
}

//---------------------------------------------------------------------

cEdInputColor* iEdWindow::CreateInputColor(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent)
{
	cEdInputColor* pInput = hplNew(cEdInputColor,(this, asLabel, apParent));
	SetUpInput(pInput, avPos, eEdInpStyle_RowLabelOnLeft);

	return pInput;
}

//---------------------------------------------------------------------

void iEdWindow::DestroyInput(iEdInput* apInput)
{
	if(apInput)
	{
		mlstInputs.remove(apInput);
		hplDelete(apInput);
	}
}

//---------------------------------------------------------------------

void iEdWindow::DispatchMessage(eEdModuleMsg aMsg, void* apData)
{
	iEdModule::DispatchMessage(aMsg, apData);

	if(mpMenu) mpMenu->DispatchMessage(aMsg, apData);
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// WINDOW - PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

void iEdWindow::AddShortcut(cGuiGlobalShortcut* apShortcut)
{
	if(apShortcut) 
	{
		apShortcut->SetEnabled(IsActive());
		mvShortcuts.push_back(apShortcut);
	}
}

//---------------------------------------------------------------------

void iEdWindow::SetUpInput(iEdInput* apInput, const cVector3f& avPos, eEdInpStyle aStyle)
{
	apInput->SetPosition(avPos);
	apInput->SetStyle(aStyle);
	apInput->UpdateLayout();

	apInput->AddCallback(eEdInputCallback_ValueEnter, this, kGuiCallback(EdInputCallback));

	mlstInputs.push_back(apInput);
}

void iEdWindow::SetUpFileInput(cEdInputFile* apInput, const cVector3f& avPos, eEdInpStyle aStyle, const tWString& asStartPath)
{
	SetUpInput(apInput, avPos, aStyle);
	apInput->SetInitialPath(asStartPath);
}

//---------------------------------------------------------------------

void iEdWindow::OnInit()
{
}

//---------------------------------------------------------------------

void iEdWindow::OnCreateLayout()
{
	cEdWindowHandler* pHandler = mpEditor->GetWindowHandler();
	iEdWindow* pBGWindow = pHandler->GetBackground();

	cVector3f vPos = 0;
	cVector2f vSize = 0;
	iWidget* pParent = NULL;
	if(pBGWindow && pBGWindow!=this)
		pParent = pBGWindow->GetBG();

	if(mpLayoutBlock)
	{
		vPos = mpLayoutBlock->mvPosition;
		vSize = mpLayoutBlock->mvSize;
	}

	mpBGWidget = CreateBGWidget(vPos, vSize, pParent);

	bool bDestroyOnDisable = mbDestroyOnSetInactive;
	SetDestroyOnSetInactive(false);
	SetActive(false);
	SetDestroyOnSetInactive(bDestroyOnDisable);

	mbLayoutCreated = true;
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PANE - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

iEdPane::iEdPane(iEditor* apEditor, const tWString& asName) : iEdWindow(apEditor, asName)
{
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PANE - PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

iWidget* iEdPane::CreateBGWidget(const cVector3f& avPos, const cVector2f& avSize, iWidget* apParent)
{
	cWidgetFrame* pFrame = mpSet->CreateWidgetFrame(avPos, avSize, false, apParent);
	pFrame->SetBackGroundColor(cColor(0.82f, 0.81f, 0.79f,1));
	pFrame->SetDrawBackground(true);
	//pFrame->SetClipActive(false);

	return pFrame;
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// POPUP - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

iEdPopUp::iEdPopUp(iEditor* apEditor, const tWString& asName) : iEdWindow(apEditor, asName)
{
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// POPUP - PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

iWidget* iEdPopUp::CreateBGWidget(const cVector3f& avPos, const cVector2f& avSize, iWidget* apParent)
{
	cGuiSet* pSet = mpEditor->GetGuiSet();
	cWidgetWindow* pWindow = pSet->CreateWidgetWindow(eWidgetWindowButtonFlag_ButtonClose, avPos, avSize, msName, apParent);
	pWindow->AddCallback(eGuiMessage_WindowClose, this, kGuiCallback(PopUpOnClose));

	return pWindow;
}

//---------------------------------------------------------------------

void iEdPopUp::OnSetActive(bool abX)
{
	if(abX)
	{
		iWidget* pBG = GetBG();
		cVector3f vPos = pBG->GetGlobalPosition();
		cVector3f vCenter = cVector3f(mpSet->GetVirtualSize()-pBG->GetSize())*0.5f;
		vPos.x = vCenter.x;
		vPos.y = vCenter.y;

		pBG->SetGlobalPosition(vPos);

		mpSet->SetFocusedWidget(GetBG());
		//mpEditor->GetWindowHandler()->mlEdPopUpCount++;
	}
	else
	{
		//mpEditor->GetWindowHandler()->mlEdPopUpCount--;
	}
}

//---------------------------------------------------------------------

bool iEdPopUp::PopUpOnClose(iWidget* apWidget, const cGuiMessageData& aData)
{
	SetActive(false);
	return true;
}
kGuiCallbackDeclaredFuncEnd(iEdPopUp, PopUpOnClose);

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// EDIT MODE PANE - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

iEdEditModePane::iEdEditModePane(iEdEditMode* apMode) : iEdPane(apMode->GetEditor(), apMode->GetName())
{
	mpEditMode = apMode;
}


//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// WINDOW HANDLER - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

cEdWindowHandler::cEdWindowHandler(iEditor* apEditor) : iEdModule(apEditor, _W("WindowHandler"))
{
	mpBackground = NULL;
}

cEdWindowHandler::~cEdWindowHandler()
{
	//////////////////////////////////////////////////////////////////////////////////////////
	// Removing windows backwards, as background is parent and first in list, 
	// and will fuck up if destroyed first
	while(mlstWindows.empty()==false)
	{
		iEdWindow* pWin = mlstWindows.back();
		mlstWindows.pop_back();
		hplDelete(pWin);
	}
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// WINDOW HANDLER - PUBLIC METHODS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

void cEdWindowHandler::AddWindow(iEdWindow* apWindow, int alLayoutBlockID, bool abActivate)
{
	if(apWindow==NULL) return;

	const cLayoutBlock* pBlock = GetLayoutBlock(alLayoutBlockID);
	apWindow->SetLayoutBlock(pBlock);

	AddWindowToCreateList(apWindow);
	if(abActivate) AddWindowToActivateList(apWindow);
}

//---------------------------------------------------------------------

void cEdWindowHandler::AddBackground(iEdWindow* apWindow)
{
	if(apWindow==NULL || mpBackground!=NULL) return;

	mpBackground = apWindow;
	AddWindow(apWindow, eLayoutBlock_BG);
}

//---------------------------------------------------------------------

void cEdWindowHandler::AddMainWindow(iEdWindow* apWindow, int alLayoutBlockID)
{
	AddWindow(apWindow, alLayoutBlockID, true);
}

//---------------------------------------------------------------------

void cEdWindowHandler::AddPopUp(iEdWindow* apWindow, bool abActivate)
{
	if(apWindow==NULL) return;

	AddWindow(apWindow, -1, abActivate);
    mlstPopups.push_back(apWindow);
}

//---------------------------------------------------------------------

void cEdWindowHandler::DestroyWindow(iEdWindow* apWindow)
{
	if(apWindow==NULL) return;

	mlstWindowsToCreate.remove(apWindow);
	mlstWindowsToActivate.remove(apWindow);
	mlstWindowsToUpdate.remove(apWindow);

	mlstWindowsToDestroy.push_back(apWindow);
}

//---------------------------------------------------------------------

iEdWindow* cEdWindowHandler::GetWindow(const tWString& asName)
{
	tEdWindowListIt it = mlstWindows.begin();
	for(;it!=mlstWindows.end(); ++it)
	{
		iEdWindow* pWin = *it;
		if(pWin->GetName()==asName)
			return pWin;
	}

	return NULL;
}

//---------------------------------------------------------------------

const cLayoutBlock* cEdWindowHandler::AddLayoutBlock(int alID, 
													 bool abRelX, bool abRelY, const cVector3f& avPos, 
													 bool abRelWidth, bool abRelHeight, const cVector2f& avSize, 
													 iWidget* apParent)
{
	if(GetLayoutBlock(alID)==NULL)
	{
		cGuiSet* pSet = mpEditor->GetGuiSet();

		const cVector2f& vScreenSize = pSet->GetVirtualSize();

		cVector3f vPos = avPos;
		cVector2f vSize = avSize;

		if(abRelX)
			vPos.x *= vScreenSize.x;
		if(abRelY)
			vPos.y *= vScreenSize.y;

		if(abRelWidth)
			vSize.x *= vScreenSize.x;
		if(abRelHeight)
			vSize.y *= vScreenSize.y;

		for(int i=0;i<2;++i)
		{
			if(avSize.v[i]==-1)
				vSize.v[i] = vScreenSize.v[i] - vPos.v[i];
		}

		mlstLayoutBlocks.push_back(cLayoutBlock(alID, vPos, vSize, apParent));

		return &mlstLayoutBlocks.back();
	}

	return NULL;
}

//---------------------------------------------------------------------

const cLayoutBlock* cEdWindowHandler::GetLayoutBlock(int alID)
{
	if(alID>=0)
	{
		tLayoutBlockListIt it = mlstLayoutBlocks.begin();
		for(;it!=mlstLayoutBlocks.end();++it)
		{
			const cLayoutBlock& block = *it;
			if(block.mlID==alID)
				return &block;
		}
	}

	return NULL;
}

//---------------------------------------------------------------------

cGuiPopUpFilePicker* cEdWindowHandler::CreateLoadDialog(tWStringVec& avLoadFilenames, const tWString& asStartPath, 
										void* apCallbackObject, tGuiCallbackFunc apCallback, 
										const tWString& asCategoryName, const tWStringList& alstCategoryStrings)
{
	cGuiPopUpFilePicker* pPicker = mpEditor->GetGuiSet()->CreatePopUpLoadFilePicker(avLoadFilenames, 
																	false, 
																	asStartPath, 
																	false, apCallbackObject, apCallback);

	//if(asCategoryName==_W("") || alstCategoryStrings.empty())
    //    pPicker->AddCategory(msFileCategoryName, msFileCategoryString);
	//else
	//{
	int lCat = pPicker->AddCategory(asCategoryName, _W(""));
	tWStringList::const_iterator it = alstCategoryStrings.begin();
	for(;it!=alstCategoryStrings.end();++it)
		pPicker->AddFilter(lCat, *it);

	//pPicker->AddOnDestroyCallback(this, kGuiCallback(PopUpOnClose));

	return pPicker;
}


//---------------------------------------------------------------------

cGuiPopUpFilePicker* cEdWindowHandler::CreateSaveDialog(tWString& asFilename, const tWString& asStartPath, 
														void* apCallbackObject, tGuiCallbackFunc apCallback,
														const tWString& asCategoryName, const tWString& asFilter)
{
	cGuiPopUpFilePicker* pPicker = mpEditor->GetGuiSet()->CreatePopUpSaveFilePicker(asFilename, asCategoryName, 
																					asFilter, asStartPath, false, 
																					apCallbackObject, apCallback);
	return pPicker;
}

//---------------------------------------------------------------------

bool cEdWindowHandler::IsPopUpActive()
{
	return mpEditor->GetGuiSet()->PopUpIsActive() || mlEdPopUpCount>0;
}

//---------------------------------------------------------------------

void cEdWindowHandler::DispatchMessage(eEdModuleMsg aMsg, void* apData)
{
	switch(aMsg)
	{
	case eEdModuleMsg_CreateLayout:
		OnCreateLayout(); break;
	case eEdModuleMsg_Update:
		OnUpdate(); break;
	default:
		iEdModule::DispatchMessage(aMsg, apData);
		BroadcastMsgToWindows(aMsg, apData);
	}
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// WINDOW HANDLER - PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

void cEdWindowHandler::BroadcastMsgToWindows(eEdModuleMsg aMsg, void* apData)
{
	tEdWindowListIt it = mlstWindows.begin();

	for(;it!=mlstWindows.end(); ++it)
	{
		iEdWindow* pWindow = *it;

		pWindow->DispatchMessage(aMsg, apData);
	}
}

void cEdWindowHandler::CreatePendingWindows()
{
	tEdWindowListIt it = mlstWindowsToCreate.begin();
	for(; it!=mlstWindowsToCreate.end(); ++it)
	{
		iEdWindow* pWin = *it;

		pWin->DispatchMessage(eEdModuleMsg_Init, NULL);
		mlstWindows.push_back(pWin);
	}
	it = mlstWindowsToCreate.begin();
	for(; it!=mlstWindowsToCreate.end(); ++it)
	{
		iEdWindow* pWin = *it;

		pWin->DispatchMessage(eEdModuleMsg_CreateLayout, NULL);
	}
	mlstWindowsToCreate.clear();
}

//---------------------------------------------------------------------

void cEdWindowHandler::OnCreateLayout()
{
	CreatePendingWindows();
}

//---------------------------------------------------------------------

void cEdWindowHandler::OnUpdate()
{
	// Create windows
	CreatePendingWindows();

	// Activate windows
	tEdWindowListIt it = mlstWindowsToActivate.begin();
	for(; it!=mlstWindowsToActivate.end(); ++it)
	{
		iEdWindow* pWin = *it;

		pWin->SetActive(true);
	}
	mlstWindowsToActivate.clear();

	// Update windows
	it = mlstWindowsToUpdate.begin();
	for(; it!=mlstWindowsToUpdate.end(); ++it)
	{
		iEdWindow* pWin = *it;

		pWin->DispatchMessage(eEdModuleMsg_Update, NULL);
	}
	mlstWindowsToUpdate.clear();

	// Destroy windows
	it = mlstWindowsToDestroy.begin();
	for(; it!=mlstWindowsToDestroy.end(); ++it)
	{
		iEdWindow* pWin = *it;

		mlstWindows.remove(pWin);
		mlstPopups.remove(pWin);
		hplDelete(pWin);
	}
	mlstWindowsToDestroy.clear();
}

//---------------------------------------------------------------------

void cEdWindowHandler::OnWorldParamsChange()
{
}

//---------------------------------------------------------------------

void cEdWindowHandler::AddWindowToCreateList(iEdWindow* apWindow)
{
	if(apWindow)
	{
		if(apWindow==mpBackground)
			mlstWindowsToCreate.push_front(apWindow);
		else
            mlstWindowsToCreate.push_back(apWindow);
	}
}

//---------------------------------------------------------------------

void cEdWindowHandler::AddWindowToActivateList(iEdWindow* apWindow)
{
	if(apWindow)
		mlstWindowsToActivate.push_back(apWindow);
}

//---------------------------------------------------------------------

void cEdWindowHandler::AddWindowToUpdateList(iEdWindow* apWindow)
{
	if(apWindow)
	{
		mlstWindowsToUpdate.remove(apWindow);
		mlstWindowsToUpdate.push_back(apWindow);
	}
}

//---------------------------------------------------------------------

void cEdWindowHandler::AddWindowToDestroyList(iEdWindow* apWindow)
{
	if(apWindow)
	{
		mlstWindowsToDestroy.remove(apWindow);
		mlstWindowsToDestroy.push_back(apWindow);
	}
}

//---------------------------------------------------------------------

bool cEdWindowHandler::PopUpOnClose(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowHandler, PopUpOnClose);

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------


