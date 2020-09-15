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

#include "scene/FogArea.h"

#include "math/Math.h"

#include "resources/Resources.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFogArea::cFogArea(tString asName, cResources *apResources) : iRenderable(asName)
	{
		mColor = cColor(1,1);
		mvSize = 1;

		mfStart = 0;
		mfEnd = 10;
		mfFalloffExp = 1.0f;

		mbShowBacksideWhenOutside = true;
		mbShowBacksideWhenInside = true;

		mbApplyTransformToBV = true;

		mBoundingVolume.SetSize(1);
	}

	//-----------------------------------------------------------------------

	cFogArea::~cFogArea()
	{
	}	

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	

	//-----------------------------------------------------------------------

	void cFogArea::SetSize(const cVector3f& avSize)
	{
		mvSize = avSize;

		mBoundingVolume.SetSize(cVector3f(mvSize.x, mvSize.y, mvSize.x));
		
		
		SetTransformUpdated();
	}
	
	//-----------------------------------------------------------------------

	cMatrixf* cFogArea::GetModelMatrix(cFrustum* apFrustum)
	{
		m_mtxModelOutput = cMath::MatrixMul(GetWorldMatrix(), cMath::MatrixScale(mvSize));

		return &m_mtxModelOutput;
	}
	//-----------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

}
