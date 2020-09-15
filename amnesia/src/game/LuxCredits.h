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

#ifndef LUX_CREDITS_H
#define LUX_CREDITS_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxCredits : public iLuxUpdateable
{
public:	
	cLuxCredits();
	~cLuxCredits();

	void LoadFonts();

	void Reset();

	void Update(float afTimeStep);

	void AddKeyPart(int alKey);
	void Setup(const tString& asMusic, bool abLoopMusic, const tString& asTextCat, const tString& asTextEntry, int alEndNum);

	void OnEnterContainer(const tString& asOldContainer);
	void OnLeaveContainer(const tString& asNewContainer);

	void OnDraw(float afFrameTime);

	void ExitPressed();

	void AppLostInputFocus();
	void AppGotInputFocus();
	
private:
	///////////////////////
	// Data
	cGraphics *mpGraphics;

	cGui* mpGui;
	cGuiSet* mpGuiSet;

	iFontData *mpFontNormal;
	iFontData *mpFontHeader;

	cViewport *mpViewport;

	cGuiGfxElement *mpBlackGfx;

	cVector2f mvScreenSize;
	cVector2f mvGuiSetSize;
	cVector2f mvGuiSetCenterSize;
	cVector2f mvGuiSetOffset;
	cVector3f mvGuiSetStartPos;

	cVector2f mvNormalFontSize;
	cVector2f mvHeaderFontSize;
	cVector2f mvTheEndFontSize;

	float mfScrollSpeed;
	float mfFadeSpeed;

	///////////////////////
	// Vars
	cBinaryBuffer mKeyBuffer;
	tString msMusic;
	bool mbLoopMusic;
	int mlEndNum;

	tWStringVec mvTextRows;

	bool mbActive;
	float mfYPos;
	int mlState;
	float mfTime;
	float mfFadeAlpha;

};

//----------------------------------------------


#endif // LUX_CREDITS_H
