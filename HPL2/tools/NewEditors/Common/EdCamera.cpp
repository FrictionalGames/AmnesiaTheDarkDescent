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

#include "../Common/EdCamera.h"

#include "../Common/EdViewport.h"

//--------------------------------------------------------------------------------

iCamController::iCamController(cEdCamera* apCam, const tWString& asName) : mpCam(apCam), msName(asName)
{
	mbForceUpdate = false;
}

//////////////////////////////////////////////////////////////////
// CAM CONTROLLER INTERFACE - PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void iCamController::Update(const cViewportClick& aClick)
{
	if(IsActive() || mbForceUpdate)
	{
		mbForceUpdate = false;

		OnUpdate(aClick);

		mpCam->UpdateEngineCamera();
	}
}

//--------------------------------------------------------------------------------

bool iCamController::LoadState(cXmlElement* apStateElem)
{
	LockToGrid(apStateElem->GetAttributeBool("LockToGrid", false));

	return true;
}

bool iCamController::SaveState(cXmlElement* apStateElem)
{
	apStateElem->SetAttributeBool("LockToGrid", IsLockedToGrid());

	return true;
}


//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM MODE - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

iCamMode::iCamMode(cEdCamera* apCam, int alID, const tWString& asName)
{
	mpCam = apCam;
	mlID = alID;
	msName = asName;

	mpCurrentController = NULL;
	mpCurrentRenderer = NULL;
}

