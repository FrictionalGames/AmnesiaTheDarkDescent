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

#include "LuxDemoEnd.h"

#include "LuxHelpFuncs.h"
#include "LuxDebugHandler.h"
#include "LuxLoadScreenHandler.h"
#include "LuxMusicHandler.h"
#include "LuxInputHandler.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxDemoEnd::cLuxDemoEnd() : iLuxUpdateable("LuxDemoEnd")
{
	///////////////////////////////
	//Get Engine
	mpGraphics = gpBase->mpEngine->GetGraphics();
	mpGui = gpBase->mpEngine->GetGui();
	mpGuiSet = mpGui->CreateSet("DemoEnd", NULL);

	///////////////////////
	// Load settings
	mvScreenSize = gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetScreenSizeFloat();

	mvGuiSetCenterSize = cVector2f(800, 600);

	LuxCalcGuiSetScreenOffset(mvGuiSetCenterSize, mvGuiSetSize, mvGuiSetOffset);
	mvGuiSetStartPos = cVector3f(-mvGuiSetOffset.x,-mvGuiSetOffset.y,0);

	mpGuiSet->SetVirtualSize(mvGuiSetSize, -1000,1000, mvGuiSetOffset);

	///////////////////////////////
	//Create Viewport
	mpViewport = gpBase->mpEngine->GetScene()->CreateViewport();
	mpViewport->SetActive(false);
	mpViewport->SetVisible(false);
	mpGuiSet->SetActive(false);
	mpGuiSet->SetDrawMouse(false);

	mpViewport->AddGuiSet(mpGuiSet);

	mpFontMessage = NULL;
	mpFontButton = NULL;

	mpGfxBackground = NULL;

	mfFadeAlpha = 1;

	///////////////////////////////
	//Load Data
	tString sSkinFile = gpBase->mpDemoCfg->GetString("DemoEnd", "Skin", "gui_default.skin");
	mpGuiSet->SetSkin(mpGui->CreateSkin(sSkinFile));

	msBackgroundFile = gpBase->mpDemoCfg->GetString("DemoEnd", "BackgroundImage", "");

	mvMessageFontSize = gpBase->mpDemoCfg->GetVector2f("DemoEnd", "MessageFontSize" ,0);
	mMessageFontColor = gpBase->mpDemoCfg->GetColor("DemoEnd", "MessageFontColor", cColor(1,1));

	mvAvailableAtFontSize = gpBase->mpDemoCfg->GetVector2f("DemoEnd", "AvailableAtFontSize" ,0);
	mAvailableAtFontColor = gpBase->mpDemoCfg->GetColor("DemoEnd", "AvailableAtFontColor", cColor(1,1));

	mvButtonFontSize = gpBase->mpDemoCfg->GetVector2f("DemoEnd", "ButtonFontSize", 0);
	mButtonFontColor = gpBase->mpDemoCfg->GetColor("DemoEnd", "ButtonFontColor", cColor(1,1));

	mfTextWidth = gpBase->mpDemoCfg->GetFloat("DemoEnd", "TextWidth",0);
	mfTextY = gpBase->mpDemoCfg->GetFloat("DemoEnd", "TextY",0);

	mfAvailableAtY = gpBase->mpDemoCfg->GetFloat("DemoEnd", "AvailableAtY",0);

	mfBuyButtonY = gpBase->mpDemoCfg->GetFloat("DemoEnd", "BuyButtonY",0);
	mfExitButtonY = gpBase->mpDemoCfg->GetFloat("DemoEnd", "ExitButtonY",0);
	
	msDestinationURL = gpBase->mpDemoCfg->GetString("DemoEnd", "DestinationURL", "");

	mpBlackGfx = mpGui->CreateGfxFilledRect(cColor(0,1), eGuiMaterial_Alpha);

	mfFadeSpeed = gpBase->mpDemoCfg->GetFloat("DemoEnd","FadeSpeed",0);

	mbShowOnAllExit = gpBase->mpDemoCfg->GetBool("DemoEnd","ShowOnAllExit",false);
	
	//Create buttons label buttons
	cVector2f vButtonSize = cVector2f(150, mvButtonFontSize.y);

	cVector3f vButtonPos;
	vButtonPos.x = 400;// - vButtonSize.x/2;
	vButtonPos.z = 10;

	vButtonPos.y = mfBuyButtonY;
	mpLBuyNow = mpGuiSet->CreateWidgetLabel(vButtonPos, vButtonSize, kTranslate("Demo", "BuyNow"), NULL);
	
	vButtonPos.y = mfExitButtonY;
	mpLExit = mpGuiSet->CreateWidgetLabel(vButtonPos, vButtonSize, kTranslate("Demo", "Exit"), NULL);

	SetUpButtonLabel(mpLBuyNow, &mfBuyNowFade, kGuiCallback(BuyNowOnPressed));
	SetUpButtonLabel(mpLExit, &mfExitFade, kGuiCallback(ExitOnPressed));

	///////////////////////////////
	//Setup variables
	Reset();
}

