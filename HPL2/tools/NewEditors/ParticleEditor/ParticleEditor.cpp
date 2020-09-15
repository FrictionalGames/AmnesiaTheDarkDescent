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

#include "../ParticleEditor/ParticleEditor.h"
#include "../ParticleEditor/HPLParticleSystem.h"

#include "../Common/EdEditModeHandler.h"

#include "../Common/GenericBG.h"
#include "../ParticleEditor/EdPEMenu.h"
#include "../Common/EdViewMenu.h"

#include "../ParticleEditor/EdEditModeParticleEmitters.h"
#include "../ParticleEditor/EdPEViewport.h"

//----------------------------------------------------------------------

iEditor* iEditor::CreateInstance(const tWString& asCommandLine)
{
	return hplNew(cParticleEditor,());
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cParticleEditor::cParticleEditor() : iEditor("ParticleEditor", "HPL Particle Editor")
{
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cParticleEditor::SetUpDirectories()
{

}

//----------------------------------------------------------------------

void cParticleEditor::SetUpEditModes()
{
	mpEditModeHandler->AddMode(hplNew(cEdEditModeParticleEmitters,(this)));
}

//----------------------------------------------------------------------

iEdMenu* cParticleEditor::CreateMenu()
{
	return hplNew(cEdPEMenu,(mpWindowHandler->GetBackground()));
}

//----------------------------------------------------------------------

iEdWorld* cParticleEditor::CreateWorld()
{
	return hplNew(cHplParticleSystem,(this));
}

//----------------------------------------------------------------------

void cParticleEditor::CreateAppSpecificLayout()
{
	mpWindowHandler->AddBackground(hplNew(cGenericBG,(this)));

	///////////////////////////////////////////////////////////////////////
	// Layout blocks definition															
	const cLayoutBlock* pEmitterBlock = mpWindowHandler->AddLayoutBlock(eLayoutBlock_EditMode, false, false, cVector3f(0,0,1), false, false, cVector2f(512, 100));  
	const cLayoutBlock* pEmitterPaneBlock = mpWindowHandler->AddLayoutBlock(eLayoutBlock_Sidebar, false, false, cVector3f(0, pEmitterBlock->mvSize.y,1), false, false, cVector2f(512, -1));
	const cLayoutBlock* pViewportBlock = mpWindowHandler->AddLayoutBlock(eLayoutBlock_ViewportArea,
																			false, false, 
																			pEmitterBlock->mvPosition + cVector3f(pEmitterBlock->mvSize.x,0,0),
																			false, false, 
																			cVector2f(512, -1)); 

	AddViewport(hplNew(cEdPEViewport,(this)), 
				pViewportBlock->mvPosition, pViewportBlock->mvSize, 
				cVector2f(0), pViewportBlock->mvSize);
		
}

//----------------------------------------------------------------------
