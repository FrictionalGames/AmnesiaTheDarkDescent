
//! File: <SimpleSurface.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_SIMPLESURFACE_H_
#define _HAPTX_SIMPLESURFACE_H_

#include "HXEngine.h"
#include "HXSolver/Surface.h"
#include "SurfaceInfo.h"

namespace HaptX
{  
	// The most basic HaptX Surface. The stiffness parameter is defined in Newtons/metre.  
	// It specifies surface stiffness in direction of the surface normal.
	// The damping parameter is used to specify damping associated with surface collision.
	// It is generated in the direction of the normal component of the motion. Damping is 
	// an important property of real world surfaces and defines their energy absorbing 
	// characteristics. It is defined in Newton seconds / metre.
	// See also	SimpleSurfaceInfo
	class SimpleSurface : public Surface
	{
	public:
		inline								SimpleSurface();						// Constructor.
		inline								SimpleSurface(SimpleSurfaceInfo* si);	// Constructor.
		virtual								~SimpleSurface();						// Destructor.
		
		inline SimpleSurfaceInfo*			GetSurfaceInfo();						// Returns the SurfaceInfo which holds the parameters for the surface.

	protected:
		// See Surface::Contact
		class Contact : public Surface::Contact
		{
		public:
			inline							Contact(SimpleSurface* s);				// Constructor.
			inline							Contact();								// The void constructor is required in all sub-classes.
	  
			inline virtual void				Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement);// Backwards compatibility function.  See also Surface::Contact::Evaluate.
			virtual void					Evaluate0D(const Vec3f& finger, Vec3f& force, Vec2f& proxy_movement);// The force rendering function. See also Surface::Contact::Evaluate.
			
			Vec3f							m_lastFinger;							// The unconstrained position from the previous HaptX loop.
			mgFloat							m_lastZ;			
			mgFloat							m_stiffness;							// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created. See also SimpleSurfaceInfo.
			mgFloat							m_damping;								// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created. See also SimpleSurfaceInfo.
			bool							m_allowStick;							// Stickiness true/false. False by default.
			mgFloat							m_velocity;								// The vertical velocity in metres per second. Valid immediately after the evaluate call.
		};	

		inline virtual Surface::Contact*	NewContact();							// New contact. 
		inline virtual Surface::Contact*	CopyContact();							// Copy contact. 
		
	protected:
		SimpleSurfaceInfo*					m_surfaceInfo;							// A pointer to the SurfaceInfo.
	};


	inline SimpleSurface::SimpleSurface() 
	{
		m_supportsTex0D = true;
		m_supportsTex2D = true;
		m_surfaceInfo = new SimpleSurfaceInfo();
	}

	inline SimpleSurface::SimpleSurface(SimpleSurfaceInfo* si) 
	{
		m_supportsTex0D = true;
		m_supportsTex2D = true;
		m_surfaceInfo = si;
	}

	inline SimpleSurface::~SimpleSurface() 
	{
		if (m_surfaceInfo) delete m_surfaceInfo;
	}

	inline SimpleSurfaceInfo* SimpleSurface::GetSurfaceInfo()
	{
		return m_surfaceInfo;
	}

	inline SimpleSurface::Contact::Contact(SimpleSurface* s) :
			Surface::Contact(s),
			m_lastFinger(Vec3f(0.0,0.0,0.0)),
			m_allowStick(false),
			m_stiffness(s->m_surfaceInfo->GetStiffness()),
			m_damping(s->m_surfaceInfo->GetDamping()),
			m_velocity(0.0)
	{
	}

	//! The void constructor is required in all sub-classes.
	inline SimpleSurface::Contact::Contact()
	{
	}

	inline void SimpleSurface::Contact::Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement)
	{
		Evaluate0D(finger,force,proxy_movement);
	}

	inline Surface::Contact* SimpleSurface::NewContact()
	{
		return new Contact(this);
	}

	inline Surface::Contact* SimpleSurface::CopyContact()
	{
		return CopyContactT(new Contact);
	}
}

#endif
