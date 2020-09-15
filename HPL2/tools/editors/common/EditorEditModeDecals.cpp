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

#include "EditorEditModeDecals.h"
#include "EditorEditModeSelect.h"

#include "EditorWindowDecals.h"
#include "EntityWrapperDecal.h"
#include "EditorHelper.h"
#include "EditorWindowViewport.h"
#include "EditorWorld.h"
#include "SurfacePicker.h"

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

cEditorEditModeDecals::cEditorEditModeDecals(iEditorBase* apEditor, iEditorWorld* apEditorWorld) : iEditorEditModeObjectCreator(apEditor, "Decals", apEditorWorld)
{
	mbCreateOnSurface = true;
}

//-----------------------------------------------------------------------------------

cEditorEditModeDecals::~cEditorEditModeDecals()
{
}

//-----------------------------------------------------------------------------------

void cEditorEditModeDecals::OnViewportMouseDown(int alButtons)
{
	if((alButtons & eGuiMouseButton_Left)==0)
		return;

	mpEditor->SetFlags(eEditorFlag_Placing, true);
}

//-----------------------------------------------------------------------------------

void cEditorEditModeDecals::OnViewportMouseUp(int alButtons)
{
	if(mpEditor->GetFlags(eEditorFlag_Placing)==false ||
		(alButtons & eGuiMouseButton_Left)==0)
		return;

	cDecalCreator* pCreator = mpEditor->GetEngine()->GetGraphics()->GetDecalCreator();
	if(pCreator->CanCreateDecal())
	{
		iEntityWrapperData* pCreationData = CreateObjectData();

		iEditorAction* pAction = CreateObject(pCreationData);
		mpEditor->AddAction(pAction);		
	}

	UpdateDecalParams();

	mpEditor->SetFlags(eEditorFlag_Placing, false);


	mbPreCreationActive = false;
}

//-----------------------------------------------------------------------------------

void cEditorEditModeDecals::OnEditorUpdate(float afTimeStep)
{
	cSurfacePicker* pPicker = mpEditor->GetEditorWorld()->GetSurfacePicker();
	pPicker->Update();

	cEditorWindowDecals* pWin = (cEditorWindowDecals*)mpWindow;
	const tString& sMaterial = pWin->GetMaterial();

	cDecalCreator* pCreator = mpEditor->GetEngine()->GetGraphics()->GetDecalCreator();
	pCreator->SetMaterial(sMaterial);
	pCreator->SetMaxTrianglesPerDecal(cEntityWrapperTypeDecal::GetGlobalMaxTriangles());

	cMatrixf mtxSurfaceRot = pPicker->GetOrientationMatrix().GetTranspose();
	cMatrixf mtxRotation = cMath::MatrixMul(cMath::MatrixInverse(mtxSurfaceRot), cMath::MatrixRotate(mvCreatorLocalRotation, eEulerRotationOrder_XYZ));
	cMatrixf mtxOrientation = mtxRotation.GetTranspose();

	cEntityWrapperDecal::BuildDecalVertexBuffer(mpEditorWorld->GetWorld(), pCreator, 
											pPicker->GetPositionInSurface(), GetCreatorScale(), mfOffset,
											mtxOrientation.GetRight(), mtxOrientation.GetUp(), mtxOrientation.GetForward(),
											sMaterial, pWin->GetColor(), 
											pWin->GetSubDivisions(), mlCurrentSubDiv, cEntityWrapperTypeDecal::GetGlobalMaxTriangles(),
											pPicker->IsAffected(eSurfaceType_StaticObject, false),
											pPicker->IsAffected(eSurfaceType_Primitive, false),
											pPicker->IsAffected(eSurfaceType_Entity, false));
}

//-----------------------------------------------------------------------------------

void cEditorEditModeDecals::DrawPostGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos)
{
	iEditorEditModeObjectCreator::DrawPostGrid(apViewport, apFunctions, avPos);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetMatrix(NULL);

	cDecalCreator* pCreator = mpEditor->GetEngine()->GetGraphics()->GetDecalCreator();
	pCreator->DrawDebug(apFunctions, false, false);
}

//-----------------------------------------------------------------------------------

void cEditorEditModeDecals::OnSetCurrent(bool abX)
{
	iEditorEditModeObjectCreator::OnSetCurrent(abX);
	if(abX)
		UpdateDecalParams();
}

//-----------------------------------------------------------------------------------

void cEditorEditModeDecals::UpdateDecalParams()
{
	/////////////////////////////////////////////
	// Randomize if needed and update parameters 
	cEditorWindowDecals* pWin = (cEditorWindowDecals*)mpWindow;
	UpdateLocalRotation();
	UpdateCreatorScale();
	mlCurrentSubDiv = pWin->GetCurrentSubDiv();

	//////////////////////////////////
	// Update random offset
	float fOffsetMin = cEntityWrapperTypeDecal::GetDecalOffsetMin();
	float fOffsetMax = cEntityWrapperTypeDecal::GetDecalOffsetMax();
	mfOffset = cMath::RandRectf(fOffsetMin, fOffsetMax);
}

//-----------------------------------------------------------------------------------

bool cEditorEditModeDecals::SetUpCreationData(iEntityWrapperData* apData)
{
	cEditorWindowDecals* pWin = (cEditorWindowDecals*)mpWindow;
	const tString& sMaterial = pWin->GetMaterial();
	if(sMaterial=="")
		return false;

	iEditorEditModeObjectCreator::SetUpCreationData(apData);

	/////////////////////////////////////
	// Build name
	tString sMatFilename = cString::GetFileName(sMaterial);
	tString sMatFileExt = cString::GetFileExt(sMaterial);
	int lMatFileExtStartPos = cString::GetLastStringPos(sMatFilename, sMatFileExt) - 1;
	tString sName = cString::Sub(sMatFilename, 0, lMatFileExtStartPos);
	apData->SetName(sName);

	cSurfacePicker* pPicker = mpEditor->GetEditorWorld()->GetSurfacePicker();

	//////////////////////////////////////
	// Set material
	apData->SetString(eDecalStr_Material, sMaterial);


	//////////////////////////////////////
	// Set material file index
	int lFileIndex = mpEditorWorld->AddFilenameToIndex("Decals", sMaterial);
	apData->SetInt(eDecalInt_FileIndex, lFileIndex);

	//////////////////////////////////////////////////
	// Set up affected types
	apData->SetBool(eDecalBool_AffectStatic, pPicker->IsAffected(eSurfaceType_StaticObject, false));
	apData->SetBool(eDecalBool_AffectPrimitive, pPicker->IsAffected(eSurfaceType_Primitive, false));
	apData->SetBool(eDecalBool_AffectEntity, pPicker->IsAffected(eSurfaceType_Entity, false));


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

iEditorWindow* cEditorEditModeDecals::CreateSpecificWindow()
{
	return hplNew(cEditorWindowDecals,(this));
}

//-----------------------------------------------------------------------------------

void cEditorEditModeDecals::CreateTypes()
{
	mvTypes.push_back(hplNew(cEntityWrapperTypeDecal,()));
}

//-----------------------------------------------------------------------------------
