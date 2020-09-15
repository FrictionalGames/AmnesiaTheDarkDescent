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

#include "EditorViewport.h"

#include "EditorBaseClasses.h"
#include "EditorGrid.h"
#include "EditorSelection.h"
#include "EditorWorld.h"

int iEditorViewport::mlViewportCount = 0;
bool iEditorViewport::mbCamPlanesUpdated = true;
cVector2f iEditorViewport::mvCamPlanes = cVector2f(0.05f, 1000.0f);

//-------------------------------------------------------------

float iViewportCameraZoomFunc::GetOrthoViewSizeFromZoom(float afX)
{
	return mfMaxDistance*exp(-afX); 
}

float iViewportCameraZoomFunc::GetDistToTargetFromZoom(float afX) 
{
	return mfMaxDistance*exp(-afX);
}

float iViewportCameraZoomFunc::GetZoomFromDistToTarget(float afX) 
{ 
	return log(mfMaxDistance/afX); 
}

float iViewportCameraZoomFunc::GetZoomFromOrthoViewSize(float afX) 
{ 
	return log(mfMaxDistance/afX); 
}

//-------------------------------------------------------------

cEditorViewportCamera::cEditorViewportCamera(iEditorViewport* apViewport)
{
	mpViewport = apViewport;

	mpEngineCamera = mpViewport->mpEngine->GetScene()->CreateCamera(eCameraMoveMode_Fly);
	mpEngineCamera->SetPitchLimits(0,0);
	mpEngineCamera->SetRotateMode(eCameraRotateMode_Matrix);

	mfZoom = 0;

	mfDistToTarget = 0;
	mvCameraSphCoords = 0;
	mvTargetSphCoords = 0;

	mfOrthoViewSize = 0;
    
	mvTargetPosition = 0;
	mvCameraPosition = 0;

	mbCameraUpdated=true;

	mbTumbleActive=false;
	mbTrackActive=false;
	mbZoomActive=false;

	mbLockToGrid=true;

	FetchSettings();

	//mpCurrentZoomFunc = hplNew(iViewportCameraZoomFunc,(mfMinZoom, mfMaxZoom));
}

//-------------------------------------------------------------

cEditorViewportCamera::~cEditorViewportCamera()
{
	//if(mpCurrentZoomFunc) hplDelete(mpCurrentZoomFunc);
}

//-------------------------------------------------------------

void cEditorViewportCamera::SetCameraType(eEditorCameraType aType)
{
	mCamType = aType;

	eProjectionType proj = eProjectionType_Orthographic;
	ePlaneNormal gridPlane = ePlaneNormal_Y;
	bool bInfiniteFarPlane = true;
	bool bKeepOrientation = true;


	cVector3f vForward;
	cVector3f vUp;
	cVector3f vRight;
	switch(mCamType)
	{
	case eEditorCameraType_OrthoFront:
		vForward =	cVector3f(0,0,1);
		vUp =		cVector3f(0,1,0);
		vRight =	cVector3f(1,0,0);

		gridPlane = ePlaneNormal_Z;
		break;

	case eEditorCameraType_OrthoRight:
		vForward =	cVector3f(1,0,0);
		vUp =		cVector3f(0,1,0);
		vRight =	cVector3f(0,0,-1);

		gridPlane = ePlaneNormal_X;
		break;

	case eEditorCameraType_OrthoTop:
		vForward =	cVector3f(0,1,0);
		vUp =		cVector3f(0,0,-1);
		vRight =	cVector3f(1,0,0);
		break;

	case eEditorCameraType_Orbit:
		proj = eProjectionType_Perspective;
		bInfiniteFarPlane = false;
		bKeepOrientation = false;
		break;

	case eEditorCameraType_Fly:
		proj = eProjectionType_Perspective;
		bInfiniteFarPlane = false;
		bKeepOrientation = false;
		break;
	}

	mpViewport->GetGrid()->SetPlaneNormal(gridPlane);
	mpEngineCamera->SetProjectionType(proj);
	mpEngineCamera->SetInifintiveFarPlane(bInfiniteFarPlane);
	SetKeepOrientation(bKeepOrientation);
	SetOrientation(vForward, vUp, vRight);
}

//-------------------------------------------------------------

bool cEditorViewportCamera::IsOrtho()
{
	return mCamType==eEditorCameraType_OrthoFront ||
		   mCamType==eEditorCameraType_OrthoRight ||
		   mCamType==eEditorCameraType_OrthoTop;
}

//-------------------------------------------------------------

void cEditorViewportCamera::LockToGrid(bool abX)
{
	if(mbLockToGrid==abX)
		return;

	mbLockToGrid = abX;
	UpdateCamera(0);
}

//-------------------------------------------------------------

