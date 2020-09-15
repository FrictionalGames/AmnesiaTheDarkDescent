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

#include "../Common/EdEditModeSelectTool.h"

#include "../Common/EdEditModeSelect.h"
#include "../Common/EdSelection.h"

#include "../Common/EdWindowViewport.h"

#include "../Common/EdGrid.h"

//----------------------------------------------------------------

iManipulatorComponent::iManipulatorComponent(iScnObjManipulator* apParent)
{
	mpParent = apParent;
}

//----------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// SELECT TOOL BASE
//////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

iScnObjManipulator::iScnObjManipulator(const tWString& asName)
{
	msName = asName;

	mpTargetObject = NULL;
	mpActiveComponent = NULL;
}

//----------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// SELECT TOOL BASE
//////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

void iScnObjManipulator::OnDraw(const cModuleDrawData& aData)
{
	aData.mpFunctions->SetDepthTest(false);
	if(mpTargetObject==NULL) return;
	for(size_t i=0; i<mvComponents.size(); ++i)
	{
		iManipulatorComponent* pComponent = mvComponents[i];
		pComponent->OnDraw(aData);
	}
	aData.mpFunctions->SetDepthTest(true);
}

void iScnObjManipulator::OnUpdate()
{
	mpActiveComponent = NULL;
	for(size_t i=0; i<mvComponents.size(); ++i)
	{
		iManipulatorComponent* pComp = mvComponents[i];
		pComp->OnUpdate();
	}
}

void iScnObjManipulator::SetActiveComponent(iManipulatorComponent* apComp)
{
	mpActiveComponent = apComp;
}

//----------------------------------------------------------------

void iScnObjManipulator::AddComponent(iManipulatorComponent* apComp)
{
	if(apComp==NULL) return;

	mvComponents.push_back(apComp);
}

//----------------------------------------------------------------
