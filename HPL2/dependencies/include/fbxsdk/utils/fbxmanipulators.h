/****************************************************************************************
 
   Copyright (C) 2013 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxmanipulators.h
#ifndef _FBXSDK_UTILS_MANIPULATORS_H_
#define _FBXSDK_UTILS_MANIPULATORS_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/core/fbxobject.h>
#include <fbxsdk/core/math/fbxvector2.h>
#include <fbxsdk/core/math/fbxvector4.h>
#include <fbxsdk/scene/geometry/fbxcamera.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

class FbxCameraManipulationState;

/** This class can be used to provide basic camera manipulation in any program using this library.
  * \nosubgrouping
  */
class FBXSDK_DLL FbxCameraManipulator : public FbxObject
{
	FBXSDK_OBJECT_DECLARE(FbxCameraManipulator, FbxObject);

public:
	//! All possible manipulation actions that can be performed on a camera using this manipulator.
	enum EAction
	{
		eNone,		//!< No action.
		eOrbit,		//!< Orbiting camera around LootAt/Interest position.
		eDolly,		//!< Moving camera closer or away from its LookAt/Intest position.
		ePan,		//!< Panning camera up, down and sideways.
		eFreePan	//!< Panning and dollying all at once.
	};

	/** Begin manipulation of the camera.
	* \param pAction The action performed for this manipulation scope.
	* \param pX Begin horizontal position of the manipulation, in pixels.
	* \param pY Begin vertical position of the manipulation, in pixels. */
	void Begin(EAction pAction, float pX, float pY);

	/** Notify manipulation of latest input.
	* \param pX Horizontal position of the manipulation, in pixels.
	* \param pY Vertical position of the manipulation, in pixels.
	* \param pZ Depth position of the manipulation, in pixels. Only used by eDollyPan action. */
	void Notify(float pX, float pY, float pZ=0);

	//! End current manipulation.
	void End();

	/** Change camera position, rotation and LookAt node to frame all objects.
	* \param pTime Current time.
	* \param pOnAnimLayer Specify which animation layer to use for the evaluation. */
	void FrameAll(const FbxTime& pTime=FBXSDK_TIME_INFINITE, int pOnAnimLayer=0);

	/** Change camera position, rotation and LookAt to frame all selected objects.
	* \param pTime Current time.
	* \param pOnAnimLayer Specify which animation layer to use for the evaluation. */
	void FrameSelected(const FbxTime& pTime=FBXSDK_TIME_INFINITE, int pOnAnimLayer=0);

	/** Retrieve current manipulation action.
	* \return The action currently performed by the camera manipulator. */
	EAction GetCurrentAction() const;

	/** The camera controlled by the manipulator. */
	FbxPropertyT<FbxReference> Camera;

	/** Width of the camera viewport, in pixels. This is used to accurately calculate to movement speed.
	* \remark If this property is not correctly set, movements will be erronous. */
	FbxPropertyT<FbxFloat> ViewportWidth;

	/** Height of the camera viewport, in pixels. This is used to accurately calculate to movement speed.
	* \remark If this property is not correctly set, movements will be erronous. */
	FbxPropertyT<FbxFloat> ViewportHeight;

	/** Invert the camera horizontal manipulation direction if set to true. False by default. */
	FbxPropertyT<FbxBool> InvertX;

	/** Invert the camera vertical manipulation direction if set to true. False by default. */
	FbxPropertyT<FbxBool> InvertY;

	/** Restore the camera transform upon destruction of the manipulator. */
	FbxPropertyT<FbxBool> Restore;

/*****************************************************************************************************************************
** WARNING! Anything beyond these lines is for internal use, may not be documented and is subject to change without notice! **
*****************************************************************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
	virtual void Construct(const FbxObject* pFrom);
	virtual void Destruct(bool pRecursive);
	virtual void ConstructProperties(bool pForceSet);
	virtual bool PropertyNotify(EPropertyNotifyType pType, FbxProperty& pProperty);

private:
	void		Reset();
	FbxCamera*	GetCamera() const;
	FbxNode*	GetCameraNode() const;
	FbxNode*	GetCameraLookAtNode() const;
	FbxNode*	GetCameraTargetUpNode() const;
	FbxVector4	GetCameraPosition() const;
	void		SetCameraPosition(const FbxVector4& pPosition);
	FbxVector4	GetCameraRotation() const;
	void		SetCameraRotation(const FbxVector4& pRotation);
	FbxVector4	GetCameraLookAtPosition() const;
	void		SetCameraLookAtPosition(const FbxVector4& pPosition);
	FbxVector4	GetCameraTargetUpPosition() const;
	void		SetCameraTargetUpPosition(const FbxVector4& pPosition);

	double		ComputeRotationAxis(FbxVector4& pFront, FbxVector4& pUp, FbxVector4& pRight) const;
	void		ComputeRotationMatrix(FbxAMatrix& pRM);
	void		UpdateCameraRotation();

	void		MoveCameraToFitBBoxInFrustum(int pOnAnimLayer, const FbxTime& pTime, const FbxVector4& pBBoxMin, const FbxVector4& pBBoxMax);
	bool		Frame(int pOnAnimLayer, const FbxTime& pTime, bool pSelected);
	void		Evaluate(double& v, const FbxTime& pTime, FbxProperty& p, FbxAnimLayer* pAnimLayer);

	EAction		mCurrentAction;
	float		mBeginMousePos[3], mLastMousePos[3];
	FbxVector4	mBeginPosition, mBeginLookAtPosition, mBeginTargetUpPosition, mRotationAxis[3];
  	FbxVector4	mInitialPosition, mInitialRotation, mInitialLookAt;
    bool        mReverseX;
#endif /* !DOXYGEN_SHOULD_SKIP_THIS *****************************************************************************************/
};

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_UTILS_MANIPULATORS_H_ */
