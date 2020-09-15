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

#ifndef HPL_VIDEO_MANAGER_H
#define HPL_VIDEO_MANAGER_H

#include "resources/ResourceManager.h"

namespace hpl {

	class cResources;
	class cGraphics;
	class iVideoStream;
	class iVideoStreamLoader;

	//----------------------------------------------------

	typedef std::list<iVideoStream*> tVideoStreamList;
	typedef tVideoStreamList::iterator tVideoStreamListIt;

	//----------------------------------------------------

	class cVideoManager : public iResourceManager
	{
	public:
		cVideoManager(cGraphics* apGraphics,cResources *apResources);
		~cVideoManager();
		
		iVideoStream* CreateVideo(const tString& asName);

		void Destroy(iResourceBase* apResource);
		void Unload(iResourceBase* apResource);

		void Update(float afTimeStep);
	private:
		cGraphics* mpGraphics;
		cResources *mpResources;
	};

};
#endif // HPL_VIDEO_MANAGER_H
