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

#include "../ModelEditor/EdHplEntityActions.h"

#include "../Common/Editor.h"
#include "../ModelEditor/HPLEntity.h"
#include "../Common/EdUserClassDefinition.h"

#include "../ModelEditor/EdObjSubMesh.h"
#include "../ModelEditor/EdObjBone.h"

cEdActionHplEntitySetClass::cEdActionHplEntitySetClass(cHplEntity* apEnt, cEdUserClass* apClass) : iEdActionWorldModifier(_W("Set Entity Class"), apEnt)
{
	mpClass = apClass;
	mpOldClass = NULL;
}

bool cEdActionHplEntitySetClass::Init()
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpWorld);
	if(pEnt==NULL)
	{
		Error("NULL cHplEntity passed to action %ls\n", GetName().c_str());
		return false;
	}
	if(mpClass==NULL)
	{
		Error("NULL class passed to action %ls\n", GetName().c_str());
		return false;
	}
	if(mpClass->GetNumChildren()!=0)
	{
		Error("Non leaf class passed to action %ls\n", GetName().c_str());
		return false;
	}

	cEdClassInstance* pOldClassInstance = pEnt->GetClassInstance();
	if(pOldClassInstance==NULL)
	{
		Error("NULL class instance found in cHplEntity\n");
		return false;
	}
	mpOldClass = static_cast<cEdUserClass*>(pOldClassInstance->GetClass());
	if(mpClass==mpOldClass)
	{
		Error("Trying to set same class\n");
		return false;
	}

	pOldClassInstance->SaveValuesToMap(mmapOldValues);

	return true;
}

void cEdActionHplEntitySetClass::DoModify()
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpWorld);
	pEnt->SetClass(mpClass);
}

void cEdActionHplEntitySetClass::UndoModify()
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpWorld);
	pEnt->SetClass(mpOldClass);
	pEnt->GetClassInstance()->LoadValuesFromMap(mmapOldValues);
}


cEdActionHplEntitySetVarValue::cEdActionHplEntitySetVarValue(cHplEntity* apEnt, const tWString& asVarName, const tWString& asValue) : iEdActionWorldModifier(_W("Set Entity Variable Value"), apEnt)
{
	msVarName = asVarName;
	msValue = asValue;
}

bool cEdActionHplEntitySetVarValue::Init()
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpWorld);
	if(pEnt==NULL)
	{
		Error("NULL cHplEntity passed to action %ls\n", GetName().c_str());
		return false;
	}
	cEdVarInstance* pVar = pEnt->GetClassInstance()->GetVarInstance(msVarName);
	if(pVar==NULL)
	{
		Error("Invalid variable name passed to action %ls\n", GetName().c_str());
		return false;
	}

	msOldValue = pVar->GetValue();
	if(msOldValue==msValue)
	{
		Error("Trying to set same value");
		return false;
	}

	return true;
}

void cEdActionHplEntitySetVarValue::DoModify()
{
	Apply(msValue);
}

void cEdActionHplEntitySetVarValue::UndoModify()
{
	Apply(msOldValue);
}

void cEdActionHplEntitySetVarValue::Apply(const tWString& asX)
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpWorld);
	pEnt->GetClassInstance()->SetVarValue(msVarName, asX);

}

//----------------------------------------------------------------------

cEdActionHplEntitySetAnimations::cEdActionHplEntitySetAnimations(cHplEntity* apEnt, const tAnimWrapperVec& avAnimations) : iEdActionWorldModifier(_W("Set Animations"), apEnt)
{
	mvOldAnims = apEnt->GetAnimations();
	mvNewAnims = avAnimations;
}

bool cEdActionHplEntitySetAnimations::Init()
{
	return true;
}

void cEdActionHplEntitySetAnimations::DoModify()
{
	Apply(mvNewAnims);
}

void cEdActionHplEntitySetAnimations::UndoModify()
{
	Apply(mvOldAnims);
}

void cEdActionHplEntitySetAnimations::Apply(const tAnimWrapperVec& avX)
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpWorld);

	pEnt->SetAnimations(avX);
}

//---------------------------------------------------------------------------------

cEdActionHplEntityImportMesh::cEdActionHplEntityImportMesh(iEdWorld* apWorld, 
														   const tString& asFilename) : iEdActionWorldModifier(_W("Import Mesh"), apWorld)
{	
	msNewMeshFilename = asFilename;
}

//---------------------------------------------------------------------------------

bool cEdActionHplEntityImportMesh::Init()
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpWorld);

	cTypeSubMesh* pTypeSubMesh = pEnt->GetTypeSubMesh();
	cTypeBone* pTypeBone = pEnt->GetTypeBone();

	msOldMeshFilename = pTypeSubMesh->GetMeshFilename();

	if(msOldMeshFilename==msNewMeshFilename)
		return false;

	mlstOldSubMeshIDs = pTypeSubMesh->GetObjectIDs();
	mlstOldBoneIDs = pTypeBone->GetObjectIDs();

	cMeshManager* pManager = mpWorld->GetEditor()->GetEngine()->GetResources()->GetMeshManager();
	cMesh* pMesh = NULL;
	
	if(msNewMeshFilename.empty()==false) 
		pMesh = pManager->CreateMesh(msNewMeshFilename);

	if(pMesh)
	{
		for(int i=0;i<pMesh->GetSubMeshNum();++i)
			mlstSubMeshIDs.push_back(pEnt->GetFreeID());

		cSkeleton* pSkeleton = pMesh->GetSkeleton();
		if(pSkeleton)
		{
			for(int i=0;i<pSkeleton->GetBoneNum();++i)
				mlstBoneIDs.push_back(pEnt->GetFreeID());
		}

		pManager->Destroy(pMesh);
	}

	return true;
}

//---------------------------------------------------------------------------------

void cEdActionHplEntityImportMesh::DoModify()
{
	Apply(msNewMeshFilename, mlstSubMeshIDs, mlstBoneIDs);
}

//---------------------------------------------------------------------------------

void cEdActionHplEntityImportMesh::UndoModify()
{
	Apply(msOldMeshFilename, mlstOldSubMeshIDs, mlstOldBoneIDs);
}

//---------------------------------------------------------------------------------

void cEdActionHplEntityImportMesh::Apply(const tString& asX, const tIntList& alstSubMeshIDs, const tIntList& alstBoneIDs)
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpWorld);
	cTypeSubMesh* pTypeSubMesh = pEnt->GetTypeSubMesh();

	tEdObjDataVec vDummyData;

	pTypeSubMesh->SetMesh(asX, true, 
							vDummyData, alstSubMeshIDs,
							vDummyData, alstBoneIDs);
}

//---------------------------------------------------------------------------------
