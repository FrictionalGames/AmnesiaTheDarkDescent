/*
 * Copyright Â© 2009-2020 Frictional Games
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

#ifndef LUX_TYPES_H
#define LUX_TYPES_H

//----------------------------------------------

#include "StdAfx.h"

//----------------------------------------------

using namespace hpl;

//----------------------------------------------

//----------------------------------------------

#define kTranslate(sCategory, sEntry) gpBase->mpEngine->GetResources()->Translate(sCategory, sEntry)

#define kCopyToVar(aVar, aVal)	(aVar->aVal = aVal)
#define kCopyFromVar(aVar, aVal)(aVal = aVar->aVal)

//----------------------------------------------

enum eLuxAxis
{
	eLuxAxis_X,
	eLuxAxis_Y,
	eLuxAxis_Z,
	eLuxAxis_LastEnum,
};

//----------------------------------------------

enum eLuxAchievement
{
	eLuxAchievement_Insanity = 0, // Platinum ¡DO NOT USE THIS!

	// tdd
	eLuxAchievement_Alchemist,
	eLuxAchievement_EscapeArtist,
	eLuxAchievement_Descendant,
	eLuxAchievement_Pipeworker,
	eLuxAchievement_Restorer,
	eLuxAchievement_Survivor,
	eLuxAchievement_Sacrifice,
	eLuxAchievement_Benefactor,
	eLuxAchievement_Illuminatus,
	eLuxAchievement_NOPE,
	eLuxAchievement_Quitter,

	///////////////////////
	// HARDMODE
	eLuxAchievement_Masochist,


	// justine
	eLuxAchievement_Egotist,
	eLuxAchievement_Altruist,
	eLuxAchievement_Vacillator,
	eLuxAchievement_StillAlive,

	//general
	eLuxAchievement_MasterArchivist,

	eLuxAchievement_LastEnum,
	
	// pig
	eLuxAchievement_TheTeeth,
	eLuxAchievement_TheThroat,
	eLuxAchievement_TheGut,
	eLuxAchievement_TheEntrails,
	eLuxAchievement_TheHeart,
	eLuxAchievement_Insomniac
};

//----------------------------------------------

enum eLuxInputState
{
	eLuxInputState_Null,
	eLuxInputState_Game,
	eLuxInputState_MainMenu,
	eLuxInputState_Inventory,
	eLuxInputState_Journal,
	eLuxInputState_Debug,
	eLuxInputState_PreMenu,
	eLuxInputState_Credits,
	eLuxInputState_DemoEnd,
	eLuxInputState_LoadScreen,

	eLuxInputState_LastEnum
};

//----------------------------------------------

enum eLuxEntityType
{
	eLuxEntityType_Prop,
	eLuxEntityType_Area,
	eLuxEntityType_Enemy,
	eLuxEntityType_Rope,
	eLuxEntityType_CommentaryIcon,
	
	eLuxEntityType_LastEnum
};

enum eLuxPropType
{
	eLuxPropType_Object,
	eLuxPropType_SwingDoor,
	eLuxPropType_Lever,
	eLuxPropType_Wheel,
	eLuxPropType_Lamp,
	eLuxPropType_Photocell,
	eLuxPropType_MoveObject,
	eLuxPropType_Item,
	eLuxPropType_Chest,
	eLuxPropType_Critter,
	eLuxPropType_LevelDoor,
	eLuxPropType_Button,
	eLuxPropType_OilBarrel,
	eLuxPropType_EmotionStone,
	eLuxPropType_NPC,
	eLuxPropType_MultiSlider,
		
	eLuxPropType_LastEnum
};

enum eLuxAreaType
{
	eLuxAreaType_Script,
	eLuxAreaType_Flashback,
	eLuxAreaType_Ladder,
	eLuxAreaType_Liquid,
	eLuxAreaType_Sticky,
	eLuxAreaType_Insanity,
	eLuxAreaType_Examine,
	eLuxAreaType_Sign,
	eLuxAreaType_SlimeDamage,

	eLuxAreaType_LastEnum
};

enum eLuxEnemyType
{
	eLuxEnemyType_Grunt,
	eLuxEnemyType_WaterLurker,
	eLuxEnemyType_ManPig,

	eLuxEnemyType_LastEnum
};

//----------------------------------------------

enum eLuxObjectType
{
	eLuxObjectType_Static,
	eLuxObjectType_Grab,
	eLuxObjectType_Push,
	eLuxObjectType_Slide,
	
	eLuxObjectType_LastEnum
};

//----------------------------------------------

enum eLuxMoveObjectType
{
	eLuxMoveObjectType_Linear,
	eLuxMoveObjectType_Angular,

	eLuxMoveObjectType_LastEnum
};

//----------------------------------------------

enum eLuxItemType
{
	eLuxItemType_Puzzle,
	eLuxItemType_Coins,
	eLuxItemType_Note,
	eLuxItemType_Diary,
	eLuxItemType_Lantern,
	eLuxItemType_Health,
	eLuxItemType_Sanity,
	eLuxItemType_LampOil,
	eLuxItemType_Tinderbox,
	eLuxItemType_HandObject,
	
	eLuxItemType_LastEnum
};

//----------------------------------------------

enum eLuxEnemyMessage
{
	eLuxEnemyMessage_TimeOut,
	eLuxEnemyMessage_TimeOut_2,
	eLuxEnemyMessage_TimeOut_3,
	eLuxEnemyMessage_TimeOut_4,

	eLuxEnemyMessage_AnimationOver,
	eLuxEnemyMessage_AnimationSpecialEvent,
	eLuxEnemyMessage_EndOfPath,

	eLuxEnemyMessage_FoodInRange,

	eLuxEnemyMessage_PlayerSeen,
	eLuxEnemyMessage_PlayerUnseen,
	eLuxEnemyMessage_PlayerDetected,
	eLuxEnemyMessage_PlayerUndetected,
	eLuxEnemyMessage_SoundHeard,
	eLuxEnemyMessage_PlayerInRange,
	eLuxEnemyMessage_PlayerOutOfRange,
	eLuxEnemyMessage_PlayerDead,

	eLuxEnemyMessage_ChangePose,

	eLuxEnemyMessage_HelpMe,
	eLuxEnemyMessage_StuckAtDoor,

	eLuxEnemyMessage_TakeHit,
	eLuxEnemyMessage_Death,

	eLuxEnemyMessage_Reset,

	eLuxEnemyMessage_LastEnum
};

//----------------------------------------------

enum eLuxEnemyMusic
{
	eLuxEnemyMusic_Search,
	eLuxEnemyMusic_Attack,
	eLuxEnemyMusic_LastEnum
};

//----------------------------------------------


enum eLuxHandObjectType
{
	eLuxHandObjectType_Melee,
	eLuxHandObjectType_Ranged,

	eLuxHandObjectType_LightSource,

    eLuxHandObjectType_LastEnum,
};

//----------------------------------------------

enum eLuxWeaponHitType
{
	eLuxWeaponHitType_Sword,
	eLuxWeaponHitType_Club,
	eLuxWeaponHitType_Bullet,
	eLuxWeaponHitType_Dud,

	eLuxWeaponHitType_LastEnum,
};

//----------------------------------------------

enum eLuxMoveState
{
	eLuxMoveState_Normal,
	eLuxMoveState_ClimbLedge,

	eLuxMoveState_LastEnum
};

//----------------------------------------------

enum eLuxPlayerState
{
	eLuxPlayerState_Normal,
	eLuxPlayerState_HandObject,
	eLuxPlayerState_UseItem,
	eLuxPlayerState_InteractGrab,
	eLuxPlayerState_InteractPush,
	eLuxPlayerState_InteractSwingDoor,
	eLuxPlayerState_InteractLever,
	eLuxPlayerState_InteractWheel,
	eLuxPlayerState_InteractSlide,
	eLuxPlayerState_Ladder,

	eLuxPlayerState_LastEnum
};

//----------------------------------------------

enum eLuxUpdateableMessage
{
	eLuxUpdateableMessage_SaveMainConfig,
	eLuxUpdateableMessage_SaveUserConfig,

	eLuxUpdateableMessage_LoadMainConfig,
	eLuxUpdateableMessage_LoadUserConfig,

	eLuxUpdateableMessage_ClearFonts,
	eLuxUpdateableMessage_LoadFonts,

	eLuxUpdateableMessage_OnGameStart,

	eLuxUpdateableMessage_OnMapEnter,
	eLuxUpdateableMessage_OnMapLeave,

	eLuxUpdateableMessage_DestroyWorldEntities,
	eLuxUpdateableMessage_CreateWorldEntities

};

//----------------------------------------------

enum eLuxHeadPosAdd
{
	eLuxHeadPosAdd_Main,
	eLuxHeadPosAdd_Bob,
	eLuxHeadPosAdd_ClimbLedge,
	eLuxHeadPosAdd_Lean,
	eLuxHeadPosAdd_Death,
	eLuxHeadPosAdd_ScreenShake,
	eLuxHeadPosAdd_Script,
	eLuxHeadPosAdd_Hurt,
	eLuxHeadPosAdd_InsanityCollapse,

	eLuxHeadPosAdd_LastEnum
};

//----------------------------------------------

enum eLuxActionCategory
{
	eLuxActionCategory_System,
	eLuxActionCategory_Movement,
	eLuxActionCategory_Action,
	eLuxActionCategory_Misc,

	eLuxActionCategory_LastEnum
};

//----------------------------------------------

enum eLuxAction
{
	eLuxAction_Exit,
	eLuxAction_ExitDirect,
	eLuxAction_ScreenShot,
	eLuxAction_PrintInfo,

	eLuxAction_LeftClick,
	eLuxAction_MiddleClick,
	eLuxAction_RightClick,
	eLuxAction_ScrollUp,
	eLuxAction_ScrollDown,
	eLuxAction_MouseButton6Click,
	eLuxAction_MouseButton7Click,
	eLuxAction_MouseButton8Click,
	eLuxAction_MouseButton9Click,

	eLuxAction_UIArrowUp,
	eLuxAction_UIArrowDown,
	eLuxAction_UIArrowLeft,
	eLuxAction_UIArrowRight,

	eLuxAction_UIPrimary,
	eLuxAction_UISecondary,
	eLuxAction_UIPrevPage,
	eLuxAction_UINextPage,
	eLuxAction_UIDelete,
	eLuxAction_UIClear,

	eLuxAction_OpenDebug,
	eLuxAction_ReloadMap,
	eLuxAction_QuickSave,
	eLuxAction_QuickLoad,
	eLuxAction_FastForward,

	eLuxAction_Inventory,
	eLuxAction_Journal,
	eLuxAction_QuestLog,
	eLuxAction_RecentText,
	eLuxAction_CrosshairToggle,

	eLuxAction_Forward,
	eLuxAction_Backward,
	eLuxAction_Right,
	eLuxAction_Left,

	eLuxAction_LeanRight,
	eLuxAction_LeanLeft,
	eLuxAction_Lean,

	eLuxAction_Attack,
	eLuxAction_Interact,
	eLuxAction_Ignite,
	eLuxAction_Rotate,
	eLuxAction_Holster,
	eLuxAction_Lantern,
	
	eLuxAction_Run,
	eLuxAction_Jump,
	eLuxAction_Crouch,

	eLuxAction_ZoomIn,
	eLuxAction_ZoomOut,
	

	eLuxAction_LastEnum
};

//----------------------------------------------

enum eLuxIgniteSource
{
	eLuxIgniteSource_Player,
	
	eLuxIgniteSource_LastEnum
};

//----------------------------------------------

enum eLuxPlayerAction
{
	eLuxPlayerAction_Interact,
	eLuxPlayerAction_Attack,
	eLuxPlayerAction_Ignite,
	eLuxPlayerAction_Holster,
	eLuxPlayerAction_Lantern,

	eLuxPlayerAction_LastEnum
};



//----------------------------------------------

enum eLuxFocusCrosshair
{
	eLuxFocusCrosshair_Default,
	eLuxFocusCrosshair_Grab,
	eLuxFocusCrosshair_Push,
	eLuxFocusCrosshair_Ignite,
	eLuxFocusCrosshair_Pick,
	eLuxFocusCrosshair_LevelDoor,
	eLuxFocusCrosshair_Ladder,

    eLuxFocusCrosshair_LastEnum
};

//----------------------------------------------

enum eLuxDamageType
{
	eLuxDamageType_BloodSplat,
	eLuxDamageType_Claws,
	eLuxDamageType_Slash,

	eLuxDamageType_LastEnum
};

//----------------------------------------------

enum eLuxGlobalVolumeType
{
	eLuxGlobalVolumeType_Script,
	eLuxGlobalVolumeType_Flashback,
	eLuxGlobalVolumeType_Death,
	eLuxGlobalVolumeType_GameMenu,
	eLuxGlobalVolumeType_InsanityCollapse,
	eLuxGlobalVolumeType_Commentary,
	eLuxGlobalVolumeType_DebugMenu,
	
	eLuxGlobalVolumeType_LastEnum
};

//----------------------------------------------

enum eLuxProgressLogLevel
{
	eLuxProgressLogLevel_Low,
	eLuxProgressLogLevel_Medium,
	eLuxProgressLogLevel_High,

	eLuxProgressLogLevel_LastEnum,
};

//----------------------------------------------

class iLuxEntity;

typedef std::multimap<tString,iLuxEntity*> tLuxEntityNameMap;
typedef tLuxEntityNameMap::iterator tLuxEntityNameMapIt;

typedef std::multimap<int,iLuxEntity*> tLuxEntityIDMap;
typedef tLuxEntityIDMap::iterator tLuxEntityIDMapIt;

typedef std::list<iLuxEntity*> tLuxEntityList;
typedef tLuxEntityList::iterator tLuxEntityListIt;

typedef cSTLIterator<iLuxEntity*, tLuxEntityList, tLuxEntityListIt> cLuxEntityIterator;

//----------------------------------------------

class iLuxEnemy;

typedef std::list<iLuxEnemy*> tLuxEnemyList;
typedef tLuxEnemyList::iterator tLuxEnemyListIt;

typedef std::set<iLuxEnemy*> tLuxEnemySet;
typedef tLuxEnemySet::iterator tLuxEnemySetIt;

typedef cSTLIterator<iLuxEnemy*, tLuxEnemyList, tLuxEnemyListIt> cLuxEnemyIterator;

//----------------------------------------------

extern bool LuxIsCorrectType(iLuxEntity *apEntity, eLuxEntityType aType, int alSubType);

//----------------------------------------------

class cLuxIdPair  : public iSerializable
{
	kSerializableClassInit(cLuxIdPair)
public:
	int mlParentId;
	int mlChildId;
};

//----------------------------------------------

extern cLuxIdPair LuxGetIdPairFromBody(iPhysicsBody *apBody);

//----------------------------------------------

class cLuxMap;

class iLuxUpdateable : public iUpdateable
{
public:
	iLuxUpdateable(const tString& asName) : iUpdateable(asName){}
	virtual ~iLuxUpdateable(){}

	virtual void LoadMainConfig(){}
	virtual void LoadUserConfig(){}

	virtual void SaveMainConfig(){}
	virtual void SaveUserConfig(){}

	virtual void LoadFonts(){}
	void ClearFonts();
	virtual void OnClearFonts() {}
	iFontData* LoadFont(const tString& asFile);

	virtual void OnGameStart(){}

	virtual void OnMapEnter(cLuxMap *apMap){}
	virtual void OnMapLeave(cLuxMap *apMap){}

	virtual void DestroyWorldEntities(cLuxMap *apMap){}
	virtual void CreateWorldEntities(cLuxMap *apMap){}

	void LuxRunMessage(eLuxUpdateableMessage aMessage, void * apData)
	{
		switch(aMessage)
		{
		case eLuxUpdateableMessage_OnMapEnter:
			OnMapEnter((cLuxMap*) apData); break;
		case eLuxUpdateableMessage_OnMapLeave:
			OnMapLeave((cLuxMap*) apData); break;
		
		case eLuxUpdateableMessage_SaveMainConfig:
			SaveMainConfig(); break;
		case eLuxUpdateableMessage_SaveUserConfig:
			SaveUserConfig(); break;

		case eLuxUpdateableMessage_DestroyWorldEntities:
			DestroyWorldEntities((cLuxMap*) apData); break;
		case eLuxUpdateableMessage_CreateWorldEntities:
			CreateWorldEntities((cLuxMap*) apData); break;

		case eLuxUpdateableMessage_OnGameStart:
			OnGameStart(); break;

		case eLuxUpdateableMessage_LoadMainConfig:
			LoadMainConfig(); break;
		case eLuxUpdateableMessage_LoadUserConfig:
			LoadUserConfig(); break;

		case eLuxUpdateableMessage_ClearFonts:
			ClearFonts(); break;
		case eLuxUpdateableMessage_LoadFonts:
			LoadFonts(); break;
		};
	}

	std::vector<iFontData*> mvFonts;
};

//----------------------------------------------

class iLuxMessageCallback
{
public:
	virtual void OnPress(bool abYes)=0;
};

//----------------------------------------------

class cLuxPlayer;

class iLuxPlayerHelper : public iLuxUpdateable
{
public:
	iLuxPlayerHelper(cLuxPlayer *apPlayer, const tString& asName) : mpPlayer(apPlayer) ,iLuxUpdateable(asName){}
	virtual ~iLuxPlayerHelper(){}

	virtual void RenderSolid(cRendererCallbackFunctions* apFunctions){}
	virtual void RenderTrans(cRendererCallbackFunctions* apFunctions){}

protected:
    cLuxPlayer *mpPlayer;
};

//----------------------------------------------

class iLuxEntity;
class cLuxMap;

class cLuxCollideCallback
{
public:
	iLuxEntity* mpCollideEntity;
	tString msCallbackFunc;
	bool mbDeleteWhenColliding;
	int mlStates;

	bool mbColliding;
};

typedef std::list<cLuxCollideCallback*> tLuxCollideCallbackList;
typedef tLuxCollideCallbackList::iterator tLuxCollideCallbackListIt;

class iLuxCollideCallbackContainer
{
public:
	iLuxCollideCallbackContainer();
	
	void DestroyCollideCallbacks();

	virtual int GetBodyNum()=0;
	virtual iPhysicsBody* GetBody(int alIdx)=0;
	
	void CheckCollisionCallback(const tString& asName, cLuxMap *apMap);
	bool CheckEntityCollision(iLuxEntity*apEntity, cLuxMap *apMap);

	bool HasCollideCallbacks(){ return mlstCollideCallbacks.empty() == false;}
	tLuxCollideCallbackList* GetCollideCallbackList(){ return &mlstCollideCallbacks;}
	void AddCollideCallback(iLuxEntity *apEntity, const tString& asCallbackFunc, bool abRemoveAtCollide, int alStates);
	void RemoveCollideCallback(cLuxCollideCallback *apCallback);
	void RemoveCollideCallback(const tString& asEntityName);
	void RemoveCollideCallbackInstantly(iLuxEntity *apEntity);
protected:

	tLuxCollideCallbackList mlstCollideCallbacks;
	tLuxCollideCallbackList mlstDeleteCallbacks;
	bool mbUpdatingCollideCallbacks;
};

typedef std::list<iLuxCollideCallbackContainer*> tLuxCollideCallbackContainerList;
typedef tLuxCollideCallbackContainerList::iterator tLuxCollideCallbackContainerListIt;

//----------------------------------------------

class cLuxAlphaFader
{
public:
	cLuxAlphaFader() : mfAlpha(0), mbActive(false) {}	

	void Reset();
	void Update(float afTimeStep);

	float mfAlpha;
	bool mbActive;
	float mfFadeInSpeed;
	float mfFadeOutSpeed;
};

//----------------------------------------------

class cLuxHeadPosAdd : public iSerializable
{
	kSerializableClassInit(cLuxHeadPosAdd)
public:
	cLuxHeadPosAdd() : mvAdd(0), mbMoving(false) {}

	cVector3f mvAdd;

	bool mbMoving;
	cVector3f mvMoveGoal;
	float mfMoveSpeed;
	float mfMoveSlowDownDist;
};

//----------------------------------------------

class cLuxEventTimer : public iSerializable
{
	kSerializableClassInit(cLuxEventTimer)
public:
	tString msName;
	tString msFunction;
	float mfCount;
	bool mbDestroyMe;
};

typedef std::list<cLuxEventTimer*> tLuxEventTimerList;
typedef tLuxEventTimerList::iterator tLuxEventTimerListIt;

//----------------------------------------

class cLuxCombineItemsCallback : public iSerializable
{
	kSerializableClassInit(cLuxCombineItemsCallback)
public:
	tString msName;
	tString msItemA;
	tString msItemB;
	tString msFunction;
	bool mbAutoDestroy;
};

typedef std::list<cLuxCombineItemsCallback*> tLuxCombineItemsCallbackList;
typedef tLuxCombineItemsCallbackList::iterator tLuxCombineItemsCallbackListIt;

//----------------------------------------------

class cLuxUseItemCallback : public iSerializable
{
	kSerializableClassInit(cLuxUseItemCallback)
public:
	tString msName;
	tString msItem;
	tString msEntity;
	tString msFunction;
	bool mbAutoDestroy;
};

typedef std::list<cLuxUseItemCallback*> tLuxUseItemCallbackList;
typedef tLuxUseItemCallbackList::iterator tLuxUseItemCallbackListIt;

//----------------------------------------------

class cLuxNote : public iSerializable
{
	kSerializableClassInit(cLuxNote)
public:
	tString msNameEntry;
	tString msTextEntry;

	tString msIconFile;
	tString msImageFile;
};


//----------------------------------------

class cLuxDiary : public iSerializable
{
	kSerializableClassInit(cLuxDiary)
public:
	tString msNameEntry;
	tString msTextEntry;

	tString msIconFile;
	tString msImageFile;
};

//----------------------------------------

class cLuxQuestNote : public iSerializable
{
	kSerializableClassInit(cLuxQuestNote)
public:
	bool mbActive;
	tString msName;
	tString msNameEntry;
	tString msTextEntry;
};

//----------------------------------------

class cLuxScriptVar : public iSerializable
{
	kSerializableClassInit(cLuxScriptVar)
public:
	cLuxScriptVar(){}
	cLuxScriptVar(const tString &asName) : msName(asName), msVal(""){}

	tString msName;
	tString msVal;
};

typedef std::map<tString, cLuxScriptVar> tLuxScriptVarMap;
typedef tLuxScriptVarMap::iterator tLuxScriptVarMapIt;

//----------------------------------------

class cLuxVoiceData : public iSerializable
{
	kSerializableClassInit(cLuxVoiceData)
public:
	tWString msText;
	tString msVoiceFile;
	tString msEffectFile;
	bool mbUsePosition;
	cVector3f mvPosition;
	float mfMinDistance;
	float mfMaxDistance;
};

//----------------------------------------------

class cLuxCollideCallback_SaveData : public iSerializable
{
	kSerializableClassInit(cLuxCollideCallback_SaveData)
public:
	void FromCallback(cLuxCollideCallback *apCallback);
	void ToCallback(cLuxMap *apMap, iLuxCollideCallbackContainer* apCallbackContainer, cLuxCollideCallback *apCallback);

	int mlCollideEntity;
	tString msCallbackFunc;
	bool mbDeleteWhenColliding;
	int mlStates;
	bool mbColliding;
};


//----------------------------------------------

class iLuxProp;

class cLuxProp_AttachedProp  : public iSerializable
{
	kSerializableClassInit(cLuxProp_AttachedProp)
public:
	cLuxProp_AttachedProp() : mpProp(NULL) {}

	tString msFileName;
	tString msName;
	cMatrixf m_mtxOffset;
	iLuxProp *mpProp;
};

typedef std::list<cLuxProp_AttachedProp*> tLuxProp_AttachedPropList;
typedef tLuxProp_AttachedPropList::iterator tLuxProp_AttachedPropListIt;

//----------------------------------------

class cLuxNode_Pos
{
	friend class cLuxAreaNodeLoader_PosNode;
public:
	cLuxNode_Pos(const tString& asName) : msName(asName){}

	const tString& GetName(){ return msName;}

	const cVector3f& GetPosition(){ return mvPos;}

private:
	tString msName;
	cVector3f mvPos;
};

//----------------------------------------

#endif // LUX_TYPES_H
