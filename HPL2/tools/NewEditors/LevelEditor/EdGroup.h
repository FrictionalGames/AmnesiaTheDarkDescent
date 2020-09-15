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

#ifndef ED_GROUP_H
#define ED_GROUP_H

#include "../Common/StdAfx.h"
#include "../Common/EdScnObject.h"

class cLevelEditor;
class cHplMap;

class cEdGroup;

//--------------------------------------------------------------------------

typedef std::list<cEdGroup*> tGroupList;
typedef tGroupList::iterator tGroupListIt;

typedef std::map<int, cEdGroup*> tGroupMap;
typedef tGroupMap::iterator tGroupMapIt;

//--------------------------------------------------------------------------

////////////////////////////////////
// Level Editor Group
//  Used to group objects in the level editor, much like a layer. Objects keep a pointer to a group they belong.
//
class cEdGroup
{
public:
	cEdGroup(cHplMap* ,unsigned int alID, const tString& asName);
	~cEdGroup();

	cHplMap* GetWorld() { return mpWorld; }

	/**
	 * Iterates all entities to find those of correct ID and set visibility
	 */
	void SetVisible(bool abX);
	bool IsVisible() { return mbVisible; }

	void SetName(const tString& asName) { msName = asName; }
	const tString& GetName() { return msName; }

	unsigned int GetID() { return mlID; }

	void AddObject(iEdScnObject*);
	void RemoveObject(iEdScnObject*);
	const tScnObjList& GetObjects() { return mlstObjects; }

	iEdAction* CreateSetNameAction(const tString&);
	iEdAction* CreateSetVisibleAction(bool);

	iEdAction* CreateAddObjectsAction(const tScnObjList&);
	iEdAction* CreateRemoveObjectsAction(const tScnObjList&);

	iEdAction* CreateAddObjectsToSelectionAction();
	iEdAction* CreateRemoveObjectsFromSelectionAction();

protected:
	static cHplMap* mpWorld;
	unsigned int mlID;
	tString msName;
	bool mbVisible;

	tScnObjList mlstObjects;
};


#endif // ED_GROUP_h

