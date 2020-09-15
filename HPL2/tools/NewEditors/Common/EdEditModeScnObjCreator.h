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

#ifndef ED_EDIT_MODE_SCN_OBJ_CREATOR_H
#define ED_EDIT_MODE_SCN_OBJ_CREATOR_H

#include "../Common/EdEditMode.h"

class iEdEditModeScnObjCreator : public iEdEditModeObjectCreator
{
public:

	iEdEditModeScnObjCreator(iEditor*, const tWString&);
	void SetCreateOnSurface(bool abX) { mbCreateOnSurface = abX; }
	bool GetCreateOnSurface() { return mbCreateOnSurface; }

	void SetAffectSurfaceType(iEdObjectType* apType, bool);
	bool GetAffectSurfaceType(int);

	const cVector3f& GetUpVector();
	const cVector3f& GetRightVector();

	virtual void OnDraw(const cModuleDrawData&);
	virtual void DrawObjectPreview(iEdViewport*, cRendererCallbackFunctions*, const cMatrixf&, bool) {}

	void UpdateCreatorScale();
	void UpdateCreatorRotation();

	void UpdateLocalRotation();
protected:
	void OnUpdate();

	virtual void OnViewportMouseDown(const cViewportClick&);
	virtual void OnViewportMouseUp(const cViewportClick&);

	void OnSetActive(bool);

	const cVector3f& GetCreatorPosition();
	const cVector3f& GetCreatorRotation();
	const cVector3f& GetCreatorScale();

	bool SetUpCreationData(iEdObjectData* apData);

	tWString msBaseName;

	bool mbCreateOnSurface;
	tBoolVec mvAffectedSurfaceTypes;

	cVector3f mvUpVector;
	cVector3f mvRightVector;

	cVector3f mvCreatorPosition;
	cVector3f mvCreatorScale;

	cVector3f mvCreatorLocalRotation;
	cVector3f mvCreatorWorldRotation;
};

#endif // ED_EDIT_MODE_SCN_OBJ_CREATOR_H
