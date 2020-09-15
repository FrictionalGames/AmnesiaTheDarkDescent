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

#include "../Common/EdEditModeCombine.h"
#include "../Common/EdWindowCombine.h"

#include "../Common/EdEditModeHandler.h"
#include "../Common/EdEditModeSelect.h"
#include "../Common/EdWindowViewport.h"

#include "../Common/Editor.h"

#include "../LevelEditor/HPLMap.h"
#include "../Common/EdAction.h"

//-------------------------------------------------------------

///////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------

cEdEditModeCombine::cEdEditModeCombine(iEditor* apEditor) : iEdEditMode(apEditor, _W("Combine"))
{
	mpSelect = static_cast<cEdEditModeSelect*>(mpEditor->GetEditModeHandler()->GetMode(_W("Select")));

	iEdWorld* pWorld = GetWorld();

	mpPicker = hplNew(cEdObjectPicker,(pWorld));

	mpFilter = hplNew(cUIPickFilter,("EditModeCombine filter"));
	mpFilter->SetAllPass(false);

	mpRay = hplNew(cUIPickMethodRay, ());
	mpBox = hplNew(cUIPickMethodBox, ());

	mpPicker->SetPickFilter(mpFilter);

	mbPressed = false;
}

//-------------------------------------------------------------

cEdEditModeCombine::~cEdEditModeCombine()
{
	hplDelete(mpPicker);
	hplDelete(mpFilter);
	hplDelete(mpRay);
	hplDelete(mpBox);
}

//-------------------------------------------------------------

///////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------

bool cEdEditModeCombine::IsBusy()
{
	return mbPressed;
}

//-------------------------------------------------------------

void cEdEditModeCombine::OnViewportMouseDown(const cViewportClick& aData)
{
	if(aData.mbLeft==false)
		return;

	cLevelEditorStaticObjectCombo* pCombo = static_cast<cEdWindowCombine*>(mpWindow)->GetCurrentCombo();
	if(pCombo == NULL)
		return;

	mbPressed = true;

	mMouseRect.x = (int)aData.mvLocalPos.x;
	mMouseRect.y = (int)aData.mvLocalPos.y;
}

//-------------------------------------------------------------

void cEdEditModeCombine::OnViewportMouseUp(const cViewportClick& aData)
{
	if(aData.mbLeft==false)
		return;

	cEdWindowCombine* pWin = static_cast<cEdWindowCombine*>(mpWindow);
	cLevelEditorStaticObjectCombo* pCombo = pWin->GetCurrentCombo();
	if(pCombo == NULL)
		return;

	mbPressed = false;

	// Set up picker
	//mpFilter->SetViewport(aData.mpViewport);
		
	if(IsRayPickingActive())
		DoRayPicking(pCombo, aData.mpViewport, mpPicker);
	else
		DoBoxPicking(pCombo, aData.mpViewport, mpPicker);

	mMouseRect.w = 0;
	mMouseRect.h = 0;
}

//-------------------------------------------------------------

void cEdEditModeCombine::OnUpdate()
{
	iEdViewport* pViewport = mpEditor->GetFocusedViewport();
	cVector2l vMousePos = pViewport->GetMouseViewportPositionInt();

	if(mbPressed)
	{
		mMouseRect.w = vMousePos.x - mMouseRect.x;
		mMouseRect.h = vMousePos.y - mMouseRect.y;
	}

	cEdWindowCombine* pWin = static_cast<cEdWindowCombine*>(mpWindow);
	cLevelEditorStaticObjectCombo* pCombo = pWin->GetCurrentCombo();
	if(pCombo==NULL)
		return;

	bool bRayPicking = IsRayPickingActive();

	if(bRayPicking)
	{
		//mpRay->SetViewport(pViewport);
		mpPicker->SetPickMethod(mpRay);
	}
	else if(mbPressed)
	{
		//mpBox->SetViewport(pViewport);
		static_cast<cUIPickMethodBox*>(mpBox)->SetMouseBox(mMouseRect);
		mpPicker->SetPickMethod(mpBox);
	}

	mpPicker->Update();

	mlstObjsToBePicked.clear();
	if(bRayPicking)
	{
		iEdScnObject* pPick = mpPicker->GetClosestPick();
		if(pPick) mlstObjsToBePicked.push_back(pPick);
	}
	else if(mbPressed)
	{
		for(int i=0;i<mpPicker->GetNumPicks();++i)
		{
			cPickData* pPick = mpPicker->GetPick(i);
			mlstObjsToBePicked.push_back(pPick->mpObject);
		}
	}
}

//-------------------------------------------------------------

void cEdEditModeCombine::OnDraw(const cModuleDrawData& aData)
{
	if(aData.mStep==eEdDrawStep_PostTrans) return;

	iEdEditMode::OnDraw(aData);

	cEdWindowCombine* pWin = static_cast<cEdWindowCombine*>(mpWindow);

	if(pWin->GetShowAllCombos())
	{
		const tStaticObjectComboList& lstCombos = GetCombos();
		tStaticObjectComboList::const_iterator itCombos = lstCombos.begin();
		for(;itCombos!=lstCombos.end();++itCombos)
		{
			cLevelEditorStaticObjectCombo* pCombo = *itCombos;
			pCombo->OnDraw(aData);
		}
	}
	else
	{
		cLevelEditorStaticObjectCombo* pCombo = pWin->GetCurrentCombo();
		if(pCombo) pCombo->OnDraw(aData);
	}

	aData.mpFunctions->SetProgram(NULL);

	///////////////////////////////////////////////////////
	// highlight candidates
	{
		tScnObjListIt it = mlstObjsToBePicked.begin();
		for(;it!=mlstObjsToBePicked.end();++it)
		{
			iEdScnObject* pEnt = *it;
			pEnt->DrawObject(aData, true, cColor(1,0.5f));
		}
	}

	////////////////////////////////////////////////
	// Show picking rect
	if(mbPressed && aData.mpViewport->IsFocused() && IsRayPickingActive()==false)
	{
		aData.mpFunctions->SetDepthTest(false);
		aData.mpFunctions->GetLowLevelGfx()->SetOrthoProjection(aData.mpViewport->GetGuiViewportSize(),-1000,1000);
		aData.mpFunctions->GetLowLevelGfx()->SetIdentityMatrix(eMatrix_ModelView);
		aData.mpFunctions->GetLowLevelGfx()->DrawLineQuad(cRect2f((float)mMouseRect.x,(float)mMouseRect.y,
															(float)mMouseRect.w,(float)mMouseRect.h),
															0,
															cColor(1,1)); 
		aData.mpFunctions->SetNormalFrustumProjection();
	}
}

