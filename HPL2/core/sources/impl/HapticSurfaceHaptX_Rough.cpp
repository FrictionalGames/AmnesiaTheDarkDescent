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

#include "impl/HapticSurfaceHaptX_Rough.h"

#include "system/LowLevelSystem.h"

namespace hpl {

	using namespace HaptX;
	using namespace Vectors;

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cHapticSurfaceHaptX_Rough::cHapticSurfaceHaptX_Rough(const tString &asName,
									HaptX::HaptXInterface *apInterface,
									float afDeviation, float afMean,
									float afDamping, float afStiffness,
									float afDynamicFriction, float afStartingFriction,
									float afTangentStiffness, float afStoppingFriction)
		: iHapticSurfaceHaptX(asName, eHapticSurfaceType_Rough, apInterface)
	{
		mpInterface->SurfaceCreateRough(&mpRoughSurfaceInfo);
		mpSurfaceInfo = mpRoughSurfaceInfo;

		mpRoughSurfaceInfo->SetDeviation(afDeviation);
		mpRoughSurfaceInfo->SetMean(afMean);

		mpRoughSurfaceInfo->SetDamping(afDamping);
		mpRoughSurfaceInfo->SetStiffness(afStiffness);

		mpRoughSurfaceInfo->SetDynamicFriction(afDynamicFriction);
		mpRoughSurfaceInfo->SetStartingFriction(afStartingFriction);
		mpRoughSurfaceInfo->SetStiffnessT(afTangentStiffness);
		mpRoughSurfaceInfo->SetStoppingFriction(afStoppingFriction);
	}

	//-----------------------------------------------------------------------

	cHapticSurfaceHaptX_Rough::~cHapticSurfaceHaptX_Rough()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	//-----------------------------------------------------------------------

}
