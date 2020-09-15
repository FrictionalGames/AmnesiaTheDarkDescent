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

#include "LuxPlayerState_InteractGrab.h"

#include "LuxPlayer.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxMapHelper.h"
#include "LuxProp.h"
#include "LuxInputHandler.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerState_InteractGrab::cLuxPlayerState_InteractGrab(cLuxPlayer *apPlayer) : iLuxPlayerState_Interact(apPlayer, eLuxPlayerState_InteractGrab)
{
	mfMaxForce = gpBase->mpGameCfg->GetFloat("Player_Interaction","GrabMaxForce",0);
	mfMaxTorque = gpBase->mpGameCfg->GetFloat("Player_Interaction","GrabMaxTorque",0);
	mfMaxAngularSpeed = gpBase->mpGameCfg->GetFloat("Player_Interaction","GrabMaxAngularSpeed",0);

	mfMaxLeaveAngularSpeed = gpBase->mpGameCfg->GetFloat("Player_Interaction","GrabMaxLeaveAngularSpeed",0);
	mfMaxLeaveLinearSpeed = gpBase->mpGameCfg->GetFloat("Player_Interaction","GrabMaxLeaveLinearSpeed",0);

	mfMinSlowPlayerMass = gpBase->mpGameCfg->GetFloat("Player_Interaction","GrabMinSlowPlayerMass",0);
	mfMaxSlowPlayerMass = gpBase->mpGameCfg->GetFloat("Player_Interaction","GrabMaxSlowPlayerMass",0);
	mfMinSlowPlayerMul = gpBase->mpGameCfg->GetFloat("Player_Interaction","GrabMinSlowPlayerMul",0);

	mForcePid.SetErrorNum(20);;
	mSpeedTorquePid.SetErrorNum(20);

	mForcePid.p = 400;
	mForcePid.i = 0;
	mForcePid.d = 40;

	mSpeedTorquePid.p = 40;
	mSpeedTorquePid.i = 0;
	mSpeedTorquePid.d = 0.4f;
}

//-----------------------------------------------------------------------

