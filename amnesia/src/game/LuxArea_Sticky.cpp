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

#include "LuxArea_Sticky.h"

#include "LuxMap.h"
#include "LuxPlayer.h"
#include "LuxPlayerState.h"

#include "LuxProp.h"


//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaLoader_Sticky::cLuxAreaLoader_Sticky(const tString& asName) : iLuxAreaLoader(asName)
{

}

cLuxAreaLoader_Sticky::~cLuxAreaLoader_Sticky()
{

}

//-----------------------------------------------------------------------

iLuxArea *cLuxAreaLoader_Sticky::CreateArea(const tString& asName, int alID, cLuxMap *apMap)
{
	cLuxArea_Sticky *pArea = hplNew(cLuxArea_Sticky, (asName, alID, apMap));
	return pArea;
}

//-----------------------------------------------------------------------

void cLuxAreaLoader_Sticky::LoadVariables(iLuxArea *apArea, cWorld *apWorld)
{
	cLuxArea_Sticky *pStickyArea = static_cast<cLuxArea_Sticky*>(apArea);


	pStickyArea->msAttachFunction = GetVarString("AttachFunction","");
	pStickyArea->msDetachFunction = GetVarString("DetachFunction","");

	pStickyArea->msAttachSound = GetVarString("AttachSound","");
	pStickyArea->msDetachSound = GetVarString("DetachSound","");

	pStickyArea->msAttachPS = GetVarString("AttachPS","");
	pStickyArea->msDetachPS = GetVarString("DetachPS","");

	pStickyArea->msAttachableBodyName = GetVarString("AttachableBodyName","");

	pStickyArea->mbMoveBody = GetVarBool("MoveBody",true);
	pStickyArea->mbRotateBody = GetVarBool("RotateBody",true);

	pStickyArea->mbCheckCenterInArea = GetVarBool("CheckCenterInArea",true);

	pStickyArea->mbCanDetach = GetVarBool("CanDetach",true);

	pStickyArea->mfPoseTime = GetVarFloat("PoseTime",1);

	gpBase->PreloadSound(pStickyArea->msAttachSound);
	gpBase->PreloadSound(pStickyArea->msDetachSound);
	gpBase->PreloadParticleSystem(pStickyArea->msAttachPS);
	gpBase->PreloadParticleSystem(pStickyArea->msDetachPS);
}

void cLuxAreaLoader_Sticky::SetupArea(iLuxArea *apArea, cWorld *apWorld)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxArea_Sticky::mbAllowAttachment = false;

//-----------------------------------------------------------------------

cLuxArea_Sticky::cLuxArea_Sticky(const tString &asName, int alID, cLuxMap *apMap)  : iLuxArea(asName,alID,apMap, eLuxAreaType_Sticky)
{
	mpAttachedBody = NULL;
	mpLastAttachedBody = NULL;

	msAttachFunction = "";
	msDetachFunction = "";

	msAttachSound = "";
	msDetachFunction = "";

	msAttachSound = "";
	msDetachSound = "";

	mbMoveBody = true;
	mbRotateBody = true;

	mbCheckCenterInArea = true;

	mfPoseTime = 0.2f;

	mbCanDetach = true;

	mfSetMtxTime = 1.0f;

	mfAttachedBodyMass =0;
	mbAttachedEntityFullGameSaved = false;

	mbFullGameSave = true;
}

//-----------------------------------------------------------------------

cLuxArea_Sticky::~cLuxArea_Sticky()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxArea_Sticky::SetupAfterLoad(cWorld *apWorld)
{
	
}

//-----------------------------------------------------------------------

