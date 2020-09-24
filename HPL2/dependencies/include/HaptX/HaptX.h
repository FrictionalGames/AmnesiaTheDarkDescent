
//! File: <HaptX.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_H_
#define _HAPTX_H_

#include "SurfaceInfo.h"
#include "ForceModelInfo.h"
#include "CameraNavigation.h"
#include "HXSolver/HaptXDefines.h"
#include "HXSolver/CollisionCallback.h"

#ifndef HAPTX_ENGINE_DLL_BUILD
	#ifdef _DEBUG
		#pragma comment(lib,"HXSolver_d.lib")
		#pragma comment(lib,"HXEngine_d.lib")
	#else
		#pragma comment(lib,"HXSolver.lib")
		#pragma comment(lib,"HXEngine.lib")
	#endif
#endif

// Contains all classes and interface functions in HaptX. 
namespace HaptX 
{
	class HaptXInterface;
	
	extern HANDLE hDeviceEnumerated;

	// Only available in HaptX Pro. The ForceModel callback function pointer. 
	typedef Vec3f (*pfnForceModel)(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t, void * userData);

	// The device enumeration callback function. This function will be 
	// called once per Touch Controller that is found. 
	typedef int (*pfnDeviceEnum)(std::wstring* deviceName, std::wstring* deviceModel, void* vendorData);

	// The message callback function.
	// Parameters: Severity - Defines the message type (HAPTX_ERR_MSG, HAPTX_WARN_MSG, HAPTX_INFO_MSG, HAPTX_DBG_MSG, HAPTX_HAPTIC_LOC) 
	typedef void (*pfnMessageCallBack)(int Severity, std::wstring Message);

	
	HAPTX_ENGINE_SPEC HaptXInterface*	GetHaptXInterface();			// Get a pointer to the HaptXInterface.
	HAPTX_ENGINE_SPEC void				ReleaseHaptXInterface();		// Release the HaptXInterface.
	
	
	// The HaptX Interface contains all functions required to set up HaptX rendering.
	class HAPTX_ENGINE_SPEC HaptXInterface 
	{
	public:
		HaptXInterface();				// Constructor
		~HaptXInterface();				// Destructor

	
		// ########################## All Devices Functions ##########################
		
		// Find all Touch Controllers that are connected to the computer. The callback 
		// function is called once per Touch Controller that is available for usage.
		// Parameters: ptr - a function pointer of type pfnDeviceEnum. 
		// vendorData - Vendor specific data, used to point out path to .ini file for the Falcon.
		// deviceType - see deviceType_e
		HX_RESULT AllDevicesEnum(pfnDeviceEnum ptr, void* vendorData = NULL, deviceType_e deviceType = ALL);
		

		// ########################## Device Functions ##########################
	
		// Create a new device instance. Each device has its own HaptX rendering thread. If there is no device
		// connected to the computer the function could be called with deviceName = NULL and deviceModel = "HAPTX_EMULATED_DEVICE"
		// to create an emulated device. See also DeviceSetEmulatedPosition.
		// Parameters:	retval - The ID of the device. see iHapticDeviceID. 
		// deviceName - Pointer to a wstring containing the name of the device. 
		// deviceModel - Pointer to a wstring containing the name of the model.
		// vendorData - Vendor specific data, used to point out path to .ini file for the Falcon.
		HX_RESULT DeviceCreate(iHapticDeviceID& retval, std::wstring* deviceName, std::wstring* deviceModel, void* vendorData);
		
		// If the Touch Controller, for some reason, is disconnected from the 
		// computer after HaptX has been initialized, the function returns false.
		// Parameters:	retval - The return value.
		// deviceID - The ID of the device.
		HX_RESULT DeviceIsConnected(bool& retval, iHapticDeviceID deviceID);

		// When disabling forces the HaptX rendering is still executed, but the 
		// resulting force vector is not sent to the Touch Controller device. 
		// By default, HaptX sends all force vectors to the device.
		// Parameters: deviceID - The ID of the device.
		HX_RESULT DeviceDisableForces(iHapticDeviceID deviceID);
	
		// When disabling forces the HaptX rendering is still executed, but the 
		// resulting force vector is not sent to the Touch Controller device. 
		// By default, HaptX sends all force vectors to the device.
		// Parameters: deviceID - The ID of the device.
		HX_RESULT DeviceEnableForces(iHapticDeviceID deviceID);
 
		// Destroy the device object with the given ID and terminate its HaptX rendering thread.
		// Parameters: deviceID - The ID of the device.
		HX_RESULT DeviceDestroy(iHapticDeviceID deviceID);

		// Execute HaptX rendering on the specified device. 
		// This function has to be called once per graphics loop.
		// Parameters: deviceID - The ID of the device. 
		HX_RESULT DeviceRender(iHapticDeviceID deviceID);

		// Reactivates the HaptX rendering for the specified device.
		// This function should always be called every time the application regains focus.
		// If another HaptX application has had focus then that application will have 
		// exclusive rights to the Touch Controller. Calling this function ensures
		// that control is set back to this application instead.
		// Parameters: deviceID - The ID of the device. 
		HX_RESULT DeviceReactivateRendering(iHapticDeviceID deviceID);

		// Get the position as reported from the Touch Controller - no transformations applied.
		// Returns the position specified in the device specific local coodinate system. 
		// The unit (e.g. millimeter or meter) is device specific.
		// Parameters: retval - hardware position in local coordinates. Unit is device specific.
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetHardwarePosition(Vec3f& retval, iHapticDeviceID deviceID);
 
		// Get the unconstrained device position in global coordinates. 
		// Global coordinates include device calibration and any camera transformations.  
		// The unconstrained position is the position of the device, before being 
		// constrained by any HaptX Surfaces.
		// Parameters: retval - Unconstrained device position in global coordinates.
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetUnconstrainedPosition(Vec3f& retval, iHapticDeviceID deviceID);

		// Get the proxy position in global coordinates. When HaptX Surfaces are used, 
		// the unconstrained device position is projected onto colliding surfaces 
		// to create the proxy position. When touching a HaptX Surface, the proxy 
		// position always remains on the surface. Global coordinates include device 
		// calibration and any camera transformations.  
		// Parameters: retval - Proxy position in global coordinates. 
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetProxyPosition(Vec3f& retval, iHapticDeviceID deviceID);
			
		// Get the position of the device in global coordinates. Global coordinates 
		// include device calibration and any camera transformations. The device position 
		// can be controlled to either stay on the surface (proxy position) or to follow 
		// the unconstrained device position into the surface. This is done by setting the 
		// proxy weighting. When rendering a graphical representation of the device position, 
		// use this method to retrieve the device position. 
		// see DeviceSetProxyWeighting.
		// Parameters: retval - device position in global coordinates. 
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetPosition(Vec3f& retval, iHapticDeviceID deviceID);
		
		// The function returns the Rotation of the Touch Controller handle 
		// in local coordinates. The local coordinate system is Touch Controller specific. 
		// 3DOF Devices will always return a Rotation with an angle of 0 radians.
		// Parameters: retval - Rotation of the Touch Controller handle (local coordinate system). 
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetHardwareRotation(Rotation& retval, iHapticDeviceID deviceID);
	
		// Get the Rotation (radians) of the Touch Controller handle in global coordinates.
		// Global coordinates include device calibration and any camera transformations.
		// 3DOF Devices will always return a Rotation with an angle of 0 radians.
		// Parameters: retval - Rotation of the Touch Controller handle. 
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetRotation(Rotation& retval, iHapticDeviceID deviceID);

		// Get the yaw, pitch and roll of the device.
		// Parameters: retval - Rotation angles of the device. 
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetYawPitchRoll(Vec3f& retval, iHapticDeviceID deviceID);
			
		// Returns true if the button with number "buttonNr" of the device currently 
		// is being pressed down. If a button with specified number does not exist the 
		// return value will always be false. The buttons are numbered 0,1,2 etc.
		// Parameters: retval - Button state.
		// deviceID - The ID of the device.
		// buttonNr - The number of the button you want know the status of.
		HX_RESULT DeviceGetButton(bool& retval, iHapticDeviceID deviceID , int buttonNr = 0);
		
