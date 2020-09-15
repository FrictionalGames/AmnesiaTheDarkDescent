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

#include "../ModelEditor/EdWindowPhysicsTest.h"

#include "../Common/EdCamera.h"
#include "../Common/EdWindowViewport.h"

#include "../ModelEditor/ModelEditor.h"
#include "../ModelEditor/HPLEntity.h"

#include <algorithm>

//------------------------------------------------------------------------------------

/////////////////////////////////////
// Static members init
bool cEdWindowPhysicsTest::mbBuoyancyActive = false;
float cEdWindowPhysicsTest::mfBuoyancyDensity = 100;
float cEdWindowPhysicsTest::mfBuoyancyLinearViscosity = 0.2f; 
float cEdWindowPhysicsTest::mfBuoyancyAngularViscosity = 0.2f;
cPlanef cEdWindowPhysicsTest::mBuoyancySurface = cPlanef(0,1,0,-1);

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// BODY PICKER
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

cBodyPicker::cBodyPicker()
{
	Clear();
}

void cBodyPicker::Clear()
{
	mfLastT = 100000.0f;
	mpPickedBody = NULL;
}

bool cBodyPicker::OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams)
{
	if(pBody->GetMass()>0)
	{
		if(apParams->mfT < mfLastT)
		{
			mpPickedBody = pBody;
			mfLastT = apParams->mfT;
			mvPos = apParams->mvPoint;
			mfDist = apParams->mfDist;
		}
	}

	return true;
}

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

int cEdWindowPhysicsTest::mlLastSelectedAnimation = -1;

//------------------------------------------------------------------------------------

cEdWindowPhysicsTest::cEdWindowPhysicsTest(iEditor* apEditor, iFrameBuffer* apFB): iEdViewport(apEditor, _W("Physics Test"), apEditor->GetEngine()->GetScene()->CreateWorld("PhysicsTestWorld"), apFB)
{
	mpTestWorld = mpEngineViewport->GetWorld();
	mpTestEntity = NULL;
	mpMainLight = NULL;
	mpBackLight = NULL;

	mpPrevAttention = NULL;

	mbDrawDebug = false;
	mbDrawSkeleton = false;

	mpCamera->AddCamMode(hplNew(cCamModePerspective,(mpCamera, 0)));
	mpCamera->GetCurrentController()->LockToGrid(false);
}

//------------------------------------------------------------------------------------

cEdWindowPhysicsTest::~cEdWindowPhysicsTest()
{
	SetActive(false);

	cScene* pScene = mpEditor->GetEngine()->GetScene();
	cPhysics* pPhysics = mpEngine->GetPhysics();

	if(mpTestEntity)
	{
		mpTestWorld->DestroyMeshEntity(mpTestEntity);
		mpTestEntity = NULL;
	}
	mpEngineViewport->SetWorld(NULL);
	mpTestWorld->SetPhysicsWorld(NULL);
	pPhysics->DestroyWorld(mpTestPhysicsWorld);
	pScene->DestroyWorld(mpTestWorld);
}

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

void cEdWindowPhysicsTest::SetSelectedAnimation(int alX)
{
	if(mpInpAnimations)
		mpInpAnimations->SetValue(alX);
}

int cEdWindowPhysicsTest::GetSelectedAnimation()
{
	return mpInpAnimations->GetValue();
}

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

