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

#ifndef LUX_PLAYER_H
#define LUX_PLAYER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class iLuxMoveState;
class iLuxPlayerState;

class cLuxPlayerLightLevel;
class cLuxPlayerInDarkness;
class cLuxPlayerHudEffect;
class cLuxPlayerLean;
class cLuxPlayerDeath;
class cLuxPlayerLantern;
class cLuxPlayerSanity;
class cLuxPlayerLookAt;
class cLuxPlayerFlashback;
class cLuxPlayerHurt;
class cLuxPlayerSpawnPS;
class cLuxPlayerCamDirEffects;
class cLuxPlayerInsanityCollapse;
class cLuxNode_PlayerStart;

class cLuxPlayerHands;

//----------------------------------------------

enum eLuxFocusIconStyle
{
	eLuxFocusIconStyle_Default,
	eLuxFocusIconStyle_Simple,

	eFocusIconStyle_LastEnum,
};

//----------------------------------------------

class cLuxPlayer : public iLuxUpdateable, public iLuxCollideCallbackContainer
{
friend class cLuxPlayer_SaveData;
friend class cLuxHelpFuncs;
public:	
	cLuxPlayer();
	~cLuxPlayer();
	
	///////////////////
	//Main callbacks
	void LoadFonts();
	void OnStart();
	void Update(float afTimeStep);
	void PostUpdate(float afTimeStep);
	void Reset();
	void OnDraw(float afFrameTime);

	void LoadUserConfig();
	void SaveUserConfig();

	void OnMapEnter(cLuxMap *apMap);
	void OnMapLeave(cLuxMap *apMap);

	void CreateWorldEntities(cLuxMap *apMap);
	void DestroyWorldEntities(cLuxMap *apMap);

	void OnEnterContainer(const tString& asOldContainer);
	void OnLeaveContainer(const tString& asNewContainer);
	
	void RenderSolid(cRendererCallbackFunctions* apFunctions);
	void RenderTrans(cRendererCallbackFunctions* apFunctions);

	////////////////////
	// Actions
	void GiveDamage(float afAmount, int alStrength, eLuxDamageType aType, bool abSpinHead, bool abLethal);
	
	void GiveSanityDamage(float afAmount);
	void LowerSanity(float afAmount, bool abUseEffect);

	void Move(eCharDir aDir, float afMul);
	void AddYaw(float afAmount);
	void AddPitch(float afAmount);
	void SetLean(float afMul);
	void AddLean(float afAdd);
	
	void Scroll(float afAmount);
	
	void DoAction(eLuxPlayerAction aAction, bool abPressed);
	
	void Run(bool abPressed);
	void Jump(bool abPressed);
	void Crouch(bool abPressed);
	
	void ChangeState(eLuxPlayerState aState);
	eLuxPlayerState GetCurrentState(){ return mState;}
	iLuxPlayerState* GetStateData(eLuxPlayerState aState){ return mvStates[aState];}
	iLuxPlayerState* GetCurrentStateData(){ return mvStates[mState];}

	void ChangeMoveState(eLuxMoveState aState);
	eLuxMoveState GetCurrentMoveState(){ return mMoveState;}
	iLuxMoveState* GetMoveStateData(eLuxMoveState aState){ return mvMoveStates[aState];}
	iLuxMoveState* GetCurrentMoveStateData(){ return mvMoveStates[mMoveState];}
	
	void MoveHeadPosAdd(eLuxHeadPosAdd aType, const cVector3f& avGoal, float afSpeed, float afSlowdownDist);

	void PlaceAtStartNode(cLuxNode_PlayerStart *apNode);

	////////////////////
	// Data
	cCamera* GetCamera(){ return mpCamera;}
	iCharacterBody* GetCharacterBody(){ return mpCharBody; }

	////////////////////
	// Variables Properties
	void SetHeadPosAdd(eLuxHeadPosAdd aType, const cVector3f& avVector);
	const cVector3f& GetHeadPosAdd(eLuxHeadPosAdd aType);
	const cVector3f& GetHeadPosAddSum(){ return mvHeadPosAddSum; }

	////////////////////
	// Properties
	void SetActive(bool abX);
	bool IsActive(){ return mbActive;}

	void SetUsePermaDeath(bool abX){mbUsePermaDeath = abX;}
	bool UsePermaDeath(){ return mbUsePermaDeath;}

