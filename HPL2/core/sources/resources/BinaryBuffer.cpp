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

#include "resources/BinaryBuffer.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/Platform.h"
#include <cstring>

#include "math/CRC.h"

// Include SDL Endian code
#ifdef USE_SDL2
#include <SDL2/SDL_endian.h>
#else
#include <SDL/SDL_endian.h>
#endif

#ifdef WIN32
	#define ZLIB_WINAPI
#endif
#include <zlib.h>

// @todo Evil quick and dirty check to Prevent me from building 64-bit until I fix this code
SDL_COMPILE_TIME_ASSERT(int, sizeof(int) == 4);
SDL_COMPILE_TIME_ASSERT(short, sizeof(short) == 2);
SDL_COMPILE_TIME_ASSERT(float, sizeof(float) == 4);
SDL_COMPILE_TIME_ASSERT(char, sizeof(char) == 1);

typedef union {
    float f;
    int i;
} FloatSwabber;

inline static int SwabFloat32(float f) {
    FloatSwabber dat1;
    dat1.f = f;
    return SDL_SwapLE32(dat1.i);
}
inline static float UnSwabFloat32(int i) {
    FloatSwabber dat1;
    dat1.i = i;
    dat1.i = SDL_SwapLE32(dat1.i);
    return dat1.f;
}


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cBinaryBuffer::cBinaryBuffer()
	{
		InitAndAllocData();
	}

	cBinaryBuffer::cBinaryBuffer(const tWString& asFile)
	{
		msFile = asFile;

		InitAndAllocData();
	}

	cBinaryBuffer::~cBinaryBuffer()
	{
		hplFree(mpData);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cBinaryBuffer::Load()
	{
		if(msFile == _W(""))
		{
			Error("Could not save binary file. File not set!\n");
			return false;
		}

		

		return Load(msFile);
	}
	
	//-----------------------------------------------------------------------

	bool cBinaryBuffer::Load(const tWString& asFile)
	{
		////////////////////////////
		// Open file
		FILE *pFile = cPlatform::OpenFile(asFile,_W("rb"));
		if(pFile==NULL)
		{
			Error("Could not open binary file '%s'\n", cString::To8Char(asFile).c_str());
			return false;
		}

		////////////////////////////
		// Get file size
		fseek(pFile,0,SEEK_END);
		size_t lFileSize = ftell(pFile);
		rewind(pFile);

		////////////////////////////
		// Set up memory
		hplFree(mpData);
		mpData = (char*)hplMalloc(lFileSize);
		mlDataSize = lFileSize;
		mlReservedDataSize = lFileSize;
		mlDataPos =0;

		////////////////////////////
		// Load data from file
		size_t lOut = fread(mpData, mlDataSize, 1, pFile);
		fclose(pFile);

		//No bytes written
		if(lOut == 0)
		{
			Error("Could not read from binary file '%s'\n", cString::To8Char(asFile).c_str());
			return false;
		}
		
		return true;
	}
	
	//-----------------------------------------------------------------------

	bool cBinaryBuffer::Save()
	{
		if(msFile == _W(""))
		{
			Error("Could not save binary file. File not set!\n");
			return false;
		}

		return Save(msFile);
	}

	//-----------------------------------------------------------------------

	bool cBinaryBuffer::Save(const tWString& asFile)
	{
		FILE *pFile = cPlatform::OpenFile(asFile,_W("wb"));
		if(pFile==NULL)
		{
			Error("Could not open binary file '%s'\n", cString::To8Char(asFile).c_str());
			return false;
		}
		
		size_t lOut = fwrite(mpData, mlDataSize, 1, pFile);
		fclose(pFile);

		//No bytes written
		if(lOut == 0)
		{
			Error("Could not write to binary file '%s'\n", cString::To8Char(asFile).c_str());
			return false;
		}
				
		return true;	
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::LoadFromCharEncode(const tString& asInputData, size_t alSize)
	{
		////////////////////////////
		// Set up memory
		hplFree(mpData);
		mpData = (char*)hplMalloc(alSize);
		mlDataSize = alSize;
		mlReservedDataSize = alSize;
		mlDataPos =0;

		////////////////////////////
		// Load from string
		cString::DecodeDataFromTextString(asInputData, mpData, alSize);
	}

	//-----------------------------------------------------------------------


	void cBinaryBuffer::SaveToCharEncode(tString& asOutputData)
	{
		cString::EncodeDataToTextString(mpData, mlDataSize, asOutputData);
	}

	//-----------------------------------------------------------------------

	bool cBinaryBuffer::Reserve(size_t alSize)
	{
		if(alSize <= mlReservedDataSize) return false;
		
		char* pNewData = (char*)hplRealloc(mpData, alSize);
		if(pNewData==NULL) return false;

		mpData = pNewData;
		mlReservedDataSize = alSize;

		return true;
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::Clear()
	{
		hplFree(mpData);
		
		InitAndAllocData();
	}

	//-----------------------------------------------------------------------

	bool cBinaryBuffer::SetPos(size_t alPos)
	{
		if(alPos >= mlDataSize) return false;
		
		mlDataPos = alPos;
		return true;
	}

	//-----------------------------------------------------------------------

	bool cBinaryBuffer::AddPos(size_t alSize)
	{
		if(mlDataPos + alSize >= mlDataSize) return false;

		mlDataPos += alSize;
		return true;
	}

	//-----------------------------------------------------------------------

	bool cBinaryBuffer::CompressAndAdd(char *apSrcData, size_t alSize, int alCompressionLevel, bool abWriteDataSize)
	{
		///////////////////////////
		// Check the parameters
		if(apSrcData==NULL) return false;
		if(alCompressionLevel>9) return false;

		///////////////////////////
		// Write a dummy size for data
		size_t lStartPos = mlDataPos;
		if(abWriteDataSize)
		{
			AddInt32(0);
		}

		///////////////////////////
		// Set up
		const size_t lMaxChunkSize = 65536;
		char vOutData[lMaxChunkSize];

		z_stream zipStream;
		zipStream.zalloc = Z_NULL;
		zipStream.zfree = Z_NULL;
		zipStream.opaque = Z_NULL;
		
		///////////////////////////
		// Init compression
		int ret = deflateInit(&zipStream, alCompressionLevel<0 ? Z_DEFAULT_COMPRESSION : alCompressionLevel);
		if (ret != Z_OK) return false;

		zipStream.avail_in = alSize;
		zipStream.next_in = (Bytef *)apSrcData;

		///////////////////////////
		// Compress, chunk by chunk
		do
		{
			//Set current output chunk
			zipStream.avail_out = lMaxChunkSize;
			zipStream.next_out = (Bytef *)&vOutData[0];

			//Compress as much as possible to current chunk
			int lRet = deflate(&zipStream, Z_FINISH);
			if(lRet == Z_STREAM_ERROR)
			{
				deflateEnd(&zipStream);
				return false;
			}

			//Write the chunk to the destination buffer
			size_t lBytesCopied = lMaxChunkSize - zipStream.avail_out;
	
			AddData(vOutData, lBytesCopied);
		}
		while (zipStream.avail_out == 0);

		///////////////////////////
		// Exit and clean up.
		deflateEnd(&zipStream);

		///////////////////////////
		// Write a dummy size for data
		if(abWriteDataSize)
		{
			size_t lTotalDataSize = mlDataPos - lStartPos - 4;
			
			int *pSizeDataPtr = (int*)mpData[lStartPos];
			*pSizeDataPtr = lTotalDataSize;
		}

		//Log("Compress Size: %d\n", mlDataPos - lStartPos);

		return true;
	}

	//-----------------------------------------------------------------------

	bool cBinaryBuffer::DecompressAndAdd(char *apSrcData, size_t alSize)
	{
		///////////////////////////
		// Check the parameters
		if(apSrcData==NULL) return false;

		
		///////////////////////////
		// Set up
		const size_t lMaxChunkSize = 65536;
		char vOutData[lMaxChunkSize];

		z_stream zipStream;
		zipStream.zalloc = Z_NULL;
		zipStream.zfree = Z_NULL;
		zipStream.opaque = Z_NULL;
		zipStream.avail_in = alSize;
		zipStream.next_in = (Bytef *)apSrcData;
		
		///////////////////////////
		// Init decompression
		int ret = inflateInit(&zipStream);
		if (ret != Z_OK) return false;

		///////////////////////////
		// Decompress, chunk by chunk
		do
		{
			//Set current output chunk
			zipStream.avail_out = lMaxChunkSize;
			zipStream.next_out = (Bytef *)&vOutData[0];

			//Decompress as much as possible to current chunk
			int ret = inflate(&zipStream, Z_NO_FLUSH);
			if(ret != Z_OK && ret != Z_STREAM_END)
			{
				inflateEnd(&zipStream);
				return false;
			}

			//Write the chunk to the destination buffer
			size_t lBytesCopied = lMaxChunkSize - zipStream.avail_out;
			AddData(vOutData, lBytesCopied);
		}
		while (zipStream.avail_out == 0 && ret != Z_STREAM_END);

		///////////////////////////
		// Exit and clean up.
		inflateEnd(&zipStream);

		return true;
	}

	//-----------------------------------------------------------------------

	bool cBinaryBuffer::DecompressAndAddFromBuffer(cBinaryBuffer *apSrcBuffer, bool abSizeDataWritten)
	{
		////////////////////////////////
		// Get the size of the data
		size_t lDataSize;
		if(abSizeDataWritten)
		{
			lDataSize = (size_t)apSrcBuffer->GetInt32();
		}
		else
		{
			lDataSize = apSrcBuffer->mlDataSize - apSrcBuffer->mlDataPos;
		}
		//Log("Decompress Size: %d Pos: %d\n", lDataSize, apSrcBuffer->mlDataPos);

		////////////////////////////////
		// Decompress the data to current buffer
		bool bRet = DecompressAndAdd(apSrcBuffer->GetDataPointerAtCurrentPos(), lDataSize);
		
		////////////////////////////////
		// Update the source buffer
		apSrcBuffer->mlDataPos += lDataSize;

		return bRet;
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::XorTransform(const char* apKeyData, size_t alKeySize)
	{
		size_t lCurrentKeyChar =0;
		for(size_t i=0; i<mlDataSize;++i)
		{
			mpData[i] = mpData[i] ^ apKeyData[lCurrentKeyChar];
			++lCurrentKeyChar;
			if(lCurrentKeyChar >= alKeySize) lCurrentKeyChar =0;
		}
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddCRC_Begin()
	{
		mlCRCStartPos = mlDataPos;
		AddInt32(0); //Get space for the CRC
	}
	
	//-----------------------------------------------------------------------

	unsigned int cBinaryBuffer::AddCRC_End(unsigned int alKey)
	{
		unsigned int lCRC = GetCRC(alKey, mlCRCStartPos+4, mlDataSize - mlCRCStartPos - 4);
		*((unsigned int*)GetDataPointerAtPos(mlCRCStartPos)) = lCRC;

		mlCRCStartPos =0;

		return lCRC;
	}

	//-----------------------------------------------------------------------

	unsigned int cBinaryBuffer::GetCRC(unsigned int alKey,int alDataPos, int alCount)
	{
		size_t lDataPos = alDataPos<0 ? mlDataPos : (size_t)alDataPos;
		size_t lDataSize = alCount<0 ? mlDataSize - lDataPos : (size_t)alCount;
		
		cCRC crcData(alKey);
		
		crcData.PutData(&mpData[lDataPos], lDataSize);

		return crcData.Done();
	}

	//-----------------------------------------------------------------------

	bool cBinaryBuffer::CheckInternalCRC(unsigned int alKey, int alCount)
	{
		if(alCount>0) alCount = alCount-4; //Skip the first four bytes

		int lSavedCRC = GetInt32();
		int lCurrentCRC = GetCRC(alKey, -1, alCount);

		return lSavedCRC == lCurrentCRC;
	}

	//-----------------------------------------------------------------------

	bool cBinaryBuffer::CheckCRC(unsigned int alCRC, unsigned int alKey, int alCount)
	{
		int lCurrentCRC = GetCRC(alKey, -1, alCount);

		return alCRC == lCurrentCRC;
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddChar(char alX)
	{
		AddData(&alX, sizeof(char));
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddUnsignedChar(unsigned char alX)
	{
		AddData(&alX, sizeof(char));
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddBool(bool abX)
	{
		char c = abX ? 1:0;
		AddData(&c, sizeof(char));
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddShort16(short alX)
	{
        alX = SDL_SwapLE16(alX);
		AddData(&alX, sizeof(short));
	}

	void cBinaryBuffer::AddUnsignedShort16(unsigned short alX)
	{
        alX = SDL_SwapLE16(alX);
		AddData(&alX, sizeof(short));
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddInt32(int alX)
	{
        alX = SDL_SwapLE32(alX);
		AddData(&alX, sizeof(int));
	}
	
	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddFloat32(float afX)
	{
#ifdef SDL_BIG_ENDIAN
        int i = SwabFloat32(afX);
		AddData(&i, sizeof(float));
#else
        AddData(afX, sizeof(float));
#endif
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddVector2f(const cVector2f& avX)
	{
#ifdef SDL_BIG_ENDIAN
        AddFloat32(avX.x);
        AddFloat32(avX.y);
#else
		AddData(t, sizeof(float)*2);
#endif
	}
	
	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddVector3f(const cVector3f& avX)
	{
#ifdef SDL_BIG_ENDIAN
        AddFloat32(avX.x);
        AddFloat32(avX.y);
        AddFloat32(avX.z);
#else
		AddData(t, sizeof(float)*3);
#endif
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddVector2l(const cVector2l& avX)
	{
        int t[2] = { SDL_SwapLE32(avX.v[0]),SDL_SwapLE32(avX.v[1]) };
		AddData(t, sizeof(int)*2);
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddVector3l(const cVector3l& avX)
	{
        int t[3] = { SDL_SwapLE32(avX.v[0]),SDL_SwapLE32(avX.v[1]), SDL_SwapLE32(avX.v[2]) };
		AddData(t, sizeof(int)*3);
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddMatrixf(const cMatrixf& a_mtxX)
	{
#ifdef SDL_BIG_ENDIAN
        AddFloat32Array(a_mtxX.v, 16);
#else
		AddData(a_mtxX.v, sizeof(float)*16);
#endif
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddQuaternion(const cQuaternion& aqX)
	{
		AddVector3f(aqX.v);
		AddFloat32(aqX.w);
	}
	
	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddColor(const cColor& avX)
	{
#ifdef SDL_BIG_ENDIAN
        AddFloat32(avX.v[0]);
        AddFloat32(avX.v[1]);
        AddFloat32(avX.v[2]);
        AddFloat32(avX.v[3]);
#else
		AddData(avX.v, sizeof(float)*4);
#endif
	}

	//-----------------------------------------------------------------------
	void cBinaryBuffer::AddString(const tString& asStr)
	{
		AddData(asStr.c_str(), sizeof(char) * (asStr.size()+1) ); //+1 for the zero!
	}
	
	//-----------------------------------------------------------------------

/*
	void cBinaryBuffer::AddStringW(const tWString& asStr)
	{
#ifdef BIGENDIAN
        AddInt32Array(asStr[i], asStr.size()+1);
#else
        AddData(asStr.c_str(), sizeof(wchar_t) * (asStr.size()+1) ); //+1 for the zero!
#endif
	}
*/

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddCharArray(const char* apData, size_t alSize)
	{
		AddData(apData, sizeof(char) * alSize);
	}
	
	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddShort16Array(const short* apData, size_t alSize)
	{
#ifdef SDL_BIG_ENDIAN
        for(size_t i=0; i<alSize; ++i) AddShort16(apData[i]);
#else
        AddData(apData, sizeof(short) * alSize);
#endif
	}
	
	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddInt32Array(const int* apData, size_t alSize)
	{
#ifdef SDL_BIG_ENDIAN
        for(size_t i=0; i<alSize; ++i) AddInt32(apData[i]);
#else
        AddData(apData, sizeof(int) * alSize);
#endif
	}
	
	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddFloat32Array(const float* apData, size_t alSize)
	{
#ifdef SDL_BIG_ENDIAN
        for (size_t i=0; i<alSize; ++i) AddFloat32(apData[i]);
#else
		AddData(apData, sizeof(float) * alSize);
#endif
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::SetInt32(int alX, size_t alPos)
    {
        //Check if requested position exists.
        if (alPos + 4 < mlDataSize) {
            alX = SDL_SwapLE32(alX);
            memcpy(mpData + alPos, &alX, 4);
        }
    }

    //-----------------------------------------------------------------------

	char cBinaryBuffer::GetChar()
	{
		char lX;
		GetData(&lX, sizeof(char));
		return lX;
	}

	unsigned char cBinaryBuffer::GetUnsignedChar()
	{
		unsigned char lX;
		GetData(&lX, sizeof(char));
		return lX;
	}
	
	//-----------------------------------------------------------------------

	bool cBinaryBuffer::GetBool()
	{
		char c;
		GetData(&c, sizeof(char));
		return c==0 ? false : true;
	}
	
	//-----------------------------------------------------------------------

	short cBinaryBuffer::GetShort16()
	{
		short lX;
		GetData(&lX, sizeof(short));
		return SDL_SwapLE16(lX);
	}

	unsigned short cBinaryBuffer::GetUnsignedShort16()
	{
		unsigned short lX;
		GetData(&lX, sizeof(unsigned short));
        return SDL_SwapLE16(lX);
	}
	
	//-----------------------------------------------------------------------

	int cBinaryBuffer::GetInt32()
	{
		int lX;
		GetData(&lX, sizeof(int));
		return SDL_SwapLE32(lX);
	}
	
	//-----------------------------------------------------------------------

	float cBinaryBuffer::GetFloat32()
	{
#ifdef SDL_BIG_ENDIAN
        int i;
        GetData(&i, sizeof(float));
        return UnSwabFloat32(i);
#else
        float fX;
        GetData(&fX, sizeof(float));
        return fX;
#endif
	}
	
	//-----------------------------------------------------------------------

	void cBinaryBuffer::GetVector2f(cVector2f *apX)
	{
#ifdef SDL_BIG_ENDIAN
        apX->x = GetFloat32();
        apX->y = GetFloat32();
#else
        GetData(apX->v, sizeof(float)*2);
#endif
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::GetVector3f(cVector3f *apX)
	{
#ifdef SDL_BIG_ENDIAN
        apX->x = GetFloat32();
        apX->y = GetFloat32();
        apX->z = GetFloat32();
#else
        GetData(apX->v, sizeof(float)*3);
#endif
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::GetVector2l(cVector2l *apX)
	{
		GetData(apX->v, sizeof(int)*2);
#ifdef SDL_BIG_ENDIAN
        apX->x = SDL_SwapLE32(apX->x);
        apX->y = SDL_SwapLE32(apX->y);
#endif
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::GetVector3l(cVector3l *apX)
	{
        GetData(apX->v, sizeof(int)*3);
#ifdef SDL_BIG_ENDIAN
        apX->x = SDL_SwapLE32(apX->x);
        apX->y = SDL_SwapLE32(apX->y);
        apX->z = SDL_SwapLE32(apX->z);
#endif
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::GetMatrixf(cMatrixf *apX)
	{
        GetFloat32Array(apX->v, 16);
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::GetQuaternion(cQuaternion* apX)
	{
		GetVector3f(&apX->v);
		apX->w = GetFloat32();
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::GetColor(cColor *apX)
	{
#ifdef SDL_BIG_ENDIAN
        apX->v[0] = GetFloat32();
        apX->v[1] = GetFloat32();
        apX->v[2] = GetFloat32();
        apX->v[3] = GetFloat32();
#else
        GetData(apX->v, sizeof(float)*4);
#endif
	}

	//-----------------------------------------------------------------------
	void cBinaryBuffer::GetString( tString *apStr)
	{
		*apStr = "";
		char c = GetChar();
		while(c != 0 && IsEOF()==false)
		{
			*apStr += c;
			c = GetChar();
		}
	}

	//-----------------------------------------------------------------------
/*
    void cBinaryBuffer::GetStringW(tWString *apStr)
    {
    #ifdef WIN32
        *apStr = _W("");
        wchar_t c = (wchar_t)GetShort16();
        while(c != 0 && IsEOF()==false)
        {
            *apStr += c;
            c = (wchar_t)GetShort16();
        }
    #else
        *apStr = _W("");
        wchar_t c = (wchar_t)GetInt32();
        while(c != 0 && IsEOF()==false)
        {
            *apStr += c;
            c = (wchar_t)GetInt32();
        }
    #endif
    }
*/

	//-----------------------------------------------------------------------

	void cBinaryBuffer::GetCharArray(char* apData, size_t alSize)
	{
		GetData(apData, sizeof(char) * alSize);
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::GetShort16Array(short* apData, size_t alSize)
	{
#ifdef SDL_BIG_ENDIAN
        for(size_t i=0; i<alSize; ++i) apData[i] = GetShort16();
#else
        GetData(apData, sizeof(short) * alSize);
#endif
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::GetInt32Array(int* apData, size_t alSize)
	{
#ifdef SDL_BIG_ENDIAN
        for(size_t i=0; i<alSize; ++i) apData[i] = GetInt32();
#else
        GetData(apData, sizeof(int) * alSize);
#endif
	}

	//-----------------------------------------------------------------------

	void cBinaryBuffer::GetFloat32Array(float* apData, size_t alSize)
	{
#ifdef SDL_BIG_ENDIAN
        for(size_t i=0; i<alSize; ++i) apData[i] = GetFloat32();
#else
		GetData(apData, sizeof(float) * alSize);
#endif
	}

	//-----------------------------------------------------------------------

	
	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cBinaryBuffer::AddData(const void *apData, size_t alSize)
	{
		///////////////////////////////////////
		//Check if data needs to be increased, if double and add size
		if(mlDataPos + alSize > mlReservedDataSize)
		{
			size_t lNewDataSize = mlDataSize*2 + alSize;
			mpData = (char*)hplRealloc(mpData, lNewDataSize);
			mlReservedDataSize = lNewDataSize;
		}

		///////////////////////////////////////
		//Add the data
		memcpy(mpData + mlDataPos, apData, alSize);
		mlDataPos += alSize;
		mlDataSize += alSize;
	}

	//-----------------------------------------------------------------------

	bool cBinaryBuffer::GetData(void *apData, size_t alSize)
	{
		//Check if there is room left to retrieve data.
		if(mlDataPos + alSize > mlDataSize)
		{
			mlDataPos = mlDataSize; //Move to EOF!
			return false;
		}

		memcpy(apData, mpData+mlDataPos, alSize);

		mlDataPos += alSize;

		return true;
	}

	//-----------------------------------------------------------------------
	
	void cBinaryBuffer::InitAndAllocData()
	{
		mlCRCStartPos =0;
		mlDataPos =0;
		mlDataSize = 0;
		mlReservedDataSize = 100;
		mpData = (char*)hplMalloc(mlReservedDataSize);
	}

	//-----------------------------------------------------------------------
}
