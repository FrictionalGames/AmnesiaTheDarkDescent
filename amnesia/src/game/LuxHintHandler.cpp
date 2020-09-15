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

#include "LuxHintHandler.h"

#include "LuxHelpFuncs.h"
#include "input/Gamepad.h"

#include <stdarg.h>
#include <stdlib.h>

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxHintHandler::cLuxHintHandler() : iLuxUpdateable("LuxHintHandler")
{
	mpFont = NULL;

	mfYPos = gpBase->mpMenuCfg->GetFloat("Hints","YPos",0);
	mvFontSize = gpBase->mpMenuCfg->GetVector2f("Hints","FontSize",0);

	mfTextDurationMul = gpBase->mpMenuCfg->GetFloat("Hints","TextDurationMul",0);

	mHintOscill.SetUp(0, 1, 0, 1, 1);

	//////////////
	// Set up icons for each gamepad button
	cGuiSet* pGuiSet = gpBase->mpGameHudSet;
	cGui* pGui = pGuiSet->GetGui();
#ifdef USE_GAMEPAD
	for(int i = 0; i < eGamepadButton_LastEnum; ++i)
	{
		//////////
		// Reset all the buttons
		mpGamepadButtonIcon[i] = NULL;
	}

# if USE_SDL2
    mpGamepadButtonIcon[eGamepadButton_A]             = pGui->CreateGfxTexture("gamepad_a.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_B]             = pGui->CreateGfxTexture("gamepad_b.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_X]             = pGui->CreateGfxTexture("gamepad_x.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_Y]             = pGui->CreateGfxTexture("gamepad_y.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_LeftShoulder]  = pGui->CreateGfxTexture("gamepad_lb.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_RightShoulder] = pGui->CreateGfxTexture("gamepad_rb.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_Back]          = pGui->CreateGfxTexture("gamepad_select.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_Start]         = pGui->CreateGfxTexture("gamepad_start.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_LeftStick]     = pGui->CreateGfxTexture("gamepad_ls.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_RightStick]    = pGui->CreateGfxTexture("gamepad_rs.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_DpadUp]        = pGui->CreateGfxTexture("gamepad_dpad_u.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_DpadRight]     = pGui->CreateGfxTexture("gamepad_dpad_r.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_DpadLeft]      = pGui->CreateGfxTexture("gamepad_dpad_l.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_DpadDown]      = pGui->CreateGfxTexture("gamepad_dpad_d.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);

	//////////
	// Dpad
	mpGamepadHatIcon[cMath::Log2ToInt(eGamepadHatState_Up)]    = pGui->CreateGfxTexture("gamepad_dpad_u.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadHatIcon[cMath::Log2ToInt(eGamepadHatState_Right)] = pGui->CreateGfxTexture("gamepad_dpad_r.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadHatIcon[cMath::Log2ToInt(eGamepadHatState_Left)]  = pGui->CreateGfxTexture("gamepad_dpad_l.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadHatIcon[cMath::Log2ToInt(eGamepadHatState_Down)]  = pGui->CreateGfxTexture("gamepad_dpad_d.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
# else
	//////////////
	// Set up the buttons
	mpGamepadButtonIcon[eGamepadButton_0] = pGui->CreateGfxTexture("gamepad_a.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_1] = pGui->CreateGfxTexture("gamepad_b.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_2] = pGui->CreateGfxTexture("gamepad_x.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_3] = pGui->CreateGfxTexture("gamepad_y.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_4] = pGui->CreateGfxTexture("gamepad_lb.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_5] = pGui->CreateGfxTexture("gamepad_rb.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_6] = pGui->CreateGfxTexture("gamepad_select.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_7] = pGui->CreateGfxTexture("gamepad_start.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_8] = pGui->CreateGfxTexture("gamepad_ls.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadButtonIcon[eGamepadButton_9] = pGui->CreateGfxTexture("gamepad_rs.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);

	//////////
	// Dpad
	mpGamepadHatIcon[cMath::Log2ToInt(eGamepadHatState_Up)]    = pGui->CreateGfxTexture("gamepad_dpad_u.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadHatIcon[cMath::Log2ToInt(eGamepadHatState_Right)] = pGui->CreateGfxTexture("gamepad_dpad_r.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadHatIcon[cMath::Log2ToInt(eGamepadHatState_Left)]  = pGui->CreateGfxTexture("gamepad_dpad_l.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGamepadHatIcon[cMath::Log2ToInt(eGamepadHatState_Down)]  = pGui->CreateGfxTexture("gamepad_dpad_d.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
# endif

	////////
	// Triggers
	for(int i = 0; i < eGamepadAxis_LastEnum; i++)
	{
		//////////
		// Reset all the sticks
		mpGampadAxisIcon[i * 2 + 0] = NULL;
		mpGampadAxisIcon[i * 2 + 1] = NULL;
	}

# if USE_SDL2
	mpGampadAxisIcon[eGamepadAxis_LeftTrigger * 2 + eGamepadAxisRange_Positive - 1]  = pGui->CreateGfxTexture("gamepad_lt.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGampadAxisIcon[eGamepadAxis_RightTrigger * 2 + eGamepadAxisRange_Positive - 1] = pGui->CreateGfxTexture("gamepad_rt.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
#else
	mpGampadAxisIcon[eGamepadAxis_2 * 2 + eGamepadAxisRange_Positive - 1] = pGui->CreateGfxTexture("gamepad_lt.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
	mpGampadAxisIcon[eGamepadAxis_2 * 2 + eGamepadAxisRange_Negative - 1] = pGui->CreateGfxTexture("gamepad_rt.tga", eGuiMaterial_Alpha, eTextureType_2D, cColor(1,1), false);
#endif

#endif
}

//-----------------------------------------------------------------------

cLuxHintHandler::~cLuxHintHandler()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxHintHandler::LoadFonts()
{
	tString sFontFile = gpBase->mpMenuCfg->GetString("Hints","DefaultFont","");
	mpFont = LoadFont(sFontFile);
}

//-----------------------------------------------------------------------

void cLuxHintHandler::OnStart()
{
	
}

//-----------------------------------------------------------------------

void cLuxHintHandler::Reset()
{
	mfAlpha =0;
	mfShowTextCount =0;
	msCurrentText = _W("");
	m_setHintsGiven.clear();
	m_setHintsBlocked.clear();
}

//-----------------------------------------------------------------------

void cLuxHintHandler::LoadUserConfig()
{
	mbActive = gpBase->mpUserConfig->GetBool("Game", "ShowHints", true);
}

//-----------------------------------------------------------------------

void cLuxHintHandler::SaveUserConfig()
{
	gpBase->mpUserConfig->SetBool("Game", "ShowHints", mbActive);
}

//-----------------------------------------------------------------------

void cLuxHintHandler::Update(float afTimeStep)
{
	UpdateHintText(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxHintHandler::OnDraw(float afFrameTime)
{
	DrawHintText(afFrameTime, gpBase->mpGameHudSet);
}

//-----------------------------------------------------------------------

void cLuxHintHandler::DrawHintText(float afFrameTime, cGuiSet *apGuiSet)
{
	if(gpBase->mbPTestActivated) return;

	if(mbActive==false || mfAlpha<=0) return;

	float fX = mHintOscill.val*0.5;
	cColor hintCol(1,fX,fX, mfAlpha);
	apGuiSet->DrawFont(mpFont, cVector3f(400,mfYPos,20),mvFontSize,hintCol, eFontAlign_Center, eGuiMaterial_FontNormal,
		_W(" %ls"), kTranslate("Hints", "HINT:").c_str());

	tWStringVec vRows;
	float fMaxWidth = 680;
	float fStartY = mfYPos + mvFontSize.y + 3.0f;
	mpFont->GetWordWrapRows(fMaxWidth,15, mvFontSize, msCurrentText, &vRows);
    if(vRows.size()==1)
	{
		apGuiSet->DrawFont(mpFont, cVector3f(400,fStartY,20),mvFontSize,cColor(1,mfAlpha), eFontAlign_Center, eGuiMaterial_FontNormal,
							_W(" %ls"), msCurrentText.c_str());
	}
	else
	{
		for(size_t i=0; i<vRows.size(); ++i)
		{
			apGuiSet->DrawFont(mpFont, cVector3f(400-fMaxWidth*0.5f, fStartY + (float)i* (mvFontSize.y+2.0f), 20),mvFontSize,cColor(1,mfAlpha), eFontAlign_Left, eGuiMaterial_FontNormal,
								_W(" %ls"), vRows[i].c_str());
		}
	}

#ifdef USE_GAMEPAD
	/////////////
	// Draw all the icons
	for(size_t i = 0; i < mvHintIcons.size(); ++i)
	{
		apGuiSet->DrawGfx(mvHintIcons[i].mpGuiIcon, mvHintIcons[i].mvPosition, mvHintIcons[i].mvSize, cColor(1,mfAlpha), eGuiMaterial_Alpha);
	}
#endif
}

void cLuxHintHandler::UpdateHintText(float afTimeStep)
{
	if(mbActive==false) return;

	if(mfAlpha>0) mHintOscill.Update(afTimeStep);

	/////////////////////
	//Decrease alpha
	if(mfShowTextCount<=0 && mfAlpha>0)
	{
		mfAlpha -= 0.5 * afTimeStep;
		if(mfAlpha <0) mfAlpha =0;
		return;
	}
	/////////////////////
	//Increase alpha
	if(mfShowTextCount > 0)
	{
		mfAlpha += 1.2f * afTimeStep;
		if(mfAlpha >1) mfAlpha =1;

		mfShowTextCount -= afTimeStep;
	}
}

//-----------------------------------------------------------------------

void cLuxHintHandler::SetActive(bool abX)
{
	mbActive = abX;

	if(mbActive==false)
	{
		mfAlpha =0;
		mfShowTextCount =0;
		msCurrentText = _W("");
	}
}

//-----------------------------------------------------------------------

bool cLuxHintHandler::Add(const tString &asName, const tWString &asMessage, float afTimeShown)
{
    if(mfShowTextCount >0) return false;

	if(m_setHintsBlocked.count(asName)>0) return false;

	tStringSetIt it = m_setHintsGiven.find(asName);
	if(it != m_setHintsGiven.end()) return false;

	m_setHintsGiven.insert(asName);

    msCurrentText = gpBase->mpHelpFuncs->ParseString(asMessage);
	mfShowTextCount = afTimeShown <=0 ? gpBase->mpHelpFuncs->GetStringDuration(msCurrentText)*mfTextDurationMul : afTimeShown;
	mfAlpha =0;

#ifdef USE_GAMEPAD
	ParseStringForGamepadIcons();
#endif

	return true;
}

//-----------------------------------------------------------------------

bool cLuxHintHandler::Remove(const tString &asName)
{
	tStringSetIt it = m_setHintsGiven.find(asName);
	if(it == m_setHintsGiven.end()) return false;

    m_setHintsGiven.erase(it);
	return true;
}

//-----------------------------------------------------------------------

void cLuxHintHandler::Block(const tString &asName)
{
	m_setHintsBlocked.insert(asName);
}

void cLuxHintHandler::UnBlock(const tString &asName)
{
	m_setHintsBlocked.erase(asName);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

#ifdef USE_GAMEPAD
void cLuxHintHandler::ParseStringForGamepadIcons()
{
	mvHintIcons.clear();

	tWString sOutput=_W("");
	tWString sCommand =_W("");
	bool bParseVar = false;
	
	int lPosition = 0;
 
	/////////////////
	// Find all the icons
    for(size_t i=0; i<msCurrentText.size(); ++i)
	{
		wchar_t lChar = msCurrentText[i];
		
		////////////////////////
		// Parse variable
		if(bParseVar)
		{
			// Add character to command
			if( lChar != '$' )
			{
				sCommand += lChar;
			}
			//Parse command
			else
			{
				///////////
				// Create the icon and get the replacement text for buttons that have no icon
				tWString sReplace = AddGamepadIconAtPosition(sCommand, lPosition);

				/////////
				// Make room for the icon
				sOutput += sReplace;
				lPosition += sReplace.size();

				bParseVar = false;
			}

		}
		////////////////////////
		// Normal parsing
		else
		{
			if(lChar == '$')
			{
                bParseVar = true;
				sCommand = _W("");
			}
			else
			{
				lPosition++;
				sOutput += lChar;
			}
		}
	}

	msCurrentText = sOutput;

	if(mvHintIcons.size() > 0)
	{
		size_t lIconIdx = 0;
		lPosition = 0;

		///////////////////
		// Get the correct size and position for each icon
		tWStringVec vRows;
		float fMaxWidth = 680;
		float fStartY = mfYPos + mvFontSize.y + 3.0f;
		mpFont->GetWordWrapRows(fMaxWidth,15, mvFontSize, msCurrentText, &vRows);
		wchar_t sRowText[1024];

		for(size_t i=0; i<vRows.size(); ++i)
		{
			///////////////
			// Convert the string
			swprintf(sRowText, 1023, _W(" %ls"), vRows[i].c_str());

			int lCount = 0;
			cVector3f vPos;

			if(vRows.size()==1)
			{
				vPos = cVector3f(400,fStartY,20);
				vPos.x -= mpFont->GetLength(mvFontSize, sRowText)/2;
			}
			else
			{
				vPos = cVector3f(400-fMaxWidth*0.5f, fStartY + (float)i* (mvFontSize.y+2.0f), 20);
			}

			//////////////////////////////////////////////////////
			// Iterate the characters in string until NULL is found
			while(sRowText[lCount] != 0)
			{
				wchar_t lGlyphNum = ((wchar_t)sRowText[lCount]);
			
				//Check if the glyph is valid (in range)
				if(	lGlyphNum < mpFont->GetFirstChar() || 
					lGlyphNum > mpFont->GetLastChar())
				{
					lCount++;
					continue;
				}
				//Get actual number of the glyph in the font.
				lGlyphNum -= mpFont->GetFirstChar();
			
				//Get glyph data and draw.
				cGlyph *pGlyph = mpFont->GetGlyph(lGlyphNum);

				if(pGlyph)
				{
					cVector2f vSize(pGlyph->mvSize * mvFontSize);
					vPos.x += pGlyph->mfAdvance*mvFontSize.x; 
				}

				if(lPosition == mvHintIcons[lIconIdx].mlCharacterPosition)
				{
					///////////////
					// Place the icon here
					mvHintIcons[lIconIdx].mvPosition = vPos;
					////////////
					// Set the size and aspect ratio
					mvHintIcons[lIconIdx].mvSize = mvHintIcons[lIconIdx].mpGuiIcon->GetImageSize();
					mvHintIcons[lIconIdx].mvSize /= mvHintIcons[lIconIdx].mvSize.y;
					mvHintIcons[lIconIdx].mvSize *= mvFontSize;

					if(++lIconIdx == mvHintIcons.size())
					{
						return;
					}
				}

				lPosition++;
				lCount++;
			}
		}
	}
}

//-----------------------------------------------------------------------

tWString cLuxHintHandler::AddGamepadIconAtPosition(const tWString& asCommand, int alPosition)
{
	////////////
	// Convert the command from wString to tString
	tString sCommand;
	sCommand.resize(asCommand.size());
	std::copy(asCommand.begin(), asCommand.end(), sCommand.begin());

	////////////////
	// Get icon from command string
	tString sSep = ".";
	tStringVec vInputParts;
	cString::GetStringVec(sCommand, vInputParts, &sSep);

	cLuxHintIcon hintIcon;
	hintIcon.mlCharacterPosition = alPosition;

	if(vInputParts[0] == "GamepadButton")
	{
		//////////
		// See if we have a icon for this button
		eGamepadButton button = iGamepad::StringToButton(vInputParts[1]);

		if(button < eGamepadButton_LastEnum)
		{
			if(mpGamepadButtonIcon[button])
			{
				hintIcon.mpGuiIcon = mpGamepadButtonIcon[button];
				mvHintIcons.push_back(hintIcon);

				if (gpBase->msCurrentLanguage == "chinese.lang")
					return _W("                ");
				return _W("    ");
			}
			else
			{
				/////////////
				// Return the translated command for buttons with no icon
				return kTranslate("ButtonNames", vInputParts[1]);
			}
		}
		else
		{
			return _W(" ");
		}
	}
	else
	{
		eGamepadHat hat = iGamepad::StringToHat(vInputParts[1]);
		eGamepadAxis axis = iGamepad::StringToAxis(vInputParts[1]);
								
		if(hat!=eGamepadHat_LastEnum)
		{
			eGamepadHatState state = iGamepad::StringToHatState(vInputParts[2]);

			if(mpGamepadHatIcon[cMath::Log2ToInt(state)])
			{
				hintIcon.mpGuiIcon = mpGamepadHatIcon[cMath::Log2ToInt(state)];
				mvHintIcons.push_back(hintIcon);
				return _W("    ");
			}
			else
			{
				/////////////
				// Return the translated command for hats with no icon
				return kTranslate("ButtonNames", vInputParts[1]+"."+vInputParts[2]);
			}
		}
		else if(axis!=eGamepadAxis_LastEnum)
		{
			eGamepadAxisRange range = iGamepad::StringToAxisRange(vInputParts[2]);

			if(mpGampadAxisIcon[axis * 2 + range - 1])
			{
				hintIcon.mpGuiIcon = mpGampadAxisIcon[axis * 2 + range - 1];
				mvHintIcons.push_back(hintIcon);
				return _W("      ");
			}
			else
			{
				/////////////
				// Return the translated command for hats with no icon
				return kTranslate("ButtonNames", vInputParts[1]+"."+vInputParts[2]);
			}
		}
		else
		{
			return _W(" ");
		}
	}

	return _W(" ");
}
#endif

//-----------------------------------------------------------------------
