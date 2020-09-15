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

#include "EditorEditModeSelectToolTranslate.h"

#include "EditorWindowViewport.h"
#include "EditorEditModeSelect.h"

#include "EditorGrid.h"

//#include "EditorActionSelectionTranslate.h"
#include "EditorActionSelection.h"
#include "EditorSelection.h"

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// TOOL TRANSLATE
//////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

cEditorEditModeSelectToolTranslate::cEditorEditModeSelectToolTranslate(cEditorEditModeSelect* apParent, cEditorSelection* apSelection) : cEditorEditModeSelectTool(eSelectToolMode_Translate,apParent, apSelection)
{
	for(int i=0;i<4;++i)
		mvArrowQuads[i].resize(4);
}

//----------------------------------------------------------------

bool cEditorEditModeSelectToolTranslate::IsActive()
{
	return mpSelection->CanTranslate();
}

//----------------------------------------------------------------

bool cEditorEditModeSelectToolTranslate::CheckRayIntersectsAxis(eSelectToolAxis aeAxis, const cVector3f& avRayStart, const cVector3f& avRayEnd)
{
	cVector3f vMin = mvAxisMin[aeAxis];
	cVector3f vMax = mvAxisMax[aeAxis];

	return cMath::CheckAABBLineIntersection(vMin,vMax,avRayStart,avRayEnd, NULL, NULL);
}

//----------------------------------------------------------------

void cEditorEditModeSelectToolTranslate::CheckMouseOverAxis()
{
	cVector3f vAxisIntersect[3];
	cVector3f vHeadIntersect[3];
	cVector3f vPlaneIntersect[3];

	bool bRayIntersectedAxis[3];
	bool bRayIntersectedPlane[3];

	float fMinDist = 999999999.0f;

	for(int i=eSelectToolAxis_X;i<eSelectToolAxis_LastEnum;++i)
	{
		vAxisIntersect[i] = 99999999999.0f;
		vHeadIntersect[i] = 99999999999.0f;
		vPlaneIntersect[i] = 99999999999.0f;

		bRayIntersectedAxis[i] = cMath::CheckAABBLineIntersection(mvAxisMin[i],
															  mvAxisMax[i], 
															  mvTransformedRayStart,
															  mvTransformedRayEnd,
															  &vAxisIntersect[i],
															  NULL) ||
							 cMath::CheckAABBLineIntersection(mvHeadMin[i],
															  mvHeadMax[i], 
															  mvTransformedRayStart,
															  mvTransformedRayEnd,
															  &vHeadIntersect[i],
															  NULL);
		
		bRayIntersectedPlane[i] = cMath::CheckAABBLineIntersection(mvAxesPlaneBoxMin, 
																   mvAxesPlaneBoxMax[i],
																   mvTransformedRayStart,
																   mvTransformedRayEnd,
																   &vPlaneIntersect[i],
																   NULL);


		//////////////////////////////////////////////////////
		// Check closest intersection
		{
			float fDistToAxis;
			float fDistToHead;
			fDistToAxis = cMath::Vector3DistSqr(mvTransformedRayStart, vAxisIntersect[i]);
			fDistToHead = cMath::Vector3DistSqr(mvTransformedRayStart, vHeadIntersect[i]);
			if(fDistToAxis < fMinDist)
			{
				fMinDist = fDistToAxis;
			}
			else if(fDistToHead < fMinDist)
			{
				fMinDist = fDistToHead;
			}
			else
				bRayIntersectedAxis[i] = false;
		
			float fDistToPlane = cMath::Vector3DistSqr(mvTransformedRayStart, vPlaneIntersect[i]);
			if(fDistToPlane < fMinDist)
			{
				fMinDist = fDistToPlane;
			}
			else
				bRayIntersectedPlane[i] = false;
		}
	}

	mvAxisMouseOver[eSelectToolAxis_X] = bRayIntersectedAxis[eSelectToolAxis_X] || bRayIntersectedPlane[0] || bRayIntersectedPlane[2];
	mvAxisMouseOver[eSelectToolAxis_Y] = bRayIntersectedAxis[eSelectToolAxis_Y] || bRayIntersectedPlane[0] || bRayIntersectedPlane[1];
	mvAxisMouseOver[eSelectToolAxis_Z] = bRayIntersectedAxis[eSelectToolAxis_Z] || bRayIntersectedPlane[1] || bRayIntersectedPlane[2];	
}

//----------------------------------------------------------------

cMatrixf& cEditorEditModeSelectToolTranslate::GetTransformMatrix()
{
	//if(mpSelection->mbTransformed)
	//{
	mmtxTransformMatrix = cMath::MatrixTranslate(mpSelection->GetCenterTranslation()*-1);
	//	mpSelection->mbTransformed = false;
	//}
	
	return mmtxTransformMatrix;
}

//----------------------------------------------------------------

//----------------------------------------------------------------

