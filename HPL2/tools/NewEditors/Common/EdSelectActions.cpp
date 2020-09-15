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

#include "../Common/EdSelectActions.h"
#include "../Common/EdSelection.h"



static void Select(cEdSelection* apSelection, const tIntList& alstIDs)
{
	apSelection->Clear();
	iEdWorld* pWorld = apSelection->GetWorld();

	tIntList::const_iterator it = alstIDs.begin();
	for(;it!=alstIDs.end();++it)
	{
		int lID = *it;
		
		iEdObject* pObj = pWorld->GetObject(lID);
		apSelection->AddComponent(static_cast<iEdScnObject*>(pObj));
	}

	// If something fucks up, turn this back to apSelection->Update(true)
	apSelection->SetUpdated();
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// SELECT OBJECTS - CONSTRUCTORS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEdActionSelectObjects::cEdActionSelectObjects(cEdSelection* apSel, const tScnObjList& alstObjects) : iEdAction(_W("Select objects"))
{
	mpSelection = apSel;
	mbValid = alstObjects.empty();

	tScnObjList::const_iterator it = alstObjects.begin();
	for(;it!=alstObjects.end(); ++it)
	{
		iEdScnObject* pObj = *it;
		if(pObj->IsSelected()==false)
			mbValid = true;
		
		mlstObjectsToSelectIDs.push_back(pObj->GetID());
	}

	const tScnObjList& lstOldSelection = mpSelection->GetComponents();
	tScnObjList::const_iterator itOld = lstOldSelection.begin();
	for(;itOld!=lstOldSelection.end(); ++itOld)
	{
		iEdScnObject* pObj = *itOld;
        
		mlstOldSelectedIDs.push_back(pObj->GetID());
	}
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// SELECT OBJECTS - PUBLIC METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cEdActionSelectObjects::Do()
{
	Select(mpSelection, mlstObjectsToSelectIDs);
}

void cEdActionSelectObjects::Undo()
{
	Select(mpSelection, mlstOldSelectedIDs);
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// DESELECT OBJECTS - CONSTRUCTORS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEdActionDeselectObjects::cEdActionDeselectObjects(cEdSelection* apSel, const tScnObjList& alstObjects) : iEdAction(_W("Select objects"))
{
	mpSelection = apSel;
	mbValid = false;

	tScnObjList::const_iterator it = alstObjects.begin();
	for(;it!=alstObjects.end(); ++it)
	{
		iEdScnObject* pObj = *it;
		if(pObj->IsSelected())
			mbValid = true;
		
		mlstObjectsToDeselectIDs.push_back(pObj->GetID());
	}

	const tScnObjList& lstOldSelection = mpSelection->GetComponents();
	tScnObjList::const_iterator itOld = lstOldSelection.begin();
	for(;itOld!=lstOldSelection.end(); ++itOld)
	{
		iEdScnObject* pObj = *itOld;
        
		mlstOldSelectedIDs.push_back(pObj->GetID());
	}
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// DESELECT OBJECTS - PUBLIC METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cEdActionDeselectObjects::Do()
{
	Select(mpSelection, mlstObjectsToDeselectIDs);
}

void cEdActionDeselectObjects::Undo()
{
	Select(mpSelection, mlstOldSelectedIDs);
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// TOGGLE OBJECT SELECTION - CONSTRUCTORS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEdActionToggleObjSelection::cEdActionToggleObjSelection(cEdSelection* apSelection, const tScnObjList& alstObjects) : iEdAction(_W("Toggle selection"))
{

}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// TOGGLE OBJECT SELECTION - PUBLIC METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cEdActionToggleObjSelection::Do()
{
}

void cEdActionToggleObjSelection::Undo()
{
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// SELECTION TRANSFORM
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

iEdActionSelectionTransform::iEdActionSelectionTransform(cEdSelection* apSelection, bool abGlobal, const cVector3f& avX) : iEdActionWorldModifier(_W("Transform Selection"), apSelection->GetWorld())
{
	mpSelection = apSelection;
	mbGlobal = abGlobal;
	mvNewTransform = avX;
}

bool iEdActionSelectionTransform::Init()
{
	mvOldTransform = GetTransform();
	if(mvOldTransform==mvNewTransform)
		return false;

	return true;
}

void iEdActionSelectionTransform::DoModify()
{
	bool bWasGlobal = mpSelection->GetGlobalTransform();
	mpSelection->SetGlobalTransform(mbGlobal);

	SetTransform(mvNewTransform);
	mpSelection->SetGlobalTransform(bWasGlobal);
}

void iEdActionSelectionTransform::UndoModify()
{
	bool bWasGlobal = mpSelection->GetGlobalTransform();
	mpSelection->SetGlobalTransform(mbGlobal);

	SetTransform(mvOldTransform);
	mpSelection->SetGlobalTransform(bWasGlobal);
}


//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// SELECTION TRANSLATE
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEdActionTranslateSelection::cEdActionTranslateSelection(cEdSelection* apSelection, 
														 bool abGlobal, 
														 const cVector3f& avX) : iEdActionSelectionTransform(apSelection, abGlobal, avX)
{
}

void cEdActionTranslateSelection::SetTransform(const cVector3f& avX)
{
	mpSelection->SetTranslation(avX);
}

const cVector3f& cEdActionTranslateSelection::GetTransform()
{
	return mpSelection->GetTranslation();
}


//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// SELECTION ROTATE - PUBLIC METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEdActionRotateSelection::cEdActionRotateSelection(cEdSelection* apSelection, 
														 bool abGlobal, 
														 const cVector3f& avX) : iEdActionSelectionTransform(apSelection, abGlobal, avX)
{
}

void cEdActionRotateSelection::SetTransform(const cVector3f& avX)
{
	mpSelection->SetRotation(avX);
}

const cVector3f& cEdActionRotateSelection::GetTransform()
{
	return mpSelection->GetRotation();
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// SELECTION SCALE - PUBLIC METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEdActionScaleSelection::cEdActionScaleSelection(cEdSelection* apSelection, 
														 bool abGlobal, 
														 const cVector3f& avX) : iEdActionSelectionTransform(apSelection, abGlobal, avX)
{
}

void cEdActionScaleSelection::SetTransform(const cVector3f& avX)
{
	mpSelection->SetScale(avX);
}

const cVector3f& cEdActionScaleSelection::GetTransform()
{
	return mpSelection->GetScale();
}

