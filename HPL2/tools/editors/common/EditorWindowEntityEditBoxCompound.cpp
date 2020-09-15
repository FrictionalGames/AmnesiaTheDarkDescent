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

#include "EditorWindowEntityEditBoxCompound.h"

#include "EntityWrapperCompoundObject.h"

#include "EditorEditModeSelect.h"

#include "EditorActionEntity.h"
#include "EditorHelper.h"

#include "EditorInput.h"

#include <algorithm>

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxCompound::cEditorWindowEntityEditBoxCompound(cEditorEditModeSelect* apEditMode, cEntityWrapperCompoundObject* apObject) : cEditorWindowEntityEditBox(apEditMode,apObject)
{
	mpEntity = apObject;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxCompound::~cEditorWindowEntityEditBoxCompound()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxCompound::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	cVector3f vPos = cVector3f(10,10,0.1f);
	AddPropertyName(mpTabGeneral);
	
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y+5;

	if(mpEntity->CanTranslate())
	{
		AddPropertyPosition(mpTabGeneral);
		mpInpPosition->SetPosition(vPos);
		vPos.y += mpInpPosition->GetSize().y+5;
	}
	if(mpEntity->CanRotate())
	{
		AddPropertyRotation(mpTabGeneral);
		mpInpRotation->SetPosition(vPos);
		vPos.y += mpInpRotation->GetSize().y+5;
	}
	if(mpEntity->CanScale())
	{
		AddPropertyScale(mpTabGeneral);
		mpInpScale->SetPosition(vPos);
		vPos.y += mpInpScale->GetSize().y+5;
	}

    ////////////////////////////////////////
	// Properties in Tab 'Specific'
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxCompound::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxCompound::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxCompound,InputCallback);


//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxCompound::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput))
		return true;

	return true;
}

//----------------------------------------------------------------------------
