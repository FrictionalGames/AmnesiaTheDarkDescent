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

#ifndef HPL_AI_NODE_GENERATOR_H
#define HPL_AI_NODE_GENERATOR_H

#include "system/SystemTypes.h"
#include "engine/EngineTypes.h"

#include "physics/PhysicsWorld.h"
#include "scene/World.h"

namespace hpl {

	class cWorld;
	
	//-------------------------------

	class cAINodeGeneratorParams
	{
	public:
		cAINodeGeneratorParams();
		
		tString msNodeType;
		
		float mfHeightFromGround;
		float mfMinWallDist;

		cVector3f mvMinPos;
		cVector3f mvMaxPos;

		float mfGridSize;
  	};

	//-------------------------------

	class cAINodeGenerator : public iPhysicsRayCallback
	{
	public:
		cAINodeGenerator();
		~cAINodeGenerator();

		void Generate(cWorld* apWorld,cAINodeGeneratorParams *apParams);

	private:
		bool OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams);

		void SaveToFile();
		void LoadFromFile();

		cAINodeGeneratorParams *mpParams;
		cWorld* mpWorld;
		tTempAiNodeList *mpNodeList;
		int mlIDCount;
	};

};
#endif // HPL_AI_NODE_GENERATOR_H
