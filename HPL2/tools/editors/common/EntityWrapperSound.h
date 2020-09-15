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

#ifndef HPLEDITOR_ENTITY_WRAPPER_SOUND_H
#define HPLEDITOR_ENTITY_WRAPPER_SOUND_H

#include "EntityWrapper.h"

class cEditorWindowViewport;

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
	cEngineEntitySound(iEntityWrapper* apParent, const tString& asIconFile);
	~cEngineEntitySound();

	bool Create(const tString& asName);
};
*/
//---------------------------------------------------------------

class cEntityWrapperTypeSound : public iEntityWrapperType
{
public:
	cEntityWrapperTypeSound();
     
protected:
	iEntityWrapperData* CreateSpecificData();
};

//---------------------------------------------------------------------

class cEntityWrapperDataSound : public iEntityWrapperData
{
public:
	cEntityWrapperDataSound(iEntityWrapperType*);

protected:
	iEntityWrapper* CreateSpecificEntity();
};

//---------------------------------------------------------------

class cEntityWrapperSound : public iEntityWrapper
{
public:
	cEntityWrapperSound(iEntityWrapperData* apData);
	virtual ~cEntityWrapperSound();

	cEntityIcon* CreateIcon();

	bool SetProperty(int, const float&);
	bool SetProperty(int, const bool&);
	bool SetProperty(int, const tString&);

	bool GetProperty(int, float&);
	bool GetProperty(int, bool&);
	bool GetProperty(int, tString&);

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected, const cColor& aHighlightCol=cColor(1,1), const cColor& aDisabledCol=cColor(0.5f,1));

	cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);

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

#endif // HPLEDITOR_ENTITY_WRAPPER_SOUND_H
