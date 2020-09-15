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

#include "LuxCritter_Bug.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxMap.h"
#include "LuxMapHelper.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_Critter_Bug::cLuxPropLoader_Critter_Bug(const tString& asName) : iLuxPropLoader_Critter(asName)
{
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_Critter_Bug::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxCritter_Bug, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Critter_Bug::LoadCritterVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxCritter_Bug  *pCritter_Bug = static_cast<cLuxCritter_Bug*>(apProp);

	///////////////////////////
	// General
	pCritter_Bug->mfMaxSpeedNormal = GetVarFloat("MaxSpeedNormal", 0);
	pCritter_Bug->mfMaxSpeedAfraid = GetVarFloat("MaxSpeedAfraid", 0);
	pCritter_Bug->mfMaxSpeed = pCritter_Bug->mfMaxSpeedNormal;

	pCritter_Bug->mfMaxSpeed = GetVarFloat("MaxSpeed", 0);
	pCritter_Bug->mfGravity = GetVarFloat("Gravity", 0);

	pCritter_Bug->mfPlayerFleeDistance = GetVarFloat("PlayerFleeDistance", 0);
	
	pCritter_Bug->mfFleeMul = GetVarFloat("FleeMul", 0);
	pCritter_Bug->mfBackToSpawnPointMul = GetVarFloat("BackToSpawnPointMul", 0);
	pCritter_Bug->mfWanderCircleRadius = GetVarFloat("WanderCircleRadius", 0);
	pCritter_Bug->mfWanderCircleDist = GetVarFloat("WanderCircleDist", 0);

	pCritter_Bug->mbHasRandomPauses = GetVarBool("HasRandomPauses", false);
	pCritter_Bug->mvRandomPauseTimeMinMax = GetVarVector2f("RandomPauseTimeMinMax", cVector2f(1,3));
	pCritter_Bug->mvRandomMoveTimeMinMax = GetVarVector2f("RandomMoveTimeMinMax", cVector2f(2,5));

	pCritter_Bug->msNormalSound = GetVarString("NormalSound", "");
	pCritter_Bug->mvNormalSoundRandMinMax = GetVarVector2f("NormalSoundRandMinMax", 0);

	pCritter_Bug->msScaredSound = GetVarString("ScaredSound", "");
	pCritter_Bug->mvScaredSoundRandMinMax = GetVarVector2f("ScaredSoundRandMinMax", 0);

}

//-----------------------------------------------------------------------

void cLuxPropLoader_Critter_Bug::LoadCritterInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxCritter_Bug::cLuxCritter_Bug(const tString &asName,int alID, cLuxMap *apMap) : iLuxProp_CritterBase(asName,alID,apMap)
{
	mfPlaySoundCount = cMath::RandRectf(1, 5);
	mfPauseCount = cMath::RandRectf(1,3);
	mbPaused = false;
}

//-----------------------------------------------------------------------