		// Returns the number of buttons on a Touch Controller device.
		// Parameters: retval - Number of available buttons.
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetNrOfButtons(int& retval, iHapticDeviceID deviceID);

	    // Set the radius of the proxy sphere that is used for collision between the 
		// contact point of the device and a HaptX Surface. If the proxy is too 
		// small relative the surface's polygons, the device can fall through in between them. 
		// If the proxy radius is very large relative the HaptX Surface's polygons, this can 
		// lead to a situation where very high numbers of polygons are being evalutated at 
		// the same time by HaptX. If the evalutated polygons originates from surfaces that 
		// are not adjacent, this can lead to minor vibrations in the Touch Controller. 
		// Parameters: deviceID - The ID of the device. 
		// radius - The new radius of the proxy sphere in application coordinates.
		HX_RESULT DeviceSetProxyRadius(iHapticDeviceID deviceID, mgFloat radius);

		// Get the radius of the proxy.
		// Parameters: retval - The radius of the proxy sphere in application coordinates.
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetProxyRadius(double& retval, iHapticDeviceID deviceID);

		// The proxy weighting scales the device position between the unconstrained 
		// position and the proxy position. It affects the return value of the 
		// DeviceGetPosition method. A value of 0 scales the device position to be 
		// equal to the unconstrained device position. A value of 1 scales the device
		// position to be equal to the proxy position. Default value is 0.9.
		// Parameters: deviceID - The ID of the device.
		// proxyWeighting - The proxy weighting. Value should be between 0.0 and 1.0.
		HX_RESULT DeviceSetProxyWeighting(iHapticDeviceID deviceID, float proxyWeighting);

		// The proxy weighting scales the device position between the unconstrained 
		// position and the proxy position. It affects the return value of the 
		// DeviceGetPosition method. A value of 0 scales the device position to be 
		// equal to the unconstrained device position. A value of 1 scales the device
		// position to be equal to the proxy position. Default value is 0.9.
		// Parameters: retval - The return value. 
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetProxyWeighting(float& retval, iHapticDeviceID deviceID);

		// Enable or disable the force ramping outside the Usable Workspace Area (UWA). 
		// Most devices are stronger at the center of the Workspace Area. If the forces
		// are not scaled at the limit of the Workspace Area, small vibrations or "kicking"
		// can be felt in the Touch Controller. The ramping is disabled by default. 
		// Parameters: forceRamping - forceRamping on\off. 
		// deviceID - The ID of the device.
		HX_RESULT DeviceSetForceRampingUWA(iHapticDeviceID deviceID, bool forceRamping);
	
		// Get the value of the boolean which controls force ramping outside 
		// the Usable Workspace Area (UWA). Most devices are stronger at the center 
		// of the Workspace Area. If the forces are not scaled at the limit of the 
		// Workspace Area, small vibrations or "kicking" can be felt in the Touch Controller. 
		// The ramping is disabled by default. 
		// Parameters: retval - the return value.
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetForceRampingUWA(bool& retval, iHapticDeviceID deviceID);

		// Set the scaling of forces for a device. The scaling value will scale all 
		// forces that are sent to the device and it will also scale the maximum allowed 
		// value. Default value: 0.7 for PHANToM devices. Possible values ranges from 0.0 - 1.0. 
		// Parameters: forceScaling - The force scaling. 
		// deviceID - The ID of the device.
		HX_RESULT DeviceSetForceScale(iHapticDeviceID deviceID, float forceScaling);
		
		// Get the scaling of forces for a device. The scaling value will scale all 
		// forces that are sent to the device and it will also scale the maximum allowed 
		// value. Default value: 1.0 for FALCON and 0.7 for PHANToM touch controllers. Possible values ranges from 0.0 - 1.0. 
		// Parameters: retval - The force scaling. 
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetForceScale(float& retval, iHapticDeviceID deviceID);
		
		// Get the HaptX loop update rate (loops per second).
		// Parameters: retval - The HaptX loop update rate.
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetHapticsRate(float& retval, iHapticDeviceID deviceID);

		// Get the maximum force of the specified device (in Newtons).
		// Parameters: retval - The maximum force.
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetMaxForce(float& retval, iHapticDeviceID deviceID);

		// Get the maximum stiffness of the specified device (in Newtons/m).
		// Parameters: retval - The maximum stiffness.
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetMaxStiffness(float& retval, iHapticDeviceID deviceID);

		// Set the scale of the HaptX world. This is used to scale any HaptX Surface 
		// or ForceModel data in the HaptX coordinate system. 
		// Only uniform scaling is supported.
		// Parameters: scale -	The scale value in X, Y and Z.
		// deviceID - The ID of the device.
		HX_RESULT DeviceSetWorldScale(iHapticDeviceID deviceID, float scale);
		
		// Get the scale of the HaptX world. The World Scale is used to scale any
		// HaptX Surface or ForceModel data in the HaptX coordinate system. 
		// Only uniform scaling is supported.
		// Parameters: retval - The scale value in X, Y and Z.
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetWorldScale(float& retval, iHapticDeviceID deviceID);

		// Set the rotation of the HaptX world. The input is a quaternion.
		// The World Rotation is used to rotate any HaptX Surface or ForceModel data 
		// in the HaptX coordinate system. 
		// Parameters: deviceID - The ID of the device.
		// qX - The X part of the quaternion. 
		// qY - The Y part of the quaternion.
		// qZ - The Z part of the quaternion. 
		// qW - The W part of the quaternion.
		HX_RESULT DeviceSetWorldRotationQ(iHapticDeviceID deviceID, float qX, float qY, float qZ, float qW);

		// Set the rotation of the HaptX world. The input is a Rotation specified with 
		// an axis and an angle (radians). The World Rotation is used to rotate any HaptX 
		// Surface or ForceModel in the HaptX coordinate system. 
		// Parameters: deviceID - The ID of the device.
		// axisX -	The X part of the axis. 
		// axisY - The Y part of the axis.
		// axisZ -	The Z part of the axis. 
		// angleRad - The angle in radians.
		HX_RESULT DeviceSetWorldRotationR(iHapticDeviceID deviceID, float axisX, float axisY, float axisZ, float angleRad);

		// Get the current rotation of the HaptX world. The rotation is not 
		// dependent on camera rotation.
		// Parameters: retval - The world rotation matrix.
		// deviceID - The ID of the device.
		HX_RESULT DeviceGetWorldRotation(Matrix4f& retval, iHapticDeviceID deviceID);

		// The boolean cameraRelative controls whether the device position should 
		// depend on the current camera position or not. See DeviceSetCameraMatrix.
		// Parameters: deviceID - The ID of the device.
		// cameraRelative - A boolean which specifies if the device position should follow the camera.
		HX_RESULT DeviceSetCameraRelative(iHapticDeviceID deviceID, bool cameraRelative);				
		
		// Enable\Disable extrapolation of the camera position in the HaptX loop.
		// Enabled by default. See DeviceSetCameraRelative and DeviceSetCameraMatrix. 
		// Parameters: deviceID - The ID of the device.
		// cameraInterpolation - A boolean which specifies if the camera position should be extrapolated.
		HX_RESULT DeviceSetCameraExtrapolation(iHapticDeviceID deviceID, bool cameraExtrapolation);	
		
		// Get the value of the boolean cameraRelative which controls if the device position
		// depends on the current camera position or not. 
		// Parameters: deviceID - The ID of the device.
		// retval - The value of the boolean cameraRelative. 
		HX_RESULT DeviceIsCameraRelative(bool& retval, iHapticDeviceID deviceID);				
		
		// Set the camera matrix. If the HaptX coordinate system is rotated relative 
		// the application's coordinate system this must be taken into consideration, i.e. 
		// the method ignores rotations specified with any of the DeviceSetRotation... methods. 
		// However, HaptX treats the input data according to the HaptXSetLeftHanded and the 
		// HaptXSetRightHanded methods. If an offset between the device and the camera position 
		// is wanted, add this to the translation part of the matrix. See DeviceSetCameraRelative.
		// Parameters: deviceID - The ID of the device. 
		// camMatrix - The camera matrix float array.
		HX_RESULT DeviceSetCameraMatrix(iHapticDeviceID deviceID, float camMatrix[16]);		

