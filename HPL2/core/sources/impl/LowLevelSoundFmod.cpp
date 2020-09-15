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

#ifdef WIN32
#pragma comment(lib, "fmodvc.lib")
#endif

#include "impl/LowLevelSoundFmod.h"
#include "system/String.h"
#include "impl/FmodSoundData.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"

#include <fmod/fmod.h>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelSoundFmod::cLowLevelSoundFmod()
	{
		mvFormats[0] = "WAV";mvFormats[1] = "OGG";mvFormats[2] = "MP3";
		mvFormats[3] = "";

		
		Log(" Initializing FMOD.\n");
		FSOUND_SetDriver(0);
		FSOUND_SetMixer(FSOUND_MIXER_AUTODETECT);

		//Debug:
		//FSOUND_SetMaxHardwareChannels(0);
		
		FSOUND_Init(44100, 32, 0); //Change this to get effects later on..

		//Setup channel limit.
		int lChannels2D, lChannels3d, lChannelsTotal;
		FSOUND_GetNumHWChannels(&lChannels2D,&lChannels3d,&lChannelsTotal);
		Log(" Number of hardware 2D channels: %d\n",lChannels2D);
		Log(" Number of hardware 3D channels: %d\n",lChannels3d);
		Log(" Number of total hardware channels: %d\n",lChannelsTotal);
        
		FSOUND_SetMinHardwareChannels(32);
		FSOUND_SetMaxHardwareChannels(32);
				
		//Default listener settings.
		float Pos[3] = {0,0,0};
		float Vel[3] = {0,0,0};

		FSOUND_3D_Listener_SetAttributes(Pos,Vel,0,0,-1,0,1,0);
		mvListenerForward = cVector3f(0,0,-1);
		mvListenerUp = cVector3f(0,1,0);

		//Default volume:
		SetVolume(1.0f);
	}

	//-----------------------------------------------------------------------

	cLowLevelSoundFmod::~cLowLevelSoundFmod()
	{
		FSOUND_Close();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHOD
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iSoundData* cLowLevelSoundFmod::LoadSoundData(const tString& asName, const tString& asFilePath,
												const tString& asType, bool abStream,bool abLoopStream)
	{
		cFmodSoundData* pSoundData = new cFmodSoundData(asName,abStream);
		
		pSoundData->SetLoopStream(abLoopStream);
		
		if(pSoundData->CreateFromFile(asFilePath)==false)
		{
			delete pSoundData;
			return NULL;
		}

		return pSoundData;
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundFmod::GetSupportedFormats(tStringList &alstFormats)
	{
		int lPos = 0;

		while(mvFormats[lPos]!="")
		{
			alstFormats.push_back(mvFormats[lPos]);
			lPos++;
		}
	}
	//-----------------------------------------------------------------------
	
	void cLowLevelSoundFmod::UpdateSound(float afTimeStep)
	{
		FSOUND_Update();
	}
	
	//-----------------------------------------------------------------------

	void cLowLevelSoundFmod::SetListenerAttributes(const cVector3f &avPos,const cVector3f &avVel,
							const cVector3f &avForward,const cVector3f &avUp)
	{
		mvListenerPosition = avPos;
		mvListenerVelocity = avVel;
		mvListenerForward = avForward;
		mvListenerUp = avUp;

		mvListenerRight = cMath::Vector3Cross(mvListenerForward,mvListenerUp);

		m_mtxListener = cMatrixf(
				-mvListenerRight.x, -mvListenerRight.y,-mvListenerRight.z, avPos.x,
				-mvListenerUp.x, -mvListenerUp.y,-mvListenerUp.z, avPos.y,
				-mvListenerForward.x, -mvListenerForward.y,-mvListenerForward.z, avPos.z,
				0, 0,0, 1
			);

		float fVel[3]={0,0,0};
		FSOUND_3D_Listener_SetAttributes(avPos.v,fVel,
										mvListenerForward.x,mvListenerForward.y,mvListenerForward.z,
										mvListenerUp.x,mvListenerUp.y,mvListenerUp.z);
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundFmod::SetListenerPosition(const cVector3f &avPos)
	{
		mvListenerPosition = avPos;
	
		FSOUND_3D_Listener_SetAttributes(avPos.v,mvListenerVelocity.v,
			mvListenerForward.x,mvListenerForward.y,mvListenerForward.z,
			mvListenerUp.x,mvListenerUp.y,mvListenerUp.z);
	}
	
	//-----------------------------------------------------------------------
		
	void cLowLevelSoundFmod::SetListenerAttenuation (bool abEnabled)
	{
		mbListenerAttenuation = abEnabled;
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundFmod::SetSetRolloffFactor(float afFactor)
	{
		FSOUND_3D_SetRolloffFactor(afFactor);
	}
	
	//-----------------------------------------------------------------------

	void cLowLevelSoundFmod::SetVolume(float afVolume)
	{
		mfVolume = afVolume;

		int lVol = (int)(255.0f*afVolume);

        FSOUND_SetSFXMasterVolume(lVol);
	}

	//-----------------------------------------------------------------------

}
