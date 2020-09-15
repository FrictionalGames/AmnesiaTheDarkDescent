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

#ifndef HPL_FOG_AREA_H
#define HPL_FOG_AREA_H

#include "graphics/GraphicsTypes.h"
#include "graphics/Renderable.h"

namespace hpl {
	
	//------------------------------------------

	class cCamera;
	class cFrustum;
	class iTexture;
	class cResources;
	
	//------------------------------------------

	class cFogArea : public iRenderable
	{
	public:
		cFogArea(tString asName, cResources *apResources);
		virtual ~cFogArea();

		//////////////////////////////
		//Properties
		const cColor& GetColor(){ return mColor; }
		void SetColor(const cColor& aCol){ mColor = aCol; }
		
		const cVector3f& GetSize(){ return mvSize;}
		void SetSize(const cVector3f& avSize);

		void SetStart(float afX){ mfStart = afX;}
		void SetEnd(float afX){ mfEnd = afX;}

		float GetStart(){ return mfStart;}
		float GetEnd(){ return mfEnd;}

		void SetFalloffExp(float afX){ mfFalloffExp = afX;}
		float GetFalloffExp(){ return mfFalloffExp;}

		void SetShowBacksideWhenOutside(bool abX){ mbShowBacksideWhenOutside=abX; }
		void SetShowBacksideWhenInside(bool abX){ mbShowBacksideWhenInside=abX; }
		bool GetShowBacksideWhenOutside(){ return mbShowBacksideWhenOutside; }
		bool GetShowBacksideWhenInside(){ return mbShowBacksideWhenInside; }

		//////////////////////////////
		//iEntity implementation
		tString GetEntityType(){ return "cFogArea";}

		///////////////////////////////
		//Renderable implementation:
		cMaterial *GetMaterial(){ return NULL;}
		iVertexBuffer* GetVertexBuffer(){ return NULL;}

		eRenderableType GetRenderType(){ return eRenderableType_FogArea;}

		int GetMatrixUpdateCount(){ return GetTransformUpdateCount();}
		cMatrixf* GetModelMatrix(cFrustum* apFrustum);
		
	private:
		cColor mColor;
		cVector3f mvSize;
		float mfStart;
		float mfEnd;
		float mfFalloffExp;

		bool mbShowBacksideWhenOutside;
		bool mbShowBacksideWhenInside;

		cMatrixf m_mtxModelOutput;
	};

};
#endif // HPL_FOG_AREA_H
