
//! File: <HaptXDefines.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_DEFINES_H_
#define _HAPTX_DEFINES_H_

#include "HXSolver.h"

namespace HaptX 
{
	#define HX_ERR_MSG		40	// Catch any error messages from within the HaptX loop in the message callback function.
	#define HX_WARN_MSG		30	// Catch warning messages in the message callback function.
	#define HX_INFO_MSG		20	// Catch information messages in the message callback function.
	#define HX_DBG_MSG		10	// Catch debug messages in the message callback function.
	#define HX_HAPTIC_LOG  -10	// Log the current force and position of the Touch Controller in the message callback function.

	#define HX_SAFETY_MAXFORCE 10.0f // For safety reasons do not allow forces stronger than 10 N. (See HaptXDisableSafetyMaxForce()) 

	// <COMBINE HaptX::HaptXResults>
	typedef DWORD HX_RESULT;

	typedef int iSurfaceID;
	typedef int iHapticDeviceID;
	typedef int iHapticShapeID;
	typedef int iForceModelID;

	// Contains HaptX message definitions. 
	namespace HaptXResults
	{
		#define HX_SUCCESS							0x00000000				// The function succeded without errors.

		#define HX_GENERIC_ERROR					0x00100000				// A generic error.
		#define HX_VALUE_OUT_OF_RANGE				HX_GENERIC_ERROR + 1	// A value is out of range.
		#define HX_OUT_OF_MEMORY					HX_GENERIC_ERROR + 2	// Out of memory.
		#define HX_NULL_POINTER						HX_GENERIC_ERROR + 3	// A null pointer is found.

		#define HX_DEVICE_ERROR						0x00200000				// A general device error.
		#define HX_DEVICE_NOT_FOUND					HX_DEVICE_ERROR + 1		// The device with a specific id is not found.
		#define HX_DEVICE_ALREADY_EXISTS			HX_DEVICE_ERROR + 2		// A device already exists.
		#define HX_DEVICE_MODEL_NOT_RECOGNIZED		HX_DEVICE_ERROR + 3		// The device model type could not be recognized.
		#define HX_DEVICE_COULD_NOT_BE_CREATED		HX_DEVICE_ERROR + 4		// The device could not be created.
		#define HX_DEVICE_NOT_AVAILABLE				HX_DEVICE_ERROR + 5		// Another application failed to yield control of the Touch Controller to this application.
		#define	HX_DEVICE_NOT_ACTIVE				HX_DEVICE_ERROR + 6		// Another application currently has control of the Touch Controller.
		#define HX_DEVICE_TYPE_ERROR				HX_DEVICE_ERROR + 7		// The device with a specific id is not of the expected type.

		#define HX_SHAPE_ERROR						0x00300000				// A general shape error.
		#define HX_SHAPE_NOT_FOUND					HX_SHAPE_ERROR + 1		// The shape with the specific id is not found.
		#define HX_SHAPE_INPUT_VALUE_ERROR			HX_SHAPE_ERROR + 2		// An input value is out of range.
		#define HX_SHAPE_INVALID_OCT_HEADER			HX_SHAPE_ERROR + 3		// The shape octree header is invalid.
		#define HX_SHAPE_INCOMPATIBLE_OCT_VERSION	HX_SHAPE_ERROR + 4		// The shape octree version is incompatible.

		#define HX_SURFACE_ERROR					0x00400000				// A general surface error.
		#define HX_SURFACE_NOT_FOUND				HX_SURFACE_ERROR + 1	// The surface with a specific id is not found.
		#define HX_SURFACE_TYPE_ERROR				HX_SURFACE_ERROR + 2	// The surface with a specific id is not of the expected type.

		#define HX_FORCEMODEL_ERROR					0x00500000				// A general ForceModel error.
		#define HX_FORCEMODEL_NOT_FOUND				HX_FORCEMODEL_ERROR + 1 // The ForceModel with a specific id is not found.
		#define HX_FORCEMODEL_NOT_ANIMATED			HX_FORCEMODEL_ERROR + 2	// The ForceModel with a specific id is not an animated type.
		#define HX_FORCEMODEL_RUNNING				HX_FORCEMODEL_ERROR + 3 // The ForceModel is already running.

		#define HX_MOUSE_ERROR						0x00600000				// A general mouse error.
		#define HX_MOUSE_ALREADY_RUNNING			HX_MOUSE_ERROR + 1		// The mouse driver is already running.
		#define HX_MOUSE_NOT_RUNNING				HX_MOUSE_ERROR + 2		// The mouse driver is not running.
		#define HX_MOUSE_NOT_FOUND					HX_MOUSE_ERROR + 3		// The mouse driver could not be found on the system.

		#define HX_SERIAL_ERROR						0x00700000				// A general serial number error.
		#define HX_SERIAL_INVALID					HX_SERIAL_ERROR + 1		// HaptXInit was called with an invalid serial number.
		#define HX_SERIAL_EXPIRED					HX_SERIAL_ERROR + 2		// The HaptX license has expired.
		#define HX_SERIAL_STOLEN					HX_SERIAL_ERROR + 3		// HaptXInit was called with a stolen serial number.
	}

	static inline bool HX_SUCCEEDED(const HX_RESULT x)						{ return (x == HX_SUCCESS || x == HX_FORCEMODEL_RUNNING || x == HX_MOUSE_ALREADY_RUNNING); }
	static inline bool HX_FAILED(const HX_RESULT x)							{ return (!(x == HX_SUCCESS || x == HX_FORCEMODEL_RUNNING || x == HX_MOUSE_ALREADY_RUNNING)); }

	enum surfaceType_e 
	{
		UNDEFINED			= 0,
		SIMPLE				= 1, 
		FRICTIONAL			= 2, 
		VIBRATING			= 3, 
		BUTTON_FRICTIONAL	= 4,
		ROUGH				= 5, 
		HEIGHTMAP			= 6, 
		STICKY_FRICTIONAL	= 7,
		STICKY_HEIGHTMAP	= 8,
		FRICTION_IMAGE		= 9
	};

	enum deviceType_e
	{
		ALL					= 0,
		PHANTOM				= 1,
		FALCON				= 2
	};

	enum deviceEnumBool_e
	{
		HX_STOP = 0,
		HX_CONTINUE = 1
	};

	enum textureCoordinates_e
	{
		HX_BOTTOM_LEFT = 0,
		HX_TOP_LEFT    = 1
	};

	enum forceCoordinates_e
	{
		HX_ABSOLUTE_GLOBAL	= 0,
		HX_ABSOLUTE_LOCAL	= 1
	};
}

#endif
