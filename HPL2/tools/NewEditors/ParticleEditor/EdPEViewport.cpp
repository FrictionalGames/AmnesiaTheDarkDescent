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

#include "../ParticleEditor/EdPEViewport.h"

#include "../Common/EdCamera.h"

#include "../Common/Editor.h"
#include "../Common/EdGrid.h"
#include "../Common/EdWorld.h"
#include "../Common/EdWindowViewport.h"

//----------------------------------------------------------------

cCamModePE::cCamModePE(cEdCamera* apCam) : iCamMode(apCam, 0, _W("ParticleEditor"))
{
	AddCamController(hplNew(cCamControllerPE,(apCam)));

	AddRenderer(_W("Main"), eRenderer_Main);
}

//----------------------------------------------------------------

cCamControllerPE::cCamControllerPE(cEdCamera* apCam) : iCamController(apCam, _W("ParticleEditor"))
{
	mbOrbiting = false;
	mbTracking = false;
	mbZooming = false;
	mbWheelUp = false;
	mbWheelDn = false;
}

//----------------------------------------------------------------

bool cCamControllerPE::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(aClick.mbLeft) 
		mbOrbiting = true;
	if(aClick.mbMid)
		mbTracking = true;
	if(aClick.mbRight)
		mbZooming = true;
	if(aClick.mbWheelUp)
		mbWheelUp = true;
	if(aClick.mbWheelDn)
		mbWheelDn = true;

	return aClick.mbLeft || aClick.mbMid || aClick.mbRight || aClick.mbWheelDn || aClick.mbWheelUp;
}

//----------------------------------------------------------------

bool cCamControllerPE::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(aClick.mbLeft) 
		mbOrbiting = false;
	if(aClick.mbMid)
		mbTracking = false;
	if(aClick.mbRight)
		mbZooming = false;
	

	return aClick.mbLeft || aClick.mbMid;
}

//----------------------------------------------------------------

void cCamControllerPE::OnUpdate(const cViewportClick& aData)
{
	cVector2f vRel = aData.mvRelPos *0.01f;

	if(mbOrbiting)
	{
		cVector2f vSphCoords = mpCam->GetCameraSphCoords();
		cCamera* pEngCam = mpCam->GetEngineCamera();
		const cVector3f vUp = pEngCam->GetUp();

		// Longitude
		if(vUp.y < kEpsilonf)
			vSphCoords.y -= vRel.x;
		else
			vSphCoords.y += vRel.x;

		// Latitude
		vSphCoords.x -= vRel.y;

		mpCam->SetCameraSphCoords(vSphCoords);
	}
	if(mbTracking)
	{
		cVector2f vMouseRel = aData.mvRelPos *0.05f;
		cVector3f vTargetPosition = mpCam->GetTargetPosition();
		cVector3f vOffset = vTargetPosition-mpCam->GetCameraPosition();
		cVector3f vNewTargetPosition;

		cCamera* pEngCam = mpCam->GetEngineCamera();
		cVector3f vDisplacement = (pEngCam->GetUp()*vMouseRel.y + 
									pEngCam->GetRight()*(-vMouseRel.x));
		vNewTargetPosition = vTargetPosition + vDisplacement;

		mpCam->SetTargetPosition(vNewTargetPosition);	
		mpCam->SetCameraPosition(vNewTargetPosition-vOffset);
	}
	if(mbZooming)
	{
		mpCam->SetDistToTarget(mpCam->GetDistToTarget()+vRel.y*2.5f);
	}
	if(mbWheelUp)
	{
		mbWheelUp = false;
		mpCam->SetDistToTarget(mpCam->GetDistToTarget()-0.5f);
	}
	if(mbWheelDn)
	{
		mbWheelDn = false;
		mpCam->SetDistToTarget(mpCam->GetDistToTarget()+0.5f);
	}
}

//----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// VIEWPORT WINDOW - CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

