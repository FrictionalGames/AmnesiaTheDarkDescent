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

#ifndef HPL_SOUND_DATA_H
#define HPL_SOUND_DATA_H

#include "system/SystemTypes.h"
#include "resources/ResourceBase.h"

namespace hpl {

	class cSoundManager;
	class iSoundChannel;
	
	class iSoundData : public iResourceBase
	{
	public:
		iSoundData(const tString& asName, const tWString& asFullPath, bool abStream) : iResourceBase(asName, asFullPath, 0),
		mpSoundManger(NULL), mbStream(abStream){}
		
		virtual ~iSoundData(){}

		virtual bool CreateFromFile(const tWString &asFile)=0;

		virtual iSoundChannel* CreateChannel(int alPriority)=0;
		

		virtual bool IsStereo()=0;

		bool IsStream(){ return mbStream;}
		void SetLoopStream(bool abX){mbLoopStream = abX;}
		bool GetLoopStream(){ return mbLoopStream;}

		bool Reload(){ return false;}
		void Unload(){}
		void Destroy(){}

		void SetSoundManager(cSoundManager* apSoundManager){
			mpSoundManger = apSoundManager;
		}
	
	protected:	
		bool mbStream;
		bool mbLoopStream;
		cSoundManager* mpSoundManger;
	};
};
#endif // HPL_SOUND_DATA_H
