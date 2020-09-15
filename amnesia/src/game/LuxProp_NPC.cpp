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

#include "LuxProp_NPC.h"

#include "LuxPlayer.h"
#include "LuxMessageHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxInventory.h"

#include "LuxMap.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_NPC::cLuxPropLoader_NPC(const tString& asName) : iLuxPropLoader(asName)
{
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_NPC::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_NPC, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_NPC::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_NPC  *pNPC = static_cast<cLuxProp_NPC*>(apProp);

	///////////////////////////
	// Get the move head bone data
	tStringVec vBoneNameVec;
	cString::GetStringVec(GetVarString("MoveHeadBones", ""), vBoneNameVec);

	tFloatVec vBoneMulVec;
	cString::GetFloatVec(GetVarString("MoveHeadBoneMuls", ""), vBoneMulVec);

	//Check so there is a skeleton
	if(mpMesh->GetSkeleton()==NULL)
	{
		Error("NPC '%s' is missing skeleton!\n", msFileName.c_str());
	}
	// Check so input is correct
    else if(vBoneNameVec.size()==0 || vBoneNameVec.size() != vBoneMulVec.size())
	{
		Error("MoveHeadBones array is emtpy or do not match size of MoveHeadBoneMuls for NPC '%s'\n", msFileName.c_str());
	}
	//Add arrays!
	else
	{
		pNPC->mvHeadMoveBones.resize(vBoneNameVec.size());
		for(size_t i=0; i<vBoneNameVec.size(); ++i)
		{
			cLuxProp_NPC_HeadMoveBone& boneData = pNPC->mvHeadMoveBones[i];

			//////////////////
			//Bone index
			boneData.mlBoneIdx = mpMesh->GetSkeleton()->GetBoneIndexByName(vBoneNameVec[i]);
			if(boneData.mlBoneIdx <-1)
			{
				Error("Move head Bone '%s' does not exist in '%s'!\n", vBoneNameVec[i].c_str(), msFileName.c_str());
				continue;
			}
			
			//////////////////
			//Multiplier
			boneData.mfMul = vBoneMulVec[i];

			//////////////////
			//Rotation vector (figure out what local coord becomes up (0,1,0) in bone
			cBone *pBone = mpMesh->GetSkeleton()->GetBoneByIndex(boneData.mlBoneIdx);
			cMatrixf mtxInvWorldBone = cMath::MatrixInverse(pBone->GetWorldTransform());
			boneData.mvRotVec = cMath::Vector3Normalize(cMath::MatrixMul(mtxInvWorldBone, cVector3f(0,1,0))); //Make sure to normalize since matrix might have scale!
		}
	}

	
	///////////////////////////
	// General
	pNPC->mfHeadMoveSpeedMul = GetVarFloat("HeadMoveSpeedMul", 0);
	pNPC->mfHeadMoveMaxSpeed = GetVarFloat("HeadMoveMaxSpeed", 0);
	pNPC->mfMaxHeadAngle = cMath::ToRad(GetVarFloat("MaxHeadAngle", 0));
}

//-----------------------------------------------------------------------

void cLuxPropLoader_NPC::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_NPC  *pNPC = static_cast<cLuxProp_NPC*>(apProp);

	pNPC->msFollowPlayerArea = apInstanceVars->GetVarString("FollowPlayerArea","");
	pNPC->SetAwake(apInstanceVars->GetVarBool("Awake", true), false);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_NPC::cLuxProp_NPC(const tString &asName, int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_NPC)
{
	 mfHeadAngle = 0;
	 mfInAreaCount =0;
	 mpFollowPlayerArea = NULL;
	 mbPlayerIsInArea = false;
	 mbAwake = true;
	 mbPlayingWakeAnim = false;
	 mbFollowPlayer = true;
}

//-----------------------------------------------------------------------

cLuxProp_NPC::~cLuxProp_NPC()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxProp_NPC::AfterWorldLoad()
{
	if(msFollowPlayerArea == "") 
	{
		Warning("NPC '%s' does not have a follow player area!\n", msName.c_str());
		return;
	}

    mpFollowPlayerArea = mpMap->GetEntityByName(msFollowPlayerArea);
	if(mpFollowPlayerArea==NULL)
	{
		Error("Could not find follow area '%s' for NPC '%s'!\n",msFollowPlayerArea.c_str(), msName.c_str());
	}
}

//-----------------------------------------------------------------------

bool cLuxProp_NPC::CanInteract(iPhysicsBody *apBody)
{
	return false;
}

//-----------------------------------------------------------------------

bool cLuxProp_NPC::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return false;
}

//-----------------------------------------------------------------------

void cLuxProp_NPC::OnSetupAfterLoad(cWorld *apWorld)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_NPC::OnResetProperties()
{

}

//-----------------------------------------------------------------------

