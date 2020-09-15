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

#include "../Common/EdAction.h"

#include "../Common/Editor.h"
#include "../Common/EdWorld.h"

//---------------------------------------------------------------------
//---------------------------------------------------------------------
/////////////////////////////////////////////////////////////////
// WORLD MODIFIER ACTION
/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------
//---------------------------------------------------------------------

//---------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

iEdActionWorldModifier::iEdActionWorldModifier(const tWString& asName, iEdWorld *apWorld) : iEdAction(asName)
{
	mpWorld = apWorld;
}

//---------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

void iEdActionWorldModifier::Do()
{
	mlModificationStamp = mpWorld->Modify();
	DoModify();

	mpWorld->GetEditor()->BroadcastModuleMsg(eEdModuleMsg_WorldModify, NULL);
}

//---------------------------------------------------------------------

void iEdActionWorldModifier::Undo()
{
	if(mpWorld->Unmodify(mlModificationStamp))
	{
		UndoModify();
		mpWorld->GetEditor()->BroadcastModuleMsg(eEdModuleMsg_WorldModify, NULL);
	}
}

//---------------------------------------------------------------------

//---------------------------------------------------------------------
//---------------------------------------------------------------------
/////////////////////////////////////////////////////////////////
// EDITOR ACTION COMPOUND ACTION
/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------
//---------------------------------------------------------------------

cEdActionCompound::cEdActionCompound(const tWString& asName) : iEdAction(asName)
{
	mbValid = true;
	mlCurrentSubActionStep = 0;
}

//---------------------------------------------------------------------

cEdActionCompound::~cEdActionCompound()
{
    STLDeleteAll(mvSubActions);
}

//---------------------------------------------------------------------

bool cEdActionCompound::Init()
{
	return mbValid && mvSubActions.empty()==false;
}

//---------------------------------------------------------------------

/*
bool cEdActionCompound::IsValid()
{
	for(size_t i=0; i<mvSubActions.size(); ++i)
	{
		iEdAction* pAction = mvSubActions[i];
		if(pAction->IsValid()==false)
			return false;
	}

	return true;
}
*/

//---------------------------------------------------------------------

void cEdActionCompound::Do()
{
	if(mlCurrentSubActionStep<0)
		mlCurrentSubActionStep=0;

	while(mlCurrentSubActionStep<(int)mvSubActions.size())
		StepForward();
}

//---------------------------------------------------------------------

void cEdActionCompound::Undo()
{
	int lSize = (int)mvSubActions.size();
	if(mlCurrentSubActionStep>=lSize)
		mlCurrentSubActionStep=lSize-1;

	while(mlCurrentSubActionStep>=0)
		StepBack();
}

//---------------------------------------------------------------------

void cEdActionCompound::AddAction(iEdAction* apAction)
{
	if(apAction==NULL)
		return;

	if(apAction->Init()==false)
	{
		hplDelete(apAction);
		mbValid = false;

		return;
	}

	if(mlCurrentSubActionStep==-1)
		mlCurrentSubActionStep=0;
	mvSubActions.push_back(apAction);
}

//---------------------------------------------------------------------

void cEdActionCompound::StepForward()
{
	if(mlCurrentSubActionStep>=(int)mvSubActions.size())
		return;

	iEdAction* pAction = mvSubActions[mlCurrentSubActionStep];
	pAction->Do();

	++mlCurrentSubActionStep;
}

//---------------------------------------------------------------------

void cEdActionCompound::StepBack()
{
	if(mlCurrentSubActionStep<0)
		return;

	iEdAction* pAction = mvSubActions[mlCurrentSubActionStep];
	pAction->Undo();

	--mlCurrentSubActionStep;
}

//---------------------------------------------------------------------

