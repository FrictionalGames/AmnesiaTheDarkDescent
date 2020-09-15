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

#include "ModelEditorWindowPhysicsTest.h"

#include "ModelEditor.h"
#include "ModelEditorWorld.h"

#include <algorithm>

//------------------------------------------------------------------------------------

/////////////////////////////////////
// Static members init
bool cModelEditorWindowPhysicsTest::mbBuoyancyActive = false;
float cModelEditorWindowPhysicsTest::mfBuoyancyDensity = 100;
float cModelEditorWindowPhysicsTest::mfBuoyancyLinearViscosity = 0.2f; 
float cModelEditorWindowPhysicsTest::mfBuoyancyAngularViscosity = 0.2f;
cPlanef cModelEditorWindowPhysicsTest::mBuoyancySurface = cPlanef(0,1,0,-1);

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
// RENDER CALLBACK
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

void cPhysicsTestRenderCallback::OnPostSolidDraw(cRendererCallbackFunctions* apFunctions)
{
	cMeshEntity* pEntity = mpWindow->mpTestEntity;

	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);

	apFunctions->SetProgram(NULL);
	apFunctions->SetTextureRange(NULL, 0);
	apFunctions->SetMatrix(NULL);

	if(mbDrawDebug)
	{
		mpWindow->mpTestPhysicsWorld->RenderDebugGeometry(apFunctions->GetLowLevelGfx(), cColor(1,1,1,1));
		std::vector<iPhysicsJoint*>& vJoints = mpWindow->mvJoints;

		for(size_t i=0;i<vJoints.size(); ++i)
		{
			iPhysicsJoint *pJoint = vJoints[i];
            
			cVector3f vPivot = pJoint->GetPivotPoint();
			apFunctions->GetLowLevelGfx()->DrawSphere(vPivot,0.2f,cColor(1,0,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(vPivot,vPivot + pJoint->GetPinDir()*0.25 ,cColor(0,1,0,1));
		}
	}

	if(mbDrawSkeleton && pEntity && pEntity->GetMesh()->GetSkeleton())
	{
		apFunctions->SetDepthTest(false);
		
		cNode3DIterator it = mpWindow->mpTestEntity->GetBoneStateRoot()->GetChildIterator();

		while(it.HasNext())
		{
			cNode3D *pChild = static_cast<cNode3D*>(it.Next());
			DrawSkeletonRec(apFunctions ,pChild);
		}
	}

	apFunctions->SetDepthTest(true);

	if(mpBodyPicker->mpPickedBody==NULL) return;

	apFunctions->GetLowLevelGfx()->DrawSphere(mpBodyPicker->mvPos,0.1f, cColor(1,0,0,1));
	apFunctions->GetLowLevelGfx()->DrawSphere(mvDragPos,0.1f, cColor(1,0,0,1));

	apFunctions->GetLowLevelGfx()->DrawLine(mpBodyPicker->mvPos, mvDragPos, cColor(1,1,1,1));
}

//------------------------------------------------------------------------------------

void cPhysicsTestRenderCallback::DrawSkeletonRec(cRendererCallbackFunctions* apFunctions, cNode3D* apBoneState)
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

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

cModelEditorWindowPhysicsTest::cModelEditorWindowPhysicsTest(iEditorBase* apEditor, iFrameBuffer* apFB): iEditorWindow(apEditor, 
																													   "Physics Test Window"),
																										 iEditorViewport(apEditor, 
																														 apEditor->GetEngine()->GetScene()->CreateWorld("PhysicsTestWorld"), 
																														 apFB)
{
	mpTestWorld = mpEngineViewport->GetWorld();
	mpTestEntity = NULL;
	mpMainLight = NULL;
	mpBackLight = NULL;

	mpPrevAttention = NULL;

	mbViewMode = false;

	mRenderCallback.mpBodyPicker = &mBodyPicker;
	mRenderCallback.mpWindow = this;
}

//------------------------------------------------------------------------------------

cModelEditorWindowPhysicsTest::~cModelEditorWindowPhysicsTest()
{
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

void cModelEditorWindowPhysicsTest::SetSelectedAnimation(int alX)
{
	if(mpInpAnimations)
		mpInpAnimations->SetValue(alX);
}

int cModelEditorWindowPhysicsTest::GetSelectedAnimation()
{
	return mpInpAnimations->GetValue();
}

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

bool cModelEditorWindowPhysicsTest::WindowSpecificInputCallback(iEditorInput* apInput)
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
			lAnim--;
			if(mpTestEntity)
				mpTestEntity->Play(lAnim, true,true);
		}
	}
	else if(apInput==mpInpShowDebug)
		mRenderCallback.mbDrawDebug = mpInpShowDebug->GetValue();
	else if(apInput==mpInpShowSkeleton)
		mRenderCallback.mbDrawSkeleton = mpInpShowSkeleton->GetValue();
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

	if(bBuoyancyUpdated)
		UpdateBuoyancy();
		

	return true;
}

