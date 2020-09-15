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

#include "impl/OpenALSoundData.h"
#include "impl/OpenALSoundChannel.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cOpenALSoundData::cOpenALSoundData(const tString& asName, bool abStream) : iSoundData(asName,_W(""), abStream)
	{
		mpSample = NULL;
		mpStream = NULL;
//		mpSoundData = NULL;
	}
	
	//-----------------------------------------------------------------------

	cOpenALSoundData::~cOpenALSoundData()
	{
		if (mbStream)
		{
			if(mpStream)
				OAL_Stream_Unload ( mpStream );//static_cast<cOAL_Stream*>(mpSoundData) );
		}
		else
		{
			if(mpSample) 
				OAL_Sample_Unload ( mpSample );//static_cast<cOAL_Sample*>(mpSoundData) );
		}
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cOpenALSoundData::CreateFromFile(const tWString &asFile)
	{
		SetFullPath(asFile);

		int lFlags=0;
		unsigned int lCaps = 0;
//		FSOUND_GetDriverCaps(0, &lCaps);//Current driver here instead of 0
		
		//Get the load flags
//		if(lCaps & FSOUND_CAPS_HARDWARE)	lFlags |= FSOUND_HW3D;
		//if(mbStream)						lFlags |= FSOUND_STREAMABLE;
		
		if(mbStream)
		{
			//mpSoundData 
			mpStream = OAL_Stream_Load ( asFile.c_str() ); 
			
				

			if(mpStream == NULL )//mpSoundData==NULL){
			{ 
				Error("Couldn't load sound stream '%s'\n", cString::To8Char(asFile).c_str());
				return false;
			}
			else
				OAL_Stream_SetLoop(mpStream,mbLoopStream);
				//mpStream->SetLoop(mbLoopStream);
				//mpSoundData->SetLoop(mbLoopStream);

		}
		else
		{
			mpSample = OAL_Sample_Load ( asFile.c_str() );
//			mpSoundData = OAL_Sample_Load ( asFile.c_str() );
			if(mpSample == NULL)//mpSoundData==NULL){
			{
				Error("Couldn't load sound data '%s'\n", cString::To8Char(asFile).c_str());
				return false;
			}
			else
				OAL_Sample_SetLoop(mpSample,true);
                //mpSample->SetLoop ( true );

		}
				
		return true;
	}
	
	//-----------------------------------------------------------------------

	iSoundChannel* cOpenALSoundData::CreateChannel(int alPriority)
	{
		//if(mpSoundData==NULL)return NULL;
		if ( (mpSample == NULL) && (mpStream == NULL) ) return NULL;

		int lHandle;
		iSoundChannel *pSoundChannel=NULL;
		if(mbStream)
		{
			lHandle = OAL_Stream_Play ( OAL_FREE, GetStream(), 1.0f, true );
			if(lHandle==-1)return NULL;
			
			pSoundChannel = hplNew( cOpenALSoundChannel, (this,lHandle, mpSoundManger) );
		}
		else
		{
			lHandle = OAL_Sample_Play ( OAL_FREE, GetSample(), 1.0f, true, alPriority);
			if(lHandle==-1)return NULL;
			
			pSoundChannel = hplNew( cOpenALSoundChannel, (this,lHandle, mpSoundManger) );
		}

		return pSoundChannel;
	}

	//-----------------------------------------------------------------------

	bool cOpenALSoundData::IsStereo()
	{
		if (mbStream)
			return (OAL_Stream_GetChannels(mpStream)==2);
		if (mpSample)
			return (OAL_Sample_GetChannels(mpSample)==2);

		return false;
	}

	//-----------------------------------------------------------------------

}
