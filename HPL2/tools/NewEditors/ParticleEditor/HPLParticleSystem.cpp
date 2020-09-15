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

#include "../ParticleEditor/HPLParticleSystem.h"
#include "../Common/Editor.h"
#include "../Common/EdResource.h"

#include "../Common/EdCamera.h"
#include "../Common/EdEditModeHandler.h"

#include "../ParticleEditor/EdEditModeParticleEmitters.h"
#include "../ParticleEditor/EdObjParticleEmitter.h"

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// HPL PS ROOT - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEdPSRoot::cEdPSRoot() : iEdObjectType(_W("Root"), "")
{
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// HPL PS ROOT - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEdObjectData* cEdPSRoot::CreateTypeSpecificData()
{
	return NULL;
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// HPL PS SESSION DATA - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cPESessionData::cPESessionData(iEdWorld* apWorld) : iEdSessionData(apWorld)
{
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// HPL PS SESSION DATA - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

bool cPESessionData::Load(cXmlElement* apElem)
{
	cHplParticleSystem* pPS = static_cast<cHplParticleSystem*>(mpWorld);

	iEdViewport* pViewport = pPS->GetEditor()->GetFocusedViewport();
	cEdCamera* pCam = pViewport->GetCamera();

	pCam->SetCameraPosition(apElem->GetAttributeVector3f("CameraPos", 10));
	pCam->SetTargetPosition(apElem->GetAttributeVector3f("TargetPos", 0));

	pPS->SetShowFloor(apElem->GetAttributeBool("ShowFloor"));
	pPS->SetShowWalls(apElem->GetAttributeBool("ShowWalls"));

	pCam->UpdateEngineCamera();

	return true;
}

bool cPESessionData::Save(cXmlElement* apElem)
{
	cHplParticleSystem* pPS = static_cast<cHplParticleSystem*>(mpWorld);

	iEdViewport* pViewport = mpWorld->GetEditor()->GetFocusedViewport();
	cEdCamera* pCam = pViewport->GetCamera();

	apElem->SetAttributeVector3f("CameraPos", pCam->GetCameraPosition());
	apElem->SetAttributeVector3f("TargetPos", pCam->GetTargetPosition());

	apElem->SetAttributeBool("ShowFloor", pPS->GetShowFloor());
	apElem->SetAttributeBool("ShowWalls", pPS->GetShowWalls());

	return true;
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// HPL PS - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cHplParticleSystem::cHplParticleSystem(iEditor* apEditor) : iEdWorld(apEditor, _W("HPL Particle System"), _W("ps"), "ParticleSystem", hplNew(cEdPSRoot, ()))
{
	mpPreviewPSData = NULL;
	mpPreviewPS = NULL;

	mbLoading = false;

	mbShowFloor = false;
	mbShowWalls = false;

	mfPreviewPSPollTimer = 1;

	AddBGColor(_W("Gray"), cColor(0.3f, 1));
	AddBGColor(_W("Black"), cColor(0, 1));
	AddBGColor(_W("White"), cColor(1));
}

cHplParticleSystem::~cHplParticleSystem()
{
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// HPL PS - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cHplParticleSystem::SetShowFloor(bool abX)
{
	if(mbShowFloor==abX)
		return;

	mbShowFloor = abX;
	mvWalls[0]->SetVisible(abX);

	mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldParamsChange, NULL);
}

//-------------------------------------------------------------------

void cHplParticleSystem::SetShowWalls(bool abX)
{
	if(mbShowWalls==abX)
		return;

	mbShowWalls = abX;
	for(int i=0;i<3; ++i)
		mvWalls[i+1]->SetVisible(abX);

	mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldParamsChange, NULL);
}

//-------------------------------------------------------------------

void cHplParticleSystem::AddBGColor(const tWString& asName, const cColor& asColor)
{
	mvBGColorNames.push_back(asName);
	mvBGColors.push_back(asColor);
}

void cHplParticleSystem::SetSelectableBGColor(int alIdx)
{
	SetBGColor(GetSelectableBGColor(alIdx));
}

//-------------------------------------------------------------------

bool cHplParticleSystem::LoadFromFile(const tWString& asFile)
{
	bool bRet;
	mbLoading = true;
	{
		bRet = iEdWorld::LoadFromFile(asFile);
	}
	mbLoading = false;

	return bRet;
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// HPL PS - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEdSessionData* cHplParticleSystem::CreateSessionData()
{
	return hplNew(cPESessionData,(this));
}

//-------------------------------------------------------------------

void cHplParticleSystem::OnInit()
{
	iEdWorld::OnInit();

	cEngine* pEng = mpEditor->GetEngine();

	mpPreviewPSData = pEng->GetResources()->GetLowLevel()->CreateXmlDocument(msXmlValue);
	
	cPhysics* pPhysics = pEng->GetPhysics();
	pPhysics->LoadSurfaceData("materials.cfg");

	mpPhysicsWorld = pPhysics->CreateWorld(true);
	mpPhysicsWorld->SetAccuracyLevel(ePhysicsAccuracy_Medium);
	mpPhysicsWorld->SetWorldSize(-300,300);
	mpPhysicsWorld->SetMaxTimeStep(1.0f / 60.0f);

	GetEngWorld()->SetPhysicsWorld(mpPhysicsWorld);

	cMeshManager* pMeshMgr = pEng->GetResources()->GetMeshManager();
	////////////////////////////////
	// Create meshes
	cMesh *pMesh =NULL;
	cMeshEntity *pBox = NULL;
			
	/////////////////
	// Create floor and walls
	pMesh = pMeshMgr->CreateMesh("editor_rect.dae");
	if(pMesh==NULL)	FatalError("Missing file");

	// Floor
	cMeshEntity* pWall = mpWorld->CreateMeshEntity("Floor",pMesh,true);
	pWall->SetMatrix(cMath::MatrixScale(6));
	pWall->SetPosition(cVector3f(0,-0.2f,0));
	pWall->SetVisible(mbShowFloor);

	cMatrixf mtxOffset = cMath::MatrixTranslate(cVector3f(0,-0.05f,0));
	iCollideShape *pShape = mpPhysicsWorld->CreateBoxShape(cVector3f(12,0.1f, 12),&mtxOffset);
	iPhysicsBody *pBody = mpPhysicsWorld->CreateBody("Floor_body", pShape);

	mvWalls.push_back(pWall);
	mvWallBodies.push_back(pBody);

	// Walls
	for(int i=0; i<3; ++i)
	{
		tString sName = "Wall" + cString::ToString(i+1);

		pMesh = pMeshMgr->CreateMesh("editor_rect.dae");
		pWall = mpWorld->CreateMeshEntity(sName,pMesh,true);
		
		cVector3f vPos(0,6.0f-0.2f,0);
		cMatrixf mtxTrans = cMath::MatrixScale(6);
		mtxTrans = cMath::MatrixMul(cMath::MatrixRotateX(kPi2f),mtxTrans);
		
		if(i==0)
		{
			vPos.x -= 6;
			mtxTrans = cMath::MatrixMul(cMath::MatrixRotateY(kPi2f),mtxTrans);
		}
		else if(i==1)
		{
			vPos.x += 6;
			mtxTrans = cMath::MatrixMul(cMath::MatrixRotateY(-kPi2f),mtxTrans);
		}
		else if(i==2)
		{
			vPos.z -= 6;
		}
		
        
		mtxTrans.SetTranslation(vPos);
		pWall->SetMatrix(mtxTrans);
		pWall->SetVisible(mbShowWalls);

		pShape = mpPhysicsWorld->CreateBoxShape(cVector3f(12,0.1f, 12),&mtxOffset);
		pBody = mpPhysicsWorld->CreateBody(sName+"_body", pShape);

		pBody->SetMatrix(mtxTrans);

		mvWalls.push_back(pWall);
		mvWallBodies.push_back(pBody);
	}
	
	/////////////////////////////////
	// Compile world
	mpWorld->Compile(false);
}

//-------------------------------------------------------------------

void cHplParticleSystem::OnReset()
{
	iEdWorld::OnReset();

	if(mpPreviewPS)
	{
		mpWorld->DestroyParticleSystem(mpPreviewPS);
		mpPreviewPS = NULL;
	}

	if(mbLoading==false)
	{
		cEdEditModeParticleEmitters* pMode = static_cast<cEdEditModeParticleEmitters*>(mpEditor->GetEditModeHandler()->GetMode(_W("Particle Emitters")));
		pMode->SetSelectedEmitter(NULL);

		iEdObjectData* pData = pMode->CreateObjectData();
		iEdObject* pObj = CreateObjFromData(pData);
		pMode->SetSelectedEmitter(pObj);
	}

	SetSelectableBGColor(0);
	SetShowFloor(false);
	SetShowWalls(false);
}

//-------------------------------------------------------------------

void cHplParticleSystem::OnUpdate()
{
	iEdWorld::OnUpdate();

	mfPreviewPSPollTimer -= mpEditor->GetTimeStep();
	if(mfPreviewPSPollTimer<0)
	{
		mfPreviewPSPollTimer = 1.0f;
		if(mbPreviewPSUpdated || 
			mpPreviewPS!=NULL && mpWorld->GetParticleSystem("PreviewPS")==NULL)
		{
			mbPreviewPSUpdated = false;
   			mpPreviewPS = mpWorld->CreateParticleSystem("PreviewPS", "PreviewPSData", mpPreviewPSData, 1);
		}
	}
}

//-------------------------------------------------------------------

void cHplParticleSystem::OnWorldAddRemObject()
{
	UpdatePreviewPS();
}

//-------------------------------------------------------------------

void cHplParticleSystem::OnWorldObjModify()
{
	UpdatePreviewPS();
}

//-------------------------------------------------------------------

bool cHplParticleSystem::LoadContentsFromXmlDoc(iXmlDocument* apDoc)
{
	cXmlNodeListIterator it = apDoc->GetChildIterator();
	while(it.HasNext())
	{
		cXmlElement* pXmlEmitter = it.Next()->ToElement();
		apDoc->GetFirstElement("Emitter");

		iEdObject* pObj = CreateObjFromXmlElement(pXmlEmitter);
	}

	return true;
}

//-------------------------------------------------------------------

bool cHplParticleSystem::SaveContentsToXmlDoc(iXmlDocument* apDoc)
{
	if(CheckDataIsValid()==false)
		return false;

	if(iEdWorld::SaveContentsToXmlDoc(apDoc)==false)
		return false;

	tEdObjectMapIt it = mmapObjects.begin();
	for(;it!=mmapObjects.end(); ++it)
	{
		iEdObject* pObj = it->second;
		pObj->Save(apDoc);
	}

	return true;
}

//-------------------------------------------------------------------

void cHplParticleSystem::UpdatePreviewPS()
{
	///////////////////////////////////////////////////////////
	// TODO: make it so that only the PS data is updated?
	mpPreviewPSData->DestroyChildren();
	if(SaveContentsToXmlDoc(mpPreviewPSData)==false)
		return;

	mbPreviewPSUpdated = true;

	///////////////////////////////////////////////////////////
	// Check if particle actually exists and destroy if so
	if(mpPreviewPS!=NULL && mpPreviewPS==mpWorld->GetParticleSystem("PreviewPS")) 
	{
		mpWorld->DestroyParticleSystem(mpPreviewPS);
		mpPreviewPS = NULL;
	}
}


//-------------------------------------------------------------------------

bool cHplParticleSystem::CheckDataIsValid()
{
	if(mmapObjects.empty())
		return false;

	bool bRet = true;
	tEdObjectMapIt it = mmapObjects.begin();
	for(;it!=mmapObjects.end();++it)
	{
		cEdObjParticleEmitter* pEmitter = static_cast<cEdObjParticleEmitter*>(it->second);
		if(cEdLoader::LoadMaterial(pEmitter->GetMaterial())==false)
		{
			bRet = false;
			break;
		}
	}

	return bRet;
}

//-------------------------------------------------------------------------
