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

#ifndef HPLEDITOR_EDITOR_UI_PICKER_H
#define HPLEDITOR_EDITOR_UI_PICKER_H

#include "EntityPicker.h"

class cEditorWindowViewport;

class cUIPickFilter : public iPickFilter
{
public:
	cUIPickFilter(const tString& asName);
	void SetViewport(cEditorWindowViewport* apViewport) { mpViewport = apViewport; }

	void Init();
    bool Passes(iEntityWrapper* apEntity);

	void SetAllPass(bool abX) { mbAllPass = abX; }
	void SetTypeFilter(int alType, bool abX);

	bool GetAllPass() { return mbAllPass; }
	bool GetTypeFilter(int alType);

	int GetNumTypeFilters();
	int GetNumActiveTypeFilters();

protected:
	virtual bool FirstPass(iEntityWrapper* apEntity);
	virtual bool SecondPass(iEntityWrapper* apEntity);

	virtual void OnFirstPass(iEntityWrapper* apEntity) {}
	virtual void OnSecondPass(iEntityWrapper* apEntity) {}

	tString msName; 
	cEditorWindowViewport* mpViewport;
	std::map<int, bool> mmapTypeFilters;
	bool mbAllPass;
};

class cUIPickMethod : public iPickMethod
{
public:
	cUIPickMethod();
	void SetViewport(cEditorWindowViewport* apViewport) { mpViewport = apViewport; }
	cPickData GetPickData();

	void Init(){}
	void PreIterateSetUp();
	bool Picks(iEntityWrapper* apEnt);
	void PostPickCleanUp();

	virtual bool PickSpecific(iEntityWrapper* apEnt)=0;

	void SetOutputList(tIntList* apList) { mpOutputIDList = apList; }

protected:
	cEditorWindowViewport* mpViewport;

	iEntityWrapper* mpEntity;
	cVector3f mvTempPosition;
	tVector3fVec mTempTriangle;
	float mfTempDistance;

	tIntList* mpOutputIDList;
};

#endif //HPLEDITOR_EDITOR_UI_PICKER_H
