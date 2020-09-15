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

#ifndef ED_SELECT_ACTIONS_H
#define ED_SELECT_ACTIONS_H

#include "../Common/EdAction.h"
#include "../Common/EdScnObject.h"

class cEdSelection;

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// SELECT OBJECTS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

class cEdActionSelectObjects : public iEdAction
{
public:
	cEdActionSelectObjects(cEdSelection*, const tScnObjList&);

	bool Init() { return true; }
	bool IsValid() { return mbValid; }

	void Do();
	void Undo();

	cEdSelection* mpSelection;
	bool mbValid;
	tIntList mlstObjectsToSelectIDs;
	tIntList mlstOldSelectedIDs;
};

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// DESELECT OBJECTS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

class cEdActionDeselectObjects : public iEdAction
{
public:
	cEdActionDeselectObjects(cEdSelection*, const tScnObjList&);

	bool Init()		{ return true; }
	bool IsValid()	{ return mbValid; }

	void Do();
	void Undo();

	cEdSelection* mpSelection;
	bool mbValid;
	tIntList mlstObjectsToDeselectIDs;
	tIntList mlstOldSelectedIDs;
};

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// TOGGLE OBJECT SELECTION
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

class cEdActionToggleObjSelection : public iEdAction
{
public:
	cEdActionToggleObjSelection(cEdSelection*, const tScnObjList&);

	bool Init() { return true; }
	bool IsValid() { return true; }

	void Do();
	void Undo();

	cEdSelection* mpSelection;
	tIntList mlstObjectsToToggleIDs;
};

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// SELECTION TRANSFORM
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

class iEdActionSelectionTransform : public iEdActionWorldModifier
{
public:
	iEdActionSelectionTransform(cEdSelection*, bool, const cVector3f&);

	bool Init();

protected:
	void DoModify();
	void UndoModify();

	virtual void SetTransform(const cVector3f&)=0;
	virtual const cVector3f& GetTransform()=0;

	cEdSelection* mpSelection;
	bool mbGlobal;

	cVector3f mvNewTransform;
	cVector3f mvOldTransform;
};

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// SELECTION TRANSLATE
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

class cEdActionTranslateSelection : public iEdActionSelectionTransform
{
public:
	cEdActionTranslateSelection(cEdSelection*, bool, const cVector3f&);

protected:
	void SetTransform(const cVector3f&);
	const cVector3f& GetTransform();
};

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// SELECTION ROTATE
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

class cEdActionRotateSelection : public iEdActionSelectionTransform
{
public:
	cEdActionRotateSelection(cEdSelection*, bool, const cVector3f&);

protected:
	void SetTransform(const cVector3f&);
	const cVector3f& GetTransform();
};

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// SELECTION SCALE
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

class cEdActionScaleSelection : public iEdActionSelectionTransform
{
public:
	cEdActionScaleSelection(cEdSelection*, bool, const cVector3f&);

protected:
	void SetTransform(const cVector3f&);
	const cVector3f& GetTransform();
};

//----------------------------------------------------------------------

#endif // ED_SELECT_ACTIONS_H
