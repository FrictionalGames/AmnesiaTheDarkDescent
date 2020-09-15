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

#ifndef ED_MULTI_SCN_OBJ_EDIT_PANE_H
#define ED_MULTI_SCN_OBJ_EDIT_PANE_H

//------------------------------------------------------------------------------

#include "../Common/StdAfx.h"
using namespace hpl;

#include "../Common/EdScnObjEditPane.h"
#include "../Common/EdScnObject.h"

#include "../Common/EdEditModeSelect.h"

#include "../Common/EdVar.h"

//------------------------------------------------------------------------------

class cEdMultiScnObjEditPane : public iEdScnObjEditPane
{
public:
	cEdMultiScnObjEditPane(iEdObject*);
	virtual ~cEdMultiScnObjEditPane();

	virtual void Create();

protected:
	virtual void OnUpdate();

	bool InputCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool CreateCompound_OnPressed(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(CreateCompound_OnPressed);

	bool WindowSpecificInputCallback(iEdInput*);

	void AddRelTranslationInput(cWidgetTab*);
	void AddRelScaleInput(cWidgetTab*);
	void AddRelRotationInput(cWidgetTab*);

	///////////////////////////////////////
	// Data
	cWidgetListBox* mpLBNames;

	cWidgetButton* mpBCreateCompound;

	cEdInputBool* mpInpGlobalTransform;

	cEdInputVec3* mpInpRelTranslation;
	cEdInputVec3* mpInpRelScale;
	cEdInputVec3* mpInpRelRotation;

	cVector3f mvBaseTranslation;
	cVector3f mvBaseScale;
	cVector3f mvBaseRotation;
};

//------------------------------------------------------------------------------

#endif // ED_MULTI_SCN_OBJ_EDIT_PANE_H
