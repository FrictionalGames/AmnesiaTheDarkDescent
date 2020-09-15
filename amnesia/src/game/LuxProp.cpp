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

#include "LuxProp.h"

#include "LuxMap.h"
#include "LuxPlayer.h"
#include "LuxInteractConnections.h"



//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxPropLoader::iLuxPropLoader(const tString& asName) : cEntityLoader_Object(asName)
{
	mfDefaultMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction", "Default_DefaultMaxFocusDist",0);

	mbForceFullGameSave = false;
}

//-----------------------------------------------------------------------

void iLuxPropLoader::BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars)
{

}

//-----------------------------------------------------------------------

void iLuxPropLoader::AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars)
{
	cLuxMap *pMap = gpBase->mpCurrentMapLoading;
	if(pMap==NULL)	return;

	iLuxProp *pProp = CreateProp(mpEntity->GetName(), mlID,pMap);

	//////////////////////////////
	// Set data
	pProp->mpWorld = apWorld;
	pProp->msFileName = msFileName;
	pProp->m_mtxOnLoadTransform = a_mtxTransform;
	pProp->mvOnLoadScale = mvScale;

	pProp->mvBodies = mvBodies;
	pProp->mvJoints = mvJoints;

	pProp->mvBodyExtraData = mvBodyExtraData;

	pProp->mvLights = mvLights;
	pProp->mvParticleSystems = mvParticleSystems;
	pProp->mvBillboards = mvBillboards;
	pProp->mvBeams = mvBeams;
	pProp->mvSoundEntities = mvSoundEntities;

	pProp->mpMeshEntity = mpEntity;

	
	//Set body userdata
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodies[i];
		iLuxEntity *pEnt = pProp;
		pBody->SetUserData(pEnt);
	}

	//////////////////////////////
	// Load base properties
	pProp->mfHealth = GetVarFloat("Health", 100);
	pProp->mlToughness = GetVarInt("Toughness", 0);

	pProp->mfMaxFocusDistance = GetVarFloat("MaxFocusDistance", 0);
	pProp->msMainBodyName = GetVarString("MainPhysicsBody", "");
	
	pProp->mbDissolveOnDestruction = GetVarBool("DissolveOnDestruction", false);
	pProp->mfDissolveTime = GetVarFloat("DissolveTime", 1.0f);

	pProp->mbShowMesh = GetVarBool("ShowMesh", true);
	if(mpEntity) mpEntity->SetVisible(pProp->mbShowMesh);

	pProp->mbShowHints = GetVarBool("ShowHints", true);

	//////////////////////////////
	// Load effects properties
	pProp->msEffectsOnSound = GetVarString("EffectsOnSound", "");
	pProp->msEffectsOffSound = GetVarString("EffectsOffSound", "");

	pProp->mfEffectsOnTime = GetVarFloat("EffectsOnTime", 1);
	pProp->mfEffectsOffTime = GetVarFloat("EffectsOffTime", 1);

	pProp->mEffectsOffLightColor = GetVarColor("EffectsOffLightColor", cColor(0,0));
	pProp->mfEffectsOffLightRadius = GetVarFloat("EffectsOffLightRadius", 1);

	//////////////////////////////
	// Load move properties
	pProp->mbMoveCheckCollision = GetVarBool("StaticMoveCheckCollision", true);
	pProp->msMoveStartSound = GetVarString("StaticMoveStartSound", "");
	pProp->msMoveStopSound = GetVarString("StaticMoveStopSound", "");
	pProp->msMoveLoopSound = GetVarString("StaticMoveLoopSound", "");

	gpBase->PreloadSound(pProp->msMoveStartSound);
	gpBase->PreloadSound(pProp->msMoveStopSound);
	gpBase->PreloadSound(pProp->msMoveLoopSound);

	//////////////////////////////
	// Load misc properties
	if(pProp->mpMeshEntity && pProp->mpMeshEntity->GetAnimationStateNum()>0 && GetVarBool("RandomizeAnimationStart", true))
	{
		cAnimationState *pAnim = pProp->mpMeshEntity->GetAnimationState(0);
		float fLength = pAnim->GetLength();
		float fStartTime = cMath::RandRectf(0, fLength);
		
		pAnim->SetTimePosition(fStartTime);
	}
		
	
	//////////////////////////////
	// Load type specific properties
	LoadVariables(pProp, apRootElem);

	//////////////////////////////
	// Extra setup

	//If focus distance is still 0 or below, set it as default.
	if(pProp->mfMaxFocusDistance<=0) pProp->mfMaxFocusDistance = mfDefaultMaxFocusDistance;

	pProp->SetupAfterLoad(apWorld);

	//Add to the current map
	pMap->AddEntity(pProp);

	pProp->SetActive(mbActive);

	///////////////////////////////
	// Instance vars
	if(apInstanceVars)
	{
		if(pProp->mpMeshEntity)
		{
			pProp->mpMeshEntity->SetRenderFlagBit(eRenderableFlag_ShadowCaster, apInstanceVars->GetVarBool("CastShadows", true));
		}

		tString sConnectedProps = apInstanceVars->GetVarString("ConnectedProps", "");
		cString::GetStringVec(sConnectedProps, pProp->mvConnectedProps);

		pProp->msCallbackFunc = apInstanceVars->GetVarString("CallbackFunc", "");
		pProp->msConnectionStateChangeCallback = apInstanceVars->GetVarString("ConnectionStateChangeCallback", "");
		pProp->mbFullGameSave = mbForceFullGameSave ? true : apInstanceVars->GetVarBool("FullGameSave", false);

		pProp->SetPlayerLookAtCallback(	apInstanceVars->GetVarString("PlayerLookAtCallback",""),
										apInstanceVars->GetVarBool("PlayerLookAtCallbackAutoRemove",false) );

		pProp->SetPlayerInteractCallback(	apInstanceVars->GetVarString("PlayerInteractCallback",""),
											apInstanceVars->GetVarBool("PlayerInteractCallbackAutoRemove",false) );

		pProp->SetStaticPhysics(apInstanceVars->GetVarBool("StaticPhysics", false));


		LoadInstanceVariables(pProp, apInstanceVars);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxProp::iLuxProp(const tString &asName, int alID, cLuxMap *apMap, eLuxPropType aPropType) : iLuxEntity(asName,alID,apMap, eLuxEntityType_Prop)
{
	mPropType = aPropType;

	mpAttachmentParent = NULL;

	mbCheckOutsidePlayer = false;
	mbIsInteractedWith = false;
	
	mbEffectsActive = true;
	mfEffectsAlpha = 1;
	mbEffectAlphaFading = true;

	mbHasFlickering = false;

	mbInteractionDisabled = false;

	mbStaticPhysics = false;

	mpMainBody = NULL;

	mfMovingVolume =0;
	mfMoveStartCount =0;

	mbMoving = false;
	mbMovingAngular = false;
	mbMovingLinear = false;
	mpMoveLoopSound = NULL;

	mfMoveLinearAcc =0;
	mfMoveLinearSpeed=0;
	mfMoveLinearSlowdownDist=0;
	
	mfMoveAngularAcc =0;
	mfMoveAngularSpeed =0;
	mfMoveAngularSlowdownDist=0;
	
	mlCurrentNonLoopAnimIndex = -1;

	mfFadeInAlpha = 1.0f;
	mfFadeInSpeed =0;
 
	m_mtxLastBodyMoveMatrix = cMatrixf::Identity;
}

//-----------------------------------------------------------------------

iLuxProp::~iLuxProp()
{
	cWorld *pWorld = mpWorld;
	iPhysicsWorld *pPhysicsWorld = pWorld->GetPhysicsWorld();

	////////////////////
	// Attachment
	if(mpAttachmentParent && mpMap && mpMap->IsDeletingAllWorldEntities()==false)
	{
		mpAttachmentParent->RemoveAttachedProp(this);
		mpAttachmentParent = NULL;
	}
	for(tLuxProp_AttachedPropListIt it = mlstAttachedProps.begin(); it != mlstAttachedProps.end(); ++it)
	{
		//Map is currently deleting all 
		if(mpMap->IsDeletingAllWorldEntities())
		{
			hplDelete(*it);
		}
		else
		{
			DestroyAttachedProp(*it);
		}
	}
	mlstAttachedProps.clear();


	////////////////////
	// Connections
	STLDeleteAll(mvInteractConnections);


	////////////////////
	// Move stuff
	if(mpMoveLoopSound && pWorld->SoundEntityExists(mpMoveLoopSound,mlMoveLoopSoundID))
	{
		pWorld->DestroySoundEntity(mpMoveLoopSound);
	}
	

	////////////////////
	// Check if entity is used
	// TODO?

	////////////////////
	// Destroy physics
	{
		//Joints
		for(size_t i=0; i<mvJoints.size(); ++i)
		{
			iPhysicsJoint *pJoint = mvJoints[i];

			if(pJoint && pPhysicsWorld->JointExists(pJoint))
			{
				pPhysicsWorld->DestroyJoint(pJoint);
			}
		}

		//Bodies
		for(size_t i=0; i<mvBodies.size(); ++i)
		{
			iPhysicsBody *pBody = mvBodies[i];
				
			pWorld->GetPhysicsWorld()->DestroyBody(pBody);
		}
	}

	////////////////////
	// Destroy graphics
	{
		//Mesh entity
		if(mpMeshEntity) pWorld->DestroyMeshEntity(mpMeshEntity);

		//Lights
		for(size_t i=0; i<mvLights.size(); ++i) pWorld->DestroyLight(mvLights[i]);
		
		//Particle systems
		for(size_t i=0; i<mvParticleSystems.size(); ++i) 
		{
			cParticleSystem *pPS = mvParticleSystems[i];
			if(pPS && pWorld->ParticleSystemExists(pPS)) pPS->Kill();
		}

		//Billboards
		for(size_t i=0; i<mvBillboards.size(); ++i)	pWorld->DestroyBillboard(mvBillboards[i]);
		
		//Beams
		for(size_t i=0; i<mvBeams.size(); ++i)		pWorld->DestroyBeam(mvBeams[i]);
		
		//Sound entities
		for(size_t i=0; i<mvSoundEntities.size(); ++i) 	pWorld->DestroySoundEntity(mvSoundEntities[i]);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxProp::SetupAfterLoad(cWorld *apWorld)
{
	//////////////////////
	// Normal setup
	mvDefaultBodySettings.resize(mvBodies.size());
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodies[i];
        
		mvDefaultBodySettings[i].mbCollideCharacter = pBody->GetCollideCharacter();
	}

	//Get the main body if any
	if(msMainBodyName != "")
	{
		mpMainBody = (iPhysicsBody*)STLFindByName(mvBodies,msName + "_"+ msMainBodyName);
        if(mpMainBody == NULL)
		{
			Warning("Could not find main physics body '%s'\n", msName.c_str());
		}
	}
	
	///////////////////////
	// Set body data
	mvBodyData.resize(mvBodies.size());
	for(size_t i=0; i<mvBodyData.size(); ++i)
	{
		mvBodyData[i].mfMass = mvBodies[i]->GetMass();
	}

	///////////////////////
	// Effects stuff
	SetupEffectData();
	
	///////////////////////
	// Setup implemented 
	OnSetupAfterLoad(apWorld);
}

//-----------------------------------------------------------------------

void iLuxProp::OnUpdate(float afTimeStep)
{
	///////////////////////
	// Prop update
	UpdateMoving(afTimeStep);
	UpdateCheckIfOutsidePlayer(afTimeStep);
	UpdateEffectFading(afTimeStep);
	UpdateMeshFading(afTimeStep);
	UpdateAnimation(afTimeStep);
	UpdateAttachedProps(afTimeStep, false);

	///////////////////////
	// Connections
	for(size_t i=0; i<mvInteractConnections.size(); ++i)
	{
		mvInteractConnections[i]->Update(afTimeStep);
		mvInteractConnections[i]->UpdateProp(afTimeStep);
	}

	//////////////////////
	// Specific update
	UpdatePropSpecific(afTimeStep);

	//if(mvLights.size() > 0)
	//	Log("End Color: %s\n", mvLights[0]->GetDiffuseColor().ToString().c_str());
}

//-----------------------------------------------------------------------

void iLuxProp::BeforeEntityDestruction()
{
	if(mbDissolveOnDestruction && mpMeshEntity)
	{
		mpMap->AddDissolveEntity(mpMeshEntity, mfDissolveTime);
	}

	BeforePropDestruction();
}

//-----------------------------------------------------------------------

void iLuxProp::OnRenderSolid(cRendererCallbackFunctions* apFunctions)
{
	return;
	if(mvBodies.size()<=0) return;
	
	cBoundingVolume* pBV = mvBodies[0]->GetBoundingVolume();
	apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(), pBV->GetMax(),cColor(1,1,1,1));
}

//-----------------------------------------------------------------------

void iLuxProp::SetEffectsActive(bool abActive, bool abFadeAndPlaySounds)
{
	if(abActive == mbEffectsActive) return;

	mbEffectsActive = abActive;

	mbEffectAlphaFading = true;

	///////////////////////
	// Effects On stuff
	if(mbEffectsActive)
	{
		////////////////
		// Create particle systems
		for(size_t i=0; i<mvParticleSystems.size(); ++i)
		{
			cLuxProp_PSData *pPSData = &mvEffectPSData[i];
			cParticleSystem *pPS = mpWorld->CreateParticleSystem(pPSData->msName, pPSData->msDataName,1);
			if(pPS)
			{
				pPS->SetMatrix(pPSData->m_mtxLocalTransform);
				if(pPSData->mpParent) pPSData->mpParent->AddChild(pPS);
			}

			mvParticleSystems[i] = pPS;
		}

		////////////////
		// Lights
		for(size_t i=0; i<mvLights.size(); ++i)
		{	
			iLight *pLight = mvLights[i];
			cLuxProp_LightData *pLightData = &mvEffectLightData[i];

			if(abFadeAndPlaySounds)
			{
				pLight->FadeTo(pLightData->mOnColor, pLightData->mfOnRadius, mfEffectsOnTime);
			}
			else 
			{
				pLight->SetDiffuseColor(pLightData->mOnColor);
				pLight->SetRadius(pLightData->mfOnRadius);
				pLight->SetFlickerActive(pLightData->mbFlickering);
			}
		}

		////////////////
		// Sounds
		for(size_t i=0; i<mvSoundEntities.size(); ++i)
		{
			cSoundEntity *pSound = mvSoundEntities[i];

			if(abFadeAndPlaySounds)	pSound->FadeIn(1.0f / mfEffectsOnTime);
			else					pSound->Play(false);
		}

		////////////////
		// Effect alpha
		if(abFadeAndPlaySounds==false) mfEffectsAlpha = 1.0f;
	}
	///////////////////////
	// Effects Off stuff
	else
	{
		////////////////
		// Kill / Destroy particle systems
		for(size_t i=0; i<mvParticleSystems.size(); ++i)
		{
			cParticleSystem *pPS = mvParticleSystems[i];
			mvParticleSystems[i] = NULL;

			if(abFadeAndPlaySounds)	pPS->Kill();
			else					mpWorld->DestroyParticleSystem(pPS);
		}

		////////////////
		// Lights
		for(size_t i=0; i<mvLights.size(); ++i)
		{	
			iLight *pLight = mvLights[i];
			cLuxProp_LightData *pLightData = &mvEffectLightData[i];

			pLight->StopFading();
			pLight->SetFlickerActive(false);
			float fOffRadius = mfEffectsOffLightRadius >=0 ? mfEffectsOffLightRadius : pLightData->mfOnRadius;
			if(abFadeAndPlaySounds)
			{	
				pLight->FadeTo(mEffectsOffLightColor, fOffRadius, mfEffectsOffTime);
			}
			else
			{	
				pLight->SetDiffuseColor(mEffectsOffLightColor);
				pLight->SetRadius(fOffRadius);
			}
		}

		////////////////
		// Sounds
		for(size_t i=0; i<mvSoundEntities.size(); ++i)
		{
			cSoundEntity *pSound = mvSoundEntities[i];

			if(abFadeAndPlaySounds)	pSound->FadeOut(1.0f / mfEffectsOffTime);
			else					pSound->Stop(false);
		}

		////////////////
		// Effect alpha
		if(abFadeAndPlaySounds==false) mfEffectsAlpha = 0.0f;
	}

	////////////////
	// Play sound
	if(abFadeAndPlaySounds)
	{
		tString sSound = mbEffectsActive ? msEffectsOnSound : msEffectsOffSound;

		if(sSound != "")
		{
			cSoundEntity *pSound = mpWorld->CreateSoundEntity(msName + "_lightlitchange",sSound, true);
			if(pSound)
			{
				pSound->SetPosition(GetWorldCenterPos());
			}
		}
	}
}

//-----------------------------------------------------------------------

void iLuxProp::GiveDamage(float afAmount, int alStrength)
{
	if(alStrength < mlToughness-1)			afAmount =0;
	else if(alStrength == mlToughness-1)	afAmount *= 0.5;
	
	SetHealth(mfHealth - afAmount);

	OnDamage(afAmount, alStrength);
}

//-----------------------------------------------------------------------

void iLuxProp::SetDisableCollisionUntilOutSidePlayer(bool abX)
{
	mbCheckOutsidePlayer = abX;

	if(mbCheckOutsidePlayer)
	{
		for(size_t i=0; i<mvBodies.size(); ++i)
		{
			iPhysicsBody *pBody = mvBodies[i];
			pBody->SetCollideCharacter(false);
		}
	}
}

//-------------------------------------------------------------------

void iLuxProp::MoveLinearTo(const cVector3f& avGoal, float afAcc, float afMaxSpeed, float afSlowdownDist, bool abResetSpeed)
{
	if(mbMoving == false && mfMoveStartCount <=0)
	{
		PlaySound("MoveStart", msMoveStartSound,true, true);
	}
	mfMoveStartCount = 1.0f;

	mbMoving = true;
	mbMovingLinear = true;
	
	mvMoveLinearGoal = avGoal;
	mfMoveLinearAcc = afAcc;
	mfMoveLinearMaxSpeed = afMaxSpeed;
	mfMoveLinearSlowdownDist = afSlowdownDist;

	if(abResetSpeed) mfMoveLinearSpeed =0;

	OnStartMove();
}

//-------------------------------------------------------------------

void iLuxProp::MoveAngularTo(const cMatrixf& a_mtxGoal, float afAcc, float afMaxSpeed, float afSlowdownDist, bool abResetSpeed, 
							 bool abUseOffset, const cVector3f &avWorldOffset, const cVector3f &avLocalOffset)
{
	if(mbMoving == false && mfMoveStartCount <=0)
	{
		PlaySound("MoveStart", msMoveStartSound,true, true);
	}
	mfMoveStartCount = 1.0f;

	mbMoving = true;
	mbMovingAngular = true;
	
	m_mtxMoveAngularGoal = a_mtxGoal;
	mfMoveAngularAcc = afAcc;
	mfMoveAngularMaxSpeed = afMaxSpeed;
	mfMoveAngularSlowdownDist = afSlowdownDist;

	if(abResetSpeed) mfMoveAngularSpeed =0;

	mbMoveAngularUseOffset = abUseOffset;
	mvMoveAngularWorldOffset = avWorldOffset;
	mvMoveAngularLocalOffset = avLocalOffset;

	mbMoveAngularNoGoal = false;

	OnStartMove();
}

//-------------------------------------------------------------------

void iLuxProp::RotateAtSpeed(	float afAcc, float afGoalSpeed, const cVector3f& avAxis, bool abResetSpeed,
								 bool abUseOffset, const cVector3f &avWorldOffset, const cVector3f &avLocalOffset)
{
	if(mbMoving == false && mfMoveStartCount <=0) 
	{
		PlaySound("MoveStart", msMoveStartSound,true, true);
	}
	mfMoveStartCount = 1.0f;

	mbMoving = true;
	mbMovingAngular = true;

	mfMoveAngularAcc = afAcc;
	mfMoveAngularMaxSpeed = afGoalSpeed;
	
	if(abResetSpeed) mfMoveAngularSpeed =0;

	mbMoveAngularNoGoal = true;
	mvMoveAngularNoGoalDir = cMath::Vector3Normalize(avAxis);

	mbMoveAngularUseOffset = abUseOffset;
	mvMoveAngularWorldOffset = avWorldOffset;
	mvMoveAngularLocalOffset = avLocalOffset;

	OnStartMove();
}

//-------------------------------------------------------------------

void iLuxProp::StopMove()
{
	mbMovingLinear=false;
	mbMovingAngular=false;
	mbMoving = false;
	
	if(mfMoveStartCount <=0)
		PlaySound("MoveStop", msMoveStopSound,true, true);
}

//-------------------------------------------------------------------

void iLuxProp::FadeInMeshEntity(float afTime)
{
	mfFadeInAlpha =0;
	mfFadeInSpeed = 1.0f/ afTime;
	if(mpMeshEntity) mpMeshEntity->SetCoverageAmount(0);
}

//-------------------------------------------------------------------

void iLuxProp::ResetProperties()
{
	///////////////////////////////////
	// Reset bodies
    for(size_t i=0; i<mvBodies.size(); ++i)
	{
		cMatrixf mtxBody = cMath::MatrixMul(m_mtxOnLoadTransform, mvBodyExtraData[i].m_mtxLocalTransform);
        iPhysicsBody *pBody = mvBodies[i];

		pBody->SetMatrix(mtxBody);
		pBody->SetLinearVelocity(0);
		pBody->SetAngularVelocity(0);
	}
	
	///////////////////////////////////
	// Reset variables
	mbMoving = false;
	mbMovingAngular = false;
	mbMovingLinear = false;
	mpMoveLoopSound = NULL;

	mfMoveAngularSpeed =0;

	mfHealth = 100;

	///////////////////////////////////
	// Reset type specific
	OnResetProperties();
}

//-------------------------------------------------------------------

void iLuxProp::PlayAnimation(const tString& asName, float afFadeTime, bool abLoop, const tString& asCallback)
{
	if(mpMeshEntity==NULL) return;

	int lIdx = mpMeshEntity->GetAnimationStateIndex(asName);
	if(lIdx<0)
	{
		Error("Could not find animation '%s' for entity '%s'\n", asName.c_str(), msName.c_str());
		return;
	}

	mpMeshEntity->PlayFadeTo(lIdx, abLoop, afFadeTime);
	if(abLoop==false)	mlCurrentNonLoopAnimIndex = lIdx;
	else				mlCurrentNonLoopAnimIndex = -1;
	
	if(abLoop==false)	msAnimCallback = asCallback;
	else				msAnimCallback = "";
}

//-------------------------------------------------------------------

void iLuxProp::SetHealth(float afX)
{
	mfHealth = afX;

	OnHealthChange();
}

//-------------------------------------------------------------------

iEntity3D* iLuxProp::GetAttachEntity()
{
	if(mvBodies.empty())
	{
		return mpMeshEntity;
	}
	else
	{
		return mpMainBody ? mpMainBody : mvBodies[0];
	}
}

//-------------------------------------------------------------------

void iLuxProp::SetStaticPhysics(bool abX)
{
	if(mbStaticPhysics == abX) return;

	mbStaticPhysics = abX;

    for(size_t i=0; i<mvBodies.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodies[i];

		pBody->SetMass(mbStaticPhysics ? 0 : mvBodyData[i].mfMass);
		pBody->SetLinearVelocity(0);
		pBody->SetAngularVelocity(0);
	}
}

//-------------------------------------------------------------------

void iLuxProp::AddAndAttachProp(const tString& asName, const tString& asFileName, const cMatrixf& a_mtxOffset)
{
	if(GetMainBody()==NULL)
	{
		Error("Could not attach prop %s to %s because it does not have a main body!\n", msName.c_str(), asFileName.c_str());
		return;
	}

	/////////////////////////////////
	// Create the entity
	mpMap->ResetLatestEntity();
	mpMap->CreateEntity(asName, asFileName, cMatrixf::Identity,1);
    
	iLuxEntity *pEntity = mpMap->GetLatestEntity();
	if(pEntity==NULL || pEntity->GetEntityType() != eLuxEntityType_Prop || pEntity->GetName() != asName)
	{
		Error("Could not create attach entity '%s' for prop '%s'\n", asFileName.c_str(), msName.c_str());
		return;
	}

	iLuxProp* pProp =  static_cast<iLuxProp*>(pEntity);

	/////////////////////////////////
	// This prop shall not be saved!
	pProp->SetIsSaved(false);

	/////////////////////////////////
	// Turn off collision on all bodies and set mass to 0
	for(size_t i=0; i<pProp->mvBodies.size(); ++i)
	{
		iPhysicsBody *pBody = pProp->mvBodies[i];
		pBody->SetMass(0);
		pBody->SetCollide(false);
		pBody->SetCollideCharacter(false);
		pBody->SetActive(false);
	}
	
	/////////////////////////////////
	// Create attachment data
	cLuxProp_AttachedProp *pAttachProp = hplNew(cLuxProp_AttachedProp, ());
	pAttachProp->msName = asName;
	pAttachProp->msFileName = asFileName;
	pAttachProp->m_mtxOffset = a_mtxOffset;
	pAttachProp->mpProp = pProp;

	mlstAttachedProps.push_back(pAttachProp);
	pProp->SetAttachmentParent(this);

	//////////////////////////////////
	// Update the attached prop matrix so it starts out properly!
	UpdateAttachedProps(0, true);
}

//-------------------------------------------------------------------

bool iLuxProp::DestroyAttachedProp(const tString& asName)
{
	tLuxProp_AttachedPropListIt it = mlstAttachedProps.begin(); 
	for(; it != mlstAttachedProps.end(); ++it)
	{
		cLuxProp_AttachedProp *pAttachedProp = *it;
        if(pAttachedProp->msName == asName)
		{
			DestroyAttachedProp(pAttachedProp);
			mlstAttachedProps.erase(it);
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------

void iLuxProp::DestroyAttachedProp(cLuxProp_AttachedProp *apAttachedProp)
{
	apAttachedProp->mpProp->mpAttachmentParent = NULL; //Must set this null, else it will try and erase itself when delted!
    mpMap->DestroyEntity(apAttachedProp->mpProp);
	hplDelete(apAttachedProp);
}

//-------------------------------------------------------------------

bool iLuxProp::RemoveAttachedProp(iLuxProp *apProp)
{
	tLuxProp_AttachedPropListIt it = mlstAttachedProps.begin(); 
	for(; it != mlstAttachedProps.end(); ++it)
	{
		cLuxProp_AttachedProp *pAttachedProp = *it;
		if(pAttachedProp->mpProp == apProp)
		{
			apProp->SetAttachmentParent(NULL);
			hplDelete(pAttachedProp);

			mlstAttachedProps.erase(it);
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------

int iLuxProp::AddInteractConnection(iLuxInteractConnection *apConnection)
{
	mvInteractConnections.push_back(apConnection);
	return (int) mvInteractConnections.size()-1;
}

iLuxInteractConnection* iLuxProp::GetInteractionConnectionFromIndex(int alIndex)
{
	return mvInteractConnections[alIndex];
}

iLuxInteractConnection* iLuxProp::GetInteractionConnectionFromName(const tString& asName)
{
	return (iLuxInteractConnection*)STLFindByName(mvInteractConnections, asName);
}

void iLuxProp::DestroyInteractConnection(iLuxInteractConnection *apConnection)
{
	STLFindAndDelete(mvInteractConnections, apConnection);
}

//-----------------------------------------------------------------------

void iLuxProp::InteractConnectionLimit(int alState)
{
	for(size_t i=0; i<mvInteractConnections.size(); ++i)
	{
		iLuxInteractConnection *pConnection = mvInteractConnections[i];
        
		if(pConnection->GetInteractionOnly() && mbIsInteractedWith==false) continue;

		mvInteractConnections[i]->OnLimit(alState);	
	}
}

void iLuxProp::InteractConnectionTurn(float afAngle, float afPrevAngle, float afMinAngle, float afMaxAngle)
{
	//if(abs(afAngle - afPrevAngle) < 0.001f) return;

	float fT = (afAngle-afMinAngle)/(afMaxAngle - afMinAngle);

	for(size_t i=0; i<mvInteractConnections.size(); ++i)
	{
		iLuxInteractConnection *pConnection = mvInteractConnections[i];

		if(pConnection->GetInteractionOnly() && mbIsInteractedWith==false) continue;

		pConnection->OnTurn(afAngle - afPrevAngle, fT);	
	}
}

//-----------------------------------------------------------------------

iPhysicsBody* iLuxProp::GetBodyFromID(int alID)
{
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodies[i];
		if(pBody->GetUniqueID() == alID) return pBody;
	}

	return NULL;
}

//-----------------------------------------------------------------------

int iLuxProp::GetBodyIndexFromName(const tString& asName)
{
	tString sFinalName = msName + '_'+asName;
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodies[i];
		if(pBody->GetName() == sFinalName) return (int)i;
	}
	return -1;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxProp::OnSetActive(bool abX)
{	
	///////////////
	//Bodies
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		mvBodies[i]->SetActive(abX);
	}

	///////////////////
	//Mesh entity
	if(mpMeshEntity && mbShowMesh)
	{
		mpMeshEntity->SetActive(abX);
		mpMeshEntity->SetVisible(abX);
	}

	///////////////////
	//Lights
	for(size_t i=0; i<mvLights.size(); ++i)
	{
		mvLights[i]->SetVisible(abX);
		mvLights[i]->SetActive(abX);
	}

	///////////////////
	//Particle systems
	for(size_t i=0; i<mvParticleSystems.size(); ++i) 
	{
		cParticleSystem *pPS = mvParticleSystems[i];
		if(pPS)
		{
			pPS->SetVisible(abX);
			pPS->SetActive(abX);
		}
	}

	///////////////////
	//Billboards
	for(size_t i=0; i<mvBillboards.size(); ++i)
	{
		mvBillboards[i]->SetActive(abX);
		mvBillboards[i]->SetVisible(abX);
	}

	///////////////////
	//Beams
	for(size_t i=0; i<mvBeams.size(); ++i)
	{
		mvBeams[i]->SetActive(abX);
		mvBeams[i]->SetVisible(abX);
	}

	///////////////////
	//Sound entities
	for(size_t i=0; i<mvSoundEntities.size(); ++i) 
	{
		cSoundEntity *pSoundEntity = mvSoundEntities[i];

		if(abX)	pSoundEntity->Play(false);
		else	pSoundEntity->Stop(false);	
	}
	
	ImplementedOnSetActive(abX);
}



//-----------------------------------------------------------------------

void iLuxProp::SetupEffectData()
{
	/////////////////////
	// Get particle data
	for(size_t i=0; i<mvParticleSystems.size(); ++i)
	{
		cParticleSystem *pPS = mvParticleSystems[i];
		cLuxProp_PSData psData;

		psData.msName = pPS->GetName().c_str();
		psData.msDataName = pPS->GetDataName();
		psData.m_mtxLocalTransform = pPS->GetLocalMatrix();
		psData.mpParent = pPS->GetEntityParent();

		mvEffectPSData.push_back(psData);
	}

	/////////////////////
	// Get light data
	for(size_t i=0; i<mvLights.size(); ++i)
	{
		iLight *pLight = mvLights[i];
		cLuxProp_LightData lightData;

		lightData.mOnColor = pLight->GetDiffuseColor();
		lightData.mfOnRadius = pLight->GetRadius();
		lightData.mbFlickering = pLight->GetFlickerActive();

		//TODO: Check all lights?
		if(i==0 && pLight->GetFlickerActive()) mbHasFlickering = true;

		mvEffectLightData.push_back(lightData);
	}

	/////////////////////
	// Get billboard data
	for(size_t i=0; i<mvBillboards.size(); ++i)
	{
		cBillboard *pBillboard = mvBillboards[i];
		cLuxProp_BillboardData bbData;

		bbData.mbConnectedToLight = BillboardConnectedToLight(pBillboard);
		bbData.mOnColor = pBillboard->GetColor();

		mvEffectBillboardData.push_back(bbData);	
	}
}

//-----------------------------------------------------------------------

bool iLuxProp::BillboardConnectedToLight(cBillboard *apBB)
{
	for(size_t light=0; light<mvLights.size(); ++light)
	{
		iLight *pLight = mvLights[light];
		std::vector<cLightBillboardConnection> *pBBVEc = pLight->GetBillboardVec();
		if(pBBVEc->empty()) continue;
        
		for(size_t i=0; i<pBBVEc->size(); ++i)
		{
			if( (*pBBVEc)[i].mpBillboard == apBB) return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------

void iLuxProp::UpdateEffectFading(float afTimeStep)
{
	if(mbEffectAlphaFading==false) return;

	/////////////////
	// Turn on effect
	if(mbEffectsActive)
	{
		mfEffectsAlpha += (1.0f/mfEffectsOnTime) * afTimeStep;
		if(mfEffectsAlpha > 1.0f)
		{
			mfEffectsAlpha = 1.0f;

			for(size_t i=0; i<mvLights.size(); ++i)
			{	
				iLight *pLight = mvLights[i];
				pLight->SetFlickerActive(mvEffectLightData[i].mbFlickering);
			}
			
			mbEffectAlphaFading = false;
		}
	}
	/////////////////
	// Turn off effect
	else
	{
		mfEffectsAlpha -= (1.0f/mfEffectsOffTime) * afTimeStep;
		if(mfEffectsAlpha < 0)
		{
			mfEffectsAlpha = 0;
			mbEffectAlphaFading = false;
		}
	}


	/////////////////////
	// Billboards
	for(size_t i=0; i<mvBillboards.size(); ++i)
	{
		cBillboard *pBillboard = mvBillboards[i];
		if(mvEffectBillboardData[i].mbConnectedToLight) continue;
		
		pBillboard->SetColor(mvEffectBillboardData[i].mOnColor * mfEffectsAlpha);
	}

	/////////////////////
	// Mesh illumination alpha
	if(mpMeshEntity)
	{
		mpMeshEntity->SetIlluminationAmount(mfEffectsAlpha);
	}
}


//-----------------------------------------------------------------------

void iLuxProp::UpdateMeshFading(float afTimeStep)
{
	if(mfFadeInAlpha >= 1 || mpMeshEntity==NULL) return;

    mfFadeInAlpha += mfFadeInSpeed * afTimeStep;
	mpMeshEntity->SetCoverageAmount(mfFadeInAlpha);
}

//-----------------------------------------------------------------------

void iLuxProp::UpdateAttachedProps(float afTimeStep, bool abForceUpdate)
{
	if(mlstAttachedProps.empty()) return;
	if(abForceUpdate==false && GetMainBody()->GetEnabled()==false) return;

	tLuxProp_AttachedPropListIt it = mlstAttachedProps.begin(); 
	for(; it != mlstAttachedProps.end(); ++it)
	{
		cLuxProp_AttachedProp *pAttachProp = *it;
		iLuxProp *pProp = pAttachProp->mpProp;
		if(pProp->GetBodyNum()<=0) continue;

		iPhysicsBody *pBody = pProp->GetBody(0);
		
		cMatrixf mtxProp = cMath::MatrixMul(GetMainBody()->GetLocalMatrix(), 
											cMath::MatrixMul(pAttachProp->m_mtxOffset, pProp->mvBodyExtraData[0].m_mtxLocalTransform));
		pBody->SetMatrix(mtxProp);
	}
}

//-----------------------------------------------------------------------

void iLuxProp::UpdateAnimation(float afTimeStep)
{
	if(mlCurrentNonLoopAnimIndex<0) return;

    cAnimationState *pAnimState = mpMeshEntity->GetAnimationState(mlCurrentNonLoopAnimIndex);	
	if(pAnimState->IsOver())
	{
		mlCurrentNonLoopAnimIndex = -1;
		if(msAnimCallback !="")
			mpMap->RunScript(msAnimCallback + "(\""+ msName + "\")");
	}
}

//-----------------------------------------------------------------------

void iLuxProp::UpdateMoveSoundVolume()
{
	if(mpMoveLoopSound==NULL) return;

	cSoundEntry *pEntry = mpMoveLoopSound->GetSoundEntry(eSoundEntityType_Main, true);
	if(pEntry) pEntry->SetVolumeMul(mfMovingVolume);
}

//-----------------------------------------------------------------------

void iLuxProp::UpdateMoving(float afTimeStep)
{
	////////////////////////////////
	// Update move start count
	if(mfMoveStartCount > 0)
	{
		mfMoveStartCount -= afTimeStep;
	}

	////////////////////////////////
	// Check if moving and update move sound
	if(mbMoving ==false)
	{
		//Fade out move sound
		if(mfMovingVolume >0)
		{
			mfMovingVolume -= afTimeStep;
			if(mfMovingVolume < 0)
			{
				mfMovingVolume =0;
				if(mpMoveLoopSound)
				{
					mpMoveLoopSound->Stop(false);
					mpMap->GetWorld()->DestroySoundEntity(mpMoveLoopSound);

					mpMoveLoopSound = NULL;
				}
			}
			else
			{
				UpdateMoveSoundVolume();
			}
		}

		return;
	}

	//////////////////////////////
	//Check if speed is enough!
	bool bHasMoveSpeed = false;
	if(mbMovingLinear)
	{
		const float fMinLinear = 0.001f * (1.0f/60.0f);

		float fSpeedSqr = cMath::Vector3DistSqr(m_mtxLastBodyMoveMatrix.GetTranslation(), GetMainBody()->GetLocalMatrix().GetTranslation());
		if(fSpeedSqr > fMinLinear*fMinLinear) bHasMoveSpeed = true;
	}
	if(bHasMoveSpeed == false && mbMovingAngular)
	{
		const float fMinAngular = 0.001f * (1.0f/60.0f);

		cVector3f vVel = cMath::MatrixEulerAngleDistance(m_mtxLastBodyMoveMatrix.GetRotation(), GetMainBody()->GetLocalMatrix().GetRotation());
		float fSpeedSqr = vVel.SqrLength();
		if(fSpeedSqr > fMinAngular*fMinAngular) bHasMoveSpeed = true;
	}

	//////////////////////////////
	//Fade in move sound
	if(bHasMoveSpeed && mfMovingVolume < 1)
	{
		//////////////////////////////
		// If no move sound, start it!
		if(mpMoveLoopSound==NULL && msMoveLoopSound != "")
		{
			mpMoveLoopSound = PlaySound("MoveLoop",msMoveLoopSound,false, true);
			if(mpMoveLoopSound)
			{
				mlMoveLoopSoundID = mpMoveLoopSound->GetCreationID();
				mpMoveLoopSound->Play(false);
				
				cSoundEntry* pEntry = mpMoveLoopSound->GetSoundEntry(eSoundEntityType_Main, true);
				if(pEntry) pEntry->SetVolumeMul(0);
			}
		}

		//////////////////////////////
		// Increase volume
		mfMovingVolume += afTimeStep*1.2f;//Important that sounds fades in faster than out! (in case mbMoving "flickers" )
		if(mfMovingVolume >1) mfMovingVolume =1.0f;
		
		//////////////////////////////
		// Update sound
		UpdateMoveSoundVolume();
	}
	//////////////////////////////
	//Fade out move sound
	else if(bHasMoveSpeed==false && mfMovingVolume > 0)
	{
		mfMovingVolume -= afTimeStep;
		if(mfMovingVolume <0)
		{
			mfMovingVolume =0;
			if(mpMoveLoopSound && mpMap->GetWorld()->SoundEntityExists(mpMoveLoopSound, mlMoveLoopSoundID))
			{
				mpMoveLoopSound->Stop(false);
				mpMap->GetWorld()->DestroySoundEntity(mpMoveLoopSound);
				
				mpMoveLoopSound = NULL;
				mlMoveLoopSoundID = -1;
			}
		}
		else
		{
			UpdateMoveSoundVolume();
		}
	}

	///////////////////////////
	// Save the last matrix before updating.
	m_mtxLastBodyMoveMatrix = GetMainBody()->GetLocalMatrix();

	///////////////////////////
	// Update movement matrix
    UpdateLinearMoving(afTimeStep);    
	UpdateAngularMoving(afTimeStep);

	///////////////////////////
	// Stop movement
	if(mbMovingLinear==false && mbMovingAngular==false)
	{
		StopMove();
	}
}

//-----------------------------------------------------------------------

void iLuxProp::UpdateLinearMoving(float afTimeStep)
{
	if(mbMovingLinear==false) return;
	
	iPhysicsBody *pBody = GetMainBody();

    cVector3f vDelta = mvMoveLinearGoal - pBody->GetLocalPosition();

	float fDist = vDelta.Length();

	cVector3f vMoveVel = 0;

	/////////////
	//Check if almost at goal
	if(fDist <= mfMoveLinearSpeed*afTimeStep*1.05f || fDist < 0.01)
	{
		mfMoveLinearSpeed =0;
		vMoveVel = vDelta / afTimeStep;
		mbMovingLinear = false;
	}
	/////////////
	//Update speed
	else
	{
		cVector3f vDir = vDelta / fDist;

		if(fDist < mfMoveLinearSlowdownDist)
		{
			mfMoveLinearSpeed = (fDist / mfMoveLinearSlowdownDist) * mfMoveLinearMaxSpeed;
		}
		else
		{
			mfMoveLinearSpeed += mfMoveLinearAcc * afTimeStep;
			if(mfMoveLinearSpeed > mfMoveLinearMaxSpeed) mfMoveLinearSpeed = mfMoveLinearMaxSpeed;
		}

		vMoveVel = vDir * mfMoveLinearSpeed;
	}

	//////////////////////////
	//Check collision, if force == 0 skip!
	if(mbMoveCheckCollision)
		CheckMoveCollision(vMoveVel, mfMoveLinearSpeed, afTimeStep);

	//////////////////////////
	//Update body position
	if(vMoveVel != 0)
	{
		pBody->StaticLinearMove(vMoveVel);
	}
}

//-----------------------------------------------------------------------

void iLuxProp::UpdateAngularMoving(float afTimeStep)
{
	if(mbMovingAngular==false) return;

	iPhysicsBody *pBody = GetMainBody();
	cVector3f vMoveVel =0;
	
	//Not used! Old stuff. The angles returned can be quite different at times when no rotaiton is needed (0,0,0) (360,0,360)
	//cVector3f vCurrentAngles =	cMath::MatrixToEulerAngles(pBody->GetLocalMatrix(), eEulerRotationOrder_XYZ);
	//cVector3f vWantedAngles =	cMath::MatrixToEulerAngles(m_mtxMoveAngularGoal, eEulerRotationOrder_XYZ);
	//cVector3f vDelta = cMath::Vector3AngleDistanceRad(vCurrentAngles, vWantedAngles);
	//Log("B---------------\n");
	//Log("Angles %s -> %s\n", cMath::Vector3ToDeg(vCurrentAngles).ToString().c_str(), cMath::Vector3ToDeg(vWantedAngles).ToString().c_str());
	
	/////////////////////////////////
	// Move without goal
	if(mbMoveAngularNoGoal)
	{
		float fAcc = mfMoveAngularMaxSpeed < mfMoveAngularSpeed ? -mfMoveAngularAcc : mfMoveAngularAcc;

		mfMoveAngularSpeed += fAcc * afTimeStep;
		if( (fAcc < 0 && mfMoveAngularSpeed < mfMoveAngularMaxSpeed) ||
			(fAcc > 0 && mfMoveAngularSpeed > mfMoveAngularMaxSpeed) )
		{
			mfMoveAngularSpeed = mfMoveAngularMaxSpeed;
			if(mfMoveAngularSpeed == 0)
			{
				mbMovingAngular = false;
			}
		}

		vMoveVel = mvMoveAngularNoGoalDir * mfMoveAngularSpeed;
	}
	/////////////////////////////////
	// Move with goal
	else
	{
		cVector3f vDelta = cMath::MatrixEulerAngleDistance(pBody->GetLocalMatrix(), m_mtxMoveAngularGoal);

		float fDist = vDelta.Length();
		
		/////////////
		//Check if almost at goal
		if(fDist <= mfMoveAngularSpeed*afTimeStep*1.05f || fDist < 0.01)
		{
			mfMoveAngularSpeed =0;
			vMoveVel = vDelta / afTimeStep;
			mbMovingAngular = false;
		}
		/////////////
		//Update speed
		else
		{
			cVector3f vDir = vDelta / fDist;

			if(fDist < mfMoveAngularSlowdownDist)
			{
				mfMoveAngularSpeed = (fDist / mfMoveAngularSlowdownDist) * mfMoveAngularMaxSpeed;
			}
			else
			{
				mfMoveAngularSpeed += mfMoveAngularAcc * afTimeStep;
				if(mfMoveAngularSpeed > mfMoveAngularMaxSpeed) mfMoveAngularSpeed = mfMoveAngularMaxSpeed;
			}

			vMoveVel = vDir * mfMoveAngularSpeed;
		}
	}

	//Log("Move speed: %s\n", vMoveVel.ToString().c_str());

	/////////////////////////////////
	// Rotate the body
	pBody->StaticAngularMove(vMoveVel);

	/////////////
	//Update position too.
	if(mbMoveAngularUseOffset)
	{
		cMatrixf mtxNewBody = cMath::MatrixMul(cMath::MatrixRotate(vMoveVel*afTimeStep,eEulerRotationOrder_XYZ), pBody->GetLocalMatrix().GetRotation());
		mtxNewBody.SetTranslation(pBody->GetLocalMatrix().GetTranslation());
		cVector3f vOffset = cMath::MatrixMul(mtxNewBody, mvMoveAngularLocalOffset);

		cVector3f vDiff = mvMoveAngularWorldOffset - vOffset;
		pBody->StaticLinearMove(vDiff / afTimeStep );
    }
}


//-----------------------------------------------------------------------

void iLuxProp::CheckMoveCollision(cVector3f& avMoveVel, float &afSpeed, float afTimeStep)
{
	if(avMoveVel==0) return;

	iPhysicsWorld *pPhysicsWorld = mpMap->GetPhysicsWorld();
	iPhysicsBody *pBody = GetMainBody();

	//Get matrix after movement
	cMatrixf mtxNew = pBody->GetLocalMatrix();
	cVector3f vAdd = avMoveVel*afTimeStep;
	mtxNew.SetTranslation(mtxNew.GetTranslation() + vAdd);

	//Log("Checking collision!\n");

	//Check collision and get push back
	cVector3f vPushVec(0);
	pPhysicsWorld->CheckShapeWorldCollision(&vPushVec,pBody->GetShape(),mtxNew, NULL, true, false);
	if(vPushVec == 0) return;
	
	//See how much of the velocity that needs to be "removed".
	float fAddLengthSqr  = vAdd.SqrLength();
	float fVelAmount = 1.0f - cMath::Vector3Dot(vPushVec, vAdd*-1.0f)/fAddLengthSqr;
	//fVelAmount -= 0.001f;
	if(fVelAmount < 0) fVelAmount =0;
	
	//Update speed and velocity
	afSpeed = afSpeed * fVelAmount;
	avMoveVel =  avMoveVel * fVelAmount;	
}

//-----------------------------------------------------------------------

void iLuxProp::UpdateCheckIfOutsidePlayer(float afTimeStep)
{
	if(mbCheckOutsidePlayer==false) return;

	///////////////////////
	// Set up variables
    bool bNotColliding = true;
	iPhysicsWorld *pPhysicsWorld = mpWorld->GetPhysicsWorld();

	cLuxPlayer *pPlayer = gpBase->mpPlayer;
	iPhysicsBody *pPlayerBody = pPlayer->GetCharacterBody()->GetCurrentBody();

	cCollideData collideData;
	collideData.SetMaxSize(1);
	
	///////////////////////
	// Iterate bodies
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodies[i];
		
		//Check this body is disabled.
		if(pBody->GetCollideCharacter()==true || mvDefaultBodySettings[i].mbCollideCharacter==false)
		{
			continue;
		}

        // Check shape collision
        cVector3f vPush(0);
		bool bCollide = pPhysicsWorld->CheckShapeCollision(	pBody->GetShape(),pBody->GetLocalMatrix(),
															pPlayerBody->GetShape(), pPlayerBody->GetLocalMatrix(),
															collideData,1,false);
		if(bCollide)
		{
			bNotColliding = false;

			/////////////////////////////////////////
			//Add a small impulse away from the player
			//  and be sure to make sure the speed in that dir is not too much!
			cVector3f vDir = pBody->GetLocalPosition() - pPlayerBody->GetLocalPosition();
			vDir.y *=0.1f; //Do not have much in the ydir!
			vDir.Normalize();
			
			cVector3f vVel = pBody->GetLinearVelocity();
			cVector3f vRelVel =  vDir * cMath::Vector3Dot(vVel, vDir);
			if(vRelVel.Length() < 0.5f)
			{
				pBody->AddImpulse(vDir *0.5f);
			}
		}
		else
		{
			pBody->SetCollideCharacter(true);
		}
	}

	///////////////////////
	// See if all bodies are outside of player
	if(bNotColliding)
	{
		mbCheckOutsidePlayer = false;
	}
}

//-----------------------------------------------------------------------

cVector3f iLuxProp::GetWorldCenterPos()
{
	cVector3f vCenterPos = 0;
	if(mvBodies.empty()==false)
	{
		iPhysicsBody *pBody = mpMainBody ? mpMainBody : mvBodies[0];
		vCenterPos = pBody->GetWorldPosition();
	}
	else
	{
		vCenterPos = mpMeshEntity->GetBoundingVolume()->GetWorldCenter();
	}
	return vCenterPos;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxPropConnectedProp)
kSerializeVar(msName, eSerializeType_String)
kEndSerialize()


//-----------------------------------------------------------------------

kBeginSerializeVirtual(iLuxProp_SaveData, iLuxEntity_SaveData)

kSerializeVar(msFileName, eSerializeType_String)
kSerializeVar(m_mtxOnLoadTransform, eSerializeType_Matrixf)
kSerializeVar(mvOnLoadScale, eSerializeType_Vector3f)

kSerializeVar(mfHealth, eSerializeType_Float32)

kSerializeVar(mbCheckOutsidePlayer, eSerializeType_Bool)

kSerializeVar(mbEffectsActive, eSerializeType_Bool)
kSerializeVar(mfEffectsAlpha, eSerializeType_Float32)

kSerializeVar(mbStaticPhysics, eSerializeType_Bool)

kSerializeVar(mbMoving, eSerializeType_Bool)

kSerializeVar(mbMovingLinear, eSerializeType_Bool)
kSerializeVar(mfMoveLinearMaxSpeed, eSerializeType_Float32)
kSerializeVar(mfMoveLinearAcc, eSerializeType_Float32)
kSerializeVar(mfMoveLinearSpeed, eSerializeType_Float32)
kSerializeVar(mfMoveLinearSlowdownDist, eSerializeType_Float32)
kSerializeVar(mvMoveLinearGoal, eSerializeType_Vector3f)

kSerializeVar(mbMovingAngular, eSerializeType_Bool)
kSerializeVar(mfMoveAngularMaxSpeed, eSerializeType_Float32)
kSerializeVar(mfMoveAngularAcc, eSerializeType_Float32)
kSerializeVar(mfMoveAngularSpeed, eSerializeType_Float32)
kSerializeVar(mfMoveAngularSlowdownDist, eSerializeType_Float32)
kSerializeVar(m_mtxMoveAngularGoal, eSerializeType_Matrixf)
kSerializeVar(mbMoveAngularUseOffset, eSerializeType_Bool)
kSerializeVar(mvMoveAngularWorldOffset, eSerializeType_Vector3f)
kSerializeVar(mvMoveAngularLocalOffset, eSerializeType_Vector3f)

kSerializeVar(mbMoveAngularNoGoal, eSerializeType_Bool)
kSerializeVar(mvMoveAngularNoGoalDir, eSerializeType_Vector3f)

kSerializeVar(mlCurrentNonLoopAnimIndex, eSerializeType_Int32)
kSerializeVar(msAnimCallback, eSerializeType_String)

kSerializeClassContainer(mvAttachedProps, cLuxProp_AttachedProp, eSerializeType_Class)

kSerializeClassContainer(mvConnectedProps, cLuxPropConnectedProp, eSerializeType_Class)

kSerializeVar(mMeshEntity, eSerializeType_Class)
kSerializeClassContainer(mvBodies, cEngineBody_SaveData, eSerializeType_Class)
kSerializeClassContainer(mvJoints, cEngineJoint_SaveData, eSerializeType_Class)
kSerializeClassContainer(mvPS, cEnginePS_SaveData, eSerializeType_Class)
kSerializeClassContainer(mvBeams, cEngineBeam_SaveData, eSerializeType_Class)
kSerializeClassContainer(mvLights, cEngineLight_SaveData, eSerializeType_Class)
kSerializeClassContainer(mvSounds, cEngineSound_SaveData, eSerializeType_Class)
kSerializeClassContainer(mvBillboards, cEngineBillboard_SaveData, eSerializeType_Class)

kSerializeClassContainer(mvInteractConnections, iLuxInteractConnection_SaveData, eSerializeType_ClassPointer)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxProp_SaveData::~iLuxProp_SaveData()
{
	for(size_t i=0; i<mvInteractConnections.Size(); ++i)
	{
		iLuxInteractConnection_SaveData *pSaveConn = mvInteractConnections[i];
		hplDelete(pSaveConn);
	}
}

//-----------------------------------------------------------------------

cEnginePS_SaveData* iLuxProp_SaveData::GetParticleSystem(cParticleSystem* apPS)
{
	if(apPS==NULL) return NULL;

	for(size_t i=0; i<mvPS.Size();++i)
		if(mvPS[i].msName == apPS->GetName()) return &mvPS[i];

	return NULL;
}
cEngineSound_SaveData* iLuxProp_SaveData::GetSoundEntity(cSoundEntity* apSound)
{
	for(size_t i=0; i<mvSounds.Size();++i)
		if(mvSounds[i].msName == apSound->GetName()) return &mvSounds[i];

	return NULL;
}
cEngineJoint_SaveData* iLuxProp_SaveData::GetJoint(iPhysicsJoint* apJoint)
{
	for(size_t i=0; i<mvJoints.Size();++i)
		if(mvJoints[i].msName == apJoint->GetName()) return &mvJoints[i];

	return NULL;
}

//-----------------------------------------------------------------------

iLuxEntity* iLuxProp_SaveData::CreateEntity(cLuxMap *apMap)
{
	cWorld *pWorld = apMap->GetWorld();
	apMap->ResetLatestEntity();
	pWorld->CreateEntity(msName,m_mtxOnLoadTransform, msFileName, mlID, true, mvOnLoadScale);

	return apMap->GetLatestEntity();
}

//-----------------------------------------------------------------------

void iLuxProp::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	iLuxProp_SaveData *pData = static_cast<iLuxProp_SaveData*>(apSaveData);

    cWorld *pWorld = mpWorld;
	iPhysicsWorld *pPhysicsWorld = pWorld->GetPhysicsWorld();

	///////////////////////
	//Properties
	kCopyToVar(pData, msFileName);
	kCopyToVar(pData, m_mtxOnLoadTransform);
	kCopyToVar(pData, mvOnLoadScale);

	kCopyToVar(pData, mfHealth);

	kCopyToVar(pData, mbCheckOutsidePlayer);

	kCopyToVar(pData, mbEffectsActive);
	kCopyToVar(pData, mfEffectsAlpha);

	kCopyToVar(pData, mbStaticPhysics);

	kCopyToVar(pData, mbMoving);

	kCopyToVar(pData, mbMovingLinear);
	kCopyToVar(pData, mfMoveLinearMaxSpeed);
	kCopyToVar(pData, mfMoveLinearAcc);
	kCopyToVar(pData, mfMoveLinearSpeed);
	kCopyToVar(pData, mfMoveLinearSlowdownDist);
	kCopyToVar(pData, mvMoveLinearGoal);

	kCopyToVar(pData, mbMovingAngular);
	kCopyToVar(pData, mfMoveAngularMaxSpeed);
	kCopyToVar(pData, mfMoveAngularAcc);
	kCopyToVar(pData, mfMoveAngularSpeed);
	kCopyToVar(pData, mfMoveAngularSlowdownDist);
	kCopyToVar(pData, m_mtxMoveAngularGoal);
	
	kCopyToVar(pData, mbMoveAngularNoGoal);
	kCopyToVar(pData, mvMoveAngularNoGoalDir);

	kCopyToVar(pData, mbMoveAngularUseOffset);
	kCopyToVar(pData, mvMoveAngularWorldOffset);
	kCopyToVar(pData, mvMoveAngularLocalOffset);

	kCopyToVar(pData, mlCurrentNonLoopAnimIndex);
	kCopyToVar(pData, msAnimCallback);

	///////////////////////
	//Attached props
    tLuxProp_AttachedPropListIt attachIt = mlstAttachedProps.begin();
	for(; attachIt != mlstAttachedProps.end(); ++attachIt)
	{
		cLuxProp_AttachedProp attachProp = *(*attachIt);
		attachProp.mpProp = NULL;
        pData->mvAttachedProps.Add(attachProp);
	}

	///////////////////////
	//Connection
	pData->mvConnectedProps.Resize(mvConnectedProps.size());
	for(size_t i=0; i<mvConnectedProps.size(); ++i)
		pData->mvConnectedProps[i].msName = mvConnectedProps[i];

	///////////////////////
	//Mesh Entity
	if(mpMeshEntity)
		pData->mMeshEntity.FromMeshEntity(mpMeshEntity);

	///////////////////////
	//Bodies
	pData->mvBodies.Resize(mvBodies.size());
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		pData->mvBodies[i].FromBody(mvBodies[i]);
	}

	///////////////////////
	//Joints
	pData->mvJoints.Resize(mvJoints.size());
	for(size_t i=0; i<mvJoints.size(); ++i)
	{
		pData->mvJoints[i].FromJoint(mvJoints[i], pPhysicsWorld);
	}

	///////////////////////
	//Particle Systems
	pData->mvPS.Resize(mvParticleSystems.size());
	for(size_t i=0; i<mvParticleSystems.size(); ++i)
	{
		if(pWorld->ParticleSystemExists(mvParticleSystems[i])==false)
		{
			mvParticleSystems[i] = NULL;
			Warning("particle system %d in %s does not exist anymore!\n",i,GetName().c_str());
		}

		pData->mvPS[i].FromPS(mvParticleSystems[i]);
	}
	
	///////////////////////
	//Lights
	pData->mvLights.Resize(mvLights.size());
	for(size_t i=0; i<mvLights.size(); ++i)
	{
		pData->mvLights[i].FromLight(mvLights[i]);
	}
	
	///////////////////////
	//Sounds
	pData->mvSounds.Resize(mvSoundEntities.size());
	for(size_t i=0; i<mvSoundEntities.size(); ++i)
	{
		pData->mvSounds[i].FromSound(mvSoundEntities[i]);
	}

	///////////////////////
	//Billboards
	pData->mvBillboards.Resize(mvBillboards.size());
	for(size_t i=0; i<mvBillboards.size(); ++i)
	{
		pData->mvBillboards[i].FromBillboard(mvBillboards[i]);
	}

	///////////////////////
	//Interact connections
	for(size_t i=0; i<mvInteractConnections.size(); ++i)
	{
		iLuxInteractConnection *pConn = mvInteractConnections[i];

        iLuxInteractConnection_SaveData *pSaveConn = pConn->CreateSaveData();
		pSaveConn->FromConnection(pConn);
		pData->mvInteractConnections.Add(pSaveConn);
	}
}

//-----------------------------------------------------------------------

void iLuxProp::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveData(apSaveData);
	iLuxProp_SaveData *pData = static_cast<iLuxProp_SaveData*>(apSaveData);

	cWorld *pWorld = mpWorld;
	iPhysicsWorld *pPhysicsWorld = pWorld->GetPhysicsWorld();

	///////////////////////
	//Properties
	kCopyFromVar(pData, m_mtxOnLoadTransform);
	kCopyFromVar(pData, mvOnLoadScale);
	
	kCopyFromVar(pData, mfHealth);

	kCopyFromVar(pData, mbCheckOutsidePlayer);

	SetEffectsActive(pData->mbEffectsActive, false);
	kCopyFromVar(pData, mfEffectsAlpha); 

	SetStaticPhysics(pData->mbStaticPhysics);

	kCopyFromVar(pData, mbMoving);
	kCopyFromVar(pData, mbMovingLinear);
	kCopyFromVar(pData, mfMoveLinearMaxSpeed);
	kCopyFromVar(pData, mfMoveLinearAcc);
	kCopyFromVar(pData, mfMoveLinearSpeed);
	kCopyFromVar(pData, mfMoveLinearSlowdownDist);
	kCopyFromVar(pData, mvMoveLinearGoal);

	kCopyFromVar(pData, mbMovingAngular);
	kCopyFromVar(pData, mfMoveAngularMaxSpeed);
	kCopyFromVar(pData, mfMoveAngularAcc);
	kCopyFromVar(pData, mfMoveAngularSpeed);
	kCopyFromVar(pData, mfMoveAngularSlowdownDist);
	kCopyFromVar(pData, m_mtxMoveAngularGoal);
	
	kCopyFromVar(pData, mbMoveAngularNoGoal);
	kCopyFromVar(pData, mvMoveAngularNoGoalDir);
	
	kCopyFromVar(pData, mbMoveAngularUseOffset);
	kCopyFromVar(pData, mvMoveAngularWorldOffset);
	kCopyFromVar(pData, mvMoveAngularLocalOffset);

	kCopyFromVar(pData, mlCurrentNonLoopAnimIndex);
	kCopyFromVar(pData, msAnimCallback);

	
	///////////////////////
	//Connections
	mvConnectedProps.resize(pData->mvConnectedProps.Size());
	for(size_t i=0; i<mvConnectedProps.size(); ++i)
		mvConnectedProps[i] = pData->mvConnectedProps[i].msName;

	///////////////////////
	//Mesh Entity
	if(mpMeshEntity)
		pData->mMeshEntity.ToMeshEntity(mpMeshEntity);

	///////////////////////
	//Bodies
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		pData->mvBodies[i].ToBody(mvBodies[i]);
	}

	///////////////////////
	//Joints
	for(size_t i=0; i<mvJoints.size(); ++i)
	{
		if(pData->mvJoints[i].mbBroken)
		{
			pPhysicsWorld->DestroyJoint(mvJoints[i]);
			mvJoints[i] = NULL;
		}
		else
		{
			pData->mvJoints[i].ToJoint(mvJoints[i]);
		}
	}

	///////////////////////
	//Lights
	for(size_t i=0; i<mvLights.size(); ++i)
	{
		pData->mvLights[i].ToLight(mvLights[i]);
	}
	
	///////////////////////
	//Particle Systems
	int lCount=0;
	for(std::vector<cParticleSystem*>::iterator it = mvParticleSystems.begin(); it != mvParticleSystems.end();)
	{
		cParticleSystem *pPS = *it;

		cEnginePS_SaveData *pSavePS = pData->GetParticleSystem(pPS);
		if(pSavePS)
		{
			pSavePS->ToPS(pPS);
			++it;
		}
		else
		{
			//check if a null was previously saved
			if(pData->mvPS.Size() == mvParticleSystems.size() && pData->mvPS[lCount].msType == "")
			{
				++it;
			}
			//a particle system has been removed.
			else
			{
				pWorld->DestroyParticleSystem(pPS);
				it = mvParticleSystems.erase(it);
			}
		}

		++lCount;
	}

	///////////////////////
	//Sounds
	for(std::vector<cSoundEntity*>::iterator it = mvSoundEntities.begin(); it != mvSoundEntities.end(); )
	{
		cSoundEntity *pSound = *it;
		cEngineSound_SaveData *pSaveSound = pData->GetSoundEntity(pSound);
		if(pSaveSound)
		{
			pSaveSound->ToSound(pSound);
			++it;
		}
		else
		{
			pWorld->DestroySoundEntity(pSound);
			it = mvSoundEntities.erase(it);
		}
	}

	///////////////////////
	//Billboards
	for(size_t i=0; i<mvBillboards.size(); ++i)
	{
		pData->mvBillboards[i].ToBillboard(mvBillboards[i]);
	}
}

//-----------------------------------------------------------------------

void iLuxProp::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
	iLuxProp_SaveData *pData = static_cast<iLuxProp_SaveData*>(apSaveData);

	///////////////////////
	//Attached props (want it in setup so it does not mess with set)
	for(size_t i=0; i<pData->mvAttachedProps.Size(); ++i)
	{
		cLuxProp_AttachedProp &attachProp = pData->mvAttachedProps[i];

		AddAndAttachProp(attachProp.msName, attachProp.msFileName, attachProp.m_mtxOffset);
	}

	///////////////////////
	//Interact connections
	for(size_t i=0; i<pData->mvInteractConnections.Size(); ++i)
	{
		iLuxInteractConnection_SaveData *pSaveConn = pData->mvInteractConnections[i];
		iLuxInteractConnection *pConn = pSaveConn->CreateConnection(mpMap);

		if(pConn) mvInteractConnections.push_back(pConn);
	}
}

//-----------------------------------------------------------------------

