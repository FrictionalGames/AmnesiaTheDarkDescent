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

#include "LuxHelpFuncs.h"

#include "LuxMapHandler.h"
#include "LuxPlayer.h"
#include "LuxEffectHandler.h"
#include "LuxMap.h"
#include "LuxEntity.h"
#include "LuxInputHandler.h"

//-----------------------------------------------------------------------

cLuxModelCache::cLuxModelCache()
{

}

//-----------------------------------------------------------------------

cLuxModelCache::~cLuxModelCache()
{
	Destroy();
}

//-----------------------------------------------------------------------

void cLuxModelCache::Create()
{
	if(gpBase->mpMapHandler->GetCurrentMap()==NULL) return;

	///////////////////////////////
	// Textures
	cResourceBaseIterator texIt = gpBase->mpEngine->GetResources()->GetTextureManager()->GetResourceBaseIterator();
	while(texIt.HasNext())
	{
		iTexture *pTex = static_cast<iTexture*>(texIt.Next());
        pTex->IncUserCount();
		mlstTextureCache.push_back(pTex);
	}

	///////////////////////////////
	// Meshes and animations
	cLuxEntityIterator entIt = gpBase->mpMapHandler->GetCurrentMap()->GetEntityIterator();
	while(entIt.HasNext())
	{
		iLuxEntity *pEntity = entIt.Next();

		//Add mesh to list and increase user count.
		if(pEntity->GetMeshEntity())
		{
			cMeshEntity *pMeshEntity = pEntity->GetMeshEntity();
			cMesh *pMesh = pEntity->GetMeshEntity()->GetMesh();

			pMesh->IncUserCount();
			mlstMeshCache.push_back(pMesh);

			for(int i=0; i<pMeshEntity->GetAnimationStateNum(); ++i)
			{
				cAnimationState *pAnimState =pMeshEntity->GetAnimationState(i);
				if(pAnimState->DataIsInMeshFile()) continue; //Data will be saved with mesh!

				cAnimation *pAnim = pAnimState->GetAnimation();

				pAnim->IncUserCount();
				mlstAnimationCache.push_back(pAnim);
			}
		}
	}
}
//-----------------------------------------------------------------------

