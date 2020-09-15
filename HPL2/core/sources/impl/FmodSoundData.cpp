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

#include "impl/FmodSoundData.h"
#include "impl/FmodSoundChannel.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cFmodSoundData::cFmodSoundData(const tString& asName, bool abStream) : iSoundData(asName,_W(""),abStream)
	{
		mpSample = NULL;
		mpStream = NULL;
	}
	
	//-----------------------------------------------------------------------

	cFmodSoundData::~cFmodSoundData()
	{
		if(mpSample){
			FSOUND_Sample_Free(mpSample);
		}
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cFmodSoundData::CreateFromFile(const tWString &asFile)
	{
		SetFullPath(asFile);

		int lFlags=0;
		
		unsigned int lCaps = 0;
		FSOUND_GetDriverCaps(0, &lCaps);//Current driver here instead of 0
		
		//Get the load flags
		if(lCaps & FSOUND_CAPS_HARDWARE)	lFlags |= FSOUND_HW3D;
		//if(mbStream)						lFlags |= FSOUND_STREAMABLE;

		if(mbStream)
		{
			if(mbLoopStream)
				mpStream = FSOUND_Stream_Open(cString::To8Char(asFile).c_str(), FSOUND_NORMAL | FSOUND_LOOP_NORMAL,0,0);
			else
				mpStream = FSOUND_Stream_Open(cString::To8Char(asFile).c_str(), FSOUND_NORMAL,0,0);

			if(mpStream==NULL){
				Error("Couldn't load sound stream '%s'\n", cString::To8Char(asFile).c_str());
				return false;
			}

		}
		else
		{
			mpSample = FSOUND_Sample_Load(FSOUND_FREE,cString::To8Char(asFile).c_str(), lFlags,0,0);
			//mpSample = FSOUND_Sample_Load(FSOUND_FREE,asFile.c_str(), FSOUND_HW3D,0,0);
			if(mpSample==NULL){
				Error("Couldn't load sound data '%s'\n", asFile.c_str());
				return false;
			}
			FSOUND_Sample_SetMinMaxDistance(mpSample, 4.0f, 10000.0f);    
			FSOUND_Sample_SetMode(mpSample, FSOUND_LOOP_NORMAL);
		}
				
		return true;
	}
	
	//-----------------------------------------------------------------------

	iSoundChannel* cFmodSoundData::CreateChannel(int alPriority)
	{
		if(mpSample==NULL && mpStream==NULL)return NULL;

		int lHandle;
		iSoundChannel *pSoundChannel=NULL;
		if(mbStream)
		{
			lHandle = FSOUND_Stream_PlayEx(FSOUND_FREE,mpStream,NULL, 1);
			if(lHandle==-1)return NULL;

			pSoundChannel = hplNew( cFmodSoundChannel,(this,lHandle, mpSoundManger));
		}
		else
		{
			lHandle = FSOUND_PlaySoundEx(FSOUND_FREE,mpSample,NULL, 1);
			if(lHandle==-1)return NULL;
			
			pSoundChannel = hplNew( cFmodSoundChannel,(this,lHandle, mpSoundManger));
		}

		return pSoundChannel;
	}

	//-----------------------------------------------------------------------

}
