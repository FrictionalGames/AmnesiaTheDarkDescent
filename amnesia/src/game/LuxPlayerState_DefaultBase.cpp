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

#include "LuxPlayerState_DefaultBase.h"

#include "LuxPlayer.h"
#include "LuxMapHelper.h"
#include "LuxProp.h"
#include "LuxMoveState.h"
#include "LuxEffectRenderer.h"
#include "LuxPlayerHands.h"



//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxPlayerState_DefaultBase::iLuxPlayerState_DefaultBase(cLuxPlayer *apPlayer, eLuxPlayerState aType) : iLuxPlayerState(apPlayer, aType)
{
	mbCurrentEntityHasOutline = false;

	/////////////////////////////
	//Load crosshair gfx
	cGui *pGui = gpBase->mpEngine->GetGui();

	mvFocusCrosshairGfx.resize(eLuxFocusCrosshair_LastEnum, NULL);
	mvFocusCrosshairGfx[eLuxFocusCrosshair_Grab] = pGui->CreateGfxImage("hud_crosshair_over_grab.tga",eGuiMaterial_Alpha);
	mvFocusCrosshairGfx[eLuxFocusCrosshair_Push] = pGui->CreateGfxImage("hud_crosshair_over_push.tga",eGuiMaterial_Alpha);
	mvFocusCrosshairGfx[eLuxFocusCrosshair_Ignite] = pGui->CreateGfxImage("hud_crosshair_over_ignite.tga",eGuiMaterial_Alpha);
	mvFocusCrosshairGfx[eLuxFocusCrosshair_Pick] = pGui->CreateGfxImage("hud_crosshair_over_pick.tga",eGuiMaterial_Alpha);
	mvFocusCrosshairGfx[eLuxFocusCrosshair_LevelDoor] = pGui->CreateGfxImage("hud_crosshair_over_leveldoor.tga",eGuiMaterial_Alpha);
	mvFocusCrosshairGfx[eLuxFocusCrosshair_Ladder] = pGui->CreateGfxImage("hud_crosshair_over_ladder.tga",eGuiMaterial_Alpha);
}

//-----------------------------------------------------------------------

