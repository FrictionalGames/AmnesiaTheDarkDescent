
//! File: <RoughSurface.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_ROUGHSURFACE_H_
#define _HAPTX_ROUGHSURFACE_H_

#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#include "FrictionalSurface.h"

namespace HaptX
{
	// RoughSurface implements an algorithmically textured surface. Mean and deviation 
	// describes a normal (gaussian) probability distribution. This distribution is used 
	// to randomly select a new starting friction when entering dynamic (slipping) contact. 
	// The surface is immediately placed back into static contact. See also RoughSurfaceInfo.
	class RoughSurface : public FrictionalSurface
	{
	protected:
		// See Surface::Contact
		class Contact : public FrictionalSurface::Contact
		{
		public:
			inline							Contact(RoughSurface* s);				// Constructor
			inline							Contact();								// Constructor
			
			inline virtual void				Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement); // Evaluate is a virtual function defined by sub-classes to produce the force rendering of the surface. See also Surface::Contact::Evaluate.
		
		protected:	  
			mgFloat							m_mean;									// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created. See also RoughSurfaceInfo.
			mgFloat							m_deviation;							// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created. See also RoughSurfaceInfo.
		};
		
		inline virtual Surface::Contact*	NewContact();							// New contact. 
		inline virtual Surface::Contact*	CopyContact();							// Copy contact. 

	public:
											RoughSurface();							// Constructor
											RoughSurface(RoughSurfaceInfo* si);		// Constructor

		RoughSurfaceInfo*					GetSurfaceInfo();						// Returns the SurfaceInfo which holds the parameters for the surface.
	};

	inline Surface::Contact* RoughSurface::NewContact()
	{
		return new Contact(this);
	}

	inline Surface::Contact* RoughSurface::CopyContact()
	{
		return CopyContactT(new Contact);
	}
}

#endif
