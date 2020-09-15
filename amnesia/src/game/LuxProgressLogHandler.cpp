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

#include "LuxProgressLogHandler.h"

#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxPlayer.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProgressLogHandler::cLuxProgressLogHandler() : iLuxUpdateable("LuxProgressLogHandler")
{
	mpFile = NULL;
	mlFileNameCount = 0;
	mlCounter =0;

	mbActive = false;
}

//-----------------------------------------------------------------------

cLuxProgressLogHandler::~cLuxProgressLogHandler()
{
	if(mpFile) fclose(mpFile);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

void cLuxProgressLogHandler::LoadUserConfig()
{
	mbActive = gpBase->mpUserConfig->GetBool("Debug", "UseProgLog", false);
	Reset();
}

void cLuxProgressLogHandler::SaveUserConfig()
{
	gpBase->mpUserConfig->SetBool("Debug", "UseProgLog", mbActive);
}

//-----------------------------------------------------------------------

void cLuxProgressLogHandler::OnStart()
{
	
}

//-----------------------------------------------------------------------

void cLuxProgressLogHandler::Reset()
{
	
}

//-----------------------------------------------------------------------

void cLuxProgressLogHandler::CreateAndResetLogFile()
{
	if(mbActive==false) return;

	/////////////////////////////
	// Close previous log file
	if(mpFile)
	{
		fclose(mpFile);
	}

	/////////////////////////////
	// Get file name of log
	cDate currentDate =  cPlatform::GetDate();
	tWString sFileName =	_W("ProgLog_") + gpBase->msProfileName + _W("_") +
		cString::ToStringW(currentDate.year,4)+_W("_") +
		cString::ToStringW(currentDate.month+1,2)+_W("_") +
		cString::ToStringW(currentDate.month_day,2)+_W("_") +
		cString::ToStringW(currentDate.hours,2)+_W("_") +
		cString::ToStringW(currentDate.minutes,2)+_W("_") +
		cString::ToStringW(currentDate.seconds,2)+_W("_") +
		cString::ToStringW(mlFileNameCount,2)+
		_W(".log");

	/////////////////////////////
	// Open file
	tWString sPath = gpBase->msBaseSavePath + sFileName;
#ifdef WIN32
	mpFile = _wfopen(sPath.c_str(),_W("w"));
#else
	mpFile = fopen(cString::To8Char(sPath).c_str(),"w");
#endif

	if(mpFile==NULL)
		Error("Could not open progress log file '%s'!\n", cString::To8Char(sPath).c_str());
	else
	{
		fprintf(mpFile, "--- Amnesia Progress Logger ----\n");
		fprintf(mpFile, "Player: %s Date: %d/%d - %d\n", cString::To8Char(gpBase->msProfileName).c_str(), currentDate.month_day, currentDate.month+1, currentDate.year);
		fprintf(mpFile, "---------------------------------\n");
	}


	mlCounter =0;
}

//-----------------------------------------------------------------------

void cLuxProgressLogHandler::Update(float afTimeStep)
{
	if(mbActive==false) return;


	mlCounter++;
}

//-----------------------------------------------------------------------

void cLuxProgressLogHandler::AddLog(eLuxProgressLogLevel aLevel, const tString& asMessage)
{
	if(mbActive==false || mpFile==NULL) return;

	int lSec = (mlCounter/60) % 60;
	int lMin = (mlCounter/(60*60)) % 60;
	int lHour = mlCounter/ (60*60*60);

	char sTemp[1024];
	sprintf(sTemp, "%02d:%02d:%02d", lHour, lMin, lSec);

	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	tString sMapName = pMap ? pMap->GetName() : "NoMap";
	cLuxPlayer *pPlayer = gpBase->mpPlayer;
	tString sHealth = cString::ToString(pPlayer->GetHealth(),0);
	tString sSanity = cString::ToString(pPlayer->GetSanity(),0);
	tString sTinderboxes = cString::ToString(pPlayer->GetTinderboxes());
	tString sOil = cString::ToString(pPlayer->GetLampOil(),0);
	tString sCoins = cString::ToString(pPlayer->GetCoins());
	
	tString sFinalMess = "# "+LevelToString(aLevel)+" # "+ sTemp;
	sFinalMess += " | "+sMapName + " | H:"+sHealth+ " | S:"+sSanity+ " | O:"+sOil+ " | T:"+sTinderboxes+ " | C:"+sCoins +" \n > ";
	for(size_t i=0; i<asMessage.size(); ++i)
	{
		char c = asMessage[i];
        sFinalMess += c;
		if(c == '\n')
		{
			sFinalMess += " > ";
		}
	}
	sFinalMess += "\n";
	
	fprintf(mpFile, "%s", sFinalMess.c_str());
	fflush(mpFile);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

tString cLuxProgressLogHandler::LevelToString(eLuxProgressLogLevel aLevel)
{
	if(aLevel == eLuxProgressLogLevel_Low)		return "Low";
	if(aLevel == eLuxProgressLogLevel_Medium)	return "Medium";
	if(aLevel == eLuxProgressLogLevel_High)		return "High";
	return "";
}

//-----------------------------------------------------------------------
