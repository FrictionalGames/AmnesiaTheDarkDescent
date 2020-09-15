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

#ifndef HPLEDITOR_EDITOR_WINDOW_DECALS_H
#define HPLEDITOR_EDITOR_WINDOW_DECALS_H

#include "EditorWindow.h"

//----------------------------------------------------------

class cEditorEditModeDecals;


enum eDecalAffectedType
{
	eDecalAffectedType_StaticObject,
	eDecalAffectedType_Entity,
	eDecalAffectedType_Primitive,

	eDecalAffectedType_LastEnum
};


//----------------------------------------------------------

class cEditorWindowDecals : public iEditModeObjectCreatorWindow
{
public:
	cEditorWindowDecals (cEditorEditModeDecals* apEditMode);
	~cEditorWindowDecals();

	void Reset();

	const tString& GetMaterial();
	float GetAngle();
	cVector3f GetSize();
	cColor GetColor();
	cVector2l GetSubDivisions();
	int GetCurrentSubDiv();

	void UpdateRotate();
	void UpdateScale();

	static eDecalAffectedType TranslateTypeToAffectedType(eEditorEntityType aType);

protected:
	void OnInit(){}
	void OnInitLayout();

	void OnLoadLevel(){}
	void OnUpdate(float afTimeStep);

	void OnSetActive(bool abX);

 	void ResetInputs();

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool WindowSpecificInputCallback(iEditorInput* apInput);

	////////////////////////////////////////////////////
	// Data
	tString msMaterial;

	// Layout stuff
	cEditorInputFile* mpInpMaterial;
	cEditorInputNumber* mpInpMaxTriangles;

	cEditorInputBool* mpInpRandomizeAngle;
	cEditorInputNumber* mpInpMinAngle;
	cEditorInputNumber* mpInpMaxAngle;

	cEditorInputBool* mpInpRandomizeSize;
	cEditorInputBool* mpInpUniformSize;
	cEditorInputVec2* mpInpMinSize;
	cEditorInputVec2* mpInpMaxSize;
	cEditorInputBool* mpInpCustomDepthActive;
	cEditorInputNumber* mpInpCustomDepth;

	cEditorInputColorFrame* mpInpColor;

	cEditorInputEnum* mpInpSubDivisions;
	cEditorInputEnum* mpInpCurrentSubDiv;

	tWStringVec mvFilesToLoad;
};

//----------------------------------------------------------

#endif // HPLEDITOR_EDITOR_WINDOW_DECALS_H
