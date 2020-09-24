
//! File: <Device.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_DEVICE_H_
#define _HAPTX_DEVICE_H_

#include <string>
#include "HXSolver.h"
#include "HaptXDefines.h"

namespace HaptX
{
	// A Device represents a hardware input device.
	class HAPTX_SOLVER_SPEC Device
	{
	public:
						Device ();		// Constructor
        virtual			~Device();		// Destructor
		
		virtual void	UpdateDevice();	// UpdateDevice is called after each render loop to update members.

	public:
		std::wstring	m_name;			// The name of the device. 
		iHapticDeviceID	m_deviceID;		// The id of the device. 
	};
}

#endif
