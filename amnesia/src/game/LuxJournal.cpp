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

#include "LuxJournal.h"

#include "LuxMapHandler.h"
#include "LuxInputHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxPlayer.h"
#include "LuxHelpFuncs.h"
#include "LuxEffectHandler.h"
#include "LuxProgressLogHandler.h"
#include "LuxMap.h"
#include "LuxInventory.h"
#include "LuxHintHandler.h"

#include "LuxAchievementHandler.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// GOBAL STRUCTS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

static eLuxJournalState gvPressExitState[eLuxJournalState_LastEnum] =
{
	eLuxJournalState_LastEnum,	//Main,
	eLuxJournalState_Main,		//Notes,
	eLuxJournalState_Main,		//Diaries,
	eLuxJournalState_Main,		//QuestLog,
	eLuxJournalState_Notes,		//OpenNote
	eLuxJournalState_Diaries,	//OpenDiary
	eLuxJournalState_Diaries	//OpenNarratedDiary
};

static tString gsBackgroundImage[eLuxJournalState_LastEnum] =
{
		"journal/bg_main.tga",	//Main,
		"journal/bg_notes.tga",		//Notes,
		"journal/bg_diaries.tga",		//Diaries,
		"journal/bg_questlog.tga",		//QuestLog,
		"",		//OpenNote
		"",		//OpenDiary
		""		//OpenNarratedDiary
};

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// WIDGET DATA
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxJournalTextData::Update(float afTimeStep)
{
    if(mpWidget->GetMouseIsOver() || mpWidget->HasFocus())
	{
		mfEffectfAlpha += afTimeStep * 4.0f;
		if(mfEffectfAlpha > 1) mfEffectfAlpha = 1;
	}
	else
	{
		mfEffectfAlpha -= afTimeStep * 2.0f;
		if(mfEffectfAlpha < 0) mfEffectfAlpha = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// LIST ENTRY
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxJournal_ListEntry::AddWidget(iWidget *apWidget)
{
	mlstWidgets.push_back(apWidget);
}

void cLuxJournal_ListEntry::SetVisible(bool abX)
{
	tWidgetListIt it = mlstWidgets.begin(); 
	for(; it != mlstWidgets.end(); ++it)
	{
		iWidget *pWidget = *it;
		pWidget->SetEnabled(abX);
		pWidget->SetVisible(abX);
	}
}

//-----------------------------------------------------------------------

void cLuxJournal_ListPage::SetVisible(bool abX)
{
	for(size_t i=0; i<mvEntries.size(); ++i)
	{
		mvEntries[i].SetVisible(abX);
	}

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// STATE DATA
//////////////////////////////////////////////////////////////////////////

cLuxJournalStateData::cLuxJournalStateData(cLuxJournal *apJournal, eLuxJournalState aState)
{
	mpJournal = apJournal;
	mState = aState;

	mpRootWidget = mpJournal->mpGuiSet->CreateWidgetDummy();

	Reset();
}

//-----------------------------------------------------------------------

void cLuxJournalStateData::Reset()
{
	mfAlpha =0;
}

//-----------------------------------------------------------------------

void cLuxJournalStateData::OnEnter()
{
	if(mState == eLuxJournalState_Diaries || mState == eLuxJournalState_Notes || mState == eLuxJournalState_QuestLog)
	{
		int lIdx = mpJournal->GetNoteListIndex(mState);
		
		mpJournal->SetNoteListPage(mpJournal->mlCurrentNoteListPage[lIdx], mState);	
	}
	
	mpJournal->mpGuiSet->SetDefaultFocusNavWidget(mpJournal->mpWidgetDefaultNav[mState]);
	mpJournal->mpGuiSet->SetFocusedWidget(mpJournal->mpWidgetDefaultNav[mState]);
}

//-----------------------------------------------------------------------

void cLuxJournalStateData::DestroySessionWidgets()
{
	for(tWidgetListIt it = mlstSessionWidgets.begin(); it != mlstSessionWidgets.end(); ++it)
	{
		mpJournal->mpGuiSet->DestroyWidget(*it);
	}	
	mlstSessionWidgets.clear();
}

//-----------------------------------------------------------------------

void cLuxJournalStateData::Update(float afTimeStep)
{
	/////////////////////
	//Alpha
	if(mState == mpJournal->mCurrentState)
	{
		mfAlpha += afTimeStep * 2.0f;
		if(mfAlpha>1) mfAlpha = 1;
	}
	else
	{
		mfAlpha -= afTimeStep * 5.0f;
		if(mfAlpha<0) mfAlpha = 0;
	}

	/////////////////////
	//Session widgets
	for(tWidgetListIt it = mlstSessionWidgets.begin(); it != mlstSessionWidgets.end(); ++it)
	{
		iWidget *pWidget = *it;
		cColor col = pWidget->GetColorMul();
		col.a = mpJournal->mfAlpha * mfAlpha;
		pWidget->SetColorMul(col);
	}
}

//-----------------------------------------------------------------------

void cLuxJournalStateData::OnDraw(float afFrameTime)
{
	float fAlpha = mfAlpha * mpJournal->mfAlpha;

	///////////////////////
	//Note open
	if(	mState==eLuxJournalState_OpenNote || 
		mState == eLuxJournalState_OpenDiary  || 
		mState == eLuxJournalState_OpenNarratedDiary)
	{
		if(mpJournal->mvPages.empty()) return;

		/////////////////////////
		// Header
		cVector3f vHeaderPos(400, mpJournal->mfNoteHeaderStartY, 3);

		mpJournal->mpGuiSet->DrawFont(mpJournal->msHeader, mpJournal->mpFontMenu,vHeaderPos, mpJournal->mfNoteHeaderFontSize, cColor(1,fAlpha),eFontAlign_Center);

		/////////////////////////
		// Text
		cLuxNotePage *pPage = &mpJournal->mvPages[mpJournal->mlCurrentNotePage];

		cVector3f vPos(400 - mpJournal->mfNoteTextWidth/2, mpJournal->mfNoteTextStartY, 3);
		
		//if(	mpJournal->mlCurrentNotePage == 0 && mpJournal->mvPages.size()==1) //Should be good to center all pages!
		{
			float fHalfRowNum = ((float)pPage->mvRows.size())*0.5f;
			vPos.y = 300 - (mpJournal->mfNoteRowDist * fHalfRowNum);	
		}
		
		for(size_t i=0; i<pPage->mvRows.size(); ++i)
		{
			mpJournal->mpGuiSet->DrawFont(pPage->mvRows[i], mpJournal->mpFontDefault, vPos, mpJournal->mvNoteFontSize, cColor(1,fAlpha));
			vPos.y += mpJournal->mfNoteRowDist;
		}
	}

	/////////////////////////
	// Background Image
	if(mpJournal->mpStateBackgroundGfx && mpJournal->mCurrentState == mState)
	{
		cVector2f vSize = mpJournal->mpStateBackgroundGfx->GetActiveSize();

		mpJournal->mpGuiSet->DrawGfx(mpJournal->mpStateBackgroundGfx, cVector3f(400-vSize.x/2, 300-vSize.y/2, 1), -1, cColor(1, fAlpha));
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// DIARY
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxDiaryContainer::~cLuxDiaryContainer()
{
	STLDeleteAll(mvDiaries);
};

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxJournal::cLuxJournal() : iLuxUpdateable("LuxJournal")
{
	///////////////////////////////
	//Get Engine
	mpGui = gpBase->mpEngine->GetGui();
	mpScene = gpBase->mpEngine->GetScene();
	mpGraphics = gpBase->mpEngine->GetGraphics();

	///////////////////////////////
	//Setup GUI stuff
	mpGuiSkin = mpGui->CreateSkin("gui_main_menu.skin");
	mpGuiSet = mpGui->CreateSet("Inventory", mpGuiSkin);
	mpGuiSet->SetDrawMouse(false);//Init
	
	mvGuiSetCenterSize = cVector2f(800, 600);
	LuxCalcGuiSetScreenOffset(mvGuiSetCenterSize, mvGuiSetSize, mvGuiSetOffset);
	mvGuiSetStartPos = cVector3f(-mvGuiSetOffset.x,-mvGuiSetOffset.y,0);

	mpGuiSet->SetVirtualSize(mvGuiSetSize, -1000,1000, mvGuiSetOffset);
	mpGuiSet->SetActive(false);

	///////////////////////////////
	//Create Viewport
	mpViewport = mpScene->CreateViewport();
	mpViewport->SetActive(false);
	mpViewport->SetVisible(false);

	mpViewport->AddGuiSet(mpGuiSet);


	///////////////////////////////
	//Load settings
	mvScreenSize = gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetScreenSizeFloat();

	mfNoteTextWidth = gpBase->mpMenuCfg->GetFloat("Journal","NoteTextWidth",0);
	mlNoteMaxPageRows = gpBase->mpMenuCfg->GetInt("Journal","NoteMaxPageRows",0);
	mvNoteFontSize = gpBase->mpMenuCfg->GetVector2f("Journal","NoteFontSize",0);
	mfNoteRowDist = gpBase->mpMenuCfg->GetFloat("Journal","NoteRowDist",0);
	mfNoteHeaderFontSize = gpBase->mpMenuCfg->GetVector2f("Journal","NoteHeaderFontSize",0);
	mfNoteHeaderStartY = gpBase->mpMenuCfg->GetFloat("Journal","NoteHeaderStartY",0);
	mfNoteTextStartY = gpBase->mpMenuCfg->GetFloat("Journal","NoteTextStartY",0);

	mfBackTextY = 530;
	mvBackTextFontSize = 28;

	mfMaxNoteListY = 450;
	mfNoteListHeaderY = 40;



	///////////////////////////////
	//Load state data
	mvStateData.resize(eLuxJournalState_LastEnum);
	for(size_t i=0; i<mvStateData.size(); ++i)
	{
		mvStateData[i] = hplNew(cLuxJournalStateData, (this, (eLuxJournalState)i) );
	}

	///////////////////////////////
	//Load Data
	mpWhiteGfx = mpGui->CreateGfxFilledRect(cColor(1,1), eGuiMaterial_Alpha);

	cParserVarContainer programVars;
	mpEffectProgram = mpGraphics->CreateGpuProgramFromShaders("InventoryEffect","inventory_screen_effect_vtx.glsl", "inventory_screen_effect_frag.glsl", &programVars);

	mpFontDefault = NULL;
	mpFontMenu = NULL;
}

//-----------------------------------------------------------------------

cLuxJournal::~cLuxJournal()
{
	STLDeleteAll(mvNotes);
	STLDeleteAll(mvQuestNotes);
	STLDeleteAll(mvDiaryContainers);
	STLDeleteAll(mvStateData);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxJournal::OnClearFonts()
{
}

void cLuxJournal::LoadFonts()
{
	tString sFontFile = gpBase->mpMenuCfg->GetString("Journal","DefaultFont","");
	tString sFontMenu = gpBase->mpMenuCfg->GetString("Journal","MenuFont","");
	mpFontDefault = LoadFont(sFontFile);
	mpFontMenu = LoadFont(sFontMenu);
}

//-----------------------------------------------------------------------

void cLuxJournal::OnStart()
{
	
}

//-----------------------------------------------------------------------

void cLuxJournal::Reset()
{
	///////////////////////////////
	//Reset variables
	mfAlpha =0;	

	mbActive = false;

	mfMouseOverPulse =0;

	mpStateBackgroundGfx = NULL;

	mbForceInstantExit = false;
	mbOpenedFromInventory = false;

	mlLastReadTextEntry = -1;
	mlLastReadTextType = -1;

	mpVoiceEntry = NULL;

	for(int i=0; i<eLuxJournalState_LastEnum; ++i)
	{
		mpImageForward[i] = NULL;	
		mpImageBackward[i] = NULL;
	}

	for(int i=0; i<3; ++i)
		mlCurrentNoteListPage[i] =0;


	STLDeleteAll(mvNotes);
	STLDeleteAll(mvQuestNotes);
	STLDeleteAll(mvDiaryContainers);

	ResetSessionVars();
}

//-----------------------------------------------------------------------

void cLuxJournal::OnGameStart()
{

}

//-----------------------------------------------------------------------

void cLuxJournal::Update(float afTimeStep)
{
	/////////////////////////
	// Update alpha
	if(mbActive)
	{
		mfAlpha += afTimeStep*2;
		if(mfAlpha >1) mfAlpha =1;
	}
	else
	{
		mfAlpha -= afTimeStep*3;
		if(mfAlpha<0)
		{
			mfAlpha =0;
			
			if(mbOpenedFromInventory)
			{
				gpBase->mpInventory->SetEnterFromJournal(true);
				gpBase->mpEngine->GetUpdater()->SetContainer("Inventory");
			}
			else
			{
				gpBase->mpEngine->GetUpdater()->SetContainer("Default");
				gpBase->mpInputHandler->ChangeState(eLuxInputState_Game);
			}
		}
	}

	/////////////////////////
	// Update Mouse over pulse
	mfMouseOverPulse += afTimeStep;
	

	/////////////////////////
	// Update state data
	for(size_t i=0; i<mvStateData.size(); ++i)
	{
		mvStateData[i]->Update(afTimeStep);
	}

	/////////////////////////
	// Update widget data
	for(tLuxJournalWidgetDataIt it = mlstSessionWidgetData.begin(); it != mlstSessionWidgetData.end(); ++it)
	{
		iLuxJournalWidgetData *pData = *it;

        pData->Update(afTimeStep);
	}

	/////////////////////
	//Check if narration voice is finnished
	if(mCurrentState == eLuxJournalState_OpenNarratedDiary && mbActive)
	{
		cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
		if(mpVoiceEntry==NULL || pSoundHandler->IsValid(mpVoiceEntry,mlVoiceEntryID)==false)
		{
			mpVoiceEntry = NULL;
		
			ExitPressed(false);
		}
	}

	////////////////////////
	//Update extra effects
	gpBase->mpEffectHandler->GetSanityGainFlash()->Update(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxJournal::OnEnterContainer(const tString& asOldContainer)
{
	////////////////////////
	//Create stuff
	CreateBackground();
	CreateGui();

	////////////////////////////
	//Set up states and viewport
	mbActive = true;

	gpBase->mpInputHandler->ChangeState(eLuxInputState_Journal);

	mpViewport->SetActive(true);
	mpViewport->SetVisible(true);
	mpGuiSet->SetActive(true);

#ifdef USE_GAMEPAD
	if(gpBase->mpInputHandler->IsGamepadPresent() == false)
	{
		mpGuiSet->SetDrawMouse(true);
		mpGuiSet->SetMouseMovementEnabled(true);
	}
	else if(gpBase->mpInputHandler->IsGamepadPresent())
	{
		mpGuiSet->SetDrawMouse(false);
		mpGuiSet->SetMouseMovementEnabled(false);
	}
#else
	mpGuiSet->SetDrawMouse(true);
#endif

	mpGuiSet->ResetMouseOver();
	mpGui->SetFocus(mpGuiSet);
	
	//gpBase->mpMapHandler->PauseSoundsAndMusic();

	/////////////////////
	//Effects
	gpBase->mpHelpFuncs->PlayGuiSoundData("journal_open", eSoundEntryType_Gui);

	gpBase->mpMapHandler->GetCurrentMap()->GetPhysicsWorld()->FadeoutAllLoopSounds(1.0f);

	gpBase->mpPlayer->StopTerrorSound();

	///////////////////////////
	//Pause voices and turn down volume on world sounds
	if(mbOpenedFromInventory == false)
	{
		gpBase->mpEffectHandler->GetPlayVoice()->PauseCurrentVoices();
		
		cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
		pSoundHandler->FadeGlobalVolume(0.5f, 0.5f,eSoundEntryType_World,eLuxGlobalVolumeType_GameMenu, false);
	}

	/////////////////////
	//Reset variables
	ResetSessionVars();

	/////////////////////
	//Load main background
	if(gsBackgroundImage[mCurrentState]!="")
		SetStateBackgroundGfx(gsBackgroundImage[mCurrentState]);
}

//-----------------------------------------------------------------------

void cLuxJournal::OnLeaveContainer(const tString& asNewContainer)
{
	///////////////////////////
	//Pause voices and turn down volume on world sounds
	if(mbOpenedFromInventory == false)
	{
		gpBase->mpEffectHandler->GetPlayVoice()->UnpauseCurrentVoices();

		cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
		pSoundHandler->FadeGlobalVolume(1.0f, 0.5f,eSoundEntryType_World,eLuxGlobalVolumeType_GameMenu, false);
	}
	
	////////////////////////////
	//Set up states
	mpViewport->SetActive(false);
	mpViewport->SetVisible(false);
	mpGuiSet->SetActive(false);

	DestroyGui();
	DestroyBackground();

	mbForceInstantExit = false;
	mbOpenedFromInventory = false;

}

//-----------------------------------------------------------------------

void cLuxJournal::OnDraw(float afFrameTime)
{
	////////////////////////
	//Draw background
	if(mpScreenGfx && mfAlpha<1) 
		mpGuiSet->DrawGfx(mpScreenGfx,mvGuiSetStartPos,mvGuiSetSize);

	if(mpScreenBgGfx)
		mpGuiSet->DrawGfx(mpScreenBgGfx,mvGuiSetStartPos+cVector3f(0,0,0.2f),mvGuiSetSize,cColor(1, mfAlpha));


	for(size_t i=0; i<mvStateData.size(); ++i)
	{
		if(mvStateData[i]->mfAlpha > 0)
			mvStateData[i]->OnDraw(afFrameTime);
	}

	//////////////////////////////////7
	//Fade
	if(mbOpenedFromInventory)
	{
		mpGuiSet->DrawGfx(mpWhiteGfx,mvGuiSetStartPos+cVector3f(0,0,20),mvGuiSetSize,cColor(0, 1.0f - mfAlpha*mfAlpha));
	}

	////////////////////////
	//Draw extra effects
	gpBase->mpEffectHandler->GetSanityGainFlash()->DrawFlash(mpGuiSet, afFrameTime);
}

//-----------------------------------------------------------------------

void cLuxJournal::ExitPressed(bool abInstantExit)
{
	if(mbActive==false) return;

	////////////////////
	//Narrated diary open
	if(	mCurrentState == eLuxJournalState_OpenNarratedDiary && 
		mlCurrentNotePage < (int)mvPages.size()-1)
	{
		SetNotePage(mlCurrentNotePage+1);	
	}
	////////////////////
	//Exit the journal directly
	else if(abInstantExit || mbForceInstantExit)
	{
		Exit();
		gpBase->mpHelpFuncs->PlayGuiSoundData("journal_close", eSoundEntryType_Gui);

		gpBase->mpHintHandler->Add("RecentlyReadText", kTranslate("Hints", "RecentlyReadText"), -1);
	}
	////////////////////
	//Go to state above.
	else
	{
		eLuxJournalState newState = gvPressExitState[mCurrentState];
	    
		if(newState == eLuxJournalState_LastEnum)
		{
			Exit();
			gpBase->mpHelpFuncs->PlayGuiSoundData("journal_close", eSoundEntryType_Gui);
		}
		else
		{
			ChangeState(newState);
		}
	}
}

//-----------------------------------------------------------------------

void cLuxJournal::Exit()
{
	if(mbActive==false) return;

	if(mbForceInstantExit)
	{
		gpBase->mpProgressLogHandler->AddLog(eLuxProgressLogLevel_Low, "Forced Instant Exit in Journal (player finished reading).");
	}

	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
	if(mpVoiceEntry && pSoundHandler->IsValid(mpVoiceEntry,mlVoiceEntryID))
	{
		mpVoiceEntry->FadeOut(3.0f);
	}
	mpVoiceEntry = NULL;

	gpBase->mpInputHandler->ChangeState(eLuxInputState_Null);

	mpGuiSet->SetDrawMouse(false);

	mpGui->SetFocus(NULL);

	mbActive = false;
}

//-----------------------------------------------------------------------

cLuxNote* cLuxJournal::AddNote(const tString& asNameAndTextEntry, const tString& asImage)
{
	cLuxNote *pNote = hplNew( cLuxNote, () );

	pNote->msNameEntry = "Note_"+asNameAndTextEntry+"_Name";
	pNote->msTextEntry = "Note_"+asNameAndTextEntry+"_Text";
	
	tString sExt = cString::GetFileExt(asImage);
	pNote->msImageFile = cString::SetFileExt( cString::SetFileExt(asImage,"")+"_large",sExt);
	pNote->msIconFile = cString::SetFileExt( cString::SetFileExt(asImage,"")+"_icon",sExt);
	
    mvNotes.push_back(pNote);

	if(gpBase->msGameName == "Amnesia - The Dark Descent -")
	{
		if(mvNotes.size() == 21)
		{
			if(gpBase->mpMainConfig->GetBool("Main", "MasterArchivis_Justine", false))
			{
				gpBase->mpAchievementHandler->UnlockAchievement(eLuxAchievement_MasterArchivist);
			}
			
			gpBase->mpMainConfig->SetBool("Main", "MasterArchivis_TDD", true);
			gpBase->mpMainConfig->Save();
		}
	}
	if(gpBase->msGameName == "Amnesia - Justine -")
	{
		if(mvNotes.size() == 9)
		{
			if(gpBase->mpMainConfig->GetBool("Main", "MasterArchivis_TDD", false))
			{
				gpBase->mpAchievementHandler->UnlockAchievement(eLuxAchievement_MasterArchivist);
			}
			
			gpBase->mpMainConfig->SetBool("Main", "MasterArchivis_Justine", true);
			gpBase->mpMainConfig->Save();
		}
	}

	return pNote;
}

//-----------------------------------------------------------------------

cLuxDiary* cLuxJournal::AddDiary(const tString& asNameAndTextEntry, const tString& asImage, int &alCurrentEntryIdx)
{
	cLuxDiary *pDiary = hplNew( cLuxDiary, () );
	
    cLuxDiaryContainer *pContainer = CreateDiaryContainer(asNameAndTextEntry);
	int lNum = (int)pContainer->mvDiaries.size()+1;

	alCurrentEntryIdx = lNum;

	pDiary->msNameEntry = "Diary_"+asNameAndTextEntry+"_Name"+cString::ToString(lNum);
	pDiary->msTextEntry = "Diary_"+asNameAndTextEntry+"_Text"+cString::ToString(lNum);

	tString sExt = cString::GetFileExt(asImage);
	pDiary->msImageFile = cString::SetFileExt( cString::SetFileExt(asImage,"")+"_large",sExt);
	pDiary->msIconFile = cString::SetFileExt( cString::SetFileExt(asImage,"")+"_icon",sExt);

	pContainer->mvDiaries.push_back(pDiary);

	return pDiary;
}

//-----------------------------------------------------------------------

bool cLuxJournal::AddQuestNote(const tString& asName, const tString& asNameAndTextEntry)
{
	for(size_t i=0; i<mvQuestNotes.size(); ++i)
	{
		if(mvQuestNotes[i]->msName == asName) return false;
	}

	cLuxQuestNote *pQuestNote = hplNew( cLuxQuestNote, () );

	pQuestNote->msName = asName;
	pQuestNote->msNameEntry = "Quest_"+asNameAndTextEntry+"_Name";
	pQuestNote->msTextEntry = "Quest_"+asNameAndTextEntry+"_Text";
	pQuestNote->mbActive = true;

	mvQuestNotes.push_back(pQuestNote);

	return true;
}

//-----------------------------------------------------------------------

bool cLuxJournal::DisableQuestNote(const tString& asName)
{
	for(size_t i=0; i<mvQuestNotes.size(); ++i)
	{
		if(mvQuestNotes[i]->msName == asName)
		{
			if(mvQuestNotes[i]->mbActive)
			{
				mvQuestNotes[i]->mbActive = false;
				return true;
			}
			else
			{
				return false;
			}
			
		}
	}

	Error("Quest '%s' does not exist!\n", asName.c_str());
	return false;
    
	/*std::vector<cLuxQuestNote*>::iterator it = mvQuestNotes.begin(); 
	for(; it != mvQuestNotes.end(); ++it)
	{
		cLuxQuestNote* pQuest = *it;
		if(pQuest->msName == asName)
		{
			hplDelete(pQuest);
            mvQuestNotes.erase(it);
			return;
		}
	}*/
}

//-----------------------------------------------------------------------

cLuxQuestNote* cLuxJournal::GetQuestNote(const tString& asName)
{
	for(size_t i=0; i<mvQuestNotes.size(); ++i)
	{
		if(mvQuestNotes[i]->msName == asName)
		{
			return mvQuestNotes[i];
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------

void cLuxJournal::ChangeState(eLuxJournalState aState)
{
	mCurrentState = aState;
	
	if(gsBackgroundImage[aState] != "")
		SetStateBackgroundGfx(gsBackgroundImage[aState]);

	for(size_t i=0; i<mvStateData.size(); ++i)
	{
		cWidgetDummy *pRoot = mvStateData[i]->mpRootWidget;
		if(i == mCurrentState)	pRoot->SetVisible(true);
		else					pRoot->SetVisible(false);
	}

	mvStateData[mCurrentState]->OnEnter();
}

//-----------------------------------------------------------------------

void cLuxJournal::OpenNote(cLuxNote *apNote, bool abNarration)
{
	///////////////////////////
	// Get the entry number of the diary
	mlLastReadTextType =0;
	mlLastReadTextCat =-1;
	mlLastReadTextEntry =-1;
	for(size_t i=0; i<mvNotes.size(); ++i)
	{
		if(mvNotes[i] == apNote)
		{
			mlLastReadTextEntry = i;
			break;
		}
	}

	///////////////////////////
	// Open the diary
	if(abNarration)
	{
		ChangeState(eLuxJournalState_OpenNarratedDiary);
		LoadNarrationText(kTranslate("Journal", apNote->msNameEntry), kTranslate("Journal", apNote->msTextEntry) );
	}
	else
	{
		ChangeState(eLuxJournalState_OpenNote);
		LoadText(kTranslate("Journal", apNote->msNameEntry), kTranslate("Journal", apNote->msTextEntry) );
	}
	
	SetStateBackgroundGfx(apNote->msImageFile);
}

//-----------------------------------------------------------------------

void cLuxJournal::OpenDiary(cLuxDiary *apDiary, bool abNarration)
{
	SetDiaryAsLastRead(apDiary);
		
	///////////////////////////
	// Open the diary
	if(abNarration)
	{
		ChangeState(eLuxJournalState_OpenNarratedDiary);
		LoadNarrationText(kTranslate("Journal", apDiary->msNameEntry), kTranslate("Journal", apDiary->msTextEntry) );
	}
	else
	{
		ChangeState(eLuxJournalState_OpenDiary);
		LoadText(kTranslate("Journal", apDiary->msNameEntry), kTranslate("Journal", apDiary->msTextEntry) );
	}
	
	SetStateBackgroundGfx(apDiary->msImageFile);
	
}
//-----------------------------------------------------------------------

void cLuxJournal::SetDiaryAsLastRead(cLuxDiary *apDiary)
{
	///////////////////////////
	// Get the cat and entry of the diary
	mlLastReadTextType = 1;
	mlLastReadTextCat = -1;
	mlLastReadTextEntry = -1;
	for(size_t cat=0; cat<mvDiaryContainers.size(); ++cat)
	{
		cLuxDiaryContainer* pCont = mvDiaryContainers[cat];
		for(size_t entry=0; entry<pCont->mvDiaries.size(); ++entry)
		{
			if(apDiary == pCont->mvDiaries[entry])
			{
				mlLastReadTextCat = cat;
				mlLastReadTextEntry = entry;
				break;
			}
		}
		if(mlLastReadTextCat >=0) break;
	}
}

//-----------------------------------------------------------------------

void cLuxJournal::OpenLastReadText()
{
	if(mlLastReadTextEntry<0 || mlLastReadTextType<0) return;

	///////////////////
	//Note
	if(mlLastReadTextType ==0)
	{
		if(mlLastReadTextEntry >= (int)mvNotes.size()) return;

		SetForceInstantExit(true);
		gpBase->mpEngine->GetUpdater()->SetContainer("Journal");
		OpenNote(mvNotes[mlLastReadTextEntry], false);
	}
	///////////////////
	//Diary
	else
	{
		if(mlLastReadTextCat<0) return;
		if(mlLastReadTextCat >= (int)mvDiaryContainers.size()) return;
		if(mlLastReadTextEntry >= (int)mvDiaryContainers[mlLastReadTextCat]->mvDiaries.size())return;
		
		SetForceInstantExit(true);
		gpBase->mpEngine->GetUpdater()->SetContainer("Journal");
		OpenDiary(mvDiaryContainers[mlLastReadTextCat]->mvDiaries[mlLastReadTextEntry], false);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxDiaryContainer* cLuxJournal::CreateDiaryContainer(const tString& asType)
{
	for(size_t i=0; i<mvDiaryContainers.size(); ++i)
	{
		cLuxDiaryContainer *pCont = mvDiaryContainers[i];
		if(pCont->msType==asType) return pCont;
	}

	cLuxDiaryContainer *pCont = hplNew(cLuxDiaryContainer, () );
	pCont->msType = asType;
	mvDiaryContainers.push_back(pCont);
	return pCont;
}

//-----------------------------------------------------------------------

void cLuxJournal::SetupLabel(cWidgetLabel *apLabel, const cVector2f& avSize, int alIdx, eLuxJournalState aState, iFontData *apFont, eFontAlign aFontAlign)
{
	if(apFont==NULL) apFont = mpFontMenu;

	apLabel->SetDefaultFontColor(cColor(1,1));
	apLabel->SetDefaultFontSize(avSize);
	apLabel->SetTextAlign(aFontAlign);
	apLabel->SetUserValue(alIdx);
	apLabel->SetDefaultFontType(apFont);
	AddSessionWidget(aState, apLabel);

	cLuxJournalTextData *pData = hplNew(cLuxJournalTextData, (apLabel, aState));
	apLabel->SetUserData(pData);
	mlstSessionWidgetData.push_back(pData);
}

//-----------------------------------------------------------------------

void cLuxJournal::SetupImage(cWidgetImage *apImage, int alIdx, eLuxJournalState aState)
{
	apImage->SetUserValue(alIdx);
	AddSessionWidget(aState, apImage);

	cLuxJournalTextData *pData = hplNew(cLuxJournalTextData, (apImage, aState));
	apImage->SetUserData(pData);
	mlstSessionWidgetData.push_back(pData);
}

//-----------------------------------------------------------------------

void cLuxJournal::SetupNavigationWidgets(eLuxJournalState aState, int alListIndex, int alForwardIndex, int alBackwardIndex, cWidgetDummy *apRoot)
{
	cVector2f vSize(65,46);
	cVector3f vPos = cVector3f(400 + mfNoteTextWidth/2 - vSize.x-80, mfNoteTextStartY + ((float)mlNoteMaxPageRows+1) * mfNoteRowDist  , 3);

	iWidget* pFwd = NULL;
	iWidget* pRet = NULL;
	iWidget* pBck = NULL;

	//Forward
	mpImageForward[aState] = mpGuiSet->CreateWidgetImage("journal/note_forward.tga",vPos,vSize,eGuiMaterial_Alpha,false,apRoot);
	mpImageForward[aState]->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(ImageButtonOnDraw));
	mpImageForward[aState]->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(NoteArrowClick));
	SetupImage(mpImageForward[aState], alForwardIndex, aState);
	pFwd = mpImageForward[aState];

	//Return
	if(aState == eLuxJournalState_Diaries || aState == eLuxJournalState_Notes || aState == eLuxJournalState_QuestLog)
	{
		cWidgetLabel* pLabel = mpGuiSet->CreateWidgetLabel(cVector3f(350,mfBackTextY,3) , cVector2f(100, 24),kTranslate("Journal","MainBack"), apRoot);
		SetupLabel(pLabel,mvBackTextFontSize, aState, aState, mpFontMenu);
		pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(NoteBackClick));
		pLabel->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(ListTextOnDraw));
		pLabel->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(UIListenerJournalPress));

		pRet = pLabel;
	}
	else
	{
		cWidgetImage* pImage = mpGuiSet->CreateWidgetImage("journal/note_return.tga",0,-1,eGuiMaterial_Alpha,false,apRoot);

		if(aState != eLuxJournalState_OpenNarratedDiary)
		{
			pImage->SetPosition(cVector3f(400 - pImage->GetSize().x/2.0f, mfBackTextY, 3));
			pImage->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(ImageButtonOnDraw));
			pImage->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(NoteBackClick));
			pImage->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(UIListenerJournalPress));
			SetupImage(pImage, aState, aState);
		}
		else
		{
			pImage->SetVisible(false);
		}

		pRet = pImage;
	}

	pRet->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(NoteBackUIButtonPress));
	pRet->SetGlobalUIInputListener(true);
	mpWidgetReturn[aState] = pRet;


	//Backward
	vPos.x = 400 - mfNoteTextWidth/2 + 80;
	mpImageBackward[aState] = mpGuiSet->CreateWidgetImage("journal/note_backward.tga",vPos,vSize,eGuiMaterial_Alpha,false,apRoot);
	mpImageBackward[aState]->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(ImageButtonOnDraw));
	mpImageBackward[aState]->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(NoteArrowClick));
	SetupImage(mpImageBackward[aState], alBackwardIndex, aState);

	pBck = mpImageBackward[aState];


	cWidgetDummy* pUIListener = mpGuiSet->CreateWidgetDummy(0, apRoot);
	pUIListener->SetUserValue(aState);
	pUIListener->SetGlobalUIInputListener(true);
	pUIListener->AddCallback(eGuiMessage_UIArrowPress, this, kGuiCallback(UIListenerArrowPress));

	////////////////////////////////////////////////////
	// Set up focus navigation
	{
		/*
		pBck->SetFocusNavigation(eUIArrow_Right, pRet);
		pRet->SetFocusNavigation(eUIArrow_Left, pBck);
		pRet->SetFocusNavigation(eUIArrow_Right, pFwd);
		pFwd->SetFocusNavigation(eUIArrow_Left, pRet);

		if(alListIndex!=-1)
		{
			for(size_t i=0; i<mvNoteListPages[alListIndex].size(); ++i)
			{
				cLuxJournal_ListPage *pListPage = &mvNoteListPages[alListIndex][i];
				cLuxJournal_ListEntry* pLastEntry = &pListPage->mvEntries.back();
				
				iWidget* pLabel = pLastEntry->mlstWidgets.front();
		
				pLabel->SetFocusNavigation(eUIArrow_Down, pRet);
			}
		}
		*/
	}
}

//-----------------------------------------------------------------------

void cLuxJournal::AddSessionWidget(eLuxJournalState aState, iWidget *apWidget)
{
	mvStateData[aState]->mlstSessionWidgets.push_back(apWidget);
}

//-----------------------------------------------------------------------

void cLuxJournal::ResetSessionVars()
{
	SetStateBackgroundGfx("");
	mCurrentState = eLuxJournalState_Main;

    mlCurrentNotePage =0;
	
	for(size_t i=0; i<mvStateData.size(); ++i)
	{
		mvStateData[i]->Reset();
	}
}

//-----------------------------------------------------------------------

void cLuxJournal::SetStateBackgroundGfx(const tString& asFile)
{
	if(mpStateBackgroundGfx)
	{
		mpGui->DestroyGfx(mpStateBackgroundGfx);
		mpStateBackgroundGfx = NULL;
	}
	
	if(asFile != "")
		mpStateBackgroundGfx = mpGui->CreateGfxTexture(asFile,eGuiMaterial_Alpha, eTextureType_Rect);
}

//-----------------------------------------------------------------------

void cLuxJournal::LoadText(const tWString &asName, const tWString &asText)
{
	msHeader = asName;

	mvPages.clear();
	
	tWStringVec sTempRows;
	mpFontDefault->GetWordWrapRows(mfNoteTextWidth, 20, 20,asText, &sTempRows);

	int lRowCount =0;
	///////////////////////////////
	// Iterate rows
	for(size_t i=0; i<sTempRows.size(); ++i)
	{
		//Insert empty row for voice tags
		if(cString::SubW(sTempRows[i],0,6)  == _W("[voice"))
		{
			if(mvPages.empty()==false)
			{
				mvPages.back().mvRows.push_back(_W(""));
				lRowCount++;
			}
			continue;
		}
		///////////////////////////////
		// If first row, max row num is reach or a new page tag found, then make new page and push all coming rows there.
		if(mvPages.empty() || lRowCount >= mlNoteMaxPageRows || sTempRows[i] == _W("[new_page]") )
		{
			mvPages.push_back(cLuxNotePage());
			lRowCount =0;
			if(sTempRows[i] == _W("[new_page]")) continue;
		}
		
		mvPages.back().mvRows.push_back(sTempRows[i]);
        lRowCount++;
	}

	SetNotePage(0);
}

//-----------------------------------------------------------------------

void cLuxJournal::LoadNarrationText(const tWString &asName ,const tWString &asText)
{
	msHeader = asName;

	mvPages.clear();

	tWStringVec sDraftRows;
	mpFontDefault->GetWordWrapRows(mfNoteTextWidth, 20, 20,asText, &sDraftRows);

    tWStringVec sTempRows;
	/////////////////////////////////
	//Make sure command rows (begining with [ is on the same line!)
	for(size_t i=0; i<sDraftRows.size(); ++i)
	{
		if(sDraftRows[i].length()==0)
		{
			sTempRows.push_back(sDraftRows[i]);
		}
		else if(sDraftRows[i][0] == _W('[') && sDraftRows[i][sDraftRows[i].length()-1] != _W(']') )
		{
			sTempRows.push_back(sDraftRows[i]);
			++i;
			if(i<sDraftRows.size()) sTempRows.back() += sDraftRows[i];
		}
		else
		{
			sTempRows.push_back(sDraftRows[i]);
		}
	}


	int lRowCount =0;
	///////////////////////////////
	// Iterate rows
	for(size_t i=0; i<sTempRows.size(); ++i)
	{
		bool bIsVoiceRow = false;
		if(sTempRows[i].length()>6 && cString::SubW(sTempRows[i],0,6)  == _W("[voice"))
		{
			bIsVoiceRow = true;
		}
		
		//Skip new page
		if(sTempRows[i] == _W("[new_page]")) continue;
		
		////////////////////////////////////////
		// If first page or a voice row, add all coming lines to new page.
		if(	mvPages.empty() || bIsVoiceRow)
		{
			mvPages.push_back(cLuxNotePage());
			lRowCount =0;
			
			////////////////////////////////////////
			// Get the filename and add to page.
			if(bIsVoiceRow)
			{
				tWString sSep = _W(" []");//Get voice and filename as seperate strings.
				tWStringVec vArgs;
				cString::GetStringVecW(sTempRows[i],vArgs, &sSep);
				
                if(vArgs.size()!=2)
				{
					Error("Voice lines '%s' has bad syntax! Found %d arguments!\n",cString::To8Char(sTempRows[i]).c_str(),vArgs.size());
				}
				else
				{
					mvPages.back().msVoice = cString::To8Char(vArgs[1]);
				}

				continue;
			}
		}

		mvPages.back().mvRows.push_back(sTempRows[i]);
		lRowCount++;
	}

	/*for(size_t i=0; i<mvPages.size(); ++i)
	{
		Log("Page %d: '%s'\n", i, mvPages[i].msVoice.c_str());
		for(size_t j=0; j<mvPages[i].mvRows.size(); ++j)
		{
			Log(" '%s'\n", cString::To8Char(mvPages[i].mvRows[j]).c_str());
		}
	}*/
	
	SetNotePage(0);
}

//-----------------------------------------------------------------------

void cLuxJournal::SetNotePage(int alPageNum)
{
	mlCurrentNotePage = alPageNum;

	///////////////////////////////
	//No arrows for narrated diary.
	//Play voice instead!
	if(mCurrentState == eLuxJournalState_OpenNarratedDiary)
	{
		if(alPageNum >=0)
		{
			cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
			if(mpVoiceEntry && pSoundHandler->IsValid(mpVoiceEntry, mlVoiceEntryID))
			{
				mpVoiceEntry->FadeOut(3.0f);
			}

			mpVoiceEntry = pSoundHandler->PlayGuiStream(mvPages[alPageNum].msVoice,false, 1.0f);
			if(mpVoiceEntry)
				mlVoiceEntryID = mpVoiceEntry->GetId();
		}

		return;
	}

	///////////////////////////////
	//Set up arrows
	int lX =  mCurrentState;
	if(mlCurrentNotePage == 0){
		mpImageBackward[lX]->SetEnabled(false);
		mpImageBackward[lX]->SetVisible(false);
	}
	else {
		mpImageBackward[lX]->SetEnabled(true);
		mpImageBackward[lX]->SetVisible(true);
	}

	if(mlCurrentNotePage >= (int)mvPages.size()-1){
		mpImageForward[lX]->SetEnabled(false);
		mpImageForward[lX]->SetVisible(false);
	}
	else {
		mpImageForward[lX]->SetEnabled(true);
		mpImageForward[lX]->SetVisible(true);
	}
}

//-----------------------------------------------------------------------

int cLuxJournal::GetNoteListIndex(eLuxJournalState aState)
{
	if(aState == eLuxJournalState_Notes)		return 0;
	else if(aState == eLuxJournalState_Diaries)	return 1;
	else										return 2;
}

//-----------------------------------------------------------------------

void cLuxJournal::SetNoteListPage(int alPageNum, eLuxJournalState aState)
{
	int lIdx = GetNoteListIndex(aState);

	mlCurrentNoteListPage[lIdx] = alPageNum;


	std::vector<cLuxJournal_ListPage> &vPages = mvNoteListPages[lIdx];
	if(mlCurrentNoteListPage[lIdx] >= vPages.size())
	{
		//No pages, disable both arrows.
		mpImageBackward[aState]->SetEnabled(false);
		mpImageBackward[aState]->SetVisible(false);

		mpImageForward[aState]->SetEnabled(false);
		mpImageForward[aState]->SetVisible(false);

		mpWidgetDefaultNav[aState] = mpWidgetReturn[aState];
		
		return;
	}


	///////////////////////////////
	//Set only current page visible
	for(size_t i=0; i<vPages.size(); ++i)
	{
		vPages[i].SetVisible(mlCurrentNoteListPage[lIdx] == i);
	}

	//////////////////////////////////////////////////////////////////
	// Set up last touches in focus navigation
	iWidget* pFirstNote = vPages[mlCurrentNoteListPage[lIdx]].mvEntries.front().mlstWidgets.front();
	iWidget* pLastNote = vPages[mlCurrentNoteListPage[lIdx]].mvEntries.back().mlstWidgets.front();

	if(aState != eLuxJournalState_QuestLog)
		mpWidgetDefaultNav[aState] = pFirstNote;
	else
		mpWidgetDefaultNav[aState] = mpWidgetReturn[aState];

	//mpWidgetReturn[aState]->SetFocusNavigation(eUIArrow_Up, pLastNote);
	//mpImageBackward[aState]->SetFocusNavigation(eUIArrow_Up, pLastNote);
	//mpImageForward[aState]->SetFocusNavigation(eUIArrow_Up, pLastNote);

	mpGuiSet->SetDefaultFocusNavWidget(mpWidgetDefaultNav[aState]);
	mpGuiSet->SetFocusedWidget(mpWidgetDefaultNav[aState]);
	
	///////////////////////////////
	//Set up arrows
	if(mlCurrentNoteListPage[lIdx] == 0){
		mpImageBackward[aState]->SetEnabled(false);
		mpImageBackward[aState]->SetVisible(false);
	}
	else {
		mpImageBackward[aState]->SetEnabled(true);
		mpImageBackward[aState]->SetVisible(true);
	}

	if(mlCurrentNoteListPage[lIdx] >= (int)vPages.size()-1){
		mpImageForward[aState]->SetEnabled(false);
		mpImageForward[aState]->SetVisible(false);
	}
	else {
		mpImageForward[aState]->SetEnabled(true);
		mpImageForward[aState]->SetVisible(true);
	}
}



//-----------------------------------------------------------------------

void cLuxJournal::CreateGui()
{
	CreateMainGui();
	CreateNotesGui();
	CreateDiariesGui();
	CreateOpenNoteGui();
	CreateQuestNotesGui();

	ChangeState(eLuxJournalState_Main);
}

//-----------------------------------------------------------------------

void cLuxJournal::DestroyGui()
{
	for(size_t i=0; i<mvStateData.size(); ++i)
	{
		mvStateData[i]->DestroySessionWidgets();
	}
    
	STLDeleteAll(mlstSessionWidgetData);

	SetStateBackgroundGfx("");
}

//-----------------------------------------------------------------------

void cLuxJournal::CreateMainGui()
{
	cWidgetDummy *pState = mvStateData[eLuxJournalState_Main]->mpRootWidget;
	
	/////////////////////////
	// State select
	cVector3f vStartPos = cVector3f(400, 240, 1);
	cWidgetLabel *pLabel = NULL;
	float fSize = 30;

	std::vector<iWidget*> vLabels;

	//Notes
	pLabel = mpGuiSet->CreateWidgetLabel(cVector3f(300, 200, 3),cVector2f(200, 25),kTranslate("Journal","Notes"), pState);
	SetupLabel(pLabel,fSize,0, eLuxJournalState_Main);
	pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(MainMenuTextClick));
	pLabel->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(MainMenuTextOnDraw));
	vLabels.push_back(pLabel);

	mpWidgetDefaultNav[eLuxJournalState_Main] = pLabel;
	
	//Diaries
	pLabel = mpGuiSet->CreateWidgetLabel(cVector3f(300, 250, 3),cVector2f(200, 25),kTranslate("Journal","Diaries"), pState);
	SetupLabel(pLabel,fSize,1, eLuxJournalState_Main);
	pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(MainMenuTextClick));
	pLabel->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(MainMenuTextOnDraw));
	vLabels.push_back(pLabel);
	
	//Quest Log
	pLabel = mpGuiSet->CreateWidgetLabel(cVector3f(300, 300, 3),cVector2f(200, 25),kTranslate("Journal","Quest Log"), pState);
	SetupLabel(pLabel,fSize,2, eLuxJournalState_Main);
	pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(MainMenuTextClick));
	pLabel->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(MainMenuTextOnDraw));
	vLabels.push_back(pLabel);

	//Back
	pLabel = mpGuiSet->CreateWidgetLabel(cVector3f(300, mfBackTextY, 3),cVector2f(200, 25),kTranslate("Journal","MainBack"), pState);
	SetupLabel(pLabel,mvBackTextFontSize, 3, eLuxJournalState_Main);
	pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(MainMenuTextClick));
	pLabel->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(MainMenuTextOnDraw));
	vLabels.push_back(pLabel);


	//////////////////////////////////////////////////////////////////////
	// Set up navigation / Label callbacks
	{
		for(size_t i=0; i<vLabels.size(); ++i)
		{
			iWidget* pWidget = vLabels[i];

			pWidget->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(MainMenuTextClick));
			pWidget->AddCallback(eGuiMessage_UIButtonPress,this, kGuiCallback(MainMenuUIButtonPress));
			pWidget->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(MainMenuTextOnDraw));



			int lPrev = i-1;
			int lNext = i+1;
			
			if(lPrev>=0)
				pWidget->SetFocusNavigation(eUIArrow_Up, vLabels[lPrev]);
			if(lNext<vLabels.size())
				pWidget->SetFocusNavigation(eUIArrow_Down,vLabels[lNext]);
		}

		mpWidgetDefaultNav[eLuxJournalState_Main] = vLabels[0];
	}
}

