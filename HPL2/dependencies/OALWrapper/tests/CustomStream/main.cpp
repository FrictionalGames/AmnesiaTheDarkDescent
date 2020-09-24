#include "OALWrapper/OAL_Funcs.h"
#include "OALWrapper/OAL_Stream.h"
#include "OALWrapper/OAL_Buffer.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>

using namespace std;

void RawInit(void *data)
{
	printf("Init\n");
}

double RawGetTime(void *data)
{
	printf("GetTime\n");
	return 0;
}

void RawSeek(void *data, float afWhere, bool abForceRebuffer)
{
	printf("Seek: %f\n", afWhere);
}

bool RawStream(void *data, cOAL_Buffer* apDestBuffer, char *buffer, unsigned int bufferSize, bool& eof)
{
	FILE *fp = (FILE*)data;
	//printf("Stream: %d - %d\n", bufferSize, ftell(fp));
	size_t lread = fread(buffer, 1, bufferSize, fp);
	if (lread) {
		apDestBuffer->Feed(buffer, lread, 0);
	} else {
		eof = true;
	}

	return lread != 0;
}

void RawDestroy(void *data)
{
	printf("Destroy\n");
}

tStreamCallbacks CustomCBs = {
	RawInit,
	RawGetTime,
	RawSeek,
	RawStream,
	RawDestroy
};

void playCustomStream(string filename)
{
	FILE *fp = fopen(filename.c_str(), "r");
	if (!fp) {
		printf ("Failed to open file\n");
		return;
	}
	fseek(fp, 0, SEEK_END);
	__off_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	tStreamInfo info = { 2, 44100, AL_FORMAT_STEREO16 };

	cOAL_Stream* pStream = OAL_Stream_LoadCustom(CustomCBs, info, (void*)fp);
	if (pStream) {
		int s1 = OAL_Stream_Play(OAL_FREE, pStream, 1.0f, false);
		while (OAL_Source_IsPlaying(s1)) {
			//printf("Playing\n");
			usleep(10000);
		}
		OAL_Source_Stop(s1);
		OAL_Stream_Unload(pStream);
	}
	fclose(fp);
}

int main (int argc, char *argv[])
{
    string strFilename;

	if ( argc <= 1 )
	{
		printf ("Usage : %s \"sample.raw\"\n", argv[0]);
        printf ("\tSpecify a sample file to play\n\n");
        exit(1);
	}
	else
	{
		strFilename.assign(argv[1]);
	}

    printf ("Initializing OpenAL... ");
    fflush(stdout);
    OAL_SetupLogging(true,eOAL_LogOutput_File,eOAL_LogVerbose_High);

    cOAL_Init_Params oal_parms;
	oal_parms.mlStreamingBufferSize = 8192;

    if (OAL_Init(oal_parms)==false)
    {
        printf ("Failed - Check your OpenAL installation\n");
        return 0;
    }
    else
	{
        printf ("Success\n");
	}

	playCustomStream(strFilename);
	
    printf ("Cleaning up...\n");

    OAL_Close ();

	return 0;
}

