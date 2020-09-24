
//! File: <Realtime.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

// Realtime is responsible for the realtime scheduled haptics
// process. This process takes rendering information from the scene-
// graph process and interpolates/extrapolates it to provide an
// update to the haptic display at a guarenteed rate of 1kHz.  We also
// provide the haptics rendering process with the position,
// orientation and button state information for the haptics
// device. Each of these communications is governed by a Lock object
// (see Lock.h).

//! NOT FOR RELEASE

#ifndef _HAPTX_REALTIME_H_
#define _HAPTX_REALTIME_H_

#include "Lock.h"
#include "ForceModel.h"
#include "HapticsManager.h"
#include "SimpleVector.h"
#include "ValueError.h"
#include "Extrapolator.h"

namespace HaptX
{  
	// errors thrown by Realtime.
	HAPTX_VALUE_ERROR(const mgFloat, MaxForceExceeded);

	class HapticsDevice;

	extern int CurrSeverity;

	class HAPTX_SOLVER_SPEC Realtime
	{
	protected:
		// Calibration stores the calibration matrix to convert from the
		// values specified by the haptic device driver and HaptX world
		// coordinates.
		class HAPTX_SOLVER_SPEC Calibration 
		{
		public:
			inline				Calibration();
			inline virtual		~Calibration();

		protected:
			friend class Realtime;
			friend class HapticsDevice;							// HapticsDevice is accessing the update function

			virtual void		Update(Matrix4f pos, Rotation orn);
			virtual void		UpdateCamera(Matrix4f cameraRot);
			
			inline Matrix4f		GetRealtime();					// get the realtime value.
			inline Matrix3f		GetRealtimeRotation();			// get the realtime rotation value.
			inline Matrix3f		GetRealtimeInverseRotation();	// get the realtime inverse rotation value.
			inline Quaternion	GetRealtimeOrnRotation();		// get the realtime stylus rotation value.
			inline void			CheckUpdated();					// check if the inputs (camera matrix or calibration matrix) has been changed

		protected:
			
			Matrix4f			m_realtimeValue;				// realtime value is accessed by the realtime process.
			Matrix4f			m_value;						// The transformation matrix.
			Matrix3f			m_realtimeRotation;				// The rotation component of the value.
			Matrix3f			m_realtimeInvRotation;			// The inverse rotation component of the value.
			Quaternion			m_realtimeOrnRotation;			// The stylus orientation calibration.
			bool				m_updated;						// true if the calibration matrix or camera matrix has changed since the last haptics frame
			Matrix4f			m_posCalib;						// The position calibration values
			Rotation			m_ornCalib;						// The rotation calibration values
		
			Matrix4f			m_camRotationMatrix;			// The camera matrix. Used if camera relative haptics is enabled.
			bool				m_cameraRelativeHaptics;			// Defines camera relative haptics is should be used.
			bool				m_useExtrapolator;				// Defines if the camera position should be extrapolated between the graphics loops.

			LinearExtrapolator<Matrix4f> m_camExtrapolator;
		};

	public:

		// State is the rendering state as updated by the haptics
		// rendering process.
		class State
		{
		public:
			inline				State();
			inline virtual		~State();

			inline void			SetOrigin(const Vec3f& o);				// set the origin.
			inline void			SetForceModels(ForceModelSum& fm);		// set the force_models.
			inline void			SetHapticsManager(HapticsManager* hm);	// set the haptics manager.

			// the evaluate function returns the force and torque according to
			// this state for the given haptic device position.
			// weighting lie in the range [0,1] and indicates the weighting
			// that the realtime process will apply to the returned values.
			// States are weighted in order to perform interpolation.
			inline ForceTorque	Evaluate(const Vec3f& pos, const Quaternion& rot, const mgFloat& weighting, const mgFloat& delta_t);

		public:
			Vec3f							m_origin;			// The position of the haptic device when the state was constructed. this acts as the origin for force calculations.
			std::auto_ptr<HapticsManager>	m_hapticsManager;	// Represents the surfaces that the proxy is currently colliding with.
			ForceModelSum					m_forceModels;		// The currently active ForceModels
		};
	
