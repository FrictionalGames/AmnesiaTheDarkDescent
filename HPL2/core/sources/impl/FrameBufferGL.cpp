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

#include "impl/FrameBufferGL.h"

#include "system/LowLevelSystem.h"

#include "impl/SDLTexture.h"
#include "impl/LowLevelGraphicsSDL.h"



namespace hpl {
	

	//////////////////////////////////////////////////////////////////////////
	// DEPTH AND STENCIL BUFFER
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cDepthStencilBufferGL::cDepthStencilBufferGL(const cVector2l& avSize, int alDepthBits, int alStencilBits)
		: iDepthStencilBuffer(avSize,alDepthBits,alStencilBits)
	{
		mlHandle =0;

		;
		
		///////////////////////////////////
		// Create packed depth and stencil
		if(alDepthBits>0 && alStencilBits>0)
		{
			///////
			//Check so extension is supported!
			if(!GLEW_EXT_packed_depth_stencil)
			{
				Error("Drivers does not support packed depth and stencil!\n");
                return;
			}
			///////
			//Check so bit sizes are correct
			if(alDepthBits != 24 || alStencilBits != 8)
			{
				Error("Depth-stencil has %d depth bits and %d stencil bits! Only a combination of 24 depth bits and 8 stencil bits is supported!\n",
						mlDepthBits, mlStencilBits);
				return;
			}

			glGenRenderbuffersEXT(1, &mlHandle);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mlHandle);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, avSize.x, avSize.y);
		}
		///////////////////////////////////
		// Create depth buffer
		else if(alDepthBits >0)
		{
			GLenum depthType = 0;
			switch(alDepthBits)
			{
				case 16: depthType = GL_DEPTH_COMPONENT16; break;
				case 24: depthType = GL_DEPTH_COMPONENT24; break;
				case 32: depthType = GL_DEPTH_COMPONENT32; break;
			};

			if(depthType == 0)
			{
				Error("Invalid depth format: %d bits! Could not create depth buffer\n",alDepthBits);
			}
			else
			{
				glGenRenderbuffersEXT(1, &mlHandle);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mlHandle);
				glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, depthType, avSize.x, avSize.y);
			}
		}
		///////////////////////////////////
		// Create stencil buffer
		else if(alStencilBits >0)
		{
			GLenum stencilType = 0;
			switch(alStencilBits)
			{
				case 1:		stencilType = GL_STENCIL_INDEX1_EXT; break;
				case 4:		stencilType = GL_STENCIL_INDEX4_EXT; break;
				case 8:		stencilType = GL_STENCIL_INDEX8_EXT; break;
				case 16:	stencilType = GL_STENCIL_INDEX16_EXT; break;
			};
			
			if(stencilType == 0)
			{
				Error("Invalid stencil format: %d bits! Could not create stencil buffer\n",alStencilBits);
			}
			else
			{
				glGenRenderbuffersEXT(1, &mlHandle);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mlHandle);
				glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, stencilType, avSize.x, avSize.y);
			}
		}

	}

	//-----------------------------------------------------------------------
	
	cDepthStencilBufferGL::~cDepthStencilBufferGL()
	{
		;
		if(mlHandle != 0)	glDeleteRenderbuffersEXT(1, &mlHandle);
	}
	
	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cFrameBufferGL::cFrameBufferGL(const tString& asName, iLowLevelGraphics* apLowLevelGraphics) : iFrameBuffer(asName, apLowLevelGraphics)
	{
		;
		glGenFramebuffersEXT(1, &mlHandle);

		mbIsUpdated = true;

	}

	//-----------------------------------------------------------------------

	cFrameBufferGL::~cFrameBufferGL()
	{
		;
		glDeleteFramebuffersEXT(1, &mlHandle);
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cFrameBufferGL::SetTexture2D(int alColorIdx, iTexture *apTexture, int alMipmapLevel)
	{
		if(CheckIfNullTexture(GL_COLOR_ATTACHMENT0_EXT,alColorIdx,apTexture)) return;

		if(apTexture->GetType() != eTextureType_2D && apTexture->GetType() != eTextureType_Rect) return;

		AttachTexture(GL_COLOR_ATTACHMENT0_EXT,alColorIdx,apTexture,alMipmapLevel,0);
	}

	//-----------------------------------------------------------------------

	void cFrameBufferGL::SetTexture3D(int alColorIdx, iTexture *apTexture, int alZ, int alMipmapLevel)
	{
		if(CheckIfNullTexture(GL_COLOR_ATTACHMENT0_EXT,alColorIdx,apTexture)) return;

		if(apTexture->GetType() != eTextureType_3D) return;

		AttachTexture(GL_COLOR_ATTACHMENT0_EXT,alColorIdx,apTexture,alMipmapLevel,alZ);
	}

	//-----------------------------------------------------------------------

	void cFrameBufferGL::SetTextureCubeMap(int alColorIdx, iTexture *apTexture, int alFace, int alMipmapLevel)
	{
		if(CheckIfNullTexture(GL_COLOR_ATTACHMENT0_EXT,alColorIdx,apTexture)) return;

		if(apTexture->GetType() != eTextureType_CubeMap) return;

		AttachTexture(GL_COLOR_ATTACHMENT0_EXT,alColorIdx,apTexture,alMipmapLevel,alFace);
	}

	//-----------------------------------------------------------------------

	void cFrameBufferGL::SetDepthTexture2D(iTexture *apTexture, int alMipmapLevel)
	{
		if(CheckIfNullTexture(GL_DEPTH_ATTACHMENT_EXT,0,apTexture)) return;

		if(apTexture->GetType() != eTextureType_2D && apTexture->GetType() != eTextureType_Rect) return;
		//TODO: Check so it is a depth texture.
		
		AttachTexture(GL_DEPTH_ATTACHMENT_EXT,0,apTexture,alMipmapLevel,0);
	}
	
	void cFrameBufferGL::SetDepthTextureCubeMap(iTexture *apTexture, int alFace, int alMipmapLevel)
	{
		if(CheckIfNullTexture(GL_DEPTH_ATTACHMENT_EXT,0,apTexture)) return;

		if(apTexture->GetType() != eTextureType_CubeMap) return;
		//TODO: Check so it is a depth texture.

		AttachTexture(GL_DEPTH_ATTACHMENT_EXT,0,apTexture,alMipmapLevel,alFace);
	}

	//-----------------------------------------------------------------------

	void cFrameBufferGL::SetDepthStencilBuffer(iDepthStencilBuffer* apBuffer)
	{
		;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mlHandle);

		////////////////////////////
		//Check if a NULL value was passed, if so remove attachments
		if(apBuffer == NULL)
		{
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,0);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,0);
			return;
		}

		cDepthStencilBufferGL *pBufferSDL = static_cast<cDepthStencilBufferGL*>(apBuffer);

		//////////////////////////////////
		//Attach depth buffer
		if(pBufferSDL->GetDepthBits() > 0)
		{
			glFramebufferRenderbufferEXT(	GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
											GL_RENDERBUFFER_EXT, pBufferSDL->GetHandle());

			mpDepthBuffer = apBuffer;
		}

		//////////////////////////////////
		//Attach stencil buffer
		if(pBufferSDL->GetStencilBits() > 0)
		{
			glFramebufferRenderbufferEXT(	GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, 
											GL_RENDERBUFFER_EXT, pBufferSDL->GetHandle());
			mpStencilBuffer = apBuffer;
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		mbIsUpdated = true;

		//If this is the first thing set to the frame buffer, use that size.
		SetFirstSize(apBuffer->GetSize());
	}

	//-----------------------------------------------------------------------

	bool cFrameBufferGL::CompileAndValidate()
	{
		;

		mbIsUpdated = true;
		bool bRet = false;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mlHandle);
		
		/////////////////////////////////////////
		//Check color buffers
		std::vector<GLenum> vColorBuffers;
		int lNumOfColorBuffers =0;
		for(int i=0; i<kMaxDrawColorBuffers; ++i)
		{
			if(mpColorBuffer[i] != NULL)
			{
				vColorBuffers.push_back(GL_COLOR_ATTACHMENT0_EXT + i);
				++lNumOfColorBuffers;
			}
		}
		//If no color buffers, turn of read and draw buffers.
		if(lNumOfColorBuffers ==0)
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		//One color buffer is active
		else if(lNumOfColorBuffers ==1)
		{
			glDrawBuffer(vColorBuffers[0]);
			glReadBuffer(vColorBuffers[0]);
		}
		//Multiple color buffers 
		else
		{
			glDrawBuffers(lNumOfColorBuffers, &vColorBuffers[0]);
		}
		
		/////////////////////////////////////////
		//Check for errors in fram buffer
		GLenum GLStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(GLStatus == GL_FRAMEBUFFER_COMPLETE_EXT)
		{
			bRet = true;
		}
		//The configuration of the frame buffer is good, but unsupported
		else if(GLStatus == GL_FRAMEBUFFER_UNSUPPORTED_EXT)
		{
			Error("Frame buffer '%s' is not complete! Format is unsupported!\n", msName.c_str());
		}
		//The configuration of the frame buffer is bad
		else
		{
			switch(GLStatus)
			{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				Error("Frame buffer '%s' is not complete! Incomplete attachments!\n", msName.c_str());
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				Error("Frame buffer '%s' is not complete! Missing attachments!\n", msName.c_str());
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				Error("Frame buffer '%s' is not complete! Incomplete dimensions!\n", msName.c_str());
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				Error("Frame buffer '%s' is not complete! Incomplete formats!\n", msName.c_str());
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				Error("Frame buffer '%s' is not complete! Incomplete draw buffer!\n", msName.c_str());
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				Error("Frame buffer '%s' is not complete! Incomplete read buffer!\n", msName.c_str());
				break;
			default:
				Error("Frame buffer '%s' is not complete! Unknown error!\n");
				break;
			};
			
		}
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		
		return bRet;
	}

	//-----------------------------------------------------------------------

	void cFrameBufferGL::PostBindUpdate()
	{
		for(int i=0; i<kMaxDrawColorBuffers; ++i)
		{
			if(mpColorBuffer[i]) PostBindUpdateAttachment(mpColorBuffer[i]);
		}
        if(mpDepthBuffer) PostBindUpdateAttachment(mpDepthBuffer);
		if(mpStencilBuffer) PostBindUpdateAttachment(mpStencilBuffer);
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cFrameBufferGL::PostBindUpdateAttachment(iFrameBufferAttachment *apAttachment)
	{
		if(apAttachment->GetFrameBufferAttachmentType() == eFrameBufferAttachment_RenderBuffer) return;

		iTexture *pTexture = static_cast<iTexture*>(apAttachment);
		if(pTexture->UsesMipMaps())
			pTexture->AutoGenerateMipmaps();
	}

	//-----------------------------------------------------------------------

	bool cFrameBufferGL::CheckIfNullTexture(int alAttachmentType, int alAttachmentIdx,iTexture *apTexture)
	{
		if(apTexture) return false;

		;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mlHandle);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, alAttachmentType + alAttachmentIdx,GL_TEXTURE_2D, 0,0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		
		return true;
	}

	//-----------------------------------------------------------------------
	void cFrameBufferGL::AttachTexture(int alAttachmentType,int alAttachmentIdx,iTexture *apTexture, int alMipmapLevel, int alExtra)
	{
		;

		eTextureType texType = apTexture->GetType();
		GLenum GLTarget = TextureTypeToGLTarget(texType);

		cSDLTexture *pTextureSDL = static_cast<cSDLTexture*>(apTexture); 

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mlHandle);
		

		/////////////////////////////////////////////////
		//2D, Rect or Cube Texture
        if(	texType == eTextureType_2D || 
			texType == eTextureType_Rect || 
			texType == eTextureType_CubeMap)
		{
			glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, alAttachmentType + alAttachmentIdx, 
										texType == eTextureType_CubeMap ? 
											GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + alExtra : GLTarget, 
										pTextureSDL->GetTextureHandle(),alMipmapLevel);

		}
		/////////////////////////////////////////////////
		//3D Texture
		else if(texType == eTextureType_3D)
		{
			glFramebufferTexture3DEXT(	GL_FRAMEBUFFER_EXT, alAttachmentType + alAttachmentIdx, 
										GLTarget, 
										pTextureSDL->GetTextureHandle(),alMipmapLevel,
										alExtra);
		}
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		//////////////////////////////
        //Set the correct buffer pointer
		switch(alAttachmentType)
		{
		case GL_COLOR_ATTACHMENT0_EXT:
			mpColorBuffer[alAttachmentIdx] = apTexture;
			break;
		case GL_DEPTH_ATTACHMENT_EXT:
			mpDepthBuffer = apTexture;
			break;
		case GL_STENCIL_ATTACHMENT_EXT:
			mpStencilBuffer = apTexture;
			break;
		}

		//////////////////////////////////////
		//If this is the first thing set to the frame buffer, use that size.
		SetFirstSize(cVector2l(apTexture->GetSize().x,apTexture->GetSize().y));

		mbIsUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cFrameBufferGL::SetFirstSize(const cVector2l &avSize)
	{	
		if(mvSize.x > -1) return;

        mvSize = avSize;
	}
	
	//-----------------------------------------------------------------------
}