cEdPEViewport::cEdPEViewport(iEditor* apEditor, iFrameBuffer* apFB): iEdViewport(apEditor, _W("Edition"), 
																				apEditor->GetWorld()->GetEngWorld(),
																				apFB)
{
	mbDrawGrid = true;
	mbDrawDebug = false;
	mbDrawAxes = false; 

	mpCamera->AddCamMode(hplNew(cCamModePE,(mpCamera)));
	mpGrid->SetPlaneNormal(ePlaneNormal_Y);
}

//----------------------------------------------------------------

cEdPEViewport::~cEdPEViewport()
{
}

//----------------------------------------------------------------

///////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////

//----------------------------------------------------------------

void cEdPEViewport::Load(cXmlElement* apElement)
{
	/*
	SetPreset((eEdWindowViewportPreset)apElement->GetAttributeInt("Preset",0));
	SetRenderMode((eRenderer)apElement->GetAttributeInt("RenderMode",0));
	SetDrawGrid(apElement->GetAttributeBool("ShowGrid", true));
	SetDrawAxes(apElement->GetAttributeBool("ShowAxes", true));

	mpCamera->SetTargetPosition(apElement->GetAttributeVector3f("CameraTarget"));
	mpCamera->SetCameraPosition(apElement->GetAttributeVector3f("CameraPosition"));
	float fZoom = apElement->GetAttributeFloat("Zoom");
//	if(fZoom==0)
//		fZoom = mCamera.GetZoomFunc()->GetZoomFromDistToTarget((mCamera.GetCameraPosition()-mCamera.GetTargetPosition()).Length());

//	mpCamera->SetZoom(fZoom);
//	mCamera.LockToGrid(apElement->GetAttributeBool("UsingLTCam", true));

	ePlaneNormal plane = mpGrid->GetPlaneNormal();

	mpGrid->SetHeight(apElement->GetAttributeFloat("GridHeight"));
	mpGrid->SetPlaneNormal((ePlaneNormal)apElement->GetAttributeInt("GridPlane", plane));
	*/
}

//----------------------------------------------------------------

void cEdPEViewport::Save(cXmlElement* apElement)
{
	/*
	apElement->SetAttributeInt("RenderMode", (int)mRenderMode);
	apElement->SetAttributeInt("Preset", (int)mPreset);
	apElement->SetAttributeBool("ShowGrid", GetDrawGrid());
	apElement->SetAttributeBool("ShowAxes", GetDrawAxes());

	apElement->SetAttributeVector3f("CameraPosition", mpCamera->GetCameraPosition());
	apElement->SetAttributeVector3f("CameraTarget", mpCamera->GetTargetPosition());
//	apElement->SetAttributeFloat("CameraZoom", mpCamera->GetZoom());

	apElement->SetAttributeFloat("GridHeight", mpGrid->GetHeight());
	apElement->SetAttributeInt("GridPlane", (int)mpGrid->GetPlaneNormal());

//	apElement->SetAttributeBool("UsingLTCam", mCamera.IsLockedToGrid());
*/
}

//----------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

bool cEdPEViewport::OnViewportUpdate(const cGuiMessageData& aData)
{
	return true;
}

//----------------------------------------------------------------

bool cEdPEViewport::OnViewportGotFocus(const cGuiMessageData& aData)
{
	return false;
}

//----------------------------------------------------------------

bool cEdPEViewport::OnViewportLostFocus(const cGuiMessageData& aData)
{
	return false;
}

//----------------------------------------------------------------

bool cEdPEViewport::OnViewportDraw(const cGuiMessageData& aData)
{
	return true;
}

//----------------------------------------------------------------

void cEdPEViewport::OnPostSolidDraw(cRendererCallbackFunctions* apFunctions)
{
	if(mbDrawGrid)
	{
		mpGrid->Draw(apFunctions);
	}

	if(mbDrawAxes)
	{
		apFunctions->SetBlendMode(eMaterialBlendMode_None);

		const cVector3f& vCenter = GetGridCenter();
		for(int i=0;i<3;++i)
		{
			cColor col = cColor(0,1);
			col.v[i] = 1;

			cVector3f vAxisStart = 0;
			vAxisStart.v[i] = vCenter.v[i] -1000.0f;

			cVector3f vAxisEnd = 0;
			vAxisEnd.v[i] = vCenter.v[i] +1000.0f;

			apFunctions->GetLowLevelGfx()->DrawLine(vAxisStart, vAxisEnd, col);
		}
	}
}

