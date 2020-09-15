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

#ifndef ED_OBJ_SOUND_H
#define ED_OBJ_SOUND_H

#include "../Common/EdScnObject.h"

class cEdWindowViewport;

//---------------------------------------------------------------

#define SoundPropIdStart 130

enum eSoundBool
{
	eSoundBool_UseDefault = SoundPropIdStart,

	eSoundBool_LastEnum,
};

enum eSoundFloat
{
	eSoundFloat_Volume = SoundPropIdStart,
	eSoundFloat_MinDist,
	eSoundFloat_MaxDist,

	eSoundFloat_LastEnum,
};

enum eSoundStr
{
	eSoundStr_Filename = SoundPropIdStart,

	eSoundStr_LastEnum,
};

//---------------------------------------------------------------
/*
class cEngineEntitySound : public cIconEntity
{
public:
	cEngineEntitySound(iEdObject* apParent, const tString& asIconFile);
	~cEngineEntitySound();

	bool Create(const tString& asName);
};
*/
//---------------------------------------------------------------

class cTypeSound : public iEdScnObjType
{
public:
	cTypeSound();
     
protected:
	iEdObjectData* CreateTypeSpecificData();
};

//---------------------------------------------------------------------

class cEdObjDataSound : public iEdScnObjData
{
public:
	cEdObjDataSound(iEdObjectType*);

protected:
	iEdObject* CreateTypeSpecificObject();
};

//---------------------------------------------------------------

class cEdObjSound : public iEdScnObject
{
public:
	cEdObjSound(iEdObjectData* apData);
	virtual ~cEdObjSound();

	cObjectIcon* CreateIcon();

	bool SetProperty(int, const float);
	bool SetProperty(int, const bool);
	bool SetProperty(int, const tString&);

	bool GetProperty(int, float&);
	bool GetProperty(int, bool&);
	bool GetProperty(int, tString&);

	iEdEditPane* CreateEditPane();

	void SetSoundEntityFile(const tString& asFile);
	const tString& GetSoundEntityFile() { return msSoundEntityFile; }

	void SetUseDefault(bool abX) { mbUseDefault = abX; }
	bool GetUseDefault() { return mbUseDefault; }

	void SetMinDistance(float afX) { mfMinDistance = afX; }
	void SetMaxDistance(float afX) { mfMaxDistance = afX; }

	float GetMinDistance() { return mfMinDistance; }
	float GetMaxDistance() { return mfMaxDistance; }

	void SetVolume(float afX) { mfVolume = afX; }
	float GetVolume() { return mfVolume; }

protected:
	void OnDraw(const cModuleDrawData&);
	//////////////////////
	// Data
	tString msSoundEntityFile;

	cSoundEntityData* mpSoundEntityData;

	bool mbUseDefault;

	float mfMinDistance;
	float mfMaxDistance;

	float mfVolume;
};

//---------------------------------------------------------------------

#endif // ED_OBJ_SOUND_H
