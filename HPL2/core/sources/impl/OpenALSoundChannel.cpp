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

#include "impl/OpenALSoundChannel.h"
#include "impl/OpenALSoundData.h"
#include "resources/SoundManager.h"

#include "math/Math.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cOpenALSoundChannel::cOpenALSoundChannel(iSoundData* apData, int alChannel,cSoundManager* apSoundManger)
	: iSoundChannel(apData, apSoundManger)
	{
		mlChannel = alChannel;

        for(int i=0;i<3;i++)
		{
			mfPosition[i] = 0;
			mfVelocity[i] = 0;
		}

		OAL_Source_SetAttributes ( mlChannel, mfPosition, mfVelocity );
		OAL_Source_SetFilterType(mlChannel, eOALFilterType_LowPass);
//		SetAffectedByEnv(true);
//		SetFilterGainHF(0.01f);
		
//		SetFiltering(true,0x3);
//		OAL_Source_SetMinMaxDistance ( mlChannel, 100000.0f, 200000.f );
	}
	
	//-----------------------------------------------------------------------

	cOpenALSoundChannel::~cOpenALSoundChannel()
	{
		if(mlChannel>=0)
			OAL_Source_Stop ( mlChannel );

		DestroyData();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cOpenALSoundChannel::Play()
	{
		//Log("Playing %s - Source %d Ref %d\n",mpData->GetName().c_str(), mlChannel & 0xFFF, (mlChannel & 0x7ffff000) >> 12);

		SetPaused(false);

		mbStopUsed = false;
	}
	
	//-----------------------------------------------------------------------
	
	void cOpenALSoundChannel::Stop()
	{
		//Log("Stopping %s - Source %d Ref %d\n",mpData->GetName().c_str(), mlChannel & 0xFFF, (mlChannel & 0x7ffff000) >> 12);
		OAL_Source_Stop ( mlChannel );
		mlChannel = -1;

		mbStopUsed = true;
	}
	
	//-----------------------------------------------------------------------
	
	void cOpenALSoundChannel::SetPaused(bool abX)
	{
//		Log("Setting %s %spaused! - Source %d\n",mpData->GetName().c_str(),abX?"":"un", mlChannel);
		
		mbPaused = abX;
		OAL_Source_SetPaused ( mlChannel, abX );

//		if(mbPaused)
//		{
			//Log("Setting %s paused!\n",mpData->GetName().c_str());
//		}
	}
	
	//-----------------------------------------------------------------------
	
	void cOpenALSoundChannel::SetSpeed(float afSpeed)
	{
		mfSpeed = afSpeed;
		
		OAL_Source_SetPitch ( mlChannel, afSpeed );
	}
	
	//-----------------------------------------------------------------------
	
	void cOpenALSoundChannel::SetVolume(float afVolume)
	{
		mfVolume = afVolume;
		
		OAL_Source_SetVolume(mlChannel, mfVolume);
	}
	
	//-----------------------------------------------------------------------
	
	void cOpenALSoundChannel::SetLooping (bool abLoop)
	{
		mbLooping = abLoop;

		OAL_Source_SetLoop ( mlChannel, abLoop );
	}

	//-----------------------------------------------------------------------

	void cOpenALSoundChannel::SetPan (float afPan)
	{
		//Log("Pan: %d\n", lPan);
//		cVector3f vPosition = mvPosition;
//		vPosition.x = 
//		OAL_Source_SetAttributes ( mlChannel, mvPosition.
//		FSOUND_SetPan(mlChannel, lPan);
	}
	
	//-----------------------------------------------------------------------

	void cOpenALSoundChannel::Set3D(bool ab3D)
	{
		mb3D = ab3D;		
	}
	
	//-----------------------------------------------------------------------
	
	void cOpenALSoundChannel::SetPositionIsRelative(bool abRelative)
	{
		mbPositionRelative = abRelative;
//		OAL_Source_SetPositionRelative ( mlChannel, abRelative );
	}

	//-----------------------------------------------------------------------
	
	void cOpenALSoundChannel::SetPosition(const cVector3f &avPos)
	{
		mvPosition = avPos;

		OAL_Source_SetAttributes ( mlChannel, mvPosition.v, mvVelocity.v );
	}
	
	//-----------------------------------------------------------------------

	void cOpenALSoundChannel::SetVelocity(const cVector3f &avVel)
	{
		mvVelocity = avVel;
		
		OAL_Source_SetAttributes ( mlChannel, mvPosition.v, mvVelocity.v );
	}
	
	//-----------------------------------------------------------------------
	
	void cOpenALSoundChannel::SetMinDistance(float afMin)
	{
		mfMinDistance = afMin;
	}

	//-----------------------------------------------------------------------
	
	void cOpenALSoundChannel::SetMaxDistance(float afMax)
	{
		mfMaxDistance = afMax;
	}
	//-----------------------------------------------------------------------
	
	bool cOpenALSoundChannel::IsPlaying()
	{
		return OAL_Source_IsPlaying( mlChannel );
	}
	

	//-----------------------------------------------------------------------

	bool cOpenALSoundChannel::IsBufferUnderrun()
	{ 
		return OAL_Source_IsBufferUnderrun(mlChannel);
	}
	double cOpenALSoundChannel::GetElapsedTime()
	{ 
		return OAL_Source_GetElapsedTime(mlChannel);
	}
	double cOpenALSoundChannel::GetTotalTime()
	{ 
		return OAL_Source_GetTotalTime(mlChannel);
	}
	void cOpenALSoundChannel::SetElapsedTime(double afTime)
	{
		if(afTime < 0) afTime =0;
		if(afTime > GetTotalTime()) afTime = GetTotalTime() - 0.0001;

		OAL_Source_SetElapsedTime(mlChannel, afTime);
	}

	//-----------------------------------------------------------------------
	
	void cOpenALSoundChannel::SetPriority(int alX)
	{
		int lPrio = alX+mlPriorityModifier;
		if(lPrio>255)lPrio = 255;

		OAL_Source_SetPriority ( mlChannel, lPrio );
	}
	
	//-----------------------------------------------------------------------
	
	int cOpenALSoundChannel::GetPriority()
	{
		return OAL_Source_GetPriority ( mlChannel );
	}
	
	//-----------------------------------------------------------------------

	void cOpenALSoundChannel::SetAffectedByEnv(bool abAffected)
	{
//		if (!(gpGame->GetSound()->GetLowLevel()->IsEnvAudioAvailable()))
//			return;
        
		iSoundChannel::SetAffectedByEnv(abAffected);

		if (mbAffectedByEnv)
			OAL_Source_SetAuxSendSlot(mlChannel,0,0);
		else
			OAL_Source_SetAuxSendSlot(mlChannel,0,-1);
	}

	void cOpenALSoundChannel::SetFiltering(bool abEnabled, int alFlags)
	{
//		if (!(gpGame->GetSound()->GetLowLevel()->IsEnvAudioAvailable()))
//			return;
        
		OAL_Source_SetFiltering(mlChannel,abEnabled, alFlags);
	}

	void cOpenALSoundChannel::SetFilterGain(float afGain)
	{
//		if (!(gpGame->GetSound()->GetLowLevel()->IsEnvAudioAvailable()))
//			return;
        
		OAL_Source_SetFilterGain(mlChannel, afGain);
	}
	
	void cOpenALSoundChannel::SetFilterGainHF( float afGainHF)
	{
//		if (!(gpGame->GetSound()->GetLowLevel()->IsEnvAudioAvailable()))
//			return;
        
		OAL_Source_SetFilterGainHF(mlChannel, afGainHF);
	}

}
