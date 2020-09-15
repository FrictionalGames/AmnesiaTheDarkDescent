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

#ifndef LUX_PLAYER_HELPERS_H
#define LUX_PLAYER_HELPERS_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxPlayer;

//----------------------------------------------

class cLuxPlayerInsanityCollapse : public iLuxPlayerHelper
{
friend class cLuxPlayer_SaveData;
public:
	cLuxPlayerInsanityCollapse(cLuxPlayer *apPlayer);
	~cLuxPlayerInsanityCollapse();

	void Reset();

	void Start();
	void Stop();
	bool IsActive(){ return mbActive; }

	void Update(float afTimeStep);

private:
	bool mbActive;

	int mlState;

	float mfHeightAddCollapseSpeed;
	float mfHeightAddAwakeSpeed;
	float mfRollCollapseSpeed;
	float mfRollAwakeSpeed;

	float mfHeightAddGoal;
	
	float mfAwakenSanity;
    
	float mfSleepTime;

	float mfSleepSpeedMul;
	float mfWakeUpSpeedMul;
	
	tString msStartSound;
	tString msAwakenSound;
	tString msSleepLoopSound;
	float mfSleepLoopSoundVolume;

	tString msSleepRandomSound;
	float mfSleepRandomMinTime;
	float mfSleepRandomMaxTime;
	
	float mfHeightAdd;
	float mfRoll;

	float mfT;
	float mfRandomCount;

	cSoundEntry *mpLoopSound;
	int mlLoopSoundID;
};

//----------------------------------------------

class cLuxPlayerCamDirEffects : public iLuxPlayerHelper
{
public:
	cLuxPlayerCamDirEffects(cLuxPlayer *apPlayer);
	~cLuxPlayerCamDirEffects();

	void Reset();

	void Update(float afTimeStep);

	float AddAndGetYawAdd(float afX);
	float AddAndGetPitchAdd(float afX);

private:
	void SetSwayActive(bool abX);
	void UpdateSway(float afTimeStep);

	float mfStartSwayMaxSanity;

	bool mbSwayActive;

	tVector2fList mlstPrevAdd;
	cVector2f mvSwayAdd;
	int mlMaxPositions;

	float mfSwayAlpha;

	cVector2f mvNextAdd;
};

//----------------------------------------------

class cLuxPlayerSpawnPS_SpawnPos
{
public:
	cParticleSystem *mpPS;
	cVector3f mvPos;
	cVector3f mvLastLocalPos;
};

class cLuxPlayerSpawnPS : public iLuxPlayerHelper
{
public:
	cLuxPlayerSpawnPS (cLuxPlayer *apPlayer);
	~cLuxPlayerSpawnPS();

	void Reset();

	void Start(const tString& asFileName);
	void Stop();

	void RespawnAll();

	bool IsActive(){ return mbActive;}
	const tString& GetFileName(){ return msFileName;}

	void Update(float afTimeStep);
private:
	void GenerateAllSpawnPos();
	cParticleSystem* CreatePS(cLuxPlayerSpawnPS_SpawnPos *apSpawnPos);

	void DestroyAllSpawnPoints();

	bool LoadSpawnPSFile(const tString& asFileName);

	bool mbActive;
	tString msFileName;
	
	tString msParticleSystem;
	float mfHeightFromFeet;
	float mfHeightAddMin;
	float mfHeightAddMax;
	float mfDensity;
	float mfRadius;
	cColor mPSColor;
	bool mbFadePS;
	float mfPSMinFadeStart;
	float mfPSMinFadeEnd;
	float mfPSMaxFadeStart;
	float mfPSMaxFadeEnd;

	std::vector<cLuxPlayerSpawnPS_SpawnPos> mvSpawnPos;
};

//----------------------------------------------

class cLuxPlayerHurt : public iLuxPlayerHelper
{
public:
	cLuxPlayerHurt (cLuxPlayer *apPlayer);
	~cLuxPlayerHurt();

	void Reset();

	void Update(float afTimeStep);

	void OnDraw(float afFrameTime);

private:
	std::vector<cGuiGfxElement*> mvNoiseGfx;
	float mfEffectStartHealth;	
	float mfMinMoveMul;
	float mfMaxPantCount;
	float mfPantSpeed;
	float mfPantSize;

	int mlCurrentNoise;
	float mfNoiseUpdateCount;

	float mfNoiseAlpha;
	float mfNoiseFreq;
	cColor mNoiseColor;
	
