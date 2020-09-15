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

#include "scene/RenderableContainer_DynBoxTree.h"

#include "graphics/Renderable.h"
#include "graphics/Renderer.h"
#include "graphics/LowLevelGraphics.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include <algorithm>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// HELPER FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	static bool HasDebug(iRenderable *apRenderable)
	{	
		return apRenderable->GetRenderFlagBit(eRenderableFlag_ContainerDebug);
	}

	//-----------------------------------------------------------------------

	static bool gbLog = false;
	
	//-----------------------------------------------------------------------

	static tString gsTempSpaces="";

	static const char *GetSpaces(int alNum)
	{
		gsTempSpaces.resize(alNum,'\t');
		return gsTempSpaces.c_str();		
	}

	//-----------------------------------------------------------------------

	static bool TestAndExpandNodeAABB(iRenderable *apObject, cVector3f& avNodeMin,cVector3f& avNodeMax)
	{
		const cVector3f &vMin = apObject->GetBoundingVolume()->GetMin();
		const cVector3f &vMax = apObject->GetBoundingVolume()->GetMax();

		//////////////////////////////////
		//Check if object is inside
		if(cMath::CheckAABBInside(vMin,vMax, avNodeMin, avNodeMax)) return false;

		//////////////////////////////////
		//Calculate the new AABB
		cMath::ExpandAABB(avNodeMin, avNodeMax, vMin, vMax);

		return true;
	}

	//-----------------------------------------------------------------------

	static bool CheckObjectIsAtEdgeOfAABB(iRenderable *apObject, const cVector3f& avNodeMin, const cVector3f& avNodeMax)
	{
		const cVector3f &vMin = apObject->GetBoundingVolume()->GetMin();
		const cVector3f &vMax = apObject->GetBoundingVolume()->GetMax();

		//if(	vMin.x == avNodeMin.x || vMin.y == avNodeMin.y || vMin.z == avNodeMin.z ||
		//	vMax.x == avNodeMax.x || vMax.y == avNodeMax.y || vMax.z == avNodeMax.z)
		if(	cMath::Abs(vMin.x - avNodeMin.x)<kEpsilonf || 
			cMath::Abs(vMin.y - avNodeMin.y)<kEpsilonf || 
			cMath::Abs(vMin.z - avNodeMin.z)<kEpsilonf ||
			
			cMath::Abs(vMax.x - avNodeMax.x)<kEpsilonf || 
			cMath::Abs(vMax.y - avNodeMax.y)<kEpsilonf || 
			cMath::Abs(vMax.z - avNodeMax.z)<kEpsilonf)
		{
			return true;
		}
		
		return false;
	}

	//-----------------------------------------------------------------------

	//0=x, 1=y, 2=z
	static float GetAxisFromVec(const cVector3f& avVec, int alAxis)
	{
		switch(alAxis)
		{
		case 0: return avVec.x;
		case 1: return avVec.y;
		case 2: return avVec.z;
		}

		return 0;
	}

	//-----------------------------------------------------------------------

	static bool SortFunc_X(iRenderable* apObjectA, iRenderable *apObjectB)
	{
		return apObjectA->GetBoundingVolume()->GetWorldCenter().x < apObjectB->GetBoundingVolume()->GetWorldCenter().x;
	}
	static bool SortFunc_Y(iRenderable* apObjectA, iRenderable *apObjectB)
	{
		return apObjectA->GetBoundingVolume()->GetWorldCenter().y < apObjectB->GetBoundingVolume()->GetWorldCenter().y;
	}
	static bool SortFunc_Z(iRenderable* apObjectA, iRenderable *apObjectB)
	{
		return apObjectA->GetBoundingVolume()->GetWorldCenter().z < apObjectB->GetBoundingVolume()->GetWorldCenter().z;
	}

	typedef bool (*tSortFunc)(iRenderable*,iRenderable*); 
	static tSortFunc gvSortFunctions[3] = {SortFunc_X, SortFunc_Y,SortFunc_Z};

	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CALLBACK
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cDynBoxTreeObjectCallback::cDynBoxTreeObjectCallback(cRenderableContainer_DynBoxTree *apContainer) : cRenderableContainerObjectCallback()
	{
		mpContainer = apContainer;
	}

	//-----------------------------------------------------------------------

	void cDynBoxTreeObjectCallback::OnTransformUpdate(iEntity3D * apEntity)
	{
		////////////////////////////////////////////
		//Get renderable object
		iRenderable *pObject = static_cast<iRenderable*>(apEntity);

		mpContainer->m_setObjectsToUpdate.insert(pObject);
	}

	//-----------------------------------------------------------------------

	
	//////////////////////////////////////////////////////////////////////////
	// NODE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cRCNode_DynBoxTree::cRCNode_DynBoxTree()
	{
		mpParent = NULL;
		mbRecalculateSplit = false;
		mbRecalculateAABB = false;

		mbIsSplit = false;
		mbObjectMoved = false;

		mbRecalculateSplitAxis = true;
		mlSplitAxis = -1;
		mfSplitPlane =0;

		mbUsesFlagsAndVisibility = false;

		mlGarbageCollectCount =0;
		mlIgnoreSplitCount =0;

		mvMeanPosition =0;
	}

	//-----------------------------------------------------------------------

	cRCNode_DynBoxTree::~cRCNode_DynBoxTree()
	{
		//Log("Deleting %d\n", this);
		STLDeleteAll(mlstChildNodes);
	}

	//-----------------------------------------------------------------------

	void cRCNode_DynBoxTree::UpdateBeforeUse()
	{
		//////////////////////////////////
		// Garbage collect check
		//  Use the var mbRecalculateSplit to see if anything has changed, else there is no use in a garbage collection.
		if(mbObjectMoved)
		{
			//////////////
			//Time for collection, counter is done
			if(mlGarbageCollectCount <=0)
			{
				if(gbLog){
					Log(" Check garbage collecion for node %d\n", this);
				}

				mlGarbageCollectCount = mpContainer->mlMaxGarbageCollectCount;

				////////////////////////////////////////
				// Make sure AABB is updated and as tight as possible
				mbRecalculateAABB = true;

				///////////////////////////////////////////
				//Check if any child nodes are leaves and if so the total number of objects are 
				// less then split threshold. If so, delete that node.
				if(HasChildNodes())
				{
					cRCNode_DynBoxTree *pChild1 = static_cast<cRCNode_DynBoxTree*>(mlstChildNodes.front());
					cRCNode_DynBoxTree *pChild2 = static_cast<cRCNode_DynBoxTree*>(mlstChildNodes.back());

					//What lower limit is best really?
					int lLowerObjectLimit = mpContainer->mlSplitThreshold;
					//int lLowerObjectLimit = (mpContainer->mlSplitThreshold*2)/3;
					//int lLowerObjectLimit = mpContainer->mlSplitThreshold - 2;
					
					//Check if nodes are leaves and if they have few enough objects
					if(	pChild1->HasChildNodes()==false && pChild2->HasChildNodes()==false &&
						(pChild1->GetObjectNum() + pChild2->GetObjectNum() + GetObjectNum() < lLowerObjectLimit))
					{
						if(gbLog){
							Log("  Collection garbage!\n");
						}
						
						//Iterate child nodes
						tRenderableContainerNodeListIt nodeIt = mlstChildNodes.begin();
						for(; nodeIt != mlstChildNodes.end(); ++nodeIt)
						{
							//Copy child object to this node
							cRCNode_DynBoxTree *pChildNode = static_cast<cRCNode_DynBoxTree*>(*nodeIt);

							tRenderableListIt objIt = pChildNode->mlstObjects.begin();
							for(; objIt != pChildNode->mlstObjects.end(); ++objIt)
							{
								iRenderable *pObject = *objIt;
								
								pObject->SetRenderContainerNode(this);
								mlstObjects.push_back(pObject);
							}
						}

						STLDeleteAll(mlstChildNodes);
						mbIsSplit = false;	//Reset that it is split;
						mbRecalculateSplitAxis = true;	//Need a new split axis!
					}
				}


				/////////////////////////////
				//Recalculate the split axis this time if there is enough objects
				// or if a node was deleted.
				if((int)mlstObjects.size() >= mpContainer->mlSplitThreshold)	
				{
					mbRecalculateSplitAxis = true;
				}
			}
			////////////////
			//Just decrement the counter.
			else 
			{
				--mlGarbageCollectCount;
			}
		}


		//////////////////////////////////
		// AABB update
		if(mbRecalculateAABB)
		{
			if(gbLog){
				Log(" Recalculating AABB for node %d\n", this);
			}

			mbRecalculateAABB = false;

			//Calculate the AABB based on the objects (if any)
			if(HasObjects()) CalculateMinMaxFromObjects();

			//Increase the AABB based on children.
			if(HasChildNodes())
			{
				cRCNode_DynBoxTree* pNodeA = static_cast<cRCNode_DynBoxTree*>(mlstChildNodes.front());
				cRCNode_DynBoxTree* pNodeB = static_cast<cRCNode_DynBoxTree*>(mlstChildNodes.back());

				cMath::ExpandAABB(mvMin,mvMax, pNodeA->GetMin(), pNodeA->GetMax());
				cMath::ExpandAABB(mvMin,mvMax, pNodeB->GetMin(), pNodeB->GetMax());
			}

			//Sphere center and radius
			mvCenter = (mvMax + mvMin) *0.5f;
			mfRadius = (mvMax - mvMin).Length()*0.5f;            
		}

		//////////////////////////////////
		// Check if a recalculate of split axis is really needed.
		if(mbRecalculateSplit)
		{
			if(GetObjectNum() < mpContainer->mlSplitThreshold && HasChildNodes()==false)
			{
				mbRecalculateSplit = false;
			}
		}
		else
		{
			//Always recalculate the split plane before a split
			if(mbIsSplit==false && GetObjectNum() >= mpContainer->mlSplitThreshold)
			{
				mbRecalculateSplit = true;
			}
		}
		
		//////////////////////////////////
		// Recalculate the split properties
		if(mbRecalculateSplit)
		{
			if(gbLog){
				Log(" Recalculating split for node %d\n", this);
			}
			
			mbRecalculateSplit = false;
			
			////////////////////////////////////////
			// Get the split axis (only do once or else the tree will become a mess!)
			if(mbRecalculateSplitAxis)
			{
				mbRecalculateSplitAxis = false;

				cVector3f vNodeSize = mvMax - mvMin;

				float fLongestSide = vNodeSize.x;
				mlSplitAxis =0;
				if(fLongestSide < vNodeSize.y){	
					fLongestSide = vNodeSize.y;
					mlSplitAxis = 1;
				}
				if(fLongestSide < vNodeSize.z){	
					fLongestSide = vNodeSize.z;	
					mlSplitAxis = 2;
				}
			}

			////////////////////////////////////////
			// Calculate the split plane by taking the average of all objects's centers
			// and the nodes split planes (but with more weight(.

			/*cVector3f vPosSum=0;
			float fWeightSum=0;
			
			//Objects
			if(mlstObjects.empty()==false)
			{
				tRenderableListIt it = mlstObjects.begin();
				for(; it != mlstObjects.end(); ++it)
				{
					iRenderable *pObject = *it;

					vPosSum += 	pObject->GetBoundingVolume()->GetPosition();
					fWeightSum += 1;
				}
			}

			//Nodes
			if(HasChildNodes())
			{
				tRenderableContainerNodeListIt it = mlstChildNodes.begin();
				for(; it != mlstChildNodes.end(); ++it)
				{
					cRCNode_DynBoxTree *pChildNode = static_cast<cRCNode_DynBoxTree*>(*it);
					
					float fObjNum = (float)pChildNode->mlstObjects.size();
					vPosSum += pChildNode->mvMeanPosition * fObjNum;
                    fWeightSum += fObjNum;
				}
			}

			mvMeanPosition = vPosSum / fWeightSum;
            mfSplitPlane = mvMeanPosition.v[mlSplitAxis];*/
		
			
			
			float fPosSum =0;
			float fWeightSum = 0;

			//Objects
			if(mlstObjects.empty()==false)
			{
				tRenderableListIt it = mlstObjects.begin();
				for(; it != mlstObjects.end(); ++it)
				{
					iRenderable *pObject = *it;

					fPosSum += 	GetAxisFromVec(pObject->GetBoundingVolume()->GetPosition(), mlSplitAxis);
					fWeightSum += 1;
				}
			}

			//Nodes
			if(HasChildNodes())
			{
				float fPosMul = (float)mpContainer->mlSplitThreshold;
				tRenderableContainerNodeListIt it = mlstChildNodes.begin();
                for(; it != mlstChildNodes.end(); ++it)
				{
					cRCNode_DynBoxTree *pChildNode = static_cast<cRCNode_DynBoxTree*>(*it);

					//Use split plane if axes are the same
                    if(pChildNode->mlSplitAxis == mlSplitAxis)
					{
						fPosSum += pChildNode->mfSplitPlane * fPosMul;
						fWeightSum += fPosMul;	
					}
					//Else just use AABB center
					else
					{
						fPosSum += pChildNode->mvCenter.v[mlSplitAxis] * fPosMul;
						fWeightSum += fPosMul;
					}
				}
			}

            mfSplitPlane = fPosSum / fWeightSum;
		}


		//////////////////////////////////
		// If enough objects, create children and objects to them.
		// Also make sure that the node does not already have childe nodes!
        if(	(int)mlstObjects.size() >= mpContainer->mlSplitThreshold &&
			HasChildNodes()==false)
		{
			/////////////
			//Check if tree has already been split
			if(mbIsSplit)
			{
				//There has been any object changes in thenode, lower the ignore split count
				// this will mean that split can be recalculated later on, but not very often.
                if(mbObjectMoved)
				{
					mlIgnoreSplitCount--;
					if(mlIgnoreSplitCount <=0)
					{
						mbIsSplit = false;
					}
				}
			}
			/////////////
			// Tree is really in need of split
			else
			{
				if(gbLog){
					Log(" Splitting node %d\n", this);
					Log(" --------------------\n", this);
				}

				mbIsSplit = true;
				mlIgnoreSplitCount = mpContainer->mlMaxIgnoreSplitCount;

				///////////////////////////////////
				// Setup before objects are added
				cVector3f vNodeSize = GetMax() - GetMin();
				
				std::vector<iRenderable*> vTempObjects;
				vTempObjects.resize(mlstObjects.size());

				int lCount =0;
				for(tRenderableListIt it = mlstObjects.begin(); it != mlstObjects.end(); ++it,++lCount)
				{
					vTempObjects[lCount] = *it;	
				}
				
				mlstObjects.clear();

				tRenderableList lstChildObjects[2];
			
				
				///////////////////////////////////
				// Iterate 
				for(size_t i=0; i<vTempObjects.size(); ++i)
				{
					iRenderable *pObject = vTempObjects[i];

					int lSplitGroup = mpContainer->GetSplitGroup(pObject,mfSplitPlane, mlSplitAxis ,vNodeSize);

					/////////////////////////////
					// If object is intersecting then add to this node, else add to a child.
					if(lSplitGroup == 2)
					{
						mlstObjects.push_back(pObject);	
						if(gbLog){
							Log("  Object '%s'/%d stay put\n", pObject->GetName().c_str(), pObject);
						}
					}
					else 
					{
						//Add to child list
						lstChildObjects[lSplitGroup].push_back(pObject);
						if(gbLog){
							Log("  Object '%s'/%d moved to node %d\n", pObject->GetName().c_str(), pObject, lSplitGroup);
						}
					}
				}

				//////////////////////////////////
				// If either child object list is empty, add objects back to node
				if(lstChildObjects[0].empty() ||  lstChildObjects[1].empty())
				{
					for(int i=0; i<2; ++i)
					for(tRenderableListIt it = lstChildObjects[i].begin(); it != lstChildObjects[i].end(); ++it)
					{
						mlstObjects.push_back(*it);
					}
				}
				//////////////////////////////////
				// If split worked, create child nodes and add objects to them
				else
				{
					///////////////////////////////////
					// Create and setup child nodes
					// Make sure to create mean position here too! ( can remove later on?)
					for(int i=0; i<2; ++i)
					{
						cRCNode_DynBoxTree* pChildNode = hplNew( cRCNode_DynBoxTree, () ); 
						pChildNode->mpContainer = mpContainer;
						pChildNode->mpParent = this;

						mlstChildNodes.push_back(pChildNode);
						
                        pChildNode->mvMeanPosition =0;                        
						for(tRenderableListIt it = lstChildObjects[i].begin(); it != lstChildObjects[i].end(); ++it)
						{
							iRenderable *pObject = *it;
							
							pChildNode->mlstObjects.push_back(pObject);

							pChildNode->mvMeanPosition += pObject->GetBoundingVolume()->GetWorldCenter();

							//Set new node to object
							pObject->SetRenderContainerNode(pChildNode);
						}

						pChildNode->mvMeanPosition = pChildNode->mvMeanPosition / (float)lstChildObjects[i].size();

						pChildNode->RecalculateSplit();
						pChildNode->RecalculateAABB();

						/////////////////////////////////////
						//Just to be extra sure, caclulate proper AABB directly!
						pChildNode->CalculateMinMaxFromObjects();
						//Sphere center and radius
						pChildNode->mvCenter = (pChildNode->mvMax + pChildNode->mvMin) *0.5f;
						pChildNode->mfRadius = (pChildNode->mvMax - pChildNode->mvMin).Length()*0.5f;
					}
				}

				if(gbLog){
					Log(" --------------------\n", this);
				}
			}
		}

		//////////////////////////////////
		// Reset variables
		mbObjectMoved = false;
	}

	//-----------------------------------------------------------------------

	void cRCNode_DynBoxTree::RemoveObject(iRenderable *apObject)
	{
		/////////////////////////////////
		// Remove object from list and clears node list
		STLFindAndRemove(mlstObjects, apObject);
	}
	
	//-----------------------------------------------------------------------

	void cRCNode_DynBoxTree::RecalculateSplit()
	{
		mbRecalculateSplit = true;

		//No need to push up, since recalculation is called on all nodes where AABB is changed.
	}

	//-----------------------------------------------------------------------

	void cRCNode_DynBoxTree::RecalculateAABB()
	{
		mbRecalculateAABB = true;

		//Do calculation in BeforeUse()...
	}

	//-----------------------------------------------------------------------

	void cRCNode_DynBoxTree::ObjectMoved()
	{
		if(mbObjectMoved) return;

		mbObjectMoved = true;
		cRCNode_DynBoxTree* pParentNode = static_cast<cRCNode_DynBoxTree*>(mpParent);       
		if(pParentNode) pParentNode->ObjectMoved();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cRenderableContainer_DynBoxTree::cRenderableContainer_DynBoxTree()
	{
		mlSplitThreshold = 30;		//Number of objects a node should have before split.

		mlMaxGarbageCollectCount = 60;		//How often (measured in accesses with change, meaning when the renderer checks the node and there has been some movement)
											//certain a "garbage check should be made. This means:
											//- If node has leaves as children, check so that the child objects contained is equal or greater then then split threshold
											//  else move all the objects in the child nodes to the node.
											//- Recalculate the split axis.

		mlMaxIgnoreSplitCount = 30;			//How often new splits should be done, after the first split has been made.
											//If objects are removed from child nodes they may be deleted and an already split
											//node is without child nodes. If the object here grows to split size a new split can happen
											//but only after a number of checks determined by this variable.
											//Node: IsSplit variable cannot be reseted since there are cases when there are enough objects but all
											//will go into the same child node and eternal iteration ensues.

		mfMaxIntersectionAmount = 0.2f;		//The amount (1=100%) an object can be over the splitting line and still not be an intersecting object
											//Also is the amount by which a new AABB can be expanded.

		mlMaxRebuildCount = 600;			//The number changes the need to occur in the tree before it is rebuilt from scratch.
		mlRebuildCount = mlMaxRebuildCount;

		mRoot.mpContainer = this;
		mRoot.mpParent = NULL;
		mRoot.mfViewDistance =0;
		mRoot.mbInsideView = true;

		mpObjectCalllback = hplNew( cDynBoxTreeObjectCallback, (this) );
	}
	
	cRenderableContainer_DynBoxTree::~cRenderableContainer_DynBoxTree()
	{
		hplDelete( mpObjectCalllback );
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cRenderableContainer_DynBoxTree::Add(iRenderable *apRenderable)
	{
		if(gbLog || HasDebug(apRenderable)){
			Log("Adding object '%s' / %d\n",apRenderable->GetName().c_str(), apRenderable);
		}
	
		///////////////////////////////////////////
		//Get the node where object is to be added and then add it there.
		cRCNode_DynBoxTree *pNode = GetAddNode(&mRoot, apRenderable);

		pNode->mlstObjects.push_back(apRenderable);
		apRenderable->SetRenderContainerNode(pNode);

		/////////////////////////
		//Add callbacks
		apRenderable->SetRenderCallback(mpObjectCalllback);
		apRenderable->AddCallback(mpObjectCalllback);

		if(gbLog || HasDebug(apRenderable)){
			Log("Added object '%s' / %d to Node %d\n",apRenderable->GetName().c_str(), apRenderable, pNode);

			Log("Objects:\n");
			tRenderableList *pObjList = pNode->GetObjectList();
			for(tRenderableListIt it = pObjList->begin(); it != pObjList->end(); ++it)
			{
				iRenderable *pObj = *it;
				tString sParent = pObj->GetEntityParent() ? pObj->GetEntityParent()->GetName() : "NULL";
				Log(" '%s'id: %d  parent: '%s' size: %s\n", pObj->GetName().c_str(),pObj->GetUniqueID(),sParent.c_str(), pObj->GetBoundingVolume()->GetSize().ToString().c_str());
			}
		}

		//Increase rebuild count.
		mlRebuildCount--;
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_DynBoxTree::Remove(iRenderable *apRenderable)
	{
		//////////////////////////////////
		// Remove from to-update list
		if(m_setObjectsToUpdate.empty()==false)
		{
			m_setObjectsToUpdate.erase(apRenderable);
		}
		
		//////////////////////////////////
		// Get the node where the object is
		cRCNode_DynBoxTree *pNode = static_cast<cRCNode_DynBoxTree*>(apRenderable->GetRenderContainerNode());	//Assume one node only
		if(pNode==NULL) return;

		if(gbLog  || HasDebug(apRenderable)){
			Log("Removing object '%s' / %d from node %d\n",apRenderable->GetName().c_str(), apRenderable, pNode);
		}

		//////////////////////////////////
		// Remove object from node.
        pNode->RemoveObject(apRenderable);

		//////////////////////////////////
		// If nodes is leaf, has no objects left and is not the root, delete it.
		if(	pNode->HasChildNodes()==false && pNode->HasObjects()==false && pNode->GetParent() != NULL)
		{
			if(gbLog || HasDebug(apRenderable)){
				Log(" Remove node %d\n", pNode);
			}
			RemoveNode(pNode);
		}
		// If not, check if AABB update is needed.
		else
		{
			if(gbLog || HasDebug(apRenderable)){
				Log(" Iterate AABB update for node %d\n", pNode);
			}
			CheckNodeAABBNeedsUpdateIterative(pNode,apRenderable);
		}
		
		////////////////////////
		//Remove callbacks
		apRenderable->SetRenderContainerNode(NULL);
		apRenderable->SetRenderCallback(NULL);
		apRenderable->RemoveCallback(mpObjectCalllback);

		//Increase rebuild count.
		mlRebuildCount--;
	}

	//-----------------------------------------------------------------------

	iRenderableContainerNode* cRenderableContainer_DynBoxTree::GetRoot()
	{
		return &mRoot;
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_DynBoxTree::Compile()
	{
		//When doing a compile, wait a little while and then do a rebuild. So all particle system and usch have come into place.
		mlRebuildCount = 10;
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_DynBoxTree::RebuildNodes()
	{
		mlRebuildCount = 0;
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_DynBoxTree::RenderDebug(cRendererCallbackFunctions *apFunctions)
	{
		apFunctions->SetDepthTest(true);
		apFunctions->SetDepthWrite(false);
		apFunctions->SetBlendMode(eMaterialBlendMode_None);

		apFunctions->SetProgram(NULL);
		apFunctions->SetTextureRange(NULL, 0);
		apFunctions->SetMatrix(NULL);


		RenderDebugNode(apFunctions, &mRoot,0);
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------
	
	void cRenderableContainer_DynBoxTree::AddNodeObjectsToRoot(cRCNode_DynBoxTree *apNode)
	{
		tRenderableListIt objIt = apNode->mlstObjects.begin();
		for(; objIt != apNode->mlstObjects.end(); ++objIt)
		{
			iRenderable *pObject = *objIt;
			
			pObject->SetRenderContainerNode(&mRoot);
			mRoot.mlstObjects.push_back(pObject);
		}

		tRenderableContainerNodeListIt nodeIt = apNode->mlstChildNodes.begin();
		for(; nodeIt != apNode->mlstChildNodes.end(); ++nodeIt)
		{
			cRCNode_DynBoxTree *pChildNode = static_cast<cRCNode_DynBoxTree*>(*nodeIt);
			
			AddNodeObjectsToRoot(pChildNode);
		}
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_DynBoxTree::SpecificUpdateBeforeRendering()
	{
		///////////////////////////////////
		// Update tree for objects that have moved
		if(m_setObjectsToUpdate.empty()==false)
		{
			tRenderableSetIt it = m_setObjectsToUpdate.begin();
			for(; it != m_setObjectsToUpdate.end(); ++it)
			{
				iRenderable *pObject = *it;
				
				UpdateObjectInContainer(pObject);
			}
            m_setObjectsToUpdate.clear();
		}


		///////////////////////////////////
		// Rebuild tree if needed
		if(mlRebuildCount >0) return;

		mlRebuildCount = mlMaxRebuildCount;
		
		tRenderableContainerNodeListIt it = mRoot.mlstChildNodes.begin();
		for(; it != mRoot.mlstChildNodes.end(); ++it)
		{
			cRCNode_DynBoxTree *pNode = static_cast<cRCNode_DynBoxTree*>(*it);

            AddNodeObjectsToRoot(pNode);
		}

		STLDeleteAll(mRoot.mlstChildNodes);

		mRoot.mbIsSplit = false;
		mRoot.mbRecalculateAABB = true;
		mRoot.mbRecalculateSplitAxis = true;
		mRoot.mbRecalculateSplit = true;
	}

	//-----------------------------------------------------------------------

	static cColor LevelColor[10] = {cColor(1,1,1),cColor(1,0,1),cColor(1,1,0),cColor(0,1,1),cColor(0,0,1),cColor(0,1,0),cColor(1,0,0),cColor(1,0.5f,1),
									cColor(1,1,0.5f), cColor(1,0.5f,0.5f)};

	void cRenderableContainer_DynBoxTree::RenderDebugNode(cRendererCallbackFunctions *apFunctions, cRCNode_DynBoxTree *apNode, int alLevel)
	{
		//AABB
		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(apNode->GetMin(),apNode->GetMax(),LevelColor[alLevel % 10]);

		//Split plane
		if(apNode->HasChildNodes())
		{
			cVector3f vMin = apNode->GetMin();
			cVector3f vMax = apNode->GetMax();
			vMin.v[apNode->mlSplitAxis] = apNode->mfSplitPlane;
			vMax.v[apNode->mlSplitAxis] = apNode->mfSplitPlane;

			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vMin, vMax,LevelColor[alLevel % 10]);

		}
				

		tRenderableContainerNodeListIt childIt = apNode->GetChildNodeList()->begin();
		for(; childIt != apNode->GetChildNodeList()->end(); ++childIt)
		{
			cRCNode_DynBoxTree *pChildNode = static_cast<cRCNode_DynBoxTree*>(*childIt);
			RenderDebugNode(apFunctions,pChildNode , alLevel+1);
		}
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_DynBoxTree::RemoveNode(cRCNode_DynBoxTree *apRemoveNode)
	{
		cRCNode_DynBoxTree *pParent = static_cast<cRCNode_DynBoxTree*>(apRemoveNode->GetParent());
		if(pParent == NULL) return;

		
		if(gbLog){
			Log(" Remove node %d from parent %d\n", apRemoveNode, pParent);
		}

		////////////////////////////////////
		// Remove the node from the parent
		STLFindAndDelete(pParent->mlstChildNodes, apRemoveNode);

		////////////////////////////////////
		// Get Remaining child node and then clear child node list
		cRCNode_DynBoxTree *pChild = static_cast<cRCNode_DynBoxTree*>(pParent->mlstChildNodes.front());
		
        pParent->mlstChildNodes.clear();
		
		////////////////////////////////////
		// Move the objects from the remaining child in parent to parent
		if(pChild->HasObjects())
		{
			if(gbLog){
				Log("  Moving objects from node %d to parent %d\n", pChild, pParent);
			}

			tRenderableListIt it = pChild->mlstObjects.begin();
			for(; it != pChild->mlstObjects.end(); ++it)
			{
				iRenderable *pChildObject = *it;
				if(gbLog) Log("   %s / %d\n", pChildObject->GetName().c_str(), pChildObject);

				//Set new node
				pChildObject->SetRenderContainerNode(pParent);
				
				pParent->mlstObjects.push_back(pChildObject);
			}
		}

		////////////////////////////////////
		// Move the child nodes from the remaining child in parent to parent
		if(pChild->HasChildNodes())
		{
			if(gbLog){
				Log("  Moving children from node %d to parent %d\n", pChild, pParent);
			}

			tRenderableContainerNodeListIt it = pChild->mlstChildNodes.begin();
			for(; it != pChild->mlstChildNodes.end(); ++it)
			{
				iRenderableContainerNode *pChild_ChildNode = *it;

				if(gbLog) Log("   %d\n", pChild_ChildNode);

				//Set new parent and add to parents child node list
                pChild_ChildNode->SetParent(pParent);				
                pParent->mlstChildNodes.push_back(pChild_ChildNode);
			}

			//Clear all nodes, else they will be deleted later on
			pChild->mlstChildNodes.clear();	
		}

		if(gbLog){
			Log(" Deleting other node %d and recalculating AABB for parent %d\n", pChild, pParent);
		}

		////////////////////////////////////
		// Delete the remaining child node.
		hplDelete(pChild);

		////////////////////////////////////
		// Recalculate the AABB of parent and set as not split
        pParent->RecalculateAABB();
        pParent->mbIsSplit = false;
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_DynBoxTree::CheckNodeAABBNeedsUpdateIterative(cRCNode_DynBoxTree *apNode, iRenderable *apObject)
	{
		if(CheckObjectIsAtEdgeOfAABB(apObject, apNode->GetMin(), apNode->GetMax()))
		{
			if(gbLog){
				Log("  Updating AABB for node %d\n", apNode);
			}

			//Calculate a new AABB size
			apNode->RecalculateAABB();
			
			//Check if parent needs update too
			cRCNode_DynBoxTree *pParent = static_cast<cRCNode_DynBoxTree*>(apNode->GetParent());
			if(pParent)
				CheckNodeAABBNeedsUpdateIterative(pParent, apObject);
		}
	}
	
	//-----------------------------------------------------------------------

	
	cRCNode_DynBoxTree *cRenderableContainer_DynBoxTree::GetAddNode(cRCNode_DynBoxTree *apStartNode, iRenderable *apObject)
	{
		mpTempNode = NULL; //Where the node is placed.
		AddObjectToNodeIterative(apStartNode, apObject);

		return mpTempNode;
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_DynBoxTree::AddObjectToNodeIterative(cRCNode_DynBoxTree *apNode, iRenderable *apObject)
	{
		if(gbLog || HasDebug(apObject) ){
			cBoundingVolume *pBV = apObject->GetBoundingVolume();
			Log(" Testing node %d. Inside: %d Intersect: %d (%s)-(%s) vs (%s)-(%s)\n",	apNode,
																			cMath::CheckAABBInside(pBV->GetMin(),pBV->GetMax(), apNode->mvMin, apNode->mvMax)?1:0,
																			cMath::CheckAABBIntersection(pBV->GetMin(),pBV->GetMax(), apNode->mvMin, apNode->mvMax)?1:0,
																			pBV->GetMin().ToString().c_str(), pBV->GetMax().ToString().c_str(), 
																			apNode->mvMin.ToString().c_str(), apNode->mvMax.ToString().c_str());
		}
		/////////////////////////////
		// Expand the AABB of node if needed
		if( TestAndExpandNodeAABB(apObject, apNode->mvMin, apNode->mvMax) )
		{
			//TODO: Perhaps sphere calc can be skipped here and done in UpdateBeforeUse instead.
			apNode->mvCenter = (apNode->mvMax + apNode->mvMin) *0.5f;
			apNode->mfRadius = (apNode->mvMax - apNode->mvMin).Length()*0.5f;

			apNode->RecalculateSplit();

			if(gbLog || HasDebug(apObject)) Log("  Expanded AABB\n");
		}

		/////////////////////////////
		// If node has no children, add it here.
		if(apNode->HasChildNodes()==false)
		{
			mpTempNode = apNode;
			return;
		}

		/////////////////////////////
		// Calculate what group the new object belongs to
		cVector3f vNodeSize = apNode->GetMax() - apNode->GetMin();
		int lSplitGroup = GetSplitGroup(apObject,apNode->mfSplitPlane, apNode->mlSplitAxis ,vNodeSize);
		
		
		/////////////////////////////
		// If object is intersecting then add to this node, else add to a child.
		if(lSplitGroup == 2)
		{
			mpTempNode = apNode;
		}
		else 
		{
			//Assuming always two children!
			if(lSplitGroup==0)	AddObjectToNodeIterative(static_cast<cRCNode_DynBoxTree*>(apNode->mlstChildNodes.front()),apObject);
			if(lSplitGroup==1)	AddObjectToNodeIterative(static_cast<cRCNode_DynBoxTree*>(apNode->mlstChildNodes.back()),apObject);
		}
	}

	//-----------------------------------------------------------------------

	/**
	 * Returns 0=above, 1=below, 2=intersection
	 */
	int cRenderableContainer_DynBoxTree::GetSplitGroup(iRenderable *apObject, float afSplitPlane, int alAxis, const cVector3f &avNodeSize)
	{
		float fMinVal =   GetAxisFromVec(apObject->GetBoundingVolume()->GetMin(),alAxis);
		float fMaxVal =   GetAxisFromVec(apObject->GetBoundingVolume()->GetMax(),alAxis);

		//////////////////////////
		//Above cut plane
		if(fMinVal >= afSplitPlane)
		{
			return 0;
		}
		//////////////////////////
		//Below cut plane
		else if(fMaxVal <= afSplitPlane)
		{
			return 1;
		}
		//////////////////////////
		//Intersection
		else
		{
			////////////////////////
			//Calculate how much the object crosses over on each side
			float fAboveDist = fMaxVal - afSplitPlane;
			float fBelowDist = afSplitPlane - fMinVal;
			
			float fNodeSize = GetAxisFromVec(avNodeSize, alAxis);	//Using the size of entire node since it one side might be very small.

			/////////////////////
			//Get the least amount of cross over, and on which side it is.
			float fMinDist = fAboveDist;
			int lDestDir = 1;	//If above is smallest, object should be below and vice verce.
			if(fBelowDist < fMinDist)
			{
				fMinDist = fBelowDist;
				lDestDir =0;
			}
			
			/////////////////////
			//Check if the amount of cross over is small enough to not treat it as an intersection.
			float fMinCrossOverAmount = fMinDist / fNodeSize;
			if(fMinCrossOverAmount <= mfMaxIntersectionAmount) return lDestDir;
			
			return 2;
		}
	}
	
	//-----------------------------------------------------------------------


	void cRenderableContainer_DynBoxTree::UpdateObjectInContainer(iRenderable* apObject)
	{
		const bool bUpdateLog=false;
		//bool bUpdateLog= HasDebug(apObject);
	

		////////////////////////////////////////////
		//Get Node of correct type
		cRCNode_DynBoxTree *pNode =  static_cast<cRCNode_DynBoxTree*>(apObject->GetRenderContainerNode());
		if(pNode==NULL) return;

		pNode->ObjectMoved();	//Notify the node that an object moved inside it.

		if(bUpdateLog) 
			Log("------- Updating %s. Node: %d -------\n",apObject->GetName().c_str(), pNode);


		////////////////////////////////////////////
		//Get bounding volume and see if still contained in node
		cBoundingVolume *pBV = apObject->GetBoundingVolume();

		if(cMath::CheckAABBInside(pBV->GetMin(), pBV->GetMax(), pNode->GetMin(), pNode->GetMax()))
		{
			//Do not Check if AABB of node needs updating, since it will only take time and not gain much since the garbaage collection will fix it after a while.

			//The node is still inside the same node
			if(bUpdateLog) {
				Log(" Still inside AABB, not doing anything!\n");
				Log("---------------------\n");
			}
			return; 
		}

		////////////////////////////////////////////
		// Iterate through the parent nodes of the current and try and find one that fits the node, stopping at root
		mpCheckForFitTempNode = pNode; //Node with fit will be placed here
		cRCNode_DynBoxTree *pParent =  static_cast<cRCNode_DynBoxTree*>(pNode->GetParent());
		if(pParent) CheckForFitIterative(pParent, pBV);

		if(bUpdateLog) Log(" Found fit in %d (by starting in parent %d)\n", mpCheckForFitTempNode, pParent);

		////////////////////////////////////////////
		// Get the node where the object is to be added
		cRCNode_DynBoxTree *pNewNode = GetAddNode(mpCheckForFitTempNode, apObject);

		if(bUpdateLog) Log("  Node to add object in: %d (leaf: %d)\n", pNewNode, pNewNode->HasChildNodes()==false ? 1:0);

		////////////////////////////////////////////
		// If the new node is not the same, then the the object needs removed from the old node and added to new.
		if(pNewNode != pNode)
		{
			if(bUpdateLog) Log("  Moving node\n");

			//Notify the new node that an object moved inside it.
			pNewNode->ObjectMoved();	

			//Decrease container rebuild count.
			mlRebuildCount--;

			/////////////////////////////////////
			//If the object is not inside new node need to recalc AABB
			cBoundingVolume *pObjBV = apObject->GetBoundingVolume();
			//if(CheckObjectIsAtEdgeOfAABB(apObject,pNewNode->mvMin,pNewNode->mvMax)) //<- Old bad code! Just have for future reference!
			if(cMath::CheckAABBInside(pObjBV->GetMin(), pObjBV->GetMax(), pNewNode->mvMin,pNewNode->mvMax)==false)
			{
				pNewNode->PushUpNeedAABBUpdate();
			}

			/////////////////////////////////////
			//Remove the node and set new one from node list
			apObject->SetRenderContainerNode(pNewNode);

			/////////////////////////////////////
			//Add to new node
			pNewNode->mlstObjects.push_back(apObject);

			/////////////////////////////////////
			//Remove from old
			pNode->RemoveObject(apObject);

			// If old node is leaf and has no objects left, delete it.
			if(pNode->HasChildNodes()==false && pNode->HasObjects()==false)
			{
				RemoveNode(pNode);
			}
			// If not, set that AABB update is needed.
			else
			{
				pNode->PushUpNeedAABBUpdate();
			}
		}
		///////////
		//If the same node, then we want to recalculate AABB
		else
		{
			if(bUpdateLog) Log("  Same node %d\n", pNode);

			pNode->PushUpNeedAABBUpdate();
		}

		if(bUpdateLog) Log("---------------------\n");
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_DynBoxTree::CheckForFitIterative(cRCNode_DynBoxTree *apNode, cBoundingVolume *apBV)
	{
		///////////////////////////////////
		//Check if there is a fit or node is root, if so set the temp var and continue
		if(	apNode->GetParent() == NULL ||
			cMath::CheckAABBInside(apBV->GetMin(), apBV->GetMax(), apNode->GetMin(), apNode->GetMax()))
		{
			mpCheckForFitTempNode = apNode;
			return; 
		}

		///////////////////////////////////////////
		//If no fit in curremt, check parent
		cRCNode_DynBoxTree *pParent =  static_cast<cRCNode_DynBoxTree*>(apNode->GetParent());
		CheckForFitIterative(pParent, apBV);
	}

	
	//-----------------------------------------------------------------------

	

}
