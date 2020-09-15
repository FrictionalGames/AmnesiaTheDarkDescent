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

#ifndef HPL_PHYSICS_BODY_H
#define HPL_PHYSICS_BODY_H

#include "scene/Entity3D.h"
#include "graphics/GraphicsTypes.h"
#include "physics/PhysicsTypes.h"

namespace hpl {

	class iPhysicsWorld;
	class iCollideShape;
	class iPhysicsMaterial;
	class iLowLevelGraphics;
	class cNode3D;
	class cSoundEntity;
	class iPhysicsJoint;
	class cPhysicsContactData;
	class iCharacterBody;
	class iHapticShape;
	class iVerletParticleContainer;

	//------------------------------------------

	class iPhysicsBody;
	class iPhysicsBodyCallback
	{
	public:
		virtual bool OnAABBCollide(iPhysicsBody *apBody, iPhysicsBody *apCollideBody)=0;
		virtual void OnBodyCollide(iPhysicsBody *apBody, iPhysicsBody *apCollideBody,cPhysicsContactData* apContactData)=0;
	};

	typedef std::list<iPhysicsBodyCallback*> tPhysicsBodyCallbackList;
	typedef tPhysicsBodyCallbackList::iterator tPhysicsBodyCallbackListIt;

	//------------------------------------------

	struct cPhysicsBody_Buoyancy
	{
		cPhysicsBody_Buoyancy() : mbActive(false), mfDensity(1), 
								mfLinearViscosity(1), mfAngularViscosity(1){}

		bool mbActive;

		float mfDensity;
		float mfLinearViscosity;
		float mfAngularViscosity;

		cPlanef mSurface;
	};

	//------------------------------------------

	class iPhysicsBody : public iEntity3D
	{
	#ifdef __GNUC__
		typedef iEntity3D __super;
	#endif
	public:
		iPhysicsBody(const tString &asName, iPhysicsWorld *apWorld,iCollideShape *apShape);
		virtual ~iPhysicsBody();

		void Destroy();

		virtual void SetMaterial(iPhysicsMaterial* apMaterial)=0;
		iPhysicsMaterial* GetMaterial();

		iCollideShape* GetShape();

		void AddJoint(iPhysicsJoint *apJoint);
		iPhysicsJoint* GetJoint(int alIndex);
		int GetJointNum();
		void RemoveJoint(iPhysicsJoint *apJoint);

		virtual void SetLinearVelocity(const cVector3f &avVel)=0;
		virtual cVector3f GetLinearVelocity() const=0;
		virtual void SetAngularVelocity(const cVector3f &avVel)=0;
		virtual cVector3f GetAngularVelocity() const=0;
		virtual void SetLinearDamping(float afDamping)=0;
		virtual float GetLinearDamping() const=0;
		virtual void SetAngularDamping(float afDamping)=0;
		virtual float GetAngularDamping() const=0;
		virtual void SetMaxLinearSpeed(float afSpeed)=0;
		virtual float GetMaxLinearSpeed() const=0;
		virtual void SetMaxAngularSpeed(float afDamping)=0;
		virtual float GetMaxAngularSpeed() const=0;
		virtual cVector3f GetInertiaVector() =0;
		virtual cMatrixf GetInertiaMatrix() =0;

		cVector3f GetVelocityAtPosition(const cVector3f& avPos);
		cVector3f GetTorqueFromForceAtPosition(const cVector3f& avForce, const cVector3f& avPos);

		virtual void  SetMass(float afMass)=0;
		virtual float GetMass() const=0;
		virtual void  SetMassCentre(const cVector3f& avCentre)=0;
		virtual cVector3f GetMassCentre() const=0;

		virtual void AddForce(const cVector3f &avForce)=0;
		virtual void AddForceAtPosition(const cVector3f &avForce, const cVector3f &avPos)=0;
		virtual void AddTorque(const cVector3f &avTorque)=0;
		virtual void AddImpulse(const cVector3f &avImpulse)=0;
		virtual void AddImpulseAtPosition(const cVector3f &avImpulse, const cVector3f &avPos)=0;

		virtual void Enable()=0;
		virtual bool GetEnabled() const=0;
		virtual void SetAutoDisable(bool abEnabled)=0;
		virtual bool GetAutoDisable() const=0;
		virtual void SetAutoDisableLinearThreshold(float afThresold)=0;
		virtual float GetAutoDisableLinearThreshold() const=0;
		virtual void SetAutoDisableAngularThreshold(float afThresold)=0;
		virtual float GetAutoDisableAngularThreshold() const=0;
		virtual void SetAutoDisableNumSteps(int alNum)=0;
		virtual int GetAutoDisableNumSteps() const=0;
		virtual void SetContinuousCollision(bool abOn)=0;
		virtual bool GetContinuousCollision()=0;

