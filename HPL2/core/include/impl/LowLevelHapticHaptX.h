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

#ifndef HPL_LOW_LEVEL_HAPTIC_HAPTX_H
#define HPL_LOW_LEVEL_HAPTIC_HAPTX_H

#include "haptic/LowLevelHaptic.h"
#include <HaptX.h>

#include <string>
#include <vector>

namespace hpl {
	
	class cMesh;

	//------------------------------------------

	enum eHaptXShapeModelType
	{
		eHaptXShapeModelType_Box,
		eHaptXShapeModelType_Sphere,
		eHaptXShapeModelType_Capsule,
		eHaptXShapeModelType_Cylinder,
		eHaptXShapeModelType_LastEnum
	};
	
	class cHaptXShapeModel
	{
	public:
		
		std::vector<int> mvIndices;
		std::vector<cVector3f> mvPositions;
	};

	//------------------------------------------

	class cLowLevelHapticHaptX : public iLowLevelHaptic
	{
	public:
		cLowLevelHapticHaptX();
		~cLowLevelHapticHaptX();

		//---------------------------------------------------------

		bool InitLowLevel();
		void UpdateLowLevel(float afTimeStep);

		float GetMaxForce();

		void SetWorldScale(float afScale);
		float GetWorldScale();

		void SetRenderingActive(bool abX);
		bool IsRenderingActive();

		bool ShapeIsInContact(iHapticShape *apShape);
		
		//---------------------------------------------------------

		bool ButtonIsPressed(int alNum);

		cVector3f GetHardwarePosition();
		cVector3f GetHardwareVelocity();

		cVector3f GetHardwareRotation();

		cVector3f GetProxyPosition();
		void SetProxyRadius(float afRadius);
		float GetProxyRadius();

		int GetNumberOfButtons();

		//---------------------------------------------------------

		iHapticSurface* CreateSimpleSurface(const tString &asName,
			float afDamping=0.0f, float afStiffness=0.9f);

		iHapticSurface* CreateFrictionalSurface(const tString &asName,
			float afDamping=0.0f, float afStiffness=0.9f,
			float afDynamicFriction=0.2f, float afStartingFriction=0.3f,
			float afTangentStiffness=0.7f, float afStoppingFriction=0.1f);

		iHapticSurface* CreateRoughSurface(const tString &asName, 
			float afDeviation=0.1f, float afMean=0.5f,
			float afDamping=0.0f, float afStiffness=0.9f,
			float afDynamicFriction=0.2f, float afStartingFriction=0.3f,
			float afTangentStiffness=0.7f, float afStoppingFriction=0.1f);

		iHapticSurface* CreateStickySurface(const tString &asName, 
			float afDeadHeight=0.004f, float afStickyStiffness=0.6f,
			float afDamping=0.0f, float afStiffness=0.9f,
			float afDynamicFriction=0.2f, float afStartingFriction=0.3f,
			float afTangenSttiffness=0.7f, float afStoppingFriction=0.1f);

		//---------------------------------------------------------

		iHapticShape* CreateBoxShape(const tString &asName, const cVector3f &avSize, cMatrixf* apOffsetMtx);
		iHapticShape* CreateSphereShape(const tString &asName, const cVector3f &avRadii, cMatrixf* apOffsetMtx);
		iHapticShape* CreateCylinderShape(const tString &asName, float afRadius, float afHeight, cMatrixf* apOffsetMtx);
		iHapticShape* CreateCapsuleShape(const tString &asName, float afRadius, float afHeight, cMatrixf* apOffsetMtx);
		iHapticShape* CreateMeshShape(const tString &asName, iVertexBuffer *apVtxBuffer);
		iHapticShape* CreateCompundShape(const tString &asName, tHapticShapeVec &avShapes);
		
		iHapticShape* CreateShapeFromPhysicsBody(const tString &asName, iPhysicsBody *apBody);

		//---------------------------------------------------------

		iHapticForce* CreateImpulseForce(const cVector3f& avForce);
		iHapticForce* CreateSinusWaveForce(const cVector3f& avDirection, float afAmp, float afFreq);
		iHapticForce* CreateSawWaveForce(const cVector3f& avDirection, float afAmp, float afFreq);
		iHapticForce* CreateSpringForce(const cVector3f& avPostition, float afStiffness, float afDamping);
		iHapticForce* CreateViscosityForce(const cVector3f& avVelocity,float afMass, float afStiffness, float afDamping);

		//---------------------------------------------------------
		
		//Haptx Specific
		HaptX::HaptXInterface* GetInterface(){ return mpInterface;}
		HaptX::iHapticDeviceID GetDeviceID(){ return mDeviceID;}

		HaptX::SurfaceInfo * GetDefaultSurface(){ return mpDefaultSurfaceInfo;}
	
	private:
		void cLowLevelHapticHaptX::ShapeModelToHaptXData(	eHaptXShapeModelType aType,
															const cVector3f& avSize,
															const cMatrixf& a_mtxTransform,
															int *apIndices,int alIndexAdd,
															HaptX::Vectors::Vec3f *apPostions);
		
		cHaptXShapeModel* CreateShapeModelFromMesh(cMesh *apMesh);
		void CreateShapeModels();


		HaptX::HaptXInterface* mpInterface;
		HaptX::iHapticDeviceID	mDeviceID;

		std::vector<cHaptXShapeModel*> mvShapeModels;

		HaptX::SurfaceInfo *mpDefaultSurfaceInfo;

		bool mbRenderingActive;

		static int HapticDeviceEnumFunc(std::wstring* asDeviceName, std::wstring* asDeviceModel,
										void *apVendorData);
		static void HaptXMsgCallback(int alSeverity, std::wstring asMessage);
	};

};
#endif // HPL_LOW_LEVEL_HAPTIC_H
