
//! File: <SurfaceInfo.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef	_HAPTX_SURFACEINFO_H_
#define _HAPTX_SURFACEINFO_H_

#include "HXEngine.h"
#include "HXSolver/LinearAlgebra.h"
#include "HXSolver/HaptXDefines.h"

namespace HaptX
{
	class HapticTexture;
	class Surface;

	// All surfaces have pointers to SurfaceInfo objects. The SurfaceInfo object contains surface 
	// parameter values. Each surface type has a specialized SurfaceInfo type, e.g. a VibratingSurface 
	// has a pointer to a VibratingSurfaceInfo. The basic SurfaceInfo contains the surface id and the surface type.
	class HAPTX_ENGINE_SPEC SurfaceInfo
	{
	public:
						SurfaceInfo();								// Constructor.
		virtual			~SurfaceInfo();								// Destructor.

		int				GetSurfaceID();								// Get the surface ID. 
		surfaceType_e	GetSurfaceType();							// Get the surface type. see also surfaceType_e.
		Surface*		GetSurface();								// Return a pointer to the surface
		
	protected:
		friend class HaptXInterface;
		
		int				m_surfaceID;								// The ID if the surface.
		surfaceType_e	m_surfaceType;								// The type of the surface. See surfaceType_e for a list of available HaptX Surface types.
	};


	// The SimpleSurfaceInfo contains SimpleSurface parameter values. See also SurfaceInfo
	class HAPTX_ENGINE_SPEC SimpleSurfaceInfo : public SurfaceInfo
	{
	public:
						SimpleSurfaceInfo();						// Constructor.
		virtual			~SimpleSurfaceInfo();						// Destructor.
		
		mgFloat			GetStiffness();								// Get the stiffness of the surface, see m_stiffness.
		mgFloat			GetDamping();								// Get the damping of the surface, see m_damping. 
		void			SetStiffness(mgFloat stiffness);			// Set the stiffness of the surface, see m_stiffness. 
		void			SetDamping(mgFloat damping);				// Set the damping of the surface, see m_damping. 
		
	protected:
		// Surface stiffness, defined in percent of the maximum allowed stiffness of the device. 
		// It specifies surface stiffness in direction of the surface normal. See also DeviceGetMaxStiffness.
		// Default value: 0.9. Recommended values: 0.0 - 1.0 
		mgFloat m_stiffness;

        // The damping parameter is used to specify damping associated with surface collision.
		// It is generated in the direction of the normal component of the motion. It is defined 
		// in Newton seconds / metre. Default value: 0.0 
		// Recommended values: 0.0 - 1.5 (Keep lower (~0.5) if stiffness > 0.2).
		mgFloat m_damping;
	};

	// VibratingSurfaceInfo contains VibratingSurface parameter values. See also SurfaceInfo
	class HAPTX_ENGINE_SPEC VibratingSurfaceInfo : public SimpleSurfaceInfo
	{
	public:
						VibratingSurfaceInfo();						// Constructor.
		virtual			~VibratingSurfaceInfo();					// Destructor.
	
		Vec2f			GetAmplitude();								// Get the amplitude of the surface vibration, see m_amplitude. 
		Vec2f			GetFrequency();								// Get the frequency of the surface vibration, see m_frequency.
		Vec2f			GetPhase();									// Get the phase of the surface vibration, see m_phase.
		void			SetAmplitude(Vec2f amplitude);				// Set the amplitude of the surface vibration. Note! Use with causion. Not advisable to raise amplitude above [1.0, 1.0], see m_amplitude.
		void			SetFrequency(Vec2f frequency);				// Set the frequency of the surface vibration, see m_frequency.
		void			SetPhase(Vec2f phase);						// Set the phase of the surface vibration, see m_phase.
		
	protected:
		// Amplitude of the sinus curve that generates the vibrating surface force (A in A*sin(ft+p)). 
		// Default value: [1.0, 1.0]. Recommended values: [0.0, 0.0] - [1.0, 1.0]. 
		Vec2f m_amplitude;

        // Frequency is the frequency of the vibrating surface in Hertz (f in A*sin(ft+p)). 
		// Default value: [10.0, 10.0]. Recommended values: [0.0, 0.0] - [100.0, 100.0].
		Vec2f m_frequency;

        // Phase is the phase of the sine function, unitless (p in A*sin(ft+p)). 
		// Default value: [0.0, 0.0]. Recommended values: [*,*] 
		Vec2f m_phase;
	};

	// FrictionalSurfaceInfo contains FrictionalSurface parameter values. See also SurfaceInfo
	class HAPTX_ENGINE_SPEC FrictionalSurfaceInfo : public SimpleSurfaceInfo
	{
	public:
						FrictionalSurfaceInfo();						// Constructor.
		virtual			~FrictionalSurfaceInfo();						// Destructor.

