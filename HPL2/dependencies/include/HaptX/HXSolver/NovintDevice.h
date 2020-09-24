
//! File: <NovintDevice.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_NOVINTDEVICE_H_
#define _HAPTX_NOVINTDEVICE_H_

#include "HXSolver.h"
#include "HapticsDevice.h"

namespace HaptX
{
	HAPTX_VALUE_ERROR(const int, CouldNotInitDevice);
	//HAPTX_VALUE_ERROR(const std::wstring, CouldntFindIniSettings);
	HAPTX_VALUE_ERROR(const int, CouldNotInitDeviceEvent);
	HAPTX_VALUE_ERROR(const mgFloat, NovintBadForceScalingValue);
	HAPTX_VALUE_ERROR(const std::wstring, IncompatibleDriverDll);
	HAPTX_VALUE_ERROR(const std::wstring, CouldNotFindDriverDll); 

	// A NovintDevice is a HapticsDevice that interfaces to a
	// Novint haptic device.
 
	// There is one transform matrix for the positional calibration and one
	// for the orientational calibration.

	// The error_state field (inherited from TrackingDevice)
	// is set to "SLEEPING" when the force rendering is switched off
	// temporarily and "WAKING" for the brief period thereafter when the
	// forces are faded back in. During normal operation, error_state
	// is empty. When forces are being saturated because of the max_force 
	// setting, the error_state is set to "MAXFORCE".

	class HAPTX_SOLVER_SPEC NovintDevice : public HapticsDevice
	{
	public:
		NovintDevice(std::wstring identifier = L"DEFAULT",char* inifilePath = NULL);		// constructor
		virtual ~NovintDevice();								// destructor
		
		virtual void		InitDevice();				// virtual functions to be overridden by NovintDevice.
		virtual HX_RESULT	StartHapticsLoop();			// virtual functions to be overridden by NovintDevice.
		virtual void		Reset();					// virtual functions to be overridden by NovintDevice.
		virtual void		StopHapticsLoop();			// virtual functions to be overridden by NovintDevice.
		virtual void		ReleaseDevice();            // virtual functions to be overridden by NovintDevice.
		virtual int			TryInit();

		virtual void		UpdateDevice();				// updateDevice is called after each render loop to update variables from the haptcs thread. 
	
	public:
		// The haptic loop can be run in "high performance mode" or normal mode. The high performance haptics loop will run 
		// at dynamic rates, typically around 4 to 5kHz. The normal haptics loop will attempt to maintain a constant 1kHz update
		// rate. The haptic device will run in high performance mode if its m_highPerfMode is set to true. 
		bool				m_highPerfMode;

		// Determines the maximum possible force that will be sent to the haptic device. If this value is greater than the
		// manufacturer specified maximum for the particular model in use, then the lower value is used. The default value is 1e10,
		// which means that the manufacturer maximum is used by default.
		mgFloat				m_maxForce;

		// May be used to switch of consideration of the manufacturer maximums. This flag should only
		// be set by advanced users and at their own risk. 
		bool				m_ignoreManufMaxForce;

		std::wstring		m_initString;				// Used to initialise the haptic device
		Vec3f				m_peakForce;				// The highest positive force generated during a render loop for each of the three axes (x,y,z). 
		std::string			m_driverVersion;			// The driver_version field is the version string reported	by the vendor driver dll.
		std::wstring		m_model;
		mgFloat				m_reduce;
		bool				m_button2;
		bool				m_button3;
		bool				m_button4;
		mgFloat				m_angleCorrection;
		bool				m_invertPitch;

		mgFloat				rt_oldForce[3];				// real-time variables - used to update variable values in the UpdateDevice() function.
		float				rt_force[3];				// real-time variables - used to update variable values in the UpdateDevice() function.
		float				rt_maxForce;				// real-time variables - used to update variable values in the UpdateDevice() function.
		float				rt_manufMaxForce;			// real-time variables - used to update variable values in the UpdateDevice() function.
		float				rt_manufMaxStiffness;		// real-time variables - used to update variable values in the UpdateDevice() function.
		float				rt_forceScale;				// real-time variables - used to update variable values in the UpdateDevice() function.
		float				rt_tempSaturation;			// real-time variables - used to update variable values in the UpdateDevice() function.
		float				rt_drawnProxyDistance;		// real-time variables - used to update variable values in the UpdateDevice() function.
		float				rt_saveProxyWeighting;		// real-time variables - used to update variable values in the UpdateDevice() function.
		float				rt_time;					// real-time variables - used to update variable values in the UpdateDevice() function.
		float				rt_count;					// real-time variables - used to update variable values in the UpdateDevice() function.
		bool				rt_force_saturation;		// real-time variables - used to update variable values in the UpdateDevice() function.
		bool				rt_comm_error;				// real-time variables - used to update variable values in the UpdateDevice() function.
		bool				rt_forcesDisabled;			// real-time variables - used to update variable values in the UpdateDevice() function.
		bool				rt_rampForcesUWA;			// real-time variables - used to update variable values in the UpdateDevice() function.

		// internal variables for haptics loop management:
		_LARGE_INTEGER last_lp_count;
		
		bool				rt_Active;
	
	protected:
        virtual void		YieldHapticLoop();			// virtual functions to be overridden by NovintDevice.
        virtual bool		TakeoverHapticLoop();       // virtual functions to be overridden by NovintDevice.
	
	private:
		bool				SignalTakeover();			// Signal in the global semaphore that this application wan't control of the Falcon.	
		int					InternalInitDevice();		// Low level initialization of the Falcon.
		std::string			GetDriverVersion();			// Return a string containing the version of the API used.

	private:
		HMODULE				H;
		int					thHandle;
		int					m_phid;
		bool				m_bRunning;
		char*				m_inifile;
	};
}

#endif
