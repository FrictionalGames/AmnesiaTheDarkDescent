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

#include "stdafx.h"
#include "SimpleCamera.h"

cSimpleCamera::cSimpleCamera(const tString& asAppName,cEngine *apGame, cWorld *apWorld, float afSpeed,cVector3f avStartPos,bool abShowFPS, bool abLoadSkin)
: iUpdateable("SimpleCamera")
{
	msAppName = asAppName;

	mpEngine = apGame;
	mfSpeed = afSpeed;

	///////////////////////////////////////////
	//Add actions
	mpEngine->GetInput()->CreateAction("Escape",0)->AddKey(eKey_Escape);
	
	mpEngine->GetInput()->CreateAction("Forward",1)->AddKey(eKey_W);
	mpEngine->GetInput()->CreateAction("Backward",2)->AddKey(eKey_S);
	mpEngine->GetInput()->CreateAction("Right",3)->AddKey(eKey_D);
	mpEngine->GetInput()->CreateAction("Left",4)->AddKey(eKey_A);
	mpEngine->GetInput()->CreateAction("Up",5)->AddKey(eKey_PageUp);
	mpEngine->GetInput()->CreateAction("Down",6)->AddKey(eKey_PageDown);
	mpEngine->GetInput()->CreateAction("Alt")->AddKey(eKey_LeftAlt);
	mpEngine->GetInput()->CreateAction("PanLeft",7)->AddKey(eKey_Q);
	mpEngine->GetInput()->CreateAction("PanRight",8)->AddKey(eKey_E);

	mpEngine->GetInput()->CreateAction("LeftMouse")->AddMouseButton(eMouseButton_Left);
	mpEngine->GetInput()->CreateAction("RightMouse")->AddMouseButton(eMouseButton_Right);
	mpEngine->GetInput()->CreateAction("MiddleMouse")->AddMouseButton(eMouseButton_Middle);

	mpEngine->GetInput()->CreateAction("WheelUp",7)->AddMouseButton(eMouseButton_WheelUp);
	mpEngine->GetInput()->CreateAction("WheelDown",8)->AddMouseButton(eMouseButton_WheelDown);

	mpEngine->GetInput()->CreateAction("ScreenShot")->AddKey(eKey_F8);
	mpEngine->GetInput()->CreateAction("PrintInfo")->AddKey(eKey_P);
	mpEngine->GetInput()->CreateAction("ListContainerInfo")->AddKey(eKey_L);
	//mpEngine->GetInput()->CreateAction("MouseMode")->AddKey(eKey_Space);

	mpEngine->GetInput()->GetLowLevel()->LockInput(true);
	mpEngine->GetInput()->GetLowLevel()->RelativeMouse(true);

	//mpEngine->GetGraphics()->GetLowLevel()->ShowCursor(true);
	
	///////////////////////////////////////////
	// Create Camera

	mpCamera = mpEngine->GetScene()->CreateCamera(eCameraMoveMode_Fly);
	mpCamera->SetPosition(avStartPos);
	
	mpCamera->SetFarClipPlane(1000);
	mpCamera->SetNearClipPlane(0.05f);
	//mpCamera->SetProjectionType(eProjectionType_Orthographic);
	cVector2f temp = cVector2f(4,4);
	mpCamera->SetOrthoViewSize(temp);
	//mpCamera->SetNearClipPlane(0.0001f);


	///////////////////////////////////////////
	// Create Viewport
	
	mpViewport = mpEngine->GetScene()->CreateViewport(mpCamera, apWorld);
	//mpViewport = mpEngine->GetScene()->CreateViewport(NULL, NULL);

	//mpViewport->SetRenderer(mpEngine->GetGraphics()->GetRenderer(eRenderer_WireFrame));
	//mpViewport->GetRenderSettings()->mClearColor = cColor(0.7f,1);

	mpEngine->GetScene()->SetCurrentListener(mpViewport);
	
	///////////////////////////////////////////
	//Gui and font
	mpGui = mpEngine->GetGui();
	if(abLoadSkin)
	{
		mpGuiSkin = mpGui->CreateSkin("gui_default.skin");
		mpGuiSet = mpGui->CreateSet("Text", mpGuiSkin);
	}
	else
	{
		mpGuiSet = mpGui->CreateSet("Text", NULL);
	}
	mpViewport->AddGuiSet(mpGuiSet);
	mpGui->SetFocus(mpGuiSet);
	mpGuiSet->SetDrawMouse(false);
	
	//Debug:
	//mpGui->SetFocus(mpGuiSet);
	//mpGuiSet->SetDrawMouse(true);
	mpTestGfx = mpGui->CreateGfxFilledRect(cColor(0,0,0,1),eGuiMaterial_Diffuse);


	/*cMatrixf mtxTransform = cMath::MatrixRotate(cVector3f(0,0,0),eEulerRotationOrder_XYZ);
	mtxTransform.SetTranslation(cVector3f(0,3.01f,0));
	mpGuiSet->Set3DTransform(mtxTransform);
	mpGuiSet->Set3DSize(cVector3f(3,2.25f,0.1f));
	mpGuiSet->SetIs3D(true);*/


	/////////////////////////////////
	// Font setup
	if(abShowFPS)
		mpFont = mpEngine->GetResources()->GetFontManager()->CreateFontData("viewer.fnt",12,32,128);
	else
		mpFont = NULL;

	/////////////////////////////////
	// Var setup
	mvMouseCameraAngles = cVector3f(cMath::ToRad(-25),0,0);
	mfMouseCameraDistance = 9.0;
	mvMouseCameraPos = 0;

	mbActive = true;
	mbMouseActive = false;
	mbCameraInMouseMode = true;
	mbMatrixFlyCamera = false;

	mbDrawFPS = true;

	mfTakeFrameTimeCount = 2;
	mfAvgFrameTime =0;

	m_mtxLastView = mpCamera->GetViewMatrix();
}

