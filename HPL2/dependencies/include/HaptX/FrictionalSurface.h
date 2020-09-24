
//! File: <FrictionalSurface.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_FRICTIONALSURFACE_H_
#define _HAPTX_FRICTIONALSURFACE_H_

#include "SimpleSurface.h"

namespace HaptX
{
	// FrictionalSurface extends the SimpleSurface with static and dynamic frictional properties.
	//
	// The starting friction is the coefficient of static friction required to move from static-contact to dynamic
	// (kinetic/coulomb) contact. When the proxy is in static contact with the surface, a tangential force of 
	// m_startingFriction * (f*n) is required to induce slipping.
	// 
	// Conversely, stopping friction is the coefficient of static friction required to move from dynamic-contact 
	// to static contact. When the proxy is in dynamic contact with the surface, a tangential force of at least 
	// m_stoppingFriction * (f*n) is required to continue slipping.
	// 
	// The dynamic friction is the coefficient of dynamic friction. When slipping, the tangential force is retarded by
	// m_dynamicFriction * (f*n).
	// 
	// The m_stiffnessT is the tangential stiffness used to calculate tangential frictional forces.
	// See also FrictionalSurfaceInfo.
	class FrictionalSurface : public SimpleSurface 
	{
	public:
							FrictionalSurface();							// Constructor
							FrictionalSurface(FrictionalSurfaceInfo* si);	// Constructor
				
							FrictionalSurfaceInfo*	GetSurfaceInfo();		// Returns the SurfaceInfo which holds the parameters for the surface.

	protected:
		// See Surface::Contact
		class Contact : public SimpleSurface::Contact 
		{  
		public:
			inline			Contact(FrictionalSurface* s);					// Constructor
			inline			Contact() {}									// The void constructor is required in all sub-classes.
		  
			virtual void	Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement);	// Evaluate to produces the rendering force and updates the proxy position. See Surface::Contact::Evaluate
		  
			mgFloat			m_startingFriction;								// Holds a copy of the SurfaceInfo value, see FrictionalSurfaceInfo.
			mgFloat			m_stoppingFriction;								// Holds a copy of the SurfaceInfo value, see FrictionalSurfaceInfo.
			mgFloat			m_dynamicFriction;								// Holds a copy of the SurfaceInfo value, see FrictionalSurfaceInfo.
			mgFloat			m_stiffnessT;									// Holds a copy of the SurfaceInfo value, see FrictionalSurfaceInfo.
			
			Vec2f			m_stickPoint;									// The texture coordinate of the proxy during static contact.
			Vec2f			m_stProxyMovement;								// The texture coordinates of our last reported proxy movement when in dynamic contact.
			bool			m_stInit;										// Indicates when we go from static to dynamic friction.
			bool			m_slipping;										// Indicates whether we are in dynamic or static contact.
			mgFloat			m_deti;											// The inverse determinant of the [ ds, dt ] 2x2 matrix.
		};

		inline virtual Surface::Contact* NewContact();						// New contact. 
		inline virtual Surface::Contact* CopyContact();						// Copy contact. 
	};

	//!  Inline member functions

	inline FrictionalSurface::Contact::Contact(FrictionalSurface* s) : SimpleSurface::Contact(s),
		m_startingFriction(s->GetSurfaceInfo()->GetStartingFriction()),
		m_stoppingFriction(s->GetSurfaceInfo()->GetStoppingFriction()),
		m_dynamicFriction(s->GetSurfaceInfo()->GetDynamicFriction()),
		m_stiffnessT(s->GetSurfaceInfo()->GetStiffnessT()), 
		m_stInit(true), m_slipping(true)
	{
	}


	inline Surface::Contact* FrictionalSurface::NewContact() 
	{
		return new Contact(this);
	}

	inline Surface::Contact* FrictionalSurface::CopyContact() 
	{
		return CopyContactT(new Contact);
	}
}

#endif
