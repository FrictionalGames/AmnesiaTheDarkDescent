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

#include "../Common/BoxCreator.h"
#include "../Common/Editor.h"
#include "../Common/EdEditMode.h"
#include "../Common/EdWindowViewport.h"

//-----------------------------------------------------------

cBoxCreator::cBoxCreator(iEditor* apEditor) : iShapeCreator(apEditor)
{
	mvDefaultSize = 1;
}

//-----------------------------------------------------------

void cBoxCreator::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(aClick.mbLeft)
		PushPoint(mvTempPoint);
	else if(aClick.mbRight)
		PopPoint();
}

//-----------------------------------------------------------

void cBoxCreator::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(aClick.mbLeft)
	{
		if(mvPoints.size()<3)
		{
			PushPoint(mvTempPoint);
			mPlaneNormal = aClick.mpViewport->GetGrid()->GetPlaneNormal();
		}
	}
}

//-----------------------------------------------------------

void cBoxCreator::Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions)
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
		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(mvPoints[0], mvTempPoint, cColor(1,1));
}

//-----------------------------------------------------------

void cBoxCreator::OnUpdate()
{
	iEdViewport* pViewport = mpEditor->GetFocusedViewport();
	cEdGrid* pGrid = pViewport->GetGrid();

	if(mvPoints.size()<2)
	{
		mvTempPoint = pViewport->GetMouseWorldPosition();
		mvTempPoint = pGrid->GetSnappedPosOnGrid(mvTempPoint);
		mvDebugPos = mvTempPoint;
	}
	else
	{
		cVector3f vGridNormal = pGrid->GetPlane().GetNormal();
		cVector3f vHeightPlaneNormal = cMath::Vector3Cross(vGridNormal, pViewport->GetEngCamera()->GetRight())*-1;
		vHeightPlaneNormal.Normalize();

		mBoxHeightPlane.FromNormalPoint(vHeightPlaneNormal, mvTempPoint);

		if(cMath::CheckPlaneLineIntersection(mBoxHeightPlane, pViewport->GetUnprojectedStart(), pViewport->GetUnprojectedEnd(), &mvTempPoint, NULL))
		{
			mvDebugPos = mvTempPoint;
			for(int i=0;i<3;++i)
				if(i!=mPlaneNormal) mvTempPoint.v[i] = mvPoints[1].v[i];
		}
		if(pGrid->GetSnapToGrid()) mvTempPoint = pGrid->GetSnappedPosInSpace(mvTempPoint);
	}
}

//-----------------------------------------------------------

bool cBoxCreator::IsDoneCreating()
{
	if(mvPoints.size()==3)
		return true;
	else
	{
        if(mbCanDrag==false)
			return true;

		if(mvPoints.size()==2 && 
			(mvPoints[0]-mvPoints[1]).Length()<kEpsilonf)
			return true;
	}

	return false;
}

//-----------------------------------------------------------

void cBoxCreator::Reset()
{
	mvPoints.clear();
}

//-----------------------------------------------------------

cVector3f cBoxCreator::GetBoxCenter()
{
	if(mvPoints.size()==2)
	{
		if(mbCanDrag) return mvPoints[0];
		else return mvPoints[1];
	}
	else 
		return (mvPoints[0]+mvPoints[2])*0.5f;
}

//-----------------------------------------------------------

cVector3f cBoxCreator::GetBoxSize()
{
	if(mbCanDrag==false || mvPoints.size()==2)
		return mvDefaultSize;

	cVector3f vSize = mvPoints[2]-mvPoints[0];
	if(vSize.Length()<kEpsilonf)
		return mvDefaultSize;

	for(int i=0;i<3;++i)
		vSize.v[i] = cMath::Abs(vSize.v[i]);

    return vSize;
}

//-----------------------------------------------------------
/*
void cBoxCreator::OnStoreMousePosition(cEdWindowViewport* apViewport, const cVector3f& avX)
{
	if(mvPoints.size()==2)
	{
		cEdWindowViewport* pViewport = mpEditMode->GetEditor()->GetFocusedViewport();
		mPlaneNormal = pViewport->GetGrid()->GetPlaneNormal();
	}
}*/

//-----------------------------------------------------------
