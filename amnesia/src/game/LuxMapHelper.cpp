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

#include "LuxMapHelper.h"

#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxEnemy.h"

#include "LuxPlayer.h"
#include "LuxPlayerHelpers.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// LINE OF SIGHT CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxLineOfSightCallback::Reset()
{
	mbIntersected = false;
}

//-----------------------------------------------------------------------

bool cLuxLineOfSightCallback::BeforeIntersect(iPhysicsBody *apBody)
{
	if(apBody->IsCharacter() || apBody->GetCollide()==false || (mbCheckShadow && apBody->GetBlocksLight()==false) )
	{
		return false;
	}

	iLuxEntity* pEntity = (iLuxEntity*)apBody->GetUserData();
	if(pEntity && pEntity->GetMeshEntity())
	{
		cMeshEntity *pMeshEntity = pEntity->GetMeshEntity();
		if(pMeshEntity)
		{
			bool bFoundSolid = false;
			for(int i=0; i< pMeshEntity->GetSubMeshEntityNum(); ++i)
			{
				cSubMeshEntity *pSubMeshEnt = pMeshEntity->GetSubMeshEntity(i);
				if(pSubMeshEnt->GetEntityParent() != (iEntity3D*)apBody)
				{ 
					continue;
				}

				cMaterial *pMaterial = pSubMeshEnt->GetMaterial(); 
				if(	pMaterial && pMaterial->GetType()->IsTranslucent()==false &&
					(mbCheckShadow==false || pSubMeshEnt->GetRenderFlagBit(eRenderableFlag_ShadowCaster)) )
				{
					bFoundSolid = true;
					break;
				}
			}
			if(bFoundSolid==false)
			{
				//if(bDebug)Log("Out on non solid!\n");
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------

bool cLuxLineOfSightCallback::OnIntersect(iPhysicsBody *pBody, cPhysicsRayParams *apParams)
{
	mbIntersected = true;
	return false;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CLOSEST ENTITY CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxClosestEntityCallback::Reset()
{
	mfClosestDist = 9999999.0f;
	mpClosestBody = NULL;
	mlCheckCount =0;
}

//-----------------------------------------------------------------------

bool cLuxClosestEntityCallback::BeforeIntersect(iPhysicsBody *apBody)
{
	//LogUpdate("Testing: %s\n", apBody->GetName().c_str());

	mlCheckCount++;

	iLuxEntity* pEntity = (iLuxEntity*)apBody->GetUserData();
	if(pEntity)
	{
		if(pEntity->IsActive()==false) return false;
		if(apBody->GetCollide()==false && pEntity->CanInteract(apBody)==false) return false;
	}
	else
	{
		if(apBody->IsCharacter()) return false; 
		if(apBody->GetCollide()==false) return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------

bool cLuxClosestEntityCallback::OnIntersect(iPhysicsBody *apBody,cPhysicsRayParams *apParams)
{
	if(apParams->mfDist < mfClosestDist)
	{
		mfClosestDist = apParams->mfDist;
        mpClosestBody = apBody;
	}

	return true;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CLOSEST CHAR COLLIDER CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxClosestCharColliderCallback::Reset()
{
	mfClosestDist = 9999999.0f;
	mpClosestBody = NULL;
	mlCheckCount =0;
	mvClosestNormal =0;
}

//-----------------------------------------------------------------------

bool cLuxClosestCharColliderCallback::BeforeIntersect(iPhysicsBody *apBody)
{
	return apBody->GetCollideCharacter() && apBody->IsCharacter()==false;
}

//-----------------------------------------------------------------------

bool cLuxClosestCharColliderCallback::OnIntersect(iPhysicsBody *apBody,cPhysicsRayParams *apParams)
{
	if(apParams->mfDist < mfClosestDist)
	{
		mfClosestDist = apParams->mfDist;
		mvClosestNormal = apParams->mvNormal;
		mpClosestBody = apBody;
	}

	return true;
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// ATTACK RAY CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxAttackRayCallback::Setup(iPhysicsBody *apSkipBody)
{
	mpSkipBody = apSkipBody;
	mbIntersection = false;
}

//-----------------------------------------------------------------------

bool cLuxAttackRayCallback::BeforeIntersect(iPhysicsBody *apBody)
{
	if(apBody->IsCharacter() || apBody->GetCollide()==false || mpSkipBody == apBody)
	{
		return false;
	}
	return true;
}

bool cLuxAttackRayCallback::OnIntersect(iPhysicsBody *apBody,cPhysicsRayParams *apParams)
{
	mbIntersection = true;

	return false;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMapHelper::cLuxMapHelper() : iLuxUpdateable("LuxMapHelper")
{
	
}

//-----------------------------------------------------------------------

cLuxMapHelper::~cLuxMapHelper()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMapHelper::OnStart()
{
	
}

//-----------------------------------------------------------------------

void cLuxMapHelper::Update(float afTimeStep)
{
}

//-----------------------------------------------------------------------

void cLuxMapHelper::Reset()
{

}

//-----------------------------------------------------------------------

bool cLuxMapHelper::ShapeDamage(iCollideShape *apShape, const cMatrixf& a_mtxTransform, const cVector3f &avOrigin,
								float afMinDamage, float afMaxDamage, float afForce, float afMaxImpulse,
								int alStrength, float afHitSpeed, eLuxDamageType aDamageType,eLuxWeaponHitType aWeaponHitType,
								bool abCheckEnemies, bool abCheckPlayer, bool abCheckProps, bool abLethalForPlayer,
								bool *apHitPlayer)
{
	cLuxMap *pCurrentMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pCurrentMap==NULL) return  false;	


	if(apHitPlayer) *apHitPlayer = false;

	///////////////////////////////
	//Set up
	cWorld *pWorld = pCurrentMap->GetWorld();
	iPhysicsWorld *pPhysicsWorld = pCurrentMap->GetPhysicsWorld();

	cBoundingVolume shapeBV =  apShape->GetBoundingVolume();
	shapeBV.SetTransform(cMath::MatrixMul(a_mtxTransform, shapeBV.GetTransform()));
	
	std::vector<iPhysicsBody*> vBodies;
	pPhysicsWorld->GetBodiesInBV(&shapeBV, &vBodies);
	if(vBodies.empty()) return false;

    cCollideData collideData;
	collideData.SetMaxSize(4);

	tVector3fList lstHitPositions;

	bool bHit=false;

	///////////////////////////////
	//Iterate
	for(size_t i=0; i<vBodies.size(); ++i)
	{
		iPhysicsBody *pBody = vBodies[i];

		if(pBody->IsActive()==false) continue;
		if(pBody->GetCollide()==false) continue;

		///////////////////////
		//Check if valid
		if(pBody->GetUserData() && abCheckProps==false) continue;
		if(pBody->IsCharacter())
		{
			if(pBody == gpBase->mpPlayer->GetCharacterBody()->GetCurrentBody())
			{
				if(abCheckPlayer == false) continue;
			}
			else
			{
				if(abCheckEnemies == false) continue;
			}
		}

		///////////////////////
		//Check collision
		if(cMath::CheckBVIntersection(shapeBV, *pBody->GetBoundingVolume())==false)
		{			
			continue;
		}
		
		bool bCollide = pPhysicsWorld->CheckShapeCollision(apShape, a_mtxTransform, pBody->GetShape(), pBody->GetLocalMatrix(),
															collideData,4, false);
		if(bCollide==false) continue;

		///////////////////////
		//Check ray cast
		mAttackRayCallback.Setup(pBody);

		pPhysicsWorld->CastRay(&mAttackRayCallback, avOrigin, pBody->GetWorldPosition(), false, false, false, true);
		if(mAttackRayCallback.mbIntersection) continue;
		
		//////////////////////
		//Set up entities
		bHit = true;
		iLuxEntity *pEntity = NULL;
		
		if(pBody->IsCharacter())
			pEntity = (iLuxEntity*)pBody->GetCharacterBody()->GetUserData();
		else
			pEntity = (iLuxEntity*)pBody->GetUserData();

		///////////////////////
		//Calculate the hit position
		cVector3f vHitPos =0;
		for(int i=0; i < collideData.mlNumOfPoints; ++i)
		{
			vHitPos += collideData.mvContactPoints[i].mvPoint;
		}
		vHitPos = vHitPos / (float)collideData.mlNumOfPoints;
		lstHitPositions.push_back(vHitPos);

        ///////////////////////
		//Add Force
		if(pBody->GetMass() > 0 || pBody->IsCharacter())
		{
			cVector3f vDir = cMath::Vector3Normalize(vHitPos - avOrigin);
			
			if(pBody->IsCharacter())
			{
				pBody->GetCharacterBody()->AddForceVelocity(vDir * afForce * 0.1f);
			}
			else
			{
				float fImpulse = cMath::Min(afForce / pBody->GetMass(), afMaxImpulse);
				pBody->AddImpulseAtPosition(vDir * fImpulse, vHitPos);
			}
		}
		
		///////////////////////
		//Add sound and effect
		
		////////////////////////
		// Enemy
		if(pEntity && pEntity->GetEntityType() == eLuxEntityType_Enemy)
		{
			iLuxEnemy *pEnemy = static_cast<iLuxEnemy*>(pEntity);
			
			//Sound
            if(pEnemy->GetHitSound(aWeaponHitType) != "")
			{
				cSoundEntity *pSound = pWorld->CreateSoundEntity("HitSound",pEnemy->GetHitSound(aWeaponHitType), true);
				if(pSound)pSound->SetPosition(vHitPos);
			}

			//Particle System
			if(pEnemy->GetHitPS(aWeaponHitType) != "")
			{
				cParticleSystem *pPS = pWorld->CreateParticleSystem("HitPS", pEnemy->GetHitPS(aWeaponHitType),1);
				if(pPS)pPS->SetPosition(vHitPos);
			}

		}
		////////////////////////
		// Normal body
		else
		{
			cSurfaceData *pSurface = pBody->GetMaterial()->GetSurfaceData();
			if(pSurface)
			{
				cSurfaceImpactData *pImpact= pSurface->GetHitDataFromSpeed(afHitSpeed);
				if(pImpact)
				{
					//Sound
					if(pImpact->GetSoundName()!="") 
					{
						cSoundEntity *pSound = pWorld->CreateSoundEntity("HitSound",pImpact->GetSoundName(), true);
						if(pSound)pSound->SetPosition(vHitPos);
					}
					//Particle system
					if(pImpact->GetPSName()!="") 
					{
						cParticleSystem *pPS = pWorld->CreateParticleSystem("HitPS", pImpact->GetPSName(),1);
						if(pPS)pPS->SetPosition(vHitPos);
					}
				}
			}
		}
		
		///////////////////////
		//Add Damage
		float fDamage = cMath::RandRectf(afMinDamage, afMaxDamage);
		if(pEntity)
		{
			pEntity->GiveDamage(fDamage,alStrength);
		}
		else if(pBody == gpBase->mpPlayer->GetCharacterBody()->GetCurrentBody())
		{
			gpBase->mpPlayer->GiveDamage(fDamage,alStrength, aDamageType, true, abLethalForPlayer);
			if(apHitPlayer) *apHitPlayer = true;
		}

		

	}

	///////////////////////
	//Get Closes Position

	return bHit;
}

//-----------------------------------------------------------------------


bool cLuxMapHelper::CheckLineOfSight(const cVector3f& avStart, const cVector3f& avEnd, bool abCheckShadows)
{
	////////////////////////////
	//Check so there really is a world
	cLuxMap *pCurrentMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pCurrentMap==NULL) return false;

	iPhysicsWorld *pPhysicsWorld = pCurrentMap->GetPhysicsWorld();

	mLineOfSightCallback.Reset();
	mLineOfSightCallback.SetCheckShadow(abCheckShadows);
	pPhysicsWorld->CastRay(	&mLineOfSightCallback, avStart,avEnd,false,false,false,true);

	return mLineOfSightCallback.GetIntersected()==false;
}
//-----------------------------------------------------------------------

bool cLuxMapHelper::GetClosestEntity(	const cVector3f& avStart,const cVector3f& avDir, float afRayLength,
										float *afDistance, iPhysicsBody** apBody, iLuxEntity **apEntity)
{
	cLuxMap *pCurrentMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pCurrentMap==NULL) return false;

	iPhysicsWorld *pPhysicsWorld = pCurrentMap->GetPhysicsWorld();

	mClosestEntityCallback.Reset();
	cVector3f vEnd = avStart + avDir*afRayLength;
	pPhysicsWorld->CastRay(	&mClosestEntityCallback, avStart,vEnd,true,false,false,true);

	//LogUpdate(" - Bodies checked: %d, Length: %f\n", mClosestEntityCallback.mlCheckCount, (vEnd - avStart).Length());

	iPhysicsBody *pBodyFound = mClosestEntityCallback.mpClosestBody;
	if(afDistance)	*afDistance = mClosestEntityCallback.mfClosestDist;
	if(apBody)		*apBody = pBodyFound;
	if(apEntity)	*apEntity = pBodyFound ? (iLuxEntity*)pBodyFound->GetUserData() : NULL;

	return *apEntity != NULL;
}

//-----------------------------------------------------------------------

bool cLuxMapHelper::GetClosestCharCollider(const cVector3f& avStart,const cVector3f& avDir, float afRayLength, float *afDistance, cVector3f *avNormal, iPhysicsBody** apBody)
{
	cLuxMap *pCurrentMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pCurrentMap==NULL) return false;

	iPhysicsWorld *pPhysicsWorld = pCurrentMap->GetPhysicsWorld();

	mClosestharColliderCallback.Reset();
	cVector3f vEnd = avStart + avDir*afRayLength;
	pPhysicsWorld->CastRay(	&mClosestharColliderCallback, avStart,vEnd,true,true,false,true);

	iPhysicsBody *pBodyFound = mClosestharColliderCallback.mpClosestBody;
	if(afDistance)	*afDistance = mClosestharColliderCallback.mfClosestDist;
	if(avNormal)	*avNormal = mClosestharColliderCallback.mvClosestNormal;
	if(apBody)		*apBody = pBodyFound;
	
	return  pBodyFound!=NULL;
}

//-----------------------------------------------------------------------

static float GetMaxRGB(const cColor &aCol)
{
	float fAmount = aCol.r;
	if(fAmount < aCol.g) fAmount = aCol.g;
	if(fAmount < aCol.b) fAmount = aCol.b;

	return fAmount;
}

float cLuxMapHelper::GetLightLevelAtPos(const cVector3f& avPos, std::vector<iLight*>* apSkipLightsVec, float afRadiusAdd)
{
	////////////////////////////
	//Check so there really is a world
	cLuxMap *pCurrentMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pCurrentMap==NULL) return 0.0f;

	
	////////////////////////////
	//Setup data
	tLightList lstIntersectingLights; 

	cWorld *pWorld = pCurrentMap->GetWorld();
	iPhysicsWorld *pPhysicsWorld = pCurrentMap->GetPhysicsWorld();
	
	float fLightLevel =0;

	iLight *pPlayerAmbLight = gpBase->mpPlayer->GetHelperInDarkness()->GetAmbientLight();
	
	////////////////////////////
	//Get lights from world
	GetLightsAtNode(pWorld->GetRenderableContainer(eWorldContainerType_Static)->GetRoot(), lstIntersectingLights, avPos);
	GetLightsAtNode(pWorld->GetRenderableContainer(eWorldContainerType_Dynamic)->GetRoot(), lstIntersectingLights, avPos);

	////////////////////////////
	//Iterate lights and get lightlevel from each
	tLightListIt it = lstIntersectingLights.begin();
	for(; it != lstIntersectingLights.end(); ++it)
	{
		iLight *pLight = *it;

		if(pLight == pPlayerAmbLight) continue;
		
		///////////////////////////
		//Check if the light is on the skip list
        if(apSkipLightsVec)
		{
			bool bSkip=false;
			for(size_t i=0; i<apSkipLightsVec->size(); ++i)
			{
				iLight *pTestLight = (*apSkipLightsVec)[i];
                if(pTestLight == pLight) 
				{
					bSkip = true;
					break;
				}
			}
			if(bSkip) continue;
		}

		
		///////////////////////////
		//Box light
		if(pLight->GetLightType() == eLightType_Box)
		{
			fLightLevel += GetMaxRGB(pLight->GetDiffuseColor());
		}
		///////////////////////////
		//Spot and Point
		else
		{
			//Check line of sight
			if(	pLight->GetLightType() == eLightType_Spot && pLight->GetCastShadows() &&
				CheckLineOfSight(pLight->GetWorldPosition(),avPos, true)==false)
			{
				continue;
			}

			//Get highest value of rg b
			float fAmount = GetMaxRGB(pLight->GetDiffuseColor());

			//Get distance to the light
			float fDist = cMath::Vector3Dist(pLight->GetWorldPosition(), avPos);

			//Calculate attenuation
			float fT = 1 - fDist / (pLight->GetRadius() + afRadiusAdd);
			if(fT<0)fT =0;
			fAmount *= fT;

			fLightLevel += fAmount;
		}
  	}

	return fLightLevel;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMapHelper::GetLightsAtNode(iRenderableContainerNode *apNode, tLightList &alstLights, const cVector3f& avPos)
{
	///////////////////////////////////////
	//Make sure node is updated and check point in bv
	apNode->UpdateBeforeUse();

	////////////////////////
	//Iterate children
	if(apNode->HasChildNodes())
	{
		tRenderableContainerNodeListIt childIt = apNode->GetChildNodeList()->begin();
		for(; childIt != apNode->GetChildNodeList()->end(); ++childIt)
		{
			iRenderableContainerNode *pChildNode = *childIt;
			
			//Make sure point is in node AABB.
			if(cMath::CheckPointInAABBIntersection(avPos, apNode->GetMin(),apNode->GetMax()) )
			{
				GetLightsAtNode(pChildNode, alstLights, avPos);
			}
		}
	}

	/////////////////////////////
	//Iterate objects
	if(apNode->HasObjects())
	{
		tRenderableListIt it = apNode->GetObjectList()->begin();
		for(; it != apNode->GetObjectList()->end(); ++it)
		{
			iRenderable *pObject = *it;
			if(pObject->IsVisible()==false || pObject->GetRenderType() != eRenderableType_Light) continue;

			bool bAdd = false;

			iLight *pLight = static_cast<iLight*>(pObject);
			switch(pLight->GetLightType())
			{
			case eLightType_Box:
				bAdd = cMath::CheckPointInBVIntersection(avPos, *pLight->GetBoundingVolume());
				break;
			case eLightType_Point:
				bAdd = cMath::CheckPointInSphereIntersection(avPos, pLight->GetWorldPosition(), pLight->GetRadius());
				break;
			case eLightType_Spot:
				cLightSpot *pSpotLight = static_cast<cLightSpot*>(pLight);
				bAdd = pSpotLight->GetFrustum()->CollidePoint(avPos);
				break;
			}

			if(bAdd) alstLights.push_back(pLight);
		}
	}

}

//-----------------------------------------------------------------------