//-----------------------------------------------------------------------


void cLuxJournal::CreateNotesGui()
{
	cWidgetDummy *pRoot = mvStateData[eLuxJournalState_Notes]->mpRootWidget;

	/////////////////////////
	// vars setup
	cVector3f vStartPos = cVector3f(250, 100, 3);
	cWidgetLabel *pLabel = NULL;
	cWidgetImage *pImage = NULL;
	eLuxJournalState state = eLuxJournalState_Notes;
	int lListIdx = 0;


	mvNoteListPages[lListIdx].clear();

	/////////////////////////////
	// Header
	pLabel = mpGuiSet->CreateWidgetLabel(cVector3f(250, mfNoteListHeaderY, 3),cVector2f(300, 32),kTranslate("Journal","Notes"), pRoot);
	SetupLabel(pLabel,28, 0, eLuxJournalState_Notes);


	/////////////////////////////
	// No Notes
	if(mvNotes.empty())
	{
		vStartPos.y = 270;
		pLabel = mpGuiSet->CreateWidgetLabel(vStartPos,cVector2f(300, 24),kTranslate("Journal","NotesEmpty"), pRoot);
		SetupLabel(pLabel,24, 0, eLuxJournalState_Notes,mpFontDefault);
	}
	/////////////////////////////
	// Has notes
	else
	{
		mvNoteListPages[lListIdx].push_back(cLuxJournal_ListPage());
		cLuxJournal_ListPage *pListPage = &mvNoteListPages[lListIdx].back();

		for(size_t i=0; i<mvNotes.size(); ++i)
		{
			cLuxNote *pNote = mvNotes[i];

			//Check if new page is needed!
			if(vStartPos.y > mfMaxNoteListY)
			{
				vStartPos.y = 100;
				mvNoteListPages[lListIdx].push_back(cLuxJournal_ListPage());
				pListPage = &mvNoteListPages[lListIdx].back();
			}
			
			//Add label
			pLabel = mpGuiSet->CreateWidgetLabel(vStartPos,cVector2f(300, 24),kTranslate("Journal",pNote->msNameEntry), pRoot);
			SetupLabel(pLabel,24, (int)i, eLuxJournalState_Notes, mpFontDefault, eFontAlign_Left);
			pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(NoteTextClick));
			pLabel->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(ListTextOnDraw));

			pLabel->AddCallback(eGuiMessage_UIButtonPress,this, kGuiCallback(JournalItemUIButtonPress));

			//Add icon
			pImage = mpGuiSet->CreateWidgetImage(pNote->msIconFile, vStartPos + cVector3f(-25,2,-0.05f), cVector2f(20, 20),eGuiMaterial_Alpha, false, pRoot);
			SetupImage(pImage,(int)i,eLuxJournalState_Notes);

			//Add list entry
			pListPage->mvEntries.push_back(cLuxJournal_ListEntry());
			pListPage->mvEntries.back().AddWidget(pLabel);
			pListPage->mvEntries.back().AddWidget(pImage);
			
			vStartPos.y += 30;
		}
	}

	for(size_t i=0; i<mvNoteListPages[lListIdx].size(); ++i)
	{
		cLuxJournal_ListPage *pListPage = &mvNoteListPages[lListIdx][i];
		for(size_t j=0; j<pListPage->mvEntries.size(); ++j)
		{
			cLuxJournal_ListEntry* pListEntry = &pListPage->mvEntries[j];
			int lPrev = j-1;
			int lNext = j+1;

			iWidget* pLabel = pListEntry->mlstWidgets.front();
			
			if(lPrev>=0)
				pLabel->SetFocusNavigation(eUIArrow_Up, pListPage->mvEntries[lPrev].mlstWidgets.front());
			if(lNext<pListPage->mvEntries.size())
				pLabel->SetFocusNavigation(eUIArrow_Down, pListPage->mvEntries[lNext].mlstWidgets.front());
		}
	}

	////////////////////////
	// Navigation
	SetupNavigationWidgets(state, lListIdx, 2, 3, pRoot);


	////////////////////////
	// Set start page
	mlCurrentNoteListPage[lListIdx] = mvNoteListPages[lListIdx].size()-1;
}

