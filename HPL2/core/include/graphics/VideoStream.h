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

#ifndef HPL_VIDEO_STREAM_H
#define HPL_VIDEO_STREAM_H

#include "graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"

namespace hpl {

	//-----------------------------------------

	class iTexture;

	//-----------------------------------------
	
	class iVideoStream : public iResourceBase
	{
	public:
		iVideoStream(const tString& asName, const tWString& asFullPath) : iResourceBase(asName, asFullPath,0){}
		virtual ~iVideoStream(){}

        virtual void Update(float afTimeStep)=0;

		virtual void Play()=0;
		virtual void Stop()=0;

		virtual void Pause(bool abX)=0;
		virtual bool IsPaused()=0;
		
		virtual void SetLoop(bool abX)=0;
		virtual bool IsLooping()=0;

		virtual void CopyToTexture(iTexture *apTexture)=0;


		const tWString& GetFileName(){return msFilePath;}
		const cVector2l& GetSize(){return mvSize;}
		
		//////////////////////////////////
		// ResourceBase implementation
		bool Reload(){ return false;}
		void Unload(){}
		void Destroy(){}


	protected:
		tWString msFilePath;
		cVector2l mvSize;
	};
};
#endif // HPL_VIDEO_STREAM_H
