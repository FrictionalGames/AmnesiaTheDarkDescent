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

#ifndef HPL_RENDERABLE_CONTAINER_H
#define HPL_RENDERABLE_CONTAINER_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "scene/SceneTypes.h"

namespace hpl {

	//-------------------------------------------

	class iRenderableContainerNode;
	class cRendererCallbackFunctions;
	class iRenderable;
	class cFrustum;

	//-------------------------------------------
	
	class cVisibleRCNodeTracker
	{
	public:
		cVisibleRCNodeTracker();

		void SwitchAndClearVisibleNodeSet();
		void SetNodeVisible(iRenderableContainerNode *apNode);
		bool WasNodeVisible(iRenderableContainerNode *apNode);

		void Reset();

	private:
		tRenderableContainerNodeSet m_setVisibleNodes[2];
		int mlCurrentVisibleNodeSet;
		int mlFrameCounter;

	};

	//-------------------------------------------
	
	class cRenderableContainerObjectCallback : public iRenderableCallback
	{
	public:
		cRenderableContainerObjectCallback();

		void OnVisibleChange(iRenderable *apObject);
		void OnRenderFlagsChange(iRenderable *apObject);
	};

	//-------------------------------------------

	class iRenderableContainerNode
	{
	friend class iRenderableContainer;
	public:
		iRenderableContainerNode();
		virtual ~iRenderableContainerNode(){}

		virtual void UpdateBeforeUse(){}

		inline tRenderableContainerNodeList* GetChildNodeList(){ return &mlstChildNodes; }
		inline bool HasChildNodes(){ return mlstChildNodes.empty() == false; }

		inline tRenderableList* GetObjectList() { return &mlstObjects; }
		inline bool HasObjects() { return mlstObjects.empty() == false; }

		inline iRenderableContainerNode* GetParent(){ return mpParent;}
		inline void SetParent(iRenderableContainerNode* apParent){ mpParent = apParent;}

		inline int GetObjectNum(){ return (int)mlstObjects.size();}
		
		inline const cVector3f& GetMin() const{ return mvMin;}
		inline const cVector3f& GetMax() const{ return mvMax;}

		inline const cVector3f GetCenter() const{ return mvCenter;}
		inline float GetRadius() const { return mfRadius;}

		inline float GetViewDistance()const{ return mfViewDistance;}
		inline void SetViewDistance(float afX){ mfViewDistance = afX;}

		inline bool IsInsideView() const{ return mbInsideView;}
		inline void SetInsideView(bool abX) { mbInsideView = abX;}

		inline bool UsesFlagsAndVisibility() { return mbUsesFlagsAndVisibility;}

		inline tRenderableFlag GetRenderFlags() const { return mlRenderFlags;}
		inline bool HasVisibleObjects() const { return mbVisibleObjects;}

		inline void SetRenderFlags(tRenderableFlag alFlags) { mlRenderFlags = alFlags;}
		inline void SetHasVisibleObjects(bool abX) { mbVisibleObjects = abX;}

		inline void SetNeedPropertyUpdate(bool abX){ mbNeedPropertyUpdate = abX;}
		inline bool GetNeedPropertyUpdate() const { return mbNeedPropertyUpdate;}

		void PushUpNeedAABBUpdate();
		inline bool GetNeedAABBUpdate() const { return mbNeedAABBUpdate;}

		inline void SetPrevFrustumCollision(eCollision aX){ mPrevFrustumCollision = aX;}
		inline eCollision GetPrevFrustumCollision() const { return mPrevFrustumCollision;}

		void CalculateMinMaxFromObjects();
	
	protected:
		cVector3f mvMin;
		cVector3f mvMax;
		float mfRadius;
		cVector3f mvCenter;

		tRenderableFlag mlRenderFlags;
		bool mbVisibleObjects;

		bool mbNeedPropertyUpdate;
		bool mbNeedAABBUpdate;

		bool mbUsesFlagsAndVisibility;
        
		//Temp structures
		float mfViewDistance;
		bool mbInsideView;
		eCollision mPrevFrustumCollision;

		iRenderableContainerNode *mpParent;
		tRenderableContainerNodeList mlstChildNodes;
		tRenderableList mlstObjects;
	};

	//-------------------------------------------
	
	class iRenderableContainer
	{
	public:
		virtual ~iRenderableContainer(){}

		void UpdateBeforeRendering();

		virtual void Add(iRenderable *apRenderable)=0;
		virtual void Remove(iRenderable *apRenderable)=0;

		virtual iRenderableContainerNode* GetRoot()=0;

        /**
         * This compiles the container. Even if the container is static, it should be possible to change orientation (scale, pos, rotation,radius etc) of added
		 * objects before this method is called. After compile is called, objects orientation can not be changed!
         */
        virtual void Compile()=0;

		virtual void RenderDebug(cRendererCallbackFunctions *apFunctions)=0;

	private:
		void CheckNeedPropertyUpdateIteration(iRenderableContainerNode* apNode);
		void CheckNeedAABBUpdateIteration(iRenderableContainerNode* apNode);

		virtual void SpecificUpdateBeforeRendering(){}
	};

	//-------------------------------------------
};
#endif // RENDERABLE_CONTAINER
