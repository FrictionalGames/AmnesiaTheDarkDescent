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

#include "input/ActionGamepadAxis.h"
#include "input/Input.h"
#include "input/Gamepad.h"

#include "math/Math.h"
#include "system/String.h"

namespace hpl
{
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cActionGamepadAxis::cActionGamepadAxis(cInput* apInput, int alPadIndex, eGamepadAxis aAxis, eGamepadAxisRange aRange, float afMinThreshold, float afMaxThreshold)
	{
		mlPadIndex = alPadIndex;
		mAxis = aAxis;
		mRange = aRange;
		mfMinThreshold = afMinThreshold;
		mfMaxThreshold = afMaxThreshold;
		
		mpInput = apInput;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cActionGamepadAxis::IsTriggerd()
	{
		iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
		if(pPad)
		{
			float fVal = pPad->GetAxisValue(mAxis);
			bool bValid = (mRange==eGamepadAxisRange_Positive && fVal>0.0f) || 
							(mRange==eGamepadAxisRange_Negative && fVal<0.0f);
			
			fVal = cMath::Abs(fVal);
			if(bValid && mfMinThreshold<=fVal && fVal<=mfMaxThreshold)
				return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------

	float cActionGamepadAxis::GetValue()
	{
		iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
		if(IsTriggerd()) return cMath::Abs(pPad->GetAxisValue(mAxis));
		else return 0.0;
	}

	//-----------------------------------------------------------------------
	
	tString  cActionGamepadAxis::GetInputName()
	{
		return iGamepad::AxisToString(mAxis) + "." + iGamepad::AxisRangeToString(mRange);
		
		//iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
		//if(pPad)
		//	return pPad->AxisRangeToString(mRange) + " " + pPad->AxisToString(mAxis);
		
		//return "None";
	}

	//-----------------------------------------------------------------------
}
