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

#include "LuxArea_Liquid.h"

#include "LuxMap.h"
#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxEnemy.h"


//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaLoader_Liquid::cLuxAreaLoader_Liquid(const tString& asName) : iLuxAreaLoader(asName)
{

}

cLuxAreaLoader_Liquid::~cLuxAreaLoader_Liquid()
{

}

//-----------------------------------------------------------------------

iLuxArea *cLuxAreaLoader_Liquid::CreateArea(const tString& asName, int alID, cLuxMap *apMap)
{
	cLuxArea_Liquid *pArea = hplNew(cLuxArea_Liquid, (asName, alID, apMap));
	return pArea;
}

//-----------------------------------------------------------------------

void cLuxAreaLoader_Liquid::LoadVariables(iLuxArea *apArea, cWorld *apWorld)
{
	cLuxArea_Liquid *pLiquidArea = static_cast<cLuxArea_Liquid*>(apArea);

	pLiquidArea->mfDensity = GetVarFloat("Density",0);
	pLiquidArea->mfLinearViscosity = GetVarFloat("LinearViscosity",0);
	pLiquidArea->mfAngularViscosity = GetVarFloat("AngularViscosity",0);

	tString sPhysicsMaterial = GetVarString("PhysicsMaterial", "");
	pLiquidArea->mpPhysicsMaterial = apWorld->GetPhysicsWorld()->GetMaterialFromName(sPhysicsMaterial);
	if(pLiquidArea->mpPhysicsMaterial ==NULL)
		Error("Could not load physics material '%s' for liquid area '%s'!\n",sPhysicsMaterial.c_str(), pLiquidArea->msName.c_str());

	pLiquidArea->mbHasWaves = GetVarBool("HasWaves",false);
	pLiquidArea->mfWaveAmp = GetVarFloat("WaveAmp",0);
	pLiquidArea->mfWaveFreq = GetVarFloat("WaveFreq",0);

	pLiquidArea->mfPlayerSpeedMul = GetVarFloat("PlayerSpeedMul",0.8f);

	float fMaxWaveDistance = GetVarFloat("MaxWaveDistance",25);
	pLiquidArea->mfMaxWaveDistanceSqr = fMaxWaveDistance*fMaxWaveDistance;
}

void cLuxAreaLoader_Liquid::SetupArea(iLuxArea *apArea, cWorld *apWorld)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxArea_Liquid::cLuxArea_Liquid(const tString &asName, int alID, cLuxMap *apMap)  : iLuxArea(asName,alID,apMap, eLuxAreaType_Liquid)
{
	mpPhysicsMaterial = NULL;
	mbFullGameSave = true;

	mfTimeCount = 0;
}

//-----------------------------------------------------------------------

cLuxArea_Liquid::~cLuxArea_Liquid()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxArea_Liquid::SetupAfterLoad(cWorld *apWorld)
{
	float fHeight = mpBody->GetShape()->GetSize().y;
	cVector3f vPos = mpBody->GetWorldPosition();
	mSurfacePlane.FromNormalPoint(	cVector3f(0,1,0), cVector3f(0,vPos.y,0) + cVector3f(0,fHeight/2,0));
}

//-----------------------------------------------------------------------

