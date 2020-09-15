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

#include "../Common/EdUIPicker.h"

#include "../Common/Editor.h"

#include "../Common/EdScnObject.h"
#include "../Common/EdViewport.h"

//----------------------------------------------------------------------

/////////////////////////////////////////////////////

cUIPickFilter::cUIPickFilter(const tString& asName)
{
	msName = asName;
	mpViewport = NULL;
	mbAllPass = false;
}

//----------------------------------------------------------------------

void cUIPickFilter::Init()
{
}

//----------------------------------------------------------------------

bool cUIPickFilter::Passes(iEdScnObject* apObj)
{
	return apObj->PassesFilter(this);
}

//----------------------------------------------------------------------

void cUIPickFilter::SetTypeFilter(iEdObjectType* apType, bool abX)
{
	mmapTypeFilters[apType] = abX;
}

//----------------------------------------------------------------------

bool cUIPickFilter::GetTypeFilter(iEdObjectType* apType)
{
	if(mbAllPass) return true;

	tTypeFilterMap::const_iterator it = mmapTypeFilters.find(apType);
	if(it==mmapTypeFilters.end())
		return false;

	return it->second;
}

//----------------------------------------------------------------------

int cUIPickFilter::GetNumTypeFilters()
{
	return (int)mmapTypeFilters.size();
}

int cUIPickFilter::GetNumActiveTypeFilters()
{
	int lCount = 0;

	// Count how many bools are set to true in the filters map
	tTypeFilterMapIt it = mmapTypeFilters.begin();
	for(;it!=mmapTypeFilters.end();++it)
	{
		if(it->second)
			++lCount;
	}

	return lCount;
}

//----------------------------------------------------------------------

cUIPickMethod::cUIPickMethod()
{
	mpViewport = NULL;

	mpObject = NULL;
	mfTempDistance = 0;
	mvTempPosition = 0;
}

cPickData cUIPickMethod::GetPickData()
{
	cPickData data;

	data.mpObject = mpObject;
	data.mfDistance = mfTempDistance;
	data.mvIntersection = mvTempPosition;
	data.mvTriangle = mTempTriangle;

	return data;
}

void cUIPickMethod::PreIterateSetUp()
{
	mpViewport = iEditor::GetInstance()->GetFocusedViewport();
}

bool cUIPickMethod::Picks(iEdScnObject* apObj)
{
	if(PickSpecific(apObj))
	{
		mpObject = apObj;
		mfTempDistance = cMath::Vector3Dist(mpViewport->GetUnprojectedStart(), mvTempPosition);

		return true;
	}

	return false;
}

void cUIPickMethod::PostPickCleanUp()
{
	mpObject = NULL;
	mTempTriangle.clear();
}

//----------------------------------------------------------------------

/*
bool cUIPickMethodRay::PickSpecific(iEdScnObject* apEnt)
{
	return apEnt->CheckRayIntersect(mpViewport, &mvTempPosition, &mTempTriangle);
}

bool cUIPickMethodBox::PickSpecific(iEdScnObject* apEnt)
{
	if(apEnt->Check2DBoxIntersect(mpViewport, mRect, NULL))
	{
		mvTempPosition = apEnt->GetTranslation();
		return true;
	}
	return false;
}
*/
