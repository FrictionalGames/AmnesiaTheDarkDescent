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

#ifndef HPL_POSTEFFECT_COMPOSITE_H
#define HPL_POSTEFFECT_COMPOSITE_H

#include "graphics/RenderFunctions.h"

namespace hpl {

	//------------------------------------------

	class cGraphics;
	class iLowLevelGraphics;
	class iPostEffect;

	//------------------------------------------

	typedef std::multimap<int, iPostEffect*, std::greater<int> > tPostEffectMap;
	typedef tPostEffectMap::iterator tPostEffectMapIt;
	
	//------------------------------------------

	class cPostEffectComposite : public iRenderFunctions
	{
	public:
		cPostEffectComposite(cGraphics *apGraphics);
		~cPostEffectComposite();
		
		void Render(float afFrameTime, cFrustum *apFrustum, iTexture *apInputTexture, cRenderTarget *apRenderTarget);

		/**
		 * Highest prio is first!
		 */
		void AddPostEffect(iPostEffect *apPostEffect, int alPrio);
		inline int GetPostEffectNum()const{ return (int)mvPostEffects.size(); }
		inline iPostEffect* GetPostEffect(int alIdx)const{ return mvPostEffects[alIdx]; }

		bool HasActiveEffects();

		float GetCurrentFrameTime(){ return mfCurrentFrameTime;}
	
	private:
		void BeginRendering(float afFrameTime, cFrustum *apFrustum, iTexture *apInputTexture, cRenderTarget *apRenderTarget);
		void EndRendering();
		void CopyToFrameBuffer(iTexture *apOutputTexture);

		tPostEffectMap m_mapPostEffects;
		std::vector<iPostEffect*> mvPostEffects;

		iFrameBuffer *mpFinalTempBuffer[2];

		float mfCurrentFrameTime;
	};

	//------------------------------------------

};
#endif // HPL_POSTEFFECT_COMPOSITE_H