cLuxCritter_Bug::~cLuxCritter_Bug()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxCritter_Bug::UpdateCritterSpecific(float afTimeStep)
{
	UpdateVelocity(afTimeStep);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxCritter_Bug::UpdateVelocity(float afTimeStep)
{
	
	/////////////////// 
	// Check if scared
	bool bScared=false;
	cVector3f vPlayerPos = gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition();
	float fPlayerDistanceSqr = cMath::Vector3DistSqr(vPlayerPos, mpBody->GetWorldPosition());
	if(fPlayerDistanceSqr < mfPlayerFleeDistance * mfPlayerFleeDistance)
	{
		bScared = true;
		mbPaused=false;
	}

	/////////////////// 
	// Pause
	if(mbHasRandomPauses && bScared==false)
	{
		mfPauseCount -= afTimeStep;
		if(mfPauseCount < 0)
		{
			if(mbPaused)
			{
				mbPaused = false;
				mfPauseCount = cMath::RandRectf(mvRandomMoveTimeMinMax.x, mvRandomMoveTimeMinMax.y);
			}
			else
			{
				mbPaused = true;
				mfPauseCount = cMath::RandRectf(mvRandomPauseTimeMinMax.x, mvRandomPauseTimeMinMax.y);
			}
		}
	}
		
	/////////////////// 
	// Movement
	if(mbPaused==false)
	{
		/////////////////// 
		// Wandering
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
		float fAngle = cMath::RandRectf(0, k2Pif);

		cVector3f vForce = cMath::MatrixMul(cMath::MatrixRotateY(fAngle),cVector3f(mfWanderCircleRadius,0,0));
		
		mvVel += (vDir*mfWanderCircleDist + vForce) * afTimeStep;
		
		/////////////////// 
		// Avoid player
		if(bScared)
		{
			float fPlayerDistance = sqrt(fPlayerDistanceSqr);
		
			if(fPlayerDistance ==0) fPlayerDistance = 0.0001f;
			
			vPlayerPos.y = mpBody->GetWorldPosition().y;
			cVector3f vWantedVel = cMath::Vector3Normalize(mpBody->GetWorldPosition() - vPlayerPos) * mfMaxSpeed;
			
			cVector3f vForce = (vWantedVel - mvVel) * mfFleeMul * (1.0f/fPlayerDistance);
			mvVel += vForce * afTimeStep;

			mfMaxSpeed = mfMaxSpeedAfraid;
		}
		else
		{
			mfMaxSpeed = mfMaxSpeedNormal;
		}

		/////////////////// 
		// Swarm around point
		{
			float fSwarmPointDist = cMath::Vector3Dist(mvSwarmPoint, mpBody->GetWorldPosition());
			
			cVector3f vSwarmPos = cVector3f(mvSwarmPoint.x, mpBody->GetWorldPosition().y, mvSwarmPoint.z);
			cVector3f vWantedVel = cMath::Vector3Normalize(vSwarmPos - mpBody->GetWorldPosition()) * mfMaxSpeed;
			vWantedVel.y=0;

			cVector3f vForce = (vWantedVel - mvVel) * 0.2f * fSwarmPointDist;
			mvVel += vForce * afTimeStep;
		}
	}
	/////////////////// 
	// Stopping
	else
	{
		mvVel -= mvVel*afTimeStep*3.0f;
	}
	
	/////////////////// 
	// Sound
	mfPlaySoundCount-=afTimeStep;
	if(mfPlaySoundCount < 0)
	{
		mfPlaySoundCount = bScared ? cMath::RandRectf(mvScaredSoundRandMinMax.x, mvScaredSoundRandMinMax.y) : 
									cMath::RandRectf(mvNormalSoundRandMinMax.x, mvNormalSoundRandMinMax.y);
		PlaySound("Critter_BugSound",bScared ? msScaredSound : msNormalSound,true, true);
	}
    

	/////////////////// 
	// Cap speed
	float fSpeed = mvVel.Length();
	if(fSpeed > mfMaxSpeed)
	{
		mvVel = (mvVel / fSpeed) * mfMaxSpeed;
	}

	////////////////////////
	// Gravity
	mvGravityVel.y -= mfGravity * afTimeStep;
	mvGravityVel = cMath::Vector3MaxLength(mvGravityVel, 10);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxCritter_Bug_SaveData, iLuxProp_CritterBase_SaveData)
kSerializeVar(mfPlaySoundCount, eSerializeType_Float32)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxCritter_Bug::CreateSaveData()
{
	return hplNew(cLuxCritter_Bug_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxCritter_Bug::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxCritter_Bug_SaveData *pData = static_cast<cLuxCritter_Bug_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData,mfPlaySoundCount);
}

//-----------------------------------------------------------------------

void cLuxCritter_Bug::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxCritter_Bug_SaveData *pData = static_cast<cLuxCritter_Bug_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData,mfPlaySoundCount);
}

//-----------------------------------------------------------------------

void cLuxCritter_Bug::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
