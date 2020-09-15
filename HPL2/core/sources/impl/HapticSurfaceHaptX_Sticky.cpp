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

#include "impl/HapticSurfaceHaptX_Sticky.h"

#include "system/LowLevelSystem.h"

namespace hpl {

	using namespace HaptX;
	using namespace Vectors;

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cHapticSurfaceHaptX_Sticky::cHapticSurfaceHaptX_Sticky(const tString &asName,
									HaptX::HaptXInterface *apInterface,
									float afDeadHeight, float afStickyStiffness,
									float afDamping, float afStiffness,
									float afDynamicFriction, float afStartingFriction,
									float afTangentStiffness, float afStoppingFriction)
		: iHapticSurfaceHaptX(asName, eHapticSurfaceType_Sticky, apInterface)
	{
		mpInterface->SurfaceCreateStickyFrictional(&mpStickySurfaceInfo);
		mpSurfaceInfo = mpStickySurfaceInfo;
		
        mpStickySurfaceInfo->SetDeadHeight(afDeadHeight);
		mpStickySurfaceInfo->SetStickyStiffness(afStickyStiffness);
		
		mpStickySurfaceInfo->SetDamping(afDamping);
		mpStickySurfaceInfo->SetStiffness(afStiffness);

		mpStickySurfaceInfo->SetDynamicFriction(afDynamicFriction);
		mpStickySurfaceInfo->SetStartingFriction(afStartingFriction);
		mpStickySurfaceInfo->SetStiffnessT(afTangentStiffness);
		mpStickySurfaceInfo->SetStoppingFriction(afStoppingFriction);
	}

	//-----------------------------------------------------------------------

	cHapticSurfaceHaptX_Sticky::~cHapticSurfaceHaptX_Sticky()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	//-----------------------------------------------------------------------

}