bool cEdWindowPhysicsTest::WindowSpecificInputCallback(iEdInput* apInput)
{
	bool bBuoyancyUpdated = false;

	if(apInput==mpInpAnimations)
	{
		int lAnim = mpInpAnimations->GetValue();
		if(lAnim<=0)
		{
			if(mpTestEntity)
				mpTestEntity->Stop();
		}
		else
		{
			if(mpTestEntity)
				mpTestEntity->Play(lAnim-1, true,true);
		}
	}
	else if(apInput==mpInpShowDebug)
		mbDrawDebug = mpInpShowDebug->GetValue();
	else if(apInput==mpInpShowSkeleton)
		mbDrawSkeleton = mpInpShowSkeleton->GetValue();
	else if(apInput==mpInpMainLight)
		mpMainLight->SetVisible(mpInpMainLight->GetValue());
	else if(apInput==mpInpBackLight)
		mpBackLight->SetVisible(mpInpBackLight->GetValue());
	else if(apInput==mpInpBuoyancyActive)
	{
		mbBuoyancyActive = mpInpBuoyancyActive->GetValue();
		bBuoyancyUpdated = true;
	}
	else if(apInput==mpInpBuoyancyDensity)
	{
		mfBuoyancyDensity = mpInpBuoyancyDensity->GetValue();
		bBuoyancyUpdated = true;
	}
	else if(apInput==mpInpBuoyancyAngularViscosity)
	{
		mfBuoyancyAngularViscosity = mpInpBuoyancyAngularViscosity->GetValue();
		bBuoyancyUpdated = true;
	}
	else if(apInput==mpInpBuoyancyLinearViscosity)
	{
		mfBuoyancyLinearViscosity = mpInpBuoyancyLinearViscosity->GetValue();
		bBuoyancyUpdated = true;
	}
	else if(apInput==mpInpBuoyancySurfaceHeight)
	{
		mBuoyancySurface.d = -mpInpBuoyancySurfaceHeight->GetValue();
		bBuoyancyUpdated = true;
	}
	else
	{
		return iEdViewport::WindowSpecificInputCallback(apInput);
	}

	if(bBuoyancyUpdated)
		UpdateBuoyancy();
		

	return true;
}

//------------------------------------------------------------------------------------

void cEdWindowPhysicsTest::SetUpRender()
{
	cScene* pScene = mpEngine->GetScene();
	cPhysics* pPhysics = mpEngine->GetPhysics();
	
	mpMainLight = mpTestWorld->CreateLightPoint("", "", false);
	mpMainLight->SetPosition(cVector3f(0,2, 3));
	mpMainLight->SetRadius(20);
	mpMainLight->SetDiffuseColor(cColor(1));
	mpMainLight->SetVisible(true);

	mpBackLight = mpTestWorld->CreateLightPoint("", "", false);
	mpBackLight->SetPosition(cVector3f(0,1, -3));
	mpBackLight->SetRadius(20);
	mpBackLight->SetDiffuseColor(cColor(1));
	mpBackLight->SetVisible(true);

	mpTestPhysicsWorld = pPhysics->CreateWorld(true);
	mpTestPhysicsWorld->SetAccuracyLevel(ePhysicsAccuracy_Medium);
	mpTestPhysicsWorld->SetWorldSize(-30,30);
	mpTestPhysicsWorld->SetMaxTimeStep(1.0f / 60.0f);

	mpTestWorld->SetPhysicsWorld(mpTestPhysicsWorld);

	/////////////////
	//Floor and water
	cMeshManager* pMgr = mpEngine->GetResources()->GetMeshManager();
	cMesh* pMesh = pMgr->CreateMesh("editor_rect.dae");
	mpFloor = mpTestWorld->CreateMeshEntity("Floor",pMesh,true);
	mpFloor->SetMatrix(cMath::MatrixScale(6));
	mpFloor->SetVisible(true);

	pMesh = pMgr->CreateMesh("editor_water_plane.dae");
	mpWater = mpTestWorld->CreateMeshEntity("Water",pMesh,true);
	mpWater->SetMatrix(cMath::MatrixScale(6));
	mpWater->SetVisible(false);

	mpTestWorld->Compile(false);

	mpCamera->SetCameraPosition(cVector3f(0,5,5));
	mpCamera->UpdateEngineCamera();
}

//------------------------------------------------------------------------------------

