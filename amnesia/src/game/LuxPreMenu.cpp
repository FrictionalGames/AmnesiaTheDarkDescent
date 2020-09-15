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

#include "LuxPreMenu.h"

#include "LuxMapHandler.h"
#include "LuxInputHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxConfigHandler.h"
#include "LuxDebugHandler.h"
#include "LuxLoadScreenHandler.h"

//-----------------------------------------------------------------------

static eFontAlign ToFontAlign(const tString& asX)
{
	tString sLowerCase = cString::ToLowerCase(asX);
	if(sLowerCase == "center")
		return eFontAlign_Center;
	else if(sLowerCase == "right")
		return eFontAlign_Right;
	else if(sLowerCase == "left")
		return eFontAlign_Left;

	Error("The font alignment '%s' does not exist!\n", asX.c_str());
	return eFontAlign_Left;
}

//-----------------------------------------------------------------------

cLuxPreMenuSection::cLuxPreMenuSection()
{
}

cLuxPreMenuSection::~cLuxPreMenuSection()
{
	STLDeleteAll(mlstTextElements);
}

//-----------------------------------------------------------------------

void cLuxPreMenuSection::AddTextElement(cLuxPreMenuTextElement* apText)
{
	if(apText) mlstTextElements.push_back(apText);
}

//-----------------------------------------------------------------------

bool cLuxPreMenuSection::HasTextElements()
{
	return mlstTextElements.empty()==false;
}

//-----------------------------------------------------------------------

cGuiGfxElement* cLuxPreMenuSection::CreateBackground(cGui* apGui, cTextureManager* apTexMgr)
{
	cGuiGfxElement* pBG = NULL;
	// Load background, or create black rect if no file is loaded
	if(msBackgroundFile=="")
	{
		pBG  = apGui->CreateGfxFilledRect(mBackgroundColor, eGuiMaterial_Alpha);
	}
	else
	{
		iTexture* pTex = apTexMgr->Create2D(msBackgroundFile, false, eTextureType_Rect);
		if(pTex) pBG = apGui->CreateGfxTexture(pTex, true, eGuiMaterial_Alpha);
	}

	return pBG;
}

//-----------------------------------------------------------------------

bool cLuxPreMenuSection::Load(cXmlElement* apElement, const cVector2f& avGuiSetSize)
{
	if(apElement==NULL)
		return false;

	mBackgroundColor = apElement->GetAttributeColor("Color", cColor(0,1));
	msBackgroundFile = apElement->GetAttributeString("Image", ""); 
	mfTime = apElement->GetAttributeFloat("Time", 2);
	mbShowFirstStartOnly = apElement->GetAttributeBool("ShowFirstStartOnly", false);
	mbHasGammaSettings = apElement->GetAttributeBool("HasGammaSettings", false);
	
	msMusic = apElement->GetAttributeString("Music", "");
	mfMusicVolume = apElement->GetAttributeFloat("MusicVolume", 0);
	mfMusicFadeTime = apElement->GetAttributeFloat("MusicFadeTime", 0);

	mbAllowSkipping = apElement->GetAttributeBool("AllowSkipping", true);
				
	//////////////////////////////
	// Retrieve text elements
	cXmlNodeListIterator itText = apElement->GetChildIterator();
	while(itText.HasNext())
	{
		cXmlElement* pTextElement = itText.Next()->ToElement();

		// Create and set up text element
		cLuxPreMenuTextElement* pText = hplNew(cLuxPreMenuTextElement,());
		if(pText->Load(pTextElement, avGuiSetSize))
			AddTextElement(pText);
		else
			hplDelete(pText);
	}

	return true;
}

//-----------------------------------------------------------------------

bool cLuxPreMenuTextElement::Load(cXmlElement* apElement, const cVector2f& avGuiSetSize)
{
	if(apElement==NULL)
		return false;

	// Load text data
	tString sCat = apElement->GetAttributeString("TextCat");
	tString sEntry = apElement->GetAttributeString("TextEntry");
	
	msText = kTranslate(sCat, sEntry);
	mvPos = apElement->GetAttributeVector3f("Pos") + cVector3f(0,0,1);
	mvFrameSize = apElement->GetAttributeVector2f("FrameSize");
	mvFontSize = apElement->GetAttributeVector2f("FontSize", cVector2f(0));
	mColor = apElement->GetAttributeColor("Color");
	mAlign = ToFontAlign(apElement->GetAttributeString("Align", "Left"));

	mfTime = apElement->GetAttributeFloat("time", 2);

	return true;
}

