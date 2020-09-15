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

#include "EditorWindowEntityEditBoxSubMesh.h"

#include "EntityWrapperBody.h"

#include "EditorEditModeSelect.h"

#include "EditorWindowViewport.h"
#include "EditorWorld.h"

#include "EditorActionEntity.h"
#include "EditorActionsSubMesh.h"

#include "EntityWrapperSubMesh.h"

#include "EditorWindowEntityEditBoxJoint.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// SUBMESH EDIT BOX
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxSubMesh::cEditorWindowEntityEditBoxSubMesh(cEditorEditModeSelect* apEditMode,cEntityWrapperSubMesh* apObject) : cEditorWindowEntityEditBox(apEditMode,apObject)
{
	mpEntity = apObject;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxSubMesh::~cEditorWindowEntityEditBoxSubMesh()
{
	
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxSubMesh::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);
	AddPropertyScale(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y +5;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y +5;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y +5;
	mpInpScale->SetPosition(vPos);
	vPos.y += mpInpScale->GetSize().y +5;

	mpInpMaterial = CreateInputFile(vPos, _W("Material"), "", mpTabGeneral);
	mpInpMaterial->SetBrowserType(eEditorResourceType_Material);
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxSubMesh::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput)==true)
		return true;

	iEditorAction* pAction = NULL;

	if(apInput==mpInpMaterial)
	{
		pAction = mpEntity->CreateSetPropertyActionString(eSubMeshStr_Material, cString::To8Char(mpInpMaterial->GetValue()));
		//pAction = hplNew(cEditorActionSubMeshSetStringProperty,(lID, pWorld, eSubMeshStringProperty_Material, cString::To8Char(mpInpMaterial->GetValue())));
	}

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxSubMesh::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);
}

//----------------------------------------------------------------------------