	public:
		Realtime(HapticsDevice* device, Calibration* calibration = NULL);	// Constructor
		virtual	~Realtime();												// Destructor 

		void SetLayerDepth(const int& layers);								// Must precede the calls to setOperators.
		void CalcEffectForceTorque(const Vec3f& pos, const Matrix3f& rot, const double& dt, int buttons, ForceTorque& output_force_torque);	// Called by HapticsDevice at the realtime rendering frequency.

		// Should be called by the render loop when new surface and force operators have become
		// available for the given layer. We install the operators in the updated_state of the realtime 
		// process and set the update flag. The position parameter should contain the global position of
		// the device used as the origin for force calculations.
		void SetOperators(const int& layer, HapticsManager* hm, ForceModelSum& force_models, const Vec3f& position);

		// Called at the beginnning of render cycle. We return:
		//  -The current HapticsDevice position (virtual coordinates)
		//  -The current HapticsDevice position (device coordinate)
		//  -proxy, a vector of proxy positions, one for each layer where
		//  -proxy[i] corresponds to m_toState.haptics_manager[i]
		//  -the current HapticsDevice orientation (virtual coordinates)
		//  -the current HapticsDevice orientation (device coordinate)
		//  -the buttons state (bit coded).
		// Note that the device calibration transform converts from device coordinates to virtual coordinates.
		void GetState(Vec3f& position, Vec3f& dev_position, std::vector<Vec3f>& proxy, Rotation& orientation, Rotation& dev_orientation, int& buttons);
	
	public:
		std::auto_ptr<Calibration > m_calibration;		// Pointer to the Calibration object
		
	protected:
		double					m_time;
		HapticsDevice*			m_device;				// The HapticsDevice instance associated with the Realtime object. We set the m_devPosition and m_position variable while making use of the m_calibration variable.
		Lock					m_updateLock;			// Used to transfer rendering updates from the haptics process to the realtime process.
		Lock					m_layerDepthLock;		// Used to change the number of layer used in the haptics process.

		std::vector<bool>		m_firstTime;			// Marks the first time position is determined. One entry for each proxy layer.
		std::vector<bool>		m_update;				// when an update arrives, it is placed in updated_state and the update flag is set. A vector with one entry for each proxy layer.
		SimpleVector<State>		m_updatedState;			// when an update arrives, it is placed in updated_state and the update flag is set. A vector with one entry for each proxy layer.
		SimpleVector<State>		m_fromState;			// We interpolate between the to and from states. Contains one entry for each proxy layer.
		SimpleVector<State>		m_toState;				// We interpolate between the to and from states. Contains one entry for each proxy layer.
	
		
		double					m_updatePeriod;			// The period with which updates from the haptics process arrive. It is calculated as a sliding average of the recent periods.
		double					m_lastUpdate;			// The time of the last update from the haptics process.

		
		Vec3f					m_position;				// Associated with the state of the HapticsDevice, as retreived by the getState function.
		Vec3f					m_devPosition;			// Associated with the state of the HapticsDevice, as retreived by the getState function.
		std::vector<Vec3f>		m_proxy;				// Associated with the state of the HapticsDevice, as retreived by the getState function.
		Rotation				m_devOrientation;		// Associated with the state of the HapticsDevice, as retreived by the getState function.
		int						m_buttons;				// Associated with the state of the HapticsDevice, as retreived by the getState function.
		
		mgFloat					m_averageMagSquared;
		
		mgFloat					m_deltaT;				// Is set at the beginning of each realtime rendering cycle to be the elapsed time since the start of the last realtime rendering cycle.
		Time					m_lastTime;
	
		int						m_layerDepth;			// The number of proxy layers used.
	};


