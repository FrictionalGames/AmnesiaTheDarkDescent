
//! File: <HeightmapSurface.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_HEIGHTMAPSURFACE_H_
#define _HAPTX_HEIGHTMAPSURFACE_H_

#include "VariableSurface.h"
#include "HapticTexture.h"

namespace HaptX
{
	// A HeightmapSurface uses an image as a height map.
	// If the image is not a greyscale image, only the BLUE channel is
	// used. The image describes a mapping from texture data to
	// height. Heightmap images are not restricted to be of dimension
	// 2^n by 2^m as OpenGL textures are.
	//
	// The bump height describes the maximum height range of the Heightmap. 
	// The byte value of each pixel indicates the depth below the normal surface.
	// An 0xff pixel (white) is below the surface and 0x00 (black) 
	// is right on the surface. See also HeightmapSurfaceInfo.
	class HeightmapSurface : public VariableSurface
	{
	protected:
		// See Surface::Contact
		class Contact : public VariableSurface::Contact
		{
		public:
			inline								Contact(HeightmapSurface* s);		// Constructor
			inline								Contact();							// The void constructor is required in all sub-classes.
  
			virtual std::pair<mgFloat, mgFloat> Sample(const mgFloat& d, const Vec2f& p) const;		// See VariableSurface::Contact::Sample.
			virtual std::pair<mgFloat, mgFloat> Sample3D(const mgFloat& d, const Vec3f& p) const;	// See VariableSurface::Contact::Sample3D.
  
			inline HeightmapSurface*			BmSurface() const;					// Returns the surface instance. 
        };

		friend class Contact;

		// Elevation holds the elevation values of the surface for each texture coordinate.
		class Elevation  
		{
		public:
			inline						Elevation(HeightmapSurfaceInfo* si);		// Constructor

			mgFloat						GetElevation(const mgFloat& tex);			// Get the bump elevation in metres at the given texture coordinate.
			mgFloat						GetElevation(const Vec2f& tex);				// Get the bump elevation in metres at the given texture coordinate.
			mgFloat						GetElevation(const Vec3f& tex) const;		// Get the bump elevation in metres at the given texture coordinate.

			virtual void				Update();

		protected:	
			friend class HeightmapSurface;

			//! TODO (ida): texture wrapping alternatives.
			mgFloat						GetPixel(int x, int y, int z);				// Get a pixel in the range 0-1. 

		protected:
			mgFloat						m_bumpHeight;								// Holds a copy of the SurfaceInfo value. Updated every time UpdateFields is called.
			int							m_width; 									// The width of the image. Updated from the HapticTexture every time Elevation::Update is called.
			int							m_height; 									// The height of the image. Updated from the HapticTexture every time Elevation::Update is called.
			int							m_depth; 									// The depth of the image. Updated from the HapticTexture every time Elevation::Update is called.
			
			std::vector<mgFloat>		m_pkElevation;								// A vector with the elevation values.
			CLockData					m_lock;										// Lock
			HeightmapSurfaceInfo*		m_surfaceInfo;								// A pointer to the SurfaceInfo.
		};

	public:
										HeightmapSurface(HeightmapSurfaceInfo* si, HapticTexture* img = NULL);	// Constructor
										HeightmapSurface(HapticTexture* img = NULL);							// Constructor
		virtual							~HeightmapSurface();													// Destructor

		HeightmapSurfaceInfo*			GetSurfaceInfo();							// Returns the SurfaceInfo which holds the parameters for the surface.
	
	protected:
	
		virtual Surface::Contact*		NewContact();								// New contact. 
		virtual Surface::Contact*		CopyContact();								// Copy contact. 
		
		inline virtual void				UpdateFields();								// UpdateFields is called once per main loop. See also Surface::UpdateFields. 

		Elevation*						m_elevation;								// Pointer to the Elevation object.

	};

	//! Inline member methods

	inline void HeightmapSurface::UpdateFields()
	{
		m_elevation->m_bumpHeight = GetSurfaceInfo()->GetBumpHeight();
	}

	inline HeightmapSurface::Contact::Contact(HeightmapSurface* s) : VariableSurface::Contact(s)
	{
		BmSurface()->m_elevation->Update();
	}

	inline HeightmapSurface::Contact::Contact()
	{
	}

	inline HeightmapSurface* HeightmapSurface::Contact::BmSurface() const
	{
		return static_cast<HeightmapSurface*>(Surface::Contact::m_surface);
	}

	inline HeightmapSurface::Elevation::Elevation(HeightmapSurfaceInfo* si) : m_width(0), m_height(0), m_depth(0) 
	{
		m_surfaceInfo = si;
		m_pkElevation.clear();
	}
}

#endif
