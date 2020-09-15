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

#ifndef ED_OBJECT_PICKER_H
#define ED_OBJECT_PICKER_H

class iEdScnObject;
class iEdWorld;

#include "../Common/StdAfx.h"

using namespace hpl;

//---------------------------------------------------------------------------------

class cPickData;
class iEdViewport;

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

class cPickData
{
public:
    iEdScnObject*	mpObject;
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

	virtual bool Passes(iEdScnObject*)=0;
};

//---------------------------------------------------------------------------------

class iPickMethod
{
public:
	virtual void Init()=0;
	virtual void PreIterateSetUp()=0;

	virtual bool Picks(iEdScnObject*)=0;
	virtual void PostPickCleanUp()=0;
	
	virtual cPickData GetPickData()=0;
};

//---------------------------------------------------------------------------------

class cEdObjectPicker
{
public:
	cEdObjectPicker(iEdWorld*);

	void SetUpdated() { mbNeedsUpdate=true; }

	void SetPickFilter(iPickFilter*);
	void SetPickMethod(iPickMethod*);

	void DrawDebug(cRendererCallbackFunctions*);

	int GetNumPicks();
	cPickData* GetPick(int alX);
	iEdScnObject* GetClosestPick();

	const tPickVec& GetPicks() { return mvPicks; }

	void Update();
protected:
	virtual void Iterate();
	void IterateRenderableNode(iRenderableContainerNode *apNode, cBoundingVolume* apBV);

	virtual void OnDraw(cRendererCallbackFunctions*){}

	iEdWorld* mpWorld;
	iEdViewport* mpViewport;

	iPickFilter* mpPickFilter;
	iPickMethod* mpPickMethod;

	tPickVec mvPicks;
	bool mbNeedsUpdate;
};

//---------------------------------------------------------------------------------

#endif // ED_OBJECT_PICKER_H
