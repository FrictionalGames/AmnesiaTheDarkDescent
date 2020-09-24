#include "OALWrapper/OAL_Funcs.h"
#include "OALWrapper/OAL_Stream.h"

#include <string>
#include <cstdlib>

using namespace std;

int main (int argc, char *argv[])
{
    string strFilename;
    string strFilename2;
    int soundptr = 2;
	if ( argc <= 2 )
	{
		printf ("Usage : %s \"music.ogg\" \"sample1.ogg\" [\"sample2.ogg\" ...]\n", argv[0]);
        printf ("\tSpecify one longer music audio file and 1 or more shorter voice audio\n\tfiles to be played at the same time\n\n");
        exit(1);
	}
	else
	{
		strFilename.assign(argv[1]);
		strFilename2.assign(argv[soundptr++]);
	}

    cOAL_Stream *pStream = NULL;
    cOAL_Stream *pStream2 = NULL;
    alGetError();

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
        printf ("Success\n");

    printf ("Loading stream \"%s\" ... ",strFilename.c_str());

    pStream = OAL_Stream_Load (strFilename.c_str());
    if (pStream) {
        printf("Success\n");
    } else {
        printf("Failed\n");
    }

    printf ("Loading stream \"%s\" ... ",strFilename2.c_str());

    pStream2 = OAL_Stream_Load (strFilename2.c_str());

    if (pStream2) {
        printf("Success\n");
    } else {
        printf("Failed\n");
    }

    if ( pStream && pStream2 ) {
        printf("Playing Main Stream...\n");
        printf("\tChannels : %d\n\tFrequency : %d\n", pStream->GetChannels(), pStream->GetFrequency() );
        printf("Playing Secondary Stream...\n");
        printf("\tChannels : %d\n\tFrequency : %d\n", pStream2->GetChannels(), pStream2->GetFrequency() );

		OAL_Source_Stop ( OAL_ALL );

        int s1 = OAL_Stream_Play ( OAL_FREE, pStream, 1.0f, false );
        int s2 = OAL_Stream_Play ( OAL_FREE, pStream2, 1.0f, false );
        while (OAL_Source_IsPlaying(s1)) {
        	//printf("%d %d:%d\n",s1,s2,OAL_Source_IsPlaying(s2));
            if ( !OAL_Source_IsPlaying(s2) ) {
                OAL_Source_Stop ( s2);
                OAL_Stream_Unload ( pStream2 );
                pStream2 = NULL;
                if (soundptr < argc) {
                    strFilename2.assign(argv[soundptr++]);
                    printf ("Loading new stream \"%s\" ... ",strFilename2.c_str());
                    pStream2 = OAL_Stream_Load(strFilename2.c_str());
                    if (pStream2) {
                        printf("Success\n");
                        printf("Switching Secondary Stream...\n");
                        printf("\tChannels : %d\n\tFrequency : %d\n", pStream2->GetChannels(), pStream2->GetFrequency() );
                        s2 = OAL_Stream_Play ( OAL_FREE, pStream2, 1.0f, false );
                    } else {
                        printf("Failed");
                        soundptr = argc;
                    }
                }
            }
        }
    }
    printf ("Cleaning up...\n");
    OAL_Stream_Unload ( pStream );
    if (pStream2) OAL_Stream_Unload ( pStream2 );

    OAL_Close ();

	return 0;
}