//-----------------------------------------------------------------------

cSimpleCamera::~cSimpleCamera()
{

}

//-----------------------------------------------------------------------

static tString gsSpaceTemp;
static const char* GetSpaces(int alLevel)
{
	gsSpaceTemp = "";
    for(int i=0; i<alLevel; ++i) gsSpaceTemp += "   ";
	return gsSpaceTemp.c_str();
}

void cSimpleCamera::ListContainerNodeData(iRenderableContainerNode *apNode, int alLevel)
{
	cVector3f vSize = apNode->GetMax() - apNode->GetMin();
	cVector3f vPos = (apNode->GetMax() + apNode->GetMin()) *0.5f;
	Log("%s--- Node: %d Size: (%s) Pos: (%s)---\n", GetSpaces(alLevel), apNode, vSize.ToString().c_str(), vPos.ToString().c_str());

	/////////////////////////////
	//Iterate objects
	if(apNode->HasObjects())
	{
		tRenderableListIt it = apNode->GetObjectList()->begin();
		for(; it != apNode->GetObjectList()->end(); ++it)
		{
			iRenderable *pObject = *it;
			//if(pObject->GetRenderType() != eRenderableType_ParticleEmitter) continue;
			Log("%s  '%s' Size: (%s) Pos: (%s)\n", GetSpaces(alLevel), pObject->GetName().c_str(), pObject->GetBoundingVolume()->GetSize().ToString().c_str(), 
													pObject->GetBoundingVolume()->GetWorldCenter().ToString().c_str());
		}
	}

	////////////////////////
	//Iterate children
	if(apNode->HasChildNodes())
	{
		tRenderableContainerNodeListIt childIt = apNode->GetChildNodeList()->begin();
		for(; childIt != apNode->GetChildNodeList()->end(); ++childIt)
		{
			iRenderableContainerNode *pChildNode = *childIt;
			ListContainerNodeData(pChildNode, alLevel+1);
		}
	}

}

