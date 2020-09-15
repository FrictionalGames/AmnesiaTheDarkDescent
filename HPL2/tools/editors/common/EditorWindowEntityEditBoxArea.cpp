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

#include "EditorWindowEntityEditBoxArea.h"

#include "EntityWrapperArea.h"

#include "EditorEditModeSelect.h"
#include "EditorEditModeAreas.h"

#include "EditorActionArea.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxArea::cEditorWindowEntityEditBoxArea(cEditorEditModeSelect* apEditMode, cEntityWrapperArea* apArea) : cEditorWindowEntityEditBoxUserDefinedEntity(apEditMode, apArea)
{
	mpEntity = apArea;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxArea::~cEditorWindowEntityEditBoxArea()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxArea::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabArea = mpTabs->AddTab(_W("Area"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyActive(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);
	AddPropertyScale(mpTabGeneral);

	mpInpScale->SetLabel(_W("Size"));

	cVector3f vPos = cVector3f(10,10,0.1f);
	////////////////////////////////////////
	//
	mpInpType = CreateInputEnum(vPos, _W("Type"), "", tWStringList(), mpTabGeneral);
	cEditorEditModeAreas* pEditMode = (cEditorEditModeAreas*)mpEditor->GetEditMode("Areas");
	for(int i=0;i<pEditMode->GetTypeNum();++i)
	{
		cEntityWrapperTypeArea* pType = (cEntityWrapperTypeArea*)pEditMode->GetType(i);
		tWString sTypeName = cString::To16Char(pType->GetAreaType());
		mpInpType->AddValue(sTypeName);
	}
	vPos.y += mpInpType->GetSize().y + 5;

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


	mpInpMesh = CreateInputFile(vPos, _W("Mesh"), "", mpTabGeneral);
	mpInpMesh->SetBrowserType(eEditorResourceType_Model);

    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddInputs(mpTabArea, cVector3f(15,30,0.1f));
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxArea::OnUpdate(float afTimeStep)
{
	if(mpEntity->GetTypeChanged())
	{
		SetRefreshInputs();
		mpEntity->SetTypeChanged(false);
	}

	cEditorWindowEntityEditBoxUserDefinedEntity::OnUpdate(afTimeStep);

	cEntityWrapperTypeArea* pType = (cEntityWrapperTypeArea*)mpEntity->GetType();
	((iEditorInput*)mpInpType)->SetValue(cString::To16Char(pType->GetAreaType()), false);
	mpInpMesh->SetValue(cString::To16Char(mpEntity->GetMeshFile()), false);
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxArea::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBoxUserDefinedEntity::WindowSpecificInputCallback(apInput))
		return true;

	iEditorAction* pAction = NULL;
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	int lID = mpEntity->GetID();
	if(apInput==mpInpType)
	{
		pAction = hplNew(cEditorActionAreaSetType,(pWorld, lID, cString::To8Char(mpInpType->GetEnumValue(mpInpType->GetValue()))));
	}
	else if(apInput==mpInpMesh)
	{
		pAction = mpEntity->CreateSetPropertyActionString(eAreaStr_Mesh, cString::To8Char(mpInpMesh->GetValue()));
	}

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