		mgFloat			GetStartingFriction();							// Get the starting friction of the surface, see m_startingFriction.
		mgFloat			GetStoppingFriction();							// Get the stopping friction of the surface, see m_stoppingFriction.
		mgFloat			GetDynamicFriction();							// Get the dynamic friction of the surface, see m_dynamicFriction.
		mgFloat			GetStiffnessT();								// Get the tangential stiffness of the surface, see m_stiffnessT.
		void			SetStartingFriction(mgFloat startingFriction);	// Set the starting friction of the surface, see m_startingFriction.
		void			SetStoppingFriction(mgFloat stoppingFriction);	// Set the stopping friction of the surface, see m_stoppingFriction.
		void			SetDynamicFriction(mgFloat dynamicFriction);	// Set the dynamic friction of the surface, see m_dynamicFriction.
		void			SetStiffnessT(mgFloat stiffnessT);				// Set the tangential stiffness of the surface, see m_stiffnessT.
		
	protected:
		// m_startingFriction is the coefficient of static friction required to move from static-contact to dynamic
		// (kinetic/coulomb) contact.  when the proxy is in static contact with the surface a tangential force of
		// starting_friction * (f*n) is required to induce slipping.
		// Default value: 0.3. Recommended values: 0.0 - 1.5.
		mgFloat m_startingFriction;
			
		// m_stoppingFriction is the coefficient of static friction required to move from dynamic-contact to static
		// contact. When the proxy is in dynamic contact with the  surface a tangential force of at least 
		// stopping_friction * (f*n) is required to continue slipping. 
		// Default value: 0.1. Recommended values: 0.0 - 1.5.
		mgFloat m_stoppingFriction;
		
		// m_dynamicFriction is the coefficient of dynamic friction. when
		// slipping, the tangential force is retarded by dynamic_friction * (f*n). 
		// Default value: 0.2. Recommended values: 0.0 - 1.5.
		mgFloat m_dynamicFriction;
				
		// m_stiffnessT is tangential stiffness used to calculate tangential frictional forces. 
		// The unit is a percentage of the device maximum stiffness (Netwtons\m).
		// Default value: 0.7. Recommended values: 0.0 - 1.0.
		mgFloat m_stiffnessT;
	};

	// ButtonFrictionalSurfaceInfo contains ButtonFrictionalSurface parameter values. See also SurfaceInfo
	class HAPTX_ENGINE_SPEC ButtonFrictionalSurfaceInfo : public FrictionalSurfaceInfo
	{
	public:
						ButtonFrictionalSurfaceInfo();				// Constructor.
		virtual			~ButtonFrictionalSurfaceInfo();				// Destructor.
		
		mgFloat			GetClickForce();							// Get the click force of the surface, see m_clickForce.
		bool*			GetArmedRef();								// Get a boolean reference to the armed state of the surface, see m_refArmed.
		void			SetArmedRef(bool* armedRef);				// Set the reference to the armed state of the surface, see m_refArmed.
		void			SetClickForce(mgFloat clickForce);			// Set the click force of the surface, see m_clickForce.

	protected:
		// m_clickForce is a pecentage of the device maximum force that is required to press 
		// the button surface into an armed state.
		// Default value: 0.7. Recommended values: 0.0 - 1.0
		mgFloat m_clickForce;

		// m_pbRefArmed is a ponter to a boolean. It's value is set to true
		// when the button surface is pressed. Default value: NULL. 
		bool* m_refArmed;
		
	};

	// RoughSurfaceInfo contains RoughSurface parameter values. See also SurfaceInfo
	class HAPTX_ENGINE_SPEC RoughSurfaceInfo : public FrictionalSurfaceInfo
	{
	public:
						RoughSurfaceInfo();							// Constructor.
		virtual			~RoughSurfaceInfo();						// Destructor.

		mgFloat			GetMean();									// Get the mean deviation of the starting friction of the surface, see m_mean.
		mgFloat			GetDeviation();								// Get the standard deviation of the starting friction of the surface, see m_deviation.
		void			SetMean(mgFloat mean);						// Set the mean of deviation of the starting friction of the surface, see m_mean.
		void			SetDeviation(mgFloat deviation);			// Set the standard deviation of the starting friction of the surface, see m_deviation.
		
	protected:
		// mean and deviation are the average and standard deviation of
		// the starting_friction value for the surface. whenever
		// the state changes to "slipping" we randomly set a new
		// starting_friction value according to a normal
		// (gaussian) distribution with the following mean and deviation.
		// Default value: 0.5. Recommended values: 0.0 - 1.0.
		mgFloat m_mean;
		
		// mean and deviation are the average and standard deviation of
		// the starting_friction value for the surface. whenever
		// the state changes to "slipping" we randomly set a new
		// starting_friction value according to a normal
		// (gaussian) distribution with the following mean and deviation. 
		// Default value: 0.1. Recommended values: 0.0 - 1.0.	
		mgFloat m_deviation;
	};