iCamMode::~iCamMode()
{
	STLDeleteAll(mvControllers);
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM MODE - PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void iCamMode::SetActive(bool abX)
{
	OnSetActive(abX);
	if(abX)
	{
		mpCam->UpdateEngineCamera();
		mpCam->GetViewport()->OnSetCamMode(this);
	}
}

//--------------------------------------------------------------------------------

void iCamMode::AddCamController(iCamController* apController)
{
	if(apController==NULL) return;

	if(mpCurrentController == NULL)
		mpCurrentController = apController;

	mvControllers.push_back(apController);
}

//--------------------------------------------------------------------------------

void iCamMode::SetController(int alX)
{
	iCamController* pController = GetController(alX);

	if(pController==NULL || mpCurrentController==pController ) return;

	mpCurrentController = pController;
}

void iCamMode::SetController(const tWString& asName)
{
	for(int i=0; i<GetNumControllers(); ++i)
	{
		iCamController* pController = GetController(i);
		if(pController->GetName()==asName)
		{
			SetController(i);
			break;
		}
	}
}

//--------------------------------------------------------------------------------

int iCamMode::GetNumControllers()
{
	return (int) mvControllers.size();
}

//--------------------------------------------------------------------------------

iCamController* iCamMode::GetController(int alX)
{
	if(alX<0 || alX >=GetNumControllers())
		return NULL;

	return mvControllers[alX];
}

//--------------------------------------------------------------------------------

void iCamMode::AddRenderer(const tWString& asName, eRenderer aX)
{
	mvRenderers.push_back(cEdCamRenderer(asName, aX));
}

//--------------------------------------------------------------------------------

void iCamMode::SetCurrentRenderer(int alX)
{
	cEdCamRenderer* pRenderer = GetRenderer(alX);

	if(pRenderer)
	{
		mpCurrentRenderer = pRenderer;
		mpCam->GetViewport()->SetRenderMode(pRenderer->mRenderer);
	}
}

void iCamMode::SetCurrentRenderer(const tWString& asName)
{
	for(int i=0; i<GetNumRenderers(); ++i)
	{
		cEdCamRenderer* pRenderer = GetRenderer(i);
		if(pRenderer->msName==asName)
		{
			SetCurrentRenderer(i);
			break;
		}
	}
}

//--------------------------------------------------------------------------------

int iCamMode::GetNumRenderers()
{
	return (int) mvRenderers.size();
}

//--------------------------------------------------------------------------------

cEdCamRenderer* iCamMode::GetRenderer(int alX)
{
	if(alX<0 || alX >=GetNumRenderers())
		return NULL;

	return &mvRenderers[alX];
}

//--------------------------------------------------------------------------------

bool iCamMode::LoadState(cXmlElement* apStateElem)
{
	SetCurrentRenderer(cString::To16Char(apStateElem->GetAttributeString("Renderer")));
	SetController(cString::To16Char(apStateElem->GetAttributeString("Controller")));

	if(mpCurrentController)	mpCurrentController->LoadState(apStateElem);

	return true;
}

bool iCamMode::SaveState(cXmlElement* apStateElem)
{
	apStateElem->SetAttributeString("Renderer", cString::To8Char(mpCurrentRenderer->msName));
	if(mpCurrentController)
	{
		apStateElem->SetAttributeString("Controller", cString::To8Char(mpCurrentController->GetName()));
		mpCurrentController->SaveState(apStateElem);
	}

	return true;
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// VIEWPORT CAMERA - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

cEdCamera::cEdCamera(iEdViewport* apViewport)
{
	mpViewport = apViewport;

	mpEngineCamera = mpViewport->mpEngine->GetScene()->CreateCamera(eCameraMoveMode_Fly);
	mpEngineCamera->SetPitchLimits(0,0);
	mpEngineCamera->SetRotateMode(eCameraRotateMode_Matrix);

	mfDistToTarget = 0;
	mvCameraSphCoords = 0;
	mvTargetSphCoords = 0;
    
	mvTargetPosition = 0;
	mvCameraPosition = 0;

	mbCameraUpdated=true;

	FetchSettings();

	mfTrackUnprojectDist = 10.0f;

	mpCurrentMode = NULL;

	mbKeepOrientation = false;

	mbCameraPosNeedsUpdate = false;
	mbCameraPosUpdated = false;
	mbCameraSphCoordsNeedUpdate = false;
	mbTargetPosNeedsUpdate = false;
	mbTargetPosUpdated = false;
	mbTargetSphCoordsNeedUpdate = false;
}

//-------------------------------------------------------------

cEdCamera::~cEdCamera()
{
	STLDeleteAll(mvModes);
	//if(mpCurrentZoomFunc) hplDelete(mpCurrentZoomFunc);
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// VIEWPORT CAMERA - PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cEdCamera::SetCameraMode(int alMode)
{
	for(int i=0; i<GetNumModes(); ++i)
	{
		iCamMode* pMode = GetMode(i);
	
		if(pMode->GetID()==alMode)
		{
			SetCameraMode(pMode);
			break;
		}
	}
}

//-------------------------------------------------------------

void cEdCamera::SetCameraMode(const tWString& asName)
{
	for(int i=0; i<GetNumModes(); ++i)
	{
		iCamMode* pMode = GetMode(i);

		if(pMode->GetName()==asName)
		{
			SetCameraMode(pMode);
			break;
		}
	}
}

//-------------------------------------------------------------

void cEdCamera::SetCameraMode(iCamMode* apMode)
{
	if(mpCurrentMode)
		mpCurrentMode->SetActive(false);

	mpCurrentMode = apMode;

	if(mpCurrentMode)
		mpCurrentMode->SetActive(true);
}

//-------------------------------------------------------------

int cEdCamera::GetNumModes()
{
	return (int)mvModes.size();
}

iCamMode* cEdCamera::GetMode(int alX)
{
	if(alX<0 || alX>=GetNumModes())
		return NULL;

	return mvModes[alX];
}

//-------------------------------------------------------------

iCamController* cEdCamera::GetCurrentController()
{
	return (mpCurrentMode)? 
				mpCurrentMode->GetCurrentController() : 
				NULL;
}

//-------------------------------------------------------------

bool cEdCamera::IsOrtho()
{
	return mpEngineCamera->GetProjectionType()==eProjectionType_Orthographic;
}

//-------------------------------------------------------------

/*void cEdViewportCamera::LockToGrid(bool abX)
{
	if(mbLockToGrid==abX)
		return;

	mbLockToGrid = abX;
	UpdateCamera(0);
}
*/
//-------------------------------------------------------------

void cEdCamera::SetOrientation(const cVector3f& avRight, const cVector3f& avUp, const cVector3f& avForward)
{
	mvRight = avRight;
	mvUp = avUp;
	mvForward = avForward;
}

//-------------------------------------------------------------

void cEdCamera::SetCameraPosition(const cVector3f& avPosition)
{
	if(mvCameraPosition == avPosition)
		return;

	mvCameraPosition = avPosition;

	mbCameraSphCoordsNeedUpdate = true;
	mbCameraPosUpdated = true;
	mbDistToTargetNeedsUpdate = true;
	mbCameraUpdated = true;
}

//-------------------------------------------------------------

void cEdCamera::SetCameraSphCoords(const cVector2f& avSphCoords)
{
	if(mvCameraSphCoords==avSphCoords) return;

	mvCameraSphCoords = avSphCoords;
	
	mbCameraPosNeedsUpdate = true;
	mbCameraUpdated = true;
}

//-------------------------------------------------------------

void cEdCamera::SetTargetPosition(const cVector3f& avTargetPos)
{
	if(mvTargetPosition == avTargetPos)	return;

	mvTargetPosition = avTargetPos;
	
	mbTargetSphCoordsNeedUpdate = true;
	mbTargetPosUpdated = true;
	mbDistToTargetNeedsUpdate = true;
	mbCameraUpdated = true;
}

//-------------------------------------------------------------

void cEdCamera::SetTargetSphCoords(const cVector2f& avSphCoords)
{
	if(mvTargetSphCoords==avSphCoords) return;

	mvTargetSphCoords = avSphCoords;
	
	mbTargetPosNeedsUpdate = true;
	mbCameraUpdated = true;
}

//-------------------------------------------------------------

void cEdCamera::SetDistToTarget(float afDist)
{
	if(mfDistToTarget==afDist) return;

	mfDistToTarget = cMath::Clamp(afDist, 0.1f, afDist);

	mbCameraPosNeedsUpdate = true;
	mbCameraUpdated = true;
}

//-------------------------------------------------------------

const cVector3f& cEdCamera::GetCameraPosition()
{
	if(mbCameraPosNeedsUpdate)
	{
		mbCameraPosNeedsUpdate = false;
		mvCameraPosition = cMath::GetPoint3DFromSphericalCoords(mvTargetPosition, GetDistToTarget(), GetCameraSphCoords());
	}

	return mvCameraPosition;
}

//-------------------------------------------------------------

const cVector2f& cEdCamera::GetCameraSphCoords()
{
	if(mbCameraSphCoordsNeedUpdate)
	{
		mbCameraSphCoordsNeedUpdate = false;
		mvCameraSphCoords = cMath::GetSphericalCoordsFromPoint3D(mvTargetPosition, mvCameraPosition);
	}

	return mvCameraSphCoords;
}

//-------------------------------------------------------------

const cVector3f& cEdCamera::GetTargetPosition()
{
	if(mbTargetPosNeedsUpdate)
	{
		mbTargetPosNeedsUpdate = false;
		mvTargetPosition = cMath::GetPoint3DFromSphericalCoords(mvCameraPosition, GetDistToTarget(), GetTargetSphCoords());
	}

	return mvTargetPosition;
}

//-------------------------------------------------------------

const cVector2f& cEdCamera::GetTargetSphCoords()
{
	if(mbTargetSphCoordsNeedUpdate)
	{
		mbTargetSphCoordsNeedUpdate = false;
		mvTargetSphCoords = cMath::GetSphericalCoordsFromPoint3D(mvCameraPosition, mvTargetPosition);
	}

	return mvTargetSphCoords;
}

//-------------------------------------------------------------

float cEdCamera::GetDistToTarget()
{
	if(mbDistToTargetNeedsUpdate)
	{
		mfDistToTarget = (mvTargetPosition-mvCameraPosition).Length();

		mbDistToTargetNeedsUpdate = false;
	}

	return mfDistToTarget;
}

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

void cEdCamera::FetchSettings()
{
	//iEditor* pEditor = mpViewport->GetEditor();
	
	/*mfMinZoom = 0;
	mfMaxZoom = 5.5f;
	mfTrackFactor = 0.005f;
	mfMouseDragZoomFactor = 0.001f;
	mfMouseWheelZoomFactor = 0.1f;
	mfTumbleFactor = 0.005f;
	*/

	mfTrackMouseMaxDistanceToTarget = 40;	
}

//-------------------------------------------------------------

void cEdCamera::FocusOnBoundingVolume(cBoundingVolume* apBV)
{
	cVector3f vMin = apBV->GetMin();
	cVector3f vMax = apBV->GetMax();

	cVector3f vCenter = (vMin+vMax)*0.5f;
	cVector3f vDiff = vMax-vMin;
	if(vDiff.Length() < 0.1f )
	{
		vMax += cVector3f(2.5f);
		vMin -= cVector3f(2.5f);
		vDiff = cVector3f(5);
	}

	mpCurrentMode->FocusSpecific(vCenter, vDiff);
	
	UpdateEngineCamera();
	/*
	float fZoom;
	cVector3f vCamOffset = mvTargetPosition - mvCameraPosition;

	cVector3f vMin = apBV->GetMin();
	cVector3f vMax = apBV->GetMax();

	//bv.SetPosition(apSelection->GetTranslation());
	//bv.SetLocalMinMax(vMin-apSelection->GetTranslation(), vMax-apSelection->GetTranslation());

	cVector3f vCenter = (vMin+vMax)*0.5f;
	cVector3f vDiff = vMax-vMin;
	if(vDiff.Length() < 0.1f )
	{
		vMax += cVector3f(2.5f);
		vMin -= cVector3f(2.5f);
		vDiff = cVector3f(5);
	}

	if(IsOrtho())
	{
		float fWidth;
		vCenter = (vMin+vMax)*0.5f;

		switch(mCamType)
		{
		case eEdCameraType_OrthoFront:
			fWidth = cMath::Max(vDiff.y,vDiff.z);
			vCenter.x = apBV->GetPosition().x;
			break;
		case eEdCameraType_OrthoRight:
			fWidth = cMath::Max(vDiff.y,vDiff.x);
			vCenter.z = apBV->GetPosition().z;
			break;
		case eEdCameraType_OrthoTop:
			fWidth = cMath::Max(vDiff.x, vDiff.z);
			vCenter.y = apBV->GetPosition().y;
			break;
		}

		fZoom = mpCurrentZoomFunc.GetZoomFromOrthoViewSize(1.5f*fWidth);
		mvTargetPosition = vCenter;
		mvCameraPosition = mvTargetPosition - vCamOffset;

		mbTargetPosUpdated = true;
		mbCameraUpdated = true;

		SetZoom(fZoom);
	}
	else
	{
		float fMaxSideLength=-1;
		for(int i=0;i<3;++i)
		{
			if(vDiff.v[i] > fMaxSideLength)
				fMaxSideLength = vDiff.v[i];
		}

		float fDistance = (fMaxSideLength*0.75f)/tan(mpEngineCamera->GetFOV()*0.5f);
		cVector3f vDir = vCamOffset;
		vDir.Normalize();

		mvTargetPosition = apBV->GetPosition();
		mvCameraPosition = mvTargetPosition + vDir*fDistance;

		mbTargetPosUpdated = true;
		mbCameraUpdated = true;

		SetZoom(-1, true);
	}

	UpdateCamera(0);*/
}

//-------------------------------------------------------------

float cEdCamera::GetCamTrackAngleLowerFactor(const cVector3f& avCam, const cVector3f& avMousePos)
{
	cVector2f vSphCoords = cMath::GetSphericalCoordsFromPoint3D(avMousePos, avCam);

	float fAngle = kPif*0.5f - vSphCoords.x;

	float fStart = kPif*0.25f;
	float fEnd = kPif*0.075f;

	if(fAngle > fStart)
		return 1.0f;
	else
		return 0.0f;
}

//-------------------------------------------------------------

const cVector3f& cEdCamera::GetPointFromClampedDistance(const cVector3f& avCenter, cVector3f& avPos, float afMaxDistance)
{
	cVector3f vDiff = avPos-avCenter;
	float fDiffLength = vDiff.Length();
	if(fDiffLength>afMaxDistance)
	{
		vDiff = vDiff/fDiffLength;
		avPos = avCenter + vDiff*afMaxDistance;
	}

	return avPos;
}

//-------------------------------------------------------------

void cEdCamera::SetCameraPlanes(const cVector2f& avX)
{
	mpEngineCamera->SetNearClipPlane(avX.x);
	mpEngineCamera->SetFarClipPlane(avX.y);
}

//-------------------------------------------------------------

void cEdCamera::UpdateEngineCamera()
{
	if(mbCameraUpdated==false)
		return;

	const cVector3f& vCamPos = GetCameraPosition();
	const cVector3f& vTarPos = GetTargetPosition();

	mpEngineCamera->SetPosition(vCamPos);

	cVector3f vForward;
	cVector3f vUp;
	cVector3f vRight;
	if(mbKeepOrientation==false)
	{
		cVector3f vDisplacementPos = cMath::GetPoint3DFromSphericalCoords(vTarPos, 1, GetCameraSphCoords()-cVector2f(0.1f,0));
		cVector3f vHelperVector = (vDisplacementPos-vTarPos);
		vHelperVector.Normalize();
		vForward= (vCamPos-vTarPos);
		vForward.Normalize();
		vRight = cMath::Vector3Cross(vHelperVector,vForward);
		vRight.Normalize();
		vUp = cMath::Vector3Cross(vForward,vRight);
		vUp.Normalize();
	}
	else
	{
		vForward = mvForward;
		vUp = mvUp;
		vRight = mvRight;
	}

	mpEngineCamera->SetForward(vForward);
	mpEngineCamera->SetRight(vRight);
	mpEngineCamera->SetUp(vUp);

	mbCameraUpdated = false;
}

bool cEdCamera::LoadState(cXmlElement* apStateElem)
{
	SetCameraPosition(apStateElem->GetAttributeVector3f("CameraPos"));
	SetTargetPosition(apStateElem->GetAttributeVector3f("CameraTarget"));

	SetCameraMode(cString::To16Char(apStateElem->GetAttributeString("Mode")));
	if(mpCurrentMode) mpCurrentMode->LoadState(apStateElem);

	return true;
}

bool cEdCamera::SaveState(cXmlElement* apStateElem)
{
	apStateElem->SetAttributeVector3f("CameraPos", GetCameraPosition());
	apStateElem->SetAttributeVector3f("CameraTarget", GetTargetPosition());

	if(mpCurrentMode)
	{
		apStateElem->SetAttributeString("Mode", cString::To8Char(mpCurrentMode->GetName()));
		mpCurrentMode->SaveState(apStateElem);
	}

	return true;
}

//-------------------------------------------------------------

void cEdCamera::OnGridUpdated()
{
	/*
	if(mbLockToGrid==false)
		return;

	cVector3f vTargetPos = GetTargetPosition();
	cVector3f vCamTargetOffset = vTargetPos-GetCameraPosition();
	
	mvTargetPosition = mpViewport->GetGrid()->GetProjectedPosOnPlane(vTargetPos);
	mvCameraPosition = mvTargetPosition - vCamTargetOffset;

	mbTargetPosUpdated = true;
	mbCameraUpdated = true;

	UpdateCamera(0);
	*/
}

//-------------------------------------------------------------

void cEdCamera::AddCamMode(iCamMode* apMode)
{
	if(apMode==NULL) return;

	if(mpCurrentMode==NULL)
		mpCurrentMode = apMode;

	mvModes.push_back(apMode);
}

//-------------------------------------------------------------
