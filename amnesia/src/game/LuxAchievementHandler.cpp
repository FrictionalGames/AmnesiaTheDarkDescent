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

#include "StdAfx.h"

#include "LuxBase.h"
#include "LuxAchievementHandler.h"
#include "LuxDebugHandler.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxAchievementHandler::iLuxAchievementHandler() : iLuxUpdateable("AchievementHandler")
{
	mbRegistered = false;
}

//-----------------------------------------------------------------------

iLuxAchievementHandler::~iLuxAchievementHandler(void)
{
}

//////////////////////////////////////////////////////////////////////////
// SCRIPT INIT
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxAchievementHandler::Reset()
{
	m_setDebugActivatedAchievements.clear();
}

//-----------------------------------------------------------------------

void iLuxAchievementHandler::RegisterAchievements()
{
	mbRegistered = true;
}

//-----------------------------------------------------------------------

void iLuxAchievementHandler::CreateAchievement(int alID, const tString& asName)
{
	std::map<int, tString>::iterator it = m_mapAchievements.find(alID);

	if(it != m_mapAchievements.end())
	{
		Error("Failed to add Achievement '%s' at id '%d'. Another Achievement already added to that ID '%s'\n", asName.c_str(), alID, it->second.c_str());
	}
	else
	{
		m_mapAchievements.insert(std::pair<int, tString>(alID, asName));
	}
}

void iLuxAchievementHandler::UnlockAchievement(int alID)
{
	if(mbRegistered == false)
	{
		Error("Failed to activate Achievement %d. iLuxAchievementHandler::RegisterAchievements() has not been called\n", alID);
		return;
	}

	std::map<int, tString>::iterator it = m_mapAchievements.find(alID);

	if(it == m_mapAchievements.end())
	{
		Error("Failed to activate Achievement %d. No achievement with that id\n", alID);
	}
	else
	{
		OnUnlockedAchievement(alID);
	}
}

//-----------------------------------------------------------------------

tString iLuxAchievementHandler::GetAchievementName(int alID)
{
	std::map<int, tString>::iterator it = m_mapAchievements.find(alID);

	if(it == m_mapAchievements.end())
	{
		Error("GetAchievementActivated failed, no Achievement with id '%d'\n", alID);
		return "";
	}
	else
	{
		return it->second;
	}
}

void iLuxAchievementHandler::OnUnlockedAchievement(int alID)
{
	if(GetAchievementUnlocked(alID) == false)
	{
		gpBase->mpDebugHandler->AddMessage(_W("Activated Achievement: ") + cString::To16Char(m_mapAchievements.find(alID)->second), false);
		m_setDebugActivatedAchievements.insert(alID);
	}
}

bool iLuxAchievementHandler::GetAchievementUnlocked(int alID)
{
	return m_setDebugActivatedAchievements.find(alID) != m_setDebugActivatedAchievements.end();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------



//-----------------------------------------------------------------------