void cEdWindowPhysicsTest::SetUpModel()
{
	mpInpAnimations->SetValue(0);

	cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
	mpTestEntity = pEnt->CreateTestEntity(mpTestWorld, mvJoints);
	if(mpTestEntity==NULL) return;
	mpTestEntity->SetVisible(true);

	// Populate animations list
	for(int i=0;i<mpTestEntity->GetAnimationStateNum();++i)
	{
		cAnimationState* pAnimState = mpTestEntity->GetAnimationState(i);
		tWString sAnimName = cString::To16Char(pAnimState->GetName());
		
		mpInpAnimations->AddValue(sAnimName);

		pAnimState->SetSpeed(pAnimState->GetBaseSpeed());
		pAnimState->SetActive(false);
		pAnimState->SetLoop(true);
	}
	mpFloor->SetPosition(cVector3f(0,mpTestEntity->GetBoundingVolume()->GetMin().y -0.2f,0));

	cMatrixf mtxOffset = cMath::MatrixTranslate(cVector3f(0,-0.05f,0));
	iCollideShape *pShape = mpTestPhysicsWorld->CreateBoxShape(cVector3f(12,0.1f, 12),&mtxOffset);
	mpFloorBody = mpTestPhysicsWorld->CreateBody("Floor", pShape);
	mpFloorBody->Enable();
	mpFloorBody->SetActive(true);
	mpFloorBody->SetPosition(mpFloor->GetWorldPosition());

	cPhysicsBodyIterator bodyIt = mpTestPhysicsWorld->GetBodyIterator();
	while(bodyIt.HasNext())
	{
		iPhysicsBody *pBody = bodyIt.Next();
		pBody->Enable();
		pBody->SetActive(true);
	}

	UpdateBuoyancy();
}

//------------------------------------------------------------------------------------

void cEdWindowPhysicsTest::UpdateBuoyancy()
{
	cPhysicsBodyIterator bodyIt = mpTestPhysicsWorld->GetBodyIterator();
	while(bodyIt.HasNext())
	{
		iPhysicsBody *pBody = bodyIt.Next();
		if(pBody==mpFloorBody)
			continue;

		pBody->SetBuoyancyActive(mbBuoyancyActive);
		pBody->SetBuoyancyDensity(mfBuoyancyDensity);
		pBody->SetBuoyancyAngularViscosity(mfBuoyancyAngularViscosity);
		pBody->SetBuoyancyLinearViscosity(mfBuoyancyLinearViscosity);
		pBody->SetBuoyancySurface(mBuoyancySurface);
	}

	mpWater->SetPosition(cVector3f(0,-mBuoyancySurface.d,0));
	mpWater->SetVisible(mbBuoyancyActive);
}

//------------------------------------------------------------------------------------

void cEdWindowPhysicsTest::OnSetActive(bool abX)
{
	iEdViewport::OnSetActive(abX);

	cScene* pScene = mpEditor->GetEngine()->GetScene();
	if(abX)
	{
		SetUpRender();
		SetUpModel();

		mbViewportEnlarged = mpEditor->IsViewportEnlarged();
		mpEditor->SetViewportsActive(false);

		if(mlLastSelectedAnimation!=-1)
			mpInpAnimations->SetValue(mlLastSelectedAnimation);
		pScene->SetCurrentListener(mpEngineViewport);
	}
	else
	{
		mlLastSelectedAnimation = mpInpAnimations->GetValue();
		pScene->SetCurrentListener(NULL);

		mpEditor->SetViewportsActive(true);
		mpEditor->SetViewportEnlarge(mbViewportEnlarged);
	}
}

//------------------------------------------------------------------------------------