void cEditorEditModeSelectToolTranslate::UpdateTransformation()
{
	iEditorBase* pEditor = mpEditMode->GetEditor();

	if(pEditor->GetFlags(eEditorFlag_MouseMoved)==false)
		return;

	cEditorWindowViewport* pViewport = pEditor->GetFocusedViewport();

	bool bIntersect = cMath::CheckPlaneLineIntersection(mEditingPlane, mvRayStart, mvRayEnd, &mvEditVectorEnd,NULL);

	if(bIntersect)
	{
		cVector3f vDisplacement = mvEditVectorEnd - mvEditVectorStart;

		for(int i=eSelectToolAxis_X; i<eSelectToolAxis_LastEnum; ++i)
		{
			if(mvAxisSelected[i]==false) 
				vDisplacement.v[i] = 0;
		}

		mvEditVectorEnd = mvEditVectorStart + vDisplacement;
		
		mpSelection->SetRelativeTranslation(vDisplacement, cEditorGrid::GetSnapToGrid());

		mpEditMode->GetEditor()->SetLayoutNeedsUpdate(true);
	}
}

//----------------------------------------------------------------

void cEditorEditModeSelectToolTranslate::UpdateToolBoundingVolume()
{
	cEditorWindowViewport *pViewport = mpEditMode->GetEditor()->GetFocusedViewport();
	cCamera* pCam = pViewport->GetCamera();

	cVector3f vViewpos = cMath::MatrixMul(pCam->GetViewMatrix(), mpSelection->GetCenterTranslation());

	if(pViewport->GetCamera()->GetProjectionType()==eProjectionType_Orthographic)
	{
		mfUsedAxisLength = pViewport->GetCamera()->GetOrthoViewSize().x*0.2f;
	}
	else
	{
		mfUsedAxisLength =  cMath::Abs(vViewpos.z)/mfAxisBaseLength;
	}

	float fAxisHalfWidth = mfUsedAxisLength*0.04f;
	float fAxisWidth = fAxisHalfWidth*2;
	float fAxisLength = mfUsedAxisLength*0.8f;

	float fHeadHalfWidth = mfUsedAxisLength*0.07f;
	float fHeadWidth = fHeadHalfWidth*2;
	float fHeadLength = mfUsedAxisLength*0.2f;

	mvAxisMin[eSelectToolAxis_X] = cVector3f(0,-fAxisHalfWidth,-fAxisHalfWidth);
	mvAxisMin[eSelectToolAxis_Y] = cVector3f(-fAxisHalfWidth,0, -fAxisHalfWidth);
	mvAxisMin[eSelectToolAxis_Z] = cVector3f(-fAxisHalfWidth,-fAxisHalfWidth,0);
	
	mvAxisMax[eSelectToolAxis_X] = mvAxisMin[eSelectToolAxis_X] + cVector3f(fAxisLength,fAxisWidth,fAxisWidth);
	mvAxisMax[eSelectToolAxis_Y] = mvAxisMin[eSelectToolAxis_Y] + cVector3f(fAxisWidth,fAxisLength,fAxisWidth);
	mvAxisMax[eSelectToolAxis_Z] = mvAxisMin[eSelectToolAxis_Z] + cVector3f(fAxisWidth,fAxisWidth,fAxisLength);

	mvHeadMin[eSelectToolAxis_X] = cVector3f(fAxisLength, -fHeadHalfWidth, -fHeadHalfWidth);
	mvHeadMin[eSelectToolAxis_Y] = cVector3f(-fHeadHalfWidth, fAxisLength, -fHeadHalfWidth);
	mvHeadMin[eSelectToolAxis_Z] = cVector3f(-fHeadHalfWidth, -fHeadHalfWidth, fAxisLength);

	mvHeadMax[eSelectToolAxis_X] = mvHeadMin[eSelectToolAxis_X] + cVector3f(fHeadLength,fHeadWidth,fHeadWidth);
	mvHeadMax[eSelectToolAxis_Y] = mvHeadMin[eSelectToolAxis_Y] + cVector3f(fHeadWidth,fHeadLength,fHeadWidth);
	mvHeadMax[eSelectToolAxis_Z] = mvHeadMin[eSelectToolAxis_Z] + cVector3f(fHeadWidth,fHeadWidth,fHeadLength);
	

	mvAxesPlaneBoxMin = 0;
	mvAxesPlaneBoxMax[0] = cVector3f(mfUsedAxisLength,mfUsedAxisLength,0) * 0.2f;
	mvAxesPlaneBoxMax[1] = cVector3f(0,mfUsedAxisLength, mfUsedAxisLength) * 0.2f;
	mvAxesPlaneBoxMax[2] = cVector3f(mfUsedAxisLength,0,mfUsedAxisLength) * 0.2f;
}

//----------------------------------------------------------------

