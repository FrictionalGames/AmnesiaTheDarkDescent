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

#include "LuxProp_SwingDoor.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxHintHandler.h"

#include "LuxMap.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_SwingDoor::cLuxPropLoader_SwingDoor(const tString& asName) : iLuxPropLoader(asName)
{
	mfDefaultMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction","SwingDoor_DefaultMaxFocusDist",0);
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_SwingDoor::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_SwingDoor, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_SwingDoor::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_SwingDoor  *pSwingDoor = static_cast<cLuxProp_SwingDoor*>(apProp);

	///////////////////////////
	// General
	pSwingDoor->mSwingDoorData.mfMoveMaxSpeed = GetVarFloat("MoveMaxSpeed", 13.5f);
	pSwingDoor->mSwingDoorData.mfMoveSlowDownFactor = GetVarFloat("MoveSlowDownFactor", 3.0f);
	pSwingDoor->mSwingDoorData.mfMoveSpeedFactor = GetVarFloat("MoveSpeedFactor", 1.0f);
	pSwingDoor->mSwingDoorData.mfMoveThrowImpulse = GetVarFloat("MoveThrowImpulse", 6.0f);

	pSwingDoor->mbCanInteractWithStaticBody = GetVarBool("CanInteractWithStaticBody", false);

	pSwingDoor->msCloseOnSound = GetVarString("CloseOnSound", "");
	pSwingDoor->msCloseOffSound = GetVarString("CloseOffSound", "");
	pSwingDoor->msLockOnSound = GetVarString("LockOnSound", "");
	pSwingDoor->msLockOffSound = GetVarString("LockOffSound", "");
	pSwingDoor->msInteractLockedSound = GetVarString("InteractLockedSound", "");

	
	pSwingDoor->mbBreakable = GetVarBool("Breakable", false);

	pSwingDoor->msDamageMesh[0] = GetVarString("DamageMesh1", "");
	pSwingDoor->msDamageMesh[1] = GetVarString("DamageMesh2", "");
	pSwingDoor->msBrokenEntity = GetVarString("BrokenEntity", "");

	pSwingDoor->msDamageSound = GetVarString("DamageSound", "");
	pSwingDoor->msDamagePS = GetVarString("DamagePS", "");

	pSwingDoor->msBreakSound = GetVarString("BreakSound", "");
	pSwingDoor->msBreakPS = GetVarString("BreakPS", "");
	pSwingDoor->mfBreakImpulse = GetVarFloat("BreakImpulse", 0);

	pSwingDoor->mfHealthDamage[0] = GetVarFloat("HealthDamage1", 0);
	pSwingDoor->mfHealthDamage[1] = GetVarFloat("HealthDamage2", 0);
}

//-----------------------------------------------------------------------

void cLuxPropLoader_SwingDoor::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_SwingDoor  *pSwingDoor = static_cast<cLuxProp_SwingDoor*>(apProp);
	
	pSwingDoor->mbDisableBreakable = apInstanceVars->GetVarBool("DisableBreakable",false);

	pSwingDoor->SetLocked(apInstanceVars->GetVarBool("Locked",false),false);
	float fOpenAmount = apInstanceVars->GetVarFloat("OpenAmount",0);
	
    pSwingDoor->SetupDoorPhysics(fOpenAmount);	
}
//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_SwingDoor::cLuxProp_SwingDoor(const tString &asName, int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_SwingDoor)
{
	mbClosed = false;
	mbLocked = false;	

	mlBrokenEntityID = -1;

	mbDisableAutoClose = false;

	mlCurrentMeshEntity = 0;
	mbBroken = false;

	for(int i=0; i<3; ++i)
		mpDamageMeshEntity[i] =NULL;

	mfInteractSoundCount =0;
}

//-----------------------------------------------------------------------

