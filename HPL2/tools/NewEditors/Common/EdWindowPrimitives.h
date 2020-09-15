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

#ifndef ED_WINDOW_PRIMITIVES_H
#define ED_WINDOW_PRIMITIVES_H

#include "../Common/EdScnObjCreatorPane.h"

//----------------------------------------------------------

class cEdEditModePrimitives;

class iPrimitiveSubType;

//----------------------------------------------------------

class cEdWindowPrimitives : public iEdScnObjCreatorPane
{
public:
	cEdWindowPrimitives(iEdEditMode*);
	~cEdWindowPrimitives();

	iPrimitiveSubType* GetCurrentType() { return mpCurrentType; }

	bool SetUpCreationData(iEdObjectData*);

	const tString& GetMaterial();
	cVector3f GetTileAmount();
	cVector3f GetTileOffset();
	float GetTextureAngle();
	bool GetAlignToWorld();
	bool GetCollides();
	bool GetCastShadows();

protected:
	void OnCreateLayout();

 	void OnReset();

	bool WindowSpecificInputCallback(iEdInput*);

	////////////////////////////////////////////////////
	// Data
	
	// Layout stuff
	// General
	cEdInputFile* mpInputMaterial;
	cEdInputBool* mpInputCastShadows;
	cEdInputBool* mpInpCollides;

	// Plane
	cEdInputNumber* mpInpTextureAngle;
	cEdInputVec3* mpInputTileAmount;
	cEdInputVec3* mpInputTileOffset;
	cEdInputBool* mpInputAlignToWorld;

	iPrimitiveSubType* mpCurrentType;
};

//----------------------------------------------------------

#endif // ED_WINDOW_PRIMITIVES_H
