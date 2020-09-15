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

#ifndef ED_EDIT_PANE_PRIMITIVE_H
#define ED_EDIT_PANE_PRIMITIVE_H

#include "../Common/StdAfx.h"

using namespace hpl;

#include "../Common/EdScnObjEditPane.h"

//----------------------------------------------------------------------

class cEdEditPanePrimitive : public iEdScnObjEditPane
{
public:
	cEdEditPanePrimitive(iEdScnObject*);
	virtual ~cEdEditPanePrimitive();

	void Create();
protected:
	void AddPrimitivePropertySet(cWidgetTab* apParentTab);
	void AddPrimitivePropertySetPlane(cWidgetTab* apParentTab);
	void OnUpdate();

	//bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	//kGuiCallbackDeclarationEnd(InputCallback);

	//bool FileMaterialCallback(iWidget* apWidget, const cGuiMessageData& aData);
	//kGuiCallbackDeclarationEnd(FileMaterialCallback);


	bool WindowSpecificInputCallback(iEdInput* apInput);

	cWidgetTab* mpTabGeneral;
	cWidgetTab* mpTabPrimitive;
	cWidgetTab* mpTabPrimitiveType;

	cEdInputVec3* mpInpTileAmount;
	cEdInputVec3* mpInpTileOffset;
	cEdInputNumber* mpInpTextureAngle;

	cEdInputFile* mpInpMaterial;
	
	cWidgetCheckBox* mpCBAlignToWorld;

    cEdInputBool* mpInpAlignToWorld;	
	cEdInputBool* mpInpCastShadows;
	cEdInputBool* mpInpCollides;

	tWStringVec mvLoadedFiles;
	tWString msLastPath;
};

//----------------------------------------------------------------------


#endif // ED_EDIT_PANE_PRIMITIVE_H
