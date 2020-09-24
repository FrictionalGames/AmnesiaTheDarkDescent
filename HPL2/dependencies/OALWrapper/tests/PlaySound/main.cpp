#include "OALWrapper/OAL_Funcs.h"
#include "OALWrapper/OAL_Sample.h"
#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;

void playSample(cOAL_Sample* pSample)
{
	if (pSample) {
        printf("Success\n");

        printf("Playing Sample...\n");
        printf("\tChannels : %d\n\tFrequency : %d\n", pSample->GetChannels(), pSample->GetFrequency() );

		OAL_Source_Stop ( OAL_ALL );

        int s1 = OAL_Sample_Play ( OAL_FREE, pSample, 1.0f, true, 10 );
		OAL_Source_SetPaused(s1, false);
        while (OAL_Source_IsPlaying(s1)) {
        }
		pSample = NULL;
    } else {
        printf("Failed\n");
    }
}

int main (int argc, char *argv[])
{
    string strFilename;
	bool useMemory = false;

	if ( argc <= 1 )
	{
		printf ("Usage : [memory] %s \"sample.(ogg|wav)\"\n", argv[0]);
        printf ("\tSpecify a sample file to play\n\n");
        exit(1);
	}
	else if (argc <= 2)
	{
		strFilename.assign(argv[1]);
	} else {
		if (string(argv[1]) == "memory") {
			useMemory = true;
		}
		strFilename.assign(argv[2]);
	}

	cOAL_Sample *pSample = NULL;

    printf ("Initializing OpenAL... ");
    fflush(stdout);
    OAL_SetupLogging(true,eOAL_LogOutput_File,eOAL_LogVerbose_High);

    cOAL_Init_Params oal_parms;

    if (OAL_Init(oal_parms)==false)
    {
        printf ("Failed - Check your OpenAL installation\n");
        return 0;
    }
    else
	{
        printf ("Success\n");
	}

	if (useMemory) {
		printf ("Loading \"%s\" from memory buffer ... ", strFilename.c_str());

		FILE *fp = fopen(strFilename.c_str(), "rb");
		if (!fp) {
			printf("Failed file not found\n");
			return 128;
		}
		fseek(fp, 0, SEEK_END);
		size_t pos = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		void *buffer = malloc(pos);
	
		fread(buffer, pos, 1, fp);
		fclose(fp);

		pSample = OAL_Sample_LoadFromBuffer(buffer, pos);
		free(buffer);
	} else {
		printf ("Loading sample \"%s\" ... ", strFilename.c_str());
		pSample = OAL_Sample_Load (strFilename);
	}

	playSample(pSample);

	if (pSample) OAL_Sample_Unload ( pSample );

    printf ("Cleaning up...\n");
    
    OAL_Close ();

	return 0;
}

