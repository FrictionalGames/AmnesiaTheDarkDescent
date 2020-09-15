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

#ifndef ED_ACTION_H
#define ED_ACTION_H

#include "../common/StdAfx.h"

#include "../common/EditorTypes.h"

using namespace hpl;

//--------------------------------------------------------

#include "../Common/EdWorld.h"

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// ACTION BASE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class iEdAction
{
public:
	iEdAction(const tWString& asName) { msName = asName; }
	virtual ~iEdAction(){}

	virtual bool Init()=0;

	virtual void Do()=0;
	virtual void Undo()=0;

	const tWString& GetName() { return msName; }

protected:
	tWString msName;
};

//--------------------------------------------------------

typedef std::list<iEdAction*> tEdActionList;
typedef tEdActionList::iterator tEdActionListIt;

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// ACTION EDWORLD MODIFIER
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class iEdActionWorldModifier : public iEdAction
{
public:
	iEdActionWorldModifier(const tWString&, iEdWorld*);

	void Do();
	void Undo();
protected:
	virtual void DoModify()=0;
	virtual void UndoModify()=0;
	
	iEdWorld* mpWorld;
	int mlModificationStamp;
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// EDITOR ACTION COMPOUND
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class cEdActionCompound : public iEdAction
{
public:
	cEdActionCompound(const tWString&);
	~cEdActionCompound();

	bool Init();

	void Do();
	void Undo();

	void AddAction(iEdAction*);

	void StepForward();
	void StepBack();

	bool IsEmpty();
protected:
	bool mbValid;

	std::vector<iEdAction*> mvSubActions;
	int mlCurrentSubActionStep;
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// ADD COMPONENTS TO AGGREGATE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

/*
class iEdActionAddComponentsToAggregate : public iEdActionWorldModifier
{
public:
	iEdActionAddComponentsToAggregate(iEdWorld*, int, const tIntList&);
	~iEdActionAddComponentsToAggregate();

	bool Init();

	void DoModify();
	void UndoModify();

protected:
	virtual iEdObjectAggregate* GetAggregateFromEntity(iEdObject* apEntity)=0;

	int mlID;
	tIntVec mvComponentIDs;
	tEntityDataVec mvOldAggregateData;
};
*/
 
//--------------------------------------------------------

/////////////////////////////////////////////////////////
// REMOVE COMPONENTS FROM AGGREGATE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

/*
class iEdActionRemoveComponentsFromAggregate : public iEdActionWorldModifier
{
public:
	iEdActionRemoveComponentsFromAggregate(iEdWorld* apEdWorld,const tIntList& alstComponentIDs);
	~iEdActionRemoveComponentsFromAggregate();

	bool Init();

	void DoModify();
	void UndoModify();

protected:
	virtual iEdObjectAggregate* GetAggregateFromEntity(iEdObj* apEntity)=0;
	tEntityDataVec mvOldAggregateData;

	tIntVec mvComponentIDs;
};
*/
//--------------------------------------------------------

#endif //ED_ACTION_H


