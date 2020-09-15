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

#include "EditorActionMisc.h"

#include "EditorWorld.h"
#include "EditorGrid.h"
#include "EditorClipPlane.h"


//--------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

iEditorActionAxisAlignedPlane::iEditorActionAxisAlignedPlane(iEditorAxisAlignedPlane* apPlane) : iEditorAction("Change editor plane properties")
{
	mpPlane = apPlane;
}

//--------------------------------------------------------------

cEditorActionPlaneChangeNormal::cEditorActionPlaneChangeNormal(iEditorAxisAlignedPlane* apPlane, int alNormal) : iEditorActionAxisAlignedPlane(apPlane)
{
	mlOldNormal = apPlane->GetPlaneNormal();
	mlNewNormal = alNormal;
}

void cEditorActionPlaneChangeNormal::Do()
{
	mpPlane->SetPlaneNormal((ePlaneNormal)mlNewNormal);
}

void cEditorActionPlaneChangeNormal::Undo()
{
	mpPlane->SetPlaneNormal((ePlaneNormal)mlOldNormal);
}

cEditorActionPlaneChangeHeight::cEditorActionPlaneChangeHeight(iEditorAxisAlignedPlane* apPlane, const cVector3f& avHeight) : iEditorActionAxisAlignedPlane(apPlane)
{
    mvOldHeights = mpPlane->GetHeights();
	mvNewHeights = avHeight;
}

void cEditorActionPlaneChangeHeight::Do()
{
	mpPlane->SetHeights(mvNewHeights);
}

void cEditorActionPlaneChangeHeight::Undo()
{
	mpPlane->SetHeights(mvOldHeights);
}

cEditorActionGridChangeSnapSeparation::cEditorActionGridChangeSnapSeparation(cEditorGrid* apGrid, 
																		 float afNewSnap) : iEditorAction("Change Grid Properties")
{
	mpGrid = apGrid;

	mfNewSnapSeparation = afNewSnap;

	mfOldSnapSeparation = mpGrid->GetSnapSeparation();
}

//--------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

void cEditorActionGridChangeSnapSeparation::Do()
{
	if(mpGrid==NULL) return;

	mpGrid->SetSnapSeparation(mfNewSnapSeparation);
}

//--------------------------------------------------------------

void cEditorActionGridChangeSnapSeparation::Undo()
{
	if(mpGrid==NULL) return;

	mpGrid->SetSnapSeparation(mfOldSnapSeparation);
}

//--------------------------------------------------------------

cEditorActionClipPlaneAdd::cEditorActionClipPlaneAdd(iEditorWorld* apWorld) : iEditorAction("Add Clip plane")
{
	mpWorld = apWorld;
}


void cEditorActionClipPlaneAdd::Do()
{
	mpPlane = mpWorld->AddClipPlane();
}

void cEditorActionClipPlaneAdd::Undo()
{
	mpWorld->RemoveClipPlane(mpPlane,false);
}

//--------------------------------------------------------------

cEditorActionClipPlaneRemove::cEditorActionClipPlaneRemove(iEditorWorld* apWorld, int alIdx) : iEditorAction("Remove Clip plane")
{
	mpWorld = apWorld;
	mlPlaneIdx = alIdx;
	mpPlane = mpWorld->GetClipPlane(mlPlaneIdx);
	mvOldClipPlanes = mpWorld->GetClipPlanes();
}

cEditorActionClipPlaneRemove::~cEditorActionClipPlaneRemove()
{
	bool bPlaneIsPresent = false;
	tEditorClipPlaneVec& vClipPlanes = mpWorld->GetClipPlanes();
	for(int i=0;i<(int)vClipPlanes.size();++i)
	{
		cEditorClipPlane* pPlane = vClipPlanes[i];
		if(pPlane==mpPlane)
		{
			bPlaneIsPresent = true;
			break;
		}
	}
	if(bPlaneIsPresent==false)
		hplDelete(mpPlane);
}

