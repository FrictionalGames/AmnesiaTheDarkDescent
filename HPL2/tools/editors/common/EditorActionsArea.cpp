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

#include "EditorActionsArea.h"

#include "EditorAction.h"

#include "EntityWrapperArea.h"

#include "EditorWorld.h"

//-------------------------------------------------------------
//-------------------------------------------------------------
////////////////////////////////////////////////////////////
// AREA SET VAR ACTION
////////////////////////////////////////////////////////////
//-------------------------------------------------------------
//-------------------------------------------------------------

cEditorActionAreaSetMesh::cEditorActionAreaSetMesh(iEditorWorld* apWorld, int alID, const tString& asX) : iEditorActionWorldModifier("Set Area Mesh", apWorld)
{
	mlID = alID;
	msNewMesh = asX;

	cEntityWrapperArea* pArea = (cEntityWrapperArea*)mpEditorWorld->GetEntity(mlID);
	msOldMesh = pArea->GetMeshFile();
}

void cEditorActionAreaSetMesh::DoModify()
{
	Apply(msNewMesh);
}

void cEditorActionAreaSetMesh::UndoModify()
{
	Apply(msOldMesh);
}

void cEditorActionAreaSetMesh::Apply(const tString& asX)
{
	cEntityWrapperArea* pArea = (cEntityWrapperArea*)mpEditorWorld->GetEntity(mlID);
	pArea->SetMeshFile(asX);
}

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------



//-------------------------------------------------------------
