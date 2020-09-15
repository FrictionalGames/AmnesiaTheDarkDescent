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

#include "EditorEditModeSelect.h"

#include "EditorBaseClasses.h"
#include "EditorWindowViewport.h"
#include "EditorWorld.h"
#include "EditorGrid.h"
#include "EditorSelection.h"

#include "EditorWindowFactory.h"
#include "EditorWindowSelect.h"
#include "EditorWindowEntityEditBox.h"
#include "EditorWindowEntityEditBoxGroup.h"

#include "EditorAction.h"
#include "EditorActionEntity.h"
#include "EditorActionCompoundObject.h"

#include "EditorHelper.h"

#include "EditorEditModeSelectToolTranslate.h"
#include "EditorEditModeSelectToolRotate.h"
#include "EditorEditModeSelectToolScale.h"


#include "EntityWrapper.h"
#include "EntityWrapperCompoundObject.h"

#include <algorithm>


//----------------------------------------------------------------------

/////////////////////////////////////////////////////

cUIPickFilter::cUIPickFilter(const tString& asName)
{
	msName = asName;
	mpViewport = NULL;
	mbAllPass = false;
}

//----------------------------------------------------------------------

void cUIPickFilter::Init()
{
}

//----------------------------------------------------------------------

bool cUIPickFilter::Passes(iEntityWrapper* apEntity)
{
	return (FirstPass(apEntity) && SecondPass(apEntity));
}

//----------------------------------------------------------------------

void cUIPickFilter::SetTypeFilter(int aType, bool abX)
{
	mmapTypeFilters[aType] = abX;
}

//----------------------------------------------------------------------

bool cUIPickFilter::GetTypeFilter(int aType)
{
	std::map<int, bool>::const_iterator it = mmapTypeFilters.find(aType);
	if(it==mmapTypeFilters.end())
		return false;

	return it->second;
}

//----------------------------------------------------------------------

int cUIPickFilter::GetNumTypeFilters()
{
	return (int)mmapTypeFilters.size();
}

int cUIPickFilter::GetNumActiveTypeFilters()
{
	int lCount = 0;

	// Count how many bools are set to true in the filters map
	std::map<int, bool>::iterator it = mmapTypeFilters.begin();
	for(;it!=mmapTypeFilters.end();++it)
	{
		if(it->second)
			++lCount;
	}

	return lCount;
}

//----------------------------------------------------------------------

bool cUIPickFilter::FirstPass(iEntityWrapper* apEntity)
{
	if(apEntity->IsVisible()==false)
		return false;

	bool bPasses = GetTypeFilter(apEntity->GetTypeID());

	if(apEntity->BelongsToCompoundObject())
	{
		if(mbAllPass)
			bPasses = false;
	}
	else
	{
		bPasses = apEntity->EntitySpecificFilterCheck(mbAllPass, bPasses);
	}

	if(bPasses) OnFirstPass(apEntity);

	return bPasses;
}

bool cUIPickFilter::SecondPass(iEntityWrapper* apEntity)
{
	if(apEntity->IsCulledByFrustum(mpViewport->GetCamera())==false)
	{
		OnSecondPass(apEntity);
		return true;
	}

	return false;
}

//----------------------------------------------------------------------

cUIPickMethod::cUIPickMethod()
{
	mpViewport = NULL;

	mpEntity = NULL;
	mfTempDistance = 0;
	mvTempPosition = 0;
}

cPickData cUIPickMethod::GetPickData()
{
	cPickData data;

	data.mpEntity = mpEntity;
	data.mfDistance = mfTempDistance;
	data.mvIntersection = mvTempPosition;
	data.mvTriangle = mTempTriangle;

	return data;
}

void cUIPickMethod::PreIterateSetUp()
{
}

bool cUIPickMethod::Picks(iEntityWrapper* apEnt)
{
	if(PickSpecific(apEnt))
	{
		mpEntity = apEnt;
		mfTempDistance = cMath::Vector3Dist(mpViewport->GetUnprojectedStart(), mvTempPosition);

		return true;
	}

	return false;
}

void cUIPickMethod::PostPickCleanUp()
{
	mpEntity = NULL;
	mTempTriangle.clear();
}

//----------------------------------------------------------------------

bool cUIPickMethodRay::PickSpecific(iEntityWrapper* apEnt)
{
	return apEnt->CheckRayIntersect(mpViewport, &mvTempPosition, &mTempTriangle);
}