	float mfPantCount;
	float mfPantPosAdd;
	float mfPantPosAddVel;
	float mfPantPosAddDir;
	float mfAlpha;

	float mfHealthRegainSpeed;
	float mfHealthRegainLimit;
};

//----------------------------------------------

class cLuxFlashbackData
{
public:
	cLuxFlashbackData(){}
	cLuxFlashbackData(const tString &asFile, const tString &asCallback) : msFile(asFile), msCallback(asCallback) {}

	tString msFile;
	tString msCallback;
};

typedef std::list<cLuxFlashbackData> tLuxFlashbackDataList;
typedef tLuxFlashbackDataList::iterator tLuxFlashbackDataListIt;

//----------------------------------------------

class cLuxPlayerFlashback : public iLuxPlayerHelper
{
friend class cLuxPlayer_SaveData;
public:
	cLuxPlayerFlashback (cLuxPlayer *apPlayer);
	~cLuxPlayerFlashback ();

	void Reset();
	
	void Start(const tString &asFlashbackFile, const tString &asCallback);

	void Update(float afTimeStep);

	void OnDraw(float afFrameTime);

	bool IsActive(){ return mbActive; }

private:
	void LoadAndPlayFlashbackFile(const tString& asFlashbackFile);

	//Data
	float mfRadialBlurSize;
	float mfRadialBlurStartDist;
	float mfWorldSoundVolume;
	float mfMoveSpeedMul;
	float mfRunSpeedMul;

	//Variables
	float mfFlashDelay;
	bool mbActive;
	float mfFlashbackStartCount;
	tString msFlashbackFile;
	tString msCallback;
	tLuxFlashbackDataList mlstFlashbackQueue;
	
};

//---------------------------------------------

class cLuxPlayerLookAt : public iLuxPlayerHelper
{
friend class cLuxPlayer_SaveData;
public:
	cLuxPlayerLookAt(cLuxPlayer *apPlayer);
	~cLuxPlayerLookAt();

	void Update(float afTimeStep);

	void Reset();

	void SetTarget(const cVector3f &avTargetPos, float afSpeedMul, float afMaxSpeed, const tString& asAtTargetCallback);
	
	void SetActive(bool abX);
	bool IsActive(){ return mbActive;}

private:
	bool mbActive;
	cVector3f mvTargetPos;

	cVector3f mvCurrentSpeed;
	float mfSpeedMul;
	float mfMaxSpeed;

	tString msAtTargetCallback;

	float mfDestFovMul;
	float mfFov;
	float mfFovSpeed;
	float mfFovMaxSpeed;
};

//----------------------------------------------

class cLuxPlayerSanity : public iLuxPlayerHelper
{
friend class cLuxPlayer_SaveData;
public:
	cLuxPlayerSanity(cLuxPlayer *apPlayer);
	~cLuxPlayerSanity();

	void Reset();

	void StartHit();
	void SetSanityLost();

	void Update(float afTimeStep);

	void OnDraw(float afFrameTime);

	float GetAtLowSanityCount(){ return mfAtLowSanityCount;}

private:
	float GetCurrentSizeMul();

	void UpdateInsaneEffects(float afTimeStep);
	void UpdateCheckEnemySeen(float afTimeStep);
	void UpdateHit(float afTimeStep);
	
	void UpdateInsanityVisuals(float afTimeStep);
	void UpdateEnemySeenEffect(float afTimeStep);
	void UpdateLosingSanity(float afTimeStep);
	void UpdateLowSanity(float afTimeStep);
	
	float mfHitAlpha;
	bool mbHitActive;
	float mfSanityLostCount;

	float mfAtLowSanityCount;

	bool mbHitIsUpdated;
	bool mbSanityLostIsUpdated;

	float mfPantCount;

	float mfCheckEnemySeenCount;

	bool mbSanityEffectUpdated;

	float mfT;
	float mfInsaneWaveAlpha;

	float mfSanityDrainCount;
	float mfSanityDrainVolume;
	float mfSanityHeartbeatCount;

	float mfSeenEnemyCount;
	bool mbEnemyIsSeen;

	float mfShowHintTimer;

	//////////////
	// Data
	float mfHitZoomInSpeed;
	float mfHitZoomOutSpeed;
	float mfHitZoomInFOVMul;
	float mfHitZoomInAspectMul;

	float mfSanityRegainSpeed;
	float mfSanityRegainLimit;
	float mfSanityVeryLowLimit;
	float mfSanityEffectsStart;

