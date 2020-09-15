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

#include "EditorWindowSounds.h"
#include "EditorEditModeSounds.h"



//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEditorWindowSounds::cEditorWindowSounds(cEditorEditModeSounds* apEditMode) : iEditModeObjectCreatorWindow(apEditMode)
{
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void cEditorWindowSounds::OnInitLayout()
{
	mpBGFrame->SetSize(cVector2f(200,600));
	mpBGFrame->SetClipActive(false);

	cVector3f vPos = cVector3f(15,15,0.1f);

	mpInpSound = CreateInputFile(vPos, _W("Sound file"), "", mpBGFrame);
	mpInpSound->SetBrowserType(eEditorResourceType_Sound);
	mpInpSound->SetInitialPath(mpEditor->GetMainLookUpDir(eDir_Sounds));
}

//---------------------------------------------------------------------------