//-------------------------------------------------------------

const tStaticObjectComboList& cEdEditModeCombine::GetCombos()
{
	cHplMap* pWorld = static_cast<cHplMap*>(mpEditor->GetWorld());

	return pWorld->GetStaticObjectCombos();
}

//-------------------------------------------------------------

cLevelEditorStaticObjectCombo* cEdEditModeCombine::GetCombo(int alID)
{
	cHplMap* pWorld = static_cast<cHplMap*>(mpEditor->GetWorld());

	return pWorld->GetStaticObjectCombo(alID);
}

//-------------------------------------------------------------

void cEdEditModeCombine::AddCombo()
{
	cHplMap* pWorld = static_cast<cHplMap*>(mpEditor->GetWorld());

	iEdAction* pAction = pWorld->CreateAddComboAction();

	mpEditor->AddAction(pAction);
}

//-------------------------------------------------------------

void cEdEditModeCombine::RemoveCombo(int alID)
{
	cHplMap* pWorld = static_cast<cHplMap*>(mpEditor->GetWorld());

	iEdAction* pAction = pWorld->CreateRemoveComboAction(alID);

	mpEditor->AddAction(pAction);
}

//-------------------------------------------------------------

///////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------

iEdWindow* cEdEditModeCombine::CreateWindow()
{
	return hplNew(cEdWindowCombine,(this));
}

//-------------------------------------------------------------

void cEdEditModeCombine::OnInit()
{
	iEdWorld* pWorld = GetWorld();
	mpFilter->SetTypeFilter(pWorld->GetTypeByName(_W("Static Object")), true);
	mpFilter->SetTypeFilter(pWorld->GetTypeByName(_W("Primitive")), true);
}

//-------------------------------------------------------------

void cEdEditModeCombine::OnViewportMouseMove()
{
	mpPicker->SetUpdated();
}

//-------------------------------------------------------------

void cEdEditModeCombine::DoRayPicking(cLevelEditorStaticObjectCombo* apCombo, 
										  iEdViewport* apViewport, 
										  cEdObjectPicker* apPicker)
{
	mpRay->SetViewport(apViewport);
	apPicker->SetPickMethod(mpRay);

	apPicker->Update();

	eCombinePickMode pickMode = ((cEdWindowCombine*)mpWindow)->GetPickMode();
	iEdScnObject* pObj = apPicker->GetClosestPick();
	
	iEdAction* pAction = CreateProperAction(apCombo, pObj, pickMode);

	mpEditor->AddAction(pAction);
}

//-------------------------------------------------------------

void cEdEditModeCombine::DoBoxPicking(cLevelEditorStaticObjectCombo* apCombo, 
										  iEdViewport* apViewport, 
										  cEdObjectPicker* apPicker)
{
	mpBox->SetViewport(apViewport);
	((cUIPickMethodBox*)mpBox)->SetMouseBox(mMouseRect);
	apPicker->SetPickMethod(mpBox);

	apPicker->Update();

	if(apPicker->GetNumPicks()==0)
		return;

	eCombinePickMode pickMode = ((cEdWindowCombine*)mpWindow)->GetPickMode();

	cEdActionCompound *pAction = hplNew(cEdActionCompound,(_W("Add objects to combo")));
	for(int i=0;i<apPicker->GetNumPicks();++i)
	{
		cPickData* pPick = apPicker->GetPick(i);
		iEdScnObject* pObj = pPick->mpObject;
	
		iEdAction* pSubAction = CreateProperAction(apCombo, pObj, pickMode);

		pAction->AddAction(pSubAction);
	}

	if(pAction->IsEmpty()==false)
		mpEditor->AddAction(pAction);
}

//-------------------------------------------------------------

iEdAction* cEdEditModeCombine::CreateProperAction(cLevelEditorStaticObjectCombo* apCombo, 
														  iEdScnObject* apObj,  
														  eCombinePickMode aMode)
{
	iEdAction* pAction = NULL;
	if(aMode==eCombinePickMode_Add)
	{
		pAction = apCombo->CreateActionAddObject(apObj);
	}
	else if(aMode==eCombinePickMode_Remove)
	{
		pAction = apCombo->CreateActionRemoveObject(apObj);
	}
	else if(aMode==eCombinePickMode_Toggle)
	{
		if(apCombo->HasObject(apObj))
			pAction = apCombo->CreateActionRemoveObject(apObj);
		else
			pAction = apCombo->CreateActionAddObject(apObj);
	}

	return pAction;
}

//-------------------------------------------------------------

bool cEdEditModeCombine::IsRayPickingActive()
{
	float fLimit = 5;
	return (cMath::Abs(mMouseRect.w) < fLimit) && (cMath::Abs(mMouseRect.h) < fLimit);
}

//-------------------------------------------------------------
