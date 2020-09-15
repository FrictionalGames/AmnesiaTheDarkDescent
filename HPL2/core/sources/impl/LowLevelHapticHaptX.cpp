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

#include "impl/LowLevelHapticHaptX.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "impl/HapticSurfaceHaptX_Simple.h"
#include "impl/HapticSurfaceHaptX_Sticky.h"
#include "impl/HapticSurfaceHaptX_Frictional.h"
#include "impl/HapticSurfaceHaptX_Rough.h"

#include "impl/HapticForceHaptX_Impulse.h"
#include "impl/HapticForceHaptX_SineWave.h"
#include "impl/HapticForceHaptX_SawWave.h"
#include "impl/HapticForceHaptX_Spring.h"
#include "impl/HapticForceHaptX_Viscosity.h"

#include "impl/HapticShapeHaptX.h"

#include "scene/Camera.h"
#include "math/Math.h"
#include "graphics/VertexBuffer.h"
#include "resources/Resources.h"
#include "resources/MeshLoaderHandler.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/VertexBuffer.h"
#include "physics/PhysicsBody.h"
#include "physics/CollideShape.h"
#include "physics/SurfaceData.h"

namespace hpl {

	using namespace HaptX;
	using namespace Vectors;
	
	HaptXInterface* gpInterface;
	iHapticDeviceID	gDeviceID;

	//////////////////////////////////////////////////////////////////////////
	// HELPERS
	//////////////////////////////////////////////////////////////////////////

	static eHaptXShapeModelType GetModelShapeType(eCollideShapeType aType)
	{
		switch(aType)
		{
		case eCollideShapeType_Box: return eHaptXShapeModelType_Box;
		case eCollideShapeType_Cylinder: return eHaptXShapeModelType_Cylinder;
		case eCollideShapeType_Capsule: return eHaptXShapeModelType_Capsule;
		case eCollideShapeType_Sphere: return eHaptXShapeModelType_Sphere;
		}
		return eHaptXShapeModelType_LastEnum;
	}

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelHapticHaptX::cLowLevelHapticHaptX()
	{
		mpInterface = NULL;
		mDeviceID = -1;
		mbRenderingActive = true;
	}

	//-----------------------------------------------------------------------

