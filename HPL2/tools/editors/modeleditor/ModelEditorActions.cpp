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

#include "ModelEditorActions.h"

#include "ModelEditor.h"
#include "ModelEditorWorld.h"

#include "../common/EntityWrapperFactory.h"
#include "../common/EntityWrapperSubMesh.h"
#include "../common/EntityWrapperBone.h"
#include "../common/EditorSelection.h"

//---------------------------------------------------------------------------------

cModelEditorActionMeshLoad::cModelEditorActionMeshLoad(cModelEditorWorld* apEditorWorld, const tString& asFilename) : iEditorActionWorldModifier("Load Mesh", apEditorWorld)
{
	cModelEditorWorld* pWorld = (cModelEditorWorld*)mpEditorWorld;

	msOldMeshFilename = pWorld->GetSubMeshType()->GetMeshFilename();
	mlstOldSubMeshIDs = pWorld->GetSubMeshType()->GetSubMeshIDs();// ((cModelEditorWorld*)mpEditorWorld)->GetSubMeshIDs();
	mlstOldBoneIDs = pWorld->GetBoneType()->GetBoneIDs(); //((cModelEditorWorld*)mpEditorWorld)->GetBoneIDs();

	msNewMeshFilename = asFilename;

	cMeshManager* pManager = mpEditorWorld->GetEditor()->GetEngine()->GetResources()->GetMeshManager();
	cMesh* pMesh = pManager->CreateMesh(asFilename);

	if(pMesh)
	{
		for(int i=0;i<pMesh->GetSubMeshNum();++i)
			mlstSubMeshIDs.push_back(mpEditorWorld->GetFreeID());

		cSkeleton* pSkeleton = pMesh->GetSkeleton();
		if(pSkeleton)
		{
			for(int i=0;i<pSkeleton->GetBoneNum();++i)
				mlstBoneIDs.push_back(mpEditorWorld->GetFreeID());
		}
	}

	pManager->Destroy(pMesh);
}

//---------------------------------------------------------------------------------

void cModelEditorActionMeshLoad::DoModify()
{
	mpEditorWorld->GetEditor()->GetSelection()->ClearEntities();
	tEntityDataVec temp;
	((cModelEditorWorld*)mpEditorWorld)->GetSubMeshType()->SetMesh(msNewMeshFilename, true, 
																	temp, mlstSubMeshIDs, 
																	temp, mlstBoneIDs);
}

//---------------------------------------------------------------------------------

void cModelEditorActionMeshLoad::UndoModify()
{
	cEditorSelection* pSelection = mpEditorWorld->GetEditor()->GetSelection();
	tEntityDataVec temp;
	((cModelEditorWorld*)mpEditorWorld)->GetSubMeshType()->SetMesh(msOldMeshFilename, true,
																	temp, mlstOldSubMeshIDs, 
																	temp, mlstOldBoneIDs);
	tIntListIt it = mlstOldSelectedIDs.begin();
	for(;it!=mlstOldSelectedIDs.end();++it)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(*it);

		pSelection->AddEntity(pEnt);
	}
}

//---------------------------------------------------------------------------------

