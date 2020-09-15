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

#include "../MaterialEditor/MaterialEditor.h"
#include "../Common/HPLMaterial.h"

#include "../Common/DirectoryHandler.h"
#include "../Common/GenericBG.h"
#include "../Common/EdWindowMaterialEditor.h"

//----------------------------------------------------------------------

iEditor* iEditor::CreateInstance(const tWString& asCommandLine)
{
	return hplNew(cMaterialEditor,(asCommandLine));
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cMaterialEditor::cMaterialEditor(const tWString& asCommandLine) : iEditor("MaterialEditor", "HPL Material Editor")
{
	msCommandLineFile = asCommandLine;
}

cMaterialEditor::~cMaterialEditor()
{
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cMaterialEditor::Update(float afTimeStep)
{
	mpWorld->DispatchMessage(eEdModuleMsg_Update, NULL);
	iEditor::Update(afTimeStep);
}

//----------------------------------------------------------------------

void cMaterialEditor::SetUpDirectories()
{
	mpDirHandler->AddLookUpDir(eDir_Textures, _W("textures"), true);
}

//----------------------------------------------------------------------

void cMaterialEditor::SetUpEditModes()
{
}

//----------------------------------------------------------------------

iEdMenu* cMaterialEditor::CreateMenu()
{
	return NULL;
}

//----------------------------------------------------------------------

iEdWorld* cMaterialEditor::CreateWorld()
{
	return hplNew(cHPLMaterial,(this));
}

//----------------------------------------------------------------------

void cMaterialEditor::CreateAppSpecificLayout()
{
	mpMainWindow = hplNew(cEdWindowMaterialEditor,(this, msCommandLineFile));
	
	mpWindowHandler->AddBackground(mpMainWindow);
}

//----------------------------------------------------------------------
