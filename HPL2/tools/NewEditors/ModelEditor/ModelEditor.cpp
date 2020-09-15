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

#include "../ModelEditor/ModelEditor.h"
#include "../ModelEditor/HPLEntity.h"
#include "../ModelEditor/ModelEditorMenu.h"

#include "../Common/EdEditModeHandler.h"
#include "../Common/DirectoryHandler.h"

#include "../Common/EdEditModeSelect.h"
#include "../Common/EdEditModeLights.h"
#include "../Common/EdEditModeBillboards.h"
#include "../Common/EdEditModeParticleSystems.h"
#include "../Common/EdEditModeSounds.h"
#include "../ModelEditor/EdEditModeShapes.h"
#include "../ModelEditor/EdEditModeJoints.h"

#include "../Common/GenericBG.h"
#include "../Common/EdWindowEditModeSidebar.h"
#include "../Common/EdWindowViewport.h"

#include "../ModelEditor/EdLowerToolbarModel.h"

#include "../ModelEditor/EdWindowEntityClass.h"
#include "../ModelEditor/EdWindowOutline.h"
#include "../ModelEditor/EdWindowAnimations.h"
#include "../ModelEditor/EdWindowModelEditorOptions.h"

//----------------------------------------------------------------------

iEditor* iEditor::CreateInstance(const tWString& asCommandLine)
{
	return hplNew(cModelEditor,());
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cModelEditor::cModelEditor() : iEditor("ModelEditor", "HPL Model Editor")
{
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cModelEditor::ShowEntityClassDialog()
{
	ShowDialog(hplNew(cEdWindowEntityClass,(this)), true);
}

void cModelEditor::ShowOutlineDialog()
{
	ShowDialog(hplNew(cEdWindowOutline,(this)), true);
}

void cModelEditor::ShowAnimationsDialog()
{
	ShowDialog(hplNew(cEdWindowAnimations,(this)), true);
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cModelEditor::SetUpDirectories()
{
	mpDirHandler->AddLookUpDir(eDir_Particles, _W("particles"), true);
}

//----------------------------------------------------------------------

void cModelEditor::SetUpEditModes()
{
	mpEditModeHandler->AddMode(hplNew(cEdEditModeSelect,(this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeLights,(this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeBillboards, (this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeParticleSystems, (this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeSounds, (this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeShapes, (this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeJoints, (this)));
}

//----------------------------------------------------------------------

iEdWindow* cModelEditor::CreatePreferencesDialog()
{
	return hplNew(cEdWindowModelEditorOptions,(this));
}

//----------------------------------------------------------------------

iEdMenu* cModelEditor::CreateMenu()
{
	return hplNew(cModelEditorMenu, (mpWindowHandler->GetBackground()));
}

//----------------------------------------------------------------------

iEdWorld* cModelEditor::CreateWorld()
{
	return hplNew(cHplEntity, (this));
}

//----------------------------------------------------------------------

void cModelEditor::CreateAppSpecificLayout()
{
	float fBaseY = 22;

	mpWindowHandler->AddBackground(hplNew(cGenericBG,(this)));
	const cLayoutBlock* pBGBlock = mpWindowHandler->GetLayoutBlock(eLayoutBlock_BG);

	///////////////////////////////////////////////////////////////////////
	// Layout blocks definition
	const cLayoutBlock* pSidebarBlock = mpWindowHandler->AddLayoutBlock(eLayoutBlock_Sidebar, 
																		false, false, 
																		cVector3f(0,fBaseY,1),
																		false, false, 
																		cVector2f(32,0));

	const cLayoutBlock* pEditModeBlock = mpWindowHandler->AddLayoutBlock(eLayoutBlock_EditMode,
																		false, false,
																		cVector3f(pBGBlock->mvSize.x-200, fBaseY, 1),
																		false, false,
																		cVector2f(200,-1));

	const cLayoutBlock* pLowerBlock = mpWindowHandler->AddLayoutBlock(eLayoutBlock_LowerToolbar,
																		false, false,
																		cVector3f(pSidebarBlock->mvPosition.x+pSidebarBlock->mvSize.x, pBGBlock->mvSize.y-50, 1),
																		false, false,
																		cVector2f(pEditModeBlock->mvPosition.x-pSidebarBlock->mvSize.x, -1));
																	
	const cLayoutBlock* pViewportBlock = mpWindowHandler->AddLayoutBlock(eLayoutBlock_ViewportArea,
																		false, false, 
																		pSidebarBlock->mvPosition + cVector3f(pSidebarBlock->mvSize.x,0,0),
																		false, false,
																		cVector2f(pEditModeBlock->mvPosition.x-pSidebarBlock->mvSize.x, pLowerBlock->mvPosition.y-pSidebarBlock->mvSize.y-fBaseY)); 

	mpWindowHandler->AddMainWindow(hplNew(cEdWindowEditModeSidebar,(this)), eLayoutBlock_Sidebar);
	mpWindowHandler->AddMainWindow(hplNew(cEdLowerToolbarModel,(this)), eLayoutBlock_LowerToolbar);

	//////////////////////////////////////////////////////////////////////
	// Add 4 viewport windows
	{
		const cVector3f& vBasePos = pViewportBlock->mvPosition;
		cVector2f vNormalSize = pViewportBlock->mvSize*0.5f;

		cVector2f vNormalPositions[4] = 
		{
			cVector2f(0,0),
			cVector2f(vNormalSize.x, 0),
			cVector2f(0, vNormalSize.y),
			cVector2f(vNormalSize.x, vNormalSize.y)
		};

		for(int i=0; i<4; ++i)
		{
			const cVector2f& vPos = vNormalPositions[i];
            AddViewport(hplNew(cEdWindowViewport,(this)), 
						vBasePos+vPos, vNormalSize-cVector2f(4,4),
						vPos, vNormalSize, 
						i);
		}
	}	
}

//----------------------------------------------------------------------
