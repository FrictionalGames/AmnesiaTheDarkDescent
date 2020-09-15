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

#include "../Common/EdEditModeScnObjCreator.h"

#include "../Common/Editor.h"
#include "../Common/EdWindowViewport.h"
#include "../Common/EdScnObject.h"

#include "../Common/EdObjectActions.h"

#include "../Common/EdScnObjCreatorPane.h"

#include "../Common/EdScnWorld.h"
#include "../Common/EdSurfacePicker.h"

//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// OBJECT CREATOR BASE - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

iEdEditModeScnObjCreator::iEdEditModeScnObjCreator(iEditor* apEditor, 
												   const tWString& asName) : iEdEditModeObjectCreator(apEditor, asName)
{
	mpType = NULL;

	mbCreateOnSurface = false;
	//mvAffectedSurfaceTypes.resize(eSurfaceType_LastEnum);
	//for(size_t i=0; i<mvAffectedSurfaceTypes.size(); ++i)
	//	mvAffectedSurfaceTypes[i]=true;

	mvCreatorLocalRotation = 0;
	mvCreatorScale = 1;
	mvCreatorWorldRotation = 0;
}

//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// OBJECT CREATOR BASE - PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void iEdEditModeScnObjCreator::SetAffectSurfaceType(iEdObjectType* apType, bool abX)
{
	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpEditor->GetWorld());
	cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();

	pPicker->SetAffectType(apType, abX);
	//cSurfacePicker* pPicker = mpEditor->GetEdWorld()->GetSurfacePicker();
	//pPicker->SetAffectType(alType, abX, false);
}

bool iEdEditModeScnObjCreator::GetAffectSurfaceType(int alType)
{
	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpEditor->GetWorld());
	cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();

	return false;

	//pPicker->etAffectType(apType, bool);
	//cSurfacePicker* pPicker = mpEditor->GetEdWorld()->GetSurfacePicker();
	//return pPicker->IsAffected(alType, false);

	return false;
}

//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// OBJECT CREATOR BASE - PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void iEdEditModeScnObjCreator::OnUpdate()
{
	if(mbCreateOnSurface)
	{
		//if(mpEditor->GetFlags(eEdFlag_MouseMoved)==false)
		//	return;

		iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpEditor->GetWorld());
		cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();
		pPicker->Update();
	}
}

//---------------------------------------------------------------------

void iEdEditModeScnObjCreator::OnViewportMouseDown(const cViewportClick& aData)
{
	if(aData.mbLeft==false)
		return;

	//mpEditor->SetFlags(eEdFlag_Placing, true);
	mbPreCreationActive = true;
}

//-----------------------------------------------------------------

void iEdEditModeScnObjCreator::OnViewportMouseUp(const cViewportClick& aData)
{
	//if(mpEditor->GetFlags(eEdFlag_Placing)==false ||
	if(aData.mbLeft==false)
		return;

	iEdObjectData* pCreationData = CreateObjectData();

	iEdAction* pAction = mpEditor->GetWorld()->CreateActionCreateObject(pCreationData);
	mpEditor->AddAction(pAction);

	//mpEditor->SetFlags(eEdFlag_Placing, false);

	UpdateLocalRotation();
	UpdateCreatorScale();

	mbPreCreationActive = false;
}

//-----------------------------------------------------------------

void iEdEditModeScnObjCreator::OnDraw(const cModuleDrawData& aData)
{
	cRendererCallbackFunctions* apFunctions = aData.mpFunctions;
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

	if(this->mbCreateOnSurface)
		static_cast<iEdScnWorld*>(GetWorld())->GetSurfacePicker()->DrawDebug(aData.mpFunctions);

	DrawObjectPreview(aData.mpViewport, aData.mpFunctions, mtxTransform, mbPreCreationActive);

	apFunctions->SetBlendMode(eMaterialBlendMode_None);
}

//-----------------------------------------------------------------

void iEdEditModeScnObjCreator::OnSetActive(bool abX)
{
	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpEditor->GetWorld());
	cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();
	if(abX)
	{		
	//	pPicker->SetAffectType(i, mvAffectedSurfaceTypes[i], false);
	}
	else
	{
	//	for(size_t i=0;i<mvAffectedSurfaceTypes.size();++i)
	//		mvAffectedSurfaceTypes[i] = pPicker->IsAffected(i, false);
	}
}

//-----------------------------------------------------------------

void iEdEditModeScnObjCreator::UpdateCreatorScale()
{
	iEdScnObjCreatorPane* pWin = static_cast<iEdScnObjCreatorPane*>(mpWindow);
	mvCreatorScale = pWin? pWin->GetScale() : 1;
}

void iEdEditModeScnObjCreator::UpdateLocalRotation()
{
	iEdScnObjCreatorPane* pWin = static_cast<iEdScnObjCreatorPane*>(mpWindow);
	mvCreatorLocalRotation = pWin? pWin->GetRotate() : 0;
}

//-----------------------------------------------------------------

void iEdEditModeScnObjCreator::UpdateCreatorRotation()
{
	cMatrixf mtxLocalRotation = cMatrixf::Identity;
	iEdScnObjCreatorPane* pWin = static_cast<iEdScnObjCreatorPane*>(mpWindow);
	
	cMatrixf mtxRotation = cMath::MatrixRotate(mvCreatorLocalRotation, eEulerRotationOrder_XYZ);
	if(mbCreateOnSurface)
	{
		iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpEditor->GetWorld());
		cMatrixf mtxSurfaceRot = pWorld->GetSurfacePicker()->GetOrientationMatrix().GetTranspose();
		mtxRotation = cMath::MatrixMul(cMath::MatrixInverse(mtxSurfaceRot), mtxRotation);
	}

	mvCreatorWorldRotation = cMath::MatrixToEulerAngles(mtxRotation, eEulerRotationOrder_XYZ);
}

//-----------------------------------------------------------------

const cVector3f& iEdEditModeScnObjCreator::GetCreatorPosition()
{
    iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpEditor->GetWorld());
	cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();
	if(mbCreateOnSurface)
	{
		if(pPicker->HasPickedSurface()) 
			mvCreatorPosition = pPicker->GetPositionInSurface();
	}
	else
		mvCreatorPosition =  mpEditor->GetFocusedViewport()->GetMouseWorldPosition();

	return mvCreatorPosition;
}

//-----------------------------------------------------------------

const cVector3f& iEdEditModeScnObjCreator::GetCreatorRotation()
{
    UpdateCreatorRotation();

	return mvCreatorWorldRotation;
}

//-----------------------------------------------------------------

const cVector3f& iEdEditModeScnObjCreator::GetCreatorScale()
{
	//UpdateCreatorScale();

	return mvCreatorScale;
}

//-----------------------------------------------------------------

bool iEdEditModeScnObjCreator::SetUpCreationData(iEdObjectData* apData)
{
	apData->SetVec3f(eScnVec3f_Translation, GetCreatorPosition());
	apData->SetVec3f(eScnVec3f_Rotation, GetCreatorRotation());
	apData->SetVec3f(eScnVec3f_Scale, GetCreatorScale());

    if(GetSelectedType()->SetUpCreationData(apData)==false)
		return false;

	iEdScnObjCreatorPane* pPane = static_cast<iEdScnObjCreatorPane*>(mpWindow);

	return pPane->SetUpCreationData(apData);
}

//-----------------------------------------------------------------
