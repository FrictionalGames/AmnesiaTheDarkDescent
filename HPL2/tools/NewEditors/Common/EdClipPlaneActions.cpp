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

#include "../Common/EdClipPlaneActions.h"

#include "../Common/EdScnWorld.h"
#include "../Common/EdClipPlane.h"


//--------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

cEdActionClipPlaneAdd::cEdActionClipPlaneAdd(iEdScnWorld* apMap) : iEdAction(_W("Add Clip plane"))
{
	mpMap = apMap;
	mlID = mpMap->GetFreeClipPlaneID();
}

bool cEdActionClipPlaneAdd::Init()
{
	cEdClipPlane* pPlane = mpMap->GetClipPlane(mlID);
	return pPlane==NULL;
}

void cEdActionClipPlaneAdd::Do()
{
	mpMap->AddClipPlane(mlID);
	//mpPlane = mpWorld->AddClipPlane();
}

void cEdActionClipPlaneAdd::Undo()
{
	mpMap->RemoveClipPlane(mlID);
	//mpWorld->RemoveClipPlane(mpPlane,false);
}

//--------------------------------------------------------------

cEdActionClipPlaneRemove::cEdActionClipPlaneRemove(iEdScnWorld* apMap, int alIdx) : iEdAction(_W("Remove Clip plane"))
{
	mpMap = apMap;
	mlID = alIdx;
	//mpPlane = mpWorld->GetClipPlane(mlPlaneIdx);
	//mvOldClipPlanes = mpWorld->GetClipPlanes();
}

cEdActionClipPlaneRemove::~cEdActionClipPlaneRemove()
{
	//bool bPlaneIsPresent = false;


	//tEdClipPlaneList& vClipPlanes = mpWorld->GetClipPlanes();
	/*for(int i=0;i<(int)vClipPlanes.size();++i)
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
		*/
}

bool cEdActionClipPlaneRemove::Init()
{
	cEdClipPlane* pPlane = mpMap->GetClipPlane(mlID);

	if(pPlane)
	{
		mbCullSide = pPlane->GetCullingOnPositiveSide();
		mvHeights = pPlane->GetHeights();
		mlNormal = (int)pPlane->GetPlaneNormal();
	}

	return pPlane!=NULL;
}

void cEdActionClipPlaneRemove::Do()
{
	mpMap->RemoveClipPlane(mlID);
}

void cEdActionClipPlaneRemove::Undo()
{
	cEdClipPlane* pPlane = mpMap->AddClipPlane(mlID);

	pPlane->SetCullingOnPositiveSide(mbCullSide);
	pPlane->SetHeights(mvHeights);
	pPlane->SetPlaneNormal((ePlaneNormal)mlNormal);
}

//--------------------------------------------------------------

cEdActionClipPlaneSetHeight::cEdActionClipPlaneSetHeight(cEdClipPlane* apPlane, const cVector3f& avX) : cEdActionPlaneChangeHeight(apPlane, avX)
{
	mpMap = apPlane->GetMap();
	mlID = apPlane->GetID();
}

void cEdActionClipPlaneSetHeight::Do()
{
	mpPlane = mpMap->GetClipPlane(mlID);
	cEdActionPlaneChangeHeight::Do();
}

void cEdActionClipPlaneSetHeight::Undo()
{
	mpPlane = mpMap->GetClipPlane(mlID);
	cEdActionPlaneChangeHeight::Undo();
}

//--------------------------------------------------------------

cEdActionClipPlaneSetNormal::cEdActionClipPlaneSetNormal(cEdClipPlane* apPlane, int aNormal) : cEdActionPlaneChangeNormal(apPlane, aNormal)
{
	mpMap = apPlane->GetMap();
	mlID = apPlane->GetID();
}

void cEdActionClipPlaneSetNormal::Do()
{
	mpPlane = mpMap->GetClipPlane(mlID);

	cEdActionPlaneChangeNormal::Do();
}

void cEdActionClipPlaneSetNormal::Undo()
{
	mpPlane = mpMap->GetClipPlane(mlID);

	cEdActionPlaneChangeNormal::Undo();	
}

//--------------------------------------------------------------

cEdActionClipPlaneSetCullingOnPositiveSide::cEdActionClipPlaneSetCullingOnPositiveSide(cEdClipPlane* apPlane, bool abX) : iEdAction(_W("Change clip plane culling"))
{
	mpMap = apPlane->GetMap();
	mlID = apPlane->GetID();

	mbNewCull = abX;
	mbOldCull = apPlane->GetCullingOnPositiveSide();
}

void cEdActionClipPlaneSetCullingOnPositiveSide::Do()
{
	cEdClipPlane* pPlane = mpMap->GetClipPlane(mlID);
	pPlane->SetCullingOnPositiveSide(mbNewCull);
}

void cEdActionClipPlaneSetCullingOnPositiveSide::Undo()
{
	cEdClipPlane* pPlane = mpMap->GetClipPlane(mlID);
	pPlane->SetCullingOnPositiveSide(mbOldCull);
}

//--------------------------------------------------------------

cEdActionClipPlaneSetActive::cEdActionClipPlaneSetActive(cEdClipPlane* apPlane, bool abActive) : iEdAction(_W("Change clip plane active"))
{
	mpMap = apPlane->GetMap();
	mlID = apPlane->GetID();

	mbNewActive = abActive;
	mbOldActive = apPlane->IsActive();
}

void cEdActionClipPlaneSetActive::Do()
{
	cEdClipPlane* pPlane = mpMap->GetClipPlane(mlID);
	pPlane->SetActive(mbNewActive);
}

void cEdActionClipPlaneSetActive::Undo()
{
	cEdClipPlane* pPlane = mpMap->GetClipPlane(mlID);
	pPlane->SetActive(mbOldActive);
}
