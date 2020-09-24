
//! File: <HapticTexture.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_HAPTICTEXTURE_H_
#define _HAPTX_HAPTICTEXTURE_H_

#include "HXEngine.h"
#include "HXSolver/mgMath.h"

namespace HaptX
{	
	class RGBA
    {
	public:
		inline RGBA(){}
        inline RGBA(const mgFloat& _r, const mgFloat& _g, const mgFloat& _b, const mgFloat& _a = 1)
		{
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}

	public:
        mgFloat r, g, b, a;
    };


	// HapticTexture defines HaptX textures.
	// The constructor takes a void pointer to the input texture and parameters that
	// describe how the texture data is stored. Please refer to HapticTexture.h for a detailed 
	// description of how to format texture data. 
	//! For example: a two-dimensional image with 256 x 256 pixels stored in the ARGB format, where 
	//! each component is represented with 8 bits, will have the following setup:
	//!  |--- A ---|--- R ---|--- G ---|--- B ---|
	//!  |0000 0000|0000 0000|0000 0000|0000 0000|
	//!  ^MSB                               LSB^
	//!  ^Index = 31                 Index = 0 ^

	// Parameter example:
	// width  = 256
	// height = 256
	// depth  = 0
	// components    = 4 
	// redBits       = 8
	// redStartBit   = 16
	// greenBits     = 8 
	// greenStartBit = 8
	// blueBits      = 8
	// blueStartBit  = 0
	// alphaBits     = 8
	// alphaStartBit = 24
	// data = pointer_to_data
	class HAPTX_ENGINE_SPEC HapticTexture
	{
	public:
		
		// Constructor.
		// Parameters: width - The width of the texture.
		// height - The height of the texture.
		// depth - The depth of the texture.
		// components - The number of components in the image (RGB = 3, RGBA = 4)
		// redBits - The number of red bits in the image.
		// redStartBit - The start index of the red bits.
		// greenBits - The number of green bits in the image.
		// greenStartBit - The start index of the green bits.
		// blueBits - The number of blue bits in the image.
		// blueStartBit - The start index of the blue bits.
		// alphaBits - The number of alpha bits in the image.
		// alphaStartBit - The start index of the alpha bits.
		// data - A pointer to the texture data.
		HapticTexture(int width, int height, int depth, int components, 
					  int redBits, int redStartBit, int greenBits, int greenStartBit, 
					  int blueBits, int blueStartBit, int alphaBits, int alphaStartBit, void* data);
		
		
		// Constructor for HapticTexture.
		// Parameters: width - The width of the texture.
		// height - The height of the texture.
		// components - The number of components in the image (RGB = 3, RGBA = 4)
		// data - A pointer to the texture data.
		HapticTexture(int width, int height, int components, unsigned char* data);

		~HapticTexture();
		
		int		GetWidth();			// Get the width of the texture. 
		int		GetHeight();		// Get the height of the texture. 
		int		GetDepth();			// Get the depth of the texture.
		int		GetNrDimensions();	// Get the number of dimensions of the texture.
		void	InvertColors();		// Invert the pixel colors

		// Get the RGBA value of a specific pixel in the texture.
		// Parameters: x - The x coordinate of the pixel.
		// y - The y coordinate of the pixel.
		// z - The z coordinate of the pixel.
		RGBA GetPixelRGBA(int x = 0, int y = 0, int z = 0);

	protected:
		int GetBitMask(int StartBit, int BitCount);

	protected:
		void* m_rawData;
		
		int	m_nrDimensions;
		int	m_width;
		int	m_depth;
		int	m_height;
		
		int	m_components;				// Number of components in the image. E.g. an RGB image has three components and an RGBA has four.
		int	m_bitsPerPixel;				// Number of bits per component in the image.
		int	m_redBits;					// Number of red bits per pixel in the image.
		int	m_greenBits;				// Number of green bits per pixel in the image.
		int m_blueBits;					// Number of blue bits per pixel in the image.
		int m_alphaBits;				// Number of alpha bits per pixel in the image.
		int m_redStartBit;
		int m_greenStartBit;
		int m_blueStartBit;
		int m_alphaStartBit;
		
		int m_bytesPerPixel;
		int m_redMask;
		int m_greenMask;
		int m_blueMask;
		int m_alphaMask;

		bool m_useUnsignedC;
		bool m_invertPixels;
	};
}

#endif