void cLuxProp_NPC::UpdatePropSpecific(float afTimeStep)
{
	UpdateWakeState(afTimeStep);
	UpdateCheckPlayerIsInArea(afTimeStep);
	UpdateHeadMovement(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxProp_NPC::BeforePropDestruction()
{

}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_NPC::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return eLuxFocusCrosshair_Default;
}

//-----------------------------------------------------------------------

void cLuxProp_NPC::SetAwake(bool abX, bool abEffects)
{
	if(mbAwake == abX) return;

	mbAwake = abX;

	if(abEffects)
	{
		if(mbAwake)	PlayAnimation("Awake", 0.3f, false, "");
		else		PlayAnimation("Sleep", 0.3f, false, "");

		mbPlayingWakeAnim = true;
	}
	else
	{
		if(mbAwake)	PlayAnimation("Idle", 0.05f, true, "");
		else		PlayAnimation("SleepIdle", 0.05f, true, "");
	}

}

//-----------------------------------------------------------------------

void cLuxProp_NPC::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
	
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxProp_NPC::UpdateWakeState(float afTimeStep)
{
	if(mbPlayingWakeAnim==false) return;

	if(mlCurrentNonLoopAnimIndex <0)
	{
		mbPlayingWakeAnim = false;

		if(mbAwake)	PlayAnimation("Idle", 0.3f, true, "");
		else		PlayAnimation("SleepIdle", 0.3f, true, "");
	}
}

//-----------------------------------------------------------------------

void cLuxProp_NPC::UpdateCheckPlayerIsInArea(float afTimeStep)
{
	if(mpFollowPlayerArea==NULL) return;

	///////////////////////
	// Only check twice per second.
	mfInAreaCount -= afTimeStep;
	if(mfInAreaCount > 0) return;
	mfInAreaCount = 0.5f;

	///////////////////////
	//Check collision with player
	mbPlayerIsInArea = mpMap->CheckCollision(mpFollowPlayerArea, gpBase->mpPlayer);
}

//-----------------------------------------------------------------------

void cLuxProp_NPC::UpdateHeadMovement(float afTimeStep)
{
	////////////////////////////////
	// Calculate angle to player
	float fWantedAngle = 0;
	
	/////////////////////////////////
	// If player is in area, look at him!
	if(mbPlayerIsInArea && mbAwake && mbFollowPlayer)
	{
		//Get the wanted directions with y=0
		cMatrixf mtxMeshInv = cMath::MatrixInverse(mpMeshEntity->GetWorldMatrix());
		cVector3f vRight = mtxMeshInv.GetRight();
		cVector3f vFwd = mtxMeshInv.GetForward();
		cVector3f vToPlayer = gpBase->mpPlayer->GetCharacterBody()->GetPosition() - mpMeshEntity->GetWorldPosition();
		vFwd.y =0; vFwd.Normalize();
		vRight.y =0; vRight.Normalize();
		vToPlayer.y=0; vToPlayer.Normalize();
		
		//Calculate angle and get correct sign.
		fWantedAngle = cMath::Vector3Angle(vFwd, vToPlayer);
		if(cMath::Vector3Dot(vRight, vToPlayer)<0)	fWantedAngle = -fWantedAngle;

		fWantedAngle = cMath::Clamp(fWantedAngle, -mfMaxHeadAngle, mfMaxHeadAngle);
	}

	////////////////////////////////
	// Move the head angle (if angle is not very close to wanted.
	if(fabs(mfHeadAngle - fWantedAngle)> 0.0001f)
	{
		float fAngleDist = cMath::GetAngleDistanceRad(mfHeadAngle, fWantedAngle);

		//Calculate the turn speed and clamp to max
		float fTurnSpeed = cMath::Clamp(mfHeadMoveSpeedMul * fAngleDist, -mfHeadMoveMaxSpeed, mfHeadMoveMaxSpeed);
		
		mfHeadAngle += fTurnSpeed * afTimeStep;
	}

	////////////////////////////////
	// Set the bones for head movement
	for(size_t i=0; i<mvHeadMoveBones.size(); ++i)
	{
		cLuxProp_NPC_HeadMoveBone& boneData = mvHeadMoveBones[i];
		if(boneData.mlBoneIdx<0) continue;
        
		cBoneState *pBoneState = mpMeshEntity->GetBoneState(boneData.mlBoneIdx);

		pBoneState->SetUsePreTransform(true);

		cQuaternion qHeadRotate(mfHeadAngle*boneData.mfMul, boneData.mvRotVec);
		pBoneState->SetPreTransform(cMath::MatrixQuaternion(qHeadRotate));
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// DEBUG
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

float cLuxProp_NPC::DrawDebug(cGuiSet *apSet,iFontData *apFont,float afStartY)
{
	//apSet->DrawFont(apFont, cVector3f(5,afStartY,1), 13,cColor(1,1),_W("Angle: %f"), cMath::ToDeg(mfHeadAngle));
	//afStartY +=14;

	return afStartY;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_NPC_SaveData, iLuxProp_SaveData)

kSerializeVar(msFollowPlayerArea, eSerializeType_String)
kSerializeVar(mfHeadAngle, eSerializeType_Float32)
kSerializeVar(mbPlayerIsInArea, eSerializeType_Bool)
kSerializeVar(mbAwake, eSerializeType_Bool)
kSerializeVar(mbPlayingWakeAnim, eSerializeType_Bool)
kSerializeVar(mbFollowPlayer, eSerializeType_Bool)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_NPC::CreateSaveData()
{
	return hplNew(cLuxProp_NPC_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_NPC::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_NPC_SaveData *pData = static_cast<cLuxProp_NPC_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData, msFollowPlayerArea);
	kCopyToVar(pData, mfHeadAngle);
	kCopyToVar(pData, mbPlayerIsInArea);
	kCopyToVar(pData, mbAwake);
	kCopyToVar(pData, mbPlayingWakeAnim);
	kCopyToVar(pData, mbFollowPlayer);
}

//-----------------------------------------------------------------------

void cLuxProp_NPC::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_NPC_SaveData *pData = static_cast<cLuxProp_NPC_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData, msFollowPlayerArea);
	kCopyFromVar(pData, mfHeadAngle);
	kCopyFromVar(pData, mbPlayerIsInArea);
	kCopyFromVar(pData, mbAwake);
	kCopyFromVar(pData, mbPlayingWakeAnim);
	kCopyFromVar(pData, mbFollowPlayer);
}

//-----------------------------------------------------------------------

void cLuxProp_NPC::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
