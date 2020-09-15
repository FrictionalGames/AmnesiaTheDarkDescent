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

#include "impl/PhysicsWorldNewton.h"

#include "impl/CollideShapeNewton.h"
#include "impl/PhysicsBodyNewton.h"
#include "impl/PhysicsMaterialNewton.h"
#include "impl/CharacterBodyNewton.h"
#include "impl/PhysicsRopeNewton.h"

#include "impl/PhysicsJointBallNewton.h"
#include "impl/PhysicsJointHingeNewton.h"
#include "impl/PhysicsJointScrewNewton.h"
#include "impl/PhysicsJointSliderNewton.h"

#include "impl/PhysicsControllerNewton.h"

#include "scene/World.h"

#include "system/LowLevelSystem.h"
#include "graphics/VertexBuffer.h"
#include "graphics/LowLevelGraphics.h"
#include "math/Math.h"
#include "resources/BinaryBuffer.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cPhysicsWorldNewton::cPhysicsWorldNewton()
		: iPhysicsWorld()
	{
		//mpNewtonWorld = NewtonCreate();
		mpNewtonWorld = NewtonCreate(NULL, NULL);

		if(mpNewtonWorld==NULL){
			Warning("Couldn't create newton world!\n");
		}
		
		/////////////////////////////////
        //Set default values to properties
		mvWorldSizeMin = cVector3f(0,0,0);
		mvWorldSizeMax = cVector3f(0,0,0);

		mvGravity = cVector3f(0,-9.81f,0);
		mfMaxTimeStep = 1.0f/60.0f;
		
		/////////////////////////////////
		//Create default material.
		int lDefaultMatId = 0;//NewtonMaterialGetDefaultGroupID(mpNewtonWorld);
		cPhysicsMaterialNewton *pMaterial = hplNew( cPhysicsMaterialNewton, ("Default",this,lDefaultMatId) );
		tPhysicsMaterialMap::value_type Val("Default",pMaterial);
		m_mapMaterials.insert(Val);
		pMaterial->UpdateMaterials();

		mpTempDepths = hplNewArray( float,500);
		mpTempNormals = hplNewArray( float,500 * 3);
		mpTempPoints = hplNewArray( float,500 * 3);
	}

	//-----------------------------------------------------------------------

	cPhysicsWorldNewton::~cPhysicsWorldNewton()
	{
		DestroyAll();
		NewtonDestroy(mpNewtonWorld);

		hplDeleteArray(mpTempDepths);
		hplDeleteArray(mpTempNormals);
		hplDeleteArray(mpTempPoints);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cPhysicsWorldNewton::Simulate(float afTimeStep)
	{
		//cPhysicsBodyNewton::SetUseCallback(false);
		//static lUpdate =0;
        //if(lUpdate % 30==0)
		{

			while(afTimeStep>mfMaxTimeStep)
			{
				NewtonUpdate(mpNewtonWorld, mfMaxTimeStep);
				afTimeStep -= mfMaxTimeStep;
			}
			NewtonUpdate(mpNewtonWorld, afTimeStep);
		}
		//lUpdate++;
		//cPhysicsBodyNewton::SetUseCallback(true);
        			
		tPhysicsBodyListIt it = mlstBodies.begin();
		for(;it != mlstBodies.end(); ++it)
		{
			cPhysicsBodyNewton* pBody = static_cast<cPhysicsBodyNewton*>(*it);
			pBody->ClearForces();
		}
	}
	
	//-----------------------------------------------------------------------

	void cPhysicsWorldNewton::SetMaxTimeStep(float afTimeStep)
	{
		mfMaxTimeStep = afTimeStep;
	}
	
	float cPhysicsWorldNewton::GetMaxTimeStep()
	{
		return mfMaxTimeStep;
	}
	
	//-----------------------------------------------------------------------

	void cPhysicsWorldNewton::SetWorldSize(const cVector3f &avMin,const cVector3f &avMax)
	{
		mvWorldSizeMin = avMin;
		mvWorldSizeMax = avMax;

		NewtonSetWorldSize(mpNewtonWorld,avMin.v, avMax.v);
	}

	cVector3f cPhysicsWorldNewton::GetWorldSizeMin()
	{
		return mvWorldSizeMin;
	}

	cVector3f cPhysicsWorldNewton::GetWorldSizeMax()
	{
		return mvWorldSizeMax;
	}

	
	//-----------------------------------------------------------------------

	void cPhysicsWorldNewton::SetGravity(const cVector3f& avGravity)
	{
		mvGravity = avGravity;
	}
	
	//-----------------------------------------------------------------------

	cVector3f cPhysicsWorldNewton::GetGravity()
	{
		return mvGravity;
	}

	//-----------------------------------------------------------------------

	void cPhysicsWorldNewton::SetAccuracyLevel(ePhysicsAccuracy aAccuracy)
	{
		mAccuracy = aAccuracy;
		
		switch(mAccuracy)
		{
		case ePhysicsAccuracy_Low:
									NewtonSetSolverModel(mpNewtonWorld,1);
									NewtonSetFrictionModel(mpNewtonWorld,1);
									break;
		case ePhysicsAccuracy_Medium:
									NewtonSetSolverModel(mpNewtonWorld,2);
									NewtonSetFrictionModel(mpNewtonWorld,1);
									break;
		case ePhysicsAccuracy_High:
									NewtonSetSolverModel(mpNewtonWorld,0);
									NewtonSetFrictionModel(mpNewtonWorld,0);
									break;
		}
	}

	//-----------------------------------------------------------------------

	ePhysicsAccuracy cPhysicsWorldNewton::GetAccuracyLevel()
	{
		return mAccuracy;
	}

	//-----------------------------------------------------------------------

	void cPhysicsWorldNewton::SetNumberOfThreads(int alThreads)
	{
		NewtonSetThreadsCount(mpNewtonWorld, alThreads);
	}
	
	int cPhysicsWorldNewton::GetNumberOfThreads()
	{
		return NewtonGetThreadsCount(mpNewtonWorld);
	}
	
	//-----------------------------------------------------------------------
	
	iCollideShape* cPhysicsWorldNewton::CreateNullShape()
	{
		iCollideShape *pShape = hplNew( cCollideShapeNewton, (eCollideShapeType_Null, 0, NULL, 
													mpNewtonWorld, this) );
		mlstShapes.push_back(pShape);

		return pShape;
	}

	//-----------------------------------------------------------------------

	iCollideShape* cPhysicsWorldNewton::CreateBoxShape(const cVector3f &avSize, cMatrixf* apOffsetMtx)
	{
		iCollideShape *pShape = hplNew( cCollideShapeNewton, (eCollideShapeType_Box, avSize, apOffsetMtx, 
													mpNewtonWorld, this) );
		mlstShapes.push_back(pShape);
		
		return pShape;
	}

	//-----------------------------------------------------------------------

	iCollideShape* cPhysicsWorldNewton::CreateSphereShape(const cVector3f &avRadii, cMatrixf* apOffsetMtx)
	{
		iCollideShape *pShape = hplNew( cCollideShapeNewton, (eCollideShapeType_Sphere, avRadii, apOffsetMtx, 
														mpNewtonWorld, this) ); 
		mlstShapes.push_back(pShape);

		return pShape;
	}

	//-----------------------------------------------------------------------

	iCollideShape* cPhysicsWorldNewton::CreateCylinderShape(float afRadius, float afHeight, cMatrixf* apOffsetMtx)
	{
		iCollideShape *pShape = hplNew( cCollideShapeNewton, (eCollideShapeType_Cylinder, 
													cVector3f(afRadius,afHeight,afRadius), 
													apOffsetMtx, 
													mpNewtonWorld, this) );
		mlstShapes.push_back(pShape);

		return pShape;
	}

	//-----------------------------------------------------------------------

	iCollideShape* cPhysicsWorldNewton::CreateCapsuleShape(float afRadius, float afHeight, cMatrixf* apOffsetMtx)
	{
		iCollideShape *pShape = hplNew( cCollideShapeNewton, (eCollideShapeType_Capsule, 
													cVector3f(afRadius,afHeight,afRadius), 
													apOffsetMtx, 
													mpNewtonWorld, this) );
		mlstShapes.push_back(pShape);

		return pShape;
	}
	
	//-----------------------------------------------------------------------

	iCollideShape* cPhysicsWorldNewton::CreateMeshShape(iVertexBuffer *apVtxBuffer)
	{
		cCollideShapeNewton *pShape = hplNew( cCollideShapeNewton, (eCollideShapeType_Mesh, 0, NULL, mpNewtonWorld,this) );
		
		pShape->CreateFromVertices(apVtxBuffer->GetIndices(),apVtxBuffer->GetIndexNum(),
									apVtxBuffer->GetFloatArray(eVertexBufferElement_Position),
									apVtxBuffer->GetElementNum(eVertexBufferElement_Position),
									apVtxBuffer->GetVertexNum());
		mlstShapes.push_back(pShape);

		return pShape;
	}

	//-----------------------------------------------------------------------

	iCollideShape* cPhysicsWorldNewton::LoadMeshShapeFromBuffer(cBinaryBuffer *apBuffer)
	{
		cCollideShapeNewton *pShape = hplNew( cCollideShapeNewton, (eCollideShapeType_Mesh,0, NULL, mpNewtonWorld,this) );
		
		pShape->CreateFromSerializedData(apBuffer);
		
		mlstShapes.push_back(pShape);

		return pShape;
	}
	
	//-----------------------------------------------------------------------

	
	void cPhysicsWorldNewton::SaveMeshShapeToBuffer(iCollideShape* apMeshShape, cBinaryBuffer *apBuffer)
	{
		cCollideShapeNewton *pNewtonMeshShape = static_cast<cCollideShapeNewton*>(apMeshShape);
		pNewtonMeshShape->SaveToSerializedData(apBuffer);
	}
	
	//-----------------------------------------------------------------------
	
	iCollideShape* cPhysicsWorldNewton::CreateCompundShape(tCollideShapeVec &avShapes)
	{
		cCollideShapeNewton *pShape = hplNew( cCollideShapeNewton, (eCollideShapeType_Compound,0, NULL, mpNewtonWorld,this) );
		pShape->CreateCompoundFromShapeVec(avShapes);
		mlstShapes.push_back(pShape);

		return pShape;
	}

	//-----------------------------------------------------------------------

	iCollideShape* cPhysicsWorldNewton::CreateStaticSceneShape(tCollideShapeVec &avShapes, tMatrixfVec *apMatrices)
	{
		cCollideShapeNewton *pShape = hplNew(	cCollideShapeNewton, (eCollideShapeType_StaticScene,0, NULL, mpNewtonWorld,this) );

        pShape->CreateStaticSceneFromShapeVec(avShapes, apMatrices);
		mlstShapes.push_back(pShape);
		return pShape;
	}

	//-----------------------------------------------------------------------

	iPhysicsJointBall* cPhysicsWorldNewton::CreateJointBall(const tString &asName, 
										const cVector3f& avPivotPoint,const cVector3f& avPinDir,
										iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)
	{
		iPhysicsJointBall *pJoint = hplNew( cPhysicsJointBallNewton, (asName,apParentBody,apChildBody,this,
														avPivotPoint,avPinDir) );
		mlstJoints.push_back(pJoint);
		return pJoint;
	}

	iPhysicsJointHinge* cPhysicsWorldNewton::CreateJointHinge(const tString &asName,
										const cVector3f& avPivotPoint,const cVector3f& avPinDir,
										iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)
	{
		iPhysicsJointHinge *pJoint = hplNew( cPhysicsJointHingeNewton, (asName,apParentBody,apChildBody,this,
										avPivotPoint,avPinDir) );
		mlstJoints.push_back(pJoint);
		return pJoint;
	}

	iPhysicsJointSlider* cPhysicsWorldNewton::CreateJointSlider(const tString &asName,
										const cVector3f& avPivotPoint,const cVector3f& avPinDir,			
										iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)
	{
		iPhysicsJointSlider *pJoint = hplNew( cPhysicsJointSliderNewton, (asName,apParentBody,apChildBody,this,
											avPivotPoint,avPinDir) );
		mlstJoints.push_back(pJoint);
		return pJoint;
	}

	iPhysicsJointScrew* cPhysicsWorldNewton::CreateJointScrew(const tString &asName,
										const cVector3f& avPivotPoint,const cVector3f& avPinDir,
										iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)
	{
		iPhysicsJointScrew *pJoint = hplNew( cPhysicsJointScrewNewton, (asName,apParentBody,apChildBody,this,
											avPivotPoint,avPinDir) );
		mlstJoints.push_back(pJoint);
		return pJoint;
	}

	//-----------------------------------------------------------------------

	iPhysicsBody* cPhysicsWorldNewton::CreateBody(const tString &asName,iCollideShape *apShape)
	{
		cPhysicsBodyNewton *pBody = hplNew( cPhysicsBodyNewton, (asName,this, apShape) );

		mlstBodies.push_back(pBody);

		return pBody;
	}

	//-----------------------------------------------------------------------

	static std::vector<iPhysicsBody*> *gpBodyVec;
	static void AddNewtonBodyToVector(const NewtonBody* apNewtonBody)//, void* userData)
	{
		cPhysicsBodyNewton* pBody = (cPhysicsBodyNewton*) NewtonBodyGetUserData(apNewtonBody);
		gpBodyVec->push_back(pBody);
	}

	void cPhysicsWorldNewton::GetBodiesInBV(cBoundingVolume *apBV, std::vector<iPhysicsBody*> *apBodyVec)
	{
		gpBodyVec = apBodyVec;

		//NewtonWorldForEachBodyInAABBDo(mpNewtonWorld,apBV->GetMin().v, apBV->GetMax().v,AddNewtonBodyToVector, NULL);
		NewtonWorldForEachBodyInAABBDo(mpNewtonWorld,apBV->GetMin().v, apBV->GetMax().v,AddNewtonBodyToVector);
	}
	
	//-----------------------------------------------------------------------

	iCharacterBody* cPhysicsWorldNewton::CreateCharacterBody(const tString &asName, const cVector3f &avSize)
	{
		cCharacterBodyNewton *pChar = hplNew( cCharacterBodyNewton, (asName,this,avSize) );

		mlstCharBodies.push_back(pChar);

		return pChar;
	}

	//-----------------------------------------------------------------------

	iPhysicsMaterial* cPhysicsWorldNewton::CreateMaterial(const tString &asName)
	{
		cPhysicsMaterialNewton *pMaterial = hplNew( cPhysicsMaterialNewton, (asName,this) );
		
		tPhysicsMaterialMap::value_type Val(asName,pMaterial);
        m_mapMaterials.insert(Val);
		
		pMaterial->UpdateMaterials();

		return pMaterial;
	}

	//-----------------------------------------------------------------------

	iPhysicsController* cPhysicsWorldNewton::CreateController(const tString &asName)
	{
		iPhysicsController* pController = hplNew( cPhysicsControllerNewton, (asName, this) );	

		mlstControllers.push_back(pController);

		return pController;
	}

	//-----------------------------------------------------------------------

	iPhysicsRope* cPhysicsWorldNewton::CreateRope(const tString &asName, const cVector3f &avStartPos, const cVector3f &avEndPos)
	{
		iPhysicsRope *pRope = hplNew( cPhysicsRopeNewton, (asName, this,avStartPos, avEndPos));
		
		mlstRopes.push_back(pRope);

		return pRope;
	}

	//-----------------------------------------------------------------------

	static bool gbRayCalcDist;
	static bool gbRayCalcNormal;
	static bool gbRayCalcPoint;
	static iPhysicsRayCallback *gpRayCallback;
	static cVector3f gvRayOrigin;
	static cVector3f gvRayEnd;
	static cVector3f gvRayDelta;
	static float gfRayLength;
	//Temp:
	static cVector3f gvRayBoxMin; 
	static cVector3f gvRayBoxMax;
	
	static cPhysicsRayParams gRayParams;

	//////////////////////////////////////
	
	static unsigned RayCastPrefilterFunc (const NewtonBody* apNewtonBody,const NewtonCollision* collision, void* userData)
	{
		cPhysicsBodyNewton* pRigidBody = (cPhysicsBodyNewton*) NewtonBodyGetUserData(apNewtonBody);
		if(pRigidBody->IsActive()==false) return 0;

		//Temp:
		cBoundingVolume *pBv = pRigidBody->GetBoundingVolume();
		if(cMath::CheckAABBIntersection(gvRayBoxMin, gvRayBoxMax, pBv->GetMin(), pBv->GetMax())==false)
		{
			return 0;
		}

		bool bRet = gpRayCallback->BeforeIntersect(pRigidBody);

		if(bRet) return 1;
		else return 0;
	}

	static float RayCastFilterFunc (const NewtonBody* apNewtonBody, const float* apNormalVec, 
								int alCollisionID, void* apUserData, float afIntersetParam)
	{
		cPhysicsBodyNewton* pRigidBody = (cPhysicsBodyNewton*) NewtonBodyGetUserData(apNewtonBody);
		if(pRigidBody->IsActive()==false) return 1;

		gRayParams.mfT = afIntersetParam;
		
		//Calculate stuff needed.
		if(gbRayCalcDist){
			gRayParams.mfDist = gfRayLength * afIntersetParam;
		}
		if(gbRayCalcNormal){
			gRayParams.mvNormal.FromVec(apNormalVec);
		}
		if(gbRayCalcPoint){
			gRayParams.mvPoint = gvRayOrigin + gvRayDelta * afIntersetParam;
		}
		
		//Call the call back
		bool bRet = gpRayCallback->OnIntersect(pRigidBody,&gRayParams);
		
		//return correct value.
		if(bRet) return 1;//afIntersetParam;
		else return 0;
	}

	//////////////////////////////////////

	void cPhysicsWorldNewton::CastRay(iPhysicsRayCallback *apCallback, 
								const cVector3f &avOrigin, const cVector3f& avEnd, 
								bool abCalcDist, bool abCalcNormal,bool abCalcPoint,
								bool abUsePrefilter)
	{
		gbRayCalcPoint = abCalcPoint;
		gbRayCalcNormal = abCalcNormal;
		gbRayCalcDist = abCalcDist;

		gvRayOrigin = avOrigin;
		gvRayEnd = avEnd;

		gvRayDelta = avEnd - avOrigin;
		gfRayLength = gvRayDelta.Length();

        gpRayCallback = apCallback;

		////////////
		//Temp:
		for(int i=0; i<3; ++i)
		{
			if(gvRayOrigin.v[i] > gvRayEnd.v[i]){
				gvRayBoxMin.v[i] = gvRayEnd.v[i];
				gvRayBoxMax.v[i] = gvRayOrigin.v[i];
			}
			else {
				gvRayBoxMin.v[i] = gvRayOrigin.v[i];
				gvRayBoxMax.v[i] = gvRayEnd.v[i];
			}
		}

		
		if(abUsePrefilter)
			NewtonWorldRayCast(mpNewtonWorld, avOrigin.v, avEnd.v,RayCastFilterFunc, NULL, RayCastPrefilterFunc);
		else
			NewtonWorldRayCast(mpNewtonWorld, avOrigin.v, avEnd.v,RayCastFilterFunc, NULL, NULL);
	}
	
	//-----------------------------------------------------------------------

	static inline void CorrectNormalDirection(cVector3f& avNormal, const cVector3f& avCollidePoint, const cVector3f& avShapeACenter)
	{
		cVector3f vCenterToCollidePoint = avCollidePoint - avShapeACenter;
		//A check if the normal points in the wrong direction.
		if(cMath::Vector3Dot(vCenterToCollidePoint,avNormal)>0)
		{
			avNormal = avNormal *-1;	
		}
	}

	bool cPhysicsWorldNewton::CheckShapeCollision(	iCollideShape* apShapeA, const cMatrixf& a_mtxA,
										iCollideShape* apShapeB, const cMatrixf& a_mtxB,
										cCollideData & aCollideData, int alMaxPoints,
										bool abCorrectNormalDirection)
	{
		cCollideShapeNewton *pNewtonShapeA = static_cast<cCollideShapeNewton*>(apShapeA);
		cCollideShapeNewton *pNewtonShapeB = static_cast<cCollideShapeNewton*>(apShapeB);

		cMatrixf mtxTransposeA = a_mtxA.GetTranspose();
		cMatrixf mtxTransposeB = a_mtxB.GetTranspose();
		
		bool bLog = false;
		//if(alMaxPoints== 1) bLog = true;
		
		//////////////////////////////
		//Check compound collision
        if(pNewtonShapeA->GetType() == eCollideShapeType_Compound || 
			pNewtonShapeB->GetType() == eCollideShapeType_Compound)
		{
			int lACount = pNewtonShapeA->GetSubShapeNum();
			int lBCount = pNewtonShapeB->GetSubShapeNum();

			bool bCollision = false;
			aCollideData.mlNumOfPoints = 0;
			int lCollideDataStart =0;

			for(int a=0; a< lACount; a++)
			{
				for(int b=0; b< lBCount; b++)
				{
					cCollideShapeNewton *pSubShapeA = static_cast<cCollideShapeNewton*>(pNewtonShapeA->GetSubShape(a));
					cCollideShapeNewton *pSubShapeB = static_cast<cCollideShapeNewton*>(pNewtonShapeB->GetSubShape(b));
					
					int lNum = NewtonCollisionCollide(mpNewtonWorld, alMaxPoints,
												pSubShapeA->GetNewtonCollision(), &(mtxTransposeA.m[0][0]),
												pSubShapeB->GetNewtonCollision(), &(mtxTransposeB.m[0][0]),
												mpTempPoints, mpTempNormals, mpTempDepths, 0);
					if(lNum<1) continue;
					if(lNum > alMaxPoints )lNum = alMaxPoints;

					bCollision = true;

					//Log("Collided %d vs %d. Num: %d\n",a,b,lNum);
					
					//Negate for each iteration.
					alMaxPoints -= lNum;

					for(int i=0; i<lNum; i++)
					{
						cCollidePoint &CollPoint = aCollideData.mvContactPoints[lCollideDataStart + i];
						CollPoint.mfDepth =  mpTempDepths[i];

						int lVertex = i*3;

						CollPoint.mvNormal.x = mpTempNormals[lVertex+0];
						CollPoint.mvNormal.y = mpTempNormals[lVertex+1];
						CollPoint.mvNormal.z = mpTempNormals[lVertex+2];

						CollPoint.mvPoint.x = mpTempPoints[lVertex+0];
						CollPoint.mvPoint.y = mpTempPoints[lVertex+1];
						CollPoint.mvPoint.z = mpTempPoints[lVertex+2];
						
						/////////
						//Correct the normal
						if(abCorrectNormalDirection && apShapeA->GetType() != eCollideShapeType_Mesh)
							CorrectNormalDirection(CollPoint.mvNormal,CollPoint.mvPoint, a_mtxA.GetTranslation());						
					}
	                
					lCollideDataStart += lNum;
					aCollideData.mlNumOfPoints += lNum;

					if(alMaxPoints <= 0) break;
				}
				if(alMaxPoints <= 0) break;
			}
			
			return bCollision;
		}
		//////////////////////////////
		//Check NON compound collision
		else 
		{
			//Log(" 1\n");
			int lNum = NewtonCollisionCollide(mpNewtonWorld, alMaxPoints,
										pNewtonShapeA->GetNewtonCollision(), &(mtxTransposeA.m[0][0]),
										pNewtonShapeB->GetNewtonCollision(), &(mtxTransposeB.m[0][0]),
										mpTempPoints, mpTempNormals, mpTempDepths, 0);
			
			if(lNum<1) return false;
			if(lNum > alMaxPoints )lNum = alMaxPoints;
			
			//Log(" 2\n");
			for(int i=0; i<lNum; i++)
			{
				cCollidePoint &CollPoint = aCollideData.mvContactPoints[i];
				CollPoint.mfDepth =  mpTempDepths[i];
				
				int lVertex = i*3;

				CollPoint.mvNormal.x = mpTempNormals[lVertex+0];
				CollPoint.mvNormal.y = mpTempNormals[lVertex+1];
				CollPoint.mvNormal.z = mpTempNormals[lVertex+2];

				CollPoint.mvPoint.x = mpTempPoints[lVertex+0];
				CollPoint.mvPoint.y = mpTempPoints[lVertex+1];
				CollPoint.mvPoint.z = mpTempPoints[lVertex+2];

				/////////
				//Correct the normal
				if(abCorrectNormalDirection && apShapeA->GetType() != eCollideShapeType_Mesh)
					CorrectNormalDirection(CollPoint.mvNormal,CollPoint.mvPoint, a_mtxA.GetTranslation());
				
			}
			
			aCollideData.mlNumOfPoints = lNum;
		}
		
		return true;
	}

	//-----------------------------------------------------------------------

	static iLowLevelGraphics *gpLowLevelGraphics;
	static cColor gDebugColor;

	static void RenderDebugPolygon(void* apUserData, int alVertexCount, const dFloat* apFaceArray, int alFaceId)
	{
		int i;

		i = alVertexCount - 1;
		cVector3f vP0(apFaceArray[i * 3 + 0], apFaceArray[i * 3 + 1], apFaceArray[i * 3 + 2]);
		for (i = 0; i < alVertexCount; ++i)
		{
			cVector3f vP1 (apFaceArray[i * 3 + 0], apFaceArray[i * 3 + 1], apFaceArray[i * 3 + 2]);

			gpLowLevelGraphics->DrawLine(vP0, vP1,gDebugColor);

			vP0 = vP1;
		}

	}

	//-----------------------------------------------------------------------

	void cPhysicsWorldNewton::RenderShapeDebugGeometry(	iCollideShape *apShape, const cMatrixf& a_mtxTransform, 
														iLowLevelGraphics *apLowLevel, const cColor& aColor)
	{
		gpLowLevelGraphics = apLowLevel;
		gDebugColor = aColor;

		cCollideShapeNewton *pNewtonShape = static_cast<cCollideShapeNewton*>(apShape);
		NewtonCollisionForEachPolygonDo (	pNewtonShape->GetNewtonCollision(), 
											&(a_mtxTransform.GetTranspose().m[0][0]), 
											RenderDebugPolygon,
											NULL);
	}
	
	//-----------------------------------------------------------------------

	void cPhysicsWorldNewton::RenderDebugGeometry(iLowLevelGraphics *apLowLevel,const cColor &aColor)
	{
		tPhysicsBodyListIt it = mlstBodies.begin();
		for(;it != mlstBodies.end(); ++it)
		{
			iPhysicsBody *pBody = *it;
			pBody->RenderDebugGeometry(apLowLevel,aColor);
		}
	}

	//-----------------------------------------------------------------------
}