		// Set the camera matrix. If the HaptX coordinate system is rotated relative 
		// the application's coordinate system this must be taken into consideration, i.e. 
		// the method ignores rotations specified with any of the DeviceSetRotation... methods. 
		// However, HaptX treats the input data according to the HaptXSetLeftHanded and the 
		// HaptXSetRightHanded methods. If an offset between the device and the camera position 
		// is wanted, add this to the translation part of the matrix. See DeviceSetCameraRelative.
		// Parameters: deviceID - The ID of the device. 
		// camMatrix - The camera matrix.
		HX_RESULT DeviceSetCameraMatrix(iHapticDeviceID deviceID, Matrix4f camMatrix);		
 
		// Set the camera matrix. See DeviceSetCameraRelative.
		// Parameters: deviceID - The ID of the device.
		// rightX - The X value of the camera right vector.
		// rightY - The Y value of the camera right vector. 
		// rightZ - The Z value of the camera right vector.
		// upX - The X value of the camera up vector.
		// upY - The Y value of the camera up vector.
		// upZ - The Z value of the camera up vector.
		// frwX - The X value of the camera forward vector.
		// frwY - The Y value of the camera forward vector.
		// frwZ - The Z value of the camera forward vector.
		// posX - The X position of the camera.
		// posY - The Y position of the camera.
		// posZ - The Z position of the camera.
		// camDeviceDistanceX - The X offset distance between the device and camera.
		// camDeviceDistanceY - The Y offset distance between the device and camera.
		// camDeviceDistanceZ - The Z offset distance between the device and camera.
		HX_RESULT DeviceSetCameraMatrix(iHapticDeviceID deviceID, 
										float rightX, float rightY, float rightZ, 
										float upX, float upY, float upZ,
										float frwX, float frwY, float frwZ,
										float posX, float posY, float posZ,
										float camDeviceDistanceX, float camDeviceDistanceY, float camDeviceDistanceZ);
		
		// Set the camera matrix. See DeviceSetCameraRelative.  
		// Parameters: deviceID - The ID of the device.
		// posX - The X position of the camera.
		// posY - The Y position of the camera.
		// posZ - The Z position of the camera.
		// rotQX - The X value of the camera rotation Quaternion.
		// rotQY - The Y value of the camera rotation Quaternion.
		// rotQZ - The Z value of the camera rotation Quaternion.
		// rotQW - The W value of the camera rotation Quaternion.
		// camDeviceDistanceX - The X offset distance between the device and camera.
		// camDeviceDistanceY - The Y offset distance between the device and camera.
		// camDeviceDistanceZ - The Z offset distance between the device and camera.
		HX_RESULT DeviceSetCameraMatrixQ(iHapticDeviceID deviceID,
										 float posX, float posY, float posZ, 
										 float rotQX,  float rotQY,  float rotQZ,  float rotQW,
										 float camDeviceDistanceX, float camDeviceDistanceY, float camDeviceDistanceZ); 

		// Set the camera matrix. See DeviceSetCameraRelative.  
		// Parameters: deviceID - The ID of the device.
		// posX - The X position of the camera.
		// posY - The Y position of the camera.
		// posZ - The Z position of the camera.
		// rotAxisX - The X value of the rotation axis.
		// rotAxisY - The Y value of the rotation axis
		// rotAxisZ - The Z value of the rotation axis
		// rotAngleRad - The Rotation angle in radians.
		// camDeviceDistanceX - The X offset distance between the device and camera.
		// camDeviceDistanceY - The Y offset distance between the device and camera.
		// camDeviceDistanceZ - The Z offset distance between the device and camera.
		HX_RESULT DeviceSetCameraMatrixR(iHapticDeviceID deviceID,
										 float posX, float posY, float posZ, 
										 float rotAxisX,  float rotAxisY,  float rotAxisZ,  float rotAngleRad,
										 float camDeviceDistanceX, float camDeviceDistanceY, float camDeviceDistanceZ); 

		// Get the camera matrix. See DeviceSetCameraRelative. 
		// Parameters: deviceID - The ID of the device.
		// retval - The camera matrix.
		HX_RESULT DeviceGetCameraMatrix(Matrix4f& retval, iHapticDeviceID deviceID);

		// Set the scale factor for all ForceModels. The scale factor is used to scale the output force
		// of all ForceModels. Range 0.0 - 1.0, Default Value: 1.0.
		// Parameters: deviceID - The ID of the device.
		// scale - The scale factor.
		HX_RESULT DeviceGetForceModelScale(float& retval, iHapticDeviceID deviceID);

		// Get the scale factor for all ForceModels. The scale factor is used to scale the output force
		// of all ForceModels. Range 0.0 - 1.0, Default Value: 1.0.
		// Parameters: deviceID - The ID of the device.
		// retval - The scale factor.
		HX_RESULT DeviceSetForceModelScale(iHapticDeviceID deviceID, float scale);

		// For safety reasons, make sure no force is ever greater than HX_SAFETY_MAXFORCE
		// SafetyMaxForce is enabled by default.
		// Parameters: deviceID - The ID of the device.
		HX_RESULT DeviceEnableSafetyMaxForce(iHapticDeviceID deviceID);

		// Disable the flag that makes sure that no force is ever greater than HX_SAFETY_MAXFORCE.
		// Warning! Use this function with extreme caution. For normal devices such as the Falcon
		// the safety limit is higher than the maximum force the device can generate.
		// So there is no need to disable this flag for those devices.
		// There are however devices that can generate forces up to 40 Newtons and higher.
		// Do not use this function unless you know what you are doing.
		// Parameters: deviceID - The ID of the device.
		HX_RESULT DeviceDisableSafetyMaxForce(iHapticDeviceID deviceID);

		// Get the state of the SafetyMaxForce flag.
		// Parameters: deviceID - The ID of the device.
		// retval - A boolean which is true if the flag is set on the specified device, otherwise false.
		HX_RESULT DeviceIsSafetyMaxForceEnabled(bool& retval, iHapticDeviceID deviceID);

		// Set a new position for a HAPTX_EMULATED_DEVICE. Any values may be used to set the position of a
		// emulated device, but typically the Keyboard or Mouse is used as direct or indirect input. See also DeviceCreate.
		// Parameters: deviceID - The ID of the device.
		// pos - The position for the HAPTX_EMULATED_DEVICE.
		HX_RESULT DeviceSetEmulatedPosition(iHapticDeviceID deviceID, Vec3f pos);

		// Set a new orientation for a HAPTX_EMULATED_DEVICE. Any values may be used to set the orientation of a
		// emulated device, but typically the Keyboard or Mouse is used as direct or indirect input. The orientation 
		// of the emulated device is relative to a starting orientation pointing down the negative z axis. 
		// See also DeviceCreate.
		// Parameters: deviceID - The ID of the device.
		// orientaion - The orientaion for the HAPTX_EMULATED_DEVICE.
		HX_RESULT DeviceSetEmulatedOrientation(iHapticDeviceID deviceID, Rotation orientaion);

		// Set a new state for the button of a HAPTX_EMULATED_DEVICE. Any values may be used to set the button state of a
		// emulated device, but typically the Keyboard or Mouse is used as direct or indirect input. See also DeviceCreate.
		// Parameters: deviceID - The ID of the device.
		// pos - The position for the HAPTX_EMULATED_DEVICE.
		HX_RESULT DeviceSetEmulatedButton(iHapticDeviceID deviceID, bool button);

		// Set a collision callback function for custom surface collisions
		// The method is a HaptX PRO exclusive.
		// Parameters: func - a pointer to a collision callback function that will be called each haptic loop (1000 frames/sec)
		// if "func" == NULL then the collision callback will be disabled
		// deviceID - The ID of the device
		HX_RESULT DeviceSetCollisionCallback(pfnCollisionCallbackFunc func, iHapticDeviceID deviceID);

		// ########################## Force Model Functions ##########################
			 
