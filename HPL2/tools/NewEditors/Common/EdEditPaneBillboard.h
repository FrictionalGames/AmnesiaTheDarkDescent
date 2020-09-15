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

#ifndef ED_EDIT_PANE_BILLBOARD_H
#define ED_EDIT_PANE_BILLBOARD_H

#include "../Common/StdAfx.h"
using namespace hpl;

#include "../Common/EdScnObjEditPane.h"

//----------------------------------------------------------------------

class iEditor;

class cEdEditModeSelect;
class cEntityWrapperBillboard;
class cEdSelectorHighlighter;

//----------------------------------------------------------------------

class cEdEditPaneBillboard : public iEdScnObjEditPane
{
public:
	cEdEditPaneBillboard(iEdScnObject*);
	virtual ~cEdEditPaneBillboard();

	void Create();
protected:
	void AddBillboardProps(cWidgetTab*);

	void OnUpdate();

	bool InputCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool HighlighterCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(HighlighterCallback);

	bool WindowSpecificInputCallback(iEdInput*);

	tWStringVec mvLoadedFiles;
	tWString msLastPath;

	cWidgetTab* mpTabGeneral;
	cWidgetTab* mpTabBillboard;

	cEdInputEnum* mpInpType;
	cEdInputNumber* mpInpOffset;
	cEdInputVec2* mpInpSize;
	cEdInputColor* mpInpColor;

	cEdInputFile* mpInpMaterial;

	cEdInputText* mpInpConnectedLight;
	cWidgetButton* mpBConnectLight;

	cEdInputBool* mpInpHalo;
	cEdInputVec3* mpInpHaloSourceSize;

	cEdSelectorHighlighter* mpLightHighlighter;
};

//----------------------------------------------------------------------


#endif // ED_EDIT_PANE_BILLBOARD_H
