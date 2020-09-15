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

#ifndef HPLEDITOR_EDITOR_GRID_H
#define HPLEDITOR_EDITOR_GRID_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "EditorAxisAlignedPlane.h"

//------------------------------------------------

class iEditorViewport;

//------------------------------------------------

class cEditorGrid : public iEditorAxisAlignedPlane
{
public:
	cEditorGrid(iEditorViewport* apViewport);

	static void SetSnapToGrid(bool abX) { mbSnapToGrid = abX; }
	static bool GetSnapToGrid() { return mbSnapToGrid; }

	static void SetSnapSeparation(const float afSeparation) { mfSnapSeparation = afSeparation; }
	static float GetSnapSeparation() { return mfSnapSeparation; }

	void SetSize(const float afSize) { mfSize = afSize; }
	float GetSize() { return mfSize; }

	void SetColor(const cColor& acCol) { mColor = acCol; }
	cColor GetColor() { return mColor; }

	void Draw(cRendererCallbackFunctions* apFunctions, const cVector3f& avPos);

	cVector3f GetSnappedPosOnGrid(const cVector3f& avWorldPos, bool abForceSnap=false);
	static cVector3f GetSnappedPosInSpace(const cVector3f& avWorldPos);

protected:
	void OnPlaneModified();
    /////////////////////////////
	// Data
	iEditorViewport* mpViewport;

	static bool mbSnapToGrid;
	static float mfSnapSeparation;
	float mfSize;

	cColor mColor;
};

//------------------------------------------------

#endif // HPLEDITOR_EDITOR_GRID_H

