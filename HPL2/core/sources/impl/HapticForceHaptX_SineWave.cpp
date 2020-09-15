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

#include "impl/HapticForceHaptX_SineWave.h"

#include "system/LowLevelSystem.h"

namespace hpl {

	using namespace HaptX;
	using namespace Vectors;

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cHapticForceHaptX_SineWave::cHapticForceHaptX_SineWave(	HaptXInterface *apInterface,
															iHapticDeviceID	aDeviceID,
															const cVector3f &avDirection,
															float afAmp, float afFreq)
						: iHapticForceHaptX(eHapticForceType_SineWave,apInterface, aDeviceID)
	{
		mpInterface->ForceModelCreateSineWave(&mpSineWaveForce,mDeviceID,
											Vec3f(avDirection.x,avDirection.y,avDirection.z),
											afAmp, afFreq);
		mpForce = mpSineWaveForce;
	}
	
	cHapticForceHaptX_SineWave::~cHapticForceHaptX_SineWave()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cHapticForceHaptX_SineWave::SetDirection(const cVector3f &avDir)
	{
		mpSineWaveForce->SetDirection(Vec3f(avDir.x,avDir.y,avDir.z));
	}
	void cHapticForceHaptX_SineWave::SetFreq(float afFreq)
	{
		mpSineWaveForce->SetFrequency(afFreq);
	}
	void cHapticForceHaptX_SineWave::SetAmp(float afAmp)
	{
		mpSineWaveForce->SetAmplitude(afAmp);
	}
	//-----------------------------------------------------------------------

}
