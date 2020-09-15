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

#include "EditorWindowEntityEditBoxStaticObject.h"

#include "EntityWrapperStaticObject.h"

#include "EditorEditModeSelect.h"

#include "EditorInput.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxStaticObject::cEditorWindowEntityEditBoxStaticObject(cEditorEditModeSelect* apEditMode, cEntityWrapperStaticObject* apObject) : cEditorWindowEntityEditBox(apEditMode,apObject)
{
	mpEntity = apObject;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxStaticObject::~cEditorWindowEntityEditBoxStaticObject()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxStaticObject::Create()
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

	mpInpMeshFile = CreateInputFile(vPos, _W("Mesh"), "", mpTabGeneral, 100);
	mpInpMeshFile->SetBrowserType(eEditorResourceType_Model);

    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddPropertySetStaticObject(mpTabStaticObject);

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

void cEditorWindowEntityEditBoxStaticObject::AddPropertySetStaticObject(cWidgetTab* apParentTab)
{
	mpInpCastShadows = CreateInputBool(0, _W("Cast shadows"), "", apParentTab);
	mpInpCollides = CreateInputBool(0, _W("Collides"), "", apParentTab);
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxStaticObject::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput))
		return true;

	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	int lID = mpEntity->GetID();
	iEditorAction* pAction = NULL;

	if(apInput==mpInpMeshFile)
	{
		pAction = mpEntity->CreateSetPropertyActionString(eStaticObjectStr_Filename, cString::To8Char(mpInpMeshFile->GetFullPath()));
	}
	else if(apInput==mpInpCastShadows)
	{
		pAction = mpEntity->CreateSetPropertyActionBool( eStaticObjectBool_CastShadows, mpInpCastShadows->GetValue());
	}
	else if(apInput==mpInpCollides)
	{
		pAction = mpEntity->CreateSetPropertyActionBool( eStaticObjectBool_Collides, mpInpCollides->GetValue());
	}

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxStaticObject::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);

	mpInpMeshFile->SetValue(cString::To16Char(mpEntity->GetFilename()), false);
	mpInpMeshFile->GetInputWidget()->SetToolTip(mpEditor->GetPathRelToWD(mpInpMeshFile->GetFullPath()));
	mpInpCastShadows->SetValue(mpEntity->GetCastShadows(), false);
	mpInpCollides->SetValue(mpEntity->GetCollides(), false);
}

//----------------------------------------------------------------------------
