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

#include "physics/PhysicsWorld.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsMaterial.h"
#include "physics/CharacterBody.h"
#include "physics/PhysicsJoint.h"
#include "physics/PhysicsController.h"
#include "physics/PhysicsRope.h"
#include "physics/SurfaceData.h"
#include "system/LowLevelSystem.h"
#include "system/System.h"
#include "math/Math.h"
#include "graphics/LowLevelGraphics.h"
#include "scene/World.h"
#include "system/Platform.h"
#include "scene/SoundEntity.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iPhysicsWorld::iPhysicsWorld()
	{
		mbLogDebug = false;
	}

	//-----------------------------------------------------------------------

	iPhysicsWorld::~iPhysicsWorld()
	{
		
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void iPhysicsWorld::Update(float afTimeStep)
	{
		//Clear all contact points.
		mvContactPoints.clear();

		////////////////////////////////////
		//Update controllers
		for(tPhysicsControllerListIt CtrlIt = mlstControllers.begin(); CtrlIt != mlstControllers.end(); ++CtrlIt)
		{
			iPhysicsController *pCtrl = *CtrlIt;

			pCtrl->Update(afTimeStep);
		}

		////////////////////////////////////
		//Update Ropes before simulate
		for(tPhysicsRopeListIt it = mlstRopes.begin(); it != mlstRopes.end(); ++it)
		{
			iPhysicsRope *pRope = *it;
			
			pRope->UpdateBeforeSimulate(afTimeStep);
		}

		////////////////////////////////////
		//Update character bodies
		START_TIMING(PhysicsCharacters)		
		for(tCharacterBodyListIt CharIt = mlstCharBodies.begin(); CharIt != mlstCharBodies.end(); ++CharIt)
		{
			iCharacterBody *pBody = *CharIt;

			pBody->Update(afTimeStep);
		}
		STOP_TIMING(PhysicsCharacters)

		
		////////////////////////////////////
		//Update the rigid bodies before simulation.
		START_TIMING(BodyBeforeSimulate)
		tPhysicsBodyList lstRemoveUpdateBodies;
		for(tPhysicsBodySetIt BodyIt = m_setUpdateBodies.begin(); BodyIt != m_setUpdateBodies.end(); ++BodyIt)
		{
			iPhysicsBody *pBody = *BodyIt;

			if(pBody->UpdateBeforeSimulate(afTimeStep)==false)
			{
				//Add to remove list
				lstRemoveUpdateBodies.push_back(pBody);
			}
		}
				
		//Iterate remove list and remove all bodies in it from the update list.
        if(lstRemoveUpdateBodies.empty()==false)
		{
			for(tPhysicsBodyListIt it = lstRemoveUpdateBodies.begin(); it != lstRemoveUpdateBodies.end(); ++it)
			{
				iPhysicsBody *pBody = *it;

				RemoveBodyFromUpdateList(pBody, false);
			}
		}
		STOP_TIMING(BodyBeforeSimulate)
		

		////////////////////////////////////
		//Simulate the physics
		START_TIMING(Simulate)				
		Simulate(afTimeStep);
		STOP_TIMING(Simulate)	
				
		////////////////////////////////////
		//Update the joints after simulation.
		for(tPhysicsJointListIt JointIt = mlstJoints.begin(); JointIt != mlstJoints.end(); )
		{
			iPhysicsJoint *pJoint = *JointIt;

			if(pJoint->OnPhysicsUpdate()==false)
			{
				++JointIt;
				continue;
			}
            
			if(pJoint->CheckBreakage())
			{
				JointIt = mlstJoints.erase(JointIt);
				hplDelete(pJoint);
			}
			else
			{
				++JointIt;
			}
		}
		////////////////////////////////////
		//Update the rigid bodies after simulation.
		START_TIMING(BodyAfterSimulate)	
		for(tPhysicsBodySetIt BodyIt = m_setUpdateBodies.begin(); BodyIt != m_setUpdateBodies.end(); ++BodyIt)
		{
			iPhysicsBody *pBody = *BodyIt;
			
			pBody->UpdateAfterSimulate(afTimeStep);
		}
		STOP_TIMING(BodyAfterSimulate)	

		////////////////////////////////////
		//Update Ropes after simulate
		for(tPhysicsRopeListIt it = mlstRopes.begin(); it != mlstRopes.end(); ++it)
		{
			iPhysicsRope *pRope = *it;

			pRope->UpdateAfterSimulate(afTimeStep);
		}
	}
	
	//-----------------------------------------------------------------------

	void iPhysicsWorld::DestroyShape(iCollideShape *apShape)
	{
		apShape->DecUserCount();
		if(apShape->HasUsers()==false)
		{
			STLFindAndDelete(mlstShapes, apShape);
		}
	}

	//-----------------------------------------------------------------------
	
	void iPhysicsWorld::DestroyBody(iPhysicsBody* apBody)
	{
		if(apBody->IsInUpdateList()) RemoveBodyFromUpdateList(apBody, true);
				
		tPhysicsBodyListIt it = mlstBodies.begin();
		for(; it != mlstBodies.end(); ++it)
		{
			iPhysicsBody *pBody = *it;
			if(pBody == apBody)
			{
				pBody->Destroy();
				hplDelete(pBody);
				mlstBodies.erase(it);
				return;
			}
		}
	}

	iPhysicsBody *iPhysicsWorld::GetBody(const tString &asName)
	{
		return (iPhysicsBody*)STLFindByName(mlstBodies, asName);
	}

	cPhysicsBodyIterator iPhysicsWorld::GetBodyIterator()
	{
		return cPhysicsBodyIterator(&mlstBodies);
	}

	//-----------------------------------------------------------------------

	void iPhysicsWorld::EnableBodiesInBV(cBoundingVolume *apBV, bool abEnabled)
	{
		mvTempBodies.resize(0);
        GetBodiesInBV(apBV, &mvTempBodies);

		for(size_t i=0; i<mvTempBodies.size(); ++i)
		{
			iPhysicsBody *pBody = mvTempBodies[i];
			
			if(pBody->GetMass() > 0 && cMath::CheckBVIntersection(*apBV,*pBody->GetBoundingVolume()))
			{
				pBody->Enable();
			}
		}
	}

	//-----------------------------------------------------------------------

	void iPhysicsWorld::AddBodyToUpdateList(iPhysicsBody *apBody)
	{
		if(apBody->IsInUpdateList()) return;

		m_setUpdateBodies.insert(apBody);
		apBody->SetInUpdateList(true);
	}

	//NOTE: Skipping removing dynamic bodies for now since they will need to be checked if enabled 
	//		Still want to remove when destroying though.
	void iPhysicsWorld::RemoveBodyFromUpdateList(iPhysicsBody *apBody, bool abDestroyingBody)
	{
		if(apBody->IsInUpdateList()==false) return;

		m_setUpdateBodies.erase(apBody);
		apBody->SetInUpdateList(false);
	}

	//-----------------------------------------------------------------------

	void iPhysicsWorld::DestroyJoint(iPhysicsJoint* apJoint)
	{
		STLFindAndDelete(mlstJoints, apJoint);
	}

	iPhysicsJoint *iPhysicsWorld::GetJoint(const tString &asName)
	{
		return (iPhysicsJoint*)STLFindByName(mlstJoints, asName);
	}

	bool iPhysicsWorld::JointExists(iPhysicsJoint* apJoint)
	{
		tPhysicsJointListIt JointIt = mlstJoints.begin();
		for(; JointIt != mlstJoints.end(); ++JointIt)
		{
			if(*JointIt == apJoint) return true;
		}
		return false;
	}

	cPhysicsJointIterator iPhysicsWorld::GetJointIterator()
	{
		return cPhysicsJointIterator(&mlstJoints);
	}

	//-----------------------------------------------------------------------

	void iPhysicsWorld::DestroyCharacterBody(iCharacterBody* apBody)
	{
		STLFindAndDelete(mlstCharBodies, apBody);		
	}
		
	iPhysicsBody *iPhysicsWorld::GetCharacterBody(const tString &asName)
	{
		return (iPhysicsBody*)STLFindByName(mlstCharBodies, asName);
	}

	//-----------------------------------------------------------------------
	
	iPhysicsMaterial* iPhysicsWorld::GetMaterialFromName(const tString &asName)
	{
		tPhysicsMaterialMapIt it = m_mapMaterials.find(asName);
		if(it == m_mapMaterials.end()){
			return NULL;
		}

		iPhysicsMaterial *pMaterial =it->second;

        if(pMaterial->IsPreloaded()==false && pMaterial->GetSurfaceData())
		{
			pMaterial->SetPreloaded(true);
			pMaterial->GetSurfaceData()->PreloadData();
		}

		return pMaterial;
	}
	
	//-----------------------------------------------------------------------

	cPhysicsMaterialIterator iPhysicsWorld::GetMaterialIterator()
	{
		return cPhysicsMaterialIterator(&m_mapMaterials);
	}

	//-----------------------------------------------------------------------

	iPhysicsRope* iPhysicsWorld::GetRope(const tString &asName)
	{
		return (iPhysicsRope*) STLFindByName(mlstRopes, asName);
	}

	iPhysicsRope* iPhysicsWorld::GetRopeFromUniqueID(int alID)
	{
		for(tPhysicsRopeListIt it = mlstRopes.begin(); it != mlstRopes.end(); ++it)
		{
			iPhysicsRope* pRope = *it;
			
			if(pRope->GetUniqueID() == alID) return pRope;
		}
		return NULL;
	}

	void iPhysicsWorld::DestroyRope(iPhysicsRope* apRope)
	{
		STLFindAndDelete(mlstRopes, apRope);
	}

	//-----------------------------------------------------------------------
	
	void iPhysicsWorld::DestroyAll()
	{
		STLDeleteAll(mlstCharBodies);
		
		//Bodies
		tPhysicsBodyListIt it = mlstBodies.begin();
		for(; it != mlstBodies.end(); ++it)
		{
			iPhysicsBody *pBody = *it;
		
			pBody->Destroy();
			hplDelete(pBody);
		}
		mlstBodies.clear();
		m_setUpdateBodies.clear();

		STLDeleteAll(mlstRopes);

		STLDeleteAll(mlstShapes);
		STLDeleteAll(mlstJoints);
		STLDeleteAll(mlstControllers);
		STLMapDeleteAll(m_mapMaterials);
	}

	//-----------------------------------------------------------------------

	
	void iPhysicsWorld::DestroyController(iPhysicsController *apController)
	{
		STLFindAndDelete(mlstControllers, apController);
	}

	//-----------------------------------------------------------------------

	void iPhysicsWorld::FadeoutAllLoopSounds(float afFadeSpeed)
	{
		////////////////////////////
		// Bodies
		tPhysicsBodyListIt bodyIt = mlstBodies.begin();
		for(; bodyIt != mlstBodies.end(); ++bodyIt)
		{
			iPhysicsBody *pBody = *bodyIt;

			cSoundEntity *pRollSound = pBody->GetRollSoundEntity();
			if(pRollSound && mpWorld->SoundEntityExists(pRollSound,pBody->GetRollSoundEntityID())) pRollSound->FadeOut(afFadeSpeed);
			pBody->SetRollSoundEntity(NULL);

			cSoundEntity *pScrapeSound = pBody->GetScrapeSoundEntity();
			if(pScrapeSound && mpWorld->SoundEntityExists(pScrapeSound, pBody->GetScrapeSoundEntityID())) pScrapeSound->FadeOut(afFadeSpeed);
			pBody->SetScrapeSoundEntity(NULL);
		}


		////////////////////////////
		// Joints
		tPhysicsJointListIt jointIt = mlstJoints.begin();
		for(; jointIt != mlstJoints.end(); ++jointIt)
		{
			iPhysicsJoint *pJoint = *jointIt;

			cSoundEntity *pLoopSound = pJoint->GetSound();
			if(pLoopSound && mpWorld->SoundEntityExists(pLoopSound, pJoint->GetSoundID())) pLoopSound->FadeOut(afFadeSpeed);
			pJoint->SetSound(NULL);
		}
	}

	//-----------------------------------------------------------------------


	void iPhysicsWorld::RenderContactPoints(iLowLevelGraphics *apLowLevel, const cColor& aPointColor,
											const cColor& aLineColor)
	{
		for(size_t i=0; i < mvContactPoints.size(); i++)
		{
			apLowLevel->DrawSphere(mvContactPoints[i].mvPoint,0.2f, aPointColor);
			apLowLevel->DrawLine(mvContactPoints[i].mvPoint, 
								mvContactPoints[i].mvNormal * mvContactPoints[i].mfDepth *0.2f,
								aLineColor);
			//Log("Rendering\n");
		}
	}
	
	//-----------------------------------------------------------------------
	
	bool iPhysicsWorld::CheckShapeWorldCollision(cVector3f *apPushVector,
							iCollideShape* apShape, const cMatrixf& a_mtxTransform,
							iPhysicsBody *apSkipBody, bool abSkipStatic, bool abIsCharacter,
							iPhysicsWorldCollisionCallback *apCallback,
							bool abCollideCharacter,
							int alMinPushStrength,
							tFlag alCollideFlags,
							bool abDebug)
	{
		cCollideData collideData;

		if(apPushVector) *apPushVector = cVector3f(0,0,0);
		bool bCollide = false;

		cBoundingVolume boundingVolume = apShape->GetBoundingVolume();
		boundingVolume.SetTransform(cMath::MatrixMul(a_mtxTransform, boundingVolume.GetTransform()));

		//Log("MAIN Position: %s Size: %s\n",boundingVolume.GetWorldCenter().ToString().c_str(),
		//	boundingVolume.GetSize().ToString().c_str());

		//if(abDebug)Log("--------------\n");

		int lBefore =0;
		int lAfter =0;
		
		mvTempBodies.resize(0);
		GetBodiesInBV(&boundingVolume, &mvTempBodies);
		
		for(size_t i=0; i<mvTempBodies.size(); ++i)
		{
			iPhysicsBody *pBody = mvTempBodies[i];
			
			if(pBody->IsActive()==false)continue;
			if(pBody->IsCharacter() && abCollideCharacter==false) continue;
			if(pBody == apSkipBody) continue;
			if(abSkipStatic && pBody->GetMass()==0 && pBody->IsCharacter()==false) continue;
			if(abIsCharacter && pBody->GetCollideCharacter()==false) continue;
			if(abIsCharacter==false && pBody->GetCollide()==false) continue;
			if(alMinPushStrength > pBody->GetPushStrength()) continue;
			if( (alCollideFlags & pBody->GetCollideFlags()) == 0)continue; 

			//Note: Still make this check, since GetBodiesInBV is not exact.
			if(cMath::CheckBVIntersection(boundingVolume,*pBody->GetBoundingVolume())==false)
			{
				continue;

			}
			
		   	collideData.SetMaxSize(32);
			bool bRet = CheckShapeCollision(apShape,a_mtxTransform, pBody->GetShape(),pBody->GetLocalMatrix(),
											collideData, 32, true);

			if(bRet && apPushVector)
			{
				//if(abDebug) Log(" Collided with '%s'\n",pBody->GetName().c_str());
					
				if(apCallback) apCallback->OnCollision(pBody, &collideData);

				for(int i=0; i< collideData.mlNumOfPoints; i++)
				{
					cCollidePoint &point = collideData.mvContactPoints[i];
				
					cVector3f vPush = point.mvNormal*point.mfDepth;

					if(std::abs(apPushVector->x) < std::abs(vPush.x)) apPushVector->x = vPush.x;
					if(std::abs(apPushVector->y) < std::abs(vPush.y)) apPushVector->y = vPush.y;
					if(std::abs(apPushVector->z) < std::abs(vPush.z)) apPushVector->z = vPush.z;
				}
			}
			if(bRet)bCollide = true;
		}
		//Log("--------------\n");
		
		return bCollide;
	}

	//-----------------------------------------------------------------------

}
