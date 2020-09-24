
//! File: <ForceModelInfo.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_FORCEMODELINFO_H_
#define _HAPTX_FORCEMODELINFO_H_

#include "HXEngine.h"
#include "HXSolver/HaptXDefines.h"
#include "HXSolver/LinearAlgebra.h"
#include "HXSolver/Extrapolator.h"

namespace HaptX 
{
	// All ForceModels (FM) derived from ForceModelAnimated have pointers to ForceModelInfo objects. The ForceModelInfo 
	// object contains FM parameter values. Each type has a specialized ForceModelInfo type, e.g. a ForceModelMagnet
	// has a pointer to a ForceModelMagnetInfo. The basic ForceModelInfo contains the FM id, the coordinate system, 
	// output force and parameter values for setting time control.
	class HAPTX_ENGINE_SPEC ForceModelInfo
	{
	public:
		ForceModelInfo();																// Constructor
		virtual ~ForceModelInfo();														// Destructor

		int					GetForceModelID();											// Get the id of the ForceModel, see m_forceModelID.
		forceCoordinates_e	GetCoordinateSystem();										// Get the coordinate system that the ForceModel is using,  see m_coordinateSystem.
		Vec3f				GetForceOut();												// Get the output force from the ForceModel, see m_forceOut.

		void				SetCoordinateSystem(forceCoordinates_e coordinateSystem);	// Set the cordinate system that the ForceModel should use, see m_coordinateSystem.
		void				SetTimeControl(bool loop, mgFloat playDuration, mgFloat idleDuration, mgFloat rampInDuration, mgFloat rampOutDuration); // Set the time control of the ForceModel, which is used to animate ForceModels over time, see m_loop, m_playDuration, m_idleDuration, m_rampInDuration, m_rampOutDuration.
		void				SetDeviceID(int DeviceID){m_DeviceID = DeviceID;}

	protected:
		friend class HaptXInterface;
		friend class ForceModelAnimated;

		// The ID of the ForceModel.
		int m_forceModelID;

		// The ID of the device that this ForceModel is created for.
		int m_DeviceID;

		// m_coordinateSystem specifies in what coordinate system the ForceModel is added and 
		// how the position of the HapticDevice is given to the ForceModel. See ForceModel_CoordinateSystem. 
		forceCoordinates_e m_coordinateSystem;

		// Defines if the ForceModel should repeat after m_playDuration + m_idleDuration. Default value: false
		bool m_loop;
		
		// Defines the how long into the one animation cycle the ForceModel is. It is set to zero after one loop cycle.
		mgFloat m_timePlayed;
		
		// Defines the time in seconds (-1 = infinite duration) that one loop cycle should generate a force. Recommended values: 0.0 - infinity.
		mgFloat m_playDuration;
		
		// Defines the time in seconds between two loop cycles when the force output is zero. Recommended values: 0.0 - infinity.
		mgFloat	m_idleDuration;
		
		// m_rampInDuration is used to linearly increase the force output before playDuration. 0 means no interpolation, 
		// and 2 means that force output is at its peak 2 second after it is started. Recommended values: 0.0 - infinity.
		mgFloat	m_rampInDuration;
		
		// rampDownDuration is used to linearly decrease the force output after playDuration. 0 means no interpolation, 
		// and 2 means that force is interpolated down during 2 seconds. Recommended values: 0.0 - infinity.
		mgFloat	m_rampOutDuration;
		
		// The output force of the ForceModel. Can not be set.
		Vec3f m_forceOut;
	};

	// The ForceModelImpulseInfo contains ForceModelImpulse parameters values, see also ForceModelInfo
	class HAPTX_ENGINE_SPEC ForceModelImpulseInfo : public ForceModelInfo
	{
	public: 
		ForceModelImpulseInfo();					// Constructor
		virtual ~ForceModelImpulseInfo();			// Destructor

		Vec3f GetForce();							// Get the force of the ForceModel, see m_force. 
		void  SetForce(Vec3f force);				// Set the force of the ForceModel, see m_force.
		
	protected:
		friend class ForceModelImpulse;
		
		// Defines the force that the Impulse ForceModel should output. This force can be animated by setting the 
		// time control parameters. The total length of the force vector will be the total force in Newtons that are sent to the device.
		// Recommended values: [0-1, 0-1, 0-1]
		Vec3f m_force;

		// <NO_DOC>
		Vec3f m_rtForce;	// force that is rotated in the haptic world.
		// </NO_DOC>
	};

	// The ForceModelSineWaveInfo contains ForceModelSineWave parameter values, see also ForceModelInfo
	class HAPTX_ENGINE_SPEC ForceModelSineWaveInfo : public ForceModelInfo
	{
	public: 
		ForceModelSineWaveInfo();					// Constructor
		virtual ~ForceModelSineWaveInfo();			// Destructor

