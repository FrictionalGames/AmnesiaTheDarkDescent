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

#include "LuxSavedGameTypes.h"

#include "LuxMap.h"
#include "LuxPlayer.h"
#include "LuxPlayerHelpers.h"
#include "LuxPlayerHands.h"
#include "LuxHandObject.h"
#include "LuxPlayerState.h"
#include "LuxInventory.h"
#include "LuxMapHandler.h"
#include "LuxMusicHandler.h"
#include "LuxEnemy.h"
#include "LuxJournal.h"
#include "LuxEffectHandler.h"
#include "LuxGlobalDataHandler.h"
#include "LuxHintHandler.h"
#include "LuxInsanityHandler.h"
#include "LuxLoadScreenHandler.h"
#include "LuxMoveState_Normal.h"


//////////////////////////////////////////////////////////////////////////
// LOAD SCREEN HANDLER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler_SaveData::FromLoadScreenHandler(cLuxLoadScreenHandler *apData)
{
	msLoadTextCat = apData->GetLoadTextCat();
	msLoadTextEntry = apData->GetLoadTextEntry();
	msLoadTextImage = apData->GetLoadTextImage ();
	mlTextRandomNum = apData->GetLoadTextEntryRandomNum();
	mlPrevTextNumBitFlags = apData->mlPrevTextNumBitFlags;
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler_SaveData::ToLoadScreenHandler(cLuxMap *apMap, cLuxLoadScreenHandler *apData)
{
	apData->SetupLoadText(msLoadTextCat, msLoadTextEntry, mlTextRandomNum, msLoadTextImage);
	apData->mlPrevTextNumBitFlags = mlPrevTextNumBitFlags;
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxLoadScreenHandler_SaveData)

kSerializeVar(msLoadTextCat, eSerializeType_String)
kSerializeVar(msLoadTextEntry, eSerializeType_String)
kSerializeVar(msLoadTextImage, eSerializeType_String)
kSerializeVar(mlTextRandomNum, eSerializeType_Int32)
kSerializeVar(mlPrevTextNumBitFlags, eSerializeType_Int32)

kEndSerialize()

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// HINT HANDLER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxInsanityHandler_SaveData::FromInsanityHandler(cLuxInsanityHandler *apData)
{
	mfNewEventCount = apData->mfNewEventCount;

	//Events
    mvEvents.Resize(apData->mvEvents.size());
	for(size_t i=0; i<mvEvents.Size(); ++i)
	{
		mvEvents[i].mbUsed = apData->mvEvents[i]->IsUsed();
	}

	//Disabled Events
	for(tStringSetIt it = apData->m_setDisabledSets.begin(); it != apData->m_setDisabledSets.end(); ++it)
	{
		mvDisabledSets.Add(cLuxInsanityHandler_Set_SaveData(*it));
	}
}

void cLuxInsanityHandler_SaveData::ToInsanityHandler(cLuxMap *apMap, cLuxInsanityHandler *apData)
{
	apData->mfNewEventCount = mfNewEventCount;

	//Events
	if(mvEvents.Size() == apData->mvEvents.size())
	{
		for(size_t i=0; i<mvEvents.Size(); ++i)
		{
			apData->mvEvents[i]->SetUsed(mvEvents[i].mbUsed);
		}
	}

	//Disabled Events
	apData->m_setDisabledSets.clear();
	for(size_t i=0; i<mvDisabledSets.Size(); ++i)
	{
		apData->m_setDisabledSets.insert(mvDisabledSets[i].msName);
	}
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxInsanityHandler_Event_SaveData)

kSerializeVar(mbUsed, eSerializeType_Bool)

kEndSerialize()

kBeginSerializeBase(cLuxInsanityHandler_Set_SaveData)

kSerializeVar(msName, eSerializeType_String)

kEndSerialize()

kBeginSerializeBase(cLuxInsanityHandler_SaveData)

kSerializeVar(mfNewEventCount, eSerializeType_Float32)
kSerializeClassContainer(mvEvents, cLuxInsanityHandler_Event_SaveData, eSerializeType_Class)
kSerializeClassContainer(mvDisabledSets, cLuxInsanityHandler_Set_SaveData, eSerializeType_Class)

kEndSerialize()

//////////////////////////////////////////////////////////////////////////
// HINT HANDLER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
void cLuxHintHandler_SaveData::FromHintHandler(cLuxHintHandler *apData)
{
	///////////////////////
	// Hints given
	tStringSetIt it = apData->m_setHintsGiven.begin();
	for(; it != apData->m_setHintsGiven.end(); ++it)
	{
		cLuxHintHandler_Hint_SaveData hint;

		hint.msName = *it;

		mlstHintsGiven.Add(hint);
	}

	///////////////////////
	// Hints blocked
	it = apData->m_setHintsBlocked.begin();
	for(; it != apData->m_setHintsBlocked.end(); ++it)
	{
		cLuxHintHandler_Hint_SaveData hint;

		hint.msName = *it;

		mlstHintsBlocked.Add(hint);
	}
}

//-----------------------------------------------------------------------

void cLuxHintHandler_SaveData::ToHintHandler(cLuxMap *apMap, cLuxHintHandler *apData)
{
	///////////////////////
	// Hints given
	apData->m_setHintsGiven.clear();
	cContainerListIterator<cLuxHintHandler_Hint_SaveData> it = mlstHintsGiven.GetIterator();
	while(it.HasNext())
	{
		cLuxHintHandler_Hint_SaveData &hint = it.Next();
		apData->m_setHintsGiven.insert(hint.msName);
	}

	///////////////////////
	// Hints blocked
	apData->m_setHintsBlocked.clear();
	it = mlstHintsBlocked.GetIterator();
	while(it.HasNext())
	{
		cLuxHintHandler_Hint_SaveData &hint = it.Next();
		apData->m_setHintsBlocked.insert(hint.msName);
	}
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxHintHandler_Hint_SaveData)

kSerializeVar(msName, eSerializeType_String)

kEndSerialize()

kBeginSerializeBase(cLuxHintHandler_SaveData)

kSerializeClassContainer(mlstHintsGiven, cLuxHintHandler_Hint_SaveData, eSerializeType_Class)
kSerializeClassContainer(mlstHintsBlocked, cLuxHintHandler_Hint_SaveData, eSerializeType_Class)

kEndSerialize()

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// GLOBAL DATA HANDLER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler_SaveData::FromGlobalDataHandler(cLuxGlobalDataHandler *apData)
{
	tLuxScriptVarMapIt it = apData->m_mapVars.begin();
	for(; it != apData->m_mapVars.end(); ++it)
	{
		mlstVars.Add(it->second);
	}
}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler_SaveData::ToGlobalDataHandler(cLuxMap *apMap, cLuxGlobalDataHandler *apData)
{
	apData->m_mapVars.clear();
	cContainerListIterator<cLuxScriptVar> it = mlstVars.GetIterator();
	while(it.HasNext())
	{
		cLuxScriptVar& scriptVar = it.Next();
		apData->m_mapVars.insert(tLuxScriptVarMap::value_type(scriptVar.msName, scriptVar));
	}
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxGlobalDataHandler_SaveData)

kSerializeClassContainer(mlstVars, cLuxScriptVar,  eSerializeType_Class)

kEndSerialize()

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// EFFECT HANDLER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxEffectHandler_SoundMul_SaveData::FromEntry(cMultipleSettingsHandler::cGSEntry* apEntry)
{
	if(apEntry)
	{
		mbActive = true;
		mfVal = apEntry->GetVal();
		mfDest = apEntry->GetDest();
		mfSpeed = apEntry->GetSpeed();
		mTypes = apEntry->GetTypes();
		mbDestroyAtDest = apEntry->GetDestroyAtDest();
	}
	else
	{
		mbActive = false;
	}
}

void cLuxEffectHandler_SoundMul_SaveData::ToEntry(int alIdx, cMultipleSettingsHandler *apHandler)
{
	if(mbActive)
	{
		cMultipleSettingsHandler::cGSEntry* pEntry = apHandler->GetEntry(alIdx);
		pEntry->SetVal(mfVal);
		pEntry->SetDest(mfDest);
		pEntry->SetSpeed(mfSpeed);
		pEntry->SetTypes(mTypes);
		pEntry->SetDestroyAtDest(mbDestroyAtDest);
	}
	else
	{
		apHandler->DestroyEntry(alIdx);
	}
}

//-----------------------------------------------------------------------

void cLuxEffectHandler_SaveData::FromEffectHandler(cLuxEffectHandler *apEffects)
{
	//////////////////////
	// Fade In / Out
	cLuxEffect_Fade *pFade = apEffects->GetFade();

	mbFade_Active = pFade->IsActive();
	mfFade_GoalAlpha = pFade->mfGoalAlpha;
	mfFade_Alpha = pFade->mfAlpha;
	mfFade_FadeSpeed = pFade->mfFadeSpeed;

	//////////////////////
	// ImageTrail
	cLuxEffect_ImageTrail *pImageTrail = apEffects->GetImageTrail();

	mbImageTrail_Active = pImageTrail->mbActive;
	mfImageTrail_Amount = pImageTrail->mfAmount;
	mfImageTrail_AmountGoal = pImageTrail->mfAmountGoal;
	mfImageTrail_FadeSpeed = pImageTrail->mfFadeSpeed;

	//////////////////////
	// Sepia COlor
	cLuxEffect_SepiaColor *pSepiaColor = apEffects->GetSepiaColor();

	mbSepiaColor_Active = pSepiaColor->mbActive;
	mfSepiaColor_Amount = pSepiaColor->mfAmount;
	mfSepiaColor_AmountGoal = pSepiaColor->mfAmountGoal;
	mfSepiaColor_FadeSpeed = pSepiaColor->mfFadeSpeed;

	//////////////////////
	// Radial blur
	cLuxEffect_RadialBlur *pRadialBlur = apEffects->GetRadialBlur();

	mbRadialBlur_Active = pRadialBlur->mbActive;
	mfRadialBlur_Size = pRadialBlur->mfSize;
	mfRadialBlur_BlurStartDist = pRadialBlur->mfBlurStartDist;
	mfRadialBlur_SizeGoal = pRadialBlur->mfSizeGoal;
	mfRadialBlur_FadeSpeed = pRadialBlur->mfFadeSpeed;

	//////////////////////
	// Voice data
	mbVoiceActive = apEffects->GetPlayVoice()->mbActive;
	msVoiceOverCallback = apEffects->GetPlayVoice()->msOverCallback;
    mvVoiceData.Clear();
	tLuxVoiceDataListIt voiceIt = apEffects->GetPlayVoice()->mlstVoices.begin();
	for(; voiceIt != apEffects->GetPlayVoice()->mlstVoices.end(); ++voiceIt)
	{
		mvVoiceData.Add(*voiceIt);
	}
	
	//////////////////////
	// Sound volume and speed mul
	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
	mvGlobalSoundVolumeMul.Resize(eLuxGlobalVolumeType_LastEnum);
	mvGlobalSoundSpeedMul.Resize(eLuxGlobalVolumeType_LastEnum);
	for(size_t i=0; i<eLuxGlobalVolumeType_LastEnum; ++i)
	{
		if(i == eLuxGlobalVolumeType_Commentary || i == eLuxGlobalVolumeType_DebugMenu)continue;

		mvGlobalSoundVolumeMul[i].FromEntry(pSoundHandler->GetGlobalVolumeSettingsHandler()->GetEntry(i, false) );
		mvGlobalSoundSpeedMul[i].FromEntry(pSoundHandler->GetGlobalSpeedSettingsHandler()->GetEntry(i, false) );
	}
}

void cLuxEffectHandler_SaveData::ToEffectHandler(cLuxMap *apMap, cLuxEffectHandler *apEffects)
{
	//////////////////////
	// Fade In / Out
	cLuxEffect_Fade *pFade = apEffects->GetFade();

	pFade->mbActive = mbFade_Active;
	pFade->mfGoalAlpha = mfFade_GoalAlpha;
	pFade->mfAlpha = mfFade_Alpha;
	pFade->mfFadeSpeed = mfFade_FadeSpeed;

	//////////////////////
	// ImageTrail
	cLuxEffect_ImageTrail *pImageTrail = apEffects->GetImageTrail();

	gpBase->mpMapHandler->GetPostEffect_ImageTrail()->SetActive(mbImageTrail_Active);
	pImageTrail->mbActive = mbImageTrail_Active;
	pImageTrail->mfAmount = mfImageTrail_Amount;
	pImageTrail->mfAmountGoal = mfImageTrail_AmountGoal;
	pImageTrail->mfFadeSpeed = mfImageTrail_FadeSpeed;

	//////////////////////
	// Sepia COlor
	cLuxEffect_SepiaColor *pSepiaColor = apEffects->GetSepiaColor();

	gpBase->mpMapHandler->GetPostEffect_Sepia()->SetActive(mbSepiaColor_Active);
	pSepiaColor->mbActive = mbSepiaColor_Active;
	pSepiaColor->mfAmount = mfSepiaColor_Amount;
	pSepiaColor->mfAmountGoal = mfSepiaColor_AmountGoal;
	pSepiaColor->mfFadeSpeed = mfSepiaColor_FadeSpeed;

	//////////////////////
	// Radial blur
	cLuxEffect_RadialBlur *pRadialBlur = apEffects->GetRadialBlur();

	gpBase->mpMapHandler->GetPostEffect_RadialBlur()->SetActive(mbRadialBlur_Active);
	pRadialBlur->mbActive = mbRadialBlur_Active;
	pRadialBlur->mfSize = mfRadialBlur_Size;
	pRadialBlur->mfBlurStartDist = mfRadialBlur_BlurStartDist;
	pRadialBlur->mfSizeGoal = mfRadialBlur_SizeGoal;
	pRadialBlur->mfFadeSpeed = mfRadialBlur_FadeSpeed;

	//////////////////////
	// Voice data
	apEffects->GetPlayVoice()->mbActive = mbVoiceActive;
	apEffects->GetPlayVoice()->msOverCallback = msVoiceOverCallback;
	apEffects->GetPlayVoice()->mlstVoices.clear();
	for(size_t i=0; i<mvVoiceData.Size(); ++i)
	{
		apEffects->GetPlayVoice()->mlstVoices.push_back(mvVoiceData[i]);
	}
	
	//////////////////////
	// Sound volume and speed mul
	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();

	for(size_t i=0; i<mvGlobalSoundVolumeMul.Size(); ++i)
	{
		if(i == eLuxGlobalVolumeType_Commentary || i == eLuxGlobalVolumeType_DebugMenu) continue;

		mvGlobalSoundVolumeMul[i].ToEntry(i, pSoundHandler->GetGlobalVolumeSettingsHandler() );
	}
	
	for(size_t i=0; i<mvGlobalSoundSpeedMul.Size(); ++i)
	{
		if(i == eLuxGlobalVolumeType_Commentary || i == eLuxGlobalVolumeType_DebugMenu) continue;

		mvGlobalSoundSpeedMul[i].ToEntry(i, pSoundHandler->GetGlobalSpeedSettingsHandler() );
	}
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxEffectHandler_SoundMul_SaveData)
kSerializeVar(mbActive, eSerializeType_Bool)
kSerializeVar(mfVal, eSerializeType_Float32)
kSerializeVar(mfDest, eSerializeType_Float32)
kSerializeVar(mfSpeed, eSerializeType_Float32)
kSerializeVar(mTypes, eSerializeType_Int32)
kSerializeVar(mbDestroyAtDest, eSerializeType_Bool)
kEndSerialize()


kBeginSerializeBase(cLuxEffectHandler_SaveData)
kSerializeVar(mbFade_Active, eSerializeType_Bool)
kSerializeVar(mfFade_GoalAlpha, eSerializeType_Float32)
kSerializeVar(mfFade_Alpha, eSerializeType_Float32)
kSerializeVar(mfFade_FadeSpeed, eSerializeType_Float32)

kSerializeVar(mbImageTrail_Active, eSerializeType_Bool)
kSerializeVar(mfImageTrail_Amount, eSerializeType_Float32)
kSerializeVar(mfImageTrail_AmountGoal, eSerializeType_Float32)
kSerializeVar(mfImageTrail_FadeSpeed, eSerializeType_Float32)

kSerializeVar(mbSepiaColor_Active, eSerializeType_Bool)
kSerializeVar(mfSepiaColor_Amount, eSerializeType_Float32)
kSerializeVar(mfSepiaColor_AmountGoal, eSerializeType_Float32)
kSerializeVar(mfSepiaColor_FadeSpeed, eSerializeType_Float32)

kSerializeVar(mbRadialBlur_Active, eSerializeType_Bool)
kSerializeVar(mfRadialBlur_Size, eSerializeType_Float32)
kSerializeVar(mfRadialBlur_BlurStartDist, eSerializeType_Float32)
kSerializeVar(mfRadialBlur_SizeGoal, eSerializeType_Float32)
kSerializeVar(mfRadialBlur_FadeSpeed, eSerializeType_Float32)

kSerializeVar(msVoiceOverCallback, eSerializeType_String)
kSerializeVar(mbVoiceActive, eSerializeType_Bool)
kSerializeClassContainer(mvVoiceData, cLuxVoiceData, eSerializeType_Class)

kSerializeClassContainer(mvGlobalSoundVolumeMul, cLuxEffectHandler_SoundMul_SaveData, eSerializeType_Class)
kSerializeClassContainer(mvGlobalSoundSpeedMul, cLuxEffectHandler_SoundMul_SaveData, eSerializeType_Class)

kEndSerialize()
//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// MUSIC HANDLER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMusicHandler_SaveData::FromMusicHandler(cLuxMusicHandler *apMusic)
{
	///////////////////
	// Current Music
	cMusicEntry *pMusicEntry = apMusic->mpMusicHandler->GetCurrentSong();
	if(pMusicEntry)
	{
		msCurrentMusic = pMusicEntry->msFileName;
		mfCurrentMusicMaxVolume = pMusicEntry->mfMaxVolume;
		mfCurrentMusicVolume = pMusicEntry->mfVolume;
		mfCurrentMusicVolumeAdd = pMusicEntry->mfVolumeAdd;
		mfCurrentMusicTime = pMusicEntry->mpStream->GetElapsedTime();
		mbCurrentMusicLoop = pMusicEntry->mbLoop;
	}
	else
	{
		msCurrentMusic = "";
	}

	///////////////////
	// Vars
	mlCurrentMaxPrio = apMusic->mlCurrentMaxPrio;
	mbEnemyClosePlaying = apMusic->mbEnemyClosePlaying;
	mbAttackPlaying = apMusic->mbEnemyPlaying[eLuxEnemyMusic_Attack];
	mbSearchPlaying = apMusic->mbEnemyPlaying[eLuxEnemyMusic_Search];

	///////////////////
	// Music
	mvMusic.Resize(apMusic->mvGameMusic.size());
	for(size_t i=0; i<mvMusic.Size(); ++i)
	{
		mvMusic[i].msFile = apMusic->mvGameMusic[i].msFile;
		mvMusic[i].mbLoop = apMusic->mvGameMusic[i].mbLoop;
		mvMusic[i].mbResume = apMusic->mvGameMusic[i].mbResume;
		mvMusic[i].mfVolume = apMusic->mvGameMusic[i].mfVolume;
		mvMusic[i].mbSpecialEffect = apMusic->mvGameMusic[i].mbSpecialEffect;
	}

	///////////////////
	// Enemies

	//Attackers
	tLuxEnemySetIt it = apMusic->m_setEnemies[eLuxEnemyMusic_Attack].begin();
	for(; it != apMusic->m_setEnemies[eLuxEnemyMusic_Attack].end(); ++it)
	{
		iLuxEnemy *pEnemy = *it;
		mvAttackerIDs.Add(pEnemy->GetID());
	}

	//Searchers
	it = apMusic->m_setEnemies[eLuxEnemyMusic_Search].begin();
	for(; it != apMusic->m_setEnemies[eLuxEnemyMusic_Search].end(); ++it)
	{
		iLuxEnemy *pEnemy = *it;
		mvAttackerIDs.Add(pEnemy->GetID());
	}
}

//-----------------------------------------------------------------------

void cLuxMusicHandler_SaveData::ToMusicHandler(cLuxMap *apMap, cLuxMusicHandler *apMusic)
{
	///////////////////
	// Current Music
	if(msCurrentMusic != "")
	{
		bool bRet = apMusic->mpMusicHandler->Play(msCurrentMusic,mfCurrentMusicMaxVolume,0.1f, mbCurrentMusicLoop, false);
		if(bRet)
		{	
			cMusicEntry *pMusicEntry = apMusic->mpMusicHandler->GetCurrentSong();
			if(pMusicEntry)
			{
				pMusicEntry->mfMaxVolume = mfCurrentMusicMaxVolume;
				pMusicEntry->mfVolume = mfCurrentMusicVolume; 
				pMusicEntry->mfVolumeAdd = mfCurrentMusicVolumeAdd;
				pMusicEntry->mpStream->SetElapsedTime(mfCurrentMusicTime);
				pMusicEntry->mbLoop = mbCurrentMusicLoop;
			}
		}
	}

	///////////////////
	// Vars
	apMusic->mlCurrentMaxPrio = mlCurrentMaxPrio;
	apMusic->mbEnemyClosePlaying = mbEnemyClosePlaying;
	apMusic->mbEnemyPlaying[eLuxEnemyMusic_Attack] = mbAttackPlaying;
	apMusic->mbEnemyPlaying[eLuxEnemyMusic_Search] = mbSearchPlaying;

	
	///////////////////
	// Music
	for(size_t i=0; i<mvMusic.Size(); ++i)
	{
		apMusic->mvGameMusic[i].msFile = mvMusic[i].msFile;
		apMusic->mvGameMusic[i].mbLoop = mvMusic[i].mbLoop;
		apMusic->mvGameMusic[i].mbResume = mvMusic[i].mbResume;
		apMusic->mvGameMusic[i].mfVolume = mvMusic[i].mfVolume;
		apMusic->mvGameMusic[i].mbSpecialEffect = mvMusic[i].mbSpecialEffect;
	}

	///////////////////
	// Enemies

	//Attackers
	for(size_t i=0; i<mvAttackerIDs.Size(); ++i)
	{
		iLuxEntity *pEntity = apMap->GetEntityByID(mvAttackerIDs[i]);
        if(pEntity && pEntity->GetEntityType() == eLuxEntityType_Enemy)
		{
			iLuxEnemy *pEnemy = static_cast<iLuxEnemy*>(pEntity);
			apMusic->m_setEnemies[eLuxEnemyMusic_Attack].insert(pEnemy);
		}
		else
		{
			Warning("Could not find entity with id %d\n", mvAttackerIDs[i]);
		}
	}

	//Searchers
	for(size_t i=0; i<mvSearcherIDs.Size(); ++i)
	{
		iLuxEntity *pEntity = apMap->GetEntityByID(mvSearcherIDs[i]);
		if(pEntity && pEntity->GetEntityType() == eLuxEntityType_Enemy)
		{
			iLuxEnemy *pEnemy = static_cast<iLuxEnemy*>(pEntity);
			apMusic->m_setEnemies[eLuxEnemyMusic_Search].insert(pEnemy);
		}
		else
		{
			Warning("Could not find entity with id %d\n", mvAttackerIDs[i]);
		}
	}
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxMusicHandler_Music_SaveData)
kSerializeVar(msFile, eSerializeType_String)
kSerializeVar(mfVolume, eSerializeType_Float32)
kSerializeVar(mbLoop, eSerializeType_Bool)
kSerializeVar(mbResume, eSerializeType_Bool)
kSerializeVar(mbSpecialEffect, eSerializeType_Bool)
kEndSerialize()

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxMusicHandler_SaveData)
kSerializeVar(msCurrentMusic, eSerializeType_String)
kSerializeVar(mfCurrentMusicVolume, eSerializeType_Float32)
kSerializeVar(mfCurrentMusicVolumeAdd, eSerializeType_Float32)
kSerializeVar(mfCurrentMusicMaxVolume, eSerializeType_Float32)
kSerializeVar(mfCurrentMusicTime, eSerializeType_Float32)
kSerializeVar(mbCurrentMusicLoop, eSerializeType_Bool)

