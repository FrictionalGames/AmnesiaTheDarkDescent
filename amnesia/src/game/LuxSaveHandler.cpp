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

#include "LuxSaveHandler.h"

#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxEntity.h"
#include "LuxProp.h"
#include "LuxPlayer.h"
#include "LuxInventory.h"
#include "LuxJournal.h"
#include "LuxMusicHandler.h"
#include "LuxMessageHandler.h"
#include "LuxSavedGameTypes.h"
#include "LuxEffectHandler.h"
#include "LuxGlobalDataHandler.h"
#include "LuxHintHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxInsanityHandler.h"
#include "LuxProgressLogHandler.h"
#include "LuxLoadScreenHandler.h"
#include "LuxSavedGame.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxSaveHandlerThreadClass::cLuxSaveHandlerThreadClass()
{
	mpThread = NULL;
	mpSaveMutex = cPlatform::CreateMutEx();
}

cLuxSaveHandlerThreadClass::~cLuxSaveHandlerThreadClass()
{
	if(IsRunning())
	{
		mpThread->Stop();
		hplDelete(mpThread);
	}
	hplDelete(mpSaveMutex);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxSaveHandlerThreadClass::IsRunning()
{
	return mpThread!=NULL;
}

//-----------------------------------------------------------------------

void cLuxSaveHandlerThreadClass::SetUpThread()
{
	mpThread = cPlatform::CreateThread(this);
	mpThread->SetPriority(eThreadPrio_Low);
	mpThread->SetUpdateRate(1.0f);
	mpThread->Start();
}

//-----------------------------------------------------------------------

void cLuxSaveHandlerThreadClass::Save(cLuxSaveGame_SaveData* apSaveData, const tWString& asFile)
{
	if(apSaveData==NULL)
		return;

	mpSaveMutex->Lock();

	mvSaveData.push_back(apSaveData);
	mvSaveFileNames.push_back(asFile);

	mpSaveMutex->Unlock();
}

//-----------------------------------------------------------------------

void cLuxSaveHandlerThreadClass::ProcessPendingSaves()
{
	std::vector<cLuxSaveGame_SaveData*> vSaveDataCopy;
	std::vector<tWString> vSaveFileNamesCopy;

	mpSaveMutex->Lock();
	if(mvSaveData.empty()==false)
	{
		vSaveDataCopy = mvSaveData;
		vSaveFileNamesCopy = mvSaveFileNames;

		mvSaveData.clear();
		mvSaveFileNames.clear();
	}
	mpSaveMutex->Unlock();

	// Force a wait until all saves are processed
	iMutex* pMutex = gpBase->mpMapHandler->mpSavedGameMutex;
	pMutex->Lock();
	{
		for(int i=0;i<(int)vSaveDataCopy.size();++i)
		{
			cLuxSaveGame_SaveData* pData = vSaveDataCopy[i];
			const tWString& sFile = vSaveFileNamesCopy[i];

			//Need to set saved maps before saving!
			pData->mpSavedMaps = gpBase->mpMapHandler->GetSavedMapCollection();

			cSerializeClass::SaveToFile(pData,sFile,"SaveGame");

			hplDelete(pData);
		}
	}
	pMutex->Unlock();
}

//-----------------------------------------------------------------------

void cLuxSaveHandlerThreadClass::UpdateThread()
{
	ProcessPendingSaves();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxSaveHandler::cLuxSaveHandler() : iLuxUpdateable("LuxSaveHandler")
{
	mbInitialized = false;
	mbStartThread = false;

	mlMaxAutoSaves =  gpBase->mpGameCfg->GetInt("Saving","MaxAutoSaves",20);
	mlSaveNameCount =0;
}

//-----------------------------------------------------------------------

cLuxSaveHandler::~cLuxSaveHandler()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxSaveHandler::OnStart()
{

}

//-----------------------------------------------------------------------

void cLuxSaveHandler::Update(float afTimeStep)
{
}

//-----------------------------------------------------------------------

void cLuxSaveHandler::Reset()
{
	if(mbInitialized==false)
	{
		mbInitialized = true;
		if(mbStartThread) mSaveHandlerThreadClass.SetUpThread();
	}
}

//-----------------------------------------------------------------------

void cLuxSaveHandler::SaveGameToFile(const tWString& asFile, bool abSaveSnapshot)
{
	Log("-------- BEGIN SAVE TO: %s ---------\n", cString::To8Char(asFile).c_str());

	cLuxSaveGame_SaveData* pData = CreateSaveGameData();

	if(mSaveHandlerThreadClass.IsRunning())
		mSaveHandlerThreadClass.Save(pData, asFile);
	else
	{
		pData->mpSavedMaps = gpBase->mpMapHandler->GetSavedMapCollection();
		cSerializeClass::SaveToFile(pData,asFile,"SaveGame");
		hplDelete(pData);
	}


	// Save snapshot?
	if(abSaveSnapshot)
	{
		tWString sFileExt = cString::GetFileExtW(asFile);
		tWString sFileName = cString::SubW(asFile,0, asFile.size()-(sFileExt.size()+1)) +  _W(".jpg");
		
		cEngine *pEngine = gpBase->mpEngine;
		
		cBitmap *pBmp = pEngine->GetGraphics()->GetLowLevel()->CopyFrameBufferToBitmap();
		pEngine->GetResources()->GetBitmapLoaderHandler()->SaveBitmap(pBmp,sFileName,0);
		hplDelete(pBmp);
	}

	Log("-------- END SAVE ---------\n");

}

//-----------------------------------------------------------------------

void cLuxSaveHandler::LoadGameFromFile(const tWString& asFile)
{
	Log("-------- BEGIN LOAD FROM %s ---------\n", cString::To8Char(asFile).c_str());

	cLuxSaveGame_SaveData * pSaveGame = hplNew(cLuxSaveGame_SaveData, ());

	//cSerializeClass::SetLog(true);
	cSerializeClass::LoadFromFile(pSaveGame,asFile);
	//cSerializeClass::SetLog(false);
	
	LoadSaveGameData(pSaveGame);

	hplDelete(pSaveGame);

	Log("-------- END LOAD ---------\n");
}

//-----------------------------------------------------------------------

bool cLuxSaveHandler::AutoSave()
{
	//////////////////////
	// HARDMODE
	if (gpBase->mbHardMode)
		return true;


	DeleteOldestSaveFiles(gpBase->msProfileSavePath, mlMaxAutoSaves);
	
	SaveGameToFile(gpBase->msProfileSavePath+GetSaveName(_W("AutoSave")));

	return true;
}


//-----------------------------------------------------------------------

bool cLuxSaveHandler::AutoLoad(bool abResetProgressLogger)
{
	// Wait until any pending save is done.
	if(mSaveHandlerThreadClass.IsRunning())
		mSaveHandlerThreadClass.ProcessPendingSaves();
	
	//Get newest file (if any!)
	tWString sFile = GetNewestSaveFile(gpBase->msProfileSavePath);
	if(sFile == _W(""))
	{
		Error("Could not find a save file in '%s'!\n", cString::To8Char(gpBase->msProfileSavePath).c_str());
		return false;
	}
	
	////////////////////////////////
	// HARDMODE
	if (gpBase->mbHardMode)
	{
		if (sFile.find(_W("HardMode")) == tWString::npos)
		{
			Error("Could not find a hard mode save file in '%s'!\n", cString::To8Char(gpBase->msProfileSavePath).c_str());
			return false;
		}
	}


	//Reset the progress logger.
	if(abResetProgressLogger)
	{
		gpBase->mpProgressLogHandler->CreateAndResetLogFile();
	}


	LoadGameFromFile(gpBase->msProfileSavePath+sFile);
		
	return true;
}

//-----------------------------------------------------------------------

bool cLuxSaveHandler::SaveFileExists()
{
	if(gpBase->msProfileSavePath==_W("")) return false;

	return GetNewestSaveFile(gpBase->msProfileSavePath)!=_W("");
}

//-----------------------------------------------------------------------

//////////////////////
// HARDMODE
bool cLuxSaveHandler::HardModeSave()
{
	SaveGameToFile(gpBase->msProfileSavePath + GetSaveName(_W("HardMode")));
	return true;
}

//-----------------------------------------------------------------------

cLuxSaveGame_SaveData *cLuxSaveHandler::CreateSaveGameData()
{
	cLuxSaveGame_SaveData *pSave = hplNew(cLuxSaveGame_SaveData, ());

	/////////////////////
	// HARDMODE

	pSave->mbHardmode = gpBase->mbHardMode;

	////////////////////

	pSave->msMapFolder = gpBase->mpMapHandler->GetMapFolder();

	pSave->mInventory.FromInventory(gpBase->mpInventory);
	pSave->mJournal.FromJournal(gpBase->mpJournal);
    pSave->mPlayer.FromPlayer(gpBase->mpPlayer);
	pSave->mMap.FromMap(gpBase->mpMapHandler->GetCurrentMap());
	pSave->mMusicHandler.FromMusicHandler(gpBase->mpMusicHandler);
	pSave->mEffectHandler.FromEffectHandler(gpBase->mpEffectHandler);
	pSave->mGlobalDataHandler.FromGlobalDataHandler(gpBase->mpGlobalDataHandler);
	pSave->mHintHandler.FromHintHandler(gpBase->mpHintHandler);
	pSave->mInsanityHandler.FromInsanityHandler(gpBase->mpInsanityHandler);
	pSave->mLoadScreenHandler.FromLoadScreenHandler(gpBase->mpLoadScreenHandler);

	return pSave;
}

//-----------------------------------------------------------------------

void cLuxSaveHandler::LoadSaveGameData(cLuxSaveGame_SaveData *apSave)
{
	Log("LOADING SAVE GAME!\n");

	cLuxMap *pOldMap = NULL;
	cLuxMap *pCurrentMap = gpBase->mpMapHandler->GetCurrentMap();

	//////////////////////////////////////
	// Draw loading screen
	gpBase->mpLoadScreenHandler->DrawMenuScreen();

	//////////////////////////////////////
	// Destroy cache data
	gpBase->mpMapHandler->DestroyDataCache();

	///////////////////
	// Reset stuff (A better way to do this? Some weak reset that can be broadcasted?)
	if(pCurrentMap)
	{
		gpBase->RunModuleMessage(eLuxUpdateableMessage_DestroyWorldEntities, pCurrentMap);

		gpBase->mpPlayer->Reset();
		gpBase->mpInventory->Reset();
		gpBase->mpJournal->Reset();
		gpBase->mpMusicHandler->Reset();
		gpBase->mpMessageHandler->Reset();
		gpBase->mpEffectHandler->Reset();
		gpBase->mpGlobalDataHandler->Reset();
		gpBase->mpHintHandler->Reset();
		gpBase->mpInsanityHandler->Reset();
		gpBase->mpLoadScreenHandler->Reset();
		gpBase->mpMapHandler->GetSavedMapCollection()->Reset();

		gpBase->mpEngine->GetSound()->GetSoundHandler()->StopAll(eSoundEntryType_World | eSoundEntryType_Gui);
		gpBase->mpEngine->GetSound()->GetMusicHandler()->Stop(100.0f);
	}


	///////////////////
	// Save cache
	cLuxModelCache cache;
	cache.Create();

	///////////////////
	// Setup variables
	tString msOldMapFolder = gpBase->mpMapHandler->GetMapFolder();
    gpBase->mpMapHandler->SetMapFolder(apSave->msMapFolder);

	///////////////////
	// Load script files

	gpBase->mpInventory->LoadScript();
	gpBase->mpGlobalDataHandler->LoadScript();
	
	///////////////////
	// Load new map
	if(	pCurrentMap==NULL || 
		msOldMapFolder != gpBase->mpMapHandler->GetMapFolder() || 
		pCurrentMap->GetFileName() != apSave->mMap.msFileName)
	{
		cLuxMap *pNewMap = gpBase->mpMapHandler->LoadMap(apSave->mMap.msFileName, false);
		if(pNewMap==NULL) FatalError("Could not load quicksave map '%s'\n", apSave->mMap.msFileName.c_str());
	
		// Destroy old and set new
		if(pCurrentMap) gpBase->mpMapHandler->DestroyMap(pCurrentMap, false);
		gpBase->mpMapHandler->SetCurrentMap(pNewMap, false, false,"");

		pCurrentMap = pNewMap;

		pCurrentMap->GetWorld()->DestroyAllSoundEntities();
		pCurrentMap->GetWorld()->DestroyAllParticleSystems();
	}
	/////////////////////
	//Clear current
	else
	{
		pCurrentMap->GetWorld()->DestroyAllSoundEntities();
		pCurrentMap->GetWorld()->DestroyAllParticleSystems();

		pCurrentMap->DestroyAllEntities();
		
		//Destroy all to be destroyed particle systems
		cParticleSystemIterator partIt = pCurrentMap->GetWorld()->GetParticleSystemIterator();
		while(partIt.HasNext())
		{
			cParticleSystem *pPS = partIt.Next();
            if(pPS->IsDying()) pPS->KillInstantly();		
		}

		//TODO: Destroy all special in player (???)
		
		// Create map data
		gpBase->RunModuleMessage(eLuxUpdateableMessage_CreateWorldEntities, pCurrentMap);
	}
	
	///////////////////
	// Load save data
	apSave->mMap.ToMap(pCurrentMap);
	apSave->mHintHandler.ToHintHandler(pCurrentMap, gpBase->mpHintHandler);
	apSave->mPlayer.ToPlayer(pCurrentMap, gpBase->mpPlayer);
	apSave->mInventory.ToInventory(pCurrentMap, gpBase->mpInventory);
	apSave->mJournal.ToJournal(pCurrentMap, gpBase->mpJournal);
	apSave->mMusicHandler.ToMusicHandler(pCurrentMap, gpBase->mpMusicHandler);
	apSave->mEffectHandler.ToEffectHandler(pCurrentMap, gpBase->mpEffectHandler);
	apSave->mGlobalDataHandler.ToGlobalDataHandler(pCurrentMap, gpBase->mpGlobalDataHandler);
	apSave->mInsanityHandler.ToInsanityHandler(pCurrentMap, gpBase->mpInsanityHandler);
	apSave->mLoadScreenHandler.ToLoadScreenHandler(pCurrentMap, gpBase->mpLoadScreenHandler);
							 
	///////////////////
	// Load saved maps
	gpBase->mpMapHandler->SetSavedMapCollection(apSave->mpSavedMaps);
	
	///////////////////
	//Hardmode
	gpBase->mbHardMode = apSave->mbHardmode;

	//////////////////////////////////
	//Clean up
	gpBase->mpHelpFuncs->CleanupData();
	
	///////////////////
	// Destroy cache
	cache.Destroy();

}

//-----------------------------------------------------------------------

tWString cLuxSaveHandler::GetProperSaveName(const tWString &asFile)
{
	tWString sProperName;

	tWStringVec vSaveNameStrings;
	tWString sSeparator = _W("_");
	cString::GetStringVecW(asFile, vSaveNameStrings, &sSeparator);

	if(vSaveNameStrings.size() != 9) return _W("INVALID SAVE FILE SYNTAX");

	// Store prefix
	sProperName = vSaveNameStrings[0];
	sProperName += _W(" ");

	// Store map name
	sProperName += _W("- ") + kTranslate("Levels", cString::To8Char(vSaveNameStrings[1])) + _W(" -") ;
	sProperName += _W(" ");

	// Store date
	sProperName += vSaveNameStrings[4] + _W("/") + vSaveNameStrings[3] + _W("-") + vSaveNameStrings[2];
	sProperName += _W(" ");

	// Store time, adding a padding '0' if only one char
	for(int i=5; i<=7; ++i)
		if(vSaveNameStrings[i].size()==1) vSaveNameStrings[i] = _W("0")+vSaveNameStrings[i];
	sProperName += vSaveNameStrings[5] + _W(":") + vSaveNameStrings[6] + _W(":")+ vSaveNameStrings[7];

	return sProperName;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

tWString cLuxSaveHandler::GetSaveName(const tWString &asPrefix)
{
	cLuxMap *pCurrentMap = gpBase->mpMapHandler->GetCurrentMap();

	tString sMapName = pCurrentMap->GetDisplayNameEntry();
	if(sMapName == "") sMapName = "NULL";

	cDate currentDate = cPlatform::GetDate();
	tWString sFileName =	asPrefix+ _W("_") +
							cString::To16Char(sMapName) + _W("_") +
							cString::ToStringW(currentDate.year)+_W("_") +
							cString::ToStringW(currentDate.month+1)+_W("_") +
							cString::ToStringW(currentDate.month_day)+_W("_") +
							cString::ToStringW(currentDate.hours)+_W("_") +
							cString::ToStringW(currentDate.minutes)+_W("_") +
							cString::ToStringW(currentDate.seconds)+_W("_") +
							cString::ToStringW(mlSaveNameCount)+
							_W(".sav");
	mlSaveNameCount++;
	if(mlSaveNameCount >= 100 || currentDate!=mLatestSaveDate) mlSaveNameCount =0;

	mLatestSaveDate = currentDate;

	return sFileName;
}

void cLuxSaveHandler::DeleteOldestSaveFiles(const tWString &asFolder, int alMax)
{
	//////////////////////////
	// Get save files
	tWStringList lstFiles;
	cPlatform::FindFilesInDir(lstFiles, asFolder, _W("*.sav"));

	//////////////////////////
	// If more than max, delete the oldest. Continue until delete failed or size is below max.
	size_t lLastSize = 0;
	while( (int)lstFiles.size() >= alMax && lLastSize != lstFiles.size())
	{
		cDate oldestDate;
		tWStringListIt oldestIt = lstFiles.end();
		tWString sOldestPath;

		bool bFirst = true;

		//////////////////////////////////////////
		//Iterate the files and look for oldest
		for(tWStringListIt it = lstFiles.begin(); it != lstFiles.end(); ++it)
		{
			tWString sFile = *it;
			tWString sPath = asFolder + sFile;
			cDate fileDate = cPlatform::FileCreationDate(sPath);

			//Log(" '%s' date: %s\n", cString::To8Char(sFile).c_str(), fileDate.ToString().c_str());

			if(bFirst || fileDate < oldestDate)
			{
				bFirst = false;
				oldestDate = fileDate;
				sOldestPath = sPath;
				oldestIt = it;
			}
		}

		//////////////////////////////////////////
		//Delete the oldest file and remove from list.
		//Log("Oldest: '%s'\n", cString::To8Char(sOldestPath).c_str());
		lLastSize = lstFiles.size();
		if(oldestIt != lstFiles.end())
		{
			cPlatform::RemoveFile(sOldestPath);
			lstFiles.erase(oldestIt);
		}
	}

}

tWString cLuxSaveHandler::GetNewestSaveFile(const tWString &asFolder)
{
	//////////////////////////
	// Get save files
	tWStringList lstFiles;
	cPlatform::FindFilesInDir(lstFiles, asFolder, _W("*.sav"));

	if(lstFiles.empty()) return _W("");

	cDate newestDate;
	tWString sNewestFile;
	bool bFirst = true;

	//////////////////////////////////////////
	//Iterate the files and look for newest
	for(tWStringListIt it = lstFiles.begin(); it != lstFiles.end(); ++it)
	{
		tWString sFile = *it;
		tWString sPath = asFolder + sFile;
		cDate fileDate = cPlatform::FileModifiedDate(sPath);

		if(bFirst || fileDate > newestDate)
		{
			bFirst = false;
			newestDate = fileDate;
			sNewestFile = sFile;
		}
	}

	return sNewestFile;
}

