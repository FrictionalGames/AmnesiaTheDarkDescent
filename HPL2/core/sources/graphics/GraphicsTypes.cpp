/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "graphics/GraphicsTypes.h"

#include "graphics/Texture.h"
#include "graphics/FrameBuffer.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// GLOBAL FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	tVertexElementFlag GetVertexElementFlagFromEnum(eVertexBufferElement aElement)
	{
		switch(aElement)
		{
		case eVertexBufferElement_Normal:			return eVertexElementFlag_Normal;
		case eVertexBufferElement_Position:			return eVertexElementFlag_Position;
		case eVertexBufferElement_Color0:			return eVertexElementFlag_Color0;
		case eVertexBufferElement_Color1:			return eVertexElementFlag_Color1;
		case eVertexBufferElement_Texture1Tangent:	return eVertexElementFlag_Texture1;
		case eVertexBufferElement_Texture0:			return eVertexElementFlag_Texture0;
		case eVertexBufferElement_Texture1:			return eVertexElementFlag_Texture1;
		case eVertexBufferElement_Texture2:			return eVertexElementFlag_Texture2;
		case eVertexBufferElement_Texture3:			return eVertexElementFlag_Texture3;
		case eVertexBufferElement_Texture4:			return eVertexElementFlag_Texture4;
		case eVertexBufferElement_User0:			return eVertexElementFlag_User0;
		case eVertexBufferElement_User1:			return eVertexElementFlag_User1;
		case eVertexBufferElement_User2:			return eVertexElementFlag_User2;
		case eVertexBufferElement_User3:			return eVertexElementFlag_User3;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	int GetVertexFormatByteSize(eVertexBufferElementFormat aFormat)
	{
		switch(aFormat)
		{
		case eVertexBufferElementFormat_Float:	return sizeof(float);
		case eVertexBufferElementFormat_Int:	return sizeof(int);
		case eVertexBufferElementFormat_Byte:	return sizeof(char);
		}
		
		return 0;
	}

	int GetVertexElementTextureUnit(eVertexBufferElement aElement)
	{
		switch(aElement)
		{
		case eVertexBufferElement_Texture1Tangent:	return 1;
		case eVertexBufferElement_Texture0:			return 0;
		case eVertexBufferElement_Texture1:			return 1;
		case eVertexBufferElement_Texture2:			return 2;
		case eVertexBufferElement_Texture3:			return 3;
		case eVertexBufferElement_Texture4:			return 4;
		}
		return -1;
	}

	//-----------------------------------------------------------------------

	int GetChannelsInPixelFormat(ePixelFormat aFormat)
	{
		switch(aFormat)
		{
		case ePixelFormat_Unknown:			return 0;
		case ePixelFormat_Alpha:			return 1;
		case ePixelFormat_Luminance:		return 1;
		case ePixelFormat_LuminanceAlpha:	return 2;
		case ePixelFormat_RGB:				return 3;
		case ePixelFormat_RGBA:				return 4;
		case ePixelFormat_BGR:				return 3;
		case ePixelFormat_BGRA:				return 4;
		case ePixelFormat_DXT1:				return 4;//Not entirely true... can be 3 too...
		case ePixelFormat_DXT2:				return 4;
		case ePixelFormat_DXT3:				return 4;
		case ePixelFormat_DXT4:				return 4;
		case ePixelFormat_DXT5:				return 4;
		case ePixelFormat_Depth16:			return 1;
		case ePixelFormat_Depth24:			return 1;
		case ePixelFormat_Depth32:			return 1;
		case ePixelFormat_Alpha16:			return 1;
		case ePixelFormat_Luminance16:		return 1;
		case ePixelFormat_LuminanceAlpha16:	return 2;
		case ePixelFormat_RGB16:			return 3;
		case ePixelFormat_RGBA16:			return 4;
		case ePixelFormat_Alpha32:			return 1;
		case ePixelFormat_Luminance32:		return 1;
		case ePixelFormat_LuminanceAlpha32:	return 2;
		case ePixelFormat_RGB32:			return 3;
		case ePixelFormat_RGBA32:			return 4;
		}	
		return 0;
	}

	//-----------------------------------------------------------------------
	int GetBytesPerPixel(ePixelFormat aFormat)
	{
		switch(aFormat)
		{
		case ePixelFormat_Unknown:			return 0;
		case ePixelFormat_Alpha:			return 1;
		case ePixelFormat_Luminance:		return 1;
		case ePixelFormat_LuminanceAlpha:	return 2;
		case ePixelFormat_RGB:				return 3;
		case ePixelFormat_RGBA:				return 4;
		case ePixelFormat_BGR:				return 3;
		case ePixelFormat_BGRA:				return 4;
		case ePixelFormat_DXT1:				return 1; //Not true... is 0.5
		case ePixelFormat_DXT2:				return 1;
		case ePixelFormat_DXT3:				return 1;
		case ePixelFormat_DXT4:				return 1;
		case ePixelFormat_DXT5:				return 1;
		case ePixelFormat_Depth16:			return 2;
		case ePixelFormat_Depth24:			return 3;
		case ePixelFormat_Depth32:			return 4;
		case ePixelFormat_Alpha16:			return 2;
		case ePixelFormat_Luminance16:		return 2;
		case ePixelFormat_LuminanceAlpha16:	return 4;
		case ePixelFormat_RGB16:			return 6;
		case ePixelFormat_RGBA16:			return 8;
		case ePixelFormat_Alpha32:			return 4;
		case ePixelFormat_Luminance32:		return 4;
		case ePixelFormat_LuminanceAlpha32:	return 8;
		case ePixelFormat_RGB32:			return 12;
		case ePixelFormat_RGBA32:			return 16;
		}	
		return 0;
	}
	
	//-----------------------------------------------------------------------
	bool PixelFormatIsCompressed(ePixelFormat aFormat)
	{
		switch(aFormat)
		{
		case ePixelFormat_DXT1:				return true;
		case ePixelFormat_DXT2:				return true;
		case ePixelFormat_DXT3:				return true;
		case ePixelFormat_DXT4:				return true;
		case ePixelFormat_DXT5:				return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------
	bool PixelFormatIsDepth(ePixelFormat aFormat)
	{
		switch(aFormat)
		{
		case ePixelFormat_Depth16:			return true;
		case ePixelFormat_Depth24:			return true;
		case ePixelFormat_Depth32:			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------

	bool PixelFormatIsFloatingPoint(ePixelFormat aFormat)
	{
		switch(aFormat)
		{
			case ePixelFormat_Alpha16:			return true;
			case ePixelFormat_Luminance16:		return true;
			case ePixelFormat_LuminanceAlpha16:	return true;
			case ePixelFormat_RGB16:			return true;
			case ePixelFormat_RGBA16:			return true;
			case ePixelFormat_Alpha32:			return true;
			case ePixelFormat_Luminance32:		return true;
			case ePixelFormat_LuminanceAlpha32:	return true;
			case ePixelFormat_RGB32:			return true;
			case ePixelFormat_RGBA32:			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// VIDEO MODE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVideoMode::cVideoMode(const cVector2l& avScreenSize, int alBitsPerPixel, int alRefreshRate) : mvScreenSize(avScreenSize),
																								   mlBitsPerPixel(alBitsPerPixel),
																								   mlRefreshRate(alRefreshRate),
																								   mlDisplay(0),
																								   mbCustom(false)
	{
	}

	cVideoMode::cVideoMode(int alDisplay, const cVector2l& avScreenSize, int alBitsPerPixel, int alRefreshRate) : mvScreenSize(avScreenSize),
																												  mlBitsPerPixel(alBitsPerPixel),
																												  mlRefreshRate(alRefreshRate),
																												  mlDisplay(alDisplay),
																												  mbCustom(false)
	{
	}

	cVideoMode::cVideoMode(const cVector2l& avScreenSize) : mvScreenSize(avScreenSize),
															mlBitsPerPixel(-1),
															mlRefreshRate(-1),
															mlDisplay(0),
															mbCustom(true)
	{
	}

	cVideoMode::cVideoMode() : mvScreenSize(cVector2l(-1,-1)),
							   mlBitsPerPixel(-1),
							   mlRefreshRate(-1),
							   mlDisplay(0),
							   mbCustom(true)
	{
	}

	bool cVideoMode::operator==(const hpl::cVideoMode &o) const
	{
		return mlDisplay == o.mlDisplay && mvScreenSize == o.mvScreenSize;
	}

	//-----------------------------------------------------------------------	

	//////////////////////////////////////////////////////////////////////////
	// FRAME BUFFER ATTACHMENT
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	iTexture* iFrameBufferAttachment::ToTexture()
	{
		if(GetFrameBufferAttachmentType() != eFrameBufferAttachment_Texture) return NULL;

		return static_cast<iTexture*>(this);
	}

	//-----------------------------------------------------------------------

	iDepthStencilBuffer* iFrameBufferAttachment::ToDepthStencilBuffer()
	{
		if(GetFrameBufferAttachmentType() != eFrameBufferAttachment_RenderBuffer) return NULL;

		return static_cast<iDepthStencilBuffer*>(this);
	}
	
	//-----------------------------------------------------------------------

}
