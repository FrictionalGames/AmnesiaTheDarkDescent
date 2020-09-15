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

#ifndef HPL_ENGINE_FILE_LOADING_H
#define HPL_ENGINE_FILE_LOADING_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "resources/ResourcesTypes.h"

namespace hpl {

	//----------------------------

	class cXmlElement;

	class iEntity3D;
	class iLight;
	class cBillboard;
	class cSoundEntity;
	class cParticleSystem;
	class cWorld;
	class cResources;
	class cFogArea;
	class cGraphics;
	class cMesh;

	//----------------------------

	class cEngineFileLoading
	{
	public:
		static cFogArea* LoadFogArea(cXmlElement* apElement, const tString& asNamePrefix, cWorld *apWorld, bool abStatic);
		static cParticleSystem* LoadParticleSystem(cXmlElement* apElement, const tString& asNamePrefix, cWorld *apWorld);
		static cSoundEntity* LoadSound(cXmlElement* apElement, const tString& asNamePrefix, cWorld *apWorld);
		static cBillboard* LoadBillboard(	cXmlElement* apElement, const tString& asNamePrefix, cWorld *apWorld, cResources *apResources, bool abStatic,
											tEFL_LightBillboardConnectionList *apLightBillboardList=NULL);
		static iLight* LoadLight(cXmlElement* apElement, const tString& asNamePrefix, cWorld *apWorld, cResources *apResources, bool abStatic);

		static cMesh* LoadDecalMeshHelper(cXmlElement* apElement, cGraphics* apGraphics, cResources* apResources, const tString& asName, const tString& asMaterial, const cColor& aColor);
			
	private:
		static void SetupWorldEntity(iEntity3D *apEntity, cXmlElement* apElement);
		
	};
};
#endif // HPL_ENGINE_FILE_LOADING_H
