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

#ifndef ED_CLIP_PLANE_ACTIONS_H
#define ED_CLIP_PLANE_ACTIONS_H

#include "../Common/EdActionMisc.h"
#include "../Common/EdClipPlane.h"

//--------------------------------------------------------------

class cEdClipPlane;
class iEdScnWorld;

//--------------------------------------------------------------

class cEdActionClipPlaneAdd : public iEdAction
{
public:
	cEdActionClipPlaneAdd(iEdScnWorld*);

	bool Init();

	void Do();
	void Undo();

protected:
	iEdScnWorld* mpMap;
	int mlID;
};

//--------------------------------------------------------------

class cEdActionClipPlaneRemove : public iEdAction
{
public:
	cEdActionClipPlaneRemove(iEdScnWorld*, int);
	~cEdActionClipPlaneRemove();

	bool Init();

	void Do();
	void Undo();

protected:
	iEdScnWorld* mpMap;
	int mlID;
	cVector3f mvHeights;
	int mlNormal;
	bool mbCullSide;
};

//--------------------------------------------------------------

class cEdActionClipPlaneSetHeight : public cEdActionPlaneChangeHeight
{
public:
	cEdActionClipPlaneSetHeight(cEdClipPlane*, const cVector3f&);

	bool Init() { return true; }

	void Do();
	void Undo();

protected:
	iEdScnWorld* mpMap;
	int mlID;
};

//--------------------------------------------------------------

class cEdActionClipPlaneSetNormal : public cEdActionPlaneChangeNormal
{
public:
	cEdActionClipPlaneSetNormal(cEdClipPlane*, int aNormal);

	bool Init() { return true; }

	void Do();
	void Undo();

protected:
	iEdScnWorld* mpMap;
	int mlID;
};

//--------------------------------------------------------------

class cEdActionClipPlaneSetCullingOnPositiveSide : public iEdAction
{
public:
	cEdActionClipPlaneSetCullingOnPositiveSide(cEdClipPlane* apPlane,
													bool abX);

	bool Init() { return true; }

	void Do();
	void Undo();
protected:
	iEdScnWorld* mpMap;
	int mlID;

	bool mbNewCull;
	bool mbOldCull;
};

//--------------------------------------------------------------

class cEdActionClipPlaneSetActive : public iEdAction
{
public:
	cEdActionClipPlaneSetActive(cEdClipPlane* apPlane,
											bool abActive);

	bool Init() { return true; }

	void Do();
	void Undo();

protected:
	iEdScnWorld* mpMap;
	int mlID;

	bool mbNewActive;
	bool mbOldActive;
};

//--------------------------------------------------------------

#endif // ED_ACTION_MISC_H