void cEdWindowPhysicsTest::OnCreateLayout()
{
	iEdViewport::OnCreateLayout();

	iWidget* pBG = GetBG();

	float fUpperBorder = 5;
	cVector3f vPos = cVector3f(10,fUpperBorder,0.1f);

	mpInpShowDebug = CreateInputBool(vPos, _W("Show Physics Debug info"), pBG);
	vPos.y += mpInpShowDebug->GetSize().y + 3;
	mpInpShowSkeleton = CreateInputBool(vPos, _W("Show Skeleton"), pBG);

	vPos.y = fUpperBorder;
	vPos.x += cMath::Max(mpInpShowDebug->GetSize().x, mpInpShowSkeleton->GetSize().x) + 10;

	mpInpAnimations = CreateInputEnum(vPos-cVector3f(0,2,0),_W("Animation"), pBG);
	mpInpAnimations->SetStyle(eEdInpStyle_RowLabelOnLeft);
	mpInpAnimations->UpdateLayout();
	mpInpAnimations->AddValue(_W("None"));

	vPos = mpInpAnimations->GetPosition() + cVector3f(mpInpAnimations->GetSize().x + 20,0,0);
	mpInpMainLight = CreateInputBool(vPos, _W("Main Light"), pBG);
	vPos.y += mpInpMainLight->GetSize().y + 3;
	mpInpBackLight = CreateInputBool(vPos, _W("Back Light"), pBG);

	mpInpMainLight->SetValue(true, false);
	mpInpBackLight->SetValue(true, false);

	vPos.y = fUpperBorder;
	vPos.x = mpInpMainLight->GetPosition().x + mpInpMainLight->GetSize().x + 20;

	mpInpBuoyancyActive = CreateInputBool(vPos, _W("Buoyancy"), pBG);

	vPos.x += mpInpBuoyancyActive->GetSize().x+10;

	mpInpBuoyancyDensity = CreateInputNumber(vPos, _W("B.Density"), pBG,50,1);
	mpInpBuoyancyDensity->SetStyle(eEdInpStyle_RowLabelOnLeft);
	mpInpBuoyancyDensity->SetTabWidth(55);
	mpInpBuoyancyDensity->UpdateLayout();
	mpInpBuoyancySurfaceHeight = CreateInputNumber(vPos + cVector3f(0, mpInpBuoyancyDensity->GetSize().y+5,0), _W("S.Height"), pBG,50,0.1f);
	mpInpBuoyancySurfaceHeight->SetStyle(eEdInpStyle_RowLabelOnLeft);
	mpInpBuoyancySurfaceHeight->SetTabWidth(55);
	mpInpBuoyancySurfaceHeight->UpdateLayout();

	vPos.x = mpInpBuoyancyDensity->GetPosition().x + cMath::Max(mpInpBuoyancyDensity->GetSize().x, mpInpBuoyancySurfaceHeight->GetSize().x) + 10;

	mpInpBuoyancyAngularViscosity = CreateInputNumber(vPos, _W("B.A.viscosity"), pBG,50,0.1f);
	mpInpBuoyancyAngularViscosity->SetStyle(eEdInpStyle_RowLabelOnLeft);
	mpInpBuoyancyAngularViscosity->UpdateLayout();
	mpInpBuoyancyLinearViscosity = CreateInputNumber(vPos + cVector3f(0, mpInpBuoyancyAngularViscosity->GetSize().y+5,0), _W("B.L.viscosity"), pBG,50,0.1f);
	mpInpBuoyancyLinearViscosity->SetStyle(eEdInpStyle_RowLabelOnLeft);
	mpInpBuoyancyLinearViscosity->UpdateLayout();

	mpInpBuoyancyActive->SetValue(mbBuoyancyActive, false);
	mpInpBuoyancyDensity->SetValue(mfBuoyancyDensity, false);
	mpInpBuoyancySurfaceHeight->SetValue(-mBuoyancySurface.d, false);
	mpInpBuoyancyAngularViscosity->SetValue(mfBuoyancyAngularViscosity, false);
	mpInpBuoyancyLinearViscosity->SetValue(mfBuoyancyLinearViscosity, false);

	SetEngineViewportPositionAndSize(0,mpFB->GetSize());
	CreateGuiViewport(pBG);
	SetGuiViewportPos(cVector3f(2,55,0.1f));
	SetGuiViewportSize(cVector2f(pBG->GetSize() - cVector2f(4,58)));
	UpdateViewport();
	SetViewportActive(true);
	//mCamera.LockToGrid(false);

	mpCamera->SetCameraPosition(10);
	mpCamera->SetTargetPosition(0);
	mpCamera->UpdateEngineCamera();
}

