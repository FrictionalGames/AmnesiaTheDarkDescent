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

#ifndef HPLEDITOR_EDITOR_ACTION_MISC_H
#define HPLEDITOR_EDITOR_ACTION_MISC_H

#include "EditorAction.h"

////////////////////////////////////////////////////////////
// GRID RELATED
////////////////////////////////////////////////////////////

//--------------------------------------------------------------

class iEditorAxisAlignedPlane;
class cEditorGrid;
class cEditorClipPlane;

//--------------------------------------------------------------

class iEditorActionAxisAlignedPlane : public iEditorAction
{
public:
	iEditorActionAxisAlignedPlane( iEditorAxisAlignedPlane* apPlane );

protected:
	iEditorAxisAlignedPlane* mpPlane;
};

class cEditorActionPlaneChangeNormal : public iEditorActionAxisAlignedPlane
{
public: 
	cEditorActionPlaneChangeNormal( iEditorAxisAlignedPlane* apPlane,
									int alNormal);

	void Do();
	void Undo();
protected:
	int mlNewNormal;
	int mlOldNormal;
};

class cEditorActionPlaneChangeHeight : public iEditorActionAxisAlignedPlane
{
public:
	cEditorActionPlaneChangeHeight(iEditorAxisAlignedPlane* apPlane,
								   const cVector3f& avX);

	void Do();
	void Undo();
protected:
	cVector3f mvNewHeights;
	cVector3f mvOldHeights;
};



class cEditorActionGridChangeSnapSeparation : public iEditorAction
{
public:
	cEditorActionGridChangeSnapSeparation(cEditorGrid* apGrid,
										  float afNewSnapSeparation);

	void Do();
	void Undo();

protected:
	//////////////////////////////
	// Data
	cEditorGrid* mpGrid;

	float mfNewSnapSeparation;
	float mfOldSnapSeparation;
};

//--------------------------------------------------------------

class cEditorActionClipPlaneAdd : public iEditorAction
{
public:
	cEditorActionClipPlaneAdd(iEditorWorld* apWorld);

	void Do();
	void Undo();

protected:
	iEditorWorld* mpWorld;
	cEditorClipPlane* mpPlane;
};

class cEditorActionClipPlaneRemove : public iEditorAction
{
public:
	cEditorActionClipPlaneRemove(iEditorWorld* apWorld, int alIdx);
	~cEditorActionClipPlaneRemove();

	void Do();
	void Undo();

protected:
	iEditorWorld* mpWorld;
	int mlPlaneIdx;
	cEditorClipPlane* mpPlane;
	tEditorClipPlaneVec mvOldClipPlanes;
};

//--------------------------------------------------------------

class cEditorActionClipPlaneSetHeight : public cEditorActionPlaneChangeHeight
{
public:
	cEditorActionClipPlaneSetHeight(iEditorWorld* apWorld, int alIdx, const cVector3f& avX);

	void Do();
	void Undo();

protected:
	int mlPlaneIdx;
	iEditorWorld* mpWorld;
};

//--------------------------------------------------------------

class cEditorActionClipPlaneSetNormal : public cEditorActionPlaneChangeNormal
{
public:
	cEditorActionClipPlaneSetNormal(iEditorWorld* apWorld, int alIdx, int aNormal);

	void Do();
	void Undo();

protected:
	int mlPlaneIdx;
	iEditorWorld* mpWorld;
};

//--------------------------------------------------------------

class cEditorActionClipPlaneSetCullingOnPositiveSide : public iEditorAction
{
public:
	cEditorActionClipPlaneSetCullingOnPositiveSide(iEditorWorld* apWorld, int alIdx,
													bool abX);

	void Do();
	void Undo();
protected:
	int mlPlaneIdx;
	iEditorWorld* mpWorld;

	bool mbNewCull;
	bool mbOldCull;
};

//--------------------------------------------------------------

class cEditorActionClipPlaneSetActive : public iEditorAction
{
public:
	cEditorActionClipPlaneSetActive(iEditorWorld* apWorld, int alIdx,
											bool abActive);

	void Do();
	void Undo();

protected:
	iEditorWorld* mpWorld;
	int mlPlaneIdx;

	bool mbNewActive;
	bool mbOldActive;

};

//--------------------------------------------------------------

#endif // HPLEDITOR_EDITOR_ACTION_MISC_H