//-----------------------------------------------------------------------

cWidgetLabel* cLuxPreMenuTextElement::CreateLabel(cGuiSet* apSet)
{
	if(apSet==NULL)	return NULL;

	cWidgetLabel* pLabel = apSet->CreateWidgetLabel(mvPos, mvFrameSize, msText);
	pLabel->SetDefaultFontColor(mColor);
	pLabel->SetDefaultFontSize(mvFontSize);
	pLabel->SetTextAlign(mAlign);
	pLabel->SetWordWrap(true);

	return pLabel;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPreMenu::cLuxPreMenu() : iLuxUpdateable("LuxPreMenu")
{
	///////////////////////////////
	//Get Engine
	mpGraphics = gpBase->mpEngine->GetGraphics();
	mpGui = gpBase->mpEngine->GetGui();
	mpGuiSet = mpGui->CreateSet("PreMenu", NULL);
	mpGuiSet->SetDrawMouse(false);//Init

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
	mpGuiSet->SetDrawMouse(false);//Init

	mpViewport->AddGuiSet(mpGuiSet);

	///////////////////////////////
	//Setup variables
	Reset();

	mpBlackFade = mpGui->CreateGfxFilledRect(cColor(0,1), eGuiMaterial_Alpha, true);
	mbExitPreMenu = false;

	///////////////////////////////////////
	// Load skin and sections
	LoadPreMenuSections();

	///////////////////////////////////////
	// Create continue button
	tWString sButtonLabel = kTranslate("PreMenu", "Continue");

	mpBContinue = mpGuiSet->CreateWidgetButton(0,0, sButtonLabel, NULL);
	iFontData* pFont = mpBContinue->GetDefaultFontType();

	float fLength = pFont->GetLength(cVector2f(pFont->GetHeight()), sButtonLabel.c_str()) + 20;

	cVector2f vSize = cVector2f(fLength, pFont->GetHeight()) + 10;
	cVector3f vPos = (cVector3f(mvGuiSetCenterSize-vSize) + cVector3f(0,-10,0.1f)) * cVector3f(0.5f, 1, 1);

	mpBContinue->SetPosition(vPos);
	mpBContinue->SetSize(vSize);

	mpBContinue->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Continue_Pressed));
	mpBContinue->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(Continue_UIPressed));
	mpBContinue->AddCallback(eGuiMessage_UIArrowPress, this, kGuiCallback(Gamma_UIArrowPressed));
	mpBContinue->SetGlobalUIInputListener(true);
	mpBContinue->SetVisible(false);

	///////////////////////////////////////
	// Create gamma widgets
	{
		cVector2f vCenter = mvGuiSetCenterSize*0.5f;
		mpGGamma = mpGuiSet->CreateWidgetDummy(0);

		// Preview image
		mpIGammaPreview = mpGuiSet->CreateWidgetImage("menu_gamma.tga", 0, -1, eGuiMaterial_Alpha, false, mpGGamma);

		vSize = mpIGammaPreview->GetSize() * 1.25f;
		//vPos = (cVector3f(mvGuiSetCenterSize-vSize) + cVector3f(0,40,0.1f)) * cVector3f(0.5f, 0.5f, 1);

		mpIGammaPreview->SetPosition(cVector3f(0,0,0.1f));
		mpIGammaPreview->SetSize(vSize);

		mpIGammaPreview->SetVisible(false);
		mpIGammaPreview->SetEnabled(false);

		// Value label
		mpLGamma = mpGuiSet->CreateWidgetLabel(0, 0, _W(""), mpGGamma);

		mpLGamma->SetVisible(false);
		mpLGamma->SetEnabled(false);

		// Slider
		mfGammaMinValue = 0.3f;
		mfGammaMaxValue = 2.0f;
		mfGammaStep = 0.05f;

		vSize.y = 25;

		mpSGamma = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, vPos, vSize, 10, mpGGamma);
		mpSGamma->AddCallback(eGuiMessage_SliderMove, this, kGuiCallback(Gamma_ChangeValue));

		int lMaxValue = cMath::RoundToInt((mfGammaMaxValue-mfGammaMinValue)/mfGammaStep);
		mpSGamma->SetMaxValue(lMaxValue);
		mpSGamma->SetBarValueSize(cMath::RoundToInt(0.25f*(float)lMaxValue));

		// Set initial value
		SetGammaValueToInput(gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetGammaCorrection(), true);

		mpSGamma->SetVisible(false);
		mpSGamma->SetEnabled(false);

		vPos = cVector3f(0,0,0.1f);
		mpIGammaPreview->SetPosition(vPos);

		vPos.y += mpIGammaPreview->GetSize().y + 10;
		mpLGamma->SetPosition(vPos);

		vPos.y += 25.0f;
		mpSGamma->SetPosition(vPos);

		vPos = cVector3f(vCenter-(mpGGamma->GetSize()*0.5f)) + cVector3f(0,20,0.1f);
		mpGGamma->SetPosition(vPos);
	}

	mpGuiSet->SetDefaultFocusNavWidget(mpBContinue);
	mpGuiSet->SetFocusedWidget(mpBContinue);
}

