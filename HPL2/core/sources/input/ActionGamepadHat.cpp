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

#include "input/ActionGamepadHat.h"
#include "input/Input.h"
#include "input/Gamepad.h"

#include "system/String.h"

namespace hpl
{
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cActionGamepadHat::cActionGamepadHat(cInput *apInput, int alPadIndex, eGamepadHat aHat, eGamepadHatState aHatState)
	{
		mlPadIndex = alPadIndex;
		mHat = aHat;
		mHatState = aHatState;
		mpInput = apInput;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cActionGamepadHat::IsTriggerd()
	{
		iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
		if(pPad)
			return pPad->HatIsInState(mHat, mHatState);

		return false;
	}

	//-----------------------------------------------------------------------

	float cActionGamepadHat::GetValue()
	{
		if(IsTriggerd())return 1.0;
		else return 0.0;
	}

	//-----------------------------------------------------------------------
	
	tString  cActionGamepadHat::GetInputName()
	{
		return iGamepad::HatToString(mHat) + "." + iGamepad::HatStateToString(mHatState);
		/*iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
		if(pPad)
			return pPad->HatToString(mHat) + "-" + pPad->HatStateToString(mHatState);

		return "None";
		*/
	}

	//-----------------------------------------------------------------------
}
