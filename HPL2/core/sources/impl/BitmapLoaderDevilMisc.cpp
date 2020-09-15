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

#include "impl/BitmapLoaderDevilMisc.h"

#include "graphics/Bitmap.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cBitmapLoaderDevilMisc::cBitmapLoaderDevilMisc() : iBitmapLoaderDevil()
	{
		AddSupportedExtension("jpg");
		AddSupportedExtension("png");
		AddSupportedExtension("tif");
		AddSupportedExtension("bmp");
		AddSupportedExtension("tga");
	}

	cBitmapLoaderDevilMisc::~cBitmapLoaderDevilMisc()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cBitmap* cBitmapLoaderDevilMisc::LoadBitmap(const tWString& asFile, tBitmapLoadFlag aFlags)
	{
		Initialize();

		//create image id
		unsigned int lImageId;
		ilGenImages(1,&lImageId);

		//Bind image
		ilBindImage(lImageId);

		//Try and load the file.
		if(LoadDevilImageW(asFile)==false)
		{
			ilDeleteImages(1,&lImageId);
			return NULL;
		}

		cBitmap *pBitmap = hplNew(cBitmap, () );

		////////////////////////////////////////
		//Get main image properties
		int lNumOfImages = ilGetInteger(IL_NUM_IMAGES);
		int lNumOfMipMaps = 1;//ilGetInteger(IL_NUM_MIPMAPS); //Skip for now...

		//If there is no image series, 0 number of images is returned, this still means that there is an image though
		if(lNumOfImages > 1)
			pBitmap->SetUpData(lNumOfImages, lNumOfMipMaps);

		cVector3l vSize;
		vSize.x = ilGetInteger(IL_IMAGE_WIDTH);
		vSize.y = ilGetInteger(IL_IMAGE_HEIGHT);
		vSize.z = ilGetInteger(IL_IMAGE_DEPTH);

		pBitmap->SetSize(vSize);

		int lBytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
		ePixelFormat pixelFormat = DevilPixelFormatToHPL(ilGetInteger(IL_IMAGE_FORMAT));

		pBitmap->SetBytesPerPixel(lBytesPerPixel);
		pBitmap->SetPixelFormat(pixelFormat);

		pBitmap->SetIsCompressed(false); //Let it be like this, for dds it will be different.

		////////////////////////////////////////
		//Loop through and copy image data
		// If 0 number of images, there is still one image to get, so set up accordingly.
		int lCount = lNumOfImages > 0 ? lNumOfImages : 1;
		for(int image=0; image < lCount; ++image)
		{
			//When 1 image only, no need to set up active image.
			if(lNumOfImages > 1)ilActiveImage(image);

			cBitmapData *pImage = pBitmap->GetData(image,0);

			int lSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
			pImage->SetData(ilGetData(), lSize);
		}

		ilDeleteImages(1,&lImageId);

		return pBitmap;
	}

	//------------------------------------------------------------


	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
}
