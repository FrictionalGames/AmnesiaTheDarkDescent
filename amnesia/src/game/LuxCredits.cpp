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

#include "LuxCredits.h"

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

cLuxCredits::cLuxCredits() : iLuxUpdateable("LuxCredits")
{
	///////////////////////////////
	//Get Engine
	mpGraphics = gpBase->mpEngine->GetGraphics();
	mpGui = gpBase->mpEngine->GetGui();
	mpGuiSet = mpGui->CreateSet("Credits", NULL);

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

	mpFontNormal = NULL;
	mpFontHeader = NULL;

	///////////////////////////////
	//Load Data
	mvNormalFontSize = gpBase->mpMenuCfg->GetVector2f("Credits","NormalFontSize",0);
	mvHeaderFontSize = gpBase->mpMenuCfg->GetVector2f("Credits","HeaderFontSize",0);
	mvTheEndFontSize = gpBase->mpMenuCfg->GetVector2f("Credits","TheEndFontSize",0);

	mpBlackGfx = mpGui->CreateGfxFilledRect(cColor(0,1), eGuiMaterial_Alpha);

	mfScrollSpeed = gpBase->mpMenuCfg->GetFloat("Credits","ScrollSpeed",0);
	mfFadeSpeed = gpBase->mpMenuCfg->GetFloat("Credits","FadeSpeed",0);

	///////////////////////////////
	//Setup variables
	Reset();
}

//-----------------------------------------------------------------------

cLuxCredits::~cLuxCredits()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxCredits::LoadFonts()
{
	tString sFontNormal = gpBase->mpMenuCfg->GetString("Credits","NormalFont","");
	tString sFontHeader = gpBase->mpMenuCfg->GetString("Credits","HeaderFont","");
	mpFontNormal = LoadFont(sFontNormal);
	mpFontHeader = LoadFont(sFontHeader);
}

//-----------------------------------------------------------------------

void cLuxCredits::Reset()
{
	msMusic = "";
	mbLoopMusic = false;
	mvTextRows.clear();

	mbActive = false;

	mfYPos =600;
	mlState =0;
	mfTime =0;
	mfFadeAlpha =0;

	mKeyBuffer.Clear();
}

//-----------------------------------------------------------------------

void cLuxCredits::OnEnterContainer(const tString& asOldContainer)
{
	////////////////////////////
	//Set up states and viewport
	gpBase->mpInputHandler->ChangeState(eLuxInputState_Credits);

	mpViewport->SetActive(true);
	mpViewport->SetVisible(true);

	mpGuiSet->SetActive(true);
	mpGui->SetFocus(mpGuiSet);

	///////////////////////////////
	//Play music
	if(msMusic != "")
	{
		cMusicHandler* pMusHandler = gpBase->mpEngine->GetSound()->GetMusicHandler();
		pMusHandler->Play(msMusic, 1.0f, 1.0f, mbLoopMusic, false);
	}
}

//-----------------------------------------------------------------------

void cLuxCredits::OnLeaveContainer(const tString& asNewContainer)
{
	//Turn off viewport and sets
    mpViewport->SetActive(false);
	mpViewport->SetVisible(false);

	mpGuiSet->SetActive(false);
}

//-----------------------------------------------------------------------


static tString DecryptCryptoStringCode(const tString &asEncStr, cBinaryBuffer &aKeyBuffer)
{
	size_t lBuffPos=0;
	tString sOutStr;

	aKeyBuffer.SetPos(0);
	for(size_t i=0; i<asEncStr.size(); ++i)
	{
		sOutStr +=  asEncStr[i] ^ aKeyBuffer.GetChar();
		if(++lBuffPos >= aKeyBuffer.GetSize()) aKeyBuffer.SetPos(0);
	}

	return sOutStr;
}

//-----------------------------------------------------------------------

void cLuxCredits::Update(float afTimeStep)
{
	mfTime += afTimeStep;

	//////////////////////////////////////////////
	// STATE 0
	if(mlState ==0 || mlState ==1)
	{
		mfYPos -= afTimeStep * mfScrollSpeed;
		
		////////////////////////////////
		//Check if the credits are over.
		float fSize[2] = {17,19};
		float fY = mfYPos;

		//Calculate Y
		for(size_t i=0; i< mvTextRows.size(); ++i)
		{
			int lSize =0;
			if(mvTextRows[i][0] == _W('*')) lSize=1;
			if(mvTextRows[i].size()<=1) fY += fSize[lSize];
			fY += fSize[lSize];
		}
		//Almost done, fade out song.
		if(fY < -120)
		{
			cMusicHandler *pMusicHandler = gpBase->mpEngine->GetSound()->GetMusicHandler();
			pMusicHandler->Stop(0.1f);
		}

		//Finished, fade out!
		if(fY < -280 && mlState==0)
		{
			mfTime =0;
			mlState++;
		}
		if(mlState ==0) return;
	}
	//////////////////////////////////////////////
	// STATE 1/4 - FADE OUT
	if(mlState ==1 || mlState==4)
	{
		mfFadeAlpha += afTimeStep * mfFadeSpeed;
		if(mfFadeAlpha > 1)
		{
			mfFadeAlpha = 1;

			//Last fae out, end game
			if(mlState==4)
			{
				gpBase->mpEngine->GetUpdater()->BroadcastMessageToAll(eUpdateableMessage_Reset);

				gpBase->mpLoadScreenHandler->DrawMenuScreen();
				gpBase->mpEngine->GetUpdater()->SetContainer("MainMenu");
			}
			//NExt state
			else
			{
				mlState++;
				if(mlState == 2)
				{
					///////////////////////
					// If ptest, see if we want to add surprise
					if(gpBase->mbPTestActivated)
					{
						/*mKeyBuffer.SetPos(0);
						int lCRC = mKeyBuffer.GetCRC(kCRCKey);
						if(lCRC == kKeyBufferCRC)
						{
						}*/
					}
				}
			}
		}
		return;
	}
	//////////////////////////////////////////////
	// STATE 2 - FADE IN
	if(mlState ==2)
	{
		mfFadeAlpha -= afTimeStep * mfFadeSpeed;
		if(mfFadeAlpha < 0)
		{
			mfFadeAlpha = 0;
			mlState++;			
		}
		return;
	}

	
}