//-----------------------------------------------------------------------

cLuxDemoEnd::~cLuxDemoEnd()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxDemoEnd::LoadFonts()
{
	tString sFontMessage = gpBase->mpDemoCfg->GetString("DemoEnd","MessageFont","");
	tString sFontButton = gpBase->mpDemoCfg->GetString("DemoEnd", "ButtonFont", "");
	mpFontMessage = LoadFont(sFontMessage);
	mpFontButton = LoadFont(sFontButton);

	if(mpFontButton)
	{
		mpLBuyNow->SetDefaultFontType(mpFontButton);
		mpLExit->SetDefaultFontType(mpFontButton);
	}
}

//-----------------------------------------------------------------------

void cLuxDemoEnd::Reset()
{
	mbExiting = false;
	mbGotoWebsiteAtExit = false;
}

//-----------------------------------------------------------------------

void cLuxDemoEnd::OnEnterContainer(const tString& asOldContainer)
{
	////////////////////////////
	//Set up states and viewport
	gpBase->mpInputHandler->ChangeState(eLuxInputState_DemoEnd);

	mpViewport->SetActive(true);
	mpViewport->SetVisible(true);

	mpGuiSet->SetActive(true);
	mpGuiSet->SetDrawMouse(true);
	mpGui->SetFocus(mpGuiSet);

	///////////////////////////////
	//Fade out music
	cMusicHandler* pMusHandler = gpBase->mpEngine->GetSound()->GetMusicHandler();
	pMusHandler->Stop(0.3f);
	
	// Load background
	iTexture* pTex = gpBase->mpEngine->GetResources()->GetTextureManager()->Create2D(msBackgroundFile, false, eTextureType_Rect);
	if(pTex) mpGfxBackground = mpGui->CreateGfxTexture(pTex, true, eGuiMaterial_Alpha);

	// Set up message text
	tWString sText = kTranslate("Demo", "EndMessage");
	mvTextRows.clear();
	mpFontMessage->GetWordWrapRows(mfTextWidth, mvMessageFontSize.y+2,mvMessageFontSize,sText,&mvTextRows);
}

//-----------------------------------------------------------------------

void cLuxDemoEnd::OnLeaveContainer(const tString& asNewContainer)
{
	//Turn off viewport and sets
    mpViewport->SetActive(false);
	mpViewport->SetVisible(false);

	mpGuiSet->SetActive(false);

	//Destroy background graphics
	if(mpGfxBackground)
	{
		mpGui->DestroyGfx(mpGfxBackground);
		mpGfxBackground = NULL;
	}
}

//-----------------------------------------------------------------------

void cLuxDemoEnd::Update(float afTimeStep)
{
	mfTime += afTimeStep;
	
	if(mbExiting)
	{
		mfFadeAlpha += afTimeStep*0.7f;
		if(mfFadeAlpha>1)
		{
			mfFadeAlpha =1;
			gpBase->mpEngine->Exit();
			if(mbGotoWebsiteAtExit)
				cPlatform::OpenBrowserWindow(cString::To16Char(msDestinationURL));
		}
	}
	else
	{
		if(mfFadeAlpha>0.0f)
		{
			mfFadeAlpha -= afTimeStep*0.7f;
			if(mfFadeAlpha< 0) mfFadeAlpha =0;
		}
	}
}

//-----------------------------------------------------------------------

void cLuxDemoEnd::Exit(bool abShowWebsite)
{
	mbExiting = true;
	mbGotoWebsiteAtExit = abShowWebsite;
	gpBase->mpInputHandler->ChangeState(eLuxInputState_Null);
	mpGuiSet->SetDrawMouse(false);
}

//-----------------------------------------------------------------------

