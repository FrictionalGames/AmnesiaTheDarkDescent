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

#ifndef ED_BODY_ACTIONS_H
#define ED_BODY_ACTIONS_H

#include "../Common/StdAfx.h"

using namespace hpl;

#include "../Common/EdScnObjectActions.h"
//#include "EditorTypes.h"
//#include "EditorActionMisc.h"

//-----------------------------------------------------------------------

class iEdWorld;
class cEdObjBody;

//-----------------------------------------------------------------------

class cEdActionBodyAddShapes : public iEdActionAggregateAddComponents
{
public:
	cEdActionBodyAddShapes(cEdObjBody* apBody, const tScnObjList& alstShapes);

protected:
	iBaseAggregateObject* GetAggregateFromObject(iEdScnObject*);
};

//-----------------------------------------------------------------------

class cEdActionBodyRemoveShapes : public iEdActionAggregateRemoveComponents
{
public:
	cEdActionBodyRemoveShapes(const tScnObjList& alstShapes);

protected:
	iBaseAggregateObject* GetAggregateFromObject(iEdScnObject*);
};

//-----------------------------------------------------------------------

#endif // ED_BODY_ACTIONS_H
