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

#ifndef HPLEDITOR_EDITOR_ACTION
#define HPLEDITOR_EDITOR_ACTION

#include "../common/StdAfx.h"

#include "../common/EditorTypes.h"

using namespace hpl;

//--------------------------------------------------------

#include "EditorWorld.h"
#include "EntityWrapper.h"

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// EDITOR ACTION BASE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class iEditorAction
{
public:
	iEditorAction(const tString& asName) { msName = asName; }
	virtual ~iEditorAction(){}

	virtual bool Create() { return true; }
	virtual bool IsValidAction() { return true; }

	virtual void Do()=0;
	virtual void Undo()=0;

	const tString& GetName() { return msName; }

protected:
	tString msName;
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// EDITOR ACTION LEVEL MODIFIER
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class iEditorActionWorldModifier : public iEditorAction
{
public:
	iEditorActionWorldModifier(const tString& asName, iEditorWorld* apEditorWorld);

	void Do();
	void Undo();

	virtual void DoModify()=0;
	virtual void UndoModify()=0;
protected:
	
	iEditorWorld* mpEditorWorld;
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// EDITOR ACTION OBJECT SET PROPERTY
/////////////////////////////////////////////////////////

//--------------------------------------------------------

template<class T>
class cEditorActionObjectSetProperty : public iEditorActionWorldModifier
{
public:
	cEditorActionObjectSetProperty(iEditorWorld* apEditorWorld, 
								   const tIntList& alstEntityIDs, 
								   iProp* apProp, 
								   const T& aNewValue) : iEditorActionWorldModifier("",apEditorWorld)
	{
		if(apProp==NULL)
		{
			Log("Error creating property set action : NULL property passed\n");
			mbValidProperty = false;
			return;
		}

		mlstEntityIDs = alstEntityIDs;
		mlPropID = apProp->GetID();
		mNewValue = aNewValue;

		mbValidProperty = (mlstEntityIDs.empty()==false);
		if(mbValidProperty)
		{
			bool bModifyingSingleEntity = (alstEntityIDs.size()==1);

			bool bNameSet = false;

			tIntListIt it = mlstEntityIDs.begin();
			for(;it!=mlstEntityIDs.end();++it)
			{
				// Fetch old value and check if the property exists in the object and if the current and next values are different
				iEntityWrapper* pEnt = GetObject(*it);
				iEntityWrapperType* pType = pEnt->GetType();
				int lObjectType = pType->GetID();

				if(bNameSet==false)
					bNameSet = true;

				T oldVal;
				mbValidProperty = mbValidProperty && pEnt->GetProperty(mlPropID, oldVal);
				if(bModifyingSingleEntity && oldVal==mNewValue)
					mbValidProperty = false;

				mlstOldValues.push_back(oldVal);
			}
		}

		msName = "Set " + mpEditorWorld->GetEditor()->GetEngine()->GetEngineTypeString(apProp->GetType()) + 
						" property " + apProp->GetName();
	}

	bool IsValidAction() { return mbValidProperty; }

protected:
	virtual void DoModify()
	{
		Apply(mNewValue);
	}
	virtual void UndoModify()
	{
		Apply(mlstOldValues);
	}

	iEntityWrapper* GetObject(int alID)
	{
		return GetObjectHelper(mpEditorWorld,alID);
	}
	static iEntityWrapper* GetObjectHelper(iEditorWorld* apEditorWorld, int alID)
	{
		return apEditorWorld->GetEntity(alID);
	}

	void Apply(const T& aX)
	{
		tIntListIt itIDs = mlstEntityIDs.begin();
		for(;itIDs!=mlstEntityIDs.end();++itIDs)
		{
			int lID = *itIDs;
			iEntityWrapper* pEnt = GetObject(lID);
			pEnt->SetProperty(mlPropID, aX);
		}

		UpdateEntities();
	}

	void Apply(const std::list<T>& alstX)
	{
		typename std::list<T>::const_iterator itValues = mlstOldValues.begin();
		tIntListIt itIDs = mlstEntityIDs.begin();
		for(;itIDs!=mlstEntityIDs.end();++itIDs, ++itValues)
		{
			const T& oldValue = *itValues;
			int lID = *itIDs;
			iEntityWrapper* pEnt = GetObjectHelper(mpEditorWorld, lID);
			pEnt->SetProperty(mlPropID, oldValue);
		}

		UpdateEntities();
	}

	void UpdateEntities()
	{
		tIntListIt itIDs = mlstEntityIDs.begin();
		for(;itIDs!=mlstEntityIDs.end();++itIDs)
		{
			int lID = *itIDs;
			iEntityWrapper* pEnt = GetObject(lID);
			pEnt->UpdateEntity();
		}
	}
	
	tIntList mlstEntityIDs;
	int mlPropID;
	bool mbValidProperty;

	std::list<T> mlstOldValues;
	T mNewValue;
};

//--------------------------------------------------------

typedef cEditorActionObjectSetProperty<int> cEditorActionSetInt;
typedef cEditorActionObjectSetProperty<float> cEditorActionSetFloat;
typedef cEditorActionObjectSetProperty<bool> cEditorActionSetBool;
typedef cEditorActionObjectSetProperty<tString> cEditorActionSetString;
typedef cEditorActionObjectSetProperty<cVector2f> cEditorActionSetVec2f;
typedef cEditorActionObjectSetProperty<cVector3f> cEditorActionSetVec3f;
typedef cEditorActionObjectSetProperty<cColor> cEditorActionSetColor;

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// EDITOR ACTION COMPOUND
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class cEditorActionCompoundAction : public iEditorAction
{
public:
	cEditorActionCompoundAction(const tString& asName);
	~cEditorActionCompoundAction();

	bool Create();

	void Do();
	void Undo();

	void AddAction(iEditorAction* apAction);

	void StepForward();
	void StepBack();

	bool IsEmpty();
protected:
	std::vector<iEditorAction*> mvSubActions;
	int mlCurrentSubActionStep;
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// USER DEFINED VARIABLE SET
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class cEditorActionUserDefinedObjectSetVariable : public iEditorActionWorldModifier
{
public:
	cEditorActionUserDefinedObjectSetVariable(iEditorWorld* apWorld, int alID, const tWString& asName, const tWString& asValue);

	void DoModify();
	void UndoModify();

protected:
	void Apply(const tWString& asX);

	int mlID;
	tWString msVarName;

	tWString msOldValue;
	tWString msNewValue;
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// ADD COMPONENTS TO AGGREGATE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class iEditorActionAggregateAddComponents : public iEditorActionWorldModifier
{
public:
	iEditorActionAggregateAddComponents(iEditorWorld* apEditorWorld, int alID, const tIntList& alstComponentIDs);
	~iEditorActionAggregateAddComponents();

	bool Create();

	void DoModify();
	void UndoModify();

protected:
	virtual iEntityWrapperAggregate* GetAggregateFromEntity(iEntityWrapper* apEntity)=0;

	int mlID;
	tIntVec mvComponentIDs;
	tEntityDataVec mvOldAggregateData;
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// REMOVE COMPONENTS FROM AGGREGATE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class iEditorActionAggregateRemoveComponents : public iEditorActionWorldModifier
{
public:
	iEditorActionAggregateRemoveComponents(iEditorWorld* apEditorWorld,const tIntList& alstComponentIDs);
	~iEditorActionAggregateRemoveComponents();

	bool Create();

	void DoModify();
	void UndoModify();

protected:
	virtual iEntityWrapperAggregate* GetAggregateFromEntity(iEntityWrapper* apEntity)=0;
	tEntityDataVec mvOldAggregateData;

	tIntVec mvComponentIDs;
};

//--------------------------------------------------------

#endif //HPLEDITOR_EDITOR_ACTION