    //! The evaluate funrction returns the force according to this state
    //! for the given haptic device position.
    inline ForceTorque Realtime::State::Evaluate(const Vec3f& pos, const Quaternion& rot, const mgFloat& weighting, const mgFloat& delta_t)
    {
	    if (m_hapticsManager.get())
	    {
		    ForceTorque tmp = m_forceModels.Evaluate6DOF(pos, rot, weighting, delta_t/2);
		    tmp.force += (*m_hapticsManager)(pos - m_origin, weighting, delta_t);
		    return tmp;
	    }

	    return m_forceModels.Evaluate6DOF(pos, rot, weighting, delta_t);
    }

    //! Set the origin.
    inline void Realtime::State::SetOrigin(const Vec3f& o)
    {
	    m_origin = o;
    }

    //! Set the haptics manager.
    inline void Realtime::State::SetHapticsManager(HapticsManager* hm)
    {
	    m_hapticsManager.reset(hm);
    }

    //! Set the force_models.
    inline void Realtime::State::SetForceModels(ForceModelSum& fm)
    {
	    m_forceModels = fm;
    }

    //! get the realtime value.
    inline void Realtime::Calibration::CheckUpdated()
    {
		if (m_cameraRelativeHaptics)
			m_camExtrapolator.UpdateExtrapolation();

		if (m_updated)
	    {
            static Vec3f v1(1.0,0.0,0.0);
            static Vec3f v2(0.0,1.0,0.0);
            static Vec3f v3(0.0,0.0,1.0);

		    // set the realtime_value from the field value.
			if(m_cameraRelativeHaptics)
				m_realtimeValue = m_camRotationMatrix * m_value;
			else
				m_realtimeValue = m_value;

		    Vec3f v = m_realtimeValue.Rotate(v1);
		    m_realtimeRotation.SetColumn(0,v);
		    m_realtimeInvRotation.SetRow(0,v);
		    v = m_realtimeValue.Rotate(v2);
		    m_realtimeRotation.SetColumn(1,v);
		    m_realtimeInvRotation.SetRow(1,v);
		    v = m_realtimeValue.Rotate(v3);
		    m_realtimeRotation.SetColumn(2,v);
		    m_realtimeInvRotation.SetRow(2,v);
			m_updated = false;
		}
		else
		{
			if (m_cameraRelativeHaptics && m_useExtrapolator)
			{
				static Vec3f v1(1.0,0.0,0.0);
				static Vec3f v2(0.0,1.0,0.0);
				static Vec3f v3(0.0,0.0,1.0);	

				m_realtimeValue = m_realtimeValue + m_camExtrapolator.GetExtrapolation();

				Vec3f v = m_realtimeValue.Rotate(v1);
				m_realtimeRotation.SetColumn(0,v);
				m_realtimeInvRotation.SetRow(0,v);
				v = m_realtimeValue.Rotate(v2);
				m_realtimeRotation.SetColumn(1,v);
				m_realtimeInvRotation.SetRow(1,v);
				v = m_realtimeValue.Rotate(v3);
				m_realtimeRotation.SetColumn(2,v);
				m_realtimeInvRotation.SetRow(2,v);
			}
		}
    }

    inline Matrix4f Realtime::Calibration::GetRealtime()
    {
	    CheckUpdated();
	    return m_realtimeValue;
    }

    inline Matrix3f Realtime::Calibration::GetRealtimeRotation()
    {
	    CheckUpdated();
	    return m_realtimeRotation;
    }

    inline Matrix3f Realtime::Calibration::GetRealtimeInverseRotation()
    {
	    CheckUpdated();
	    return m_realtimeInvRotation;
    }

    inline Quaternion Realtime::Calibration::GetRealtimeOrnRotation()
    {
	    CheckUpdated();
	    return m_realtimeOrnRotation;
    }

    //! constructor
    inline Realtime::Calibration::Calibration()
    {
		m_cameraRelativeHaptics = false;
		m_updated = false;
		m_useExtrapolator = true;
    }

    //! destructor
    inline Realtime::Calibration::~Calibration()
    {
    }

    //! constructor
    inline Realtime::State::State(): m_origin(0.0,0.0,0.0), m_forceModels(-1)  
    {
    }

    //! destructor
    inline Realtime::State::~State()
    {
    }
}

#endif