		virtual void SetGravity(bool abEnabled)=0;
		virtual bool GetGravity() const=0;

		virtual void RenderDebugGeometry(iLowLevelGraphics *apLowLevel,const cColor &aColor)=0;

		bool UpdateBeforeSimulate(float afTimeStep);
		void UpdateAfterSimulate(float afTimeStep);

		void StaticLinearMove(const cVector3f& avVelocity);
		void StaticAngularMove(const cVector3f& avVelocity);

		void SetBlocksSound(bool abX){ mbBlocksSound = abX;}
		bool GetBlocksSound(){ return mbBlocksSound;}

		void SetBlocksLight(bool abX){ mbBlocksLight = abX;}
		bool GetBlocksLight(){ return mbBlocksLight;}

		/**
		 * Must be true with both bodies in a collision if any effect is to be played.
		 */
		void SetUseSurfaceEffects(bool abX){ mbUseSurfaceEffects = abX;}
		bool GetUseSurfaceEffects(){ return mbUseSurfaceEffects;}
		

		void SetScrapeSoundEntity(cSoundEntity *apEntity);
		cSoundEntity* GetScrapeSoundEntity(){ return mpScrapeSoundEntity;}
		void SetScrapeSoundEntityID(int alID){ mlScrapeSoundEntityID = alID;}
		int GetScrapeSoundEntityID(){ return mlScrapeSoundEntityID;}
		void SetScrapeBody(iPhysicsBody *apBody){ mpScrapeBody =apBody;}
		iPhysicsBody * GetScrapeBody(){ return mpScrapeBody;}
		const cMatrixf& GetPreveScrapeMatrix(){ return m_mtxPrevScrapeMatrix;}
		void SetPreveScrapeMatrix(const cMatrixf& a_mtxMtx){ m_mtxPrevScrapeMatrix = a_mtxMtx;}

		void SetRollSoundEntity(cSoundEntity *apEntity);
		cSoundEntity* GetRollSoundEntity(){ return mpRollSoundEntity;}
		int GetRollSoundEntityID(){ return mlRollSoundEntityID;}

		void SetHasSlide(bool abX){ mbHasSlide = abX;}
		bool HasSlide(){ return mbHasSlide;}

		bool HasCollision(){ return mbHasCollision;}

		void SetUserData(void* apUserData){ mpUserData = apUserData;}
		void* GetUserData(){ return mpUserData;}

		void AddBodyCallback(iPhysicsBodyCallback *apCallback);
		void RemoveBodyCallback(iPhysicsBodyCallback *apCallback);

		bool OnAABBCollision(iPhysicsBody *apBody);
		void OnCollide(iPhysicsBody *apBody,cPhysicsContactData* apContactData);

		void SetCollide(bool abX){mbCollide = abX;}
		bool GetCollide(){ return mbCollide;}

		void SetIsCharacter(bool abX){ mbIsCharacter = abX;}
		bool IsCharacter(){ return mbIsCharacter;}

		void SetCollideCharacter(bool abX){ mbCollideCharacter = abX;}
		bool GetCollideCharacter(){ return mbCollideCharacter;}

		void SetCharacterBody(iCharacterBody *apCharBody){ mpCharacterBody = apCharBody;}
		iCharacterBody* GetCharacterBody(){ return mpCharacterBody;}

		void SetPushStrength(int alX){ mlPushStrength = alX;}
		int GetPushStrength(){ return mlPushStrength;}

		void SetCollideFlags(tFlag alX) { mlCollideFlags = alX;}
		inline tFlag GetCollideFlags() const { return mlCollideFlags;}
		
		void SetIsRagDoll(bool abX){ mbIsRagDoll = abX;}
		bool IsRagDoll(){ return mbIsRagDoll;}

		void SetCollideRagDoll(bool abX){ mbCollideRagDoll = abX;}
		bool GetCollideRagDoll(){ return mbCollideRagDoll;}

		void SetVolatile(bool abX){ mbVolatile = abX;}
		bool IsVolatile(){ return mbVolatile;}

		void SetPushedByCharacterGravity(bool abX){ mbPushedByCharacterGravity = abX;}
		bool GetPushedByCharacterGravity(){ return mbPushedByCharacterGravity;}

		void SetBuoyancyActive(bool abX){ mBuoyancy.mbActive = abX;}
		void SetBuoyancyDensity(float afX){ mBuoyancy.mfDensity = afX;}
		void SetBuoyancyLinearViscosity(float afX){ mBuoyancy.mfLinearViscosity = afX;}
		void SetBuoyancyAngularViscosity(float afX){ mBuoyancy.mfAngularViscosity = afX;}
		void SetBuoyancySurface(const cPlanef &aP){ mBuoyancy.mSurface = aP;}

