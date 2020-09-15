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

#ifndef LUX_ACHIEVEMENT_HANDLER_H
#define LUX_ACHIEVEMENT_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"
#include "LuxTypes.h"

//-------------------------------------------------

class iLuxAchievementHandler : public iLuxUpdateable
{
public:
	iLuxAchievementHandler();
	virtual ~iLuxAchievementHandler();

	///////////////////////////////
	// General
	void Reset();

	////////////////
	// Achievements
	void CreateAchievement(int alID, const tString& asName);
	void UnlockAchievement(int alID);
	virtual void RegisterAchievements();
	
	tString GetAchievementName(int alID);
	virtual bool GetAchievementUnlocked(int alID);
	virtual tString GetAchievementDisplayName(int alID){return "";}
	virtual tString GetAchievementDescription(int alID){return "";}
	virtual int GetAchievementIcon(int alID){return -1;}

	virtual void OnUnlockedAchievement(int alID);

protected:
	std::map<int, tString> m_mapAchievements;
	std::set<int> m_setDebugActivatedAchievements;
	bool mbRegistered;
};

#endif // LUX_ACHIEVEMENT_HANDLER_H

