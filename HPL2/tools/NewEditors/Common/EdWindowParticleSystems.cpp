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

#include "../Common/EdWindowParticleSystems.h"
#include "../Common/EdEditModeParticleSystems.h"

#include "../Common/EdWorld.h"
#include "../Common/EdObjParticleSystem.h"

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdWindowParticleSystems::cEdWindowParticleSystems(iEdEditMode* apMode) : iEdScnObjCreatorPane(apMode)
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEdWindowParticleSystems::SetUpCreationData(iEdObjectData* apData)
{
	apData->SetString(eParticleSystemStr_File, mpInpPSFile->GetValue8Char());
	apData->SetColor(eParticleSystemCol_Color, mpInpColor->GetValue());

	return true;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cEdWindowParticleSystems::OnCreateLayout()
{
	iEdScnObjCreatorPane::OnCreateLayout();

	cVector3f vPos = cVector3f(15,15,0.1f);

	AddNameControls();
	mpInpBaseName->SetPosition(vPos);
	vPos.y += mpInpBaseName->GetSize().y + 10;

	mpInpPSFile = CreateInputParticleSystem(vPos, _W("PS File"),  GetBG());
	vPos.y += mpInpPSFile->GetSize().y + 10;

	mpInpColor = CreateInputColor(vPos, _W("Color"),  GetBG());

}

//---------------------------------------------------------------------------

