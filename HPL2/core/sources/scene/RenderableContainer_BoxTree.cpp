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

#include "scene/RenderableContainer_BoxTree.h"

#include "graphics/Renderable.h"
#include "graphics/Renderer.h"
#include "graphics/LowLevelGraphics.h"

#include "system/LowLevelSystem.h"

#include <algorithm>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// NODE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cRCNode_BoxTree::cRCNode_BoxTree()
	{
		mpParent = NULL;

		mbUsesFlagsAndVisibility = true;
	}

	//-----------------------------------------------------------------------

	cRCNode_BoxTree::~cRCNode_BoxTree()
	{
		STLDeleteAll(mlstChildNodes);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cRenderableContainer_BoxTree::cRenderableContainer_BoxTree()
	{
		mlMinLeafObjects = 12;		//The minimum number of objects in each leaf.
		mfMinSideLength = 3.0f;		//The smallest largest side allowed before the splitting stop.
		mfMaxSideLength = 30.0f;	//The largest side allowed for splitting to stop (overrides mlMinLeafObjects!)

		mfMaxIntersectionAmount = 0.03f;	//The amount (1=100%) an object can be over the splitting line and still not be an intersecting object
											//Also is the amount by which a new AABB can be expanded.

		mlMaxVolumeCalcObjects = 200;		//The maximum number of objects present if a cpu intense volume calculations should be used.

		//THere are too few objects to create an intersection group, objects that have the relative length (percent of node size)
		//will still remian in intersection.
		mfMinForceIntersectionRelativeSize = 0.8f;

		//Create the root
		mpRoot = hplNew( cRCNode_BoxTree, ());
		mpRoot->mpParent = NULL;
		mpRoot->mfViewDistance =0;
		mpRoot->mbInsideView = true;

		mpObjectCalllback = hplNew( cRenderableContainerObjectCallback, () );
	}
	
	cRenderableContainer_BoxTree::~cRenderableContainer_BoxTree()
	{
		if(mpRoot) hplDelete(mpRoot);

		hplDelete( mpObjectCalllback );
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cRenderableContainer_BoxTree::Add(iRenderable *apRenderable)
	{
		m_mlstTempObjects.push_back(apRenderable);
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_BoxTree::Remove(iRenderable *apRenderable)
	{
		STLFindAndRemove(m_mlstTempObjects, apRenderable);
	}

	//-----------------------------------------------------------------------

	iRenderableContainerNode* cRenderableContainer_BoxTree::GetRoot()
	{
		return mpRoot;
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_BoxTree::Compile()
	{
		//Create root (delete first if needed)
		if(mpRoot) hplDelete(mpRoot);
		mpRoot = hplNew( cRCNode_BoxTree, ());
		mpRoot->mpParent = NULL;
		mpRoot->mfViewDistance =0;
		mpRoot->mbInsideView = true;

		//Set up temp root node.
		cBoxTreeTempNode tempRoot(NULL);
		
		/////////////////////////////////////////////////
		//Start by building the temp nodes where every node contains all children (that will later be in child nodes) and
		//will later be used to easily calculated bounding volume for each node.
		tRenderableListIt it = m_mlstTempObjects.begin();
		for(; it != m_mlstTempObjects.end(); ++it)
		{
			tempRoot.mlstObjects.push_back(*it);
		}
		CompileTempNode(&tempRoot,0,-1);

		//////////////////////////////
		//Build the actual node tree from temp nodes
		BuildNodeFromTemp(&tempRoot, mpRoot,0);
	}

	//-----------------------------------------------------------------------

	int glCount =0;
	int glDrawLevel=0;

	void cRenderableContainer_BoxTree::RenderDebug(cRendererCallbackFunctions *apFunctions)
	{
		glCount++;
		if(glCount > 300)
		{
			glCount =0;
			glDrawLevel++;
			if(glDrawLevel > 5) glDrawLevel =0;
		}
		
		apFunctions->SetDepthTest(true);
		apFunctions->SetDepthWrite(false);
		apFunctions->SetBlendMode(eMaterialBlendMode_None);

		apFunctions->SetProgram(NULL);
		apFunctions->SetTextureRange(NULL, 0);
		apFunctions->SetMatrix(NULL);


		RenderDebugNode(apFunctions, mpRoot,0);
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	
	static tString gsTempSpaces="";

	const char *GetSpaces(int alNum)
	{
		gsTempSpaces.resize(alNum,'\t');
        return gsTempSpaces.c_str();		
	}

	//-----------------------------------------------------------------------

	//0=x, 1=y, 2=z
	float GetAxisFromVec(const cVector3f& avVec, int alAxis)
	{
		switch(alAxis)
		{
		case 0: return avVec.x;
		case 1: return avVec.y;
		case 2: return avVec.z;
		}

		return 0;
	}

	//Checks is child is NULL, and if so creates child else just returns it.
	cBoxTreeTempNode* CreateNodeIfNeeded(cBoxTreeTempNode* apParentNode, cBoxTreeTempNode* &apChildNode)
	{
		if(apChildNode) return apChildNode;

        apChildNode = hplNew( cBoxTreeTempNode,(apParentNode) );
		apParentNode->mlstChildren.push_back(apChildNode);

		return apChildNode;
	}

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

	/**
	 * Returns 0=above, 1=below, 2=intersection
	 */
	int cRenderableContainer_BoxTree::GetSplitGroup(iRenderable *apObject, float afCutPlane, int alAxis, const cVector3f &avNodeSize)
	{
		float fMinVal =   GetAxisFromVec(apObject->GetBoundingVolume()->GetMin(),alAxis);
		float fMaxVal =   GetAxisFromVec(apObject->GetBoundingVolume()->GetMax(),alAxis);
		
		//////////////////////////
		//Above cut plane
		if(fMinVal >= afCutPlane)
		{
			return 0;
		}
		//////////////////////////
		//Below cut plane
		else if(fMaxVal <= afCutPlane)
		{
			return 1;
		}
		//////////////////////////
		//Intersection
		else
		{
			////////////////////////
			//Calculate how much the object crosses over on each side
			float fAboveDist = fMaxVal - afCutPlane;
			float fBelowDist = afCutPlane - fMinVal;
			
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
	
	float cRenderableContainer_BoxTree::CalculateObjectsVolume(tRenderableList &alstObjects)
	{
		if(alstObjects.empty()) return 0;

		cVector3f vMin,vMax;
		CalculateMinMax(&alstObjects,vMin,vMax);

		cVector3f vSize = vMax - vMin;

		return vSize.x * vSize.y * vSize.z;
	}

	//-----------------------------------------------------------------------
	
	float cRenderableContainer_BoxTree::CalculateBestCutPlane(tRenderableList &alstObjects, int alAxis, const cVector3f &avNodeSize)
	{
		float fBestCutPlane =0;
		float fBestHeuristic =-1;

		////////////////////////////////////
		// Iterate objects and use each min value as a split value
		tRenderableListIt splitObjectIt = alstObjects.begin();
		for(; splitObjectIt != alstObjects.end(); ++splitObjectIt)
		{
            iRenderable *pSplitObject = *splitObjectIt;
			float fCutPlaneVal = GetAxisFromVec(pSplitObject->GetBoundingVolume()->GetMin(), alAxis);
			tRenderableList vObjectGroup[3];


			////////////////////////////////////
			// Iterate objects and split them as below, over or intersecting and increase min, max for that volume.
			tRenderableListIt testObjectIt = alstObjects.begin();
			for(; testObjectIt != alstObjects.end(); ++testObjectIt)
			{
				iRenderable *pTestObject = *testObjectIt;

				int lGroup = GetSplitGroup(pTestObject,fCutPlaneVal,alAxis,avNodeSize);
				
				//Above cut plane
				if(lGroup==0)		vObjectGroup[0].push_back(pTestObject);
				//Below cut plane
				else if(lGroup==1)	vObjectGroup[1].push_back(pTestObject);
				//Intersection
				else				vObjectGroup[2].push_back(pTestObject);
			}

			///////////////////////////////////
			// TODO: Calculate heuristic
			float VolMul0 = CalculateObjectsVolume(vObjectGroup[0]) * (float)vObjectGroup[0].size();
			float VolMul1 = CalculateObjectsVolume(vObjectGroup[1]) * (float)vObjectGroup[1].size();
			float VolMul2 = CalculateObjectsVolume(vObjectGroup[2]) * (float)vObjectGroup[2].size();

			float fH = VolMul0 + VolMul1 + VolMul2;

			if(fH < fBestHeuristic || fBestHeuristic <0)
			{
				fBestHeuristic = fH;
				fBestCutPlane = fCutPlaneVal;
			}
		}

		return fBestCutPlane;
	}

	//-----------------------------------------------------------------------

	static float GetLongestSide(const cVector3f& avSize)
	{
		float fLongestSide = avSize.x;
		if(fLongestSide < avSize.y)	fLongestSide = avSize.y;
		if(fLongestSide < avSize.z)	fLongestSide = avSize.z;
		return fLongestSide;
	}

	
	void cRenderableContainer_BoxTree::CompileTempNode(cBoxTreeTempNode *apNode, int alLevel, int alSplitAxis)
	{
		const bool bLog = false;

		///////////////////////////
		// Calculate the size of the node
		cVector3f vMin, vMax;
		CalculateMinMax(&apNode->mlstObjects,vMin,vMax);
		cVector3f vNodeSize = vMax - vMin;

		float fLongestSide = GetLongestSide(vNodeSize);
		if(fLongestSide < mfMinSideLength)
		{
			if(bLog)
			{
				Log("%s==================\n",GetSpaces(alLevel));
				Log("%s Skipping Node %d because largest side is to short (only %f)\n",GetSpaces(alLevel),apNode,fLongestSide);
				Log("%s==================\n",GetSpaces(alLevel));
			}
			return;
		}

		///////////////////////////
		// See if there is enough object to continue splitting
		int lNumOfObjects = (int)apNode->mlstObjects.size();
		if(	lNumOfObjects < mlMinLeafObjects *2 &&	//Multiplied by two, because we want to split it too.
			apNode->mpParent != NULL &&				//Always try and split first node independant of object number!
			fLongestSide < mfMaxSideLength)			//Override number of objects if the longest side is long enough!			
		{
			if(bLog)
			{
				Log("%s==================\n",GetSpaces(alLevel));
				Log("%s Skipping Node %d because of too few objects (only %d)\n",GetSpaces(alLevel),apNode, lNumOfObjects);
				Log("%s==================\n",GetSpaces(alLevel));
			}
			return;
		}
				
		///////////////////////////
		// Calculate the axis
		int lAxis = alSplitAxis;
		if(lAxis <0) //If axis < 0, make it depend on largest side
		{
			if(fLongestSide == vNodeSize.x)			lAxis =0;			
			else if(fLongestSide == vNodeSize.y)	lAxis =1;
			else if(fLongestSide == vNodeSize.z)	lAxis =2;
		}
		
		
		if(bLog)
		{
			Log("%sNode %d Parent: %d level %d. Axis: %d Size: (%s) Objects: %d\n",GetSpaces(alLevel),apNode,apNode->mpParent, alLevel,lAxis,vNodeSize.ToString().c_str(), lNumOfObjects);
			Log("%s==================================\n",GetSpaces(alLevel));
		}


		////////////////////////////////
		//Setup vector with all objects
		std::vector<iRenderable*> vSortedObjects;
		vSortedObjects.reserve(apNode->mlstObjects.size());

		for(tRenderableListIt it = apNode->mlstObjects.begin(); it != apNode->mlstObjects.end(); ++it)
		{
			vSortedObjects.push_back(*it);
		}


		float fCutPlaneVal;

		////////////////////////////////////////
		// Calculate best cut plane by evaluation several and comparing with a heuristic
		// Only use if there the number of objects is than a certain number (to increase speed)
		if((int)apNode->mlstObjects.size() < mlMaxVolumeCalcObjects)
		{
			fCutPlaneVal = CalculateBestCutPlane(apNode->mlstObjects,lAxis,vNodeSize);
		}
		//////////////////////////////
		// If many objects, do a faster calculation for split plane.
		// Get Plane cut value by sorting temp vec and getting median.
		else
		{
			int lMedian = lNumOfObjects / 2;

			//Sort vector
			std::sort(vSortedObjects.begin(), vSortedObjects.end(), gvSortFunctions[lAxis]);

			//Get the cut value
			fCutPlaneVal = GetAxisFromVec(vSortedObjects[lMedian]->GetBoundingVolume()->GetMin(), lAxis);
		}
				
		
		////////////////////////////////////////
		//Add objects to children depending on where they are in the clipping plane
		
		if(bLog) Log("%s ---------------\n",GetSpaces(alLevel));
        
		cBoxTreeTempNode *pHighChild=NULL;			//For objects above fCutPlaneVal
		cBoxTreeTempNode *pLowChild=NULL;			//For objects below fCutPlaneVal
		cBoxTreeTempNode *pIntersectChild=NULL;		//For object both above and below
				
		for(size_t i=0; i<vSortedObjects.size(); ++i)
		{
			iRenderable *pObject = vSortedObjects[i];
			if(bLog) Log("%s object: '%s', pos: (%s)",GetSpaces(alLevel), pObject->GetName().c_str(), 
													pObject->GetBoundingVolume()->GetWorldCenter().ToString().c_str());
			
			int lGroup = GetSplitGroup(pObject,fCutPlaneVal,lAxis,vNodeSize);

            //////////////////////////
			//Above cut plane
			if(lGroup==0)
			{
				if(bLog) Log("  adding above...\n");
				CreateNodeIfNeeded(apNode, pHighChild)->mlstObjects.push_back(pObject);
			}
			//////////////////////////
			//Below cut plane
			else if(lGroup==1)
			{
				if(bLog) Log("  adding below...\n");
				CreateNodeIfNeeded(apNode, pLowChild)->mlstObjects.push_back(pObject);
			}
			//////////////////////////
			//Intersection
			else
			{
				if(bLog) Log("  adding intersect...\n");
				CreateNodeIfNeeded(apNode, pIntersectChild)->mlstObjects.push_back(pObject);
			}
		}
		
		

		/////////////////////////////////////
		//Get longest sides of each group
		float fHigh_LongestSide=0,  fLow_LongestSide=0, fIntersect_LongestSide=0;
		if(pHighChild)
			fHigh_LongestSide = GetLongestSide(CalculateSize(&pHighChild->mlstObjects));
		if(pLowChild)
			fLow_LongestSide = GetLongestSide(CalculateSize(&pLowChild->mlstObjects));
		if(pIntersectChild)
			fIntersect_LongestSide = GetLongestSide(CalculateSize(&pIntersectChild->mlstObjects));

		if(bLog)
		{
			if(pHighChild) Log("%s objects above: %d Longest side: %f\n",GetSpaces(alLevel), pHighChild->mlstObjects.size(), fHigh_LongestSide);
			if(pLowChild) Log("%s objects below: %d Longest side: %f\n",GetSpaces(alLevel), pLowChild->mlstObjects.size(), fLow_LongestSide);
			if(pIntersectChild) Log("%s objects intersected: %d Longest side: %f\n",GetSpaces(alLevel), pIntersectChild->mlstObjects.size(), fIntersect_LongestSide);
		}
		
		/////////////////////////////////////
		//Check if any child node as less than minimum objects, if so rearrange.
		bool bMovedObjects = false;
		
		// Intersected:
		// If intersected is too small then spread these out among high and low.
		if(pIntersectChild && (int)pIntersectChild->mlstObjects.size() < mlMinLeafObjects && fIntersect_LongestSide < mfMaxSideLength)
		{
			bool bSkippedMoving=false;
			tRenderableListIt it = pIntersectChild->mlstObjects.begin();
			for(; it != pIntersectChild->mlstObjects.end();)
			{
				iRenderable *pObject = *it;
				
				float fMinVal =   GetAxisFromVec(pObject->GetBoundingVolume()->GetMin(),lAxis);
				float fMaxVal =   GetAxisFromVec(pObject->GetBoundingVolume()->GetMax(),lAxis);
				float fNodeSize = GetAxisFromVec(vNodeSize, lAxis);	
				float fObjectSize = fMaxVal - fMinVal;

				float fAboveDist = fMaxVal - fCutPlaneVal;
				float fBelowDist = fCutPlaneVal - fMinVal;
				
				
				//Check if object is large enough to remain.
				if(fObjectSize > fNodeSize * mfMinForceIntersectionRelativeSize)
				{
					bSkippedMoving = true;
					++it;
				}
				//If object is in more in the high child, move it there
				else if( pHighChild && (fAboveDist > fBelowDist || pLowChild==NULL) )
				{
					if(bLog) Log("%s  moving '%s' to high!\n",GetSpaces(alLevel),pObject->GetName().c_str());
					pHighChild->mlstObjects.push_back(pObject);	
					it = pIntersectChild->mlstObjects.erase(it);
					bMovedObjects = true;
				}
				//If objects is more the lower child, move it there
				else if(pLowChild)
				{
					if(bLog) Log("%s  moving '%s' to low!\n",GetSpaces(alLevel),pObject->GetName().c_str());
					pLowChild->mlstObjects.push_back(pObject);	
					it = pIntersectChild->mlstObjects.erase(it);
					bMovedObjects = true;
				}
				//No high or low, then nothing to move to.
				else
				{
					bSkippedMoving = true;
					++it;
				}
			}
			
			//If not all objects where moved, then do not delete node.
			if(bSkippedMoving ==false)
			{
				STLFindAndDelete(apNode->mlstChildren, pIntersectChild);
				pIntersectChild = NULL;
			}
		}
		// Above:
		if(pHighChild && (int)pHighChild->mlstObjects.size() < mlMinLeafObjects && pIntersectChild && fHigh_LongestSide < mfMaxSideLength)
		{
			if(bLog) Log("%s moving high to intersected!\n",GetSpaces(alLevel));
			pIntersectChild->mlstObjects.splice(pIntersectChild->mlstObjects.end(),pHighChild->mlstObjects);		
			STLFindAndDelete(apNode->mlstChildren, pHighChild);
			pHighChild = NULL;
			bMovedObjects = true;
		}
		// Below
		if(pLowChild && (int)pLowChild->mlstObjects.size() < mlMinLeafObjects && pIntersectChild && fLow_LongestSide < mfMaxSideLength)
		{
			if(bLog) Log("%s moving low to intersected!\n",GetSpaces(alLevel));
			pIntersectChild->mlstObjects.splice(pIntersectChild->mlstObjects.end(),pLowChild->mlstObjects);		
			STLFindAndDelete(apNode->mlstChildren, pLowChild);
			pLowChild = NULL;
			bMovedObjects = true;
		}
		
		if(bLog && bMovedObjects)
		{
			if(pHighChild) Log("%s objects above: %d\n",GetSpaces(alLevel), pHighChild->mlstObjects.size());
			if(pLowChild) Log("%s objects below: %d\n",GetSpaces(alLevel), pLowChild->mlstObjects.size());
			if(pIntersectChild) Log("%s objects intersected: %d\n",GetSpaces(alLevel), pIntersectChild->mlstObjects.size());
		}
		
		if(bLog) Log("%s ---------------\n",GetSpaces(alLevel));
		
		///////////////////////////////////////
		//If only one child node was created, try splitting on another axis
		// only one child means no change in the space partition so no use.
		// when all axes have been tried
		if(apNode->mlstChildren.size() <= 1)
		{
			if(bLog) Log("%s==================================\n",GetSpaces(alLevel));

			STLDeleteAll(apNode->mlstChildren);
			
			//If not all split directions have been tried, try another.
			apNode->mlSplitAxisCount++;
			if(apNode->mlSplitAxisCount < 3)
			{
				int lNewAxis = lAxis +1;
				if(lNewAxis > 2) lNewAxis =0;

				CompileTempNode(apNode,alLevel, lNewAxis);
			}
			
			return;
		}

		////////////////////////////////////////
		// Compile all of the nodes created.
		
		if(pHighChild)
		{
			CompileTempNode(pHighChild,alLevel+1, -1);
		}
		if(pLowChild)
		{
			CompileTempNode(pLowChild,alLevel+1, -1);
		}
		if(pIntersectChild)
		{
			CompileTempNode(pIntersectChild,alLevel+1, -1);
		}

		if(bLog) Log("%s==================================\n",GetSpaces(alLevel));
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_BoxTree::BuildNodeFromTemp(cBoxTreeTempNode *apTempNode, cRCNode_BoxTree *apNode, int alLevel)
	{
		const bool bLog = false;

		bool bIsLeaf = apTempNode->mlstChildren.empty();

		if(bLog)
		{
			Log("%s-----------------------------------\n",GetSpaces(alLevel));
			Log("%sTempNode level %d Children: %d Objects: %d\n",GetSpaces(alLevel), alLevel,apTempNode->mlstChildren.size(),apTempNode->mlstObjects.size() );
			tRenderableListIt it = apTempNode->mlstObjects.begin();
			for(; it != apTempNode->mlstObjects.end(); ++it)
			{
				iRenderable *pObject = *it;
				Log("%s   '%s'\n",GetSpaces(alLevel), pObject->GetName().c_str());
			}
		}

		////////////////////////////
		//Create the bounding volume, box and sphere
		CalculateMinMax(&apTempNode->mlstObjects,apNode->mvMin,apNode->mvMax);

		apNode->mvCenter = (apNode->mvMax + apNode->mvMin) *0.5f;
		apNode->mfRadius = (apNode->mvMax - apNode->mvMin).Length()*0.5f;
				
		////////////////////////////
		//If leaf, add objects.
		if(bIsLeaf)
		{
			if(bLog) {
                Log("%sIs leaf so adding all objects to it!\n",GetSpaces(alLevel));
				Log("%s-----------------------------------\n",GetSpaces(alLevel));
			}
			
			tRenderableListIt it = apTempNode->mlstObjects.begin();
			for(; it != apTempNode->mlstObjects.end(); ++it)
			{
				iRenderable *pObject = *it;

				apNode->mlstObjects.push_back(pObject);

				//Set object callback and node.
				pObject->SetRenderCallback(mpObjectCalllback);
				pObject->SetRenderContainerNode(apNode);
			}
		}
		

		////////////////////////////
		//Add children
       tBoxTreeTempNodeListIt childIt = apTempNode->mlstChildren.begin();
	   for(; childIt != apTempNode->mlstChildren.end(); ++childIt)
	   {
			cBoxTreeTempNode *pTempChildNode = *childIt;
			cRCNode_BoxTree *pChildNode = hplNew(cRCNode_BoxTree, () );
            pChildNode->mpParent = apNode;
			apNode->mlstChildNodes.push_back(pChildNode);
			
            BuildNodeFromTemp(pTempChildNode, pChildNode,alLevel+1);
	   }
	}

	//-----------------------------------------------------------------------
	static cColor LevelColor[10] = {cColor(1,1,1),cColor(1,0,1),cColor(1,1,0),cColor(0,1,1),cColor(0,0,1),cColor(0,1,0),cColor(1,0,0),cColor(1,0.5f,1),
									cColor(1,1,0.5f), cColor(1,0.5f,0.5f)};
	void cRenderableContainer_BoxTree::RenderDebugNode(cRendererCallbackFunctions *apFunctions, cRCNode_BoxTree *apNode, int alLevel)
	{
		//if(apNode->GetNodeList()->empty())
		if(glDrawLevel==alLevel || (glDrawLevel > alLevel && apNode->GetChildNodeList()->empty()))
			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(apNode->GetMin(),apNode->GetMax(),LevelColor[alLevel % 10]);

		tRenderableContainerNodeListIt childIt = apNode->GetChildNodeList()->begin();
		for(; childIt != apNode->GetChildNodeList()->end(); ++childIt)
		{
			cRCNode_BoxTree *pChildNode = static_cast<cRCNode_BoxTree*>(*childIt);
			RenderDebugNode(apFunctions,pChildNode , alLevel+1);
		}


		tRenderableListIt objIt = apNode->GetObjectList()->begin();
		for(; objIt != apNode->GetObjectList()->end(); ++objIt)
		{
			iRenderable *pObj = *objIt;
			//cBoundingVolume *pBV = pObj->GetBoundingVolume();
			//apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(),pBV->GetMax(),LevelColor[alLevel % 10]);	
		}
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_BoxTree::CalculateMinMax(tRenderableList *apObjectList, cVector3f& avMin, cVector3f& avMax)
	{
		cVector3f vNodeMin(100000.0f);
		cVector3f vNodeMax(-100000.0f);
		tRenderableListIt it = apObjectList->begin();
		for(; it != apObjectList->end(); ++it)
		{
			iRenderable *pObject = *it;

			const cVector3f &vMin = pObject->GetBoundingVolume()->GetMin();
			const cVector3f &vMax = pObject->GetBoundingVolume()->GetMax();

			if(vNodeMin.x > vMin.x) vNodeMin.x = vMin.x;
			if(vNodeMin.y > vMin.y) vNodeMin.y = vMin.y;
			if(vNodeMin.z > vMin.z) vNodeMin.z = vMin.z;

			if(vNodeMax.x < vMax.x) vNodeMax.x = vMax.x;
			if(vNodeMax.y < vMax.y) vNodeMax.y = vMax.y;
			if(vNodeMax.z < vMax.z) vNodeMax.z = vMax.z;
		}

		avMin = vNodeMin;
		avMax = vNodeMax;
	}

	cVector3f cRenderableContainer_BoxTree::CalculateSize(tRenderableList *apObjectList)
	{
		cVector3f vMin, vMax;
		CalculateMinMax(apObjectList, vMin, vMax);
		return vMax - vMin;
	}

	//-----------------------------------------------------------------------

}
