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

#include "LuxPostEffects.h"

#include "LuxMapHandler.h"

//////////////////////////////////////////////////////////////////////////
// VARIABLES
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

#define kVar_afAlpha			0
#define kVar_afT				1
#define kVar_avScreenSize		2
#define kVar_afAmpT				3
#define kVar_afWaveAlpha		4
#define kVar_afZoomAlpha		5

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// INSANITY
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPostEffect_Insanity::cLuxPostEffect_Insanity(cGraphics *apGraphics, cResources *apResources) : iLuxPostEffect(apGraphics, apResources)
{
	//////////////////////////////
	// Create program
	cParserVarContainer vars;
	vars.Add("UseUv");
	mpProgram = mpGraphics->CreateGpuProgramFromShaders("LuxInsanity","deferred_base_vtx.glsl", "posteffect_insanity_frag.glsl", &vars);
	if(mpProgram)
	{
		mpProgram->GetVariableAsId("afAlpha",kVar_afAlpha);
		mpProgram->GetVariableAsId("afT",kVar_afT);
		mpProgram->GetVariableAsId("avScreenSize",kVar_avScreenSize);
		mpProgram->GetVariableAsId("afAmpT",kVar_afAmpT);
		mpProgram->GetVariableAsId("afWaveAlpha",kVar_afWaveAlpha);
		mpProgram->GetVariableAsId("afZoomAlpha",kVar_afZoomAlpha);
	}


	//////////////////////////////
	// Textures
	mvAmpMaps.resize(3);
	
	for(size_t i=0; i<mvAmpMaps.size(); ++i)
		mvAmpMaps[i] = mpResources->GetTextureManager()->Create2D("posteffect_insanity_ampmap"+cString::ToString((int)i), false);

	mpZoomMap = mpResources->GetTextureManager()->Create2D("posteffect_insanity_zoom.jpg", false);

	//////////////////////////////
	// Init vars
	mfT =0;
	mfAnimCount =0;
	mfWaveAlpha = 0.0f;
	mfZoomAlpha = 0.0f;
	mfWaveSpeed =0.0f;
}

//-----------------------------------------------------------------------

cLuxPostEffect_Insanity::~cLuxPostEffect_Insanity()
{

}

//-----------------------------------------------------------------------

void cLuxPostEffect_Insanity::Update(float afTimeStep)
{
	mfT += afTimeStep * mfWaveSpeed;
	
	mfAnimCount += afTimeStep * 0.15f;

	float fMaxAnim = (float)mvAmpMaps.size();
	if(mfAnimCount >= fMaxAnim) mfAnimCount = mfAnimCount-fMaxAnim;
}

//-----------------------------------------------------------------------


iTexture* cLuxPostEffect_Insanity::RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer)
{
	/////////////////////////
	// Init render states
	mpCurrentComposite->SetFlatProjection();
	mpCurrentComposite->SetBlendMode(eMaterialBlendMode_None);
	mpCurrentComposite->SetChannelMode(eMaterialChannelMode_RGBA);

	/////////////////////////
	// Render the to final buffer
	// This function sets to frame buffer if post effect is last!
	SetFinalFrameBuffer(apFinalTempBuffer);

	mpCurrentComposite->SetTexture(0, apInputTexture);
	
	int lAmp0 = (int)mfAnimCount;
	int lAmp1 = (int)(mfAnimCount+1);
	if(lAmp1 >= (int) mvAmpMaps.size()) lAmp1 = 0;
	float fAmpT = cMath::GetFraction(mfAnimCount);

	//Log("AnimCount: %f - %d %d - %f\n", mfAnimCount, lAmp0, lAmp1, fAmpT);

	mpCurrentComposite->SetTexture(1, mvAmpMaps[lAmp0]);
	mpCurrentComposite->SetTexture(2, mvAmpMaps[lAmp1]);
	mpCurrentComposite->SetTexture(3, mpZoomMap);
	
	mpCurrentComposite->SetProgram(mpProgram);
	if(mpProgram)
	{
		mpProgram->SetFloat(kVar_afAlpha, 1.0f);
		mpProgram->SetFloat(kVar_afT, mfT);
		mpProgram->SetVec2f(kVar_avScreenSize, mpLowLevelGraphics->GetScreenSizeFloat());
		mpProgram->SetFloat(kVar_afAmpT, fAmpT);
		mpProgram->SetFloat(kVar_afWaveAlpha, mfWaveAlpha);
		mpProgram->SetFloat(kVar_afZoomAlpha, mfZoomAlpha);
	}
	

	DrawQuad(0,1,apInputTexture, true);

	mpCurrentComposite->SetTextureRange(NULL, 1);
	
	return apFinalTempBuffer->GetColorBuffer(0)->ToTexture();
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// POST EFFECT HANDLER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPostEffectHandler::cLuxPostEffectHandler() : iLuxUpdateable("LuxPostEffectHandler")
{
	cGraphics *pGraphics = gpBase->mpEngine->GetGraphics();
	cResources *pResources = gpBase->mpEngine->GetResources();

	///////////////////////
	// Create post effects
	mpInsanity = hplNew(cLuxPostEffect_Insanity, (pGraphics, pResources) );
	AddEffect(mpInsanity, 25);
	mpInsanity->SetActive(true);
}

//-----------------------------------------------------------------------

cLuxPostEffectHandler::~cLuxPostEffectHandler()
{
	STLDeleteAll(mvPostEffects);
}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::OnStart()
{

}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::Update(float afTimeStep)
{
	for(size_t i=0; i<mvPostEffects.size(); ++i)
	{
		iLuxPostEffect *pPostEffect = mvPostEffects[i];

        if(pPostEffect->IsActive()) pPostEffect->Update(afTimeStep);
	}
}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::Reset()
{

}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::LoadMainConfig()
{
	cConfigFile *pMainCfg = gpBase->mpMainConfig;

	mpInsanity->SetDisabled(pMainCfg->GetBool("Graphics", "PostEffectInsanity", true)==false);

}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::SaveMainConfig()
{
	cConfigFile *pMainCfg = gpBase->mpMainConfig;

	pMainCfg->SetBool("Graphics", "PostEffectInsanity", mpInsanity->IsDisabled()==false);
}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::AddEffect(iLuxPostEffect *apPostEffect, int alPrio)
{
	mvPostEffects.push_back(apPostEffect);
	apPostEffect->SetActive(false);
	gpBase->mpMapHandler->GetViewport()->GetPostEffectComposite()->AddPostEffect(apPostEffect, alPrio);
}

//-----------------------------------------------------------------------
