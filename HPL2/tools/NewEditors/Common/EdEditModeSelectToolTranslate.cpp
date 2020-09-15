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

#include "../Common/EdEditModeSelectToolTranslate.h"

#include "../Common/EdWindowViewport.h"
#include "../Common/EdCamera.h"
#include "../Common/EdScnObject.h"
//#include "../Common/EdEditModeSelect.h"

#include "../Common/Editor.h"

//------------------------------------------------------------------------

iTranslateComponent::iTranslateComponent(iScnObjManipulator* apParent) : iManipulatorComponent(apParent)
{
	mbMouseOver = false;
	mvMousePickPos = 0;
}

cTranslateAxis::cTranslateAxis(iScnObjManipulator* apManipulator, const cMatrixf& amtxOffset) : iTranslateComponent(apManipulator)
{
	cMeshManager* pMgr = iEditor::GetInstance()->GetEngine()->GetResources()->GetMeshManager();

	cMesh* pMesh = pMgr->CreateMesh("translate_axis.dae");
	
	mpVB = pMesh->GetSubMesh(0)->GetVertexBuffer();

	mBV.SetLocalMinMax(cVector3f(-0.1f, 0, -0.1f), cVector3f(0.1f, 1, 0.1f));

	mmtxOffset = amtxOffset;
	mvAxis = cMath::MatrixMul(mmtxOffset, cVector3f(0,1,0));
	for(int i=0; i<3; ++i)
		mvAxis.v[i] = cMath::RoundFloatToDecimals(mvAxis.v[i], 3);
}

//------------------------------------------------------------------------

void cTranslateAxis::OnDraw(const cModuleDrawData& aData)
{
	cTranslateManipulator* pParent = static_cast<cTranslateManipulator*>(mpParent);
	cMatrixf mtxDrawMatrix = cMath::MatrixMul(pParent->mpTargetObject->GetTranslationMatrix(), 
												cMath::MatrixMul(mmtxOffset, pParent->mmtxDrawSize));

	bool bHighlight = (mpParent->mpActiveComponent==this);
	if(bHighlight==false)
	{
		for(size_t i=0; i<mvPlanes.size(); ++i)
		{
			cTranslatePlane* pPlane = mvPlanes[i];

			bHighlight = (mpParent->mpActiveComponent==pPlane);
			if(bHighlight) break;
		}
	}

	cColor col = bHighlight? (pParent->mbTransformStarted? cColor(1,1,1,1): cColor(1,1,0,1)) : cColor(mvAxis.x, mvAxis.y, mvAxis.z, 1);
	aData.mpFunctions->SetDepthTest(false);
	aData.mpFunctions->SetDepthWrite(false);
	aData.mpViewport->DrawSolidColorVertexBuffer(mpVB, &mtxDrawMatrix, col, aData.mpFunctions);
	aData.mpFunctions->SetDepthTest(false);

	/*
	cColor col = mpParent->mpActiveComponent==this? (pParent->mbTransformStarted? cColor(0,1,0,1) : cColor(1,0,1,1)) : cColor(1,0,0,1);
	aData.mpFunctions->GetLowLevelGfx()->DrawBoxMinMax(mBV.GetMin(), mBV.GetMax(), col);
	*/
}

//------------------------------------------------------------------------

void cTranslateAxis::OnUpdate()
{
	iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();
	cTranslateManipulator* pParent = static_cast<cTranslateManipulator*>(mpParent);

	mBV.SetTransform(cMath::MatrixMul(pParent->mpTargetObject->GetTranslationMatrix(), cMath::MatrixMul(mmtxOffset, pParent->mmtxSize)));

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

cPlanef cTranslateAxis::GetEditPlane()
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

void cTranslateAxis::AddPlane(cTranslatePlane* apPlane)
{
	mvPlanes.push_back(apPlane);
}

cVector3f cTranslateAxis::GetRestrainedPos(const cVector3f& avPos)
{
	return mvAxis*avPos;
}

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// TRANSLATE AXIS
//////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cTranslatePlane::cTranslatePlane(iScnObjManipulator* apParent, 
								 cTranslateAxis* apAxis1, cTranslateAxis* apAxis2) : iTranslateComponent(apParent)
{
	mpAxis1 = apAxis1;
	mpAxis1->AddPlane(this);

	mpAxis2 = apAxis2;
	mpAxis2->AddPlane(this);

	mBV.SetLocalMinMax(cVector3f(0, 0, 0), (mpAxis1->GetAxis()+mpAxis2->GetAxis())*0.3f);

	mbMouseOver = false;
}

//------------------------------------------------------------------------

cPlanef cTranslatePlane::GetEditPlane()
{
	cPlanef plane;

	iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();

	cVector3f vNormal = cMath::Vector3Cross(mpAxis1->GetAxis(), mpAxis2->GetAxis());

	plane.FromNormalPoint(vNormal, mvMousePickPos);

	return plane;
}

//------------------------------------------------------------------------

void cTranslatePlane::OnDraw(const cModuleDrawData& aData)
{
}

//------------------------------------------------------------------------

void cTranslatePlane::OnUpdate()
{
	iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();
	cTranslateManipulator* pParent = static_cast<cTranslateManipulator*>(mpParent);

	mBV.SetTransform(cMath::MatrixMul(pParent->mpTargetObject->GetTranslationMatrix(), pParent->mmtxSize));

	float fT;
	mbMouseOver = cMath::CheckAABBLineIntersection(mBV.GetMin(), mBV.GetMax(), pViewport->GetUnprojectedStart(), pViewport->GetUnprojectedEnd(), &mvMousePickPos, &fT);
	if(mbMouseOver && fT<pParent->mfT)
	{
		pParent->mfT = fT;
		pParent->SetActiveComponent(this);
        pParent->mvDragStartPos = mvMousePickPos*GetAxis();
	}
}

