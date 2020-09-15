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

#include "LuxEntity.h"

#include "LuxMap.h"
#include "LuxPlayer.h"
#include "LuxMapHelper.h"

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxEntity::iLuxEntity(const tString &asName, int alID, cLuxMap *apMap, eLuxEntityType aEntityType) : iLuxCollideCallbackContainer()
{
	msName = asName;
	mpMap = apMap;
	mEntityType = aEntityType;
	mlID = alID;
	mbActive = true;
	mbFullGameSave = false;

	msCallbackFunc = "";
	msConnectionStateChangeCallback = "";

	mfMaxFocusDistance = 2.0f;

	mbDestroyMe = false;
	mbIsSaved = true;

	mCustomFocusCrossHair = eLuxFocusCrosshair_Default;

	msInteractCallback = "";
	mbInteractCallbackRemove = true;

	msLookAtCallback = "";
	mbLookAtCallbackRemove = true;
	mfLookAtCount =0;
	mbIsLookedAt = false;

	mbInteractionDisabled = false;
}

//-----------------------------------------------------------------------

iLuxEntity::~iLuxEntity()
{
	for(size_t i=0; i<mvPreloadedMeshes.size();++i)
	{
		gpBase->mpEngine->GetResources()->GetMeshManager()->Destroy(mvPreloadedMeshes[i]);
	}

	tLuxCollideCallbackContainerListIt it = mlstCollideCallbackParents.begin();
	for(; it != mlstCollideCallbackParents.end(); ++it)
	{
		iLuxCollideCallbackContainer *pParent = *it;
		pParent->RemoveCollideCallbackInstantly(this);
	}

	STLDeleteAll(mvConnections);

	DestroyCollideCallbacks();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxEntity::UpdateLogic(float afTimeStep)
{	
	//////////////////////
	// Normal update
	UpdateCheckCollideCallback(afTimeStep);
	UpdatePlayerLookAt(afTimeStep);


	///////////////////////
	// Update implemented 
	OnUpdate(afTimeStep);
}

//-----------------------------------------------------------------------

cSoundEntity* iLuxEntity::PlaySound(const tString& asName, const tString& asFile, bool abRemoveWhenDone, bool abAttach)
{
	if(asFile == "") return NULL;

	cSoundEntity *pSound = mpMap->GetWorld()->CreateSoundEntity(msName + "_" + asName,asFile,abRemoveWhenDone);
	iEntity3D *pAttachEnt = GetAttachEntity();
	if(pSound && pAttachEnt)
	{
		if(abAttach)	pAttachEnt->AddChild(pSound);
		else			pSound->SetPosition(pAttachEnt->GetWorldPosition());
	}

	return pSound;
}

//-----------------------------------------------------------------------

void iLuxEntity::RunCallbackFunc(const tString& asType)
{
	if(msCallbackFunc=="")return;

	mpMap->RunScript(msCallbackFunc + "(\""+msName+"\", \""+asType+"\")" );
}

//-----------------------------------------------------------------------

void iLuxEntity::RunInteractCallbackFunc()
{
	if(msInteractCallback=="")return;
	
	mpMap->RunScript(msInteractCallback + "(\""+msName+"\")");
	
	if(mbInteractCallbackRemove) msInteractCallback = "";
}

//-----------------------------------------------------------------------

void iLuxEntity::SetActive(bool abX)
{
	if(mbActive == abX) return;

	mbActive = abX;

    OnSetActive(abX);
}

//-----------------------------------------------------------------------

void iLuxEntity::AddCollideCallbackParent(iLuxCollideCallbackContainer* apCallback)
{
	mlstCollideCallbackParents.push_back(apCallback);
}

void iLuxEntity::RemoveCollideCallbackParent(iLuxCollideCallbackContainer* apCallback)
{
	STLFindAndRemove(mlstCollideCallbackParents, apCallback);
}

//-----------------------------------------------------------------------

void iLuxEntity::SetPlayerInteractCallback(const tString &asCallbackFunc, bool abRemoveWhenInteracted)
{
	msInteractCallback = asCallbackFunc;
	mbInteractCallbackRemove = abRemoveWhenInteracted;
}

//-----------------------------------------------------------------------

void iLuxEntity::SetPlayerLookAtCallback(const tString &asCallbackFunc, bool abRemoveWhenLookedAt)
{
	msLookAtCallback = asCallbackFunc;
	mbLookAtCallbackRemove = abRemoveWhenLookedAt;
}

//-----------------------------------------------------------------------

void iLuxEntity::AddConnection(const tString& asName, iLuxEntity *apEntity, bool abInvertStateSent, int alStatesUsed, const tString &asCallbackFunc)
{
	cLuxEntityConnection *pConnection = hplNew(cLuxEntityConnection, (asName, apEntity,abInvertStateSent, alStatesUsed, asCallbackFunc));

	mvConnections.push_back(pConnection);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxEntity::UpdateCheckCollideCallback(float afTimeStep)
{
	CheckCollisionCallback(msName, mpMap);
}


//-----------------------------------------------------------------------

void iLuxEntity::UpdatePlayerLookAt(float afTimeStep)
{
	if(msLookAtCallback == "") return;

	if(mfLookAtCount > 0)
	{
		mfLookAtCount-=afTimeStep;
		return;
	}
	mfLookAtCount = 0.3f;

	//////////////////////////////////////
	// Iterate bodies and check frustum and then line of sight
	bool bLookingAt=false;
	cCamera *pCamera = gpBase->mpPlayer->GetCamera();
	cFrustum *pFrustum = pCamera->GetFrustum();
	for(int i = 0; i < GetBodyNum(); i++)
	{
		iPhysicsBody *pBody = GetBody(i);
		cBoundingVolume *pBV = pBody->GetBoundingVolume();

		/////////////////////////
		// Frustum check
		if(pFrustum->CollideBoundingVolume(pBV) == eCollision_Outside)
		{
			continue;
		}
		
		/////////////////////////
		// Set up line of sight check
		cVector3f vStart = pCamera->GetPosition();
		cVector3f vEnd = pBV->GetWorldCenter();
		
		cVector3f vDir = vEnd - vStart;
		float fSqrDist = vDir.SqrLength();
		
		if(fSqrDist > 50*50) continue;

		/////////////////////////
		// Check if center of screen is over object
		cVector3f vIntersection;
		cVector3f vCentreLineEnd = vStart + pCamera->GetForward() * 50;
		if(cMath::CheckAABBLineIntersection(pBV->GetMin(), pBV->GetMax(), vStart, vCentreLineEnd,&vIntersection, NULL)==false)
		{
			continue;
		}
		
		/////////////////////////
		// If close enough then it is visible
		float fSqrRadius = pBV->GetRadius()*pBV->GetRadius()+0.05f;
		if(fSqrDist < fSqrRadius)
		{
			bLookingAt = true;
			break;
		}

		/////////////////////////
		// Check line of sight
		vDir.Normalize();
		cVector3f vLineOfSightTestPos[5];
		float fHalfRadius = pBV->GetRadius() * 0.5f;

		vLineOfSightTestPos[0] = vEnd - vDir*pBV->GetRadius();
		vLineOfSightTestPos[1] = vLineOfSightTestPos[0] + pCamera->GetUp() * fHalfRadius;
		vLineOfSightTestPos[2] = vLineOfSightTestPos[0] - pCamera->GetUp() * fHalfRadius;
		vLineOfSightTestPos[3] = vLineOfSightTestPos[0] + pCamera->GetRight() * fHalfRadius;
		vLineOfSightTestPos[4] = vLineOfSightTestPos[0] - pCamera->GetRight() * fHalfRadius;
				
		for(int i=0; i<5; ++i)
		{
			if(gpBase->mpMapHelper->CheckLineOfSight(vStart, vLineOfSightTestPos[i], false))
			{
				bLookingAt = true;
				break;
			}
		}
		if(bLookingAt) break;
	}

	//////////////////////////////////////
	// Check if looking at
	if(bLookingAt && mbIsLookedAt == false)
	{
		tString sTempCallback = msLookAtCallback;
		if(mbLookAtCallbackRemove) msLookAtCallback = "";

		mpMap->RunScript(sTempCallback + "(\""+msName+"\", 1)" );
	}
	else if(bLookingAt==false && mbIsLookedAt)
	{
		mpMap->RunScript(msLookAtCallback + "(\""+msName+"\", -1)" );	
	}

	mbIsLookedAt = bLookingAt;
}

//-----------------------------------------------------------------------

void iLuxEntity::ConnectionStateChange(int alState)
{
	//////////////////////////////////
	// Callback
	if(msConnectionStateChangeCallback != "")
	{
		mpMap->RunScript(	msConnectionStateChangeCallback + 
							"(\""+msName+"\", "+ cString::ToString(alState) + ")");
	}

    //////////////////////////////////
	// Iterate connections
	for(size_t i=0; i< mvConnections.size(); ++i)
	{
		cLuxEntityConnection *pConn = mvConnections[i];
		
		//See if this state will send a message.
		if(pConn->GetStateUsed()!=0)
		{
			if(alState != pConn->GetStateUsed()) continue;
		}

		//Get state sent to child, invert if set so.
		int lState = pConn->GetInvertStateSent() ? -alState : alState;

		//Log("Send state '%s'%d to '%s'as %d\n", msName.c_str(), alState, pConn->GetEntity()->GetName().c_str(), lState);

        pConn->GetEntity()->OnConnectionStateChange(this,lState);

		if(pConn->GetCallbackFunc()!="")
		{
			//Syntax: ConnectionName,ParentEnt, ChildEnt, state
			tString sCommand = pConn->GetCallbackFunc() + "(\"" + pConn->GetName() + "\"," +
															"\"" + msName +  + "\"," +
															"\"" + pConn->GetEntity()->GetName() + "\"," +
															cString::ToString(lState) + ")";
			mpMap->RunScript(sCommand);		
		}
	}
}

//-----------------------------------------------------------------------

void iLuxEntity::PreloadEntityModel(const tString &asFile)
{
	cResources *pResources = gpBase->mpEngine->GetResources();

	tString sFileName = cString::SetFileExt(asFile,"ent");
	
	//////////////////////
	// Load XML document
	iXmlDocument *pEntityDoc = pResources->LoadXmlDocument(sFileName);
	if(pEntityDoc==NULL){
		Error("Could not load xml file '%s'\n", sFileName.c_str());
		return;
	}
	
	//////////////////////
	// Get Model File name
	cXmlElement *pModelDataElem = pEntityDoc->GetFirstElement("ModelData");
	cXmlElement *pMeshElem = pModelDataElem->GetFirstElement("Mesh");

	tString sModelFile = pMeshElem->GetAttributeString("Filename","");

	//////////////////////
	// Load Mesh
	cMesh *pMesh = pResources->GetMeshManager()->CreateMesh(sModelFile);
	if(pMesh)
		mvPreloadedMeshes.push_back(pMesh);
	else
		Error("Could not load mesh '%s' when preloading!\n",sModelFile.c_str());

	pResources->DestroyXmlDocument(pEntityDoc);
}

//-----------------------------------------------------------------------

bool iLuxEntity::CollidesWithPlayer()
{
	iPhysicsWorld *pPhysicsWorld = mpMap->GetPhysicsWorld();
	cCollideData collideData;
	collideData.SetMaxSize(1);

	iPhysicsBody *pPlayerBody = gpBase->mpPlayer->GetCharacterBody()->GetCurrentBody();

	for(int i=0; i<GetBodyNum(); ++i)
	{
		iPhysicsBody *pBody = GetBody(i);

		if(cMath::CheckBVIntersection(*pPlayerBody->GetBoundingVolume(), *pBody->GetBoundingVolume())==false)
		{
			continue;
		}
		
		if(pPhysicsWorld->CheckShapeCollision(pBody->GetShape(), pBody->GetLocalMatrix(), pPlayerBody->GetShape(), pPlayerBody->GetLocalMatrix(), collideData,1, false))
		{
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxEntityConnection_SaveData)
kSerializeVar(msName,eSerializeType_String)
kSerializeVar(mlEntityId,eSerializeType_Int32)
kSerializeVar(mbInvertStateSent,eSerializeType_Bool)
kSerializeVar(mlStatesUsed,eSerializeType_Int32)
kSerializeVar(msCallbackFunc,eSerializeType_String)
kEndSerialize()

void cLuxEntityConnection_SaveData::FromConnection(cLuxEntityConnection *apConnection)
{
	mlEntityId = apConnection->mpEntity->GetID();
	
	kCopyFromVar(apConnection, msName);
	kCopyFromVar(apConnection, mbInvertStateSent);
	kCopyFromVar(apConnection, mlStatesUsed);
	kCopyFromVar(apConnection, msCallbackFunc);
}

void cLuxEntityConnection_SaveData::ToConnection(cLuxEntityConnection *apConnection, cLuxMap *apMap)
{
	apConnection->mpEntity = apMap->GetEntityByID(mlEntityId);
	if(apConnection->mpEntity ==NULL)
	{
		Error("Could not find entity  with id %d for connection '%s'\n", mlEntityId, msName.c_str());
	}

	kCopyToVar(apConnection, msName);
	kCopyToVar(apConnection, mbInvertStateSent);
	kCopyToVar(apConnection, mlStatesUsed);
	kCopyToVar(apConnection, msCallbackFunc);
}

//-----------------------------------------------------------------------

kBeginSerializeBaseVirtual(iLuxEntity_SaveData)
kSerializeVar(msName,eSerializeType_String)
kSerializeVar(mbFullGameSave,eSerializeType_Bool)
kSerializeVar(mbActive,eSerializeType_Bool)
kSerializeVar(mlEntityType,eSerializeType_Int32)
kSerializeVar(mlID,eSerializeType_Int32)

kSerializeVar(mfMaxFocusDistance, eSerializeType_Float32)

kSerializeVar(mbInteractionDisabled, eSerializeType_Bool)

kSerializeVar(msCallbackFunc, eSerializeType_String)
kSerializeVar(msConnectionStateChangeCallback, eSerializeType_String)

kSerializeVar(msInteractCallback,eSerializeType_String)
kSerializeVar(mbInteractCallbackRemove,eSerializeType_Bool)

kSerializeVar(msLookAtCallback,eSerializeType_String)
kSerializeVar(mbLookAtCallbackRemove,eSerializeType_Bool)
kSerializeVar(mbIsLookedAt,eSerializeType_Bool)

kSerializeClassContainer(mvConnections, cLuxEntityConnection_SaveData, eSerializeType_Class)
kSerializeClassContainer(mlstCollideCallbacks, cLuxCollideCallback_SaveData, eSerializeType_Class)

kEndSerialize()

//-----------------------------------------------------------------------

void iLuxEntity::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	apSaveData->mlEntityType = mEntityType;

	kCopyToVar(apSaveData, msName);
	kCopyToVar(apSaveData, mbFullGameSave);
	kCopyToVar(apSaveData, mbActive);
	kCopyToVar(apSaveData, mlID);
	kCopyToVar(apSaveData, mbInteractionDisabled);

	kCopyToVar(apSaveData, mfMaxFocusDistance);

	kCopyToVar(apSaveData, msCallbackFunc);
	kCopyToVar(apSaveData, msConnectionStateChangeCallback);

	kCopyToVar(apSaveData, msInteractCallback);
	kCopyToVar(apSaveData, mbInteractCallbackRemove);

	kCopyToVar(apSaveData, msLookAtCallback);
	kCopyToVar(apSaveData, mbLookAtCallbackRemove);
	kCopyToVar(apSaveData, mbIsLookedAt);

	apSaveData->mvConnections.Resize(mvConnections.size());
    for(size_t i=0; i<mvConnections.size();++i)
	{
		apSaveData->mvConnections[i].FromConnection(mvConnections[i]);
	}

	for(tLuxCollideCallbackListIt it = mlstCollideCallbacks.begin(); it != mlstCollideCallbacks.end(); ++it)
	{
		cLuxCollideCallback *pCallback = *it;
		cLuxCollideCallback_SaveData saveCallback;
		saveCallback.FromCallback(pCallback);

		apSaveData->mlstCollideCallbacks.Add(saveCallback);
	}
}

//-----------------------------------------------------------------------

void iLuxEntity::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	kCopyFromVar(apSaveData, msName);
	kCopyFromVar(apSaveData, mbFullGameSave);
	SetActive(apSaveData->mbActive);
	kCopyFromVar(apSaveData, mlID);
	kCopyFromVar(apSaveData, mbInteractionDisabled);

	kCopyFromVar(apSaveData, mfMaxFocusDistance);

	kCopyFromVar(apSaveData, msCallbackFunc);
	kCopyFromVar(apSaveData, msConnectionStateChangeCallback);

	kCopyFromVar(apSaveData, msInteractCallback);
	kCopyFromVar(apSaveData, mbInteractCallbackRemove);

	kCopyFromVar(apSaveData, msLookAtCallback);
	kCopyFromVar(apSaveData, mbLookAtCallbackRemove);
	kCopyFromVar(apSaveData, mbIsLookedAt);
}

//-----------------------------------------------------------------------

void iLuxEntity::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	mvConnections.resize(apSaveData->mvConnections.Size());
	for(size_t i=0; i<apSaveData->mvConnections.Size();++i)
	{
		mvConnections[i] = hplNew( cLuxEntityConnection, () );
		apSaveData->mvConnections[i].ToConnection(mvConnections[i], mpMap);
	}

	cContainerListIterator<cLuxCollideCallback_SaveData> it = apSaveData->mlstCollideCallbacks.GetIterator();
	while(it.HasNext())
	{
		cLuxCollideCallback_SaveData& saveCallback = it.Next();
		cLuxCollideCallback *pCallback = hplNew(cLuxCollideCallback, ());
		
		saveCallback.ToCallback(mpMap, this, pCallback);
		mlstCollideCallbacks.push_back(pCallback);
	}
}

//-----------------------------------------------------------------------
