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

#include "../Common/EdEditModeSelect.h"

#include "../Common/Editor.h"
#include "../Common/EdWindowViewport.h"
#include "../Common/EdWorld.h"
#include "../Common/EdGrid.h"
#include "../Common/EdSelection.h"

#include "../Common/EdWindowSelect.h"
#include "../Common/EdScnObjEditPane.h"

#include "../Common/EdAction.h"

//#include "../Common/EdHelper.h"

#include "../Common/EdInputHandler.h"
#include "../Common/EdScnObject.h"

#include <algorithm>

//----------------------------------------------------------------------

bool cUIPickMethodRay::PickSpecific(iEdScnObject* apEnt)
{
	return apEnt->CheckRayIntersect(mpViewport, &mvTempPosition, &mTempTriangle);
}

bool cUIPickMethodBox::PickSpecific(iEdScnObject* apEnt)
{
	if(apEnt->Check2DBoxIntersect(mpViewport, mRect, NULL))
	{
		mvTempPosition = apEnt->GetTranslation();
		return true;
	}
	return false;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

cEdEditModeSelect* iEdSelectMode::mpSelect = NULL;

iEdSelectMode::iEdSelectMode(cEdEditModeSelect* apSelect)
{
	if(mpSelect==NULL)
		mpSelect = apSelect;
}

void iEdSelectMode::AddObjectToSelect(iEdScnObject* apObj)
{
	mlstObjectsToSelect.push_back(apObj);
}

void iEdSelectMode::RemoveObjectToSelect(iEdScnObject* apObj)
{
	mlstObjectsToSelect.remove(apObj);
}

void iEdSelectMode::SetObjectsToSelect(const tScnObjList& alstObjects)
{
	mlstObjectsToSelect = alstObjects;
}

void iEdSelectMode::ClearObjects()
{
	mlstObjectsToSelect.clear();
}

cEdSelectMode_SelectMain::cEdSelectMode_SelectMain(cEdEditModeSelect* apSelect) : iEdSelectMode(apSelect)
{
}

bool cEdSelectMode_SelectMain::IsSelectionUpdated(const tScnObjList& alstObjects)
{
	const tScnObjList& lstSelectedObjects = mpSelect->GetSelection()->GetComponents();

	if(alstObjects.size()!=lstSelectedObjects.size())
	{
		SetObjectsToSelect(alstObjects);
		return true;
	}

	ClearObjects();

	bool bRet = false;
	tScnObjList::const_iterator it = alstObjects.begin();
	for(;it!=alstObjects.end();++it)
	{
		iEdScnObject* pObj = *it;
		AddObjectToSelect(pObj);

		if(pObj->IsSelected()==false)
			bRet = true;
	}

	return bRet;
}

cEdSelectMode_SelectAlways::cEdSelectMode_SelectAlways(cEdEditModeSelect* apSelect) : iEdSelectMode(apSelect)
{
}

bool cEdSelectMode_SelectAlways::IsSelectionUpdated(const tScnObjList& alstObjects)
{
	const tScnObjList& lstSelectedObjects = mpSelect->GetSelection()->GetComponents();

	return true;
}

cEdSelectMode_Toggle::cEdSelectMode_Toggle(cEdEditModeSelect* apSelect) : iEdSelectMode(apSelect)
{
}

bool cEdSelectMode_Toggle::IsSelectionUpdated(const tScnObjList& alstObjects)
{
	const tScnObjList& lstSelectedObjects = mpSelect->GetSelection()->GetComponents();

	if(alstObjects.empty())
		return false;

	SetObjectsToSelect(lstSelectedObjects);

	tScnObjList::const_iterator it = alstObjects.begin();
	for(;it!=alstObjects.end(); ++it)
	{
		iEdScnObject* pObj = *it;

		if(pObj->IsSelected()==false)
			AddObjectToSelect(pObj);
		else
			RemoveObjectToSelect(pObj);
	}

	return true;
}

cEdSelectMode_Deselect::cEdSelectMode_Deselect(cEdEditModeSelect* apSelect) : iEdSelectMode(apSelect)
{
}

bool cEdSelectMode_Deselect::IsSelectionUpdated(const tScnObjList& alstObjects)
{
	const tScnObjList& lstSelectedObjects = mpSelect->GetSelection()->GetComponents();

	if(alstObjects.empty())
		return false;

	SetObjectsToSelect(lstSelectedObjects);

	bool bRet = false;
	tScnObjList::const_iterator it = alstObjects.begin();
	for(;it!=alstObjects.end(); ++it)
	{
		iEdScnObject* pObj = *it;

		if(pObj->IsSelected())
		{
			bRet = true;
			RemoveObjectToSelect(pObj);
		}
	}

	return bRet;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//////////////////////////////////////////////////////////////////
// SELECT EDIT MODE
//////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------
//----------------------------------------------------------------------

//----------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------


cEdEditModeSelect::cEdEditModeSelect(iEditor* apEditor) : iEdEditMode(apEditor,_W("Select"))
{
	mfPickingLength = 100;

	mpSelection = hplNew(cEdSelection,(this));

	mpEditPane = NULL;
	mpCurrentCompoundAction = NULL;

	mpModeSelect = hplNew(cEdSelectMode_SelectMain,(this));
	mpModeToggle = hplNew(cEdSelectMode_Toggle,(this));
	mpModeDeselect = hplNew(cEdSelectMode_Deselect,(this));

	mpCurrentManipulator = NULL;

	mpFilter = hplNew(cUIPickFilter,("Selection filter"));
	mpFilter->SetAllPass(true);

	mpPicker = hplNew(cEdObjectPicker,(GetWorld()));

	mpDefaultSelector = hplNew(cEdSelectorMain, (this));
	mpCurrentSelector = NULL;

	mbAllowTransform = true;

	mbCloneOnTransform = false;

	mbUpdateEditPane = false;

	iEdScnObjEditPane::mpSelect = this;
}

cEdEditModeSelect::~cEdEditModeSelect()
{
	hplDelete(mpSelection);
	hplDelete(mpFilter);
	hplDelete(mpDefaultSelector);
}

//----------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

bool cEdEditModeSelect::IsBusy()
{
	return IsSelecting() || IsManipulating();
}

bool cEdEditModeSelect::IsSelecting()
{
	return mpCurrentSelector && mpCurrentSelector->IsBusy();
}

bool cEdEditModeSelect::IsManipulating()
{
	return mbAllowTransform && mpCurrentManipulator && mpCurrentManipulator->IsBusy();
}

void cEdEditModeSelect::SetCurrentSelector(iEdSelector* apSelector)
{
	if(apSelector==NULL) apSelector = mpDefaultSelector;

	if(mpCurrentSelector==apSelector) return;

	if(mpCurrentSelector) mpCurrentSelector->OnSetActive(false);
	mpCurrentSelector = apSelector;
	if(mpCurrentSelector) mpCurrentSelector->OnSetActive(true);
}

void cEdEditModeSelect::SetCurrentManipulator(iScnObjManipulator* apManipulator)
{
	if(mpCurrentManipulator) mpCurrentManipulator->Activate(NULL);

	mpCurrentManipulator = apManipulator;

	if(mpCurrentManipulator) mpCurrentManipulator->Activate(mpSelection);
}

//----------------------------------------------------------------------

iEdSelectMode* cEdEditModeSelect::GetCurrentSelectMode()
{
	if(mbCtrlPressed)
		return mpModeDeselect;
	if(mbShiftPressed)
		return mpModeToggle;

	return mpModeSelect;
}

//----------------------------------------------------------------------

void cEdEditModeSelect::OnReset()
{
	mpSelection->SetChangeEditModeOnChangeSelection(false);
	mpSelection->Clear();
	mpSelection->SetChangeEditModeOnChangeSelection(true);

	mpSelection->SetUpdated(false);

	mbUpdateEditPane = true;

	SetCurrentSelector(NULL);
	if(mpCurrentSelector) mpCurrentSelector->Reset();

	mbCloneOnTransform = false;
}

//----------------------------------------------------------------------

void cEdEditModeSelect::OnUpdate()
{
	if(mbSelectChange)
	{
		mbSelectChange = false;

		if(mpSelection->HasComponents()==false)
			SetCurrentManipulator(NULL);
	}

	if(mbAllowTransform && mpCurrentManipulator && mpSelection->HasComponents())
		mpCurrentManipulator->OnUpdate();

	cInput* pInput = mpEditor->GetEngine()->GetInput();
	int lKeyModifiers = pInput->GetKeyboard()->GetModifier();
	
	mbShiftPressed = (lKeyModifiers&eKeyModifier_Shift)!=0;
	mbCtrlPressed = (lKeyModifiers&eKeyModifier_Ctrl)!=0;
	//mbCloneOnTransform = pInput->IsTriggerd("Duplicate");

	mbPickThrough = pInput->GetMouse()->ButtonIsDown(eMouseButton_Right);
	
	if(mpCurrentSelector)
		mpCurrentSelector->OnUpdate();
	
	if(mbUpdateEditPane)
	{
		mbUpdateEditPane = false;

		if(mpEditPane) 
		{
			mpEditor->GetWindowHandler()->DestroyWindow(mpEditPane);
			mpEditPane = NULL;
		}

		mpEditPane = mpSelection->CreateEditPane();

		if(mpEditPane)
		{
			//static_cast<cEdWindowSelect*>(mpWindow)->PositionEditPane(mpEditPane);
			mpEditor->GetWindowHandler()->AddWindow(mpEditPane, -1, true);
		}
	}
}

//----------------------------------------------------------------------

void cEdEditModeSelect::OnWorldAddRemObject()
{
	OnWorldSelectChange();
}

//----------------------------------------------------------------------

void cEdEditModeSelect::OnWorldObjModify()
{
	if(mpEditPane)
		mpEditPane->SetUpdated();
}

//----------------------------------------------------------------------

void cEdEditModeSelect::OnWorldSelectChange()
{
	mbUpdateEditPane = true;
	mbSelectChange = true;
}

//----------------------------------------------------------------------

void cEdEditModeSelect::OnViewportMouseMove()
{
	mpPicker->SetUpdated();
}

//----------------------------------------------------------------------

void cEdEditModeSelect::OnViewportMouseDown(const cViewportClick& aClick)
{
	/////////////////////////////////////////////////////
	// Check manipulator first
	if(mbShiftPressed==false && mbCtrlPressed==false &&
		mbAllowTransform && mpCurrentManipulator && mpCurrentManipulator->OnViewportMouseDown(aClick))
	{
		if(mpCurrentManipulator->IsBusy())
		{
			mpCurrentCompoundAction = BeginAction();
			
			if(mbCloneOnTransform)
			{
				mpCurrentCompoundAction->AddAction(mpSelection->CreateCloneObjectsAction(true));
				mpCurrentCompoundAction->StepForward();

				mpCurrentManipulator->Activate(mpSelection);
			}
		}
	}
	else if(mpCurrentSelector && mpCurrentSelector->OnViewportMouseDown(aClick))
	{
	}
}

//----------------------------------------------------------------------

void cEdEditModeSelect::OnViewportMouseUp(const cViewportClick& aClick)
{
	/////////////////////////////////////////////
	// If not left mouse button, nothing to do
	if(IsManipulating() && mpCurrentManipulator->OnViewportMouseUp(aClick))
	{
		cEdActionCompound* pAction = GetCurrentAction();
		if(pAction)
		{
			pAction->AddAction(mpCurrentManipulator->CreateAction());
			EndCurrentAction();
		}
	}
	else
	{
		if(IsSelecting())
			mpCurrentSelector->OnViewportMouseUp(aClick);
	}
}

//----------------------------------------------------------------------

void cEdEditModeSelect::OnDraw(const cModuleDrawData& aData)
{
	aData.mpFunctions->SetProgram(NULL);
	aData.mpFunctions->SetTextureRange(NULL,0);
	
	aData.mpFunctions->SetMatrix(NULL);

	mpSelection->OnDraw(aData);

	if(mpCurrentManipulator && IsSelecting()==false && aData.mStep==eEdDrawStep_PostTrans) mpCurrentManipulator->OnDraw(aData);
	if(mpCurrentSelector && IsManipulating()==false && aData.mStep==eEdDrawStep_PostTrans) mpCurrentSelector->Draw(aData);
	
	aData.mpFunctions->SetBlendMode(eMaterialBlendMode_None);
	aData.mpFunctions->SetMatrix(NULL);

	//Debug
	/*
	if(mvTriangle.empty()==false)
	{
		apFunctions->SetDepthTest(false);
		float fColor = 0;
		for(int i=0;i<(int)mvTriangle.size();i+=3)
		{
			fColor = ((float)i)/30.0f;
			apFunctions->GetLowLevelGfx()->DrawLine(mvTriangle[i], mvTriangle[i+1], cColor(fColor,0,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(mvTriangle[i+1], mvTriangle[i+2], cColor(1,0,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(mvTriangle[i+2], mvTriangle[i], cColor(1,0,0,1));
		}
		apFunctions->SetDepthTest(true);
	}*/
	//apFunctions->GetLowLevelGfx()->DrawLine(mvRayStart, mvRayEnd, cColor(0,1,0,1));
	
}


//----------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------


iEdWindow* cEdEditModeSelect::CreateWindow()
{
	return hplNew(cEdWindowSelect,(this));
}


//----------------------------------------------------------------------

void cEdEditModeSelect::OnSetActive(bool abX)
{
	if(mpEditPane)	mpEditPane->SetActive(abX);
	if(abX==false)
	{
		if(mpSelection->HasComponents())
			mpEditor->AddAction(mpSelection->CreateSelectObjectsAction(tScnObjList()));
	}
}

//----------------------------------------------------------------------

cEdActionCompound* cEdEditModeSelect::BeginAction()
{
	mpCurrentCompoundAction = hplNew(cEdActionCompound,(_W("Transformation")));

	return GetCurrentAction();
}

//----------------------------------------------------------------------

cEdActionCompound* cEdEditModeSelect::GetCurrentAction()
{
	return mpCurrentCompoundAction;
}

//----------------------------------------------------------------------

void cEdEditModeSelect::EndCurrentAction()
{
	mpEditor->AddAction(mpCurrentCompoundAction);
	mpCurrentCompoundAction = NULL;
}

//----------------------------------------------------------------------

/*
void cEdEditModeSelect::AddEntityFilter(eEdEntityType aType)
{
    mlstEntityFilters.remove(aType);
	mlstEntityFilters.push_back(aType);

	mpFilter->SetTypeFilter(aType, false);
}
*/
//----------------------------------------------------------------------

//----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// ENTITY SELECTOR BASE
///////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

iEdSelector::iEdSelector(cEdEditModeSelect* apSelect)
{
	mpSelect = apSelect;
	mpRay = hplNew(cUIPickMethodRay,());
}

iEdSelector::~iEdSelector()
{
	if(mpSelect->GetCurrentSelector()==this)
		mpSelect->SetCurrentSelector(NULL);
	
	hplDelete(mpRay);
}

//----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------


//----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// NORMAL ENTITY SELECTOR - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEdSelectorMain::cEdSelectorMain(cEdEditModeSelect* apEditMode) : iEdSelector(apEditMode)
{
	mpBox = hplNew(cUIPickMethodBox,());

	mbIsSelecting = false;
}

//----------------------------------------------------------------------

cEdSelectorMain::~cEdSelectorMain()
{
	hplDelete(mpBox);
}

//----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

bool cEdSelectorMain::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(aClick.mbLeft==false &&
		aClick.mbWheelDn==false && aClick.mbWheelUp==false) return false;
	mbIsSelecting = true;

	const cVector2f& vMousePos = aClick.mvLocalPos;

	mMouseRect.x = (int) vMousePos.x;
	mMouseRect.y = (int) vMousePos.y;

	return true;
}

//----------------------------------------------------------------------

bool cEdSelectorMain::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(mbIsSelecting==false)
		return false;

	mbIsSelecting = false;

	cEdWindowSelect* pWindow = static_cast<cEdWindowSelect*>(mpSelect->GetWindow());
	
	cEdSelection* pSelection = mpSelect->GetSelection();

	////////////////////////////////////////////////////////
	// Set up Mouse parameters
	mpBox->SetMouseBox(mMouseRect);

	cEdObjectPicker* pPicker = mpSelect->GetPicker();

	cUIPickMethod* pMethod = mpBox;
	bool bRayPickingActive = IsRayPickingActive();
	if(bRayPickingActive) pMethod = mpRay;

	pPicker->SetPickMethod(pMethod);
	
	// Set up and perform picking
	cUIPickFilter* pFilter = mpSelect->GetFilter();
	pPicker->SetPickFilter(pFilter);
	
	pPicker->Update();

	tScnObjList lstObjects;

	/////////////////////////////////////////////////////
	// Check if the selection has changed
	iEdSelectMode* pMode = mpSelect->GetCurrentSelectMode();
	
	// Ray picking == single entity
	if(bRayPickingActive)
	{
		int lPickIndex = -1;
		int lNumPicks = pPicker->GetNumPicks();

		if(lNumPicks!=0)
		{
			if(ComparePickVectors(pPicker->GetPicks(), mvPicks))
			{
				//if(mpSelect->GetPickThroughActive())
				//{
					for(int i=0;i<lNumPicks;++i)
					{
						cPickData* pData = pPicker->GetPick(i);
						if(pData->mpObject->IsSelected())
						{
							lPickIndex=i;
							break;
						}
					}

					//if(aClick.mbWheelUp)
						lPickIndex+=1;
					//else if(aClick.mbWheelDn)
					//	lPickIndex-=1;

					if(lPickIndex >= lNumPicks)
						lPickIndex=-1;
				//}
			}
			else
				lPickIndex=0;
		}

		cPickData* pData = pPicker->GetPick(lPickIndex);
		if(pData)
			lstObjects.push_back(pData->mpObject);

		mvPicks = pPicker->GetPicks();
	}
	//////////////////////////////////////
	// Box picking
	else
	{
		for(int i=0;i<pPicker->GetNumPicks();++i)
			lstObjects.push_back(pPicker->GetPick(i)->mpObject);
	}
	
	if(pMode->IsSelectionUpdated(lstObjects))
	{
		const tScnObjList& lstObjectsToSelect = pMode->GetObjectsToSelect();
		iEdAction *pAction = mpSelect->GetSelection()->CreateSelectObjectsAction(lstObjectsToSelect);

		mpSelect->GetEditor()->AddAction(pAction);
	}

	return true;
}

