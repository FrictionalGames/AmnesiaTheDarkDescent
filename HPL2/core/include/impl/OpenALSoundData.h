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

#ifndef HPL_OPENAL_SOUND_DATA_H
#define HPL_OPENAL_SOUND_DATA_H

#include "sound/SoundData.h"

#ifdef USE_OALWRAPPER
# include "OALWrapper/OAL_Funcs.h"
#else
# include "OpenAL/OAL_Funcs.h"
#endif

namespace hpl {

	class cOpenALSoundData : public iSoundData
	{
	public:
		cOpenALSoundData(const tString& asName, bool abStream);
		~cOpenALSoundData();

		bool CreateFromFile(const tWString &asFile);

		iSoundChannel* CreateChannel(int alPriority);

		bool IsStream(){ return mbStream;}

		bool IsStereo();

		cOAL_Sample*	GetSample(){ return ( mpSample ); } //static_cast<cOAL_Sample*> (mpSoundData));}
		cOAL_Stream*	GetStream(){ return ( mpStream ); } //static_cast<cOAL_Stream*> (mpSoundData));}
	
	private:
		cOAL_Sample*	mpSample;
		cOAL_Stream*	mpStream;

//iOAL_Loadable*	mpSoundData;
	};
};
#endif // HPL_OPENAL_SOUND_DATA_H
