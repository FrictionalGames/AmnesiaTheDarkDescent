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

#include "EditorEditModeSelectToolScale.h"

#include "EditorWindowViewport.h"

#include "EditorEditModeSelect.h"
#include "EditorSelection.h"

#include "EditorGrid.h"

#include "EditorActionSelection.h"


//----------------------------------------------------------------------

cEditorEditModeSelectToolScale::cEditorEditModeSelectToolScale(cEditorEditModeSelect* apParent, cEditorSelection* apSelection)  : cEditorEditModeSelectTool(eSelectToolMode_Scale, apParent, apSelection)
{
	for(int i=0;i<6;++i)
		mvCubeQuads[i].resize(4);
	Reset();
}

//----------------------------------------------------------------

bool cEditorEditModeSelectToolScale::IsActive()
{
	return mpSelection->CanScale();
}

//----------------------------------------------------------------

void cEditorEditModeSelectToolScale::DrawAxes(cEditorWindowViewport* apViewport, cRendererCallbackFunctions *apFunctions, float afAxisLength)
{

	float fHalfSide = 0.05f*afAxisLength;

	mvCubeQuads[0][0].pos = cVector3f(fHalfSide,-fHalfSide,fHalfSide);
	mvCubeQuads[0][1].pos = cVector3f(fHalfSide,fHalfSide,fHalfSide);
	mvCubeQuads[0][2].pos = cVector3f(fHalfSide,fHalfSide,-fHalfSide);
	mvCubeQuads[0][3].pos = cVector3f(fHalfSide,-fHalfSide,-fHalfSide);

	mvCubeQuads[1][0].pos = cVector3f(-fHalfSide,fHalfSide,fHalfSide);
	mvCubeQuads[1][1].pos = cVector3f(-fHalfSide,-fHalfSide,fHalfSide);
	mvCubeQuads[1][2].pos = cVector3f(-fHalfSide,-fHalfSide,-fHalfSide);
	mvCubeQuads[1][3].pos = cVector3f(-fHalfSide,fHalfSide,-fHalfSide);

	mvCubeQuads[2][0].pos = cVector3f(fHalfSide,fHalfSide,-fHalfSide);
	mvCubeQuads[2][1].pos = cVector3f(-fHalfSide,fHalfSide,-fHalfSide);
	mvCubeQuads[2][2].pos = cVector3f(-fHalfSide,-fHalfSide,-fHalfSide);
	mvCubeQuads[2][3].pos = cVector3f(fHalfSide,-fHalfSide,-fHalfSide);

	mvCubeQuads[3][0].pos = cVector3f(fHalfSide,fHalfSide,fHalfSide);
	mvCubeQuads[3][1].pos = cVector3f(fHalfSide,-fHalfSide,fHalfSide);
	mvCubeQuads[3][2].pos = cVector3f(-fHalfSide,-fHalfSide,fHalfSide);
	mvCubeQuads[3][3].pos = cVector3f(-fHalfSide,fHalfSide,fHalfSide);

	mvCubeQuads[4][0].pos = cVector3f(fHalfSide,fHalfSide,-fHalfSide);
	mvCubeQuads[4][1].pos = cVector3f(fHalfSide,fHalfSide,fHalfSide);
	mvCubeQuads[4][2].pos = cVector3f(-fHalfSide,fHalfSide,fHalfSide);
	mvCubeQuads[4][3].pos = cVector3f(-fHalfSide,fHalfSide,-fHalfSide);

	mvCubeQuads[5][0].pos = cVector3f(fHalfSide,-fHalfSide,fHalfSide);
	mvCubeQuads[5][1].pos = cVector3f(fHalfSide,-fHalfSide,-fHalfSide);
	mvCubeQuads[5][2].pos = cVector3f(-fHalfSide,-fHalfSide,-fHalfSide);
	mvCubeQuads[5][3].pos = cVector3f(-fHalfSide,-fHalfSide,fHalfSide);


	cMatrixf mtxTransform = cMath::MatrixMul(cMath::MatrixTranslate(mpSelection->GetCenterTranslation()),
											 cMath::MatrixRotate(mpSelection->GetCenterRotation(),eEulerRotationOrder_XYZ));

	apFunctions->SetMatrix(&mtxTransform);

	cVector3f vAxes[3];
	vAxes[0] = cVector3f(afAxisLength + mvAxisAddedLength.x,0,0);
	vAxes[1] = cVector3f(0,afAxisLength + mvAxisAddedLength.y,0);
	vAxes[2] = cVector3f(0,0,afAxisLength + mvAxisAddedLength.z);

	cColor col[3];

	for(int i=eSelectToolAxis_X; i<eSelectToolAxis_LastEnum; ++i)
	{
		col[i] = mvAxisSelected[i]?mColorSelected:(mvAxisMouseOver[i]?mColorMouseOver:mvAxisColor[i]);
		apFunctions->GetLowLevelGfx()->DrawLine(0, vAxes[i], col[i]);

		for(int j=0; j<6;++j)
			apFunctions->GetLowLevelGfx()->DrawQuad(mvCubeQuads[j],col[0] + col[1] + col[2]);

		// DEBUG: draw axes bounding boxes
		//apFunctions->GetLowLevelGfx()->DrawBoxMinMax(mvAxisMin[i], mvAxisMax[i], cColor(1,1));
		//apFunctions->GetLowLevelGfx()->DrawBoxMinMax(mvHeadMin[i], mvHeadMax[i], cColor(1,1));
	}
	
	float fCubeCenterOffset = 0.05f*afAxisLength;
	cMatrixf mtxTranslate[3];
	mtxTranslate[0] = cMath::MatrixMul(mtxTransform,cMath::MatrixTranslate(vAxes[0] - cVector3f(fCubeCenterOffset,0,0)));
	mtxTranslate[1] = cMath::MatrixMul(mtxTransform,cMath::MatrixTranslate(vAxes[1] - cVector3f(0,fCubeCenterOffset,0)));
	mtxTranslate[2] = cMath::MatrixMul(mtxTransform,cMath::MatrixTranslate(vAxes[2] - cVector3f(0,0,fCubeCenterOffset)));

	for(int i=0;i<3;++i)
	{
		apFunctions->SetMatrix(&mtxTranslate[i]);
		for(int j=0; j<6;++j)
			apFunctions->GetLowLevelGfx()->DrawQuad(mvCubeQuads[j],col[i]);
	}

	apFunctions->SetMatrix(NULL);

}

