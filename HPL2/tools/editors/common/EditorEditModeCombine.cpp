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

#include "EditorEditModeCombine.h"
#include "EditorWindowCombine.h"

#include "EditorEditModeSelect.h"
#include "EditorWindowViewport.h"

#include "../leveleditor/LevelEditorWorld.h"
#include "../leveleditor/LevelEditorActions.h"

//-------------------------------------------------------------

///////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------

cEditorEditModeCombine::cEditorEditModeCombine(iEditorBase* apEditor) : iEditorEditMode(apEditor, "Combine")
{
	mpFilter = hplNew(cUIPickFilter,("EditModeCombine filter"));
	mpFilter->SetAllPass(false);
	mpFilter->SetTypeFilter(eEditorEntityType_StaticObject, true);
	mpFilter->SetTypeFilter(eEditorEntityType_Primitive, true);

	mpRay = hplNew(cUIPickMethodRay, ());
	mpBox = hplNew(cUIPickMethodBox, ());

	mbPressed = false;
}

//-------------------------------------------------------------

cEditorEditModeCombine::~cEditorEditModeCombine()
{
	hplDelete(mpFilter);
	hplDelete(mpRay);
	hplDelete(mpBox);
}

//-------------------------------------------------------------

///////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------

void cEditorEditModeCombine::OnViewportMouseDown(int alButtons)
{
	if(alButtons!=eGuiMouseButton_Left)
		return;

	cLevelEditorStaticObjectCombo* pCombo = ((cEditorWindowCombine*)mpWindow)->GetCurrentCombo();
	if(pCombo == NULL)
		return;

	mbPressed = true;

	cEditorWindowViewport* pViewport = mpEditor->GetFocusedViewport();
	cVector2l vMousePos = pViewport->GetMouseViewportPositionInt();

	mMouseRect.x = vMousePos.x;
	mMouseRect.y = vMousePos.y;
}

//-------------------------------------------------------------

void cEditorEditModeCombine::OnViewportMouseUp(int alButtons)
{
	if(alButtons!=eGuiMouseButton_Left)
		return;

	cLevelEditorStaticObjectCombo* pCombo = ((cEditorWindowCombine*)mpWindow)->GetCurrentCombo();
	if(pCombo == NULL)
		return;

	mbPressed = false;

	cEditorWindowViewport* pViewport = mpEditor->GetFocusedViewport();

	// Set up picker
	cEntityPicker* pPicker = mpEditor->GetEditorWorld()->GetPicker();
	mpFilter->SetViewport(pViewport);
	pPicker->SetPickFilter(mpFilter);
	
	if(IsRayPickingActive())
		DoRayPicking(pCombo, pViewport, pPicker);
	else
		DoBoxPicking(pCombo, pViewport, pPicker);

	mMouseRect.w = 0;
	mMouseRect.h = 0;
}

//-------------------------------------------------------------

void cEditorEditModeCombine::OnEditorUpdate(float afTimeStep)
{
	cEditorWindowViewport* pViewport = mpEditor->GetFocusedViewport();
	cVector2l vMousePos = pViewport->GetMouseViewportPositionInt();

	if(mbPressed)
	{
		mMouseRect.w = vMousePos.x - mMouseRect.x;
		mMouseRect.h = vMousePos.y - mMouseRect.y;
	}

	cEditorWindowCombine* pWin = (cEditorWindowCombine*)mpWindow;
	cLevelEditorStaticObjectCombo* pCombo = pWin->GetCurrentCombo();
	if(pCombo==NULL)
		return;

	cEntityPicker* pPicker = mpEditor->GetEditorWorld()->GetPicker();
	mpFilter->SetViewport(pViewport);
	pPicker->SetPickFilter(mpFilter);

	bool bRayPicking = IsRayPickingActive();

	if(bRayPicking)
	{
		mpRay->SetViewport(pViewport);
		pPicker->SetPickMethod(mpRay);
	}
	else if(mbPressed)
	{
		mpBox->SetViewport(pViewport);
		((cUIPickMethodBox*)mpBox)->SetMouseBox(mMouseRect);
		pPicker->SetPickMethod(mpBox);
	}

	pPicker->Update();

	mlstEntsToBePicked.clear();
	if(bRayPicking)
	{
		iEntityWrapper* pPick = pPicker->GetClosestPick();
		if(pPick) mlstEntsToBePicked.push_back(pPick);
	}
	else if(mbPressed)
	{
		for(int i=0;i<pPicker->GetNumPicks();++i)
			mlstEntsToBePicked.push_back(pPicker->GetPick(i)->mpEntity);
	}
}

//-------------------------------------------------------------

