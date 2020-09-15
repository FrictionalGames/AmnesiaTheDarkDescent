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

#include "SphereCreator.h"
#include "EditorBaseClasses.h"
#include "EditorEditMode.h"
#include "EditorWindowViewport.h"

cSphereCreator::cSphereCreator(iEditorEditMode* apEditMode) : iShapeCreator(apEditMode)
{
	mfDefaultRadius = 1;
}

void cSphereCreator::OnViewportMouseDown(int alButtons)
{
	if(alButtons==eGuiMouseButton_Left)
		StoreMousePosition(mvTempPoint);
	else if(alButtons==eGuiMouseButton_Right)
		mvPoints.pop_back();
}

void cSphereCreator::OnViewportMouseUp(int alButtons)
{
	if(alButtons==eGuiMouseButton_Left && mvPoints.size()<2)
		StoreMousePosition(mvTempPoint);
}

void cSphereCreator::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions)
{
	apFunctions->SetProgram(NULL);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetMatrix(NULL);

	/*apFunctions->GetLowLevelGfx()->DrawSphere(vDebugPos, 0.1f, cColor(0,0,1,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vDebugPos, vDebugPos-cVector3f(vDebugPos.x,0,0), cColor(1,0,0,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vDebugPos, vDebugPos-cVector3f(0,vDebugPos.y,0), cColor(0,1,0,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vDebugPos, vDebugPos-cVector3f(0,0,vDebugPos.z), cColor(0,0,1,1));
	*/
	if(mvPoints.empty()==false && mbDragging==true)
	{
		apFunctions->GetLowLevelGfx()->DrawLine(mvPoints[0]-cVector3f(0.05f,0,0), mvPoints[0]+cVector3f(0.05f,0,0), cColor(1,1));
		apFunctions->GetLowLevelGfx()->DrawLine(mvPoints[0]-cVector3f(0,0.05f,0), mvPoints[0]+cVector3f(0,0.05f,0), cColor(1,1));
		apFunctions->GetLowLevelGfx()->DrawLine(mvPoints[0]-cVector3f(0,0,0.05f), mvPoints[0]+cVector3f(0,0,0.05f), cColor(1,1));
		apFunctions->GetLowLevelGfx()->DrawLine(mvPoints[0], mvTempPoint, cColor(1,1));
		apFunctions->GetLowLevelGfx()->DrawSphere(mvPoints[0], mfRadius, cColor(1,1));
	}
}

void cSphereCreator::OnEditorUpdate()
{
	cEditorWindowViewport* pViewport = mpEditMode->GetEditor()->GetFocusedViewport();
	cEditorGrid* pGrid = pViewport->GetGrid();

	pViewport->GetMouseWorldPosition();
	mvTempPoint = pGrid->GetSnappedPosOnGrid(mpEditMode->GetEditor()->GetPosOnGridFromMousePos(false));
	vDebugPos = mvTempPoint;
	if(mvPoints.size()>0)
		mfRadius = (mvTempPoint-mvPoints[0]).Length();
	//else
	//{
		/*
		cVector3f vGridNormal = 0;
		vGridNormal.v[mPlaneNormal] = 1.0f;
		cVector3f vHeightPlaneNormal = cMath::Vector3Cross(vGridNormal, pViewport->GetCamera()->GetRight())*-1;
		vHeightPlaneNormal.Normalize();

		mSphRadiusPlane.FromNormalPoint(vHeightPlaneNormal, mvTempPoint);

		if(cMath::CheckPlaneLineIntersection(mSphRadiusPlane, pViewport->GetUnprojectedStart(), pViewport->GetUnprojectedEnd(), &mvTempPoint, NULL))
		{
			vDebugPos = mvTempPoint;			
		}
		if(pGrid->GetSnapToGrid()) mvTempPoint = pGrid->GetSnappedPosInSpace(mvTempPoint);
		*/
	//}
}

bool cSphereCreator::IsDoneCreating()
{
	return mvPoints.size()==2;
}

void cSphereCreator::Reset()
{
	mvPoints.clear();
}

cVector3f cSphereCreator::GetSphereCenter()
{
	if(mvPoints.size()==2)
		return mvPoints[0];
	else
		return 0;
}

float cSphereCreator::GetRadius()
{
	if(mbDragging==false || mfRadius<=kEpsilonf)
		return mfDefaultRadius;

    return mfRadius;
}

void cSphereCreator::OnStoreMousePosition(const cVector3f& avX)
{
	if(mvPoints.size()==1)
	{
		cEditorWindowViewport* pViewport = mpEditMode->GetEditor()->GetFocusedViewport();
		mPlaneNormal = pViewport->GetGrid()->GetPlaneNormal();
	}
}