//----------------------------------------------------------------------

void cEdSelectorMain::OnUpdate()
{
	if(mbIsSelecting)
	{
		iEdViewport* pViewport = mpSelect->GetEditor()->GetFocusedViewport();
		const cVector2f& vMouseViewportPos = pViewport->GetMouseViewportPosition();
		mMouseRect.w = (int)vMouseViewportPos.x - mMouseRect.x;
		mMouseRect.h = (int)vMouseViewportPos.y - mMouseRect.y;
	}
}

//----------------------------------------------------------------------

void cEdSelectorMain::Draw(const cModuleDrawData& aData)
{
	///////////////////////////////////////////////////
	// Highlight current selection
	aData.mpFunctions->SetBlendMode(eMaterialBlendMode_None);

	aData.mpFunctions->SetDepthTest(true);
	aData.mpFunctions->SetDepthWrite(true);

	if(aData.mpViewport->IsFocused()==false || mbIsSelecting==false)
		return;

	///////////////////////////////////////////////////
	// If selecting, show the selection rectangle
	if(IsRayPickingActive()==false)
	{
		aData.mpFunctions->GetLowLevelGfx()->PushMatrix(eMatrix_ModelView);
		aData.mpFunctions->GetLowLevelGfx()->PushMatrix(eMatrix_Projection);

		aData.mpFunctions->GetLowLevelGfx()->SetOrthoProjection(aData.mpViewport->GetGuiViewportSize(),-1000,1000);
		aData.mpFunctions->GetLowLevelGfx()->SetIdentityMatrix(eMatrix_ModelView);
		aData.mpFunctions->GetLowLevelGfx()->DrawLineQuad(cRect2f((float)mMouseRect.x,(float)mMouseRect.y,
															(float)mMouseRect.w,(float)mMouseRect.h),
															1000,
															cColor(1,1)); 
		aData.mpFunctions->SetNormalFrustumProjection();

		aData.mpFunctions->GetLowLevelGfx()->PopMatrix(eMatrix_ModelView);
		aData.mpFunctions->GetLowLevelGfx()->PopMatrix(eMatrix_Projection);
	}
}

