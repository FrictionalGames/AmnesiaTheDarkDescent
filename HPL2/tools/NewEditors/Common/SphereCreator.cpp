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

#include "../Common/SphereCreator.h"
#include "../Common/Editor.h"
#include "../Common/EdEditMode.h"
#include "../Common/EdWindowViewport.h"

cSphereCreator::cSphereCreator(iEditor* apEditor) : iShapeCreator(apEditor)
{
	mfDefaultRadius = 1;
}

void cSphereCreator::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(aClick.mbLeft)
		StoreMousePosition(aClick);
	else if(aClick.mbRight)
		mvPoints.pop_back();
}

void cSphereCreator::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(aClick.mbLeft && mvPoints.size()<2)
		StoreMousePosition(aClick);
}

void cSphereCreator::Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions)
{
	apFunctions->SetProgram(NULL);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetMatrix(NULL);

	/*apFunctions->GetLowLevelGfx()->DrawSphere(vDebugPos, 0.1f, cColor(0,0,1,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vDebugPos, vDebugPos-cVector3f(vDebugPos.x,0,0), cColor(1,0,0,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vDebugPos, vDebugPos-cVector3f(0,vDebugPos.y,0), cColor(0,1,0,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vDebugPos, vDebugPos-cVector3f(0,0,vDebugPos.z), cColor(0,0,1,1));
	*/
	if(mvPoints.empty()==false && mbCanDrag==true)
	{
		apFunctions->GetLowLevelGfx()->DrawLine(mvPoints[0]-cVector3f(0.05f,0,0), mvPoints[0]+cVector3f(0.05f,0,0), cColor(1,1));
		apFunctions->GetLowLevelGfx()->DrawLine(mvPoints[0]-cVector3f(0,0.05f,0), mvPoints[0]+cVector3f(0,0.05f,0), cColor(1,1));
		apFunctions->GetLowLevelGfx()->DrawLine(mvPoints[0]-cVector3f(0,0,0.05f), mvPoints[0]+cVector3f(0,0,0.05f), cColor(1,1));
		apFunctions->GetLowLevelGfx()->DrawLine(mvPoints[0], mvTempPoint, cColor(1,1));
		apFunctions->GetLowLevelGfx()->DrawSphere(mvPoints[0], mfRadius, cColor(1,1));
	}
}

void cSphereCreator::OnUpdate()
{
	iEdViewport* pViewport = mpEditor->GetFocusedViewport();
	cEdGrid* pGrid = pViewport->GetGrid();

	mvTempPoint = pGrid->GetSnappedPosOnGrid(pViewport->GetMouseWorldPosition());
//	vDebugPos = mvTempPoint;
	if(mvPoints.size()>0)
		mfRadius = (mvTempPoint-mvPoints[0]).Length();
	else
	{
		cVector3f vGridNormal = 0;
		vGridNormal.v[pGrid->GetPlaneNormal()] = 1.0f;
		cVector3f vHeightPlaneNormal = cMath::Vector3Cross(vGridNormal, pViewport->GetEngCamera()->GetRight())*-1;
		vHeightPlaneNormal.Normalize();

		mSphRadiusPlane.FromNormalPoint(vHeightPlaneNormal, mvTempPoint);

		if(cMath::CheckPlaneLineIntersection(mSphRadiusPlane, pViewport->GetUnprojectedStart(), pViewport->GetUnprojectedEnd(), &mvTempPoint, NULL))
		{
			vDebugPos = mvTempPoint;			
		}
		if(pGrid->GetSnapToGrid()) mvTempPoint = pGrid->GetSnappedPosInSpace(mvTempPoint);
	}
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
	if(mbCanDrag==false || mfRadius<=kEpsilonf)
		return mfDefaultRadius;

    return mfRadius;
}
/*
void cSphereCreator::OnStoreMousePosition(cEdWindowViewport* apViewport, const cVector3f& avX)
{
	if(mvPoints.size()==1)
		mPlaneNormal = apViewport->GetGrid()->GetPlaneNormal();
}*/
