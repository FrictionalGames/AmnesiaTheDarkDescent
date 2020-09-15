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

#include "EditorActionsSubMesh.h"

#include "EntityWrapperSubMesh.h"
#include "EntityWrapperBody.h"

#include "EditorBaseClasses.h"

#include "EditorWorld.h"

//-------------------------------------------------------------

///////////////////////////////////////////////////////////////
// ATTACH BODY
///////////////////////////////////////////////////////////////

//-------------------------------------------------------------

cEditorActionSubMeshAttachBody::cEditorActionSubMeshAttachBody(int alID, iEdWorld* apEditorWorld, int alBodyID) : iEdActionWorldModifier("Attach body", apEditorWorld)
{
	mlID = alID;

	mlNewBodyID = alBodyID;
	
	cEntityWrapperSubMesh* pSubMesh = (cEntityWrapperSubMesh*)mpWorld->GetEntity(mlID);
	iEntityWrapper* pBody = pSubMesh->GetParent();

	if(pBody)
		mlOldBodyID = pBody->GetID();
	else
		mlOldBodyID = -1;
}

void cEditorActionSubMeshAttachBody::Do()
{
	Apply(mlNewBodyID);

	iEdActionWorldModifier::Do();
}

void cEditorActionSubMeshAttachBody::Undo()
{
	Apply(mlOldBodyID);

	iEdActionWorldModifier::Undo();
}

void cEditorActionSubMeshAttachBody::Apply(int alBodyID)
{
	cEntityWrapperSubMesh* pSubMesh = (cEntityWrapperSubMesh*)mpWorld->GetEntity(mlID);
	
	cEntityWrapperBody* pBody = NULL;
	if(alBodyID!=-1)
		pBody = (cEntityWrapperBody*) mpWorld->GetEntity(alBodyID);

	pSubMesh->SetParent(pBody);
}

//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
