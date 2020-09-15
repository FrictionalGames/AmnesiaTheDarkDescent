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

#ifndef HPL_PHYSICS_WORLD_H
#define HPL_PHYSICS_WORLD_H

#include <map>
#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "physics/PhysicsTypes.h"

#include "physics/CollideData.h"

#include "engine/SaveGame.h"

namespace hpl {

	class iCollideShape;
	class iVertexBuffer;
	class iPhysicsBody;
	class iLowLevelGraphics;
	class iPhysicsMaterial;
	class iCharacterBody;
	class iPhysicsJoint;
	class iPhysicsJointBall;
	class iPhysicsJointHinge;
	class iPhysicsJointScrew;
	class iPhysicsJointSlider;
	class iPhysicsController;
	class iPhysicsRope;
	class cBinaryBuffer;

	class cWorld;
	class cBoundingVolume;
	
	typedef std::list<iCollideShape*> tCollideShapeList;
	typedef tCollideShapeList::iterator tCollideShapeListIt;

	typedef std::vector<iCollideShape*> tCollideShapeVec;
	typedef tCollideShapeVec::iterator tCollideShapeVecIt;

	typedef std::list<iPhysicsBody*> tPhysicsBodyList;
	typedef tPhysicsBodyList::iterator tPhysicsBodyListIt;

	typedef std::set<iPhysicsBody*> tPhysicsBodySet;
	typedef tPhysicsBodySet::iterator tPhysicsBodySetIt;

	typedef std::list<iPhysicsJoint*> tPhysicsJointList;
	typedef tPhysicsJointList::iterator tPhysicsJointListIt;

	typedef std::list<iPhysicsController*> tPhysicsControllerList;
	typedef tPhysicsControllerList::iterator tPhysicsControllerListIt;

	typedef std::list<iCharacterBody*> tCharacterBodyList;
	typedef tCharacterBodyList::iterator tCharacterBodyListIt;

	typedef std::list<iPhysicsRope*> tPhysicsRopeList;
	typedef tPhysicsRopeList::iterator tPhysicsRopeListIt;

	typedef std::map<tString, iPhysicsMaterial*> tPhysicsMaterialMap;
	typedef tPhysicsMaterialMap::iterator tPhysicsMaterialMapIt;
		
	typedef cSTLMapIterator<iPhysicsMaterial*, tPhysicsMaterialMap, tPhysicsMaterialMapIt> cPhysicsMaterialIterator;
	
	typedef cSTLIterator<iPhysicsBody*, tPhysicsBodyList, tPhysicsBodyListIt> cPhysicsBodyIterator;
	typedef cSTLIterator<iPhysicsJoint*, tPhysicsJointList, tPhysicsJointListIt> cPhysicsJointIterator;

	//----------------------------------------------------

	class iPhysicsWorld
	{
	public:
		iPhysicsWorld();
		virtual ~iPhysicsWorld();

		//########################################################################################
		//! \name General
		//########################################################################################
		//! @{
	
		void Update(float afTimeStep);
		virtual void Simulate(float afTimeStep)=0;

		virtual void  SetMaxTimeStep(float afTimeStep)=0;
		virtual float GetMaxTimeStep()=0;

		virtual void SetWorldSize(const cVector3f &avMin,const cVector3f &avMax)=0;
		virtual cVector3f GetWorldSizeMin()=0;
		virtual cVector3f GetWorldSizeMax()=0;
		
		virtual void SetGravity(const cVector3f& avGravity)=0;
		virtual cVector3f GetGravity()=0;

		virtual void SetAccuracyLevel(ePhysicsAccuracy aAccuracy)=0;
		virtual ePhysicsAccuracy GetAccuracyLevel()=0;

		virtual void SetNumberOfThreads(int alThreads)=0;
		virtual int GetNumberOfThreads()=0;

		//! @}

		//########################################################################################
		//! \name Shapes
		//########################################################################################
		//! @{

		virtual iCollideShape* CreateNullShape()=0;
		virtual iCollideShape* CreateBoxShape(const cVector3f &avSize, cMatrixf* apOffsetMtx)=0;
		virtual iCollideShape* CreateSphereShape(const cVector3f &avRadii, cMatrixf* apOffsetMtx)=0;
		virtual iCollideShape* CreateCylinderShape(float afRadius, float afHeight, cMatrixf* apOffsetMtx)=0;
		virtual iCollideShape* CreateCapsuleShape(float afRadius, float afHeight, cMatrixf* apOffsetMtx)=0;
		
		virtual iCollideShape* CreateMeshShape(iVertexBuffer *apVtxBuffer)=0;
		/**
		 * The buffer position must be pointing to where the data is saved!
		 */
		virtual iCollideShape* LoadMeshShapeFromBuffer(cBinaryBuffer *apBuffer)=0;
		/**
		 * The shape must be a mesh shape!
		 */
		virtual void SaveMeshShapeToBuffer(iCollideShape* apMeshShape, cBinaryBuffer *apBuffer)=0;
		
		virtual iCollideShape* CreateCompundShape(tCollideShapeVec &avShapes)=0;
		virtual iCollideShape* CreateStaticSceneShape(tCollideShapeVec &avShapes, tMatrixfVec *apMatrices)=0;
		void DestroyShape(iCollideShape *apShape);
		
		//! @}
		
		//########################################################################################
		//! \name Joints
		//########################################################################################
		//! @{