//--------------------------------------------------------------------

void cEdSelectorMain::OnSetActive(bool abX)
{
	//mpEditMode->SetAllowTransform(abX);
}

//--------------------------------------------------------------------

void cEdSelectorMain::Reset()
{
}

//--------------------------------------------------------------------

bool cEdSelectorMain::IsRayPickingActive()
{
	return (cMath::Abs(mMouseRect.w)<5 && cMath::Abs(mMouseRect.h)<5);
}

//--------------------------------------------------------------------

bool cEdSelectorMain::ComparePickVectors(const tPickVec& av1, const tPickVec& av2)
{
	if(av1.size()!=av2.size())
		return false;

	for(size_t i=0;i<av1.size();++i)
	{
		if(av1[i].mpObject!=av2[i].mpObject)
			return false;
	}

	return true;
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// BODY HIGHLIGHTER - CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdSelectorHighlighter::cEdSelectorHighlighter(cEdEditModeSelect* apEditMode) : iEdSelector(apEditMode)
{
	mpFilter = hplNew(cUIPickFilter,("Highlighter filter"));
	mpObjectUnderPointer = NULL;

	mpMouseDownCallbackObject = NULL;
	mpMouseDownCallback = NULL;

	mpMouseUpCallbackObject = NULL;
	mpMouseUpCallback = NULL;

	mpMouseOverCallbackObject = NULL;
	mpMouseOverCallback = NULL;
}

cEdSelectorHighlighter::~cEdSelectorHighlighter()
{
	hplDelete(mpFilter);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

bool cEdSelectorHighlighter::IsAtLeastOneFilterActive()
{
	return mpFilter->GetNumActiveTypeFilters()>0;
}

//----------------------------------------------------------------------------

void cEdSelectorHighlighter::OnSetActive(bool abX)
{
	mpSelect->SetAllowTransform(!abX);
}

//----------------------------------------------------------------------------

void cEdSelectorHighlighter::OnUpdate()
{
	////////////////////////////////////////////
	// If mouse has not moved, do nothing
	if(mpSelect->GetEditor()->GetInputHandler()->MouseHasMoved()==false)
		return;

	//if(mpEditMode->GetEditor()->GetFlags(eEdFlag_MouseMoved)==false)
	//	return;

	//////////////////////////////////////////////////////
	// Set up variables
	iEdViewport* pViewport = mpSelect->GetEditor()->GetFocusedViewport();
	cEdObjectPicker* pPicker = mpSelect->GetPicker();

	pPicker->SetPickFilter(mpFilter);
	pPicker->SetPickMethod(mpRay);
	
	pPicker->Update();
	mpObjectUnderPointer = pPicker->GetClosestPick();

	if(mpObjectUnderPointer && mpMouseOverCallbackObject && mpMouseOverCallback)
		mpMouseOverCallback(mpMouseOverCallbackObject, NULL, cGuiMessageData());
}

//----------------------------------------------------------------------------

void cEdSelectorHighlighter::Draw(const cModuleDrawData& aData)
{
	if(mpObjectUnderPointer)
		mpObjectUnderPointer->DrawObject(aData, true, cColor(0,0.5f,1,0.75f));

	tScnObjListIt it = mlstHighlightedEntities.begin();
	for(;it!=mlstHighlightedEntities.end();++it)
	{
		iEdScnObject* pObj = *it;
		if(pObj==mpObjectUnderPointer) continue;

		pObj->DrawObject(aData, true, cColor(0, 1, 0, 0.75f));
	}
}

//----------------------------------------------------------------------------

bool cEdSelectorHighlighter::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(mpMouseDownCallbackObject && mpMouseDownCallback)
		return mpMouseDownCallback(mpMouseDownCallbackObject,NULL, cGuiMessageData());

	return false;
}

//----------------------------------------------------------------------------

bool cEdSelectorHighlighter::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(mpMouseUpCallbackObject && mpMouseUpCallback)
		return mpMouseUpCallback(mpMouseUpCallbackObject,NULL, cGuiMessageData());

	return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

void cEdSelectorHighlighter::SetMouseDownCallback(void *apCallbackObject, tGuiCallbackFunc apCallback)
{
	mpMouseDownCallbackObject = apCallbackObject;
	mpMouseDownCallback = apCallback;
}



//----------------------------------------------------------------------------

void cEdSelectorHighlighter::SetMouseUpCallback(void *apCallbackObject, tGuiCallbackFunc apCallback)
{
	mpMouseUpCallbackObject = apCallbackObject;
	mpMouseUpCallback = apCallback;
}


//----------------------------------------------------------------------------

void cEdSelectorHighlighter::SetMouseOverCallback(void *apCallbackObject, tGuiCallbackFunc apCallback)
{
	mpMouseOverCallbackObject = apCallbackObject;
	mpMouseOverCallback = apCallback;
}

//----------------------------------------------------------------------------

void cEdSelectorHighlighter::SetHighlightedEntities(const tScnObjList& alstHighlightedEntities)
{
	mlstHighlightedEntities = alstHighlightedEntities;
}

//----------------------------------------------------------------------------

void cEdSelectorHighlighter::AddHighlightedEntity(iEdScnObject* apObj)
{
	if(apObj==NULL)
		return;

	tScnObjListIt it = find(mlstHighlightedEntities.begin(), mlstHighlightedEntities.end(), apObj);
	if(it==mlstHighlightedEntities.end())
		mlstHighlightedEntities.push_back(apObj);
}

//----------------------------------------------------------------------------

void cEdSelectorHighlighter::RemoveHighlightedEntity(iEdScnObject* apObj)
{
	if(apObj==NULL)
		return;

	tScnObjListIt it = find(mlstHighlightedEntities.begin(), mlstHighlightedEntities.end(), apObj);
	if(it!=mlstHighlightedEntities.end())
		mlstHighlightedEntities.erase(it);
}

//----------------------------------------------------------------------------

void cEdSelectorHighlighter::ClearHighlightedEntities()
{
	mlstHighlightedEntities.clear();
}

//----------------------------------------------------------------------------





