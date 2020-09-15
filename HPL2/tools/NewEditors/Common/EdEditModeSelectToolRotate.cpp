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

#include "../Common/EdEditModeSelectToolRotate.h"

#include "../Common/Editor.h"
#include "../Common/EdScnObject.h"

//#include "EditorEditModeSelect.h"
//#include "EditorSelection.h"

#include "../Common/EdViewport.h"

//#include "EditorActionSelection.h"
//#include "EditorGrid.h"

//----------------------------------------------------------------

cRotateManipulator::cRotateManipulator() : iScnObjManipulator(_W("GenericRotate"))
{
	mbTransformStarted = false;

	mfUsedAxisLength = mfAxisBaseLength;
	
	float fWidth = kPif*0.15f;
	float fHalfWidth = fWidth*0.5f;
	float fPi3_2 = kPif*1.5f;

	mRectAxisAreaSphCoords[eSelectToolAxis_X].push_back(cRect2f(0,kPi2f-fHalfWidth,k2Pif, fWidth));
	mRectAxisAreaSphCoords[eSelectToolAxis_X].push_back(cRect2f(0,fPi3_2-fHalfWidth, k2Pif, fWidth));

	mRectAxisAreaSphCoords[eSelectToolAxis_Y].push_back(cRect2f(kPi2f-fHalfWidth,0,fWidth,k2Pif));
	mRectAxisAreaSphCoords[eSelectToolAxis_Y].push_back(cRect2f(fPi3_2-fHalfWidth, 0,fWidth,k2Pif));

	mRectAxisAreaSphCoords[eSelectToolAxis_Z].push_back(cRect2f(0,0,k2Pif,fHalfWidth));
	mRectAxisAreaSphCoords[eSelectToolAxis_Z].push_back(cRect2f(0,k2Pif-fHalfWidth,k2Pif, fHalfWidth));
	mRectAxisAreaSphCoords[eSelectToolAxis_Z].push_back(cRect2f(0,kPif-fHalfWidth, k2Pif, fWidth));

	for(int i=0;i<3;++i)
	{
		mvAxisColor[i] = cColor(0,1);
		mvAxisColor[i].v[i] = 1;

		mvAxisMouseOver[i] = false;
		mvAxisSelected[i] = false;
	}
	
	mColorSelected = cColor(1,1,0,1);
	mColorMouseOver = cColor(0.5f,0.5f,0,1);

	mfAxisBaseLength = 3;
	mfUsedAxisLength = 3;
}

//----------------------------------------------------------------

bool cRotateManipulator::IsActive()
{
	return true;
	//return mpSelection->CanRotate();
}

//----------------------------------------------------------------

void cRotateManipulator::OnUpdate()
{
	if(mpTargetObject==NULL)
		return;

	UpdateMouseRay();
	UpdateToolBoundingVolume();

	if(mbTransformStarted)
		UpdateTransformation();
	else
		CheckMouseOverAxis();
}

bool cRotateManipulator::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(aClick.mbLeft==false || CheckAxisSelected()==false)
		return false;

	mbTransformStarted = true;
	StartTransform();

	mvOldRotation = mpTargetObject->GetRotation();
	mmtxOldRotation = mpTargetObject->GetRotationMatrix();

	return true;
}

bool cRotateManipulator::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(mbTransformStarted==false)
		return false;

	Reset();
	EndTransform();
	mbTransformStarted = false;

	cVector3f mvRotation = mpTargetObject->GetRotation();
	mpTargetObject->SetRotation(mvOldRotation);

	iEditor::GetInstance()->AddAction(mpTargetObject->CreateSetVector3fAction(eScnVec3f_Rotation, mvRotation));

	return true;
}

//----------------------------------------------------------------

