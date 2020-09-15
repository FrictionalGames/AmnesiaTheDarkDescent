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

#ifndef HPL_GUI_MATERIAL_H
#define HPL_GUI_MATERIAL_H

#include "system/SystemTypes.h"


namespace hpl {

	class iLowLevelGraphics;
	
	class iGuiMaterial
	{
	public:
		iGuiMaterial(const tString &asName,iLowLevelGraphics *apLowLevelGraphics) :
						msName(asName), mpLowLevelGraphics(apLowLevelGraphics) {}
		virtual ~iGuiMaterial(){}

		virtual void BeforeRender()=0;
		virtual void AfterRender()=0;

		const tString& GetName() const {return msName;}

	protected:
		tString msName;
		iLowLevelGraphics *mpLowLevelGraphics;
	};

};
#endif // HPL_GUI_MATERIAL_H