//------------------------------------------------------------------------------------

void cModelEditorWindowPhysicsTest::SetUpRender()
{
	AddViewportCallback(&mRenderCallback);

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


	mCamera.SetCameraType(eEditorCameraType_Orbit);
	mCamera.SetCameraPosition(cVector3f(0,2,3));
	mCamera.SetTargetPosition(cVector3f(0));
	mCamera.SetZoom(-1, true);
	mCamera.UpdateCamera(0);
	mCamera.LockToGrid(false);

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
}

//------------------------------------------------------------------------------------

void cModelEditorWindowPhysicsTest::SetUpModel()
{
	mpInpAnimations->SetValue(0);

	cModelEditorWorld* pModel = (cModelEditorWorld*) mpEditor->GetEditorWorld();
	if(pModel->GetEntities().size()==0 || pModel->GetMesh()==NULL) return;

	tWString sFullPath = cString::To16Char(pModel->GetMeshFilename());
	cEngine* pEngine = mpEditor->GetEngine();
	cResources* pRes = pEngine->GetResources();
	iXmlDocument* pDoc = pRes->GetLowLevel()->CreateXmlDocument();
	pModel->Save(pDoc);

	cEditorEntityLoader* pLoader = mpEditor->GetEngineEntityLoader();
	
	mpTestEntity = (cMeshEntity*)pLoader->LoadEntityFromElement(0,
																"TestEntity",
																pDoc, 
																mpTestWorld,
																"",sFullPath, 
																true, true, true, true, true);
	mvJoints = pLoader->GetJoints();
	if(mpTestEntity==NULL)
		mpTestEntity->SetSourceFile("");
	mpTestEntity->SetVisible(true);

	// Populate animations list
	for(int i=0;i<mpTestEntity->GetAnimationStateNum();++i)
	{
		cAnimationState* pAnimState = mpTestEntity->GetAnimationState(i);
		
		mpInpAnimations->AddValue(cString::To16Char(pAnimState->GetName()));

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

	pRes->DestroyXmlDocument(pDoc);

	UpdateBuoyancy();
}

//------------------------------------------------------------------------------------

void cModelEditorWindowPhysicsTest::UpdateBuoyancy()
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

void cModelEditorWindowPhysicsTest::OnSetActive(bool abX)
{
	cScene* pScene = mpEditor->GetEngine()->GetScene();
	if(abX)
	{
		SetUpRender();
		SetUpModel();

 		pScene->SetCurrentListener(mpEngineViewport);
		mpInpAnimations->SetValue(((cModelEditor*)mpEditor)->GetLastSelectedAnimation());
	}
	else
	{
		pScene->SetCurrentListener(mpEditor->GetEngineViewport());
		((cModelEditor*)mpEditor)->SetLastSelectedAnimation(mpInpAnimations->GetValue());
	}
		

	SetViewportActive(abX);

	mpEditor->SetFlags(eEditorFlag_ViewportsDisabled, abX);
}

//------------------------------------------------------------------------------------

void cModelEditorWindowPhysicsTest::OnInitLayout()
{
	mpBGFrame->SetPosition(mpEditor->GetLayoutVec3f(eLayoutVec3_ViewportAreaPos));
	mpBGFrame->SetSize(mpEditor->GetLayoutVec2f(eLayoutVec2_ViewportAreaSize));
	mpBGFrame->SetClipActive(false);
	mpBGFrame->SetDrawFrame(true);

	float fUpperBorder = 5;
	cVector3f vPos = cVector3f(10,fUpperBorder,0.1f);

	mpInpShowDebug = CreateInputBool(vPos, _W("Show Physics Debug info"), "", mpBGFrame);
	vPos.y += mpInpShowDebug->GetSize().y + 3;
	mpInpShowSkeleton = CreateInputBool(vPos, _W("Show Skeleton"), "", mpBGFrame);

	vPos.y = fUpperBorder;
	vPos.x += cMath::Max(mpInpShowDebug->GetSize().x, mpInpShowSkeleton->GetSize().x) + 10;

	mpInpAnimations = CreateInputEnum(vPos-cVector3f(0,2,0),_W("Animation"),"", tWStringList(), mpBGFrame);
	mpInpAnimations->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
	mpInpAnimations->UpdateLayout();
	mpInpAnimations->AddValue(_W("None"));

	vPos = mpInpAnimations->GetPosition() + cVector3f(mpInpAnimations->GetSize().x + 20,0,0);
	mpInpMainLight = CreateInputBool(vPos, _W("Main Light"), "", mpBGFrame);
	vPos.y += mpInpMainLight->GetSize().y + 3;
	mpInpBackLight = CreateInputBool(vPos, _W("Back Light"), "", mpBGFrame);

	mpInpMainLight->SetValue(true, false);
	mpInpBackLight->SetValue(true, false);

	vPos.y = fUpperBorder;
	vPos.x = mpInpMainLight->GetPosition().x + mpInpMainLight->GetSize().x + 20;

	mpInpBuoyancyActive = CreateInputBool(vPos, _W("Buoyancy"), "", mpBGFrame);

	vPos.x += mpInpBuoyancyActive->GetSize().x+10;

	mpInpBuoyancyDensity = CreateInputNumber(vPos, _W("B.Density"), "", mpBGFrame,50,1);
	mpInpBuoyancyDensity->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
	mpInpBuoyancyDensity->SetTabWidth(55);
	mpInpBuoyancyDensity->UpdateLayout();
	mpInpBuoyancySurfaceHeight = CreateInputNumber(vPos + cVector3f(0, mpInpBuoyancyDensity->GetSize().y+5,0), _W("S.Height"), "", mpBGFrame,50,0.1f);
	mpInpBuoyancySurfaceHeight->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
	mpInpBuoyancySurfaceHeight->SetTabWidth(55);
	mpInpBuoyancySurfaceHeight->UpdateLayout();

	vPos.x = mpInpBuoyancyDensity->GetPosition().x + cMath::Max(mpInpBuoyancyDensity->GetSize().x, mpInpBuoyancySurfaceHeight->GetSize().x) + 10;

	mpInpBuoyancyAngularViscosity = CreateInputNumber(vPos, _W("B.A.viscosity"), "", mpBGFrame,50,0.1f);
	mpInpBuoyancyAngularViscosity->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
	mpInpBuoyancyAngularViscosity->UpdateLayout();
	mpInpBuoyancyLinearViscosity = CreateInputNumber(vPos + cVector3f(0, mpInpBuoyancyAngularViscosity->GetSize().y+5,0), _W("B.L.viscosity"), "", mpBGFrame,50,0.1f);
	mpInpBuoyancyLinearViscosity->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
	mpInpBuoyancyLinearViscosity->UpdateLayout();

	mpInpBuoyancyActive->SetValue(mbBuoyancyActive, false);
	mpInpBuoyancyDensity->SetValue(mfBuoyancyDensity, false);
	mpInpBuoyancySurfaceHeight->SetValue(-mBuoyancySurface.d, false);
	mpInpBuoyancyAngularViscosity->SetValue(mfBuoyancyAngularViscosity, false);
	mpInpBuoyancyLinearViscosity->SetValue(mfBuoyancyLinearViscosity, false);

	SetEngineViewportPositionAndSize(0,mpFB->GetSize());
	CreateGuiViewport(mpBGFrame);
	SetGuiViewportPos(cVector3f(2,55,0.1f));
	SetGuiViewportSize(cVector2f(mpBGFrame->GetSize() - cVector2f(4,58)));
	UpdateViewport();
	SetViewportActive(true);
	mCamera.LockToGrid(false);

}

//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------

bool cModelEditorWindowPhysicsTest::OnViewportUpdate(const cGuiMessageData& aData)
{
	cInput* pInput = mpEditor->GetEngine()->GetInput();
	mbViewMode = pInput->GetKeyboard()->KeyIsDown(eKey_LeftAlt);
#if defined(__linux__)
	mbViewMode = mbViewMode || pInput->GetKeyboard()->KeyIsDown(eKey_LeftSuper) || pInput->GetKeyboard()->KeyIsDown(eKey_Z);
#endif

	if(mbViewMode && mpSet->GetAttentionWidget()!=mpImgViewport)
	{
		mpPrevAttention = mpSet->GetAttentionWidget();
		mpSet->SetAttentionWidget(mpImgViewport);

		pInput->GetLowLevel()->LockInput(true);
		pInput->GetLowLevel()->RelativeMouse(true);
	}
	else if(mbViewMode==false && mpSet->GetAttentionWidget()==mpImgViewport)
	{
		mpSet->SetAttentionWidget(mpPrevAttention);
		mpPrevAttention = NULL;

		pInput->GetLowLevel()->LockInput(false);
		pInput->GetLowLevel()->RelativeMouse(false);

		mCamera.SetTumbleActive(false);
		mCamera.SetTrackActive(false);
		mCamera.SetZoomActive(false);
	}

	if(mBodyPicker.mpPickedBody)
	{
		//Get pos of start.
		mBodyPicker.mvPos = cMath::MatrixMul(mBodyPicker.mpPickedBody->GetLocalMatrix(), 
			mBodyPicker.mvLocalPos);

		//Get Drag pos
		const cVector3f& vStart = GetUnprojectedStart();
		const cVector3f& vDir = GetUnprojectedDir();
		cVector3f vDragPos =  vStart + vDir*mBodyPicker.mfDist;
		mRenderCallback.mvDragPos = vStart + vDir*mBodyPicker.mfDist;

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

bool cModelEditorWindowPhysicsTest::OnViewportMouseDown(const cGuiMessageData& aData)
{
	cInput* pInput = mpEditor->GetEngine()->GetInput();

	//////////////////////////////////////////
	// Body picking
	if(aData.mlVal & eGuiMouseButton_Left)
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

				mRenderCallback.mvDragPos = mBodyPicker.mvPos;

				cMatrixf mtxInvWorld = cMath::MatrixInverse(mBodyPicker.mpPickedBody->GetLocalMatrix());
				mBodyPicker.mvLocalPos = cMath::MatrixMul(mtxInvWorld, mBodyPicker.mvPos);
			}
		}
	}
	if((aData.mlVal&eGuiMouseButton_WheelUp) || (aData.mlVal&eGuiMouseButton_WheelDown))
	{
		cVector2f vZoom = 0;
		if(aData.mlVal & eGuiMouseButton_WheelUp)
			vZoom=-1;
		if(aData.mlVal & eGuiMouseButton_WheelDown)
			vZoom=1;

		if(vZoom!=0)
		{
			mCamera.Zoom(vZoom,true);
			mCamera.UpdateCamera(0);
		}
	}

	if(mbViewMode)
	{
		if(aData.mlVal&eGuiMouseButton_Left)
			mCamera.SetTumbleActive(true);
		if(aData.mlVal&eGuiMouseButton_Middle)
			mCamera.SetTrackActive(true);
		if(aData.mlVal&eGuiMouseButton_Right)
			mCamera.SetZoomActive(true);
	}
	
	return true;
}

