
//! File: <PhantomDevice.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_PHANTOMDEVICE_H_
#define _HAPTX_PHANTOMDEVICE_H_

#include "HXSolver.h"
#include "HapticsDevice.h"

typedef unsigned int HHD;
typedef unsigned long HDSchedulerHandle;

namespace HaptX
{
	// <NO_DOC>
	HAPTX_VALUE_ERROR(const int, CouldNotInitPhantom);
	HAPTX_VALUE_ERROR(const std::wstring, CouldntFindIniSettings);
	HAPTX_VALUE_ERROR(const int, CouldNotInitPhantomEvent);
	HAPTX_VALUE_ERROR(const mgFloat, BadForceScalingValue);
	HAPTX_VALUE_ERROR(const std::wstring, IncompatibleHdDll);
	HAPTX_VALUE_ERROR(const std::wstring, CouldNotFindHdDll); 
	// </NO_DOC>

	// Contains functionality for interfacing with Sensable hd.dll.
	namespace HDDriver 
	{
		// A PhantomDevice is a HapticsDevice that interfaces to a Sensable PHANTOM.
		// 
		// There is one transformation matrix for the positional calibration and one
		// for the rotational calibration.
		//
		// The error_state field (inherited from TrackingDevice) is set to "SLEEPING" 
		// when the force rendering is switched off temporarily, and "WAKING" for the 
		// brief period thereafter when the forces are faded back in. During normal 
		// operation, error_state is empty. When forces are being saturated in relation 
		// to the max_force setting, the error_state is set to "MAXFORCE".
		class HAPTX_SOLVER_SPEC PhantomDevice : public HapticsDevice
		{
		public:
			PhantomDevice(std::wstring identifier = L"Default PHANToM");	// Constructor
			virtual ~PhantomDevice();										// Destructor
			
			virtual void		InitDevice();				// Virtual function to be overridden by PhantomDevice.
			virtual HX_RESULT	StartHapticsLoop();			// Virtual function to be overridden by PhantomDevice.
			virtual void		Reset();					// Virtual function to be overridden by PhantomDevice.
			virtual void		StopHapticsLoop();			// Virtual function to be overridden by PhantomDevice.
			virtual void		ReleaseDevice();            // Virtual function to be overridden by PhantomDevice.
			virtual int			TryInit();

			virtual void		UpdateDevice();				// UpdateDevice is called after each main loop to allow the PhantomDevice to update class members used in the HaptX loop. 
		
		protected:
            virtual void		YieldHapticLoop();			// Virtual function to be overridden by PhantomDevice.
            virtual bool		TakeoverHapticLoop();       // virtual function to be overridden by PhantomDevice.

		public:
			// The HaptX loop can be run in "high performance mode" or normal mode. The high 
			// performance loop will run at dynamic rates, typically around 4 to 5kHz. 
			// The normal HaptX loop will attempt to maintain a constant update rate of 1 kHz. 
			// A PHANTOM will run in high performance mode if its m_highPerfMode is set to true. 
			bool				m_highPerfMode;

			// Determines the maximum possible force that will be sent to the PHANTOM. If this 
			// value is greater than the manufacturer specified maximum for the particular 
			// PHANTOM model, the lower value is used. The default value is 1e10, which means 
			// that the manufacturer maximum is used by default.
			mgFloat				m_maxForce;

			// May be used to switch of the use of the manufacturer maximum force safety flag. 
			// m_ignoreManufMaxForce should only be set by advanced users and at their own risk. 
			bool				m_ignoreManufMaxForce;

			std::wstring		m_initString;				// Used to initialise the PHANTOM (the default is "Default PHANToM").
			Vec3f				m_peakForce;				// The highest positive force generated during a HaptX render loop for each of the three axes (x,y,z). 
			std::string			m_driverVersion;			// The driver_version field is the version string reported by the PHANTOM driver dll.
			std::wstring		m_model;
			mgFloat				m_reduce;
			bool				m_button2;
			mgFloat				m_angleCorrection;
			bool				m_invertPitch;

			mgFloat				rt_oldForce[3];				// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_force[3];				// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_maxForce;				// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_manufMaxForce;			// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_manufMaxStiffness;		// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_forceScale;				// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_tempSaturation;			// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_drawnProxyDistance;		// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_saveProxyWeighting;		// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_time;					// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_count;					// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			bool				rt_force_saturation;		// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			bool				rt_comm_error;				// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			bool				rt_forcesDisabled;			// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			bool				rt_rampForcesUWA;			// HaptX loop variable - used to update variable values in the UpdateDevice() function.