		Vec3f	GetDirection();						// Get the direction of the wave ForceModel, see m_direction.
		mgFloat GetAmplitude();						// Get the amplitude of the wave ForceModel, see m_amplitude.
		mgFloat GetFrequency();						// Get the frequency of the wave ForceModel, see m_frequency.
		void	SetDirection(Vec3f direction);		// Set the direction of the wave ForceModel, see m_direction.
		void	SetAmplitude(mgFloat amplitude);	// Set the amplitude of the wave ForceModel, see m_amplitude.
		void	SetFrequency(mgFloat frequency);	// Set the frequency of the wave ForceModel, see m_frequency.

	protected:
		friend class ForceModelSineWave;

		Vec3f	m_direction;						// The direction of the wave force which is normalized
		mgFloat m_amplitude;						// The amplitude of the wave force controls the force output, Recommended values: 0.0 - 1.0,
		mgFloat m_frequency;						// The frequency of the wave force in Hz.

		// <NO_DOC>
		Vec3f m_rtDirection;						// direction that is rotated in the haptic world.
		// </NO_DOC>
	};

	// The ForceModelSawWaveInfo contains ForceModelSawWave parameter values, see also ForceModelSineWaveInfo
	class HAPTX_ENGINE_SPEC ForceModelSawWaveInfo : public ForceModelSineWaveInfo
	{
	public:
		ForceModelSawWaveInfo();					// Constructor
		virtual ~ForceModelSawWaveInfo();			// Destructor

	protected:
		friend class ForceModelSawWave;
	};

	// The ForceModelSquareWaveInfo contains ForceModelSquareWave parameter values, see also ForceModelSineWaveInfo
	class HAPTX_ENGINE_SPEC ForceModelSquareWaveInfo : public ForceModelSineWaveInfo
	{
	public:
		ForceModelSquareWaveInfo();					// Constructor
		virtual ~ForceModelSquareWaveInfo();		// Destructor

	protected:
		friend class ForceModelSquareWave;
	};

	// The ForceModelExplosionInfo contains ForceModelExplosion parameter values, see also ForceModelInfo
	class HAPTX_ENGINE_SPEC ForceModelExplosionInfo : public ForceModelSineWaveInfo
	{
	public:
		ForceModelExplosionInfo();							// Constructor
		virtual ~ForceModelExplosionInfo();					// Destructor

		Vec3f	GetOrigin();								// Get the origin of the Explosion ForceModel, see m_origin.
		mgFloat GetEffectDistance();						// Get the effect distance of the Explosion ForceModel, see m_effectDistance.
		mgFloat	GetExplosionForce();						// Get the repelling force of the Explosion ForceModel, see m_explosionForce. 
		void	SetOrigin(Vec3f origin);					// Set the origin of the Explosion ForceModel, see m_origin.
		void	SetEffectDistance(mgFloat effectDistance);	// Set the effect distance of the Explosion ForceModel, see m_effectDistance.
		void	SetExplosionForce(mgFloat explosionForce);	// Set the repelling force of the Explosion ForceModel, see m_explosionForce
			
	protected:
		friend class ForceModelExplosion;

		Vec3f m_origin;										// The origin of the explosion.
		mgFloat m_effectDistance;							// The effect distance of the explosion 
		mgFloat m_explosionForce;							// The repelling force of the explosion near the origin, Recomended values: 0.0 - 1.0

		// <NO_DOC>
		Vec3f m_scOrigin;									// origin that is scaled in the haptic world.
		mgFloat m_scEffectDistance;							// effectDistance that is scaled in the haptic world
		// </NO_DOC>
	};

	// The ForceModelMagnetInfo contains ForceModelMagnet parameter values, see also ForceModelInfo
	class HAPTX_ENGINE_SPEC ForceModelMagnetInfo : public ForceModelInfo
	{	
	public:
		ForceModelMagnetInfo();								// Constructor
		virtual ~ForceModelMagnetInfo();					// Destructor

		Vec3f	GetOrigin();								// Get the origin of the magnet ForceModel, see m_origin.
		mgFloat GetMagnetForce();							// Get the force magnitude of the magnet ForceModel, see m_magnetForce.
		mgFloat GetOrigoSnapDistance();						// Get the origo snap distance of the magnet ForceModel, see m_origoSnapDistance.
		mgFloat GetEffectDistance();						// Get the effect distance of the magnet ForceModel, see m_effectDistance.
		void	SetOrigin(Vec3f origin);					// Set the origin of the magnet ForceModel, see m_origin.
		void	SetMagnetForce(mgFloat magnetForce);		// Set the force magnitude of the magnet ForceModel, see m_magnetForce.
		void	SetOrigoSnapDistance(mgFloat origoSnapDistance);// Set the snap distance of the magnet ForceModel, see m_snapDistance.
		void	SetEffectDistance(mgFloat effectDistance);	// Set the effect distance of the magnet ForceModel, see m_effectDistance.
	