void cLuxDemoEnd::OnDraw(float afFrameTime)
{
	//////////////////////////////////////////////
	// Background
	mpGuiSet->DrawGfx(mpBlackGfx, mvGuiSetStartPos, mvGuiSetSize);

	if(mpGfxBackground)
	{
		//Draw the image with correct ratio, always filling out height-wise.
		cVector2f vSize = mpGfxBackground->GetImageSize();
		float fRatio = vSize.x / vSize.y;
		float fRatioMul = fRatio / (4.0f/3.0f);

		mpGuiSet->DrawGfx(mpGfxBackground, cVector3f(400*(1-fRatioMul),0,5), cVector2f(800*fRatioMul, 600),cColor(1,1));
	}

	//////////////////////////////////////////////
	// Text
	if(mpFontMessage)
	{
		float fY = mfTextY;

		for(size_t i=0; i< mvTextRows.size(); ++i)
		{
			////////////////////////
			// Text row
			if(mvTextRows[i].empty()==false)
			{
				mpGuiSet->DrawFont(mvTextRows[i], mpFontMessage, cVector3f(400.0f-mfTextWidth/2.0f,fY,10), mvMessageFontSize, mMessageFontColor,eFontAlign_Left);
			}

			fY += mvMessageFontSize.y + 1;
		}
		mpGuiSet->DrawFont( kTranslate("Demo", "AvailableAt"), mpFontMessage, cVector3f(400.0f,mfAvailableAtY,10), mvAvailableAtFontSize, mAvailableAtFontColor,eFontAlign_Center);
	}


	//////////////////////////////////////////////
	// Fade
	if(mfFadeAlpha > 0)
	{
		mpGuiSet->DrawGfx(mpBlackGfx, mvGuiSetStartPos+cVector3f(0,0,20), mvGuiSetSize, cColor(1, mfFadeAlpha));
	}
}

//-----------------------------------------------------------------------

void cLuxDemoEnd::AppLostInputFocus()
{
	cMusicHandler* pMusHdlr = gpBase->mpEngine->GetSound()->GetMusicHandler();
	pMusHdlr->Pause();
}

//-----------------------------------------------------------------------

void cLuxDemoEnd::AppGotInputFocus()
{
	cMusicHandler* pMusHdlr = gpBase->mpEngine->GetSound()->GetMusicHandler();
	pMusHdlr->Resume();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxDemoEnd::SetUpButtonLabel(cWidgetLabel* apLabel, float* apFadeValue, tGuiCallbackFunc apCallback)
{
	///////////////////////////////////////////
	// Add callbacks
	apLabel->AddCallback(eGuiMessage_OnUpdate, this, kGuiCallback(ButtonLabelOnUpdate));
	apLabel->AddCallback(eGuiMessage_OnDraw, this, kGuiCallback(ButtonLabelOnDraw));
	apLabel->AddCallback(eGuiMessage_MouseEnter, this, kGuiCallback(ButtonLabelOnMouseEnter));
	apLabel->AddCallback(eGuiMessage_MouseLeave, this, kGuiCallback(ButtonLabelOnMouseLeave));
	apLabel->AddCallback(eGuiMessage_MouseUp, this, apCallback);

	// Set up fading helper
	*apFadeValue = 0.0f;
	apLabel->SetUserData(apFadeValue);

	// Set up size, color and other stuff
	apLabel->SetDefaultFontSize(mvButtonFontSize);
	apLabel->SetDefaultFontColor(mButtonFontColor);
	apLabel->SetTextAlign(eFontAlign_Center);
	apLabel->SetAutogenerateSize(true);

	// Center label in position
	cVector3f vPos = apLabel->GetLocalPosition() - cVector3f(apLabel->GetSize().x*0.5f, 0, 0);
    apLabel->SetPosition(vPos);
}

//-----------------------------------------------------------------------

bool cLuxDemoEnd::BuyNowOnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	Exit(true);
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxDemoEnd, BuyNowOnPressed);

//-----------------------------------------------------------------------

bool cLuxDemoEnd::ExitOnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	Exit(false);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxDemoEnd, ExitOnPressed);

//-----------------------------------------------------------------------

bool cLuxDemoEnd::ButtonLabelOnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bMouseOver = apWidget->GetUserValue()==1;
	cColor col = mButtonFontColor;
	float* pFade = (float*)apWidget->GetUserData();

	if(bMouseOver)
	{
		if(*pFade<1.0f)
		{
			*pFade += aData.mfVal*3.0f;
			if(*pFade>1)*pFade =1;
		}
	}
	else
	{
		if(*pFade>0)
		{
			*pFade -= aData.mfVal;
			if(*pFade<0)*pFade =0;
		}
	}

	float fT = *pFade;

	col = col*(1.0f-fT) + cColor(1,0,0)*fT;

	apWidget->SetDefaultFontColor(col);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxDemoEnd, ButtonLabelOnUpdate);

//-----------------------------------------------------------------------

bool cLuxDemoEnd::ButtonLabelOnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxDemoEnd, ButtonLabelOnDraw);

//-----------------------------------------------------------------------

bool cLuxDemoEnd::ButtonLabelOnMouseEnter(iWidget* apWidget, const cGuiMessageData& aData)
{
	apWidget->SetUserValue(1);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxDemoEnd, ButtonLabelOnMouseEnter);

//-----------------------------------------------------------------------

bool cLuxDemoEnd::ButtonLabelOnMouseLeave(iWidget* apWidget, const cGuiMessageData& aData)
{
	apWidget->SetUserValue(0);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxDemoEnd, ButtonLabelOnMouseLeave);

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

