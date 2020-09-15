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

#ifndef LUX_EFFECT_HANDLER_H
#define LUX_EFFECT_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------


class iLuxEffect
{
public:
	iLuxEffect() : mbActive(false) {}
	~iLuxEffect(){}

	virtual void Update(float afTimeStep)=0;
	virtual void OnDraw(float afFrameTime)=0;
	virtual void Reset()=0;

	virtual void DoAction(eLuxPlayerAction aAction, bool abPressed){}


	bool IsActive(){ return mbActive;}
	void SetActive(bool abX){ mbActive = abX;}

protected:
	bool mbActive;    
};


//----------------------------------------------

class cLuxEffect_PlayCommentary : public iLuxEffect
{
public:
	cLuxEffect_PlayCommentary();
	~cLuxEffect_PlayCommentary();

	void Start(const tString &asTalker,const tString &asTopic, const tString &asFile, int alIconId);
	void Stop();
	
	void Update(float afTimeStep);
	void OnDraw(float afFrameTime);
	void Reset();

private:
	//////////////////////
	//Data
	cSoundHandler *mpSoundHandler;
	cMusicHandler *mpMusicHandler;
	
	//////////////////////
	//Variables
	cSoundEntry *mpSoundEntry;
	int mlSoundEntryID;

	tString msTalker;
	tString msTopic;
	int mlIconID;
};

//----------------------------------------------

class cLuxEffect_EmotionFlash : public iLuxEffect
{
public:
	cLuxEffect_EmotionFlash();
	~cLuxEffect_EmotionFlash();

	void ClearFonts();
	void LoadFonts();

	void Start(const tString &asTextCat, const tString &asTextEntry, const tString &asSound);
	void Reset();
	
	void Update(float afTimeStep);
	void OnDraw(float afFrameTime);
	
	void DoAction(eLuxPlayerAction aAction, bool abPressed);

private:
	cGuiGfxElement *mpWhiteGfx;
	iFontData *mpFont;
	cVector2f mvFontSize;

	float mfAlpha;

	int mlStep;
	float mfCount;
	
	float mfTextAlpha;
	float mfTextTime;
	tWStringVec mvTextRows;
};

//----------------------------------------------

class cLuxEffect_RadialBlur : public iLuxEffect
{
	friend class cLuxEffectHandler_SaveData;
public:
	cLuxEffect_RadialBlur();

	void SetBlurStartDist(float afDist);
	void FadeTo(float afSize, float afSpeed);

	float GetCurrentSize(){ return mfSize;}

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime){}
	void Reset();

private:
	float mfSize;
	float mfSizeGoal;
	float mfFadeSpeed;
	float mfBlurStartDist;
};

//----------------------------------------------

class cLuxEffect_SepiaColor : public iLuxEffect
{
	friend class cLuxEffectHandler_SaveData;
public:
	cLuxEffect_SepiaColor();

	void FadeTo(float afAmount, float afSpeed);

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime){}
	void Reset();

private:
	float mfAmount;
	float mfAmountGoal;
	float mfFadeSpeed;
};

//-----------------------------------------

class cLuxEffect_ShakeScreen_Shake
{
public:
	float mfMaxSize;
	float mfSize;
	float mfTime;
	float mfFadeInTime;
	float mfMaxFadeInTime;
	float mfFadeOutTime;
	float mfMaxFadeOutTime;
};

class cLuxEffect_ShakeScreen : public iLuxEffect
{
public:
	cLuxEffect_ShakeScreen();
	~cLuxEffect_ShakeScreen();

	void Start(float afAmount, float afTime, float afFadeInTime,float afFadeOutTime);

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime){}
	void Reset();

private:

	std::list<cLuxEffect_ShakeScreen_Shake> mlstShakes;
};

//----------------------------------------------

class cLuxEffect_ImageTrail : public iLuxEffect
{
friend class cLuxEffectHandler_SaveData;
public:
	cLuxEffect_ImageTrail();

	void FadeTo(float afAmount, float afSpeed);

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime){}
	void Reset();

private:
	float mfAmount;
	float mfAmountGoal;
	float mfFadeSpeed;
};

//----------------------------------------------

class cLuxEffect_Fade : public iLuxEffect
{
friend class cLuxEffectHandler_SaveData;
public:
	cLuxEffect_Fade();
	~cLuxEffect_Fade();

	void FadeIn(float afTime);
	void FadeOut(float afTime);

	bool IsFading();

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime);
	void Reset();

	void SetDirectAlpha(float afX);

private:
	cGuiGfxElement *mpWhiteGfx;

	float mfGoalAlpha;
	float mfAlpha;
	float mfFadeSpeed;
};


//----------------------------------------------

class cLuxEffect_SanityGainFlash : public iLuxEffect
{
public:
	cLuxEffect_SanityGainFlash();
	~cLuxEffect_SanityGainFlash();

	void Start();

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime);
	void Reset();

	void DrawFlash(cGuiSet *apSet ,float afTimeStep);

