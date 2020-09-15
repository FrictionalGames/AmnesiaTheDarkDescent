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

#include "EditorEditModeSelectToolRotate.h"

#include "EditorEditModeSelect.h"
#include "EditorSelection.h"

#include "EditorWindowViewport.h"

#include "EditorActionSelection.h"
#include "EditorGrid.h"

//----------------------------------------------------------------

cEditorEditModeSelectToolRotate::cEditorEditModeSelectToolRotate(cEditorEditModeSelect* apParent, cEditorSelection* apSelection) : cEditorEditModeSelectTool(eSelectToolMode_Rotate, apParent, apSelection)
{
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
}

//----------------------------------------------------------------

bool cEditorEditModeSelectToolRotate::IsActive()
{
	return mpSelection->CanRotate();
}

//----------------------------------------------------------------

void cEditorEditModeSelectToolRotate::CheckMouseOverAxis()
{
	cVector3f vIntersection[2];
	cVector2f vIntersectionSphCoords[2];
	float fT[2];
	int lClosestIntIndex;

	float fLowestT=999999999.0f;
	int lAxisWithLowestT=-1;

	iEditorBase* pEditor = mpEditMode->GetEditor();
	cEditorWindowViewport* pViewport = pEditor->GetFocusedViewport();


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

		
		cMatrixf mtxTransform = cMath::MatrixMul(cMath::MatrixTranslate(mpSelection->GetCenterTranslation()),
												 cMath::MatrixRotate(mpSelection->GetCenterRotation(),eEulerRotationOrder_XYZ));
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

	cVector3f vCenterToPoint = mvMouseDownCoords-mpSelection->GetCenterTranslation();
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

	vRotationAxis = cMath::MatrixMul(cMath::MatrixRotate(mpSelection->GetCenterRotation(), eEulerRotationOrder_XYZ), vRotationAxis);
	mvTangentVector = cMath::Vector3Cross(vCenterToPoint, vRotationAxis);

	cCamera* pCam = pViewport->GetCamera();

	mvScreenSpaceTangentVector = cMath::MatrixMul(pCam->GetViewMatrix().GetRotation(), mvTangentVector);
	mvScreenSpaceTangentVector.z = 0;
	mvScreenSpaceTangentVector.Normalize();

	mvDisplacement = 0;
	
}

//----------------------------------------------------------------

bool cEditorEditModeSelectToolRotate::PointIntersectsAxis(eSelectToolAxis aAxis, const cVector2f& avPoint)
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

void cEditorEditModeSelectToolRotate::DrawAxes(cEditorWindowViewport* apViewport, cRendererCallbackFunctions *apFunctions, float afAxisLength)
{
	apFunctions->SetDepthTest(false);
	apFunctions->SetTextureRange(NULL,0);

	cCamera *pCam = apViewport->GetCamera();
	cMatrixf mtxCam = pCam->GetViewMatrix();
	cVector3f vNormal = pCam->GetForward()*-1;
	mClipPlane.FromNormalPoint(vNormal, mpSelection->GetCenterTranslation()-vNormal*0.1f);

	apFunctions->GetLowLevelGfx()->SetClipPlane(0,mClipPlane);

	cColor col[3];

	for(int i=eSelectToolAxis_X; i<eSelectToolAxis_LastEnum; ++i)
		col[i] = mvAxisSelected[i]?mColorSelected:(mvAxisMouseOver[i]?mColorMouseOver:mvAxisColor[i]);


	cMatrixf mtxTransform = cMath::MatrixMul(cMath::MatrixTranslate(mpSelection->GetCenterTranslation()),
											 cMath::MatrixRotate(mpSelection->GetCenterRotation(),eEulerRotationOrder_XYZ));


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

cMatrixf& cEditorEditModeSelectToolRotate::GetTransformMatrix()
{
	//if(mpSelection->mbTransformed)
	//{
		mmtxTransformMatrix = cMath::MatrixMul(cMath::MatrixRotate(mpSelection->GetCenterRotation()*-1,eEulerRotationOrder_ZYX),
												cMath::MatrixTranslate(mpSelection->GetCenterTranslation()*-1));
	//	mpSelection->mbTransformed = false;
	//}
	
	return mmtxTransformMatrix;
}

//----------------------------------------------------------------

void cEditorEditModeSelectToolRotate::UpdateTransformation()
{
	iEditorBase* pEditor = mpEditMode->GetEditor();

	if(pEditor->GetFlags(eEditorFlag_MouseMoved)==false)
		return;

	if(mvAxisSelected[0]==false && 
	   mvAxisSelected[1]==false &&
	   mvAxisSelected[2]==false)
		return;

	cCamera* pCam = mpEditMode->GetEditor()->GetFocusedViewport()->GetCamera();
	cVector3f vInt1, vInt2;
	//float fT1, fT2;
	cVector3f vClosestIntersection;
	cVector2f vIntSph;

	float fRotationAmount = 0;


	cEditorWindowViewport* pViewport = mpEditMode->GetEditor()->GetFocusedViewport();
	mvMousePos = pViewport->GetMouseViewportPosition();
	mvMousePos.y = pViewport->GetGuiViewportSize().y - mvMousePos.y;

	mvDisplacement = (mvMousePos - mvOldMousePos);
	mvDisplacement.z = 0;
	mvDisplacement = mvDisplacement * -0.01f;

	//mvEditVectorStart = mvMouseDownCoords;

	fRotationAmount = cMath::Vector3Dot(mvDisplacement,mvScreenSpaceTangentVector);

	//mvEditVectorEnd = mvEditVectorStart + mvScreenSpaceTangentVector*fRotationAmount;
	
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
	
	mpSelection->SetRelativeRotation(mvDisplacement,mpEditMode->GetEditor()->GetGrid()->GetSnapToGrid());

	mpEditMode->GetEditor()->SetLayoutNeedsUpdate(true);
}

//----------------------------------------------------------------

void cEditorEditModeSelectToolRotate::UpdateToolBoundingVolume()
{
	cEditorWindowViewport *pViewport = mpEditMode->GetEditor()->GetFocusedViewport();
	cCamera* pCam = pViewport->GetCamera();

	if(pViewport->GetCamera()->GetProjectionType()==eProjectionType_Orthographic)
	{
		mfUsedAxisLength = pCam->GetOrthoViewSize().x*mfOrthoAxisPercentSize;
	}
	else
	{
		cVector3f vViewpos = cMath::MatrixMul(pCam->GetViewMatrix(), mpSelection->GetCenterTranslation());
		mfUsedAxisLength =  cMath::Abs(vViewpos.z)/mfAxisBaseLength;
	}
}

//----------------------------------------------------------------

iEditorAction* cEditorEditModeSelectToolRotate::CreateAction()
{
	bool bUseSnap = mpEditMode->GetEditor()->GetGrid()->GetSnapToGrid();

	cVector3f vDisplacement = mvDisplacement;
	mvDisplacement = 0;
	if(vDisplacement==0)
		return NULL;

	return hplNew(cEditorActionSelectionRotate,(mpEditMode, mpEditMode->GetEditorWorld(), vDisplacement, bUseSnap) );
}

//----------------------------------------------------------------

void cEditorEditModeSelectToolRotate::Reset()
{
	cEditorEditModeSelectTool::Reset();
}

//----------------------------------------------------------------