//-----------------------------------------------------------------------

cLuxPreMenu::~cLuxPreMenu()
{
	//Destroy data
	STLDeleteAll(mvSections);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPreMenu::OnEnterContainer(const tString& asOldContainer)
{
	//Unlock input if not in window
	if(gpBase->mpConfigHandler->mbFullscreen==false) {
		gpBase->mpEngine->GetInput()->GetLowLevel()->LockInput(false);
	}
	gpBase->mpEngine->GetInput()->GetLowLevel()->RelativeMouse(false);

	////////////////////////////
	//Set up states and viewport
	gpBase->mpInputHandler->ChangeState(eLuxInputState_PreMenu);

	mpViewport->SetActive(true);
	mpViewport->SetVisible(true);

	mpGuiSet->SetActive(true);
	mpGui->SetFocus(mpGuiSet);

	///////////////////////////////
	//Load data
	mlCurrentSectionIdx = -1;
	mpCurrentBackground = NULL;
	mpCurrentSection = NULL;

	mCurrentState = eLuxPreMenuState_Initial;
}

//-----------------------------------------------------------------------

void cLuxPreMenu::OnLeaveContainer(const tString& asNewContainer)
{
	//Unlock input if not in window
	if (gpBase->mpDebugHandler->GetDebugWindowActive()==false)
	{
		if(	gpBase->mpConfigHandler->mbFullscreen==false)
		{
			gpBase->mpEngine->GetInput()->GetLowLevel()->LockInput(true);
		}
		gpBase->mpEngine->GetInput()->GetLowLevel()->RelativeMouse(true);
	}
	
	//Turn off viewport and sets
    mpViewport->SetActive(false);
	mpViewport->SetVisible(false);

	mpGuiSet->SetActive(false);

	gpBase->mpLoadScreenHandler->DrawMenuScreen();
}

//-----------------------------------------------------------------------

void cLuxPreMenu::Update(float afTimeStep)
{
	UpdateActions(afTimeStep);
	UpdateState();

	////////////////////////////////////////////////////////
	// Exit premenu
	if(mbExitPreMenu)
	{
		if(gpBase->mbShowMenu)
		{
			gpBase->mpEngine->GetUpdater()->SetContainer("MainMenu");
		}
		else
		{
			gpBase->mpInputHandler->ChangeState(eLuxInputState_Game);

			//Create (if not existing) and set profile.
			gpBase->CreateProfile(gpBase->msDefaultProfileName);
			gpBase->SetProfile(gpBase->msDefaultProfileName);

			//Load user config
			if(gpBase->InitUserConfig()==false)
			{
				gpBase->Exit();
				return;
			}

			gpBase->mpEngine->GetUpdater()->SetContainer("Default");

			//Load map and start game.
			//By using "" user config values are used.
			gpBase->StartGame("", "", "");
		}
	}
}

//-----------------------------------------------------------------------

void cLuxPreMenu::SetGammaValueToInput(float afGamma, bool abGenCallback)
{
	afGamma = cMath::Clamp(afGamma, mfGammaMinValue, mfGammaMaxValue);

	float fMaxSliderValue = (float) mpSGamma->GetMaxValue();
	float fRange = mfGammaMaxValue-mfGammaMinValue;

	int lValue = cMath::RoundToInt((afGamma-mfGammaMinValue)*fMaxSliderValue/fRange);
	
	mpSGamma->SetValue(lValue, abGenCallback);
}

//-----------------------------------------------------------------------

bool cLuxPreMenu::Gamma_ChangeValue(iWidget* apWidget, const cGuiMessageData& aData)
{
	float fSliderRelValue = ((float)mpSGamma->GetValue())/(float)mpSGamma->GetMaxValue();
	float fRange = mfGammaMaxValue-mfGammaMinValue;

	float fGamma = mfGammaMinValue + fRange*fSliderRelValue;

	mpLGamma->SetText(kTranslate("OptionsMenu","Gamma") + _W(": ") + cString::ToStringW(fGamma, 2, true));
	gpBase->mpEngine->GetGraphics()->GetLowLevel()->SetGammaCorrection(fGamma);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxPreMenu, Gamma_ChangeValue);

//-----------------------------------------------------------------------

bool cLuxPreMenu::Continue_Pressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	mCurrentState = eLuxPreMenuState_FadeOut;

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxPreMenu, Continue_Pressed);