//-----------------------------------------------------------------------

void cLuxJournal::CreateDiariesGui()
{
	cWidgetDummy *pRoot = mvStateData[eLuxJournalState_Diaries]->mpRootWidget;
	
	/////////////////////////
	// vars setup
	cVector3f vStartPos = cVector3f(250, 100, 3);
	cWidgetLabel *pLabel = NULL;
	cWidgetImage *pImage = NULL;
	eLuxJournalState state = eLuxJournalState_Diaries;
	int lListIdx = 1;

	mvNoteListPages[lListIdx].clear();

	/////////////////////////////
	// Header
	pLabel = mpGuiSet->CreateWidgetLabel(cVector3f(250, mfNoteListHeaderY, 3),cVector2f(300, 32),kTranslate("Journal","Diaries"), pRoot);
	SetupLabel(pLabel,28, 0, eLuxJournalState_Diaries);


	/////////////////////////////
	// No Diaries
	if(mvDiaryContainers.empty())
	{
		vStartPos.y = 270;
		pLabel = mpGuiSet->CreateWidgetLabel(vStartPos,cVector2f(300, 24),kTranslate("Journal","DiariesEmpty"), pRoot);
		SetupLabel(pLabel,24, 0, eLuxJournalState_Diaries,mpFontDefault);
	}
	/////////////////////////////
	// Has Diaries
	else
	{
		mvNoteListPages[lListIdx].push_back(cLuxJournal_ListPage());
		cLuxJournal_ListPage *pListPage = &mvNoteListPages[lListIdx].back();

		for(size_t cont=0; cont<mvDiaryContainers.size(); ++cont)
		{
			cLuxDiaryContainer *pCont = mvDiaryContainers[cont];

			for(size_t i=0; i<pCont->mvDiaries.size(); ++i)
			{
				cLuxDiary *pDiary = pCont->mvDiaries[i];

				//Check if new page is needed!
				if(vStartPos.y > mfMaxNoteListY)
				{
					vStartPos.y = 100;
					mvNoteListPages[lListIdx].push_back(cLuxJournal_ListPage());
					pListPage = &mvNoteListPages[lListIdx].back();
				}

				pLabel = mpGuiSet->CreateWidgetLabel(vStartPos,cVector2f(300, 24),kTranslate("Journal",pDiary->msNameEntry), pRoot);
				SetupLabel(pLabel,24, (int)i, eLuxJournalState_Diaries, mpFontDefault, eFontAlign_Left);
				pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(DiaryTextClick));

				pLabel->AddCallback(eGuiMessage_UIButtonPress,this, kGuiCallback(JournalItemUIButtonPress));

				pLabel->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(ListTextOnDraw));

				cLuxJournalTextData* pData =  (cLuxJournalTextData*)pLabel->GetUserData();
				pData->mpExtraData = pDiary;

				pImage = mpGuiSet->CreateWidgetImage(pDiary->msIconFile, vStartPos - cVector3f(23,0,1), cVector2f(20, 20),eGuiMaterial_Alpha, false, pRoot);
				SetupImage(pImage,(int)i,eLuxJournalState_Diaries);

				//Add list entry
				pListPage->mvEntries.push_back(cLuxJournal_ListEntry());
				pListPage->mvEntries.back().AddWidget(pLabel);
				pListPage->mvEntries.back().AddWidget(pImage);

				vStartPos.y += 30;
			}
		}
	}

	for(size_t i=0; i<mvNoteListPages[lListIdx].size(); ++i)
	{
		cLuxJournal_ListPage *pListPage = &mvNoteListPages[lListIdx][i];
		for(size_t j=0; j<pListPage->mvEntries.size(); ++j)
		{
			cLuxJournal_ListEntry* pListEntry = &pListPage->mvEntries[j];
			int lPrev = j-1;
			int lNext = j+1;

			iWidget* pLabel = pListEntry->mlstWidgets.front();
			
			if(lPrev>=0)
				pLabel->SetFocusNavigation(eUIArrow_Up, pListPage->mvEntries[lPrev].mlstWidgets.front());
			if(lNext<pListPage->mvEntries.size())
				pLabel->SetFocusNavigation(eUIArrow_Down, pListPage->mvEntries[lNext].mlstWidgets.front());
		}
	}

	////////////////////////
	// Navigation
	SetupNavigationWidgets(state, lListIdx, 2, 3, pRoot);


	////////////////////////
	// Set start page
	mlCurrentNoteListPage[lListIdx] = mvNoteListPages[lListIdx].size()-1;
}