bool cUIPickMethodBox::PickSpecific(iEntityWrapper* apEnt)
{
	if(apEnt->Check2DBoxIntersect(mpViewport, mRect, NULL))
	{
		mvTempPosition = apEnt->GetPosition();
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


cEditorEditModeSelect::cEditorEditModeSelect(iEditorBase* apEditor, 
											 iEditorWorld* apEditorWorld) : iEditorEditMode(apEditor,"Select")
{
	mpEditorWorld = apEditorWorld;
	mpType = hplNew(cEntityWrapperTypeCompoundObject,());
	mpEditorWorld->AddEntityType(mpType);
	
	mpEditBox = NULL;

	mfPickingLength = 100;

	cEditorSelection* pSelection = mpEditor->GetSelection();
	pSelection->ClearEntities();

	mvTools.resize(eSelectToolMode_LastEnum+1);
	mvTools[eSelectToolMode_Translate] = hplNew(cEditorEditModeSelectToolTranslate,(this,pSelection));
	mvTools[eSelectToolMode_Rotate] = hplNew(cEditorEditModeSelectToolRotate,(this,pSelection));
	mvTools[eSelectToolMode_Scale] = hplNew(cEditorEditModeSelectToolScale,(this,pSelection));
	mvTools[eSelectToolMode_LastEnum] = NULL;

	mpCurrentTool = mvTools[eSelectToolMode_Translate];

	mpFilter = hplNew(cUIPickFilter,("EditModeSelect filter"));
	mpFilter->SetAllPass(true);

	mpCurrentSelector = NULL;
	mpDefaultSelector = hplNew(cEntitySelectorNormal, (this));
	SetCurrentEntitySelector(NULL);

	mbToggleSelect = false;
	mbAlwaysDeselect = false;
	mbCloneOnTransform = false;

	mbEditBoxNeedsRefresh = true;

	mpCurrentCompoundAction = NULL;
}

cEditorEditModeSelect::~cEditorEditModeSelect()
{
	for(int i=0; i< eSelectToolMode_LastEnum;++i)
	{
		hplDelete(mvTools[i]);
	}

	hplDelete(mpFilter);
	hplDelete(mpDefaultSelector);
}

//----------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cEditorEditModeSelect::SetCurrentEntitySelector(iEntitySelector* apSelector)
{
	if(apSelector==NULL) 
		apSelector = mpDefaultSelector;
	if(apSelector == mpCurrentSelector)
		return;

	
	if(mpCurrentSelector)
		mpCurrentSelector->OnSetCurrent(false);
	
	mpCurrentSelector = apSelector;

	if(mpCurrentSelector)
		mpCurrentSelector->OnSetCurrent(true);
}

//----------------------------------------------------------------------

eSelectActionType cEditorEditModeSelect::GetSelectActionType()
{
	if(mbToggleSelect)
		return eSelectActionType_Toggle;
	else if(mbAlwaysDeselect)
		return eSelectActionType_Deselect;

	return eSelectActionType_Select;
}

//----------------------------------------------------------------------

void cEditorEditModeSelect::PostEditModesCreation()
{
	for(int i=0;i<mpEditorWorld->GetEntityTypeNum();++i)
	{
		iEntityWrapperType* pType = mpEditorWorld->GetEntityType(i);
		AddEntityFilter((eEditorEntityType)pType->GetID());
	}
}

//----------------------------------------------------------------------

void cEditorEditModeSelect::Reset()
{
	mpEditor->GetSelection()->ClearEntities();

	mpCurrentTool = mvTools[eSelectToolMode_Translate];
	if(mpCurrentTool) mpCurrentTool->Reset();

	SetCurrentEntitySelector(NULL);
	if(mpCurrentSelector) mpCurrentSelector->Reset();

	mbToggleSelect = false;
	mbAlwaysDeselect = false;
	mbCloneOnTransform = false;

	mbEditBoxNeedsRefresh = true;
}

//----------------------------------------------------------------------

void cEditorEditModeSelect::OnEditorUpdate(float afTimeStep)
{
	cEditorSelection* pSelection = mpEditor->GetSelection();
	cInput* pInput = mpEditor->GetEngine()->GetInput();
	int lKeyModifiers = pInput->GetKeyboard()->GetModifier();
	
	mbToggleSelect = (lKeyModifiers&eKeyModifier_Shift)!=0;
	mbAlwaysDeselect = (lKeyModifiers&eKeyModifier_Ctrl)!=0;
	mbCloneOnTransform = pInput->IsTriggerd("Duplicate");

	if(mpCurrentTool && mbAllowTransform && pSelection->IsEmpty()==false)
		mpCurrentTool->OnEditorUpdate();
	
	if(mpCurrentSelector)
		mpCurrentSelector->OnEditorUpdate();
	
	if(mpEditor->GetSelection()->IsUpdated())
	{
		ShowEditBox();

		mpEditor->GetSelection()->SetUpdated(false);
	}
}

//----------------------------------------------------------------------

void cEditorEditModeSelect::OnViewportMouseDown(int alButtons)
{
	if((alButtons&eGuiMouseButton_Left)==0)
		return;

	if(mbAlwaysDeselect==false && mbToggleSelect==false && 
		mbAllowTransform && mpCurrentTool && mpCurrentTool->OnViewportMouseDown(alButtons))
	{
		cEditorSelection* pSelection = mpEditor->GetSelection();
		pSelection->UpdateReferenceTransforms();

		if(mpEditor->GetFlags(eEditorFlag_Editing))
		{
			mpCurrentCompoundAction = hplNew(cEditorActionCompoundAction,("Transformation"));

			if(mbCloneOnTransform)
			{
				mpCurrentCompoundAction->AddAction(CreateCloneEntitiesAction(pSelection->GetEntityIDs()));
				mpCurrentCompoundAction->StepForward();
				mpEditor->SetLayoutNeedsUpdate(true);
			}
		}
	}
	else
	{
		mpEditor->SetFlags(eEditorFlag_Selecting,true);
		if(mpCurrentSelector) mpCurrentSelector->OnViewportMouseDown();
	}
}

//----------------------------------------------------------------------

void cEditorEditModeSelect::OnViewportMouseUp(int alButtons)
{
	/////////////////////////////////////////////
	// If not left mouse button, nothing to do
	if((alButtons&eGuiMouseButton_Left)==0)
		return;

	///////////////////////////////////////////////////////////////////
	// If transforms are allowed and the tool is working, use tool, else use selector
	if(mbAllowTransform && mpCurrentTool && mpCurrentTool->OnViewportMouseUp(alButtons))
	{
		cEditorActionCompoundAction* pAction = GetCurrentCompoundAction();
		if(pAction)
		{
			pAction->AddAction(mpCurrentTool->CreateAction());
			EndCurrentCompoundAction();
		}
	}
	else
	{
		if(mpCurrentSelector) 
		{
			mpCurrentSelector->OnViewportMouseUp();
		}
	}

	mpEditor->SetFlags(eEditorFlag_Selecting | eEditorFlag_Editing, false);
}

//----------------------------------------------------------------------

void cEditorEditModeSelect::DrawPostGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions *apFunctions, const cVector3f &avPos)
{
	iEditorEditMode::DrawPostGrid(apViewport,apFunctions,avPos);

	apFunctions->SetProgram(NULL);
	apFunctions->SetTextureRange(NULL,0);
	
	apFunctions->SetMatrix(NULL);

	if(mpCurrentSelector)
		mpCurrentSelector->Draw(apViewport, apFunctions);

	if(mpCurrentTool && mbAllowTransform)
		mpCurrentTool->Draw(apViewport,apFunctions);

	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetMatrix(NULL);

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

iEditorWindow* cEditorEditModeSelect::CreateSpecificWindow()
{
	return hplNew(cEditorWindowSelect,(this));
}

//----------------------------------------------------------------------

void cEditorEditModeSelect::ShowEditBox()
{
	cEditorWindowSelect* pWin = (cEditorWindowSelect*)mpWindow;
	///////////////////////////////////////////
	// Destroy old EditBox (if any)
	if(mpEditBox!=NULL) 
	{
		mpEditBox->SetActive(false);
		cEditorWindowFactory::DestroyEditorWindow((iEditorWindow*)mpEditBox);
		mpEditBox = NULL;

		((cEditorWindowSelect*)mpWindow)->SetSelectableTools(true,true,true);
	}

	//////////////////////////////////////////
	// Open new one according to selection
	if(mpEditor->GetSelection()->IsEmpty()==false)
	{
		mpEditBox = CreateEditBoxWindow();
		if(mpEditBox)
			mpEditBox->SetActive(true);
	}
}

//----------------------------------------------------------------------

cEditorWindowEntityEditBox* cEditorEditModeSelect::CreateEditBoxWindow()
{
	cEditorWindowEntityEditBox* pWindow = NULL;
	cEditorSelection* pSelection = mpEditor->GetSelection();

	tEntityWrapperList& lstEntities = pSelection->GetEntities();
	bool bCanTranslate = pSelection->CanTranslate();
	bool bCanRotate = pSelection->CanRotate();
	bool bCanScale = pSelection->CanScale();

	iEntityWrapper* pFirstEnt = lstEntities.front();
	

	///////////////////////////////////////////////////////////
	// If only one entity in selection, show specific EditBox
	if(lstEntities.size()==1)
	{
		pWindow = pFirstEnt->CreateEditBox(this);
	}
	///////////////////////////////////////////////////////////
	// If more than one, show group EditBox
	else if(lstEntities.size()>1)
	{
		if(pSelection->IsHomogeneousSelection())
			pWindow = pFirstEnt->CreateGroupAllSameTypeEditBox(this, lstEntities);
		else
			pWindow = hplNew(cEditorWindowEntityEditBoxGroup,(this, lstEntities));
	}
	else
	{
		bCanTranslate = false;
		bCanRotate = false;
		bCanScale = false;
	}

	((cEditorWindowSelect*)mpWindow)->SetSelectableTools(bCanTranslate, bCanRotate, bCanScale);
	/////////////////////////////
	// Check if pointer is valid
	if(pWindow==NULL)
		return NULL;

	pWindow->Init();
	mpEditor->AddWindow(pWindow);
	pWindow->SetPosition(mpWindow->GetPosition() + cVector2f(0,mpWindow->GetSize().y));

	return pWindow;
}

//----------------------------------------------------------------------

void cEditorEditModeSelect::SetSelectToolMode(eSelectToolMode aMode)
{
	if(mpCurrentTool==mvTools[aMode])
		return;

	mpCurrentTool = mvTools[aMode];
	if(mpCurrentTool)
		mpCurrentTool->Reset();		
}

void cEditorEditModeSelect::OnSetCurrent(bool abX)
{
	if(abX == false)
	{
		if(mpEditBox!=NULL)
		{
			mpEditBox->SetActive(false);
			cEditorWindowFactory::DestroyEditorWindow((iEditorWindow*)mpEditBox);
			mpEditBox = NULL;
		}

		if(mpEditor->GetSelection()->IsEmpty()==false)
		{
			tIntList lstIDs;
			mpEditor->AddAction(CreateSelectEntityAction(lstIDs,eSelectActionType_Clear));
		}
	}
	else
	{
		((cEditorWindowSelect*)mpWindow)->SetSelectableTools(false,false,false);
	}
}

//----------------------------------------------------------------------

iEditorAction* cEditorEditModeSelect::CreateDeleteEntitiesAction(const tIntList& alstEntityIDs)
{
	if(alstEntityIDs.empty())
		return NULL;

	iEditorAction* pAction = hplNew(cEditorActionEntityDelete,(mpEditorWorld, mpEditor->GetSelection(), alstEntityIDs));
	return pAction;
}

//----------------------------------------------------------------------

iEditorAction* cEditorEditModeSelect::CreateCloneEntitiesAction(const tIntList& alstEntityIDs)
{
	if(alstEntityIDs.empty())
		return NULL;

	iEditorAction* pAction = hplNew(cEditorActionEntityClone,(mpEditorWorld, mpEditor->GetSelection(), alstEntityIDs));
	return pAction;
}
//----------------------------------------------------------------------

iEditorAction* cEditorEditModeSelect::CreateSelectEntityAction(const tIntList& alstEntityIDs, eSelectActionType aType)
{
	cEditorSelection* pSelection = mpEditor->GetSelection();

	bool bListsEqual = (alstEntityIDs==pSelection->GetEntityIDs());

	if(aType==eSelectActionType_Select)
	{
		//if(alstEntityIDs.size()==1)
		//{
		//	int lID = alstEntityIDs.front();
		//	if(pSelection->HasEntity(lID))
		//		aType=eSelectActionType_Toggle;
		//}
		//else
		//{
			if(bListsEqual)
				return NULL;
		//}
	}
	else if(aType==eSelectActionType_Clear || aType==eSelectActionType_Deselect)
	{
		if(pSelection->IsEmpty())
			return NULL;
	}

	return hplNew(cEditorActionEntitySelect,(this, alstEntityIDs,aType));
}

//----------------------------------------------------------------------

iEditorAction* cEditorEditModeSelect::CreateCompoundObjectAction()
{
	iEditorAction* pAction = NULL;

	cEditorSelection* pSelection = mpEditor->GetSelection();

	tIntList lstEntityIDs;

	int lCompoundObjectsFound = 0;
	bool bDetachObjectsFromCompound = true;

	tEntityWrapperList& lstEntities = pSelection->GetEntities();
	tEntityWrapperListIt it = lstEntities.begin();
	for(;it!=lstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;

		if(pEnt->GetTypeID()==eEditorEntityType_Compound)
		{
			++lCompoundObjectsFound;
			cEntityWrapperCompoundObject* pCompound = (cEntityWrapperCompoundObject*)pEnt;
			cEditorHelper::GetIDsFromEntityList(pCompound->GetComponents(), lstEntityIDs, false);
		}
		else
		{
			if(pEnt->GetCompoundObject()==NULL)
				bDetachObjectsFromCompound = false;

			lstEntityIDs.push_back(pEnt->GetID());
		}
	}

	if(lstEntityIDs.empty())
		return NULL;

	if(bDetachObjectsFromCompound)
	{
		if(lCompoundObjectsFound==1)
			pAction = hplNew(cEditorActionCompoundObjectRemoveEntities,(mpEditorWorld, lstEntityIDs));
	}
	else
	{
		// XXX
		cEditorActionCompoundAction* pCompoundAction = hplNew(cEditorActionCompoundAction,("Create compound object"));
		int lNewCompoundID = mpEditorWorld->GetFreeID();
		iEntityWrapperData* pData = mpType->CreateData();
		pData->SetID(lNewCompoundID);
		pData->SetName(mpEditorWorld->GenerateName(pData->GetName()));

		pCompoundAction->AddAction(hplNew(cEditorActionObjectCreate,(mpEditorWorld, pData)));
		pCompoundAction->AddAction(hplNew(cEditorActionCompoundObjectAddEntities,(mpEditorWorld, lNewCompoundID, lstEntityIDs)));
		pCompoundAction->AddAction(CreateSelectEntityAction(tIntList(1,lNewCompoundID), eSelectActionType_Select));

		pAction = pCompoundAction;
	}

	return pAction;
}

//----------------------------------------------------------------------

cEditorActionCompoundAction* cEditorEditModeSelect::BeginCompoundAction()
{
	mpCurrentCompoundAction = hplNew( cEditorActionCompoundAction,("Transformation"));

	return mpCurrentCompoundAction;
}

//----------------------------------------------------------------------

cEditorActionCompoundAction* cEditorEditModeSelect::GetCurrentCompoundAction()
{
	return mpCurrentCompoundAction;
}

//----------------------------------------------------------------------

void cEditorEditModeSelect::EndCurrentCompoundAction()
{
	mpEditor->AddAction(mpCurrentCompoundAction);
	mpCurrentCompoundAction = NULL;
}

//----------------------------------------------------------------------

void cEditorEditModeSelect::AddEntityFilter(eEditorEntityType aType)
{
    mlstEntityFilters.remove(aType);
	mlstEntityFilters.push_back(aType);

	mpFilter->SetTypeFilter(aType, false);
}

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

iEntitySelector::iEntitySelector(cEditorEditModeSelect* apEditMode)
{
	mpEditMode = apEditMode;
	mpRay = hplNew(cUIPickMethodRay,());
}

iEntitySelector::~iEntitySelector()
{
	if(mpEditMode->GetCurrentEntitySelector()==this)
		mpEditMode->SetCurrentEntitySelector(NULL);

	hplDelete(mpRay);
}

//----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------


//----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// NORMAL ENTITY SELECTOR
///////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEntitySelectorNormal::cEntitySelectorNormal(cEditorEditModeSelect* apEditMode) : iEntitySelector(apEditMode)
{
	mpBox = hplNew(cUIPickMethodBox,());

	mbIsSelecting = false;
}

//----------------------------------------------------------------------

cEntitySelectorNormal::~cEntitySelectorNormal()
{
	hplDelete(mpBox);
}

//----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

bool cEntitySelectorNormal::OnViewportMouseDown()
{
	iEditorBase* pEditor = mpEditMode->GetEditor();
	if(pEditor->GetFlags(eEditorFlag_Selecting)==false)
		return false;

	mbIsSelecting = true;

	cEditorWindowViewport* pViewport = mpEditMode->GetEditor()->GetFocusedViewport();

	cVector2f vMousePos = pViewport->GetMouseViewportPosition();

	mMouseRect.x = (int) vMousePos.x;
	mMouseRect.y = (int) vMousePos.y;

	return true;
}

//----------------------------------------------------------------------

bool cEntitySelectorNormal::OnViewportMouseUp()
{
	if(mbIsSelecting==false)
		return false;

	mbIsSelecting = false;

	cEditorWindowViewport* pViewport = mpEditMode->GetEditor()->GetFocusedViewport();
	cEditorWindowSelect* pWSelect = (cEditorWindowSelect*)mpEditMode->GetEditorWindow();

	cEntityPicker* pPicker = mpEditMode->GetEditorWorld()->GetPicker();
	
	cEditorSelection* pSelection = mpEditMode->GetEditor()->GetSelection();

	////////////////////////////////////////////////////////
	// Set up Mouse parameters
	mpBox->SetMouseBox(mMouseRect);

	bool bRayPickingActive = IsRayPickingActive();
	cUIPickMethod* pMethod = bRayPickingActive ? (cUIPickMethod*)mpRay : (cUIPickMethod*)mpBox;

	// Set up and perform picking
	tIntList lstEntityIDs;
	cUIPickFilter* pFilter = mpEditMode->GetFilter();
	pFilter->SetViewport(pViewport);
	pPicker->SetPickFilter(pFilter);
	pMethod->SetViewport(pViewport);
	pPicker->SetPickMethod(pMethod);

	pPicker->Update();

	/////////////////////////////////////////////////////
	// Check if the selection has changed
	eSelectActionType type = mpEditMode->GetSelectActionType();
	bool bMustUpdateSelection = false;

	// Ray picking == single entity
	if(bRayPickingActive)
	{
		bMustUpdateSelection = true;

		if(ComparePickVectors(pPicker->GetPicks(), mvPicks)==false)
		{
			iEntityWrapper* pEnt = pPicker->GetClosestPick();
			if(pEnt)
                lstEntityIDs.push_back(pEnt->GetID());
		}
		else
		{
			int lPickIndex = 0;
			for(int i=0;i<pPicker->GetNumPicks();++i)
			{
				if(type==eSelectActionType_Select)
				{
					cPickData* pData = pPicker->GetPick(i);
					if(pSelection->HasEntity(pData->mpEntity))
					{
						lPickIndex=i+1;
						break;
					}
				}
			}
			cPickData* pData = pPicker->GetPick(lPickIndex);
			if(pData)
				lstEntityIDs.push_back(pData->mpEntity->GetID());
		}

		mvPicks = pPicker->GetPicks();
	}
	//////////////////////////////////////
	// Box picking
	else
	{
		for(int i=0;i<pPicker->GetNumPicks();++i)
			lstEntityIDs.push_back(pPicker->GetPick(i)->mpEntity->GetID());

		int lSelectionSize = pSelection->GetNumEntities();
		int lBoxSelectionSize = (int)lstEntityIDs.size();

		if(lSelectionSize != lBoxSelectionSize)
			bMustUpdateSelection = true;
		else
		{
			if(type!=eSelectActionType_Select)
				bMustUpdateSelection = true;
			else
			{
				tIntListIt it = lstEntityIDs.begin();
				for(;it!=lstEntityIDs.end(); ++it)
				{
					int lEntityID = *it;
					if(pSelection->HasEntity(lEntityID)==false)
					{
						bMustUpdateSelection = true;
						break;
					}
				}
			}
		}
	}
	

	if(bMustUpdateSelection)
	{
		iEditorAction *pAction = mpEditMode->CreateSelectEntityAction(lstEntityIDs, type);

		mpEditMode->GetEditor()->AddAction(pAction);
	}

	return true;
}

//----------------------------------------------------------------------

void cEntitySelectorNormal::OnEditorUpdate()
{
	if(mbIsSelecting)
	{
		cEditorWindowViewport* pViewport = mpEditMode->GetEditor()->GetFocusedViewport();
		const cVector2f& vMouseViewportPos = pViewport->GetMouseViewportPosition();
		mMouseRect.w = (int)vMouseViewportPos.x - mMouseRect.x;
		mMouseRect.h = (int)vMouseViewportPos.y - mMouseRect.y;
	}
}

//----------------------------------------------------------------------

void cEntitySelectorNormal::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions)
{

	///////////////////////////////////////////////////
	// Highlight current selection
	apFunctions->SetBlendMode(eMaterialBlendMode_None);

	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	// Iterate through entities and draw them selected
	cEditorSelection* pSelection = mpEditMode->GetEditor()->GetSelection();
	tEntityWrapperListIt selectionIt = pSelection->GetEntities().begin();
	for(;selectionIt!=pSelection->GetEntities().end();++selectionIt)
	{
		iEntityWrapper* pEnt = *selectionIt;

		if(pEnt->IsVisible())
			pEnt->Draw(apViewport,apFunctions, mpEditMode, true);
	}

	if(apViewport->IsFocused()==false || mbIsSelecting==false)
		return;

	///////////////////////////////////////////////////
	// If selecting, show the selection rectangle
	if(IsRayPickingActive()==false)
	{
		apFunctions->SetDepthTest(false);
		apFunctions->GetLowLevelGfx()->SetOrthoProjection(apViewport->GetGuiViewportSize(),-1000,1000);
		apFunctions->GetLowLevelGfx()->SetIdentityMatrix(eMatrix_ModelView);
		apFunctions->GetLowLevelGfx()->DrawLineQuad(cRect2f((float)mMouseRect.x,(float)mMouseRect.y,
															(float)mMouseRect.w,(float)mMouseRect.h),
															0,
															cColor(1,1)); 
		apFunctions->SetNormalFrustumProjection();
	}
}

//--------------------------------------------------------------------

void cEntitySelectorNormal::OnSetCurrent(bool abX)
{
	mpEditMode->SetAllowTransform(abX);
}

//--------------------------------------------------------------------

void cEntitySelectorNormal::Reset()
{
}

//--------------------------------------------------------------------

bool cEntitySelectorNormal::IsRayPickingActive()
{
	return (cMath::Abs(mMouseRect.w)<5 && cMath::Abs(mMouseRect.h)<5);
}

//--------------------------------------------------------------------

bool cEntitySelectorNormal::ComparePickVectors(const tPickVec& av1, const tPickVec& av2)
{
	if(av1.size()!=av2.size())
		return false;

	for(int i=0;i<(int)av1.size();++i)
	{
		if(av1[i].mpEntity!=av2[i].mpEntity)
			return false;
	}

	return true;
}

//--------------------------------------------------------------------

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// BODY HIGHLIGHTER
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEntitySelectorHighlighter::cEntitySelectorHighlighter(cEditorEditModeSelect* apEditMode) : iEntitySelector(apEditMode)
{
	mpFilter = hplNew(cUIPickFilter,("Highlighter filter"));
	mpEntityUnderPointer = NULL;

	mpMouseDownCallbackObject = NULL;
	mpMouseDownCallback = NULL;

	mpMouseUpCallbackObject = NULL;
	mpMouseUpCallback = NULL;

	mpMouseOverCallbackObject = NULL;
	mpMouseOverCallback = NULL;
}

cEntitySelectorHighlighter::~cEntitySelectorHighlighter()
{
	hplDelete(mpFilter);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

bool cEntitySelectorHighlighter::IsAtLeastOneFilterActive()
{
	return mpFilter->GetNumActiveTypeFilters()>0;
}

//----------------------------------------------------------------------------

void cEntitySelectorHighlighter::OnSetCurrent(bool abX)
{
	mpEditMode->SetAllowTransform(false);
}

//----------------------------------------------------------------------------

void cEntitySelectorHighlighter::OnEditorUpdate()
{
	////////////////////////////////////////////
	// If mouse has not moved, do nothing
	if(mpEditMode->GetEditor()->GetFlags(eEditorFlag_MouseMoved)==false)
		return;

	//////////////////////////////////////////////////////
	// Set up variables
	cEditorWindowViewport* pViewport = mpEditMode->GetEditor()->GetFocusedViewport();
	cEntityPicker* pPicker = mpEditMode->GetEditorWorld()->GetPicker();
	mpFilter->SetViewport(pViewport);
	pPicker->SetPickFilter(mpFilter);
	mpRay->SetViewport(pViewport);
	pPicker->SetPickMethod(mpRay);

	pPicker->Update();
	mpEntityUnderPointer = pPicker->GetClosestPick();

	if(mpEntityUnderPointer && mpMouseOverCallbackObject && mpMouseOverCallback)
		mpMouseOverCallback(mpMouseOverCallbackObject, NULL, cGuiMessageData());
}

//----------------------------------------------------------------------------

void cEntitySelectorHighlighter::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions)
{
	cCamera* pCam = apViewport->GetCamera();
	if(mpEntityUnderPointer)
	{
		//cBoundingVolume* pBV = mpEntityUnderPointer->GetPickBV(apViewport);
		mpEntityUnderPointer->Draw(apViewport, apFunctions, NULL, true, cColor(0,0.5f,1,1));
		apFunctions->GetLowLevelGfx()->DrawSphere(mpEntityUnderPointer->GetPosition(), 0.1f, cColor(0,0.5f,1,1));
	}
	tEntityWrapperListIt it = mlstHighlightedEntities.begin();
	for(;it!=mlstHighlightedEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		pEnt->Draw(apViewport, apFunctions, NULL, true, cColor(0,0.5f,1,1));
		//cBoundingVolume* pBV = pEnt->GetPickBV(apViewport);
		//apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(), pBV->GetMax(), cColor(0,0.5f,1,1));
	}
}

//----------------------------------------------------------------------------

bool cEntitySelectorHighlighter::OnViewportMouseDown()
{
	if(mpMouseDownCallbackObject && mpMouseDownCallback)
		return mpMouseDownCallback(mpMouseDownCallbackObject,NULL, cGuiMessageData());

	return false;
}

//----------------------------------------------------------------------------

bool cEntitySelectorHighlighter::OnViewportMouseUp()
{
	if(mpMouseUpCallbackObject && mpMouseUpCallback)
		return mpMouseUpCallback(mpMouseUpCallbackObject,NULL, cGuiMessageData());

	return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

void cEntitySelectorHighlighter::SetMouseDownCallback(void *apCallbackObject, tGuiCallbackFunc apCallback)
{
	mpMouseDownCallbackObject = apCallbackObject;
	mpMouseDownCallback = apCallback;
}



//----------------------------------------------------------------------------

void cEntitySelectorHighlighter::SetMouseUpCallback(void *apCallbackObject, tGuiCallbackFunc apCallback)
{
	mpMouseUpCallbackObject = apCallbackObject;
	mpMouseUpCallback = apCallback;
}


//----------------------------------------------------------------------------

void cEntitySelectorHighlighter::SetMouseOverCallback(void *apCallbackObject, tGuiCallbackFunc apCallback)
{
	mpMouseOverCallbackObject = apCallbackObject;
	mpMouseOverCallback = apCallback;
}

//----------------------------------------------------------------------------

void cEntitySelectorHighlighter::SetHighlightedEntities(const tEntityWrapperList& alstHighlightedEntities)
{
	mlstHighlightedEntities = alstHighlightedEntities;
}

//----------------------------------------------------------------------------

void cEntitySelectorHighlighter::AddHighlightedEntity(iEntityWrapper* apEntity)
{
	if(apEntity==NULL)
		return;

	tEntityWrapperListIt it = find(mlstHighlightedEntities.begin(), mlstHighlightedEntities.end(), apEntity);
	if(it==mlstHighlightedEntities.end())
		mlstHighlightedEntities.push_back(apEntity);
}

//----------------------------------------------------------------------------

void cEntitySelectorHighlighter::RemoveHighlightedEntity(iEntityWrapper* apEntity)
{
	if(apEntity==NULL)
		return;

	tEntityWrapperListIt it = find(mlstHighlightedEntities.begin(), mlstHighlightedEntities.end(), apEntity);
	if(it!=mlstHighlightedEntities.end())
		mlstHighlightedEntities.erase(it);
}

//----------------------------------------------------------------------------

void cEntitySelectorHighlighter::ClearHighlightedEntities()
{
	mlstHighlightedEntities.clear();
}

//----------------------------------------------------------------------------