void cEditorEditModeSelectToolTranslate::DrawAxes(cEditorWindowViewport* apViewport, cRendererCallbackFunctions *apFunctions, float afAxisLength)
{
	cVector3f vAxes[3];
	cColor col[3];

	float fX = 0.2f*afAxisLength;
	float fYZ = 0.05f*afAxisLength;

	mvArrowQuads[0][0].pos = cVector3f(-fX,fYZ,fYZ);
	mvArrowQuads[0][1].pos = cVector3f(0,0,0);
	mvArrowQuads[0][2].pos = cVector3f(0,0,0);
	mvArrowQuads[0][3].pos = cVector3f(-fX,-fYZ,fYZ);

	mvArrowQuads[1][0].pos = cVector3f(-fX,-fYZ,-fYZ);
	mvArrowQuads[1][1].pos = cVector3f(0,0,0);
	mvArrowQuads[1][2].pos = cVector3f(0,0,0);
	mvArrowQuads[1][3].pos = cVector3f(-fX,fYZ,-fYZ);

	mvArrowQuads[2][0].pos = cVector3f(-fX,fYZ,-fYZ);
	mvArrowQuads[2][1].pos = cVector3f(0,0,0);
	mvArrowQuads[2][2].pos = cVector3f(0,0,0);
	mvArrowQuads[2][3].pos = cVector3f(-fX,fYZ,fYZ);

	mvArrowQuads[3][0].pos = cVector3f(-fX,-fYZ,fYZ);
	mvArrowQuads[3][1].pos = cVector3f(0,0,0);
	mvArrowQuads[3][2].pos = cVector3f(0,0,0);
	mvArrowQuads[3][3].pos = cVector3f(-fX,-fYZ,-fYZ);

	
	cMatrixf mtxTransform = cMath::MatrixTranslate(mpSelection->GetCenterTranslation());
	apFunctions->SetMatrix(&mtxTransform);

	for(int i=eSelectToolAxis_X; i<eSelectToolAxis_LastEnum; ++i)
	{
		col[i] = mvAxisSelected[i] ? mColorSelected : (mvAxisMouseOver[i] ? mColorMouseOver : mvAxisColor[i]);
		vAxes[i] = 0;
		vAxes[i].v[i] = afAxisLength;

		apFunctions->GetLowLevelGfx()->DrawLine(0, vAxes[i], col[i]);

		// DEBUG: Draw axes bounding boxes
		//apFunctions->GetLowLevelGfx()->DrawBoxMinMax(mvAxisMin[i], mvAxisMax[i], cColor(1,1));
		//apFunctions->GetLowLevelGfx()->DrawBoxMinMax(mvHeadMin[i], mvHeadMax[i], cColor(1,1));
	}

	apFunctions->GetLowLevelGfx()->DrawBoxMinMax(0, (vAxes[0]+vAxes[1])*.2f, col[0] + col[1]);
	apFunctions->GetLowLevelGfx()->DrawBoxMinMax(0, (vAxes[0]+vAxes[2])*.2f, col[0] + col[2]);
	apFunctions->GetLowLevelGfx()->DrawBoxMinMax(0, (vAxes[1]+vAxes[2])*.2f, col[1] + col[2]);

	cMatrixf mtxXAxisHeadTransform = cMath::MatrixMul(cMath::MatrixTranslate(cVector3f(afAxisLength,0,0)),mtxTransform);
	cMatrixf mtxYAxisHeadTransform = cMath::MatrixMul(cMath::MatrixTranslate(cVector3f(0,afAxisLength,0)),
													  cMath::MatrixMul(mtxTransform, cMath::MatrixRotate(cVector3f(0,0,kPi2f),eEulerRotationOrder_XYZ)));
	cMatrixf mtxZAxisHeadTransform = cMath::MatrixMul(cMath::MatrixTranslate(cVector3f(0,0,afAxisLength)),
													  cMath::MatrixMul(mtxTransform, cMath::MatrixRotate(cVector3f(0,-kPi2f,0),eEulerRotationOrder_XYZ)));

	// TODO: Fixed
	apFunctions->SetMatrix(&mtxXAxisHeadTransform);
	for(int i=0; i<4;++i)
		apFunctions->GetLowLevelGfx()->DrawQuad(mvArrowQuads[i],col[0]);

	apFunctions->SetMatrix(&mtxYAxisHeadTransform);
	for(int i=0; i<4;++i)
		apFunctions->GetLowLevelGfx()->DrawQuad(mvArrowQuads[i],col[1]);

	apFunctions->SetMatrix(&mtxZAxisHeadTransform);
	for(int i=0; i<4;++i)
		apFunctions->GetLowLevelGfx()->DrawQuad(mvArrowQuads[i],col[2]);

}

//-----------------------------------------------------------------------------------

iEditorAction* cEditorEditModeSelectToolTranslate::CreateAction()
{
	bool bUseSnap = mpEditMode->GetEditor()->GetGrid()->GetSnapToGrid();
	cVector3f vDisplacement = mvEditVectorEnd - mvEditVectorStart;

	mvEditVectorStart = 0;
	mvEditVectorEnd = 0;

	if(vDisplacement==0)
		return NULL;

	return hplNew(cEditorActionSelectionTranslate,(mpEditMode, vDisplacement, bUseSnap) );
}

//-----------------------------------------------------------------------------------