void cRotateManipulator::CheckMouseOverAxis()
{
	cVector3f vIntersection[2];
	cVector2f vIntersectionSphCoords[2];
	float fT[2];
	int lClosestIntIndex;

	float fLowestT=999999999.0f;
	int lAxisWithLowestT=-1;

	iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();


	///////////////////////////////////////////////////////////
	// Better check both intersections
	if(cMath::CheckSphereLineIntersection(0,mfUsedAxisLength,mvTransformedRayStart,mvTransformedRayEnd,&fT[0],&vIntersection[0],&fT[1],&vIntersection[1]))
	{
		if(fT[0]!=-1 && fT[1]!=-1)
		{
			if(fT[0]<fT[1])
				lClosestIntIndex = 0;
			else
				lClosestIntIndex = 1;
		}
		else if(fT[0]!=-1)
			lClosestIntIndex = 0;
		else
			lClosestIntIndex = 1;
		
		for(int i=0;i<2;++i)
			vIntersectionSphCoords[i] = cMath::GetSphericalCoordsFromPoint3D(0,vIntersection[i]);

		
		cMatrixf mtxTransform = cMath::MatrixMul(mpTargetObject->GetTranslationMatrix(),
												 mpTargetObject->GetRotationMatrix());
		mvMouseDownCoords = cMath::MatrixMul(mtxTransform, vIntersection[lClosestIntIndex]);
		mvEditVectorStart = mvMouseDownCoords;

		bool bHasSelectedAnyAxis = false;
		for(int i=eSelectToolAxis_X;i<eSelectToolAxis_LastEnum;++i)
		{
			mvAxisMouseOver[i] = false;

			for(int j=0;j<2;++j)
			{
				if(PointIntersectsAxis((eSelectToolAxis)i, vIntersectionSphCoords[j]))
				{
					mvAxisMouseOver[i] = true;
					if(fT[j]<fLowestT)
					{
						lAxisWithLowestT = i;
						fLowestT = fT[j];
					}
				}
			}
		}
		for(int i=eSelectToolAxis_X;i<eSelectToolAxis_LastEnum;++i)
		{
			mvAxisMouseOver[i] = (i==lAxisWithLowestT);
		}
	}
	else
	{
		for(int i=eSelectToolAxis_X;i<eSelectToolAxis_LastEnum;++i)
			mvAxisMouseOver[i] = false;
	}


	mvOldMousePos = pViewport->GetMouseViewportPosition();
	mvOldMousePos.y = pViewport->GetGuiViewportSize().y - mvOldMousePos.y;

	mvMousePos = mvOldMousePos;

	cVector3f vCenterToPoint = mvMouseDownCoords-mpTargetObject->GetTranslation();
	cVector3f vRotationAxis;

	if(mvAxisMouseOver[0])
	{
		vRotationAxis = cVector3f(1,0,0);
	}
	else if(mvAxisMouseOver[1])
	{
		vRotationAxis = cVector3f(0,1,0);
	}
	else if(mvAxisMouseOver[2])
	{
		vRotationAxis = cVector3f(0,0,1);
	}

	vRotationAxis = cMath::MatrixMul(mpTargetObject->GetRotationMatrix(), vRotationAxis);
	mvTangentVector = cMath::Vector3Cross(vCenterToPoint, vRotationAxis);

	cCamera* pCam = pViewport->GetEngCamera();

	mvScreenSpaceTangentVector = cMath::MatrixMul(pCam->GetViewMatrix().GetRotation(), mvTangentVector);
	mvScreenSpaceTangentVector.z = 0;
	mvScreenSpaceTangentVector.Normalize();

	mvDisplacement = 0;
	
}

//----------------------------------------------------------------

bool cRotateManipulator::CheckAxisSelected()
{
	bool bAnySelected = false;
	for(int i=0; i<3; ++i)
	{
		mvAxisSelected[i] = mvAxisMouseOver[i];
		bAnySelected = bAnySelected || mvAxisSelected[i];
	}

	return bAnySelected;
}

//----------------------------------------------------------------

bool cRotateManipulator::PointIntersectsAxis(eSelectToolAxis aAxis, const cVector2f& avPoint)
{
	bool bIntersect = false;

	for(int i=0;i<(int)mRectAxisAreaSphCoords[aAxis].size();++i)
	{
		cRect2f& rect = mRectAxisAreaSphCoords[aAxis][i];

		bIntersect = bIntersect || cMath::CheckPointInRectIntersection(avPoint, rect);
	}

	return bIntersect;
}

//----------------------------------------------------------------

void cRotateManipulator::OnDraw(const cModuleDrawData& aData)
{
	if(mpTargetObject==NULL) return;

    aData.mpFunctions->SetDepthTest(false);
	aData.mpFunctions->SetTextureRange(NULL,0);

	aData.mpFunctions->SetMatrix(NULL);

	cVector3f vPos, vSize;
	aData.mpViewport->GetViewSpacePositionAndSize(mpTargetObject->GetTranslation(), 0.25f, vPos, vSize); 
	
	/*DEBUG*/
	//apFunctions->GetLowLevelGfx()->DrawLine(mvEditVectorStart, mvEditVectorEnd, cColor(1));

	//apFunctions->GetLowLevelGfx()->DrawSphere(mvEditVectorStart, 0.2f, cColor(0,0,1,1));
	//apFunctions->GetLowLevelGfx()->DrawSphere(mvEditVectorEnd, 0.2f, cColor(1,0,0,1));

	//apFunctions->GetLowLevelGfx()->DrawSphere(mpSelection->GetCenterTranslation(), mfUsedAxisLength, cColor(1));
	
	
	DrawAxes(aData.mpViewport, aData.mpFunctions, vSize.x);

	aData.mpFunctions->SetMatrix(NULL);
}

