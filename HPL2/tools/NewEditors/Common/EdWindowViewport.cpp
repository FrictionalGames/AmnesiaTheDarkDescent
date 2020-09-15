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

#include "../Common/EdWindowViewport.h"

#include "../Common/EdViewMenu.h"

#include "../Common/EdCamera.h"

#include "../Common/Editor.h"
#include "../Common/EdGrid.h"
#include "../Common/EdClipPlane.h"
#include "../Common/EdEditMode.h"
#include "../Common/EdWorld.h"
#include "../Common/EdSelection.h"
#include "../Common/EdSurfacePicker.h"

#include "../Common/EdEditModeHandler.h"
#include "../Common/EdEditModeSelect.h"
#include "../Common/EdSelection.h"
#include "../Common/EdAction.h"

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// WINVIEWPORT CAM CONTROLLER - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

iEdWindowViewportCamController::iEdWindowViewportCamController(cEdCamera* apCam, const tWString& asName) : iCamController(apCam, asName)
{
	mbViewMode = false;
}

void iEdWindowViewportCamController::UpdateInput()
{
	mbViewMode = mpCam->GetViewport()->GetEditor()->GetEngine()->GetInput()->GetKeyboard()->KeyIsDown(eKey_LeftAlt);
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM CONTROLLER ORTHO - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

cCamControllerOrtho::cCamControllerOrtho(cEdCamera* apCam) : iEdWindowViewportCamController(apCam, _W("Ortho"))
{
	mbIsTracking = false;
	mbIsZooming = false;
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM CONTROLLER ORTHO - PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

bool cCamControllerOrtho::IsActive()
{
	return mbIsTracking || mbIsZooming;
}

//--------------------------------------------------------------------------------

bool cCamControllerOrtho::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(IsViewModeActive())
	{
		if(IsActive())	return true;
			
		if(aClick.mbMid)
		{
			mbIsTracking = true;

			mvTrackRefCamPos = mpCam->GetCameraPosition();
			mvTrackRefTargetPos = mpCam->GetTargetPosition();
			mvTrackCamTargetOffset = mvTrackRefTargetPos-mvTrackRefCamPos;
			mvTrackRefMousePos = aClick.mpViewport->GetMouseWorldPosition(true);

			return true;
		}
		if(aClick.mbRight)
		{
			mbIsZooming = true;
			return true;
		}
	}

	if(aClick.mbAlt==false && aClick.mbControl==false && aClick.mbShift==false)
	{
		if(aClick.mbWheelUp)
		{
			Zoom(-1 * 0.25f);
			mbForceUpdate = true;

			return true;
		}
		if(aClick.mbWheelDn)
		{
			Zoom(1 * 0.25f);
			mbForceUpdate = true;

			return true;
		}
	}

	return false;
}

bool cCamControllerOrtho::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(IsActive())
	{
		mbIsTracking = false;
		mbIsZooming = false;

		return false;
	}

	return false;
}

//--------------------------------------------------------------------------------

bool cCamControllerOrtho::LoadState(cXmlElement* apStateElem)
{
	if(iCamController::LoadState(apStateElem)==false)
		return false;

	Zoom(cVector2f(0, apStateElem->GetAttributeFloat("OrthoSize")));

	return true;
}

