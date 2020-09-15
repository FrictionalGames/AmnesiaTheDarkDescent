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

#include "EditorEditMode.h"
#include "EditorAction.h"
#include "EditorWindowFactory.h"
#include "EditorWindow.h"
#include "EditorWindowViewport.h"

#include "EditorWorld.h"

#include "EntityWrapper.h"

#include "EditorActionEntity.h"

#include "EditorEditModeSelect.h"

#include "SurfacePicker.h"

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

iEditorEditMode::iEditorEditMode(iEditorBase *apEditor, const tString& asName)
{
	mpEditor = apEditor;
	msName = asName;

	mpWindow = NULL;
}

//---------------------------------------------------------------------

iEditorEditMode::~iEditorEditMode()
{
	//cEditorWindowFactory::DestroyEditorWindow(mpWindow);
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

void iEditorEditMode::SetCurrent(bool abX)
{
	if(mpWindow) mpWindow->SetActive(abX);

	OnSetCurrent(abX);
}
//---------------------------------------------------------------------

void iEditorEditMode::DrawPreGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos)
{
/*	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	apFunctions->SetBlendMode(eMaterialBlendMode_None);*/
}

//---------------------------------------------------------------------

void iEditorEditMode::DrawPostGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos)
{
	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);

	iEditorWorld* pEditorWorld = mpEditor->GetEditorWorld();

	//pEditorWorld->GetPicker()->DrawDebug(apFunctions);

	tEntityWrapperMap& mapEntities = pEditorWorld->GetEntities();
	tEntityWrapperMapIt it = mapEntities.begin();
	for(;it!=mapEntities.end();++it)
	{
		iEntityWrapper* pEnt = it->second;

		if(pEnt->IsVisible() && 
			pEnt->IsCulledByFrustum(apViewport->GetCamera())==false &&
			pEnt->IsCulledByClipPlanes()==false)
		{
			pEnt->Draw(apViewport,apFunctions,this, false);
		}
	}
}

//---------------------------------------------------------------------