void cEditorViewportCamera::SetOrientation(const cVector3f& avForward, const cVector3f& avUp, const cVector3f& avRight)
{
	mvForward = avForward;
	mvUp = avUp;
	mvRight = avRight;
}

//-------------------------------------------------------------

void cEditorViewportCamera::SetZoom(float afX, bool abComputeFromDistance)
{
	//if(mpCurrentZoomFunc==NULL) return;

	if(abComputeFromDistance)
	{
		mfDistToTarget = (mvTargetPosition-mvCameraPosition).Length();
		mfZoom = mpCurrentZoomFunc.GetZoomFromDistToTarget(mfDistToTarget);
	}
	else
		mfZoom = afX;

	mfZoom = cMath::Clamp(mfZoom, mfMinZoom,mfMaxZoom);
	mfDistToTarget = mpCurrentZoomFunc.GetDistToTargetFromZoom(mfZoom);
	mfOrthoViewSize = mpCurrentZoomFunc.GetOrthoViewSizeFromZoom(mfZoom);

	mbCameraPosNeedsUpdate = true;
	mbTargetPosNeedsUpdate = true;
	mbCameraUpdated = true;
}

//-------------------------------------------------------------

void cEditorViewportCamera::SetCameraPosition(const cVector3f& avPosition)
{
	if(mvCameraPosition == avPosition)
		return;

	mvCameraPosition = avPosition;

	mbCameraSphCoordsNeedUpdate = true;
	mbCameraPosUpdated = true;
	mbCameraUpdated = true;
}

//-------------------------------------------------------------

void cEditorViewportCamera::SetCameraSphCoords(const cVector2f& avSphCoords)
{
	if(mvCameraSphCoords==avSphCoords) return;

	mvCameraSphCoords = avSphCoords;
	
	mbCameraPosNeedsUpdate = true;
	mbCameraUpdated = true;
}

//-------------------------------------------------------------

void cEditorViewportCamera::SetTargetPosition(const cVector3f& avTargetPos)
{
	if(mvTargetPosition == avTargetPos)	return;

	mvTargetPosition = avTargetPos;
	
	mbTargetSphCoordsNeedUpdate = true;
	mbTargetPosUpdated = true;
	mbCameraUpdated = true;
}

//-------------------------------------------------------------

void cEditorViewportCamera::SetTargetSphCoords(const cVector2f& avSphCoords)
{
	if(mvTargetSphCoords==avSphCoords) return;

	mvTargetSphCoords = avSphCoords;
	
	mbTargetPosNeedsUpdate = true;
	mbCameraUpdated = true;
}

//-------------------------------------------------------------

const cVector3f& cEditorViewportCamera::GetCameraPosition()
{
	if(mbCameraPosNeedsUpdate)
	{
		if(mCamType==eEditorCameraType_Orbit) 
			mvCameraPosition = cMath::GetPoint3DFromSphericalCoords(mvTargetPosition, mfDistToTarget, GetCameraSphCoords());
		
		mbCameraPosNeedsUpdate = false;
	}

	return mvCameraPosition;
}

//-------------------------------------------------------------

const cVector2f& cEditorViewportCamera::GetCameraSphCoords()
{
	if(mbCameraSphCoordsNeedUpdate)
	{
		mvCameraSphCoords = cMath::GetSphericalCoordsFromPoint3D(mvTargetPosition, mvCameraPosition);
		
		mbCameraSphCoordsNeedUpdate = false;
	}

	return mvCameraSphCoords;
}

//-------------------------------------------------------------

const cVector3f& cEditorViewportCamera::GetTargetPosition()
{
	if(mbTargetPosNeedsUpdate)
	{
		if(mCamType==eEditorCameraType_Fly)
			mvTargetPosition = cMath::GetPoint3DFromSphericalCoords(mvCameraPosition, mfDistToTarget, GetTargetSphCoords());
		
		mbTargetPosNeedsUpdate = false;
	}

	return mvTargetPosition;
}

//-------------------------------------------------------------

const cVector2f& cEditorViewportCamera::GetTargetSphCoords()
{
	if(mbTargetSphCoordsNeedUpdate)
	{
		mvTargetSphCoords = cMath::GetSphericalCoordsFromPoint3D(mvCameraPosition, mvTargetPosition);

		mbTargetSphCoordsNeedUpdate = false;
	}

	return mvTargetSphCoords;
}

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

void cEditorViewportCamera::FetchSettings()
{
	iEditorBase* pEditor = mpViewport->GetEditorBase();
	
	mfMinZoom = 0;
	mfMaxZoom = 5.5f;
	mfTrackFactor = cString::ToFloat(pEditor->GetSetting("TrackFactor").c_str(), 0.005f);
	mfMouseDragZoomFactor = cString::ToFloat(pEditor->GetSetting("ZoomFactor").c_str(), 0.001f);
	mfMouseWheelZoomFactor = cString::ToFloat(pEditor->GetSetting("MouseWheelZoom").c_str(), 0.1f);
	mfTumbleFactor = cString::ToFloat(pEditor->GetSetting("TumbleFactor").c_str(), 0.005f);

	mfTrackMouseMaxDistanceToTarget = 40;	
}