void cSimpleCamera::Update(float afFrameTime)
{
	m_mtxLastView = mpCamera->GetViewMatrix();

	if(mpEngine->GetInput()->BecameTriggerd("Escape"))
	{
		mpEngine->Exit();
	}
	
	mfTakeFrameTimeCount -= afFrameTime;
	if(mfTakeFrameTimeCount <0)
	{
		mfTakeFrameTimeCount = 1;
		mlstFrameTimes.push_back(mpEngine->GetAvgFrameTimeInMS());
		
		double fTotalFrame =0;
		if(mlstFrameTimes.size() > 10) mlstFrameTimes.pop_front();
		std::list<float>::iterator it = mlstFrameTimes.begin();
		for(; it != mlstFrameTimes.end(); ++it)
		{
			fTotalFrame += *it;
		}
		mfAvgFrameTime = (float)(fTotalFrame / (double)mlstFrameTimes.size());
	}
	

	//Not really needed.
	/*if(mpEngine->GetInput()->BecameTriggerd("MouseMode"))
	{
		SetMouseMode(!mbMouseActive);
	}*/

	if(mpEngine->GetInput()->BecameTriggerd("ScreenShot"))
	{
		tWString sFileName = _W("");
		int lCount = 0;
		
		do{
			sFileName = _W("Screen_")+cString::To16Char(msAppName)+_W("_");
			if(lCount >= 100)		sFileName += _W("")+cString::ToStringW(lCount);
			else if(lCount >= 10)	sFileName += _W("0")+cString::ToStringW(lCount);
			else					sFileName += _W("00")+cString::ToStringW(lCount);

			sFileName += _W(".jpg");
			++lCount;

		}
		while(cPlatform::FileExists(sFileName));
		
		cBitmap *pBmp = mpEngine->GetGraphics()->GetLowLevel()->CopyFrameBufferToBitmap();
		mpEngine->GetResources()->GetBitmapLoaderHandler()->SaveBitmap(pBmp,sFileName,0);
		hplDelete(pBmp);
	}

	if(mpEngine->GetInput()->BecameTriggerd("PrintInfo"))
	{
		//Log("Pos: %s Pitch: %f Yaw: %f\n", pCam->GetPosition().ToString().c_str(), pCam->GetPitch(),pCam->GetYaw());
		cVector3f vPos = mpCamera->GetPosition();
		Log("---- Camera Information -----------\n");
		Log("Position(cVector3f(%f, %f, %f))\n",vPos.x, vPos.y, vPos.z);
		Log("Pitch(%f)\n", mpCamera->GetPitch());
		Log("Yaw(%f)\n", mpCamera->GetYaw());
		Log("-----------------------------------\n");

		mpViewport->GetRenderSettings()->mbLog =true;
	}

	if(mpEngine->GetInput()->BecameTriggerd("ListContainerInfo"))
	{
		Log("---------- DYNAMIC CONTAINER BEGIN --------------\n");
		ListContainerNodeData(mpViewport->GetWorld()->GetRenderableContainer(eWorldContainerType_Dynamic)->GetRoot(), 0);
		Log("---------- DYNAMIC CONTAINER END --------------\n");
	}

	if(mbActive== false) return;

	/////////////////////////
	// Mouse mode
	if(mbMouseActive)
	{
		bool bAltDown = mpEngine->GetInput()->IsTriggerd("Alt");

		cVector2l vAbsRel = mpEngine->GetInput()->GetMouse()->GetRelPosition();
		cVector2f vRel = cVector2f((float)vAbsRel.x,(float)vAbsRel.y) / mpEngine->GetGraphics()->GetLowLevel()->GetScreenSizeFloat();
		
		if(mbCameraInMouseMode)
		{
			if(mpCamera->GetUp().y < 0)	vRel.x = -vRel.x;
			
			//////////////////////
			// Camera Rotation
			if( (mpEngine->GetInput()->IsTriggerd("RightMouse") && !bAltDown) ||
				(mpEngine->GetInput()->IsTriggerd("LeftMouse") && bAltDown) )
			{
				mvMouseCameraAngles.y -= vRel.x * 4.0f;
				mvMouseCameraAngles.x -= vRel.y * 4.0f;
				//if(mvMouseCameraAngles.x > kPi2f) mvMouseCameraAngles.x = kPi2f;
				//if(mvMouseCameraAngles.x < -kPi2f) mvMouseCameraAngles.x = -kPi2f;
			}

			//////////////////////
			// Camera Zoom
			if(mpGuiSet->GetFocusedWidget()==NULL)
			{
				if(mpEngine->GetInput()->BecameTriggerd("WheelUp"))		mfMouseCameraDistance -= mfMouseCameraDistance *0.2f;
				if(mpEngine->GetInput()->BecameTriggerd("WheelDown"))	mfMouseCameraDistance += mfMouseCameraDistance *0.2f;
			}

			if( (bAltDown && mpEngine->GetInput()->IsTriggerd("RightMouse")) ||
				(!bAltDown && mpEngine->GetInput()->IsTriggerd("MiddleMouse")) )
			{
				mfMouseCameraDistance += vRel.y * mfMouseCameraDistance * 3.2f;
			}
		
			

			if(mfMouseCameraDistance > 100) mfMouseCameraDistance = 100;
			if(mfMouseCameraDistance < 0.01f) mfMouseCameraDistance = 0.01f;

			//////////////////////
			// Camera position
			cVector3f vForward = mpCamera->GetForward();
			vForward.y=0; vForward.Normalize();
			cVector3f vRight = mpCamera->GetRight();
			vRight.y=0; vRight.Normalize();
			float fSpeed = 2.0f * afFrameTime * mfMouseCameraDistance;
			cVector3f vUp = cVector3f(0,1,0);

			if(bAltDown && mpEngine->GetInput()->IsTriggerd("MiddleMouse"))
			{
				cVector3f vRightMove = vRight;vRightMove.y=0;
				cVector3f vFwdMove = vForward;vFwdMove.y=0;

				mvMouseCameraPos += vRightMove * fSpeed * vRel.x * -30;
				mvMouseCameraPos += vFwdMove * fSpeed * vRel.y * 30;
			}

			if(mpEngine->GetInput()->IsTriggerd(1)) mvMouseCameraPos += vForward * fSpeed;
			if(mpEngine->GetInput()->IsTriggerd(2)) mvMouseCameraPos -= vForward * fSpeed;
			if(mpEngine->GetInput()->IsTriggerd(3)) mvMouseCameraPos += vRight * fSpeed;
			if(mpEngine->GetInput()->IsTriggerd(4)) mvMouseCameraPos -= vRight * fSpeed;;
			if(mpEngine->GetInput()->IsTriggerd(5)) mvMouseCameraPos += vUp * fSpeed;
			if(mpEngine->GetInput()->IsTriggerd(6)) mvMouseCameraPos -= vUp * fSpeed;

			//////////////////////////////////////
			// Set Camera properties
			CalculateMouseCamera();
		}

		/////////////////////
		//GUI Input
		mpGui->SendMousePos(mpEngine->GetInput()->GetMouse()->GetAbsPosition(),
							mpEngine->GetInput()->GetMouse()->GetRelPosition());


		
		while(mpEngine->GetInput()->GetKeyboard()->KeyIsPressed())
			mpGui->SendKeyPress(mpEngine->GetInput()->GetKeyboard()->GetKey());
		
		if(!bAltDown)
		{
			if(mpEngine->GetInput()->BecameTriggerd("LeftMouse"))
				mpGui->SendMouseClickDown(eGuiMouseButton_Left);
			if(mpEngine->GetInput()->WasTriggerd("LeftMouse"))
				mpGui->SendMouseClickUp(eGuiMouseButton_Left);
			if(mpEngine->GetInput()->DoubleTriggerd("LeftMouse", 0.3f))
				mpGui->SendMouseDoubleClick(eGuiMouseButton_Left);
			
			if(mpEngine->GetInput()->BecameTriggerd("WheelUp"))
				mpGui->SendMouseClickDown(eGuiMouseButton_WheelUp);
			if(mpEngine->GetInput()->BecameTriggerd("WheelDown"))
				mpGui->SendMouseClickDown(eGuiMouseButton_WheelDown);
		}
		
		return;
	}
	/////////////////////////
	// Normal mode
	else
	{
		float fMul = mpEngine->GetStepSize();	
		
		if(mbMatrixFlyCamera)
		{
			cVector3f vCamPos = mpCamera->GetPosition();
			cMatrixf mtxCamRotate = mpCamera->GetRotationMatrix();
			
			cVector3f vRight = mtxCamRotate.GetRight();
			cVector3f vUp = mtxCamRotate.GetUp();
			cVector3f vFwd = mtxCamRotate.GetForward()*-1;
			
			////////////////////
			//Position
			cVector3f vAdd(0);
			if(mpEngine->GetInput()->IsTriggerd(1)) vAdd.z += mfSpeed * fMul;
			if(mpEngine->GetInput()->IsTriggerd(2)) vAdd.z += -mfSpeed* fMul;
			if(mpEngine->GetInput()->IsTriggerd(3)) vAdd.x += mfSpeed * fMul;
			if(mpEngine->GetInput()->IsTriggerd(4)) vAdd.x += -mfSpeed * fMul;
			if(mpEngine->GetInput()->IsTriggerd(5)) vAdd.y += mfSpeed * fMul;
			if(mpEngine->GetInput()->IsTriggerd(6)) vAdd.y += -mfSpeed * fMul;

			vCamPos += vRight*vAdd.x + vUp*vAdd.y + vFwd*vAdd.z;

			/////////////////////
			//Rotation
			cVector3f vRot(0);
			if(mpEngine->GetInput()->IsTriggerd(7)) vRot.z += 1.2 * fMul;
			if(mpEngine->GetInput()->IsTriggerd(8)) vRot.z += -1.2 * fMul;
			
			cVector2l vAbsRel = mpEngine->GetInput()->GetMouse()->GetRelPosition();
			cVector2f vRel = cVector2f((float)vAbsRel.x,(float)vAbsRel.y) / mpEngine->GetGraphics()->GetLowLevel()->GetScreenSizeFloat();
			vRot.y += vRel.x * 1.7f;
			vRot.x += vRel.y * 1.7f;


			cVector3f vRotateAxis = vRight*vRot.x + vUp*vRot.y + vFwd*vRot.z;
			float fRotateLength = vRotateAxis.Length();
			vRotateAxis.Normalize();
			
			cQuaternion qRot; qRot.FromAngleAxis(fRotateLength, vRotateAxis);
			mtxCamRotate = cMath::MatrixMul(mtxCamRotate, cMath::MatrixQuaternion(qRot)); //Needs to be reversed order because this matrix is an inverse.
			
			/////////////////////
			//Update camera
			mpCamera->SetRotationMatrix(mtxCamRotate);
			mpCamera->SetPosition(vCamPos);
		}
		else
		{
			if(mpEngine->GetInput()->IsTriggerd(1)) mpCamera->MoveForward(mfSpeed * fMul);
			if(mpEngine->GetInput()->IsTriggerd(2)) mpCamera->MoveForward(-mfSpeed* fMul);
			if(mpEngine->GetInput()->IsTriggerd(3)) mpCamera->MoveRight(mfSpeed * fMul);
			if(mpEngine->GetInput()->IsTriggerd(4)) mpCamera->MoveRight(-mfSpeed * fMul);
			if(mpEngine->GetInput()->IsTriggerd(5)) mpCamera->MoveUp(mfSpeed * fMul);
			if(mpEngine->GetInput()->IsTriggerd(6)) mpCamera->MoveUp(-mfSpeed * fMul);
			if(mpEngine->GetInput()->IsTriggerd(7)) mpCamera->AddRoll(1.2 * fMul);
			if(mpEngine->GetInput()->IsTriggerd(8)) mpCamera->AddRoll(-1.2 * fMul);

			cVector2l vAbsRel = mpEngine->GetInput()->GetMouse()->GetRelPosition();
			cVector2f vRel = cVector2f((float)vAbsRel.x,(float)vAbsRel.y) / mpEngine->GetGraphics()->GetLowLevel()->GetScreenSizeFloat();
			mpCamera->AddYaw(-vRel.x * 1.7f);
			mpCamera->AddPitch(-vRel.y * 1.7f);
		}
	}
}

