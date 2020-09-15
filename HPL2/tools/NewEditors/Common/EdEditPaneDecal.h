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

#ifndef ED_EDIT_PANE_DECAL_H
#define ED_EDIT_PANE_DECAL_H

#include "../Common/StdAfx.h"
using namespace hpl;

#include "../Common/EdScnObjEditPane.h"

//----------------------------------------------------------------------

class cEdEditPaneDecal : public iEdScnObjEditPane
{
public:
	cEdEditPaneDecal(iEdScnObject*);
	virtual ~cEdEditPaneDecal();

	void Create();
protected:
	void AddDecalProps(cWidgetTab*);
	void OnUpdate();

	bool InputCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool WindowSpecificInputCallback(iEdInput*);

	cWidgetTab* mpTabGeneral;
	cWidgetTab* mpTabDecal;

	tWidgetList mlstBSurfaceTypes;

	cEdInputFile* mpInpMaterial;
	cEdInputColor* mpInpColor;

	cEdInputEnum* mpInpSubDivisions;
	cEdInputEnum* mpInpCurrentSubDiv;

	cEdInputNumber* mpInpOffset;
	cEdInputNumber* mpInpMaxTris;

	bool mbClearingSubDivValues;
};

//----------------------------------------------------------------------


#endif // ED_EDIT_PANE_DECAL_H