cVector3f cTranslatePlane::GetAxis()
{
	return mpAxis1->GetAxis()+mpAxis2->GetAxis();
}

cVector3f cTranslatePlane::GetRestrainedPos(const cVector3f& avPos)
{
	return avPos;
}

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// TOOL TRANSLATE
//////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cTranslateManipulator::cTranslateManipulator() : iScnObjManipulator(_W("GenericTranslate"))
{
	mbTransformStarted = false;

	cTranslateAxis* pAxis_X = hplNew(cTranslateAxis,(this, cMath::MatrixRotate(cVector3f(0, 0, cMath::ToRad(-90)), eEulerRotationOrder_XYZ)));
	cTranslateAxis* pAxis_Y = hplNew(cTranslateAxis,(this, cMath::MatrixRotate(cVector3f(0, cMath::ToRad(90), 0), eEulerRotationOrder_XYZ)));
	cTranslateAxis* pAxis_Z = hplNew(cTranslateAxis,(this, cMath::MatrixRotate(cVector3f(cMath::ToRad(90), 0, cMath::ToRad(-180)), eEulerRotationOrder_XYZ)));

	cTranslatePlane* pPlane_XY = hplNew(cTranslatePlane,(this, pAxis_X, pAxis_Y));
	cTranslatePlane* pPlane_XZ = hplNew(cTranslatePlane,(this, pAxis_X, pAxis_Z));
	cTranslatePlane* pPlane_YZ = hplNew(cTranslatePlane,(this, pAxis_Y, pAxis_Z));


	mvComponents.push_back(pAxis_X);
	mvComponents.push_back(pAxis_Y);
	mvComponents.push_back(pAxis_Z);
	mvComponents.push_back(pPlane_XY);
	mvComponents.push_back(pPlane_XZ);
	mvComponents.push_back(pPlane_YZ);
}

//------------------------------------------------------------------------

void cTranslateManipulator::OnDraw(const cModuleDrawData& aData)
{
	if(mpTargetObject==NULL)
		return;

	cVector3f vPos, vSize;
	aData.mpViewport->GetViewSpacePositionAndSize(mpTargetObject->GetTranslation(), 0.25f, vPos, vSize);
	mmtxDrawSize = cMath::MatrixScale(vSize);
	iScnObjManipulator::OnDraw(aData);

	/*
	aData.mpFunctions->GetLowLevelGfx()->DrawLine(mvDragStartPos, cColor(0,1,0,1), mvDragStartPos+mvDragStartOffset, cColor(0,0,1,1));
	aData.mpFunctions->GetLowLevelGfx()->DrawLine(aData.mpViewport->GetUnprojectedStart(), cColor(1,0,0,1), mvDragStartPos, cColor(0,1,0,1));

	aData.mpFunctions->GetLowLevelGfx()->DrawLine(mvDragStartPos, cColor(1,0,0,1), mvDragEndPos,  cColor(0,1,0,1));
	*/
}

//------------------------------------------------------------------------

void cTranslateManipulator::OnUpdate()
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

		mfT = pViewport->GetRayEndDistance();

		SetActiveComponent(NULL);

		iScnObjManipulator::OnUpdate();
	}
}

//------------------------------------------------------------------------

bool cTranslateManipulator::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(mpActiveComponent)
	{
		StartTransform();
		return true;
	}

	return false;
}

//------------------------------------------------------------------------

bool cTranslateManipulator::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(mpActiveComponent && mbTransformStarted)
	{
		EndTransform();
		return true;
	}

	return false;
}

//------------------------------------------------------------------------

void cTranslateManipulator::StartTransform()
{
	mbTransformStarted = true;

	mvOldTranslation = mpTargetObject->GetTranslation();

	mvDragStartOffset = mvOldTranslation-mvDragStartPos;
	iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();

	iTranslateComponent* pAxis = reinterpret_cast<iTranslateComponent*>(mpActiveComponent);
	mPlane = pAxis->GetEditPlane();
}

//------------------------------------------------------------------------

void cTranslateManipulator::EndTransform()
{
	mbTransformStarted = false;

	cVector3f vNewTranslation = mpTargetObject->GetTranslation();

	mpTargetObject->SetTranslation(mvOldTranslation);
	iEditor::GetInstance()->AddAction(mpTargetObject->CreateSetVector3fAction(eScnVec3f_Translation, vNewTranslation));
}

//------------------------------------------------------------------------

void cTranslateManipulator::UpdateTransform()
{
	if(mbTransformStarted==false) return;

	iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();

	cMath::CheckPlaneLineIntersection(mPlane, pViewport->GetUnprojectedStart(), pViewport->GetUnprojectedEnd(), &mvDragEndPos, NULL);

	iTranslateComponent* pComponent = static_cast<iTranslateComponent*>(mpActiveComponent);
	cVector3f vAxis = pComponent->GetAxis();
	mvDragEndPos = mvDragEndPos*vAxis;
	
	cVector3f vNewTranslate = pViewport->GetGrid()->GetSnappedPosInSpace(mvDragEndPos+mvDragStartOffset, vAxis.x>kEpsilonf, vAxis.y>kEpsilonf, vAxis.z>kEpsilonf);
	
	mpTargetObject->SetTranslation(vNewTranslate);
}

//----------------------------------------------------------------------