		// Create your own custom force model with the use of a function pointer. 
		// The method is a HaptX PRO exclusive. 
		// Example: Vec3f MyForceFunction(const Vec3f &pos, const mgFloat &weight, const mgFloat &delta_t, void* userData)
		// The returned vector is treated by HaptX as a force vector (Newtons).
		// Parameters: retval - The return value, An info object.
		// deviceID - The ID of the device.
		// ptr - A function pointer to the function where the force is calculated.
		// coordinateSystem - see forceCoordinates_e.
		// userData - Pointer to any custom data structure containing application specific data. 
		HX_RESULT ForceModelCreateCustom3DOF(ForceModelInfo** retval, iHapticDeviceID deviceID, pfnForceModel ptr, forceCoordinates_e coordinateSystem, void* userData = NULL);

		// <NO_DOC>
		HX_RESULT ForceModelCreateMachineGun(ForceModelInfo** retval, iHapticDeviceID deviceID, float caliber = 7.62f, int roundsPerMinute = 450, float weight = 7.0f);
		// </NO_DOC>

		// A ForceModel simulating automatic rifle recoil. 
		// MachineGun pushes the device backwards and upwards.
		// Parameters: caliber - Ammunition caliber (mm) determines the recoil towards the user. Max value 12.7.
		// deviceID - The ID of the device.
		// roundsPerMinute - Rate of fire in automatic mode.
		// weight - The weight of the weapon (kg) reduces the raising motion caused by recoil. Max value 10.
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateMachineGun(ForceModelImpulseInfo** retval, iHapticDeviceID deviceID, float caliber = 7.62f, int roundsPerMinute = 450, float weight = 7.0f);
		
		// <NO_DOC>
		HX_RESULT ForceModelCreateExplosion(ForceModelInfo** retval, iHapticDeviceID deviceID, Vec3f origin = Vec3f(0.0,0.0,0.0), float effectDistance = 50.0f, float explosionForce = 0.3f, float explosionFrequency = 4.0f, float playDuration = 2.0f);
		// </NO_DOC>

		// A ForceModel simulating an explosion. Explosion is a combination of a repelling magnet and a sinus 
		// vibration with maximum force output at the origin and zero force output at the effectDistance. Explosion 
		// is time controlled by default.
		// Parameters: origin - The origin of the explosion. 
		// deviceID - The ID of the device.
		// effectDistance - The radial distance to the explosion.
		// explosionForce - The magnitude of the repelling force.
		// explosionFrequency - The frequency of the sinus curve. 
		// playDuration - Explosion duration (seconds).
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateExplosion(ForceModelExplosionInfo** retval, iHapticDeviceID deviceID, Vec3f origin = Vec3f(0.0,0.0,0.0), float effectDistance = 50.0f, float explosionForce = 0.3f, float explosionFrequency = 4.0f, float playDuration = 2.0f);

		// <NO_DOC>
		HX_RESULT ForceModelCreateSineWave(ForceModelInfo** retval, iHapticDeviceID deviceID, Vec3f direction = Vec3f(0.0,1.0,0.0), float amplitude = 0.2f, float frequency = 2.0f);
		// </NO_DOC>
		
		// A sine wave ForceModel.
		// Parameters: direction - The normalized direction of the wave. 
		// deviceID - The ID of the device.
		// amplitude - The amplitude of the wave.
		// frequency - The frequency of the wave (Hz).
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateSineWave(ForceModelSineWaveInfo** retval, iHapticDeviceID deviceID, Vec3f direction = Vec3f(0.0,1.0,0.0), float amplitude = 0.2f, float frequency = 2.0f);

		// <NO_DOC>
		HX_RESULT ForceModelCreateSawWave(ForceModelInfo** retval, iHapticDeviceID deviceID, Vec3f direction = Vec3f(0.0,1.0,0.0), float amplitude = 0.8f, float frequency = 4.0f);
		// </NO_DOC>
		
		// A saw wave ForceModel.
		// Parameters: direction - The normal direction of the wave. 
		// deviceID - The ID of the device.
		// amplitude - The amplitude of the wave.
		// frequency - The frequency of the wave (Hz).
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateSawWave(ForceModelSawWaveInfo** retval, iHapticDeviceID deviceID, Vec3f direction = Vec3f(0.0,1.0,0.0), float amplitude = 0.8f, float frequency = 4.0f);

		// <NO_DOC>
		HX_RESULT ForceModelCreateSquareWave(ForceModelInfo** retval, iHapticDeviceID deviceID, Vec3f direction = Vec3f(0.0f, 1.0f, 0.0f), float amplitude = 0.2f, float frequency = 2.0f);
		// </NO_DOC>

		// A square wave ForceModel. 
		// Parameters: direction - The normal direction of the wave. 
		// deviceID - The ID of the device.
		// amplitude - The amplitude of the wave.
		// frequency - The frequency of the wave (Hz).
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateSquareWave(ForceModelSquareWaveInfo** retval, iHapticDeviceID deviceID, Vec3f direction = Vec3f(0.0,1.0,0.0), float amplitude = 0.3f, float frequency = 2.0f);
 
		// <NO_DOC>
		HX_RESULT ForceModelCreateImpulse(ForceModelInfo** retval, iHapticDeviceID deviceID, Vec3f force = Vec3f(0.0,0.0,0.4));
		// </NO_DOC>

		// A ForceModel simulating an impulse force. The method is a HaptX PRO exclusive. 
		// The duration of the impulse is controlled with the time parameter, which is 
		// available in the ForceModelInfo object. If the time parameter is not specified, 
		// the impulse will have infinite duration and the resulting effect will be 
		// a constant force. 
		// Parameters: force - The force.
		// deviceID - The ID of the device.
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateImpulse(ForceModelImpulseInfo** retval, iHapticDeviceID deviceID, Vec3f force = Vec3f(0.0,0.0,0.4));

		// <NO_DOC>
		HX_RESULT ForceModelCreateMagnet(ForceModelInfo** retval, iHapticDeviceID deviceID, Vec3f origin = Vec3f(0.0,0.0,0.0), float magnetForce = 0.6f, float snapDistance = 5.5f, float effectDistance = 10.0f);
		// </NO_DOC>
		
		// A ForceModel simulating a magnetic force. The force is weakest at the effectDistance and origin, and 
		// increases linearly to the origoSnapDistance. 
		// The relation between magnetic force and snap distance determines how distinct the "snap" force is.
		// Please note that HaptX will override any values that might cause instability of the touch controller.
		// Parameters: origin - The position of the magnet.
		// deviceID - The ID of the device.
		// magnetForce - The strength of the magnet.
		// origoSnapDistance - The distance from origo where the force output equals magnetForce.
		// effectDistance - The effect distance from origo where force output is zero.
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateMagnet(ForceModelMagnetInfo** retval, iHapticDeviceID deviceID, Vec3f origin = Vec3f(0.0,0.0,0.0), float magnetForce = 0.6f, float origoSnapDistance = 5.5f, float effectDistance = 10.0f);
		
		// <NO_DOC>
		HX_RESULT ForceModelCreateRepellingMagnet(ForceModelInfo** retval, iHapticDeviceID deviceID, Vec3f origin = Vec3f(0.0,0.0,0.0), float magnetForce = 0.6f, float origoSnapDistance = 1.5f, float effectDistance = 10.0f);
		// </NO_DOC>

		// A ForceModel simulating a radial repelling magnetic force which pushes the device from the origo. 
		// The force output is zero at the effectDistance
		// Parameters: origin - The position of the magnet.
		// deviceID - The ID of the device.
		// magnetForce - The strength of the magnet.
		// snapDistance - The snap distance.
		// effectDistance - The effect distance.
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateRepellingMagnet(ForceModelRepellingMagnetInfo** retval, iHapticDeviceID deviceID, Vec3f origin = Vec3f(0.0,0.0,0.0), float magnetForce = 0.6f, float snapDistance = 1.5f, float effectDistance = 10.0f);
		
		// <NO_DOC>
		HX_RESULT ForceModelCreateViscosity(ForceModelInfo** retval, iHapticDeviceID deviceID, Vec3f velocity = Vec3f(0.0,0.0,0.0), float mass = 4.0f, float stiffness = 0.5f, float damping = 50.0f);
		// </NO_DOC>

