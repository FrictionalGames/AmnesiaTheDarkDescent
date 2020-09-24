
//! File: <EmulatedDevice.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_EMULATEDDEVICE_H_
#define _HAPTX_EMULATEDDEVICE_H_

#include "HapticsDevice.h"

namespace HaptX
{
	// An EmulatedDevice is a HapticsDevice that gets its state from user input instead of
	// from a device driver. The members m_setPosition, m_setOrientation and m_setButton 
	// should be set by the user. The m_force and m_torque are the calculated force and torque 
	// that would have been sent to the haptics device drivers.
	// 
	// The EmulatedDevice is useful for developers that want to test their applications 
	// without having a real haptics device connected to the system.
	// 
	// Any values may be used to set the values of a EmulatedDevice, but typically 
	// one would use the Keyboard or Mouse as direct or indirect input.
	// 
	// The position and orientation used in the haptics loop is changed so that the device 
	// moves towards the latest m_setPosition and m_setOrientation using weighted averages 
	// that can be customised using m_positionWeighting and m_orientationWeighting. This also 
	// affects the values of m_position, m_orientation, m_devPosition and m_devOrientation. 

	class HAPTX_SOLVER_SPEC EmulatedDevice : public HapticsDevice
	{
	public:
		
							EmulatedDevice();		// Constructor.
		virtual				~EmulatedDevice();		// Destructor.

		virtual HX_RESULT	StartHapticsLoop();		// Virtual function which is overridden from HapticsDevice.
			
		virtual void		InitDevice();			// Virtual function which is overridden from HapticsDevice.
		virtual void		Reset();				// Virtual function which is overridden from HapticsDevice.
		virtual void		StopHapticsLoop();		// Virtual function which is overridden from HapticsDevice.
		virtual void		UpdateDevice();			// Virtual function which is overridden from HapticsDevice.
		virtual void		ThreadFunction();		// Virtual function which is overridden from HapticsDevice.

	public:

		Vec3f				m_setPosition;		// Sets the position of the EmulatedDevice in world coordinates. Default: 0,0,0 (m)
		Rotation			m_setOrientation;	// Sets the orientation of the EmulatedDevice relative to a starting orientation pointing down the negative z axis. Default: 1,0,0,0 (rad)
		bool				m_setButton;		// Sets the state of the button to true/false. Default: false
		Vec3f				m_force;			// The calculated force that would have been sent to the haptics device drivers (N).
		Vec3f				m_torque;			// The calculated torque that would have been sent to the haptics device drivers (N).

		// Used in the real-time loop to weight the previous real-time position with the m_setPosition value and move the device towards the 
		// m_setPostition value. Default:0.01
		mgFloat m_positionWeighting;

		// Used in the real-time loop to weight the previous real-time orientation with the m_setOrientation value and rotate the device towards the 
		// m_setOrientation value. Default:0.01
		mgFloat m_orientationWeighting;

		// real-time variables
		_LARGE_INTEGER		last_lp_count;
		float				rt_time;
		float				rt_count;
		Vec3f				rt_position;
		Rotation			rt_orientation;
		bool				rt_button;
		Vec3f				rt_force;
		Vec3f				rt_torque;
		mgFloat				rt_positionWeighting;
		mgFloat				rt_orientationWeighting;

		Vec3f				m_newPosition;
		Rotation			m_newOrientation;

	};
}

#endif