	float mfSanityWaveAlphaMul;
	float mfSanityWaveSpeedMul;

	float mfSanityLowLimit;
	float mfSanityLowLimitMaxTime;
	float mfSanityLowNewSanityAmount;

	float mfCheckNearEnemyInterval;
	float mfNearEnemyDecrease;
	float mfNearCritterDecrease;
};

//----------------------------------------------

class cLuxPlayerLantern : public iLuxPlayerHelper
{
public:	
	cLuxPlayerLantern(cLuxPlayer *apPlayer);
	~cLuxPlayerLantern();

	void OnStart();
	void Update(float afTimeStep);
	void Reset();

	void OnMapEnter(cLuxMap *apMap);
	void OnMapLeave(cLuxMap *apMap);

	void CreateWorldEntities(cLuxMap *apMap);
	void DestroyWorldEntities(cLuxMap *apMap);

	void SetActive(bool abX, bool abUseEffects, bool abCheckForOilAndItems=true, bool abCheckIfAllowed=true);
	bool IsActive(){ return mbActive;}

	void SetDisabled(bool abX);
	bool GetDisabled(){ return mbDisabled;}

	iLight* GetLight(){ return mpLight;}

private:
	cColor mDefaultColor;
	float mfRadius;
	tString msGobo;
	bool mbCastShadows;
	cVector3f mvLocalOffset;
	tString msTurnOnSound;
	tString msTurnOffSound;
	tString msOutOfOilSound;
	tString msDisabledSound;
	float mfLowerOilSpeed;
	float mfFadeLightOilAmount;
	
	bool mbDisabled;
	bool mbActive;
	float mfAlpha;
	cLightPoint *mpLight;
	
};

//----------------------------------------------

class cLuxPlayerDeath : public iLuxPlayerHelper
{
public:
	cLuxPlayerDeath(cLuxPlayer *apPlayer);
	~cLuxPlayerDeath();

	void LoadFonts();
	void LoadUserConfig();
	void SaveUserConfig();

	void Reset();

	void Start();

	void Update(float afTimeStep);
	void PostUpdate(float afTimeStep);


	void OnDraw(float afFrameTime);

	void OnPressButton();

	float GetFadeAlpha(){ return mfFadeAlpha;}

	void DisableStartSound(){ mbSkipStartSound = true; }

	bool ShowHint(){ return mbShowHint;}
	void SetShowHint(bool abX){ mbShowHint=abX;}

	void SetHint(const tString& asCat, const tString& asEntry);
	const tString& GetHintCat(){ return msHintCat; }
	const tString& GetHintEntry(){ return msHintEntry; }



private:
	void ResetGame();

	bool mbToMainMenu;
	bool mbActive;

	bool mbShowHint;

	int mlState;

	tString msStartSound;
	tString msAwakenSound;

	bool mbSkipStartSound;

	float mfHeightAddSpeed;
	float mfRollSpeed;

	float mfHeightAddGoal;
	float mfHeightAddGoalCrouch;
	float mfFadeOutTime;

	float mfMaxSanityGain;
	float mfMaxHealthGain;
	float mfMaxOilGain;
	float mfMinSanityGain;
	float mfMinHealthGain;
	float mfMinOilGain;

	float mfHeightAdd;
	float mfRoll;

	float mfT;

	float mfMinHeightAdd;

	tString msHintCat;
	tString msHintEntry;

	tWString msCurrentHintText;

	cGuiGfxElement *mpWhiteModGfx;

	iFontData *mpFont;
    	
	float mfFadeAlpha;
	float mfTextAlpha1;
	float mfTextAlpha2;
	float mfTextOnScreenCount;
	float mfWhiteCount;
	
	cLinearOscillation mFlashOscill;

	cSoundEntry *mpVoiceEntry;
	int mlVoiceEntryId;
};


//----------------------------------------------

class cLuxPlayerLean : public iLuxPlayerHelper
{
public:	
	cLuxPlayerLean(cLuxPlayer *apPlayer);
	~cLuxPlayerLean();
	
	void CreateWorldEntities(cLuxMap *apMap);
	void DestroyWorldEntities(cLuxMap *apMap);

	void Update(float afTimeStep);

	void SetLean(float afMul);
	void AddLean(float afAdd);

	void Reset();
private:
	iCollideShape *mpHeadShape;

	float mfDir;
	float mfDirAdd;
	float mfMovement;
	float mfRotation;

	float mfMaxMovement;
	float mfMaxRotation;
	float mfMaxTime;