		// The ForceModel simulates that a mass is connected to the Touch Controller through a spring. 
		// The velocity of the mass is stored internally when the ForceModel is stopped, and used again 
		// when it is started. This behaviour can be changed by setting the velocity to (0,0,0) when 
		// the ForceModel is started.
		// Parameters: velocity - The velocity of the mass.
		// deviceID - The ID of the device.
		// mass - The mass of the object.
		// stiffness - The stiffness of the spring.
		// damping - The damping of the spring gives friction characteristics to the ForceModel. Recomended values 0 - 200
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateViscosity(ForceModelViscosityInfo** retval, iHapticDeviceID deviceID, Vec3f velocity = Vec3f(0.0,0.0,0.0), float mass = 4.0f, float stiffness = 0.5f, float damping = 50.0f);
		
		// <NO_DOC>
		HX_RESULT ForceModelCreateSpring(ForceModelInfo** retval, iHapticDeviceID deviceID, Vec3f origin = Vec3f(0.0,0.0,0.0), float stiffness = 0.4f, float damping = 0.0f);
		// </NO_DOC>

		// A ForceModel simulating a radial spring which pulls the device with a linear force towards the origo.
		// Parameters: origin - The origin of the spring force.
		// deviceID - The ID of the device.
		// stiffness - The stiffness of the spring.
		// damping - The damping of the spring gives friction characteristics to the ForceModel. Recomended values 0 - 200
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateSpring(ForceModelSpringInfo** retval, iHapticDeviceID deviceID, Vec3f origin = Vec3f(0.0,0.0,0.0), float stiffness = 0.4f, float damping = 0.0f);

		// <NO_DOC>
		HX_RESULT ForceModelCreateRigidBodyInteraction(ForceModelInfo** retval, iHapticDeviceID deviceID, int max_contacts);
		// </NO_DOC>

		// This ForceModel is used to create a realistic feel while interacting with physical object in a game environment.
		// Parameters: deviceID - The ID of the device.
		// max_contacts - maximum contact points
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateRigidBodyInteraction(ForceModelRBIInfo** retval, iHapticDeviceID deviceID, int max_contacts);

		// <NO_DOC>
		HX_RESULT ForceModelCreateNavigation(ForceModelInfo** retval, iHapticDeviceID deviceID, Vec3f origin = Vec3f(0,0,0), float springRadius = 15.0f, float springStiffness = 0.05f, float buttonStrokeDistance = 1.0f, float buttonForce = 0.05);
		// </NO_DOC>

		// This ForceModel can be visualized as two cylinders that is aligned around the depth axis. 
		// The outer cylinder acts a spring constraint and pulls the proxy towards the origin when springRadius is exceeded. 
		// The inner cylinder can be used to create a button like haptic effect before the spring effect is reached which 
		// creates a "button" force when crossing the border to the springRadius. The inner cylinder radius is calculated 
		// from the springRadius and hence decreases the inner volume inside the cylinders with zero force output.
		// Parameters: deviceID - The ID of the device.
		// origin - The origin in world coordinates
		// springRadius - Distance from origin where the spring volume starts (the outer cylinder).
		// springStiffness - The stiffness of the spring, see springRadius.
		// buttonStrokeDistance - Distance from springRadius towards the origin which determines the stroke of the button (inner cylinder), see buttonForce. Needs to be balanced with buttonForce, 0 means no button effect.
		// buttonForce - Force that needs to be exceeded to reach springRadius. Needs to be balanced with buttonStrokeDistance, 0 means no button effect.
		// retval - An info object which can be used to change parameters of the ForceModel.
		HX_RESULT ForceModelCreateNavigation(ForceModelNavigationInfo** retval, iHapticDeviceID deviceID, Vec3f origin= Vec3f(0,0,0), float springRadius = 15.0f, float springStiffness = 0.05f, float buttonStrokeDistance = 1.0f, float buttonForce = 0.05);

		// Destroy the specified ForceModel.
		// Parameters: forceModelID - The ID of the ForceModel.
		HX_RESULT ForceModelDestroy(iForceModelID forceModelID);

		// Start playing a ForceModel on a specific device.
		// Parameters: forceModelID - The ID of the ForceModel.
		HX_RESULT ForceModelStart(iForceModelID fmID);

		// Stop playing a ForceModel on a specific device.
		// Parameters: forceModelID - The ID of the ForceModel.
		HX_RESULT ForceModelStop(iForceModelID fmID);

		// Returns whether the ForceModel is currently playing on the specified device or not. 
		// Parameters: forceModelID - The ID of the ForceModel.
		// retval - A boolean which is true if the ForceModel is active on the specific device, otherwise false.
		HX_RESULT ForceModelIsStarted(bool& retval, iForceModelID fmID);


		// ########################## Shape Functions ##########################

		// Create a new HaptX Shape from a vertex list. The function assumes that the three first
		// vertices constitutes the first triangle and that the next three are triangle 2 etc. 
		// The HaptX Shape will not have any texture coordinates and HaptX Surfaces which utilise 
		// texture data to simulate a haptic effect will be treated as smooth surfaces. 
		// Parameters: retval - The ID of the shape.
		// vertices - A vertex list. 
		// numberofVertices - The number of vertices in the vertex list.
		// surfaceID - The ID of the surface that this shape should use. 
		// faceNormals - The normals. If NULL then normals are calculated.
		// oneSided - If the triangles should be one sided or two sided.
		// flipNormals - Flips the normals (only relevant for one sided shapes).
		// maxTrisPerNode - Affects the HaptX internal optimization algorithms. The default value is 50.  
		// HapticCache - Create a shape from a previously stored cached version. See ShapeGetHapticCache.
		HX_RESULT ShapeCreate(iHapticShapeID& retval, const Vec3f* vertices, size_t numberOfVertices, iSurfaceID surfaceID, const Vec3f* faceNormals = NULL, bool oneSided = true, bool flipNormals = false, unsigned char maxTrisPerNode = 50, void* HapticCache = NULL);

		// Creates a new HaptX Shape from a vertex and an index list. The HaptX Shape will not 
		// have any texture coordinates and HaptX Surfaces which utilise texture data to 
		// simulate a haptic effect will be treated as smooth surfaces. 
		// Parameters: retval - The ID of the shape. 
		// vertices - A vertex list. 
		// numberOfVertices - The number of vertices in the vertex list. 
		// indices - An index list. 
		// numberofIndices - The number of indices in the index list. 
		// surfaceID - The ID of the surface that this shape should use. 
		// faceNormals - The normals. If NULL then normals are calculated.
		// oneSided - If the triangles should be one sided or two sided.
		// flipNormals - Flips the normals (only relevant for one sided shapes).
		// maxTrisPerNode - Affects the HaptX internal optimization algorithms. The default value is 50.
		// HapticCache - Create a shape from a previously stored cached version. See ShapeGetHapticCache.
		HX_RESULT ShapeCreate(iHapticShapeID& retval, const Vec3f* vertices, size_t numberOfVertices, const int* indices, size_t numberofIndices, iSurfaceID surfaceID, const Vec3f* faceNormals = NULL, bool oneSided = true, bool flipNormals = false, unsigned char maxTrisPerNode = 50, void* HapticCache = NULL);


