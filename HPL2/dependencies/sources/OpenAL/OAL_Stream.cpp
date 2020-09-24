#include "OpenAL/OAL_Types.h"
#include "OpenAL/OAL_Device.h"
#include "OpenAL/OAL_Helper.h"
#include "OpenAL/OAL_Stream.h"
#include "OpenAL/OAL_Source.h"
#include "OpenAL/OAL_Buffer.h"

#include "system/MemoryManager.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "system/Platform.h"

//-----------------------------------------------------------------------------------

unsigned int cOAL_Stream::mlBufferSize = STREAMING_BLOCK_SIZE;
unsigned int cOAL_Stream::mlBufferCount = 4;

//-----------------------------------------------------------------------------------

cOAL_Stream::cOAL_Stream() : iOAL_AudioData(eOAL_AudioDataType_Stream, mlBufferCount), mpBoundSource(NULL)
{
	mpPCMBuffer = (char*)malloc(mlBufferSize*sizeof(char));
	mvOALBufferIDs = (ALuint*)malloc(mlBufferCount*sizeof(ALuint));
	for(int i=0;i<(int)mlBufferCount;++i)
		mvOALBufferIDs[i] = mvBuffers[i]->GetObjectID();

	mfProcessedBuffersTime = 0;
}

cOAL_Stream::~cOAL_Stream()
{
	//hpl::Log(" OAL: Destroying stream %p\n", this);

	if(mpBoundSource != NULL)
		mpBoundSource->Stop();

	free(mpPCMBuffer);
	free(mvOALBufferIDs);
}

//-----------------------------------------------------------------------------------

void cOAL_Stream::Update()
{
	if(mpBoundSource==NULL)
		return;

	//unsigned long lTimeStart = hpl::cPlatform::GetApplicationTime();
	//unsigned long lTimeToRebuffer = 0;

	if(mbNeedsRebuffering)
	{
		bool bPlaying = mpBoundSource->mbPlaying;
		bool bPaused = mpBoundSource->mbPaused;
		mpBoundSource->LowLevelStop();
		mlBuffersUsed=0;
		mfProcessedBuffersTime=GetTime();
		int lQueuedBuffers = mpBoundSource->GetQueuedBuffers();
		for(int i=0;i<lQueuedBuffers;++i)
			mpBoundSource->Unqueue();

		for(int i=0;i<(int)mvBuffers.size();++i)
		{
			cOAL_Buffer* pBuffer = mvBuffers[i];
			if(Stream(pBuffer))
			{
				++mlBuffersUsed;
				mpBoundSource->Queue(pBuffer);
			}
		}
		if(mlBuffersUsed!=0)
			mbNeedsRebuffering = false;
		if(bPlaying && bPaused==false)
			mpBoundSource->LowLevelPlay();

		//lTimeToRebuffer = hpl::cPlatform::GetApplicationTime()-lTimeStart;
		//hpl::Log("Stream rebuffering took %d ms\n", lTimeToRebuffer);
	}

	if(mlBuffersUsed==0)
		return;

	int lProcessedBuffers = mpBoundSource->GetProcessedBuffers();
	// For every buffer that has been played, unqueue it, refill it with streamed data and enqueue it again
	while(lProcessedBuffers)
	{
		// Then perform the actual streaming
		cOAL_Buffer* pBuffer = mpBoundSource->Unqueue();
		mfProcessedBuffersTime += pBuffer->GetBufferTime();
		if(mfProcessedBuffersTime > mfTotalTime)
			mfProcessedBuffersTime-=mfTotalTime;
		
		if(Stream(pBuffer))
		{
			mpBoundSource->Queue(pBuffer);
		}
		else
		{
            if(mbEOF && (mbLoop || mpBoundSource->mbLoop))
			{
				Seek(0, false);
				Stream(pBuffer);
				mpBoundSource->Queue(pBuffer);
			}
		}
		lProcessedBuffers--;
	}
	
	//hpl::Log("Stream update took %d ms\n", hpl::cPlatform::GetApplicationTime()-(lTimeStart+lTimeToRebuffer));

}

//-----------------------------------------------------------------------------------

void cOAL_Stream::DoBuffering()
{
}

//-----------------------------------------------------------------------------------

bool cOAL_Stream::HasBufferUnderrun()
{
	if(mpBoundSource==NULL)
		return false;

	if(IsEOF()==false)
	{
		//hpl::Log("Have not reached end of file\n");
		return true;
	}

	if(mpBoundSource->GetQueuedBuffers()>0)
	{
		//hpl::Log("Source still has %d buffers to play\n", mpBoundSource->GetQueuedBuffers());
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------
/*
void cOAL_Stream::Log( eOAL_LogVerbose aVerboseLevelReq, eOAL_LogMsg aMessageType, const char* asMessage, ...)
{
	if(asMessage == NULL)
		return;
	if(!mbLogEnabled)
		return;

	if(aVerboseLevelReq < mLogVerboseLevel)
		return;

	string sMessage;

	char text[2048];
	va_list ap;
	va_start(ap, asMessage);
	vsprintf(text, asMessage, ap);
	va_end(ap);
	char streamId[20];

	sprintf(streamId, "Stream %s - ", msFilename.c_str());

	switch(aMessageType)
	{
	case eOAL_LogMsg_Command:
		sMessage.append("[COMMAND] ");
		break;
	case eOAL_LogMsg_Info:
		sMessage.append("[INFO] ");
		break;
	case eOAL_LogMsg_Error:
		sMessage.append("[ERROR] ");
	default:
		break;
	}

	sMessage.append(streamId).append(text);

	Write(sMessage);

}
*/
//-----------------------------------------------------------------------------------