//-----------------------------------------------------------------------

void cLuxJournal::CreateQuestNotesGui()
{
	cWidgetDummy *pRoot = mvStateData[eLuxJournalState_QuestLog]->mpRootWidget;

	/////////////////////////
	// vars setup
	cVector3f vStartPos = cVector3f(150, 100, 3);
	cWidgetLabel *pLabel = NULL;
	eLuxJournalState state = eLuxJournalState_QuestLog;
	int lListIdx = 2;

	mvNoteListPages[lListIdx].clear();

	/////////////////////////////
	// Header
	pLabel = mpGuiSet->CreateWidgetLabel(cVector3f(250, mfNoteListHeaderY, 3),cVector2f(300, 32),kTranslate("Journal","Quest Log"), pRoot);
	SetupLabel(pLabel,28, 0, eLuxJournalState_QuestLog);


	/////////////////////////////
	// Check if there are any active quests
	bool bHasActiveQuests = false;
	for(size_t i=0; i<mvQuestNotes.size(); ++i)
	{
		cLuxQuestNote *pQuest = mvQuestNotes[i];
		if(pQuest->mbActive)
		{
			bHasActiveQuests = true;
			break;
		}
	}

	/////////////////////////////
	// No Quests
	if(bHasActiveQuests==false)
	{
		vStartPos.y = 270;
		pLabel = mpGuiSet->CreateWidgetLabel(vStartPos,cVector2f(300, 24),kTranslate("Journal","QuestsEmpty"), pRoot);
		SetupLabel(pLabel,24, 0, eLuxJournalState_QuestLog,mpFontDefault);
	}
	/////////////////////////////
	// Has Quests
	else
	{
		mvNoteListPages[lListIdx].push_back(cLuxJournal_ListPage());
		cLuxJournal_ListPage *pListPage = &mvNoteListPages[lListIdx].back();

		for(int i=(int)mvQuestNotes.size()-1; i>=0; --i)
		{
			cLuxQuestNote *pQuest = mvQuestNotes[i];
			if(pQuest->mbActive==false) continue;

			tWStringVec vRows;
			mpFontDefault->GetWordWrapRows(500, 21, 19, kTranslate("Journal",pQuest->msTextEntry),&vRows);

			//Check if new page is needed! make sure to check so all rows fit
			if(vStartPos.y > 200 && (vStartPos.y + (vRows.size()-1)*21 ) > mfMaxNoteListY)
			{
				vStartPos.y = 100;
				mvNoteListPages[lListIdx].push_back(cLuxJournal_ListPage());
				pListPage = &mvNoteListPages[lListIdx].back();
			}

			//Add list entry
			pListPage->mvEntries.push_back(cLuxJournal_ListEntry());
			
			//Start sign ("-")
			pLabel = mpGuiSet->CreateWidgetLabel(vStartPos - cVector3f(15,0,1),cVector2f(12, 24),_W("-"), pRoot);
			SetupLabel(pLabel,19, (int)i, eLuxJournalState_QuestLog, mpFontDefault, eFontAlign_Left);
			pListPage->mvEntries.back().AddWidget(pLabel);

			//Text
			for(size_t j=0; j<vRows.size(); ++j)
			{
				pLabel = mpGuiSet->CreateWidgetLabel(vStartPos,cVector2f(500, 24),vRows[j], pRoot);
				SetupLabel(pLabel,19, (int)i, eLuxJournalState_QuestLog, mpFontDefault, eFontAlign_Left);
				pListPage->mvEntries.back().AddWidget(pLabel);
				vStartPos.y += 21;
			}

									
			vStartPos.y += 30;
		}
	}

	////////////////////////
	// Navigation
	SetupNavigationWidgets(state, lListIdx, 2, 3, pRoot);


	////////////////////////
	// Set start page
	mlCurrentNoteListPage[lListIdx] = mvNoteListPages[lListIdx].size()-1;
}

