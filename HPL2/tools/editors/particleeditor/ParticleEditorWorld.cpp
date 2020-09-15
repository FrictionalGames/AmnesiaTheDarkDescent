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

#include "ParticleEditorWorld.h"
#include "EntityWrapperParticleEmitter.h"

#include "../common/EditorBaseClasses.h"
#include "../common/EditorHelper.h"


//-------------------------------------------------------------------------

cParticleEditorWorld::cParticleEditorWorld(iEditorBase* apEditor) : iEditorWorld(apEditor, "ParticleSystem")
{
	mpTestPS = NULL;
	mpTestData = mpEditor->GetEngine()->GetResources()->GetLowLevel()->CreateXmlDocument();

	mbShowFloor = false;
	mbShowWalls = false;
	mbEmittersUpdated = false;
	mbPSDataUpdated = false;

	cPhysics* pPhysics = mpEditor->GetEngine()->GetPhysics();
	cMeshManager* pManager = mpEditor->GetEngine()->GetResources()->GetMeshManager();
	pPhysics->LoadSurfaceData("materials.cfg");
    
	mpPhysicsWorld = pPhysics->CreateWorld(true);
	mpPhysicsWorld->SetAccuracyLevel(ePhysicsAccuracy_Medium);
	mpPhysicsWorld->SetWorldSize(-300,300);
	mpPhysicsWorld->SetMaxTimeStep(1.0f / 60.0f);

	mpWorld->SetPhysicsWorld(mpPhysicsWorld);

	////////////////////////////////
	// Create meshes
	cMesh *pMesh =NULL;
	cMeshEntity *pBox = NULL;
			
	/////////////////
	//Floor
	mvWalls.resize(4);
	
	pMesh = pManager->CreateMesh("editor_rect.dae");
	mvWalls[0] = mpWorld->CreateMeshEntity("Floor",pMesh,true);
	mvWalls[0]->SetMatrix(cMath::MatrixScale(6));
	mvWalls[0]->SetPosition(cVector3f(0,-0.2f,0));
	mvWalls[0]->SetVisible(mbShowFloor);

	cMatrixf mtxOffset = cMath::MatrixTranslate(cVector3f(0,-0.05f,0));
	iCollideShape *pShape = mpPhysicsWorld->CreateBoxShape(cVector3f(12,0.1f, 12),&mtxOffset);
	iPhysicsBody *pBody = mpPhysicsWorld->CreateBody("Floor", pShape);

	for(int i=0; i<3; ++i)
	{
		pMesh = pManager->CreateMesh("editor_rect.dae");
		cMeshEntity *pWall = mpWorld->CreateMeshEntity("Wall",pMesh,true);
		
		cVector3f vPos(0,6.0f-0.2f,0);
		cMatrixf mtxTrans = cMath::MatrixScale(6);
		mtxTrans = cMath::MatrixMul(cMath::MatrixRotateX(kPi2f),mtxTrans);
		
		if(i==0)
		{
			vPos.x -= 6;
			mtxTrans = cMath::MatrixMul(cMath::MatrixRotateY(kPi2f),mtxTrans);
		}
		if(i==1)
		{
			vPos.x += 6;
			mtxTrans = cMath::MatrixMul(cMath::MatrixRotateY(-kPi2f),mtxTrans);
		}
		if(i==2)
		{
			vPos.z -= 6;
		}
		
        
		mtxTrans.SetTranslation(vPos);
		pWall->SetMatrix(mtxTrans);
		pWall->SetVisible(mbShowWalls);
		mvWalls[i+1] = pWall;

		iCollideShape *pShape = mpPhysicsWorld->CreateBoxShape(cVector3f(12,0.1f, 12),&mtxOffset);
		iPhysicsBody *pBody = mpPhysicsWorld->CreateBody("Floor", pShape);

		pBody->SetMatrix(mtxTrans);
	}
	
	/////////////////////////////////
	// Compile world
	mpWorld->Compile(false);

	Reset();
}

//-------------------------------------------------------------------------

cParticleEditorWorld::~cParticleEditorWorld()
{
	mpEditor->GetEngine()->GetResources()->DestroyXmlDocument(mpTestData);

}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

bool cParticleEditorWorld::Load(iXmlDocument* apDoc)
{
	if(iEditorWorld::Load(apDoc)==false)
		return false;

	cXmlNodeListIterator it = apDoc->GetChildIterator();
	while(it.HasNext())
	{
		iEntityWrapper* pEnt = NULL;
		cXmlElement* pXmlEntity = it.Next()->ToElement();
		if(pXmlEntity->GetValue()!="ParticleEmitter")
			continue;

		iEntityWrapperData* pData = mlstEntityTypes.back()->CreateData();
		pData->Load(pXmlEntity);
		pData->SetID(GetFreeID());

		pEnt = pData->CreateEntity();

		AddObject(pEnt);

		pEnt->OnPostDeployAll(true);

		//hplDelete(pData);
	}
	
	mbEmittersUpdated=true;

	UpdateParticleSystem();

	return true;

}

