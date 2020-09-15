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

#include "graphics/Bitmap.h"

#include "system/LowLevelSystem.h"

#include <memory>
#include <cstring>


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// BITMAP DATA
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cBitmapData::cBitmapData()
	{
		mpData = NULL;
		mlSize =0;
	}
	
	//-----------------------------------------------------------------------

	cBitmapData::~cBitmapData()
	{
		if(mpData) hplDeleteArray( mpData );
	}
	

	//-----------------------------------------------------------------------

	void cBitmapData::SetData(const unsigned char* apData, int alSize)
	{
		if(mpData && mlSize != alSize) hplDelete( mpData );
		
		mlSize = alSize;
		mpData = hplNewArray(unsigned char, mlSize);
        
		memcpy(mpData, apData, mlSize);
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cBitmap::cBitmap()
	{
		mvImages.resize(1);
		
		mbDataIsCompressed = false;

		mvSize = 0;
		mPixelFormat = ePixelFormat_Unknown;

		mlBytesPerPixel =0;

		msFileName = _W("");

		mlNumOfMipMaps =1;
		mlNumOfImages =1;
	}

	//-----------------------------------------------------------------------

	cBitmap::~cBitmap()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cBitmapData* cBitmap::GetData(int alImage, int alMipMapLevel)
	{
		if(alImage >= mlNumOfImages) return NULL;
		if(alMipMapLevel >= mlNumOfMipMaps) return NULL;

		return &mvImages[alImage * mlNumOfMipMaps + alMipMapLevel];
	}
	
	//-----------------------------------------------------------------------

	void cBitmap::SetUpData(int alNumOfImages, int alNumOfMipmaps)
	{
		mlNumOfImages = alNumOfImages;
		mlNumOfMipMaps = alNumOfMipmaps;

		mvImages.resize(mlNumOfImages * mlNumOfMipMaps);
	}

	//-----------------------------------------------------------------------

	void cBitmap::CreateData(const cVector3l& avSize, ePixelFormat aFormat, int alImage, int alMipMap)
	{
		mvSize = avSize;
		if(mvSize.x<=0)mvSize.x =1;
		if(mvSize.y<=0)mvSize.y =1;
		if(mvSize.z<=0)mvSize.z =1;

		mPixelFormat = aFormat;
		mlBytesPerPixel = GetChannelsInPixelFormat(aFormat);

        int lDataSize = mvSize.x * mvSize.y * mvSize.z * mlBytesPerPixel;
		
		cBitmapData* pData = GetData(alImage, alMipMap);
		pData->mpData = hplNewArray(unsigned char, lDataSize);
		pData->mlSize = lDataSize;
	}

	//-----------------------------------------------------------------------

	void cBitmap::Clear( const cColor& aColor,int alImage, int alMipMap)
	{
		if(mbDataIsCompressed) return; //Cannot draw to compressed data!

		int lDataCount = mvSize.x * mvSize.y * mvSize.z;

		unsigned char *pPixelData = GetData(alImage, alMipMap)->mpData;
		
		unsigned char vClearColor[] = {	FloatColorToUChar(aColor.r), 
										FloatColorToUChar(aColor.g),
										FloatColorToUChar(aColor.b),
										FloatColorToUChar(aColor.a)
										};
		unsigned char *pClearData = ConvertDataToFormat(vClearColor,ePixelFormat_RGBA,mPixelFormat);

		while(lDataCount)
		{
			memcpy(pPixelData,pClearData,mlBytesPerPixel);
			pPixelData += mlBytesPerPixel;

			--lDataCount;
		}
	}

	//-----------------------------------------------------------------------

	
	void cBitmap::Blit(	cBitmap *apSrcBmp, const cVector3l& avDestPosition,const cVector3l& avSrcSize,
						const cVector3l& avSrcPosition,
						int alDestImage, int alDestMipMap,
						int alSrcImage, int alSrcMipMap)
	{
		if(mbDataIsCompressed) return; //Cannot blit compressed data!

		////////////////////////////////////////
		//Check so positions are in bounds
		if(	avDestPosition.x >= mvSize.x || avDestPosition.y >= mvSize.y || 
			avDestPosition.z >= mvSize.z)
		{
			return;
		}
		if(	avSrcPosition.x >= apSrcBmp->GetSize().x || avSrcPosition.y >= apSrcBmp->GetSize().y || 
			avSrcPosition.z >= apSrcBmp->GetSize().z)
		{
			return;
		}


		////////////////////////////////////////
		//Check so source size is not too large for source
		
		// Get bounds and check so they are in range
		int lSrcWidth = avSrcSize.x >	apSrcBmp->GetSize().x ?	apSrcBmp->GetSize().x : avSrcSize.x;
		int lSrcHeight = avSrcSize.y >	apSrcBmp->GetSize().y ?	apSrcBmp->GetSize().y : avSrcSize.y;
		int lSrcDepth = avSrcSize.z >	apSrcBmp->GetSize().z ?	apSrcBmp->GetSize().z : avSrcSize.z;

		if(lSrcWidth <= 0)	lSrcWidth =1;
		if(lSrcHeight <= 0) lSrcHeight =1;
		if(lSrcDepth <= 0)	lSrcDepth =1;

		// Get coordinates and check for negative source coordinates
		cVector3l vSrcPos = avSrcPosition;
		if(vSrcPos.x < 0) {
			lSrcWidth += vSrcPos.x; vSrcPos.x =0;
		}
		if(vSrcPos.y < 0) {
			lSrcHeight += vSrcPos.y; vSrcPos.y =0;
		}
		if(vSrcPos.z < 0) {
			lSrcDepth += vSrcPos.z; vSrcPos.z =0;
		}

		// Check if image is outside of bounds in source
		if(vSrcPos.x + lSrcWidth > apSrcBmp->GetSize().x)		lSrcWidth = apSrcBmp->GetSize().x - vSrcPos.x;
		if(vSrcPos.y + lSrcHeight > apSrcBmp->GetSize().y)		lSrcHeight = apSrcBmp->GetSize().y - vSrcPos.y;
		if(vSrcPos.z + lSrcDepth > apSrcBmp->GetSize().z)		lSrcDepth = apSrcBmp->GetSize().z - vSrcPos.z;

		////////////////////////////////////////
		//Check so source size is not too large for destination
		
		//Get coordinates and check for negative
		cVector3l vDestPos = avDestPosition;
		if(vDestPos.x < 0) {
			lSrcWidth += vDestPos.x; 
			vSrcPos.x -= vDestPos.x;
			vDestPos.x =0;
		}
		if(vDestPos.y < 0) {
			lSrcHeight += vDestPos.y; 
			vSrcPos.y -= vDestPos.y;
			vDestPos.y =0;
		}
		if(vDestPos.z < 0) {
			lSrcDepth += vDestPos.z;
			vSrcPos.z -= vDestPos.z;
			vDestPos.z =0;
		}

		// Check if image is outside of bounds in destination
		if(vDestPos.x + lSrcWidth > mvSize.x)		lSrcWidth = mvSize.x - vDestPos.x;
		if(vDestPos.y + lSrcHeight > mvSize.y)		lSrcHeight = mvSize.y - vDestPos.y;
		if(vDestPos.z + lSrcDepth > mvSize.z)		lSrcDepth = mvSize.z - vDestPos.z;

		//If any size dimension is zero, then we skip drawing
		if(lSrcWidth <=0 || lSrcHeight <=0 || lSrcDepth <=0) 
		{
			return;
		}

		////////////////////////////////////////
		//Get data from destination and source
        unsigned char *pSrcData = apSrcBmp->GetData(alSrcImage, alSrcMipMap)->mpData;
		int lSrcPixelSize = apSrcBmp->GetBytesPerPixel();

		unsigned char *pDestData = GetData(alSrcImage, alSrcMipMap)->mpData;
		int lDestPixelSize = GetBytesPerPixel();

		////////////////////////////////////////
		//Set up variables for iteration
		int lSrcAdress = vSrcPos.x + (vSrcPos.y * apSrcBmp->GetSize().x) + (vSrcPos.z * apSrcBmp->GetSize().x * apSrcBmp->GetSize().y);
		lSrcAdress *= lSrcPixelSize;
		int lDestAdress =  vDestPos.x + (vDestPos.y * mvSize.x) + (vDestPos.z * mvSize.x * mvSize.y);
		lDestAdress *= lDestPixelSize;

		unsigned char* pSrcPixel = &pSrcData[lSrcAdress];
		unsigned char* pDestPixel = &pDestData[lDestAdress];
		
        int lWidthCount = lSrcWidth;		
		int lHeightCount = lSrcHeight;
		int lDepthCount = lSrcDepth;

		int lSrcWidthAdd = (apSrcBmp->GetWidth() - lSrcWidth) * lSrcPixelSize;
		int lDestWidthAdd = (mvSize.x - lSrcWidth) * lDestPixelSize;

		int lSrcHeightAdd = (apSrcBmp->GetHeight() - lSrcHeight) * apSrcBmp->GetWidth() * lSrcPixelSize;
		int lDestHeightAdd = (mvSize.y - lSrcHeight) * mvSize.x * lSrcPixelSize;

		ePixelFormat srcPixelFormat = apSrcBmp->GetPixelFormat();

		bool bOutOfBounds = false;
		while(lDepthCount)
		{
			//Log("x:%d y:%d\n",lWidthCount, lHeightCount);
			CopyPixel(pDestPixel, mPixelFormat, pSrcPixel, srcPixelFormat);
			
			
			pSrcPixel += lSrcPixelSize;
			pDestPixel += lDestPixelSize;

			--lWidthCount;
			if(!lWidthCount)
			{
				--lHeightCount;
				lWidthCount = lSrcWidth;

				pSrcPixel += lSrcWidthAdd;
				pDestPixel += lDestWidthAdd;

				if(!lHeightCount)
				{
					--lDepthCount;
					lHeightCount = lSrcHeight;

					pSrcPixel += lSrcHeightAdd;
					pDestPixel += lDestHeightAdd;
				}
			}
		}

		/*if(bOutOfBounds)
		{
				Log(" (%d, %d, %d) (%d, %d, %d) (%dx%dx%d) (%dx%dx%d) (%dx%dx%d)\n",	avDestPosition.x,avDestPosition.y, avDestPosition.z,
																					avSrcPosition.x,avSrcPosition.y, avSrcPosition.z,
																					avSrcSize.x, avSrcSize.y, avSrcSize.z,
																					apSrcBmp->GetSize().x, apSrcBmp->GetSize().y, apSrcBmp->GetSize().z,
																					GetSize().x, GetSize().y, GetSize().z
													);
		}*/
	}
	
	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	//TODO: Make this inline? If so, at top off cpp and not in header
	void cBitmap::CopyPixel(	unsigned char* apDest, ePixelFormat aDestFormat, 
								unsigned char* apSrc, ePixelFormat aSrcFormat)
	{
		int lChannels = GetChannelsInPixelFormat(aDestFormat);
		memcpy(apDest, ConvertDataToFormat(apSrc,aSrcFormat, aDestFormat), lChannels);
	}

	//-------------------------------------------------------------------------

	//TODO: Make this inline?
	static unsigned char gvTempPixelData1[4];
	static unsigned char gvTempPixelData2[4];
	unsigned char* cBitmap::ConvertDataToFormat(unsigned char* apPixelData, ePixelFormat aSrcFormat, ePixelFormat aDestFormat)
	{
		if(aSrcFormat == aDestFormat) return apPixelData;
		
		// Make it into a general RGBA format
		unsigned char *pSrcPixel = ConvertDataToRGBA(apPixelData, aSrcFormat);
		unsigned char *pDestPixel = gvTempPixelData2;
		
		////////////////////////////////////////
		// Convert into wanted format
		switch(aDestFormat)
		{
		case ePixelFormat_Alpha:
			pDestPixel[0] = pSrcPixel[3];
			break;
		case ePixelFormat_Luminance:
			pDestPixel[0] = pSrcPixel[0];
			break;
		case ePixelFormat_LuminanceAlpha:
			pDestPixel[0] = pSrcPixel[0];
			pDestPixel[1] = pSrcPixel[3];
			break;
		case ePixelFormat_RGB:
			pDestPixel[0] = pSrcPixel[0];
			pDestPixel[1] = pSrcPixel[1];
			pDestPixel[2] = pSrcPixel[2];
			break;
		case ePixelFormat_RGBA:
			pDestPixel[0] = pSrcPixel[0];
			pDestPixel[1] = pSrcPixel[1];
			pDestPixel[2] = pSrcPixel[2];
			pDestPixel[3] = pSrcPixel[3];
			break;
		case ePixelFormat_BGR:
			pDestPixel[0] = pSrcPixel[2];
			pDestPixel[1] = pSrcPixel[1];
			pDestPixel[2] = pSrcPixel[0];
			break;
		case ePixelFormat_BGRA:
			pDestPixel[0] = pSrcPixel[2];
			pDestPixel[1] = pSrcPixel[1];
			pDestPixel[2] = pSrcPixel[0];
			pDestPixel[3] = pSrcPixel[3];
			break;
		}

		return pDestPixel;
	}

	//-----------------------------------------------------------------------
	unsigned char* cBitmap::ConvertDataToRGBA(unsigned char* apPixelData, ePixelFormat aFormat)
	{
		unsigned char *pTempPixel = gvTempPixelData1;
		switch(aFormat)
		{
		case ePixelFormat_Alpha:
			pTempPixel[0] = 255;
			pTempPixel[1] = 255;
			pTempPixel[2] = 255;
			pTempPixel[3] = *apPixelData;
			break;
		case ePixelFormat_Luminance:
			pTempPixel[0] = *apPixelData;
			pTempPixel[1] = *apPixelData;
			pTempPixel[2] = *apPixelData;
			pTempPixel[3] = 255;
			break;
		case ePixelFormat_LuminanceAlpha:
			pTempPixel[0] = apPixelData[0];
			pTempPixel[1] = apPixelData[0];
			pTempPixel[2] = apPixelData[0];
			pTempPixel[3] = apPixelData[1];
			break;
		case ePixelFormat_RGB:
			pTempPixel[0] = apPixelData[0];
			pTempPixel[1] = apPixelData[1];
			pTempPixel[2] = apPixelData[2];
			pTempPixel[3] = 255;
			break;
		case ePixelFormat_RGBA:
			pTempPixel[0] = apPixelData[0];
			pTempPixel[1] = apPixelData[1];
			pTempPixel[2] = apPixelData[2];
			pTempPixel[3] = apPixelData[3];
			break;
		case ePixelFormat_BGR:
			pTempPixel[0] = apPixelData[2];
			pTempPixel[1] = apPixelData[1];
			pTempPixel[2] = apPixelData[0];
			pTempPixel[3] = 255;
			break;
		case ePixelFormat_BGRA:
			pTempPixel[0] = apPixelData[2];
			pTempPixel[1] = apPixelData[1];
			pTempPixel[2] = apPixelData[0];
			pTempPixel[3] = apPixelData[3];
			break;
		}

		return pTempPixel;
	}

	//-----------------------------------------------------------------------
}
