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

#include "impl/SDLTexture.h"

#include "system/LowLevelSystem.h"
#include "math/Math.h"
#include "graphics/Bitmap.h"

#include "impl/LowLevelGraphicsSDL.h"

#include <cmath>
#include <cstring>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSDLTexture::cSDLTexture(const tString& asName, eTextureType aType, eTextureUsage aUsage, iLowLevelGraphics* apLowLevelGraphics)
				: iTexture(asName,_W(""),aType, aUsage, apLowLevelGraphics)
	{
		mbContainsData = false;
				
		mpGfxSDL = static_cast<cLowLevelGraphicsSDL*>(mpLowLevelGraphics);

		mlTextureIndex = 0;
		mfTimeCount =0;

		mfTimeDir = 1;
	}

	cSDLTexture::~cSDLTexture()
	{
		;

		for(size_t i=0; i<mvTextureHandles.size(); ++i)
		{
			glDeleteTextures(1,(GLuint *)&mvTextureHandles[i]);
		}
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateFromBitmap(cBitmap* apBmp)
	{
		;

		GenerateHandles(1);

        return CreateFromBitmapToIndex(apBmp,0);
	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateAnimFromBitmapVec(std::vector<cBitmap*> *avBitmaps)
	{
		;

		GenerateHandles((int)avBitmaps->size());

		//////////////////////////////////
		//Create textures
		for(size_t i=0; i< mvTextureHandles.size(); ++i)
		{
			if(CreateFromBitmapToIndex( (*avBitmaps)[i], (int)i)==false)
			{
				return false;
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateCubeFromBitmapVec(std::vector<cBitmap*> *avBitmaps)
	{
		;

		/////////////////////////////
		//Check so everything is correct
		if(mUsage == eTextureUsage_RenderTarget || mType != eTextureType_CubeMap)
		{
			return false;
		}

		if(avBitmaps->size()<6){
			Error("Only %d bitmaps supplied for creation of cube map, 6 needed.",avBitmaps->size());
			return false;
		}

		GenerateHandles(1);

		/////////////////////////////
		//Create the faces on the cube
		for(int i=0; i< 6; i++)
		{
			cBitmap *pBmp = (*avBitmaps)[i];

			bool bRet = CreateTexture(	mvTextureHandles[0],
										pBmp->GetData(0,0), pBmp->GetNumOfMipMaps(),
										pBmp->GetSize(), pBmp->GetPixelFormat(),i,
										mbUseMipMaps,true);
			if(bRet==false) return false;
		}

		SetupProperties(mvTextureHandles[0]);

		return true;
	}


	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateFromRawData(const cVector3l &avSize,ePixelFormat aPixelFormat, unsigned char *apData)
	{
		;

		GenerateHandles(1);

		mvSize = avSize;
		mPixelFormat = aPixelFormat;

		//Make sure size is now below 0
		if(mvSize.x<1)mvSize.x=1;
		if(mvSize.y<1)mvSize.y=1;
		if(mvSize.z<1)mvSize.z=1;

		//////////////////////////////
		//Get gl properties
		GLenum GLTarget = TextureTypeToGLTarget(mType);
		
		//////////////////////////////////
		//Create the texture data
		glEnable(GLTarget);
		glBindTexture(GLTarget, mvTextureHandles[0]);

		int lDataSize = avSize.x * avSize.y * avSize.z * GetBytesPerPixel(aPixelFormat);
		bool bRet = CopyTextureDataToGL(mvTextureHandles[0],0,apData, lDataSize, avSize, aPixelFormat,0);
		if(bRet==false)
		{
			glDisable(GLTarget);
			return false;
		}

		mbContainsData = true;
		
		if(mbUseMipMaps)
		{
			GenerateMipMaps(GLTarget,aPixelFormat,avSize,apData,lDataSize,0);
		}
		

		SetupProperties(mvTextureHandles[0]);

		glDisable(GLTarget);
		
		return true;
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetRawData(	int alLevel, const cVector3l& avOffset, const cVector3l& avSize, 
									ePixelFormat aPixelFormat, void *apData)
	{
		;

		if(mbContainsData==false) return;
		if(apData==NULL) return;

		GLenum GLTarget = TextureTypeToGLTarget(mType);
		GLenum GLFormat = PixelFormatToGLFormat(aPixelFormat);

		glEnable(GLTarget);
		glBindTexture(GLTarget, mvTextureHandles[0]);

		if(mType == eTextureType_1D)
		{
			glTexSubImage1D(GLTarget,alLevel,avOffset.x, avSize.x,
							GLFormat,GL_UNSIGNED_BYTE,apData);
		}
		else if(mType == eTextureType_2D || mType == eTextureType_Rect)
		{
			glTexSubImage2D(GLTarget,alLevel,avOffset.x,avOffset.y, avSize.x,avSize.y,
							GLFormat,GL_UNSIGNED_BYTE,apData);
		}
		else if(mType == eTextureType_3D)
		{
			glTexSubImage3D(GLTarget,alLevel,avOffset.x,avOffset.y,avOffset.z, 
							avSize.x,avSize.y,avSize.z,
							GLFormat,GL_UNSIGNED_BYTE,apData);
		}

		glDisable(GLTarget);
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::Update(float afTimeStep)
	{
		if(mvTextureHandles.size() > 1)
		{
			float fMax = (float)(mvTextureHandles.size());
			mfTimeCount += afTimeStep * (1.0f/mfFrameTime) * mfTimeDir;

			if(mfTimeDir > 0)
			{
				if(mfTimeCount >= fMax)
				{
					if(mAnimMode == eTextureAnimMode_Loop)
					{
						mfTimeCount =0;
					}
					else
					{
						mfTimeCount = fMax - 1.0f;
						mfTimeDir = -1.0f;
					}
				}
			}
			else
			{
				if(mfTimeCount < 0)
				{
					mfTimeCount =1;
					mfTimeDir = 1.0f;
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::HasAnimation()
	{
		return mvTextureHandles.size() > 1;
	}
	
	void cSDLTexture::NextFrame()
	{
		mfTimeCount += mfTimeDir;

		if(mfTimeDir > 0)
		{
			float fMax = (float)(mvTextureHandles.size());
			if(mfTimeCount >= fMax)
			{
				if(mAnimMode == eTextureAnimMode_Loop)
				{
					mfTimeCount =0;
				}
				else
				{
					mfTimeCount = fMax - 1.0f;
					mfTimeDir = -1.0f;
				}
			}
		}
		else
		{
			if(mfTimeCount < 0)
			{
				mfTimeCount =1;
				mfTimeDir = 1.0f;
			}
		}
	}

	void cSDLTexture::PrevFrame()
	{
		mfTimeCount -= mfTimeDir;

		if(mfTimeDir < 0)
		{
			float fMax = (float)(mvTextureHandles.size());
			if(mfTimeCount >= fMax)
			{
				if(mAnimMode == eTextureAnimMode_Loop)
				{
					mfTimeCount =0;
				}
				else
				{
					mfTimeCount = fMax - 1.0f;
					mfTimeDir = -1.0f;
				}
			}
		}
		else
		{
			if(mfTimeCount < 0)
			{
				mfTimeCount =1;
				mfTimeDir = 1.0f;
			}
		}
	}
	
	float cSDLTexture::GetT()
	{
		return cMath::Modulus(mfTimeCount,1.0f);
	}

	float cSDLTexture::GetTimeCount()
	{
		return mfTimeCount;
	}
	void cSDLTexture::SetTimeCount(float afX)
	{
		mfTimeCount = afX;
	}
	int cSDLTexture::GetCurrentLowlevelHandle()
	{
		return GetTextureHandle();
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetFilter(eTextureFilter aFilter)
	{
		if(mFilter == aFilter) return;

		;

		mFilter = aFilter;
		if(mbContainsData)
		{
			GLenum GLTarget = GetGLTextureTargetEnum(mType);

			glEnable(GLTarget);
			for(size_t i=0; i < mvTextureHandles.size(); ++i)
			{
				glBindTexture(GLTarget, mvTextureHandles[i]);

				if(mbUseMipMaps && mType != eTextureType_Rect)
				{
					switch(mFilter)
					{
					case eTextureFilter_Nearest:
						glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
						glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						break;
					case eTextureFilter_Bilinear:
						glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
						glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						break;
					case eTextureFilter_Trilinear:
						glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						break;
					}
				}
				else
				{
					switch(mFilter)
					{
					case eTextureFilter_Nearest:
						glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						break;
					case eTextureFilter_Bilinear:
					case eTextureFilter_Trilinear:
						glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						break;
					}
					
				}
			}

			glDisable(GLTarget);
		}
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetAnisotropyDegree(float afX)
	{
		if(mbContainsData==false) return;
		if(!mpLowLevelGraphics->GetCaps(eGraphicCaps_AnisotropicFiltering)) return;
		if(afX < 1.0f) return;
		if(afX > (float) mpLowLevelGraphics->GetCaps(eGraphicCaps_MaxAnisotropicFiltering)) return;

		if(mfAnisotropyDegree == afX) return;

		;

		mfAnisotropyDegree = afX;

		GLenum GLTarget = GetGLTextureTargetEnum(mType);

		glEnable(GLTarget);
		for(size_t i=0; i < mvTextureHandles.size(); ++i)
		{
			glBindTexture(GLTarget, mvTextureHandles[i]);

			glTexParameterf(GLTarget,GL_TEXTURE_MAX_ANISOTROPY_EXT ,mfAnisotropyDegree);
		}

		glDisable(GLTarget);
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetWrapS(eTextureWrap aMode)
	{
		if(mType == eTextureType_Rect) return; //Rect only has one mode! (clamp to edge)

		;

		mWrapS = aMode;

		if(mbContainsData==false) return;
		
		GLenum GLTarget = GetGLTextureTargetEnum(mType);

		glEnable(GLTarget);
		for(size_t i=0; i < mvTextureHandles.size(); ++i)
		{
			glBindTexture(GLTarget, mvTextureHandles[i]);

			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_S,GetGLWrapEnum(aMode));
		}

		glDisable(GLTarget);
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetWrapT(eTextureWrap aMode)
	{
		if(mType == eTextureType_Rect) return; //Rect only has one mode! (clamp to edge)

		;

		mWrapT = aMode;

		if(mbContainsData==false) return;
		
		GLenum GLTarget = GetGLTextureTargetEnum(mType);

		glEnable(GLTarget);
		for(size_t i=0; i < mvTextureHandles.size(); ++i)
		{
			glBindTexture(GLTarget, mvTextureHandles[i]);

			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_T,GetGLWrapEnum(aMode));
		}

		glDisable(GLTarget);
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetWrapR(eTextureWrap aMode)
	{
		if(mType == eTextureType_Rect) return; //Rect only has one mode! (clamp to edge)

		;

		mWrapR = aMode;

		if(mbContainsData==false) return;
		
		GLenum GLTarget = GetGLTextureTargetEnum(mType);

		glEnable(GLTarget);
		for(size_t i=0; i < mvTextureHandles.size(); ++i)
		{
			glBindTexture(GLTarget, mvTextureHandles[i]);

			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_R,GetGLWrapEnum(aMode));
		}

		glDisable(GLTarget);
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetWrapSTR(eTextureWrap aMode)
	{
		if(mType == eTextureType_Rect) return; //Rect only has one mode! (clamp to edge)

		;

		mWrapS = aMode;
		mWrapT = aMode;
		mWrapR = aMode;

		if(mbContainsData==false) return;

		GLenum GLTarget = GetGLTextureTargetEnum(mType);

		glEnable(GLTarget);
		for(size_t i=0; i < mvTextureHandles.size(); ++i)
		{
			glBindTexture(GLTarget, mvTextureHandles[i]);

			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_S,GetGLWrapEnum(aMode));
			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_T,GetGLWrapEnum(aMode));
			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_R,GetGLWrapEnum(aMode));
		}

		glDisable(GLTarget);
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetCompareMode(eTextureCompareMode aMode)
	{
		mCompareMode = aMode;

		if(mbContainsData==false) return;

		;

		GLenum GLTarget = GetGLTextureTargetEnum(mType);
		GLenum GLCompareMode = GetGLTextureCompareMode(mCompareMode);
		
		glEnable(GLTarget);
		for(size_t i=0; i < mvTextureHandles.size(); ++i)
		{
			glBindTexture(GLTarget, mvTextureHandles[i]);

			glTexParameteri(GLTarget,GL_TEXTURE_COMPARE_MODE,GLCompareMode);			
		}
		glDisable(GLTarget);
	}
	
	
	void cSDLTexture::SetCompareFunc(eTextureCompareFunc aFunc)
	{
		mCompareFunc = aFunc;

		if(mbContainsData==false) return;

		;

		GLenum GLTarget = GetGLTextureTargetEnum(mType);
		GLenum GLCompareFunc = GetGLTextureCompareFunc(mCompareFunc);

		glEnable(GLTarget);
		for(size_t i=0; i < mvTextureHandles.size(); ++i)
		{
			glBindTexture(GLTarget, mvTextureHandles[i]);

			glTexParameteri(GLTarget,GL_TEXTURE_COMPARE_FUNC,GLCompareFunc);			
		}
		glDisable(GLTarget);
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::AutoGenerateMipmaps()
	{
		if(mbUseMipMaps==false) return;

		;

		if(GLEW_EXT_framebuffer_object)
		{
			GLenum GLTarget = GetGLTextureTargetEnum(mType);

			glEnable(GLTarget);
			for(size_t i=0; i < mvTextureHandles.size(); ++i)
			{
				glBindTexture(GLTarget, mvTextureHandles[i]);
				glGenerateMipmapEXT(GLTarget);
			}

			glDisable(GLTarget);
		}
	}
	
	//-----------------------------------------------------------------------

	unsigned int cSDLTexture::GetTextureHandle()
	{
		if(mbContainsData==false) return 0;

		size_t lTexHandleNum = mvTextureHandles.size();

		if(lTexHandleNum > 1)
		{
			int lFrame = (int) mfTimeCount;
			return mvTextureHandles[lFrame];
		}
		else if(lTexHandleNum==0)
		{
			return 0;
		}
		else
		{
			return mvTextureHandles[0];
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void cSDLTexture::GenerateHandles(int alNumOfHandles)
	{
		if( (int)mvTextureHandles.size() < alNumOfHandles)
		{
			int lNewHandles = alNumOfHandles - (int)mvTextureHandles.size();
			mvTextureHandles.resize(alNumOfHandles);
			glGenTextures(lNewHandles,(GLuint *)&mvTextureHandles[alNumOfHandles-lNewHandles]);
		}
	}
	
	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateFromBitmapToIndex(cBitmap* apBmp, int alIdx)
	{
		////////////////////////////
		//Create Cubemap texture
		if(mType == eTextureType_CubeMap)
		{
			//Check so there are at least 6 images.
			if(apBmp->GetNumOfImages()<6)
			{
				Error("Bitmap has to few images (%d) to create cubmap texture '%s'\n", apBmp->GetNumOfImages(),msName.c_str());
				return false;
			}

			for(int i=0; i<6; ++i)
			{
				bool bRet = CreateTexture(	mvTextureHandles[alIdx],
											apBmp->GetData(i,0),apBmp->GetNumOfMipMaps(),
											apBmp->GetSize(), apBmp->GetPixelFormat(),
											i,mbUseMipMaps,	true);
				if(bRet == false) return false;
			}


		}
		////////////////////////////
		//Create 1d, 2d or 3d texture
		else
		{
			bool bRet = CreateTexture(	mvTextureHandles[alIdx],
										apBmp->GetData(0,0),apBmp->GetNumOfMipMaps(),
										apBmp->GetSize(), apBmp->GetPixelFormat(),
										0,mbUseMipMaps,true);
			if(bRet == false) return false;
			
		}

		SetupProperties(mvTextureHandles[alIdx]);

		return true;
	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateTexture(	int alTextureHandle,
										cBitmapData* apBitmapImage, int alNumOfMipMaps,
										const cVector3l avSize, ePixelFormat aPixelFormat, 
										int alFaceNum,bool abGenerateMipMaps,
										bool abCheckForResize)
	{
		/////////////////////////////
		//Check so size is power of 2
		if((!cMath::IsPow2(avSize.x) || !cMath::IsPow2(avSize.y)) && mType != eTextureType_Rect)
		{
			Warning("Texture '%s' does not have a pow2 size!\n",msName.c_str());
		}

		//////////////////////////////
		//Setup properties
		mvSize = avSize;
		mPixelFormat = aPixelFormat;
		mbIsCompressed = PixelFormatIsCompressed(aPixelFormat);

		//////////////////////////////
		//Get gl properties
		GLenum GLTarget = TextureTypeToGLTarget(mType);
		GLenum GLFormat = PixelFormatToGLFormat(aPixelFormat);
		
		//The mipmap level in the BitmapImage array that is top level in hierarchy
		int lStartMipMapLevel = 0; 

		/////////////////////////////
		//Check if resize is needed, 
		// cannot do with compressed textures unless they have custom mipmaps
		unsigned char *pResizeData = NULL;
		int lResizeDataSize = 0;
		if(	mlSizeDownScaleLevel > 0 && abCheckForResize && 
			(mbIsCompressed==false || alNumOfMipMaps>1) &&
			PixelFormatIsDepth(aPixelFormat) == false)
		{
			//////////////////////
			//Resize by using a higher level mipmap as start
			if(alNumOfMipMaps > 1)
			{
				if(alNumOfMipMaps-1 < (int)mlSizeDownScaleLevel)
					lStartMipMapLevel = alNumOfMipMaps-1;
				else
					lStartMipMapLevel = (int)mlSizeDownScaleLevel;

				//Change the size
				mvSize.x >>= lStartMipMapLevel;
				mvSize.y >>= lStartMipMapLevel;
				mvSize.z >>= lStartMipMapLevel;

				if(mvSize.x==0) mvSize.x =1;if(mvSize.y==0) mvSize.y =1;if(mvSize.z==0) mvSize.z =1;
			}
			//////////////////////
			//Resize by changing data (do only for 2D textures)
			else if((mType == eTextureType_2D || mType == eTextureType_CubeMap) &&
					mvSize.x > mvMinDownScaleSize.x &&	mvSize.y > mvMinDownScaleSize.y )
			{
				//Shrink the size as much as possible until minimum is reached
				cVector3l vNewSize = mvSize;
				for(int i=0; i<(int)mlSizeDownScaleLevel; ++i)
				{
					vNewSize.x >>= 1;
					vNewSize.y >>= 1;

					if(vNewSize.x <= mvMinDownScaleSize.x || mvSize.y <= mvMinDownScaleSize.y) break;
				}

				//Create new data and resize current data to it.
				int lChannels = GetBytesPerPixel(aPixelFormat);
				lResizeDataSize = vNewSize.x * vNewSize.y * lChannels;
				pResizeData = hplNewArray(unsigned char, lResizeDataSize);

				
				gluScaleImage(	GLFormat,mvSize.x, mvSize.y, GL_UNSIGNED_BYTE, apBitmapImage->mpData,
								vNewSize.x, vNewSize.y,GL_UNSIGNED_BYTE,pResizeData);

				mvSize = vNewSize;
			}
		}

		//////////////////////////////////
		//Bind texture
		glEnable(GLTarget);
		glBindTexture(GLTarget, alTextureHandle);

		//Log("Texture: '%s' StartLevel: %d Mipmap num: %d GenerateMipmaps: %d\n", msName.c_str(), lStartMipMapLevel,alNumOfMipMaps, abGenerateMipMaps);
		
		/////////////////////////////
		//Create texture, including mipmaps if generating.
		bool bRet = true;
		int lMipMapCount = abGenerateMipMaps ? alNumOfMipMaps : lStartMipMapLevel+1;
		cVector3l vSize = mvSize;
		for(int i=lStartMipMapLevel; i<lMipMapCount; ++i)
		{
			if(vSize.x==0) vSize.x =1;
			if(vSize.y==0) vSize.y =1;
			if(vSize.z==0) vSize.z =1;

			int lImageMipMap = i;
			int lCount = i-lStartMipMapLevel;

			//Log(" %d, %d\n", lCount, lImageMipMap);


			unsigned char *pData = apBitmapImage[lImageMipMap].mpData;
			int lSize = apBitmapImage[lImageMipMap].mlSize;

			//Use resized data if available
			if(lCount==0 && pResizeData)
			{
				//Log("Using new data! Size: %dx%d data: %d\n",vSize.x, vSize.y,pResizeData);
				pData = pResizeData;
				lSize = lResizeDataSize;
			}
			
			if(CopyTextureDataToGL(	alTextureHandle,lCount,pData,lSize, vSize,aPixelFormat,alFaceNum) == false)
			{
				bRet = false;
				break;
			}

			vSize.x >>= 1;
			vSize.y >>= 1;
			vSize.z >>= 1;
		}

		//Check for errors
		if(bRet==false)
		{
			glDisable(GLTarget);
			Error("Could not create GL texture %s\n",msName.c_str());
			return false;
		}

		//Mark that the texture got some data now.
		mbContainsData = true;

		//////////////////////////////////
		//Generate mipmaps
		if(abGenerateMipMaps && alNumOfMipMaps <= 1)
		{
			if(pResizeData)
				GenerateMipMaps(GLTarget,aPixelFormat,mvSize,pResizeData,lResizeDataSize,alFaceNum);
			else
				GenerateMipMaps(GLTarget,aPixelFormat,mvSize,apBitmapImage->mpData,apBitmapImage->mlSize,alFaceNum);
		}

		//Destroy resized data if available
		if(pResizeData) hplDeleteArray(pResizeData);


		glDisable(GLTarget);

		return true;
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::GenerateMipMaps(	GLenum aGLTarget, ePixelFormat aPixelFormat,const cVector3l avSize, 
										unsigned char *apData,int alDataSize, int alFaceNum)
	{
		GLenum GLFormat = PixelFormatToGLFormat(aPixelFormat);
		GLenum GLInternalFormat = PixelFormatToGLInternalFormat(aPixelFormat);
		int lBytesPerPixel = GetBytesPerPixel(aPixelFormat);

		////////////////////////////////
		// Generate mipmaps if the format is not compressed.
		if(mUsage == eTextureUsage_RenderTarget)
		{
			if(GLEW_EXT_framebuffer_object)
			{
				glGenerateMipmapEXT(aGLTarget);
			}
			else
			{
				mbUseMipMaps = false;	
			}
		}
		else if(PixelFormatIsCompressed(aPixelFormat)==false)
		{
			if(mType == eTextureType_1D)
			{
				gluBuild1DMipmaps(	aGLTarget,GLInternalFormat,avSize.x,
									GLFormat, GL_UNSIGNED_BYTE, 
									apData);
			}
			else if(mType == eTextureType_2D ||	mType == eTextureType_CubeMap)
			{
				gluBuild2DMipmaps(	mType == eTextureType_CubeMap ?  
									GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + alFaceNum : aGLTarget,
									GLInternalFormat,avSize.x,avSize.y,
									GLFormat, GL_UNSIGNED_BYTE, 
									apData);
			}
			

			//Calculate memory taken by mipmaps
			cVector3l vTempSize = avSize;
			while(vTempSize.x>1 || vTempSize.y>1 || vTempSize.z>1)
			{
				vTempSize.x >>= 1;
				vTempSize.y >>= 1;
				vTempSize.z >>= 1;
				if(vTempSize.x==0)vTempSize.x=1; if(vTempSize.y==0)vTempSize.y=1; if(vTempSize.z==0)vTempSize.z=1;
				
				mlMemorySize += vTempSize.x * vTempSize.y * vTempSize.z * lBytesPerPixel;
			}
		}
		////////////////////////////////
		// Mipmaps could not be generated, turn off their usage.
		else
		{
			mbUseMipMaps = false;
		}
	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::CopyTextureDataToGL(	int alTextureHandle, int alLevel,unsigned char *apData,int alDataSize,
											const cVector3l avSize, ePixelFormat aPixelFormat,int alFaceNum)
	{
		GLenum GLTarget = TextureTypeToGLTarget(mType);
		GLenum GLFormat = PixelFormatToGLFormat(aPixelFormat);
		GLenum GLInternalFormat = PixelFormatToGLInternalFormat(aPixelFormat);
		

		//Clear gl errors.
		while(glGetError()!=GL_NO_ERROR);

		//Update memory size
		mlMemorySize += alDataSize;

		/////////////////////////////////////////
		// Load compressed data
		if(PixelFormatIsCompressed(aPixelFormat))
		{
			GLenum GLCompressionFormat = GetGLCompressionFormatFromPixelFormat(aPixelFormat);

			if(mType == eTextureType_1D)
			{
				glCompressedTexImage1DARB(	GLTarget, alLevel, 
					GLCompressionFormat, 
					avSize.x,
					0, alDataSize, apData);
			}
			else if(mType == eTextureType_2D || mType == eTextureType_CubeMap)
			{
				glCompressedTexImage2DARB(	mType == eTextureType_CubeMap ?  
					GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + alFaceNum : GLTarget, 
					alLevel, 
					GLCompressionFormat, 
					avSize.x, avSize.y,
					0, alDataSize, apData);
			}
			else if(mType == eTextureType_3D)
			{
				glCompressedTexImage3DARB(	GLTarget, alLevel, 
					GLCompressionFormat, 
					avSize.x, avSize.y,avSize.z,
					0, alDataSize, apData);
			}
		}
		/////////////////////////////////////////
		// Load uncompressed data
		else
		{
			GLenum glType = PixelFormatIsFloatingPoint(aPixelFormat) ? GL_FLOAT : GL_UNSIGNED_BYTE;

			if(mType == eTextureType_1D)
			{
				glTexImage1D(	GLTarget, alLevel, GLInternalFormat, 
					avSize.x,0,GLFormat,
					glType, apData);
			}
			else if(mType == eTextureType_2D || mType == eTextureType_Rect ||
				mType == eTextureType_CubeMap)
			{
				glTexImage2D(	mType == eTextureType_CubeMap ?  
					GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + alFaceNum : GLTarget, 
					alLevel, GLInternalFormat, 
					avSize.x, avSize.y,
					0, GLFormat, glType, apData);
			}
			else if(mType == eTextureType_3D)
			{
				glTexImage3D(	GLTarget, alLevel, 
					GLInternalFormat, avSize.x, avSize.y,avSize.z,
					0, GLFormat, glType, apData);
			}
		}

		//Check if something went wrong
		if(glGetError()!=GL_NO_ERROR) return false;

		return true;
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetupProperties(int alTextureHandle)
	{
		if(mbContainsData==false) return;
		
		GLenum GLTarget = TextureTypeToGLTarget(mType);

		glEnable(GLTarget);
		glBindTexture(GLTarget, alTextureHandle);

		/////////////////////////////////////////
		//Filtering
		if(mbUseMipMaps && mType != eTextureType_Rect)
		{
			if(mFilter == eTextureFilter_Bilinear)
				glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			else
				glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else{
			glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//////////////////////
		// Rect (force clamp to edge skip anisotropy)
        if(mType == eTextureType_Rect)
		{
			mWrapS = eTextureWrap_ClampToEdge;
			mWrapT = eTextureWrap_ClampToEdge;
			mWrapR = eTextureWrap_ClampToEdge;

			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_S,GetGLWrapEnum(mWrapS));
			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_T,GetGLWrapEnum(mWrapT));
			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_R,GetGLWrapEnum(mWrapR));
		}
		/////////////////////
		// Normal
		else
		{
			/////////////////////////////////////////
			//Wrapping
			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_S,GetGLWrapEnum(mWrapS));
			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_T,GetGLWrapEnum(mWrapT));
			glTexParameteri(GLTarget,GL_TEXTURE_WRAP_R,GetGLWrapEnum(mWrapR));

			/////////////////////////////////////////
			//Anisotropic filtering
			if(	mpLowLevelGraphics->GetCaps(eGraphicCaps_AnisotropicFiltering) &&
				(float) mpLowLevelGraphics->GetCaps(eGraphicCaps_MaxAnisotropicFiltering) >= mfAnisotropyDegree)
			{
				glTexParameterf(GLTarget,GL_TEXTURE_MAX_ANISOTROPY_EXT ,mfAnisotropyDegree);
			}
		}

		glDisable(GLTarget);
	}

	//-----------------------------------------------------------------------

	unsigned char* cSDLTexture::ResizePixelData(unsigned char *apData, int alBytesPerPixel)
	{
		if(mlSizeDownScaleLevel<=0 || mvSize.x <= mvMinDownScaleSize.x) return NULL;

		unsigned char *pNewSrc = NULL;
		
		int lOldW = mvSize.x;
		int lOldH = mvSize.y;

		int lSizeDiv = (int)powf((int)2,(int)mlSizeDownScaleLevel);

		mvSize.x /= lSizeDiv;
		mvSize.y /= lSizeDiv;

		while(mvSize.x < mvMinDownScaleSize.x)
		{
			mvSize.x*=2;
			mvSize.y*=2;
			lSizeDiv/=2;
		}

		pNewSrc = hplNewArray( unsigned char, alBytesPerPixel * mvSize.x * mvSize.y);

		int lWidthCount = mvSize.x;
		int lHeightCount = mvSize.y;
		int lOldAdd = alBytesPerPixel*lSizeDiv;
		int lOldHeightAdd = alBytesPerPixel*lOldW*(lSizeDiv-1);

		unsigned char *pOldPixel = apData;
		unsigned char *pNewPixel = pNewSrc;

		while(lHeightCount)
		{
			memcpy(pNewPixel, pOldPixel,alBytesPerPixel);

			pOldPixel += lOldAdd;
			pNewPixel += alBytesPerPixel;

			lWidthCount--;
			if(!lWidthCount)
			{
				lWidthCount = mvSize.x;
				lHeightCount--;
				pOldPixel += lOldHeightAdd;
			}
		}

		return pNewSrc;
	}

	//-----------------------------------------------------------------------
}