kSerializeVar(mlCurrentMaxPrio, eSerializeType_Int32)
kSerializeVar(mbEnemyClosePlaying, eSerializeType_Bool)
kSerializeVar(mbAttackPlaying, eSerializeType_Bool)
kSerializeVar(mbSearchPlaying, eSerializeType_Bool)

kSerializeClassContainer(mvMusic,  cLuxMusicHandler_Music_SaveData, eSerializeType_Class)
kSerializeVarContainer(mvAttackerIDs,eSerializeType_Int32)
kSerializeVarContainer(mvSearcherIDs,eSerializeType_Int32)
kEndSerialize()

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// JOURNAL DIARY CONTAINER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxJournal_DiaryCont_SaveData)
kSerializeVar(msType, eSerializeType_String)
kSerializeClassContainer(mvDiaries, cLuxDiary, eSerializeType_Class)
kEndSerialize()


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// JOURNAL
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxJournal_SaveData::cLuxJournal_SaveData()
{

}

//-----------------------------------------------------------------------

cLuxJournal_SaveData::~cLuxJournal_SaveData()
{

}

//-----------------------------------------------------------------------

void cLuxJournal_SaveData::FromJournal(cLuxJournal *apJournal)
{
	/////////////
	//Vars
	mlLastReadTextCat = apJournal->mlLastReadTextCat;
	mlLastReadTextEntry = apJournal->mlLastReadTextEntry;
	mlLastReadTextType = apJournal->mlLastReadTextType;

	/////////////
	//Notes
    mvNotes.Resize(apJournal->mvNotes.size());
	for(size_t i=0; i<mvNotes.Size(); ++i)
	{
		mvNotes[i] = *apJournal->mvNotes[i];
	}

	/////////////
	//Diaries
	mvDiaryConts.Resize(apJournal->mvDiaryContainers.size());
	for(size_t cont=0; cont<mvDiaryConts.Size(); ++cont)
	{
		mvDiaryConts[cont].msType = apJournal->mvDiaryContainers[cont]->msType;
		mvDiaryConts[cont].mvDiaries.Resize(apJournal->mvDiaryContainers[cont]->mvDiaries.size());
		for(size_t i=0; i<mvDiaryConts[cont].mvDiaries.Size(); ++i)
		{
			mvDiaryConts[cont].mvDiaries[i] = *apJournal->mvDiaryContainers[cont]->mvDiaries[i];
		}
	}

	/////////////
	//Quest notes
	mvQuestNotes.Resize(apJournal->mvQuestNotes.size());
	for(size_t i=0; i<mvQuestNotes.Size(); ++i)
	{
		mvQuestNotes[i] = *apJournal->mvQuestNotes[i];
	}
}

