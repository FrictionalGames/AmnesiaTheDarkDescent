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

#include "LuxProp_CritterBase.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxMap.h"
#include "LuxMapHelper.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxPropLoader_Critter::iLuxPropLoader_Critter(const tString asName) : iLuxPropLoader(asName)
{

}

//-----------------------------------------------------------------------

void iLuxPropLoader_Critter::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	iLuxProp_CritterBase  *pCritter = static_cast<iLuxProp_CritterBase*>(apProp);

	pCritter->msDeathEntity = GetVarString("DeathEntity", "");
	pCritter->msDeathPS = GetVarString("DeathPS", "");
	pCritter->msDeathSound = GetVarString("DeathSound", "");

	pCritter->mfAttackSizeRadius = GetVarFloat("AttackSizeRadius", 0);
	pCritter->mvAttackOffset = GetVarVector3f("AttackOffset", 0);
	pCritter->mvAttackDamageMinMax = GetVarVector2f("AttackDamageMinMax", 0);
	pCritter->mvAttackForce = GetVarVector2f("AttackForce", 0);
	pCritter->mlAttackStrength = GetVarInt("AttackStrength", 0);
	pCritter->msAttackHitSound = GetVarString("AttackHitSound", "");

	cVector3f vMeshOffsetPos = GetVarVector3f("MeshOffsetPos", 0);
	cVector3f vMeshOffsetRot = cMath::Vector3ToRad(GetVarVector3f("MeshOffsetRot", 0));
	cVector3f vMeshOffsetScale = GetVarVector3f("MeshOffsetScale", 1);

	pCritter->m_mtxMeshOffset = cMath::MatrixMul(cMath::MatrixRotate(vMeshOffsetRot, eEulerRotationOrder_XYZ), cMath::MatrixScale(vMeshOffsetScale));
	pCritter->m_mtxMeshOffset.SetTranslation(vMeshOffsetPos);

	LoadCritterVariables(apProp, apRootElem);
}


//-----------------------------------------------------------------------

