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

#ifndef HPL_LOW_LEVEL_HAPTIC_H
#define HPL_LOW_LEVEL_HAPTIC_H

#include "math/MathTypes.h"
#include "haptic/HapticTypes.h"

namespace hpl {

	class iHapticShape;
	class iHapticForce;
	class iHapticSurface;

	class iVertexBuffer;
	class cCamera;
	class cResources;
	class iPhysicsBody;

	class iLowLevelHaptic
	{
	public:
		iLowLevelHaptic();
		virtual ~iLowLevelHaptic();

		//########################################################################################
		//! \name General
		//########################################################################################
		//! @{

		void Update(float afTimeStep);
		bool Init(cResources *apResources);
		
		void DestroyAllShapes();
		void DestroyAll();

		virtual float GetMaxForce()=0;

		virtual void SetWorldScale(float afScale)=0;
		virtual float GetWorldScale()=0;

		virtual void SetRenderingActive(bool abX)=0;
		virtual bool IsRenderingActive()=0;

		virtual bool ShapeIsInContact(iHapticShape *apShape)=0;

		void SetCamera(cCamera *apCamera){ mpCamera = apCamera;}
		cCamera* GetCamera(){ return mpCamera;}
		void SetCameraOffset(const cVector3f &avOffset){ mvCameraOffset = avOffset;}
		cVector3f GetCameraOffset(){ return mvCameraOffset; }

		void SetUpdateShapes(bool abX){ mbUpdateShapes = abX;}
		bool GetUpdateShapes(){ return mbUpdateShapes;}

		void SetForceMul(float afX) {mfForceMul = afX;}
		float GetForceMul() {return mfForceMul;}

		//! @}

		//########################################################################################
		//! \name Input
		//########################################################################################
		//! @{
		
		virtual bool ButtonIsPressed(int alNum)=0;

		virtual cVector3f GetHardwarePosition()=0;
		virtual cVector3f GetHardwareRotation()=0;

		cVector3f GetHardwarePosDelta();
		
		cVector2f GetProxyScreenPos(const cVector2f& avScreenSize);
		cVector2f GetProxyScreenDeltaPos(const cVector2f& avScreenSize);

		cVector2f GetRelativeVirtualMousePos();
		cVector2f GetVirtualMousePos();
		void SetVirtualMousePosBounds(	const cVector2f &avMin,const cVector2f &avMax,
											const cVector2f &avScreenSize);
		
		virtual cVector3f GetProxyPosition()=0;
		virtual void SetProxyRadius(float afRadius)=0;
		virtual float GetProxyRadius()=0;

		virtual int GetNumberOfButtons()=0;

		//! @}

		//########################################################################################
		//! \name Surfaces
		//########################################################################################
		//! @{

		virtual iHapticSurface* CreateSimpleSurface(const tString &asName,
													float afDamping=0.0f, float afStiffness=0.9f)=0;
		
		virtual iHapticSurface* CreateFrictionalSurface(const tString &asName,
														float afDamping=0.0f, float afStiffness=0.9f,
														float afDynamicFriction=0.2f, float afStartingFriction=0.3f,
														float afTangentStiffness=0.7f, float afStoppingFriction=0.1f)=0;
		
		virtual iHapticSurface* CreateRoughSurface(const tString &asName, 
													float afDeviation=0.1f, float afMean=0.5f,
													float afDamping=0.0f, float afStiffness=0.9f,
													float afDynamicFriction=0.2f, float afStartingFriction=0.3f,
													float afTangentStiffness=0.7f, float afStoppingFriction=0.1f)=0;
		
		virtual iHapticSurface* CreateStickySurface(const tString &asName, 
													float afDeadHeight=0.004f, float afStickyStiffness=0.6f,
													float afDamping=0.0f, float afStiffness=0.9f,
													float afDynamicFriction=0.2f, float afStartingFriction=0.3f,
													float afTangentStiffness=0.7f, float afStoppingFriction=0.1f)=0;
		
		iHapticSurface* GetSurfaceFromName(const tString &asName);

		

		//! @}

		//########################################################################################
		//! \name Shapes
		//########################################################################################
		//! @{

		virtual iHapticShape* CreateBoxShape(const tString &asName, const cVector3f &avSize, cMatrixf* apOffsetMtx)=0;
		virtual iHapticShape* CreateSphereShape(const tString &asName, const cVector3f &avRadii, cMatrixf* apOffsetMtx)=0;
		virtual iHapticShape* CreateCylinderShape(const tString &asName, float afRadius, float afHeight, cMatrixf* apOffsetMtx)=0;
		virtual iHapticShape* CreateCapsuleShape(const tString &asName, float afRadius, float afHeight, cMatrixf* apOffsetMtx)=0;
		virtual iHapticShape* CreateMeshShape(const tString &asName, iVertexBuffer *apVtxBuffer)=0;
		virtual iHapticShape* CreateCompundShape(const tString &asName, tHapticShapeVec &avShapes)=0;

		virtual iHapticShape* CreateShapeFromPhysicsBody(const tString &asName, iPhysicsBody *apBody)=0;

		void DestroyShape(iHapticShape* apShape);
		bool ShapeExists(iHapticShape* apShape);

		cHapticShapeIterator GetShapeIterator();

		//! @}

		//########################################################################################
		//! \name Forces
		//########################################################################################
		//! @{

		virtual iHapticForce* CreateImpulseForce(const cVector3f& avForce)=0;
		virtual iHapticForce* CreateSinusWaveForce(const cVector3f& avDirection, float afAmp, float afFreq)=0;
		virtual iHapticForce* CreateSawWaveForce(const cVector3f& avDirection, float afAmp, float afFreq)=0;
		virtual iHapticForce* CreateSpringForce(const cVector3f& avPostition, float afStiffness, float afDamping)=0;
		virtual iHapticForce* CreateViscosityForce(const cVector3f& avVelocity,float afMass, float afStiffness, float afDamping)=0;

		void DestroyForce(iHapticForce* apForce);

		void StopAllForces();

		//! @}

	protected:
		virtual bool InitLowLevel()=0;
		virtual void UpdateLowLevel(float afTimeStep)=0;

		cResources *mpResources;
	
		cCamera *mpCamera;
		cVector3f mvCameraOffset;

		tHapticSurfaceMap m_mapSurfaces;
		tHapticForceList mlstForces;
		tHapticShapeList mlstShapes;

		float mfForceMul;

		cVector3f mvPreviousHardwarePos;
		cVector3f mvCurrentHardwarePos;

		cVector2f mvPreviousScreenPos;
		cVector2f mvCurrentScreenPos;
		bool mbScreenPosFirstTime;

		cVector2f mvMinMousePos;
		cVector2f mvMaxMousePos;
		cVector2f mvScreenSize;

		bool mbUpdateShapes;
	};

};
#endif // HPL_LOW_LEVEL_HAPTIC_H
