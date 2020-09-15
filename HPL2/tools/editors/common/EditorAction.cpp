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

#include "EditorAction.h"

#include "EditorWorld.h"

#include "EditorSelection.h"

//---------------------------------------------------------------------
//---------------------------------------------------------------------
/////////////////////////////////////////////////////////////////
// EDITOR ACTION WORLD MODIFIER
/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------
//---------------------------------------------------------------------

//---------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------


iEditorActionWorldModifier::iEditorActionWorldModifier(const tString& asName, iEditorWorld *apEditorWorld) : iEditorAction(asName)
{
	mpEditorWorld = apEditorWorld;
}

//---------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

void iEditorActionWorldModifier::Do()
{
	mpEditorWorld->IncModifications();
	DoModify();
}

//---------------------------------------------------------------------

void iEditorActionWorldModifier::Undo()
{
	mpEditorWorld->DecModifications();
	UndoModify();
}

//---------------------------------------------------------------------

//---------------------------------------------------------------------
//---------------------------------------------------------------------
/////////////////////////////////////////////////////////////////
// EDITOR ACTION COMPOUND ACTION
/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------
//---------------------------------------------------------------------

cEditorActionCompoundAction::cEditorActionCompoundAction(const tString& asName) : iEditorAction(asName)
{
	mlCurrentSubActionStep = 0;
}

//---------------------------------------------------------------------

cEditorActionCompoundAction::~cEditorActionCompoundAction()
{
    STLDeleteAll(mvSubActions);
}

//---------------------------------------------------------------------

bool cEditorActionCompoundAction::Create()
{
	for(int i=0;i<(int)mvSubActions.size();++i)
	{
		iEditorAction* pAction = mvSubActions[i];
		if(pAction->Create()==false)
			return false;
	}

	return true;
}

//---------------------------------------------------------------------

void cEditorActionCompoundAction::Do()
{
	if(mlCurrentSubActionStep<0)
		mlCurrentSubActionStep=0;

	while(mlCurrentSubActionStep<(int)mvSubActions.size())
		StepForward();
}

//---------------------------------------------------------------------

void cEditorActionCompoundAction::Undo()
{
	int lSize = (int)mvSubActions.size();
	if(mlCurrentSubActionStep>=lSize)
		mlCurrentSubActionStep=lSize-1;

	while(mlCurrentSubActionStep>=0)
		StepBack();
}

//---------------------------------------------------------------------

void cEditorActionCompoundAction::AddAction(iEditorAction* apAction)
{
	if(apAction==NULL)
		return;

	if(apAction->IsValidAction()==false)
	{
		hplDelete(apAction);
		return;
	}

	if(mlCurrentSubActionStep==-1)
		mlCurrentSubActionStep=0;
	mvSubActions.push_back(apAction);
}

//---------------------------------------------------------------------

void cEditorActionCompoundAction::StepForward()
{
	if(mlCurrentSubActionStep>=(int)mvSubActions.size())
		return;

	iEditorAction* pAction = mvSubActions[mlCurrentSubActionStep];
	pAction->Do();

	++mlCurrentSubActionStep;
}

//---------------------------------------------------------------------

void cEditorActionCompoundAction::StepBack()
{
	if(mlCurrentSubActionStep<0)
		return;

	iEditorAction* pAction = mvSubActions[mlCurrentSubActionStep];
	pAction->Undo();

	--mlCurrentSubActionStep;
}

//---------------------------------------------------------------------

bool cEditorActionCompoundAction::IsEmpty()
{
	return mvSubActions.empty();
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

cEditorActionUserDefinedObjectSetVariable::cEditorActionUserDefinedObjectSetVariable(iEditorWorld* apWorld, int alID,
																					 const tWString& asName, const tWString& asValue) : iEditorActionWorldModifier("Set Object Var", apWorld)
{
	mlID = alID;
	msVarName = asName;
	msNewValue = asValue;

	iEntityWrapperUserDefinedEntity* pEnt = (iEntityWrapperUserDefinedEntity*)mpEditorWorld->GetEntity(mlID);
	cEditorVarInstance* pVar = pEnt->GetVar(msVarName);
	if(pVar)
		msOldValue = pVar->GetValue();
}

//---------------------------------------------------------------------

void cEditorActionUserDefinedObjectSetVariable::DoModify()
{
	Apply(msNewValue);
}

//---------------------------------------------------------------------

void cEditorActionUserDefinedObjectSetVariable::UndoModify()
{
	Apply(msOldValue);
}

//---------------------------------------------------------------------

void cEditorActionUserDefinedObjectSetVariable::Apply(const tWString& asX)
{
	iEntityWrapperUserDefinedEntity* pEnt = (iEntityWrapperUserDefinedEntity*)mpEditorWorld->GetEntity(mlID);
	pEnt->SetVar(msVarName, asX);
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

iEditorActionAggregateAddComponents::iEditorActionAggregateAddComponents(iEditorWorld* apEditorWorld, int alID, const tIntList& alstComponentIDs) : iEditorActionWorldModifier("Add entities to Compound Object", apEditorWorld)
{
	mlID = alID;
	mvComponentIDs.insert(mvComponentIDs.end(), alstComponentIDs.begin(), alstComponentIDs.end());
}

//---------------------------------------------------------------------

iEditorActionAggregateAddComponents::~iEditorActionAggregateAddComponents()
{
	STLDeleteAll(mvOldAggregateData);
}

//---------------------------------------------------------------------

bool iEditorActionAggregateAddComponents::Create()
{
	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		int lID = mvComponentIDs[i];
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(lID);
		iEntityWrapperData* pData = NULL;
		iEntityWrapper* pAggregate = GetAggregateFromEntity(pEnt);
		if(pAggregate)
			pData = pAggregate->CreateCopyData();

		mvOldAggregateData.push_back(pData);
	}

	return true;
}

//---------------------------------------------------------------------

void iEditorActionAggregateAddComponents::DoModify()
{
	iEntityWrapperAggregate* pAggregate = (iEntityWrapperAggregate*)mpEditorWorld->GetEntity(mlID);
	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mvComponentIDs[i]);
		iEntityWrapperAggregate* pOldAggregate = GetAggregateFromEntity(pEnt);//;->GetCompoundObject();
		pAggregate->AddComponent(pEnt);

		if(pOldAggregate && pOldAggregate->GetComponents().empty())
			mpEditorWorld->DestroyEntityWrapper(pOldAggregate);
	}
	pAggregate->UpdateEntity();
}

