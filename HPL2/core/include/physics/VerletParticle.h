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

#ifndef HPL_VERLET_PARTICLE_H
#define HPL_VERLET_PARTICLE_H

#include "physics/PhysicsTypes.h"
#include "system/SystemTypes.h"
#include "math/MathTypes.h"

namespace hpl {

	//------------------------------------------

	class iPhysicsBody;
	class iPhysicsWorld;

	class cVerletParticle;
	class iVerletParticleContainer;

	//------------------------------------------

	typedef std::list<cVerletParticle*> tVerletParticleList;
	typedef tVerletParticleList::iterator tVerletParticleListIt;

	typedef cSTLIterator<cVerletParticle*, tVerletParticleList, tVerletParticleListIt> cVerletParticleIterator;

	//------------------------------------------

	class cVerletParticleRayCallback : public iPhysicsRayCallback
	{
		friend class iVerletParticleContainer;
	public:
		cVerletParticleRayCallback(iVerletParticleContainer *apContainer);

		void Reset();

		bool BeforeIntersect(iPhysicsBody *pBody);
		bool OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams);

	private:
		iVerletParticleContainer *mpContainer;

		bool mbIntersected;
		cVector3f mvIntersectPos;
		cVector3f mvIntersectNormal;
		float mfClosestDist;
	};
	
	//------------------------------------------

	class cVerletParticle
	{
		friend class iVerletParticleContainer;
	public:
		cVerletParticle(iVerletParticleContainer *apRope);
		cVerletParticle(iVerletParticleContainer *apRope, const cVector3f& avPos, float afInvMass);
		~cVerletParticle();

		void UpdateMovement(float afTimeStep);

		void SetPosition(const cVector3f& avPos, bool abSetPrevPos);
		void AddPosition(const cVector3f& avAdd, bool abSetPrevPos){ SetPosition(mvPosition+avAdd, abSetPrevPos); }
		inline const cVector3f& GetPosition() const { return mvPosition; }
		
		inline void SetPrevPosition(const cVector3f& avPos) { mvPrevPosition = avPos; }
		inline const cVector3f& GetPrevPosition() const { return mvPrevPosition; }
		
		inline void SetSmoothPosition(const cVector3f& avPos) { mvSmoothPosition = avPos;}
		inline const cVector3f& GetSmoothPosition() const { return mvSmoothPosition;}

		void SetInvMass(float afInvMass){ mfInvMass = afInvMass; }
		inline float GetInvMass() const { return mfInvMass; }

	private:
		iVerletParticleContainer *mpContainer;

		cVector3f mvPosition;
		cVector3f mvPrevPosition;
		cVector3f mvSmoothPosition;
		float mfInvMass;
	};

	//------------------------------------------
	
	class iVerletParticleContainer
	{
		friend class cVerletParticleRayCallback;
		friend class cVerletParticle;
	public:
		iVerletParticleContainer(const tString &asName, iPhysicsWorld *apWorld);
		virtual ~iVerletParticleContainer();

		const tString& GetName(){return msName;}

		void SetUniqueID(int alX){ mlUniqueID = alX;}
		int GetUniqueID(){ return mlUniqueID;}

		virtual void RemoveAttachedBody(iPhysicsBody *apBody, bool abRemoveContainerFromBody=true)=0;

		void SetGravityForce(const cVector3f& avX){ mvGravityForce = avX;}
		const cVector3f& GetGravityForce(){ return mvGravityForce;}

		void SetDamping(float afX);
		float GetDamping(){ return mfDamping;}
		
		void SetParticleRadius(float afX);
		float GetParticleRadius(){ return mfParticleRadius;}

		void SetSlideAmount(float afX){ mfSlideAmount = afX; }
		float GetSlideAmount(){ return mfSlideAmount;}

		void SetCollide(bool abX){ mbCollide  = abX;}
		bool GetCollide(){ return mbCollide;}

		void SetSleeping(bool abX);

		inline int GetUpdateCount() const { return mlUpdateCount; }

		cVerletParticleIterator GetParticleIterator(){ return cVerletParticleIterator(&mlstParticles); }
		
	protected:
		virtual bool CheckParticleBodyCollision(iPhysicsBody *apBody)=0;
		virtual bool CheckSpecificDataSleeping()=0;	//Tested when container is NOT sleeping
		virtual bool CheckSpecificDataAwake()=0;		//Tested when container IS sleeping
		virtual void SetSpecificDataSleeping(bool abSleeping)=0;
		
		void PreUpdate(float afTimeStep);

		void UpdateLengthConstraint(cVerletParticle *apP1, cVerletParticle *apP2, float afLength);
		void UpdateParticleCollisionConstraint(cVerletParticle *apPart, const cVector3f &avPrevPos, float afRadius);

		tString msName;
		iPhysicsWorld *mpWorld;

		int mlUniqueID;

		cVerletParticleRayCallback *mpRayParticleCallback;
		
		tVerletParticleList mlstParticles;

		bool mbCollide;

		bool mbSleeping;
		float mfSleepCheckCount;
		float mfSleepCheckTime;
		float mfSleepCheckSqrLimit;
		int mlSleepCount;
		int mlSleepMaxCount;
                
		cVector3f mvGravityForce;
		float mfDamping;
		float mfDampingMul;
		
		float mfParticleRadius;

		float mfSlideAmount;

		int mlUpdateCount;
	};


	//------------------------------------------


};
#endif // HPL_VERLET_PARTICLE_H
