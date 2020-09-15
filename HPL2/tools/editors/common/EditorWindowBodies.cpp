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

#include "EditorWindowBodies.h"
#include "EditorEditModeBodies.h"

#include "EditorGrid.h"

//-------------------------------------------------------------------

static tWString gsEditorShapeName[] =
{
	_W("Box"),
	_W("Cylinder"),
	_W("Capsule"),
	_W("Sphere")
};

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEditorWindowBodies::cEditorWindowBodies(cEditorEditModeBodies* apEditMode) : iEditModeObjectCreatorWindow(apEditMode)
{
}

cEditorWindowBodies::~cEditorWindowBodies()
{
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

//-------------------------------------------------------------------

//-------------------------------------------------------------------

void cEditorWindowBodies::Reset()
{
	ResetInputs();
}

//-------------------------------------------------------------------

eEditorBodyShape cEditorWindowBodies::GetBodyShapeType()
{
	return (eEditorBodyShape) mpShapeSelection->GetSelectedItem();
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEditorWindowBodies::OnInitLayout()
{
	//////////////////////
	// Set up layout
    mpBGFrame->SetSize(cVector2f(200,600));

	cVector3f vPos = cVector3f(5,5,1);
	cVector2f vSize;

	///////////////////////////////////////////////////
	// Shape Selection
	mpShapeSelection = mpSet->CreateWidgetComboBox(vPos,cVector2f(190,13),_W(""), mpBGFrame);
	for(int i=0; i< eEditorBodyShape_LastEnum; ++i)
	{
		mpShapeSelection->AddItem(gsEditorShapeName[i]);
	}
	mpShapeSelection->SetSelectedItem(0);
	
}

void cEditorWindowBodies::OnSetActive(bool abX)
{
	if(abX)
		Reset();
}

//-------------------------------------------------------------------

void cEditorWindowBodies::ResetInputs()
{
	
}


//-------------------------------------------------------------------


