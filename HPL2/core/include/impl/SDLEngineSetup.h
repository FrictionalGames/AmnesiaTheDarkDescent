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

#ifndef HPL_SDL_ENGINESETUP_H
#define HPL_SDL_ENGINESETUP_H

#include "system/SystemTypes.h"
#include "engine/LowLevelEngineSetup.h"

namespace hpl {

	class iLowLevelSystem;;
	class iLowLevelGraphics;
	class iLowLevelInput;
	class iLowLevelResources;
	class iLowLevelSound;
	class iLowLevelPhysics;
	class iLowLevelHaptic;

	class cSDLEngineSetup : public iLowLevelEngineSetup
	{
	public:
		cSDLEngineSetup(tFlag alHplSetupFlags);
		~cSDLEngineSetup();
		
		cInput* CreateInput(cGraphics* apGraphics);
		cSystem* CreateSystem();
		cGraphics* CreateGraphics();
		cResources* CreateResources(cGraphics* apGraphics);
		cScene* CreateScene(cGraphics* apGraphics, cResources* apResources, cSound* apSound,
							cPhysics *apPhysics, cSystem *apSystem,cAI *apAI,cGui *apGui,cHaptic *apHaptic);
		cSound* CreateSound();
		cPhysics* CreatePhysics();
		cAI* CreateAI();
		cHaptic* CreateHaptic();

	private:
		iLowLevelSystem *mpLowLevelSystem;
		iLowLevelGraphics *mpLowLevelGraphics;
		iLowLevelInput *mpLowLevelInput;
		iLowLevelResources *mpLowLevelResources;
//		#ifdef WIN32
//			cLowLevelSoundFmod* mpLowLevelSound;
//		#else
			iLowLevelSound*	mpLowLevelSound;
//		#endif
		iLowLevelPhysics* mpLowLevelPhysics;
		iLowLevelHaptic* mpLowLevelHaptic;
	};
};
#endif // HPL_SDL_ENGINESETUP_H