void cLuxArea_Liquid::OnUpdate(float afTimeStep)
{
	//Do not update this unless it is a proper game update (when eveything is 100% intialized)
	if(afTimeStep < gpBase->mpEngine->GetStepSize()*0.8f) return;

	///////////////////////////
	// Get data
	iPhysicsWorld *pPhysicsWorld = mpMap->GetPhysicsWorld();
	iPhysicsBody *pAreaBody = mpBody;
	
	float fSurfaceY =	mpBody->GetWorldPosition().y + mpBody->GetShape()->GetSize().y /2;

	cCollideData collideData;
	collideData.SetMaxSize(1);

	///////////////////////////
	// Update count
	mfTimeCount += afTimeStep;

	
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
		if(cMath::CheckBVIntersection(*pBody->GetBoundingVolume(), *pAreaBody->GetBoundingVolume())==false)
		{
			bInsideWater = false;
		}

		///////////////////////////////
		//Check for collision
		if(bInsideWater)
		{
			if(pPhysicsWorld->CheckShapeCollision(	pBody->GetShape(),pBody->GetLocalMatrix(), pAreaBody->GetShape(), pAreaBody->GetLocalMatrix(),
													collideData,1, false)==false)
			{
				bInsideWater = false;
			}
		}
		
		/////////////////////////
		//Character specific
		if(pBody->IsCharacter())
		{
			DoBuoyancyOnCharBody(pBody->GetCharacterBody(),fSurfaceY, bInsideWater);
		}
		/////////////////////////
		//Normal body specific
		else
		{
			DoBuoyancyOnBody(pBody, fSurfaceY, bInsideWater);
		}
	}
	
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxArea_Liquid::DoBuoyancyOnBody(iPhysicsBody *apBody, float afSurfaceY, bool abInsideWater)
{
	/////////////////////////
	//Check if in water
	if(abInsideWater==false)
	{
		apBody->SetBuoyancyActive(false);
		return;
	}

	/////////////////////////
	//Interaction check
	if(gpBase->mpPlayer->GetCurrentStateData()->AllowBuoyancy(apBody)==false)
	{
		apBody->SetBuoyancyActive(false);
		return;
	}

	///////////////////////////////
	//Add Buoyancy
	if(apBody->GetBuoyancyActive()==false)
	{
		apBody->SetBuoyancySurface(mSurfacePlane);
		apBody->SetBuoyancyDensity(mfDensity);
		apBody->SetBuoyancyLinearViscosity(mfLinearViscosity);
		apBody->SetBuoyancyAngularViscosity(mfAngularViscosity);

		SplashEffect(apBody, afSurfaceY);

		apBody->SetBuoyancyActive(true);
	}

	////////////////////////
	// Add waves
	cVector3f vCamPos = gpBase->mpPlayer->GetCamera()->GetPosition();
	float fDistSqr = cMath::Vector3DistSqr(vCamPos, apBody->GetLocalPosition());
	if(mbHasWaves && fDistSqr < mfMaxWaveDistanceSqr)
	{
		cVector3f vPos = cMath::MatrixMul(apBody->GetLocalMatrix(),apBody->GetMassCentre());

		float fAddX = sin(mfTimeCount * mfWaveFreq + vPos.x * 15)*mfWaveAmp*0.3f;
		float fAddZ = sin(mfTimeCount * mfWaveFreq + vPos.z * 15)*mfWaveAmp*0.3f;

		cPlanef tempPlane;
		tempPlane.FromNormalPoint(	cVector3f(0,1,0), cVector3f(0,afSurfaceY + fAddX + fAddZ,0));

		apBody->SetBuoyancySurface(tempPlane);
		apBody->Enable();
	}
}

//-----------------------------------------------------------------------

void cLuxArea_Liquid::DoBuoyancyOnCharBody(iCharacterBody *apCharBody, float afSurfaceY, bool abInsideWater)
{
	cLuxPlayer *pPlayer = gpBase->mpPlayer;

	//////////////////////////////////
	// Player
	if(apCharBody == pPlayer->GetCharacterBody())
	{
		////////////////////
		//In water
		if(abInsideWater)
		{
			if(pPlayer->IsInWater()==false)
			{
				pPlayer->SetIsInWater(true);
				pPlayer->SetWaterSpeedMul(mfPlayerSpeedMul);
				if(mpPhysicsMaterial)
				{
					pPlayer->SetWaterStepSound(mpPhysicsMaterial->GetSurfaceData()->GetStepType());
				}
				SplashEffect(apCharBody->GetCurrentBody(), afSurfaceY);
			}
		}
		////////////////////
		//Out of water
		else
		{
			pPlayer->SetIsInWater(false);
		}
	}
	//////////////////////////////////
	// Enemy
	else
	{
		iLuxEnemy *pEnemy = (iLuxEnemy*)apCharBody->GetUserData();
		if(pEnemy==NULL) return;

        if(abInsideWater)
		{
			if(pEnemy->IsInWater()==false)
			{
				pEnemy->SetInWater(true);
				pEnemy->SetWaterSurfaceY(afSurfaceY);
				pEnemy->SetWaterSurfaceData(mpPhysicsMaterial->GetSurfaceData());
			}
		}
		else
		{
			pEnemy->SetInWater(false);
		}
	}
}

//-----------------------------------------------------------------------

