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

#include "../LevelEditor/EdGroup.h"

#include "../Common/EdEditModeHandler.h"
#include "../Common/EdEditModeSelect.h"
#include "../Common/EdSelection.h"

#include "../Common/Editor.h"
#include "../LevelEditor/HPLMap.h"
#include "../LevelEditor/HPLMapActions.h"

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

cHplMap* cEdGroup::mpWorld = NULL;

//--------------------------------------------------------------------

cEdGroup::cEdGroup(cHplMap* apWorld, unsigned int alID,const tString& asName)
{
	if(mpWorld==NULL) mpWorld = apWorld;

	mlID = alID;
	msName = asName;
}

cEdGroup::~cEdGroup()
{
	tScnObjList lstObjectsCopy = mlstObjects;
	tScnObjListIt it = lstObjectsCopy.begin();
	for(;it!=lstObjectsCopy.end();++it)
		RemoveObject(*it);
}

//--------------------------------------------------------------------

void cEdGroup::SetVisible(bool abX)
{
	mbVisible = abX;
	tScnObjListIt it = mlstObjects.begin();
	for(;it!=mlstObjects.end();++it)
	{
		iEdScnObject* pObj = *it;
		//pObj->SetUpdated();
	}
}

//--------------------------------------------------------------------

void cEdGroup::AddObject(iEdScnObject* apObj)
{
	if(apObj==NULL) return;

	// Remove from previous
	cMapObjExtension* pExt = static_cast<cMapObjExtension*>(apObj->GetExtension());
	pExt->SetGroup(this, true);

	mlstObjects.remove(apObj);
	mlstObjects.push_back(apObj);
}

void cEdGroup::RemoveObject(iEdScnObject* apObj)
{
	if(apObj==NULL) return;

	cMapObjExtension* pExt = static_cast<cMapObjExtension*>(apObj->GetExtension());
	
	pExt->SetGroup(NULL, false);

	mlstObjects.remove(apObj);
}

//--------------------------------------------------------------------

iEdAction* cEdGroup::CreateSetNameAction(const tString& asX)
{
	return hplNew(cEdActionGroupSetName,(this, asX));
}

//--------------------------------------------------------------------

iEdAction* cEdGroup::CreateSetVisibleAction(bool abX)
{
	return hplNew(cEdActionGroupSetVisible,(this, abX));
}

//--------------------------------------------------------------------

iEdAction* cEdGroup::CreateAddObjectsAction(const tScnObjList& alstObjects)
{
	return NULL;
}

//--------------------------------------------------------------------

iEdAction* cEdGroup::CreateRemoveObjectsAction(const tScnObjList& alstObjects)
{
	return NULL;
}

//--------------------------------------------------------------------

iEdAction* cEdGroup::CreateAddObjectsToSelectionAction()
{
	cEdEditModeSelect* pSelect = static_cast<cEdEditModeSelect*>(mpWorld->GetEditor()->GetEditModeHandler()->GetMode(_W("Select")));
	cEdSelection* pSelection = pSelect->GetSelection();

	tScnObjList lstObjectsToSelect = pSelection->GetComponents();
	tScnObjListIt it = mlstObjects.begin();
	for(;it!=mlstObjects.end();++it)
	{
		iEdScnObject* pObj = *it;
		if(pObj->IsSelected()==false)
			lstObjectsToSelect.push_back(pObj);
	}

	return pSelection->CreateSelectObjectsAction(lstObjectsToSelect);
}

//--------------------------------------------------------------------

iEdAction* cEdGroup::CreateRemoveObjectsFromSelectionAction()
{
	cEdEditModeSelect* pSelect = static_cast<cEdEditModeSelect*>(mpWorld->GetEditor()->GetEditModeHandler()->GetMode(_W("Select")));
	cEdSelection* pSelection = pSelect->GetSelection();

	tScnObjList lstObjectsToSelect = pSelection->GetComponents();
	tScnObjListIt it = mlstObjects.begin();
	for(;it!=mlstObjects.end();++it)
	{
		iEdScnObject* pObj = *it;
		lstObjectsToSelect.remove(pObj);
	}

	return pSelection->CreateSelectObjectsAction(lstObjectsToSelect);
}
