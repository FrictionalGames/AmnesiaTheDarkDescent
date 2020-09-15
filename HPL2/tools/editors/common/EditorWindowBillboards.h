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

#ifndef HPLEDITOR_EDITOR_WINDOW_BILLBOARDS_H
#define HPLEDITOR_EDITOR_WINDOW_BILLBOARDS_H

#include "EditorWindow.h"

//----------------------------------------------------------

class cEditorEditModeBillboards;

//----------------------------------------------------------

class cEditorWindowBillboards : public iEditModeObjectCreatorWindow
{
public:
	cEditorWindowBillboards(cEditorEditModeBillboards* apEditMode);

	const tWString& GetBBMaterial() { return mpInpMaterial->GetFullPath(); }
	const cColor& GetBBColor() { return mpInpColor->GetValue(); }

protected:
	void OnInit(){}
	void OnInitLayout();

	void OnLoadLevel(){}
	void OnUpdate(float afTimeStep){}
	
	////////////////////////////////////////////////////
	// Data
	cEditorEditModeBillboards* mpEditMode;

	// Layout stuff
	cWidgetGroup* mpGroupTypeSelector;

	cEditorInputFile* mpInpMaterial;
	cEditorInputColorFrame* mpInpColor;
};

//----------------------------------------------------------

#endif // HPLEDITOR_EDITOR_WINDOW_BILLBOARDS_H