//----------------------------------------------------------------

bool cEdPEViewport::OnViewportMouseDown(const cGuiMessageData& aData)
{
	if(mpImgViewport->GetMouseIsOver()==false &&
		mpSet->GetAttentionWidget()!=mpImgViewport)
		return false;

	cViewportClick click = cViewportClick(this, aData);

	mpEditor->LockToViewport(this);
	mpSet->SetAttentionWidget(mpImgViewport);

	mpCamera->GetCurrentController()->OnViewportMouseDown(click);

	return true;
}

//----------------------------------------------------------------

bool cEdPEViewport::OnViewportMouseUp(const cGuiMessageData& aData)
{
	cViewportClick click = cViewportClick(this, aData);

	mpEditor->LockToViewport(NULL);
	mpSet->SetAttentionWidget(NULL);

	mpCamera->GetCurrentController()->OnViewportMouseUp(click);

	return true;
}

//----------------------------------------------------------------

bool cEdPEViewport::OnViewportMouseMove(const cGuiMessageData& aData)
{
	cViewportClick click = cViewportClick(this, aData);

	iCamController* pController = mpCamera->GetCurrentController();
	if(pController)
		pController->Update(click);

	return true;
}

//----------------------------------------------------------------

bool cEdPEViewport::OnViewportKeyPress(const cGuiMessageData& aData)
{
	return false;
}

//----------------------------------------------------------------

bool cEdPEViewport::OnViewportKeyRelease(const cGuiMessageData& aData)
{
	return false;
}

//----------------------------------------------------------------

void cEdPEViewport::OnSetCamMode(iCamMode* apMode)
{
	iEdViewport::OnSetCamMode(apMode);
}

//----------------------------------------------------------------

void cEdPEViewport::OnCreateLayout()
{
	iEdViewport::OnCreateLayout();

	/////////////////////////////////////////////
	// Gfx Init
	mpGfxRedFrame = mpSet->GetGui()->CreateGfxFilledRect(cColor(1,1,1,1), eGuiMaterial_Diffuse);
	
	mpGfxPointerTumble = mpSet->GetGui()->CreateGfxImage("gui_def_pointer_tumble.tga",eGuiMaterial_Alpha);
	mpGfxPointerTrack = mpSet->GetGui()->CreateGfxImage("gui_def_pointer_track.tga",eGuiMaterial_Alpha);
	mpGfxPointerZoom = mpSet->GetGui()->CreateGfxImage("gui_def_pointer_zoom.tga",eGuiMaterial_Alpha);
	
	/////////////////////////////////////////////
	// Layout init
	cWidgetFrame* pFrame = static_cast<cWidgetFrame*>(GetBG());
	pFrame->SetDrawFrame(true);

	mpPresetLabel = mpSet->CreateWidgetLabel(cVector3f(5,5,1),0,_W(""), GetBG());
	mpPresetLabel->SetDefaultFontColor(cColor(1,1));
	
	// Render view init
	CreateGuiViewport(GetBG());
	SetGuiViewportPos(cVector3f(0,0,0.05f));
}

//----------------------------------------------------------------

void cEdPEViewport::OnReset()
{
	mpCamera->SetCameraMode(0);
	mpCamera->SetCameraPosition(10);
	mpCamera->UpdateEngineCamera();

	SetDrawAxes(false);
	SetDrawGrid(true);

	mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldParamsChange, NULL);
}

//----------------------------------------------------------------

void cEdPEViewport::OnUpdate()
{
	iEdViewport::OnUpdate();
}

//-------------------------------------------------------

void cEdPEViewport::OnSetSize(const cVector2f& avX)
{
	SetGuiViewportSize(GetBG()->GetSize());
}

//----------------------------------------------------------------

void cEdPEViewport::OnSetActive(bool abX)
{
	SetViewportActive(abX);
}

//----------------------------------------------------------------
