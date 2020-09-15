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

#include "../ModelEditor/EdEditPaneBone.h"

//#include "EntityWrapperBone.h"

//#include "EdEditModeSelect.h"

//#include "EditorActionEntity.h"
//#include "EditorHelper.h"

//#include "EditorInput.h"

//#include <algorithm>

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneBone::cEdEditPaneBone(iEdObject* apObject) : iEdScnObjEditPane(apObject)
{
}

//----------------------------------------------------------------------------

cEdEditPaneBone::~cEdEditPaneBone()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneBone::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabAttachments = mpTabs->AddTab(_W("Attachments"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	mpInpName->SetPosition(cVector3f(5,5,0.1f));

    ////////////////////////////////////////
	// Attachment stuff
	AddPropertySetAttachments(mpTabAttachments);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
