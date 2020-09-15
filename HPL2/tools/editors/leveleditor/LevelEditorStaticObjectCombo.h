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
#include "../common/EditorTypes.h"

using namespace hpl;

//------------------------------------------------------

class cLevelEditorWorld;
class iEntityWrapper;
class iEditorAction;
class cEditorWindowViewport;

//------------------------------------------------------

class cLevelEditorStaticObjectCombo
{
public:
	cLevelEditorStaticObjectCombo(cLevelEditorWorld* apWorld, int alComboID);
	~cLevelEditorStaticObjectCombo();

	int GetID() { return mlComboID; }

	bool AddObject(iEntityWrapper* apObj);
	bool RemoveObject(iEntityWrapper* apObj);
	bool HasObject(iEntityWrapper* apObj);
	const tEntityWrapperList& GetObjects() { return mlstEntities; }

	const cColor& GetColor() { return mColor; }
	void SetColor(const cColor& aCol);

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions);

	bool Load(cXmlElement* apElement);
	bool Save(cXmlElement* apElement);

	iEditorAction* CreateActionAddObject(iEntityWrapper* apObj);
	iEditorAction* CreateActionRemoveObject(iEntityWrapper* apObj);
	iEditorAction* CreateActionSetColor(const cColor& aCol);
    
private:
	bool IsValidObject(iEntityWrapper* apObj);

	cLevelEditorWorld* mpWorld;
	int mlComboID;

	tEntityWrapperList mlstEntities;

	cColor mColor;
	iGpuProgram* mpDrawProg;
};

//------------------------------------------------------

typedef std::list<cLevelEditorStaticObjectCombo*> tStaticObjectComboList;
typedef tStaticObjectComboList::iterator tStaticObjectComboListIt;

//------------------------------------------------------

#endif // LEVEL_EDITOR_STATIC_OBJECT_COMBO_H