//-----------------------------------------------------------------------

void cSimpleCamera::OnDraw(float afFrameTime)
{
	if(mpFont && mbDrawFPS)
	{
		mpGuiSet->DrawFont(mpFont, cVector3f(5,5,5),12, cColor(1,1), _W("AvgFrame: %f  FPS: %f Avg: %f"), mpEngine->GetAvgFrameTimeInMS(),mpEngine->GetFPS(), mfAvgFrameTime);
	}
}

void cSimpleCamera::OnPostRender(float afFrameTime)
{
	mpViewport->GetRenderSettings()->mbLog =false;
}

//-----------------------------------------------------------------------

void cSimpleCamera::AppGotMouseFocus()
{
	if(mbMouseActive) mpGuiSet->SetDrawMouse(true);
}

void cSimpleCamera::AppLostMouseFocus()
{
	if(mbMouseActive) mpGuiSet->SetDrawMouse(false);
}
//-----------------------------------------------------------------------

void cSimpleCamera::SetMouseMode(bool abActive)
{
	mbMouseActive = abActive;

	mpGuiSet->SetDrawMouse(mbMouseActive);

	if(mbMouseActive == true && mbCameraInMouseMode)
	{
		mvMouseCameraPos = mpCamera->GetPosition() + mpCamera->GetForward() * 9.0f;
		mfMouseCameraDistance = 9.0f;
		
		mvMouseCameraAngles.x = mpCamera->GetPitch();
		mvMouseCameraAngles.y = mpCamera->GetYaw();

		mpCamera->SetRotateMode(eCameraRotateMode_Matrix);
	}
	else
	{
		if(mbMatrixFlyCamera)
			mpCamera->SetRotateMode(eCameraRotateMode_Matrix);
		else
			mpCamera->SetRotateMode(eCameraRotateMode_EulerAngles);
	}

	if(mbMouseActive && mbCameraInMouseMode) CalculateMouseCamera();
}