	protected:
		friend class ForceModelMagnet;

		// The origo snap distance of the magnet determines the distance from origo where the magnet starts to generate a spring force towards origo. 
		// This distance can be felt as a small bump in the otherwise linear force. It is important to set this value 
		// in combination with m_magnetForce to achieve stable forces around the origin. 
		// Recommended values: as low as possible.
		mgFloat m_originSnapDistance;
		mgFloat m_magnetForce;						// The force of the magnet at m_origoSnapDistance. 
		mgFloat m_effectDistance;					// The effect distance of the magnet.
		Vec3f	m_origin;							// The origin of the magnet.

		// <NO_DOC>
		mgFloat m_scOriginSnapDistance;				// scaled in the haptic world.
		mgFloat m_scEffectDistance;					// scaled in the haptic world
		Vec3f	m_rtOrigin;							// origin that is rotated in the haptic world.
		// </NO_DOC>
	};

	// The ForceModelSpringInfo contains ForceModelSpring parameter values, see also ForceModelInfo
	class HAPTX_ENGINE_SPEC ForceModelSpringInfo : public ForceModelInfo
	{	
	public:
		ForceModelSpringInfo();								// Constructor
		virtual ~ForceModelSpringInfo();					// Destructor

		Vec3f	GetOrigin();								// Get the origin of the Spring ForceModel, see m_origin.
		mgFloat GetStiffness();								// Get the stiffness of the Spring ForceModel, see m_stiffness.
		mgFloat GetDamping();								// Get the damping of the Spring ForceModel, see m_damping.
		void	SetOrigin(Vec3f origin);					// Set the origin of the Spring ForceModel, see m_origin.
		void	SetStiffness(mgFloat stiffness);			// Set the stiffness of the Spring ForceModel, see m_stiffness.
		void	SetDamping(mgFloat damping);				// Set the damping of the Spring ForceModel, see m_damping.
		void	SetUseExtrapolator(bool use_extrapolator);	// Turn extrapolator on/off

	protected:
		friend class ForceModelSpring;

		Vec3f	m_origin;							// The origin of the spring.
		mgFloat m_stiffness;						// The stiffness of the spring.
		mgFloat m_damping;							// The damping of the spring, which gives the spring friction charachteristic.
		bool	m_useExtrapolator;					// should we use the extrapolator?

		// <NO_DOC>
		Vec3f						m_scOrigin;							// origin that is scaled in the haptic world.
		Vec3f						m_usedOrigin;						// the origin value that is used in calculations (if extrapolator in used)
#pragma warning(disable : 4251)											// needs to have dll-interface to be used by clients of class
		LinearExtrapolator<Vec3f>	m_originExtrapolator;				// extrapolator for "m_origin"
#pragma warning(default : 4251)
		// </NO_DOC>
	};

	// The ForceModelRepellingMagnetInfo contains ForceModelRepellingMagnet parameters values, see also ForceModelInfo
	class HAPTX_ENGINE_SPEC ForceModelRepellingMagnetInfo : public ForceModelMagnetInfo
	{
	public:
		ForceModelRepellingMagnetInfo();			// Constructor
		virtual ~ForceModelRepellingMagnetInfo();	// Destructor

	public:
		friend class ForceModelRepellingMagnet;
	};

	// The ForceModelViscosityInfo contains ForceModelViscosity parameters values, see also ForceModelInfo
	class HAPTX_ENGINE_SPEC ForceModelViscosityInfo : public ForceModelInfo
	{
	public:
		ForceModelViscosityInfo();					// Constructor
		virtual ~ForceModelViscosityInfo();			// Destructor

		Vec3f	GetVelocity();						// Get the velocity of the Viscosity ForceModel, see m_velocity.
		mgFloat GetMass();							// Get the mass of the Viscosity ForceModel, see m_mass.
		mgFloat GetStiffness();						// Get the stiffness of the Viscosity ForceModel, see m_stiffness.
		mgFloat GetDamping();						// Get the damping of the Viscosity ForceModel, see m_damping.
		void	SetVelocity(Vec3f velocity);		// Set the velocity of the Viscosity ForceModel, see m_velocity.
		void	SetMass(mgFloat mass);				// Set the mass of the Viscosity ForceModel, see m_mass.
		void	SetStiffness(mgFloat stiffness);	// Set the stiffness of the Viscosity ForceModel, see m_stiffness.
		void	SetDamping(mgFloat damping);		// Set the damping of the Viscosity ForceModel, see m_damping.

