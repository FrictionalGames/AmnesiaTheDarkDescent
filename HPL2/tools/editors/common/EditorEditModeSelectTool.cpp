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

#include "EditorEditModeSelectTool.h"

#include "EditorEditModeSelect.h"
#include "EditorSelection.h"

#include "EditorWindowViewport.h"

#include "EditorGrid.h"

//----------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// SELECT TOOL BASE
//////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

cEditorEditModeSelectTool::cEditorEditModeSelectTool(eSelectToolMode aToolMode, cEditorEditModeSelect* apEditMode, cEditorSelection* apSelection)
{
	mToolMode = aToolMode;
	mpEditMode = apEditMode;
	mpSelection = apSelection;

	for(int i=eSelectToolAxis_X;i<eSelectToolAxis_LastEnum;++i)
	{
		mvAxisColor[i] = cColor(0,1);
		mvAxisColor[i].v[i] = 1;
	}
	
	mColorSelected = cColor(1,1,0,1);
	mColorMouseOver = cColor(0.5f,0.5f,0,1);

	mfAxisBaseLength = 3;
	mfUsedAxisLength = 3;
	mfOrthoAxisPercentSize = 0.2f;
	
	Reset();
}

//----------------------------------------------------------------

void cEditorEditModeSelectTool::OnEditorUpdate()
{
	iEditorBase* pEditor = mpEditMode->GetEditor();
	if(pEditor->GetFlags(eEditorFlag_MouseMoved)==false)
		return;

	UpdateMouseRay();
	UpdateToolBoundingVolume();

	if(pEditor->GetFlags(eEditorFlag_Editing))
		UpdateTransformation();
	else
		CheckMouseOverAxis();
}

//----------------------------------------------------------------

bool cEditorEditModeSelectTool::OnViewportMouseDown(int alButtons)
{
	if((alButtons&eGuiMouseButton_Left)==0 || CheckAxisSelected()==false)
		return false;
	
	iEditorBase* pEditor = mpEditMode->GetEditor();
	pEditor->SetFlags(eEditorFlag_Editing, true);

	BuildEditPlane();

	cMath::CheckPlaneLineIntersection(mEditingPlane, mvRayStart, mvRayEnd, &mvEditVectorStart,NULL);
	mvEditVectorEnd = mvEditVectorStart;

	return true;
}


//----------------------------------------------------------------

bool cEditorEditModeSelectTool::OnViewportMouseUp(int alButtons)
{
	Reset();

	iEditorBase* pEditor = mpEditMode->GetEditor();

	if((alButtons&eGuiMouseButton_Left)==0 || pEditor->GetFlags(eEditorFlag_Editing)==false)
		return false;

	pEditor->SetFlags(eEditorFlag_Editing, false);

	return true;
}

//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

void cEditorEditModeSelectTool::Reset()
{
	for(int i=eSelectToolAxis_X;i<eSelectToolAxis_LastEnum;++i)
	{
		mvAxisSelected[i] = false;
		mvAxisMouseOver[i] = false;
	}
}

//----------------------------------------------------------------------
	
bool cEditorEditModeSelectTool::CheckAxisSelected()
{
	bool bAnySelected = false;
	for(int i=0; i<3; ++i)
	{
		mvAxisSelected[i] = mvAxisMouseOver[i];
		bAnySelected = bAnySelected || mvAxisSelected[i];
	}

	return bAnySelected;
}

//----------------------------------------------------------------------

void cEditorEditModeSelectTool::UpdateMouseRay()
{
	if(mpEditMode->GetEditor()->GetFlags(eEditorFlag_MouseMoved)==false)
		return;

	cEditorWindowViewport* pViewport = mpEditMode->GetEditor()->GetFocusedViewport();

	mvRayStart = pViewport->GetUnprojectedStart();
	mvRayEnd = mvRayStart + pViewport->GetUnprojectedDir()*pViewport->GetRayEndDistance();

	cMatrixf mtxTransform = GetTransformMatrix();

	mvTransformedRayStart = cMath::MatrixMul(mtxTransform,mvRayStart);
	mvTransformedRayEnd = cMath::MatrixMul(mtxTransform,mvRayEnd);

    return;
}

//----------------------------------------------------------------------

void cEditorEditModeSelectTool::Draw(cEditorWindowViewport* apViewport,cRendererCallbackFunctions *apFunctions)
{
	if(mpSelection->IsEmpty()) return;

    apFunctions->SetDepthTest(false);
	apFunctions->SetTextureRange(NULL,0);

	cCamera *pCam = apViewport->GetCamera();

	float fAxisLength;

	apFunctions->SetMatrix(NULL);

	cVector3f vViewPos;
	switch(pCam->GetProjectionType())
	{
	case eProjectionType_Orthographic:
		fAxisLength = pCam->GetOrthoViewSize().x * mfOrthoAxisPercentSize;
		break;
	case eProjectionType_Perspective:
		vViewPos = cMath::MatrixMul(pCam->GetViewMatrix(), mpSelection->GetCenterTranslation());
		fAxisLength = cMath::Abs(vViewPos.z)/mfAxisBaseLength;
		break;
	default:
		break;
	}

	/*DEBUG*/
	//apFunctions->GetLowLevelGfx()->DrawLine(mvEditVectorStart, mvEditVectorEnd, cColor(1));

	//apFunctions->GetLowLevelGfx()->DrawSphere(mvEditVectorStart, 0.2f, cColor(0,0,1,1));
	//apFunctions->GetLowLevelGfx()->DrawSphere(mvEditVectorEnd, 0.2f, cColor(1,0,0,1));

	//apFunctions->GetLowLevelGfx()->DrawSphere(mpSelection->GetCenterTranslation(), mfUsedAxisLength, cColor(1));
	
	
	DrawAxes(apViewport, apFunctions, fAxisLength);

	apFunctions->SetMatrix(NULL);
}

//----------------------------------------------------------------------
 
void cEditorEditModeSelectTool::BuildEditPlane()
{
		cVector3f vNormal;
		cVector3f vForward = mpEditMode->GetEditor()->GetFocusedViewport()->GetCamera()->GetForward();
		
		if(mvAxisSelected[eSelectToolAxis_X])
		{
			if(mvAxisSelected[eSelectToolAxis_Y])
				vNormal = cVector3f(0,0,1);
			else if(mvAxisSelected[eSelectToolAxis_Z])
				vNormal = cVector3f(0,1,0);
			else
				vNormal = vForward;
		}
		else if(mvAxisSelected[eSelectToolAxis_Y])
		{
			if(mvAxisSelected[eSelectToolAxis_Z])
				vNormal = cVector3f(1,0,0);
			else
                vNormal = vForward;
		}
		else if(mvAxisSelected[eSelectToolAxis_Z])
		{
			vNormal = vForward;
		}
		
		mEditingPlane.FromNormalPoint(vNormal, mpSelection->GetCenterTranslation());

		//Log("Edit plane : %s %s\n", vNormal.ToString().c_str(), mpSelection->GetCenterTranslation().ToString().c_str());
}

//----------------------------------------------------------------------