//----------------------------------------------------------------

cMatrixf& cEditorEditModeSelectToolScale::GetTransformMatrix()
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

void cEditorEditModeSelectToolScale::UpdateTransformation()
{
	iEditorBase* pEditor = mpEditMode->GetEditor();

	if(pEditor->GetFlags(eEditorFlag_MouseMoved)==false)
		return;

	float fScaleAmount = 0;

	cEditorWindowViewport* pViewport = pEditor->GetFocusedViewport();
	cVector2f vMousePos = pViewport->GetMouseViewportPosition();
	vMousePos.y = pViewport->GetGuiViewportSize().y - vMousePos.y;

	cVector3f vDisplacement = (vMousePos - mvOldMousePos);
	vDisplacement.z = 0;
	vDisplacement = vDisplacement * 0.05f;

	fScaleAmount = cMath::Vector3Dot(vDisplacement,mvScreenSpaceAxis);

	for(int i=0;i<3;++i)
	{
		if(mvAxisSelected[i])
		{
			vDisplacement.v[i] = fScaleAmount;
		}
		else
		{
			vDisplacement.v[i] = 0;
		}
		mvAxisAddedLength.v[i] = vDisplacement.v[i];
	}

	

	mpSelection->SetRelativeScale(vDisplacement,mpEditMode->GetEditor()->GetGrid()->GetSnapToGrid());

	mvEditVectorEnd = mvEditVectorStart + vDisplacement;

	pEditor->SetLayoutNeedsUpdate(true);

}

//----------------------------------------------------------------------

