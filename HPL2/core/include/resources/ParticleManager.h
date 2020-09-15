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

#ifndef HPL_PARTICLE_SYSTEM_MANAGER_H
#define HPL_PARTICLE_SYSTEM_MANAGER_H

#include "resources/ResourceManager.h"

#include "math/MathTypes.h"

class TiXmlElement;

namespace hpl {

	class cGraphics;
	class cResources;
	class cParticleSystem;
	class cParticleSystemData;
	class cXmlElement;

	//----------------------------------------------------
	
	typedef std::map<tString, cParticleSystemData*> tParticleSystemData3DMap;
	typedef tParticleSystemData3DMap::iterator tParticleSystemData3DMapIt;


	//----------------------------------------------------

	class cParticleManager : public iResourceManager
	{
	public:
		cParticleManager(cGraphics* apGraphics,cResources *apResources);
		~cParticleManager();
		
		cParticleSystem* CreatePS(const tString& asName,const tString& asType, cVector3f avSize);

		/////////////////////////////////////////////////
		// This method is a hack, just so everyone knows
		cParticleSystem* CreatePS(const tString& asName, const tString& asDataName, cXmlElement* apElement,cVector3f avSize);

		void AddData(cParticleSystemData *apData);

		void Preload(const tString& asFile);
		
		void Destroy(iResourceBase* apResource);
		void Unload(iResourceBase* apResource);
	private:
		cGraphics* mpGraphics;
		cResources *mpResources;

		std::list<cParticleSystem*> mlstSystems;
	};

};
#endif // HPL_PARTICLE_SYSTEM_MANAGER_H