		bool GetBuoyancyActive(){ return mBuoyancy.mbActive;}
		float GetBuoyancyDensity(){ return mBuoyancy.mfDensity;}
		float GetBuoyancyLinearViscosity(){ return mBuoyancy.mfLinearViscosity;}
		float GetBuoyancyAngularViscosity(){ return mBuoyancy.mfAngularViscosity;}
		cPlanef SetBuoyancySurface(){ return mBuoyancy.mSurface;}

		float GetBuoyancyDensityMul(){ return mfBuoyancyDensityMul;}
		void SetBuoyancyDensityMul(float afX){ mfBuoyancyDensityMul = afX;}

		void SetGravityCanAttachCharacter(bool abX){ mbGravityCanAttachCharacter = abX;}
		void SetGravityAttachmentRotation(bool abX){ mbGravityAttachmentRotation = abX;}
		void SetGravityAttachmentVelocity(bool abX){ mbGravityAttachmentVelocity = abX;}
		void SetGravityAttachmentVelocityAxes(eVelocityAxes aAxes){ mGravityAttachmentVelocityAxes = aAxes;}

		bool GetGravityCanAttachCharacter(){ return mbGravityCanAttachCharacter;}
		bool GetGravityAttachmentRotation(){ return mbGravityAttachmentRotation;}
		bool GetGravityAttachmentVelocity(){ return mbGravityAttachmentVelocity;}
		eVelocityAxes GetGravityAttachmentVelocityAxes(){ return mGravityAttachmentVelocityAxes;}
		
		void AddConnectedCharacter(iCharacterBody *apCharBody);
		void RemoveConnectedCharacter(iCharacterBody *apCharBody);

		bool IsInUpdateList(){ return mbInUpdateList;}
		void SetInUpdateList(bool abX){ mbInUpdateList = abX;}

		void AddAttachedVerletContainer(iVerletParticleContainer *apContainer);
		void RemoveAttachedVerletContainer(iVerletParticleContainer *apContainer);
		
		iHapticShape *GetHapticShape(){ return mpHapticShape;}
		void SetHapticShape(iHapticShape *apShape){ mpHapticShape = apShape;}

		iPhysicsWorld *GetWorld(){ return mpWorld;}

		void DisableAfterSimulation(){ mbDisableAfterSimulation = true;}

		//Entity implementation
		tString GetEntityType(){ return "Body";}

		virtual void DeleteLowLevel()=0;
	protected:
        iPhysicsWorld *mpWorld;
		iCollideShape *mpShape;
		iPhysicsMaterial *mpMaterial;
		
		iCharacterBody *mpCharacterBody;

		std::vector<iPhysicsJoint*> mvJoints;

		bool mbDestroying;
        
		std::list<iCharacterBody*> mlstConnectedCharBodies;
		        		
		iPhysicsBody *mpScrapeBody;
		cSoundEntity *mpScrapeSoundEntity;
		int mlScrapeSoundEntityID;
		cSoundEntity *mpRollSoundEntity;
		int mlRollSoundEntityID;
		cMatrixf m_mtxPrevScrapeMatrix;
		bool mbHasSlide;
		int mlSlideCount;
		int mlImpactCount;
		bool mbInUpdateList;

		float mfBuoyancyDensityMul;

		bool mbStaticMovement;
		cVector3f mvTotalStaticLinearVel;
		cVector3f mvTotalStaticAngularVel;

		tFlag mlCollideFlags;

		bool mbPushedByCharacterGravity;

		bool mbBlocksSound;
		bool mbBlocksLight;
		bool mbIsCharacter;
		bool mbCollideCharacter;
		bool mbIsRagDoll;
		bool mbCollideRagDoll;
		bool mbVolatile;
		bool mbUseSurfaceEffects;

		bool mbGravityCanAttachCharacter;
		bool mbGravityAttachmentRotation;
		bool mbGravityAttachmentVelocity;
		eVelocityAxes mGravityAttachmentVelocityAxes;

		int mlPushStrength;

		cPhysicsBody_Buoyancy mBuoyancy;

		bool mbDisableAfterSimulation;

		bool mbHasCollision;

		tPhysicsBodyCallbackList mlstBodyCallbacks;

		iHapticShape *mpHapticShape;

		void *mpUserData;

		tVerletParticleContainerList mlstAttachedVerletContainers;

		bool mbCollide;
	};
};
#endif // HPL_PHYSICS_BODY_H
