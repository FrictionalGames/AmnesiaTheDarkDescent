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

#ifndef HPLEDITOR_EDITOR_EDIT_MODE_DECALS_H
#define HPLEDITOR_EDITOR_EDIT_MODE_DECALS_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "EditorEditMode.h"
#include "EditorEditModeSelect.h"

#include "EditorTypes.h"

//------------------------------------------------------------

class iEditorBase;
class iEditorWorld;

class cEditorEditModeDecals;

//------------------------------------------------------------

//------------------------------------------------------------

class cEditorEditModeDecals : public iEditorEditModeObjectCreator
{
	friend class cEditorDecalCreator;
public:
	cEditorEditModeDecals(iEditorBase* apEditor,
						  iEditorWorld* apEditorWorld);
	~cEditorEditModeDecals();

	void DrawPostGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos);

	void OnViewportMouseDown(int alButtons);
	void OnViewportMouseUp(int alButtons);

	void OnEditorUpdate(float afTimeStep);

	void Reset(){}

	void SetCurrentSubDiv(int alX) { mlCurrentSubDiv = alX; }

	void UpdateDecalRotation();
	void UpdateDecalSize();

protected:
	void OnSetCurrent(bool abX);
	void UpdateDecalParams();

	bool SetUpCreationData(iEntityWrapperData* apData);
	void CreateTypes();

	iEditorWindow* CreateSpecificWindow();

	int mlCurrentSubDiv;
	float mfOffset;

	tIntVec mvAffectedEntIDs;
};

//------------------------------------------------------------

#endif // HPLEDITOR_EDITOR_EDIT_MODE_DECALS_H