//-----------------------------------------------------------------------

void cLuxJournal::CreateOpenNoteGui()
{
	////////////////////////////////
	// Diary and note
	for(int i=0; i<3; ++i)
	{
		eLuxJournalState state = i==0 ? eLuxJournalState_OpenNote : (i == 1 ? eLuxJournalState_OpenDiary : eLuxJournalState_OpenNarratedDiary);
		
		cWidgetDummy *pRoot = mvStateData[state]->mpRootWidget;
		
		cWidgetLabel *pImage = NULL;
		cWidgetLabel *pLabel = NULL;

		////////////////////////
		// Navigation
		SetupNavigationWidgets(state, -1, 0,1, pRoot);

		mpWidgetDefaultNav[state] = mpWidgetReturn[state];
	}

	////////////////////////////////
	// Narrated note
	cWidgetDummy *pRoot = mvStateData[eLuxJournalState_OpenNarratedDiary]->mpRootWidget;
	
	cWidgetFrame *pClickFrame = mpGuiSet->CreateWidgetFrame(mvGuiSetStartPos, mvGuiSetSize, false, pRoot);
	//pClickFrame->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(NoteClickFrameClick));
	pClickFrame->AddCallback(eGuiMessage_UIButtonPress,this, kGuiCallback(NoteClickFrameClick));
	pClickFrame->SetGlobalUIInputListener(true);

	mpImageBackward[eLuxJournalState_OpenNarratedDiary]->SetVisible(false);
	mpImageBackward[eLuxJournalState_OpenNarratedDiary]->SetEnabled(false);
	mpImageForward[eLuxJournalState_OpenNarratedDiary]->SetVisible(false);;
	mpImageForward[eLuxJournalState_OpenNarratedDiary]->SetVisible(false);;
}

