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

#ifndef HPL_HAPTIC_FORCE_H
#define HPL_HAPTIC_FORCE_H

#include "math/MathTypes.h"
#include "haptic/HapticTypes.h"

namespace hpl {

	class iHapticForce
	{
	public:
		iHapticForce(eHapticForceType aType) : mType(aType){}
		virtual ~iHapticForce(){}

		eHapticForceType GetType(){ return mType;}

		virtual void SetActive(bool abX)=0;
		virtual bool IsActive()=0;

		virtual void SetForce(const cVector3f &avForce)=0;
		virtual void SetRelativeForce(const cVector3f &avForce)=0;

		virtual void SetDirection(const cVector3f &avDir)=0;
		virtual void SetFreq(float afFreq)=0;
		virtual void SetAmp(float afAmp)=0;

		virtual void SetTimeControl(bool abLoop,float afTime,float afIdleTime, 
									float afFadeInTime, float afFadeOutTime)=0;
	
	protected:
		eHapticForceType mType;
	};

};
#endif // HPL_HAPTIC_FORCE_H
