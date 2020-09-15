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

#include "LuxHandObject_LightSource.h"

#include "LuxMap.h"
#include "LuxPlayer.h"
#include "LuxPlayerHands.h"
#include "LuxMapHelper.h"
#include "LuxHelpFuncs.h"

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxHandObject_LightSource::cLuxHandObject_LightSource(const tString& asName, cLuxPlayerHands *apHands) : iLuxHandObject(asName, apHands)
{
	mfSwayAngle =0;
	mfSwayVel =0;
}

cLuxHandObject_LightSource::~cLuxHandObject_LightSource()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::LoadImplementedVars(cXmlElement *apVarsElem)
{
	float fFadeInTime = apVarsElem->GetAttributeFloat("FadeInTime", 0);
	float fFadeOutTime = apVarsElem->GetAttributeFloat("FadeOutTime", 0);
	if(fFadeInTime == 0) fFadeInTime = 0.001f;
	if(fFadeOutTime == 0) fFadeOutTime = 0.001f;
	mfFadeInSpeed = 1.0f / fFadeInTime;
	mfFadeOutSpeed = 1.0f / fFadeOutTime;

	mbHasSwayPhysics = apVarsElem->GetAttributeBool("HasSwayPhysics", false);
	mfMaxSwayVel = apVarsElem->GetAttributeFloat("MaxSwayVel", 0);
	mvSwayAngleLimits = cMath::Vector2ToRad(apVarsElem->GetAttributeVector2f("SwayAngleLimits", 0));
	mvSwayDownAngleLimits= cMath::Vector2ToRad(apVarsElem->GetAttributeVector2f("SwayDownAngleLimits", 0));
	
	mfSwayGravity = apVarsElem->GetAttributeFloat("SwayGravity", 0);
	mfSwayFriction = apVarsElem->GetAttributeFloat("SwayFriction", 0);
	mvSwayPinDir = apVarsElem->GetAttributeVector3f("SwayPinDir", 1);
	mvSwayPinDir.Normalize();
	mfSwayPlayerSpeedMul = apVarsElem->GetAttributeFloat("SwayPlayerSpeedMul", 0);
	mfSwayCameraRollMul = apVarsElem->GetAttributeFloat("SwayCameraRollMul", 0);

	msSkipSwaySubMesh = apVarsElem->GetAttributeString("SkipSwaySubMesh", "");
}

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::ImplementedCreateEntity(cLuxMap *apMap)
{
	mvDefaultLightColors.resize(mvLights.size());
	mvDefaultLightFlicker.resize(mvLights.size());
	mvLightFadeOutColor.resize(mvLights.size());

	for(size_t i=0; i<mvLights.size(); ++i)
	{
		mvDefaultLightColors[i] = mvLights[i]->GetDiffuseColor();
		mvDefaultLightFlicker[i] =mvLights[i]->GetFlickerActive();
		mvLights[i]->SetFlickerActive(false);
	}

	mvDefaultSubMeshMatrix.resize(mpMeshEntity->GetSubMeshEntityNum());
	for(size_t i=0; i<mpMeshEntity->GetSubMeshEntityNum(); ++i)
	{
		cSubMeshEntity *pSubEnt = mpMeshEntity->GetSubMeshEntity(i);
		mvDefaultSubMeshMatrix[i] = pSubEnt->GetLocalMatrix();
	}
}

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::ImplementedDestroyEntity(cLuxMap *apMap)
{
}

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::ImplementedReset()
{
	mfSwayAngle =0;
	mfSwayVel =0;
}

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::Update(float afTimeStep)
{
	bool bUpdate = false;
	bool bUpdateDone = false;

	///////////////////
	// Sway Physics
    if(mbHasSwayPhysics)	
	{
		UpdateSwayPhysics(afTimeStep);	
	}

	///////////////////
	// Fade out
	if(mpHands->GetState() == eLuxHandsState_Holster)
	{
		for(size_t i=0; i<mvLights.size(); ++i)
		{
			if(mvDefaultLightFlicker[i])
			{
				mvLights[i]->SetFlickerActive(false);
				mvLights[i]->StopFading();
			}
		}

		mpHands->mfHandObjectAlpha -= mfFadeOutSpeed * afTimeStep;
		if(mpHands->mfHandObjectAlpha < 0.0f) mpHands->mfHandObjectAlpha = 0.0f;
		
		bUpdate = true;
	}
	///////////////////
	// Fade in
	else if(mpHands->mfHandObjectAlpha < 1.0f)
	{
		mpHands->mfHandObjectAlpha += mfFadeInSpeed * afTimeStep;
		if(mpHands->mfHandObjectAlpha > 1.0f)
		{
			mpHands->mfHandObjectAlpha = 1.0f;
			bUpdateDone = true;
		}

		bUpdate = true;
	}

	///////////////////
	// Calculate fade out color
	if(mpHands->GetState() != eLuxHandsState_Holster)
	{
		for(size_t i=0; i<mvLights.size(); ++i)
		{
			mvLightFadeOutColor[i] = mvLights[i]->GetDiffuseColor();
		}
	}
	

	///////////////////
	// Set alpha
	if(bUpdate)
	{
		mpMeshEntity->SetIlluminationAmount(mpHands->mfHandObjectAlpha);

		for(size_t i=0; i<mvBillboards.size(); ++i)
		{
			cColor col = mvBillboards[i]->GetColor();
			col.a = mpHands->mfHandObjectAlpha;
			mvBillboards[i]->SetColor(col);
		}
		
		for(size_t i=0; i<mvParticleSystems.size(); ++i)
		{
			cColor col = mvParticleSystems[i]->GetColor();
			col.a = mpHands->mfHandObjectAlpha;
			mvParticleSystems[i]->SetColor(col);
		}
		
		for(size_t i=0; i<mvLights.size(); ++i)
		{
			if(mpHands->GetState() == eLuxHandsState_Holster)
			{
				mvLights[i]->SetDiffuseColor(mvLightFadeOutColor[i] * mpHands->mfHandObjectAlpha);
			}
			else
			{
				mvLights[i]->SetDiffuseColor(mvDefaultLightColors[i] * mpHands->mfHandObjectAlpha);
			}
		}
	}

	if(bUpdateDone)
	{
		for(size_t i=0; i<mvLights.size(); ++i)
		{
			if(mvDefaultLightFlicker[i]) mvLights[i]->SetFlickerActive(true);
		}
	}
}

