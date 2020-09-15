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

#include "LuxMessageHandler.h"

#include "LuxMapHandler.h"
#include "LuxInputHandler.h"
#include "LuxHelpFuncs.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMessageHandler::cLuxMessageHandler() : iLuxUpdateable("LuxMusicHandler")
{
	cGui *pGui = gpBase->mpEngine->GetGui();

	mpBlackGfx 	= pGui->CreateGfxFilledRect(cColor(0,1),eGuiMaterial_Alpha);

	mpFont = NULL;

	msQuestAddedSound = gpBase->mpMenuCfg->GetString("Messages", "QuestAddedSound","");
	tString sQuestAddedImage = gpBase->mpMenuCfg->GetString("Messages", "QuestAddedIcon","");
	if(sQuestAddedImage != "")	mpQuestAddedIcon = pGui->CreateGfxImage(sQuestAddedImage, eGuiMaterial_Alpha);
	else					mpQuestAddedIcon = NULL;

	mvFontSize = gpBase->mpMenuCfg->GetVector2f("Messages", "GameMessageFontSize",0);
}

//-----------------------------------------------------------------------

cLuxMessageHandler::~cLuxMessageHandler()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMessageHandler::LoadFonts()
{
	tString sFontFile = "game_default.fnt";
	mpFont = LoadFont(sFontFile);
}

void cLuxMessageHandler::OnStart()
{
	
}

//-----------------------------------------------------------------------

void cLuxMessageHandler::Reset()
{
	mbPauseMessageActive = false;
	mfPauseMessageAlpha =0;

	mbQuestMessageActive = false;
	mfQuestMessageAlpha = 0;
	mfQuestMessageTime = 0;

	mQuestOscill.SetUp(0,1,0, 1,1);

	mvLines.clear();
	mpCallback = NULL;
	mbMessageYesNo = false;

	mfMessageAlpha =0;
	mfMessageTime = 0;
}

//-----------------------------------------------------------------------

void cLuxMessageHandler::LoadUserConfig()
{
	mbShowSubtitles = gpBase->mpUserConfig->GetBool("Game", "ShowSubtitles", true);
	mbShowEffectSubtitles = gpBase->mpUserConfig->GetBool("Game", "ShowEffectSubtitles", false);
}

//-----------------------------------------------------------------------

void cLuxMessageHandler::SaveUserConfig()
{
	gpBase->mpUserConfig->SetBool("Game", "ShowSubtitles", mbShowSubtitles);
	gpBase->mpUserConfig->SetBool("Game", "ShowEffectSubtitles", mbShowEffectSubtitles);
}

//-----------------------------------------------------------------------

void cLuxMessageHandler::OnMapEnter(cLuxMap *apMap)
{
	Reset();
}

void cLuxMessageHandler::OnMapLeave(cLuxMap *apMap)
{

}

//-----------------------------------------------------------------------

void cLuxMessageHandler::StarQuestAddedMessage()
{
	if(mbQuestMessageActive) return;

	mbQuestMessageActive = true;
	mfQuestMessageAlpha =0;
	mfQuestMessageTime =0;
}


//-----------------------------------------------------------------------

void cLuxMessageHandler::StartPauseMessage(const tWString& asText, bool abYesNo, iLuxMessageCallback *apCallback)
{
	mvLines.clear();
	
    mpFont->GetWordWrapRows(500,mvFontSize.y+2,mvFontSize, gpBase->mpHelpFuncs->ParseString(asText),&mvLines);
	mbMessageYesNo = abYesNo;
	mpCallback = apCallback;

	SetPauseMessageActive(true);
}

//-----------------------------------------------------------------------

void cLuxMessageHandler::SetMessage(const tWString& asText, float afTime)
{
	tWString sParsedText = gpBase->mpHelpFuncs->ParseString(asText);
	mfMessageAlpha =0.0f;
	mfMessageTime = afTime <=0 ? gpBase->mpHelpFuncs->GetStringDuration(sParsedText) : afTime;
	
	mvMessageRows.clear();

	mpFont->GetWordWrapRows(700,mvFontSize.y+2,mvFontSize, sParsedText, &mvMessageRows);
}

//-----------------------------------------------------------------------

void cLuxMessageHandler::Update(float afTimeStep)
{
	////////////////////////////////
	// Message
	if(mfMessageTime > 0)
	{
		mfMessageTime -= afTimeStep;
		
		mfMessageAlpha += afTimeStep*1;
		if(mfMessageAlpha > 1) mfMessageAlpha =1;
	}
	else if(mfMessageAlpha > 0)
	{
		mfMessageAlpha -= afTimeStep * 1.5f;
		if(mfMessageAlpha < 0) mfMessageAlpha =0;
	}

	////////////////////////////////
	// Quest message
	if(mbQuestMessageActive)
	{
		if(mfQuestMessageAlpha < 0.1)
		{
			mfQuestMessageAlpha += afTimeStep;
			if(mfQuestMessageAlpha >= 0.1)
				gpBase->mpHelpFuncs->PlayGuiSoundData(msQuestAddedSound, eSoundEntryType_Gui); //Have it here so that is not played directly!
		}
		else if(mfQuestMessageAlpha < 1)
		{
			mfQuestMessageAlpha += afTimeStep;
			if(mfQuestMessageAlpha > 1)
			{
				mfQuestMessageAlpha =1;
			}
		}
		else
		{
			mfQuestMessageTime += afTimeStep;
			if(mfQuestMessageTime > 3)
			{
				mbQuestMessageActive = false;
			}
		}
	}
	else if(mfQuestMessageAlpha > 0)
	{
		mfQuestMessageAlpha -= afTimeStep;
		if(mfQuestMessageAlpha <0) mfQuestMessageAlpha =0;
	}

	if(mfQuestMessageAlpha >0) mQuestOscill.Update(afTimeStep);


	////////////////////////////////
	// Pause message
	if(mbPauseMessageActive)
	{
		mfPauseMessageAlpha += afTimeStep * 2.0f;
		if(mfPauseMessageAlpha > 1)mfPauseMessageAlpha = 1;
	}
	else
	{
		mfPauseMessageAlpha -= afTimeStep * 2.0f;
		if(mfPauseMessageAlpha < 0) mfPauseMessageAlpha =0;
	}
}