bool cEdActionCompound::IsEmpty()
{
	return mvSubActions.empty();
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

/*
iEdActionAggregateAddComponents::iEdActionAggregateAddComponents(iEdWorld* apEdWorld, int alID, const tIntList& alstComponentIDs) : iEdActionWorldModifier("Add entities to Compound Object", apEdWorld)
{
	mlID = alID;
	mvComponentIDs.insert(mvComponentIDs.end(), alstComponentIDs.begin(), alstComponentIDs.end());
}

//---------------------------------------------------------------------

iEdActionAggregateAddComponents::~iEdActionAggregateAddComponents()
{
	STLDeleteAll(mvOldAggregateData);
}

//---------------------------------------------------------------------

bool iEdActionAggregateAddComponents::Create()
{
	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		int lID = mvComponentIDs[i];
		iEdObject* pEnt = mpEdWorld->GetObject(lID);
		iEdObjectData* pData = NULL;
		iEdObject* pAggregate = GetAggregateFromEntity(pEnt);
		if(pAggregate)
			pData = pAggregate->CreateCopyData();

		mvOldAggregateData.push_back(pData);
	}

	return true;
}

//---------------------------------------------------------------------

void iEdActionAggregateAddComponents::DoModify()
{
	iEdObjectAggregate* pAggregate = (iEdObjectAggregate*)mpEdWorld->GetObject(mlID);
	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		iEdObject* pEnt = mpEdWorld->GetObject(mvComponentIDs[i]);
		iEdObjectAggregate* pOldAggregate = GetAggregateFromEntity(pEnt);//;->GetCompoundObject();
		pAggregate->AddComponent(pEnt);

		if(pOldAggregate && pOldAggregate->GetComponents().empty())
			mpEdWorld->DestroyEntityWrapper(pOldAggregate);
	}
	pAggregate->UpdateEntity();
}

//---------------------------------------------------------------------

void iEdActionAggregateAddComponents::UndoModify()
{
	iEdObjectAggregate* pAggregate = (iEdObjectAggregate*)mpEdWorld->GetObject(mlID);
	tEntityWrapperList lstAggregates;
	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		iEdObject* pEnt = mpEdWorld->GetObject(mvComponentIDs[i]);
		iEdObjectAggregate* pOldAggregate = NULL;
		iEdObjectData* pOldAggregateData = mvOldAggregateData[i];
		if(pOldAggregateData)
			pOldAggregate = (iEdObjectAggregate*)mpEdWorld->GetObject(pOldAggregateData->GetID());

		pAggregate->RemoveComponent(pEnt);

		if(pOldAggregate==NULL)
			pOldAggregate = (iEdObjectAggregate*)mpEdWorld->CreateEntityWrapperFromData(pOldAggregateData);

		if(pOldAggregate)
		{
			pOldAggregate->AddComponent(pEnt);
			lstAggregates.push_back(pOldAggregate);
		}
	}

	tEntityWrapperListIt itAggregate = lstAggregates.begin();
	for(;itAggregate!=lstAggregates.end();++itAggregate)
	{
		iEdObject* pAggregate = *itAggregate;
		pAggregate->OnPostDeployAll(false);
		pAggregate->UpdateEntity();
	}

	if(pAggregate->GetComponents().empty())
		mpEdWorld->DestroyEntityWrapper(pAggregate);
	else
		pAggregate->UpdateEntity();
}
*/
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------


/*
iEdActionAggregateRemoveComponents::iEdActionAggregateRemoveComponents(iEdWorld* apEdWorld, 
																					 const tIntList& alstComponentIDs) : iEdActionWorldModifier("Detach entities from compound object", apEdWorld)
{
	mvComponentIDs.insert(mvComponentIDs.begin(),alstComponentIDs.begin(),alstComponentIDs.end());
}

//---------------------------------------------------------------------

iEdActionAggregateRemoveComponents::~iEdActionAggregateRemoveComponents()
{
	STLDeleteAll(mvOldAggregateData);
}

//---------------------------------------------------------------------

bool iEdActionAggregateRemoveComponents::Create()
{
	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		iEdObject* pEnt = mpEdWorld->GetObject(mvComponentIDs[i]);

		iEdObjectAggregate* pAggregate = GetAggregateFromEntity(pEnt);
		iEdObjectData* pData = pAggregate?pAggregate->CreateCopyData():NULL;

		mvOldAggregateData.push_back(pData);
	}
	
	return true;
}
//---------------------------------------------------------------------

void iEdActionAggregateRemoveComponents::DoModify()
{
	cEdSelection* pSelection = mpEdWorld->GetEditor()->GetSelection();

	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		iEdObject* pEnt = mpEdWorld->GetObject(mvComponentIDs[i]);
		iEdObjectAggregate* pAggregate = GetAggregateFromEntity(pEnt);
		if(pAggregate)
		{
			pSelection->RemoveEntity(pAggregate);

			pAggregate->RemoveComponent(pEnt);
			if(pAggregate->GetComponents().empty())
				mpEdWorld->DestroyEntityWrapper(pAggregate);

			pSelection->AddEntity(pEnt);
		}
	}
}

//---------------------------------------------------------------------

void iEdActionAggregateRemoveComponents::UndoModify()
{
	cEdSelection* pSelection = mpEdWorld->GetEditor()->GetSelection();
	pSelection->ClearEntities();

	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		iEdObject* pEnt = mpEdWorld->GetObject(mvComponentIDs[i]);
		iEdObjectAggregate* pAggregate = (iEdObjectAggregate*)mpEdWorld->GetObject(mvOldAggregateData[i]->GetID());
		if(pAggregate==NULL)
			pAggregate = (iEdObjectAggregate*)mpEdWorld->CreateEntityWrapperFromData(mvOldAggregateData[i]);

		pAggregate->AddComponent(pEnt);

		pSelection->AddEntity(pAggregate);
	}
}
*/

//---------------------------------------------------------------------
