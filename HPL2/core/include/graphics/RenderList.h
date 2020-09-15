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

#ifndef HPL_RENDER_LIST_H
#define HPL_RENDER_LIST_H

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "system/SystemTypes.h"

namespace hpl {

	//---------------------------------------------

	class iRenderable;
	class iLight;
	class cFrustum;
	class cFogArea;

	//---------------------------------------------

	typedef cSTLIterator<iRenderable*, tRenderableVec, tRenderableVecIt> cRenderableVecIterator;

	//---------------------------------------------
	
	class cRenderList
	{
	public:
		cRenderList();
		~cRenderList();

		void Setup(float afFrameTime, cFrustum *apFrustum);

		void AddObject(iRenderable *apObject);

		void Compile(tRenderListCompileFlag aFlags);
		
		bool ArrayHasObjects(eRenderListType aType);
		cRenderableVecIterator GetArrayIterator(eRenderListType aType);

		cRenderableVecIterator GetOcclusionQueryObjectIterator();

		void Clear();

		iLight* GetLight(int alIdx){ return mvLights[alIdx];}
		int GetLightNum(){ return(int)mvLights.size();}

		cFogArea* GetFogArea(int alIdx){ return mvFogAreas[alIdx];}
		int GetFogAreaNum(){ return(int)mvFogAreas.size();}

		void PrintAllObjects();
		
		//Temp:
		int GetSolidObjectNum(){ return (int)mvSolidObjects.size();}
		iRenderable* GetSolidObject(int alIdx){ return mvSolidObjects[alIdx];}

		int GetTransObjectNum(){ return (int)mvTransObjects.size();}
		iRenderable* GetTransObject(int alIdx){ return mvTransObjects[alIdx];}

	private:
		void CompileArray(eRenderListType aType);
		
		void FindNearestLargeSurfacePlane();

		float mfFrameTime;
		cFrustum *mpFrustum;

		tRenderableVec mvOcclusionQueryObjects;
		tRenderableVec mvSolidObjects;
		tRenderableVec mvTransObjects;
		tRenderableVec mvDecalObjects;
		tRenderableVec mvIllumObjects;
		std::vector<iLight*> mvLights;
		std::vector<cFogArea*> mvFogAreas;

		tRenderableVec mvSortedArrays[eRenderListType_LastEnum];
	};

	//---------------------------------------------

};
#endif // HPL_RENDER_LIST_H