cLuxPlayerState_InteractGrab::~cLuxPlayerState_InteractGrab()
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractGrab::OnEnterState(eLuxPlayerState aPrevState)
{
	/////////////////////////////////
	//Get the variables
	SetupInteractVars();

	cCamera *pCam = mpPlayer->GetCamera();

	mpGrabData = mpCurrentProp->GetGrabData();

	//////////////////////////////
	//Reset variables
	mForcePid.Reset();
	mSpeedTorquePid.Reset();

	//////////////////////////////////
	// Set the rotation as the current of the body
	if(mpGrabData->mbGrabUseOffset)
	{
		m_mtxBodyRotation = cMath::MatrixRotate(mpGrabData->mvGrabRotationOffset, eEulerRotationOrder_XYZ);	
	}
	//////////////////////////////////
	// Set rotation based on entity file offset
	else
	{
		cVector3f vCamRotation( pCam->GetPitch(), pCam->GetYaw(), pCam->GetRoll());
		cMatrixf mtxCamRot = cMath::MatrixRotate(vCamRotation, eEulerRotationOrder_XYZ);
		cMatrixf mtxInvCamRot = cMath::MatrixInverse(mtxCamRot);

		m_mtxBodyRotation = cMath::MatrixMul(mtxInvCamRot, mpCurrentBody->GetLocalMatrix().GetRotation());
		
		mvLocalBodyOffset = mpCurrentBody->GetLocalPosition() - mvCurrentFocusPos;
		mvLocalBodyOffset = cMath::MatrixMul( mtxInvCamRot, mvLocalBodyOffset);
	}
	
	if(mpGrabData->mbGrabUseDepth)
	{
		mfDepth = mpGrabData->mfGrabDepth;
	}
	else
	{
		mfDepth = cMath::Vector3Dist(mvCurrentFocusPos, pCam->GetPosition())-0.08f;
	}
	


	//////////////////////////////
	//Save properties of the current body and all bodies attached to it
	mvBodyProperties.clear();
	SaveBodyProperties(mpCurrentBody);

	//////////////////////////////
	//Set up properties for all bodies and calculate total mass
	mfMassSum = 0;
	float fProperMassSum=0;
	for(size_t i=0; i<mvBodyProperties.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodyProperties[i].mpBody;

		fProperMassSum += pBody->GetMass();

		if(pBody==mpCurrentBody) pBody->SetGravity(false);
		if(pBody==mpCurrentBody) pBody->SetCollideCharacter(false);
		pBody->SetMass(pBody->GetMass() * mpGrabData->mfGrabMassMul);
		
		pBody->SetAngularVelocity(0);
		pBody->SetLinearVelocity(0);

		mfMassSum += pBody->GetMass();
	}

	mpPlayer->GetCharacterBody()->SetMass(mpPlayer->GetDefaultMass() + fProperMassSum);

	mpCurrentProp->SetDisableCollisionUntilOutSidePlayer(false);

	if(fProperMassSum >= mfMinSlowPlayerMass)
	{
		if(fProperMassSum >= mfMaxSlowPlayerMass)
		{
			mpPlayer->SetInteractionMoveSpeedMul(mfMinSlowPlayerMul);
		}
		else
		{
			float fMul = (fProperMassSum-mfMinSlowPlayerMass)/(mfMaxSlowPlayerMass-mfMinSlowPlayerMass);
			fMul = fMul*mfMinSlowPlayerMul + (1.0f-fMul);
			mpPlayer->SetInteractionMoveSpeedMul(fMul);
		}
	}

	///////////////////////
	//Calculate the max distance
	mfMaxDistance = cMath::Vector3Dist(mpPlayer->GetCamera()->GetPosition(), mpCurrentBody->GetLocalPosition());
	mfMaxDistance = cMath::Max(mfMaxDistance, mpGrabData->mfGrabMaxDepth) * 1.1f + 0.2f;

	///////////////////////
	//Setup Material
	mbCustomMaterialsSetup = true;
	/*
	mbCustomMaterialsSetup = false;
	cMeshEntity *pMeshEntity = mpCurrentProp->GetMeshEntity();
	mvSubMeshProperties.resize(pMeshEntity->GetSubMeshEntityNum());
	for(int i=0; i<pMeshEntity->GetSubMeshEntityNum(); ++i)
	{
		cSubMeshEntity *pSubEnt = pMeshEntity->GetSubMeshEntity(i);
		cGrabbedSubMeshProperties *pSubProp = &mvSubMeshProperties[i];

		cMaterial *pOldMat = pSubEnt->GetMaterial();
		
		//////////////
		// Only make a transperant version if the material is solid diffuse
		tString sOldMatName = cString::ToLowerCase(pOldMat->GetType()->GetName());
        if(sOldMatName == "soliddiffuse")
		{
			iMaterialType *pMatType = gpBase->mpEngine->GetGraphics()->GetMaterialType("translucent");
			cMaterial* pCustomMat = hplNew( cMaterial, ("GrabTransCustom",	gpBase->mpEngine->GetGraphics(), 
																			gpBase->mpEngine->GetResources(), pMatType) );
			pCustomMat->SetDepthTest(true);
			pCustomMat->SetBlendMode(eMaterialBlendMode_Mul);

			iTexture *pDiffTex = pOldMat->GetTexture(eMaterialTexture_Diffuse);
			pDiffTex->IncUserCount();
			pCustomMat->SetTexture(eMaterialTexture_Diffuse,pDiffTex);
			
			pCustomMat->Compile();
			
			pSubProp->mpCustomTransMaterial = pCustomMat;
		}
		else
		{
			pSubProp->mpCustomTransMaterial = NULL;
		}

		/////////////////////////////////////
		// Set up material
		if(mbCustomMaterialsSetup==false)
		{
		cMeshEntity *pMeshEntity = mpCurrentProp->GetMeshEntity();
		for(int i=0; i<pMeshEntity->GetSubMeshEntityNum(); ++i)
		{
		cSubMeshEntity *pSubEnt = pMeshEntity->GetSubMeshEntity(i);
		cGrabbedSubMeshProperties *pSubProp = &mvSubMeshProperties[i];

		pSubEnt->SetCustomMaterial(pSubProp->mpCustomTransMaterial);
		}
		mbCustomMaterialsSetup = true;
		}

	}*/
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractGrab::OnLeaveState(eLuxPlayerState aNewState)
{
	///////////////////////
	//Reset Material
	/*cMeshEntity *pMeshEntity = mpCurrentProp->GetMeshEntity();
	for(int i=0; i<pMeshEntity->GetSubMeshEntityNum(); ++i)
	{
		cSubMeshEntity *pSubEnt = pMeshEntity->GetSubMeshEntity(i);
		cGrabbedSubMeshProperties *pSubProp = &mvSubMeshProperties[i];
		
		if(pSubProp->mpCustomTransMaterial)
		{
			pSubEnt->SetCustomMaterial(NULL, false);

			hplDelete(pSubProp->mpCustomTransMaterial);
			pSubProp->mpCustomTransMaterial = NULL;
		}
	}*/
	mvSubMeshProperties.clear();

	///////////////////////
	//Reset body
	for(size_t i=0; i<mvBodyProperties.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodyProperties[i].mpBody;

		pBody->SetGravity(mvBodyProperties[i].mbHasGravity);
		pBody->SetMass(mvBodyProperties[i].mfMass);

	}

	//Do want player to collide with prop until outside of it.
	mpCurrentProp->SetDisableCollisionUntilOutSidePlayer(true);

	// Need to so pointer exists! (as this might be on leave!)
	if(mpPlayer->GetCharacterBody())
	{
		mpPlayer->GetCharacterBody()->SetMass(mpPlayer->GetDefaultMass());
		mpPlayer->SetInteractionMoveSpeedMul(1.0f);
	}

	ResetInteractVars();
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractGrab::Update(float afTimeStep)
{
	
}

void cLuxPlayerState_InteractGrab::PostUpdate(float afTimeStep)
{
	/////////////////////////////////////
	// Init
	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	iPhysicsWorld *pPhysicsWorld = pMap->GetWorld()->GetPhysicsWorld();

	cCamera *pCam = mpPlayer->GetCamera();
	iCharacterBody *pPlayerBody = mpPlayer->GetCharacterBody();

	/////////////////////////////////////
	// Check if still in distance
	float fDistance = cMath::Vector3Dist(pCam->GetPosition(), mpCurrentBody->GetLocalPosition());
	if(fDistance > mfMaxDistance)
	{
		mpPlayer->ChangeState(mPreviousState);	
		return;
	}
	
	/////////////////////////////////////
	// Get the final body transform

	// Camera rotation
	cVector3f vCamRotation( pCam->GetPitch(), pCam->GetYaw(), pCam->GetRoll());
	cMatrixf mtxCamTransform = cMath::MatrixRotate(vCamRotation, eEulerRotationOrder_XYZ);
	mtxCamTransform.SetTranslation(pCam->GetPosition());


	// The base postion
	cVector3f vBasePos;
	if(mpGrabData->mbGrabUseOffset)
	{
		vBasePos = mpGrabData->mvGrabPositionOffset + cVector3f(0,0,-mfDepth);
	}
	else
	{
		vBasePos = cVector3f(0,0,-mfDepth) + mvLocalBodyOffset;
	}
	
	//The final body matrix.
	cMatrixf mtxGoal = cMath::MatrixMul(cMath::MatrixTranslate(vBasePos), m_mtxBodyRotation);
	mtxGoal = cMath::MatrixMul(mtxCamTransform,mtxGoal);
	
	///////////////////////
	//Force
	cVector3f vWantedPos = mtxGoal.GetTranslation();
	cVector3f vError = vWantedPos - mpCurrentBody->GetLocalPosition();

	cVector3f vForce = mForcePid.Output(vError, afTimeStep) * mfMassSum;
	
	//Make sure force is not too large
	vForce = cMath::Vector3MaxLength(vForce, mfMaxForce);

	mpCurrentBody->AddForce(vForce * mpGrabData->mfForceMul);


	if(mpGrabData->mbUseRotation==false) return;
	/////////////////////////
	// Get the wanted speed
	cVector3f vWantedRotSpeed=0;
	
	cMatrixf mtxGoalInv = cMath::MatrixInverse(mtxGoal);
	
	cVector3f vWantedUp = mtxGoalInv.GetUp();
	cVector3f vWantedRight = mtxGoalInv.GetRight();

	cMatrixf mtxBodyInv = cMath::MatrixInverse(mpCurrentBody->GetLocalMatrix());
	cVector3f vUp = mtxBodyInv.GetUp();
	cVector3f vRight = mtxBodyInv.GetRight();

	//Up alignment
	cVector3f vRotateAxis = cMath::Vector3Cross(vUp,vWantedUp);

	float fError = cMath::Vector3Angle(vWantedUp,vUp);
	vWantedRotSpeed += vRotateAxis * fError * 100;

	//Right alignment
	vRotateAxis = cMath::Vector3Cross(vRight,vWantedRight);

	fError = cMath::Vector3Angle(vWantedRight,vRight);
	vWantedRotSpeed += vRotateAxis * fError *100;

	//Make sure wanted speed is not too large
	float fSpeed = vWantedRotSpeed.Length();
	if(fSpeed > mfMaxAngularSpeed)
	{
		vWantedRotSpeed = (vWantedRotSpeed / fSpeed) * 6.0f;
	}
	
	/////////////////////////
	// Set speed by torque
	cVector3f vRotError = vWantedRotSpeed - mpCurrentBody->GetAngularVelocity();
		
	cVector3f vTorque =  mSpeedTorquePid.Output(vRotError,afTimeStep);
	vTorque = cMath::MatrixMul(mpCurrentBody->GetInertiaMatrix(), vTorque);

	//Make sure force is not too large
	vTorque = cMath::Vector3MaxLength(vTorque, mfMaxTorque);
	
	mpCurrentBody->AddTorque(vTorque * mpGrabData->mfTorqueMul);
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_InteractGrab::OnDoAction(eLuxPlayerAction aAction,bool abPressed)
{
	////////////////////////////
	// Interact
	if(aAction == eLuxPlayerAction_Interact)
	{
		// Pressed
		if(abPressed==false)
		{
			///////////////////////
			//Limit the body speed
			for(size_t i=0; i<mvBodyProperties.size(); ++i)
			{
				iPhysicsBody *pBody = mvBodyProperties[i].mpBody;

				float fLinearSpeed = pBody->GetLinearVelocity().Length();
				if(fLinearSpeed > mfMaxLeaveLinearSpeed)
					pBody->SetLinearVelocity( (pBody->GetLinearVelocity()/fLinearSpeed)*mfMaxLeaveLinearSpeed );

				float fAngularSpeed = pBody->GetAngularVelocity().Length();
				if(fAngularSpeed > mfMaxLeaveAngularSpeed)
					pBody->SetAngularVelocity( (pBody->GetAngularVelocity()/fAngularSpeed)*mfMaxLeaveAngularSpeed );
			}

			///////////////////////
			//Change state
			mpPlayer->ChangeState(mPreviousState);

			return false;
		}
	}
	////////////////////////////
	// Attack
	else if(aAction == eLuxPlayerAction_Attack)
	{
		// Pressed
		if(abPressed)
		{
			/////////////////////////////////////
			// Set the body position as right in front of camera
			cCamera *pCam = mpPlayer->GetCamera();
			cVector3f vCamRotation( pCam->GetPitch(), pCam->GetYaw(), pCam->GetRoll());
			cMatrixf mtxCamTransform = cMath::MatrixRotate(vCamRotation, eEulerRotationOrder_XYZ);
			mtxCamTransform.SetTranslation(pCam->GetPosition());

			/*cVector3f vBasePos = cVector3f(0,0,-mfDepth);
			m_mtxFinalTransform = cMath::MatrixMul(cMath::MatrixTranslate(vBasePos), m_mtxBodyRotation);
			m_mtxFinalTransform = cMath::MatrixMul(mtxCamTransform,m_mtxFinalTransform);

			mpCurrentBody->SetMatrix(m_mtxFinalTransform);*/

			/////////////////////////////////////
			// Add the impulse and reset speed
			mpCurrentBody->SetLinearVelocity(0);
			mpCurrentBody->SetAngularVelocity(0);
			mpCurrentBody->AddImpulse(pCam->GetForward() * mpGrabData->mfGrabThrowImpulse);

			mpPlayer->ChangeState(mPreviousState);

			return false;
		}
	}
	
	return true;
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractGrab::OnScroll(float afAmount)
{
	mfDepth += afAmount * mpGrabData->mfGrabDepthInc;
	if(mfDepth < mpGrabData->mfGrabMinDepth) mfDepth = mpGrabData->mfGrabMinDepth;
	if(mfDepth >mpGrabData-> mfGrabMaxDepth) mfDepth = mpGrabData->mfGrabMaxDepth;
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_InteractGrab::OnAddYaw(float afAmount)
{
	cInput* pInput = gpBase->mpEngine->GetInput();
	if(pInput->IsTriggerd(eLuxAction_Rotate) && mpGrabData->mbUseRotation)
	{
		m_mtxBodyRotation = cMath::MatrixMul(cMath::MatrixRotateY(afAmount * -3.2f),m_mtxBodyRotation);
		return false;
	}

	return true;
}

bool cLuxPlayerState_InteractGrab::OnAddPitch(float afAmount)
{
	cInput* pInput = gpBase->mpEngine->GetInput();
	if(pInput->IsTriggerd(eLuxAction_Rotate) && mpGrabData->mbUseRotation)
	{
#ifdef USE_GAMEPAD
		////////////////
		// We have both gamepad and mouse connected?
		if(gpBase->mpInputHandler->IsGamepadPresent())
		{
			/////////////
			// Check which one of them was used for the input this frame
#if USE_SDL2
			if(cMath::Abs(gpBase->mpInputHandler->GetGamepad()->GetAxisValue(eGamepadAxis_RightY)) > 0.0f)
#else
			if(cMath::Abs(gpBase->mpInputHandler->GetGamepad()->GetAxisValue(eGamepadAxis_3)) > 0.0f)
#endif
			{
				//Gamepad was used
				if(gpBase->mpInputHandler->GetInvertGamepadLook()) afAmount = -afAmount;
			}
			else if(gpBase->mpInputHandler->GetInvertMouse()) afAmount = -afAmount;
		}
		else if(gpBase->mpInputHandler->GetInvertMouse()) afAmount = -afAmount;
#else
		if(gpBase->mpInputHandler->GetInvertMouse()) afAmount = -afAmount;
#endif
		m_mtxBodyRotation = cMath::MatrixMul(cMath::MatrixRotateX(afAmount * -3.2f),m_mtxBodyRotation);
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------

cGuiGfxElement* cLuxPlayerState_InteractGrab::GetCrosshair()
{
	return NULL;
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractGrab::OnSaveBody(iPhysicsBody *apBody, float &afMass, bool &abCollideCharacter)
{
	for(size_t i=0; i<mvBodyProperties.size(); ++i)
	{
		if(mvBodyProperties[i].mpBody == apBody)
		{
			afMass = mvBodyProperties[i].mfMass;
			abCollideCharacter = mvBodyProperties[i].mbCollideCharacter;
			break;
		}
	}
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_InteractGrab::AllowBuoyancy(iPhysicsBody *apBody)
{
	iLuxEntity *pBodyEntity = static_cast<iLuxEntity*>(apBody->GetUserData());

	if(pBodyEntity == (iLuxEntity*)mpCurrentProp) return false;

	return true;
}

//-----------------------------------------------------------------------

float cLuxPlayerState_InteractGrab::DrawDebug(cGuiSet *apSet,iFontData *apFont, float afStartY)
{
	cVector3f vCurrentRot = cMath::MatrixToEulerAngles(mpCurrentBody->GetLocalMatrix().GetRotation(), eEulerRotationOrder_XYZ);
	cVector3f vWantedRot = cMath::MatrixToEulerAngles(m_mtxBodyRotation, eEulerRotationOrder_XYZ);
	cVector3f vRotError(	cMath::GetAngleDistanceRad(vCurrentRot.x, vWantedRot.x),
							cMath::GetAngleDistanceRad(vCurrentRot.y, vWantedRot.y),
							cMath::GetAngleDistanceRad(vCurrentRot.z, vWantedRot.z));

	vCurrentRot = cMath::Vector3ToDeg(vCurrentRot);
	vWantedRot = cMath::Vector3ToDeg(vWantedRot);
	vRotError = cMath::Vector3ToDeg(vRotError);


	/*apSet->DrawFont(apFont, cVector3f(5,afStartY,0),12,cColor(1,1),_W("Current: %ls"), cString::To16Char(vCurrentRot.ToString()).c_str());
	afStartY += 13;
	apSet->DrawFont(apFont, cVector3f(5,afStartY,0),12,cColor(1,1),_W("Wanted: %ls"), cString::To16Char(vWantedRot.ToString()).c_str());
	afStartY += 13;
	apSet->DrawFont(apFont, cVector3f(5,afStartY,0),12,cColor(1,1),_W("Error: %ls"), cString::To16Char(vRotError.ToString()).c_str());
	afStartY += 13;*/


	return afStartY;
}

void cLuxPlayerState_InteractGrab::RenderSolid(cRendererCallbackFunctions* apFunctions)
{
	return;
	apFunctions->SetMatrix(NULL);

	cCamera *pCam = mpPlayer->GetCamera();

	cVector3f vCamRotation( pCam->GetPitch(), pCam->GetYaw(), pCam->GetRoll());
	cMatrixf mtxCamTransform = cMath::MatrixRotate(vCamRotation, eEulerRotationOrder_XYZ);
	mtxCamTransform.SetTranslation(pCam->GetPosition());

	cVector3f vBasePos = mpGrabData->mvGrabPositionOffset + cVector3f(0,0,-mfDepth);
	cMatrixf mtxGoal = cMath::MatrixMul(cMath::MatrixTranslate(vBasePos), m_mtxBodyRotation);
	mtxGoal = cMath::MatrixMul(mtxCamTransform,mtxGoal);

	cMatrixf mtxGoalInv = cMath::MatrixInverse(mtxGoal);

	cVector3f vWantedUp = mtxGoalInv.GetUp();
	cVector3f vWantedRight = mtxGoalInv.GetRight();

	cMatrixf mtxBodyInv = cMath::MatrixInverse(mpCurrentBody->GetLocalMatrix());
	cVector3f vUp = mtxBodyInv.GetUp();
	cVector3f vRight = mtxBodyInv.GetRight();

	cVector3f vPos = mpCurrentBody->GetLocalPosition();
	apFunctions->GetLowLevelGfx()->DrawLine(vPos, vPos+vWantedUp, cColor(0,1,0,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vPos, vPos+vUp, cColor(0,1,1,1));

	apFunctions->GetLowLevelGfx()->DrawLine(vPos, vPos+vWantedRight, cColor(1,0,0,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vPos, vPos+vRight, cColor(1,0,1,1));

}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractGrab::SaveBodyProperties(iPhysicsBody *apBody)
{
	cGrabbedBodyProperties bodyData;

	bodyData.mpBody = apBody;
	bodyData.mbHasGravity = apBody->GetGravity();
	bodyData.mbCollideCharacter = apBody->GetCollideCharacter();
	bodyData.mfMass = apBody->GetMass();

	mvBodyProperties.push_back(bodyData);

    for(int i=0; i<apBody->GetJointNum();++i)
	{
		iPhysicsJoint *pJoint = apBody->GetJoint(i);
		iPhysicsBody *pChild = pJoint->GetChildBody();
		iPhysicsBody *pParent = pJoint->GetParentBody();
		if(pChild && BodyIsAdded(pChild)==false)  SaveBodyProperties(pChild);
		if(pParent && BodyIsAdded(pParent)==false)  SaveBodyProperties(pParent);
	}
}

//-----------------------------------------------------------------------


bool cLuxPlayerState_InteractGrab::BodyIsAdded(iPhysicsBody *apBody)
{
	for(size_t i=0; i<mvBodyProperties.size(); ++i)
	{
		if(mvBodyProperties[i].mpBody == apBody) return true;
	}
	return false;
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxPlayerState_InteractGrab_SaveData, iLuxPlayerState_Interact_SaveData)
kSerializeVar(m_mtxBodyRotation, eSerializeType_Matrixf)
kSerializeVar(mvLocalBodyOffset, eSerializeType_Vector3f)
kSerializeVar(mfDepth, eSerializeType_Float32)
kSerializeVar(mfMaxDistance, eSerializeType_Float32)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxPlayerState_SaveData* cLuxPlayerState_InteractGrab::CreateSaveData()
{
	return hplNew(cLuxPlayerState_InteractGrab_SaveData, ());
}

//-----------------------------------------------------------------------


void cLuxPlayerState_InteractGrab::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	cLuxPlayerState_InteractGrab_SaveData *pData = static_cast<cLuxPlayerState_InteractGrab_SaveData*>(apSaveData);

	
	///////////////////////
	// Save vars
	kCopyToVar(pData, m_mtxBodyRotation);
	kCopyToVar(pData, mvLocalBodyOffset);
	kCopyToVar(pData, mfDepth);
	kCopyToVar(pData, mfMaxDistance);
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractGrab::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
	cLuxPlayerState_InteractGrab_SaveData *pData = static_cast<cLuxPlayerState_InteractGrab_SaveData*>(apSaveData);

	///////////////////////
	// Setup before entering
	
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractGrab::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
	cLuxPlayerState_InteractGrab_SaveData *pData = static_cast<cLuxPlayerState_InteractGrab_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
	kCopyFromVar(pData, m_mtxBodyRotation);
	kCopyFromVar(pData, mvLocalBodyOffset);
	kCopyFromVar(pData, mfDepth);
	kCopyFromVar(pData, mfMaxDistance);
}

//-----------------------------------------------------------------------



