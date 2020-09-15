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

#ifndef HPL_CAMERA_H
#define HPL_CAMERA_H

#include "math/MathTypes.h"
#include "math/Frustum.h"
#include "scene/Node3D.h"

namespace hpl {
	
	//--------------------------------------------

	enum eCameraMoveMode
	{
		eCameraMoveMode_Walk,
		eCameraMoveMode_Fly,
		eCameraMoveMode_LastEnum
	};
	
	enum eCameraRotateMode
	{
		eCameraRotateMode_EulerAngles,
		eCameraRotateMode_Matrix,
		eCameraRotateMode_LastEnum
	};

	//--------------------------------------------

	class iLowLevelGraphics;
	class iEntity3D;
	
	class cCamera
	{
	public:
		cCamera();
		~cCamera();

		const cVector3f& GetPosition()const{return mvPosition;}
		void SetPosition(const cVector3f& avPos);

		/**
		 * Move forward (or back) according to the move mode.
		 * \param afDist 
		 */
		void MoveForward(float afDist);
		/**
		* Move right (or left) according to the move mode.
		* \param afDist 
		*/
		void MoveRight(float afDist);
		/**
		* Move up (or down) according to the move mode.
		* \param afDist 
		*/
		void MoveUp(float afDist);

		void SetFOV(float afAngle);
		float GetFOV() { return mfFOV;}

		void SetAspect(float afSpect);
		float GetAspect() { return mfAspect;}

		void SetFarClipPlane(float afX);
		float GetFarClipPlane() { return mfFarClipPlane;}
		
		void SetNearClipPlane(float afX);
		float GetNearClipPlane() { return mfNearClipPlane;}

		void SetProjectionType(eProjectionType aType);
		eProjectionType GetProjectionType() { return mProjectionType; }

		void SetOrthoViewSize(const cVector2f &avSize);
		const cVector2f& GetOrthoViewSize(){ return mvViewSize;}
		
		/**
		 * This sets the far plane so that no far clipping is made.
		 * The FarClipPlane is still used for creating bounding box and frustum and 
		 * should be set to some value.
		 */
		void SetInifintiveFarPlane(bool abX){ mbInfFarPlane = abX; mbProjectionUpdated = true;}
		bool GetInifintiveFarPlane(){ return mbInfFarPlane;}

		cFrustum* GetFrustum();

		eCameraRotateMode GetRotateMode(){return mRotateMode;}
		
		eCameraMoveMode GetMoveMode(){return mMoveMode;}
		/**
		* Set the mode to calculate the rotation angles.
		* EulerAngles: Yaw, Pitch and Roll are used.
		* Matrix: The matrix is changed directly.
		*/
		void SetRotateMode(eCameraRotateMode aMode);
		/**
		* Set the mode to calculate movement.
		* Walk: only moving in the XZ plane
		* Fly: moving in the dir the camera is facing.
		*/
		void SetMoveMode(eCameraMoveMode aMode);

		/**
		 * Resets all rotation
		 */
		void ResetRotation();
		
		/**
		 * Unproject the screen coordinate to a world space position and direction
		*/
		void UnProject(cVector3f *apPosition, cVector3f *apDirection, const cVector2f& avScreenPos, const cVector2f& avVirtualScreenSize=1);
		void UnProjectHelper(cVector3f* apPosition, cVector3f* apDirection, const cVector2f& avScreenPos, const cVector2f& avVirtualScreenSize,
							 const cMatrixf& amtxCameraRotation,
							 const cVector3f& avCameraPos, const cVector3f& avCameraFwd, const cVector3f& avCameraUp, const cVector3f& avCameraRgt);

		void AttachEntity(iEntity3D *apEntity);
		void RemoveEntity(iEntity3D *apEntity);
		cNode3D* GetAttachmentNode(){ return &mNode;}
		void ClearAttachedEntities();

		//////////////////////////////////////////////////
		////////// EULER ANGLES ROTATION /////////////////
		//////////////////////////////////////////////////
		
		void SetPitch(float afAngle);
		void SetYaw(float afAngle);
		void SetRoll(float afAngle);
		
		void AddPitch(float afAngle);
		void AddYaw(float afAngle);
		void AddRoll(float afAngle);

		float GetPitch() const{ return mfPitch;}
		float GetYaw() const{ return mfYaw;}
		float GetRoll() const{ return mfRoll;}

		/**
		 * Set the limits within the pitch can move. 0 and 0 equals no limits.
		 */
		void SetPitchLimits(float afMin, float afMax){ mfPitchLimitMin = afMin; mfPitchLimitMax = afMax; }
		float GetPitchMinLimit(){return mfPitchLimitMin;}
		float GetPitchMaxLimit(){return mfPitchLimitMax;}

		void SetYawLimits(float afMin, float afMax){ mfYawLimitMin = afMin; mfYawLimitMax = afMax; }
		float GetYawMinLimit(){return mfYawLimitMin;}
		float GetYawMaxLimit(){return mfYawLimitMax;}

		//////////////////////////////////////////////////
		////////// MATRIX ROTATION ///////////////////////
		//////////////////////////////////////////////////

		void SetForward(const cVector3f& avX);
		void SetRight(const cVector3f& avX);
		void SetUp(const cVector3f& avX);

		void SetRotationMatrix(const cMatrixf& a_mtxRot);

		cVector3f GetForward();
		cVector3f GetRight();
		cVector3f GetUp();

		const cMatrixf& GetRotationMatrix() { return m_mtxMatrixRotation; }

		//////////////////////////////////////////////////
		////////// PROPERTIES /////////////////
		//////////////////////////////////////////////////
		
		const cMatrixf& GetViewMatrix();
		const cMatrixf& GetProjectionMatrix();
		
		const cMatrixf& GetMoveMatrix();

		//iCamera stuff:
		void SetModelViewMatrix(iLowLevelGraphics* apLowLevel);
		void SetProjectionMatrix(iLowLevelGraphics* apLowLevel);
		cVector3f GetEyePosition(); 

		//////////////////////////////////////////////////
		////////// RENDER SPECIFIC ///////////////////////
		//////////////////////////////////////////////////
		
		void SetPrevView(const cMatrixf &a_mtxA){ m_mtxPrevView = a_mtxA;}
		void SetPrevProjection(const cMatrixf &a_mtxA){m_mtxPrevProjection = a_mtxA;}
		
		cMatrixf& GetPrevView(){ return m_mtxPrevView;}
		cMatrixf& GetPrevProjection(){ return m_mtxPrevProjection;}

	private:
		void UpdateMoveMatrix();

		cVector3f mvPosition;

		float mfFOV;
		float mfAspect;
		float mfFarClipPlane;
		float mfNearClipPlane;

		cVector2f mvViewSize;

		eProjectionType mProjectionType;

		float mfPitch;
		float mfYaw;
		float mfRoll;
		float mfPitchLimitMin;
		float mfPitchLimitMax;
		float mfYawLimitMin;
		float mfYawLimitMax;
		
		eCameraRotateMode mRotateMode;
		eCameraMoveMode mMoveMode;

		cMatrixf m_mtxView;
		cMatrixf m_mtxProjection;
		cMatrixf m_mtxMove;

		cMatrixf m_mtxPrevView;
		cMatrixf m_mtxPrevProjection;

		cMatrixf m_mtxMatrixRotation;

		cNode3D mNode;

		cFrustum mFrustum;

		bool mbInfFarPlane;

		bool mbViewUpdated;
		bool mbProjectionUpdated;
		bool mbMoveUpdated;
		bool mbFrustumUpdated;
	};

};
#endif // HPL_CAMERA_H
