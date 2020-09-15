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

#ifndef ED_EDIT_MODE_AREAS_H
#define ED_EDIT_MODE_AREAS_H

#include "../Common/StdAfx.h"

using namespace hpl;

#include "../Common/EdEditModeScnObjCreator.h"

//#include "../Common/EditorTypes.h"

//------------------------------------------------------------

class iEditor;
class iEdWorld;

class cBoxCreator;

//------------------------------------------------------------


//------------------------------------------------------------

class cEdEditModeAreas : public iEdEditModeScnObjCreator
{
public:
	cEdEditModeAreas(iEditor*);
	~cEdEditModeAreas();

	void SetAreaType(const tString& asAreaType) { msAreaType = asAreaType; }

	void OnViewportMouseDown(const cViewportClick&);
	void OnViewportMouseUp(const cViewportClick&);

	void OnUpdate();

	void OnDraw(const cModuleDrawData&);

	//void DrawObjectPreview(iEdViewport*, cRendererCallbackFunctions*, const cMatrixf&, bool) {}

protected:
	bool SetUpCreationData(iEdObjectData* apData);
	iEdObjectType* CreateType();

	void OnSetActive(bool abX);

	iEdWindow* CreateWindow();

	tString msAreaType;

	cBoxCreator* mpBoxCreator;
};

//------------------------------------------------------------

#endif // ED_EDIT_MODE_AREAS_H