//---------------------------------------------------------------------

void iEditorActionAggregateAddComponents::UndoModify()
{
	iEntityWrapperAggregate* pAggregate = (iEntityWrapperAggregate*)mpEditorWorld->GetEntity(mlID);
	tEntityWrapperList lstAggregates;
	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mvComponentIDs[i]);
		iEntityWrapperAggregate* pOldAggregate = NULL;
		iEntityWrapperData* pOldAggregateData = mvOldAggregateData[i];
		if(pOldAggregateData)
			pOldAggregate = (iEntityWrapperAggregate*)mpEditorWorld->GetEntity(pOldAggregateData->GetID());

		pAggregate->RemoveComponent(pEnt);

		if(pOldAggregate==NULL)
			pOldAggregate = (iEntityWrapperAggregate*)mpEditorWorld->CreateEntityWrapperFromData(pOldAggregateData);

		if(pOldAggregate)
		{
			pOldAggregate->AddComponent(pEnt);
			lstAggregates.push_back(pOldAggregate);
		}
	}

	tEntityWrapperListIt itAggregate = lstAggregates.begin();
	for(;itAggregate!=lstAggregates.end();++itAggregate)
	{
		iEntityWrapper* pAggregate = *itAggregate;
		pAggregate->OnPostDeployAll(false);
		pAggregate->UpdateEntity();
	}

	if(pAggregate->GetComponents().empty())
		mpEditorWorld->DestroyEntityWrapper(pAggregate);
	else
		pAggregate->UpdateEntity();
}

//---------------------------------------------------------------------

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------



iEditorActionAggregateRemoveComponents::iEditorActionAggregateRemoveComponents(iEditorWorld* apEditorWorld, 
																					 const tIntList& alstComponentIDs) : iEditorActionWorldModifier("Detach entities from compound object", apEditorWorld)
{
	mvComponentIDs.insert(mvComponentIDs.begin(),alstComponentIDs.begin(),alstComponentIDs.end());
}

//---------------------------------------------------------------------

iEditorActionAggregateRemoveComponents::~iEditorActionAggregateRemoveComponents()
{
	STLDeleteAll(mvOldAggregateData);
}

//---------------------------------------------------------------------

bool iEditorActionAggregateRemoveComponents::Create()
{
	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mvComponentIDs[i]);

		iEntityWrapperAggregate* pAggregate = GetAggregateFromEntity(pEnt);
		iEntityWrapperData* pData = pAggregate?pAggregate->CreateCopyData():NULL;

		mvOldAggregateData.push_back(pData);
	}
	
	return true;
}
//---------------------------------------------------------------------

void iEditorActionAggregateRemoveComponents::DoModify()
{
	cEditorSelection* pSelection = mpEditorWorld->GetEditor()->GetSelection();

	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mvComponentIDs[i]);
		iEntityWrapperAggregate* pAggregate = GetAggregateFromEntity(pEnt);
		if(pAggregate)
		{
			pSelection->RemoveEntity(pAggregate);

			pAggregate->RemoveComponent(pEnt);
			if(pAggregate->GetComponents().empty())
				mpEditorWorld->DestroyEntityWrapper(pAggregate);

			pSelection->AddEntity(pEnt);
		}
	}
}

//---------------------------------------------------------------------

void iEditorActionAggregateRemoveComponents::UndoModify()
{
	cEditorSelection* pSelection = mpEditorWorld->GetEditor()->GetSelection();
	pSelection->ClearEntities();

	for(int i=0;i<(int)mvComponentIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mvComponentIDs[i]);
		iEntityWrapperAggregate* pAggregate = (iEntityWrapperAggregate*)mpEditorWorld->GetEntity(mvOldAggregateData[i]->GetID());
		if(pAggregate==NULL)
			pAggregate = (iEntityWrapperAggregate*)mpEditorWorld->CreateEntityWrapperFromData(mvOldAggregateData[i]);

		pAggregate->AddComponent(pEnt);

		pSelection->AddEntity(pAggregate);
	}
}

//---------------------------------------------------------------------