			//! Internal variable for HaptX loop management.
			_LARGE_INTEGER last_lp_count;

		private:
			HMODULE				H;
 			HDSchedulerHandle	thHandle;
			HHD					m_phid;
			bool				m_bRunning;
		};
	} //! ~namespace HDDriver

#ifndef _WIN64	
	// Contains functionality for interfacing with Sensable PHANTOM devices, 
	// except for the Sensable Omni which requires hd.dll. 
	namespace GhostDriver 
	{
		class MotorModel;
		class ChassisModel;
		
		// see also HaptX::HDDriver::PhantomDevice 
		class HAPTX_SOLVER_SPEC PhantomDevice : public HapticsDevice 
		{
		public:
			PhantomDevice(std::wstring identifier = L"Default PHANToM");	// Constructor
			virtual ~PhantomDevice();										// Destructor

			virtual void		InitDevice();				// Virtual function to be overridden by PhantomDevice.
			virtual HX_RESULT	StartHapticsLoop();			// Virtual function to be overridden by PhantomDevice.
			virtual void		Reset();					// Virtual function to be overridden by PhantomDevice.
			virtual void		StopHapticsLoop();			// Virtual function to be overridden by PhantomDevice.

			virtual void		UpdateDevice();				// UpdateDevice is called after each main loop to allow the PhantomDevice to update class members used in the HaptX loop.
			virtual int			TryInit();

			void				WriteTemperatures();
			void				InitMotors(const std::vector<int>& motor_types);
			
		public:
			bool				m_highPerfMode;				// See also HaptX::HDDriver::PhantomDevice
			std::wstring		m_initString;				// See also HaptX::HDDriver::PhantomDevice
			Vec3f				m_peakForce;				// See also HaptX::HDDriver::PhantomDevice
			mgFloat				m_maxForce;					// See also HaptX::HDDriver::PhantomDevice
			bool				m_ignoreManufMaxForce;		// See also HaptX::HDDriver::PhantomDevice
			std::wstring		m_driverVersion;			// See also HaptX::HDDriver::PhantomDevice
			std::wstring		m_model;					// See also HaptX::HDDriver::PhantomDevice

			Vec3f				m_temperature;				// Variable set by the HaptX loop.
			Vec3f				m_encoders;					// Variable set by the HaptX loop.
			Vec3f				m_gimbalEncoders;			// Variable set by the HaptX loop.
			Vec3f				m_encodersReset;
			Vec3f				m_gimbalEncodersReset;
			mgFloat				m_roomTemperature;
			Time				m_tempWriteTime;
			std::string			m_tempFile;

			float				rt_force[3];				// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_maxForce;				// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_manufMaxForce;			// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_manufMaxStiffness;		// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_forceScale;				// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_tempSaturation;			// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_drawnProxyDistance;		// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_saveProxyWeighting;		// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_time;					// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_count;					// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_encoders[6];				// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			float				rt_encodersReset[6];		// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			bool				rt_force_saturation;		// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			bool				rt_gimbal_present;			// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			bool				rt_comm_error;				// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			bool				rt_forcesDisabled;			// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			MotorModel*			rt_motors[6];				// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			ChassisModel*		rt_chassis;					// HaptX loop variable - used to update variable values in the UpdateDevice() function.
			Matrix3f			rt_jacobianT;				// HaptX loop variable - used to update variable values in the UpdateDevice() function.

			_LARGE_INTEGER		last_lp_count;				// Internal variable for HaptX loop management.
			HANDLE				thHandle;					// Internal variable for HaptX loop management.
			HANDLE				thEvent;					// Internal variable for HaptX loop management.
			HANDLE				f;							// Internal variable for HaptX loop management.

			int					m_phid;
			int					m_running;
			Time				m_wakeTime;
			float				m_cummDt;

			HMODULE				H;

			enum sleepState_e
			{
				AWAKE	 = 0,
				WAKING	 = 1,
				SLEEPING = 2
			};

			sleepState_e m_sleepState;

			struct phantomGeometry_t
			{
				inline phantomGeometry_t(): l1(0), l2(0), r1(0), r2(0), r3(0), r4(0), r5(0), r6(0), ttm1(0) {}
			  
				mgFloat l1, l2;
				mgFloat r1, r2, r3, r4, r5, r6;
				mgFloat ttm1;
			};

			phantomGeometry_t geometry;
		};
	} //! ~ namespace GhostDriver
#endif
}

#endif