//-----------------------------------------------------------------------

void cLuxMessageHandler::OnDraw(float afFrameTime)
{
	DrawMessage();
	DrawPauseMessage();
	DrawQuestAdded();
}
//-----------------------------------------------------------------------

void cLuxMessageHandler::DoAction(eLuxPlayerAction aAction, bool abPressed)
{
    if(abPressed)
	{
		SetPauseMessageActive(false);

		if(mbMessageYesNo)
		{
			if(aAction == eLuxPlayerAction_Interact)
			{
				if(mpCallback) mpCallback->OnPress(true);
			}
			else if(aAction == eLuxPlayerAction_Attack)
			{
				if(mpCallback) mpCallback->OnPress(false);
			}
		}
	}
}

//-----------------------------------------------------------------------

void cLuxMessageHandler::SetPauseMessageActive(bool abX)
{
	if(mbPauseMessageActive == abX) return;

	mbPauseMessageActive = abX;

	gpBase->mpMapHandler->SetUpdateActive(mbPauseMessageActive? false : true);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMessageHandler::DrawQuestAdded()
{
	if(mfQuestMessageAlpha <= 0 || mpQuestAddedIcon==NULL) return;

	cVector2f vPos2D = gpBase->mvHudVirtualCenterSize - mpQuestAddedIcon->GetActiveSize() - cVector2f(20,20);
	cVector3f vPos(vPos2D.x, vPos2D.y, 10);
	vPos -= gpBase->mvHudVirtualStartPos;//minus since coordinates are negative!

	gpBase->mpGameHudSet->DrawGfx(mpQuestAddedIcon, vPos, -1, cColor(1, mfQuestMessageAlpha));
	
	for(int i=0; i<2; ++i)
		gpBase->mpGameHudSet->DrawGfx(mpQuestAddedIcon, vPos+cVector3f(0,0,1), -1, cColor(mfQuestMessageAlpha*mQuestOscill.val, 1), eGuiMaterial_Additive);
}

//-----------------------------------------------------------------------

void cLuxMessageHandler::DrawMessage()
{
	if(mfMessageAlpha <= 0) return;

	float fAlpha = mfMessageAlpha;
	if(mfPauseMessageAlpha > 0)
	{
		fAlpha = mfMessageAlpha * (1-mfPauseMessageAlpha);
	}
	
	cVector3f vTextPos = cVector3f(400, 345, 4);
	for(size_t i=0; i<mvMessageRows.size(); ++i)
	{
		gpBase->mpGameHudSet->DrawFont(mvMessageRows[i], mpFont,vTextPos,mvFontSize, cColor(1, fAlpha),eFontAlign_Center);
		vTextPos.y += mvFontSize.y+2;
	}
	
}

//-----------------------------------------------------------------------

void cLuxMessageHandler::DrawPauseMessage()
{
	if(mfPauseMessageAlpha <= 0) return;

	////////////////////////
	// Black background
	gpBase->mpGameHudSet->DrawGfx(mpBlackGfx,cVector3f(0,0,3),cVector2f(800,600),cColor(1, mfPauseMessageAlpha*0.5f));

	////////////////////////
	// Text
	cVector3f vTextPos = cVector3f(150, 200, 4);
	for(size_t i=0; i< mvLines.size(); ++i)
	{
		gpBase->mpGameHudSet->DrawFont(mvLines[i], mpFont,vTextPos,mvFontSize, cColor(1, mfPauseMessageAlpha));

		vTextPos.y += mvFontSize.y+2;
	}

	////////////////////////
	// Yes / No
	if(mbMessageYesNo)
	{
		vTextPos.y += 10.0f;
		vTextPos.x = 170;

		gpBase->mpGameHudSet->DrawFont(mpFont,vTextPos,mvFontSize, cColor(1, mfPauseMessageAlpha),_W("Yes"));

		vTextPos.x += 80;
		gpBase->mpGameHudSet->DrawFont(mpFont,vTextPos,mvFontSize-cVector2f(1), cColor(1, mfPauseMessageAlpha),_W("(%ls)"), gpBase->mpInputHandler->GetInputName("Interact").c_str());


		vTextPos.x = 170;
		vTextPos.y += mvFontSize.y+4;
		gpBase->mpGameHudSet->DrawFont(mpFont,vTextPos,mvFontSize, cColor(1, mfPauseMessageAlpha),_W("No"));

		vTextPos.x += 80;
		gpBase->mpGameHudSet->DrawFont(mpFont,vTextPos,mvFontSize-cVector2f(1), cColor(1, mfPauseMessageAlpha),_W("(%ls)"), gpBase->mpInputHandler->GetInputName("Attack").c_str());

	}
}
