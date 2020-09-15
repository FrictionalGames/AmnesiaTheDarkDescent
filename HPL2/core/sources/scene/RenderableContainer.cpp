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

#include "scene/RenderableContainer.h"

#include "system/LowLevelSystem.h"

#include "graphics/Renderable.h"
#include "math/Math.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// VISIBLE NODE TRACKER
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVisibleRCNodeTracker::cVisibleRCNodeTracker()
	{
		mlCurrentVisibleNodeSet =0;

		mlFrameCounter =0;
	}

	//-----------------------------------------------------------------------

	void cVisibleRCNodeTracker::SwitchAndClearVisibleNodeSet()
	{
		mlCurrentVisibleNodeSet++;
		if(mlCurrentVisibleNodeSet>=2) mlCurrentVisibleNodeSet=0;
		
		m_setVisibleNodes[mlCurrentVisibleNodeSet].clear();
	}

	void cVisibleRCNodeTracker::SetNodeVisible(iRenderableContainerNode *apNode)
	{
		m_setVisibleNodes[mlCurrentVisibleNodeSet].insert(apNode);
	}

	bool cVisibleRCNodeTracker::WasNodeVisible(iRenderableContainerNode *apNode)
	{
		int lPrevSet = mlCurrentVisibleNodeSet==0 ? 1 : 0;

		tRenderableContainerNodeSetIt it  = m_setVisibleNodes[lPrevSet].find(apNode);
		return it != m_setVisibleNodes[lPrevSet].end();
	}

	//-----------------------------------------------------------------------

	void cVisibleRCNodeTracker::Reset()
	{
		mlCurrentVisibleNodeSet =0;
		mlFrameCounter =0;

		for(int i=0; i<2; i++) m_setVisibleNodes[i].clear();
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// RENDERABLE CALLBACK
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cRenderableContainerObjectCallback::cRenderableContainerObjectCallback()
	{

	}

	//-----------------------------------------------------------------------
	
	static void PushUpNeedPropertyUpdate(iRenderableContainerNode *apNode)
	{
		apNode->SetNeedPropertyUpdate(true);
		
		if(apNode->GetParent()) PushUpNeedPropertyUpdate(apNode->GetParent());
	}

	static inline void PushUpNeedPropertyUpdateFromObject(iRenderable *apObject)
	{
		iRenderableContainerNode *pNode = apObject->GetRenderContainerNode();
		
		if(pNode) PushUpNeedPropertyUpdate(pNode);
	}

	//-----------------------------------------------------------------------

	void cRenderableContainerObjectCallback::OnVisibleChange(iRenderable *apObject)
	{
		PushUpNeedPropertyUpdateFromObject(apObject);
	}
	
	//-----------------------------------------------------------------------

	void cRenderableContainerObjectCallback::OnRenderFlagsChange(iRenderable *apObject)
	{
		PushUpNeedPropertyUpdateFromObject(apObject);	
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// RENDERABLE CONTAINER NODE
	//////////////////////////////////////////////////////////////////////////
	
	iRenderableContainerNode::iRenderableContainerNode()
	{
		mvMin =0;
		mvMax =0;
		mfRadius =0;
		mvCenter =0;

		mbNeedPropertyUpdate = true;
		mbNeedAABBUpdate = false;
		mPrevFrustumCollision = eCollision_Outside;
	}

	//-----------------------------------------------------------------------

	void iRenderableContainerNode::PushUpNeedAABBUpdate()
	{
		if(mbNeedAABBUpdate) return;

		mbNeedAABBUpdate = true;

		if(mpParent) mpParent->PushUpNeedAABBUpdate();
	}

	//-----------------------------------------------------------------------

	void iRenderableContainerNode::CalculateMinMaxFromObjects()
	{
		cVector3f vNodeMin(100000.0f);
		cVector3f vNodeMax(-100000.0f);
		
		tRenderableListIt it = mlstObjects.begin();
		for(; it != mlstObjects.end(); ++it)
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

		mvMin = vNodeMin;
		mvMax = vNodeMax;
	}

	//////////////////////////////////////////////////////////////////////////
	// RENDERABLE CONTAINER
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void iRenderableContainer::UpdateBeforeRendering()
	{
		iRenderableContainerNode *pRoot = GetRoot();

		/////////////////////////
		//Check if root or any children needs to update properties
		if(pRoot->GetNeedPropertyUpdate())
		{
			CheckNeedPropertyUpdateIteration(pRoot);
		}

		/////////////////////////
		//Check if root or any children needs to update AABB
		if(pRoot->GetNeedAABBUpdate())
		{
			CheckNeedAABBUpdateIteration(pRoot);
		}

		/////////////////////////
		//Container specific Update
		SpecificUpdateBeforeRendering();
	}

	//-----------------------------------------------------------------------

	void iRenderableContainer::CheckNeedPropertyUpdateIteration(iRenderableContainerNode* apNode)
	{
		//Reset node variables
		apNode->mlRenderFlags = 0;
		apNode->mbVisibleObjects = false;
		apNode->mbNeedPropertyUpdate = false;

		/////////////////////////////
		// See if child nodes need update and update variables according to nodes too
		if(apNode->HasChildNodes())
		{
			tRenderableContainerNodeListIt childIt = apNode->GetChildNodeList()->begin();
			for(; childIt != apNode->GetChildNodeList()->end(); ++childIt)
			{
				iRenderableContainerNode *pChildNode = *childIt;
				
				if(pChildNode->GetNeedPropertyUpdate()) CheckNeedPropertyUpdateIteration(pChildNode);
				
				//Update flags according to node.
				if(pChildNode->HasVisibleObjects()) apNode->mbVisibleObjects = true;
				apNode->mlRenderFlags |= pChildNode->GetRenderFlags();
			}
		}

		/////////////////////////////
		// Update variables according to objects if any
		if(apNode->HasObjects())
		{
			tRenderableListIt it = apNode->GetObjectList()->begin();
			for(; it != apNode->GetObjectList()->end(); ++it)
			{
				iRenderable *pObject = *it;

				if(pObject->IsVisible() ) apNode->mbVisibleObjects = true;
				apNode->mlRenderFlags |= pObject->GetRenderFlags();
			}
		}
	}

	//-----------------------------------------------------------------------

	void iRenderableContainer::CheckNeedAABBUpdateIteration(iRenderableContainerNode* apNode)
	{
		apNode->mbNeedAABBUpdate = false;

		if(apNode->HasObjects()) 
		{
			apNode->CalculateMinMaxFromObjects();		
		}
		else
		{
			apNode->mvMin = 100000;
			apNode->mvMax = -100000;
		}
		
		/////////////////////////////
		// See if child nodes need update and update variables according to nodes too
		if(apNode->HasChildNodes())
		{
			tRenderableContainerNodeListIt childIt = apNode->GetChildNodeList()->begin();
			for(; childIt != apNode->GetChildNodeList()->end(); ++childIt)
			{
				iRenderableContainerNode *pChildNode = *childIt;

				if(pChildNode->GetNeedAABBUpdate()) CheckNeedAABBUpdateIteration(pChildNode);
				
				cMath::ExpandAABB(apNode->mvMin, apNode->mvMax, pChildNode->mvMin, pChildNode->mvMax);
			}
		}

		//////////////////////////
		// Calculate sphere size and pos
		apNode->mvCenter = (apNode->mvMax + apNode->mvMin) *0.5f;
		apNode->mfRadius = (apNode->mvMax - apNode->mvMin).Length()*0.5f;
	}

	//-----------------------------------------------------------------------
}