//-----------------------------------------------------------------------

void cLuxJournal::CreateBackground()
{
	CreateScreenTextures();
	RenderBackgroundImage();
}

//-----------------------------------------------------------------------

void cLuxJournal::CreateScreenTextures()
{
	iLowLevelGraphics *pLowGfx = mpGraphics->GetLowLevel();
	cVector3l vTexSize = pLowGfx->GetScreenSizeInt();
	vTexSize.z = 0;

	mpScreenTexture = mpGraphics->CreateTexture("Screen",eTextureType_Rect,eTextureUsage_RenderTarget);
	mpScreenTexture->CreateFromRawData(vTexSize,ePixelFormat_RGBA,NULL);
	mpScreenTexture->SetWrapSTR(eTextureWrap_ClampToEdge);

	mpScreenBgTexture = mpGraphics->CreateTexture("ScreenBlur",eTextureType_Rect,eTextureUsage_RenderTarget);
	mpScreenBgTexture->CreateFromRawData(vTexSize,ePixelFormat_RGBA,NULL);

	mpScreenGfx = mpGui->CreateGfxTexture(mpScreenTexture,false,eGuiMaterial_Diffuse);
	mpScreenBgGfx = mpGui->CreateGfxTexture(mpScreenBgTexture,false,eGuiMaterial_Alpha);
}