//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------

bool cEdWindowPhysicsTest::OnViewportUpdate(const cGuiMessageData& aData)
{
	/*
#if defined(__linux__)
	mbViewMode = mbViewMode || pInput->GetKeyboard()->KeyIsDown(eKey_LeftSuper) || pInput->GetKeyboard()->KeyIsDown(eKey_Z);
#endif

	if(mbViewMode && mpSet->GetAttentionWidget()!=mpImgViewport)
	{
		mpPrevAttention = mpSet->GetAttentionWidget();
		mpSet->SetAttentionWidget(mpImgViewport);

		pInput->GetLowLevel()->LockInput(true);
	}
	else if(mbViewMode==false && mpSet->GetAttentionWidget()==mpImgViewport)
	{
		mpSet->SetAttentionWidget(mpPrevAttention);
		mpPrevAttention = NULL;

		pInput->GetLowLevel()->LockInput(false);

		mCamera.SetTumbleActive(false);
		mCamera.SetTrackActive(false);
		mCamera.SetZoomActive(false);
	}
	*/

	if(mBodyPicker.mpPickedBody)
	{
		//Get pos of start.
		mBodyPicker.mvPos = cMath::MatrixMul(mBodyPicker.mpPickedBody->GetLocalMatrix(), 
			mBodyPicker.mvLocalPos);

		//Get Drag pos
		const cVector3f& vStart = GetUnprojectedStart();
		const cVector3f& vDir = GetUnprojectedDir();
		cVector3f vDragPos =  vStart + vDir*mBodyPicker.mfDist;
		mvDragPos = vStart + vDir*mBodyPicker.mfDist;

		//Spring testing:
		cVector3f vForce = (vDragPos - mBodyPicker.mvPos)*20 - 
			(mBodyPicker.mpPickedBody->GetLinearVelocity()*0.4f);


		mBodyPicker.mpPickedBody->AddForceAtPosition(vForce *  mBodyPicker.mpPickedBody->GetMass(),
			mBodyPicker.mvPos);
	}

	/*
	cPhysicsBodyIterator itBodies = mpTestPhysicsWorld->GetBodyIterator();
	while(itBodies.HasNext())
	{
		iPhysicsBody* pBody = itBodies.Next();

		Log("Body %s matrix %s\n", pBody->GetName().c_str(), pBody->GetWorldMatrix().ToFileString().c_str());
	}
	*/

	return true;
}

//------------------------------------------------------------------------------------

bool cEdWindowPhysicsTest::OnViewportMouseDown(const cGuiMessageData& aData)
{
	cInput* pInput = mpEditor->GetEngine()->GetInput();

	cViewportClick click = cViewportClick(this, aData);

	//////////////////////////////////////////
	// Body picking
	if(mpCamera->GetCurrentController()->OnViewportMouseDown(click)==false)
	{
		if(click.mbLeft)
		{
			if(mBodyPicker.mpPickedBody==NULL)
			{
				const cVector3f& vStart = GetUnprojectedStart();
				const cVector3f& vDir = GetUnprojectedDir();
				
				cVector3f vEnd = vStart + vDir*100.0f;

				mBodyPicker.Clear();

				mpTestPhysicsWorld->CastRay(&mBodyPicker,vStart,vEnd,true,false,true);

				if(mBodyPicker.mpPickedBody)
				{
					mBodyPicker.mpPickedBody->SetAutoDisable(false);

					mvDragPos = mBodyPicker.mvPos;

					cMatrixf mtxInvWorld = cMath::MatrixInverse(mBodyPicker.mpPickedBody->GetLocalMatrix());
					mBodyPicker.mvLocalPos = cMath::MatrixMul(mtxInvWorld, mBodyPicker.mvPos);
				}
			}
		}
	}
	
	return true;
}