//-------------------------------------------------------------

void cEditorViewportCamera::SetTrackActive(bool abX)
{
	mbTrackActive = abX;
	if(abX)
	{
		mvTrackRefCameraPos = GetCameraPosition();
		mvTrackRefTargetPos = GetTargetPosition();
		mvTrackRefMousePos = mpViewport->GetMouseWorldPosition(true);
		mvTrackCamTargetOffset = mvTrackRefTargetPos-mvTrackRefCameraPos;
	}
}

//-------------------------------------------------------------

void cEditorViewportCamera::Tumble(const cVector2f& avParams)
{
	cVector2f vSphCoords;
	switch(mCamType)
	{
	case eEditorCameraType_Orbit:
		vSphCoords = GetCameraSphCoords();
		//if(cMath::Vector3Dot(mpEngineCamera->GetUp(), cVector3f(0,1,0)>0))

		// Longitude
		if(mpEngineCamera->GetUp().y<0)
			vSphCoords.y -= mfTumbleFactor * avParams.x;
		else
			vSphCoords.y += mfTumbleFactor * avParams.x;

		// Latitude
		vSphCoords.x -= mfTumbleFactor * avParams.y;

		SetCameraSphCoords(vSphCoords);
		break;
	case eEditorCameraType_Fly:
		vSphCoords = GetTargetSphCoords();
		mvTargetSphCoords.x += mfTumbleFactor * avParams.x;
		mvTargetSphCoords.y += mfTumbleFactor * avParams.y;
		SetTargetPosition(cMath::GetPoint3DFromSphericalCoords(mvCameraPosition, mfDistToTarget, mvTargetSphCoords));
		break;
	default:
		break;
	}
}

//-------------------------------------------------------------

void cEditorViewportCamera::Track(const cVector2f& avParams)
{
	cVector3f vCamTargetOffset = GetTargetPosition()-GetCameraPosition();
	cVector3f vDisplacement = 0;

	cVector3f vRayStart;
	cVector3f vRayDir;
	cVector3f vRayEnd;

	if(IsOrtho())
	{
		mpEngineCamera->UnProjectHelper(&vRayStart, &vRayDir, mpViewport->GetMouseViewportPosition(), mpViewport->GetGuiViewportSize(),
										cMatrixf(),
										mvTrackRefCameraPos, mpEngineCamera->GetForward(), mpEngineCamera->GetUp(), mpEngineCamera->GetRight());
	}
	else
	{
		vRayStart = mvTrackRefCameraPos;
		vRayDir = mpViewport->GetUnprojectedDir();
	}

	vRayEnd = vRayStart + vRayDir*mpViewport->mfRayEndDistance;

	if(IsOrtho() || mbLockToGrid)
	{
		cEditorGrid* pGrid = mpViewport->GetGrid();
		if(cMath::CheckPlaneLineIntersection(pGrid->GetPlane(), 
											vRayStart, 
											vRayEnd, 
											&mvTrackNewMousePos, NULL)==false)
		{
			mvTrackNewMousePos = pGrid->GetProjectedPosOnPlane(vRayEnd + vRayDir*vRayEnd.v[pGrid->GetPlaneNormal()]);
		}		
		cVector3f vNewTargetPosition = mvTrackRefTargetPos - mvTrackNewMousePos+mvTrackRefMousePos;
		cVector3f vDiff = GetSmoothedDisplacement(vNewTargetPosition-mvTargetPosition, 4);

		mvTargetPosition = mvTargetPosition + vDiff;
	}
	else
	{
		vDisplacement = (mpEngineCamera->GetUp()*avParams.y + 
						 mpEngineCamera->GetRight()*(-avParams.x))*mfTrackFactor;
		mvTargetPosition += vDisplacement;
	}
	
	mvCameraPosition = mvTargetPosition - vCamTargetOffset;
	
	mbTargetPosUpdated = true;
	mbCameraUpdated = true;
}

//-------------------------------------------------------------

void cEditorViewportCamera::Zoom(const cVector2f& avParams, bool abUseWheel)
{
	float fFactor = abUseWheel?mfMouseWheelZoomFactor : mfMouseDragZoomFactor;

	SetZoom(mfZoom - avParams.y*fFactor);
}

//-------------------------------------------------------------