void cLuxArea_Sticky::OnUpdate(float afTimeStep)
{
	UpdateAttachBody(afTimeStep);
	UpdateCollision(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxArea_Sticky::AttachBody(iPhysicsBody *apBody)
{
	if(mpAttachedBody) return;

	//Log("Attaching body %s\n", pAttachBody->GetName().c_str());

	/////////////////////////
	//If in an interact state, set the previous state		
	gpBase->mpPlayer->GetCurrentStateData()->OnAttachBodyToStickyArea(apBody);

	/////////////////////////
	//Snap it into place.
	mtxAttachedStart = apBody->GetLocalMatrix();

	mfSetMtxTime = 0;

	apBody->SetLinearVelocity(0);
	apBody->SetAngularVelocity(0);

	mbAttachedBodyGravity = apBody->GetGravity();
	apBody->SetGravity(false);

	mfAttachedBodyMass = apBody->GetMass();
	apBody->SetMass(0);

	iLuxEntity *pEntity = (iLuxEntity*)apBody->GetUserData();
	if(pEntity)
	{
		mbAttachedEntityFullGameSaved = pEntity->GetFullGameSave();
		pEntity->SetFullGameSave(true);
	}
	else
	{
		mbAttachedEntityFullGameSaved = false;
	}

	mpAttachedBody = apBody;
	mpLastAttachedBody = mpAttachedBody;
}

//-----------------------------------------------------------------------

void cLuxArea_Sticky::DetachBody()
{
	if(mpAttachedBody==NULL) return;

	cWorld *pWorld = mpMap->GetWorld();

	//Callback function
	if(msDetachFunction!="")
	{
		mpMap->RunScript(GetCallbackFunc(msDetachFunction,mpAttachedBody));
	}

	//Sound
	if(msDetachSound!="")
	{
		cSoundEntity *pSound = pWorld->CreateSoundEntity("DetachSound",msDetachSound,true);
		if(pSound) pSound->SetPosition(mpBody->GetWorldPosition());
	}

	//Particle System
	if(msDetachPS!="")
	{
		cParticleSystem *pPS = pWorld->CreateParticleSystem("DetachPS",msDetachPS,1);
		if(pPS) pPS->SetMatrix(mpBody->GetWorldMatrix());
	}

	mpAttachedBody->SetGravity(mbAttachedBodyGravity);
	mpAttachedBody->SetMass(mfAttachedBodyMass);
	mpAttachedBody->Enable();

	iLuxEntity *pEntity = (iLuxEntity*)mpAttachedBody->GetUserData();
	if(pEntity) pEntity->SetFullGameSave(mbAttachedEntityFullGameSaved);

	mpAttachedBody = NULL;
}


//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxArea_Sticky::UpdateAttachBody(float afTimeStep)
{
	if(mpAttachedBody==NULL || mfSetMtxTime >= 1) return;
	
	//////////////////////////////////
	// Move and rotate
	if(mbMoveBody && mbRotateBody)
	{
		if(mfPoseTime==0)	mfSetMtxTime = 1.0f;
		else				mfSetMtxTime += afTimeStep / mfPoseTime;

		cMatrixf mtxGoal = mpBody->GetWorldMatrix();
		mtxGoal.SetTranslation(mpBody->GetWorldPosition() - mpAttachedBody->GetMassCentre());

		cMatrixf mtxNew = cMath::MatrixSlerp(mfSetMtxTime,mtxAttachedStart,mtxGoal,true);

		mpAttachedBody->SetMatrix(mtxNew);
	}
	//////////////////////////////////
	// Move only
	else if(mbMoveBody && !mbRotateBody)
	{
		if(mfPoseTime==0)	mfSetMtxTime = 1.0f;
		else				mfSetMtxTime += afTimeStep / mfPoseTime;

		cVector3f vGoal = mpBody->GetWorldPosition() - mpAttachedBody->GetMassCentre();

		cVector3f vNew =	mtxAttachedStart.GetTranslation()* (1-mfSetMtxTime) +
			vGoal *mfSetMtxTime;

		mpAttachedBody->SetPosition(vNew);
	}
	//////////////////////////////////
	// Rotate only
	else if(!mbMoveBody && mbRotateBody)
	{
		if(mfPoseTime==0)	mfSetMtxTime = 1.0f;
		else				mfSetMtxTime += afTimeStep / mfPoseTime;

		cMatrixf mtxGoal = mpBody->GetWorldMatrix();

		cMatrixf mtxNew = cMath::MatrixSlerp(mfSetMtxTime,mtxAttachedStart,mtxGoal,true);

		mtxNew.SetTranslation(mpAttachedBody->GetWorldPosition());
		mpAttachedBody->SetMatrix(mtxNew);
	}
	//////////////////////////////////
	// No transformation
	else
	{
		mfSetMtxTime = 1.0f;
	}


	//////////////////////////////////
	// If attached body in place, do effects
	if(mfSetMtxTime >= 1)
	{
		cWorld *pWorld = mpMap->GetWorld();

		//Sound
		if(msAttachSound!="")
		{
			cSoundEntity *pSound = pWorld->CreateSoundEntity("AttachSound",msAttachSound,true);
			if(pSound) pSound->SetPosition(mpBody->GetWorldPosition());
		}

		//Particle System
		if(msAttachPS!="")
		{
			cParticleSystem *pPS = pWorld->CreateParticleSystem("AttachPS",msAttachPS,1);
			if(pPS) pPS->SetMatrix(mpBody->GetWorldMatrix());
		}
	}
}

//-----------------------------------------------------------------------

void cLuxArea_Sticky::UpdateCollision(float afTimeStep)
{
	if(mpAttachedBody) return;

	///////////////////////////
	// Get data
	iPhysicsWorld *pPhysicsWorld = mpMap->GetPhysicsWorld();
	iPhysicsBody *pAreaBody = mpBody;

	cCollideData collideData;
	collideData.SetMaxSize(1);

	///////////////////////////
	// Iterate bodies
	std::vector<iPhysicsBody*> vBodies;
	pPhysicsWorld->GetBodiesInBV(mpBody->GetBoundingVolume(),&vBodies);

	for(size_t i=0; i<vBodies.size(); ++i)
	{
		iPhysicsBody *pBody = vBodies[i];

		if(pBody->GetCollide()==false || pBody->IsActive()==false) continue;
		if(pBody->GetMass()==0 && pBody->IsCharacter()==false) continue;

		bool bInsideWater = true;

		/////////////////////////
		//Bounding volume check
		bool bInsideBV = false;
		if(mbCheckCenterInArea)
		{
			cVector3f vPos = cMath::MatrixMul(pBody->GetLocalMatrix(),pBody->GetMassCentre());
			bInsideBV = cMath::CheckPointInBVIntersection(vPos,*pAreaBody->GetBoundingVolume());
		}
		else
		{
			bInsideBV = cMath::CheckBVIntersection(*pBody->GetBoundingVolume(),*pAreaBody->GetBoundingVolume());
		}

		//////////////////////////
		//If not inside then skip this body
		if(bInsideBV==false)
		{
			//If this was the last body that was attached, it has now moved out and does not need to be rejected.
			if(pBody == mpLastAttachedBody)
				mpLastAttachedBody = NULL;
			
			continue;
		}

		//////////////////////////
		//If the previously attached body is still inside, then skip it.
		if(pBody == mpLastAttachedBody) continue;

		///////////////////////////
		// See if body is right for attaching
		mbAllowAttachment = false;
		if(msAttachableBodyName!="")
		{
			if(cString::GetFirstStringPos(pBody->GetName(), msAttachableBodyName)<0) continue;
			mbAllowAttachment = true;
		}

		///////////////////////////////
		//Check for collision
        if(pPhysicsWorld->CheckShapeCollision(	pBody->GetShape(),pBody->GetLocalMatrix(), pAreaBody->GetShape(), pAreaBody->GetLocalMatrix(),
												collideData,1, false)==false)
		{
			continue;
		}
	
		///////////////////////////
		// Call callback and see if it should be attached.
		if(msAttachFunction!="")
		{
			mpMap->RunScript(GetCallbackFunc(msAttachFunction,pBody));

			if(mbAllowAttachment==false) continue;
		}

		///////////////////////////
		// Attach
		AttachBody(pBody);
	}

}


//-----------------------------------------------------------------------

tString cLuxArea_Sticky::GetCallbackFunc(const tString &asFunc,iPhysicsBody *apBody)
{
	return asFunc + "(\"" + msName + "\",\"" + apBody->GetName() + "\")"; 
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxArea_Sticky_SaveData, iLuxArea_SaveData)

kSerializeVar(msAttachFunction, eSerializeType_String)
kSerializeVar(msDetachFunction, eSerializeType_String)

kSerializeVar(msAttachSound, eSerializeType_String)
kSerializeVar(msDetachSound, eSerializeType_String)

kSerializeVar(msAttachPS, eSerializeType_String)
kSerializeVar(msDetachPS, eSerializeType_String)

kSerializeVar(mbCanDetach, eSerializeType_Bool)

kSerializeVar(mfPoseTime, eSerializeType_Float32)

kSerializeVar(mbCheckCenterInArea, eSerializeType_Bool)

kSerializeVar(msAttachableBodyName, eSerializeType_String)

kSerializeVar(mlAttachedEntityID, eSerializeType_Int32)
kSerializeVar(mlAttachedBodyID, eSerializeType_Int32)

kSerializeVar(mfAttachedBodyMass, eSerializeType_Float32)
kSerializeVar(mbAttachedBodyGravity, eSerializeType_Bool)
kSerializeVar(mbAttachedEntityFullGameSaved, eSerializeType_Bool)

kSerializeVar(mfSetMtxTime, eSerializeType_Float32)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxArea* cLuxArea_Sticky_SaveData::CreateArea(cLuxMap *apMap)
{
	return hplNew(cLuxArea_Sticky, (msName, mlID, apMap));
}

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxArea_Sticky::CreateSaveData()
{
	return hplNew(cLuxArea_Sticky_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxArea_Sticky::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::SaveToSaveData(apSaveData);
	cLuxArea_Sticky_SaveData *pData = static_cast<cLuxArea_Sticky_SaveData*>(apSaveData);

	//////////////////////
	// Attached body
	if(mpAttachedBody)
	{
		iLuxEntity *pEntity = static_cast<iLuxEntity*>(mpAttachedBody->GetUserData());
		
		pData->mlAttachedBodyID = mpAttachedBody->GetUniqueID();
		pData->mlAttachedEntityID = pEntity->GetID();
	}
	else
	{
		pData->mlAttachedBodyID = -1;
		pData->mlAttachedEntityID = -1;
	}

	/////////////////////////
	// Properties
	kCopyToVar(pData,msAttachFunction);
	kCopyToVar(pData,msDetachFunction);

	kCopyToVar(pData,msAttachSound);
	kCopyToVar(pData,msDetachSound);

	kCopyToVar(pData,msAttachPS);
	kCopyToVar(pData,msDetachPS);

	kCopyToVar(pData,mbCanDetach);

	kCopyToVar(pData,mfPoseTime);

	kCopyToVar(pData,mbCheckCenterInArea);

	kCopyToVar(pData,msAttachableBodyName);

	kCopyToVar(pData,mfAttachedBodyMass);
	kCopyToVar(pData,mbAttachedBodyGravity);
	kCopyToVar(pData,mbAttachedEntityFullGameSaved);

	kCopyToVar(pData,mfSetMtxTime);
}

//-----------------------------------------------------------------------

void cLuxArea_Sticky::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::LoadFromSaveData(apSaveData);
	cLuxArea_Sticky_SaveData *pData = static_cast<cLuxArea_Sticky_SaveData*>(apSaveData);

	//////////////////////
	// Attached body
	if(pData->mlAttachedEntityID >=0)
	{
		iLuxEntity *pEntity = mpMap->GetEntityByID(pData->mlAttachedEntityID);
		if(pEntity && pEntity->GetEntityType() == eLuxEntityType_Prop)
		{
			iLuxProp *pProp = static_cast<iLuxProp*>(pEntity);
			mpAttachedBody = pProp->GetBodyFromID(pData->mlAttachedBodyID);
			if(mpAttachedBody==NULL)
			{
				Error("Could not load body with id %d in prop '%s'\n", pData->mlAttachedBodyID, pProp->GetName().c_str());
			}
		}
		else
		{
			Error("Could not load prop with id %d\n", pData->mlAttachedEntityID);
		}
		mpLastAttachedBody = mpAttachedBody;
	}
	else
	{
		mpAttachedBody = NULL;
		mpLastAttachedBody = NULL;
	}

	
	/////////////////////////
	// Properties
	kCopyFromVar(pData,msAttachFunction);
	kCopyFromVar(pData,msDetachFunction);

	kCopyFromVar(pData,msAttachSound);
	kCopyFromVar(pData,msDetachSound);

	kCopyFromVar(pData,msAttachPS);
	kCopyFromVar(pData,msDetachPS);

	kCopyFromVar(pData,mbCanDetach);

	kCopyFromVar(pData,mfPoseTime);

	kCopyFromVar(pData,mbCheckCenterInArea);

	kCopyFromVar(pData,msAttachableBodyName);

	kCopyFromVar(pData,mfAttachedBodyMass);
	kCopyFromVar(pData,mbAttachedBodyGravity);
	kCopyFromVar(pData,mbAttachedEntityFullGameSaved);

	kCopyFromVar(pData,mfSetMtxTime);
}

//-----------------------------------------------------------------------

void cLuxArea_Sticky::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);

}

//-----------------------------------------------------------------------

