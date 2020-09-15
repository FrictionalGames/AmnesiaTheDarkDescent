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

#ifndef ED_SCN_OBJ_CREATOR_PANE_H
#define ED_SCN_OBJ_CREATOR_PANE_H

#include "../Common/EdWindowHandler.h"

class iEdObjectData;

class iEdScnObjCreatorPane : public iEdEditModePane
{
public:
	iEdScnObjCreatorPane(iEdEditMode*);

	virtual bool SetUpCreationData(iEdObjectData*) { return true; }

	const cVector3f& GetRotate();
	void SetScaleUpdated() { mbScaleUpdated=true; }
	virtual void UpdateScale();

	const cVector3f& GetScale();
	void SetRotateUpdated() { mbRotateUpdated=true; }
	virtual void UpdateRotate();

protected:
	void AddNameControls();
	void AddTypeControls();
	void AddCreateOnSurfaceControls(bool abAddNormalControls=false);
	void AddScaleControls();
	void AddRotateControls();

	bool SurfaceButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SurfaceButton_OnPressed);
	bool WindowSpecificInputCallback(iEdInput* apInput);

	void OnReset();
	/////////////////////////////////////////////////////
	// Data
	cWidgetDummy* mpGSurface;
	cEdInputBool* mpInpCreateOnSurface;
	std::vector<cWidgetButton*> mvBSurfaceTypes;

	cEdInputText* mpInpBaseName;

	cEdInputEnum* mpInpSubType;

	cWidgetDummy* mpGScale;
	cEdInputBool* mpInpRandScale;
	cEdInputVec3* mpInpMinScale;
	cEdInputVec3* mpInpMaxScale;

	cWidgetDummy* mpGRotate;
	cEdInputBool* mpInpRandRotate;
	cEdInputVec3* mpInpMinRotate;
	cEdInputVec3* mpInpMaxRotate;

	cVector3f mvPos;
	cVector3f mvRotate;
	cVector3f mvScale;

	bool mbScaleUpdated;
	bool mbRotateUpdated;
};

//---------------------------------------------------------------------

#endif // ED_SCN_OBJ_CREATOR_PANE_H
