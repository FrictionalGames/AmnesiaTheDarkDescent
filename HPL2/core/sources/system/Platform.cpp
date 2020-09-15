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

#include "system/Platform.h"
#include "system/LowLevelSystem.h"

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"

//---------------------------------------------------------------

namespace hpl
{

	//---------------------------------------------------------------

	/////////////////////////////////////////////////////////////////
	// Predicate for sorting video mode lists

	bool VideoComp::operator() (const cVideoMode& aVM1, const cVideoMode& aVM2) const
	{
		//Log("Comparing video mode (1) %dx%d/%d-%d to (2) %dx%d/%d-%d\n", aVM1.mvScreenSize.x, aVM1.mvScreenSize.y, 
																			//aVM1.mlBitsPerPixel, aVM1.mlRefreshRate,
		//													aVM2.mvScreenSize.x, aVM2.mvScreenSize.y, 
																			//aVM2.mlBitsPerPixel, aVM1.mlRefreshRate);

        if(aVM1.mlDisplay != aVM2.mlDisplay)
        {
            return aVM1.mlDisplay < aVM2.mlDisplay;
        }

		if(aVM1.mvScreenSize.x != aVM2.mvScreenSize.x)
		{
			//if(aVM1.mvScreenSize.x < aVM2.mvScreenSize.x)
			//	Log("(1) wins by width\n");
			//else
			//	Log("(2) wins by width\n");

			return aVM1.mvScreenSize.x < aVM2.mvScreenSize.x;
		}

		if(aVM1.mvScreenSize.y != aVM2.mvScreenSize.y)
		{
			//if(aVM1.mvScreenSize.y < aVM2.mvScreenSize.y)
			//	Log("(1) wins by height\n");
			//else
			//	Log("(2) wins by height\n");

			return aVM1.mvScreenSize.y < aVM2.mvScreenSize.y;
		}

//		if(aVM1.mlBitsPerPixel != aVM2.mlBitsPerPixel)
//		{
//			//if(aVM1.mlBitsPerPixel < aVM2.mlBitsPerPixel)
//			//	Log("(1) wins by BPP\n");
//			//else
//			//	Log("(2) wins by BPP\n");
//
//			return aVM1.mlBitsPerPixel < aVM2.mlBitsPerPixel;
//		}

//		if(aVM1.mlRefreshRate != aVM2.mlBitsPerPixel)
//		{
//			//if(aVM1.mlRefreshRate < aVM2.mlRefreshRate)
//			//	Log("(1) wins by refresh\n");
//			//else
//			//	Log("(2) wins by refresh\n");
//
//			return aVM1.mlRefreshRate < aVM2.mlRefreshRate;
//		}

		//Log("(1) & (2) are equal\n");

		return false;
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	void cPlatform::CreateMessageBox( eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, ...)
	{
		va_list ap;	

		if (fmt == NULL)
			return;	
		va_start(ap, fmt);
		CreateMessageBoxBase(eType, asCaption, fmt, ap);
		va_end(ap);
	}

	void cPlatform::CreateMessageBox( const wchar_t* asCaption, const wchar_t *fmt, ...)
	{
		va_list ap;	
		if (fmt == NULL)
			return;	
		va_start(ap, fmt);
		CreateMessageBoxBase( eMsgBoxType_Default, asCaption, fmt, ap );
		va_end(ap);
	}

	void cPlatform::CreateMessageBox( const wchar_t* asCaption, const wchar_t *fmt, va_list ap)
	{
		CreateMessageBoxBase(eMsgBoxType_Default, asCaption, fmt, ap);
	}

	void cPlatform::CreateMessageBox( eMsgBoxType eType, const wchar_t* asCaption, const wchar_t *fmt, va_list ap)
	{
		CreateMessageBoxBase(eType, asCaption, fmt, ap);
	}

	//---------------------------------------------------------------

}