//------------------------------------------------------------------------------------

bool cModelEditorWindowPhysicsTest::OnViewportMouseUp(const cGuiMessageData& aData)
{
	cInput* pInput = mpEditor->GetEngine()->GetInput();

	if(aData.mlVal & eGuiMouseButton_Left)
	{
		if(mBodyPicker.mpPickedBody) mBodyPicker.mpPickedBody->SetAutoDisable(true);
		mBodyPicker.Clear();
	}

	pInput->GetLowLevel()->LockInput(false);
	pInput->GetLowLevel()->RelativeMouse(false);

	if(mbViewMode)
	{
		if(aData.mlVal&eGuiMouseButton_Left)
			mCamera.SetTumbleActive(false);
		if(aData.mlVal&eGuiMouseButton_Middle)
			mCamera.SetTrackActive(false);
		if(aData.mlVal&eGuiMouseButton_Right)
			mCamera.SetZoomActive(false);
	}
	
	return true;
}

//------------------------------------------------------------------------------------

bool cModelEditorWindowPhysicsTest::OnViewportKeyPress(const cGuiMessageData& aData)
{

	return true;
}

//------------------------------------------------------------------------------------

bool cModelEditorWindowPhysicsTest::OnViewportKeyRelease(const cGuiMessageData& aData)
{
	return true;
}

//------------------------------------------------------------------------------------
