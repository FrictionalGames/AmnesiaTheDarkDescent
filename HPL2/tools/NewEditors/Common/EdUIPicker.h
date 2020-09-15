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

#ifndef ED_UI_PICKER_H
#define ED_UI_PICKER_H

#include "../Common/EdObjectPicker.h"

//----------------------------------------------------------------------------

class iEdViewport;
class iEdObjectType;
class iEdScnObjType;

//----------------------------------------------------------------------------

typedef std::map<iEdObjectType*, bool>	tTypeFilterMap;
typedef tTypeFilterMap::iterator		tTypeFilterMapIt;

//----------------------------------------------------------------------------

class cUIPickFilter : public iPickFilter
{
public:
	cUIPickFilter(const tString& asName);
	void SetViewport(iEdViewport* apViewport) { mpViewport = apViewport; }

	void Init();
    bool Passes(iEdScnObject*);

	void SetAllPass(bool abX) { mbAllPass = abX; }
	void SetTypeFilter(iEdObjectType*, bool);

	bool GetAllPass() { return mbAllPass; }
	bool GetTypeFilter(iEdObjectType*);

	int GetNumTypeFilters();
	int GetNumActiveTypeFilters();

	const tTypeFilterMap& GetTypeFilters() { return mmapTypeFilters; }
	void SetTypeFilters(const tTypeFilterMap& aTypeFilter) { mmapTypeFilters = aTypeFilter; }

protected:
	//virtual void OnFirstPass(iEdScnObject* apEntity) {}
	//virtual void OnSecondPass(iEdScnObject* apEntity) {}

	tString msName; 
	iEdViewport* mpViewport;
	tTypeFilterMap	mmapTypeFilters;
	bool mbAllPass;
};

class cUIPickMethod : public iPickMethod
{
public:
	cUIPickMethod();
	void SetViewport(iEdViewport* apViewport) { mpViewport = apViewport; }
	cPickData GetPickData();

	void Init(){}
	void PreIterateSetUp();
	bool Picks(iEdScnObject* apEnt);
	void PostPickCleanUp();

	virtual bool PickSpecific(iEdScnObject* apEnt)=0;

	void SetOutputList(tIntList* apList) { mpOutputIDList = apList; }

protected:
	iEdViewport* mpViewport;

	iEdScnObject* mpObject;
	cVector3f mvTempPosition;
	tVector3fVec mTempTriangle;
	float mfTempDistance;

	tIntList* mpOutputIDList;
};

#endif //ED_UI_PICKER_H
