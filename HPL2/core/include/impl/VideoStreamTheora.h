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

#ifndef HPL_VIDEO_STREAM_THEORA_H
#define HPL_VIDEO_STREAM_THEORA_H

#include "graphics/VideoStream.h"
#include "resources/VideoLoader.h"

#include <theora/theora.h>

namespace hpl {

	//-----------------------------------------
	class  cVideoStreamTheora_Loader;

	class cVideoStreamTheora : public iVideoStream
	{
	public:
		cVideoStreamTheora(const tString& asName, cVideoStreamTheora_Loader* apLoader);
		~cVideoStreamTheora();

        bool LoadFromFile(const tWString& asFilePath);
		
		void Update(float afTimeStep);

		void Play();
		void Stop();

		void Pause(bool abX);
		bool IsPaused(){ return mbPaused;}

		void SetLoop(bool abX);
		bool IsLooping(){ return mbLooping;}

		void CopyToTexture(iTexture *apTexture);

	private:
		void DrawFrameToBuffer();
		int BufferData(FILE *pFile ,ogg_sync_state *apOggSynchState);
		void QueuePage(ogg_page *apPage);
		bool GetHeaders();
		bool InitDecoders();
		void ResetStreams();
		
		cVideoStreamTheora_Loader *mpLoader;

		FILE *mpFile;

		bool mbLooping;
		bool mbPaused;
		bool mbPlaying;

		float mfTime;

		unsigned char *mpFrameBuffer;
		
		ogg_sync_state   mOggSyncState;
		ogg_stream_state mTheoraStreamState;
		theora_info      mTheoraInfo;
		theora_comment mTheoraComment;
		theora_state	mTheoraState;

		int          mlVideobufReady;
		ogg_int64_t  mlVideobufGranulePos;
		double       mfVideobufTime;

		bool mbVideoLoaded;
		bool mbVideoFrameReady;
		int mlBufferSize;
	};

	//-----------------------------------------

	class cVideoStreamTheora_Loader : public iVideoLoader
	{
	friend class cVideoStreamTheora;
	public:
		cVideoStreamTheora_Loader();
		~cVideoStreamTheora_Loader();
		
		iVideoStream* LoadVideo(const tWString& asName);
	
	private:
		unsigned char* mpYuvToR;
		unsigned char* mpYuvToB;
		
		unsigned short* mpYuv_G_UV;
		unsigned char* mpYuv_G_Y_UV;
	};

	//-----------------------------------------

};
#endif // HPL_VIDEO_STREAM_THEORA_H