//-----------------------------------------------------------------------

void cLuxJournal::RenderBackgroundImage()
{
	iLowLevelGraphics *pLowGfx = mpGraphics->GetLowLevel();

	//////////////////////////////
	// Create frame buffers
	iFrameBuffer *pEffectBuffer  = mpGraphics->CreateFrameBuffer("InventoryEffectbuffer");
	pEffectBuffer->SetTexture2D(0,mpScreenBgTexture);
	pEffectBuffer->CompileAndValidate();

	//////////////////////////////
	// Render

	//Render scene again without gui.
	gpBase->mpHelpFuncs->RenderBackgroundScreen(false);


	//Set up main states
	pLowGfx->SetBlendActive(false);
	pLowGfx->SetDepthTestActive(false);
	pLowGfx->SetDepthWriteActive(false);

	pLowGfx->SetOrthoProjection(mvScreenSize,-1000,1000);
	pLowGfx->SetIdentityMatrix(eMatrix_ModelView);

	//Copy screen to screen texture
	pLowGfx->CopyFrameBufferToTexure(mpScreenTexture,0,pLowGfx->GetScreenSizeInt(),0);

	//Bind shader and draw
	mpEffectProgram->Bind();
	pLowGfx->SetCurrentFrameBuffer(pEffectBuffer);

	pLowGfx->SetTexture(0,mpScreenTexture);

	pLowGfx->DrawQuad(0,mvScreenSize,cVector2f(0, mvScreenSize.y),cVector2f(mvScreenSize.x,0),cColor(1,1));
	mpEffectProgram->UnBind();

	//Copy a copy of the full gui with all HUD!
	pLowGfx->SetCurrentFrameBuffer(NULL);
	pLowGfx->SetTexture(0,NULL);

	gpBase->mpHelpFuncs->RenderBackgroundScreen(true);
	pLowGfx->CopyFrameBufferToTexure(mpScreenTexture,0,pLowGfx->GetScreenSizeInt(),0);



	///////////////////////
	// Exit

	//Render states
	pLowGfx->SetTexture(0,NULL);
	pLowGfx->SetCurrentFrameBuffer(NULL);
	pLowGfx->SetDepthTestActive(true);

	//Flush the rendering
	pLowGfx->FlushRendering();
	pLowGfx->WaitAndFinishRendering();

	//Destroy data
	mpGraphics->DestroyFrameBuffer(pEffectBuffer);
}	

//-----------------------------------------------------------------------

void cLuxJournal::DestroyBackground()
{
	if(mpScreenGfx) mpGui->DestroyGfx(mpScreenGfx);
	if(mpScreenTexture) mpGraphics->DestroyTexture(mpScreenTexture);
	if(mpScreenBgGfx) mpGui->DestroyGfx(mpScreenBgGfx);
	if(mpScreenBgTexture) mpGraphics->DestroyTexture(mpScreenBgTexture);

	mpScreenGfx = NULL;
	mpScreenTexture = NULL;
	mpScreenBgTexture = NULL;
	mpScreenBgGfx = NULL;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CALLBACK METHODS
//////////////////////////////////////////////////////////////////////////


bool cLuxJournal::MainMenuTextClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	//Notes
	if(apWidget->GetUserValue()==0)
	{
		ChangeState(eLuxJournalState_Notes);
		mpWidgetDefaultNav[eLuxJournalState_Main] = apWidget;
	}
	//Diaries
	else if(apWidget->GetUserValue()==1)
	{
		ChangeState(eLuxJournalState_Diaries);
		mpWidgetDefaultNav[eLuxJournalState_Main] = apWidget;
	}
	//Quest Log
	else if(apWidget->GetUserValue()==2)
	{
		ChangeState(eLuxJournalState_QuestLog);
		mpWidgetDefaultNav[eLuxJournalState_Main] = apWidget;
	}
	//Back
	else if(apWidget->GetUserValue()==3)
	{
		ExitPressed(false);
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, MainMenuTextClick);

//-----------------------------------------------------------------------

bool cLuxJournal::MainMenuUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==eUIButton_Secondary)
	{
		if(UIListenerButtonPress(apWidget, aData) == false)
		{
			return NoteBackClick(apWidget, cGuiMessageData(eGuiMouseButton_Left));
		}

		return true;
	}
	
	if(aData.mlVal==eUIButton_Primary)
		return MainMenuTextClick(apWidget, aData);

	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, MainMenuUIButtonPress);

