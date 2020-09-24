
// File: <TrackingDevice.h>
// Copyright (c) 1997-2007, Reachin Technologies AB.
// All rights reserved.
// The copyright to the computer program herein is the property of Reachin
// Technologies AB, Sweden. The program may be used and/or copied only with the
// written permission of Reachin Technologies AB or in accordance with the terms
// stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_TRACKINGDEVICE_H_
#define _HAPTX_TRACKINGDEVICE_H_

#include "ThreadDevice.h"
#include "LinearAlgebra.h"

namespace HaptX
{
	// A TrackingDevice represents a generic 3d tracker with a
	// switch.
	// The m_position and m_orientation fields describe
	// the device in global coordinates (metres). The corresponding 
	// m_devPosition and m_devOrientation are the positions
	// as read from the device in device-space coordinates. The 
	// m_posCalibration and m_ornCalibration define the
	// mapping from device to global coordinates. The m_button
	// defines the state of the device's button. The 
	// m_orientation describes the rotation of the device
	// relative to a starting orientation pointing down the negative z axis.
	class HAPTX_SOLVER_SPEC TrackingDevice : public ThreadDevice
	{
	public:
		TrackingDevice();					// Constructor
		virtual ~TrackingDevice();			// Destructor

	public:
		// The m_position and m_orientation fields describe 
		// the device in global coordinates (metres).
		Vec3f m_position;

		// The m_posCalibration and m_ornCalibration define the mapping from device 
		// coordinates to global coordinates.
		Matrix4f m_posCalibration;

		// The m_devPosition and m_devOrientation are the positions
		// as read from the device in device-space coordinates.
		Vec3f m_devPosition;

		// The m_position and m_orientation fields describe 
		// the device in global coordinates (metres).
		Rotation m_orientation;

		// The m_posCalibration and m_ornCalibration define the mapping from device 
		// coordinates to global coordinates.
		Rotation m_ornCalibration;
	
		// The m_devPosition and m_devOrientation are the positions
		// as read from the device in device-space coordinates.
		Rotation m_devOrientation;
	
		// The m_button holds the status of the button of the device.
		bool m_button;
	
		// Holds information about device errors. During normal operation, m_errorState is empty.
		std::string m_errorState;
	};
}

#endif
