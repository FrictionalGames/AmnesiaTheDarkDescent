
//! File: <CameraNavigation.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _CAMERA_NAVIGATION_H_
#define _CAMERA_NAVIGATION_H_

#include "HXSolver/LinearAlgebra.h"
#include "HXSolver/HaptXDefines.h"


namespace HaptX
{
	class HaptXInterface;
	class ForceModelInfo;
	class ForceModelNavigationInfo;

	enum cameraNavigationType_e
	{
		HX_CAMERA_NAVIGATION_RELATIVE,
		HX_CAMERA_NAVIGATION_ABSOLUTE,
		HX_CAMERA_NAVIGATION_PENUMBRA_RELATIVE,
		HX_CAMERA_NAVIGATION_FPS,
		HX_CAMERA_NAVIGATION_CUSTOM
	};

	class ICameraNavigation
	{
	public:
		ICameraNavigation()
		{
			m_cameraRotation.x = m_cameraRotation.y = 0.0;
			m_cameraSensitivity.x = m_cameraSensitivity.y = 1.0;
			m_controllerOffset.x = 0.0;
			m_controllerOffset.y = 0.0;
			m_controllerOffset.z = 0.0;
		}

		virtual ~ICameraNavigation()															{}

		void								SetCameraRotation(Vectors::Vec2f rot)				{ m_cameraRotation = rot; }
		void								SetControllerOffset(Vectors::Vec3f off)				{ m_controllerOffset = off; }
		void								SetCameraSensitivity(Vectors::Vec2f sen)			{ m_cameraSensitivity = sen; }
		
		Vectors::Vec2f						GetCameraRotation()									{ return m_cameraRotation; }
		Vectors::Vec3f						GetControllerOffset()								{ return m_controllerOffset; }
		Vectors::Vec2f						GetCameraSensitivity()								{ return m_cameraSensitivity; }

		virtual cameraNavigationType_e		GetNavigationType() = 0;
		virtual HX_RESULT					Update(mgFloat deltaTime) = 0;

	protected:
		Vectors::Vec2f						m_cameraRotation;
		Vectors::Vec2f						m_cameraSensitivity;
		Vectors::Vec3f						m_controllerOffset;
		HaptXInterface*						m_hxInterface;
		iHapticDeviceID						m_hxDeviceID;
	};

	// ########################################################################################################################

	class CameraNavigation_Relative : public ICameraNavigation
	{
	public:
		CameraNavigation_Relative(HaptXInterface* eng, const iHapticDeviceID& id);
		virtual ~CameraNavigation_Relative();

		virtual void			SetForceButtonDown(bool b);
		virtual void			SetActivationButton(int num);

		virtual bool			GetForceButtonDown();
		virtual int				GetActivationButton();

		cameraNavigationType_e	GetNavigationType();
		HX_RESULT				Update(mgFloat deltaTime);

	protected:
		Vectors::Vec3f			m_initp;
		bool					m_firstTime;
		bool					m_forceButtonDown;
		int						m_useButtonNum;
	};

	// ########################################################################################################################

	class CameraNavigation_Absolute : public ICameraNavigation
	{
	public:
		CameraNavigation_Absolute(HaptXInterface* eng, const iHapticDeviceID& id);
		virtual ~CameraNavigation_Absolute();

		virtual void			SetForceButtonDown(bool b);
		virtual void			SetActivationButton(int num);

		virtual bool			GetForceButtonDown();
		virtual int				GetActivationButton();

		cameraNavigationType_e	GetNavigationType();
		HX_RESULT				Update(mgFloat deltaTime);

	protected:
		Vectors::Vec3f			m_initp;
		bool					m_firstTime;
		bool					m_forceButtonDown;
		int						m_useButtonNum;
	};

	// ########################################################################################################################

	class CameraNavigation_PenumbraRelative : public ICameraNavigation
	{
	public:
		CameraNavigation_PenumbraRelative(HaptXInterface* eng, const iHapticDeviceID& id);
		virtual ~CameraNavigation_PenumbraRelative();

		virtual void				SetEffectRadius(mgFloat r);
		virtual void				SetEffectAspect(mgFloat a);
		virtual void				SetSpringStrength(mgFloat s);

		virtual mgFloat				GetEffectRadius();
		virtual mgFloat				GetEffectAspect();
		virtual mgFloat				GetSpringStrength();

		cameraNavigationType_e		GetNavigationType();
		HX_RESULT					Update(mgFloat deltaTime);

	protected:
		mgFloat						m_effectRadius;
		mgFloat						m_effectAspect;
		mgFloat						m_springStrength;
		ForceModelInfo*				m_forceModel;
	};

	// ########################################################################################################################

	class  CameraNavigation_FPS : public ICameraNavigation
	{
	public:
		CameraNavigation_FPS(HaptXInterface* eng, const iHapticDeviceID& id);
		virtual ~CameraNavigation_FPS();

		virtual void				SetCameraSensitivityBoundary(Vec2f cs);	//	Sets the relative camera sensitivity, see m_cameraSensitivityBoundary
		virtual void				SetExploring(bool t);					//	Sets if the camera should be set in exploration mode or not, see m_exploring
		virtual void				SetBoundaryRadius(mgFloat r);			//	Sets the boundary radius, see m_boundaryRadius
		virtual void				SetBoundaryStiffness(mgFloat s);		//	Sets the stiffness of the navigation spring, see m_boundaryStiffness
		virtual void				SetBoundaryClickForce(mgFloat c);		//	Sets the force that is needed to be exceeded to get to m_boundaryRadius, see m_boundaryClickForce

		virtual Vec2f				GetCameraSensitivityBoundary();			//	Gets the relative camera sensitivity, see m_cameraSensitivityBoundary
		virtual bool				GetExploring();							//	Gets m_exploring parameter
		virtual mgFloat				GetBoundaryRadius();					//	Gets the boundary radius, see m_boundaryRadius	
		virtual mgFloat				GetBoundaryStiffness();					//	Gets the stiffness of the navigation spring, see m_boundaryStiffness
		virtual mgFloat				GetBoundaryClickForce();				//	Gets the force that is needed to be exceeded to get to m_boundaryRadius, see m_boundaryClickForce

		cameraNavigationType_e		GetNavigationType();
		HX_RESULT					Update(mgFloat deltaTime);

	protected:
		ForceModelNavigationInfo*	m_forceModelNavigation;			//	info object for the ForceModel 
		Vec3f						m_lastPos;						//	m_lastPos is used internally for the absolute navigation
		Vec3f						m_internalPos;					//	used internally for absolute navigation
		Vec2f						m_cameraSensitivityBoundary;	//	determines how much the relative camera will rotate when the boundary distance has been crossed, default Vec2f(0.0005, 0.0005)
		bool						m_exploring;					//	determines if the volume inside m_boundaryRadius is used for absolute camera navigation or not, default false 
		mgFloat						m_boundaryRadius;				//	the radius of the cylinder that defines the border between absolute / explore navigation and relative navigation, default 15.0
		mgFloat						m_boundaryStiffness;			//	the stiffness of the spring that is outside m_boundaryRadius, default 0.15
		mgFloat						m_boundaryClickForce;			//	the "button" force that defines the haptic border at m_boundaryRadius, default 0.1
		mgFloat						m_deviceRadius;					//	used internally to get correct position with world scale from hardware position
		mgFloat						m_worldScale;					//	used internally with m_deviceRadius
		
	};
};

#endif
