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

#include "../Common/EdEditModeDecals.h"

#include "../Common/EdWindowDecals.h"
#include "../Common/EdObjDecal.h"
//#include "../Common/EdHelper.h"
#include "../Common/Editor.h"
#include "../Common/EdWindowViewport.h"
#include "../Common/EdScnWorld.h"
#include "../Common/EdSurfacePicker.h"

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

cEdEditModeDecals::cEdEditModeDecals(iEditor* apEditor) : iEdEditModeScnObjCreator(apEditor, _W("Decals"))
{
	mbCreateOnSurface = true;
}

//-----------------------------------------------------------------------------------

cEdEditModeDecals::~cEdEditModeDecals()
{
}

//-----------------------------------------------------------------------------------

void cEdEditModeDecals::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(aClick.mbLeft==false)
		return;

	mbPreCreationActive = true;
}

//-----------------------------------------------------------------------------------

void cEdEditModeDecals::OnViewportMouseUp(const cViewportClick& aClick)
{
	//if(mpEditor->GetFlags(eEdFlag_Placing)==false ||
	if(aClick.mbLeft==false)
		return;

	cDecalCreator* pCreator = mpEditor->GetEngine()->GetGraphics()->GetDecalCreator();
	if(pCreator->CanCreateDecal())
	{
		iEdObjectData* pCreationData = CreateObjectData();

		iEdAction* pAction = mpEditor->GetWorld()->CreateActionCreateObject(pCreationData);
		mpEditor->AddAction(pAction);		
	}

	UpdateDecalParams();

	mbPreCreationActive = false;
}

//-----------------------------------------------------------------------------------

void cEdEditModeDecals::OnUpdate()
{
	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpEditor->GetWorld());
	cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();
	pPicker->Update();

	cEdWindowDecals* pWin = static_cast<cEdWindowDecals*>(mpWindow);
	const tString& sMaterial = pWin->GetMaterial();

	cDecalCreator* pCreator = mpEditor->GetEngine()->GetGraphics()->GetDecalCreator();
	pCreator->SetMaterial(sMaterial);
	pCreator->SetMaxTrianglesPerDecal(cTypeDecal::GetGlobalMaxTriangles());

	cMatrixf mtxSurfaceRot = pPicker->GetOrientationMatrix().GetTranspose();
	cMatrixf mtxRotation = cMath::MatrixMul(cMath::MatrixInverse(mtxSurfaceRot), cMath::MatrixRotate(mvCreatorLocalRotation, eEulerRotationOrder_XYZ));
	cMatrixf mtxOrientation = mtxRotation.GetTranspose();

	cEdObjDecal::BuildDecalVertexBuffer(GetWorld()->GetEngWorld(), pCreator, 
											pPicker->GetPositionInSurface(), GetCreatorScale(), mfOffset,
											mtxOrientation.GetRight(), mtxOrientation.GetUp(), mtxOrientation.GetForward(),
											sMaterial, pWin->GetColor(), 
											pWin->GetSubDivisions(), mlCurrentSubDiv, cTypeDecal::GetGlobalMaxTriangles(),
											pPicker, pPicker->GetAffectedTypes());
}

//-----------------------------------------------------------------------------------

void cEdEditModeDecals::OnDraw(const cModuleDrawData& aData)
{
	aData.mpFunctions->SetTextureRange(NULL,0);
	
	cDecalCreator* pCreator = mpEditor->GetEngine()->GetGraphics()->GetDecalCreator();
	pCreator->DrawDebug(aData.mpFunctions, true, false);
}

//-----------------------------------------------------------------------------------

void cEdEditModeDecals::OnSetActive(bool abX)
{
	iEdEditModeObjectCreator::OnSetActive(abX);
	if(abX)
		UpdateDecalParams();
}

//-----------------------------------------------------------------------------------

void cEdEditModeDecals::UpdateDecalParams()
{
	/////////////////////////////////////////////
	// Randomize if needed and update parameters 
	cEdWindowDecals* pWin = static_cast<cEdWindowDecals*>(mpWindow);
	UpdateLocalRotation();
	UpdateCreatorScale();
	mlCurrentSubDiv = pWin->GetCurrentSubDiv();

	//////////////////////////////////
	// Update random offset
	float fOffsetMin = cTypeDecal::GetDecalOffsetMin();
	float fOffsetMax = cTypeDecal::GetDecalOffsetMax();
	mfOffset = cMath::RandRectf(fOffsetMin, fOffsetMax);
}

//-----------------------------------------------------------------------------------

bool cEdEditModeDecals::SetUpCreationData(iEdObjectData* apData)
{
	cEdWindowDecals* pWin = (cEdWindowDecals*)mpWindow;
	const tString& sMaterial = pWin->GetMaterial();
	if(sMaterial=="")
		return false;

	iEdEditModeScnObjCreator::SetUpCreationData(apData);

	/////////////////////////////////////
	// Build name
	tString sMatFilename = cString::GetFileName(sMaterial);
	tString sMatFileExt = cString::GetFileExt(sMaterial);
	int lMatFileExtStartPos = cString::GetLastStringPos(sMatFilename, sMatFileExt) - 1;
	tWString sName = cString::To16Char(cString::Sub(sMatFilename, 0, lMatFileExtStartPos));
	apData->SetName(sName);

	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpEditor->GetWorld());
	cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();

	//////////////////////////////////////
	// Set material
	apData->SetString(eDecalStr_Material, sMaterial);

	//////////////////////////////////////////////////
	// Set up affected types
	cEdObjDecalData* pData = static_cast<cEdObjDecalData*>(apData);
	pData->SetTypeFilter(pPicker->GetAffectedTypes());

	//////////////////////////////////////
	// Set material file index
	cIndexedFile* pMat = cTypeDecal::mpIndex->CreateIndexedFile(sMaterial);
	apData->SetInt(eDecalInt_FileIndex, pMat->mlID);
	
	//////////////////////////////////////////////////
	// Set up remaining decal parameters
	cDecalCreator* pCreator = mpEditor->GetEngine()->GetGraphics()->GetDecalCreator();

	apData->SetFloat(eDecalFloat_Offset, mfOffset);
	apData->SetColor(eDecalCol_Color, pWin->GetColor());
	apData->SetInt(eDecalInt_MaxTris, -1);
	apData->SetInt(eDecalInt_CurrentSubDiv, pCreator->GetCurrentSubDiv());

	cVector2l vSubDivs = pWin->GetSubDivisions();
	apData->SetVec2f(eDecalVec2f_SubDivs, cVector2f((float)vSubDivs.x, (float)vSubDivs.y));

	return true;
}

//-----------------------------------------------------------------------------------

iEdWindow* cEdEditModeDecals::CreateWindow()
{
	return hplNew(cEdWindowDecals,(this));
}

//-----------------------------------------------------------------------------------

iEdObjectType* cEdEditModeDecals::CreateType()
{
	return hplNew(cTypeDecal,());
}

//-----------------------------------------------------------------------------------
