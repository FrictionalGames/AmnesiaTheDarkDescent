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

#include "../Common/EdScnObjEditPane.h"

#include "../Common/Editor.h"
#include "../Common/EdWorld.h"
#include "../Common/EdGrid.h"
#include "../Common/EdScnObject.h"

#include "../Common/EdSelection.h"
#include "../Common/EdWindowSelect.h"
//#include "EditorActionSelection.h"
//#include "EditorActionEntity.h"
//#include "EditorHelper.h"

#include <algorithm>

//----------------------------------------------------------------------------

iEdEditPane::iEdEditPane(iEdObject* apObj) : iEdPane(apObj->GetWorld()->GetEditor(), _W("EditPane: ")+apObj->GetTypeName())
{
	mpObject = apObj;
}


void iEdEditPane::AddAction(iEdAction* apAction)
{
	mpEditor->AddAction(apAction);
}

//----------------------------------------------------------------------------
