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

#ifndef LUX_INSANITY_HANDLER_H
#define LUX_INSANITY_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class iLuxInstanityEvent
{
public:
	iLuxInstanityEvent();
	virtual ~iLuxInstanityEvent();

	/////////////////
	// General
	virtual void Update(float afTimeStep){}
	virtual void OnDraw(float afFrameTime){}

	void LoadData(cXmlElement * apVarElem);
	virtual void OnLoadData(cXmlElement * apVarElem)=0;

	void Start();
	virtual void OnStart()=0;
	virtual void OnExit()=0;

	/////////////////
	// Properties
	const tString& GetName(){ return msName;}
	const tString& GetSet(){ return msSet;}
	float GetMaxSanity(){ return mfMaxSanity;}
	
	void SetUsed(bool abX){ mbUsed = abX; }
	bool IsUsed(){ return mbUsed; }

	bool IsOver(){ return mbOver; }

protected:
	void EventIsDone(){ mbOver = true;}

private:
    tString msName;
	tString msSet;
	bool mbUsed;
	bool mbOver;
	float mfMaxSanity;
    	
};

//----------------------------------------------

class cLuxInstanityEvent_Bugs_Bug
{
public:
	bool mbActive;

	cVector2f mvPos;
	cVector2f mvVel;
	float mfSizeMul;
	float mfAngle;

	float mfAlpha;
};

class cLuxInstanityEvent_Bugs : public iLuxInstanityEvent
{
public:
	cLuxInstanityEvent_Bugs();
	~cLuxInstanityEvent_Bugs();

	void OnLoadData(cXmlElement * apVarElem);

	void OnStart();
	void OnExit();

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime);

private:
	//////////////////
	//Data
	int mlNumOfBugs;
	tString msLoopSound;
	float mfSoundVolume;
	float mfDuration;

	float mfMinSizeMul;
	float mfMaxSizeMul;

	float mfWanderCircleDist;
	float mfWanderCircleRadius;
	float mfSwarmPointMul;
	float mfMaxSpeed;

	cGuiGfxElement *mpBugImage;

	//////////////////
	//Vars
	std::vector<cLuxInstanityEvent_Bugs_Bug> mvBugs; 

	cSoundEntry *mpSoundEntry;
	int mlSoundEntryID;

	float mfTimeCount;
};

//----------------------------------------------

class cLuxInstanityEvent_Particles : public iLuxInstanityEvent
{
public:
	cLuxInstanityEvent_Particles();
	~cLuxInstanityEvent_Particles();

	void OnLoadData(cXmlElement * apVarElem);

	void OnStart();
	void OnExit();

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime);

private:
	//////////////////
	//Data
	tString msSoundFile;
	bool mbLoopSound;
	float mfSoundVolume;
	float mfSoundFadeInTime;
	float mfSoundFadeOutTime;

	tString msParticleSystem;

	float mfDuration;
	
	//////////////////
	//Vars
	float mfTimeCount;

	cSoundEntry *mpSoundEntry;
	int mlSoundEntryID;
	
	cParticleSystem *mpPS;
};

//----------------------------------------------

class cLuxInstanityEvent_SoundStream : public iLuxInstanityEvent
{
public:
	cLuxInstanityEvent_SoundStream();
	~cLuxInstanityEvent_SoundStream();
	
	void OnLoadData(cXmlElement * apVarElem);

	void OnStart();
	void OnExit();

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime);
	
private:
	tString msFile;
	float mfVolume;
	float mfSoundDelayTime;
	bool mbFadeScreen;
	cGuiGfxElement *mpWhiteGfx;
	cColor mFadeColor;
	float mfFadeInSpeed;
	float mfFadeOutSpeed;
	bool mbDisablePlayer;
	tString msFadeImageFile;

	tString msSubtitleCat;
	tString msSubtitleEntry;

	std::vector<tWString> mvCurrentTextRows;

	float mfSoundCount;
    float mfFadeAlpha;
	float mfFadeAlphaSpeed;
	cSoundEntry *mpSoundEntry;
	int mlSoundEntryID;
	cGuiGfxElement *mpFadeImage;
};

//----------------------------------------------

class cLuxInstanityEvent_Steps : public iLuxInstanityEvent
{
public:
	cLuxInstanityEvent_Steps();
	~cLuxInstanityEvent_Steps();

	void OnLoadData(cXmlElement * apVarElem);

	void OnStart();
	void OnExit();

	void Update(float afTimeStep);
	void OnDraw(float afFrameTime);

private:
	tString msSound;
	float mfStepTime;
	int mlStepNum;
	float mfDistance;
	
	float mfTimeMulPerStep;
	float mfDistanceMulPerStep;

   	float mfSoundCountMax;
	float mfSoundCount;
	int mlCount;
	cVector3f mvPosition;

};

//----------------------------------------------


class cLuxInsanityHandler : public iLuxUpdateable
{
friend class cLuxInsanityHandler_SaveData;
public:	
	cLuxInsanityHandler();
	~cLuxInsanityHandler();
	
	void OnStart();
	void Reset();
	void Update(float afTimeStep);
	void OnDraw(float afFrameTime);

	void StartEvent();
	void StartEvent(int alIdx);
	void StartEvent(const tString &asName);
	void StopCurrentEvent();

	int GetEventNum(){ return (int)mvEvents.size();}
	iLuxInstanityEvent *GetEvent(int alIdx){ return mvEvents[alIdx]; };

	void ReloadEvents();

	void EnableSet(const tString &asSet);
	void DisableSet(const tString &asSet);

	/////////////////
	// Properties
	bool NewEventIsPossible();
	float GetNewEventCount(){ return mfNewEventCount;}
	int GetCurrentEvent(){ return mlCurrentEvent; }

 
private:
	iLuxInstanityEvent* EventTypeToData(const tString& asType);
	void LoadEvents(const tString& asFile);

	bool SetIsDisabled(const tString& asSet);

	//////////////////
	// Data
	float mfMaxSanity_LongWait;
	float mfMaxSanity_MedWait;
	float mfMaxSanity_ShortWait;

	float mfTimeBetween_LongWait;
	float mfTimeBetween_MedWait;
	float mfTimeBetween_ShortWait;


	
	std::vector<iLuxInstanityEvent*> mvEvents;
	
	//////////////////
	// Variables
	float mfNewEventCount;
	int mlCurrentEvent;

	tStringSet m_setDisabledSets;
};

//----------------------------------------------


#endif // LUX_Insanity_HANDLER_H
