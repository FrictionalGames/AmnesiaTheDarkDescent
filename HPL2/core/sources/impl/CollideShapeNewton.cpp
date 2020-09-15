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

#include "impl/CollideShapeNewton.h"

#include "physics/PhysicsWorld.h"
#include "system/LowLevelSystem.h"
#include "system/Platform.h"
#include "resources/BinaryBuffer.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cCollideShapeNewton::cCollideShapeNewton(eCollideShapeType aType, const cVector3f &avSize, 
											cMatrixf* apOffsetMtx, NewtonWorld* apNewtonWorld,
											iPhysicsWorld *apWorld)
	: iCollideShape(apWorld)
	{
		mpNewtonCollision = NULL;
		mpNewtonWorld = apNewtonWorld;
		mvSize = avSize;
		mType = aType;

		mfVolume = 0;

		float *pMtx = NULL;
		cMatrixf mtxTranspose;
		if(apOffsetMtx)
		{
			m_mtxOffset = *apOffsetMtx;
			mtxTranspose = m_mtxOffset.GetTranspose();

			pMtx = &(mtxTranspose.m[0][0]);
		}
		else
			m_mtxOffset = cMatrixf::Identity;

		////////////////////////////////////////////
		// Create Newton collision
		
		switch(aType)
		{
		case eCollideShapeType_Null:		mpNewtonCollision = NewtonCreateNull(apNewtonWorld); break;
		
		case eCollideShapeType_Box:			mpNewtonCollision = NewtonCreateBox(apNewtonWorld,
												mvSize.x, mvSize.y, mvSize.z, 
												0, pMtx); break;
		
		case eCollideShapeType_Sphere:		mpNewtonCollision = NewtonCreateSphere(apNewtonWorld,
												//mvSize.x, mvSize.y, mvSize.z, if not all values are equal then this does not work
												mvSize.x, mvSize.x,mvSize.x,	//so this is better!
												0, pMtx); break;

		case eCollideShapeType_Cylinder:	mpNewtonCollision = NewtonCreateCylinder(apNewtonWorld,
												mvSize.x, mvSize.y, 
												0, pMtx); break;
		
		case eCollideShapeType_Capsule:		mpNewtonCollision = NewtonCreateCapsule(apNewtonWorld,
												mvSize.x, mvSize.y, 
												0, pMtx); break;
		}
		
		////////////////////////////////////////////
		// Calculate Bounding volume and volume.
		if(mType == eCollideShapeType_Box)
		{
			mBoundingVolume.SetSize(mvSize);

			mfVolume = mvSize.x * mvSize.y *mvSize.z;
		}
		else if(mType == eCollideShapeType_Sphere)
		{
			mBoundingVolume.SetSize(mvSize*2);

			mfVolume = (4.0f / 3.0f) * kPif * (mvSize.x*mvSize.x*mvSize.x);
		}
		else if(mType == eCollideShapeType_Cylinder || 
				mType == eCollideShapeType_Capsule)
		{
			mBoundingVolume.SetSize(cVector3f(mvSize.y,mvSize.x*2,mvSize.x*2));
			
			//Not gonna be correct for capsule...
			if(mType == eCollideShapeType_Cylinder)
				mfVolume = kPif * (mvSize.x*mvSize.x)*mvSize.y;
			else
			{
				//Height of the cylinder part.
                float fCylHeight = mvSize.y - (mvSize.x*2);
				mfVolume =0;
				
				//The volume of the cylinder part.
				if(fCylHeight>0)
					mfVolume += kPif * (mvSize.x*mvSize.x)*fCylHeight;
				
				//The volume of the sphere part.
				mfVolume += (4.0f / 3.0f) * kPif * (mvSize.x*mvSize.x*mvSize.x);
			}
		}

		mBoundingVolume.SetTransform(m_mtxOffset);
	}

	//-----------------------------------------------------------------------

	cCollideShapeNewton::~cCollideShapeNewton()
	{
		//Release Newton Collision
		if(mpNewtonCollision)
			NewtonReleaseCollision(mpNewtonWorld,mpNewtonCollision);
		
		//Release all subshapes (for compound objects)
		for(int i=0; i < (int)mvSubShapes.size(); i++)
		{
			mpWorld->DestroyShape(mvSubShapes[i]);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iCollideShape* cCollideShapeNewton::GetSubShape(int alIdx)
	{
		if(mType == eCollideShapeType_Compound)
			return mvSubShapes[alIdx];
		else
			return this;
	}

	int cCollideShapeNewton::GetSubShapeNum()
	{
		if(mType == eCollideShapeType_Compound)
			return (int)mvSubShapes.size();
		else
			return 1;
	}
	
	//-----------------------------------------------------------------------

	cVector3f cCollideShapeNewton::GetInertia(float afMass)
	{
		cVector3f vInertia(1,1,1);
		
		// Box
		if(mType == eCollideShapeType_Box)
		{
			float fInv = 1.0f / 12.0f;
			vInertia = cVector3f(
						afMass * (mvSize.y * mvSize.y + mvSize.z * mvSize.z) * fInv,
						afMass * (mvSize.x * mvSize.x + mvSize.z * mvSize.z) * fInv,
						afMass * (mvSize.x * mvSize.x + mvSize.y * mvSize.y) * fInv
						);
		}
		// Sphere
		else if(mType == eCollideShapeType_Sphere)
		{
			float fI = 0.4f * afMass * mvSize.x * mvSize.x;
			vInertia = cVector3f(fI, fI,fI);
		}
		//Cylinder
		else if(mType == eCollideShapeType_Cylinder)
		{
			float fRadius = mvSize.x;
			vInertia.x = afMass * (fRadius*fRadius) * (1.0f/4.0f) + afMass * (mvSize.y*mvSize.y) * (1.0f / 12.0f);
			vInertia.y = vInertia.x;
			vInertia.z = afMass * (fRadius*fRadius) * (1.0f/2.0f);
		}
		//Capsule
		else if(mType == eCollideShapeType_Capsule)
		{
			float fRadius = mvSize.x;
			vInertia.x = afMass * (fRadius*fRadius) * (1.0f/4.0f) + afMass * (mvSize.y*mvSize.y) * (1.0f / 12.0f);
			vInertia.y = vInertia.x;
			vInertia.z = afMass * (fRadius*fRadius) * (1.0f/2.0f);
		}
		//Compound
		//This is only a very bad approximation.
		else if(mType == eCollideShapeType_Compound)
		{
			cVector3f vBoxSize = mBoundingVolume.GetSize();
			float fBoxVolume = vBoxSize.x * vBoxSize.y * vBoxSize.z;

			float fInv = 1.0f / 12.0f;
			vInertia = cVector3f(
				afMass * (vBoxSize.y * vBoxSize.y + vBoxSize.z * vBoxSize.z) * fInv,
				afMass * (vBoxSize.x * vBoxSize.x + vBoxSize.z * vBoxSize.z) * fInv,
				afMass * (vBoxSize.x * vBoxSize.x + vBoxSize.y * vBoxSize.y) * fInv
				);
			//Scale of a bit of the inertia since the compound is not a 100% solid box
			vInertia = vInertia * (1.0f - ((fBoxVolume / mfVolume)*0.3f));
		}

		return vInertia;
	}

	//-----------------------------------------------------------------------

	void cCollideShapeNewton::CreateStaticSceneFromShapeVec(tCollideShapeVec &avShapes, tMatrixfVec *apMatrices)
	{
		mpNewtonCollision = NewtonCreateSceneCollision(mpNewtonWorld, 0);

		mvSubShapes.reserve(avShapes.size());
        
		for(size_t i=0; i<avShapes.size(); ++i)
		{
			mvSubShapes.push_back(avShapes[i]);

			cCollideShapeNewton *pNewtonShape = static_cast<cCollideShapeNewton*>(avShapes[i]);

			NewtonSceneProxy* pProxy = NewtonSceneCollisionCreateProxy(mpNewtonCollision, pNewtonShape->GetNewtonCollision());
			//cMatrixf mtxTransform = cMatrixf::Identity;
			//NewtonSceneProxySetMatrix(pProxy, &mtxTransform.GetTranspose().m[0][0]);
			if(apMatrices)
			{
				//TODO!
			}
			
		}
		
		NewtonSceneCollisionOptimize(mpNewtonCollision);
	}


	//-----------------------------------------------------------------------

	
	void cCollideShapeNewton::CreateCompoundFromShapeVec(tCollideShapeVec &avShapes)
	{	
		std::vector<NewtonCollision*> vNewtonColliders;
		
		vNewtonColliders.reserve(avShapes.size());
		mvSubShapes.reserve(avShapes.size());

		mfVolume =0;

		for(size_t i=0; i< avShapes.size(); i++)
		{
			mvSubShapes.push_back(avShapes[i]);
		
			cCollideShapeNewton *pNewtonShape = static_cast<cCollideShapeNewton*>(avShapes[i]);
			vNewtonColliders.push_back(pNewtonShape->GetNewtonCollision());

			mfVolume += pNewtonShape->GetVolume();
		}

		mpNewtonCollision = NewtonCreateCompoundCollision(mpNewtonWorld, (int)vNewtonColliders.size(),
															&vNewtonColliders[0], 0);

		// Create bounding volume
		cVector3f vFinalMax = avShapes[0]->GetBoundingVolume().GetMax();
		cVector3f vFinalMin = avShapes[0]->GetBoundingVolume().GetMin();

		for(size_t i=1; i< avShapes.size(); i++)
		{
			cVector3f vMax = avShapes[i]->GetBoundingVolume().GetMax();
			cVector3f vMin = avShapes[i]->GetBoundingVolume().GetMin();

			if(vFinalMax.x < vMax.x) vFinalMax.x = vMax.x;
			if(vFinalMin.x > vMin.x) vFinalMin.x = vMin.x;

			if(vFinalMax.y < vMax.y) vFinalMax.y = vMax.y;
			if(vFinalMin.y > vMin.y) vFinalMin.y = vMin.y;

			if(vFinalMax.z < vMax.z) vFinalMax.z = vMax.z;
			if(vFinalMin.z > vMin.z) vFinalMin.z = vMin.z;
		}

		mBoundingVolume.SetLocalMinMax(vFinalMin, vFinalMax);
		
	}
	
	//-----------------------------------------------------------------------

	void cCollideShapeNewton::CreateFromVertices(	const unsigned int* apIndexArray, int alIndexNum,
													const float *apVertexArray, int alVtxStride, int alVtxNum)
	{
		float vTriVec[9];

		bool bOptimize = false;
		bool bCreatedPlane = false;
		cPlanef plane;

		int lCount=0;

		//unsigned long lStartTime = cPlatform::GetApplicationTime();
		//Log("%d triangles: ", alIndexNum/3);

		mpNewtonCollision = NewtonCreateTreeCollision(mpNewtonWorld, 0);
		//Log("-- Creating mesh collision.:\n");
		NewtonTreeCollisionBeginBuild(mpNewtonCollision);
		for(int tri = 0; tri < alIndexNum; tri+=3)
		{
			//Log("tri: %d:\n", tri/3);
			for(int idx =0; idx < 3; idx++)
			{
				int lVtx = apIndexArray[tri + 2-idx]*alVtxStride;
                
				vTriVec[idx*3 + 0] = apVertexArray[lVtx + 0];
				vTriVec[idx*3 + 1] = apVertexArray[lVtx + 1];
				vTriVec[idx*3 + 2] = apVertexArray[lVtx + 2];
			}

			//if(alIndexNum<100)
			//{
			//	for(int i=0;i<3; ++i) Log("%f, %f, %f, ", vTriVec[i*3 + 0],vTriVec[i*3 + 1],vTriVec[i*3 + 2]);
			//	Log("\n");
			//}
			//lCount+=9;

			//cannot allow negative zeros! it is true!
			/*for(int i=0;i<9; ++i)
			{
				if(fabs(vTriVec[i]-0.0f) < 0.000001f) vTriVec[i] = 0;
			}*/

			if(false)//bOptimize==false)
			{
				cPlanef tempPlane;
				cVector3f vP1(vTriVec[0+0],vTriVec[0+1],vTriVec[0+2]);
				cVector3f vP2(vTriVec[1*3+0],vTriVec[1*3+1],vTriVec[1*3+2]);
				cVector3f vP3(vTriVec[2*3+0],vTriVec[2*3+1],vTriVec[2*3+2]);
				
				tempPlane.FromPoints(vP1, vP2, vP3);

				//Log("P1: %s P2: %s P3: %s\n",vP1.ToString().c_str(),vP2.ToString().c_str(),vP3.ToString().c_str());
				//Log("Plane: a: %f b: %f c: %f d: %f\n",tempPlane.a,tempPlane.b,tempPlane.c,tempPlane.d);
				
				/////////////////////////////////
				//Check if the triangles are on the same plane. If so, do not optimize
				// Do not check first triangle
				if(bCreatedPlane==false)
				{
					plane = tempPlane;
					bCreatedPlane = true;
				}
				// First plane created.
				else
				{
					//Check the plane equation is different from the first one (allow a small difference thoug).
					if(	std::abs(plane.a - tempPlane.a) > 0.001f ||
						std::abs(plane.b - tempPlane.b) > 0.001f ||
						std::abs(plane.c - tempPlane.c) > 0.001f ||
						std::abs(plane.d - tempPlane.d) > 0.001f )
					{
						bOptimize = true;
					}
				}
			}
					    
            NewtonTreeCollisionAddFace(mpNewtonCollision,3,vTriVec,sizeof(float)*3,1);
		}

		NewtonTreeCollisionEndBuild(mpNewtonCollision,0);//bOptimize ? 1: 0);

		//Log(" %dms\n", cPlatform::GetApplicationTime()-lStartTime);

		//Log("Numbers: %d\n",lCount);

		//Set bounding box size
		mBoundingVolume.AddArrayPoints(apVertexArray, alVtxNum);
		mBoundingVolume.CreateFromPoints(alVtxStride);
	}

	//-----------------------------------------------------------------------

	static void NewtonWriteToBinaryBuffer(void* apSerializeHandle, const void* apNewtonBuffer, int alSize)
	{
		cBinaryBuffer *pBinBuff = (cBinaryBuffer*)apSerializeHandle;
		pBinBuff->AddInt32Array((int*)apNewtonBuffer, (size_t)(alSize>>2));
	}

	void cCollideShapeNewton::SaveToSerializedData(cBinaryBuffer* apBinBuffer)
	{
		apBinBuffer->AddVector3f(mBoundingVolume.GetLocalMin());
		apBinBuffer->AddVector3f(mBoundingVolume.GetLocalMax());
		NewtonCollisionSerialize(mpNewtonWorld, mpNewtonCollision, NewtonWriteToBinaryBuffer, (void*)apBinBuffer);
	}

	//-----------------------------------------------------------------------

	static void NewtonReadFromBinaryBuffer(void* apSerializeHandle, void* apNewtonBuffer, int alSize)
	{
		cBinaryBuffer *pBinBuff = (cBinaryBuffer*)apSerializeHandle;
		pBinBuff->GetInt32Array((int*)apNewtonBuffer, (size_t)(alSize>>2));
	}

	void cCollideShapeNewton::CreateFromSerializedData(cBinaryBuffer* apBinBuffer)
	{
		cVector3f vMin, vMax;
		apBinBuffer->GetVector3f(&vMin);
		apBinBuffer->GetVector3f(&vMax);

		mpNewtonCollision = NewtonCreateCollisionFromSerialization (mpNewtonWorld, NewtonReadFromBinaryBuffer, (void*)apBinBuffer);

		mBoundingVolume.SetLocalMinMax(vMin, vMax);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

}
