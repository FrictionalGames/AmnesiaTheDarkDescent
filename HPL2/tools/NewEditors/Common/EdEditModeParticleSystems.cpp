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

#include "../Common/EdEditModeParticleSystems.h"

#include "../Common/Editor.h"
#include "../Common/EdActionHandler.h"
#include "../Common/EdWorld.h"

#include "../Common/EdObjParticleSystem.h"

#include "../Common/EdWindowViewport.h"
#include "../Common/EdWindowParticleSystems.h"

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEdEditModeParticleSystems::cEdEditModeParticleSystems(iEditor* apEditor) : iEdEditModeScnObjCreator(apEditor,_W("Particle Systems"))
{
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

/*
bool cEdEditModeParticleSystems::SetUpCreationData(iEdObjectData* apData)
{
	if(iEdEditModeScnObjCreator::SetUpCreationData(apData)==false)
		return false;

	//cEdWindowParticleSystems* pWin = (cEdWindowParticleSystems*)GetEditorWindow();

	//apData->SetString(eParticleSystemStr_File, cString::To8Char(pWin->GetPSFileName()));
	//apData->SetColor(eParticleSystemCol_Color, pWin->GetPSColor());

	return true;	
}
*/

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

iEdObjectType* cEdEditModeParticleSystems::CreateType()
{
	return hplNew(cTypeParticleSystem, ());
}

//-----------------------------------------------------------------

iEdWindow* cEdEditModeParticleSystems::CreateWindow()
{
	return hplNew(cEdWindowParticleSystems,(this));
}

//-----------------------------------------------------------------