		// Create a HaptX Shape from vertex, index and texture coordinate lists. 
		//! 
		//! HX_BOTTOM_LEFT							HX_TOP_LEFT
		//! Y										_______X	
		//! |										|
		//! |										|
		//! |_______X								|Y
		//! 
		// Parameters: retval - The ID of the shape.
		// vertices - A vertex list. 
		// numberofVertices - The number of vertices in the vertex list. 
		// indices - An index list. 
		// numberofIndices - The number of indices in the index list. 
		// textureCoords - A texture coordinate list.
		// numberofTextureCoords - The number of texture coordinates in the list.
		// surfaceID - The ID of the surface that this shape should use. 
		// faceNormals - The normals. If NULL then normals are calculated.
		// oneSided - If the triangles should be one sided or two sided.
		// flipNormals - Flips the normals (only relevant for one sided shapes).
		// maxTrisPerNode - Affects the HaptX internal optimization algorithms. The default value is 50. 
		// textureOrigin - Where the origin of the texture is placed. Use either HX_BOTTOM_LEFT or HX_TOP_LEFT.
		// HapticCache - Create a shape from a previously stored cached version. See ShapeGetHapticCache.
		HX_RESULT ShapeCreate(iHapticShapeID& retval, const Vec3f* vertices, size_t numberOfVertices, const int* indices, size_t numberofIndices, const Vec3f* textureCoords, size_t numberofTextureCoords, const int* textureIndices, size_t numberofTextureIndices, iSurfaceID surfaceID, const Vec3f* faceNormals = NULL, bool oneSided = true, bool flipNormals = false, unsigned char maxTrisPerNode = 50, textureCoordinates_e textureOrigin = HX_BOTTOM_LEFT, void* HapticCache = NULL);

		// Create a box HaptX Shape.
		// Parameters; retval - The ID of the shape.
		// xl, yl, zl - The size of the box.
		// surfaceID - The ID of the surface that this shape should use.
		// oneSided - If the triangles should be one sided or two sided.
		// flipNormals - Flips the normals (only relevant for one sided shapes).
		HX_RESULT ShapeCreateBox(iHapticShapeID& retval, float xl, float yl, float zl, iSurfaceID surfaceID, bool oneSided = true, bool flipNormals = false);
		
		// Create a sphere HaptX Shape.
		// Parameters; retval - The ID of the shape.
		// radius - The radius of the sphere.
		// surfaceID - The ID of the surface that this shape should use.
		// segments - Specify how tessellated the sphere should be.
		// oneSided - If the triangles should be one sided or two sided.
		// flipNormals - Flips the normals (only relevant for one sided shapes).
		HX_RESULT ShapeCreateSphere(iHapticShapeID& retval, float radius, iSurfaceID surfaceID, int segments = 20, bool oneSided = true, bool flipNormals = false);
		
		// Create a cone HaptX Shape.
		// Parameters; retval - The ID of the shape.
		// radius - The radius of the cone.
		// height - The length of the cone.
		// surfaceID - The ID of the surface that this shape should use.
		// segments - Specify how tessellated the cone should be.
		// oneSided - If the triangles should be one sided or two sided.
		// flipNormals - Flips the normals (only relevant for one sided shapes).
		HX_RESULT ShapeCreateCone(iHapticShapeID& retval, float radius, float height, iSurfaceID surfaceID, int segments = 20, bool oneSided = true, bool flipNormals = false);
		
		// Create a tube HaptX Shape.
		// Parameters; retval - The ID of the shape.
		// bottomInnerRadius - The bottom inner radius of the tube.
		// bottomOuterRadius - The bottom outer radius of the tube.
		// topInnerRadius - The top inner radius of the tube.
		// topOuterRadius - The top outer radius of the tube.
		// height - The length of the tube.
		// surfaceID - The ID of the surface that this shape should use.
		// segments - Specify how tessellated the tube should be.
		// oneSided - If the triangles should be one sided or two sided.
		// flipNormals - Flips the normals (only relevant for one sided shapes).
		HX_RESULT ShapeCreateTube(iHapticShapeID& retval, float bottomInnerRadius, float bottomOuterRadius, float topInnerRadius, float topOuterRadius, float height, iSurfaceID surfaceID, int segments = 24, bool oneSided = true, bool flipNormals = false);
		
		// Create a cylinder HaptX Shape.
		// Parameters; retval - The ID of the shape.
		// bottomRadius - The bottom radius of the cylinder.
		// topRadius - The top radius of the cylinder.
		// height - The length of the cylinder.
		// surfaceID - The ID of the surface that this shape should use.
		// segments - Specify how tessellated the cylinder should be.
		// oneSided - If the triangles should be one sided or two sided.
		// flipNormals - Flips the normals (only relevant for one sided shapes).
		HX_RESULT ShapeCreateCylinder(iHapticShapeID& retval, float bottomRadius, float topRadius, float height, iSurfaceID surfaceID, int segments = 24, bool oneSided = true, bool flipNormals = false);

		// Clones a HaptX shape
		// Parameters: shapeID - The shape to clone
		// surface - The surface that will be attached to the cloned shape
		// retval - The cloned shape
		HX_RESULT ShapeClone(iHapticShapeID& retval, iHapticShapeID shapeID, iSurfaceID surface);

		// Destroy a shape.
		// Parameters: shapeID - The ID of the shape.
		HX_RESULT ShapeDestroy(iHapticShapeID shapeID);

		// Enable a shape to be rendered on all devices.
		// Parameters: shapeID - The ID of the shape.
		HX_RESULT ShapeEnable(iHapticShapeID shapeID);
		
		// Enable a shape to be rendered on a specific device.
		// Parameters: shapeID - The ID of the shape. 
		// deviceID - The ID of the device.
		HX_RESULT ShapeEnable(iHapticShapeID shapeID, iHapticDeviceID deviceID);
		
		// Disable a shape from HaptX rendering on all devices.
		// Parameters: shapeID - The ID of the shape.
		HX_RESULT ShapeDisable(iHapticShapeID shapeID);
		
		// Disable a shape from HaptX rendering on a specific device.
		// Parameters: shapeID - The ID of the shape. 
		// deviceID - The ID of the device.
		HX_RESULT ShapeDisable(iHapticShapeID shapeID, iHapticDeviceID deviceID);
		
		// Returns whether the shape is enabled for rendering on the specified device or not. 
		// Parameters: shapeID - The ID of the shape. 
		// deviceID - The ID of the device.
		// retval - The return value.
		HX_RESULT ShapeIsEnabled(bool& retval, iHapticShapeID shapeID, iHapticDeviceID deviceID);
		
		// Set a surface to a shape.
		// Parameters: shapeID - The ID of the shape. 
		// surfaceID - The ID of the surface.
		HX_RESULT ShapeSetSurface(iHapticShapeID shapeID, iSurfaceID surfaceID);

		// Set custom data for a shape.
		// Parameters: shapeID - The ID of the shape.
		// data - a pointer to the data
		HX_RESULT ShapeSetCustomData(iHapticShapeID shapeID, void* data);

		// Get the custom data that has been set for the shape.
		// Parameters: shapeID - The ID of the shape.
		// data - a pointer to the data
		HX_RESULT ShapeGetCustomData(iHapticShapeID shapeID, void** data);

		// Set the transform of a shape. Use HaptXSetLeftHanded or HaptXSetRightHanded
		// to specify how HaptX should interpret the transformation data.
		// Parameters: shapeID - The ID of the shape. 
		// matrix - The transformation matrix.
		HX_RESULT ShapeSetTransform(iHapticShapeID shapeID, HaptX::Vectors::Matrix4f matrix);

		// Set the transform of a shape. Use HaptXSetLeftHanded or HaptXSetRightHanded
		// to specify how HaptX should interpret the transformation data.
		// Parameters: shapeID - The ID of the shape. 
		// m00 through m33 specifies the transformation matrix.
		HX_RESULT ShapeSetTransform(iHapticShapeID shapeID,
									float m00, float m01, float m02, float m03,
									float m10, float m11, float m12, float m13,
									float m20, float m21, float m22, float m23,
									float m30, float m31, float m32, float m33);

		// Set the transform of a shape. Use HaptXSetLeftHanded or HaptXSetRightHanded
		// to specify how HaptX should interpret the transformation data.
		// Parameters: shapeID - The ID of the shape. 
		// m - The matrix float array.
		HX_RESULT ShapeSetTransform(iHapticShapeID shapeID, float m[16]);

		// Set the transform of a shape. Use HaptXSetLeftHanded or HaptXSetRightHanded
		// to specify how HaptX should interpret the transformation data.
		// Parameters: shapeID - The ID of the shape. 
		// posX - The position in X. 
		// posY - The position in Y.
		// posZ - The position in Z. 
		// rotQX - The rotation (quaternion) X. 
		// rotQY - The rotation (quaternion) Y.
		// rotQZ - The rotation (quaternion) Z. 
		// rotQW - The rotation (quaternion) W. 
		// scaleX - The scale in X.
		// scaleY - The scale in Y. 
		// scaleZ - The scale in Z.
		HX_RESULT ShapeSetTransformQ(iHapticShapeID shapeID,
									 float posX, float posY, float posZ, 
									 float rotQX, float rotQY, float rotQZ, float rotQW,
									 float scaleX, float scaleY, float scaleZ);