void cRotateManipulator::DrawAxes(iEdViewport* apViewport, cRendererCallbackFunctions *apFunctions, float afAxisLength)
{
	apFunctions->SetDepthTest(false);
	apFunctions->SetTextureRange(NULL,0);

	cCamera *pCam = apViewport->GetEngCamera();
	cMatrixf mtxCam = pCam->GetViewMatrix();
	cVector3f vNormal = pCam->GetForward()*-1;
	mClipPlane.FromNormalPoint(vNormal, mpTargetObject->GetTranslation()-vNormal*0.1f);

	apFunctions->GetLowLevelGfx()->SetClipPlane(0,mClipPlane);

	cColor col[3];

	for(int i=eSelectToolAxis_X; i<eSelectToolAxis_LastEnum; ++i)
		col[i] = mvAxisSelected[i]?mColorSelected:(mvAxisMouseOver[i]?mColorMouseOver:mvAxisColor[i]);


	cMatrixf mtxTransform = cMath::MatrixMul(mpTargetObject->GetTranslationMatrix(),
											 mpTargetObject->GetRotationMatrix());


	apFunctions->SetMatrix(&mtxTransform);


	apFunctions->GetLowLevelGfx()->SetClipPlaneActive(0,true);
	
	apFunctions->GetLowLevelGfx()->DrawSphere(0, afAxisLength, 
											  col[0],
											  col[1], 
											  col[2]);
	
	apFunctions->GetLowLevelGfx()->SetClipPlaneActive(0,false);


	apFunctions->SetMatrix(NULL);
}

//----------------------------------------------------------------

cMatrixf& cRotateManipulator::GetTransformMatrix()
{
	//if(mpSelection->mbTransformed)
	//{
	mmtxTransformMatrix = cMath::MatrixMul(mpTargetObject->GetTranslationMatrix(), mpTargetObject->GetRotationMatrix());
	mmtxTransformMatrix = cMath::MatrixInverse(mmtxTransformMatrix);
	//	mpSelection->mbTransformed = false;
	//}
	
	return mmtxTransformMatrix;
}

//----------------------------------------------------------------

void cRotateManipulator::UpdateTransformation()
{
	if(mvAxisSelected[0]==false && 
	   mvAxisSelected[1]==false &&
	   mvAxisSelected[2]==false)
		return;

	iEditor* pEditor = iEditor::GetInstance();

	cCamera* pCam = pEditor->GetFocusedViewport()->GetEngCamera();
	cVector3f vInt1, vInt2;
	//float fT1, fT2;
	cVector3f vClosestIntersection;
	cVector2f vIntSph;

	float fRotationAmount = 0;


	iEdViewport* pViewport = pEditor->GetFocusedViewport();
	mvMousePos = pViewport->GetMouseViewportPosition();
	mvMousePos.y = pViewport->GetGuiViewportSize().y - mvMousePos.y;

	mvDisplacement = (mvMousePos - mvOldMousePos);
	mvDisplacement.z = 0;
	mvDisplacement = mvDisplacement * -0.01f;

	fRotationAmount = cMath::Vector3Dot(mvDisplacement,mvScreenSpaceTangentVector);
	fRotationAmount = cEdGrid::GetSnappedValue(fRotationAmount, true, cMath::ToRad(15));
	
	if(mvAxisSelected[0])
	{
		mvDisplacement = cVector3f(fRotationAmount,0,0);
	}
	else if(mvAxisSelected[1])
	{
		mvDisplacement = cVector3f(0,fRotationAmount,0);
	}
	else if(mvAxisSelected[2]) 
	{
		mvDisplacement = cVector3f(0,0,fRotationAmount);
	}

	cMatrixf mtxRotation = cMath::MatrixRotate(mvDisplacement, eEulerRotationOrder_XYZ);
	mtxRotation = cMath::MatrixMul(mmtxOldRotation, mtxRotation);
	
	mpTargetObject->SetRotationMatrix(mtxRotation);
}

//----------------------------------------------------------------

void cRotateManipulator::UpdateToolBoundingVolume()
{
	iEdViewport *pViewport = iEditor::GetInstance()->GetFocusedViewport();

	cVector3f vPos, vSize;
	pViewport->GetViewSpacePositionAndSize(mpTargetObject->GetTranslation(), 0.25f, vPos, vSize);

	mfUsedAxisLength = vSize.x;
}

//----------------------------------------------------------------

void cRotateManipulator::Reset()
{
	for(int i=0;i<3;++i)
	{
		mvAxisMouseOver[i] = false;
		mvAxisSelected[i] = false;
	}
}

//----------------------------------------------------------------


void cRotateManipulator::UpdateMouseRay()
{
	iEdViewport* pViewport = iEditor::GetInstance()->GetFocusedViewport();

	mvRayStart = pViewport->GetUnprojectedStart();
	mvRayEnd = pViewport->GetUnprojectedEnd();

	cMatrixf mtxTransform = GetTransformMatrix();

	mvTransformedRayStart = cMath::MatrixMul(mtxTransform,mvRayStart);
	mvTransformedRayEnd = cMath::MatrixMul(mtxTransform,mvRayEnd);

    return;
}

