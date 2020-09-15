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

#ifndef HPL_HAPTIC_SHAPE_HAPTX_H
#define HPL_HAPTIC_SHAPE_HAPTX_H

#include "haptic/HapticShape.h"

#include <HaptX.h>

namespace hpl {

	class cLowLevelHapticHaptX;

	class cHapticShapeHaptX : public iHapticShape
	{
	public:
		cHapticShapeHaptX(	const tString &asName, eHapticShapeType aType,
							const cVector3f& avSize, 
							cLowLevelHapticHaptX *apLowLevel);
		virtual ~cHapticShapeHaptX();

		void SetEnabled(bool abX);
		bool GetEnabled();

		void SetTransform(const cMatrixf &a_mtxTransform);
		cMatrixf GetTransform();
		
		cVector3f GetAppliedForce();

		void SetSurface(iHapticSurface *apSurface);

		void RenderDebug(iLowLevelGraphics *apLowLevel, const cColor &aColor);

		void CreateFromVertices(const unsigned int* apIndexArray, int alIndexNum,
								const float *apPosArray, int alPosStride,
								const float *apTexArray, int alTexStride,
								int alVtxNum);


		HaptX::iHapticShapeID GetHaptXShapeID(){return mShapeID;}
		void SetHaptXShapeID(HaptX::iHapticShapeID aID){ mShapeID = aID;}

		int mlIndexNum;
		int mlPosNum;
		int *mpIndices;
		HaptX::Vectors::Vec3f *mpHxPositions;
		cVector3f *mpHplPositions;
			
	private:
		HaptX::iHapticShapeID mShapeID;

		cLowLevelHapticHaptX *mpLowLevel;
		HaptX::HaptXInterface* mpInterface;
	};

};
#endif // HPL_HAPTIC_SHAPE_H