void cEditorViewportCamera::FocusOnSelection(cEditorSelection* apSelection)
{
	float fZoom;
	cVector3f vCamOffset = mvTargetPosition - mvCameraPosition;

	const cVector3f& vMin = apSelection->GetSelectionAABBMin();
	const cVector3f& vMax = apSelection->GetSelectionAABBMax();

	cBoundingVolume bv = cBoundingVolume();
	bv.SetPosition(apSelection->GetCenterTranslation());
	bv.SetLocalMinMax(vMin-apSelection->GetCenterTranslation(), vMax-apSelection->GetCenterTranslation());

	cVector3f vCenter;
	cVector3f vDiff = vMax-vMin;

	if(IsOrtho())
	{
		float fWidth;
		vCenter = (vMin+vMax)*0.5f;

		switch(mCamType)
		{
		case eEditorCameraType_OrthoFront:
			fWidth = cMath::Max(vDiff.y,vDiff.z);
			vCenter.x = apSelection->GetCenterTranslation().x;
			break;
		case eEditorCameraType_OrthoRight:
			fWidth = cMath::Max(vDiff.y,vDiff.x);
			vCenter.z = apSelection->GetCenterTranslation().z;
			break;
		case eEditorCameraType_OrthoTop:
			fWidth = cMath::Max(vDiff.x, vDiff.z);
			vCenter.y = apSelection->GetCenterTranslation().y;
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
		cVector3f vDir = cMath::Vector3Normalize(vCamOffset);
		float fMinBVSide = cMath::Min(vDiff.x, cMath::Min(vDiff.y, vDiff.z));
		if(fMinBVSide<kEpsilonf)
			fMinBVSide = 1;

		mvTargetPosition = bv.GetPosition();
		mvCameraPosition = mvTargetPosition + vDir*fMinBVSide;
		int lCount = 0;
		do
		{
			mvCameraPosition += vDir*fMinBVSide;
			mpEngineCamera->SetPosition(mvCameraPosition);
			++lCount;
		}while(BoundingVolumeFitsInView(bv)==false && lCount<10);

		mbTargetPosUpdated = true;
		mbCameraUpdated = true;

		SetZoom(-1, true);
	}

	UpdateCamera(0);
}

//-------------------------------------------------------------

void cEditorViewportCamera::UpdateCamera(const cVector2f& avMouseRel)
{
	if(mbTumbleActive)
	{
		Tumble(avMouseRel);
	}
	else if(mbTrackActive)
	{
		Track(avMouseRel);
	}
	else if(mbZoomActive)
	{
		Zoom(avMouseRel);
	}

	UpdateEngineCamera();
}

//-------------------------------------------------------------

cVector3f cEditorViewportCamera::GetSmoothedDisplacement(const cVector3f& avDisp, float afMaxLength, float afTurnTo )
{
	cVector3f vNewDisp = avDisp;
	if(afMaxLength>=0)
	{
		float fDispLength = vNewDisp.Length();
		if(fDispLength>afMaxLength)
		{
			vNewDisp = vNewDisp/fDispLength;
			vNewDisp = vNewDisp*afTurnTo;
		}
	}

	return vNewDisp;
}

//-------------------------------------------------------------

const cVector3f& cEditorViewportCamera::GetPointFromClampedDistance(const cVector3f& avCenter, cVector3f& avPos, float afMaxDistance)
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

void cEditorViewportCamera::SetCameraPlanes(const cVector2f& avX)
{
	mpEngineCamera->SetNearClipPlane(avX.x);
	mpEngineCamera->SetFarClipPlane(avX.y);
}

//-------------------------------------------------------------

void cEditorViewportCamera::UpdateEngineCamera()
{
	if(mbCameraUpdated==false)
		return;

	const cVector3f& vCamPos = GetCameraPosition();
	const cVector3f& vTarPos = GetTargetPosition();

	mpEngineCamera->SetPosition(vCamPos);
	cMatrixf mtxCamRotation = cMath::MatrixRotate(cVector3f(GetCameraSphCoords())*-1 , eEulerRotationOrder_ZYX);
	mpEngineCamera->SetRotationMatrix(mtxCamRotation);

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

	mpEngineCamera->SetOrthoViewSize(cVector2f(mfOrthoViewSize, (1/mpEngineCamera->GetAspect())*mfOrthoViewSize));

	mbCameraUpdated = false;
}

//-------------------------------------------------------------

void cEditorViewportCamera::OnGridUpdated()
{
	if(mbLockToGrid==false)
		return;

	cVector3f vTargetPos = GetTargetPosition();
	cVector3f vCamTargetOffset = vTargetPos-GetCameraPosition();
	
	mvTargetPosition = mpViewport->GetGrid()->GetProjectedPosOnPlane(vTargetPos);
	mvCameraPosition = mvTargetPosition - vCamTargetOffset;

	mbTargetPosUpdated = true;
	mbCameraUpdated = true;

	UpdateCamera(0);
}

//-------------------------------------------------------------

float cEditorViewportCamera::BinaryDistanceSearch(float &afStart, float &afEnd, const cBoundingVolume &aBV)
{
	float fMiddle = (afStart+afEnd)*0.5f;
	cBoundingVolume bv = aBV;

	cFrustum* pFrustum = mpEngineCamera->GetFrustum();

	cVector3f vOldPos = mvCameraPosition;
	cVector3f vDir = cMath::Vector3Normalize(mvTargetPosition-vOldPos);

	cVector3f vNormalMin, vNormalMax;
	cRect2f vNormalClipRect;

	float fStartArea, fEndArea;
	bool bStartInside, bEndInside;
	fStartArea = 0.1f;
	fEndArea = 0.1f;
	bStartInside = false;
	bEndInside = false;

	mpEngineCamera->SetPosition(mvTargetPosition+vDir*afStart);
	cMath::GetNormalizedClipRectFromBV(vNormalMin, vNormalMax, bv, mpEngineCamera->GetFrustum(), mpEngineCamera->GetFrustum()->GetFOV()*0.5f);
	vNormalClipRect = cRect2f(vNormalMin.x, vNormalMin.y, vNormalMax.x-vNormalMin.x, vNormalMax.y-vNormalMin.y);
	if(vNormalClipRect.x > -0.7f && vNormalClipRect.y > -0.7f &&
		vNormalClipRect.w <= 1.4f && vNormalClipRect.h <= 1.4f)
	{
		fStartArea = vNormalClipRect.w*vNormalClipRect.h;
		bStartInside = true;
	}

	mpEngineCamera->SetPosition(mvTargetPosition+vDir*afEnd);
	cMath::GetNormalizedClipRectFromBV(vNormalMin, vNormalMax, bv, mpEngineCamera->GetFrustum(), mpEngineCamera->GetFrustum()->GetFOV()*0.5f);
	vNormalClipRect = cRect2f(vNormalMin.x, vNormalMin.y, vNormalMax.x-vNormalMin.x, vNormalMax.y-vNormalMin.y);
	if(vNormalClipRect.x > -0.7f && vNormalClipRect.y > -0.7f &&
		vNormalClipRect.w <= 1.4f && vNormalClipRect.h <= 1.4f)
	{
		fEndArea = vNormalClipRect.w*vNormalClipRect.h;
		bEndInside = true;
	}

	float fRet;
	if(bStartInside && fStartArea>fEndArea)
	{
		fRet = afStart;
		afEnd = fMiddle;
	}
	else
	{
		fRet = afEnd;
		afStart = fMiddle;
	}

	mpEngineCamera->SetPosition(vOldPos);

	return fRet;
}

//-------------------------------------------------------------

bool cEditorViewportCamera::BoundingVolumeFitsInView(const cBoundingVolume &aBV)
{
	cBoundingVolume bv = aBV;

	cFrustum* pFrustum = mpEngineCamera->GetFrustum();

	cVector3f vNormalMin, vNormalMax;
	cRect2f vNormalClipRect;

	if(cMath::GetNormalizedClipRectFromBV(vNormalMin, vNormalMax, bv, pFrustum, pFrustum->GetFOV()*0.5f))
	{
		vNormalClipRect = cRect2f(vNormalMin.x, vNormalMin.y, vNormalMax.x-vNormalMin.x, vNormalMax.y-vNormalMin.y);
		if(vNormalClipRect.x > -0.7f && vNormalClipRect.y > -0.7f &&
			vNormalClipRect.w <= 1.4f && vNormalClipRect.h <= 1.4f)
			return true;
	}

	return false;
}

//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------

iEditorViewport::iEditorViewport(iEditorBase* apEditor, cWorld* apWorld, iFrameBuffer* apFB, bool abDestroyFBOnExit)
{
	++mlViewportCount;
	tString sViewportCount = "Viewport" + cString::ToString(mlViewportCount) + "_";

	mpEditorBase = apEditor;
	mpEngine = mpEditorBase->GetEngine();
	mpGfx = mpEngine->GetGraphics();
	mpGuiSet = apEditor->GetSet();

	mpFB = NULL;

	mCamera = cEditorViewportCamera(this);
	mpEngineViewport = mpEngine->GetScene()->CreateViewport(mCamera.GetEngineCamera(), apWorld, true);
	SetViewportActive(false);

	mpImgViewport = NULL;
	mvViewportPos = 0;
	mvViewportSize = 0;
	mvEngineViewportPos = -1;
	mvEngineViewportSize = -1;
	mvUVStart = -1;
	mvUVSize = -1;

	mfRayEndDistance = 200;

	//mbMouseMoved = true;
	mbMousePositionUpdated = true;
	mbUnprojectionUpdated = true;

	mbDestroyFBOnExit = abDestroyFBOnExit;

	mpGrid = hplNew(cEditorGrid, (this));

	SetFrameBuffer(apFB);
}

//-------------------------------------------------------------

iEditorViewport::~iEditorViewport()
{
	mpEngineViewport->SetFrameBuffer(NULL);
	mpEngine->GetScene()->DestroyViewport(mpEngineViewport);
	if(mpImgViewport) mpGuiSet->DestroyWidget(mpImgViewport);

	hplDelete(mpGrid);

	if(mbDestroyFBOnExit)
	{
		mpEngine->GetGraphics()->GetLowLevel()->SetCurrentFrameBuffer(NULL);
		mpEngine->GetGraphics()->DestroyFrameBuffer(mpFB);
	}
}

//-------------------------------------------------------------

void iEditorViewport::LogDebug()
{
	//cCamera* pCam = mCamera.GetEngineCamera();

	//Log("FocusedViewport debug info - FOV:%f Aspect:%f NearPlane:%f FarPlane:%f\n", pCam->GetFOV(), pCam->GetAspect(), pCam->GetNearClipPlane(), pCam->GetFarClipPlane());
}

//-------------------------------------------------------------

void iEditorViewport::SetViewportActive(bool abX)
{
	mpEngineViewport->SetActive(abX);
	mpEngineViewport->SetVisible(abX);
}

//-------------------------------------------------------------

void iEditorViewport::CreateGuiViewport(iWidget* apParentWidget)
{
	if(mpImgViewport) mpGuiSet->DestroyWidget(mpImgViewport);

	mpImgViewport = mpGuiSet->CreateWidgetImage("", mvViewportPos, mvViewportSize, eGuiMaterial_Alpha, false, apParentWidget);
	mpImgViewport->AddCallback(eGuiMessage_OnUpdate, this, kGuiCallback(Viewport_OnUpdate));
	mpImgViewport->AddCallback(eGuiMessage_GotFocus, this, kGuiCallback(Viewport_OnGotFocus));
	mpImgViewport->AddCallback(eGuiMessage_LostFocus, this, kGuiCallback(Viewport_OnLostFocus));

	mpImgViewport->AddCallback(eGuiMessage_OnDraw, this, kGuiCallback(Viewport_OnDraw));

	mpImgViewport->AddCallback(eGuiMessage_MouseMove, this, kGuiCallback(Viewport_OnMouseMove));
	mpImgViewport->AddCallback(eGuiMessage_MouseDown, this, kGuiCallback(Viewport_OnMouseDown));
	mpImgViewport->AddCallback(eGuiMessage_MouseUp, this, kGuiCallback(Viewport_OnMouseUp));
	mpImgViewport->AddCallback(eGuiMessage_KeyPress, this, kGuiCallback(Viewport_OnKeyPress));
	mpImgViewport->AddCallback(eGuiMessage_KeyRelease, this, kGuiCallback(Viewport_OnKeyRelease));

	mbViewportNeedsUpdate = true;
}

//-------------------------------------------------------------


void iEditorViewport::SetRenderMode(eRenderer aMode)
{
	if(mCamera.IsOrtho())
		aMode = eRenderer_WireFrame;

	mRenderMode = aMode;
	
	iRenderer* pRenderer = mpGfx->GetRenderer(mRenderMode);
	mpEngineViewport->SetRenderer(pRenderer);
	SetClearColor(mpEditorBase->GetEditorWorld()->GetBGDefaultColor());
}

//-------------------------------------------------------------

void iEditorViewport::SetFrameBuffer(iFrameBuffer* apFB)
{
	if(apFB==NULL || mpFB==apFB) return;
	
	mpFB = apFB;
	iFrameBufferAttachment* pColorBuffer = mpFB->GetColorBuffer(0);
	if(pColorBuffer) mpRenderTarget = pColorBuffer->ToTexture();
	
	mpEngineViewport->SetFrameBuffer(mpFB);
	mbViewportNeedsUpdate = true;
}

//-------------------------------------------------------------

void iEditorViewport::UpdateViewport()
{
	if(mpImgViewport==NULL || mbViewportNeedsUpdate==false)
		return;

	cGui* pGui = mpGuiSet->GetGui();

	////////////////////////////////////////////
	// Destroy previous Image
	cGuiGfxElement* pImg = mpImgViewport->GetImage();
	if(pImg) pGui->DestroyGfx(pImg);

	////////////////////////////////////////////
	// Set updated one	
	pImg = pGui->CreateGfxTexture(mpRenderTarget, false, eGuiMaterial_Diffuse, cColor(1,1), true, mvUVStart, mvUVEnd);
	mpImgViewport->SetImage(pImg);
	
	mbViewportNeedsUpdate = false;
}

//-------------------------------------------------------------

void iEditorViewport::SetGuiViewportPos(const cVector3f& avPos)
{
	if(mvViewportPos==avPos) return;
	mvViewportPos = avPos;

	if(mpImgViewport)
		mpImgViewport->SetPosition(mvViewportPos);
}

//-------------------------------------------------------------

void iEditorViewport::SetGuiViewportSize(const cVector2f& avSize)
{
	if(mvViewportSize==avSize) return;
	mvViewportSize = avSize;

	mCamera.GetEngineCamera()->SetAspect(avSize.x/avSize.y);

	if(mpImgViewport)
		mpImgViewport->SetSize(mvViewportSize);
}

//-------------------------------------------------------------

void iEditorViewport::SetEngineViewportPositionAndSize(const cVector2l& avPos, const cVector2l& avSize)
{
	if(mvEngineViewportPos==avPos && mvEngineViewportSize==avSize) return;
	mvEngineViewportPos = avPos;
	mpEngineViewport->SetPosition(mvEngineViewportPos);
	mvEngineViewportSize = avSize;
	mpEngineViewport->SetSize(mvEngineViewportSize);

	const cVector2l& vFBSize = mpFB->GetSize();
	cVector2f vFBSizeFloat = cVector2f((float)vFBSize.x, (float)vFBSize.y);

	cVector2f vPosFloat = cVector2f((float)mvEngineViewportPos.x, (float)mvEngineViewportPos.y);
	cVector2f vSizeFloat = cVector2f((float)mvEngineViewportSize.x, (float)mvEngineViewportSize.y);

	mvUVStart = (vPosFloat+cVector2f(0,vSizeFloat.y)) / 
				vFBSizeFloat;

	if(mvUVStart.y>=1.0f) mvUVStart.y-=1.0f;

	mvUVSize =  vSizeFloat/
			   vFBSizeFloat;

	mvUVEnd = mvUVStart + mvUVSize;

	mbViewportNeedsUpdate = true;
}

//-------------------------------------------------------------

void iEditorViewport::SetEngineViewportSize(const cVector2l& avSize)
{
	if(mvEngineViewportSize==avSize) return;
	mvEngineViewportSize = avSize;
	mpEngineViewport->SetSize(mvEngineViewportSize);

	const cVector2l& vFBSize = mpFB->GetSize();

	mvUVSize = cVector2f((float)mvEngineViewportSize.x, (float)mvEngineViewportSize.y) /
			   cVector2f((float)vFBSize.x, (float)vFBSize.y);
	mvUVStart = cVector2f(mvUVStart.x, mvUVSize.y-mvUVStart.y);
	mbViewportNeedsUpdate = true;
}

//-------------------------------------------------------------

const cVector2f& iEditorViewport::GetMouseViewportPosition()
{
	if(mbMousePositionUpdated)
	{
		cVector2f vViewportGlobalPos = cVector2f(mpImgViewport->GetGlobalPosition().x,mpImgViewport->GetGlobalPosition().y);
		mvMouseViewportPos = mvMouseScreenPos - vViewportGlobalPos;

		mvMouseViewportPos.x = cMath::Clamp(mvMouseViewportPos.x, 0, GetGuiViewportSize().x);
		mvMouseViewportPos.y = cMath::Clamp(mvMouseViewportPos.y, 0, GetGuiViewportSize().y);

		mbMousePositionUpdated = false;
	}

	return mvMouseViewportPos;
}

//-------------------------------------------------------------

cVector2l iEditorViewport::GetMouseViewportPositionInt()
{
	const cVector2f& vPos = GetMouseViewportPosition();

	return cVector2l((int)vPos.x, (int)vPos.y);
}

//-------------------------------------------------------------

const cVector3f& iEditorViewport::GetUnprojectedStart()
{
	UpdateUnprojection();
	
	return mvUnprojectedStart;
}

//-------------------------------------------------------------

const cVector3f& iEditorViewport::GetUnprojectedDir()
{
	UpdateUnprojection();
	
	return mvUnprojectedDir;
}

//-------------------------------------------------------------

const cVector3f& iEditorViewport::GetUnprojectedEnd()
{
	UpdateUnprojection();

	return mvUnprojectedEnd;
}

cBoundingVolume* iEditorViewport::GetRayBV()
{
	UpdateUnprojection();

	return &mRayBV;
}

//-------------------------------------------------------------

const cVector3f& iEditorViewport::GetMouseWorldPosition(bool abForceUpdate)
{
	if(mpEditorBase->GetFlags(eEditorFlag_MouseMoved) || abForceUpdate)
	{
		const cVector3f& vStart = GetUnprojectedStart();
		const cVector3f& vEnd = GetUnprojectedEnd();
		
		if(cMath::CheckPlaneLineIntersection(mpGrid->GetPlane(), vStart,vEnd, &mvMouseWorldPos,NULL)==false)
		{
			const cVector3f& vDir = GetUnprojectedDir();
			mvMouseWorldPos = mpGrid->GetProjectedPosOnPlane(vEnd + vDir*vEnd.v[mpGrid->GetPlaneNormal()]);
		}

	}

	return mvMouseWorldPos;
}

//-------------------------------------------------------------

const cVector3f& iEditorViewport::GetGridCenter()
{
	switch(mCamera.GetCameraType())
	{
	case eEditorCameraType_Fly:
		return mCamera.GetCameraPosition();
	default:
		return mCamera.GetTargetPosition();
	}    
}

//-------------------------------------------------------------

void iEditorViewport::AddViewportCallback(iRendererCallback* apCallback)
{
	mpEngineViewport->AddRendererCallback(apCallback);
}

//-------------------------------------------------------------

void iEditorViewport::UpdateCameraPlanes()
{
	mCamera.SetCameraPlanes(mvCamPlanes);
}

//-------------------------------------------------------------

void iEditorViewport::SetClearColor(const cColor& aX)
{
	cColor clearCol = aX;
	if(mRenderMode==eRenderer_Main)
		clearCol = cColor(0,1);
	
	mpEngineViewport->GetRenderSettings()->mClearColor = clearCol;
}

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

bool iEditorViewport::Viewport_OnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	mCamera.SetCameraPlanes(mvCamPlanes);
	return OnViewportUpdate(aData);
}
kGuiCallbackDeclaredFuncEnd(iEditorViewport,Viewport_OnUpdate);