iEditorWindow* iEditorEditMode::CreateWindow()
{
	mpWindow = CreateSpecificWindow();
	if(mpWindow)
	{
		mpWindow->Init();
		mpEditor->AddWindow(mpWindow);
		mpWindow->SetActive(false);
	}

	return mpWindow;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

iEditorEditModeObjectCreator::iEditorEditModeObjectCreator(iEditorBase* apEditor, 
														   const tString& asName,
														   iEditorWorld* apEditorWorld) : iEditorEditMode(apEditor, asName)
{
	mpEditorWorld = apEditorWorld;
	mbPreCreationActive = false;
	mlSubType = 0;

	mbCreateOnSurface = false;
	mvAffectedSurfaceTypes.resize(eSurfaceType_LastEnum);
	for(int i=0;i<(int)mvAffectedSurfaceTypes.size();++i)
		mvAffectedSurfaceTypes[i]=true;

	mvCreatorLocalRotation = 0;
	mvCreatorScale = 1;
	mvCreatorWorldRotation = 0;
}

iEditorEditModeObjectCreator::~iEditorEditModeObjectCreator()
{
}

void iEditorEditModeObjectCreator::OnAdd()
{
	SetUpTypes();
}

//---------------------------------------------------------------------

void iEditorEditModeObjectCreator::SetAffectSurfaceType(int alType, bool abX)
{
	cSurfacePicker* pPicker = mpEditor->GetEditorWorld()->GetSurfacePicker();
	pPicker->SetAffectType(alType, abX, false);
}

bool iEditorEditModeObjectCreator::GetAffectSurfaceType(int alType)
{
	cSurfacePicker* pPicker = mpEditor->GetEditorWorld()->GetSurfacePicker();
	return pPicker->IsAffected(alType, false);
}


//---------------------------------------------------------------------

void iEditorEditModeObjectCreator::OnEditorUpdate(float afTimeStep)
{
	if(mbCreateOnSurface)
	{
		if(mpEditor->GetFlags(eEditorFlag_MouseMoved)==false)
			return;

		cSurfacePicker* pPicker = mpEditor->GetEditorWorld()->GetSurfacePicker();
		pPicker->Update();
	}
}

//---------------------------------------------------------------------

void iEditorEditModeObjectCreator::OnViewportMouseDown(int alButtons)
{
	if(alButtons & eGuiMouseButton_Left)
	{
		mpEditor->SetFlags(eEditorFlag_Placing, true);
		
		mbPreCreationActive = true;
	}
}

//-----------------------------------------------------------------

void iEditorEditModeObjectCreator::OnViewportMouseUp(int alButtons)
{
	if(mpEditor->GetFlags(eEditorFlag_Placing)==false ||
		(alButtons & eGuiMouseButton_Left)==0)
		return;

	iEntityWrapperData* pCreationData = CreateObjectData();

	iEditorAction* pAction = CreateObject(pCreationData);
	mpEditor->AddAction(pAction);

	mpEditor->SetFlags(eEditorFlag_Placing, false);

	UpdateLocalRotation();
	UpdateCreatorScale();

	mbPreCreationActive = false;
}

//-----------------------------------------------------------------

iEntityWrapperData* iEditorEditModeObjectCreator::CreateObjectData()
{
	iEntityWrapperType* pType = GetType();
	if(pType==NULL)
		return NULL;

	iEntityWrapperData* pCreationData = pType->CreateData();
	if(pCreationData && SetUpCreationData(pCreationData) && pCreationData->PostCreateSetUp())
	{
		pCreationData->SetName(mpEditorWorld->GenerateName(pCreationData->GetName()));
		return pCreationData;
	}
	else
	{
		if(pCreationData)
			hplDelete(pCreationData);
	}

	return NULL;
}

//-----------------------------------------------------------------

iEditorAction* iEditorEditModeObjectCreator::CreateObject(iEntityWrapperData* apData)
{
	if(apData==NULL)
		return NULL;

	return hplNew(cEditorActionObjectCreate,(mpEditorWorld, apData));
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

void iEditorEditModeObjectCreator::DrawPostGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos)
{
	iEditorEditMode::DrawPostGrid(apViewport,apFunctions,avPos);

	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	cMatrixf mtxTransform = cMath::MatrixMul(cMath::MatrixTranslate(GetCreatorPosition()),
			cMath::MatrixMul( cMath::MatrixRotate(GetCreatorRotation(), eEulerRotationOrder_XYZ),
			cMath::MatrixScale(GetCreatorScale())));

	cColor col = mbPreCreationActive?cColor(0,1,0,1):cColor(1,0,0,1);
	apFunctions->GetLowLevelGfx()->DrawSphere(mtxTransform.GetTranslation(),0.1f,col);

	DrawObjectPreview(apViewport, apFunctions, mtxTransform, mbPreCreationActive);

	apFunctions->SetBlendMode(eMaterialBlendMode_None);
}

//-----------------------------------------------------------------

int iEditorEditModeObjectCreator::GetTypeNum()
{
	return (int)mvTypes.size();
}

//-----------------------------------------------------------------

void iEditorEditModeObjectCreator::OnSetCurrent(bool abX)
{
	cSurfacePicker* pPicker = mpEditor->GetEditorWorld()->GetSurfacePicker();
	if(abX)
	{
		for(int i=0;i<(int)mvAffectedSurfaceTypes.size();++i)
            pPicker->SetAffectType(i, mvAffectedSurfaceTypes[i], false);
	}
	else
	{
		for(int i=0;i<(int)mvAffectedSurfaceTypes.size();++i)
			mvAffectedSurfaceTypes[i] = pPicker->IsAffected(i, false);
	}
}

//-----------------------------------------------------------------

void iEditorEditModeObjectCreator::UpdateCreatorScale()
{
	iEditModeObjectCreatorWindow* pWin = (iEditModeObjectCreatorWindow*)mpWindow;
	mvCreatorScale = pWin? pWin->GetScale() : 1;
}

void iEditorEditModeObjectCreator::UpdateLocalRotation()
{
	iEditModeObjectCreatorWindow* pWin = (iEditModeObjectCreatorWindow*)mpWindow;
	mvCreatorLocalRotation = pWin? pWin->GetRotate() : 0;
}

//-----------------------------------------------------------------

void iEditorEditModeObjectCreator::UpdateCreatorRotation()
{
	cMatrixf mtxLocalRotation = cMatrixf::Identity;
	iEditModeObjectCreatorWindow* pWin = (iEditModeObjectCreatorWindow*)mpWindow;
	
	cMatrixf mtxRotation = cMath::MatrixRotate(mvCreatorLocalRotation, eEulerRotationOrder_XYZ);
	if(mbCreateOnSurface)
	{
		cMatrixf mtxSurfaceRot = mpEditorWorld->GetSurfacePicker()->GetOrientationMatrix().GetTranspose();
		mtxRotation = cMath::MatrixMul(cMath::MatrixInverse(mtxSurfaceRot), mtxRotation);
	}

	mvCreatorWorldRotation = cMath::MatrixToEulerAngles(mtxRotation, eEulerRotationOrder_XYZ);
}

//-----------------------------------------------------------------

const cVector3f& iEditorEditModeObjectCreator::GetCreatorPosition()
{
	cSurfacePicker* pPicker = mpEditor->GetEditorWorld()->GetSurfacePicker();
	if(mbCreateOnSurface)
	{
		if(pPicker->HasPickedSurface()) mvCreatorPosition = pPicker->GetPositionInSurface();
	}
	else
		mvCreatorPosition =  mpEditor->GetPosOnGridFromMousePos();

	return mvCreatorPosition;
}

//-----------------------------------------------------------------

const cVector3f& iEditorEditModeObjectCreator::GetCreatorRotation()
{
    UpdateCreatorRotation();

	return mvCreatorWorldRotation;
}

//-----------------------------------------------------------------

const cVector3f& iEditorEditModeObjectCreator::GetCreatorScale()
{
	//UpdateCreatorScale();

	return mvCreatorScale;
}

//-----------------------------------------------------------------

bool iEditorEditModeObjectCreator::SetUpCreationData(iEntityWrapperData* apData)
{
	cVector3f vPos = mpEditor->GetFocusedViewport()->GetMouseWorldPosition();
	
	apData->SetVec3f(eObjVec3f_Position, GetCreatorPosition());
	apData->SetVec3f(eObjVec3f_Rotation, mvCreatorWorldRotation);
	apData->SetVec3f(eObjVec3f_Scale, mvCreatorScale);
	
	return true;
}

//-----------------------------------------------------------------

void iEditorEditModeObjectCreator::SetUpTypes()
{
	CreateTypes();
	for(int i=0;i<(int)mvTypes.size();++i)
	{
		mpEditorWorld->AddEntityType(mvTypes[i]);
	}
}

//-----------------------------------------------------------------

iEntityWrapperType* iEditorEditModeObjectCreator::GetType(int alX)
{
	if(alX<0 || alX>=GetTypeNum())
		return NULL;

	return mvTypes[alX];
}

iEntityWrapperType* iEditorEditModeObjectCreator::GetType()
{
	return GetType(mlSubType);
}

//-----------------------------------------------------------------
