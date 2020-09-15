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

#include "EditorActionArea.h"
#include "EntityWrapperArea.h"

cEditorActionAreaSetType::cEditorActionAreaSetType(iEditorWorld* apWorld, int alID, const tString& asType) : iEditorActionWorldModifier("Set area type", apWorld)
{
	mlID = alID;
	msNewType = asType;
	
	cEntityWrapperArea* pArea = (cEntityWrapperArea*) apWorld->GetEntity(alID);
	msOldType = pArea->GetAreaType();
	pArea->GetClass()->SaveValuesToMap(mmapOldValues);
}

void cEditorActionAreaSetType::DoModify()
{
	cEntityWrapperArea* pArea = (cEntityWrapperArea*) mpEditorWorld->GetEntity(mlID);
	pArea->SetAreaType(msNewType);
	pArea->GetClass()->LoadValuesFromMap(mmapOldValues);
}

void cEditorActionAreaSetType::UndoModify()
{
	cEntityWrapperArea* pArea = (cEntityWrapperArea*) mpEditorWorld->GetEntity(mlID);
	pArea->SetAreaType(msOldType);
	pArea->GetClass()->LoadValuesFromMap(mmapOldValues);
}

