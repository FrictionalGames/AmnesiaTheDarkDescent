
//! File: <ImageSurface.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_IMAGESURFACE_H_
#define _HAPTX_IMAGESURFACE_H_

#include "FrictionalSurface.h"
#include "HapticTexture.h"
//! TODO (lukas): #include "TextureTransform.h"

namespace HaptX
{
    // Base class for HaptX Surfaces that use texture data as input to create surface effects.
    class ImageSurface : public FrictionalSurface
    {
    protected:
		// See Surface::Contact
        class Contact : public FrictionalSurface::Contact
        {
        public:
									Contact(ImageSurface* s);			// Constructor
									Contact();							// The void constructor is required in all sub-classes.
        protected:	  
			mgFloat*				m_pkImageData;						// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created.
			int						m_iWidth;							// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created.
			int						m_iHeight;							// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created.
			int						m_iDepth;							// Holds a copy of the SurfaceInfo value. Updated every time a new Contact is created.
        };

    public:
									ImageSurface(ImageSurfaceInfo* si, HapticTexture* _texture); // Constructor
		virtual						~ImageSurface();					// Destructor
		
		ImageSurfaceInfo*			GetSurfaceInfo();					// Returns the SurfaceInfo which holds the parameters for the surface.
        
		void						UpdateTexture();					// Updates the texture if it has been changed.

        virtual Surface::Contact*	NewContact();						// New contact. 
        virtual Surface::Contact*	CopyContact();						// Copy contact. 

	public:
 		mgFloat*					m_pkImageData;						// Pointer to the image data.
    };
}

#endif
