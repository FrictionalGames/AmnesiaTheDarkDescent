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

#include "../Common/EdObjectPicker.h"
#include "../Common/EdWorld.h"

#include "../Common/Editor.h"
#include "../Common/EdScnObject.h"

#include <algorithm>

//-----------------------------------------------------------------------------

bool SortHitsByDistance(const cPickData& apHit1, const cPickData& apHit2)
{
	return apHit1.mfDistance < apHit2.mfDistance;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

cEdObjectPicker::cEdObjectPicker(iEdWorld* apWorld)
{
	mpWorld = apWorld;
	mpPickFilter = NULL;
	mpPickMethod = NULL;

	mbNeedsUpdate = false;
}

//-----------------------------------------------------------------------------

void cEdObjectPicker::SetPickFilter(iPickFilter* apFilter)
{
	if(mpPickFilter==apFilter)
		return;

	mpPickFilter = apFilter;
	if(mpPickFilter)
		mpPickFilter->Init();

	SetUpdated();
}

void cEdObjectPicker::SetPickMethod(iPickMethod* apMethod)
{
	if(mpPickMethod==apMethod)
		return;

	mpPickMethod = apMethod;
	if(mpPickMethod)
		mpPickMethod->Init();

	SetUpdated();
}

//-----------------------------------------------------------------------------

void cEdObjectPicker::DrawDebug(cRendererCallbackFunctions* apFunctions)
{
	apFunctions->SetMatrix(NULL);
	for(int i=0;i<GetNumPicks();++i)
	{
		cPickData* pData = GetPick(i);
		apFunctions->GetLowLevelGfx()->DrawSphere(pData->mvIntersection, 2, cColor(0,1,0,1));
	}

	cPickData* pData = GetPick(0);
	if(pData)
	{
		tVector3fVec& vTri = pData->mvTriangle;
		if(vTri.empty()==false)
		{
			apFunctions->GetLowLevelGfx()->DrawLine(vTri[0], vTri[1],cColor(0,1,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(vTri[1], vTri[2],cColor(0,1,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(vTri[2], vTri[0],cColor(0,1,0,1));
		}
	}

	OnDraw(apFunctions);
}

//-----------------------------------------------------------------------------

int cEdObjectPicker::GetNumPicks()
{
	return (int)mvPicks.size();
}

//-----------------------------------------------------------------------------

cPickData* cEdObjectPicker::GetPick(int alX)
{
	if(alX<0 || alX>=GetNumPicks())
		return NULL;

	return &mvPicks[alX];
}

//-----------------------------------------------------------------------------

iEdScnObject* cEdObjectPicker::GetClosestPick()
{
	///////////////////////////////////////////////
	// Picks are supposed to be ordered by distance
	if(mvPicks.empty())
		return NULL;
	else
		return mvPicks[0].mpObject;
}

//-----------------------------------------------------------------------------

void cEdObjectPicker::Update()
{
	//if(mpPickFilter==NULL || 
	//	mpPickMethod==NULL)
	//	return;
	if(mbNeedsUpdate==false)
		return;

	mbNeedsUpdate = false;

	mvPicks.clear();

	if(mpPickMethod) 
		mpPickMethod->PreIterateSetUp();

	Iterate();
	
	// Sort picks by distance to view position
	if(mvPicks.empty()==false) sort(mvPicks.begin(), mvPicks.end(), SortHitsByDistance);
}

//-----------------------------------------------------------------------------

void cEdObjectPicker::Iterate()
{
	if(mpPickMethod==NULL)
		return;

	iEdViewport* pViewport = mpWorld->GetEditor()->GetFocusedViewport();
	cBoundingVolume frustumBV = pViewport->GetEngCamera()->GetFrustum()->GetBoundingVolume();
    
	tScnObjList lstObjects;
	/////////////////////////////
	// Get Container
    cWorld *pWorld = mpWorld->GetEngWorld();
	iRenderableContainer* pContainer = pWorld->GetRenderableContainer(eWorldContainerType_Dynamic);
	
	/////////////////////////////
	// Search nodes in container
	pContainer->UpdateBeforeRendering();
    IterateRenderableNode(pContainer->GetRoot(), &frustumBV);
}

//-----------------------------------------------------------------------------

void cEdObjectPicker::IterateRenderableNode(iRenderableContainerNode *apNode, cBoundingVolume* apBV)
{
	apNode->UpdateBeforeUse();

	if(apNode->GetParent()!=NULL)
	{
		if(cMath::CheckAABBIntersection(apNode->GetMin(), apNode->GetMax(), apBV->GetMin(), apBV->GetMax())==false) 
			return;
	}

	/////////////////////////////
	//Iterate objects
	if(apNode->HasObjects())
	{
		tRenderableListIt it = apNode->GetObjectList()->begin();
		for(; it!=apNode->GetObjectList()->end(); ++it)
		{
			iRenderable *pRenderable = *it;
			iEdScnObject* pObj = static_cast<iEdScnObject*>(pRenderable->GetRenderableUserData());
            
			if(pObj==NULL || 
				pObj->IsVisible()==false || 
				pRenderable->CollidesWithBV(apBV)==false)
				continue;

			iEdScnObject* pObject = NULL;
			if(pObj->CheckPicked((cUIPickFilter*)mpPickFilter, (cUIPickMethod*)mpPickMethod, &pObject))
			{
				cPickData pick = mpPickMethod->GetPickData();
				pick.mpObject = pObject;

				mvPicks.push_back(pick);
			}
			
			mpPickMethod->PostPickCleanUp();
		}
	}

	////////////////////////
	//Iterate children
	if(apNode->HasChildNodes())
	{
		tRenderableContainerNodeListIt childIt = apNode->GetChildNodeList()->begin();
		for(; childIt != apNode->GetChildNodeList()->end(); ++childIt)
		{
			iRenderableContainerNode *pChildNode = *childIt;
			IterateRenderableNode(pChildNode, apBV);
		}
	}
}
