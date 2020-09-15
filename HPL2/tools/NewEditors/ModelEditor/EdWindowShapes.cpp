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

#include "../ModelEditor/EdWindowShapes.h"
#include "../ModelEditor/EdEditModeShapes.h"

#include "../ModelEditor/EdObjShape.h"

#include "../Common/EdGrid.h"

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

cEdWindowShapes::cEdWindowShapes(iEdEditMode* apMode) : iEdScnObjCreatorPane(apMode)
{
}

cEdWindowShapes::~cEdWindowShapes()
{
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------
//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEdWindowShapes::OnCreateLayout()
{
	iEdScnObjCreatorPane::OnCreateLayout();
	//////////////////////
	// Set up layout
	cVector3f vPos = cVector3f(15,15,0.1f);

	AddNameControls();
	mpInpBaseName->SetPosition(vPos);
	vPos.y += mpInpBaseName->GetSize().y + 10;

	///////////////////////////////////////////////////
	// Shape Selection
	AddTypeControls();
	mpInpSubType->SetPosition(vPos);
	//mpInpShapeType = CreateInputEnum(vPos, _W("Shape Type"), GetBG());
	//for(int i=0; i<eEdShape_LastEnum; ++i)
	//	mpInpShapeType->AddValue(gsEditorShapeName[i]);
}

//-------------------------------------------------------------------