/*
void cParticleEditorWorld::LoadWorldObjects(cXmlElement* apWorldObjectsElement)
{
	//////////////////////////////////////////////
	// Load map contents
    if(apWorldObjectsElement->GetValue()!="ParticleSystem")
		return;

	cXmlNodeListIterator it = apWorldObjectsElement->GetChildIterator();
	while(it.HasNext())
	{
		iEntityWrapper* pEnt = NULL;
		cXmlElement* pXmlEntity = it.Next()->ToElement();
		if(pXmlEntity->GetValue()!="ParticleEmitter")
			continue;

		//iEntityWrapperData* pData = cEntityWrapperParticleEmitter::CreateData();
		//pData->Load(pXmlEntity);
		//pData->mlID = GetFreeID();

		//pEnt = pData->CreateEntity();

		//AddObject(pEnt);

		//hplDelete(pData);
	}
	
	mbEmittersUpdated=true;

	UpdateParticleSystem();
}
*/
//-------------------------------------------------------------------------

bool cParticleEditorWorld::Save(iXmlDocument* apXmlDoc)
{
	if(CheckDataIsValid()==false)
		return false;

	if(iEditorWorld::Save(apXmlDoc)==false)
		return false;

	tEntityWrapperMapIt it = mmapEntities.begin();
	for(;it!=mmapEntities.end();++it)
	{
		iEntityWrapper* pEnt = it->second;
		pEnt->Save(apXmlDoc);
	}

	mlLastSavedModification = mlNumModifications;

	return true;
}

//-------------------------------------------------------------------------

void cParticleEditorWorld::Reset()
{
	iEditorWorld::Reset();

	SetBGColor(cColor(0,1));
	SetShowFloor(false);
	SetShowWalls(false);

	UpdateParticleSystem();
	mbEmittersUpdated = false;
}

//-------------------------------------------------------------------------

iEntityWrapper* cParticleEditorWorld::AddEmitter(iEntityWrapperData* apData)
{
	iEntityWrapperData* pData = (cEntityWrapperDataParticleEmitter*)apData;

	tString sName;

	if(pData==NULL)
	{
		pData = this->mlstEntityTypes.back()->CreateData();;
		sName = GenerateName("Emitter");
	}
	else
		sName = "Copy of " + pData->GetName();

	pData->SetID(GetFreeID());
	pData->SetName(sName);

	iEntityWrapper* pEmitter = pData->CreateEntity();
	if(AddObject(pEmitter))
		mbEmittersUpdated = true;

	pEmitter->OnPostDeployAll(false);

	UpdateParticleSystem();

	return pEmitter;
//	return NULL;
}

//-------------------------------------------------------------------------

void cParticleEditorWorld::RemoveEmitter(iEntityWrapper* apEmitter)
{
	if(apEmitter==NULL)
		return;

	DestroyEntityWrapper(apEmitter);
	mbEmittersUpdated = true;

	UpdateParticleSystem();
}

//-------------------------------------------------------------------------

void cParticleEditorWorld::UpdateParticleSystem()
{
	///////////////////////////////////////////////////////////
	// TODO: make it so that only the PS data is updated?
	mpTestData->DestroyChildren();
	Save(mpTestData);

	///////////////////////////////////////////////////////////
	// Check if particle actually exists and destroy if so
	if(mpTestPS==mpWorld->GetParticleSystem("TestPS")) mpWorld->DestroyParticleSystem(mpTestPS);
	mpTestPS = NULL;
	mbPSDataUpdated = true;
}

//-------------------------------------------------------------------------

void cParticleEditorWorld::SetShowFloor(bool abX)
{
	mbShowFloor = abX;
	mvWalls[0]->SetVisible(abX);
}

//-------------------------------------------------------------------------

void cParticleEditorWorld::SetShowWalls(bool abX)
{
	mbShowWalls = abX;
	for(size_t i=1;i<mvWalls.size();++i)
	{
		mvWalls[i]->SetVisible(abX);
	}
}

//-------------------------------------------------------------------------

void cParticleEditorWorld::SetBGColor(const cColor& aCol)
{
	mpWorld->SetSkyBoxColor(aCol);
	mpWorld->SetSkyBoxActive(true);
}

//-------------------------------------------------------------------------

cColor cParticleEditorWorld::GetBGColor()
{
	return mpWorld->GetSkyBoxColor();
}

//-------------------------------------------------------------------------

bool cParticleEditorWorld::CheckDataIsValid()
{
	if(mmapEntities.empty())
		return false;

	bool bRet = true;
	tEntityWrapperMapIt it = mmapEntities.begin();
	for(;it!=mmapEntities.end();++it)
	{
		cEntityWrapperParticleEmitter* pEmitter = (cEntityWrapperParticleEmitter*)it->second;
		if(cEditorHelper::LoadResourceFile(eEditorResourceType_Material, pEmitter->GetMaterial())==false)
		{
			bRet = false;
			break;
		}
	}

	return bRet;
}

//-------------------------------------------------------------------------

void cParticleEditorWorld::OnEditorUpdate()
{
	if(mbPSDataUpdated || mpTestPS!=NULL && mpWorld->GetParticleSystem("TestPS")==NULL)
	{
		mbPSDataUpdated = false;
   		mpTestPS = mpWorld->CreateParticleSystem("TestPS", "TestPSData", mpTestData, 1);
	}
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