void cEditorActionClipPlaneRemove::Do()
{
	mpWorld->RemoveClipPlane(mpPlane, false);
}

void cEditorActionClipPlaneRemove::Undo()
{
	mpWorld->SetClipPlanes(mvOldClipPlanes);	
}

//--------------------------------------------------------------

cEditorActionClipPlaneSetHeight::cEditorActionClipPlaneSetHeight(iEditorWorld* apWorld, int alIdx, const cVector3f& avX) : cEditorActionPlaneChangeHeight(apWorld->GetClipPlane(alIdx), avX)
{
	mlPlaneIdx = alIdx;
	mpWorld = apWorld;
}

void cEditorActionClipPlaneSetHeight::Do()
{
	mpPlane = mpWorld->GetClipPlane(mlPlaneIdx);
	cEditorActionPlaneChangeHeight::Do();
}

void cEditorActionClipPlaneSetHeight::Undo()
{
	mpPlane = mpWorld->GetClipPlane(mlPlaneIdx);
	cEditorActionPlaneChangeHeight::Undo();
}


//--------------------------------------------------------------

cEditorActionClipPlaneSetNormal::cEditorActionClipPlaneSetNormal(iEditorWorld* apWorld, int alIdx, int aNormal) : cEditorActionPlaneChangeNormal(apWorld->GetClipPlane(alIdx), aNormal)
{
	mlPlaneIdx = alIdx;
	mpWorld = apWorld;
}

void cEditorActionClipPlaneSetNormal::Do()
{
	mpPlane = mpWorld->GetClipPlane(mlPlaneIdx);

	cEditorActionPlaneChangeNormal::Do();
}

void cEditorActionClipPlaneSetNormal::Undo()
{
	mpPlane = mpWorld->GetClipPlane(mlPlaneIdx);

	cEditorActionPlaneChangeNormal::Undo();	
}

//--------------------------------------------------------------

cEditorActionClipPlaneSetCullingOnPositiveSide::cEditorActionClipPlaneSetCullingOnPositiveSide(iEditorWorld* apWorld, int alIdx, bool abX) : iEditorAction("Change clip plane culling")
{
	mpWorld = apWorld;
	mlPlaneIdx = alIdx;

	mbNewCull = abX;
	cEditorClipPlane* pPlane = mpWorld->GetClipPlane(mlPlaneIdx);
	mbOldCull = pPlane->GetCullingOnPositiveSide();
}

void cEditorActionClipPlaneSetCullingOnPositiveSide::Do()
{
	cEditorClipPlane* pPlane = mpWorld->GetClipPlane(mlPlaneIdx);
	
	pPlane->SetCullingOnPositiveSide(mbNewCull);
}

void cEditorActionClipPlaneSetCullingOnPositiveSide::Undo()
{
	cEditorClipPlane* pPlane = mpWorld->GetClipPlane(mlPlaneIdx);
	
	pPlane->SetCullingOnPositiveSide(mbOldCull);
}

//--------------------------------------------------------------

cEditorActionClipPlaneSetActive::cEditorActionClipPlaneSetActive(iEditorWorld* apWorld, int alPlaneIdx, bool abActive) : iEditorAction("Change clip plane active")
{
	mpWorld = apWorld;
	mlPlaneIdx = alPlaneIdx;
	
	cEditorClipPlane* pPlane = mpWorld->GetClipPlane(mlPlaneIdx);
	mbOldActive = pPlane->IsActive();

	mbNewActive = abActive;
}

void cEditorActionClipPlaneSetActive::Do()
{
	cEditorClipPlane* pPlane = mpWorld->GetClipPlane(mlPlaneIdx);
	
	pPlane->SetActive(mbNewActive);
}

void cEditorActionClipPlaneSetActive::Undo()
{
	cEditorClipPlane* pPlane = mpWorld->GetClipPlane(mlPlaneIdx);

	pPlane->SetActive(mbOldActive);
}
