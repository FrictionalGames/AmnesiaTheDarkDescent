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

#include "impl/BitmapLoaderDevil.h"

#include "graphics/Bitmap.h"
#include "system/LowLevelSystem.h"

#include "system/String.h"
#include "system/Platform.h"

#ifdef WIN32
#include <io.h>
#endif

namespace hpl {

	bool iBitmapLoaderDevil::mbIsInitialized = false;

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iBitmapLoaderDevil::iBitmapLoaderDevil()
	{
	}
	
	iBitmapLoaderDevil::~iBitmapLoaderDevil()
	{
		
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool iBitmapLoaderDevil::SaveBitmap(cBitmap* apBitmap,const tWString& asFile, tBitmapSaveFlag aFlags)
	{
		Initialize();

		//create image id
		unsigned int lImageId;
		ilGenImages(1,&lImageId);

		//Bind image
		ilBindImage(lImageId);

		////////////////////////////////////////
		//Set the data from bitmap
		ILubyte lBytesPerPixel = apBitmap->GetBytesPerPixel();
		ILenum lILFormat = HPLPixelFormatToDevil(apBitmap->GetPixelFormat());	

		if(lILFormat ==0)
		{
			Error("Could not save image to '%s'! Pixel format of bitmap was not valid!\n",cString::To8Char(asFile).c_str());
			ilDeleteImages(1,&lImageId);
			return false;
		}
		
		ilTexImage(	apBitmap->GetWidth(), apBitmap->GetHeight(),apBitmap->GetDepth(),lBytesPerPixel,
					lILFormat,IL_UNSIGNED_BYTE, apBitmap->GetData(0,0)->mpData);
		
		
		////////////////////////////////////////
		//Save to file
		//Get the type of file
		ILenum lType = FileNameToDevilTypeW(asFile);
		if(lType == 0) {
			Error("Filename %s has not a valid image extension!\n",cString::To8Char(asFile).c_str());
			ilDeleteImages(1,&lImageId);
			return false;
		}
		//Get a pointer to the file
		FILE *pFile = cPlatform::OpenFile(asFile, _W("w+b"));
		if(pFile == NULL){
			Error("Could not open file %s for writing!\n",cString::To8Char(asFile).c_str());
			ilDeleteImages(1,&lImageId);
			return false;
		}

		//Save the image to file
		int	lRet = ilSaveF(lType, pFile);
		if(lRet ==0) Error("Error when saving image '%s' to file!\n",cString::To8Char(asFile).c_str());
		
		//Close file pointer
		fclose(pFile);

		////////////////////////////////////////
		//Exit
		ilDeleteImages(1,&lImageId);
		

		return lRet != 0;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	bool iBitmapLoaderDevil::LoadDevilImageW(const tWString& asFile)
	{
		ILenum lType = FileNameToDevilTypeW(asFile);
		if(lType == 0) {
			Error("Filename %s has not a valid image extension!\n",cString::To8Char(asFile).c_str());
			return false;
		}

		FILE *pFile = cPlatform::OpenFile(asFile, _W("rb"));
		if(pFile == NULL){
			Error("Could not open file %s for reading!\n",cString::To8Char(asFile).c_str());
			return false;
		}
		
		int lRet = ilLoadF(lType, pFile);

		fclose(pFile);

		return lRet;
	}

	//-----------------------------------------------------------------------

	static void ILAPIENTRY   DevilClose(ILHANDLE apHandle)
	{
		fclose((FILE*)apHandle);
	}
	static ILHANDLE ILAPIENTRY DevilOpen(const ILstring apFile)
	{
		return fopen(apFile, "wb");
	}
	static ILint ILAPIENTRY    DevilPutc(ILubyte alData, ILHANDLE apHandle)
	{
		return fputc(alData, (FILE*)apHandle);
	}
	static ILint ILAPIENTRY    DevilSeek(ILHANDLE apHandle, ILint alOffset, ILint alMode)
	{
		return fseek((FILE*)apHandle,alOffset,alMode);
	}
	static ILint  ILAPIENTRY   DevilTell(ILHANDLE apHandle)
	{
		return ftell((FILE*)apHandle);
	}
	static ILint ILAPIENTRY    DevilWrite(const void* apBuffer, ILuint alSize, ILuint alNum, ILHANDLE apHandle)
	{
		return (ILint)fwrite(apBuffer,alSize,alNum,(FILE*)apHandle);
	}
	static ILboolean ILAPIENTRY DevilEof(ILHANDLE apHandle)
	{
		ILuint OrigPos, FileSize;

		// Find out the filesize for checking for the end of file
		OrigPos = DevilTell(apHandle);
		DevilSeek(apHandle, 0, IL_SEEK_END);
		FileSize = DevilTell(apHandle);
		DevilSeek(apHandle, OrigPos, IL_SEEK_SET);

		if (DevilTell(apHandle) >= FileSize) return IL_TRUE;
		return IL_FALSE;
	}

	ILint ILAPIENTRY DevilGetC(ILHANDLE Handle)
	{
		return fgetc((FILE*)Handle);
	}

	ILint ILAPIENTRY DevilRead(void *Buffer, ILuint Size, ILuint Number, ILHANDLE Handle)
	{
		return fread(Buffer, Size, Number, (FILE*)Handle);
	}

	//-----------------------------------------------------------------------

	void iBitmapLoaderDevil::Initialize()
	{
		if(mbIsInitialized==true) return;

		Log(" Initializing DevIL\n");
		ilInit();

		Log("  Vendor String: %s\n",ilGetString(IL_VENDOR));
		Log("  Version String: %s\n", ilGetString(IL_VERSION_NUM));
		int libVersion = ilGetInteger(IL_VERSION_NUM);
		Log("  Version Number: %d\n", libVersion);
		
		if (libVersion != IL_VERSION) {
			Log("  ***** WARNING *****\n");
			Log("  Expecting IL version %d and found version %d\n",IL_VERSION,libVersion);
			Log("  Cube Maps may not load correctly in this version and will not render correctly in game\n");
			Log("  Please use the included version of libIL and not a custom build if you have issues\n");
			Log("  ***** WARNING *****\n");
		}

		ilSetWrite(DevilOpen,DevilClose,DevilPutc,DevilSeek,DevilTell,DevilWrite);
		ilSetRead(DevilOpen,DevilClose,DevilEof,DevilGetC,DevilRead, DevilSeek,DevilTell);

		ilEnable(IL_ORIGIN_SET);
		ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_UPPER_LEFT);

		mbIsInitialized = true;
	}

	//-----------------------------------------------------------------------

	ePixelFormat iBitmapLoaderDevil::DevilPixelFormatToHPL(int alFormat)
	{
		switch(alFormat)
		{
		case IL_RGB:		return ePixelFormat_RGB;
		case IL_RGBA:		return ePixelFormat_RGBA;
		case IL_BGR:		return ePixelFormat_BGR;
		case IL_BGRA:		return ePixelFormat_BGRA;
		case IL_LUMINANCE:	return ePixelFormat_Alpha;	//Not sure this is good.
		}

		return ePixelFormat_Unknown;
	}

	//-----------------------------------------------------------------------

	ILenum iBitmapLoaderDevil::HPLPixelFormatToDevil(ePixelFormat aFormat)
	{
		switch(aFormat)
		{
		case ePixelFormat_RGB:			return IL_RGB;
		case ePixelFormat_RGBA:			return IL_RGBA;
		case ePixelFormat_BGR:			return IL_BGR;
		case ePixelFormat_BGRA:			return IL_BGRA;
		case ePixelFormat_Luminance:	return IL_LUMINANCE;
		}

		return 0;
	}

	//-----------------------------------------------------------------------
	
	ILenum iBitmapLoaderDevil::FileNameToDevilTypeW(const tWString& asFile)
	{
		tWString sExt = cString::ToLowerCaseW(cString::GetFileExtW(asFile));
        if(sExt==_W("")) return 0;
		
		if(sExt == _W("bmp"))	return IL_BMP;
		if(sExt == _W("jpeg"))	return IL_JPG;
		if(sExt == _W("jpg"))	return IL_JPG;
		if(sExt == _W("pcx"))	return IL_PCX;
		if(sExt == _W("png"))	return IL_PNG;
		if(sExt == _W("tga"))	return IL_TGA;
		if(sExt == _W("tif"))	return IL_TIF;
		if(sExt == _W("dds"))	return IL_DDS;
		if(sExt == _W("psd"))	return IL_PSD;
		if(sExt == _W("sgi"))	return IL_SGI;
		
		return 0;
	}

	//-----------------------------------------------------------------------
}
