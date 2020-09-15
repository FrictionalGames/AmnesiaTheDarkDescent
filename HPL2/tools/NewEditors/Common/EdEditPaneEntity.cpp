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

#include "../Common/EdEditPaneEntity.h"

#include "../Common/EdObjEntity.h"
#include "../LevelEditor/LevelEditor.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneEntity::cEdEditPaneEntity(iEdScnObject* apObject) : iEdUserObjEditPane(apObject)
{
}

//----------------------------------------------------------------------------

cEdEditPaneEntity::~cEdEditPaneEntity()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneEntity::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabEntity = mpTabs->AddTab(_W("Entity"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyActive(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);
	AddPropertyScale(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y+5;
	mpInpActive->SetPosition(vPos);
	vPos.y += mpInpActive->GetSize().y+5;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y+5;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y+5;
	mpInpScale->SetPosition(vPos);
	vPos.y += mpInpScale->GetSize().y+5;

	mpInpFile = CreateInputFile(vPos, _W("Entity File"), _W("Entities"), mpTabGeneral, 100, mpEditor->GetMainLookUpDir(eDir_Entities));

    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddInputs(mpTabEntity, cVector3f(10, 10, 0.1f));
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

bool cEdEditPaneEntity::WindowSpecificInputCallback(iEdInput* apInput)
{
	if(apInput==mpInpFile)
	{
		iEdAction* pAction = mpObject->CreateSetStringAction(eEntityStr_Filename, cString::To8Char(mpInpFile->GetFullPath()));
		AddAction(pAction);
	}
	else
		return iEdScnObjEditPane::WindowSpecificInputCallback(apInput);

	return true;
}

//----------------------------------------------------------------------------

void cEdEditPaneEntity::OnUpdate()
{
	iEdUserObjEditPane::OnUpdate();

	cEdObjEntity* pEnt = static_cast<cEdObjEntity*>(mpObject);

	mpInpFile->SetValue(cString::To16Char(pEnt->GetEntFile()->msRelativePath), false);
	mpInpFile->GetInputWidget()->SetToolTip(mpEditor->GetPathRelToWD(mpInpFile->GetFullPath()));
}

//----------------------------------------------------------------------------
