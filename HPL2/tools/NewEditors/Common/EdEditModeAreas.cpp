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

#include "../Common/EdEditModeAreas.h"

#include "../Common/Editor.h"

#include "../Common/EdWindowAreas.h"

//#include "../Common/EdActionsArea.h"
#include "../Common/EdObjArea.h"
#include "../Common/EdWorld.h"

#include "../Common/BoxCreator.h"

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEdEditModeAreas::cEdEditModeAreas(iEditor* apEditor) : iEdEditModeScnObjCreator(apEditor, _W("Areas"))
{
	mpBoxCreator = hplNew(cBoxCreator, (apEditor));
}

//-----------------------------------------------------------------

cEdEditModeAreas::~cEdEditModeAreas()
{
	hplDelete(mpBoxCreator);
}

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

void cEdEditModeAreas::OnViewportMouseDown(const cViewportClick& aClick)
{
	cTypeArea* pType = static_cast<cTypeArea*>(GetSelectedType());
	mpBoxCreator->SetDefaultSize(pType->GetDefaultSize());
	mpBoxCreator->SetDraggingEnabled(pType->GetDrawAsSphere()==false && pType->IsScalable()==true);

	mpBoxCreator->OnViewportMouseDown(aClick);

	mbPreCreationActive =true;
}

//-----------------------------------------------------------------

void cEdEditModeAreas::OnViewportMouseUp(const cViewportClick& aClick)
{
	mpBoxCreator->OnViewportMouseUp(aClick);

	if(mpBoxCreator->IsDoneCreating())
	{
		iEdAction* pAction = mpEditor->GetWorld()->CreateActionCreateObject(CreateObjectData());

		mpEditor->AddAction(pAction);

		mpBoxCreator->Reset();

		mbPreCreationActive = false;
	}
}

//-----------------------------------------------------------------

void cEdEditModeAreas::OnUpdate()
{
	mpBoxCreator->OnUpdate();
}

//-----------------------------------------------------------------

void cEdEditModeAreas::OnDraw(const cModuleDrawData& aData)
{
	mpBoxCreator->Draw(aData.mpViewport, aData.mpFunctions);
}

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

iEdWindow* cEdEditModeAreas::CreateWindow()
{
	return hplNew(cEdWindowAreas,(this));
}

//-----------------------------------------------------------------

bool cEdEditModeAreas::SetUpCreationData(iEdObjectData* apData)
{
	cEdWindowAreas* pWin = static_cast<cEdWindowAreas*>(mpWindow);
	if(iEdEditModeScnObjCreator::SetUpCreationData(apData)==false)
		return false;

	apData->SetVec3f(eScnVec3f_Translation, mpBoxCreator->GetBoxCenter());
	apData->SetVec3f(eScnVec3f_Scale, mpBoxCreator->GetBoxSize());

	return true;
}

//-----------------------------------------------------------------

iEdObjectType* cEdEditModeAreas::CreateType()
{
	return hplNew(cTypeRootArea,());
}

//-----------------------------------------------------------------

void cEdEditModeAreas::OnSetActive(bool abX)
{
	mpBoxCreator->Reset();
}

//-----------------------------------------------------------------


