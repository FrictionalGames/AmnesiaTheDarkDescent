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

#ifndef HPL_RENDERABLE_CONTAINER_DYNBOXTREE_H
#define HPL_RENDERABLE_CONTAINER_DYNBOXTREE_H

#include "scene/RenderableContainer.h"

namespace hpl {

	//-------------------------------------------

	class cBoundingVolume;

	class cRCNode_DynBoxTree;
	class cRenderableContainer_DynBoxTree;

	//-------------------------------------------


	class cDynBoxTreeObjectCallback : public cRenderableContainerObjectCallback, public iEntityCallback
	{
	public:
		cDynBoxTreeObjectCallback(cRenderableContainer_DynBoxTree *apContainer);

		void OnTransformUpdate(iEntity3D * apEntity);

	private:
		cRenderableContainer_DynBoxTree *mpContainer;
	};

	
	//-------------------------------------------

	class cRCNode_DynBoxTree : public iRenderableContainerNode
	{
	friend class cRenderableContainer_DynBoxTree;
	friend class cDynBoxTreeObjectCallback;
	public:
		cRCNode_DynBoxTree();
		~cRCNode_DynBoxTree();

		void UpdateBeforeUse();

		void RemoveObject(iRenderable *apObject);

		void RecalculateSplit();
		void RecalculateAABB();

		void ObjectMoved();

	private:
		cRenderableContainer_DynBoxTree *mpContainer;

		int mlSplitAxis;
		float mfSplitPlane;

		bool mbRecalculateSplit;
		bool mbRecalculateSplitAxis;
		bool mbRecalculateAABB;
        bool mbObjectMoved;

		bool mbIsSplit;

		int mlIgnoreSplitCount;
		int mlGarbageCollectCount;

		cVector3f mvMeanPosition;
	};

	//-------------------------------------------
	
	class cRenderableContainer_DynBoxTree : public iRenderableContainer
	{
	friend class cRCNode_DynBoxTree;
	friend class cDynBoxTreeObjectCallback;
	public:
		cRenderableContainer_DynBoxTree();
		~cRenderableContainer_DynBoxTree();

		void Add(iRenderable *apRenderable);
		void Remove(iRenderable *apRenderable);

		iRenderableContainerNode* GetRoot();

        void Compile();	

		void RebuildNodes();

		void RenderDebug(cRendererCallbackFunctions *apFunctions);
	
	private:
		void AddNodeObjectsToRoot(cRCNode_DynBoxTree *apNode);
		void SpecificUpdateBeforeRendering();

		void RenderDebugNode(cRendererCallbackFunctions *apFunctions, cRCNode_DynBoxTree *apNode, int alLevel);

		void RemoveNode(cRCNode_DynBoxTree *apNode);
		void CheckNodeAABBNeedsUpdateIterative(cRCNode_DynBoxTree *apNode, iRenderable *apObject);
		
		cRCNode_DynBoxTree *GetAddNode(cRCNode_DynBoxTree *apStartNode, iRenderable *apObject);
		void AddObjectToNodeIterative(cRCNode_DynBoxTree *apNode, iRenderable *apObject);

		int GetSplitGroup(iRenderable *apObject, float afSplitPlane, int alAxis, const cVector3f &avNodeSize);

		void UpdateObjectInContainer(iRenderable* apObject);
		void CheckForFitIterative(cRCNode_DynBoxTree *apNode, cBoundingVolume *apBV);

		
		cRCNode_DynBoxTree mRoot;

		int mlSplitThreshold;
		float mfMaxIntersectionAmount;
		int mlMaxIgnoreSplitCount;
		int mlMaxGarbageCollectCount;

		int mlRebuildCount;
		int mlMaxRebuildCount;

		cRCNode_DynBoxTree *mpCheckForFitTempNode;

		tRenderableSet m_setObjectsToUpdate;

		cDynBoxTreeObjectCallback *mpObjectCalllback;

		cRCNode_DynBoxTree *mpTempNode;
	};

	//-------------------------------------------
};
#endif // HPL_RENDERABLE_CONTAINER_DYNBOXTREE_H
