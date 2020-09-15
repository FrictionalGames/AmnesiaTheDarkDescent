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

#include "generate/VoxelMap.h"

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"

#include <cstring>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVoxelMap::cVoxelMap(const cVector3l& avSize)
	{
		mpData = NULL;
		SetSize(avSize);

		mfVoxelSize = 0.1f;
		mvPosition = 0;
	}

	//-----------------------------------------------------------------------

	cVoxelMap::~cVoxelMap()
	{
		hplDeleteArray(mpData);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cVoxelMap::SetVoxel(const cVector3l &avPos, char alVal)
	{
		mpData[avPos.z * mvSize.x*mvSize.y + avPos.y * mvSize.x + avPos.x] = alVal;
	}
	
	//-----------------------------------------------------------------------

	void cVoxelMap::SetSize(const cVector3l& avSize)
	{
		if(mpData) hplDeleteArray(mpData);
		
        mvSize = avSize;

		mpData = hplNewArray(unsigned char, mvSize.x*mvSize.y*mvSize.z*sizeof(unsigned char));
		memset(mpData, 0, mvSize.x*mvSize.y*mvSize.z*sizeof(unsigned char));
	}

	//-----------------------------------------------------------------------

	void cVoxelMap::DebugRender(iLowLevelGraphics *apLowGfx, const cColor &aCol)
	{
		for(int z=0; z<mvSize.z; ++z)
		for(int y=0; y<mvSize.y; ++y)
		for(int x=0; x<mvSize.x; ++x)
		{
			if(mpData[z * mvSize.x*mvSize.y + y * mvSize.x + x]==1)
			{
				apLowGfx->DrawSphere(mvPosition + cVector3f((float)x, (float)y, (float)z)*mfVoxelSize + cVector3f(mfVoxelSize/2), mfVoxelSize/2, aCol);
			}
		}
	}

	//-----------------------------------------------------------------------

}
