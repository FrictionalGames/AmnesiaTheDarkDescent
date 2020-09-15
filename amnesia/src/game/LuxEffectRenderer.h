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

#ifndef LUX_EFFECT_RENDERER_H
#define LUX_EFFECT_RENDERER_H

//----------------------------------------------

#include "LuxBase.h"

class cGlowObject
{
public:
	cGlowObject() {}
	cGlowObject(iRenderable* apObject, float afAlpha) : mpObject(apObject), mfAlpha(afAlpha) {}

	iRenderable* mpObject;
	float mfAlpha;
};

//----------------------------------------------

class cLuxEffectRenderer : public iLuxUpdateable
{
public:	
	cLuxEffectRenderer();
	~cLuxEffectRenderer();

	void Reset();

	void Update(float afTimeStep);

	void ClearRenderLists();

	void RenderSolid(cRendererCallbackFunctions* apFunctions);
	void RenderTrans(cRendererCallbackFunctions* apFunctions);

	void AddOutlineObject(iRenderable *apObject);
	void ClearOutlineObjects();

	void AddFlashObject(iRenderable *apObject, float afAlpha);
	void AddEnemyGlow(iRenderable *apObject, float afAlpha);

private:
	void RenderFlashObjects(cRendererCallbackFunctions* apFunctions);
	void RenderEnemyGlow(cRendererCallbackFunctions* apFunctions);
	
	void RenderOutline(cRendererCallbackFunctions* apFunctions);
	void RenderOutlineBlur(cRendererCallbackFunctions* apFunctions, iTexture *apInputTex);
		
	std::vector<cGlowObject> mvFlashObjects;
	std::vector<cGlowObject> mvEnemyGlowObjects;
	
	std::vector<iRenderable*> mvOutlineObjects;

	iFrameBuffer *mpDeferredAccumBuffer;
	iFrameBuffer *mpFrameBufferColor;

	int mlBlurSizeDiv;
	iGpuProgram *mpBlurProgram[2];
	iFrameBuffer *mpBlurBuffer[2];
	iTexture *mpBlurTexture[2];

	iTexture *mpOutlineColorTexture;

	iGpuProgram *mpOutlineColorProgram[2];
	iGpuProgram *mpOutlineStencilProgram;
	iGpuProgram *mpOutlineStencilAlphaProgram;

	iGpuProgram *mpFlashProgram;
	cLinearOscillation mFlashOscill;

	iGpuProgram *mpEnemyGlowProgram;

	cMatrixf m_mtxTemp;
};

//----------------------------------------------


#endif // LUX_EFFECT_RENDERER_H
