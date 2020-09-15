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

#include "../Common/EdEditPaneStaticObject.h"

#include "../Common/EdObjStaticObject.h"
#include "../Common/Editor.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneStaticObject::cEdEditPaneStaticObject(iEdScnObject* apObject) : iEdScnObjEditPane(apObject)
{
}

//----------------------------------------------------------------------------

cEdEditPaneStaticObject::~cEdEditPaneStaticObject()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneStaticObject::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabStaticObject = mpTabs->AddTab(_W("Static Object"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);
	AddPropertyScale(mpTabGeneral);
	
	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y+5;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y+5;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y+5;
	mpInpScale->SetPosition(vPos);
	vPos.y += mpInpScale->GetSize().y+5;

	mpInpMeshFile = CreateInputMesh(vPos, _W("Mesh"), mpTabGeneral);

    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddStaticObjectProps(mpTabStaticObject);

	vPos = cVector3f(10,10,0.1f);

	mpInpCastShadows->SetPosition(vPos);
	vPos.y += mpInpCastShadows->GetSize().y+5;
	mpInpCollides->SetPosition(vPos);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneStaticObject::AddStaticObjectProps(cWidgetTab* apParentTab)
{
	mpInpCastShadows = CreateInputBool(0, _W("Cast shadows"),  apParentTab);
	mpInpCollides = CreateInputBool(0, _W("Collides"),  apParentTab);
}

//----------------------------------------------------------------------------

bool cEdEditPaneStaticObject::WindowSpecificInputCallback(iEdInput* apInput)
{
	if(iEdScnObjEditPane::WindowSpecificInputCallback(apInput))
		return true;

	iEdAction* pAction = NULL;

	if(apInput==mpInpMeshFile)
	{
		pAction = mpObject->CreateSetStringAction(eStaticObjectStr_Filename, cString::To8Char(mpInpMeshFile->GetFullPath()));
	}
	else if(apInput==mpInpCastShadows)
	{
		pAction = mpObject->CreateSetBoolAction(eStaticObjectBool_CastShadows, mpInpCastShadows->GetValue());
	}
	else if(apInput==mpInpCollides)
	{
		pAction = mpObject->CreateSetBoolAction(eStaticObjectBool_Collides, mpInpCollides->GetValue());
	}

	AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------

void cEdEditPaneStaticObject::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();

	cEdObjStaticObject* pObj = static_cast<cEdObjStaticObject*>(mpObject);

	mpInpMeshFile->SetValue(cString::To16Char(pObj->GetMeshFile()->msRelativePath), false);
	mpInpMeshFile->GetInputWidget()->SetToolTip(mpEditor->GetPathRelToWD(mpInpMeshFile->GetFullPath()));

	mpInpCastShadows->SetValue(pObj->GetCastShadows(), false);
	mpInpCollides->SetValue(pObj->GetCollides(), false);
}

//----------------------------------------------------------------------------
