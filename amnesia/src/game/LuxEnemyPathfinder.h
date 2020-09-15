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

#ifndef LUX_ENEMY_PATHFINDER_H
#define LUX_ENEMY_PATHFINDER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class iLuxEnemy;
class cLuxEnemyMover;

//----------------------------------------------

class cLuxEnemyPathfinder
{
friend class cLuxEnemyPathfinder_SaveData;
public:	
	cLuxEnemyPathfinder(iLuxEnemy *apEnemy, cLuxEnemyMover *apMover);
	virtual ~cLuxEnemyPathfinder();

	//////////////////////
	//General
	void SetupAfterLoad(cWorld *apWorld);
	void AfterWorldLoad();
	
	void OnUpdate(float afTimeStep);

	//////////////////////
	//Actions
	bool MoveTo(const cVector3f& avPos);
	void Stop();

	cAINode* GetNodeAtPos(	const cVector3f &avPos,float afMinDistance,float afMaxDistance, bool abGetClosest, 
							bool abPosToNodeFreePathCheck,bool abEnemyToNodeFreePathCheck,
							cAINode *apSkipNode, int alFreePathRayNum=-1, 
							tAIFreePathFlag alFreePathFlags=eAIFreePathFlag_SkipDynamic,
							bool abSkipUsedNodes=true);
	
	//////////////////////
	//Properties
	tAINodeList* GetNodeList(){ return &mlstPathNodes;}

	bool IsMoving(){ return mbMoving;}
	cVector3f GetNextGoalPos();
	const cVector3f& GetFinalGoalPos();

	cAINodeContainer* GetNodeContainer(){ return mpNodeContainer;}

	//////////////////////
	//Data


	//////////////////////
	//Debug
	void OnRenderSolid(cRendererCallbackFunctions* apFunctions);
	
	//////////////////////
	//Save data stuff
	
private:
	void UpdateMoving(float afTimeStep);

	iLuxEnemy *mpEnemy;
	cLuxEnemyMover *mpMover;

	cAINodeContainer *mpNodeContainer;
	cAStarHandler *mpAStar;

    bool mbMoving;
	cVector3f mvMoveGoalPos;

	tAINodeList mlstPathNodes;
	std::list<float> mlstPathNodeDistances;
};

//----------------------------------------------


#endif // LUX_ENEMY_PATHFINDER_H
