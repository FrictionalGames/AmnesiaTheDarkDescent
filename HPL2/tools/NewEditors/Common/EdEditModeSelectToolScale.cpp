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

#include "../Common/EdEditModeSelectToolScale.h"

#include "../Common/Editor.h"

#include "../Common/EdWindowViewport.h"
#include "../Common/EdCamera.h"

#include "../Common/EdEditModeSelect.h"
#include "../Common/EdSelection.h"

#include "../Common/EdGrid.h"

//------------------------------------------------------------------------

cScaleAxis::cScaleAxis(iScnObjManipulator* apManipulator, const cMatrixf& amtxOffset) : iManipulatorComponent(apManipulator)
{
	cMeshManager* pMgr = iEditor::GetInstance()->GetEngine()->GetResources()->GetMeshManager();

	cMesh* pMesh = pMgr->CreateMesh("scale_axis.dae");
	
	mpVB = pMesh->GetSubMesh(0)->GetVertexBuffer();

	mBV.SetLocalMinMax(cVector3f(-0.1f, 0, -0.1f), cVector3f(0.1f, 1, 0.1f));

	mmtxOffset = amtxOffset;
	mvAxis = cMath::MatrixMul(mmtxOffset, cVector3f(0,1,0));
	for(int i=0; i<3; ++i)
		mvAxis.v[i] = cMath::RoundFloatToDecimals(mvAxis.v[i], 3);
	mbMouseOver = false;
}

//------------------------------------------------------------------------

void cScaleAxis::OnDraw(const cModuleDrawData& aData)
{
	cScaleManipulator* pParent = static_cast<cScaleManipulator*>(mpParent);
	cMatrixf mtxDrawMatrix = cMath::MatrixMul(pParent->mpTargetObject->GetTranslationMatrix(), 
										cMath::MatrixMul(pParent->mpTargetObject->GetRotationMatrix(),
														cMath::MatrixMul(mmtxOffset, pParent->mmtxDrawSize)));

	cColor col = mpParent->mpActiveComponent==this? (pParent->mbTransformStarted? cColor(1,1,1,1): cColor(1,1,0,1)) : cColor(mvAxis.x, mvAxis.y, mvAxis.z, 1);
	aData.mpFunctions->SetDepthTest(true);
	aData.mpFunctions->SetDepthWrite(false);
	aData.mpViewport->DrawSolidColorVertexBuffer(mpVB, &mtxDrawMatrix, col, aData.mpFunctions);
	aData.mpFunctions->SetDepthTest(false);
}

//------------------------------------------------------------------------

void cScaleAxis::OnUpdate()
{
	iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();
	cScaleManipulator* pParent = static_cast<cScaleManipulator*>(mpParent);

	cMatrixf mtxDrawMatrix = cMath::MatrixMul(pParent->mpTargetObject->GetTranslationMatrix(), 
										cMath::MatrixMul(pParent->mpTargetObject->GetRotationMatrix(),
														cMath::MatrixMul(mmtxOffset, pParent->mmtxSize)));
	mBV.SetTransform(mtxDrawMatrix);

	float fT;
	mbMouseOver = cMath::CheckAABBLineIntersection(mBV.GetMin(), mBV.GetMax(), pViewport->GetUnprojectedStart(), pViewport->GetUnprojectedEnd(), &mvMousePickPos, &fT);
	if(mbMouseOver && fT<pParent->mfT)
	{
		pParent->mfT = fT;
		pParent->SetActiveComponent(this);
        pParent->mvDragStartPos = mvMousePickPos*mvAxis;
	}
}

//------------------------------------------------------------------------

cPlanef cScaleAxis::GetEditPlane()
{
	cPlanef plane;

	iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();

	cVector3f vDirToIntersect = mvMousePickPos - pViewport->GetUnprojectedStart();
	vDirToIntersect.Normalize();

	cVector3f vUp = cMath::Vector3Cross(mvAxis, vDirToIntersect);
	cVector3f vNormal = cMath::Vector3Cross(vUp, mvAxis);

	plane.FromNormalPoint(vNormal, mvMousePickPos);

	return plane;
}

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// TOOL SCALE
//////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cScaleManipulator::cScaleManipulator(const tWString& asX) : iScnObjManipulator(asX)
{
	mbTransformStarted = false;
}

//------------------------------------------------------------------------

void cScaleManipulator::Init()
{
	AddComponent(hplNew(cScaleAxis,(this, cMath::MatrixRotateZ(cMath::ToRad(-90)))));
	AddComponent(hplNew(cScaleAxis,(this, cMatrixf::Identity)));
	AddComponent(hplNew(cScaleAxis,(this, cMath::MatrixRotateX(cMath::ToRad(90)))));
}

