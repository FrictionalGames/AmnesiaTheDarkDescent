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

#ifndef LUX_HINT_HANDLER_H
#define LUX_HINT_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//---------------------------------------

#ifdef USE_GAMEPAD

class cLuxHintIcon
{
public:
	cGuiGfxElement *mpGuiIcon;
	cVector3f mvPosition;
	cVector2f mvSize;
	int mlCharacterPosition;
};

#endif

//----------------------------------------

class cLuxHintHandler : public iLuxUpdateable
{
friend class cLuxHintHandler_SaveData;
public:	
	cLuxHintHandler();
	~cLuxHintHandler();
	
	void LoadFonts();
	void OnStart();
	void Update(float afTimeStep);
	void LoadUserConfig();
	void SaveUserConfig();
	void Reset();
	void OnDraw(float afFrameTime);

	void DrawHintText(float afFrameTime, cGuiSet *apGuiSet);
	void UpdateHintText(float afTimeStep);

	void SetActive(bool abX);
	bool IsActive() { return mbActive; }

	/**
	* if time shown is <=0 then the life time is calculated based on string length.
	*/
	bool Add(const tString &asName, const tWString &asMessage, float afTimeShown);
	bool Remove(const tString &asName);

	void Block(const tString &asName);
	void UnBlock(const tString &asName);

private:
#ifdef USE_GAMEPAD
	void ParseStringForGamepadIcons();
	tWString AddGamepadIconAtPosition(const tWString& asCommand, int alPosition);
#endif

	//////////////////
	// Data
	iFontData *mpFont;

	float mfYPos;
	cVector2f mvFontSize;

	float mfTextDurationMul;
	
	//////////////////
	// Variables
	bool mbActive;

	tStringSet m_setHintsGiven;
	tStringSet m_setHintsBlocked;

	cLinearOscillation mHintOscill;

	float mfAlpha;
	float mfShowTextCount;
	tWString msCurrentText;

#ifdef USE_GAMEPAD
	cGuiGfxElement* mpGamepadButtonIcon[eGamepadButton_LastEnum];
	cGuiGfxElement* mpGamepadHatIcon[4];
	cGuiGfxElement* mpGampadAxisIcon[eGamepadAxis_LastEnum * 2];

	std::vector<cLuxHintIcon> mvHintIcons;
#endif
};

//----------------------------------------------


#endif // LUX_HINT_HANDLER_H