void cLuxJournal_SaveData::ToJournal(cLuxMap *apMap, cLuxJournal *apJournal)
{
	/////////////
	//Vars
	apJournal->mlLastReadTextCat = mlLastReadTextCat;
	apJournal->mlLastReadTextEntry = mlLastReadTextEntry;
	apJournal->mlLastReadTextType = mlLastReadTextType;

	/////////////
	//Notes
	apJournal->mvNotes.resize(mvNotes.Size());
	for(size_t i=0; i<mvNotes.Size(); ++i)
	{
		apJournal->mvNotes[i] = hplNew(cLuxNote, ());
		*apJournal->mvNotes[i] = mvNotes[i];
	}

	/////////////
	//Diaries
	apJournal->mvDiaryContainers.resize(mvDiaryConts.Size());
	for(size_t cont=0; cont<mvDiaryConts.Size(); ++cont)
	{
		apJournal->mvDiaryContainers[cont] = hplNew(cLuxDiaryContainer, ());
		apJournal->mvDiaryContainers[cont]->msType = mvDiaryConts[cont].msType;
		apJournal->mvDiaryContainers[cont]->mvDiaries.resize(mvDiaryConts[cont].mvDiaries.Size());
		for(size_t i=0; i<mvDiaryConts[cont].mvDiaries.Size(); ++i)
		{
			apJournal->mvDiaryContainers[cont]->mvDiaries[i] = hplNew(cLuxDiary, ());
			*apJournal->mvDiaryContainers[cont]->mvDiaries[i] = mvDiaryConts[cont].mvDiaries[i];
		}
	}

	/////////////
	//QuestNotes
	apJournal->mvQuestNotes.resize(mvQuestNotes.Size());
	for(size_t i=0; i<mvQuestNotes.Size(); ++i)
	{
		apJournal->mvQuestNotes[i] = hplNew(cLuxQuestNote, ());
		*apJournal->mvQuestNotes[i] = mvQuestNotes[i];
	}
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxJournal_SaveData)
kSerializeVar(mlLastReadTextCat, eSerializeType_Int32)
kSerializeVar(mlLastReadTextEntry, eSerializeType_Int32)
kSerializeVar(mlLastReadTextType, eSerializeType_Int32)
kSerializeClassContainer(mvNotes, cLuxNote, eSerializeType_Class)
kSerializeClassContainer(mvDiaryConts, cLuxJournal_DiaryCont_SaveData, eSerializeType_Class)
kSerializeClassContainer(mvQuestNotes, cLuxQuestNote, eSerializeType_Class)
kEndSerialize()

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// INVENTORY ITEM
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxInventory_Item_SaveData::FromItem(cLuxInventory_Item *apItem)
{
	msName = apItem->GetName();
	msSubType = apItem->GetSubType();
	mlType = apItem->GetType();
	msVal = apItem->GetStringVal();
	msExtraVal = apItem->GetExtraStringVal();

	msImageFile = apItem->GetImageName();

	mfAmount = apItem->GetAmount();
	mlCount = apItem->GetCount();
	msGameNameEntry = apItem->GetGameNameEntry();
	msGameDescEntry = apItem->GetGameDescEntry();
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxInventory_Item_SaveData)
kSerializeVar(msName, eSerializeType_String)
kSerializeVar(mlType, eSerializeType_Int32)
kSerializeVar(msSubType, eSerializeType_String)
kSerializeVar(msVal, eSerializeType_String)
kSerializeVar(msExtraVal, eSerializeType_String)

