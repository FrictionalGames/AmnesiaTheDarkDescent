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

#include "../ParticleEditor/EdEditModeParticleEmitters.h"

#include "../Common/Editor.h"

#include "../ParticleEditor/EdWindowEmitters.h"
#include "../ParticleEditor/EdObjParticleEmitter.h"

#include "../Common/EdEditPane.h"

//--------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------

cEdEditModeParticleEmitters::cEdEditModeParticleEmitters(iEditor* apEditor) : iEdEditModeObjectCreator(apEditor, _W("Particle Emitters"))
{
	mpSelectedEmitter = NULL;
	mpSelectedEmitterPane = NULL;
}

//--------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------

void cEdEditModeParticleEmitters::SetSelectedEmitter(iEdObject* apEmitter)
{
	if(mpSelectedEmitter==apEmitter)
		return;

	if(mpSelectedEmitterPane)
	{
		mpEditor->GetWindowHandler()->DestroyWindow(mpSelectedEmitterPane);
		mpSelectedEmitterPane = NULL;
	}

	mpSelectedEmitter = apEmitter;

	if(mpSelectedEmitter)
	{
		mpSelectedEmitterPane = mpSelectedEmitter->CreateEditPane();
		mpEditor->GetWindowHandler()->AddWindow(mpSelectedEmitterPane,eLayoutBlock_Sidebar);
	}
}

//--------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------

void cEdEditModeParticleEmitters::OnReset()
{
}

//--------------------------------------------------------------------------------------

iEdWindow* cEdEditModeParticleEmitters::CreateWindow()
{
	return hplNew(cEdWindowEmitters,(this));
}

//--------------------------------------------------------------------------------------

iEdObjectType* cEdEditModeParticleEmitters::CreateType()
{
	return hplNew(cTypeParticleEmitter,());
}

//--------------------------------------------------------------------------------------
