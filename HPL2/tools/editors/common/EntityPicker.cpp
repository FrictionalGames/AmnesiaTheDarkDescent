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

#include "EntityPicker.h"
#include "EditorWorld.h"

#include <algorithm>

//-----------------------------------------------------------------------------

bool SortHitsByDistance(const cPickData& apHit1, const cPickData& apHit2)
{
	return apHit1.mfDistance < apHit2.mfDistance;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

cEntityPicker::cEntityPicker(iEditorWorld* apWorld)
{
	mpWorld = apWorld;
	mpPickFilter = NULL;
	mpPickMethod = NULL;
}

//-----------------------------------------------------------------------------

void cEntityPicker::SetPickFilter(iPickFilter* apFilter)
{
	mpPickFilter = apFilter;
	if(mpPickFilter)
		mpPickFilter->Init();
}

void cEntityPicker::SetPickMethod(iPickMethod* apMethod)
{
	mpPickMethod = apMethod;
	if(mpPickMethod)
		mpPickMethod->Init();
}

//-----------------------------------------------------------------------------

void cEntityPicker::DrawDebug(cRendererCallbackFunctions* apFunctions)
{
	apFunctions->SetMatrix(NULL);
	for(int i=0;i<GetNumPicks();++i)
	{
		cPickData* pData = GetPick(i);
		apFunctions->GetLowLevelGfx()->DrawSphere(pData->mvIntersection, 0.1f, cColor(0,1,0,1));
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

int cEntityPicker::GetNumPicks()
{
	return (int)mvPicks.size();
}

//-----------------------------------------------------------------------------

cPickData* cEntityPicker::GetPick(int alX)
{
	if(alX<0 || alX>=GetNumPicks())
		return NULL;

	return &mvPicks[alX];
}

//-----------------------------------------------------------------------------

iEntityWrapper* cEntityPicker::GetClosestPick()
{
	///////////////////////////////////////////////
	// Picks are supposed to be ordered by distance
	if(mvPicks.empty())
		return NULL;
	else
		return mvPicks[0].mpEntity;
}

//-----------------------------------------------------------------------------

void cEntityPicker::Update()
{
	//if(mpPickFilter==NULL || 
	//	mpPickMethod==NULL)
	//	return;

	mvPicks.clear();

	if(mpPickMethod) 
		mpPickMethod->PreIterateSetUp();

	Iterate();
	
	// Sort picks by distance to view position
	if(mvPicks.empty()==false) sort(mvPicks.begin(), mvPicks.end(), SortHitsByDistance);
}

//-----------------------------------------------------------------------------

void cEntityPicker::Iterate()
{
	if(mpPickMethod==NULL)
		return;

	tEntityWrapperMap& entities = mpWorld->GetEntities();
	tEntityWrapperMap::const_iterator it = entities.begin();
	for(;it!=entities.end();++it)
	{
		iEntityWrapper* pEnt = it->second;
		if(mpPickFilter->Passes(pEnt)==false)
			continue;

        if(mpPickMethod->Picks(pEnt))
			mvPicks.push_back(mpPickMethod->GetPickData());// adds pick

		mpPickMethod->PostPickCleanUp();
	}

}

//-----------------------------------------------------------------------------
