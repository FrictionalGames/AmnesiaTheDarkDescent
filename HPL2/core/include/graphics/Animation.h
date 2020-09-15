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

#ifndef HPL_ANIMATION_H
#define HPL_ANIMATION_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "resources/ResourceBase.h"

namespace hpl {

	class cAnimationTrack;
	
	typedef std::vector<cAnimationTrack*> tAnimationTrackVec;
	typedef tAnimationTrackVec::iterator tAnimationTrackVecIt;

	class cAnimation : public iResourceBase
	{
	public:
		cAnimation(const tString &asName, const tWString &asFullPath, const tString &asFile);
		~cAnimation();

		float GetLength();
		void SetLength(float afTime);

		cAnimationTrack* CreateTrack(const tString &asName, tAnimTransformFlag aFlags);
		cAnimationTrack* GetTrack(int alIndex);
		cAnimationTrack* GetTrackByName(const tString &asName);
		void ReserveTrackNum(int alNum);
		int GetTrackNum();

		void SmoothAllTracks(float afAmount, float afPow, int alSamples,bool abTranslation, bool abRotation);

		const char* GetAnimationName(){ return msAnimName.c_str();}
		void SetAnimationName(const tString &asName){ msAnimName =asName;}
		
		tString& GetFileName(){ return msFileName;}

		//Resources implementation
		bool Reload(){ return false;}
		void Unload(){}
		void Destroy(){}

	private:
		tString msAnimName;
		tString msFileName;

		float mfLength;
		
		tAnimationTrackVec mvTracks;
	};

};
#endif // HPL_ANIMATION_H
