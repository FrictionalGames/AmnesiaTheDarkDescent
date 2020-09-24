
//! File: <DeviceManager.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_DEVICEMANAGER_H_
#define _HAPTX_DEVICEMANAGER_H_

#include <map>
#include "HXSolver.h"

#pragma warning( disable: 4251 )								//! Disable warning bescause STL templates are exported implicitly.

namespace HaptX
{
	class DeviceManager;
	class HapticsDevice;
	class TrackingDevice;
	class Device;

	HAPTX_SOLVER_SPEC DeviceManager*	GetDeviceManager();		// Global function to get a pointer to the DeviceManager.
	HAPTX_SOLVER_SPEC void				CreateDeviceManager();	// Global function to create the DeviceManager.
	HAPTX_SOLVER_SPEC void				ReleaseDeviceManager();	// Global function to release the DeviceManager.
	HAPTX_SOLVER_SPEC void				ClearAndFreeHMPool();	
  
	// The DeviceManager handles all registerd devices. When a device gets registered in the DeviceManager
	// an ID is assigned to it. The DeviceManager is also taking care of the deletion of devices, which
	// includes shutting down the HaptX thread for the device.
	// The DeviceManager also has functions for handling the render lists of a HapticsDevice (both HapticShapes
	// and ForceModels).
	class HAPTX_SOLVER_SPEC DeviceManager 
	{
	public:
						DeviceManager();										// Constructor.
						~DeviceManager();										// Destructor.
		
		int				RegisterDevice(HapticsDevice* d);						// RegisterDevice adds the HapticsDevice to the map of devices and returns a device id to the HapticsDevice. -1 is returned if the regisration is unsuccessful.
		void			UnregisterDevice(int deviceID);							// Deletes the device with the given id.

		TrackingDevice* GetTrackingDevice(int deviceID);						// Returns a pointer to the TrackingDevice with the specified id. NULL if no such id exists or if the device instance with that id isn't a TrackingDevice. 
		Device*			GetDevice(std::wstring* name);							// Returns a pointer to the Device with the specified name or NULL if no such name exists. 
		HapticsDevice*	GetHapticsDevice(int deviceID);							// Returns a pointer to the HapticsDevice with the specified id. NULL if no such id exists  or if the device instance with that id isn't a HapticsDevice. 
		
		bool			DeviceExists(int deviceID);								// Returns true if a Device with the deviceID exists.
		bool			RemoveShapeFromRenderLists(int shapeID);				// Removes the HapticShape id from the render list of all devices.
		bool			AddShapeToRenderLists(int shapeID);						// Adds a HapticShape id to the render list of all devices.
		bool			RemoveForceModelFromRenderLists(int forceModelID);		// Removes the ForceModel id from the render list of all devices.

	public:
		std::map<int, HapticsDevice*> m_mapHapticDevices;						// A map which contains all Device instances and their ID's.

	protected:
		int				m_nextID;												// The ID that the next device will get.
	};	
} 

#endif
