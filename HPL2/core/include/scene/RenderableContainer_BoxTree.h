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

#ifndef HPL_RENDERABLE_CONTAINER_BOXTREE_H
#define HPL_RENDERABLE_CONTAINER_BOXTREE_H

#include "scene/RenderableContainer.h"

namespace hpl {

	//-------------------------------------------

	class cBoxTreeTempNode;

	typedef std::list<cBoxTreeTempNode*> tBoxTreeTempNodeList;
	typedef std::list<cBoxTreeTempNode*>::iterator tBoxTreeTempNodeListIt;

	class cBoxTreeTempNode
	{
	public:
		cBoxTreeTempNode(cBoxTreeTempNode *apParent) : mpParent(apParent), mlSplitAxisCount(0) {}
		~cBoxTreeTempNode(){ STLDeleteAll(mlstChildren); };

		cBoxTreeTempNode *mpParent;
		int mlSplitAxisCount;

		tBoxTreeTempNodeList mlstChildren;
		tRenderableList mlstObjects;
	};

	//-------------------------------------------
	
	class cRCNode_BoxTree : public iRenderableContainerNode
	{
	friend class cRenderableContainer_BoxTree;
	public:
		cRCNode_BoxTree();
		~cRCNode_BoxTree();

	};

	//-------------------------------------------
	
	class cRenderableContainer_BoxTree : public iRenderableContainer
	{
	public:
		cRenderableContainer_BoxTree();
		~cRenderableContainer_BoxTree();

		void Add(iRenderable *apRenderable);
		/**
		 * Note that this is only allowed before compilation!
		 */
		void Remove(iRenderable *apRenderable);

		iRenderableContainerNode* GetRoot();

        void Compile();	

		void RenderDebug(cRendererCallbackFunctions *apFunctions);

		void SetMinLeafObjects(int alX){mlMinLeafObjects = alX;}
		int GetMinLeafObjects(){ return mlMinLeafObjects;}

		void SetMinSideLength(float afX){mfMinSideLength = afX;}
		float GetMinSideLength(){ return mfMinSideLength;}

		void SetMaxSideLength(float afX){mfMaxSideLength = afX;}
		float GetMaxnSideLength(){ return mfMaxSideLength;}

		void SetMaxVolumeCalcObjects(int alX){mlMaxVolumeCalcObjects = alX;}
		int GetMaxVolumeCalcObjects(){ return mlMaxVolumeCalcObjects;}

		void SetMaxIntersectionAmount(float afX){mfMaxIntersectionAmount = afX;}
		float GetMaxIntersectionAmount(){ return mfMaxIntersectionAmount;}
		
		void SetMinForceIntersectionRelativeSize(float afX){mfMinForceIntersectionRelativeSize = afX;}
		float GetMinForceIntersectionRelativeSize(){ return mfMinForceIntersectionRelativeSize;}
		

	private:
		void CompileTempNode(cBoxTreeTempNode *apNode, int alLevel, int alSplitAxis);
		void BuildNodeFromTemp(cBoxTreeTempNode *apTempNode, cRCNode_BoxTree *apNode, int alLevel);

		void RenderDebugNode(cRendererCallbackFunctions *apFunctions, cRCNode_BoxTree *apNode, int alLevel);

		void CalculateMinMax(tRenderableList *apObjectList, cVector3f& avMin, cVector3f& avMax);
		cVector3f CalculateSize(tRenderableList *apObjectList);

		float CalculateObjectsVolume(tRenderableList &alstObjects);
		float CalculateBestCutPlane(tRenderableList &alstObjects, int alAxis, const cVector3f &avNodeSize);

		int GetSplitGroup(iRenderable *apObject, float afCutPlane, int alAxis, const cVector3f &avNodeSize);
		
		cRCNode_BoxTree* mpRoot;

		int mlMinLeafObjects;
		float mfMinSideLength;
		float mfMaxSideLength;
		float mfMaxIntersectionAmount;
		int mlMaxVolumeCalcObjects;
		float mfMinForceIntersectionRelativeSize;

		tRenderableList m_mlstTempObjects;

		cRenderableContainerObjectCallback *mpObjectCalllback;
	};

	//-------------------------------------------
};
#endif // HPL_RENDERABLE_CONTAINER_BOXTREE_H