		// Set the transform of a shape. Use HaptXSetLeftHanded or HaptXSetRightHanded
		// to specify how HaptX should interpret the transformation data.
		// Parameters: shapeID - The ID of the shape. 
		// posX - The position in X. 
		// posY - The position in Y.
		// posZ - The position in Z. 
		// rotAxisX - The rotation axis X. 
		// rotAxisY - The rotation axis Y.
		// rotAxisZ - The rotation axis Z. 
		// rotAngleRad - The rotation angle in radians.
		// scaleX - The scale in X. 
		// scaleY - The scale in Y. 
		// scaleZ - The scale in Z.
		HX_RESULT ShapeSetTransformR(iHapticShapeID shapeID,
									 float posX, float posY, float posZ, 
									 float rotAxisX, float rotAxisY, float rotAxisZ, float rotAngleRad,
									 float scaleX, float scaleY, float scaleZ);

		// Get the transformation matrix.
		// NOTE! The returned matrix is dependent on the current coordinate system, as 
		// specified with the HaptXSetLeftHanded or HaptXSetRightHanded methods. 
		// Parameters: shapeID - The ID of the HapticShape. 
		// retval - The transformation matrix.
		HX_RESULT ShapeGetTransform(Matrix4f& retval, iHapticShapeID shapeID);
		 
		// Returns the total force applied on the HaptX Shape by all devices in the scene. 
		// Parameters: shapeID - The ID of the shape. 
		// retval - The total force applied on the shape by all devices.
		HX_RESULT ShapeGetAppliedForce(Vec3f& retval, iHapticShapeID shapeID);
		
		// Returns the force applied on the HaptX Shape by the specified device. 
		// Parameters: deviceID - The ID of the device. 
		// shapeID - The ID of the shape.
		// retval - The force applied on the shape by the device.
		HX_RESULT ShapeGetAppliedForce(Vec3f& retval, iHapticDeviceID deviceID, iHapticShapeID shapeID);

		// Returns the total torque applied on the HaptX Shape by all devices in the scene. 
		// Parameters: shapeID - The ID of the shape. 
		// retval - The total torque applied on the shape by all devices.
		HX_RESULT ShapeGetAppliedTorque(Vec3f& retval, iHapticShapeID shapeID);
		
		// Returns the torque applied on the HaptX Shape by the specific device. 
		// Parameters: deviceID - The ID of the device. 
		// shapeID - The ID of the shape.
		// retval - The torque applied on the shape by the device.
		HX_RESULT ShapeGetAppliedTorque(Vec3f& retval, iHapticDeviceID deviceID, iHapticShapeID shapeID);

		// Get the number of devices that are currently in contact with the shape.
		// Parameters: shapeID - The ID of the shape.
		// retval - The number of devices currently in contact with the shape.
		HX_RESULT ShapeGetNumDevicesInContact(int& retval, iHapticShapeID shapeID);

		// Returns true if the specified device is in contact with the shape.
		// Parameters: deviceID - The ID of the device. 
		// shapeID - The ID of the shape.
		// retval - True if the device is in contact with the shape.
		HX_RESULT ShapeIsDeviceInContact(bool& retval, iHapticDeviceID deviceID, iHapticShapeID shapeID);

		// Get the IDs of the triangles that the specified device currently is in contact with. 
		// The function returns a maximum number of three triangle IDs. 
		// If the device is not in contact with any triangles the ID's will be -1.
		// If the device is in contact with one triangle the function will return 
		// its ID in retval1. retval2 and retval3 will be -1, etc. 
		// Parameters: deviceID - The ID of the device.
		// shapeID - The ID of the shape.
		// retval1 - The ID of the first triangle that is in contact with the device.
		// retval2 - The ID of the second triangle that is in contact with the device.
		// retval3 - The ID of the third triangle that is in contact with the device.
		HX_RESULT ShapeGetContactTriangleIDs(int& retval1, int& retval2, int& retval3, iHapticDeviceID deviceID, iHapticShapeID shapeID);

		// Calculate the size (in bytes) needed to store a binary version of the HaptX Shape.
		// See ShapeGetHapticCache for more details.
		// Parameters: retval - The size (in bytes) needed to store a binary version of the HaptX Shape.
		// shapeID - The ID of the shape.
		HX_RESULT ShapeGetBinaryCacheSize(long& retval, iHapticShapeID shapeID);

		// Fill a buffer with a binary version of the HaptX Shape. The creation of complex 
		// HaptX Shapes using a precalculated cached version is significantly faster than 
		// normal shape creation. This function can be used to get the data from the cache that, 
		// if stored in a file, can be used in calls to ShapeCreate to reduce load time.
		// Parameters: Buffer - A pointer to allocated memory that will be filled with the cached HaptX Shape.
		// MaxVal - The size (in bytes) of the Buffer. Use ShapeGetHapticCacheSize to calculate the size needed.
		// ActVal - Returns the size (in bytes) actually used by shapeGetHapticCache
		// shapeID - The ID of the shape.
		HX_RESULT ShapeGetBinaryCache(void* Buffer, long MaxVal, long& ActVal, iHapticShapeID shapeID);	
		

		// ########################## Surface Functions ##########################

		// <NO_DOC>
		HX_RESULT SurfaceCreateSimple(SurfaceInfo** retval);
		// </NO_DOC>

		// Create a SimpleSurface. 
		// Parameters: retval - An info object which can be used to adjust the parameters of the surface.
		HX_RESULT SurfaceCreateSimple(SimpleSurfaceInfo** retval);
		
		// <NO_DOC>
		HX_RESULT SurfaceCreateFrictional(SurfaceInfo** retval);
		// </NO_DOC>
 
		// Create a FrictionalSurface. 
		// Parameters: retval - An info object which can be used to adjust the parameters of the surface.
		HX_RESULT SurfaceCreateFrictional(FrictionalSurfaceInfo** retval);
		 
		// <NO_DOC>
		HX_RESULT SurfaceCreateVibrating(SurfaceInfo** retval);
		// </NO_DOC>

		// Create a VibratingSurface. 
		// Parameters: retval - An info object which can be used to adjust the parameters of the surface.
		HX_RESULT SurfaceCreateVibrating(VibratingSurfaceInfo** retval);

		// <NO_DOC>
		HX_RESULT SurfaceCreateRough(SurfaceInfo **retval);
		// </NO_DOC>

		// Create a RoughSurface. 
		// Parameters: retval - An info object which can be used to adjust the parameters of the surface.
		HX_RESULT SurfaceCreateRough(RoughSurfaceInfo** retval);
		
		// <NO_DOC>
		HX_RESULT SurfaceCreateStickyFrictional(SurfaceInfo** retval);
		// </NO_DOC>

		// Create a StickySurface with friction. 
		// Parameters: retval - An info object which can be used to adjust the parameters of the surface.
		HX_RESULT SurfaceCreateStickyFrictional(StickyFrictionalSurfaceInfo** retval);
		
		// <NO_DOC>
		HX_RESULT SurfaceCreateStickyHeightmapped(SurfaceInfo** retval);
		// </NO_DOC>

		// HaptX PRO exclusive. Create a StickySurface with a heightmap. 
		// Parameters: retval - An info object which can be used to adjust the parameters of the surface.
		HX_RESULT SurfaceCreateStickyHeightmapped(StickyHeightmapSurfaceInfo** retval);
			 
		// <NO_DOC>
		HX_RESULT SurfaceCreateHeightmapped(SurfaceInfo** retval, HapticTexture* texture);
		// </NO_DOC>

