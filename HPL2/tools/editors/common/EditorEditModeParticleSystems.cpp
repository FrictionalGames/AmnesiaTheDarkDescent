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

#include "EditorEditModeParticleSystems.h"

#include "EditorBaseClasses.h"
#include "EditorActionHandler.h"
#include "EditorWorld.h"

#include "EntityWrapperParticleSystem.h"

#include "EditorWindowViewport.h"
#include "EditorWindowParticleSystems.h"

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEditorEditModeParticleSystems::cEditorEditModeParticleSystems(iEditorBase* apEditor,
															   iEditorWorld* apEditorWorld) : iEditorEditModeObjectCreator(apEditor,"Particle Systems", apEditorWorld)
{
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

bool cEditorEditModeParticleSystems::SetUpCreationData(iEntityWrapperData* apData)
{
	if(iEditorEditModeObjectCreator::SetUpCreationData(apData)==false)
		return false;

	cEditorWindowParticleSystems* pWin = (cEditorWindowParticleSystems*)GetEditorWindow();

	apData->SetString(eParticleSystemStr_File, cString::To8Char(pWin->GetPSFileName()));
	apData->SetColor(eParticleSystemCol_Color, pWin->GetPSColor());

	return true;	
}

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

void cEditorEditModeParticleSystems::CreateTypes()
{
	mvTypes.push_back(hplNew(cEntityWrapperTypeParticleSystem,()));
}

//-----------------------------------------------------------------

iEditorWindow* cEditorEditModeParticleSystems::CreateSpecificWindow()
{
	return hplNew(cEditorWindowParticleSystems,(this));
}

//-----------------------------------------------------------------


