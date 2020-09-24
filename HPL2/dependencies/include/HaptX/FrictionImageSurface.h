
//! File: <FrictionImageSurface.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_FRICTIONIMAGESURFACE_H_
#define _HAPTX_FRICTIONIMAGESURFACE_H_

#include "ImageSurface.h"

namespace HaptX 
{
	// FrictionImageSurface is a FrictionalSurface where the starting friction and the dynamic friction is
	// determined by the greyscale component of an image.
	// See also ImageSurfaceInfo.
	class FrictionImageSurface : public ImageSurface 
	{
	protected:
		// See Surface::Contact
		class Contact : public ImageSurface::Contact 
		{
		public:
			inline					Contact(FrictionImageSurface* s) : ImageSurface::Contact(s){}	// Constructor
			inline					Contact()													{}	// Constructor
		 
			virtual void			Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement); // A virtual function defined by sub-classes of Surface to produce the force rendering of the surface. See Also Surface::Contact::Evaluate
		};

	public:
									FrictionImageSurface(HapticTexture* _texture = NULL);			// Constructor
									FrictionImageSurface(ImageSurfaceInfo* si, HapticTexture* _texture = NULL); // Constructor
		
		ImageSurfaceInfo*			GetSurfaceInfo();												// Returns the SurfaceInfo which holds the parameters for the surface.
		
		virtual Surface::Contact*	NewContact();													// New contact. 
		virtual Surface::Contact*	CopyContact();													// Copy contact. 
	};
}

#endif