	protected:
		friend class ForceModelViscosity;

		
		Vec3f m_velocity;		// The velocity of the viscosity ForceModel which can be set to create an effect of catching a movable object. Default value: [0.0, 0.0, 0.0]. 
		mgFloat m_mass;			// The mass of the viscosity ForceModel must greater than zero, Recommended values: 0.1 - 20.0. 
		mgFloat m_stiffness;	// The stiffness of the viscosity ForceModel. Recommended values: 0.1 - 1.0.
		mgFloat m_damping;		// The damping of the viscosity ForceModel. Recommended values: 0.0 - 1.0.

		// <NO_DOC> 
		Vec3f m_rtVelocity;		// velocity that is rotated in the haptic world.
		// </NO_DOC>
	};

	// The ForceModelRBIInfo contains ForceModelRBI parameters values, see also ForceModelInfo
	class HAPTX_ENGINE_SPEC ForceModelRBIInfo : public ForceModelInfo
	{
	public:
		ForceModelRBIInfo();					// Constructor
		virtual ~ForceModelRBIInfo();			// Destructor

		int		GetMaxContacts();															// Returns the maximum amount of conntacts allowed
		int		GetInactiveContacts();														// Returns how many contacts that are not activated
		void	ActivateContact(int num, Vec3f contactPoint, Vec3f normal, mgFloat force);	// Activates a contact (no more then GetMaxContacts() is allowed)
		void	DeactivateContact(int num);													// Deactivates a contact
		void	DeactivateAllContacts();													// Deactivate all contacts

	protected:
		void	AllocateRigidBodyContacts(int count);

	protected:
		friend class ForceModelRBI;

		// This is a collision contact point. This is used by ForceModelRBI
		struct rigidBodyContactInfo_t
		{
			Vec3f	contactPoint;
			Vec3f	normal;
			mgFloat	force;
			bool	active;

			rigidBodyContactInfo_t() : contactPoint(Vec3f(0.0,0.0,0.0)), normal(Vec3f(0.0,1.0,0.0)), force(0.0), active(false)
			{
			}

			void operator = (const rigidBodyContactInfo_t& p)
			{
				contactPoint = p.contactPoint;
				normal = p.normal;
				force = p.force;
				active = p.active;
			}
		};

		rigidBodyContactInfo_t*		m_bodyContacts;
		int							m_maxContacts;
		int							m_inactiveContacts;
	};

	// The ForceModelNavigationInfo contains ForceModelNavigation parameters values, see also ForceModelInfo
	class HAPTX_ENGINE_SPEC ForceModelNavigationInfo : public ForceModelInfo
	{
	public:
		ForceModelNavigationInfo();						// Constructor
		virtual ~ForceModelNavigationInfo();			// Destructor

		Vec3f GetOrigin();								// Returns the origin of the ForceModel, see m_origin
		mgFloat GetButtonStrokeDistance();				// Returns the distance of the button effect, see m_buttonStrokeRadius
		mgFloat GetButtonForce();						// Return the force of the button effect, see m_buttonForce
		mgFloat GetSpringRadius();						// Returns the radius where the spring effect starts, see m_springRadius;
		mgFloat GetSpringStiffness();					// Returns the stiffness of the ,see m_springStiffness

		void SetOrigin(Vec3f origin);								// Sets the origin of the ForceModel, see m_origin
		void SetSpringRadius(mgFloat springRadius);					// Sets the radius where the spring effect starts, see m_springRadius;
		void SetSpringStiffness(mgFloat springStiffness);			// Sets the stiffness of the ,see m_springStiffness
		void SetButtonStrokeDistance(mgFloat buttonStrokeDistance);	// Sets the distance of the button effect, see m_buttonStrokeRadius
		void SetButtonForce(mgFloat buttonForce);					// Sets the force of the button effect, see m_buttonForce
		

	protected:
		friend class ForceModelNavigation;

		Vec3f m_origin;							// The origin of the navigation spring and the button stroke cylinders
		mgFloat m_springRadius;					// Defines the radius from the origin that the spring effect will start. Note that this radius needs to be larger or equal to m_buttonStrokeRadius
		mgFloat m_springStiffness;				// The stiffness of the spring that is activated after the distance m_buttonStrokeRadius is exceeded.
		mgFloat m_buttonStrokeDistance;			// Determines the button distance from m_springRadius and hence how far the button can be pressed before the spring is reached. This value needs to be set in conjunction with m_buttonForce, and 0 means no button effect.
		mgFloat m_buttonForce;					// The force that needs to be exceeded to get to the spring volume, 0 means no button effect
		
		
		// <NO_DOC>
		Vec3f m_scOrigin;						// origin that is scaled in the haptic world.
		mgFloat m_scSpringRadius;				// springRadius that is scaled in the haptic world.
		mgFloat m_scButtonStrokeDistance;		// buttonStrokeDistance that is scaled in the haptic world.
		// </NO_DOC>

	};
}

#endif
