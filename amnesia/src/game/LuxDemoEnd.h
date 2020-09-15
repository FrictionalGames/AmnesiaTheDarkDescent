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

#ifndef LUX_DEMO_END_H
#define LUX_DEMO_END_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxDemoEnd : public iLuxUpdateable
{
public:	
	cLuxDemoEnd();
	~cLuxDemoEnd();

	void LoadFonts();

	void Reset();

	void Update(float afTimeStep);

	void OnEnterContainer(const tString& asOldContainer);
	void OnLeaveContainer(const tString& asNewContainer);

	void OnDraw(float afFrameTime);

	void Exit(bool abShowWebsite);

	void AppLostInputFocus();
	void AppGotInputFocus();

	bool ShowOnAllExit(){ return mbShowOnAllExit;}
	
private:
	void SetUpButtonLabel(cWidgetLabel* apLabel, float* apFadeValue, tGuiCallbackFunc apCallback);

	bool BuyNowOnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(BuyNowOnPressed);

	bool ExitOnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ExitOnPressed);

	bool ButtonLabelOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ButtonLabelOnUpdate);
	bool ButtonLabelOnDraw(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ButtonLabelOnDraw);
	bool ButtonLabelOnMouseEnter(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ButtonLabelOnMouseEnter);
	bool ButtonLabelOnMouseLeave(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ButtonLabelOnMouseLeave);
	///////////////////////
	// Data
	cGraphics *mpGraphics;

	cGui* mpGui;
	cGuiSet* mpGuiSet;

	iFontData *mpFontMessage;
	iFontData *mpFontButton;

	cViewport *mpViewport;

	cGuiGfxElement *mpBlackGfx;

	cVector2f mvScreenSize;
	cVector2f mvGuiSetSize;
	cVector2f mvGuiSetCenterSize;
	cVector2f mvGuiSetOffset;
	cVector3f mvGuiSetStartPos;

	float mfTextWidth;
	float mfTextY;

	float mfAvailableAtY;

	float mfBuyButtonY;
	float mfExitButtonY;
	
	cVector2f mvMessageFontSize;
	cVector2f mvAvailableAtFontSize;
	cVector2f mvButtonFontSize;

	cColor mMessageFontColor;
	cColor mAvailableAtFontColor;
	cColor mButtonFontColor;

	float mfFadeSpeed;

	bool mbShowOnAllExit;

	///////////////////////
	// Vars
	bool mbActive;
	float mfYPos;
	int mlState;
	float mfTime;
	float mfFadeAlpha;

	bool mbExiting;
	bool mbGotoWebsiteAtExit;
		
	float mfBuyNowFade;
	cWidgetLabel* mpLBuyNow;
	float mfExitFade;
	cWidgetLabel* mpLExit;

	tString msBackgroundFile;
	cGuiGfxElement* mpGfxBackground;

    tWStringVec mvTextRows;
	
	tString msDestinationURL;
};

//----------------------------------------------


#endif // LUX_DEMO_END_H
