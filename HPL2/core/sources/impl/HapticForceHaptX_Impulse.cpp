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

#include "impl/HapticForceHaptX_Impulse.h"

#include "system/LowLevelSystem.h"

namespace hpl {

	using namespace HaptX;
	using namespace Vectors;

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cHapticForceHaptX_Impulse::cHapticForceHaptX_Impulse(	HaptXInterface *apInterface,
															iHapticDeviceID	aDeviceID)
						: iHapticForceHaptX(eHapticForceType_Impulse,apInterface, aDeviceID)
	{
		mpInterface->ForceModelCreateImpulse(&mpImpulseForce,mDeviceID);
		mpForce = mpImpulseForce;
	}
	
	cHapticForceHaptX_Impulse::~cHapticForceHaptX_Impulse()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cHapticForceHaptX_Impulse::SetForce(const cVector3f &avForce)
	{
		float fMaxForce;
		mpInterface->DeviceGetMaxForce(fMaxForce,mDeviceID);
		mpImpulseForce->SetForce(Vec3f(avForce.x,avForce.y, avForce.z) / fMaxForce);
	}

	void cHapticForceHaptX_Impulse::SetRelativeForce(const cVector3f &avForce)
	{
		mpImpulseForce->SetForce(Vec3f(avForce.x,avForce.y, avForce.z));
	}
	
	//-----------------------------------------------------------------------

}