kSerializeVar(msImageFile, eSerializeType_String)

kSerializeVar(mfAmount, eSerializeType_Float32)
kSerializeVar(mlCount, eSerializeType_Int32)
kSerializeVar(msGameNameEntry, eSerializeType_String)
kSerializeVar(msGameDescEntry, eSerializeType_String)
kEndSerialize()

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// INVENTORY 
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInventory_SaveData::cLuxInventory_SaveData()
{

}

cLuxInventory_SaveData::~cLuxInventory_SaveData()
{

}

//-----------------------------------------------------------------------

void cLuxInventory_SaveData::FromInventory(cLuxInventory *apInventory)
{
	///////////////////////////
	//Variables
	mbDisabled = apInventory->mbDisabled;
	mlEquippedHandItem = apInventory->GetItemIndex(apInventory->GetEquippedHandItem());

	///////////////////////////
	//Items
	mvItems.Resize(apInventory->mvItems.size());
	for(size_t i=0; i<mvItems.Size(); ++i)
	{
		mvItems[i].FromItem(apInventory->mvItems[i]);
	}

	///////////////////////////
	//Combine callbacks
	{
		tLuxCombineItemsCallbackListIt it = apInventory->mlstCombineCallbacks.begin();
		for(; it != apInventory->mlstCombineCallbacks.end(); ++it)
		{
			cLuxCombineItemsCallback *pCallback = *it;
			mvCombineCallbacks.Add(*pCallback);	
		}
	}
}