	void SetCurrentPermaDeathSound(const tString& asSound){ msCurrentPermaDeathSound = asSound;}
	const tString& GetCurrentPermaDeathSound(){ return msCurrentPermaDeathSound;}

	void SetHealth(float afX);
	void SetSanity(float afX);
	void SetLampOil(float afX);

	void AddHealth(float afX);
	void AddSanity(float afX, bool abShowEffect=true);
	void AddLampOil(float afX);

	float GetHealth(){ return mfHealth; }
	float GetSanity(){ return mfSanity; }
	float GetLampOil(){ return mfLampOil; }

	float GetTerror(){ return mfTerror; }
	
	void SetTerror(float afX){ mfTerror=afX; }
	void AddTerrorEnemy(iLuxEnemy *apEnemy);
	void RemoveTerrorEnemy(iLuxEnemy *apEnemy);
	void ClearTerrorEnemies();
	void StopTerrorSound();

	int GetBodyNum(){ return 1;}
	iPhysicsBody* GetBody(int alIdx){ return mpCharBody->GetCurrentBody();};

	bool IsInWater(){ return mbIsInWater;}
	void SetIsInWater(bool abX){ mbIsInWater = abX;}
	void SetWaterStepSound(const tString& asStepSound){ msWaterStepSound = asStepSound; }
	const tString& GetWaterStepSound(){ return msWaterStepSound; }
	void SetWaterSpeedMul(float afX){ mfWaterSpeedMul = afX;}
	float GetWaterSpeedMul(){ return mfWaterSpeedMul;}

	void SetLookSpeedMul(float afX){ mfLookSpeedMul = afX;}
	float GetLookSpeedMul() { return mfLookSpeedMul;}

	void SetFocusText(const tWString &asText);

	int GetCoins(){ return mlCoins;}
	void SetCoins(int alX){ mlCoins = alX;}
	void AddCoins(int alX);

	int GetTinderboxes(){ return mlTinderboxes;}
	void SetTinderboxes(int alX){ mlTinderboxes = alX; }
	void AddTinderboxes(int alX){ mlTinderboxes += alX; }
	
	bool IsDead(){ return mfHealth <=0;}

	float GetDefaultMass() {return mfDefaultMass;}

	const cVector3f& GetCameraPosAdd() { return mvCameraPosAdd;}

	const cVector3f& GetBodySize(){ return mvBodySize;}
	const cVector3f& GetBodyCrouchSize(){ return mvBodyCrouchSize;}
	
	bool GetPressedMove(){ return mbPressedMove;}
	bool IsPressingRun(){ return mbPressingRun;}
	bool IsPressingJump(){ return mbPressingJump;}

	void SetJumpDisabled(bool abX){ mbJumpDisabled = abX;}
	bool GetJumpDisabled(){ return mbJumpDisabled;}

	void SetCrouchDisabled(bool abX){ mbCrouchDisabled = abX; }
	bool GetCrouchDisabled() { return mbCrouchDisabled; }

	void SetSanityDrainDisabled(bool abX){ mbSanityDrainDisabled = abX;}
	bool GetSanityDrainDisabled(){ return mbSanityDrainDisabled;}

	float GetInteractionMoveSpeedMul(){ return mfInteractionMoveSpeedMul;}
	void SetInteractionMoveSpeedMul(float afX){ mfInteractionMoveSpeedMul = afX;}

	void SetEventMoveSpeedMul(float afX){ mfEventMoveSpeedMul = afX;}
	void SetEventRunSpeedMul(float afX){ mfEventRunSpeedMul = afX;}
	float GetEventMoveSpeedMul(){ return mfEventMoveSpeedMul;}
	float GetEventRunSpeedMul(){ return mfEventRunSpeedMul;}
	
	void SetScriptMoveSpeedMul(float afX){ mfScriptMoveSpeedMul = afX;}
	void SetScriptRunSpeedMul(float afX){ mfScriptRunSpeedMul = afX;}
	float GetScriptMoveSpeedMul(){ return mfScriptMoveSpeedMul;}
	float GetScriptRunSpeedMul(){ return mfScriptRunSpeedMul;}

	void SetHurtMoveSpeedMul(float afX){ mfHurtMoveSpeedMul = afX;}
	float GetHurtMoveSpeedMul(){ return mfHurtMoveSpeedMul;}