void iLuxPropLoader_Critter::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{


	LoadCritterInstanceVariables(apProp, apInstanceVars);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// RAY CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxCritterRayCallback::Reset()
{
	mbIntersected = false;
	mfClosestT = 99999.0f;
    mpBody=NULL;
}

bool cLuxCritterRayCallback::BeforeIntersect(iPhysicsBody *pBody)
{
	if(pBody->GetCollide()==false || pBody->IsCharacter() || mpCritterBase->mpBody == pBody) return false;
	return true;
}

bool cLuxCritterRayCallback::OnIntersect(iPhysicsBody *pBody, cPhysicsRayParams *apParams)
{
	if(apParams->mfT < mfClosestT)
	{
		mfClosestT = apParams->mfT;
		mvNormal = apParams->mvNormal;
		mvPos = apParams->mvPoint;
		mpBody = pBody;
	}
	mbIntersected = true;
    
	
	return true;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxProp_CritterBase::iLuxProp_CritterBase(const tString &asName,int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_Critter)
{
	mvVel = 0;
    mvGravityVel =0;
	mlAnimState =0;
	mvGroundNormal =cVector3f(0,1,0);

	mbUseRayCollision = true;
	
	mpRayCallback = hplNew(cLuxCritterRayCallback, (this));

	mbColliding = false;

	mbUpdateAnimation = true;

	mbCausesSanityDecrease = false;
}

//-----------------------------------------------------------------------

iLuxProp_CritterBase::~iLuxProp_CritterBase()
{
	hplDelete(mpRayCallback);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool iLuxProp_CritterBase::CanInteract(iPhysicsBody *apBody)
{
	return false;
	
}

//-----------------------------------------------------------------------

bool iLuxProp_CritterBase::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	iPhysicsBody *pBody = mpMainBody ? mpMainBody : apBody;

	return true;
}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::OnSetupAfterLoad(cWorld *apWorld)
{
	mpBody = mvBodies[0];

	mvBaseMatrices.resize(mpMeshEntity->GetSubMeshEntityNum());
	for(int i=0; i<mpMeshEntity->GetSubMeshEntityNum(); ++i)
	{
		mvBaseMatrices[i] = mpMeshEntity->GetSubMeshEntity(i)->GetLocalMatrix();
	}

	
	mvSwarmPoint = mpBody->GetWorldPosition();

	if(mpMeshEntity->GetAnimationStateNum() >0)
	{
		mpMeshEntity->Play(0, true,true);
	}

	if(mfAttackSizeRadius > 0)
		mpDamageShape = apWorld->GetPhysicsWorld()->CreateSphereShape(mfAttackSizeRadius, NULL);
	else
		mpDamageShape = NULL;


	mvGroundNormal = cMath::MatrixMul(mpBody->GetLocalMatrix().GetRotation(), mvGroundNormal);
	cVector3f vForward = cMath::MatrixMul(mpBody->GetLocalMatrix().GetRotation(), cVector3f(0,0,-1));
	cVector3f vRight;
	CreateOrthoVectors(vRight, mvGroundNormal, vForward);

	mlstFwdDirs.push_back(vForward);

	//Remove any rotation from the body!
	cMatrixf mtxNoRotation = cMatrixf::Identity;
	mtxNoRotation.SetTranslation(mpBody->GetLocalPosition());
	mpBody->SetMatrix(mtxNoRotation);
}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::OnResetProperties()
{

}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::UpdatePropSpecific(float afTimeStep)
{
	/////////////////////////////////////
	//If pLayer is far away do not update
	cVector3f vPlayerPos = gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition();
	float fPlayerDistanceSqr = cMath::Vector3DistSqr(vPlayerPos, mpBody->GetWorldPosition());
	if(fPlayerDistanceSqr > 15*15) return;

	UpdateCritterSpecific(afTimeStep);

	UpdateMovement(afTimeStep);	
	UpdateMesh(afTimeStep);

}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::BeforePropDestruction()
{
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair iLuxProp_CritterBase::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return eLuxFocusCrosshair_Default;
}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::OnRenderSolid(cRendererCallbackFunctions* apFunctions)
{
	return;
	iPhysicsWorld *pPhysicsWorld = mpWorld->GetPhysicsWorld();

	pPhysicsWorld->RenderShapeDebugGeometry(mpBody->GetShape(), mpBody->GetLocalMatrix(), apFunctions->GetLowLevelGfx(), cColor(1,1));
	
	apFunctions->GetLowLevelGfx()->DrawLine(mpBody->GetLocalPosition(), mpBody->GetLocalPosition()+mvGroundNormal*0.5f,cColor(1,0,0,1));
	cVector3f vFwdDir = cMath::Vector3Normalize(mvVel==0 ? mlstFwdDirs.back() : mvVel);
	apFunctions->GetLowLevelGfx()->DrawLine(mpBody->GetLocalPosition(), mpBody->GetLocalPosition()+vFwdDir*0.5f,cColor(0,1,0,1));

	if(mpDamageShape)
	{
		//cVector3f vDir = cMath::Vector3Normalize(gpBase->mpPlayer->GetCharacterBody()->GetPosition() - mpBody->GetLocalPosition());
		//pPhysicsWorld->RenderShapeDebugGeometry(mpDamageShape, GetAttackMatrix(vDir),apFunctions->GetLowLevelGfx(), cColor(1,1));
	}


	/*for(int i=0; i<mpMeshEntity->GetSubMeshEntityNum(); ++i)
	{
		cSubMeshEntity *pSubEnt = mpMeshEntity->GetSubMeshEntity(i);
		cBoundingVolume *pBV =  pSubEnt->GetBoundingVolume();
		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(), pBV->GetMax(),cColor(1,1));
	}*/
}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::OnHealthChange()
{
	///////////////
	// Death
	if(mfHealth <=0) 
	{
		cWorld *pWorld = mpMap->GetWorld();

		if(msDeathSound != "")
			PlaySound("CritterDeath",msDeathSound, true, true);

		if(msDeathPS != "")
		{
			cParticleSystem *pPS = pWorld->CreateParticleSystem("CritterDeath", msDeathPS,1);
			if(pPS)
			{
				pPS->SetPosition(mpBody->GetLocalPosition());
			}
		}

		if(msDeathEntity != "")
		{
			pWorld->CreateEntity(msName+"_Dead", mpMeshEntity->GetWorldMatrix(), msDeathEntity);
		}

		OnKillCritter();

		mpMap->DestroyEntity(this);
	}
}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::OnDamage(float afAmount, int alStrength)
{
	OnDamageCritter(afAmount);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::CreateOrthoVectors(cVector3f &avRight, cVector3f& avUp, cVector3f &avForward)
{
	if(avForward == avUp || avForward == avUp*-1)
	{
		avForward.x = avUp.z;
		avForward.y = -avUp.x;
		avForward.z = avUp.x;
	}
	avRight = cMath::Vector3Cross(avUp, avForward);
	avRight.Normalize();
	avForward = cMath::Vector3Cross(avRight, avUp);
	avForward.Normalize();
}
//-----------------------------------------------------------------------

float iLuxProp_CritterBase::GetDistanceToPlayer()
{
	return cMath::Vector3Dist(gpBase->mpPlayer->GetCharacterBody()->GetPosition(),mpBody->GetLocalPosition());
}

float iLuxProp_CritterBase::GetDistanceToPlayer2D()
{
	return cMath::Vector2DistXZ(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition(),mpBody->GetLocalPosition());
}

//-----------------------------------------------------------------------

cMatrixf iLuxProp_CritterBase::GetAttackMatrix(const cVector3f& avDir)
{
	/*if(mvVel ==0)
	{
		if(mlstFwdDirs.empty())		vForward = cVector3f(0,0,1);
		else						vForward = mlstFwdDirs.back();
	}
	else
	{
		vForward = cMath::Vector3Normalize(mvVel);
	}

	cVector3f vRight;
	CreateOrthoVectors(vRight, mvGroundNormal, vForward);

	cMatrixf mtxTemp = cMatrixf::Identity;
	mtxTemp.SetForward(vForward);
	mtxTemp.SetUp(mvGroundNormal);
	mtxTemp.SetRight(vRight);

	cMatrixf mtxOffset = mtxTemp.GetTranspose();*/

	cVector3f vPos =	mpBody->GetLocalPosition() + avDir * mvAttackOffset.z;

	cMatrixf mtxOffset = cMatrixf::Identity;
	mtxOffset.SetTranslation(vPos);

	return mtxOffset;
}

//-----------------------------------------------------------------------

bool iLuxProp_CritterBase::Attack(const cVector3f& avDir)
{
	if(mpDamageShape==NULL) return false;

	bool bHit = gpBase->mpMapHelper->ShapeDamage(mpDamageShape, GetAttackMatrix(avDir), mpBody->GetLocalPosition(),
												mvAttackDamageMinMax.x, mvAttackDamageMinMax.y, 
												mvAttackForce.x, mvAttackForce.y, mlAttackStrength, 3, 
												eLuxDamageType_BloodSplat, eLuxWeaponHitType_Sword,
												false, true, false, false);	
	if(bHit)
	{
		PlaySound("CritterAttack",msAttackHitSound,true, true);
	}
	return bHit;
}

//-----------------------------------------------------------------------

cVector3f iLuxProp_CritterBase::GetWanderAdd(float afLength, float afRadius, float afTimeStep)
{
	cVector3f vDir;
	if(mvVel==0)
	{
		if(mlstFwdDirs.empty())
		{
			float fAngle = cMath::RandRectf(0, k2Pif);
			vDir = cMath::MatrixMul(cMath::MatrixRotateY(fAngle),cVector3f(1,0,0));
		}
		else
		{
			vDir = mlstFwdDirs.back();
		}
	}
	else
	{
		vDir = cMath::Vector3Normalize(mvVel);
	}
	vDir = vDir - mvGroundNormal * cMath::Vector3Dot(mvGroundNormal, vDir);//Only want the direction in the current movement plane.

	float fAngle = cMath::RandRectf(0, k2Pif);

	cQuaternion qRotation;
	qRotation.FromAngleAxis(fAngle, mvGroundNormal);

	cVector3f vForce = cMath::MatrixMul(cMath::MatrixQuaternion(qRotation),cVector3f(2,0,0));

	return (vDir*1 + vForce) * afTimeStep;
}

//-----------------------------------------------------------------------

cVector3f iLuxProp_CritterBase::GetTowardPlayerAdd(bool abDependOnDistance, float afTimeStep)
{
	cVector3f vToPlayer = gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition() - mpBody->GetLocalPosition();
	float fToPlayerDist = vToPlayer.Length();
	vToPlayer.Normalize();
	vToPlayer = vToPlayer - mvGroundNormal * cMath::Vector3Dot(mvGroundNormal, vToPlayer);//Only want the direction in the current movement plane.

	cVector3f vAdd = vToPlayer * afTimeStep;
	if(abDependOnDistance) vAdd = vAdd * (1.0f/fToPlayerDist);

	return vAdd;
}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::UpdateMovement(float afTimeStep)
{
	if(mvVel ==0) return;
	
	////////////////////////
	// Move the body
	cVector3f vVelAdd = mvVel * afTimeStep;
	cVector3f vGravityAdd = mvGravityVel * afTimeStep;
	
	if(mbUseRayCollision)
	{
		CheckRayCollision(vVelAdd, vGravityAdd, afTimeStep);
	}
	else
	{
		CheckShapeCollision(vVelAdd, vGravityAdd, afTimeStep);
	}
}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::UpdateMesh(float afTimeStep)
{
	if(mvVel!=0)
	{
		mlstFwdDirs.push_back(cMath::Vector3Normalize(mvVel));
		if(mlstFwdDirs.size() > 20) mlstFwdDirs.pop_front();
	}
	
	////////////////////////////
	// Get smooth forward
	cVector3f vForward = cVector3f(0,0,1);
	if(mlstFwdDirs.empty()==false)
	{
		cVector3f vTotalFwd=0;
		for(tVector3fListIt it = mlstFwdDirs.begin(); it != mlstFwdDirs.end(); ++it)
		{
			vTotalFwd += *it;
		}
		
		vForward = vTotalFwd / (float)mlstFwdDirs.size();
	}

	////////////////////////////
	// Get smooth up
	mlstUpDirs.push_back(mvGroundNormal);
	if(mlstUpDirs.size() > 40) mlstUpDirs.pop_front();
	cVector3f vTotalUp=0;
	for(tVector3fListIt it = mlstUpDirs.begin(); it != mlstUpDirs.end(); ++it)
	{
		vTotalUp += *it;
	}	
	cVector3f vUp = vTotalUp / (float)mlstUpDirs.size();
	
	////////////////////////////
	// Create matrix
	cVector3f vRight;
	CreateOrthoVectors(vRight, vUp, vForward);
	
	cMatrixf mtxRotate = cMatrixf::Identity;
	mtxRotate.SetForward(vForward);
	mtxRotate.SetUp(vUp);
	mtxRotate.SetRight(vRight);

	//////////////////////////////
	//Mesh Entity orientation
	if(mpMeshEntity->GetMesh()->GetSkeleton())
	{
		cMatrixf mtxRot = cMath::MatrixMul(cMath::MatrixMul(mtxRotate.GetTranspose(), cMath::MatrixRotateY(kPif)), m_mtxMeshOffset);

		mpMeshEntity->SetMatrix(mtxRot);
	}
	else
	{
		for(int i=0; i<mpMeshEntity->GetSubMeshEntityNum(); ++i)
		{
			cMatrixf mtxLocal = cMath::MatrixMul(cMath::MatrixMul(mtxRotate.GetTranspose(),m_mtxMeshOffset), mvBaseMatrices[i]);
			//cMatrixf mtxLocal = cMath::MatrixMul(mtxRotate.GetTranspose(),mvBaseMatrices[i]);
			
			mpMeshEntity->GetSubMeshEntity(i)->SetMatrix(mtxLocal);
		}
	}

	//////////////////////////////
	//Animation
	if(mpMeshEntity->GetAnimationStateNum() >0 && mbUpdateAnimation)
	{
		float fSpeed = mvVel.Length();

		/////////////
		// Idle
		if(mlAnimState==0)
		{
			if(fSpeed > 0.1)
			{
				mpMeshEntity->PlayFadeToName("Walk", true, 0.3f);
				mlAnimState = 1;
			}
		}
		/////////////
		// Walk
		else if(mlAnimState==1)
		{
			cAnimationState *pAnim = mpMeshEntity->GetAnimationStateFromName("Walk");
			if(pAnim)
			{
				pAnim->SetSpeed(fSpeed);
			}

			if(fSpeed < 0.05f)
			{
				mpMeshEntity->PlayFadeToName("Idle", true, 0.3f);
				mlAnimState =0;
			}
		}
        
	}
}


//-----------------------------------------------------------------------

void iLuxProp_CritterBase::CheckRayCollision(const cVector3f& avVelAdd,const cVector3f& avGravityAdd, float afTimeStep)
{
	iPhysicsWorld *pPhysicsWorld = mpWorld->GetPhysicsWorld();
	cVector3f vPos = mpBody->GetWorldPosition();
	bool bCollided = false;

	//Vel
	if(avVelAdd != 0)
	{
		mpRayCallback->Reset();

		cVector3f vDir = cMath::Vector3Normalize(avVelAdd);
		cVector3f vExtraAdd = vDir * mpBody->GetBoundingVolume()->GetSize().x * 0.5f;
		
		pPhysicsWorld->CastRay(mpRayCallback, vPos, vPos + avVelAdd + vExtraAdd, false,true,true, true);
		if(mpRayCallback->GetIntersected())
		{
			cVector3f vNormal = mpRayCallback->GetNormal();
			vNormal.y =0; vNormal.Normalize();
			//vPos = mpRayCallback->GetPos() + vNormal*mpBody->GetShape()->GetRadius();

			mvVel = mvVel - vNormal * cMath::Vector3Dot(vNormal, mvVel);
			mvVel.y =0;
		}
		else
		{
			vPos += avVelAdd;
		}
	}
	//Gravity
	if(avGravityAdd != 0)
	{
		mpRayCallback->Reset();

		cVector3f vDest = vPos + mpBody->GetBoundingVolume()->GetSize().y*0.5f + avGravityAdd;
		pPhysicsWorld->CastRay(mpRayCallback, vPos, vPos + mpBody->GetBoundingVolume()->GetSize().y*-0.5f + avGravityAdd, false,true,true, true);
		if(mpRayCallback->GetIntersected())
		{
			mvGravityVel =0;
		}
		else
		{
			vPos += avGravityAdd;
		}
	}

	mpBody->SetPosition(vPos);
}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::CheckShapeCollision(const cVector3f& avVelAdd,const cVector3f& avGravityAdd, float afTimeStep)
{
	iPhysicsWorld *pPhysicsWorld = mpWorld->GetPhysicsWorld();
	cVector3f vPos = mpBody->GetWorldPosition();
	

	mbColliding = false;

	/////////////////////////////////////
	// Position
	if(avVelAdd != 0)
	{
		vPos += avVelAdd;

		cVector3f vPushVec=0;
		bool bCollide = pPhysicsWorld->CheckShapeWorldCollision(&vPushVec, mpBody->GetShape(), cMath::MatrixTranslate(vPos), mpBody);

		if(bCollide)
		{
			//if(cMath::Vector3Dot(vPushVec, mvGroundNormal)>0)
			{
				mbColliding = true;
			}

			if(vPushVec != 0)
			{
				vPos += vPushVec;
				
				cVector3f vPushDir = cMath::Vector3Normalize(vPushVec);

				OnShapeCollision(vPushVec, afTimeStep);
				
				mvVel = mvVel - vPushDir * cMath::Vector3Dot(vPushDir, mvVel);
			}
			//mvVel.y=0;
		}
	}
	/////////////////////////////////////
	// Gravity
	if(avGravityAdd != 0)
	{
		vPos += avGravityAdd;

		cVector3f vPushVec=0;
		bool bCollide = pPhysicsWorld->CheckShapeWorldCollision(&vPushVec, mpBody->GetShape(), cMath::MatrixTranslate(vPos), mpBody);
		if(bCollide)
		{
			mbColliding = true;
			
			if(vPushVec != 0)
			{
				vPos += vPushVec;

				cVector3f vPushDir = cMath::Vector3Normalize(vPushVec);

				OnShapeCollision(vPushVec, afTimeStep);

				mvGravityVel = mvGravityVel - vPushDir * cMath::Vector3Dot(vPushDir, mvGravityVel);
				mvGravityVel = mvGroundNormal * cMath::Vector3Dot(mvGravityVel, mvGroundNormal);
			}
		}
	}
	
	mpBody->SetPosition(vPos);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(iLuxProp_CritterBase_SaveData, iLuxProp_SaveData)
kSerializeVar(mvVel, eSerializeType_Vector3f)
kSerializeVar(mvGravityVel, eSerializeType_Vector3f)
kSerializeVar(mvSwarmPoint, eSerializeType_Vector3f)
kSerializeVar(mlAnimState, eSerializeType_Int32)
kSerializeVar(mvGroundNormal, eSerializeType_Vector3f)
kSerializeVar(mbColliding, eSerializeType_Bool)
kSerializeVar(mbUpdateAnimation, eSerializeType_Bool)
kEndSerialize()

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	iLuxProp_CritterBase_SaveData *pData = static_cast<iLuxProp_CritterBase_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData,mvVel);
	kCopyToVar(pData,mvGravityVel);
	kCopyToVar(pData,mvSwarmPoint);
	kCopyToVar(pData,mlAnimState);
	kCopyToVar(pData,mvGroundNormal);
	kCopyToVar(pData,mbColliding);
	kCopyToVar(pData,mbUpdateAnimation);
	
}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	iLuxProp_CritterBase_SaveData *pData = static_cast<iLuxProp_CritterBase_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData,mvVel);
	kCopyFromVar(pData,mvGravityVel);
	kCopyFromVar(pData,mvSwarmPoint);
	kCopyFromVar(pData,mlAnimState);
	kCopyFromVar(pData,mvGroundNormal);
	kCopyFromVar(pData,mbColliding);
	kCopyFromVar(pData,mbUpdateAnimation);
}

//-----------------------------------------------------------------------

void iLuxProp_CritterBase::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