void cEditorEditModeCombine::DrawPostGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos)
{
	iEditorEditMode::DrawPostGrid(apViewport,apFunctions,avPos);

	cEditorWindowCombine* pWin = (cEditorWindowCombine*)mpWindow;

	if(pWin->GetShowAllCombos())
	{
		const tStaticObjectComboList& lstCombos = GetCombos();
		tStaticObjectComboList::const_iterator itCombos = lstCombos.begin();
		for(;itCombos!=lstCombos.end();++itCombos)
		{
			cLevelEditorStaticObjectCombo* pCombo = *itCombos;
			pCombo->Draw(apViewport, apFunctions);
		}
	}
	else
	{
		cLevelEditorStaticObjectCombo* pCombo = pWin->GetCurrentCombo();
		if(pCombo) pCombo->Draw(apViewport, apFunctions);
	}

	apFunctions->SetProgram(NULL);

	///////////////////////////////////////////////////////
	// highlight candidates
	{
		tEntityWrapperListIt it = mlstEntsToBePicked.begin();
		for(;it!=mlstEntsToBePicked.end();++it)
		{
			iEntityWrapper* pEnt = *it;
			pEnt->Draw(apViewport, apFunctions, NULL, true);
		}
	}

	////////////////////////////////////////////////
	// Show picking rect
	if(mbPressed && apViewport->IsFocused() && IsRayPickingActive()==false)
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

//-------------------------------------------------------------

void cEditorEditModeCombine::Reset()
{
}

//-------------------------------------------------------------

const tStaticObjectComboList& cEditorEditModeCombine::GetCombos()
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditor->GetEditorWorld();

	return pWorld->GetStaticObjectCombos();
}

//-------------------------------------------------------------

cLevelEditorStaticObjectCombo* cEditorEditModeCombine::GetCombo(int alID)
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditor->GetEditorWorld();

	return pWorld->GetStaticObjectCombo(alID);
}

//-------------------------------------------------------------

void cEditorEditModeCombine::AddCombo()
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditor->GetEditorWorld();

	iEditorAction* pAction = hplNew(cLevelEditorActionAddStaticObjectCombo,(pWorld));

	mpEditor->AddAction(pAction);
}

//-------------------------------------------------------------

void cEditorEditModeCombine::RemoveCombo(int alID)
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditor->GetEditorWorld();

	iEditorAction* pAction = hplNew(cLevelEditorActionRemoveStaticObjectCombo,(pWorld, alID));

	mpEditor->AddAction(pAction);
}

//-------------------------------------------------------------

///////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------

iEditorWindow* cEditorEditModeCombine::CreateSpecificWindow()
{
	return hplNew(cEditorWindowCombine,(this));
}

//-------------------------------------------------------------

void cEditorEditModeCombine::DoRayPicking(cLevelEditorStaticObjectCombo* apCombo, 
										  cEditorWindowViewport* apViewport, 
										  cEntityPicker* apPicker)
{
	mpRay->SetViewport(apViewport);
	apPicker->SetPickMethod(mpRay);

	apPicker->Update();

	eCombinePickMode pickMode = ((cEditorWindowCombine*)mpWindow)->GetPickMode();
	iEntityWrapper* pObj = apPicker->GetClosestPick();
	
	iEditorAction* pAction = CreateProperAction(apCombo, pObj, pickMode);

	mpEditor->AddAction(pAction);
}

//-------------------------------------------------------------

void cEditorEditModeCombine::DoBoxPicking(cLevelEditorStaticObjectCombo* apCombo, 
										  cEditorWindowViewport* apViewport, 
										  cEntityPicker* apPicker)
{
	mpBox->SetViewport(apViewport);
	((cUIPickMethodBox*)mpBox)->SetMouseBox(mMouseRect);
	apPicker->SetPickMethod(mpBox);

	apPicker->Update();

	if(apPicker->GetNumPicks()==0)
		return;

	eCombinePickMode pickMode = ((cEditorWindowCombine*)mpWindow)->GetPickMode();

	cEditorActionCompoundAction *pAction = hplNew(cEditorActionCompoundAction,("Add objects to combo"));
	for(int i=0;i<apPicker->GetNumPicks();++i)
	{
		cPickData* pPick = apPicker->GetPick(i);
		iEntityWrapper* pObj = pPick->mpEntity;
	
		iEditorAction* pSubAction = CreateProperAction(apCombo, pObj, pickMode);

		pAction->AddAction(pSubAction);
	}

	if(pAction->IsEmpty()==false)
		mpEditor->AddAction(pAction);
}

//-------------------------------------------------------------

iEditorAction* cEditorEditModeCombine::CreateProperAction(cLevelEditorStaticObjectCombo* apCombo, 
														  iEntityWrapper* apObj,  
														  eCombinePickMode aMode)
{
	iEditorAction* pAction = NULL;
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

bool cEditorEditModeCombine::IsRayPickingActive()
{
	float fLimit = 5;
	return (cMath::Abs(mMouseRect.w) < fLimit) && (cMath::Abs(mMouseRect.h) < fLimit);
}

//-------------------------------------------------------------
