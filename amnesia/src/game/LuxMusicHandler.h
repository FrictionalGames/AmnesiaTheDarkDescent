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

#ifndef LUX_MUSIC_HANDLER_H
#define LUX_MUSIC_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxMusic
{
friend class cLuxMusicHandler;
public:
	cLuxMusic();

	void Reset();

	tString msFile;
	bool mbLoop;
	bool mbResume;
	float mfVolume;
	bool mbSpecialEffect;	//If started by enemy close or similar.
};

//----------------------------------------


class cLuxMusicHandler : public iLuxUpdateable
{
friend class cLuxMusicHandler_SaveData;
public:	
	cLuxMusicHandler();
	~cLuxMusicHandler();
	
	void OnStart();
	void Update(float afTimeStep);
	void Reset();

	float GetVolumeMul(){ return mfVolumeMul;}

	int GetMaxPrio(){ return mlMaxPrio;}
	int GetCurrentMaxPrio(){ return mlCurrentMaxPrio;}

	cLuxMusic* GetMusic(int alPrio){ return &mvGameMusic[alPrio]; }

	void OnMapEnter(cLuxMap *apMap);
	void OnMapLeave(cLuxMap *apMap);

	void Play(const tString &asFile, bool abLoop,float afVolume, float afFadeTime, int alPrio, bool abResume, bool abSpecialEffect);
	void Stop(float afFadeTime, int alPrio);

	void AddEnemy(eLuxEnemyMusic aType, iLuxEnemy *apEnemy);
	void RemoveEnemy(eLuxEnemyMusic aType, iLuxEnemy *apEnemy);
	bool EnemyExist(eLuxEnemyMusic aType, iLuxEnemy *apEnemy);
	
	void AddSearcher(iLuxEnemy *apEnemy);
	void RemoveSearcher(iLuxEnemy *apEnemy);
	bool SearcherExist(iLuxEnemy *apEnemy);
 
private:
	void UpdateDangerMusic(float afTimeStep);
	void UpdateEnemyMusic(float afTimeStep, eLuxEnemyMusic aType);

	void PlayHighestPriority();

	//////////////////
	// Data
	cMusicHandler *mpMusicHandler;
	int mlMaxPrio;
	float mfVolumeMul;

	//////////////////
	// Variables
	float mfUpdateDangerCount;
	float mfUpdateAttackCount;

	std::vector<cLuxMusic> mvGameMusic;
	
	int mlCurrentMaxPrio;

	tLuxEnemySet m_setEnemies[eLuxEnemyMusic_LastEnum];
	
	bool mbEnemyPlaying[eLuxEnemyMusic_LastEnum];
	float mfEnemyPlayCount[eLuxEnemyMusic_LastEnum];
	float mfEnemyStopCount[eLuxEnemyMusic_LastEnum];

	bool mbEnemyClosePlaying;
	float mfEnemyCloseCount;
	float mfEnemyGoneCount;
};

//----------------------------------------------


#endif // LUX_MUSIC_HANDLER_H
