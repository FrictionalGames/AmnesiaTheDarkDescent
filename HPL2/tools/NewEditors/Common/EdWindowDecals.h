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

#ifndef ED_WINDOW_DECALS_H
#define ED_WINDOW_DECALS_H

#include "../Common/EdScnObjCreatorPane.h"

//----------------------------------------------------------

class cEdEditModeDecals;

//----------------------------------------------------------

class cEdWindowDecals : public iEdScnObjCreatorPane
{
public:
	cEdWindowDecals (cEdEditModeDecals* apEditMode);
	~cEdWindowDecals();

	bool SetUpCreationData(iEdObjectData*);

	void Reset();

	const tString& GetMaterial();
	float GetAngle();
	cVector3f GetSize();
	cColor GetColor();
	cVector2l GetSubDivisions();
	int GetCurrentSubDiv();

	void UpdateRotate();
	void UpdateScale();

protected:
	void OnCreateLayout();

	void OnUpdate();

	void OnSetActive(bool abX);

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool WindowSpecificInputCallback(iEdInput* apInput);

	////////////////////////////////////////////////////
	// Data
	tString msMaterial;

	// Layout stuff
	cEdInputFile* mpInpMaterial;
	cEdInputNumber* mpInpMaxTriangles;

	cEdInputBool* mpInpRandomizeAngle;
	cEdInputNumber* mpInpMinAngle;
	cEdInputNumber* mpInpMaxAngle;

	cEdInputBool* mpInpRandomizeSize;
	cEdInputBool* mpInpUniformSize;
	cEdInputVec2* mpInpMinSize;
	cEdInputVec2* mpInpMaxSize;
	cEdInputBool* mpInpCustomDepthActive;
	cEdInputNumber* mpInpCustomDepth;

	cEdInputColor* mpInpColor;

	cEdInputEnum* mpInpSubDivisions;
	cEdInputEnum* mpInpCurrentSubDiv;

	tWStringVec mvFilesToLoad;
};

//----------------------------------------------------------

#endif // ED_WINDOW_DECALS_H