void cLuxModelCache::Destroy()
{
	for(std::list<cMesh*>::iterator it = mlstMeshCache.begin(); it != mlstMeshCache.end(); ++it)
	{
		gpBase->mpEngine->GetResources()->GetMeshManager()->Destroy(*it);
	}
    
	for(std::list<cAnimation*>::iterator it = mlstAnimationCache.begin(); it != mlstAnimationCache.end(); ++it)
	{
		gpBase->mpEngine->GetResources()->GetAnimationManager()->Destroy(*it);
	}

	for(std::list<iTexture*>::iterator it = mlstTextureCache.begin(); it != mlstTextureCache.end(); ++it)
	{
		gpBase->mpEngine->GetResources()->GetTextureManager()->Destroy(*it);
	}

	mlstMeshCache.clear();
	mlstAnimationCache.clear();
	mlstTextureCache.clear();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxHelpFuncs::cLuxHelpFuncs() : iLuxUpdateable("LuxHelpFuncs")
{
	mpSet = gpBase->mpEngine->GetGui()->CreateSet("DirectToScree", NULL);
	mpSet->SetActive(false);
	mpSet->SetVirtualSize(gpBase->mvHudVirtualSize,-1000, 1000, gpBase->mvHudVirtualOffset);

	mpLowLevelGfx = gpBase->mpEngine->GetGraphics()->GetLowLevel();

	mpFontDefault = NULL;

	mfTextDuration_StartTime = gpBase->mpMenuCfg->GetFloat("General", "TextDuration_StartTime",0);
	mfTextDuration_MinTime = gpBase->mpMenuCfg->GetFloat("General", "TextDuration_MinTime",0);
	mfTextDuration_CharTime = gpBase->mpMenuCfg->GetFloat("General", "TextDuration_CharTime",0);

	Reset();
}

//-----------------------------------------------------------------------

cLuxHelpFuncs::~cLuxHelpFuncs()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxHelpFuncs::LoadFonts()
{
	mpFontDefault = LoadFont("game_default.fnt");
}

//-----------------------------------------------------------------------

bool cLuxHelpFuncs::PlayGuiSoundData(const tString& asName,eSoundEntryType aDestType, float afVolMul, eSoundEntityType aSoundType, bool abSkipPreviousRandom,
									cLuxSoundExtraData *apOutputData)
{
	if(asName=="") return false;

	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
	cResources *pResources = gpBase->mpEngine->GetResources();
	
	cSoundEntityData *pSoundData = pResources->GetSoundEntityManager()->CreateSoundEntity(asName);
	if(pSoundData == NULL) return false;
	
	tString sSoundName = pSoundData->GetRandomSoundName(aSoundType, abSkipPreviousRandom);
	cSoundEntry *pSound = pSoundHandler->PlayGui(sSoundName, false, pSoundData->GetVolume()*afVolMul,cVector3f(0,0,1),aDestType);

	if(apOutputData)
	{
		apOutputData->mfMinDistance = pSoundData->GetMinDistance();
		apOutputData->mfMaxDistance = pSoundData->GetMaxDistance();
		apOutputData->mfVolume = pSoundData->GetVolume();
		apOutputData->mpSoundEntry = pSound;
	}

	return true;
}

//-----------------------------------------------------------------------

void cLuxHelpFuncs::DrawSetToScreen(bool abClearScreen, const cColor& aCol, cGuiSet* apSet)
{
	///////////////////////////
	// Clear screen
	if(abClearScreen)
	{
		mpLowLevelGfx->SetClearColor(aCol);
        mpLowLevelGfx->ClearFrameBuffer(eClearFrameBufferFlag_Color);		
	}

	///////////////////////////
	// Draw set
	cGuiSet* pSet = mpSet;
	if(apSet!=NULL)
		pSet = apSet;
		
	pSet->Render(NULL);
	pSet->ClearRenderObjects();
	
	mpLowLevelGfx->FlushRendering();
	mpLowLevelGfx->SwapBuffers();
}

//-----------------------------------------------------------------------

void cLuxHelpFuncs::CleanupData()
{
	//Destroy particle systems and sounds that are not in use.
	gpBase->mpEngine->GetResources()->GetSoundManager()->DestroyUnused(20);
	gpBase->mpEngine->GetResources()->GetParticleManager()->DestroyUnused(10);

}

//-----------------------------------------------------------------------

tWString cLuxHelpFuncs::ParseString(const tWString& asInput)
{
	tWString sOutput=_W("");
	tWString sCommand =_W("");
	bool bParseVar = false;

    for(size_t i=0; i<asInput.size(); ++i)
	{
		wchar_t lChar = asInput[i];
		
		////////////////////////
		// Parse variable
		if(bParseVar)
		{
			// Add character to command
			if( (lChar >= 'a' && lChar <= 'z') || (lChar >= 'A' && lChar <= 'Z') )
			{
				sCommand += lChar;
			}
			//Parse command
			else
			{
				sOutput += ParseStringCommand(sCommand);
				sOutput += lChar;
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
				sOutput += lChar;
			}
		}
	}

	return sOutput;
}

//-----------------------------------------------------------------------

float cLuxHelpFuncs::GetStringDuration(const tWString& asStr)
{
	float fTime = mfTextDuration_StartTime + (float)asStr.length() * mfTextDuration_CharTime;
	if(fTime < mfTextDuration_MinTime) fTime = mfTextDuration_MinTime;
	return fTime;
}

//-----------------------------------------------------------------------

void cLuxHelpFuncs::RenderBackgroundScreen(bool abDrawFullHUD)
{
	gpBase->mpMapHandler->GetViewport()->SetVisible(true);
	gpBase->mpGameHudSet->ClearRenderObjects();

	if(abDrawFullHUD==false)
	{
		gpBase->mpPlayer->RunHelperMessage(eUpdateableMessage_OnDraw,0.0001f);
	}
	else
	{
		gpBase->mpPlayer->OnDraw(0.01f);
		gpBase->mpEffectHandler->OnDraw(0.01f);
	}
	
    tFlag lFlags = tSceneRenderFlag_World | tSceneRenderFlag_PostEffects;
	if(abDrawFullHUD) lFlags |= tSceneRenderFlag_Gui;
	
	gpBase->mpEngine->GetScene()->Render(0.0001f, lFlags);
	
	if(abDrawFullHUD==false)
		gpBase->mpGameHudSet->Render(NULL);

	gpBase->mpGameHudSet->ClearRenderObjects();
	gpBase->mpMapHandler->GetViewport()->SetVisible(false);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


tWString cLuxHelpFuncs::ParseStringCommand(const tWString& asCommand)
{
	tWString sPrefix = cString::SubW(asCommand, 0, 3);
	
	////////////////////////////////
	// Button
	if(sPrefix == _W("But") )
	{
		tString sAction = cString::To8Char(cString::SubW(asCommand, 3));

		///////////////////////////
		// Get action
        cAction *pAction = gpBase->mpEngine->GetInput()->GetAction(sAction);
		if(pAction==NULL || pAction->GetSubActionNum()==0)
		{
			Error("String parser could not find action '%s' invalid!\n", sAction.c_str());
			return _W("BADACTION");			
		}

		///////////////////////////
		// Create string from action
		tWString sOutput = _W("");
		for(size_t i=0; i<pAction->GetSubActionNum(); ++i)
		{
			iSubAction *pSubAction = pAction->GetSubAction(i);
			if(i!=0) sOutput += _W(" / ");

#ifdef USE_GAMEPAD
			tString sType = pSubAction->GetInputType();
			////////////
			// 
			if(sType == "GamepadButton"
			|| sType == "GamepadAxis"
			|| sType == "GamepadHat")
			{
				tWString sConverted;

				///////////
				// Add a new format containing the gamepad type and button
				sOutput += _W("$");

				//////////
				// Convert the type to wstring and append
				size_t lSize = sType.length() + 1;
				sConverted.resize(lSize);
				std::copy(sType.begin(), sType.end(), sConverted.begin());

				sOutput += cString::SubW(sConverted, 0, lSize-1);
				sOutput += _W(".");

				//////////
				// Convert name to wstring and append
				tString sName = pSubAction->GetInputName();
				lSize = sName.length() + 1;
				sConverted.resize(lSize);
				std::copy(sName.begin(), sName.end(), sConverted.begin());

				sOutput += cString::SubW(sConverted, 0, lSize-1);
				
				//////
				// Enclose it with a $, since "." and " " is used in the names
				sOutput += _W("$");
			}
			else
			{
				sOutput += kTranslate("ButtonNames", pSubAction->GetInputName());
			}
#else
			sOutput += kTranslate("ButtonNames", pSubAction->GetInputName());
#endif
		}


		return sOutput;
	}
	else if(asCommand == _W("Look"))
	{
		tWString sOutput = _W("");
#ifdef USE_GAMEPAD
		sOutput += kTranslate("Hints", "Mouse");

		if(gpBase->mpInputHandler->IsGamepadPresent())
		{
			sOutput += +_W(" / ")+kTranslate("Hints", "Thumbstick");
		}
#else
		sOutput += kTranslate("Hints", "Mouse");
#endif
		return sOutput;
	}

	////////////////////////////////
	// Unknown Command
	Error("String parser command prefix '%s' in command '%s' invalid!\n", cString::To8Char(sPrefix).c_str(), cString::To8Char(asCommand).c_str());
	return _W("BADCOMMAND");
}

//-----------------------------------------------------------------------