//-----------------------------------------------------------------------

bool cLuxJournal::MainMenuTextOnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	cLuxJournalTextData *pData = (cLuxJournalTextData*)apWidget->GetUserData();

	if(pData->mfEffectfAlpha >0)
	{
		float fT = 0.3f + (sin(mfMouseOverPulse*2.5f)+1)*0.7f*0.5f;
		float fXAdd = 1 + (1-fT)*5.0f;

		cVector3f vPos = apWidget->GetGlobalPosition() - cVector3f(fXAdd/2,2,1);
		vPos.x += apWidget->GetSize().x/2;

		cVector2f vSize = apWidget->GetDefaultFontSize() + cVector2f(fXAdd,4);

		float fAlpha = apWidget->GetColorMul().a*fT *pData->mfEffectfAlpha;
		mpGuiSet->DrawFont(	apWidget->GetText(), mpFontMenu, vPos, vSize, cColor(1,0,0, fAlpha),
							eFontAlign_Center);
	}
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, MainMenuTextOnDraw);

//-----------------------------------------------------------------------

bool cLuxJournal::ListTextOnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	cLuxJournalTextData *pData = (cLuxJournalTextData*)apWidget->GetUserData();
	cWidgetLabel *pLabel = static_cast<cWidgetLabel*>(apWidget);

	if(pData->mfEffectfAlpha >0)
	{
		float fT = 0.3f + (sin(mfMouseOverPulse*2.5f)+1)*0.7f*0.5f;
		float fXAdd = 1 + (1-fT)*5.0f;

		cVector2f vSize = apWidget->GetDefaultFontSize() + cVector2f(fXAdd,4);
		float fAlpha = apWidget->GetColorMul().a*fT *pData->mfEffectfAlpha;
		
		//////////////////////
		//Centered text
		if(pLabel->GetTextAlign() == eFontAlign_Center)
		{
			cVector3f vPos = apWidget->GetGlobalPosition() - cVector3f(fXAdd/2,2,1);
			vPos.x += apWidget->GetSize().x/2;
			
			mpGuiSet->DrawFont(	apWidget->GetText(), apWidget->GetDefaultFontType(), vPos, vSize, cColor(1,0,0, fAlpha), eFontAlign_Center);
		}
		//////////////////////
		//Left Aligned
		else if(pLabel->GetTextAlign() == eFontAlign_Left)
		{
			float fSizeAdd =	apWidget->GetDefaultFontType()->GetLength(vSize, apWidget->GetText().c_str()) - 
								apWidget->GetDefaultFontType()->GetLength(apWidget->GetDefaultFontSize(), apWidget->GetText().c_str());

			cVector3f vPos = apWidget->GetGlobalPosition() - cVector3f(fSizeAdd/2,2,1);

			mpGuiSet->DrawFont(	apWidget->GetText(), apWidget->GetDefaultFontType(), vPos, vSize, cColor(1,0,0, fAlpha), eFontAlign_Left);
		}

		
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, ListTextOnDraw);

//-----------------------------------------------------------------------

bool cLuxJournal::NoteTextClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	OpenNote(mvNotes[apWidget->GetUserValue()], false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, NoteTextClick);

//-----------------------------------------------------------------------

bool cLuxJournal::NoteBackClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	//if(UIListenerButtonPress(apWidget, aData))
	//{
	//	return true;
	//}

	if(aData.mlVal != eUIButton_Primary && aData.mlVal != eUIButton_Secondary)
		return true;

	ExitPressed(false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, NoteBackClick);

//-----------------------------------------------------------------------

bool cLuxJournal::UIListenerJournalPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal == eUIButton_Primary)
		UIListenerButtonPress(apWidget, aData);

	else if (aData.mlVal == eUIButton_Secondary)
		ExitPressed(false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, UIListenerJournalPress);

//-----------------------------------------------------------------------

bool cLuxJournal::DiaryTextClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	cLuxJournalTextData* pData = (cLuxJournalTextData*)apWidget->GetUserData();

	OpenDiary((cLuxDiary*)pData->mpExtraData, false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, DiaryTextClick);

//-----------------------------------------------------------------------

bool cLuxJournal::ImageButtonOnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	cLuxJournalTextData *pData = (cLuxJournalTextData*)apWidget->GetUserData();
	cWidgetImage *pImage = static_cast<cWidgetImage*>(apWidget);
	
	if(pData->mfEffectfAlpha >0)
	{
		/*float fT = 0.3f + (sin(mfMouseOverPulse*2.5f)+1)*0.7f*0.5f;
		float fAdd = 1 + (1-fT)*16.0f;

		cVector3f vPos = pImage->GetGlobalPosition() - cVector3f(fAdd/2,fAdd/2,1);
		
		cVector2f vSize = pImage->GetImage()->GetActiveSize() + cVector2f(fAdd,fAdd);

		float fAlpha = apWidget->GetColorMul().a*fT *pData->mfEffectfAlpha;
		mpGuiSet->DrawGfx(	pImage->GetImage(), vPos, vSize, cColor(1,0,0, fAlpha));*/
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, ImageButtonOnDraw);

//-----------------------------------------------------------------------

bool cLuxJournal::NoteArrowClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	///////////////////////////7
	// Open Note

	//Forward
	if(apWidget->GetUserValue()==0)		
	{
		SetNotePage(mlCurrentNotePage+1);
	}
	//Backward
	else if(apWidget->GetUserValue()==1)
	{
		SetNotePage(mlCurrentNotePage-1);
	}
	///////////////////////////7
	// Note List

	//Forward
	else if(apWidget->GetUserValue()==2) 
	{
		SetNoteListPage(mlCurrentNoteListPage[GetNoteListIndex(mCurrentState)]+1, mCurrentState);
	}
	//Backward
	else if(apWidget->GetUserValue()==3)
	{
		SetNoteListPage(mlCurrentNoteListPage[GetNoteListIndex(mCurrentState)]-1, mCurrentState);
	}

	///////////////////////////7
	// Play sound
	gpBase->mpHelpFuncs->PlayGuiSoundData("journal_page", eSoundEntryType_Gui);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, NoteArrowClick);

//-----------------------------------------------------------------------

bool cLuxJournal::NoteClickFrameClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	ExitPressed(false);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, NoteClickFrameClick);

//-----------------------------------------------------------------------

bool cLuxJournal::JournalItemUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==eUIButton_Primary)
	{
		cLuxJournalTextData* pData = static_cast<cLuxJournalTextData*>(apWidget->GetUserData());
		eLuxJournalState type = pData->mType;

		switch(type)
		{
		case eLuxJournalState_Diaries:
			return DiaryTextClick(apWidget, aData);
		case eLuxJournalState_Notes:
			return NoteTextClick(apWidget, aData);
		}
	}

	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, JournalItemUIButtonPress);

//-----------------------------------------------------------------------

bool cLuxJournal::UIListenerArrowPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lType = apWidget->GetUserValue();

	iWidget* pBack = mpImageBackward[lType];
	iWidget* pForward = mpImageForward[lType];

	bool bLeftAllowed = pBack->IsVisible();
	bool bRightAllowed = pForward->IsVisible();

	
	
	switch(aData.mlVal)
	{
	case eUIArrow_Left:
		if(bLeftAllowed) return NoteArrowClick(pBack, cGuiMessageData(eGuiMouseButton_Left));
		break;
	case eUIArrow_Right:
		if(bRightAllowed) return NoteArrowClick(pForward, cGuiMessageData(eGuiMouseButton_Left));
		break;
	case eUIArrow_Down:
#ifdef USE_GAMEPAD
# if USE_SDL2
		if(gpBase->mpInputHandler->GetGamepad() && gpBase->mpInputHandler->GetGamepad()->ButtonIsDown(eGamepadButton_DpadDown) == false) break;
# else
		if(gpBase->mpInputHandler->GetGamepad() && gpBase->mpInputHandler->GetGamepad()->HatIsInState(eGamepadHat_0, eGamepadHatState_Down) == false) break;
# endif
#endif
		if(lType == eLuxJournalState_OpenNote
		|| lType == eLuxJournalState_OpenDiary
		|| lType == eLuxJournalState_OpenNarratedDiary)
		{
			ExitPressed(false);
			return true;
		}
		break;
	}
	
	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, UIListenerArrowPress);

//-----------------------------------------------------------------------

bool cLuxJournal::UIListenerButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lType = apWidget->GetUserValue();

	if(!(lType == eLuxJournalState_OpenNote || lType == eLuxJournalState_OpenDiary	|| lType == eLuxJournalState_OpenNote || lType == eLuxJournalState_OpenNarratedDiary)) return false;

	iWidget* pBack = mpImageBackward[lType];
	iWidget* pForward = mpImageForward[lType];

	bool bLeftAllowed = pBack->IsVisible();
	bool bRightAllowed = pForward->IsVisible();

	switch(aData.mlVal)
	{
	case eUIButton_Secondary:
		if(bLeftAllowed) return NoteArrowClick(pBack, cGuiMessageData(eGuiMouseButton_Left));
		else
		{
			ExitPressed(false);
			return true;
		}
	case eUIButton_Primary:
		if(bRightAllowed) return NoteArrowClick(pForward, cGuiMessageData(eGuiMouseButton_Left));
		break;
	}

	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, UIListenerButtonPress);

//-----------------------------------------------------------------------

bool cLuxJournal::NoteBackUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	/*
	if(aData.mlVal==eUIButton_Secondary)
	{
		return NoteBackClick(apWidget, aData);
	}
	*/
	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxJournal, NoteBackUIButtonPress);

//-----------------------------------------------------------------------

