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

#ifndef HPL_BITMAP_H
#define HPL_BITMAP_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {

	//-----------------------------------------

	class cBitmapData
	{
	public:
		cBitmapData();
		~cBitmapData();

		void SetData(const unsigned char* apData, int alSize);

		unsigned char *mpData;
		int mlSize;	
	};

	//-----------------------------------------
	
	class cBitmap
	{
	public:
		cBitmap();
		~cBitmap();

		inline const cVector3l& GetSize() const { return mvSize;}
		inline int GetWidth() const { return mvSize.x;}
		inline int GetHeight() const { return mvSize.y;}
		inline int GetDepth() const { return mvSize.z;}

		void SetSize(const cVector3l& avSize){ mvSize = avSize; }
        
		inline ePixelFormat GetPixelFormat() const { return mPixelFormat;}
		inline char GetBytesPerPixel() const { return mlBytesPerPixel; }
		inline int GetNumOfMipMaps() const { return mlNumOfMipMaps;}
		inline int GetNumOfImages() const { return mlNumOfImages;}

		void SetPixelFormat(ePixelFormat aFormat){ mPixelFormat = aFormat;}
		void SetBytesPerPixel(char alBpp){ mlBytesPerPixel = alBpp;}
		
		cBitmapData* GetData(int alImage, int alMipMapLevel);
		void SetUpData(int alNumOfImages, int alNumOfMipmaps);
		inline bool IsCompressed() const { return mbDataIsCompressed;}
		void SetIsCompressed(bool abX) { mbDataIsCompressed = abX;}

		inline const tWString& GetFileName()const { return msFileName;}
		void SetFileName(const tWString& asFileName) { msFileName = asFileName;}

		void CreateData(const cVector3l& avSize, ePixelFormat aFormat, int alImage, int alMipMap);
	
		void Clear( const cColor& aColor,int alImage, int alMipMap);

		void Blit(	cBitmap *apSrc,
					const cVector3l& avDestPosition,const cVector3l& avSrcSize,
					const cVector3l& avSrcPosition,
					int alDestImage=0, int alDestMipMap=0,
					int alSrcImage=0, int alSrcMipMap=0);

		
		
	private:
		void CopyPixel(	unsigned char* apDest, ePixelFormat aDestFormat, 
						unsigned char* apSrc, ePixelFormat aSrcFormat);
		unsigned char* ConvertDataToFormat(unsigned char* apPixelData, ePixelFormat aSrcFormat, ePixelFormat aDestFormat);
		unsigned char* ConvertDataToRGBA(unsigned char* apPixelData, ePixelFormat aFormat);

		std::vector<cBitmapData> mvImages;
		bool mbDataIsCompressed;

		tWString msFileName;

		cVector3l mvSize;
		ePixelFormat mPixelFormat;

		char mlBytesPerPixel;
		
		int mlNumOfImages;
		int mlNumOfMipMaps;
	};
};
#endif // HPL_BITMAP_H
