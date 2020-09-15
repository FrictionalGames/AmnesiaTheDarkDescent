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

#include "../ModelEditor/ModelEditorActions.h"

#include "../ModelEditor/ModelEditor.h"
#include "../ModelEditor/HplEntity.h"

#include "../ModelEditor/EdObjSubMesh.h"
#include "../ModelEditor/EdObjBone.h"
#include "../Common/EdSelection.h"

//---------------------------------------------------------------------------------

cEdActionMeshLoad::cEdActionMeshLoad(iEdWorld* apWorld, const tString& asFilename) : iEdActionWorldModifier(_W("Load Mesh"), apWorld)
{
	cHplEntity* pWorld = static_cast<cHplEntity*>(mpWorld);

	//msOldMeshFilename = pWorld->GetSubMeshType()->GetMeshFilename();
	//mlstOldSubMeshIDs = pWorld->GetSubMeshType()->GetSubMeshIDs();// ((cModelEditorWorld*)mpWorld)->GetSubMeshIDs();
	//mlstOldBoneIDs = pWorld->GetBoneType()->GetBoneIDs(); //((cModelEditorWorld*)mpWorld)->GetBoneIDs();

	msNewMeshFilename = asFilename;
}

//---------------------------------------------------------------------------------

bool cEdActionMeshLoad::Init()
{
	cMeshManager* pMgr = this->mpWorld->GetEditor()->GetEngine()->GetResources()->GetMeshManager();
	cMesh* pMesh = pMgr->CreateMesh(msNewMeshFilename);
	
	if(pMesh)
	{
		for(int i=0;i<pMesh->GetSubMeshNum();++i)
			mlstSubMeshIDs.push_back(mpWorld->GetFreeID());

		cSkeleton* pSkeleton = pMesh->GetSkeleton();
		if(pSkeleton)
		{
			for(int i=0;i<pSkeleton->GetBoneNum();++i)
				mlstBoneIDs.push_back(mpWorld->GetFreeID());
		}
		pMgr->Destroy(pMesh);

		return true;
	}

	return false;
}

//---------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------

void cEdActionMeshLoad::DoModify()
{
	/*
	mpWorld->GetEditor()->GetSelection()->ClearEntities();
	tEntityDataVec temp;
	((cModelEditorWorld*)mpWorld)->GetSubMeshType()->SetMesh(msNewMeshFilename, true, 
																	temp, mlstSubMeshIDs, 
																	temp, mlstBoneIDs);
																	*/
}

//---------------------------------------------------------------------------------

void cEdActionMeshLoad::UndoModify()
{
	/*
	cEditorSelection* pSelection = mpWorld->GetEditor()->GetSelection();
	tEntityDataVec temp;
	*/
	/*
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpWorld);
	pEnt->GetSubMeshType()->SetMesh(msOldMeshFilename, true,
									temp, mlstOldSubMeshIDs, 
									temp, mlstOldBoneIDs);
	tIntListIt it = mlstOldSelectedIDs.begin();
	for(;it!=mlstOldSelectedIDs.end();++it)
	{
		iEntityWrapper* pEnt = mpWorld->GetEntity(*it);

		pSelection->AddEntity(pEnt);
	}
	*/
}

//---------------------------------------------------------------------------------

