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

//----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// VIEWPORT WINDOW - CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

cEdPEViewport::cEdPEViewport(iEditor* apEditor, iFrameBuffer* apFB): iEdViewport(apEditor, _W("Edition"), 
																				apEditor->GetWorld()->GetEngWorld(),
																				apFB)
{
	mbDrawGrid = false;
	mbDrawDebug = false;
	mbDrawAxes = false; 

	mpCamera->AddCamMode(hplNew(cCamModePerspective, (mpCamera, eEdCameraType_Perspective)));
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
	//////////////////////////////////////////////
	// Draw red frame around selected viewport
	if(IsFocused()==false)
		return true;

	cVector3f vPos = GetBG()->GetGlobalPosition() + cVector3f(0,0,2);
	cVector2f vSize = GetBG()->GetSize();

	//bool bViewMode =mpEditor->GetFlags(eEdFlag_ViewModeActive);
	//bool bViewLocked = mpEditor->GetFlags(eEdFlag_ViewLocked);

	cColor color = cColor(1,0,0,1);//(bViewMode && bViewLocked==false)?cColor(1,1,0,1):cColor(1,0,0,1);

	mpSet->DrawGfx(mpGfxRedFrame, vPos, cVector2f(vSize.x,2), color);
	mpSet->DrawGfx(mpGfxRedFrame, vPos + cVector3f(0,vSize.y-2,0), cVector2f(vSize.x,2), color);
	mpSet->DrawGfx(mpGfxRedFrame, vPos, cVector2f(2,vSize.y), color);
	mpSet->DrawGfx(mpGfxRedFrame, vPos + cVector3f(vSize.x-2,0,0), cVector2f(2,vSize.y), color);
	
	return true;
}

//----------------------------------------------------------------

void cEdPEViewport::OnPostSolidDraw(cRendererCallbackFunctions* apFunctions)
{
	//mpGrid->Draw(apFunctions, GetGridCenter());
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
}

//----------------------------------------------------------------

void cEdPEViewport::OnCreateLayout()
{
	iEdPane::OnCreateLayout();

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
}

//----------------------------------------------------------------

void cEdPEViewport::OnUpdate()
{
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
