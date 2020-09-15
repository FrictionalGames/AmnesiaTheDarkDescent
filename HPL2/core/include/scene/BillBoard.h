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

#ifndef HPL_BILLBOARD_H
#define HPL_BILLBOARD_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

#include "scene/Entity3D.h"
#include "graphics/Renderable.h"

namespace hpl {

	class cMaterialManager;
	class cResources;
	class cGraphics;
	class iLowLevelGraphics;
	class cMaterial;
	class iVertexBuffer;
	
	//------------------------------------------

	class cBillboard : public iRenderable
	{
	#ifdef __GNUC__
		typedef iRenderable __super;
	#endif
	public:
		cBillboard(const tString asName,const cVector2f& avSize,eBillboardType aType, cResources *apResources,cGraphics *apGraphics);
		~cBillboard();

		void SetMaterial(cMaterial * apMaterial);

		void SetSize(const cVector2f& avSize);
		cVector2f GetSize(){ return mvSize;}

		eBillboardType GetType(){ return mType;}

		void SetAxis(const cVector3f& avAxis);
		cVector3f GetAxis(){ return mvAxis;}

		void SetForwardOffset(float afOffset);
		float GetForwardOffset(){ return mfForwardOffset;}

		void SetColor(const cColor &aColor);
		const cColor& GetColor(){ return mColor;}

		void SetHaloAlpha(float afX);
		float GetHaloAlpha(){ return mfHaloAlpha;}

		/////////////////////////////////
		//Halo stuff
		void SetIsHalo(bool abX);
		bool IsHalo(){ return mbIsHalo;}

		void SetHaloSourceSize(const cVector3f &avSize);
		cVector3f GetHaloSourceSize(){return mvHaloSourceSize;}
		
		bool UsesOcclusionQuery();
		void AssignOcclusionQuery(iRenderer *apRenderer);
		bool RetrieveOcculsionQuery(iRenderer *apRenderer);
		
		/////////////////////////////////
		//Entity implementation
		tString GetEntityType(){ return "Billboard";}

		bool IsVisible();
		
		//Renderable implementations
		cMaterial *GetMaterial(){ return mpMaterial;}
		iVertexBuffer* GetVertexBuffer(){return mpVtxBuffer;}

		cMatrixf* GetModelMatrix(cFrustum *apFrustum);

		int GetMatrixUpdateCount();

		eRenderableType GetRenderType(){ return eRenderableType_Billboard;}

	private:
		cMaterialManager* mpMaterialManager;
		iLowLevelGraphics* mpLowLevelGraphics;
		
		cMaterial *mpMaterial;
		iVertexBuffer* mpVtxBuffer;

		cMatrixf m_mtxHaloOcclusionMatrix;
		cMatrixf m_mtxTempTransform;

		eBillboardType mType;
		cVector2f mvSize;
		cVector3f mvAxis;

		int mlLastRenderCount;

		bool mbIsHalo;
		cVector3f mvHaloSourceSize;
		cBoundingVolume *mpHaloSourceBV;
		int mbHaloSizeUpdated;
		int mlHaloBVMatrixCount;
		
		float mfForwardOffset;
		cColor mColor;
		float mfHaloAlpha;
	};

};
#endif // HPL_BILLBOARD_H
