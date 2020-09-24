
//! File: <VibratingSurface.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_VIBRATINGSURFACE_H_
#define _HAPTX_VIBRATINGSURFACE_H_

#define _USE_MATH_DEFINES
#include <math.h>

#include "SimpleSurface.h"

namespace HaptX
{
	// VibratingSurface extends the SimpleSurface by adding vibrating properties.
	// Note! Use with causion. Not advisable to raise amplitude above [1.0, 1.0]
	// 
	// Two sine functions generate the vibration, one horisontal and one vertical. 
	// The sine functions look like: A*sin(2*pi*f*t+p), where (A) is the amplitude,
	// (f) is the frequency (Hertz), and (p) is the phase. See also VibratingSurfaceInfo.
	class VibratingSurface : public SimpleSurface
	{
	public:
		inline								VibratingSurface();							// Constructor
		inline								VibratingSurface(VibratingSurfaceInfo* si); // Constructor

		VibratingSurfaceInfo*				GetSurfaceInfo();							// Returns the SurfaceInfo which holds the parameters for the surface.

		inline virtual Surface::Contact*	NewContact();								// New contact. 
		inline virtual Surface::Contact*	CopyContact();								// Copy contact. 

	protected:
		// See Surface::Contact
		class Contact : public SimpleSurface::Contact
		{
		public:
			
			inline							Contact(VibratingSurface* s);				// Initialise any state variables of the contact, possibly copying from the member values of the surface.
  			inline							Contact();									// The void constructor is required in all sub-classes.
  
			virtual void					Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement); 	// Evaluate produces the rendering force and updates the proxy position. See also Surface::Contact::Evaluate
		
		public:	  
			Vec2f							m_amplitude;								// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created. See also VibratingSurfaceInfo.
			Vec2f							m_frequency;								// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created. See also VibratingSurfaceInfo.
			Vec2f							m_phase;									// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created. See also VibratingSurfaceInfo.
		};
	};

	//! Inline member methods
	inline VibratingSurface::VibratingSurface() : SimpleSurface(new VibratingSurfaceInfo())
	{
	}

	inline VibratingSurface::VibratingSurface(VibratingSurfaceInfo* si) : SimpleSurface(si)
	{
	}

	inline Surface::Contact* VibratingSurface::NewContact()
	{
		return new Contact(this);
	}

	inline Surface::Contact* VibratingSurface::CopyContact()
	{
		return CopyContactT(new Contact);
	}

	inline VibratingSurface::Contact::Contact(VibratingSurface* s) : SimpleSurface::Contact(s), m_amplitude(s->GetSurfaceInfo()->GetAmplitude()), m_frequency(s->GetSurfaceInfo()->GetFrequency()), m_phase(s->GetSurfaceInfo()->GetPhase())
	{
	}

	inline VibratingSurface::Contact::Contact()
	{
	}
}

#endif