//------------------------------------------------------------------------

void cScaleManipulator::OnDraw(const cModuleDrawData& aData)
{
	if(mpTargetObject==NULL)
		return;

	cVector3f vPos, vSize;
	aData.mpViewport->GetViewSpacePositionAndSize(mpTargetObject->GetTranslation(), 0.25f, vPos, vSize);
	mmtxDrawSize = cMath::MatrixScale(vSize);
	iScnObjManipulator::OnDraw(aData);
}

//------------------------------------------------------------------------

void cScaleManipulator::OnUpdate()
{
	if(mpTargetObject==NULL)
		return;

	if(mbTransformStarted)
		UpdateTransform();
	else
	{
		iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();
		cVector3f vPos, vSize;
		pViewport->GetViewSpacePositionAndSize(mpTargetObject->GetTranslation(), 0.25f, vPos, vSize);

		mmtxSize = cMath::MatrixScale(vSize);
		mmtxRotate = mpTargetObject->GetRotationMatrix();

		mfT = pViewport->GetRayEndDistance();

		SetActiveComponent(NULL);

		iScnObjManipulator::OnUpdate();
	}
}

//------------------------------------------------------------------------

bool cScaleManipulator::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(mpActiveComponent)
	{
		StartTransform();
		return true;
	}

	return false;
}

//------------------------------------------------------------------------

bool cScaleManipulator::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(mpActiveComponent && mbTransformStarted)
	{
		EndTransform();
		return true;
	}

	return false;
}

//------------------------------------------------------------------------

void cScaleManipulator::StartTransform()
{
	mbTransformStarted = true;

	StoreOldTransform();

	cScaleAxis* pAxis = static_cast<cScaleAxis*>(mpActiveComponent);
	mPlane = pAxis->GetEditPlane();

	iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();

	cMath::CheckPlaneLineIntersection(mPlane, pViewport->GetUnprojectedStart(), pViewport->GetUnprojectedEnd(), &mvDragStartPos, NULL);
	mvDragStartPos = cMath::MatrixMul(cMath::MatrixInverse(mmtxRotate), mvDragStartPos);
	mvDragStartPos *= pAxis->GetAxis();
}

//------------------------------------------------------------------------

void cScaleManipulator::EndTransform()
{
	mbTransformStarted = false;

	StoreCurrentTransform();
	FallToOldTransform();

	/*cVector3f vNewScale = mpTargetObject->GetScale();

	mpTargetObject->SetScale(mvOldScale);
	iEditor::GetInstance()->AddAction(mpTargetObject->CreateSetVector3fAction(eScnVec3f_Scale, vNewScale));
	*/
}

//------------------------------------------------------------------------

void cScaleManipulator::UpdateTransform()
{
	if(mbTransformStarted==false) return;

	iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();

	cMath::CheckPlaneLineIntersection(mPlane, pViewport->GetUnprojectedStart(), pViewport->GetUnprojectedEnd(), &mvDragEndPos, NULL);
	mvDragEndPos = cMath::MatrixMul(cMath::MatrixInverse(mmtxRotate), mvDragEndPos);

	cScaleAxis* pAxis = static_cast<cScaleAxis*>(mpActiveComponent);
	const cVector3f& vAxis = pAxis->GetAxis();
	mvDragEndPos *= vAxis;

	mvTempScale = mvOldScale + mvDragEndPos-mvDragStartPos;
	for(int i=0; i<3; ++i)
	{
		mvTempScale.v[i] = cEdGrid::GetSnappedValue(mvTempScale.v[i], true, 0.25f);
	}

	SetTransform();
}

//----------------------------------------------------------------------

iEdAction* cScaleManipulator::CreateAction()
{
	return mpTargetObject->CreateSetVector3fAction(eScnVec3f_Scale, mvTempScale);
}

void cScaleManipulator::StoreOldTransform()
{
	mvOldScale = mpTargetObject->GetScale();
}

void cScaleManipulator::StoreCurrentTransform()
{
	mvNewScale = mpTargetObject->GetScale();
}

void cScaleManipulator::SetTransform()
{
	mpTargetObject->SetScale(mvTempScale);
}

void cScaleManipulator::FallToOldTransform()
{
	mpTargetObject->SetScale(mvOldScale);
}


//----------------------------------------------------------------------


//----------------------------------------------------------------------

//----------------------------------------------------------------------
