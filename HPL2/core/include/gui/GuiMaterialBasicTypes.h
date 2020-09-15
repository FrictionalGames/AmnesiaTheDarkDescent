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

#ifndef HPL_GUI_MATERIAL_BASIC_TYPES_H
#define HPL_GUI_MATERIAL_BASIC_TYPES_H

#include "gui/GuiMaterial.h"

namespace hpl {

	//-----------------------------------------
	
	class cGuiMaterial_Diffuse : public iGuiMaterial
	{
	public:
		cGuiMaterial_Diffuse(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("Diffuse",apLowLevelGraphics){}
		~cGuiMaterial_Diffuse(){}

		void BeforeRender();
		void AfterRender();
	};

	//-----------------------------------------

	class cGuiMaterial_Alpha : public iGuiMaterial
	{
	public:
		cGuiMaterial_Alpha(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("Alpha",apLowLevelGraphics){}
		~cGuiMaterial_Alpha(){}

		void BeforeRender();
		void AfterRender();
	};

	//-----------------------------------------

	class cGuiMaterial_FontNormal : public iGuiMaterial
	{
	public:
		cGuiMaterial_FontNormal(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("FontNormal",apLowLevelGraphics){}
		~cGuiMaterial_FontNormal(){}

		void BeforeRender();
		void AfterRender();
	};

	//-----------------------------------------

	class cGuiMaterial_Additive : public iGuiMaterial
	{
	public:
		cGuiMaterial_Additive(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("Additive",apLowLevelGraphics){}
		~cGuiMaterial_Additive(){}

		void BeforeRender();
		void AfterRender();
	};

	//-----------------------------------------

	class cGuiMaterial_Modulative : public iGuiMaterial
	{
	public:
		cGuiMaterial_Modulative(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("Modulative",apLowLevelGraphics){}
		~cGuiMaterial_Modulative(){}

		void BeforeRender();
		void AfterRender();
	};

	//-----------------------------------------
	
	class cGuiMaterial_PremulAlpha : public iGuiMaterial
	{
	public:
		cGuiMaterial_PremulAlpha(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("Alpha",apLowLevelGraphics){}
		~cGuiMaterial_PremulAlpha(){}

		void BeforeRender();
		void AfterRender();
	};

	//-----------------------------------------

};
#endif // HPL_GUI_MATERIAL_BASIC_TYPES_H
