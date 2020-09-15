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

#ifndef HPL_RENDERABLE_H
#define HPL_RENDERABLE_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "scene/SceneTypes.h"

#include "scene/Entity3D.h"


namespace hpl {

	class cMaterial;
	class iVertexBuffer;
	class cBoundingVolume;
	class cFrustum;
	class iLight;
	class cCamera;
	class iRenderableCallback;
	class cRenderList;
	class iRenderableContainerNode;
	class iPhysicsBody;
	class iRenderer;

	//------------------------------------------

	class iRenderable : public iEntity3D
	{
	#ifdef __GNUC__
		typedef iEntity3D __super;
	#endif
	public:
		iRenderable(const tString &asName);
		virtual ~iRenderable() {}

		virtual cMaterial *GetMaterial()=0;
		virtual iVertexBuffer* GetVertexBuffer()=0;

		virtual bool CollidesWithBV(cBoundingVolume *apBV);
		virtual bool CollidesWithFrustum(cFrustum *apFrustum);

		virtual cMatrixf* GetModelMatrix(cFrustum *apFrustum)=0;

		virtual eRenderableType GetRenderType()=0;

		virtual void UpdateGraphicsForFrame(float afFrameTime){}
		virtual bool UpdateGraphicsForViewport(cFrustum *apFrustum,float afFrameTime){ return true;}

		virtual bool UsesOcclusionQuery(){ return false; }
		virtual void AssignOcclusionQuery(iRenderer *apRenderer){}
		virtual bool RetrieveOcculsionQuery(iRenderer *apRenderer){ return true;}

		virtual void SetRenderFlagBit(tRenderableFlag alFlagBit, bool abSet);
		bool GetRenderFlagBit(tRenderableFlag alFlagBit){ return (mlRenderFlags & alFlagBit)!=0;} 
		inline tRenderableFlag GetRenderFlags() const { return mlRenderFlags;}

		virtual bool IsVisible(){ return mbIsVisible && mfCoverageAmount >0; }
		void SetVisible(bool abVisible);
		/**
		 * This is needed since IsVisible does not return the actual var value!
		 */
		bool GetVisibleVar(){ return mbIsVisible;}
		virtual void OnChangeVisible(){}
		
		virtual void SetIlluminationAmount(float afX){ mfIlluminationAmount = afX;}
		inline float GetIlluminationAmount()const { return mfIlluminationAmount;}
		
		void SetCoverageAmount(float afX);
		inline float GetCoverageAmount()const { return mfCoverageAmount;}

		void SetLargePlaneSurfacePlacement(int alX){ mlLargePlaneSurfacePlacement = alX;};
		inline int GetLargePlaneSurfacePlacement(){ return mlLargePlaneSurfacePlacement;};

		/**
		 * Should return a different number each time the renderable model matrix is updated. never -1
		 * \return
		 */
		virtual int GetMatrixUpdateCount()=0;

		inline void SetModelMatrixPtr(cMatrixf *apMtx){ mpModelMatrix = apMtx;}
		inline cMatrixf* GetModelMatrixPtr() const { return mpModelMatrix;}

		inline float GetViewSpaceZ() const { return mfViewSpaceZ;}
		inline void SetViewSpaceZ(float afZ){ mfViewSpaceZ = afZ;}

		cMatrixf* GetInvModelMatrix();

		inline void SetPrevMatrix(const cMatrixf& a_mtxPrev){m_mtxPrevious = a_mtxPrev;}
		inline cMatrixf& GetPrevMatrix(){ return m_mtxPrevious;}

		const cVector3f& GetCalcScale();

		void SetStatic(bool abX){ mbStatic = abX;}
		bool IsStatic() const { return mbStatic;}

		inline int GetRenderFrameCount() const { return mlRenderFrameCount;}
		inline void SetRenderFrameCount(const int alCount) { mlRenderFrameCount = alCount;}
		
		bool GetIsOneSided(){ return mbIsOneSided;}
		const cVector3f& GetOneSidedNormal(){ return mvOneSidedNormal;}

		void SetRenderCallback(iRenderableCallback *apCallback){ mpRenderCallback = apCallback; }

		inline iRenderableContainerNode* GetRenderContainerNode(){ return mpRenderContainerNode; }
		inline void SetRenderContainerNode(iRenderableContainerNode* apNode){ mpRenderContainerNode = apNode; }

		void SetRenderableUserData(void* apData) { mpRenderableUserData = apData; }
		void* GetRenderableUserData() { return mpRenderableUserData; }

	protected:
		cMatrixf m_mtxInvModel;
		cMatrixf m_mtxPrevious;
		cMatrixf *mpModelMatrix;

		iRenderableCallback *mpRenderCallback;

		bool mbIsOneSided;
		cVector3f mvOneSidedNormal;

		int mlLastMatrixCount;

		tRenderableFlag mlRenderFlags;

		bool mbIsVisible;

		bool mbStatic;

		bool mbForceShadow;
		
		int mlLargePlaneSurfacePlacement;

		int mlRenderFrameCount;

		int mlCalcScaleMatrixCount;
		cVector3f mvCalcScale;

		float mfViewSpaceZ;

		float mfIlluminationAmount;
		float mfCoverageAmount;

		iRenderableContainerNode *mpRenderContainerNode;

		void* mpRenderableUserData;
	};
};
#endif // HPL_RENDERABLE_H