//------------------------------------------------------------------------------------

bool cEdWindowPhysicsTest::OnViewportMouseUp(const cGuiMessageData& aData)
{
	cViewportClick click = cViewportClick(this, aData);

	mpCamera->GetCurrentController()->OnViewportMouseUp(click);
	if(click.mbLeft)
	{
		if(mBodyPicker.mpPickedBody) mBodyPicker.mpPickedBody->SetAutoDisable(true);
		mBodyPicker.Clear();
	}
	
	return true;
}

//------------------------------------------------------------------------------------

bool cEdWindowPhysicsTest::OnViewportKeyPress(const cGuiMessageData& aData)
{
	return true;
}

//------------------------------------------------------------------------------------

bool cEdWindowPhysicsTest::OnViewportKeyRelease(const cGuiMessageData& aData)
{
	return true;
}

//------------------------------------------------------------------------------------

void cEdWindowPhysicsTest::OnPostSolidDraw(cRendererCallbackFunctions* apFunctions)
{
	if(mpTestEntity==NULL) return;

	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);

	apFunctions->SetProgram(NULL);
	apFunctions->SetTextureRange(NULL, 0);
	apFunctions->SetMatrix(NULL);

	if(mbDrawDebug)
	{
		mpTestPhysicsWorld->RenderDebugGeometry(apFunctions->GetLowLevelGfx(), cColor(1,1,1,1));
		for(size_t i=0;i<mvJoints.size(); ++i)
		{
			iPhysicsJoint *pJoint = mvJoints[i];
            
			cVector3f vPivot = pJoint->GetPivotPoint();
			apFunctions->GetLowLevelGfx()->DrawSphere(vPivot,0.2f,cColor(1,0,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(vPivot,vPivot + pJoint->GetPinDir()*0.25 ,cColor(0,1,0,1));
		}
	}

	if(mbDrawSkeleton && mpTestEntity->GetMesh()->GetSkeleton())
	{
		apFunctions->SetDepthTest(false);
		
		cNode3DIterator it = mpTestEntity->GetBoneStateRoot()->GetChildIterator();

		while(it.HasNext())
		{
			cNode3D *pChild = static_cast<cNode3D*>(it.Next());
			DrawSkeletonRec(apFunctions ,pChild);
		}
	}

	apFunctions->SetDepthTest(true);

	if(mBodyPicker.mpPickedBody)
	{
		apFunctions->GetLowLevelGfx()->DrawSphere(mBodyPicker.mvPos,0.1f, cColor(1,0,0,1));
		apFunctions->GetLowLevelGfx()->DrawSphere(mvDragPos,0.1f, cColor(1,0,0,1));

		apFunctions->GetLowLevelGfx()->DrawLine(mBodyPicker.mvPos, mvDragPos, cColor(1,1,1,1));
	}
}

//------------------------------------------------------------------------------------

void cEdWindowPhysicsTest::DrawSkeletonRec(cRendererCallbackFunctions* apFunctions, cNode3D* apBoneState)
{
	cVector3f vCameraSpacePos = cMath::MatrixMul(apFunctions->GetFrustum()->GetViewMatrix(), apBoneState->GetWorldPosition());		
	float fSize = cMath::Min(-0.01f * vCameraSpacePos.z, 0.03f);
	apFunctions->GetLowLevelGfx()->DrawSphere(apBoneState->GetWorldPosition(),fSize,cColor(1,0,0,1));
	
	
	cNode3DIterator it = apBoneState->GetChildIterator();
	while(it.HasNext())
	{
		cNode3D *pChild = static_cast<cNode3D*>(it.Next());
		apFunctions->GetLowLevelGfx()->DrawLine(apBoneState->GetWorldPosition(), pChild->GetWorldPosition(), cColor(1,1));
		DrawSkeletonRec(apFunctions, pChild);
	}
}

//------------------------------------------------------------------------------------

