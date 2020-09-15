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

#include "../Common/EdEditModeFogAreas.h"

#include "../Common/Editor.h"

#include "../Common/EdWindowFogAreas.h"

#include "../Common/EdObjFogArea.h"
#include "../Common/EdWorld.h"

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEdEditModeFogAreas::cEdEditModeFogAreas(iEditor* apEditor) : iEdEditModeScnObjCreator(apEditor, _W("FogAreas"))
{
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

bool cEdEditModeFogAreas::SetUpCreationData(iEdObjectData* apData)
{
	if(iEdEditModeScnObjCreator::SetUpCreationData(apData)==false)
		return false;

	cEdWindowFogAreas* pWin = static_cast<cEdWindowFogAreas*>(mpWindow);
	apData->SetColor(eFogAreaCol_Color, pWin->GetFogAreaColor());

	return true;
}

//-----------------------------------------------------------------

iEdWindow* cEdEditModeFogAreas::CreateWindow()
{
	return hplNew(cEdWindowFogAreas,(this));
}

//-----------------------------------------------------------------

iEdObjectType* cEdEditModeFogAreas::CreateType()
{
	return hplNew(cTypeFogArea,());
}

//-----------------------------------------------------------------

