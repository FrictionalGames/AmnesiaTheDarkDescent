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

#ifndef ED_EDIT_PANE_BODY_H
#define ED_EDIT_PANE_BODY_H

#include "../Common/StdAfx.h"
using namespace hpl;

#include "../Common/EdScnObjEditPane.h"

//----------------------------------------------------------------------

class iEntityWrapper;
class cEdEditModeSelect;
class cEntityWrapperBody;

//----------------------------------------------------------------------

class cEdEditPaneBody : public iEdScnObjEditPane
{
public:
	cEdEditPaneBody(iEdObject*);
	virtual ~cEdEditPaneBody();

	void Create();

	void AddAttachedChild(iEdScnObject* apChild);
	void RemoveAttachedChild(iEdScnObject* apChild);

protected:
	void OnUpdate(); 

	void AddPropertySetBody(cWidgetTab* apParentTab);

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool WindowSpecificInputCallback(iEdInput* apInput);

	//////////////////////////////////
	// Data
	cWidgetTab* mpTabGeneral;
	cWidgetTab* mpTabBody;
	cWidgetTab* mpTabAttachments;

	cEdInputEnum* mpInpMaterial;
	cEdInputNumber* mpInpMass;
	cEdInputNumber* mpInpAngularDamping;
	cEdInputNumber* mpInpLinearDamping;
	cEdInputNumber* mpInpMaxAngularSpeed;
	cEdInputNumber* mpInpMaxLinearSpeed;

	cEdInputNumber* mpInpBuoyancyDensityMul;

	cEdInputBool* mpInpBlocksSound;
	cEdInputBool* mpInpContinuousCollision;
	
	cEdInputBool* mpInpCanAttachCharacter;
	cEdInputBool* mpInpPushedByCharacterGravity;
	
	cEdInputBool* mpInpCollideCharacter;
	cEdInputBool* mpInpCollideNonCharacter;
	
	cEdInputBool* mpInpVolatile;

	cEdInputBool* mpInpUseSurfaceEffects;
	cEdInputBool* mpInpHasGravity;
};

//----------------------------------------------------------------------


#endif // ED_EDIT_PANE_BODY_H
