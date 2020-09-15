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

#ifndef HPL_PHYSICS_MATERIAL_H
#define HPL_PHYSICS_MATERIAL_H

#include "math/MathTypes.h"



namespace hpl {

	class iPhysicsWorld;
	class cSurfaceData;

	//----------------------------------------------------

	//! Enum the different combination modes
	//! This decides how two material properties are combined when colliding.
	//! If the two differ the enum with the highest num.
	enum ePhysicsMaterialCombMode
	{
		//! result = (value1 + value2)/2
		ePhysicsMaterialCombMode_Average=	0,
		//! result = min(value1, value2)
		ePhysicsMaterialCombMode_Min=		1,
		//! result = value1 * value2
		ePhysicsMaterialCombMode_Multiply=	2,
		//! result = max(value1, value2)
		ePhysicsMaterialCombMode_Max=		3,
		//! Internal.
		ePhysicsMaterialCombMode_LastEnum
	};
	
	//----------------------------------------------------

	class cPhysicsContactData
	{
	public:
		cPhysicsContactData() {
			mfMaxContactNormalSpeed =0;
			mfMaxContactTangentSpeed =0;
			mvContactNormal = cVector3f(0,0,0);
			mvContactPosition = cVector3f(0,0,0);
			mvForce = cVector3f(0,0,0);
		}

		float mfMaxContactNormalSpeed;
		float mfMaxContactTangentSpeed;
		cVector3f mvContactNormal;
		cVector3f mvContactPosition;
		cVector3f mvForce;
	};


	//----------------------------------------------------

	
	class iPhysicsMaterial
	{
	public:
		iPhysicsMaterial(const tString &asName, iPhysicsWorld *apWorld) 
				: msName(asName), mpWorld(apWorld),mpSurfaceData(NULL),
					mbPreloaded(false){}
		virtual ~iPhysicsMaterial(){}

		const tString& GetName() const{ return msName;}

		virtual void SetElasticity(float afElasticity)=0;
		virtual float GetElasticity() const=0;
		virtual void SetStaticFriction(float afElasticity)=0;
		virtual float GetStaticFriction() const=0;
		virtual void SetKineticFriction(float afElasticity)=0;
		virtual float GetKineticFriction() const=0;
		
		virtual void SetFrictionCombMode(ePhysicsMaterialCombMode aMode)=0;
		virtual ePhysicsMaterialCombMode GetFrictionCombMode() const=0;
		virtual void SetElasticityCombMode(ePhysicsMaterialCombMode aMode)=0;
		virtual ePhysicsMaterialCombMode GetElasticityCombMode() const=0;

		void SetSurfaceData(cSurfaceData *apData){ mpSurfaceData = apData;}
		cSurfaceData* GetSurfaceData(){ return mpSurfaceData;}

		void SetPreloaded(bool abX){ mbPreloaded = abX;}
		bool IsPreloaded(){ return mbPreloaded;}

	protected:
		iPhysicsWorld *mpWorld;
		tString msName;

		bool mbPreloaded;

		cSurfaceData *mpSurfaceData;
	};
};
#endif // HPL_PHYSICS_MATERIAL_H