	cLowLevelHapticHaptX::~cLowLevelHapticHaptX()
	{
		STLDeleteAll(mvShapeModels);

		DestroyAll();

		if(mDeviceID >=0)
			mpInterface->DeviceDestroy(mDeviceID);
		
		HaptX::ReleaseHaptXInterface();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// GENERAL METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	bool cLowLevelHapticHaptX::InitLowLevel()
	{
		//Get the interface
		mpInterface = HaptX::GetHaptXInterface();
		if (!mpInterface){ 
			Error("Failed to retrieve HaptX interface\n");
			return false;
		}
		gpInterface = mpInterface;
		
		//Initalize the interface
		if (mpInterface->HaptXInit( _W("Frictional Games"),
									_W("Penumbra Overture"),
									_W("Episode 1"),
									_W("Windows 32 bit"),
									_W("4VTJ-W7EH-DQUF-JADW")) != HX_SUCCESS)
		{
			Error("HaptXInit failed\n");
			HaptX::ReleaseHaptXInterface();
			return false;
		}
		
		//Get the active device
		gDeviceID = -1;
		mpInterface->HaptXHookMessages(HX_INFO_MSG, HaptXMsgCallback); //Register to recieve all messages with a severity down to and including Information messages. 
		mpInterface->AllDevicesEnum(HapticDeviceEnumFunc);
		mDeviceID = gDeviceID;
		if (mDeviceID < 0){
			Error("Failed to create a device. Please make sure the Touch Controller is connected.\n");
			HaptX::ReleaseHaptXInterface();
			return false;
		}
		
		//Set up world
		mpInterface->DeviceSetWorldScale(mDeviceID,0.03f);
		mpInterface->HaptXSetRightHanded();
		mpInterface->DeviceSetCameraRelative(mDeviceID,true);

		//Create default surface
		mpInterface->SurfaceCreateSimple(&mpDefaultSurfaceInfo); 

		//Set up variables
		mvPreviousHardwarePos = GetHardwarePosition();

		//Create needed data
		CreateShapeModels();

		return true;
	}
	
	//-----------------------------------------------------------------------

	void cLowLevelHapticHaptX::UpdateLowLevel(float afTimeStep)
	{
		/////////////////////////////////////
		//Check if device is connected
		bool bConnected = false;
		mpInterface->DeviceIsConnected(bConnected,mDeviceID);
		if(bConnected == false) return;

		/////////////////////////////////////
		//Update Camera
		if(mpCamera)
		{
			cMatrixf mtxProxyCam = cMath::MatrixRotate(cVector3f(	mpCamera->GetPitch(),
																	mpCamera->GetYaw(),
																	mpCamera->GetRoll()),
																	eEulerRotationOrder_XYZ);
			
			cVector3f vProxyCamPos = mpCamera->GetPosition() +
									 mpCamera->GetForward() * mvCameraOffset.z +
									 mpCamera->GetRight() * mvCameraOffset.x +
									 mpCamera->GetUp() * mvCameraOffset.y;

			
			mtxProxyCam.SetTranslation(vProxyCamPos * GetWorldScale());
			mpInterface->DeviceSetCameraMatrix(mDeviceID,mtxProxyCam.v);
		}

		/////////////////////////////////////
		//Render all stuff
		mpInterface->DeviceRender(mDeviceID);
	}

	//-----------------------------------------------------------------------

	float cLowLevelHapticHaptX::GetMaxForce()
	{
		float fForce;
		mpInterface->DeviceGetMaxForce(fForce,mDeviceID);
		return fForce;
	}

	//-----------------------------------------------------------------------

	void cLowLevelHapticHaptX::SetWorldScale(float afScale)
	{
		mpInterface->DeviceSetWorldScale(mDeviceID,afScale);
	}

	float cLowLevelHapticHaptX::GetWorldScale()
	{
		float fScale;
		mpInterface->DeviceGetWorldScale(fScale,mDeviceID);
		return fScale;
	}

	//-----------------------------------------------------------------------

	void cLowLevelHapticHaptX::SetRenderingActive(bool abX)
	{
		if(mbRenderingActive == abX) return;

		mbRenderingActive = abX;

		if(abX)	mpInterface->HaptXEnableShapeRendering();
		else	mpInterface->HaptXDisableShapeRendering();
	}

	//-----------------------------------------------------------------------

	bool cLowLevelHapticHaptX::IsRenderingActive()
	{
		return mbRenderingActive;
	}

	//-----------------------------------------------------------------------

	bool cLowLevelHapticHaptX::ShapeIsInContact(iHapticShape *apShape)
	{
		bool bContact;
		cHapticShapeHaptX *pHXShape = static_cast<cHapticShapeHaptX*>(apShape);
		mpInterface->ShapeIsDeviceInContact(bContact,mDeviceID,pHXShape->GetHaptXShapeID());
		return bContact;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// INPUT METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cLowLevelHapticHaptX::ButtonIsPressed(int alNum)
	{
		bool abPressed;
		mpInterface->DeviceGetButton(abPressed, mDeviceID,alNum);
		return abPressed;
	}
	
	//-----------------------------------------------------------------------
	
	cVector3f cLowLevelHapticHaptX::GetHardwarePosition()
	{
		Vec3f vPos;
		mpInterface->DeviceGetHardwarePosition(vPos, mDeviceID);
		return cVector3f((float)vPos.x, (float)vPos.y, (float)vPos.z);
	}

	cVector3f cLowLevelHapticHaptX::GetHardwareRotation()
	{
		Vec3f vRot;
		mpInterface->DeviceGetHardwarePosition(vRot, mDeviceID);
		return cVector3f((float)vRot.x, (float)vRot.y, (float)vRot.z);
	}

	//-----------------------------------------------------------------------

	cVector3f cLowLevelHapticHaptX::GetProxyPosition()
	{
		Vec3f vPos;
		mpInterface->DeviceGetProxyPosition(vPos, mDeviceID);
		return cVector3f((float)vPos.x, (float)vPos.y, (float)vPos.z);
	}

	void cLowLevelHapticHaptX::SetProxyRadius(float afRadius)
	{
		mpInterface->DeviceSetProxyRadius(mDeviceID, (mgFloat)afRadius*GetWorldScale());
	}

	float cLowLevelHapticHaptX::GetProxyRadius()
	{
		double fRadius;
		mpInterface->DeviceGetProxyRadius(fRadius, mDeviceID);
		return (float)fRadius / GetWorldScale();
	}

	//-----------------------------------------------------------------------

	int cLowLevelHapticHaptX::GetNumberOfButtons()
	{
		int lNum;
		mpInterface->DeviceGetNrOfButtons(lNum, mDeviceID);
		return lNum;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// SURFACE METHODS
	//////////////////////////////////////////////////////////////////////////


	//-----------------------------------------------------------------------

	iHapticSurface* cLowLevelHapticHaptX::CreateSimpleSurface(const tString &asName,
										float afDamping, float afStiffness)
	{
		iHapticSurfaceHaptX* pSurface = hplNew( cHapticSurfaceHaptX_Simple,(asName,mpInterface, afDamping,afStiffness) );

		tHapticSurfaceMap::value_type Val(asName,pSurface);
		m_mapSurfaces.insert(Val);

		return pSurface;
	}

	//-----------------------------------------------------------------------

	iHapticSurface* cLowLevelHapticHaptX::CreateFrictionalSurface(const tString &asName,
		float afDamping, float afStiffness,
		float afDynamicFriction, float afStartingFriction,
		float afTangentStiffness, float afStoppingFriction)
	{
		iHapticSurfaceHaptX* pSurface = hplNew( cHapticSurfaceHaptX_Frictional, (asName,mpInterface, afDamping,afStiffness,
														afDynamicFriction, afStartingFriction,
														afTangentStiffness,afStoppingFriction) );

		tHapticSurfaceMap::value_type Val(asName,pSurface);
		m_mapSurfaces.insert(Val);

		return pSurface;
	}

	//-----------------------------------------------------------------------


	iHapticSurface* cLowLevelHapticHaptX::CreateRoughSurface(const tString &asName, 
		float afDeviation, float afMean,
		float afDamping, float afStiffness,
		float afDynamicFriction, float afStartingFriction,
		float afTangentStiffness, float afStoppingFriction)
	{
		iHapticSurfaceHaptX* pSurface = hplNew( cHapticSurfaceHaptX_Rough, (asName,mpInterface, afDeviation,afMean,
																			afDamping,afStiffness,
																			afDynamicFriction, afStartingFriction,
																			afTangentStiffness,afStoppingFriction) );

		tHapticSurfaceMap::value_type Val(asName,pSurface);
		m_mapSurfaces.insert(Val);

		return pSurface;
	}

	//-----------------------------------------------------------------------

	iHapticSurface* cLowLevelHapticHaptX::CreateStickySurface(const tString &asName, 
		float afDeadHeight, float afStickyStiffness,
		float afDamping, float afStiffness,
		float afDynamicFriction, float afStartingFriction,
		float afTangentStiffness, float afStoppingFriction)
	{
		iHapticSurfaceHaptX* pSurface = hplNew( cHapticSurfaceHaptX_Rough, (asName,mpInterface, afDeadHeight, afStickyStiffness,
																		afDamping,afStiffness,
																		afDynamicFriction, afStartingFriction,
																		afTangentStiffness,afStoppingFriction) );

		tHapticSurfaceMap::value_type Val(asName,pSurface);
		m_mapSurfaces.insert(Val);

		return pSurface;
	}	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SHAPE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iHapticShape* cLowLevelHapticHaptX::CreateBoxShape(const tString &asName,const cVector3f &avSize, cMatrixf* apOffsetMtx)
	{
		return NULL;
	}

	//-----------------------------------------------------------------------

	iHapticShape* cLowLevelHapticHaptX::CreateSphereShape(const tString &asName,const cVector3f &avRadii, cMatrixf* apOffsetMtx)
	{
		return NULL;
	}

	//-----------------------------------------------------------------------

	iHapticShape* cLowLevelHapticHaptX::CreateCylinderShape(const tString &asName, float afRadius, float afHeight, cMatrixf* apOffsetMtx)
	{
		return NULL;
	}

	//-----------------------------------------------------------------------

	iHapticShape* cLowLevelHapticHaptX::CreateCapsuleShape(const tString &asName, float afRadius, float afHeight, cMatrixf* apOffsetMtx)
	{
		return NULL;
	}

	//-----------------------------------------------------------------------

	iHapticShape* cLowLevelHapticHaptX::CreateMeshShape(const tString &asName, iVertexBuffer *apVtxBuffer)
	{
		cHapticShapeHaptX *pShape = hplNew( cHapticShapeHaptX, (asName,eHapticShapeType_Mesh,1,this) );
		
		pShape->CreateFromVertices(	apVtxBuffer->GetIndices(),apVtxBuffer->GetIndexNum(),
									
									apVtxBuffer->GetFloatArray(eVertexBufferElement_Position),
									apVtxBuffer->GetElementNum(eVertexBufferElement_Position),

									NULL,
									3,
									
									apVtxBuffer->GetVertexNum());

		mlstShapes.push_back(pShape);

		return pShape;
	}

	//-----------------------------------------------------------------------

	iHapticShape* cLowLevelHapticHaptX::CreateCompundShape(const tString &asName, tHapticShapeVec &avShapes)
	{
		return NULL;
	}

	//-----------------------------------------------------------------------

	iHapticShape* cLowLevelHapticHaptX::CreateShapeFromPhysicsBody(	const tString &asName, 
																	iPhysicsBody *apBody)
	{
		iCollideShape *pBaseShape = apBody->GetShape();
		if(pBaseShape->GetType() == eCollideShapeType_Mesh) return NULL;

		/////////////////////////////////////////////////////////
		//Calculate the number of indices and positions required
		int lIndexNum=0;
		int lPosNum=0;

		for(int i=0; i<pBaseShape->GetSubShapeNum(); ++i)
		{
			iCollideShape *pShape = pBaseShape->GetSubShape(i);
			eHaptXShapeModelType modelType = GetModelShapeType(pShape->GetType());

			lIndexNum += (int)mvShapeModels[modelType]->mvIndices.size();
			lPosNum += (int)mvShapeModels[modelType]->mvPositions.size();
		}

		int *pIndices = hplNewArray(int, lIndexNum );
		Vec3f *pPositions = hplNewArray(Vec3f,lPosNum);
		
		/////////////////////////////////////////////////////////
		//Create data from all sub shapes.
		int lIndexCount =0;
		int lPosCount=0;
		for(int i=0; i<pBaseShape->GetSubShapeNum(); ++i)
		{
			iCollideShape *pShape = pBaseShape->GetSubShape(i);
			eHaptXShapeModelType modelType = GetModelShapeType(pShape->GetType());
			
			ShapeModelToHaptXData(	modelType,pShape->GetSize(),pShape->GetOffset(),
									&pIndices[lIndexCount], lPosCount,
									&pPositions[lPosCount]);

			lIndexCount += (int)mvShapeModels[modelType]->mvIndices.size();
			lPosCount += (int)mvShapeModels[modelType]->mvPositions.size();
		}

		/////////////////////////////////////////////////////////
		//Create final shape
		cHapticShapeHaptX *pShape = hplNew( cHapticShapeHaptX, (asName,eHapticShapeType_Mesh,1,this) );
		
		iHapticShapeID hShapeID;
		mpInterface->ShapeCreate(hShapeID, 
								pPositions,lPosNum,
								pIndices, lIndexNum,
								GetDefaultSurface()->GetSurfaceID(),
								NULL,true,true);
		pShape->SetHaptXShapeID(hShapeID);

		//Temp, delete these instead.
		/*pShape->mlIndexNum = lIndexNum;
		pShape->mlPosNum = lPosNum;
		pShape->mpIndices = pIndices;
		pShape->mpHxPositions = pPositions;*/

		hplDeleteArray(pIndices);
		hplDeleteArray(pPositions);
		
		pShape->SetBody(apBody);
		mlstShapes.push_back(pShape);

		apBody->SetHapticShape(pShape);

		/////////////////////////////////////////////////////////
		//Set surface
		iPhysicsMaterial *pMaterial = apBody->GetMaterial();
		iHapticSurface *pSurface = pMaterial->GetSurfaceData()->GetHapticSurface();
        if(pSurface)
		{
			pShape->SetSurface(pSurface);
		}
        

		return pShape;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// FORCE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iHapticForce* cLowLevelHapticHaptX::CreateImpulseForce(const cVector3f& avForce)
	{
		iHapticForce *pForce = hplNew( cHapticForceHaptX_Impulse, (mpInterface,mDeviceID) );
		mlstForces.push_back(pForce);

		return pForce;
	}

	//-----------------------------------------------------------------------

	iHapticForce* cLowLevelHapticHaptX::CreateSinusWaveForce(const cVector3f& avDirection, float afAmp, float afFreq)
	{
		iHapticForce *pForce = hplNew( cHapticForceHaptX_SineWave, (mpInterface,mDeviceID,
										avDirection,afAmp,afFreq) );
		mlstForces.push_back(pForce);

		return pForce;
	}

	//-----------------------------------------------------------------------

	iHapticForce* cLowLevelHapticHaptX::CreateSawWaveForce(	const cVector3f& avDirection, 
															float afAmp, float afFreq)
	{
		iHapticForce *pForce = hplNew( cHapticForceHaptX_SawWave, (mpInterface,mDeviceID,avDirection,afAmp,afFreq) );
		mlstForces.push_back(pForce);

		return pForce;
	}

	//-----------------------------------------------------------------------

	iHapticForce* cLowLevelHapticHaptX::CreateSpringForce(	const cVector3f& avPostition, 
															float afStiffness, float afDamping)
	{
		iHapticForce *pForce = hplNew( cHapticForceHaptX_Spring, (mpInterface,mDeviceID,avPostition,afStiffness, afDamping) );
		mlstForces.push_back(pForce);

		return pForce;
	}

	//-----------------------------------------------------------------------

	iHapticForce* cLowLevelHapticHaptX::CreateViscosityForce(const cVector3f& avVelocity,float afMass, float afStiffness, float afDamping)
	{
		iHapticForce *pForce = hplNew( cHapticForceHaptX_Viscosity, (mpInterface,mDeviceID,avVelocity, afMass, afStiffness, afDamping) );
		mlstForces.push_back(pForce);

		return pForce;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cLowLevelHapticHaptX::ShapeModelToHaptXData(	eHaptXShapeModelType aType,
														const cVector3f& avSize,
														const cMatrixf& a_mtxTransform,
														int *apIndices,int alIndexAdd,
														Vec3f *apPostions)
	{
		cHaptXShapeModel *pShapeModel = mvShapeModels[aType];
		if(pShapeModel==NULL) return;

		cMatrixf mtxFinal;
		if(aType == eHaptXShapeModelType_Cylinder || aType == eHaptXShapeModelType_Capsule)
		{
			//Rotate to align on the X axis.
			mtxFinal = cMath::MatrixMul(cMath::MatrixRotateZ(kPi2f),
										cMath::MatrixScale(avSize));
		}
		else
		{
			mtxFinal = cMath::MatrixScale(avSize);
		}
		
		mtxFinal = cMath::MatrixMul( a_mtxTransform, mtxFinal);

		//Copy indices
		for(size_t i=0;i<pShapeModel->mvIndices.size();++i)
		{
			apIndices[i] = pShapeModel->mvIndices[i] + alIndexAdd;
		}

		//Copy and transform positions
		for(size_t i=0;i<pShapeModel->mvPositions.size();++i)
		{
			cVector3f vPos = cMath::MatrixMul(mtxFinal, pShapeModel->mvPositions[i]);
			apPostions[i] = Vec3f(vPos.x,vPos.y,vPos.z);
		}
	}
	
	//-----------------------------------------------------------------------

	cHaptXShapeModel * cLowLevelHapticHaptX::CreateShapeModelFromMesh(cMesh *apMesh)
	{
		iVertexBuffer* pVtxBuff = apMesh->GetSubMesh(0)->GetVertexBuffer();
		
		cHaptXShapeModel *pShapeModel = hplNew( cHaptXShapeModel, () );

		////////////////////
		//Indices
		pShapeModel->mvIndices.resize(pVtxBuff->GetIndexNum());
		for(int i=0; i< pVtxBuff->GetIndexNum(); ++i) 
		{
			pShapeModel->mvIndices[i] = (int)(pVtxBuff->GetIndices())[i];
		}
		
		////////////////////////
		//Postions
		float *pPosVec = pVtxBuff->GetFloatArray(eVertexBufferElement_Position);
		int lPosStride = pVtxBuff->GetElementNum(eVertexBufferElement_Position);
		
		pShapeModel->mvPositions.resize(pVtxBuff->GetVertexNum());
		for(int i=0; i<pVtxBuff->GetVertexNum(); ++i)
		{
			const float* pPos = &pPosVec[i * lPosStride];
			cVector3f &vDest = pShapeModel->mvPositions[i];

			vDest.x = pPos[0];
			vDest.y = pPos[1];
			vDest.z = pPos[2];
		}

		//Log("VertexNum: %d\n",pVtxBuff->GetVertexNum());

		
		return pShapeModel;
	}

	//-----------------------------------------------------------------------


	void cLowLevelHapticHaptX::CreateShapeModels()
	{
		/////////////////////////////////
		//Init
		mvShapeModels.resize(eHaptXShapeModelType_LastEnum,NULL);
		
		cMeshLoaderHandler *pMeshHandler = mpResources->GetMeshLoaderHandler();
		cMesh *pMesh = NULL;

		////////////////////////////////
		//Box
		pMesh = pMeshHandler->LoadMesh(_W("core/haptx/haptx_shape_box.dae"),0);
		if(pMesh==NULL) FatalError("Could not load mesh core/haptx/haptx_shape_box.dae!\n");

		mvShapeModels[eHaptXShapeModelType_Box] = CreateShapeModelFromMesh(pMesh);
		hplDelete(pMesh);

		////////////////////////////////
		//Cylinder
		pMesh = pMeshHandler->LoadMesh(_W("core/haptx/haptx_shape_cylinder.dae"),0);
		if(pMesh==NULL) FatalError("Could not load mesh core/haptx/haptx_shape_cylinder.dae!\n");

		mvShapeModels[eHaptXShapeModelType_Cylinder] = CreateShapeModelFromMesh(pMesh);
		hplDelete(pMesh);

		////////////////////////////////
		//Capsule
		pMesh = pMeshHandler->LoadMesh(_W("core/haptx/haptx_shape_cylinder.dae"),0);
		if(pMesh==NULL) FatalError("Could not load mesh core/haptx/haptx_shape_cylinder.dae!\n");

		mvShapeModels[eHaptXShapeModelType_Capsule] = CreateShapeModelFromMesh(pMesh);
		hplDelete(pMesh);

		////////////////////////////////
		//Sphere
		pMesh = pMeshHandler->LoadMesh(_W("core/haptx/haptx_shape_sphere.dae"),0);
		if(pMesh==NULL) FatalError("Could not load mesh core/haptx/haptx_shape_sphere.dae!\n");

		mvShapeModels[eHaptXShapeModelType_Sphere] = CreateShapeModelFromMesh(pMesh);
		hplDelete(pMesh);

	}
	
	//-----------------------------------------------------------------------

	int cLowLevelHapticHaptX::HapticDeviceEnumFunc(	std::wstring* asDeviceName, 
													std::wstring* asDeviceModel,
													void* apVendorData)
	{
		if(gpInterface->DeviceCreate(gDeviceID,asDeviceName,asDeviceModel,apVendorData) != HX_SUCCESS)
		{
			gDeviceID = -1;
		}
		return HaptX::HX_STOP;
	}

	//-----------------------------------------------------------------------

	void cLowLevelHapticHaptX::HaptXMsgCallback(int alSeverity, std::wstring asMessage)
	{
		Log("%s", cString::To8Char(asMessage).c_str());
	}
	
	//-----------------------------------------------------------------------

}