//-----------------------------------------------------------------------

void cLuxCredits::AddKeyPart(int alKey)
{
	mKeyBuffer.AddInt32(alKey);
}

//-----------------------------------------------------------------------

void cLuxCredits::Setup(const tString& asMusic, bool abLoopMusic, const tString& asTextCat, const tString& asTextEntry, int alEndNum)
{
	msMusic = asMusic;
	mbLoopMusic = abLoopMusic;
	mlEndNum = alEndNum;


	tWString sText = kTranslate(asTextCat, asTextEntry);
	mpFontNormal->GetWordWrapRows(750, 19,17,sText,&mvTextRows);	
}

//-----------------------------------------------------------------------

void cLuxCredits::ExitPressed()
{
	//Only skip forward during text roll or The End
	if(mlState!=0 && mlState!=3) return; 
	
	//Always show for at least 3 sec
	if(mfTime <3) return;
	
	mlState++;
	mfTime =0;

	//If start scroll fade out, stop music.
	if(mlState==1)
	{
		cMusicHandler *pMusicHandler = gpBase->mpEngine->GetSound()->GetMusicHandler();
		pMusicHandler->Stop(0.1f);
	}

}

//-----------------------------------------------------------------------

void cLuxCredits::OnDraw(float afFrameTime)
{
	//////////////////////////////////////////////
	// Background
	mpGuiSet->DrawGfx(mpBlackGfx, mvGuiSetStartPos, mvGuiSetSize);

	//////////////////////////////////////////////
	// Fade
	if(mfFadeAlpha > 0)
	{
		mpGuiSet->DrawGfx(mpBlackGfx, mvGuiSetStartPos+cVector3f(0,0,20), mvGuiSetSize, cColor(1, mfFadeAlpha));
	}
	
	//////////////////////////////////////////////
	// STATE 0 - CREDITS
	if(mlState == 0 || mlState==1)
	{
		float fRowAdd[2] = {mvNormalFontSize.y+2, mvHeaderFontSize.y+2};
		float fY = mfYPos;
		for(size_t i=0; i< mvTextRows.size(); ++i)
		{
			int lSize =0;
			
			////////////////////////
			// String is empty row
			if(mvTextRows[i].size()<=1)
			{
				fY += fRowAdd[lSize];
			}
			////////////////////////
			// Text row
			else
			{
				//Check if header
				if(mvTextRows[i][0] == _W('*'))	lSize=1;

				////////////////////////
				//Draw Font
				if(fY >= -fRowAdd[lSize])
				{
					if(fY > 600) continue;

					float fAlpha = fY/300;
					if(fAlpha > 1) fAlpha = (2 - fAlpha);

					//Normal
					if(lSize==0)
						mpGuiSet->DrawFont(mvTextRows[i], mpFontNormal, cVector3f(400,fY,10), mvNormalFontSize, cColor(1,fAlpha),eFontAlign_Center);
					//Header
					else
						mpGuiSet->DrawFont(cString::SubW(mvTextRows[i],1), mpFontHeader, cVector3f(400,fY,10), mvHeaderFontSize, cColor(0.8f,fAlpha),eFontAlign_Center);
				}
			}

			//Increase row counter
			fY += fRowAdd[lSize];
		}
	}
	//////////////////////////////////////////////
	// STATE 1 - SECRET CODE
	else if(mlState >=2)
	{	
		mpGuiSet->DrawFont(kTranslate("General", "TheEnd"), mpFontHeader, cVector3f(400,290,10),mvTheEndFontSize,cColor(1,1),eFontAlign_Center);

		//Secret code
		if(mlEndNum >=0 && mlEndNum<=2 && gpBase->mbPTestActivated==false)
		{
			tWString sCode = _W("");
			if(mlEndNum==0)	sCode = _W("tyr299");
			if(mlEndNum==1)	sCode = _W("odn314");
			if(mlEndNum==2)	sCode = _W("lke271");

			mpGuiSet->DrawFont(sCode, mpFontNormal, cVector3f(400,560,10),15,cColor(0.65f,1),eFontAlign_Center);
		}
		else if(gpBase->mbPTestActivated)
		{
			//mKeyBuffer.SetPos(0);
			//int lCRC = mKeyBuffer.GetCRC(kCRCKey);
			//if(lCRC != kKeyBufferCRC)
			//	mpGuiSet->DrawFont(_W("Well done, but far from perfect..."), mpFontNormal, cVector3f(400,560,10),11,cColor(0.65f,1),eFontAlign_Center);
		}
	}
}

//-----------------------------------------------------------------------

void cLuxCredits::AppLostInputFocus()
{
	cMusicHandler* pMusHdlr = gpBase->mpEngine->GetSound()->GetMusicHandler();
	pMusHdlr->Pause();
}

//-----------------------------------------------------------------------

void cLuxCredits::AppGotInputFocus()
{
	cMusicHandler* pMusHdlr = gpBase->mpEngine->GetSound()->GetMusicHandler();
	pMusHdlr->Resume();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

