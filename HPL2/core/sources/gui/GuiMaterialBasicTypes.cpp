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

#include "gui/GuiMaterialBasicTypes.h"

#include "graphics/LowLevelGraphics.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// DIFFUSE
	//////////////////////////////////////////////////////////////////////////

	
	//-----------------------------------------------------------------------

	void cGuiMaterial_Diffuse::BeforeRender()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_Zero);
	}
	
	//-----------------------------------------------------------------------

	void cGuiMaterial_Diffuse::AfterRender()
	{
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// ALPHA
	//////////////////////////////////////////////////////////////////////////

	
	//-----------------------------------------------------------------------

	void cGuiMaterial_Alpha::BeforeRender()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

	void cGuiMaterial_Alpha::AfterRender()
	{
		//Not needed right?
		//mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// FONT NORMAL
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGuiMaterial_FontNormal::BeforeRender()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

	void cGuiMaterial_FontNormal::AfterRender()
	{
		//Not needed right?
		//mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// ADDITIVE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGuiMaterial_Additive::BeforeRender()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_One);
	}

	//-----------------------------------------------------------------------

	void cGuiMaterial_Additive::AfterRender()
	{
		//Not needed right?
		//mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// MODULATIVE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGuiMaterial_Modulative::BeforeRender()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc_DestColor, eBlendFunc_Zero);
	}

	//-----------------------------------------------------------------------

	void cGuiMaterial_Modulative::AfterRender()
	{
		//Not needed right?
		//mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
	}

    //-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PREMUL ALPHA
	//////////////////////////////////////////////////////////////////////////


	//-----------------------------------------------------------------------

	void cGuiMaterial_PremulAlpha::BeforeRender()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

	void cGuiMaterial_PremulAlpha::AfterRender()
	{
		//Not needed right?
		//mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

}
