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

#include "LuxMusicHandler.h"

#include "LuxMap.h"
#include "LuxMapHandler.h"
#include "LuxPlayer.h"
#include "LuxEnemy.h"


//////////////////////////////////////////////////////////////////////////
// MUSIC
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMusic::cLuxMusic()
{
	Reset();
}

void cLuxMusic::Reset()
{
	msFile = "";
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMusicHandler::cLuxMusicHandler() : iLuxUpdateable("LuxMusicHandler")
{
	mpMusicHandler = gpBase->mpEngine->GetSound()->GetMusicHandler();

	mlMaxPrio = 8 + eLuxEnemyMusic_LastEnum;
	mfVolumeMul = gpBase->mpGameCfg->GetFloat("Sound","InGameMusicVolume",1.0f);

	mvGameMusic.resize(mlMaxPrio+1);

}

//-----------------------------------------------------------------------

cLuxMusicHandler::~cLuxMusicHandler()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMusicHandler::OnStart()
{
	
}

//-----------------------------------------------------------------------

void cLuxMusicHandler::Reset()
{
	for(size_t i=0; i< mvGameMusic.size(); ++i)
	{
		mvGameMusic[i].Reset();
	}

	mlCurrentMaxPrio = -1;

	for(int i=0; i<eLuxEnemyMusic_LastEnum; ++i)
	{
		m_setEnemies[i].clear();

		mbEnemyPlaying[i] = false;
		mfEnemyPlayCount[i] = 0;
		mfEnemyStopCount[i] = 0;
	}

	gpBase->mpEngine->GetSound()->GetMusicHandler()->ResetResumeData();

	mbEnemyClosePlaying = false;
	mfEnemyCloseCount =0;
	mfEnemyGoneCount =0;

	mfUpdateDangerCount = 0;
	mfUpdateAttackCount = 0;
}

//-----------------------------------------------------------------------

void cLuxMusicHandler::Update(float afTimeStep)
{
	if(gpBase->mpPlayer->IsDead()) return;

	///////////////////////////////////
	//Check if close by music should be played.
	UpdateDangerMusic(afTimeStep);	

	///////////////////////////////////
	//Check if attack music should be played.
	UpdateEnemyMusic(afTimeStep, eLuxEnemyMusic_Search);
	UpdateEnemyMusic(afTimeStep, eLuxEnemyMusic_Attack);
	
	///////////////////////////////////
	//Check if current song is over
	if(mlCurrentMaxPrio >=0)
	{
		if(mpMusicHandler->GetCurrentSong()==NULL)
		{
			mvGameMusic[mlCurrentMaxPrio].msFile = "";
			mlCurrentMaxPrio = -1;
			PlayHighestPriority();
		}
	}
}


//-----------------------------------------------------------------------

void cLuxMusicHandler::Play(const tString &asFile, bool abLoop,float afVolume, float afFadeTime, int alPrio, bool abResume, bool abSpecialEffect)
{
	if(alPrio> mlMaxPrio) alPrio = mlMaxPrio;

	if(mvGameMusic[alPrio].msFile == asFile) return;

	float fVolume = afVolume * mfVolumeMul;

	//If higher or equal to song playing, start playing
	if(mlCurrentMaxPrio <= alPrio)
	{
		float fFadeSpeed = fVolume / afFadeTime;
		mpMusicHandler->Play(asFile,fVolume,fFadeSpeed,abLoop, abResume);
		mlCurrentMaxPrio = alPrio;
	}

	//Set game music properites
	mvGameMusic[alPrio].mbSpecialEffect = abSpecialEffect;
	mvGameMusic[alPrio].mbLoop = abLoop;
	mvGameMusic[alPrio].mbResume = abResume;
	mvGameMusic[alPrio].mfVolume = fVolume;
	mvGameMusic[alPrio].msFile = asFile;
}

//-----------------------------------------------------------------------

void cLuxMusicHandler::Stop(float afFadeTime, int alPrio)
{
	if(alPrio> mlMaxPrio) alPrio = mlMaxPrio;

	//Check if there is any song playing at this prio
	if(mvGameMusic[alPrio].msFile != "")
	{
		mvGameMusic[alPrio].msFile = "";

		//Check the current song playing is this
		if(alPrio == mlCurrentMaxPrio)
		{
			float fFadeSpeed = mvGameMusic[alPrio].mfVolume / afFadeTime;
			mpMusicHandler->Stop(fFadeSpeed);

			//Play next song in priority if there is any.
			mlCurrentMaxPrio = -1;
			PlayHighestPriority();
		}
	}
}

//-----------------------------------------------------------------------

void cLuxMusicHandler::AddEnemy(eLuxEnemyMusic aType, iLuxEnemy *apEntity)
{
	m_setEnemies[aType].insert(apEntity);
}

//-----------------------------------------------------------------------

void cLuxMusicHandler::RemoveEnemy(eLuxEnemyMusic aType, iLuxEnemy *apEntity)
{
	m_setEnemies[aType].erase(apEntity);
}

bool cLuxMusicHandler::EnemyExist(eLuxEnemyMusic aType, iLuxEnemy *apEntity)
{
	tLuxEnemySetIt it = m_setEnemies[aType].find(apEntity);
	if(it == m_setEnemies[aType].end()) return false;

	return true;
}

//-----------------------------------------------------------------------

void cLuxMusicHandler::OnMapEnter(cLuxMap *apMap)
{
	for(int i=0; i<eLuxEnemyMusic_LastEnum; ++i)
	{
		m_setEnemies[i].clear();
		Stop(2.0f, 8+i);
	}

	//Update(2.0f);
}

void cLuxMusicHandler::OnMapLeave(cLuxMap *apMap)
{
	for(int i=0; i<eLuxEnemyMusic_LastEnum; ++i)
	{
		m_setEnemies[i].clear();
		Stop(2.0f, 8+i);
	}


	for(int i=0; i<60; ++i)Update(0.1f);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMusicHandler::UpdateDangerMusic(float afTimeStep)
{
	////////////////////////////
	// HARDMODE early out
	if (gpBase->mbHardMode == true) return;

	for(int i=0; i<eLuxEnemyMusic_LastEnum; ++i)
	{
		if(mbEnemyPlaying[i]) return;
	}
	
	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pMap==NULL) return;
	
	////////////////////////////
	//Music not playing
	if(mbEnemyClosePlaying==false)
	{
		tString sMusic ="";
		int lHighestPrio =-1;
		cLuxEnemyIterator enemyIt = pMap->GetEnemyIterator();
		while(enemyIt.HasNext())
		{
			iLuxEnemy *pEnemy = enemyIt.Next();

			if(pEnemy->GetHealth() <= 0 || pEnemy->IsActive()==false) continue;

			if(pEnemy->GetPlayerInRange())
			{
				if(pEnemy->GetDangerMusic()!= "" && (sMusic=="" || lHighestPrio < pEnemy->GetDangerMusicPrio()))
				{
					sMusic = pEnemy->GetDangerMusic();
					lHighestPrio = pEnemy->GetDangerMusicPrio();
				}
			}
		}

		if(sMusic != "")
		{
			mfEnemyGoneCount =0;
			mfEnemyCloseCount += afTimeStep;
			if(mfEnemyCloseCount > 2.0f)
			{
				Play(sMusic,true,1.0f, 6.0f, 5, true, true);
				mbEnemyClosePlaying = true;
			}
		}
	}
	/////////////////////////
	//Music playing
	else
	{
		bool bFound=false;
		cLuxEnemyIterator enemyIt = pMap->GetEnemyIterator();
		while(enemyIt.HasNext())
		{
			iLuxEnemy *pEnemy = enemyIt.Next();

			if(pEnemy->GetHealth() <= 0 || pEnemy->IsActive()==false) continue;

			if(pEnemy->GetPlayerInRange())
			{
				bFound = true;
			}
		}

		if(bFound == false)
		{
			mfEnemyCloseCount =0;
			mfEnemyGoneCount += afTimeStep;
			if(mfEnemyGoneCount > 6.0f)
			{
				Stop(6.0f, 5);
				mbEnemyClosePlaying = false;
			}
		}
	}
}

//-----------------------------------------------------------------------

void cLuxMusicHandler::UpdateEnemyMusic(float afTimeStep, eLuxEnemyMusic aType)
{
	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pMap==NULL) return;

	for(int i=aType+1; i<eLuxEnemyMusic_LastEnum; ++i)
	{
		if(mbEnemyPlaying[i]) return;
	}

	////////////////////////////////////////
	//Attack not playing and attackers active
	if(mbEnemyPlaying[aType]==false && m_setEnemies[aType].empty()==false)
	{
		mfEnemyStopCount[aType] =0;
		mfEnemyPlayCount[aType] += afTimeStep;
		if(mfEnemyPlayCount[aType] > 0.1f)
		{
			//////////////////////
			// Go through attackers and get music with highest priority
			tString sMusic = "";
			int lMaxPrio = -9999;

			tLuxEnemySetIt it = m_setEnemies[aType].begin();
			for(; it != m_setEnemies[aType].end(); ++it)
			{
				iLuxEnemy *pEnemy = *it;

				if(pEnemy->GetMusic(aType) != "" && pEnemy->GetMusicPrio(aType) > lMaxPrio)
				{
					sMusic = pEnemy->GetMusic(aType);
					lMaxPrio = pEnemy->GetMusicPrio(aType);
				}
			}

			//////////////////////
			// Play music if found
			if(sMusic != "")
			{
				//Stop close music
				if(mbEnemyClosePlaying)
				{
					mbEnemyClosePlaying = false;
					Stop(2.0f, 5);
				}

				//Stop lower prio
				for(int i=0; i<aType; ++i)
				{
					mbEnemyPlaying[i] = false;
					Stop(2.0f, 8+aType);
				}

				Play(sMusic, true, 1.0f, 1.5f, 8+aType, true, true);
				mbEnemyPlaying[aType] = true;
				//Log("Play!");
				mfEnemyPlayCount[aType] =0;
			}
		}
	}
	////////////////////////////////////////
	//Attack playing and no attackers active
	else if(mbEnemyPlaying[aType] && m_setEnemies[aType].empty())
	{
		mfEnemyPlayCount[aType] =0;
		mfEnemyStopCount[aType] += afTimeStep;

		if(mfEnemyStopCount[aType] > 1.2f)
		{
			mbEnemyPlaying[aType] = false;
			Stop(5.0f, 8+aType);
			//Log("Stop!");
			mfEnemyStopCount[aType] =0;
		}
	}
}

//-----------------------------------------------------------------------

void cLuxMusicHandler::PlayHighestPriority()
{
	for(int i= mlMaxPrio; i>=0; --i)
	{
		if(mvGameMusic[i].msFile != "")
		{
			//If looped, play
			if(mvGameMusic[i].mbLoop)
			{
				mpMusicHandler->Play(	mvGameMusic[i].msFile, mvGameMusic[i].mfVolume,	0.3f,true, mvGameMusic[i].mbResume);
				mlCurrentMaxPrio = (int)i;
				break;
			}
			//If not looped remove
			else
			{
				mvGameMusic[i].msFile = "";
			}
		}
	}
}

//-----------------------------------------------------------------------

