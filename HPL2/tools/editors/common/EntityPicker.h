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

#ifndef HPLEDITOR_ENTITY_PICKER_H
#define HPLEDITOR_ENTITY_PICKER_H

class iEntityWrapper;
class iEditorWorld;

#include "StdAfx.h"

using namespace hpl;

//---------------------------------------------------------------------------------

class cPickData;
class cEditorWindowViewport;

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

class cPickData
{
public:
    iEntityWrapper*	mpEntity;
	cVector3f		mvIntersection;
	float			mfDistance;
	tVector3fVec	mvTriangle;
};

//---------------------------------------------------------------------------------

typedef std::vector<cPickData> tPickVec;

//---------------------------------------------------------------------------------

class iPickFilter
{
public:
	virtual void Init()=0;

	virtual bool Passes(iEntityWrapper* apEntity)=0;
};

//---------------------------------------------------------------------------------

class iPickMethod
{
public:
	virtual void Init()=0;
	virtual void PreIterateSetUp()=0;

	virtual bool Picks(iEntityWrapper* apEntity)=0;
	virtual void PostPickCleanUp()=0;
	
	virtual cPickData GetPickData()=0;
};

//---------------------------------------------------------------------------------

class cEntityPicker
{
public:
	cEntityPicker(iEditorWorld* apWorld);

	void SetPickFilter(iPickFilter* apFilter);
	void SetPickMethod(iPickMethod* apMethod);

	void DrawDebug(cRendererCallbackFunctions* apFunctions);

	int GetNumPicks();
	cPickData* GetPick(int alX);
	iEntityWrapper* GetClosestPick();

	const tPickVec& GetPicks() { return mvPicks; }

	void Update();
protected:
	virtual void Iterate();
	virtual void OnDraw(cRendererCallbackFunctions* apFunctions){}

	iEditorWorld* mpWorld;

	iPickFilter* mpPickFilter;
	iPickMethod* mpPickMethod;

	tPickVec mvPicks;
};

//---------------------------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_PICKER_H