//-------------------------------------------------------------

bool iEditorViewport::Viewport_OnGotFocus(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportGotFocus(aData);
}
kGuiCallbackDeclaredFuncEnd(iEditorViewport,Viewport_OnGotFocus);

//-------------------------------------------------------------

bool iEditorViewport::Viewport_OnLostFocus(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportLostFocus(aData);
}
kGuiCallbackDeclaredFuncEnd(iEditorViewport,Viewport_OnLostFocus);

//-------------------------------------------------------------

bool iEditorViewport::Viewport_OnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportDraw(aData);
}
kGuiCallbackDeclaredFuncEnd(iEditorViewport,Viewport_OnDraw);

//-------------------------------------------------------------

bool iEditorViewport::Viewport_OnMouseMove(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mpEditorBase->GetFlags(eEditorFlag_MouseMoved)==false)
		return false;
	if(mpImgViewport->GetMouseIsOver()==false && mpGuiSet->GetFocusedWidget()!=mpImgViewport)
		return false;

	mvMouseScreenPos = aData.mvPos;
	mbMousePositionUpdated = true;
	mbUnprojectionUpdated = true;

	mCamera.UpdateCamera(aData.mvRel);

	return OnViewportMouseMove(aData);
}
kGuiCallbackDeclaredFuncEnd(iEditorViewport,Viewport_OnMouseMove);