bool cCamControllerOrtho::SaveState(cXmlElement* apStateElem)
{
	if(iCamController::SaveState(apStateElem)==false)
		return false;

	apStateElem->SetAttributeFloat("OrthoSize", mpCam->GetEngineCamera()->GetOrthoViewSize().x);
	
	return true;
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM CONTROLLER ORTHO - PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cCamControllerOrtho::OnUpdate(const cViewportClick& aClick)
{
	if(mbIsTracking)
		Track(0);
	if(mbIsZooming)
		Zoom(aClick.mvRelPos * 0.5f);
}

//--------------------------------------------------------------------------------

void cCamControllerOrtho::Track(const cVector2f& avMouseRelPos)
{
	cCamera* pEngCam = mpCam->GetEngineCamera();
	iEdViewport* pViewport = mpCam->GetViewport();
	cEdGrid* pGrid = pViewport->GetGrid();

	cVector3f vCamTargetPos = mpCam->GetTargetPosition();

	cVector3f vRayStart, vRayDir, vRayEnd;

	pEngCam->UnProjectHelper(&vRayStart, &vRayDir, pViewport->GetMouseViewportPosition(), pViewport->GetGuiViewportSize(),
								cMatrixf(),
								mvTrackRefCamPos, pEngCam->GetForward(), pEngCam->GetUp(), pEngCam->GetRight());

	vRayEnd = vRayStart + vRayDir*pViewport->GetRayEndDistance();

	cMath::CheckPlaneLineIntersection(pGrid->GetPlane(), 
										vRayStart, vRayEnd, 
										&mvTrackNewMousePos, NULL);

	cVector3f vNewTargetPos = mvTrackRefTargetPos - mvTrackNewMousePos+mvTrackRefMousePos;
	cVector3f vNewCameraPos = vNewTargetPos - mvTrackCamTargetOffset;

	mpCam->SetTargetPosition(vNewTargetPos);
	mpCam->SetCameraPosition(vNewCameraPos);
}

//--------------------------------------------------------------------------------

void cCamControllerOrtho::Zoom(const cVector2f& avMouseRelPos)
{
	cCamera* pEngCam = mpCam->GetEngineCamera();
	iEdViewport* pViewport = mpCam->GetViewport();

	float fOrthoViewSize = pEngCam->GetOrthoViewSize().x;
	float fInvAspect = 1/pEngCam->GetAspect();

	fOrthoViewSize += avMouseRelPos.y;
	fOrthoViewSize = cMath::Clamp(fOrthoViewSize, 0.1f, fOrthoViewSize);
	pEngCam->SetOrthoViewSize(cVector2f(fOrthoViewSize, fOrthoViewSize*fInvAspect));
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM CONTROLLER MAYA - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

cCamControllerMaya::cCamControllerMaya(cEdCamera* apCam) : iEdWindowViewportCamController(apCam, _W("Maya"))
{
	mbIsTumbling = false;
	mbIsTracking = false;
	mbIsZooming = false;

	mfTumbleFactor = 0.005f;
	mfTrackFactor = 0.01f;
	mfZoomFactor = 0.01f;

	mbLockedToGrid = true;
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM CONTROLLER MAYA - PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cCamControllerMaya::LockToGrid(bool abX)
{
	mbLockedToGrid = abX;
}


//--------------------------------------------------------------------------------

bool cCamControllerMaya::IsActive()
{
	return mbIsTumbling || mbIsTracking || mbIsZooming;
}

//--------------------------------------------------------------------------------

bool cCamControllerMaya::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(IsViewModeActive())
	{
		if(IsActive()) return true;

		if(aClick.mbLeft)
		{
			mbIsTumbling = true;

			return true;
		}
		if(aClick.mbMid)
		{
			mbIsTracking = true;

			mvTrackRefCameraPos = mpCam->GetCameraPosition();
			mvTrackRefTargetPos = mpCam->GetTargetPosition();
			mvTrackCamTargetOffset = mvTrackRefTargetPos-mvTrackRefCameraPos;
			mvTrackRefMousePos = mpCam->GetViewport()->GetMouseWorldPosition(true);
			//mvTrackRefMousePos = mpCam->GetPointFromClampedDistance(mvTrackRefTargetPos, mvTrackRefMousePos, mpCam->mfTrackMouseMaxDistanceToTarget);

			return true;
		}
		if(aClick.mbRight)
		{
			mbIsZooming = true;
			return true;
		}
	}

	if(aClick.mbAlt==false && aClick.mbControl==false && aClick.mbShift==false)
	{
		if(aClick.mbWheelUp)
		{
			Zoom(-1 * 0.25f);
			mbForceUpdate = true;

			return true;
		}
		if(aClick.mbWheelDn)
		{
			Zoom(1 * 0.25f);
			mbForceUpdate = true;

			return true;
		}
	}

	return false;
}

bool cCamControllerMaya::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(IsActive())
	{
		mbIsTumbling = false;
		mbIsTracking = false;
		mbIsZooming = false;
	}
	/*
	if(aClick.mbLeft)
		mbIsTumbling = false;
	if(aClick.mbMid)
		mbIsTracking = false;
	if(aClick.mbRight)
		mbIsZooming = false;
	if(aClick.mbWheelUp)
		mbIsZooming = false;
	if(aClick.mbWheelDn)
		mbIsZooming = false;
		*/

	return false;
}
//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM CONTROLLER MAYA - PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cCamControllerMaya::OnUpdate(const cViewportClick& aClick)
{
	if(mbIsTumbling)
		Tumble(aClick.mvRelPos * mfTumbleFactor);
	if(mbIsTracking)
		Track(aClick.mvRelPos * mfTrackFactor);
	if(mbIsZooming)
		Zoom(aClick.mvRelPos * mfZoomFactor);
}

//--------------------------------------------------------------------------------

void cCamControllerMaya::Tumble(const cVector2f& avMouseRel)
{
	cVector2f vSphCoords = mpCam->GetCameraSphCoords();
	cCamera* pEngCam = mpCam->GetEngineCamera();
	const cVector3f vUp = pEngCam->GetUp();

	// Longitude
	if(vUp.y < kEpsilonf)
		vSphCoords.y -= avMouseRel.x;
	else
		vSphCoords.y += avMouseRel.x;

	// Latitude
	vSphCoords.x -= avMouseRel.y;

	mpCam->SetCameraSphCoords(vSphCoords);
}

//--------------------------------------------------------------------------------

void cCamControllerMaya::Track(const cVector2f& avMouseRel)
{
	cVector3f vTargetPosition = mpCam->GetTargetPosition();
	cVector3f vNewTargetPosition;

	iEdViewport* pViewport = mpCam->GetViewport();

	const cVector3f& vRayStart = mvTrackRefCameraPos;
	const cVector3f& vRayDir = pViewport->GetUnprojectedDir();
	cVector3f vRayEnd = vRayStart + vRayDir*pViewport->GetRayEndDistance();

	if(IsLockedToGrid())
	{
		cEdGrid* pGrid = pViewport->GetGrid();

		cMath::CheckPlaneLineIntersection(pGrid->GetPlane(), 
											vRayStart, vRayEnd, 
											&mvTrackNewMousePos, NULL);

		vNewTargetPosition = mvTrackRefTargetPos - mvTrackNewMousePos+mvTrackRefMousePos;
		vNewTargetPosition = pGrid->GetProjectedPosOnPlane(vNewTargetPosition);
	}
	else
	{
		cCamera* pEngCam = mpCam->GetEngineCamera();
		cVector3f vDisplacement = (pEngCam->GetUp()*avMouseRel.y + 
									pEngCam->GetRight()*(-avMouseRel.x));
		vNewTargetPosition = vTargetPosition + vDisplacement;
	}

	mpCam->SetTargetPosition(vNewTargetPosition);	
	mpCam->SetCameraPosition(vNewTargetPosition-mvTrackCamTargetOffset);
}

//--------------------------------------------------------------------------------

void cCamControllerMaya::Zoom(const cVector2f& avMouseRel)
{
	float fDistToTarget = mpCam->GetDistToTarget();

	fDistToTarget += avMouseRel.y;

	mpCam->SetDistToTarget(cMath::Clamp(fDistToTarget, 0.5f, fDistToTarget) );
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM MODE ORTHO - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

cCamModeOrtho::cCamModeOrtho(cEdCamera* apCam, int alID, const tWString& asName, ePlaneNormal aPlane,
							 const cVector3f& avRight, const cVector3f& avUp, const cVector3f& avForward) : iCamMode(apCam, alID, asName)
{
	mPlane = aPlane;
	mvRight = avRight;
	mvUp = avUp;
	mvForward = avForward;

	AddCamController(hplNew(cCamControllerOrtho,(mpCam)));

	AddRenderer(_W("Wireframe"), eRenderer_WireFrame);
}

//--------------------------------------------------------------------------------

void cCamModeOrtho::GetViewSpacePositionAndSize(const cVector3f& avWorldPosition, const cVector3f& avWorldSize, 
													 cVector3f& avViewSpacePosition, cVector3f& avViewSpaceSize)
{
	cCamera* pEngineCam = mpCam->GetEngineCamera();
	avViewSpacePosition = cMath::MatrixMul(pEngineCam->GetViewMatrix(), avWorldPosition);
	avViewSpaceSize = avWorldSize * pEngineCam->GetOrthoViewSize().x * 0.75f;
}

void cCamModeOrtho::FocusSpecific(const cVector3f& avCenter, const cVector3f& avDiff)
{
	float fWidth=-1;
	for(int i=ePlaneNormal_X; i<ePlaneNormal_LastEnum; ++i)
	{
		if(i==mPlane)
			continue;

		fWidth = cMath::Max(fWidth, avDiff.v[i]);
	}

	cVector3f vCamOffset = mpCam->GetCameraPosition()-mpCam->GetTargetPosition();

	mpCam->SetTargetPosition(avCenter);
	mpCam->SetCameraPosition(avCenter+vCamOffset);

	cCamera* pEngCam = mpCam->GetEngineCamera();
	iEdViewport* pViewport = mpCam->GetViewport();

	float fOrthoViewSize = pEngCam->GetOrthoViewSize().x;
	float fInvAspect = 1/pEngCam->GetAspect();

	fOrthoViewSize = fWidth*1.5f;
	fOrthoViewSize = cMath::Clamp(fOrthoViewSize, 0.1f, fOrthoViewSize);
	pEngCam->SetOrthoViewSize(cVector2f(fOrthoViewSize, fOrthoViewSize*fInvAspect));


	/*
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
	*/
	/*
	mvTargetPosition = vCenter;
	mvCameraPosition = mvTargetPosition - vCamOffset;

	mbTargetPosUpdated = true;
	mbCameraUpdated = true;

	SetZoom(fZoom);
	*/
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM MODE ORTHO - PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cCamModeOrtho::OnSetActive(bool abX)
{
	if(abX)
	{
		cCamera* pEngCam = mpCam->GetEngineCamera();
		pEngCam->SetProjectionType(eProjectionType_Orthographic);
		pEngCam->SetInifintiveFarPlane(true);
		pEngCam->SetOrthoViewSize(cVector2f(10, 10/pEngCam->GetAspect()));

		mpCam->SetKeepOrientation(true);
		mpCam->GetViewport()->GetGrid()->SetPlaneNormal(mPlane);
		mpCam->SetOrientation(mvRight, mvUp, mvForward);

		mpCam->SetTargetPosition(0);
		mpCam->SetCameraPosition(mvForward * mpCam->GetViewport()->GetRayEndDistance()*0.9f);

		SetCurrentRenderer(0);
	}
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM MODE PERSP - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

cCamModePerspective::cCamModePerspective(cEdCamera* apCam, int alID) : iCamMode(apCam, alID, _W("Perspective"))
{
	AddCamController(hplNew(cCamControllerMaya,(apCam)));

	AddRenderer(_W("Wireframe"), eRenderer_WireFrame);
	AddRenderer(_W("Shaded"), eRenderer_Main);
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM MODE PERSP - PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cCamModePerspective::GetViewSpacePositionAndSize(const cVector3f& avWorldPosition, const cVector3f& avWorldSize, 
													 cVector3f& avViewSpacePosition, cVector3f& avViewSpaceSize)
{
	cCamera* pEngineCam = mpCam->GetEngineCamera();
	avViewSpacePosition = cMath::MatrixMul(pEngineCam->GetViewMatrix(), avWorldPosition);
	avViewSpaceSize = avWorldSize * cMath::Abs(avViewSpacePosition.z);
}

void cCamModePerspective::FocusSpecific(const cVector3f& avCenter, const cVector3f& avDiff)
{
	float fMaxSideLength=-1;
	for(int i=0;i<3;++i)
	{
		if(avDiff.v[i] > fMaxSideLength)
			fMaxSideLength = avDiff.v[i];
	}

	cCamera* pEngCam = mpCam->GetEngineCamera();
	cVector3f vCamOffset = mpCam->GetCameraPosition() - mpCam->GetTargetPosition();
	cVector3f vDir = vCamOffset;
	vDir.Normalize();

	float fDistance = cMath::Abs((fMaxSideLength*0.75f)/tan(pEngCam->GetFOV()*0.5f));

	mpCam->SetTargetPosition(avCenter);
	mpCam->SetCameraPosition(avCenter+vDir*fDistance);
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CAM MODE PERSP - PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cCamModePerspective::OnSetActive(bool abX)
{
	if(abX)
	{
		mpCam->SetKeepOrientation(false);

		cCamera* pEngCam = mpCam->GetEngineCamera();
		pEngCam->SetProjectionType(eProjectionType_Perspective);
		pEngCam->SetInifintiveFarPlane(false);

		iEdViewport* pViewport = mpCam->GetViewport();
		
		cEdGrid* pGrid = pViewport->GetGrid();
		pGrid->SetPlaneNormal(ePlaneNormal_Y);

		mpCam->SetTargetPosition(0);
		mpCam->SetCameraPosition(cVector3f(10));

		SetCurrentRenderer(1);
	}
}




///////////////////////////////////////////////////////////////////
// VIEWPORT WINDOW - CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

cEdWindowViewport::cEdWindowViewport(iEditor* apEditor, iFrameBuffer* apFB): iEdViewport(apEditor, _W("Edition"), 
																				apEditor->GetWorld()->GetEngWorld(),
																				apFB)
{
	mbDrawGrid = true;
	mbDrawDebug = false;
	mbDrawAxes = true; 

	mpPrevAttention = NULL;

	mpCamera->AddCamMode(hplNew(cCamModeOrtho,(mpCamera, eEdCameraType_OrthoFront, _W("Front"), ePlaneNormal_Z, cVector3f(1,0,0), cVector3f(0,1,0), cVector3f(0,0,1))));
	mpCamera->AddCamMode(hplNew(cCamModeOrtho,(mpCamera, eEdCameraType_OrthoRight, _W("Right"), ePlaneNormal_X, cVector3f(0,0,-1), cVector3f(0,1,0), cVector3f(1,0,0))));
	mpCamera->AddCamMode(hplNew(cCamModeOrtho,(mpCamera, eEdCameraType_OrthoTop, _W("Top"),	ePlaneNormal_Y, cVector3f(1,0,0), cVector3f(0,0,-1), cVector3f(0,1,0))));
	mpCamera->AddCamMode(hplNew(cCamModePerspective, (mpCamera, eEdCameraType_Perspective)));

	SetMenu(hplNew(cEdViewMenu,(this)));
}

//----------------------------------------------------------------

cEdWindowViewport::~cEdWindowViewport()
{
}

//----------------------------------------------------------------

///////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////

//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

/*
void cEdWindowViewport::FocusOnSelection(cEdSelection* apSelection)
{
    if(apSelection==NULL || apSelection->IsEmpty())
		return;

	mCamera.FocusOnSelection(apSelection);
}
*/

//----------------------------------------------------------------

bool cEdWindowViewport::LoadState(cXmlElement* apElement)
{
	if(iEdViewport::LoadState(apElement)==false)
		return false;

	//SetPreset((eEdWindowViewportPreset)apElement->GetAttributeInt("Preset",0));
	//SetRenderMode((eRenderer)apElement->GetAttributeInt("RenderMode",0));
	SetDrawGrid(apElement->GetAttributeBool("ShowGrid", true));
	SetDrawAxes(apElement->GetAttributeBool("ShowAxes", true));

	//mpCamera->SetTargetPosition(apElement->GetAttributeVector3f("CameraTarget"));
	//mpCamera->SetCameraPosition(apElement->GetAttributeVector3f("CameraPosition"));
	//float fZoom = apElement->GetAttributeFloat("Zoom");
//	if(fZoom==0)
//		fZoom = mCamera.GetZoomFunc()->GetZoomFromDistToTarget((mCamera.GetCameraPosition()-mCamera.GetTargetPosition()).Length());

//	mpCamera->SetZoom(fZoom);
//	mCamera.LockToGrid(apElement->GetAttributeBool("UsingLTCam", true));

	//ePlaneNormal plane = mpGrid->GetPlaneNormal();

	//mpGrid->SetHeight(apElement->GetAttributeFloat("GridHeight"));
	//mpGrid->SetPlaneNormal((ePlaneNormal)apElement->GetAttributeInt("GridPlane", plane));

	return true;
}

//----------------------------------------------------------------

bool cEdWindowViewport::SaveState(cXmlElement* apElement, cXmlElement** apStateElem)
{
	cXmlElement* pStateElem;

	if(iEdViewport::SaveState(apElement, &pStateElem)==false)
		return false;

	pStateElem->SetAttributeBool("ShowGrid", GetDrawGrid());
	pStateElem->SetAttributeBool("ShowAxes", GetDrawAxes());

	if(apStateElem)
		*apStateElem = pStateElem;

	return true;
}

//----------------------------------------------------------------

iEdAction* cEdWindowViewport::CreateFocusAction()
{
	cEdEditModeSelect* pSelect = static_cast<cEdEditModeSelect*>(mpEditor->GetEditModeHandler()->GetMode(_W("Select")));
	cEdSelection* pSelection = pSelect->GetSelection();
	if(pSelection->HasComponents()==false)
		return NULL;

	const cVector3f& vHeights = pSelection->GetTranslation();
	bool bAtLeastOneHeightChanged = false;

	cEdActionCompound* pAction = hplNew(cEdActionCompound,(_W("Focus")));
	for(int i=0;i<mpEditor->GetNumViewports();++i)
	{
		cEdWindowViewport* pViewport = static_cast<cEdWindowViewport*>(mpEditor->GetViewport(i));
		cEdGrid* pGrid = pViewport->GetGrid();

		pAction->AddAction(pGrid->CreateSetHeightsAction(vHeights));

		pViewport->GetCamera()->FocusOnBoundingVolume(pSelection->GetBoundingVolume());
	}

	if(pAction->IsEmpty())
	{
		hplDelete(pAction);
		pAction = NULL;
	}

	return pAction;
}

//----------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

bool cEdWindowViewport::OnViewportUpdate(const cGuiMessageData& aData)
{
	iEdWindowViewportCamController* pController = static_cast<iEdWindowViewportCamController*>(mpCamera->GetCurrentController());

	if(pController->IsViewModeActive())
	{
		if(mpEditor->IsLockedToViewport()==false)
			mpEditor->LockToViewport(this);
	}
	else if(pController->IsActive()==false)
	{
		if(mpEditor->IsLockedToViewport())
			mpEditor->LockToViewport(NULL);
	}

	return true;
}

//----------------------------------------------------------------

bool cEdWindowViewport::OnViewportGotFocus(const cGuiMessageData& aData)
{
	return false;
}

//----------------------------------------------------------------

bool cEdWindowViewport::OnViewportLostFocus(const cGuiMessageData& aData)
{
	return false;
}

//----------------------------------------------------------------

bool cEdWindowViewport::OnViewportDraw(const cGuiMessageData& aData)
{
	//////////////////////////////////////////////
	// Draw red frame around selected viewport
	if(IsFocused()==false)
		return true;

	cVector3f vPos = GetBG()->GetGlobalPosition() + cVector3f(0,0,2);
	cVector2f vSize = GetBG()->GetSize();

	iEdWindowViewportCamController* pController = static_cast<iEdWindowViewportCamController*>(mpCamera->GetCurrentController());

	cColor color = (pController->IsViewModeActive())?cColor(1,1,0,1):cColor(1,0,0,1);

	mpSet->DrawGfx(mpGfxRedFrame, vPos, cVector2f(vSize.x,2), color);
	mpSet->DrawGfx(mpGfxRedFrame, vPos + cVector3f(0,vSize.y-2,0), cVector2f(vSize.x,2), color);
	mpSet->DrawGfx(mpGfxRedFrame, vPos, cVector2f(2,vSize.y), color);
	mpSet->DrawGfx(mpGfxRedFrame, vPos + cVector3f(vSize.x-2,0,0), cVector2f(2,vSize.y), color);
	
	return true;
}

//----------------------------------------------------------------

bool cEdWindowViewport::OnViewportMouseDown(const cGuiMessageData& aData)
{
	if(IsFocused()==false)
		return false;

	cViewportClick click = cViewportClick(this, aData);

	mpEditor->LockToViewport(this);

	if(mpCamera->GetCurrentController()->OnViewportMouseDown(click)==false)
		mpEditor->ViewportMouseDown(click);

	return true;
}

//----------------------------------------------------------------

bool cEdWindowViewport::OnViewportMouseUp(const cGuiMessageData& aData)
{
	cViewportClick click = cViewportClick(this, aData);

	mpEditor->LockToViewport(NULL);

	mpCamera->GetCurrentController()->OnViewportMouseUp(click);
	mpEditor->ViewportMouseUp(click);

	return true;
}

//----------------------------------------------------------------

bool cEdWindowViewport::OnViewportMouseMove(const cGuiMessageData& aData)
{
	if(IsFocused()==false && mpEditor->IsLockedToViewport()==false)
	{
		mpEditor->SetFocusedViewport(this);
		return true;
	}

	//cViewportClick click = cViewportClick(this, aData);

	//iCamController* pController = mpCamera->GetCurrentController();
	//if(pController)
	//	pController->Update(click);

	return true;
}

//----------------------------------------------------------------

bool cEdWindowViewport::OnViewportKeyPress(const cGuiMessageData& aData)
{
	return false;
}

//----------------------------------------------------------------

bool cEdWindowViewport::OnViewportKeyRelease(const cGuiMessageData& aData)
{
	return false;
}

//----------------------------------------------------------------

void cEdWindowViewport::OnSetLock(bool abX)
{
	if(abX)
		mpSet->SetAttentionWidget(mpImgViewport);
	else
		mpSet->SetAttentionWidget(NULL);
}

//----------------------------------------------------------------

void cEdWindowViewport::OnSetCamMode(iCamMode* apMode)
{
	iEdViewport::OnSetCamMode(apMode);

	mpPresetLabel->SetText(apMode->GetName());
	mpMenu->SetUpdated();
}

//----------------------------------------------------------------

void cEdWindowViewport::OnCreateLayout()
{
	iEdPane::OnCreateLayout();
	float fMenuHeight = 22;

	/////////////////////////////////////////////
	// Gfx Init
	mpGfxRedFrame = mpSet->GetGui()->CreateGfxFilledRect(cColor(1,1,1,1), eGuiMaterial_Diffuse);
	
	//mpGfxPointerTumble = mpSet->GetGui()->CreateGfxImage("gui_def_pointer_tumble.tga",eGuiMaterial_Alpha);
	//mpGfxPointerTrack = mpSet->GetGui()->CreateGfxImage("gui_def_pointer_track.tga",eGuiMaterial_Alpha);
	//mpGfxPointerZoom = mpSet->GetGui()->CreateGfxImage("gui_def_pointer_zoom.tga",eGuiMaterial_Alpha);
	
	/////////////////////////////////////////////
	// Layout init
	cWidgetFrame* pFrame = static_cast<cWidgetFrame*>(GetBG());
	pFrame->SetDrawFrame(true);

	mpPresetLabel = mpSet->CreateWidgetLabel(cVector3f(5,5+fMenuHeight,1),0,_W(""), GetBG());
	mpPresetLabel->SetDefaultFontColor(cColor(1,1));
	
	// Render view init
	CreateGuiViewport(GetBG());
	SetGuiViewportPos(cVector3f(0,fMenuHeight,0.05f));
}

//----------------------------------------------------------------

void cEdWindowViewport::OnSetSize(const cVector2f& avX)
{
	SetGuiViewportSize(GetBG()->GetSize());
}

//----------------------------------------------------------------

void cEdWindowViewport::OnPostSolidDraw(cRendererCallbackFunctions* apFunctions)
{
	if(mbDrawGrid)
	{
		mpGrid->Draw(apFunctions);
	}

	if(mbDrawAxes)
	{
		apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);

		const cVector3f& vCenter = GetGridCenter();
		for(int i=0;i<3;++i)
		{
			cColor col = cColor(0,0.5f);
			col.v[i] = 1;

			cVector3f vAxisStart = 0;
			vAxisStart.v[i] = vCenter.v[i] -1000.0f;

			cVector3f vAxisEnd = 0;
			vAxisEnd.v[i] = vCenter.v[i] +1000.0f;

			apFunctions->GetLowLevelGfx()->DrawLine(vAxisStart, vAxisEnd, col);
		}
	}

	cModuleDrawData drawData = cModuleDrawData(this, apFunctions, eEdDrawStep_PostSolid);
	mpEditor->BroadcastModuleMsg(eEdModuleMsg_Draw, &drawData);
}

//----------------------------------------------------------------

void cEdWindowViewport::OnPostTranslucentDraw(cRendererCallbackFunctions* apFunctions)
{
	cModuleDrawData drawData = cModuleDrawData(this, apFunctions, eEdDrawStep_PostTrans);
	mpEditor->BroadcastModuleMsg(eEdModuleMsg_Draw, &drawData);
}

//----------------------------------------------------------------