		virtual iPhysicsJointBall* CreateJointBall(const tString &asName,
											const cVector3f& avPivotPoint,const cVector3f& avPinDir,
											iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)=0;
		virtual iPhysicsJointHinge* CreateJointHinge(const tString &asName,
											const cVector3f& avPivotPoint,const cVector3f& avPinDir,
											iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)=0;
		virtual iPhysicsJointSlider* CreateJointSlider(const tString &asName,
											const cVector3f& avPivotPoint,const cVector3f& avPinDir,			
											iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)=0;
		virtual iPhysicsJointScrew* CreateJointScrew(const tString &asName,
											const cVector3f& avPivotPoint,const cVector3f& avPinDir,
											iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)=0;
		void DestroyJoint(iPhysicsJoint* apJoint);
		iPhysicsJoint *GetJoint(const tString &asName);
		bool JointExists(iPhysicsJoint* apJoint);
		cPhysicsJointIterator GetJointIterator();
		
		//! @}
		
		//########################################################################################
		//! \name Materials
		//########################################################################################
		//! @{

		virtual iPhysicsMaterial* CreateMaterial(const tString &asName)=0;
		iPhysicsMaterial* GetMaterialFromName(const tString &asName);
		cPhysicsMaterialIterator GetMaterialIterator();

		//! @}
		
		//########################################################################################
		//! \name Bodies
		//########################################################################################
		//! @{

		virtual iPhysicsBody* CreateBody(const tString &asName,iCollideShape *apShape)=0;
		void DestroyBody(iPhysicsBody* apBody);
		iPhysicsBody *GetBody(const tString &asName);
		cPhysicsBodyIterator GetBodyIterator();

		virtual iCharacterBody *CreateCharacterBody(const tString &asName, const cVector3f &avSize)=0;
		void DestroyCharacterBody(iCharacterBody* apBody);
		iPhysicsBody *GetCharacterBody(const tString &asName);

		virtual void GetBodiesInBV(cBoundingVolume *apBV, std::vector<iPhysicsBody*> *apBodyVec)=0;
		void EnableBodiesInBV(cBoundingVolume *apBV, bool abEnabled);

		void AddBodyToUpdateList(iPhysicsBody *apBody);
		void RemoveBodyFromUpdateList(iPhysicsBody *apBody, bool abDestroyingBody);


		//! @}

		//########################################################################################
		//! \name Misc
		//########################################################################################
		//! @{
		
		virtual iPhysicsRope* CreateRope(const tString &asName, const cVector3f &avStartPos, const cVector3f &avEndPos)=0;
		iPhysicsRope* GetRope(const tString &asName);
		iPhysicsRope* GetRopeFromUniqueID(int alID);
		void DestroyRope(iPhysicsRope* apRope);

		
		//! @}

		//########################################################################################
		//! \name Tools
		//########################################################################################
		//! @{

		void SetLogDebug(bool abX){ mbLogDebug = abX;}
		bool GetLogDebug(){ return mbLogDebug;}
		
		virtual iPhysicsController *CreateController(const tString &asName)=0;
		void DestroyController(iPhysicsController *apController);

		void FadeoutAllLoopSounds(float afFadeSpeed);

		tCollidePointVec* GetContactPoints(){ return &mvContactPoints;}
		void SetSaveContactPoints(bool abX){ mbSaveContactPoints = abX;}
		bool GetSaveContactPoints(){ return mbSaveContactPoints;}
		void RenderContactPoints(iLowLevelGraphics *apLowLevel, const cColor& aPointColor, const cColor& aLineColor);

		virtual void CastRay(iPhysicsRayCallback *apCallback, 
							const cVector3f &avOrigin, const cVector3f& avEnd, 
							bool abCalcDist, bool abCalcNormal, bool abCalcPoint,
							bool abUsePrefilter=false)=0;

		virtual void RenderShapeDebugGeometry(	iCollideShape *apShape, const cMatrixf& a_mtxTransform, 
												iLowLevelGraphics *apLowLevel, const cColor& aColor)=0;
		
		virtual void RenderDebugGeometry(iLowLevelGraphics *apLowLevel, const cColor& aColor)=0;

		virtual bool CheckShapeCollision(	iCollideShape* apShapeA, const cMatrixf& a_mtxA,
										iCollideShape* apShapeB, const cMatrixf& a_mtxB,
										cCollideData & aCollideData, int alMaxPoints,
										bool abCorrectNormalDirection)=0;

		bool CheckShapeWorldCollision(	cVector3f *apPushVector,
										iCollideShape* apShape, const cMatrixf& a_mtxTransform,
										iPhysicsBody *apSkipBody=NULL, bool abSkipStatic=false,
										bool abIsCharacter=false, 
										iPhysicsWorldCollisionCallback *apCallback=NULL,
										bool abCollideCharacter=true,
										int alMinPushStrength=0,
										tFlag alCollideFlags = eFlagBit_All, 
										bool abDebug=false);
		
		void DestroyAll();

		cWorld* GetWorld(){ return mpWorld;}
		void SetWorld(cWorld *apWorld){ mpWorld = apWorld;}
		//! @}

	protected:
		tCollideShapeList mlstShapes;
		tPhysicsBodyList mlstBodies;
		tPhysicsBodySet m_setUpdateBodies;
		tCharacterBodyList mlstCharBodies;
		tPhysicsMaterialMap m_mapMaterials;
		tPhysicsJointList mlstJoints;
		tPhysicsControllerList mlstControllers;
		tPhysicsRopeList mlstRopes;
		cWorld *mpWorld;

		std::vector<iPhysicsBody*> mvTempBodies;

		bool mbLogDebug;

		tCollidePointVec mvContactPoints;
		bool mbSaveContactPoints;
	};
};
#endif // HPL_PHYSICS_WORLD_H

