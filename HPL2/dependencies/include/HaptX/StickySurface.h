
//! File: <StickySurface.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_STICKYSURFACE_H_
#define _HAPTX_STICKYSURFACE_H_

#include "HeightmapSurface.h"

namespace HaptX 
{
	// When the device is in contact with the surface, it feels no different 
	// than a regular FrictionalSurface. However, when the device is lifted off 
	// the surface a sticky force is generated, effectively pulling the device back. 
	// Implementation details: Just below the surface, there is a "dead" region. 
	// As the device is pushed into the dead region and lifted back out, a sticky force 
	// is generated. The sticky stiffness is defined in Newtons per metre. The force 
	// required to lift the device off the surface is defined by dead_height * sticky_stiffness. 
	// See also	StickyFrictionalSurfaceInfo.
	class StickyFrictionalSurface : public FrictionalSurface 
	{
	protected:
		// See Surface::Contact
		class Contact : public FrictionalSurface::Contact 
		{
		public:
			inline						Contact(StickyFrictionalSurface* s);	// Constructor
			inline						Contact() {}							// Constructor
			
			inline virtual void			Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement);// Evaluate is a virtual function defined by sub-classes to produce the force rendering of the surface. See Surface::Contact::Evaluate

		public:
			bool						m_sticking;								// Boolean which is true if we are stuck to the surface.
			mgFloat						m_deadHeight;							// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created. See also StickyFrictionalSurfaceInfo
			mgFloat						m_stickyStiffness;					// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created. See also StickyFrictionalSurfaceInfo
		};
		
	public:
										StickyFrictionalSurface();			// Constructor

		StickyFrictionalSurfaceInfo*	GetSurfaceInfo();						// Returns the SurfaceInfo which holds the parameters for the surface.

		virtual Surface::Contact*		NewContact();							// New contact. 
		virtual Surface::Contact*		CopyContact();							// Copy contact. 
	};

	// When the device is in contact with the surface, it feels no different 
	// than a regular HeightmapSurface. However, when the device is lifted off 
	// the surface a sticky force is generated, effectively pulling the device back. 
	// Implementation details: Just below the surface, there is a "dead" region. 
	// As the device is pushed into the dead region and lifted back out, a sticky force 
	// is generated. The sticky stiffness is defined in Newtons per metre. The force 
	// required to lift the device off the surface is defined by dead_height * sticky_stiffness. 
	// See also	StickyHeightmapSurfaceInfo.
	class StickyHeightmapSurface : public HeightmapSurface 
	{
	protected:
		// See Surface::Contact
		class Contact : public HeightmapSurface::Contact 
		{
		public:
			inline						Contact(StickyHeightmapSurface* s);	// Constructor
			inline						Contact() {}							// Constructor

			inline virtual void			Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement);// Evaluate is a virtual function defined by sub-classes to produce the force rendering of the surface. See Surface::Contact::Evaluate
			
		public:
			bool						m_sticking;								// Boolean which is true if we are stuck to the surface
			mgFloat						m_deadHeight;							// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created. See also StickyHeightmapSurfaceInfo
			mgFloat						m_stickyStiffness;					// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created. See also StickyHeightmapSurfaceInfo
		};

	public:
										StickyHeightmapSurface();				// Constructor

		StickyHeightmapSurfaceInfo*	GetSurfaceInfo();						// Returns the SurfaceInfo which holds the parameters for the surface.

		virtual Surface::Contact*		NewContact();							// New contact. 
		virtual Surface::Contact*		CopyContact();							// Copy contact. 
	};

	//! Inline methods
	inline StickyFrictionalSurface::Contact::Contact(StickyFrictionalSurface* s) : FrictionalSurface::Contact(s), 
																					   m_sticking(false), 
																					   m_deadHeight(s->GetSurfaceInfo()->GetDeadHeight()),
													   								   m_stickyStiffness(s->GetSurfaceInfo()->GetStickyStiffness())
	{
	}

	inline void StickyFrictionalSurface::Contact::Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement) 
	{
		Vec3f f(finger.x,finger.y,finger.z + m_deadHeight);
		
		if (!m_sticking) 
		{
			if (f.z < 0.0) 
			{
				m_sticking = true;
				FrictionalSurface::Contact::Evaluate(f,ds,dt,st_origin,force,proxy_movement);
			}
			else
			{
				force = Vec3f(0.0,0.0,0.0);
				proxy_movement = Vec2f(f.x,f.y);
			}
		}
		else
		{
			if (f.z < 0.0) 
			{
				FrictionalSurface::Contact::Evaluate(f,ds,dt,st_origin,force,proxy_movement);
			}
			else
			{
				FrictionalSurface::Contact::Evaluate(Vec3f(f.x,f.y,0.0),ds,dt,st_origin,force,proxy_movement);
				force -= f * m_stickyStiffness * m_deviceMaxStiffness;
			}
		}
	}

	inline StickyHeightmapSurface::Contact::Contact(StickyHeightmapSurface* s) : HeightmapSurface::Contact(s), 
																					 m_sticking(false), 
																					 m_deadHeight(s->GetSurfaceInfo()->GetDeadHeight()),
																					 m_stickyStiffness(s->GetSurfaceInfo()->GetStickyStiffness())
	{
	}
	 	  
	inline void StickyHeightmapSurface::Contact::Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement) 
	{
		Vec3f f(finger.x,finger.y,finger.z + m_deadHeight);
		
		if (!m_sticking) 
		{
			if (f.z < 0.0) 
			{
				m_sticking = true;
				HeightmapSurface::Contact::Evaluate(f,ds,dt,st_origin,force,proxy_movement);
			}
			else
			{
				force = Vec3f(0.0,0.0,0.0);
				proxy_movement = Vec2f(f.x,f.y);
			}
		}
		else
		{
			if (f.z < 0.0) 
			{
				HeightmapSurface::Contact::Evaluate(f,ds,dt,st_origin,force,proxy_movement);
			}
			else
			{
				HeightmapSurface::Contact::Evaluate(Vec3f(f.x,f.y,0.0),ds,dt,st_origin,force, proxy_movement);
				force -= f * m_deviceMaxStiffness * m_stickyStiffness;
			}
		}
	}
}

#endif