		// Create a HeightmapSurface. The surface uses the texture values
		// to calculate the height at a specific position on the surface. 
		// If the texture has RGB or RGBA values only the blue channel is used.
		// Parameters: retval - An info object which can be used to adjust the parameters of the surface.
		// texture - The texture to use for HaptX Heightmapping.
		HX_RESULT SurfaceCreateHeightmapped(HeightmapSurfaceInfo** retval, HapticTexture* texture);

		// <NO_DOC>
		HX_RESULT SurfaceCreateFrictionalButton(SurfaceInfo** retval, bool* activeRef);
		// </NO_DOC>

		// Create a ButtonSurface with friction. 
		// Parameters: retval - An info object which can be used to adjust the parameters of the surface.
		// activeRef - A reference to a boolean which is set to true when the button is pressed.
		HX_RESULT SurfaceCreateFrictionalButton(ButtonFrictionalSurfaceInfo** retval, bool* activeRef);

		// <NO_DOC>
		HX_RESULT SurfaceCreateFrictionmapped(SurfaceInfo** retval, HapticTexture* texture);
		// </NO_DOC>
 
		// HaptX PRO exclusive. Create a surface which uses greyscale texture values to map friction.
		// Black pixel - maximum friction. White pixel - minimum friction. 
		// texture - The texture to be used as a HaptX Friction Map.
		HX_RESULT SurfaceCreateFrictionmapped(ImageSurfaceInfo** retval, HapticTexture* texture);
		
		// Destroy a surface. 
		// Parameters: surfaceID - The ID of the surface.
		HX_RESULT SurfaceDestroy(iSurfaceID surfaceID);
		
		// Get the surface type. 
		// Parameters: retval - the surface type. 
		// surfaceID - The ID of the surface.
		HX_RESULT SurfaceGetType(surfaceType_e& retval, iSurfaceID surfaceID);
	
		// Get the info object of a surface. 
		// Parameters: retval - The info object. 
		// surfaceID - The ID of the surface.
		HX_RESULT SurfaceGetInfo(SurfaceInfo** retval, iSurfaceID surfaceID);

		// Returns a boolean which is true if the device is in contact with the surface.
		// If multiple shapes use the same surface this function will return true whenever 
		// any one of the shapes with the surface applied is touched.
		// Parameters: deviceID - The ID of the device. 
		// surfaceID - The ID of the surface.
		// retval - True if the device is in contact with the surface.
		HX_RESULT SurfaceIsDeviceInContact(bool& retval, iHapticDeviceID deviceID, iSurfaceID surfaceID);


		// ########################## Other Functions ##########################
	
		// Set the polygon reduction distance. The polygon reduction distance specifies within what range polygons will be processed.
		// v - reduction distance [0.001, 0.02] (low value = better performance but more collision errors, high value = lower performance but less collision errors)
		HX_RESULT HaptXSetPolygonReductionDistance(mgFloat v);

		// Get the polygon reduction distance. The polygon reduction distance specifies within what range polygons will be processed.
		// v - reduction distance
		HX_RESULT HaptXGetPolygonReductionDistance(mgFloat& v);

		// Create a navigation camera
		// Parameters: cam - The camera object.
		// deviceID - The ID of the device.
		// type - The camera type that we want to create
		HX_RESULT HaptXCreateCamera(ICameraNavigation** cam, const iHapticDeviceID& deviceID, cameraNavigationType_e type);

		// Destroy a navigation camera
		// Parameters: cam - A pointer to a camera
		HX_RESULT HaptXDestroyCamera(ICameraNavigation* cam);

		// Disable all Surface Haptics in the application. No HaptX Shapes are used in 
		// the rendering calculations. Surface Haptics is enabled by default. 
		HX_RESULT HaptXDisableShapeRendering();

		// Enable all Surface Haptics in the application. All HaptX Shapes that are enabled 
		// are used in the rendering calculations. Surface Haptics is enabled by default.
		HX_RESULT HaptXEnableShapeRendering();

		// Disable all ForceModel Haptics in the application. No ForceModels are used in 
		// the rendering calculations. ForceModel Haptics is enabled by default.
		HX_RESULT HaptXDisableForceModelRendering();

		// Enable all ForceModel Haptics in the application. All ForceModels that are started 
		// are used in the rendering calculations. ForceModel Haptics is enabled by default.
		HX_RESULT HaptXEnableForceModelRendering();

		// Disable all HaptX force rendering in the application, both ForceModel Haptics 
		// and Surface Haptics. HaptX rendering is enabled by default.
		HX_RESULT HaptXDisableAllRendering();
		
		// Enable all HaptX force rendering in the application, both ForceModel Haptics 
		// and Surface Haptics. HaptX rendering is enabled by default.
		HX_RESULT HaptXEnableAllRendering();

		// Configure HaptX to use a left-handed coordinate system, see coordinateSystems.
		//! The HaptX right-handed and left-handed coordinate systems:
		//! 
		//!    right			   left
		//! 	y|				   y| z
		//! 	 |___x				|/__x
		//!    z/
		HX_RESULT HaptXSetLeftHanded();
		 
		// Configure HaptX to use a right-handed coordinate system, see coordinateSystems.
		//! see HaptXSetLeftHanded
		HX_RESULT HaptXSetRightHanded();

		// HaptXHookMessages is used to route messages from within the HaptX 
		// rendering loop to the host program. 
		// Parameters:
		// MinimumSeverity can be called with these possible values:
		// (Please note that when called with one severity level all messages of a 
		// higher severity level will be sent as well. I.e if registered to receive 
		// warnings, error messages will be sent as well.) 
		// HAPTX_ERR_MSG - Used to catch any errors from within the HaptX loop.
		// HAPTX_WARN_MSG - Used to catch warnings.
		// HAPTX_INFO_MSG - Used to catch information messages.
		// HAPTX_DBG_MSG - Used to catch debug messages.
		// HAPTX_HAPTIC_LOC - Used to log the current force and position of the device.
		// Please note that this information is sent every HaptX loop and will therefore 
		// be sent 1000 times each second. Please use it with caution.
		//
		// Example: The CallbackFunc should be defined like this:
		// void CallbackFunc(int Severity, std::wstring Message);
		HX_RESULT HaptXHookMessages(int MinimumSeverity, pfnMessageCallBack CallbackFunc);

		// Initialize HaptX. This function must be called before any other HaptX functions.
		HX_RESULT HaptXInit();
		
		// Initialize HaptX. This function must be called before any other HaptX functions.
		HX_RESULT HaptXInit(wchar_t* Company, wchar_t* Product,wchar_t* Version, wchar_t* Platform, wchar_t* LicenseKey);
		
		// ########################## Mouse Driver Functions ##########################
		
		// Start the HaptX Mouse Driver. HaptX PRO exclusive. The HaptX Mouse Driver must 
		// be installed on the computer prior to using this method.
		// Parameters: speed - Mouse pointer speed relative the device movement. Recomended values 0.5 - 2.0
		HX_RESULT MouseStartDriver(float SpeedCoef = 1.0f);
		
		// Update position of the mouse. HaptX PRO exclusive.
		// This function should be called every graphics loop when the mouse driver is active.
		// Please note that the dy parameter for instance shall be seen as any movement of the 
		// touch controller since the last graphics loop that should move the mouse along the Y-axis.
		// For example: If you have defined a HaptX plane in the X,Z-Plane and use that plane as a 
		// virtual desktop, you should use changes along the Z-axis, not the Y-axis, as your dy parameter.
		// Parameters: dx - Change along the X-axis since last graphics loop.
		// dy - Change along the Y-axis since last graphics loop.
		// Buttons - Bitmask used to report the state of the buttons on the touch controller. Bit 0 = Button 0, Bit 1 = Button 1 and so forth.
		HX_RESULT MouseUpdatePosition(float dx, float dy, int Buttons);

		// Stop the HaptX Mouse Driver. HaptX PRO exclusive. 
		HX_RESULT MouseStopDriver();

	private:
		
		//! The HaptX right-handed and left-handed coordinate systems:
		//! 
		//!    right			   left
		//! 	y|				   y| z
		//! 	 |___x				|/__x
		//!    z/
		bool m_useLeftHanded;			// Used to specify if a left-handed or right-handed coordinate system is used. 
	};
}

#endif
