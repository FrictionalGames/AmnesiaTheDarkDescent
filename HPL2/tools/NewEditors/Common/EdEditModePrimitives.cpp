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

#include "../Common/EdEditModePrimitives.h"

#include "../Common/EdObjPrimitivePlane.h"

#include "../Common/Editor.h"
#include "../Common/EdWorld.h"
#include "../Common/EdWindowPrimitives.h"

#include "../Common/EdWindowViewport.h"


//---------------------------------------------------------------------------

cEdEditModePrimitives::cEdEditModePrimitives(iEditor* apEditor) : iEdEditModeScnObjCreator(apEditor, _W("Primitives")) 
{
	mpSubType = NULL;
}

//---------------------------------------------------------------------------

cEdEditModePrimitives::~cEdEditModePrimitives()
{
}

//---------------------------------------------------------------------------

iEdObjectData* cEdEditModePrimitives::CreateObjectData()
{
	iEdObjectData* pData = NULL;
	
	if(mpSubType->mpCreator)
	{
		pData = mpSubType->CreateData();

		//////////////////////////////
		// General data setup (name)
		iEdEditModeScnObjCreator::SetUpCreationData(pData);
		mpSubType->mpCreator->SetUpData(mpWindow, pData);

		if(pData->GetString(ePrimitiveStr_Material)=="") //|| pData->PostCreateSetUp()==false)
		{
			hplDelete(pData);
			pData = NULL;
		}
	}

	return pData;
}

//---------------------------------------------------------------------------

void cEdEditModePrimitives::OnViewportMouseDown(const cViewportClick& aData)
{
	if(aData.mbLeft==false) return;

	mpSubType->mpCreator->OnViewportMouseDown(aData);

	mbPreCreationActive = true;
}

//---------------------------------------------------------------------------

void cEdEditModePrimitives::OnViewportMouseUp(const cViewportClick& aData)
{
	if(aData.mbRight)
	{
		mpSubType->mpCreator->Reset();
		return;
	}

	if(mpSubType->mpCreator)
	{
		mpSubType->mpCreator->OnViewportMouseUp(aData);

		if(mpSubType->mpCreator->IsDoneCreating())
		{
			iEdObjectData* pData = CreateObjectData();
			iEdAction* pAction = mpEditor->GetWorld()->CreateActionCreateObject(pData);
			mpEditor->AddAction(pAction);

			mbPreCreationActive = false;
		}
	}	
}

//---------------------------------------------------------------------------

void cEdEditModePrimitives::OnDraw(const cModuleDrawData& aData)
{
	iEdEditModeScnObjCreator::OnDraw(aData);

	mpSubType->mpCreator->Draw(aData.mpViewport, aData.mpFunctions);
}

//---------------------------------------------------------------------------

iEdWindow* cEdEditModePrimitives::CreateWindow()
{
	return hplNew(cEdWindowPrimitives,(this));
}

//---------------------------------------------------------------------------

iEdObjectType* cEdEditModePrimitives::CreateType()
{
	return hplNew(cTypePrimitive,());
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