//-----------------------------------------------------------------------

void cLuxInventory_SaveData::ToInventory(cLuxMap *apMap, cLuxInventory *apInventory)
{
	///////////////////////////
	//Variables
	apInventory->mbDisabled = mbDisabled;
	if(mlEquippedHandItem >0)
	{
		cLuxInventory_Item *pItem = apInventory->GetItem(mlEquippedHandItem);
		apInventory->SetEquippedHandItem(pItem);
	}
	else
	{
		apInventory->SetEquippedHandItem(NULL);
	}


	///////////////////////////
	//Items
	for(size_t i=0; i<mvItems.Size(); ++i)
	{
		cLuxInventory_Item_SaveData& saveItem = mvItems[i];

		cLuxInventory_Item *pItem = apInventory->AddItem(	saveItem.msName, (eLuxItemType)saveItem.mlType, saveItem.msSubType, 
															saveItem.msImageFile, saveItem.mfAmount, saveItem.msVal, saveItem.msExtraVal);
		pItem->SetGameNameEntry(saveItem.msGameNameEntry);
		pItem->SetGameDescEntry(saveItem.msGameDescEntry);
		pItem->SetCount(saveItem.mlCount);
	}

	///////////////////////////
	//Combine callbacks
	for(size_t i=0; i<mvCombineCallbacks.Size(); ++i)
	{
		cLuxCombineItemsCallback *pCallback = hplNew(cLuxCombineItemsCallback, ());
		*pCallback = mvCombineCallbacks[i];
		apInventory->mlstCombineCallbacks.push_back(pCallback);
	}
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxInventory_SaveData)
kSerializeVar(mbDisabled, eSerializeType_Bool)
kSerializeVar(mlEquippedHandItem, eSerializeType_Int32)
kSerializeClassContainer(mvItems, cLuxInventory_Item_SaveData, eSerializeType_Class)
kSerializeClassContainer(mvCombineCallbacks, cLuxCombineItemsCallback, eSerializeType_Class)
kEndSerialize()

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PLAYER HANDS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayerHands_SaveData::FromPlayerHands(cLuxPlayerHands *apPlayerHands)
{
	mlState = apPlayerHands->mHandState;
	
	if(apPlayerHands->mpCurrentHandObject)
		msCurrentHandObject = apPlayerHands->mpCurrentHandObject->GetName();
	else
		msCurrentHandObject = "";

	msCurrentAnim = apPlayerHands->msCurrentAnim;

    mMesh.FromMeshEntity(apPlayerHands->mpHandsEntity);

	mfHandObjectChargeCount = apPlayerHands->mfHandObjectChargeCount;
	mlHandObjectState = apPlayerHands->mlHandObjectState;
	mbHandObjectAttackDown = apPlayerHands->mbHandObjectAttackDown;
	mbHandObjectInteractDown = apPlayerHands->mbHandObjectInteractDown;
	mfHandObjectAlpha = apPlayerHands->mfHandObjectAlpha;
}

void cLuxPlayerHands_SaveData::ToPlayerHands(cLuxMap *apMap, cLuxPlayerHands *apPlayerHands)
{
	apPlayerHands->mHandState = (eLuxHandsState)mlState;

	if(msCurrentHandObject != "")
	{
		apPlayerHands->mpCurrentHandObject = apPlayerHands->GetHandObject(msCurrentHandObject);
		if(apPlayerHands->mpCurrentHandObject)
			apPlayerHands->CreateAndAttachHandObject(apMap, apPlayerHands->mpCurrentHandObject);
	}

	apPlayerHands->msCurrentAnim = msCurrentAnim;

	mMesh.ToMeshEntity(apPlayerHands->mpHandsEntity);

	apPlayerHands->mfHandObjectChargeCount = mfHandObjectChargeCount;
	apPlayerHands->mlHandObjectState = mlHandObjectState;
	apPlayerHands->mbHandObjectAttackDown = mbHandObjectAttackDown;
	apPlayerHands->mbHandObjectInteractDown = mbHandObjectInteractDown;
	apPlayerHands->mfHandObjectAlpha = mfHandObjectAlpha;

	//If hand object is active, set state as idle
	if(apPlayerHands->mHandState == eLuxHandsState_HandObject && apPlayerHands->mpCurrentHandObject)
	{
		apPlayerHands->PlayAnim(apPlayerHands->mpCurrentHandObject->GetAnimIdle(),true);
		apPlayerHands->mHandState = eLuxHandsState_Idle;
	}
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxPlayerHands_SaveData)
kSerializeVar(mlState, eSerializeType_Int32)
kSerializeVar(msCurrentHandObject, eSerializeType_String)
kSerializeVar(msCurrentAnim, eSerializeType_String)
kSerializeVar(mMesh, eSerializeType_Class)

kSerializeVar(mfHandObjectChargeCount, eSerializeType_Float32)
kSerializeVar(mlHandObjectState, eSerializeType_Int32)
kSerializeVar(mbHandObjectAttackDown, eSerializeType_Bool)
kSerializeVar(mbHandObjectInteractDown, eSerializeType_Bool)
kSerializeVar(mfHandObjectAlpha, eSerializeType_Float32)
kEndSerialize()

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PLAYER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayer_SaveData::cLuxPlayer_SaveData()
{
	mpStateData = NULL;

	// Default values on new variables, to not break old saves
	mfDarkness_Active = true;
	mfScriptJumpForceMul = 1.0f;
}

cLuxPlayer_SaveData::~cLuxPlayer_SaveData()
{
	if(mpStateData) hplDelete(mpStateData);
}

//-----------------------------------------------------------------------

