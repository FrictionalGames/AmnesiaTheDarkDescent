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

#include "input/Mouse.h"

namespace hpl
{
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iMouse::iMouse(tString asName) : iInputDevice(asName, eInputDeviceType_Mouse)
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	tString iMouse::ButtonToString(eMouseButton aButton)
	{
		switch(aButton)
		{
			case eMouseButton_Left: return "LeftMouse";
			case eMouseButton_Middle: return "MiddleMouse";
			case eMouseButton_Right: return "RightMouse";
			case eMouseButton_WheelUp: return "WheelUp";
			case eMouseButton_WheelDown: return "WheelDown";
			case eMouseButton_Button6: return "Mouse6";
			case eMouseButton_Button7: return "Mouse7";
			case eMouseButton_Button8: return "Mouse8";
			case eMouseButton_Button9: return "Mouse9";
		}
		return "Unknown";
	}

	//-----------------------------------------------------------------------

	eMouseButton iMouse::StringToButton(const tString& asButton)
	{
		if(asButton=="LeftMouse") return eMouseButton_Left;
		if(asButton=="MiddleMouse") return eMouseButton_Middle;
		if(asButton=="RightMouse") return eMouseButton_Right;
		if(asButton=="WheelUp") return eMouseButton_WheelUp;
		if(asButton=="WheelDown") return eMouseButton_WheelDown;
		if(asButton=="Mouse6") return eMouseButton_Button6;
		if(asButton=="Mouse7") return eMouseButton_Button7;
		if(asButton=="Mouse8") return eMouseButton_Button8;
		if(asButton=="Mouse9") return eMouseButton_Button9;
		
		return eMouseButton_LastEnum;
	}

	//-----------------------------------------------------------------------

}