//-----------------------------------------------------------------------

bool cLuxHandObject_LightSource::DoAction(eLuxPlayerAction aAction, bool abPressed)
{
	return false;
}

//-----------------------------------------------------------------------

bool cLuxHandObject_LightSource::AnimationIsOver()
{
	return true;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::UpdateSwayPhysics(float afTimeStep)
{
	/////////////////////////////
	// Update vel
	{
		/////////////////////////////
		// Gravity
		cCamera *pCam = gpBase->mpPlayer->GetCamera();
		float fDownAngle = pCam->GetPitch() * -1.0f;
		fDownAngle += pCam->GetRoll() * mfSwayCameraRollMul;
		fDownAngle = cMath::Clamp(fDownAngle, mvSwayDownAngleLimits.x, mvSwayDownAngleLimits.y);

		float fAngle = mfSwayAngle - fDownAngle;

		float fForce = -sin(fAngle) * mfSwayGravity;
		mfSwayVel += fForce * afTimeStep;

		/////////////////////////////
		// Player velocity
        iCharacterBody *pCharBody = gpBase->mpPlayer->GetCharacterBody();
        float fPlayerSpeed = pCharBody->GetVelocity(gpBase->mpEngine->GetStepSize()).Length();	
		if(pCharBody->GetMoveSpeed(eCharDir_Forward)<0) fPlayerSpeed = -fPlayerSpeed;

		mfSwayVel += -fPlayerSpeed * mfSwayPlayerSpeedMul;

		/////////////////////////////
		// Cap and Friction
		if(mfSwayVel> mfMaxSwayVel)		mfSwayVel = mfMaxSwayVel;
		if(mfSwayVel< -mfMaxSwayVel)	mfSwayVel = -mfMaxSwayVel;

		mfSwayVel -= mfSwayVel*mfSwayFriction*afTimeStep;
	}

	/////////////////////////////
	// Update Angle
    mfSwayAngle += mfSwayVel * afTimeStep;

	//Min
	if(mfSwayAngle < mvSwayAngleLimits.x)
	{
		mfSwayVel =0;
		mfSwayAngle = mvSwayAngleLimits.x;
	}

	//Max
	if(mfSwayAngle > mvSwayAngleLimits.y)
	{
		mfSwayVel =0;
		mfSwayAngle = mvSwayAngleLimits.y;
	}
	
	/////////////////////////////
	// Update Model matrix
	cMatrixf mtxSway = cMath::MatrixRotate(mvSwayPinDir * mfSwayAngle, eEulerRotationOrder_XYZ);
	//cMatrixf mtxTrans = cMath::MatrixMul(m_mtxOffset, mtxSway);
	//mpMeshEntity->SetMatrix(mtxTrans);

	for(size_t i=0; i<mpMeshEntity->GetSubMeshEntityNum(); ++i)
	{
		cSubMeshEntity *pSubEnt = mpMeshEntity->GetSubMeshEntity(i);
		if(pSubEnt->GetSubMesh()->GetName() == msSkipSwaySubMesh) continue;
		//Log("'%s'\n",pSubEnt->GetSubMesh()->GetName().c_str());
		
		pSubEnt->SetMatrix(cMath::MatrixMul(mtxSway, mvDefaultSubMeshMatrix[i]) );
	}
	mpMeshEntity->SetMatrix(m_mtxOffset);
}

//-----------------------------------------------------------------------

