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

#include "../Common/EdWindowBillboards.h"
#include "../Common/EdEditModeBillboards.h"

#include "../Common/EdObjBillboard.h"

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdWindowBillboards::cEdWindowBillboards(iEdEditMode* apMode) : iEdScnObjCreatorPane(apMode)
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEdWindowBillboards::SetUpCreationData(iEdObjectData* apData)
{
	apData->SetString(eBillboardStr_Material, mpInpMaterial->GetValue8Char());
	apData->SetColor(eBillboardCol_Color, mpInpColor->GetValue());

	return true;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cEdWindowBillboards::OnCreateLayout()
{
	iEdPane::OnCreateLayout();

	cVector3f vPos = cVector3f(15,15,0.1f);

	AddNameControls();
	mpInpBaseName->SetPosition(vPos);
	vPos.y += mpInpBaseName->GetSize().y + 10;
	
	mpInpMaterial = CreateInputMaterial(vPos, _W("Material"), GetBG());
	vPos.y += mpInpMaterial->GetSize().y + 10;

	mpInpColor = CreateInputColor(vPos, _W("Color"),  GetBG());
}

//---------------------------------------------------------------------------