	void SetInsanityCollapseSpeedMul(float afX){ mfInsanityCollapseSpeedMul = afX;}
	float GetInsanityCollapseSpeedMul(){ return mfInsanityCollapseSpeedMul;}
	
	void SetScriptJumpForceMul(float afX){ mfScriptJumpForceMul = afX;}
	float GetScriptJumpForceMul(){ return mfScriptJumpForceMul;}

	void SetCurrentFocusDistance(float afX){ mfCurrentFocusDistance = afX;}
	
	void SetEntityInFocus(iLuxEntity *apEntity){ mpEntityInFocus =apEntity;}
	iLuxEntity* GetEntityInFocus(){ return mpEntityInFocus;}


	void SetBodyInFocus(iPhysicsBody *apBody){ mpBodyInFocus =apBody;}
	iPhysicsBody* GetBodyInFocus(){ return mpBodyInFocus;}

	void SetScriptShowFocusIconAndCrossHair(bool abX){ mbScriptShowFocusIconAndCrossHair = abX;}
	
	void SetCurrentHandObjectDrawn(bool abX);

	float GetAvgSpeed(){ return mfAvgSpeed; }

	void FadeFOVMulTo(float afX, float afSpeed);
	void FadeAspectMulTo(float afX, float afSpeed);
	void FadeRollTo(float afX, float afSpeedMul, float afMaxSpeed);
	void FadeLeanRollTo(float afX, float afSpeedMul, float afMaxSpeed);
	void SetRoll(float afX);

	bool GetNoFallDamage(){ return mbNoFallDamage;}
	void SetNoFallDamage(bool abX){ mbNoFallDamage = abX;}

	bool GetShowCrosshair() { return mbShowCrossHair; }
	void SetShowCrosshair(bool abX) { mbShowCrossHair = abX; }

	eLuxFocusIconStyle GetFocusIconStyle() { return mFocusIconStyle; }
	void SetFocusIconStyle(eLuxFocusIconStyle aX) { mFocusIconStyle = aX; }

	static eLuxFocusIconStyle StringToFocusIconStyle(const tString& asX);
	static tString FocusIconStyleToString(eLuxFocusIconStyle aX);
	
	////////////////////
	// Free cam
	void SetFreeCamActive(bool abX);
	void SetFreeCamSpeed(float afSpeed);
	
	////////////////////
	// Helpers
	cLuxPlayerLightLevel *GetHelperLightLevel(){ return mpHelperLightLevel;}
	cLuxPlayerInDarkness *GetHelperInDarkness(){ return mpHelperInDarkness;}
	cLuxPlayerHudEffect *GetHelperHudEffect(){ return mpHudEffect;}
	cLuxPlayerLantern *GetHelperLantern(){ return mpLantern;}
	cLuxPlayerSanity *GetHelperSanity(){ return mpSanity; }
	cLuxPlayerLookAt *GetHelperLookAt(){ return mpLookAt; }
	cLuxPlayerDeath *GetHelperDeath(){ return mpDeath; }
	cLuxPlayerFlashback *GetHelperFlashback(){ return mpFlashback; }
	cLuxPlayerSpawnPS* GetHelperSpawnPS(){ return mpSpawnPS;}
	cLuxPlayerCamDirEffects* GetCamDirEffects(){ return mpCamDirEffects;}
	cLuxPlayerInsanityCollapse* GetInsanityCollapse(){ return mpInsanityCollapse;}

	cLuxPlayerHands* GetHands(){ return mpHands;}

	void RunHelperMessage(eUpdateableMessage aMessage, float afX);
	void RunHelperLuxMessage(eLuxUpdateableMessage aMessage, void *apData);

	
private:
	bool CanDrawCrossHair();
	void DrawHud(float afFrameTime);

	void UpdateHeadPosAdd(float afTimeStep);
	void UpdateCamera(float afTimeStep);
	void UpdateTerror(float afTimeStep);
	void UpdateLean(float afTimeStep);
	void UpdateFocusText(float afTimeStep);
	void UpdateAvgSpeed(float afTimeStep);
	
	void SpinHead(float afSpeed);
	void UpdateHeadSpin(float afTimeStep);

	void CreateCharacterBody(iPhysicsWorld *apPhysicsWorld);

	cCamera *mpCamera;
	iCharacterBody *mpCharBody;

	///////////////////////////////
	// Variables
	bool mbActive;

