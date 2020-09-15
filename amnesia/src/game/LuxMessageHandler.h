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

#ifndef LUX_MESSAGE_HANDLER_H
#define LUX_MESSAGE_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------


class cLuxMessageHandler : public iLuxUpdateable
{
friend class cLuxMusicHandler_SaveData;
public:	
	cLuxMessageHandler();
	~cLuxMessageHandler();
	
	void LoadFonts();
	void OnStart();
	void Update(float afTimeStep);
	void Reset();

	void LoadUserConfig();
	void SaveUserConfig();

	void OnMapEnter(cLuxMap *apMap);
	void OnMapLeave(cLuxMap *apMap);

	void StarQuestAddedMessage();

	void StartPauseMessage(const tWString& asText, bool abYesNo, iLuxMessageCallback *apCallback);
	
	/**
	* if time is <=0 then the life time is calculated based on string length.
	*/
	void SetMessage(const tWString& asText, float afTime);
	bool IsMessageActive(){ return mfMessageTime>0; }

	void OnDraw(float afFrameTime);

	void DoAction(eLuxPlayerAction aAction, bool abPressed);

	bool IsPauseMessageActive(){ return mbPauseMessageActive; }
	void SetPauseMessageActive(bool abX);

	bool ShowSubtitles(){ return mbShowSubtitles;}
	void SetShowSubtitles(bool abX){ mbShowSubtitles=abX;}

	bool ShowEffectSubtitles(){ return mbShowEffectSubtitles; }
	void SetShowEffectSubtitles(bool abX){ mbShowEffectSubtitles=abX; }
private:
	void DrawQuestAdded();
	void DrawMessage();
	void DrawPauseMessage();
	
	
	//////////////////
	// Data
	cGuiGfxElement *mpBlackGfx;
	iFontData *mpFont;

	cGuiGfxElement *mpQuestAddedIcon;
	tString msQuestAddedSound;
	
	cVector2f mvFontSize;
	
	//////////////////
	// Variables
	bool mbShowSubtitles;
	bool mbShowEffectSubtitles;

	bool mbPauseMessageActive;
	float mfPauseMessageAlpha;

	float mfMessageAlpha;
	float mfMessageTime;

	bool mbQuestMessageActive;
	float mfQuestMessageAlpha;
	float mfQuestMessageTime;

	cLinearOscillation mQuestOscill;

	tWStringVec mvMessageRows;

	tWStringVec mvLines;
	bool mbMessageYesNo;
	iLuxMessageCallback* mpCallback;
};

//----------------------------------------------


#endif // LUX_MESSAGE_HANDLER_H