void cLuxArea_Liquid::SplashEffect(iPhysicsBody *apBody, float afSurfaceY)
{
	if(mpPhysicsMaterial==NULL) return;

	cSurfaceData *pSurface = mpPhysicsMaterial->GetSurfaceData();

	float fSpeed;
	if(apBody->IsCharacter())
		fSpeed = apBody->GetCharacterBody()->GetForceVelocity().Length();
	else
		fSpeed = apBody->GetLinearVelocity().Length();

	cSurfaceImpactData *pImpact = pSurface->GetImpactDataFromSpeed(fSpeed);

	if(pImpact == NULL) return;

	cVector3f vEffectPos = cMath::MatrixMul(apBody->GetLocalMatrix(),apBody->GetMassCentre());
	vEffectPos.y = afSurfaceY+0.01f;

	cWorld *pWorld = mpMap->GetWorld();

	if(pImpact->GetPSName() != "")
	{
		cParticleSystem *pPS = pWorld->CreateParticleSystem("Splash", pImpact->GetPSName(),1);

		if(pPS) pPS->SetPosition(vEffectPos);
	}

	if(pImpact->GetSoundName() != "")
	{
		cSoundEntity *pSound = pWorld->CreateSoundEntity("Splash",pImpact->GetSoundName(),true);
		
		if(pSound) pSound->SetPosition(vEffectPos);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxArea_Liquid_SaveData, iLuxArea_SaveData)
kSerializeVar(mfDensity, eSerializeType_Float32)
kSerializeVar(mfLinearViscosity, eSerializeType_Float32)
kSerializeVar(mfAngularViscosity, eSerializeType_Float32)

kSerializeVar(msPhysicsMaterial, eSerializeType_String)

kSerializeVar(mbHasWaves, eSerializeType_Bool)
kSerializeVar(mfWaveAmp, eSerializeType_Float32)
kSerializeVar(mfWaveFreq, eSerializeType_Float32)

kSerializeVar(mfPlayerSpeedMul, eSerializeType_Float32)
kSerializeVar(mfMaxWaveDistanceSqr, eSerializeType_Float32)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxArea* cLuxArea_Liquid_SaveData::CreateArea(cLuxMap *apMap)
{
	return hplNew(cLuxArea_Liquid, (msName, mlID, apMap));
}

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxArea_Liquid::CreateSaveData()
{
	return hplNew(cLuxArea_Liquid_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxArea_Liquid::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::SaveToSaveData(apSaveData);
	cLuxArea_Liquid_SaveData *pData = static_cast<cLuxArea_Liquid_SaveData*>(apSaveData);

	if(mpPhysicsMaterial)
		pData->msPhysicsMaterial = mpPhysicsMaterial->GetName();

	kCopyToVar(pData, mfDensity);
	kCopyToVar(pData, mfLinearViscosity);
	kCopyToVar(pData, mfAngularViscosity);

	kCopyToVar(pData, mbHasWaves);
	kCopyToVar(pData, mfWaveAmp);
	kCopyToVar(pData, mfWaveFreq);

	kCopyToVar(pData, mfPlayerSpeedMul);
	kCopyToVar(pData, mfMaxWaveDistanceSqr);

}

//-----------------------------------------------------------------------

void cLuxArea_Liquid::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::LoadFromSaveData(apSaveData);
	cLuxArea_Liquid_SaveData *pData = static_cast<cLuxArea_Liquid_SaveData*>(apSaveData);

	if(pData->msPhysicsMaterial != "")
	{
		mpPhysicsMaterial = mpMap->GetPhysicsWorld()->GetMaterialFromName(pData->msPhysicsMaterial);
		if(mpPhysicsMaterial ==NULL)
			Error("Could not load physics material '%s' for liquid area '%s'!\n",pData->msPhysicsMaterial.c_str(), msName.c_str());
	}

	kCopyFromVar(pData, mfDensity);
	kCopyFromVar(pData, mfLinearViscosity);
	kCopyFromVar(pData, mfAngularViscosity);

	kCopyFromVar(pData, mbHasWaves);
	kCopyFromVar(pData, mfWaveAmp);
	kCopyFromVar(pData, mfWaveFreq);

	kCopyFromVar(pData, mfPlayerSpeedMul);
	kCopyFromVar(pData, mfMaxWaveDistanceSqr);
}

//-----------------------------------------------------------------------

void cLuxArea_Liquid::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);

}

//-----------------------------------------------------------------------

