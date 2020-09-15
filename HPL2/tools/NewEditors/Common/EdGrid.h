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

#ifndef ED_GRID_H
#define ED_GRID_H

#include "../Common/StdAfx.h"

using namespace hpl;

#include "../Common/EdAxisAlignedPlane.h"

//------------------------------------------------

class iEdViewport;
class iEdAction;

//------------------------------------------------

class cEdGrid : public iEdAxisAlignedPlane
{
public:
	cEdGrid(iEdViewport* apViewport);

	static void SetSnapToGrid(bool abX) { mbSnapToGrid = abX; }
	static bool GetSnapToGrid() { return mbSnapToGrid; }

	static void SetSnapSeparation(const float afSeparation);
	static float GetSnapSeparation() { return mfSnapSeparation; }

	void SetSize(const float afSize) { mfSize = afSize; }
	float GetSize() { return mfSize; }

	void SetColor(const cColor& acCol) { mColor = acCol; }
	cColor GetColor() { return mColor; }

	void Draw(cRendererCallbackFunctions* apFunctions);
	void Draw(cRendererCallbackFunctions* apFunctions, const cVector3f& avPos);

	cVector3f GetSnappedPosOnGrid(const cVector3f& avWorldPos, bool abForceSnap=false);
	static cVector3f GetSnappedPosInSpace(const cVector3f& avWorldPos, bool abSnapX=true, bool abSnapY=true, bool abSnapZ=true);

	static float GetSnappedValue(const float afX, bool abSnap, float afSnapValue);

	iEdAction* CreateSetSnapSeparationAction(float afX);

protected:
	void OnPlaneModified();
    /////////////////////////////
	// Data
	iEdViewport* mpViewport;

	static bool	 mbSnapToGrid;
	static float mfSnapSeparation;
	static float mfInvSnapSeparation;
	static float mfMinSnapSep;
	static float mfMaxSnapSep;

	static float mfSize;
	cColor mColor;
};

//------------------------------------------------

#endif // ED_GRID_H