//-----------------------------------------------------------------------------

bool cLuxPreMenu::Continue_UIPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal == eUIButton_Primary)
	{
		mCurrentState = eLuxPreMenuState_FadeOut;

		return true;
	}
	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxPreMenu, Continue_UIPressed);

//-----------------------------------------------------------------------------

bool cLuxPreMenu::Gamma_UIArrowPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal == eUIArrow_Left)
	{
		mpSGamma->SetValue(mpSGamma->GetValue() - 1, true);
	}
	else if(aData.mlVal == eUIArrow_Right)
	{
		mpSGamma->SetValue(mpSGamma->GetValue() + 1, true);
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxPreMenu, Gamma_UIArrowPressed);

//-----------------------------------------------------------------------

void cLuxPreMenu::ButtonPressed()
{
	if(mpCurrentSection && mpCurrentSection->mbAllowSkipping==false) return;
	
	mCurrentState = eLuxPreMenuState_FastFadeOut;
}

//-----------------------------------------------------------------------

void cLuxPreMenu::OnDraw(float afFrameTime)
{
	///////////////////////////////////////
	// Draw fade rect and background
	mpGuiSet->DrawGfx(mpBlackFade, mvGuiSetStartPos + cVector3f(0,0,2), mvGuiSetSize, cColor(1, mfAlphaFade));
	mpGuiSet->DrawGfx(mpBlackFade, mvGuiSetStartPos - cVector3f(0,0,2), mvGuiSetSize, cColor(1, 1));

	if(mpCurrentBackground)
	{
		//Draw the image with correct ratio, always filling out height-wise.
		cVector2f vSize = mpCurrentBackground->GetImageSize();
		float fRatio = vSize.x / vSize.y;
		float fRatioMul = fRatio / (4.0f/3.0f);

		mpGuiSet->DrawGfx(mpCurrentBackground, cVector3f(400*(1-fRatioMul),0,0), cVector2f(800*fRatioMul, 600),cColor(1,1));
	}
}

//-----------------------------------------------------------------------

void cLuxPreMenu::AppLostInputFocus()
{
	if(gpBase->mpMapHandler->MapIsLoaded()==false)
	{
		cMusicHandler* pMusHdlr = gpBase->mpEngine->GetSound()->GetMusicHandler();
		pMusHdlr->Pause();
	}
}

//-----------------------------------------------------------------------

