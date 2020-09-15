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

#include "impl/VideoStreamTheora.h"

#include "system/LowLevelSystem.h"
#include "system/Platform.h"
#include "math/Math.h"
#include "graphics/Texture.h"
#include "system/String.h"

#include <cstdio>
#include <cstring>

#pragma comment(lib, "libogg.lib")
#pragma comment(lib, "libtheora.lib")

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// LOADER
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVideoStreamTheora_Loader::cVideoStreamTheora_Loader()
	{
		//////////////////////////////////////
		// Set up extensions
		AddSupportedExtension("dds");

		//////////////////////////////////////
		// Calculate YUV -> RBG
		
		//////////////////////////////
		//Create buffers
		mpYuvToR = hplNewArray( unsigned char,256*256);
		mpYuvToB = hplNewArray( unsigned char,256*256);

		mpYuv_G_UV = hplNewArray( unsigned short,256*256);
		mpYuv_G_Y_UV = hplNewArray( unsigned char,256*1024);

		//////////////////////////////
		//Values for R and B
		float fY, fI,fJ,fUV;
		for(int i=0; i<256; ++i)
		for(int y=0; y<256; ++y)
		{
			fY = 1.164f * ((float)y -16);
			fI = (float)i;
			
			//I = V
			mpYuvToR[y + i*256] = (unsigned char)cMath::Clamp(fY + 1.596f*(fI - 128),0,255);

			//I = U
			mpYuvToB[y + i*256] = (unsigned char)cMath::Clamp(fY + 2.018f * (fI - 128),0,255);
		}

		///////////////////////////////////////////////////
		//Split the Green computation into two buffer
		float fMinVal = 0.813f * (-128.0f) - 0.391f * 127;
		float fMaxVal = 0.813f * (127.0f) + 0.391f * 128;

		//Green UV part
		for(int i=0; i<256; ++i)
		for(int j=0; j<256; ++j)
		{
			//I= U J = V
			fJ = (float)j;
			fI = (float)i;

			float fUV = 0.813f*(fJ - 128) - 0.391f * (fI - 128);
			mpYuv_G_UV[i*256 + j] = (unsigned short)cMath::Clamp(((fUV - fMinVal) / (fMaxVal - fMinVal)) * 1023.0f,0,1023.0f);
		}

		//Green Y * UV part
		for(int i=0; i<1024; ++i)
		for(int y=0; y<256; ++y)
		{
			fY = 1.164f * ((float)y -16);
			fUV = (float)i;
			fUV = (fUV / 1023.0f) * (fMaxVal - fMinVal) + fMinVal;

			mpYuv_G_Y_UV[i*256 + y] = (unsigned char)cMath::Clamp(fY - fUV, 0, 255);
		}

	}

	cVideoStreamTheora_Loader::~cVideoStreamTheora_Loader()
	{
		hplDeleteArray(mpYuvToR);
		hplDeleteArray(mpYuvToB);

		hplDeleteArray(mpYuv_G_UV);
		hplDeleteArray(mpYuv_G_Y_UV);
	}

	//-----------------------------------------------------------------------
	
	iVideoStream* cVideoStreamTheora_Loader::LoadVideo(const tWString& asFile)
	{ 
		tString sName = cString::To8Char(cString::GetFileNameW(asFile));
		cVideoStreamTheora *pVideo = hplNew( cVideoStreamTheora, (sName,this) );
		
		if(pVideo->LoadFromFile(asFile)==false)
		{
			hplDelete(pVideo);
			return NULL;
		}
		return pVideo;
	}


	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cVideoStreamTheora::cVideoStreamTheora(const tString& asName, cVideoStreamTheora_Loader* apLoader) : iVideoStream(asName, _W(""))
	{
		mpLoader = apLoader;

		mpFile = NULL;

		mbLooping = false;
		mbPaused = false;
		mbPlaying = false;

		mlVideobufReady =0;
		mlVideobufGranulePos=-1;
		mfVideobufTime=0;

		mfTime = 0;

		mlBufferSize = 4096;

		mbVideoFrameReady = false;

		mpFrameBuffer = NULL;

		//Theora structs that we want until class i deleted.
		theora_comment_init(&mTheoraComment);
		theora_info_init(&mTheoraInfo);

		ogg_sync_init(&mOggSyncState);
	}

	//-----------------------------------------------------------------------


	cVideoStreamTheora::~cVideoStreamTheora()
	{
		if(mpFile) fclose(mpFile);

		ogg_sync_clear(&mOggSyncState);

		theora_comment_clear(&mTheoraComment);
		theora_info_clear(&mTheoraInfo);

		ogg_stream_clear(&mTheoraStreamState);

		if(mbVideoLoaded)
		{
			theora_clear(&mTheoraState);
			if(mpFrameBuffer) hplDeleteArray(mpFrameBuffer);
		}
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cVideoStreamTheora::LoadFromFile(const tWString& asFilePath)
	{
		SetFullPath(asFilePath);

		////////////////////////////////
		//Try to open the file
		mpFile = cPlatform::OpenFile(asFilePath, _W("rb"));
		if(mpFile==NULL)
		{
			return false;
		}
		
		msFilePath = asFilePath;

		////////////////////////////////
		//Init stream

		//Set up variables
		mbVideoLoaded = false;

		//Get headers
		if(GetHeaders()==false)	return false;
		
		//Initialize decoders and attributes
		if(InitDecoders()==false) return false;
        

		return true;
	}

	//-----------------------------------------------------------------------

	void cVideoStreamTheora::Update(float afTimeStep)
	{
		if(mbPlaying==false || mbPaused) return;

		mfTime += afTimeStep;
		////////////////////////////////
		// Theora Decode packets until the video is ahead of real time. 
		// (This could be skipped once non-keyframe seeks can be made?)
		while(!mlVideobufReady && mfVideobufTime < mfTime)
		{
			//Get first packet and decode,
			ogg_packet packet;
			if(ogg_stream_packetout(&mTheoraStreamState,&packet)>0)
			{
				if(theora_decode_packetin(&mTheoraState,&packet)==0)
				{
					//Get new time for current fram position.
					mlVideobufGranulePos = mTheoraState.granulepos;
					mfVideobufTime = theora_granule_time(&mTheoraState,mlVideobufGranulePos);

					mbVideoFrameReady = true;	
				}
			}
			//No packets left, get new page.
			else
			{
				//Get Next page
				ogg_page page;
				if(ogg_sync_pageout(&mOggSyncState,&page) > 0)
				{
					QueuePage(&page);	
				}
				//No pages left, read more buffer data.
				else
				{
					int bytes= BufferData(mpFile,&mOggSyncState);
					//Fill streams with pages.
					if(bytes!=0)
					{
						while(ogg_sync_pageout(&mOggSyncState,&page)>0) QueuePage(&page);
					}
					//No more buffer data in file, stop video				
					else
					{
						if(mbLooping==false) mbPlaying = false;
						ResetStreams();
					}
					
				}
			}
		}

		
		////////////////////////////////
		// Theora Check for end of file.
		if(!mlVideobufReady && feof(mpFile))
		{
			if(mbLooping==false) mbPlaying = false;
			ResetStreams();
		}
	}

	//-----------------------------------------------------------------------

	void cVideoStreamTheora::Play()
	{
		mbPlaying = true;
	}
	
	//-----------------------------------------------------------------------

	void cVideoStreamTheora::Stop()
	{
		mbPlaying = false;
		ResetStreams();
	}

	//-----------------------------------------------------------------------

	void cVideoStreamTheora::Pause(bool abX)
	{
		mbPaused = abX;
	}
	//-----------------------------------------------------------------------


	void cVideoStreamTheora::SetLoop(bool abX)
	{
		mbLooping = abX;
	}
	
	//-----------------------------------------------------------------------

	void cVideoStreamTheora::CopyToTexture(iTexture *apTexture)
	{
		if(mbVideoLoaded==false || mpFrameBuffer ==NULL) return;

		if(mbVideoFrameReady)
		{
			DrawFrameToBuffer();
			apTexture->SetRawData(0,0,cVector3l(mvSize.x,mvSize.y,1) ,ePixelFormat_RGB,mpFrameBuffer);
			mbVideoFrameReady = false;
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cVideoStreamTheora::DrawFrameToBuffer()
	{
		////////////////////////////////
		//Get YUV buffer
		yuv_buffer yuvBuffer;
		theora_decode_YUVout(&mTheoraState,&yuvBuffer);
		
		/////////////////////////////
		// Set up variables

		//This is offsets in the SOURCE DATA
		size_t lCropOffsetY = mTheoraInfo.offset_x + yuvBuffer.y_stride * mTheoraInfo.offset_y;
		size_t lCropOffsetUV = mTheoraInfo.offset_x/2 + yuvBuffer.uv_stride * (mTheoraInfo.offset_y/2);

		//Pointers to conversion buffers.
		unsigned char* pYuvToR = mpLoader->mpYuvToR;
		unsigned char* pYuvToB = mpLoader->mpYuvToB;

		unsigned short* pYuv_G_UV = mpLoader->mpYuv_G_UV;
		unsigned char*  pYuv_G_Y_UV = mpLoader->mpYuv_G_Y_UV;

		/////////////////////////////////
		//Write Y to R

		//Get start of buffers
		unsigned char *pSrcBuffer = mpFrameBuffer;
		unsigned char *pYBuffer = yuvBuffer.y + lCropOffsetY;
		
		//Set up counters
		int lXCount = mvSize.x;
		int lYCount = mvSize.y;
		
		//Precalculate some values
		const int lYBufferAdd = yuvBuffer.y_stride - mvSize.x;

		//Go through Y surface of copy to R in frame buffer
		while(lYCount)
		{
			*pSrcBuffer = *pYBuffer;

			pSrcBuffer+=3;
			pYBuffer++;

			--lXCount;
			if(!lXCount)
			{
				pYBuffer += lYBufferAdd;
				lXCount = mvSize.x;
				--lYCount;
			}
		}

		/////////////////////////////////
		//Write UV and use R as Y

		//Get start of buffers
		pSrcBuffer = mpFrameBuffer;
		unsigned char *pUBuffer = yuvBuffer.u + lCropOffsetUV;
		unsigned char *pVBuffer = yuvBuffer.v + lCropOffsetUV;
		
		//Setup counters
		lXCount = mvSize.x/2;
		lYCount = mvSize.y/2;
		
		//Precalculate some values
		const int lXRowAdd = mvSize.x/2;
		const int lSourceRowSize = mvSize.x*3;
		const int lUVBufferAdd = yuvBuffer.uv_stride - mvSize.x/2;
		unsigned char y;
		
		//Go through UV surfaces and convert to RGB in frame buffer
		while(lYCount)
		{
			const unsigned short u_add = (*pUBuffer) <<8;
			const unsigned short v_add = (*pVBuffer) <<8;
			const unsigned int g_uv_add = (pYuv_G_UV[u_add + (*pVBuffer)])<<8;

			//Here we draw to a 2x2 large piece of source.
			y = *pSrcBuffer;
			pSrcBuffer[0] = pYuvToR[y + v_add];
			pSrcBuffer[1] = pYuv_G_Y_UV[y +(g_uv_add)];
			pSrcBuffer[2] = pYuvToB[y + u_add];	

			pSrcBuffer+= 3;
			y = *pSrcBuffer;
			pSrcBuffer[0] = pYuvToR[y + v_add];
			pSrcBuffer[1] = pYuv_G_Y_UV[y +(g_uv_add)];
			pSrcBuffer[2] = pYuvToB[y + u_add];			

			pSrcBuffer += lSourceRowSize - 3;
			y = *pSrcBuffer;
			pSrcBuffer[0] = pYuvToR[y + v_add];
			pSrcBuffer[1] = pYuv_G_Y_UV[y +(g_uv_add)];
			pSrcBuffer[2] = pYuvToB[y + u_add];			

			pSrcBuffer += 3;
			y = *pSrcBuffer;
			pSrcBuffer[0] = pYuvToR[y + v_add];
			pSrcBuffer[1] = pYuv_G_Y_UV[y +(g_uv_add)];
			pSrcBuffer[2] = pYuvToB[y + u_add];			

			pSrcBuffer-= lSourceRowSize;

			pSrcBuffer+=3;

			pUBuffer++;
			pVBuffer++;

			--lXCount;
			if(!lXCount)
			{
				pSrcBuffer+= lSourceRowSize;
				pVBuffer += lUVBufferAdd;
				pUBuffer += lUVBufferAdd;
				lXCount = lXRowAdd;
				--lYCount;
			}
		}
	}

	//-----------------------------------------------------------------------
	
	/**
	  * Gets BufferSize bytes to a sync buffer and checks how many bytes that
	  * where written to it.
	  */
	int cVideoStreamTheora::BufferData(FILE *pFile ,ogg_sync_state *apOggSynchState)
	{
		ogg_page tempPage;
		if (ogg_sync_pageout(apOggSynchState, &tempPage) ==1) 
		{
			Warning("Still pages in buffer do not read another one! Go tell the programmer he messed up!\n");
			return 0;
		}

		//Get new buffer of BufferSize bytes
		char *pBuffer = ogg_sync_buffer(apOggSynchState,mlBufferSize);
		int lBytes= (int)fread(pBuffer,1,mlBufferSize,pFile);

		//Tell sync state we just wrote BufferSize bytes, so file (data?) pointer is increased by that.
		ogg_sync_wrote(apOggSynchState,lBytes);

		return lBytes;
	}

	//-----------------------------------------------------------------------

	/**
	 * Queues a page in one of the stream, right now there is on the Theora stream
	 * but could add a Vorbis stream here too.
	 * pushing a page can be done blindly; a stream won't accept a page that does not belong to it.
	 */
	void cVideoStreamTheora::QueuePage(ogg_page *apPage)
	{	
		if(mbVideoLoaded)
		{
			if(ogg_stream_pagein(&mTheoraStreamState,apPage)!=0)
			{
				//I think the page can simply be skipped. Data is destroyed with buffer.... I think :S
			}
		}
		
	}

	//-----------------------------------------------------------------------
	
	/**
	  * Initializes data needed for decoding and sets up video properties
	  * Must be called after GetHeaders()
	  * Could add init for Vorbis here too...
	*/
	bool cVideoStreamTheora::InitDecoders()
	{
		if(mbVideoLoaded)
		{
			theora_decode_init(&mTheoraState,&mTheoraInfo);	 //Init the Theora state based on info

			mvSize = cVector2l(mTheoraInfo.frame_width, mTheoraInfo.frame_height);

			mpFrameBuffer = hplNewArray(unsigned char,mvSize.x * mvSize.y *3);
		}
		else
		{
			//Tear down the partial Theora setup
			theora_info_clear(&mTheoraInfo);
			theora_comment_clear(&mTheoraComment);
		}

		return true;
	}

	//-----------------------------------------------------------------------

	/**
	  * Reinitializes all stream data and searches past header packets.
	  * Vorbis must be reset here too if used.
	*/
	void cVideoStreamTheora::ResetStreams()
	{
		/////////////////////////////////////
		// Clear all remaining packages and pages
		ogg_packet packet;
		while(ogg_stream_packetout(&mTheoraStreamState,&packet)>0);

		ogg_page testPage;
		while(ogg_sync_pageout(&mOggSyncState,&testPage));
		

		////////////////////////////////
		//Reset variables
		mfTime =0;
		mfVideobufTime =0;
		mbVideoFrameReady = false;
		
		////////////////////////////////
		//Clear all data structures

		//Setup stream and get past headers
		ogg_stream_clear(&mTheoraStreamState);
		ogg_sync_reset(&mOggSyncState);

		theora_clear(&mTheoraState);
		theora_decode_init(&mTheoraState,&mTheoraInfo);

		//Get start of file
		fclose(mpFile);
		mpFile = cPlatform::OpenFile(msFilePath, _W("rb"));

		BufferData(mpFile,&mOggSyncState);

		//////////////////////////////////
		// Get the streams and get past headers.
		int lPackCount=0;
		while(lPackCount<3)
		{
			ogg_page page;
			while(lPackCount<3 && ogg_sync_pageout(&mOggSyncState,&page)>0)
			{
				////////////////////////////
				//Check if stream is Theora
				if(lPackCount == 0)
				{
					//Current page is at beggning of stream.
					if(ogg_page_bos(&page))
					{
						ogg_stream_state testStream;
						ogg_packet testPacket;

						//Init test stream
						ogg_stream_init(&testStream,ogg_page_serialno(&page));
						ogg_stream_pagein(&testStream,&page);						//Put current page into stream.
						ogg_stream_packetout(&testStream,&testPacket);			//Put first packet out in testPacket.

						//Check if packet is a header.
						if(testPacket.packet[0] & 0x80)	{
							memcpy(&mTheoraStreamState,&testStream,sizeof(testStream));						//Copy contents of test stream to Theora stream
							lPackCount =1;
						}
						//If no type we are interested in, skip it.
						else {
							ogg_stream_clear(&testStream);
						}
					}
					else
					{
					}
				}
				////////////////////////////
				//Get past all header packets.
				if(lPackCount > 0)
				{	
					QueuePage(&page);

					while(lPackCount<3)
					{
						ogg_packet testPacket;
						int lRet = ogg_stream_packetout(&mTheoraStreamState, &testPacket);
						if(lRet < 1) break; //No packets left, we need new page.
						++lPackCount;
					}
				}
			}
			//Fill buffer with more data from file.
			BufferData(mpFile,&mOggSyncState);
		}
	}

	//-----------------------------------------------------------------------

	/**
	 * This function goes through all streams found in the file and looks for first Theora header.
	 * Here one could add a search for Vorbis headers too.
	 */
	bool cVideoStreamTheora::GetHeaders()
	{
		int lTheoraHeaderPackets =0;

		//////////////////////////////////////////
		// Initialize all usable streams and get headers
		bool bFoundHeaders=false;
		while(!bFoundHeaders)
		{
			int lRet = BufferData(mpFile,&mOggSyncState);
			if(lRet==0)
			{
				Error("Found no header in video file '%s'\n",msFilePath.c_str());
				return false;
			}
			//Get first page in sync state buffer
			ogg_page page;
			while(ogg_sync_pageout(&mOggSyncState,&page)>0)
			{
				
				/////////////////////////////////
				// Check if page is beginning of stream, else queue it.
				if(!ogg_page_bos(&page))
				{
					//Don't leak the page; get it into the appropriate stream
					//Does this only mean that if we have already intialized a stream and the next page is for it,
					//we cannot just leave it be or else that stream will be missing info, yeah that must be it.
					QueuePage(&page);
					
					continue;
				}

				/////////////////////////////////
				//Init the test stream with page
				ogg_stream_state testStream;
				ogg_stream_init(&testStream,ogg_page_serialno(&page));	//Init stream using logical serial to stream page belongs to.
				ogg_stream_pagein(&testStream,&page);						//Put current page into stream.
				
				/////////////////////////////////
				//Get first packet in stream and check if it is a header.
				ogg_packet testPacket;
				ogg_stream_packetout(&testStream,&testPacket);				

				if(!mbVideoLoaded && theora_decode_header(&mTheoraInfo,&mTheoraComment,&testPacket)>=0)
				{
					memcpy(&mTheoraStreamState,&testStream,sizeof(testStream));						//Copy contents of test stream to Theora stream
					lTheoraHeaderPackets = 1;
					mbVideoLoaded = true;
				}
				else
				{
					//We have some stream we do not want, destroy it.
					ogg_stream_clear(&testStream);
				}

				if(mbVideoLoaded) bFoundHeaders = true;
			}
		}

		//////////////////////////////////////////
		// Get remaining header data
		while(lTheoraHeaderPackets && lTheoraHeaderPackets<3)
		{
			int lRet;
			ogg_packet packet;

			//////////////////////////////////////////
			//Look for further Theora headers
			while( lTheoraHeaderPackets && lTheoraHeaderPackets<3)
			{
				//////////////////////////
				// Get next packet
				lRet = ogg_stream_packetout(&mTheoraStreamState,&packet);
				if(lRet == 0) break; //End of page, need to load new page.

				if(lRet<0){
					Error("Error parsing Theora stream headers in '%s'; corrupt stream?\n",msFilePath.c_str());
					return false;
				}
				if(theora_decode_header(&mTheoraInfo,&mTheoraComment,&packet))
				{
					Error("Error parsing Theora stream headers in '%s'; corrupt stream?\n",msFilePath.c_str());
					return false;
				}
				lTheoraHeaderPackets++;
				if(lTheoraHeaderPackets==3)break;
			}

			//////////////////////
			// Try to get next page.
			ogg_page page;
			if(ogg_sync_pageout(&mOggSyncState,&page)>0)
			{
				QueuePage(&page); 
			}
			else
			{
				//No pages left, load more data.
				int lRet=BufferData(mpFile,&mOggSyncState); 
				if(lRet==0)
				{
					Error("End of file while searching for codec headers in '%s'.\n",msFilePath.c_str());
					return false;
				}
			}
		}
		

		return true;
	}
	
	//-----------------------------------------------------------------------

}
