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

#include "../LevelEditor/LevelEditor.h"
#include "../LevelEditor/HPLMap.h"

#include "../Common/DirectoryHandler.h"

#include "../Common/EdEditModeHandler.h"

#include "../Common/EdEditModeSelect.h"
#include "../Common/EdEditModeLights.h"
#include "../Common/EdEditModeBillboards.h"
#include "../Common/EdEditModeParticleSystems.h"
#include "../Common/EdEditModeSounds.h"
#include "../Common/EdEditModeStaticObjects.h"
#include "../Common/EdEditModePrimitives.h"
#include "../Common/EdEditModeEntities.h"
#include "../Common/EdEditModeDecals.h"
#include "../Common/EdEditModeAreas.h"
#include "../Common/EdEditModeFogAreas.h"
#include "../Common/EdEditModeCombine.h"

#include "../Common/GenericBG.h"
#include "../Common/EdWindowEditModeSidebar.h"
#include "../Common/EdWindowViewport.h"

#include "../LevelEditor/EdLowerToolbarLevel.h"
#include "../LevelEditor/LevelEditorMenu.h"

#include "../Common/EdWindowFind.h"
#include "../Common/EdWindowOptions.h"
#include "../LevelEditor/EdWindowGroup.h"
#include "../LevelEditor/EdWindowLevelSettings.h"

//----------------------------------------------------------------------

iEditor* iEditor::CreateInstance(const tWString& asCommandLine)
{
	return hplNew(cLevelEditor,());
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cLevelEditor::cLevelEditor() : iEditor("LevelEditor", "HPL Level Editor")
{
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cLevelEditor::ShowFindDialog()
{
	mpWindowFind->SetActive(true);
}

void cLevelEditor::ShowGroupDialog()
{
	ShowDialog(hplNew(cEdWindowGroup,(this)), true);
}

void cLevelEditor::ShowLevelSettingsDialog()
{
	ShowDialog(hplNew(cEdWindowLevelSettings,(this)), true);
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cLevelEditor::SetUpDirectories()
{
	mpDirHandler->AddLookUpDir(eDir_StaticObjects, _W("static_objects"), true);
	mpDirHandler->AddLookUpDir(eDir_Entities, _W("entities"), true);
	mpDirHandler->AddLookUpDir(eDir_Particles, _W("particles"), true);
	mpDirHandler->AddLookUpDir(eDir_Sounds, _W("sounds"), true);
}

//----------------------------------------------------------------------

void cLevelEditor::SetUpEditModes()
{
	mpEditModeHandler->AddMode(hplNew(cEdEditModeSelect,(this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeLights,(this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeBillboards, (this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeParticleSystems, (this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeSounds, (this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeStaticObjects,(this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModePrimitives,(this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeDecals,(this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeEntities,(this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeAreas,(this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeFogAreas,(this)));
	mpEditModeHandler->AddMode(hplNew(cEdEditModeCombine,(this)));
}

//----------------------------------------------------------------------

iEdWindow* cLevelEditor::CreatePreferencesDialog()
{
	return hplNew(iEdWindowOptions,(this));
}

//----------------------------------------------------------------------

iEdMenu* cLevelEditor::CreateMenu()
{
	return hplNew(cLevelEditorMenu,(mpWindowHandler->GetBackground()));
}

//----------------------------------------------------------------------

iEdWorld* cLevelEditor::CreateWorld()
{
	return hplNew(cHplMap, (this));
}

//----------------------------------------------------------------------

void cLevelEditor::CreateAppSpecificLayout()
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

	float fLowerToolbarHeight = 50;
	const cLayoutBlock* pLowerBlock = mpWindowHandler->AddLayoutBlock(eLayoutBlock_LowerToolbar,
																		false, false,
																		cVector3f(pSidebarBlock->mvPosition.x+pSidebarBlock->mvSize.x, pBGBlock->mvSize.y-fLowerToolbarHeight, 1),
																		false, false,
																		cVector2f(pEditModeBlock->mvPosition.x-pSidebarBlock->mvSize.x, -1));
																	
	const cLayoutBlock* pViewportBlock = mpWindowHandler->AddLayoutBlock(eLayoutBlock_ViewportArea,
																		false, false, 
																		pSidebarBlock->mvPosition + cVector3f(pSidebarBlock->mvSize.x,0,0),
																		false, false,
																		cVector2f(pEditModeBlock->mvPosition.x-pSidebarBlock->mvSize.x, pLowerBlock->mvPosition.y-pSidebarBlock->mvSize.y-fBaseY)); 

	mpWindowHandler->AddMainWindow(hplNew(cEdWindowEditModeSidebar,(this)), eLayoutBlock_Sidebar);
	mpWindowHandler->AddMainWindow(hplNew(cEdLowerToolbarLevel,(this)), eLayoutBlock_LowerToolbar);


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

	mpWindowFind = hplNew(cEdWindowFind,(this));
	mpWindowHandler->AddPopUp(mpWindowFind, false);
}

//----------------------------------------------------------------------