	// HeightmapSurfaceInfo contains HeightmapSurface parameter values. See also SurfaceInfo
	class HAPTX_ENGINE_SPEC HeightmapSurfaceInfo : public FrictionalSurfaceInfo
	{
	public:
						HeightmapSurfaceInfo();						// Constructor.
		virtual			~HeightmapSurfaceInfo();					// Destructor.

		mgFloat			GetBumpHeight();							// Get the maximum bump height of the surface, see m_bumpHeight.
		HapticTexture*	GetTexture();								// Get a pointer to the HapticTexture of the surface, see m_texture.
		void			SetBumpHeight(mgFloat bumpHeight);			// Set the maximum bump height of the surface, see m_bumpHeight.
		void			SetTexture(HapticTexture* texture);			// Set the HapticTexture of the surface, see m_texture.

	public:
		bool			m_textureUpdated;							// Set to true every time SetTexture() is used and causes the HeightmapSurface to update the texture.

	protected:
		// The maximum height (in meters) of the bumps. i.e the difference in height 
		// between a black and a white pixel in the texture.
		// Default value: 0.001. Recommended values: 0.0 - 0.05
		mgFloat m_bumpHeight;

		// A pointer to the texture. Default value: NULL 
		HapticTexture* m_texture;
	};

	// ImageSurfaceInfo contains FrictionmapSurface parameter values. See also SurfaceInfo
	class HAPTX_ENGINE_SPEC ImageSurfaceInfo : public FrictionalSurfaceInfo
	{
	public:
						ImageSurfaceInfo ();						// Constructor.
		virtual			~ImageSurfaceInfo();						// Destructor.

		HapticTexture*	GetTexture();								// Get a pointer to the HapticTexture of the surface, see m_texture.
		void			SetTexture(HapticTexture* texture);			// Set the HapticTexture of the surface, see m_texture.

	public:
		bool			m_textureUpdated;							// Set to true every time SetTexture() is used and causes the FrictionmapSurface to update the texture.

	protected:
		HapticTexture*	m_texture;									// A pointer to the texture.
	};
	
	// StickyFrictionalSurfaceInfo contains StickyFrictionalSurface parameter values. See also SurfaceInfo
	class HAPTX_ENGINE_SPEC StickyFrictionalSurfaceInfo : public FrictionalSurfaceInfo
	{
	public:
						StickyFrictionalSurfaceInfo();					// Constructor.
		virtual			~StickyFrictionalSurfaceInfo();					// Destructor.

		mgFloat			GetDeadHeight();								// Get the dead height of the surface, see m_deadHeight.
		mgFloat			GetStickyStiffness();							// Get the sticky stiffness of the surface, see m_stickyStiffness
		void			SetDeadHeight(mgFloat deadHeight);				// Set the dead height of the surface, see m_deadHeight.
		void			SetStickyStiffness(mgFloat stickyStiffness);	// Set the sticky stiffness of the surface, see m_stickyStiffness
		
	protected:
		// The dead height describes the height in metres of the dead region.
		// Default value: 0.004. Recommended values: 0.0 - 0.01.
		mgFloat m_deadHeight;

		// The sticky stiffness describes the stiffness per metre of the dead region sticky force. 
		// The unit is a percentage of the device's maximum force. 
		// Default value: 0.6. Recommended values: 0.0 - 1.0
		mgFloat m_stickyStiffness;
	};

	// StickyHeightmapSurfaceInfo contains StickyHeightmapSurface parameter values. See also SurfaceInfo
	class HAPTX_ENGINE_SPEC StickyHeightmapSurfaceInfo : public HeightmapSurfaceInfo
	{
	public:
						StickyHeightmapSurfaceInfo();					// Constructor.
		virtual			~StickyHeightmapSurfaceInfo();					// Destructor.
		
		mgFloat			GetDeadHeight();								// Get the dead height of the surface, see m_kDeadHeight.
		mgFloat			GetStickyStiffness();							// Get the sticky stiffness of the surface, see m_kStickyStiffness
		void			SetDeadHeight(mgFloat deadHeight);				// Set the dead height of the surface, see m_kDeadHeight.	
		void			SetStickyStiffness(mgFloat stickyStiffness);	// Set the sticky stiffness of the surface, see m_kStickyStiffness

	protected:
		// The dead height describes the height in metres of the dead region.
		// Default value: 0.004. Recommended values: 0.0 - 0.01.
		mgFloat m_deadHeight;

		// The sticky stiffness describes the stiffness (in Force
		// per metre) of the dead region sticky force. The unit is a percentage of the device's maximum force.
		// Default value: 0.6. Recommended values:  0.0 - 1.0
		mgFloat m_stickyStiffness;
	};
}

#endif
