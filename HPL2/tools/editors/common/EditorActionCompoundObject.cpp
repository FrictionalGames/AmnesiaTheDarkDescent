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

#include "EditorActionCompoundObject.h"

#include "EditorWorld.h"
#include "EditorSelection.h"

#include "EntityWrapperCompoundObject.h"


static iEntityWrapperAggregate* GetCompoundObject(iEntityWrapper* apEntity)
{
	return apEntity->GetCompoundObject();
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

cEditorActionCompoundObjectAddEntities::cEditorActionCompoundObjectAddEntities(iEditorWorld* apEditorWorld, 
																			   int alID, const tIntList& alstEntityIDs) : iEditorActionAggregateAddComponents(apEditorWorld, alID, alstEntityIDs)
{
/*	mlID = alID;

	tIntList& lstEntityIDs = (tIntList&)alstEntityIDs;
	tIntListIt it = lstEntityIDs.begin();
	for(;it!=lstEntityIDs.end();++it)
	{
		int lID = *it;
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(lID);
		iEntityWrapperData* pData = NULL;
		iEntityWrapper* pCompound = pEnt->GetCompoundObject();
		if(pCompound)
			pData = pCompound->CreateCopyData();

		mvEntityIDs.push_back(lID);
		mvOldCompoundData.push_back(pData);
	}*/
}

//---------------------------------------------------------------------

iEntityWrapperAggregate* cEditorActionCompoundObjectAddEntities::GetAggregateFromEntity(iEntityWrapper* apEntity)
{
	return GetCompoundObject(apEntity);
}

//---------------------------------------------------------------------

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------



cEditorActionCompoundObjectRemoveEntities::cEditorActionCompoundObjectRemoveEntities(iEditorWorld* apEditorWorld, 
																					 const tIntList& alstEntityIDs) : iEditorActionAggregateRemoveComponents(apEditorWorld, alstEntityIDs)
{
/*	mvEntityIDs.insert(mvEntityIDs.begin(),alstEntityIDs.begin(),alstEntityIDs.end());

	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mvEntityIDs[i]);

		cEntityWrapperCompoundObject* pObj = pEnt->GetCompoundObject();
		iEntityWrapperData* pData = pObj?pObj->CreateCopyData():NULL;

		mvOldCompoundData.push_back(pData);
	}*/
}

//---------------------------------------------------------------------

/*
cEditorActionCompoundObjectRemoveEntities::~cEditorActionCompoundObjectRemoveEntities()
{
	STLDeleteAll(mvOldCompoundData);
}
*/
//---------------------------------------------------------------------

iEntityWrapperAggregate* cEditorActionCompoundObjectRemoveEntities::GetAggregateFromEntity(iEntityWrapper* apEntity)
{
	return GetCompoundObject(apEntity);
}

/*
void cEditorActionCompoundObjectRemoveEntities::DoModify()
{
	cEditorSelection* pSelection = mpEditorWorld->GetEditor()->GetSelection();

	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mvEntityIDs[i]);
		cEntityWrapperCompoundObject* pObj = pEnt->GetCompoundObject();
		if(pObj)
		{
			pSelection->RemoveEntity(pObj);

			pObj->RemoveEntity(pEnt);
			if(pObj->GetEntities().empty())
				mpEditorWorld->DestroyEntityWrapper(pObj);

			pSelection->AddEntity(pEnt);
		}
	}
}

//---------------------------------------------------------------------

void cEditorActionCompoundObjectRemoveEntities::UndoModify()
{
	cEditorSelection* pSelection = mpEditorWorld->GetEditor()->GetSelection();
	pSelection->ClearEntities();

	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mvEntityIDs[i]);
		cEntityWrapperCompoundObject* pObj = (cEntityWrapperCompoundObject*)mpEditorWorld->GetEntity(mvOldCompoundData[i]->GetID());
		if(pObj==NULL)
			pObj = (cEntityWrapperCompoundObject*)mpEditorWorld->CreateEntityWrapperFromData(mvOldCompoundData[i]);

		pObj->AddEntity(pEnt);

		pSelection->AddEntity(pObj);
	}
}
*/
//---------------------------------------------------------------------