//-------------------------------------------------------------

bool iEditorViewport::Viewport_OnMouseDown(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportMouseDown(aData);
}
kGuiCallbackDeclaredFuncEnd(iEditorViewport,Viewport_OnMouseDown);

//-------------------------------------------------------------

bool iEditorViewport::Viewport_OnMouseUp(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportMouseUp(aData);
}
kGuiCallbackDeclaredFuncEnd(iEditorViewport,Viewport_OnMouseUp);

//-------------------------------------------------------------

bool iEditorViewport::Viewport_OnKeyPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportKeyPress(aData);
}
kGuiCallbackDeclaredFuncEnd(iEditorViewport,Viewport_OnKeyPress);

//-------------------------------------------------------------

bool iEditorViewport::Viewport_OnKeyRelease(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportKeyRelease(aData);
}
kGuiCallbackDeclaredFuncEnd(iEditorViewport,Viewport_OnKeyRelease);

//-------------------------------------------------------------

void iEditorViewport::UpdateUnprojection()
{
	if(mbUnprojectionUpdated==false) return;

	const cVector2f& vMouseViewportPos = GetMouseViewportPosition();
	const cVector2f& vViewportSize = GetGuiViewportSize();
	
	if(cMath::CheckPointInRectIntersection(vMouseViewportPos, cRect2f(0,0,vViewportSize.x, vViewportSize.y)))
	{
		cCamera* pCam = mCamera.GetEngineCamera();
		pCam->UnProject(&mvUnprojectedStart, &mvUnprojectedDir, vMouseViewportPos, vViewportSize);

		mvUnprojectedEnd = mvUnprojectedStart + mvUnprojectedDir*mfRayEndDistance;
	}
	cVector3f vMin = 99999;
	cVector3f vMax = -99999;
	cMath::ExpandAABB(vMin, vMax, mvUnprojectedStart, mvUnprojectedEnd);
	mRayBV.SetLocalMinMax(vMin, vMax);

	mbUnprojectionUpdated = false;
}

//-------------------------------------------------------------



//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------

//-------------------------------------------------------------


