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

#ifndef LEVEL_EDITOR_STATIC_OBJECT_COMBO_H
#define LEVEL_EDITOR_STATIC_OBJECT_COMBO_H

//------------------------------------------------------

#include "../common/StdAfx.h"
#include "../Common/EdScnObject.h"
//#include "../LevelEditor/HPLMap.h"

using namespace hpl;

//------------------------------------------------------

class cHplMap;
class iEdScnObject;
class iEdAction;
class iEdViewport;

//------------------------------------------------------

class cLevelEditorStaticObjectCombo
{
public:
	cLevelEditorStaticObjectCombo(cHplMap* apWorld, int alComboID);
	~cLevelEditorStaticObjectCombo();

	cHplMap* GetWorld() { return mpWorld; }

	int GetID() { return mlComboID; }

	bool AddObject(iEdScnObject* apObj);
	bool RemoveObject(iEdScnObject* apObj);
	bool HasObject(iEdScnObject* apObj);
	const tScnObjList& GetObjects() { return mlstObjects; }

	const cColor& GetColor() { return mColor; }
	void SetColor(const cColor& aCol);

	void OnDraw(const cModuleDrawData&);

	bool Load(cXmlElement* apElement);
	bool Save(cXmlElement* apElement);

	iEdAction* CreateActionAddObject(iEdScnObject* apObj);
	iEdAction* CreateActionRemoveObject(iEdScnObject* apObj);
	iEdAction* CreateActionSetColor(const cColor& aCol);
    
private:
	bool IsValidObject(iEdScnObject* apObj);

	cHplMap* mpWorld;
	int mlComboID;

	tScnObjList mlstObjects;

	cColor mColor;
};

//------------------------------------------------------

typedef std::list<cLevelEditorStaticObjectCombo*> tStaticObjectComboList;
typedef tStaticObjectComboList::iterator tStaticObjectComboListIt;

//------------------------------------------------------

#endif // LEVEL_EDITOR_STATIC_OBJECT_COMBO_H