cLuxProp_SwingDoor::~cLuxProp_SwingDoor()
{
	for(int i=1; i<3; ++i)
	{
		if(mpDamageMeshEntity[i]) 
			mpWorld->DestroyMeshEntity(mpDamageMeshEntity[i]);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_SwingDoor::CanInteract(iPhysicsBody *apBody)
{
	if(	apBody->GetMass()==0 && mbCanInteractWithStaticBody==false && mpMap->BodyIsInDetachableStickyArea(apBody)==false) 
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------

bool cLuxProp_SwingDoor::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	///////////////////////////////
	//If body mass is 0, get a dynamic body
	if(apBody->GetMass()==0 && mbCanInteractWithStaticBody)
	{
		for(size_t i=0; i<mvBodies.size(); ++i)
		{
			if(mvBodies[i]->GetMass() > 0)
			{
				apBody = mvBodies[i];
				break;
			}
		}
	}

	///////////////////////////////
	//Get special swing door data
	cLuxSwingDoorJointData *pData = GetJointDataFromBody(apBody);
	if(pData==NULL) Error("Could not find swing door data for body '%s'\n", apBody->GetName().c_str());

	mpMap->DetachBodyFromStickyArea(apBody);

	if(mbLocked && mfInteractSoundCount <=0)
	{
		PlaySound("InteractLocked",msInteractLockedSound,true, true);
		mfInteractSoundCount = 0.5f;
	}

	if(mbLocked==false)
	{
		SetClosed(false, true);
	}

	mbDisableAutoClose = false;

	if(mbLocked == false  && mbShowHints)
		gpBase->mpHintHandler->Add("EntitySwingDoor", kTranslate("Hints", "EntitySwingDoor"), 0);

	cLuxPlayerStateVars::SetupInteraction(apBody, avPos);
	gpBase->mpPlayer->ChangeState(eLuxPlayerState_InteractSwingDoor);
	
	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::OnSetupAfterLoad(cWorld *apWorld)
{
	////////////////////////////////////
	// Set up joints
	int lNum=0;
	for(size_t i=0; i< mvJoints.size(); ++i)
	{
		iPhysicsJoint *pJoint = mvJoints[i];
		if(pJoint->GetType() != ePhysicsJointType_Hinge) continue;

		iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);
		mvJointData.push_back(cLuxSwingDoorJointData());

		iPhysicsBody *pChildBody = pJoint->GetChildBody();

		///////////////////////
		//Set update properties
		pHingeJoint->SetStickyMinLimit(true);
		pHingeJoint->SetStickyMaxLimit(true);

		///////////////////////
		//Set up data
		mvJointData[lNum].mpHingeJoint = pHingeJoint;
		mvJointData[lNum].mpChildBody = pChildBody;

		mvJointData[lNum].mfMaxAngle = pHingeJoint->GetMaxAngle();

		++lNum;
	}

	////////////////////////////////////
	// Get Dyn body (this only works for single doors!)
	iPhysicsBody *pDynBody = NULL;
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		if(mvBodies[i]->GetMass()!=0){
			pDynBody = mvBodies[i];
			break;
		}
	}

	cMatrixf mtxInvDynBody = pDynBody ? cMath::MatrixInverse(pDynBody->GetLocalMatrix()) : cMatrixf::Identity;
	
	////////////////////////////////////
	// Set up damage mesh entities
	cMeshManager *pMeshManager = gpBase->mpEngine->GetResources()->GetMeshManager();
	mpDamageMeshEntity[0] = mpMeshEntity;
	for(int i=0; i<2;++i)
	{
		if(msDamageMesh[i]=="") continue;
		//Log("DamageMesh: %d\n", i);

		//////////////////////////
		//Create Mesh Entity
		cMesh *pMesh = pMeshManager->CreateMesh(msDamageMesh[i]);
		if(pMesh==NULL)
		{
			Error("Could not load damage mesh '%s' for '%s'\n", msDamageMesh[i].c_str(), msFileName.c_str());
			continue;
		}

		cMeshEntity *pDamageEntity = apWorld->CreateMeshEntity(msName+"damage"+cString::ToString(i+1),pMesh);
		pDamageEntity->SetMatrix(mpMeshEntity->GetLocalMatrix());

		for(int sub=0; sub < pDamageEntity->GetSubMeshEntityNum(); ++sub)
		{
			cSubMeshEntity *pDamSubEnt = pDamageEntity->GetSubMeshEntity(sub);
			int lIdx = mpMeshEntity->GetMesh()->GetSubMeshIndex(pDamSubEnt->GetSubMesh()->GetName());
			//Log(" sub '%s': %d\n", pDamSubEnt->GetName().c_str(), sub);
			if(lIdx < 0)
			{
				//This is not ideal but should be good enough.
				if(pDynBody)
				{
					cMatrixf mtxWorld = cMath::MatrixMul(m_mtxOnLoadTransform, pDamSubEnt->GetLocalMatrix());
					pDamSubEnt->SetMatrix(cMath::MatrixMul(mtxInvDynBody, mtxWorld));

					pDynBody->AddChild(pDamSubEnt);
				}
				continue;
			}
			cSubMeshEntity *pBaseSubEnt = mpMeshEntity->GetSubMeshEntity(lIdx);
			pDamSubEnt->SetMatrix(pBaseSubEnt->GetLocalMatrix());
			if(pBaseSubEnt->GetEntityParent())
			{
				pBaseSubEnt->GetEntityParent()->AddChild(pDamSubEnt);
			}
		}
				
		pDamageEntity->SetVisible(false);
		pDamageEntity->SetActive(false);
		mpDamageMeshEntity[i+1] = pDamageEntity;
	}
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::OnResetProperties()
{
	if(mbBroken)
	{
		mbBroken = false;
		if(mlBrokenEntityID >=0)
		{
			iLuxEntity *pEntity = mpMap->GetEntityByID(mlBrokenEntityID);
			if(pEntity)
			{
				mpMap->DestroyEntity(pEntity);
			}
		}
	}
	
	SetupDoorPhysics(0.0f);

	SetCurrentDamageLevel(0);

	//////////////////////////////
	// Enable all bodies and meshes
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodies[i];
		if(pBody->GetMass()==0) continue;

		pBody->SetActive(true);

		/////////////////////////////////////////
		// Check what children math mesh entity sub entities
		cEntity3DIterator entIt = pBody->GetChildIterator();
		while(entIt.HasNext())
		{
			iEntity3D *pEntity = entIt.Next();
			for(int j=0; j<mpMeshEntity->GetSubMeshEntityNum(); ++j)
			{
				cSubMeshEntity *pMeshEnt = mpMeshEntity->GetSubMeshEntity(j);
				if((iEntity3D*)pMeshEnt == pEntity)
				{
					pMeshEnt->SetActive(true);
					pMeshEnt->SetVisible(true);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::UpdatePropSpecific(float afTimeStep)
{
	if(mfInteractSoundCount >0)
		mfInteractSoundCount-=afTimeStep;

	////////////////////////////////
	// If the door is close to 0 angle, then close it
	if(mbClosed==false && IsInteractedWith()==false && mbDisableAutoClose == false && mvJoints.size()==1)
	{
		for(size_t i=0; i<mvJointData.size(); ++i)
		{
			if(cMath::Abs(mvJointData[i].mpHingeJoint->GetAngle()) < cMath::ToRad(10))
			{
				SetClosed(true, true);
			}
		}
	}
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::BeforePropDestruction()
{

}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_SwingDoor::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(CanInteract(apBody)) return eLuxFocusCrosshair_Grab;

	return eLuxFocusCrosshair_Default;
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::ImplementedOnSetActive(bool abX)
{
	for(int i=0; i<3; ++i)
	{
		if(mpDamageMeshEntity[i])
		{
			bool bActive = abX ? mlCurrentMeshEntity==i : false;

			mpDamageMeshEntity[i]->SetVisible(bActive);
			mpDamageMeshEntity[i]->SetActive(bActive);
		}
	}
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::OnHealthChange()
{
	if(mbBreakable==false || mbDisableBreakable) return;
	if(mbBroken) return;

	tString msSound = "";
	tString msPS = "";

	/////////////////////////////
	// Get Main body
	iPhysicsBody *pMainBody = NULL;
	int lMainBodyIdx =-1;

	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		if(mvBodies[i]->GetMass()!=0)
		{
			pMainBody = mvBodies[i];
			lMainBodyIdx = (int)i;
		}
	}

	////////////////////////
	// Broken
	if(mfHealth <= 0 && mbBroken == false)
	{
		mbBroken = true;	

		SetCurrentDamageLevel(0);

		////////////////////////////
		//Disable all bodies + any children
		for(size_t i=0; i<mvBodies.size(); ++i)
		{
			iPhysicsBody *pBody = mvBodies[i];
			if(pBody->GetMass()==0) continue;

			pMainBody = pBody;
			lMainBodyIdx = (int)i;

			pBody->SetActive(false);

			/////////////////////////////////////////
			// Check what children math mesh entity sub entities
			cEntity3DIterator entIt = pBody->GetChildIterator();
			while(entIt.HasNext())
			{
				iEntity3D *pEntity = entIt.Next();
				for(int j=0; j<mpMeshEntity->GetSubMeshEntityNum(); ++j)
				{
					cSubMeshEntity *pMeshEnt = mpMeshEntity->GetSubMeshEntity(j);
					if((iEntity3D*)pMeshEnt == pEntity)
					{
						pMeshEnt->SetActive(false);
						pMeshEnt->SetVisible(false);
					}
				}
			}
		}
		
		////////////////////////////
		//Create broken entity
		if(pMainBody && msBrokenEntity != "")
		{
			cEntityBodyExtraData* pBodyData = &mvBodyExtraData[lMainBodyIdx];

			cMatrixf mtxInvLocalBody = cMath::MatrixInverse(pBodyData->m_mtxLocalTransform);
			cMatrixf mtxEntity = cMath::MatrixMul(pMainBody->GetLocalMatrix(), mtxInvLocalBody);

			mpMap->ResetLatestEntity();
			mpMap->CreateEntity(msName + "_broken", msBrokenEntity, mtxEntity, mvOnLoadScale);	

			////////////////////////
			//Add impulse and velocity
			iLuxEntity *pEntity = mpMap->GetLatestEntity();
			if(pEntity && pEntity->GetEntityType() == eLuxEntityType_Prop)
			{
				mlBrokenEntityID = pEntity->GetID();

				iLuxProp *pProp = static_cast<iLuxProp*>(pEntity);

				for(int i=0; i< pProp->GetBodyNum(); ++i)
				{
					iPhysicsBody *pNewBody = pProp->GetBody(i);
					cVector3f vNewBodyCenter = cMath::MatrixMul(pNewBody->GetLocalMatrix(), pNewBody->GetMassCentre());

					cVector3f vImpulseDir = cMath::Vector3Normalize(vNewBodyCenter - mtxEntity.GetTranslation());

					pNewBody->AddImpulse(vImpulseDir*mfBreakImpulse + pMainBody->GetLinearVelocity());
				}
			}
		}
		
		msSound = msBreakSound;
		msPS = msBreakPS;

		///////////////////////
		// Callback
		RunCallbackFunc("Break");
	}
	////////////////////////
	// Damage 2
	else if(mfHealth <  mfHealthDamage[1] && mlCurrentMeshEntity<=1)
	{
		SetCurrentDamageLevel(2);	

		msSound = msDamageSound;
		msPS = msDamagePS;
	}
	////////////////////////
	// Damage 1
	else if(mfHealth < mfHealthDamage[0] && mlCurrentMeshEntity==0)
	{
		SetCurrentDamageLevel(1);

		msSound = msDamageSound;
		msPS = msDamagePS;
	}

	/////////////////////////////
	// Effects
	if(pMainBody)
	{
		///////////////////////
		// Create Sound
		if(msSound != "")
		{
			cSoundEntity *pSound = mpWorld->CreateSoundEntity(msName + "_BreakSound", msSound, true);
			if(pSound) pSound->SetPosition(pMainBody->GetLocalPosition());
		}

		///////////////////////
		// Create Particle System
		if(msPS != "")
		{
			cParticleSystem *pPS = mpWorld->CreateParticleSystem(msName + "_BreakPS", msPS,1);
			if(pPS) pPS->SetMatrix(pMainBody->GetLocalMatrix());
		}
	}
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::OnDamage(float afAmount, int alStrength)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::SetClosed(bool abClosed, bool abEffects)
{
	if(mbClosed == abClosed) return;

	mbClosed = abClosed;

	for(size_t i=0; i<mvJointData.size(); ++i)
	{
		iPhysicsJointHinge *pHingeJoint = mvJointData[i].mpHingeJoint;

		if(mbClosed)
		{
			pHingeJoint->SetMaxAngle(cMath::ToRad(2.0f));
			pHingeJoint->SetStickyMinLimit(false);
			pHingeJoint->SetStickyMaxLimit(false);
		}
		else
		{
			pHingeJoint->SetMaxAngle(mvJointData[i].mfMaxAngle);
			pHingeJoint->SetStickyMinLimit(true);
			pHingeJoint->SetStickyMaxLimit(true);
		}
	}

	//Add force to all bodies so that the door really closes.
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodies[i];

		pBody->AddForce(cVector3f(1,1,1));
	}

	if(abEffects)
	{
		if(mbClosed)PlaySound("CloseOn",msCloseOnSound,true, true);
		else		PlaySound("CloseOff",msCloseOffSound,true, true);
	}
}

//-----------------------------------------------------------------------

int cLuxProp_SwingDoor::GetDoorState()
{
	int lState =0;

	for(size_t i=0; i<mvJointData.size(); ++i)
	{
		iPhysicsJointHinge *pHingeJoint = mvJointData[i].mpHingeJoint;

		float fAbsAngle = cMath::Abs(pHingeJoint->GetAngle());
		
		float fClosedAngle = cMath::ToRad(5);
		float fOpenAngle = cMath::Abs(mvJointData[i].mfMaxAngle)*0.70f;

		if(fAbsAngle < fClosedAngle)
		{
			if(lState == 1) return 0;
			lState = -1;
		}
		else if(fAbsAngle >  fOpenAngle)
		{
			if(lState == -1) return 0;
			lState = 1;	
		}
		else
		{
			return 0;
		}
	}

	return lState;
}

//-----------------------------------------------------------------------


void cLuxProp_SwingDoor::SetLocked(bool abLocked, bool abEffects)
{
	if(mbLocked == abLocked) return;

	mbLocked = abLocked;
	
	if(mbLocked) SetClosed(true, false);

	if(abEffects)
	{
		if(mbLocked)PlaySound("LockedOn",msLockOnSound,true, true);
		else		PlaySound("LockedOff",msLockOffSound,true, true);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::SetCurrentDamageLevel(int alX)
{
	if(mlCurrentMeshEntity == alX) return;

	mlCurrentMeshEntity = alX;

	for(int i=0; i<3; ++i)
	{
		if(mpDamageMeshEntity[i])
		{
			bool bActive = mlCurrentMeshEntity==i;

			mpDamageMeshEntity[i]->SetVisible(bActive);
			mpDamageMeshEntity[i]->SetActive(bActive);
		}
	}
}

//-----------------------------------------------------------------------

cMeshEntity* cLuxProp_SwingDoor::GetEffectMeshEntity()
{
	if(mlCurrentMeshEntity <0) return NULL;

    return mpDamageMeshEntity[mlCurrentMeshEntity];
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
	if(alState > 0)		SetLocked(true, true);
	else				SetLocked(false, true);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


void cLuxProp_SwingDoor::SetupDoorPhysics(float afOpenAmount)
{
	////////////////////////////////////
	// Set open amount
	if(afOpenAmount>0)
	{
		for(size_t i=0; i<mvJointData.size(); ++i)
		{
			iPhysicsJointHinge *pHingeJoint = mvJointData[i].mpHingeJoint;
			iPhysicsBody *pChildBody = mvJointData[i].mpChildBody;
			float fWantedAngle = mvJointData[i].mfMaxAngle * afOpenAmount;

			cQuaternion qRotation; qRotation.FromAngleAxis(-fWantedAngle, pHingeJoint->GetPinDir());
			cMatrixf mtxRotation = cMath::MatrixQuaternion(qRotation);
			cMatrixf mtxBody = pChildBody->GetLocalMatrix();

			cVector3f vBodyPos = pChildBody->GetLocalPosition();
			cVector3f vPivotOffset = pHingeJoint->GetPivotPoint() - pChildBody->GetLocalPosition();

			mtxBody.SetTranslation(vPivotOffset*-1);
			mtxBody = cMath::MatrixMul(mtxRotation, mtxBody);
			mtxBody.SetTranslation(mtxBody.GetTranslation() + vPivotOffset + vBodyPos);

			pChildBody->SetMatrix(mtxBody);
		}
	}

	////////////////////////////////////
	// Close door (using no effects) if in range. 
	if(mvJoints.size()==1)
	{
		for(size_t i=0; i<mvJointData.size(); ++i)
		{
			if(cMath::Abs(mvJointData[i].mpHingeJoint->GetAngle()) < cMath::ToRad(10))
			{
				SetClosed(true, false);
			}
		}
	}
}

//-----------------------------------------------------------------------

cLuxSwingDoorJointData* cLuxProp_SwingDoor::GetJointDataFromBody(iPhysicsBody *apBody)
{
	for(size_t i=0; i<mvJointData.size(); ++i)
	{
		if(mvJointData[i].mpChildBody == apBody) return &mvJointData[i];
	}
	return NULL;
}

//-----------------------------------------------------------------------

cLuxSwingDoorJointData* cLuxProp_SwingDoor::GetJointDataFromJoint(iPhysicsJoint *apJoint)
{
	for(size_t i=0; i<mvJointData.size(); ++i)
	{
		if(mvJointData[i].mpHingeJoint == apJoint) return &mvJointData[i];
	}
	return NULL;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_SwingDoor_SaveData, iLuxProp_SaveData)

kSerializeVar(mbLocked, eSerializeType_Bool)
kSerializeVar(mbBroken, eSerializeType_Bool)
kSerializeVar(mbClosed, eSerializeType_Bool)
kSerializeVar(mlBrokenEntityID, eSerializeType_Int32)
kSerializeVar(mlCurrentMeshEntity, eSerializeType_Int32)
kSerializeVar(mbDisableBreakable, eSerializeType_Bool)
kSerializeVar(mbDisableAutoClose, eSerializeType_Bool)

kSerializeVar(mDamageMesh1, eSerializeType_Class)
kSerializeVar(mDamageMesh2, eSerializeType_Class)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_SwingDoor::CreateSaveData()
{
	return hplNew(cLuxProp_SwingDoor_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_SwingDoor_SaveData *pData = static_cast<cLuxProp_SwingDoor_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData, mbLocked);
	kCopyToVar(pData, mbBroken);
	kCopyToVar(pData, mbClosed);
	kCopyToVar(pData, mlBrokenEntityID);
	kCopyToVar(pData, mlCurrentMeshEntity);
	kCopyToVar(pData, mbDisableBreakable);
	kCopyToVar(pData, mbDisableAutoClose);

	if(mpDamageMeshEntity[1])
		pData->mDamageMesh1.FromMeshEntity(mpDamageMeshEntity[1]);
	if(mpDamageMeshEntity[2])
		pData->mDamageMesh2.FromMeshEntity(mpDamageMeshEntity[2]);
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_SwingDoor_SaveData *pData = static_cast<cLuxProp_SwingDoor_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	SetClosed(pData->mbClosed, false);
	kCopyFromVar(pData, mbLocked);
	kCopyFromVar(pData, mbBroken);
	kCopyFromVar(pData, mlBrokenEntityID);
	kCopyFromVar(pData, mlCurrentMeshEntity);
	kCopyFromVar(pData, mbDisableBreakable);
	kCopyFromVar(pData, mbDisableAutoClose);

	if(mpDamageMeshEntity[1])
		pData->mDamageMesh1.ToMeshEntity(mpDamageMeshEntity[1]);
	if(mpDamageMeshEntity[2])
		pData->mDamageMesh2.ToMeshEntity(mpDamageMeshEntity[2]);
}

//-----------------------------------------------------------------------

void cLuxProp_SwingDoor::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
