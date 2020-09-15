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

#ifndef ED_OBJ_COMPOUND_H
#define ED_OBJ_COMPOUND_H

#include "../Common/EdScnObject.h"

//--------------------------------------------------------------------------

class cTypeCompound : public iBaseAggregateType
{
public:
	cTypeCompound();

	bool AllowNullEngineObject() { return true; }

protected:
	iEdObjectData* CreateTypeSpecificData();
};

//--------------------------------------------------------------------------

class cEdObjCompoundData : public iBaseAggregateObjData
{
public:
	cEdObjCompoundData(iEdObjectType*);
	~cEdObjCompoundData();

protected:
	iEdObject* CreateTypeSpecificObject();
};

//--------------------------------------------------------------------------

class cEdObjCompound : public iBaseAggregateObject
{
public:
	cEdObjCompound(iEdObjectData*);
	~cEdObjCompound();

	void OnAddComponent(iEdScnObject*);
	void OnRemoveComponent(iEdScnObject*);

	void OnWorldPostTransDraw(const cModuleDrawData&);
	iEdEditPane* CreateEditPane();

	iEdAction* CreateAddComponentsAction(const tScnObjList&);
	iEdAction* CreateRemoveComponentsAction(const tScnObjList&);
protected:
	bool OnCreate() { return true; }

	void UpdateComponents();
};

//--------------------------------------------------------------------------

#endif // ED_OBJ_COMPOUND_H