void cEditorEditModeSelectToolScale::CheckMouseOverAxis()
{
	iEditorBase* pEditor = mpEditMode->GetEditor();
	cEditorWindowViewport* pViewport = pEditor->GetFocusedViewport();

	bool bAllAxesSelected = cMath::CheckAABBLineIntersection(mvAllAxesHandleMin, 
															 mvAllAxesHandleMax, 
															 mvTransformedRayStart, 
															 mvTransformedRayEnd, 
															 NULL,NULL);

	for(int i=eSelectToolAxis_X;i<eSelectToolAxis_LastEnum;++i)
	{
		mvAxisMouseOver[i] = bAllAxesSelected ? true :
												(
													cMath::CheckAABBLineIntersection(mvAxisMin[i],
																					 mvAxisMax[i],
																					 mvTransformedRayStart,
																					 mvTransformedRayEnd,
																					 NULL,NULL) ||
													cMath::CheckAABBLineIntersection(mvHeadMin[i],
																					 mvHeadMax[i],
																					 mvTransformedRayStart,
																					 mvTransformedRayEnd,
																					 NULL,NULL));

		if(mvAxisMouseOver[i])
			mvScreenSpaceAxis.v[i] = 1;
		else
			mvScreenSpaceAxis.v[i] = 0;
	}

	mvScreenSpaceAxis = cMath::MatrixMul(cMath::MatrixRotate(mpSelection->GetCenterRotation(), eEulerRotationOrder_XYZ), mvScreenSpaceAxis);

	cCamera* pCam = pViewport->GetCamera();

	mvScreenSpaceAxis = cMath::MatrixMul(pCam->GetViewMatrix().GetRotation(), mvScreenSpaceAxis);
	mvScreenSpaceAxis.z = 0;
	mvScreenSpaceAxis.Normalize();

	mvOldMousePos = pViewport->GetMouseViewportPosition();
	mvOldMousePos.y = pViewport->GetGuiViewportSize().y - mvOldMousePos.y;

	

/*
	mvAxisMouseOver[eSelectToolAxis_X] = mvAxisMouseOver[eSelectToolAxis_X] || bRayIntersectedPlane[0] || bRayIntersectedPlane[2];
	mvAxisMouseOver[eSelectToolAxis_Y] = mvAxisMouseOver[eSelectToolAxis_Y] || bRayIntersectedPlane[0] || bRayIntersectedPlane[1];
	mvAxisMouseOver[eSelectToolAxis_Z] = mvAxisMouseOver[eSelectToolAxis_Z] || bRayIntersectedPlane[1] || bRayIntersectedPlane[2];
*/
}

//----------------------------------------------------------------------

bool cEditorEditModeSelectToolScale::CheckRayIntersectsAxis(eSelectToolAxis aeAxis, const cVector3f& avRayStart, const cVector3f& avRayEnd)
{
	cVector3f vMin = mvAxisMin[aeAxis];
	cVector3f vMax = mvAxisMax[aeAxis];

	return cMath::CheckAABBLineIntersection(vMin,vMax,avRayStart,avRayEnd, NULL, NULL);
}

//----------------------------------------------------------------------

void cEditorEditModeSelectToolScale::Reset()
{
	cEditorEditModeSelectTool::Reset();

	mvAxisAddedLength = 0;
}

//----------------------------------------------------------------------

void cEditorEditModeSelectToolScale::UpdateToolBoundingVolume()
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
	float fAxisLength = mfUsedAxisLength*0.9f;

	float fHeadHalfWidth = mfUsedAxisLength*0.07f;
	float fHeadWidth = fHeadHalfWidth*2;
	float fHeadLength = mfUsedAxisLength*0.1f;

	mvAllAxesHandleMin = cVector3f(-fHeadHalfWidth);
	mvAllAxesHandleMax = cVector3f(fHeadHalfWidth);

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

	/*
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

	float fAxisHalfWidth = mfUsedAxisLength*0.07f;
	float fAxisWidth = fAxisHalfWidth*2;

	mvAxisMin[eSelectToolAxis_X] = cVector3f(0,-fAxisHalfWidth,-fAxisHalfWidth);
	mvAxisMin[eSelectToolAxis_Y] = cVector3f(-fAxisHalfWidth,0, -fAxisHalfWidth);
	mvAxisMin[eSelectToolAxis_Z] = cVector3f(-fAxisHalfWidth,-fAxisHalfWidth,0);
	
	mvAxisMax[eSelectToolAxis_X] = mvAxisMin[eSelectToolAxis_X] + cVector3f(mfUsedAxisLength,fAxisWidth,fAxisWidth);
	mvAxisMax[eSelectToolAxis_Y] = mvAxisMin[eSelectToolAxis_Y] + cVector3f(fAxisWidth,mfUsedAxisLength,fAxisWidth); 
	mvAxisMax[eSelectToolAxis_Z] = mvAxisMin[eSelectToolAxis_Z] + cVector3f(fAxisWidth,fAxisWidth,mfUsedAxisLength);*/
}

//----------------------------------------------------------------------

iEditorAction* cEditorEditModeSelectToolScale::CreateAction()
{
	bool bUseSnap = mpEditMode->GetEditor()->GetGrid()->GetSnapToGrid();

	cVector3f vDisplacement = mvEditVectorEnd - mvEditVectorStart;
	mvEditVectorStart = 0;
	mvEditVectorEnd = 0;

	if(vDisplacement==0)
		return NULL;

	return hplNew(cEditorActionSelectionScale,(mpEditMode,mpEditMode->GetEditorWorld(), vDisplacement, bUseSnap) );
}


//----------------------------------------------------------------------


//----------------------------------------------------------------------