void cSimpleCamera::SetMatrixFlyCamera(bool abX)
{
	mbMatrixFlyCamera = abX;

	if(mbMouseActive==false && mbMatrixFlyCamera)
	{
		mpCamera->SetRotateMode(eCameraRotateMode_Matrix);
	}
	
}

//-----------------------------------------------------------------------

void cSimpleCamera::CalculateMouseCamera()
{
	cVector3f vCameraPos(0,0,mfMouseCameraDistance);
	cMatrixf mtxRotation = cMath::MatrixRotate(mvMouseCameraAngles, eEulerRotationOrder_XYZ);
	vCameraPos = cMath::MatrixMul(mtxRotation,vCameraPos);

	//LUis algo:
	/*cVector3f vExtraPos(0,0,mfMouseCameraDistance);
	cVector3f vExtraRotate = mvMouseCameraAngles;
	vExtraRotate.x = 0.1f;
	mtxRotation = cMath::MatrixRotate(vExtraRotate, eEulerRotationOrder_XYZ);
	vExtraPos = cMath::MatrixMul(mtxRotation,vExtraPos);

	cMatrixf mtxView = cMatrixf::Identity;

	cVector3f vForward = cVector3f(0,0,1);
	cVector3f vUp = cVector3f(0,1,0);
	cVector3f vRight = cVector3f(1,0,0);

	vForward = cMath::Vector3Normalize(vCameraPos);
	cVector3f vHelper = cMath::Vector3Normalize(vExtraPos);
	
	vRight = cMath::Vector3Cross(cVector3f(0,1,0), vHelper);
	vRight.Normalise();

	vUp = cMath::Vector3Cross(vForward, vRight);
	vUp.Normalise();
	
	mpCamera->SetPosition(mvMouseCameraPos + vCameraPos);
	mpCamera->SetUp(vUp);
	mpCamera->SetRight(vRight);
	mpCamera->SetForward(vForward);*/
	
	cMatrixf mtxCamRotation = cMath::MatrixRotate(mvMouseCameraAngles*-1 , eEulerRotationOrder_ZYX);
	mpCamera->SetRotationMatrix(mtxCamRotation);
	mpCamera->SetPosition(mvMouseCameraPos + vCameraPos);
		
	/*mpCamera->SetPosition(mvMouseCameraPos + vCameraPos);
	mpCamera->SetPitch(mvMouseCameraAngles.x);
	mpCamera->SetYaw(mvMouseCameraAngles.y);*/
}
//-----------------------------------------------------------------------