void cLuxPlayer_SaveData::FromPlayer(cLuxPlayer *apPlayer)
{
	//////////////////////
	///State
	iLuxPlayerState *pState = apPlayer->GetCurrentStateData();
	if(pState->IsSaved())
	{
		mlState = apPlayer->GetCurrentState();

		mpStateData = pState->CreateSaveData();
		pState->SaveToSaveData(mpStateData);
	}
	else
	{
		mlState = eLuxPlayerState_Normal;
		mpStateData = NULL;
	}

	//////////////////////
	///Variables
	mbActive = apPlayer->mbActive;

	mfHealth = apPlayer->mfHealth;
	mfSanity = apPlayer->mfSanity;
	mfLampOil = apPlayer->mfLampOil;

	mfTerror = apPlayer->mfTerror;
	mlCoins = apPlayer->mlCoins;
	mlTinderboxes = apPlayer->mlTinderboxes;

	mvHeadSpinSpeed = apPlayer->mvHeadSpinSpeed;

	mbLanternOn = apPlayer->GetHelperLantern()->IsActive();
	mbLanternDisabled = apPlayer->GetHelperLantern()->GetDisabled();

	msDeathHintCat = apPlayer->GetHelperDeath()->GetHintCat();
	msDeathHintEntry = apPlayer->GetHelperDeath()->GetHintEntry();

	cLuxMoveState_Normal *pMoveNormal = static_cast<cLuxMoveState_Normal*>(apPlayer->GetMoveStateData(eLuxMoveState_Normal));
    mbCrouching = pMoveNormal->mbCrouching;

	//////////////////////
	/// SpawnPS
	mbSpawnPSActive = apPlayer->GetHelperSpawnPS()->IsActive();
	mbSpawnPSFile = apPlayer->GetHelperSpawnPS()->GetFileName();
	
	//////////////////////
	/// Flashback
	mbFlashbackActive = apPlayer->GetHelperFlashback()->mbActive;
	mfFlashbackStartCount =	apPlayer->GetHelperFlashback()->mfFlashbackStartCount;
	mfFlashDelay = apPlayer->GetHelperFlashback()->mfFlashDelay;
	msFlashbackFile = apPlayer->GetHelperFlashback()->msFlashbackFile;
	msFlashbackCallback = apPlayer->GetHelperFlashback()->msCallback;
	mlstFlashbackQueue.Clear();
	tLuxFlashbackDataListIt flashIt = apPlayer->GetHelperFlashback()->mlstFlashbackQueue.begin();
	for(; flashIt != apPlayer->GetHelperFlashback()->mlstFlashbackQueue.end(); ++flashIt)
	{
		cLuxFlashbackData& flashData = *flashIt;
		cLuxFlashbackData_SaveData saveData;
		saveData.msFile = flashData.msFile;
		saveData.msCallback = flashData.msCallback;

		mlstFlashbackQueue.Add(saveData);
	}

	//////////////////////
	/// Insanity collapse
	mbInsanityCollapse_Active = apPlayer->GetInsanityCollapse()->mbActive;
	mlInsanityCollapse_State = apPlayer->GetInsanityCollapse()->mlState;
	mfInsanityCollapse_HeightAdd = apPlayer->GetInsanityCollapse()->mfHeightAdd;
	mfInsanityCollapse_Roll = apPlayer->GetInsanityCollapse()->mfRoll;
	mfInsanityCollapse_T = apPlayer->GetInsanityCollapse()->mfT;;

	//////////////////////
	/// Sanity
	mfSanity_AtLowSanityCount = apPlayer->GetHelperSanity()->mfAtLowSanityCount;

	//////////////////////
	// Look At
	mbLookAt_Active = apPlayer->GetHelperLookAt()->mbActive;
	mvLookAt_TargetPos = apPlayer->GetHelperLookAt()->mvTargetPos;
	mvLookAt_CurrentSpeed = apPlayer->GetHelperLookAt()->mvCurrentSpeed;
	mfLookAt_SpeedMul = apPlayer->GetHelperLookAt()->mfSpeedMul;
	mfLookAt_MaxSpeed = apPlayer->GetHelperLookAt()->mfMaxSpeed;
	msLookAt_AtTargetCallback = apPlayer->GetHelperLookAt()->msAtTargetCallback;
	mfLookAt_DestFovMul = apPlayer->GetHelperLookAt()->mfDestFovMul;
	mfLookAt_Fov = apPlayer->GetHelperLookAt()->mfFov;
	mfLookAt_FovSpeed = apPlayer->GetHelperLookAt()->mfFovSpeed;
	mfLookAt_FovMaxSpeed = apPlayer->GetHelperLookAt()->mfFovMaxSpeed;

	//////////////////////
	// Darkness
	mfDarkness_Active = apPlayer->GetHelperInDarkness()->mbActive;

	//////////////////////
	/// Misc vars
	kCopyFromVar(apPlayer, mbIsInWater);
	kCopyFromVar(apPlayer, msWaterStepSound);
	kCopyFromVar(apPlayer, mfWaterSpeedMul);

	kCopyFromVar(apPlayer, mbJumpDisabled);
	kCopyFromVar(apPlayer, mbCrouchDisabled);
	kCopyFromVar(apPlayer, mbSanityDrainDisabled);

	kCopyFromVar(apPlayer, mfInteractionMoveSpeedMul);
	kCopyFromVar(apPlayer, mfEventMoveSpeedMul);
	kCopyFromVar(apPlayer, mfEventRunSpeedMul);
	kCopyFromVar(apPlayer, mfScriptMoveSpeedMul);
	kCopyFromVar(apPlayer, mfScriptRunSpeedMul);
	kCopyFromVar(apPlayer, mfLookSpeedMul);
	kCopyFromVar(apPlayer, mfHurtMoveSpeedMul);
	kCopyFromVar(apPlayer, mfInsanityCollapseSpeedMul);

	kCopyFromVar(apPlayer, mfScriptJumpForceMul);

	kCopyFromVar(apPlayer, mfAspectMul);
	kCopyFromVar(apPlayer, mfFOVMul);
	kCopyFromVar(apPlayer, mfAspectMulGoal);
	kCopyFromVar(apPlayer, mfFOVMulGoal);
	kCopyFromVar(apPlayer, mfAspectMulSpeed);
	kCopyFromVar(apPlayer, mfFOVMulSpeed);

	kCopyFromVar(apPlayer, mfRoll);
	kCopyFromVar(apPlayer, mfRollGoal);
	kCopyFromVar(apPlayer, mfRollSpeedMul);
	kCopyFromVar(apPlayer, mfRollMaxSpeed);

	kCopyFromVar(apPlayer, mvCamAnimPos);
	kCopyFromVar(apPlayer, mvCamAnimPosGoal);
	kCopyFromVar(apPlayer, mfCamAnimPosSpeedMul);
	kCopyFromVar(apPlayer, mfCamAnimPosMaxSpeed);

	kCopyFromVar(apPlayer, mbNoFallDamage);

	kCopyFromVar(apPlayer, mbScriptShowFocusIconAndCrossHair);

	//TODO: Get the correct move mode!?

	//////////////////////
	///Body
	mCharBody.FromBody(apPlayer->mpCharBody);
	
	//////////////////////
	///Camera
	cCamera *pCam = apPlayer->GetCamera();
	mvCameraAngles = cVector3f(pCam->GetPitch(),pCam->GetYaw(), pCam->GetRoll());

	mfPitchMaxLimit = pCam->GetPitchMaxLimit();
	mfPitchMinLimit = pCam->GetPitchMinLimit();
	mfYawMaxLimit = pCam->GetYawMaxLimit();
	mfYawMinLimit = pCam->GetYawMinLimit();

	mvHeadPosAdds.Resize(apPlayer->mvHeadPosAdds.size());
	for(size_t i=0; i<mvHeadPosAdds.Size(); ++i)
		mvHeadPosAdds[i] = apPlayer->mvHeadPosAdds[i];

	///////////////////
	// Terror Enemies
	tLuxEnemySetIt it = apPlayer->m_setTerrorEnemies.begin();
	for(; it != apPlayer->m_setTerrorEnemies.end(); ++it)
	{
		iLuxEnemy *pEnemy = *it;
		mvTerrorEnemyIDs.Add(pEnemy->GetID());
	}

	//////////////////////
	///Player hands
	mPlayerHands.FromPlayerHands(apPlayer->GetHands());

	//////////////////////
	///Collide callbacks
	for(tLuxCollideCallbackListIt it = apPlayer->mlstCollideCallbacks.begin(); it != apPlayer->mlstCollideCallbacks.end(); ++it)
	{
		cLuxCollideCallback *pCallback = *it;
		cLuxCollideCallback_SaveData saveCallback;
		saveCallback.FromCallback(pCallback);

		mlstCollideCallbacks.Add(saveCallback);
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer_SaveData::ToPlayer(cLuxMap *apMap,cLuxPlayer *apPlayer)
{
	//////////////////////
	///State
	eLuxPlayerState playerState = (eLuxPlayerState)mlState;
	iLuxPlayerState *pState = apPlayer->GetStateData(playerState);

	if(mpStateData) pState->LoadFromSaveDataBeforeEnter(apMap, mpStateData);
	apPlayer->ChangeState(playerState);
	if(mpStateData) pState->LoadFromSaveDataAfterEnter(apMap, mpStateData);

	//////////////////////
	///Variables
	apPlayer->mbActive = mbActive;

	apPlayer->mfHealth = mfHealth;
	apPlayer->mfSanity = mfSanity;
	apPlayer->mfLampOil = mfLampOil;
	
	apPlayer->mfTerror = mfTerror;
	apPlayer->mlCoins = mlCoins;
	apPlayer->mlTinderboxes = mlTinderboxes;

	apPlayer->mvHeadSpinSpeed = mvHeadSpinSpeed;

	apPlayer->GetHelperLantern()->SetActive(mbLanternOn, false, false);
	apPlayer->GetHelperLantern()->SetDisabled(mbLanternDisabled);

	apPlayer->GetHelperDeath()->SetHint(msDeathHintCat, msDeathHintEntry);

	cLuxMoveState_Normal *pMoveNormal = static_cast<cLuxMoveState_Normal*>(apPlayer->GetMoveStateData(eLuxMoveState_Normal));
	pMoveNormal->mbCrouching = mbCrouching;

	//////////////////////
	/// SpawnPS
	if(mbSpawnPSActive)
	{
		apPlayer->GetHelperSpawnPS()->Start(mbSpawnPSFile); 
	}
	else
	{
		apPlayer->GetHelperSpawnPS()->Stop();
	}
	 
	//////////////////////
	/// Flashback
	apPlayer->GetHelperFlashback()->mbActive = mbFlashbackActive;
	apPlayer->GetHelperFlashback()->mfFlashbackStartCount = mfFlashbackStartCount;
	apPlayer->GetHelperFlashback()->mfFlashDelay = mfFlashDelay;
	apPlayer->GetHelperFlashback()->msFlashbackFile = msFlashbackFile;
	apPlayer->GetHelperFlashback()->msCallback = msFlashbackCallback;
	
	apPlayer->GetHelperFlashback()->mlstFlashbackQueue.clear();
	cContainerListIterator<cLuxFlashbackData_SaveData> flashIt =mlstFlashbackQueue.GetIterator();
	while(flashIt.HasNext())
	{
		cLuxFlashbackData_SaveData saveData = flashIt.Next();
		apPlayer->GetHelperFlashback()->mlstFlashbackQueue.push_back(cLuxFlashbackData(saveData.msFile,saveData.msCallback) );
	}

	//////////////////////
	/// Insanity collapse
	apPlayer->GetInsanityCollapse()->mbActive = mbInsanityCollapse_Active;
	apPlayer->GetInsanityCollapse()->mlState = mlInsanityCollapse_State;
	apPlayer->GetInsanityCollapse()->mfHeightAdd = mfInsanityCollapse_HeightAdd;
	apPlayer->GetInsanityCollapse()->mfRoll = mfInsanityCollapse_Roll;
	apPlayer->GetInsanityCollapse()->mfT = mfInsanityCollapse_T;

	//////////////////////
	/// Sanity
	apPlayer->GetHelperSanity()->mfAtLowSanityCount = mfSanity_AtLowSanityCount;

	//////////////////////
	// Look At
	apPlayer->GetHelperLookAt()->mbActive = mbLookAt_Active;
	apPlayer->GetHelperLookAt()->mvTargetPos = mvLookAt_TargetPos;
	apPlayer->GetHelperLookAt()->mvCurrentSpeed = mvLookAt_CurrentSpeed;
	apPlayer->GetHelperLookAt()->mfSpeedMul = mfLookAt_SpeedMul;
	apPlayer->GetHelperLookAt()->mfMaxSpeed = mfLookAt_MaxSpeed;
	apPlayer->GetHelperLookAt()->msAtTargetCallback = msLookAt_AtTargetCallback;
	apPlayer->GetHelperLookAt()->mfDestFovMul = mfLookAt_DestFovMul;
	apPlayer->GetHelperLookAt()->mfFov = mfLookAt_Fov;
	apPlayer->GetHelperLookAt()->mfFovSpeed = mfLookAt_FovSpeed;
	apPlayer->GetHelperLookAt()->mfFovMaxSpeed = mfLookAt_FovMaxSpeed;
	
	//////////////////////
	// Darkness
	apPlayer->GetHelperInDarkness()->SetActive(mfDarkness_Active);

	//////////////////////
	/// Misc vars
	kCopyToVar(apPlayer, mbIsInWater);
	kCopyToVar(apPlayer, msWaterStepSound);
	kCopyToVar(apPlayer, mfWaterSpeedMul);

	kCopyToVar(apPlayer, mbJumpDisabled);
	kCopyToVar(apPlayer, mbCrouchDisabled);
	kCopyFromVar(apPlayer, mbSanityDrainDisabled);

	kCopyToVar(apPlayer, mfInteractionMoveSpeedMul);
	kCopyToVar(apPlayer, mfEventMoveSpeedMul);
	kCopyToVar(apPlayer, mfEventRunSpeedMul);
	kCopyToVar(apPlayer, mfScriptMoveSpeedMul);
	kCopyToVar(apPlayer, mfScriptRunSpeedMul);
	kCopyToVar(apPlayer, mfLookSpeedMul);
	kCopyToVar(apPlayer, mfHurtMoveSpeedMul);
	kCopyToVar(apPlayer, mfInsanityCollapseSpeedMul);

	kCopyToVar(apPlayer, mfScriptJumpForceMul);

	kCopyToVar(apPlayer, mfAspectMul);
	kCopyToVar(apPlayer, mfFOVMul);
	kCopyToVar(apPlayer, mfAspectMulGoal);
	kCopyToVar(apPlayer, mfFOVMulGoal);
	kCopyToVar(apPlayer, mfAspectMulSpeed);
	kCopyToVar(apPlayer, mfFOVMulSpeed);

	kCopyToVar(apPlayer, mfRoll);
	kCopyToVar(apPlayer, mfRollGoal);
	kCopyToVar(apPlayer, mfRollSpeedMul);
	kCopyToVar(apPlayer, mfRollMaxSpeed);

	kCopyToVar(apPlayer, mvCamAnimPos);
	kCopyToVar(apPlayer, mvCamAnimPosGoal);
	kCopyToVar(apPlayer, mfCamAnimPosSpeedMul);
	kCopyToVar(apPlayer, mfCamAnimPosMaxSpeed);

	kCopyToVar(apPlayer, mbNoFallDamage);

	kCopyToVar(apPlayer, mbScriptShowFocusIconAndCrossHair);

	//TODO: Set the correct move mode!


	//////////////////////
	///Body
	mCharBody.ToBody(apPlayer->mpCharBody);


	//////////////////////
	///Camera
	cCamera *pCam = apPlayer->GetCamera();
	pCam->SetPitchLimits(mfPitchMinLimit, mfPitchMaxLimit);
	pCam->SetYawLimits(mfYawMinLimit, mfYawMaxLimit);
	
	pCam->SetPitch(mvCameraAngles.x);
	pCam->SetYaw(mvCameraAngles.y);
	pCam->SetRoll(mvCameraAngles.z);

	if(apPlayer->mvHeadPosAdds.size() == mvHeadPosAdds.Size())
	{
		for(size_t i=0; i<apPlayer->mvHeadPosAdds.size(); ++i)
			apPlayer->mvHeadPosAdds[i] = mvHeadPosAdds[i];
	}
	
	///////////////////
	// Terror Enemies
	for(size_t i=0; i<mvTerrorEnemyIDs.Size(); ++i)
	{
		iLuxEntity *pEntity = apMap->GetEntityByID(mvTerrorEnemyIDs[i]);
		if(pEntity && pEntity->GetEntityType() == eLuxEntityType_Enemy)
		{
			iLuxEnemy *pEnemy = static_cast<iLuxEnemy*>(pEntity);
			apPlayer->m_setTerrorEnemies.insert(pEnemy);
		}
		else
		{
			Warning("Could not find entity with id %d\n", mvTerrorEnemyIDs[i]);
		}
	}

	//////////////////////
	///Player hands
	mPlayerHands.ToPlayerHands(apMap, apPlayer->GetHands());

	//////////////////////
	///Collide callbacks
	cContainerListIterator<cLuxCollideCallback_SaveData> it = mlstCollideCallbacks.GetIterator();
	while(it.HasNext())
	{
		cLuxCollideCallback_SaveData& saveCallback = it.Next();
		cLuxCollideCallback *pCallback = hplNew(cLuxCollideCallback, ());

		saveCallback.ToCallback(apMap, apPlayer, pCallback);
		apPlayer->mlstCollideCallbacks.push_back(pCallback);
	}
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxFlashbackData_SaveData)

kSerializeVar(msFile,		eSerializeType_String)
kSerializeVar(msCallback,	eSerializeType_String)

kEndSerialize()


kBeginSerializeBase(cLuxPlayer_SaveData)

kSerializeVar(mlState, eSerializeType_Int32)
kSerializeVar(mpStateData, eSerializeType_ClassPointer)

kSerializeVar(mbActive, eSerializeType_Bool)

kSerializeVar(mfHealth, eSerializeType_Float32)
kSerializeVar(mfSanity, eSerializeType_Float32)
kSerializeVar(mfLampOil, eSerializeType_Float32)
kSerializeVar(mfTerror, eSerializeType_Float32)
kSerializeVar(mlCoins, eSerializeType_Int32)
kSerializeVar(mlTinderboxes, eSerializeType_Int32)

kSerializeVar(mbIsInWater, eSerializeType_Bool)
kSerializeVar(msWaterStepSound, eSerializeType_String)
kSerializeVar(mfWaterSpeedMul, eSerializeType_Float32)

kSerializeVar(mbJumpDisabled, eSerializeType_Bool)
kSerializeVar(mbCrouchDisabled, eSerializeType_Bool)
kSerializeVar(mbSanityDrainDisabled, eSerializeType_Bool)

kSerializeVar(mbCrouching, eSerializeType_Bool)

kSerializeVar(mfInteractionMoveSpeedMul, eSerializeType_Float32)
kSerializeVar(mfEventMoveSpeedMul, eSerializeType_Float32)
kSerializeVar(mfEventRunSpeedMul, eSerializeType_Float32)
kSerializeVar(mfScriptMoveSpeedMul, eSerializeType_Float32)
kSerializeVar(mfScriptRunSpeedMul, eSerializeType_Float32)
kSerializeVar(mfLookSpeedMul, eSerializeType_Float32)
kSerializeVar(mfHurtMoveSpeedMul, eSerializeType_Float32)
kSerializeVar(mfInsanityCollapseSpeedMul, eSerializeType_Float32)

kSerializeVar(mfScriptJumpForceMul, eSerializeType_Float32)

kSerializeVar(mfAspectMul, eSerializeType_Float32)
kSerializeVar(mfFOVMul, eSerializeType_Float32)
kSerializeVar(mfAspectMulGoal, eSerializeType_Float32)
kSerializeVar(mfFOVMulGoal, eSerializeType_Float32)
kSerializeVar(mfAspectMulSpeed, eSerializeType_Float32)
kSerializeVar(mfFOVMulSpeed, eSerializeType_Float32)

kSerializeVar(mfRoll, eSerializeType_Float32)
kSerializeVar(mfRollGoal, eSerializeType_Float32)
kSerializeVar(mfRollSpeedMul, eSerializeType_Float32)
kSerializeVar(mfRollMaxSpeed, eSerializeType_Float32)

kSerializeVar(mvCamAnimPos, eSerializeType_Vector3f)
kSerializeVar(mvCamAnimPosGoal, eSerializeType_Vector3f)
kSerializeVar(mfCamAnimPosSpeedMul, eSerializeType_Float32)
kSerializeVar(mfCamAnimPosMaxSpeed, eSerializeType_Float32)

kSerializeVar(mvHeadSpinSpeed, eSerializeType_Vector2f)

kSerializeVar(mbNoFallDamage, eSerializeType_Bool)

kSerializeVar(mbScriptShowFocusIconAndCrossHair, eSerializeType_Bool)

kSerializeVar(mbLanternOn, eSerializeType_Bool)
kSerializeVar(mbLanternDisabled, eSerializeType_Bool)

kSerializeVar(msDeathHintCat, eSerializeType_String)
kSerializeVar(msDeathHintEntry, eSerializeType_String)

kSerializeVar(mbSpawnPSActive, eSerializeType_Bool)
kSerializeVar(mbSpawnPSFile, eSerializeType_String)

kSerializeVar(mbFlashbackActive, eSerializeType_Bool)
kSerializeVar(mfFlashbackStartCount, eSerializeType_Float32)
kSerializeVar(mfFlashDelay, eSerializeType_Float32)
kSerializeVar(msFlashbackFile, eSerializeType_String)
kSerializeVar(msFlashbackCallback, eSerializeType_String)
kSerializeClassContainer(mlstFlashbackQueue,cLuxFlashbackData_SaveData, eSerializeType_Class)

kSerializeVar(mbInsanityCollapse_Active, eSerializeType_Bool)
kSerializeVar(mlInsanityCollapse_State, eSerializeType_Int32)
kSerializeVar(mfInsanityCollapse_HeightAdd, eSerializeType_Float32)
kSerializeVar(mfInsanityCollapse_Roll, eSerializeType_Float32)
kSerializeVar(mfInsanityCollapse_T, eSerializeType_Float32)

kSerializeVar(mfSanity_AtLowSanityCount, eSerializeType_Float32)

kSerializeVar(mbLookAt_Active, eSerializeType_Bool)
kSerializeVar(mvLookAt_TargetPos, eSerializeType_Vector3f)
kSerializeVar(mvLookAt_CurrentSpeed, eSerializeType_Vector3f)
kSerializeVar(mfLookAt_SpeedMul, eSerializeType_Float32)
kSerializeVar(mfLookAt_MaxSpeed, eSerializeType_Float32)
kSerializeVar(msLookAt_AtTargetCallback, eSerializeType_String)
kSerializeVar(mfLookAt_DestFovMul, eSerializeType_Float32)
kSerializeVar(mfLookAt_Fov, eSerializeType_Float32)
kSerializeVar(mfLookAt_FovSpeed, eSerializeType_Float32)
kSerializeVar(mfLookAt_FovMaxSpeed, eSerializeType_Float32)
	
kSerializeVar(mfDarkness_Active, eSerializeType_Bool)

kSerializeVar(mCharBody, eSerializeType_Class)

kSerializeVar(mvCameraAngles, eSerializeType_Vector3f)

kSerializeVar(mfPitchMaxLimit, eSerializeType_Float32)
kSerializeVar(mfPitchMinLimit, eSerializeType_Float32)
kSerializeVar(mfYawMaxLimit, eSerializeType_Float32)
kSerializeVar(mfYawMinLimit, eSerializeType_Float32)

kSerializeClassContainer(mvHeadPosAdds, cLuxHeadPosAdd, eSerializeType_Class)

kSerializeVarContainer(mvTerrorEnemyIDs, eSerializeType_Int32)

kSerializeVar(mPlayerHands, eSerializeType_Class)

kSerializeClassContainer(mlstCollideCallbacks, cLuxCollideCallback_SaveData, eSerializeType_Class)

kEndSerialize()

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// SAVED GAME
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
cLuxSaveGame_SaveData::cLuxSaveGame_SaveData()
{
	mpSavedMaps = NULL;
	mbHardmode = false;
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxSaveGame_SaveData)

////////////////////////
// HARDMODE

kSerializeVar(mbHardmode, eSerializeType_Bool)

////////////////////////

kSerializeVar(msMapFolder, eSerializeType_String)
kSerializeVar(mMap, eSerializeType_Class)
kSerializeVar(mInventory, eSerializeType_Class)
kSerializeVar(mJournal, eSerializeType_Class)
kSerializeVar(mPlayer, eSerializeType_Class)
kSerializeVar(mMusicHandler, eSerializeType_Class)
kSerializeVar(mEffectHandler, eSerializeType_Class)
kSerializeVar(mGlobalDataHandler, eSerializeType_Class)
kSerializeVar(mHintHandler, eSerializeType_Class)
kSerializeVar(mInsanityHandler, eSerializeType_Class)
kSerializeVar(mLoadScreenHandler, eSerializeType_Class)

kSerializeVar(mpSavedMaps, eSerializeType_ClassPointer)


kEndSerialize()

//-----------------------------------------------------------------------



