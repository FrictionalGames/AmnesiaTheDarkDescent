
//! File: <HapticsDevice.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_HAPTICSDEVICE_H_
#define _HAPTX_HAPTICSDEVICE_H_

// State in the global semaphore used to make sure only one HaptX application tries to access the Touch Controller at a time.
#define HX_RUNNING		0
// see HX_RUNNING
#define HX_TAKEOVER		1
// see HX_RUNNING
#define HX_STOPPED		2

// Offset used in the global semaphore for multiple applications.
// For example: A HaptX application that communicates with a Novint Touch Controller does not need 
// to stop another HaptX application from communicating with a Sensable Touch Controller and so forth.
#define HX_HW_SENSABLE	0
// see HX_HW_SENSABLE
#define HX_HW_NOVINT	1

#include <list>

#include "TrackingDevice.h"
#include "CollisionState.h"
#include "CollisionCallback.h"

namespace HaptX
{
	class Realtime;

    // A callback function used for sending shapes to the HaptX loop. It is called each 
	// time the HapticsDevice::Render function is executed. A pointer to the CollistionState 
	// object is passed as an argument to enable customisation of passing data to the HaptX loop. 
	typedef int (*pfnSetUpShapesCB)(CollisionState* cs, const int& index, const double& delta_t, std::list<int> shapeRenderList);

    // A callback function used for sending ForceModels to the HaptX loop. It is called each 
	// time the HapticsDevice::Render function is executed. A pointer to the CollistionState 
	// object is passed as an argument to enable customisation of passing data to the HaptX loop. 	
	typedef int (*pfnSetUpForceModelsCB)(CollisionState* cs, const Matrix4f& m_kCameraMatrix, std::list<int> forceModelRenderList);
	
    // A HapticsDevice represents a Touch controller. Each HapticsDevice has its own HaptX 
	// thread which runs in about 1 kHz. The render function should be called once per 
	// graphics loop to update the data in the HaptX loop. A HapticsDevice has two callback 
	// functions which are used for setting up rendering of HaptX Shapes and ForceModels. 
	// They are called from the HapticsDevice::Render function. To create support for a new 
	// Touch Controller, inherit HapticsDevice and implement the following functions: 
	// InitDevice, StartHapticsLoop, Reset, StopHapticsLoop and UpdateDevice. 
    class HAPTX_SOLVER_SPEC HapticsDevice : public TrackingDevice
    {
    public:	
									HapticsDevice();										// Constructor
        virtual						~HapticsDevice();										// Destructor
	
		void						SetCollisionCallback(pfnCollisionCallbackFunc func);		// set the collision callback function for this device

		void						RegisterShapesCallback(pfnSetUpShapesCB ptr);			// Register a callback function to send HapticShapes to the HaptX loop.
		void						RegisterForceModelsCallback(pfnSetUpForceModelsCB ptr);	// Register a callback function to send ForceModels to the HaptX loop.
	    
		inline void					EvaluateDrawnProxies(Vec3f finger, std::vector<Vec3f>* proxies, mgFloat weighting);	// DrawnProxies are a weighted average of the unconstrained positions and the proxy positions with respect to the proxy_weighting parameter. 

		inline virtual void			InitDevice()				{}							// Virtual function to be overridden by subclasses of HapticsDevice. 
        inline virtual HX_RESULT	StartHapticsLoop()			{ return HX_SUCCESS; }		// Virtual function to be overridden by subclasses of HapticsDevice. 
		inline virtual void			Reset()						{}							// Virtual function to be overridden by subclasses of HapticsDevice. 
        inline virtual void			StopHapticsLoop()			{}							// Virtual function to be overridden by subclasses of HapticsDevice. 
		inline virtual void			ReleaseDevice()				{}							// Virtual function to be overridden by subclasses of HapticsDevice. 
		virtual void				UpdateDevice();											// UpdateDevice is called after each main loop to allow the HapticsDevice to update class members used in the HaptX loop.

		void						Render(const int &index, Matrix4f world_scale = Matrix4f());	// Perform HaptX rendering on the given scene.

