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

#ifndef ED_SCN_OBJ_ACTIONS_H
#define ED_SCN_OBJ_ACTIONS_H

#include "../Common/EdAction.h"
#include "../Common/EdScnObject.h"

class iEdWorld;
class iEdScnObject;
class iUserObject;
class iBaseAggregateObject;

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// ADD COMPONENTS TO AGGREGATE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class iEdActionSetObjMatrix : public iEdActionWorldModifier
{
public:
	iEdActionSetObjMatrix(const tWString&, iEdWorld*, iEdScnObject*, const cMatrixf&);

	bool Init();

protected:
	void DoModify();
	void UndoModify();
	virtual const cMatrixf& GetMatrix(iEdScnObject*)=0;
	virtual void SetMatrix(iEdScnObject*, const cMatrixf&)=0;

	int mlID;
	iEdScnObject* mpObject;
	cMatrixf mmtxOldMatrix;
	cMatrixf mmtxNewMatrix;
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// ADD COMPONENTS TO AGGREGATE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class cEdActionSetTranslationMatrix : public iEdActionSetObjMatrix
{
public:
	cEdActionSetTranslationMatrix(iEdWorld*, iEdScnObject*, const cMatrixf&);

protected:
	const cMatrixf& GetMatrix(iEdScnObject*);
	void SetMatrix(iEdScnObject*, const cMatrixf&);
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// ADD COMPONENTS TO AGGREGATE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class cEdActionSetRotationMatrix : public iEdActionSetObjMatrix
{
public:
	cEdActionSetRotationMatrix(iEdWorld*, iEdScnObject*, const cMatrixf&);

protected:
	const cMatrixf& GetMatrix(iEdScnObject*);
	void SetMatrix(iEdScnObject*, const cMatrixf&);
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// ADD COMPONENTS TO AGGREGATE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class cEdActionSetScaleMatrix : public iEdActionSetObjMatrix
{
public:
	cEdActionSetScaleMatrix(iEdWorld*, iEdScnObject*, const cMatrixf&);

protected:
	const cMatrixf& GetMatrix(iEdScnObject*);
	void SetMatrix(iEdScnObject*, const cMatrixf&);
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// ADD COMPONENTS TO AGGREGATE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class cEdActionSetWorldMatrix : public iEdActionSetObjMatrix
{
public:
	cEdActionSetWorldMatrix(iEdWorld*, iEdScnObject*, const cMatrixf&);

protected:
	const cMatrixf& GetMatrix(iEdScnObject*);
	void SetMatrix(iEdScnObject*, const cMatrixf&);
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// ADD COMPONENTS TO AGGREGATE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class iEdActionAggregateAddComponents : public iEdActionWorldModifier
{
public:
	iEdActionAggregateAddComponents(iBaseAggregateObject*, const tScnObjList&);
	~iEdActionAggregateAddComponents();

	bool Init();

	void DoModify();
	void UndoModify();

protected:
	virtual iBaseAggregateObject* GetAggregateFromObject(iEdScnObject*)=0;

	int mlID;
	tIntVec mvComponentIDs;
	tEdObjDataVec mvOldAggregateData;
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// REMOVE COMPONENTS FROM AGGREGATE
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class iEdActionAggregateRemoveComponents : public iEdActionWorldModifier
{
public:
	iEdActionAggregateRemoveComponents(const tScnObjList&);
	~iEdActionAggregateRemoveComponents();

	bool Init();

	void DoModify();
	void UndoModify();

protected:
	virtual iBaseAggregateObject* GetAggregateFromObject(iEdScnObject*)=0;
	tEdObjDataVec mvOldAggregateData;

	tIntVec mvComponentIDs;
};

//--------------------------------------------------------

class cEdActionCompoundAddComponents : public iEdActionAggregateAddComponents
{
public:
	cEdActionCompoundAddComponents(iBaseAggregateObject*, const tScnObjList&);

protected:
	iBaseAggregateObject* GetAggregateFromObject(iEdScnObject*);
};

//--------------------------------------------------------

class cEdActionCompoundRemoveComponents : public iEdActionAggregateRemoveComponents
{
public:
	cEdActionCompoundRemoveComponents(const tScnObjList&);

protected:
	iBaseAggregateObject* GetAggregateFromObject(iEdScnObject*);
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// SET OBJECT CHILDREN
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class cEdActionScnObjSetChildren : public iEdActionWorldModifier
{
public:
	cEdActionScnObjSetChildren(iEdScnObject*, const tScnObjList&);

	bool Init();

protected:
	void DoModify();
	void UndoModify();
	void Apply(const tScnObjList&);


	int mlID;
	tScnObjList mlstNewChildren;
	tScnObjList mlstOldChildren;
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// USER DEFINED VARIABLE SET
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class cEdActionSetObjectVar : public iEdActionWorldModifier
{
public:
	cEdActionSetObjectVar(iUserObject*, const tWString&, const tWString&);

	bool Init();

	void DoModify();
	void UndoModify();

protected:
	void Apply(const tWString&);

	int mlID;
	tWString msVarName;

	tWString msOldValue;
	tWString msNewValue;
};

//--------------------------------------------------------

/////////////////////////////////////////////////////////
// USER DEFINED OBJECT TYPE SET
/////////////////////////////////////////////////////////

//--------------------------------------------------------

class cEdActionSetObjectUserType : public iEdActionWorldModifier
{
public:
	cEdActionSetObjectUserType(iUserObject*, iUserTypeNode*);

	bool Init();

	void DoModify();
	void UndoModify();

protected:
	void Apply(iUserTypeNode*);

	int mlID;
	iUserTypeNode* mpOldType;
	iUserTypeNode* mpNewType;
};

//--------------------------------------------------------


#endif // ED_SCN_OBJ_ACTIONS_H
