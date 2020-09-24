/**
	@file OAL_OggSample.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Derived class for containing Ogg Vorbis Sample data 
*/

#include "OpenAL/OAL_OggSample.h"
#include "OpenAL/OAL_Buffer.h"
#include "system/Platform.h"
#include "system/MemoryManager.h"
#include "system/LowLevelSystem.h"
#include <cstring>

//-------------------------------------------------------------------------------

extern ov_callbacks gCallbacks;

//-------------------------------------------------------------------------------

///////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------

bool cOAL_OggSample::CreateFromFile(const wstring &asFilename)
{
	DEF_FUNC_NAME("cOAL_OggSample::Load()");
	FUNC_USES_AL;

	if(mbStatus==false)
		return false;

	Reset();
	
	char *pPCMBuffer;
	bool bEOF = false;
	int lOpenResult;
	int lCurrent_section;
	long lDataSize = 0;

	msFilename = asFilename;

	FILE *fileHandle = hpl::cPlatform::OpenFile(asFilename, L"rb");
	
	// If no file is present, set the error status and return
	if(!fileHandle)
	{
		mbStatus = false;
		return false;
	}

	// If not an Ogg file, set status and exit
	OggVorbis_File ovFileHandle;
	if(lOpenResult = ov_open_callbacks(fileHandle, &ovFileHandle, NULL, 0, gCallbacks)<0)	
	{
		fclose ( fileHandle );
		mbStatus = false;
		return false;
	}

	// Get file info
	vorbis_info *viFileInfo = ov_info ( &ovFileHandle, -1 );
	mlChannels = viFileInfo->channels;
	mFormat = (mlChannels == 2)?AL_FORMAT_STEREO16:AL_FORMAT_MONO16;
	mlFrequency = viFileInfo->rate;
	mlSamples = (long) ov_pcm_total ( &ovFileHandle, -1 );
	mfTotalTime = ov_time_total( &ovFileHandle, -1 );

	// Reserve memory for 'mlChannels' channels of 'mlSamples' * 2 bytes of data each
	int lSizeInBytes = mlSamples * mlChannels * GetBytesPerSample();
	pPCMBuffer = (char *) hplMalloc (lSizeInBytes);																
	memset (pPCMBuffer, 0, lSizeInBytes);

	// Loop which loads chunks of decoded data into a buffer
	while(!bEOF)
	{
		long lChunkSize = ov_read ( &ovFileHandle			,										
									pPCMBuffer + lDataSize	, 
									STREAMING_BLOCK_SIZE	, 
									SYS_ENDIANNESS			,
									2, 1, &lCurrent_section );

		if(lChunkSize == 0)
			bEOF = true;
		// If we get a negative value, then something went wrong. Clean up and set error status.
		else if(lChunkSize < 0)
		{
			hplFree(pPCMBuffer);
			ov_clear(&ovFileHandle);
			// ov_clear closes the file handle for us
			mbStatus = false;
			return false;
		}
		else 
			lDataSize += lChunkSize;
	}

	cOAL_Buffer* pBuffer = mvBuffers[0];
	if(lDataSize)
	{
		// If something went wrong, set error status. Clean up afterwards.
		mbStatus = pBuffer->Feed((ALvoid*)pPCMBuffer, lDataSize);
	}
	hplFree(pPCMBuffer);																				
	ov_clear(&ovFileHandle);
	// ov_clear closes the file handle for us

	return true;
}