iLuxPlayerState_DefaultBase::~iLuxPlayerState_DefaultBase()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::OnEnterState(eLuxPlayerState aPrevState)
{
	mpBodyInFocus = NULL;
	mpEntityInFocus = NULL;

    ImplementedOnEnterState(aPrevState);
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::OnLeaveState(eLuxPlayerState aNewState)
{
	gpBase->mpEffectRenderer->ClearOutlineObjects();

	ImplementedOnLeaveState(aNewState);
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::OnMapEnter(cLuxMap *apMap)
{
	
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::OnMapLeave(cLuxMap *apMap)
{
	mpBodyInFocus = NULL;
	mpEntityInFocus = NULL;
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::Update(float afTimeStep)
{
	/////////////////////////////////////////
	// Get the entity currently in focus
	cCamera *pCam = mpPlayer->GetCamera();

	gpBase->mpMapHelper->GetClosestEntity(	pCam->GetPosition(), pCam->GetForward(), 20.0f, 
											&mfFocusDistance,&mpBodyInFocus,&mpEntityInFocus);
	mvFocusPos = pCam->GetPosition() + pCam->GetForward()*mfFocusDistance;

	AddOutlineObjects(mpBodyInFocus,mpEntityInFocus, mvFocusPos);

	//For other classes to use focused
	mpPlayer->SetEntityInFocus(mpEntityInFocus);
	mpPlayer->SetBodyInFocus(mpBodyInFocus);
	mpPlayer->SetCurrentFocusDistance(mfFocusDistance);

	if(mpEntityInFocus && CanInteractWithEntity())
		mpPlayer->SetFocusText(mpEntityInFocus->GetFocusText());
	else
		mpPlayer->SetFocusText(_W(""));

	//update implemented class
	ImplementedUpdate(afTimeStep);
}

void iLuxPlayerState_DefaultBase::PostUpdate(float afTimeStep)
{
	
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::OnDraw(cGuiSet *apGuiSet ,float afFrameTime)
{
	if(mpEntityInFocus && CanInteractWithEntity())
	{
		mpEntityInFocus->InFocusDraw(apGuiSet, afFrameTime);
	}
}

//-----------------------------------------------------------------------

bool iLuxPlayerState_DefaultBase::OnDoAction(eLuxPlayerAction aAction,bool abPressed)
{
	if(mpPlayer->GetCurrentMoveStateData()->GetAllowsInteraction()==false) return true;

	////////////////////////////
	// Implemented
	if(ImplementedDoAction(aAction, abPressed)==false)
	{
		return false;
	}

	////////////////////////////
	// Interact
	if(aAction == eLuxPlayerAction_Interact)
	{
		// Pressed
		if(abPressed)
		{
			if(CanInteractWithEntity())
			{
				mpEntityInFocus->OnInteract(mpBodyInFocus,mvFocusPos);
				mpEntityInFocus->RunInteractCallbackFunc();
			}
			return false;
		}
	}
	

	return true;
}

//-----------------------------------------------------------------------

cGuiGfxElement* iLuxPlayerState_DefaultBase::GetCrosshair()
{ 
	if(mpEntityInFocus && mfFocusDistance < mpEntityInFocus->GetMaxFocusDistance())
	{
		eLuxFocusCrosshair crossHair = mpEntityInFocus->GetFocusCrosshair(mpBodyInFocus, mvFocusPos);
		if(mpEntityInFocus->GetInteractionDisabled()) crossHair = eLuxFocusCrosshair_Default;

		if(	crossHair == eLuxFocusCrosshair_LastEnum ||
			crossHair == eLuxFocusCrosshair_Default)
		{
			return mpDefaultCrosshairGfx;
		}
		else
		{
			if(mpPlayer->GetFocusIconStyle()==eLuxFocusIconStyle_Default)
				return mvFocusCrosshairGfx[crossHair];
			else
				return mpSimpleInteractCrosshairGfx;
		}
	}
	else
	{
		return mpDefaultCrosshairGfx;
	}
}

//-----------------------------------------------------------------------

float iLuxPlayerState_DefaultBase::DrawDebug(cGuiSet *apSet,iFontData *apFont, float afStartY)
{
	tWString sBodyName = mpBodyInFocus ? cString::To16Char(mpBodyInFocus->GetName()) : _W("None");
	apSet->DrawFont(apFont, cVector3f(5, afStartY,10),14,cColor(1,1),_W("Focus Body: %ls Dist: %f"), sBodyName.c_str(),mfFocusDistance);

	afStartY += 15;

	return afStartY;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool iLuxPlayerState_DefaultBase::ShowOutlineOnEntity(iLuxEntity *apEntity, iPhysicsBody *apBody, const cVector3f &avFocusPos)
{
	if(apEntity->GetEntityType() != eLuxEntityType_Prop) return false;

	iLuxProp *pProp = static_cast<iLuxProp*>(apEntity);

	return pProp->GetPropType()==eLuxPropType_Item && apEntity->CanInteract(apBody) && apEntity->GetInteractionDisabled()==false;
}

//-----------------------------------------------------------------------

bool iLuxPlayerState_DefaultBase::CanInteractWithEntity()
{
	if(	mpEntityInFocus && mfFocusDistance < mpEntityInFocus->GetMaxFocusDistance() && 
		mpEntityInFocus->CanInteract(mpBodyInFocus) && mpEntityInFocus->GetInteractionDisabled()==false)
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------

static bool BodyIsAdded(iPhysicsBody *apBody, tPhysicsBodyList &alstBodies)
{
	for(tPhysicsBodyListIt it = alstBodies.begin(); it != alstBodies.end(); ++it)
	{
		if(apBody == *it) return true;
	}
	return false;
}

void iLuxPlayerState_DefaultBase::AddOutlineObjects(iPhysicsBody *apBody, iLuxEntity *apEntity, const cVector3f &avFocusPos)
{
	gpBase->mpEffectRenderer->ClearOutlineObjects();
	if(	apEntity==NULL || 
		apEntity->GetEntityType()!= eLuxEntityType_Prop || 
		mfFocusDistance > apEntity->GetMaxFocusDistance() || 
		ShowOutlineOnEntity(apEntity, apBody, avFocusPos)==false)
	{
		mbCurrentEntityHasOutline = false;
		return;
	}

	mbCurrentEntityHasOutline = true;

	iLuxProp *pProp = static_cast<iLuxProp*>(apEntity);
	
	/////////////////////////////
	//Show all connected bodies
	if(pProp->ShowOutlinesOnConnectedBodies())
	{
		tPhysicsBodyList lstAttachedBodies;
		GetAttachedBodies(apBody, lstAttachedBodies);
		
		cMeshEntity *pMeshEntity = pProp->GetEffectMeshEntity();
		if(pMeshEntity==NULL) return;
		for(int i=0; i<pMeshEntity->GetSubMeshEntityNum(); ++i)
		{
			cSubMeshEntity *pSubEnt = pMeshEntity->GetSubMeshEntity(i);
	        
			if(BodyIsAdded(static_cast<iPhysicsBody*>(pSubEnt->GetEntityParent()), lstAttachedBodies))
			{
				gpBase->mpEffectRenderer->AddOutlineObject(pSubEnt);
			}
		}
	}
	/////////////////////////////
	//Show only focused body.
	else
	{
		cMeshEntity *pMeshEntity = pProp->GetEffectMeshEntity();
		for(int i=0; i<pMeshEntity->GetSubMeshEntityNum(); ++i)
		{
			cSubMeshEntity *pSubEnt = pMeshEntity->GetSubMeshEntity(i);

			if(static_cast<iPhysicsBody*>(pSubEnt->GetEntityParent()) == apBody)
			{
				gpBase->mpEffectRenderer->AddOutlineObject(pSubEnt);
			}
		}
	}
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::GetAttachedBodies(iPhysicsBody *apBody, tPhysicsBodyList &alstBodies)
{
	alstBodies.push_back(apBody);

	for(int i=0; i<apBody->GetJointNum();++i)
	{
		iPhysicsJoint *pJoint = apBody->GetJoint(i);
		iPhysicsBody *pChild = pJoint->GetChildBody();
		iPhysicsBody *pParent = pJoint->GetParentBody();
		if(pChild && BodyIsAdded(pChild, alstBodies)==false)  GetAttachedBodies(pChild,alstBodies);
		if(pParent && BodyIsAdded(pParent, alstBodies)==false)  GetAttachedBodies(pParent,alstBodies);
	}
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeVirtual(iLuxPlayerState_DefaultBase_SaveData, iLuxPlayerState_SaveData)
kEndSerialize()

//-----------------------------------------------------------------------


void iLuxPlayerState_DefaultBase::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	iLuxPlayerState_DefaultBase_SaveData *pData = static_cast<iLuxPlayerState_DefaultBase_SaveData*>(apSaveData);


	///////////////////////
	// Save vars
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
	iLuxPlayerState_DefaultBase_SaveData *pData = static_cast<iLuxPlayerState_DefaultBase_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
	iLuxPlayerState_DefaultBase_SaveData *pData = static_cast<iLuxPlayerState_DefaultBase_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------


