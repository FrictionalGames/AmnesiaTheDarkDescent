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

#include "ai/AStar.h"

#include "ai/AINodeContainer.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// NODE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cAStarNode::cAStarNode(cAINode *apAINode)
	{
		mpParent = NULL;
		mpAINode = apAINode;
	}

	//-----------------------------------------------------------------------

	bool cAStarNodeCompare::operator()(cAStarNode* apNodeA,cAStarNode* apNodeB) const
	{
		return apNodeA->mpAINode < apNodeB->mpAINode;
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cAStarHandler::cAStarHandler(cAINodeContainer *apContainer)
	{
		mlMaxIterations = -1;

		mpContainer = apContainer;

		mpCallback = NULL;
	}

	//-----------------------------------------------------------------------

	cAStarHandler::~cAStarHandler()
	{
		STLDeleteAll(m_setClosedList);
		STLDeleteAll(m_setOpenList);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cAStarHandler::GetPath(const cVector3f& avStart, const cVector3f& avGoal,tAINodeList *apNodeList)
	{
		float fMaxHeight = mpContainer->GetMaxHeight()*1.5f;

		/////////////////////////////////////////////////
		// check if there is free path from start to goal
		float fHeight = fabs(avStart.y - avGoal.y);
		if(fHeight <= fMaxHeight && mpContainer->FreePath(avStart,avGoal,-1,eAIFreePathFlag_SkipDynamic))
		{
			mpGoalNode = NULL;
			return true;
		}

		////////////////////////////////////////////////
		//Reset all variables
		//These should just be cleared and pools used instead.
		STLDeleteAll(m_setClosedList);
		STLDeleteAll(m_setOpenList);
		m_setGoalNodes.clear();
		mpGoalNode=NULL;

		//Set goal position
		mvGoal = avGoal;
		
		
		////////////////////////////////////////////////
		//Find nodes reachable from the start and goal position (use double 2*2 distance)
		float fMaxDist = mpContainer->GetMaxEdgeDistance()*2; //float fMaxDist = mpContainer->GetMaxEdgeDistance()*mpContainer->GetMaxEdgeDistance()*4;
		
		/////////////////////
		//Check with Start
		//Log(" Get Start\n");
		cAINodeIterator startNodeIt =  mpContainer->GetNodeIterator(avStart,fMaxDist);
		while(startNodeIt.HasNext())
		{
			cAINode *pAINode = startNodeIt.Next();
			//Log("Check node: %s\n",pAINode->GetName().c_str());

			float fHeight = fabs(avStart.y - pAINode->GetPosition().y);
			float fDist = cMath::Vector3Dist(avStart,pAINode->GetPosition()); //float fDist = cMath::Vector3DistSqr(avStart,pAINode->GetPosition());
			if(fDist < fMaxDist && fHeight <= fMaxHeight)
			{
				//Check if path is clear
				if(mpContainer->FreePath(avStart,pAINode->GetPosition(),-1,	eAIFreePathFlag_SkipDynamic))
				{
					AddOpenNode(pAINode,NULL,fDist);
				}
			}
		}
		//Log(" Found start\n");

		////////////////////////////////
		//Check with Goal
		//Log(" Get Goal\n");
		cAINodeIterator goalNodeIt =  mpContainer->GetNodeIterator(avGoal,fMaxDist);
		while(goalNodeIt.HasNext())
		{
			cAINode *pAINode = goalNodeIt.Next();
			//Log("Check node: %s\n",pAINode->GetName().c_str());
			
			float fHeight = fabs(avGoal.y - pAINode->GetPosition().y);
			float fDist = cMath::Vector3Dist(avGoal,pAINode->GetPosition()); //fDist = cMath::Vector3DistSqr(avGoal,pAINode->GetPosition());
			if(fDist < fMaxDist && fHeight <= fMaxHeight)
			{
				//Check if path is clear
				if(mpContainer->FreePath(avGoal,pAINode->GetPosition(),-1, eAIFreePathFlag_SkipDynamic))
				{
					m_setGoalNodes.insert(pAINode);
				}
			}
		}
		//Log(" Found goal\n");
		
		/*for(int i=0; i<mpContainer->GetNodeNum(); ++i)
		{
			cAINode *pAINode = mpContainer->GetNode(i);

			////////////////////////////////
			//Check with Start
			float fHeight = fabs(avStart.y - pAINode->GetPosition().y);
			float fDist = cMath::Vector3Dist(avStart,pAINode->GetPosition());
			//float fDist = cMath::Vector3DistSqr(avStart,pAINode->GetPosition());
			if(fDist < fMaxDist && fHeight <= fMaxHeight)
			{
				//Check if path is clear
				if(mpContainer->FreePath(avStart,pAINode->GetPosition(),-1))
				{
					AddOpenNode(pAINode,NULL,fDist);
				}
			}

			////////////////////////////////
			//Check with Goal
			fHeight = fabs(avGoal.y - pAINode->GetPosition().y);
			fDist = cMath::Vector3Dist(avGoal,pAINode->GetPosition());
			//fDist = cMath::Vector3DistSqr(avGoal,pAINode->GetPosition());
			if(fDist < fMaxDist && fHeight <= fMaxHeight)
			{
				//Check if path is clear
				if(mpContainer->FreePath(avGoal,pAINode->GetPosition(),3))
				{
					m_setGoalNodes.insert(pAINode);
				}
			}
		}*/

		////////////////////////////////////////////////
		//Iterate the algorithm
		IterateAlgorithm();

		////////////////////////////////////////////////
		//Check if goal was found, if so build path.
		if(mpGoalNode)
		{
			if(apNodeList)
			{
				cAStarNode *pParentNode = mpGoalNode;
				while(pParentNode != NULL)
				{
					apNodeList->push_back(pParentNode->mpAINode);
					pParentNode = pParentNode->mpParent;
				}
			}

			return true;
		}
		else
		{
			 return false;
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cAStarHandler::IterateAlgorithm()
	{
		int lIterationCount=0;
		while(m_setOpenList.empty()==false && (mlMaxIterations <0 || lIterationCount < mlMaxIterations))
		{
			cAStarNode *pNode = GetBestNode();
			cAINode *pAINode = pNode->mpAINode;

			//////////////////////
			// Check if current node can reach goal
			if(IsGoalNode(pAINode))
			{
				mpGoalNode = pNode;
				break;
			}

			/////////////////////
			//Add nodes connected to current
			int lEdgeCount = pAINode->GetEdgeNum();
			for(int i=0; i< lEdgeCount; ++i)
			{
				cAINodeEdge *pEdge = pAINode->GetEdge(i);

				if(mpCallback == NULL || mpCallback->CanAddNode(pAINode, pEdge->mpNode))
				{
					AddOpenNode(pEdge->mpNode, pNode, pNode->mfDistance + pEdge->mfDistance);
					//AddOpenNode(pEdge->mpNode, pNode, pNode->mfDistance + pEdge->mfSqrDistance);
				}
			}

			++lIterationCount;
		}
	}


	//-----------------------------------------------------------------------

	void cAStarHandler::AddOpenNode(cAINode *apAINode, cAStarNode *apParent, float afDistance)
	{
		//TODO: free path check with dynamic objects here.

		//TODO: Some pooling here would be good.
		cAStarNode *pNode = hplNew( cAStarNode, (apAINode) );

		//Check if it is in closed list.
		tAStarNodeSetIt it = m_setClosedList.find(pNode);
		if(it != m_setClosedList.end()){
			hplDelete(pNode);
			return;
		}

		//Try to add it to the open list
		std::pair<tAStarNodeSetIt, bool> testPair = m_setOpenList.insert(pNode);
		if(testPair.second == false){
			hplDelete(pNode);
			return;
		}

		pNode->mfDistance = afDistance;
		pNode->mfCost = Cost(afDistance,apAINode,apParent) + Heuristic(pNode->mpAINode->GetPosition(), mvGoal);
		pNode->mpParent = apParent;
	}

	//-----------------------------------------------------------------------

	cAStarNode* cAStarHandler::GetBestNode()
	{
		tAStarNodeSetIt it = m_setOpenList.begin();
		tAStarNodeSetIt bestIt = it;
		cAStarNode* pBestNode = *it; ++it;

		//Iterate open list and find the best node.
		for(; it != m_setOpenList.end(); ++it)
		{
			cAStarNode *pNode = *it;
			if(pBestNode->mfCost > pNode->mfCost)
			{
				pBestNode = pNode;
				bestIt = it;
			}
		}

		//Remove node from open
		m_setOpenList.erase(bestIt);

		//Add to closed list
		m_setClosedList.insert(pBestNode);

		return pBestNode;
	}

	//-----------------------------------------------------------------------
	
	float cAStarHandler::Cost(float afDistance, cAINode *apAINode, cAStarNode *apParent)
	{
		if(apParent)
		{
			float fHeight = (1+fabs(apAINode->GetPosition().y - apParent->mpAINode->GetPosition().y));
			return afDistance * fHeight;
		}
		else
			return afDistance;
	}

	//-----------------------------------------------------------------------

	float cAStarHandler::Heuristic(const cVector3f& avStart, const cVector3f& avGoal)
	{
		//return cMath::Vector3DistSqr(avStart, avGoal);
		return cMath::Vector3Dist(avStart, avGoal);
	}

	//-----------------------------------------------------------------------

	bool cAStarHandler::IsGoalNode(cAINode *apAINode)
	{
		tAINodeSetIt it = m_setGoalNodes.find(apAINode);
		if(it == m_setGoalNodes.end()) return false;

		return true;
	}

	//-----------------------------------------------------------------------
}