		inline bool					IsFirstRender()				{ return m_firstTime; }		// Returns true if the Render function has yet to be called for the first time. Used internally.
		static void					ResetDevice(int deviceID);
	
        // CalculateForceTorque is called from the HaptX loop and is an integral part 
        // of the HaptX rendering of RealtimeGeometries, HaptX Surfaces and ForceModels. 
        // The parameter called buttons is a bit coded integer for the buttons.
        inline void					CalculateForceTorque(const Vec3f& pos, const Matrix3f& rot, const double& dt, int buttons, ForceTorque& output_force_torque);
	
		void						SetCameraRelative(bool cameraRelative);
		void						SetCameraExtrapolation(bool useExtrapolator);
		bool						GetCameraRelative();
		bool						GetCameraExtrapolation();
		void						UpdateCamera(Matrix4f camMatrix);
		void						UpdateCalibration(Matrix4f pos_calib, Matrix4f orn_calib);

		void						SetWorldRotation(Matrix4f worldRotation);
		void						SetWorldRotationInverse(Matrix4f worldRotationInv);
		void						SetWorldScale(Matrix4f worldScale);
		void						SetWorldScaleInverse(Matrix4f worldScaleInv);
		Matrix4f					GetWorldRotation();
		Matrix4f					GetWorldRotationInverse();
		Matrix4f					GetWorldScale();
		Matrix4f					GetWorldScaleInverse();	
		bool						IsActiveApp(){return m_ActiveApp;}
		void						EnableSafetyMaxForce(bool enable);
		bool						SafetyMaxForceEnabled(){return m_SafetyMaxForceOn;}

	protected:	
        void						SetLayerDepth(const int &layers);						// Set the number of proxy layers. Used internally. 
		inline virtual void			YieldHapticLoop(){}                                     // Virtual function to be overriden by subclasses.
		inline virtual bool			TakeoverHapticLoop(){return true;}                      // Virtual function to be overriden by subclasses.                
		float						GetSafeForce(float Force);								// Make sure force does not exceed HX_SAFETY_MAXFORCE if m_SafetyMaxForceOn is enabled.

	private:
		int							SetUpShapes(pfnSetUpShapesCB ptr, CollisionState* cs, const int& index, const double& delta_t, std::list<int> shapeRenderList);
		int							SetUpForceModels(pfnSetUpForceModelsCB ptr, CollisionState* cs, const Matrix4f& m_kCameraMatrix, std::list<int> forceModelRenderList);
	
	public:
		pfnCollisionCallbackFunc	m_collisionCallback;										// pointer to the collision callback function
		collisionResult_t			m_collisionResult;

		Vec3f						m_proxy;													// Holds the proxy position if the scene only has one layer. Otherwise the proxies field holds the positions of the different proxies.
		mgFloat						m_radius;													// Holds the proxy radius if the scene only has one layer. Otherwise the radii field holds the radii of the different proxies.
		
		std::vector<Vec3f>			m_proxies;													// Currently not in use. The m_proxies and m_radii, if used, are associated with proxy layering. The size of the proxies field is determined by the number of layers used in the applictaion. It provides the positions of all of the proxies.
		std::vector<Vec3f>			m_drawnProxies;												// Currently not in use. m_drawnProxies contain weighted averages of the unconstrained positions and the proxy positions, with respect to the proxy_weighting field value. 
		std::vector<mgFloat>		m_radii;													// Currently not in use. The m_proxies and m_radii, if used, are associated with proxy layering. Specifically for layer i, if defined, the proxy will have radius radii->get(i).

		// The final device position is interpolated from the unconstrained position 
        // and the proxy position. m_proxyWeighting is a weighting parameter used in 
        // the interpolation. When a HaptX Surface is touched, the proxy position is 
        // calculated by projecting the unconstrained position onto the surface. 
		// To make the proxy co-locate with the unconstrained position when touching 
        // a surface, m_proxyWeighting should be set to 0.
		mgFloat						m_proxyWeighting;

