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

#ifndef HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_COMPOUND_H
#define HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_COMPOUND_H

#include "../common/StdAfx.h"
using namespace hpl;

#include "EditorWindowEntityEditBox.h"

//----------------------------------------------------------------------

class iEntityWrapper;

class cEditorEditModeSelect;

class cEntityWrapperCompoundObject;

//----------------------------------------------------------------------

class cEditorWindowEntityEditBoxCompound : public cEditorWindowEntityEditBox
{
public:
	cEditorWindowEntityEditBoxCompound(cEditorEditModeSelect* apEditMode,cEntityWrapperCompoundObject* apObject);
	virtual ~cEditorWindowEntityEditBoxCompound();

	void Create();

protected:
	void OnUpdate(float afTimeStep); 

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool WindowSpecificInputCallback(iEditorInput* apInput);


	//////////////////////////////////
	// Data
	cWidgetTab* mpTabGeneral;
	cWidgetTab* mpTabCompound;
	
	cEntityWrapperCompoundObject* mpEntity;
};

//----------------------------------------------------------------------


#endif // HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_COMPOUND_H
