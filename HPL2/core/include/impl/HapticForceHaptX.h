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

#ifndef HPL_HAPTIC_FORCE_HAPTX_H
#define HPL_HAPTIC_FORCE_HAPTX_H

#include "haptic/HapticForce.h"

#include <HaptX.h>

namespace hpl {

	class iHapticForceHaptX : public iHapticForce
	{
	public:
		iHapticForceHaptX(	eHapticForceType aType, HaptX::HaptXInterface *apInterface,
							HaptX::iHapticDeviceID	aDeviceID);
		virtual ~iHapticForceHaptX();

		void SetActive(bool abX);
		bool IsActive();

		virtual void SetForce(const cVector3f &avForce){}
		virtual void SetRelativeForce(const cVector3f &avForce){}

		virtual void SetDirection(const cVector3f &avForce){}
		virtual void SetFreq(float afFreq){}
		virtual void SetAmp(float afAmp){}

		void SetTimeControl(bool abLoop,float afTime,float afIdleTime, float afFadeInTime, float afFadeOutTime);
	
	protected:
		HaptX::HaptXInterface *mpInterface;
		HaptX::iHapticDeviceID	mDeviceID;
		HaptX::ForceModelInfo *mpForce;		
	};

};
#endif // HPL_HAPTIC_FORCE_HAPTX_H