	bool mbUsePermaDeath;
	tString msCurrentPermaDeathSound;

	bool mbNoFallDamage;

	eLuxPlayerState mState;
	eLuxMoveState mMoveState;

	float mfHealth;
	float mfSanity;
	float mfLampOil;
	float mfTerror;
	int mlCoins;
	int mlTinderboxes;

	bool mbPressedMove;
	bool mbPressingRun;
	bool mbPressingJump;

	bool mbIsInWater;
	tString msWaterStepSound;
	float mfWaterSpeedMul;

	float mfInteractionMoveSpeedMul;

	float mfLookSpeedMul;

	bool mbJumpDisabled;
	bool mbCrouchDisabled;
	bool mbSanityDrainDisabled;

	float mfEventMoveSpeedMul;
	float mfEventRunSpeedMul;

	float mfScriptMoveSpeedMul;
	float mfScriptRunSpeedMul;

	float mfHurtMoveSpeedMul;

	float mfInsanityCollapseSpeedMul;

	float mfScriptJumpForceMul;

	cVector2f mvHeadSpinSpeed;

	cVector3f mvHeadPosAddSum;
	
	float mfCurrentFocusDistance;
	iLuxEntity *mpEntityInFocus;
	iPhysicsBody* mpBodyInFocus;

	tLuxEnemySet m_setTerrorEnemies;

	cSoundEntry *mpTerrorSound;
	int mlTerrorSoundID;

	float mfAspectMul;
	float mfFOVMul;
	float mfAspectMulGoal;
	float mfFOVMulGoal;
	float mfAspectMulSpeed;
	float mfFOVMulSpeed;
	
	float mfRoll;
	float mfRollGoal;
	float mfRollSpeedMul;
	float mfRollMaxSpeed;

	float mfLeanRoll;
	float mfLeanRollGoal;
	float mfLeanRollSpeedMul;
	float mfLeanRollMaxSpeed;

	cVector3f mvCamAnimPos;
	cVector3f mvCamAnimPosGoal;
	float mfCamAnimPosSpeedMul;
	float mfCamAnimPosMaxSpeed;
	
	tWString msFocusText;
	tWString msLastFocusText;
	float mfFocusTextAlpha;

	bool mbShowCrossHair;
	bool mbScriptShowFocusIconAndCrossHair;

	eLuxFocusIconStyle mFocusIconStyle;

	std::list<float> mlstPrevSpeeds;
	int mlMaxPrevSpeeds;
	float mfAvgSpeed;

	///////////////////////////////
	// Data
	cVector3f mvBodySize;
	cVector3f mvBodyCrouchSize;
	cVector3f mvCameraPosAdd;

	float mfHeadSpinDamageSpeed;
	float mfHeadSpinDeacc;

	float mfDefaultMass;

	float mfAspect;
	float mfFOV;

	float mfTerrorIncSpeed;
	float mfTerrorDecSpeed;

	float mfAutoKillYPos;

	cVector2f mvHeadSpeed;

	tString msTerrorSound;

	iFontData *mpFocusFont;

	std::vector<cLuxHeadPosAdd> mvHeadPosAdds;

	cLuxPlayerLightLevel *mpHelperLightLevel;
	cLuxPlayerInDarkness *mpHelperInDarkness;
	cLuxPlayerLantern *mpLantern;
	cLuxPlayerHudEffect *mpHudEffect;
	cLuxPlayerLean *mpLean;
	cLuxPlayerDeath *mpDeath;
	cLuxPlayerSanity *mpSanity;
	cLuxPlayerLookAt *mpLookAt;
	cLuxPlayerFlashback *mpFlashback;
	cLuxPlayerHurt *mpHurt;
	cLuxPlayerSpawnPS *mpSpawnPS;
	cLuxPlayerCamDirEffects *mpCamDirEffects;
	cLuxPlayerInsanityCollapse *mpInsanityCollapse;
	

	cLuxPlayerHands *mpHands;

	std::vector<iLuxPlayerHelper*> mvHelpers;
	std::vector<iLuxMoveState*> mvMoveStates;
	std::vector<iLuxPlayerState*> mvStates;


	//////////////////////
	// Free camera
	bool mbFreeCameraActive;
	float mfFreeCameraSpeed;
};

//----------------------------------------------


#endif // LUX_PLAYER_H
