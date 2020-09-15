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

#include "../Common/EdEditPaneArea.h"

#include "../Common/EdObjArea.h"

#include "../Common/EdEditModeAreas.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneArea::cEdEditPaneArea(iEdScnObject* apObject) : iEdUserObjEditPane(apObject)
{
}

//----------------------------------------------------------------------------

cEdEditPaneArea::~cEdEditPaneArea()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneArea::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabArea = mpTabs->AddTab(_W("Area"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddCompoundDetachButton(mpTabGeneral);
	AddPropertyName(mpTabGeneral);
	AddPropertyActive(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);
	AddPropertyScale(mpTabGeneral);

	mpInpScale->SetLabel(_W("Size"));

	cVector3f vPos = cVector3f(10,10,0.1f);
	////////////////////////////////////////
	//
	mpInpType = CreateInputEnum(vPos, _W("Type"), mpTabGeneral);
	cEdObjArea* pArea = static_cast<cEdObjArea*>(mpObject);
	iUserTypeNode* pAreaType = static_cast<iUserTypeNode*>(pArea->GetType());
	iUserTypeRoot* pRootType = pAreaType->GetUserTypeRoot();
	for(int i=0; i<pRootType->GetNumChildren(); ++i)
	{
		iEdObjectType* pType = pRootType->GetChild(i);
		mpInpType->AddValue(pType->GetName(), pType);
	}

	vPos.y += mpInpType->GetSize().y + 5;

	if(mpBRemoveFromCompound)
	{
		mpBRemoveFromCompound->SetPosition(vPos);
		vPos.y += mpBRemoveFromCompound->GetSize().y + 5;
	}
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


	mpInpMesh = CreateInputMesh(vPos, _W("Mesh"),  mpTabGeneral);

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

void cEdEditPaneArea::OnUpdate()
{
	iEdUserObjEditPane::OnUpdate();

	cEdObjArea* pArea = static_cast<cEdObjArea*>(mpObject);

	mpInpType->SetValue(pArea->GetType()->GetName(), false);
	mpInpMesh->SetValue(cString::To16Char(pArea->GetMeshFile()), false);
}

//----------------------------------------------------------------------------

bool cEdEditPaneArea::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdAction* pAction = NULL;
	cEdObjArea* pArea = static_cast<cEdObjArea*>(mpObject);

	if(apInput==mpInpType)
	{
		iUserTypeNode* pType = static_cast<iUserTypeNode*>(mpInpType->GetSelectedUserData());
		pAction = pArea->CreateSetUserTypeAction(pType);

		SetUpdated();
	}
	else if(apInput==mpInpMesh)
	{
		pAction = mpObject->CreateSetStringAction(eAreaStr_Mesh, mpInpMesh->GetValue8Char());
	}
	else
		return iEdUserObjEditPane::WindowSpecificInputCallback(apInput);

	AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