		bool						m_disconnected;												// If m_bDisconnected is true there is a communication error with the Touch Controller. 
		bool						m_forcesDisabled;											// If m_bForcesDisabled is true no forces will be rendered to the Touch Controller.
		bool						m_rampForcesUWA;
		Vec3f						m_stylusAngles;
		int							m_nrOfButtons;
		mgFloat						m_hapticsRate;

		// Used to scale the rendered force. It must be in the range 0.0 to 1.0 and is useful for
		// scaling forces down for safety, or calibrating the accuracy of the rendered force.
		mgFloat						m_forceScaling;
		
		std::list<int>				m_shapeRenderList;							// A list of HapticShape ID's. It determines which HapticShapes HaptX will render. 
		std::list<int>				m_forceModelRenderList;						// A list of ForceModel ID's. It determines which ForceModels HaptX will render. 
		
		mgFloat						m_maxAllowedForce;							// The manufacturer specified maximum for the particular hardware model. 
		mgFloat						m_maxAllowedStiffness;						// The manufacturer specified maximum for the particular hardware model. 
		mgFloat						m_maxDynamicStiffness;						// Dynamic stiffness is used for ForceModels. 

		mgFloat						m_hapticTime;

   
    protected:
	    friend class HaptXInterface;

        int							m_numLayers;								// internal state data.
        bool						m_startedLoop;								// internal state data.
        bool						m_firstTime;								// internal state data.
        Vec3f						_position;									// internal state data.
        Vec3f						_dev_position;								// internal state data.
        std::vector<Vec3f>			_new_proxy;									// internal state data.
        std::vector<Vec3f>			_proxy;										// internal state data.
        Rotation					_orientation;								// internal state data.
        Rotation					_dev_orientation;							// internal state data.
        int							_buttons;									// internal state data.
        Time						m_lastTime;									// internal state data.
		Matrix4f					m_cameraMatrix;								// The global to local matrix for the camera. This is used to determine the position of the device if relativeCameraHaptics is enabled.
		Matrix4f					m_worldScale;								// The scale of the HaptX world for this device. 
		Matrix4f					m_worldRotation;							// The rotation of the HaptX world for this device.
		Matrix4f					m_worldScaleInv;							// The inverse scale of the HaptX world for this device.
		Matrix4f					m_worldRotationInv;							// The inverse rotation of the HaptX world for this device.
		mgFloat						m_forceModelScale;							// Scale all ForceModels on this device.


        std::auto_ptr<CollisionState>	cs;										// The CollisionState object associated with this HapticsDevice.
		Realtime*					m_realtime;									// The realtime renderer for this HapticsDevice. Sub-classes should use CalculateForce in order to access this object. 
        HANDLE						m_MMF;										// The handle to the memory mapped area of the pagefile used to handle multiple instances of HaptX applications sharing the same Touch Controller.
        int*						m_MMFData;                                  // The pointer to the data in the memory mapped area.
		bool						m_ActiveApp;								// A flag that indicates if this application currently has access to the Touch Controller.
		bool						m_SafetyMaxForceOn;							// A flag that indicates if forces should be clamped at HX_SAFETY_MAXFORCE or not.

	private:
		pfnSetUpShapesCB			m_ShapesCallback;							// A pointer to the callback function.
		pfnSetUpForceModelsCB		m_ForceModelsCallback;						// A pointer to the callback function.
    };

    //! Interpolates between the unconstrained position and the proxy position.
	inline void HapticsDevice::EvaluateDrawnProxies(Vec3f finger, std::vector<Vec3f>* proxies, mgFloat weighting)
    {
	    const std::vector<Vec3f>& p = *proxies;
	    m_drawnProxies.resize(p.size());

	    std::vector< Vec3f >::iterator j = m_drawnProxies.begin();
	    for (std::vector< Vec3f >::const_iterator i=p.begin(); i!=p.end(); ++i,++j)
		{
		    (*j) = (*i) * weighting + (1-weighting) * finger;
		}
    }



}

#endif
