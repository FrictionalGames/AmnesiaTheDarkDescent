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

#include "../Common/EdConfigHandler.h"

//------------------------------------------------------------


//------------------------------------------------------------

cEdConfigHandler::cEdConfigHandler(iEditor* apEditor) : iEdModule(apEditor, _W("ConfigHandler"))
{
}

//------------------------------------------------------------

//------------------------------------------------------------

//------------------------------------------------------------

void cEdConfigHandler::OnLoadGlobalConfig(cConfigFile* apCfg)
{
}

//------------------------------------------------------------

void cEdConfigHandler::OnLoadLocalConfig(cConfigFile* apCfg)
{
	mlScreenWidth = apCfg->GetInt("Screen", "Width", 1024);
	mlScreenHeight = apCfg->GetInt("Screen", "Height", 768);

	mTextureFilter = (eTextureFilter) apCfg->GetInt("Graphics", "TextureFilter", eTextureFilter_Trilinear);
	mlTextureQuality = apCfg->GetInt("Graphics", "TextureQuality", 0);
	mfTextureAnisotropy = apCfg->GetFloat("Graphics", "TextureAnisotropy", 1.0f);
	mShadowQuality = (eShadowMapQuality) apCfg->GetInt("Graphics", "ShadowQuality", eShadowMapQuality_Medium);
}

//------------------------------------------------------------

void cEdConfigHandler::OnSaveLocalConfig(cConfigFile* apCfg)
{
	apCfg->SetInt("Screen", "Width", mlScreenWidth);
	apCfg->SetInt("Screen", "Height", mlScreenHeight);

	apCfg->SetInt("Graphics", "TextureFilter", mTextureFilter);
	apCfg->SetInt("Graphics", "TextureQuality", mlTextureQuality);
	apCfg->SetFloat("Graphics", "TextureAnisotropy", mfTextureAnisotropy);
	apCfg->SetInt("Graphics", "ShadowQuality", mShadowQuality);
}

//------------------------------------------------------------

//------------------------------------------------------------
