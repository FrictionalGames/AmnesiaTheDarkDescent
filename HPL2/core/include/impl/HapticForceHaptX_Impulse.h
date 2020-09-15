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

#ifndef HPL_HAPTIC_FORCE_HAPTX_IMPULSE_H
#define HPL_HAPTIC_FORCE_HAPTX_IMPULSE_H

#include "impl/HapticForceHaptX.h"

#include <HaptX.h>

namespace hpl {

	class cHapticForceHaptX_Impulse : public iHapticForceHaptX
	{
	public:
		cHapticForceHaptX_Impulse(	HaptX::HaptXInterface *apInterface,
									HaptX::iHapticDeviceID	aDeviceID);
		~cHapticForceHaptX_Impulse();

		void SetForce(const cVector3f &avForce);
		void SetRelativeForce(const cVector3f &avForce);
		
	protected:
		HaptX::ForceModelImpulseInfo *mpImpulseForce;		
	};

};
#endif // HPL_HAPTIC_FORCE_HAPTX_H
