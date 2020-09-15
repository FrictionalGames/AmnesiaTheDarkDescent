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

#include "physics/PhysicsBody.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsJoint.h"
#include "system/LowLevelSystem.h"
#include "physics/PhysicsMaterial.h"
#include "physics/SurfaceData.h"
#include "physics/CharacterBody.h"
#include "physics/VerletParticle.h"

#include "scene/World.h"
#include "scene/Scene.h"
#include "engine/Engine.h"

#include "scene/SoundEntity.h"
#include "sound/SoundHandler.h"
#include "sound/SoundChannel.h"

#include "math/Math.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iPhysicsBody::iPhysicsBody(const tString &asName,iPhysicsWorld *apWorld,iCollideShape *apShape) 
		: iEntity3D(asName)
	{
		mpWorld = apWorld;
		mpShape = apShape;
		
		//Increment user count for the shape
		apShape->IncUserCount();

		mBoundingVolume.SetLocalMinMax(apShape->GetBoundingVolume().GetMin(),
										apShape->GetBoundingVolume().GetMax());

		//Set the default material so that body always has a material.
		mpMaterial = mpWorld->GetMaterialFromName("Default");

		mpCharacterBody = NULL;

		mbBlocksSound = false;
		mbBlocksLight = true;
		mbUseSurfaceEffects = true;
	
		mpScrapeBody = NULL;
		mpScrapeSoundEntity = NULL;
		mlScrapeSoundEntityID = -1;
		mpRollSoundEntity = NULL;
		mlRollSoundEntityID = -1;
		mbHasSlide = false;

		mfBuoyancyDensityMul = 1.0f;

		mlSlideCount = 0;
		mlImpactCount = 0;

		mbInUpdateList = false;

		mlPushStrength = 0;

		mbGravityCanAttachCharacter = true;
		mbGravityAttachmentRotation = true;
		mbGravityAttachmentVelocity= true;
		mGravityAttachmentVelocityAxes = eVelocityAxes_XYZ;

		mpUserData = NULL;

		mpHapticShape = NULL;

		mbPushedByCharacterGravity = true;

		mbIsCharacter = false;
		mbCollideCharacter = true;

		mbIsRagDoll = false;
		mbCollideRagDoll = true;

		mbCollide = true;

		mbVolatile = false;

		mbDisableAfterSimulation = false;

		mbHasCollision = false;

		mbDestroying = false;

		m_mtxPrevScrapeMatrix = cMatrixf::Identity;

		mlCollideFlags = eFlagBit_All;

		mvTotalStaticLinearVel = 0;
		mvTotalStaticAngularVel = 0;
		mbStaticMovement = false;

	}

	//-----------------------------------------------------------------------

	iPhysicsBody::~iPhysicsBody()
	{
	}

	//-----------------------------------------------------------------------


	void iPhysicsBody::Destroy()
	{
		mbDestroying = true;
		
		mpWorld->DestroyShape(mpShape);

		///////////////////////////
		//Remove connected characters
		tCharacterBodyListIt it = mlstConnectedCharBodies.begin();
		for(; it != mlstConnectedCharBodies.end(); ++it)
		{
			iCharacterBody *pCharBody = *it;
			pCharBody->DisconnectBody();
		}

		///////////////////////////
		//Remove from verlet container
		tVerletParticleContainerListIt conIt = mlstAttachedVerletContainers.begin();
		for(; conIt != mlstAttachedVerletContainers.end(); ++conIt)
		{
			iVerletParticleContainer *pCont = *conIt;
			pCont->RemoveAttachedBody(this, false);	
		}
		mlstAttachedVerletContainers.clear();

		///////////////////////////
		//Remove Joints
		for(int i=0; i<(int)mvJoints.size(); i++)
		{
			iPhysicsJoint *pJoint = mvJoints[i];

			pJoint->RemoveBody(this);
			
			if(pJoint->GetParentBody()==NULL && pJoint->GetChildBody()==NULL)
			{
				mpWorld->DestroyJoint(pJoint);
			}

			//Skip removing for now, just makes things messy...
			/*if(	pJoint->GetParentBody() == this ||
				(pJoint->GetParentBody() == NULL && pJoint->GetChildBody()== this) )
			{
				//Log("  Destroy joint %d\n",(size_t)pJoint);
				mpWorld->DestroyJoint(pJoint);
			}
			else if(pJoint->GetParentBody() == this)
			{
				//Remove this body from the joint
				pJoint->RemoveBody(this);
			}*/
		}

		//Log("Deleted body '%s'\n",msName.c_str());

		////////////////////////
		// Remove sounds
		if(mpScrapeSoundEntity && mpWorld->GetWorld()->SoundEntityExists(mpScrapeSoundEntity, mlScrapeSoundEntityID)) 
		{
			mpWorld->GetWorld()->DestroySoundEntity(mpScrapeSoundEntity);
		}
		if(mpRollSoundEntity  && mpWorld->GetWorld()->SoundEntityExists(mpRollSoundEntity,mlRollSoundEntityID)) 
		{
			mpWorld->GetWorld()->DestroySoundEntity(mpRollSoundEntity);
		}

		////////////////////////
		// Lowlevel stuff
		DeleteLowLevel();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVector3f iPhysicsBody::GetVelocityAtPosition(const cVector3f& avPos)
	{
		cVector3f vLocalPos = avPos - GetLocalPosition();
		cVector3f vMassCentre = GetMassCentre();
		if(vMassCentre != cVector3f(0,0,0))
		{
			vMassCentre = cMath::MatrixMul(GetLocalMatrix().GetRotation(),vMassCentre);
			vLocalPos -= vMassCentre;
		}

		return GetLinearVelocity() + cMath::Vector3Cross(GetAngularVelocity(), vLocalPos);
	}

	//-----------------------------------------------------------------------

	cVector3f iPhysicsBody::GetTorqueFromForceAtPosition(const cVector3f& avForce, const cVector3f& avPos)
	{
		cVector3f vLocalPos = avPos - GetLocalPosition();
		cVector3f vMassCentre = GetMassCentre();
		if(vMassCentre != cVector3f(0,0,0))
		{
			vMassCentre = cMath::MatrixMul(GetLocalMatrix().GetRotation(),vMassCentre);
			vLocalPos -= vMassCentre;
		}

		return cMath::Vector3Cross(vLocalPos, avForce);
	}

	//-----------------------------------------------------------------------

	void iPhysicsBody::AddJoint(iPhysicsJoint *apJoint)
	{
		mvJoints.push_back(apJoint);
	}

	iPhysicsJoint* iPhysicsBody::GetJoint(int alIndex)
	{
		return mvJoints[alIndex];
	}

	int iPhysicsBody::GetJointNum()
	{
		return (int)mvJoints.size();
	}

	void iPhysicsBody::RemoveJoint(iPhysicsJoint *apJoint)
	{
		std::vector<iPhysicsJoint*>::iterator it = mvJoints.begin();
		for(; it != mvJoints.end(); ++it)
		{
			if(*it == apJoint){
				mvJoints.erase(it);
				break;
			}
		}
	}
	
	//-----------------------------------------------------------------------

	bool iPhysicsBody::UpdateBeforeSimulate(float afTimeStep)
	{
		/////////////////////
		//See if the body should still be in update list. 
		bool bNeedsUpdate = false;

		int lTemp=0;
		if(HasSlide() || GetScrapeSoundEntity()){ bNeedsUpdate = true; lTemp=0;}
		if(GetMass() > 0) {
			if(GetEnabled()){ bNeedsUpdate = true; lTemp=1;}
		}
		else {
			if(mbStaticMovement){ bNeedsUpdate=true; lTemp=2;}
		}

		/////////////////////
		//Reset variables
		SetHasSlide(false);
		mbHasCollision = false;

		/////////////////////
		//Check if update is needed, else return
		if(bNeedsUpdate==false)
		{
			//Log(" (removed %s from list %d)", lTemp);
			return false;
		}

		//////////////////////////////////
		//Static velocity update
		if(mbStaticMovement)
		{
			if(	mvTotalStaticLinearVel == cVector3f(0) &&
				mvTotalStaticAngularVel == cVector3f(0))
			{
				mbStaticMovement = false;
			}

			SetLinearVelocity(mvTotalStaticLinearVel);
			SetAngularVelocity(mvTotalStaticAngularVel);

			cBoundingVolume tempBV;
			tempBV.SetSize(GetBoundingVolume()->GetSize() * 1.2f);
			tempBV.SetPosition(GetBoundingVolume()->GetWorldCenter());
			mpWorld->EnableBodiesInBV(&tempBV, true);
		}
		
		return true;
	}

	//-----------------------------------------------------------------------
	
	void iPhysicsBody::UpdateAfterSimulate(float afTimeStep)
	{
		//////////////////////////////////
		//Static velocity update
        if(mbStaticMovement)
		{
			cVector3f vPos = GetLocalPosition();
			
			if(mvTotalStaticLinearVel != 0)
			{
				vPos += mvTotalStaticLinearVel * afTimeStep;
			}
			if(mvTotalStaticAngularVel != 0)
			{
				cMatrixf mtxRot = GetLocalMatrix().GetRotation();
				
				mtxRot = cMath::MatrixMul( cMath::MatrixRotate(mvTotalStaticAngularVel * afTimeStep, eEulerRotationOrder_XYZ), mtxRot );

				SetMatrix(mtxRot);
			}
			SetPosition(vPos);

			mvTotalStaticLinearVel = 0;
			mvTotalStaticAngularVel = 0;
		}

		//////////////////////////////////
		//Check disabling from callback
		if(mbDisableAfterSimulation)
		{
			mbDisableAfterSimulation = false;
			SetAngularVelocity(0);
			SetLinearVelocity(0);
		}
		
		//////////////////////////////////
		//Check slide sound
		//Log("Slide: %d\n", HasSlide());
		if(HasSlide()==false)
		{
			if(GetScrapeSoundEntity())
			{
				//Log("Checking stop sound (%d), body (%d): Count: %d \n", GetScrapeSoundEntity(), this, mlSlideCount);
				if(mlSlideCount<=0)
				{
					//Log("Stopped scrape %d on body '%s' IN BODY!\n", (size_t)GetScrapeSoundEntity(),
					//												 GetName().c_str());
					
					if(mpWorld->GetWorld())
					{
						if(mpWorld->GetWorld()->SoundEntityExists(GetScrapeSoundEntity(), mlScrapeSoundEntityID))
						{
							GetScrapeSoundEntity()->FadeOut(5.2f);
						}
					}
					
					SetScrapeSoundEntity(NULL);
					SetScrapeBody(NULL);
				}
				else if(mlSlideCount>0)
				{
					mlSlideCount--;
				}
			}
		}
		else
		{
			mlSlideCount = 8;
		}

		//////////////////////////////////
		//Update rolling sound
		if(GetEnabled() && mpMaterial && mpMaterial->GetSurfaceData())
		{
			mpMaterial->GetSurfaceData()->UpdateRollEffect(this);
		}
	}

	//-----------------------------------------------------------------------

	void iPhysicsBody::StaticLinearMove(const cVector3f& avVelocity)
	{
		if(GetMass()!=0) return;

		mvTotalStaticLinearVel += avVelocity;
		mbStaticMovement = true;

		mpWorld->AddBodyToUpdateList(this);
	}

	void iPhysicsBody::StaticAngularMove(const cVector3f& avVelocity)
	{
		if(GetMass()!=0) return;

		mvTotalStaticAngularVel += avVelocity;
		mbStaticMovement = true;

		mpWorld->AddBodyToUpdateList(this);
	}

	//-----------------------------------------------------------------------

	void iPhysicsBody::AddBodyCallback(iPhysicsBodyCallback *apCallback)
	{
		mlstBodyCallbacks.push_back(apCallback);
	}
	
	void iPhysicsBody::RemoveBodyCallback(iPhysicsBodyCallback *apCallback)
	{
		tPhysicsBodyCallbackListIt it = mlstBodyCallbacks.begin();
		for(; it != mlstBodyCallbacks.end(); ++it)
		{
			if(apCallback == *it)
			{
				mlstBodyCallbacks.erase(it);
				break;
			}
		}
	}

	//-----------------------------------------------------------------------

	void iPhysicsBody::SetScrapeSoundEntity(cSoundEntity *apEntity)
	{
		mpScrapeSoundEntity = apEntity;
		if(apEntity)
			mlScrapeSoundEntityID = apEntity->GetCreationID();
		else
			mlScrapeSoundEntityID = -1;
	}

	//-----------------------------------------------------------------------


	void iPhysicsBody::SetRollSoundEntity(cSoundEntity *apEntity)
	{
		mpRollSoundEntity = apEntity;
		if(apEntity)
			mlRollSoundEntityID = apEntity->GetCreationID();
		else
			mlRollSoundEntityID = -1;
	}

	//-----------------------------------------------------------------------
	
	bool iPhysicsBody::OnAABBCollision(iPhysicsBody *apBody)
	{
		if(mlstBodyCallbacks.empty()) return true;

		bool bReturn = true;

		tPhysicsBodyCallbackListIt it = mlstBodyCallbacks.begin();
		for(; it != mlstBodyCallbacks.end(); ++it)
		{
			iPhysicsBodyCallback *pCallback = *it;

			if(pCallback->OnAABBCollide(this, apBody)==false) bReturn = false;
		}

		return bReturn;
	}
	
	//-----------------------------------------------------------------------

	void iPhysicsBody::OnCollide(iPhysicsBody *apBody, cPhysicsContactData* apContactData)
	{
		mbHasCollision = true;

		if(mlstBodyCallbacks.empty()) return;

		tPhysicsBodyCallbackListIt it = mlstBodyCallbacks.begin();
		for(; it != mlstBodyCallbacks.end(); ++it)
		{
			iPhysicsBodyCallback *pCallback = *it;

			pCallback->OnBodyCollide(this, apBody,apContactData);
		}
	}

	//-----------------------------------------------------------------------

	iPhysicsMaterial* iPhysicsBody::GetMaterial()
	{
		return mpMaterial;
	}
	
	//-----------------------------------------------------------------------
	
	iCollideShape* iPhysicsBody::GetShape()
	{
		return mpShape;
	}

	//-----------------------------------------------------------------------
	
	void iPhysicsBody::AddConnectedCharacter(iCharacterBody *apCharBody)
	{
		if(mbDestroying) return;
		
		mlstConnectedCharBodies.push_back(apCharBody);
	}
	
	void iPhysicsBody::RemoveConnectedCharacter(iCharacterBody *apCharBody)
	{
		if(mbDestroying) return;

		STLFindAndRemove(mlstConnectedCharBodies,apCharBody);
	}

	//-----------------------------------------------------------------------
	
	void iPhysicsBody::AddAttachedVerletContainer(iVerletParticleContainer *apContainer)
	{
		mlstAttachedVerletContainers.push_back(apContainer);
	}

	//-----------------------------------------------------------------------

	
	void iPhysicsBody::RemoveAttachedVerletContainer(iVerletParticleContainer *apContainer)
	{
		tVerletParticleContainerListIt it = mlstAttachedVerletContainers.begin();
		for(; it != mlstAttachedVerletContainers.end(); )
		{
			iVerletParticleContainer *pCont = *it;
            if(pCont == apContainer)
			{
				it = mlstAttachedVerletContainers.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	//-----------------------------------------------------------------------
}