private:
	cGuiGfxElement *mpWhiteGfx;

	float mfAlpha;

	int mlStep;
	float mfCount;

	float mfFadeInSpeed;
	float mfWhiteSpeed;
	float mfFadeOutSpeed;

	cColor mColor;
	tString msSound;
	float mfFadeInTime;
	float mfFadeOutTime;
};

//----------------------------------------------

class cLuxEffect_Flash : public iLuxEffect
{
public:
	cLuxEffect_Flash();
	~cLuxEffect_Flash();
	
	void Start(float afFadeIn, float afWhite, float afFadeOut);
	void Reset();

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime);

private:
	cGuiGfxElement *mpWhiteGfx;

	float mfAlpha;

	int mlStep;
	float mfCount;

	float mfFadeInSpeed;
	float mfWhiteSpeed;
	float mfFadeOutSpeed;
};

//----------------------------------------------

typedef std::list<cLuxVoiceData> tLuxVoiceDataList;
typedef tLuxVoiceDataList::iterator tLuxVoiceDataListIt;

class cLuxEffect_PlayVoice : public iLuxEffect
{
friend class cLuxEffectHandler_SaveData;
public:
	cLuxEffect_PlayVoice();
	~cLuxEffect_PlayVoice();

	void StopVoices(float afFadeOutSpeed);
	void AddVoice(	const tString& asVoiceFile, const tString& asEffectFile,
					const tString& asTextCat, const tString& asTextEntry, bool abUsePostion, 
					const cVector3f& avPosition, float afMinDistance, float afMaxDistance);

	void PauseCurrentVoices();
	void UnpauseCurrentVoices();

	void SetOverCallback(const tString& asFunc){ msOverCallback = asFunc;}

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime);
	void Reset();

	void SetVolumeMul(float afMul);
	float GetVolumeMul(float afMul){ return mfVolumeMul;}

	/**
	 * Returns true if all voices (not effect files) are done playing
	 */
	bool VoiceDonePlaying();

private:
	//////////////////////
	//Data
	cSoundHandler *mpSoundHandler;
	cVector2f mvFontSize;
	float mfRowWidth;

	//////////////////////
	//Variables
	bool mbPaused;
	tString msOverCallback;
	tLuxVoiceDataList mlstVoices;
	cSoundEntry *mpVoiceEntry;
	int mlVoiceEntryID;
	cSoundEntry *mpEffectEntry;
	int mlEffectEntryID;
	std::vector<tWString> mvCurrentTextRows;

	float mfVolumeMul;
};

//----------------------------------------------

class cLuxEffectHandler : public iLuxUpdateable
{
public:	
	cLuxEffectHandler();
	~cLuxEffectHandler();

	///////////////////////////
	// General
	void OnClearFonts();
	void LoadFonts();
	void OnStart();
	void Update(float afTimeStep);
	void Reset();

	void OnMapEnter(cLuxMap *apMap);
	void OnMapLeave(cLuxMap *apMap);

	void OnDraw(float afFrameTime);

	void DoAction(eLuxPlayerAction aAction, bool abPressed);
	
	///////////////////////////
	// Properties
	bool GetPlayerIsPaused(){ return mbPlayerIsPaused;}
	void SetPlayerIsPaused(bool abX);

	///////////////////////////
	// Effects
	cLuxEffect_Fade *GetFade(){ return mpFade;}
	cLuxEffect_Flash *GetFlash(){ return mpFlash;}
	cLuxEffect_SanityGainFlash *GetSanityGainFlash(){ return mpSanityGainFlash;}
	cLuxEffect_PlayVoice *GetPlayVoice(){ return mpPlayVoice;}
	cLuxEffect_ImageTrail *GetImageTrail(){ return mpImageTrail;}
	cLuxEffect_ShakeScreen *GetScreenShake(){ return mpScreenShake;}
	cLuxEffect_SepiaColor *GetSepiaColor(){ return mpSepiaColor;}
	cLuxEffect_RadialBlur *GetRadialBlur(){ return mpRadialBlur;}
	cLuxEffect_EmotionFlash *GetEmotionFlash(){ return mpEmotionFlash;}
	cLuxEffect_PlayCommentary *GetPlayCommentary(){ return  mpPlayCommentary;}

private:
	cLuxEffect_Fade *mpFade;
	cLuxEffect_Flash *mpFlash;
	cLuxEffect_SanityGainFlash *mpSanityGainFlash;
	cLuxEffect_PlayVoice *mpPlayVoice;
	cLuxEffect_ImageTrail *mpImageTrail;
	cLuxEffect_ShakeScreen *mpScreenShake;
	cLuxEffect_SepiaColor *mpSepiaColor;
	cLuxEffect_RadialBlur *mpRadialBlur;
	cLuxEffect_EmotionFlash *mpEmotionFlash;
	cLuxEffect_PlayCommentary *mpPlayCommentary;

	std::vector<iLuxEffect*> mvEffects;	

	bool mbPlayerIsPaused;
};

//----------------------------------------------


#endif // LUX_EFFECT_HANDLER_H
