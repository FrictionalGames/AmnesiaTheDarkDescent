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

#ifndef LUX_LOAD_SCREEN_HANDLER_H
#define LUX_LOAD_SCREEN_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

enum eLuxLoadScreenState
{
	eLuxLoadScreenState_Game,
	eLuxLoadScreenState_LastEnum
};

//----------------------------------------

class cLuxLoadScreenHandler : public iLuxUpdateable
{
friend class cLuxLoadScreenHandler_SaveData;
public:	
	cLuxLoadScreenHandler();
	~cLuxLoadScreenHandler();
	
	void LoadFonts();

	void OnStart();
	void Reset();

	void Update(float afTimeStep);

	void OnEnterContainer(const tString& asOldContainer);
	void OnLeaveContainer(const tString& asNewContainer);

	void OnDraw(float afFrameTime);

	/**
	* Called when game loading is done. Must be called after DrawGameScreen has been called!
	*/
	void GameScreenLoadDone(const tString& asEndSound, float afLoadTime);
	
	void DrawGameScreen();
	void DrawMenuScreen();
	void DrawBlankScreen();

	void ExitPressed();

	/////////////////////
	//Properties
	/**
	* If alRandomNum > 1, then it will randomize between 1 and alRandom for each LoadScreen giving entry the suffix XX (eg 01). If <=1 then no suffix is added
	*/
	void SetupLoadText(const tString& asCat, const tString& asEntry, int alRandomNum, const tString& asImage);
	
	const tString& GetLoadTextCat(){ return msLoadTextCat;}
	const tString& GetLoadTextEntry(){ return msLoadTextEntry;}
	const tString& GetLoadTextImage(){ return msLoadTextImage;}
	int GetLoadTextEntryRandomNum(){ return mlTextRandomNum; }

private:
	void Exit();
	tString GetGameScreenTextEntry();
	void LoadCurrentImage(const tString &asImage);
	    
	void UpdateGameState(float afTimeStep);
	void DrawGameState(float afFrameTime);
	void DrawGameScreen(cGuiSet *apSet);
	
	//////////////////
	// Data
	cGui *mpGui;
	cScene *mpScene;
	cGraphics *mpGraphics;

	cViewport *mpViewport;
	cGuiSkin *mpGuiSkin;
	cGuiSet *mpGuiSet;

	cGuiGfxElement* mpWhiteGfx;
	
	cVector2f mvScreenSize;
	cVector2f mvGuiSetCenterSize;//Size of the part that is inside a 4:3 ratio!
	cVector2f mvGuiSetSize;
	cVector2f mvGuiSetOffset;
	cVector3f mvGuiSetStartPos;

	iFontData *mpFontDefault;

	cVector2f mvLoadingFontSize;
	cVector2f mvTextFontSize;
	float mfLoadingY;
	float mfTextWithImageY;
	float mfTextAloneY;
	float mfImageY;
	float mfTextMaxWidth;

	float mfFadeOutTime;
	float mfTextDurationMul;

	//////////////////
	// Variables
	cGuiGfxElement* mpCurrentImage;
	tString msCurrentImage;
	
	tString msLoadTextCat;
	tString msLoadTextEntry;
	tString msLoadTextImage;
	int mlTextRandomNum;
	int mlPrevTextNumBitFlags;

	tString msGameScreenEndSound;

	float mfLoadingAlpha;
    float mfAlpha;
	float mfExtraTime;
	eLuxLoadScreenState mState;
	tString msCurrentGameScreenTextEntry;
	tString msCurrentGameScreenImage;
};

//----------------------------------------------


#endif // LUX_LOAD_SCREEN_HANDLER_H