	float mfMoveSpeed;

	bool mbIntersect;

	bool mbPressed;
};

//----------------------------------------------

class cLuxPlayerDamageData
{
public:
	std::vector<cGuiGfxElement*> mvImages;
};

class cLuxPlayerHudEffect_Splash
{
public:
	cGuiGfxElement *mpImage;	
	cVector3f mvPos;
	cVector2f mvSize;
	float mfAlpha;
	float mfAlphaMul;

	cVector3f mvPosVel;
	cVector2f mvSizeVel;
	float mfAlphaVel;
	float mfAlphaMoveStart;
};

typedef std::list<cLuxPlayerHudEffect_Splash> cLuxPlayerHudEffect_SplashList;
typedef cLuxPlayerHudEffect_SplashList::iterator cLuxPlayerHudEffect_SplashListIt;

class cLuxPlayerHudEffect : public iLuxPlayerHelper
{
public:
	cLuxPlayerHudEffect(cLuxPlayer *apPlayer);
	~cLuxPlayerHudEffect();

	void AddDamageSplash(eLuxDamageType aType);
	void Flash(const cColor& aColor, eGuiMaterial aFlashMaterial, float afInTime, float afOutTime);

	void OnDraw(float afFrameTime);
	void Update(float afTimeStep);
	void Reset();
	
private:
	void DrawSplashes(float afFrameTime);
	void UpdateSplashes(float afTimeStep);

	void DrawFlash(float afFrameTime);
	void UpdateFlash(float afTimeStep);

	void LoadDamageData(cLuxPlayerDamageData *apData, const tString& asName);

	std::vector<cLuxPlayerDamageData> mvDamageTypes;
	cLuxPlayerHudEffect_SplashList mlstSplashes;

	cGuiGfxElement *mpFlashGfx;
	cColor mFlashColor;
	eGuiMaterial mFlashMaterial;
	float mfFlashAlphaSpeed;
	float mfFlashAlphaOutSpeed;
	float mfFlashAlpha;
	bool mbFlashActive;
};


//----------------------------------------------

class cLuxPlayerLightLevel : public iLuxPlayerHelper
{
public:	
	cLuxPlayerLightLevel(cLuxPlayer *apPlayer);
	~cLuxPlayerLightLevel();
	
	void OnStart();
	void Update(float afTimeStep);
	void Reset();

	void OnMapEnter(cLuxMap *apMap);

	float GetExtendedLightLevel(){ return mfExtendedLightLevel;}
	float GetNormalLightLevel(){ return mfNormalLightLevel;}

private:
	float mfExtendedLightLevel;	//Uses longer range on point lights
	float mfNormalLightLevel;	//Uses normal radius
	float mfUpdateCount;

	float mfRadiusAdd;
};

//----------------------------------------------

class cLuxPlayerInDarkness : public iLuxPlayerHelper
{
friend class cLuxPlayer_SaveData;
public:	
	cLuxPlayerInDarkness(cLuxPlayer *apPlayer);
	~cLuxPlayerInDarkness();

	void OnStart();
	void Update(float afTimeStep);
	void Reset();

	void OnMapEnter(cLuxMap *apMap);
	void OnMapLeave(cLuxMap *apMap);

	void CreateWorldEntities(cLuxMap *apMap);
	void DestroyWorldEntities(cLuxMap *apMap);

	cLightPoint* GetAmbientLight(){ return mpAmbientLight;}

	bool InDarkness();

	void SetActive(bool abX);

private:
	cSoundHandler *mpSoundHandler;

	float mfMinDarknessLightLevel;
	
	float mfAmbientLightMinLightLevel;
	float mfAmbientLightRadius;
	float mfAmbientLightIntensity;
	float mfAmbientLightFadeInTime;
	float mfAmbientLightFadeOutTime;
	cColor mAmbientLightColor;

	tString msLoopSoundFile;
	float mfLoopSoundVolume;
	float mfLoopSoundStartupTime;
	float mfLoopSoundFadeInSpeed;
	float mfLoopSoundFadeOutSpeed;

	float mfSanityLossPerSecond;
	float mfSanityLossMul;

	bool mbInDarkness;
	float mfShowHintTimer;

	cSoundEntry *mpLoopSound;
	float mfLoopSoundCount;

	cLightPoint *mpAmbientLight;
	bool mbAmbientLightIsOn;

	bool mbActive;
};

//----------------------------------------------


#endif // LUX_MAP_HANDLER_H