void cLuxPreMenu::AppGotInputFocus()
{
	if(gpBase->mpMapHandler->MapIsLoaded()==false)
	{
		cMusicHandler* pMusHdlr = gpBase->mpEngine->GetSound()->GetMusicHandler();
		pMusHdlr->Resume();
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPreMenu::UpdateActions(float afTimeStep)
{
	//////////////////////////////////////////
	// Update stuff according to state
	switch(mCurrentState)
	{
	case eLuxPreMenuState_Initial:
		// Set initial fade value
		mfAlphaFade = 0;
		break;
	case eLuxPreMenuState_Final:
		{
			mbExitPreMenu = true;
		}
		break;

	case eLuxPreMenuState_FadeIn:
		{
			// Update alpha for fading in
			if(mfAlphaFade>0)
				mfAlphaFade -= afTimeStep;
			else
				mfAlphaFade = 0;
		}
		break;
	case eLuxPreMenuState_FadeOut:
		{
			// Update alpha for fading out
			if(mfAlphaFade<1)
				mfAlphaFade += afTimeStep;
			else
			{
				mfAlphaFade = 1;
			}
		}
		break;
	case eLuxPreMenuState_FastFadeOut:
		{
			// Fast fade out (2x)
			if(mfAlphaFade<1)
				mfAlphaFade += afTimeStep*2;
			else
			{
				mfAlphaFade = 1;
			}
		}
		break;
	case eLuxPreMenuState_ShowPremenuSection:
		{
			// Update pic show timer
			mfTimer -= afTimeStep;
		}
		break;
	}
}

//-----------------------------------------------------------------------

void cLuxPreMenu::UpdateState()
{
	cTextureManager* pTexMgr = gpBase->mpEngine->GetResources()->GetTextureManager();
	//////////////////////////////////////////////////////////////
	// Set next state according to conditions and current state
	switch(mCurrentState)
	{
	// Initial state, just change to next
	case eLuxPreMenuState_Initial:
		{
			mCurrentState = eLuxPreMenuState_FadeOut;
		}
		break;
	// Final state, no checks
	case eLuxPreMenuState_Final:
		{
		}
		break;
	// FadeIn state, check if fade is done
	case eLuxPreMenuState_FadeIn:
		{
			if(mfAlphaFade==0)
			{
				mfTimer = mpCurrentSection->GetTime();
				mCurrentState = eLuxPreMenuState_ShowPremenuSection;
			}
		}
		break;
	// (Fast)FadeOut state, check if fade is done, also sets up next section
	case eLuxPreMenuState_FadeOut:
	case eLuxPreMenuState_FastFadeOut:
		{
			if(mfAlphaFade==1)
			{
				if(mpCurrentBackground)
				{
					mpGui->DestroyGfx(mpCurrentBackground);
					mpCurrentBackground = NULL;
				}

				for(int i=0;i<(int)mvCurrentLabels.size();++i)
				{
					mpGuiSet->DestroyWidget(mvCurrentLabels[i]);
				}
				mvCurrentLabels.clear();

				// Fade is done, get next section, if no sections left, move to final state
				++mlCurrentSectionIdx;

				//If not first start, skip any sections that are only shown on firs start.
				if(gpBase->mpConfigHandler->mbFirstStart==false)
				{
					while(	mlCurrentSectionIdx<(int)mvSections.size() &&
							mvSections[mlCurrentSectionIdx]->ShowFirstStartOnly() )
					{
						++mlCurrentSectionIdx;
					}
				}

				////////////////////////////
				// All Sections Shown
				if(mlCurrentSectionIdx==(int)mvSections.size())
				{
					mCurrentState = eLuxPreMenuState_Final;
					mpCurrentSection = NULL;
				}
				////////////////////////////
				// New Sections
				else
				{
					mCurrentState = eLuxPreMenuState_FadeIn;

					mpCurrentSection = mvSections[mlCurrentSectionIdx];

					/////////////////////////////////
					// Set up current section
					if(mpCurrentSection)
					{
						bool bHasTextElements = mpCurrentSection->HasTextElements();
						bool bGuiSetActive = (bHasTextElements || mpCurrentSection->HasGammaSettings());

						////////////////////////////////////////////
						// Set up Background
						mpCurrentBackground = mpCurrentSection->CreateBackground(mpGui, pTexMgr);

						////////////////////////////////////////////
						// Set up Gui elements
						mpLGamma->SetVisible(mpCurrentSection->HasGammaSettings());
						mpLGamma->SetEnabled(mpCurrentSection->HasGammaSettings());
						mpSGamma->SetVisible(mpCurrentSection->HasGammaSettings());
						mpSGamma->SetEnabled(mpCurrentSection->HasGammaSettings());

						mpIGammaPreview->SetVisible(mpCurrentSection->HasGammaSettings());
						mpIGammaPreview->SetEnabled(mpCurrentSection->HasGammaSettings());

						mpBContinue->SetVisible(bGuiSetActive);
						mpBContinue->SetEnabled(bGuiSetActive);
						mpGuiSet->SetDrawMouse(bGuiSetActive);

						////////////////////////////////////////////
						// Set up text
						const tPreMenuTextList& lstText = mpCurrentSection->GetTextElements();
						tPreMenuTextList::const_iterator itText = lstText.begin();
						for(;itText!=lstText.end();++itText)
						{
							cLuxPreMenuTextElement* pText = *itText;

							// One label per element, word wrap enabled
							cWidgetLabel* pLabel = pText->CreateLabel(mpGuiSet);
							mvCurrentLabels.push_back(pLabel);
						}

						////////////////////////////////////////////
						// Play music
						if(mpCurrentSection->msMusic != "")
						{
							cMusicHandler* pMusHandler = gpBase->mpEngine->GetSound()->GetMusicHandler();

							float fFadeSpeed = mpCurrentSection->mfMusicFadeTime ==0 ? 100.0f : 1.0f / mpCurrentSection->mfMusicFadeTime;
							pMusHandler->Play(mpCurrentSection->msMusic, mpCurrentSection->mfMusicVolume, fFadeSpeed, true, false);
						}
					}
				}
			}
		}
		break;
		// ShowPremenuSection state, if no text, check if timer is done or some key was pressed
	case eLuxPreMenuState_ShowPremenuSection:
		{
			if(mpCurrentSection->HasTextElements()==false && 
				mpCurrentSection->HasGammaSettings()==false &&
				mfTimer <= 0)
			{
				mCurrentState = eLuxPreMenuState_FadeOut;
			}
		}
		break;
	}
}

//-----------------------------------------------------------------------

void cLuxPreMenu::LoadPreMenuSections()
{
	///////////////////////////////////////////////
	// First clear any previously loaded stuff
	cResources* pRes = gpBase->mpEngine->GetResources();

	//////////////////////////////////////////////////////////////////////////////
	// Check file pointed by config file and retrieve image files from there
	iXmlDocument* pDoc = pRes->GetLowLevel()->CreateXmlDocument();
	tWString sConfigPath = gpBase->msPreMenuConfigPath;
#ifdef USERDIR_RESOURCES
	if (gpBase->msUserResourceDir.length()
		&& cPlatform::FileExists(gpBase->msUserResourceDir + sConfigPath)) {
		sConfigPath = gpBase->msUserResourceDir + sConfigPath;
	}
#endif
	if(pDoc->CreateFromFile(sConfigPath))
	{
		/////////////////////////////////////////////////////
		// Set up skin
		tString sSkinFile = pDoc->GetAttributeString("skin", "gui_default.skin");
		mpGuiSet->SetSkin(mpGui->CreateSkin(sSkinFile));

		cXmlNodeListIterator it = pDoc->GetChildIterator();
		while(it.HasNext())
		{
			cXmlElement* pSectionElement = it.Next()->ToElement();

			cLuxPreMenuSection* pSection = hplNew(cLuxPreMenuSection, ());
			if(pSection->Load(pSectionElement, mvGuiSetCenterSize))
				mvSections.push_back(pSection);
			else
				hplDelete(pSection);
		}
	}

	pRes->DestroyXmlDocument(pDoc);
}

//-----------------------------------------------------------------------

