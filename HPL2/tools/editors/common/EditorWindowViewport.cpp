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

#include "EditorWindowViewport.h"

#include "EditorBaseClasses.h"
#include "EditorGrid.h"
#include "EditorClipPlane.h"
#include "EditorEditMode.h"
#include "EditorWorld.h"
#include "EditorSelection.h"
#include "SurfacePicker.h"

#include "EntityWrapper.h"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////
// VIEWPORT CALLBACK
///////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
//--------------------------------------------------------------------

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

cViewportCallback::cViewportCallback()
{
	mpEditor = NULL;
	mpViewport = NULL;
}

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

void cViewportCallback::OnPostSolidDraw(cRendererCallbackFunctions* apFunctions)
{
	if(mpEditor==NULL)
		return;

	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	this->mpEditor->GetEditorWorld()->GetSurfacePicker()->DrawDebug(apFunctions);

	if(mpViewport->GetDrawGrid())
	{
		cEditorGrid* pGrid = mpViewport->GetGrid();
		if(pGrid) pGrid->Draw(apFunctions,mpViewport->GetGridCenter());
	}
	if(mpViewport->GetDrawAxes())
	{
		const cVector3f& vCenter = mpViewport->GetGridCenter();
		for(int i=0;i<3;++i)
		{
			cColor col = cColor(0,1);
			col.v[i] = 1;

			cVector3f vAxisStart = 0;
			cVector3f vAxisEnd = 0;
			vAxisStart.v[i] = vCenter.v[i] -1000.0f;
			vAxisEnd.v[i] = vCenter.v[i] +1000.0f;
			apFunctions->GetLowLevelGfx()->DrawLine(vAxisStart, vAxisEnd, col);
		}
	}
	tEditorClipPlaneVec& vClipPlanes = mpEditor->GetEditorWorld()->GetClipPlanes();
	for(int i=0;i<(int)vClipPlanes.size();++i)
	{
		vClipPlanes[i]->Draw(apFunctions, 0);
	}

	apFunctions->GetLowLevelGfx()->DrawSphere(mpViewport->GetVCamera()->GetTargetPosition(),0.1f, cColor(0,1,1,1));

	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(false);
	apFunctions->SetDepthWrite(false);

	//apFunctions->GetLowLevelGfx()->DrawSphere(mpEditor->,0.1f, cColor(0,1,0,1));
	const cVector3f& vRefMousePos = mpViewport->GetVCamera()->GetTrackRefMousePos();
	const cVector3f& vMouseNewPos = mpViewport->GetVCamera()->GetTrackNewMousePos();
	cVector3f vMousePos = mpViewport->GetMouseWorldPosition();
	//mpViewport->GetVCamera()->ClampDistanceFromRefTarget(vMousePos, 40);
	
	//apFunctions->GetLowLevelGfx()->DrawSphere(vMousePos, 0.1f, cColor(0,1,0,1));
	
	//apFunctions->GetLowLevelGfx()->DrawLine(vRefMousePos, vMouseNewPos, cColor(1,0,0,1));
	//apFunctions->GetLowLevelGfx()->DrawLine(vRefMousePos, 0, cColor(0,0,1,1));
	//apFunctions->GetLowLevelGfx()->DrawLine(mpViewport->GetVCamera()->GetTrackRefMousePos(), mpViewport->GetVCamera()->GetTrackRefTargetPos()-mpViewport->GetVCamera()->GetTrackRefMousePos(), cColor(1,0,0,1));
	//apFunctions->GetLowLevelGfx()->DrawLine(mpViewport->GetVCamera()->GetTrackRefTargetPos(), mpViewport->GetVCamera()->GetTrackNewMousePos(), cColor(1,0,0,1));
	//apFunctions->GetLowLevelGfx()->DrawLine(mpViewport->GetVCamera()->GetTrackRefTargetPos(), vMousePos, cColor(0,1,0,1));

	if(mpViewport->GetDrawDebug())
	{
		apFunctions->SetBlendMode(eMaterialBlendMode_Add);

		// Quick temp debug code btw
		cVector3f& vPos1 = mpViewport->vDebugLineStart;
		cVector3f& vPos2 = mpViewport->vDebugLineEnd;
		cVector3f& vGridPos = mpViewport->vDebugGridPos;
		cVector3f& vSnapPos = mpViewport->vDebugSnappedGridPos;

		apFunctions->GetLowLevelGfx()->DrawLine(vPos1,vPos2,cColor(0,0,1,1));
		apFunctions->GetLowLevelGfx()->DrawSphere(vPos1, 0.01f, cColor(0,1,0,1));
		apFunctions->GetLowLevelGfx()->DrawSphere(vPos2, 0.2f, cColor(0,1,0,1));
		apFunctions->GetLowLevelGfx()->DrawSphere(vGridPos, 0.3f, cColor(1,0,0,1));
		apFunctions->GetLowLevelGfx()->DrawSphere(vSnapPos, 0.3f, cColor(1,1,0,1));
		
		//apFunctions->GetLowLevelGfx()->DrawSphere(m->GetTarget(),0.1f, cColor(0,1,1,1));
	}
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
}

//--------------------------------------------------------------------

void cViewportCallback::OnPostTranslucentDraw(cRendererCallbackFunctions* apFunctions)
{
	if(mpViewport==NULL)
		return;

	cVector3f& vMousePos = mpEditor->GetPosOnGridFromMousePos();

	apFunctions->SetMatrix(NULL);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	iEditorEditMode* pEditMode = mpEditor->GetCurrentEditMode();

	if(pEditMode)
		pEditMode->DrawPreGrid(mpViewport, apFunctions, vMousePos);

	apFunctions->SetBlendMode(eMaterialBlendMode_None);

	if(pEditMode)
		pEditMode->DrawPostGrid(mpViewport, apFunctions, vMousePos);
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
//--------------------------------------------------------------------
///////////////////////////////////////////////////////////////////
// VIEWPORT WINDOW
///////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
//--------------------------------------------------------------------

//----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

cEditorWindowViewport::cEditorWindowViewport(iEditorBase* apEditor, 
											 iFrameBuffer* apFB,
											 bool abAddViewMenu):	iEditorWindow(apEditor,
																				  "Viewport Window"),
																	iEditorViewport(apEditor,
																					apEditor->GetEditorWorld()->GetWorld(),
																					apFB)
{
	mbDrawGrid = true;
	mbDrawDebug = false;
	mbDrawAxes = true; 
	mbAddViewMenu = abAddViewMenu;

	////////////////////////////////////
	// Init renderer callback (grid,...)
	mViewportCallback.mpEditor = apEditor;
	mViewportCallback.mpViewport = this;
	AddViewportCallback(&mViewportCallback);

	vDebugGridPos = 0;
	vDebugLineEnd = 0;
	vDebugLineStart = 0;
	vDebugSnappedGridPos = 0;

	mpMainMenu = NULL;
	mpPrevAttention = NULL;

	mpShortcutToggleGrid = NULL;
}

//----------------------------------------------------------------

cEditorWindowViewport::~cEditorWindowViewport()
{
}

//----------------------------------------------------------------

///////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////

//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

void cEditorWindowViewport::SetPreset(eEditorWindowViewportPreset aPreset)
{
	tWString sLabel;
	cVector3f vCameraPos;
	cVector3f vTargetPos = 0;

	eRenderer renderMode = eRenderer_WireFrame;

	mPreset = aPreset;
	switch(mPreset)
	{
	case eEditorWindowViewportPreset_Front:
		sLabel = _W("Front");
		vCameraPos = cVector3f(0,0,0.9f*mfRayEndDistance);
		break;
	case eEditorWindowViewportPreset_Right:
		sLabel = _W("Right");
		vCameraPos = cVector3f(0.9f*mfRayEndDistance,0,0);
		break;
	case eEditorWindowViewportPreset_Top:
		sLabel = _W("Top");
		vCameraPos = cVector3f(0,0.9f*mfRayEndDistance,0);
		break;
	case eEditorWindowViewportPreset_Perspective:
		sLabel = _W("Perspective");
		vCameraPos = cVector3f(10);
		renderMode = eRenderer_Main;
		break;
	}
	mpPresetLabel->SetText(sLabel);
	mCamera.SetCameraType((eEditorCameraType)mPreset);
	mCamera.SetCameraPosition(vCameraPos);
	mCamera.SetTargetPosition(vTargetPos);
	mCamera.SetZoom(0,true);
	mCamera.UpdateCamera(0);

	SetRenderMode(renderMode);
	
	UpdateMenu();
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------

void cEditorWindowViewport::SetFocused(bool abX)
{
	if(abX==false)
	{
		mCamera.SetTumbleActive(false);
		mCamera.SetTrackActive(false);
		mCamera.SetZoomActive(false);
	}

	if(mpShortcutToggleGrid)
		mpShortcutToggleGrid->SetEnabled(abX);
}

//----------------------------------------------------------------

bool cEditorWindowViewport::IsFocused()
{
	return mpEditor->GetFocusedViewport()==this;
}

//----------------------------------------------------------------

void cEditorWindowViewport::LookAtEntity(iEntityWrapper* apEntity)
{
	if(apEntity)
	{
		mCamera.SetTargetPosition(apEntity->GetPosition());
	}
}

//----------------------------------------------------------------

void cEditorWindowViewport::FocusOnSelection(cEditorSelection* apSelection)
{
    if(apSelection==NULL || apSelection->IsEmpty())
		return;

	mCamera.FocusOnSelection(apSelection);
}

//----------------------------------------------------------------

void cEditorWindowViewport::Load(cXmlElement* apElement)
{
	SetPreset((eEditorWindowViewportPreset)apElement->GetAttributeInt("Preset",0));
	SetRenderMode((eRenderer)apElement->GetAttributeInt("RenderMode",0));
	SetDrawGrid(apElement->GetAttributeBool("ShowGrid", true));
	SetDrawAxes(apElement->GetAttributeBool("ShowAxes", true));

	mCamera.SetTargetPosition(apElement->GetAttributeVector3f("CameraTarget"));
	mCamera.SetCameraPosition(apElement->GetAttributeVector3f("CameraPosition"));
	float fZoom = apElement->GetAttributeFloat("Zoom");
	if(fZoom==0)
		fZoom = mCamera.GetZoomFunc()->GetZoomFromDistToTarget((mCamera.GetCameraPosition()-mCamera.GetTargetPosition()).Length());

	mCamera.SetZoom(fZoom);
	mCamera.LockToGrid(apElement->GetAttributeBool("UsingLTCam", true));

	mCamera.UpdateCamera(0);

	ePlaneNormal plane = mpGrid->GetPlaneNormal();

	mpGrid->SetHeight(apElement->GetAttributeFloat("GridHeight"));
	mpGrid->SetPlaneNormal((ePlaneNormal)apElement->GetAttributeInt("GridPlane", plane));
}

//----------------------------------------------------------------

void cEditorWindowViewport::Save(cXmlElement* apElement)
{
	apElement->SetAttributeInt("RenderMode", (int)mRenderMode);
	apElement->SetAttributeInt("Preset", (int)mPreset);
	apElement->SetAttributeBool("ShowGrid", GetDrawGrid());
	apElement->SetAttributeBool("ShowAxes", GetDrawAxes());

	apElement->SetAttributeVector3f("CameraPosition", mCamera.GetCameraPosition());
	apElement->SetAttributeVector3f("CameraTarget", mCamera.GetTargetPosition());
	apElement->SetAttributeFloat("CameraZoom", mCamera.GetZoom());

	apElement->SetAttributeFloat("GridHeight", mpGrid->GetHeight());
	apElement->SetAttributeInt("GridPlane", (int)mpGrid->GetPlaneNormal());

	apElement->SetAttributeBool("UsingLTCam", mCamera.IsLockedToGrid());
}

//----------------------------------------------------------------

//----------------------------------------------------------------

void cEditorWindowViewport::SetEnlarged(bool abX)
{
	cVector3f vPos;
	cVector2f vSize;
	cVector2l vFBPos;
	cVector2l vFBSize;

	if(abX)
	{
		vPos = mvEnlargedPosition;
		vSize = mvEnlargedSize;
		vFBPos = cVector2l(0);
		vFBSize = mpFB->GetSize();		
	}
	else
	{
		vPos = mvNormalPosition;
		vSize = mvNormalSize;
		vFBPos = mvNormalFBPos;
		vFBSize = mvNormalFBSize;
	}

	SetEngineViewportPositionAndSize(vFBPos, vFBSize);
	SetPosition(vPos);
	SetSize(vSize);
	UpdateViewport();
}


//----------------------------------------------------------------

//----------------------------------------------------------------
//----------------------------------------------------------------

//----------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

bool cEditorWindowViewport::MenuView_Presets(iWidget *apWidget, const cGuiMessageData& aData)
{
	tWString sLabel = apWidget->GetText();
	eEditorWindowViewportPreset preset = eEditorWindowViewportPreset_Perspective;

	if(sLabel==_W("Front"))
	{
		preset = eEditorWindowViewportPreset_Front;
	}
    else if(sLabel==_W("Right"))
	{
		preset = eEditorWindowViewportPreset_Right;
	}
	else if(sLabel==_W("Top"))
	{
		preset = eEditorWindowViewportPreset_Top;
	}

	SetPreset(preset);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowViewport, MenuView_Presets);

//----------------------------------------------------------------

bool cEditorWindowViewport::MenuView_Rendering(iWidget *apWidget, const cGuiMessageData& aData)
{
	///////////////////////////////////////////////////
	// Set render mode
	cWidgetMenuItem* pItem = (cWidgetMenuItem*) apWidget;

	for(int i=0;i<eRenderer_LastEnum;++i)
	{
		bool bSelected = (pItem==mpMainMenuRenderModes[i]);

		if(bSelected)
			SetRenderMode((eRenderer)i);

		mpMainMenuRenderModes[i]->SetChecked(bSelected);
	}
	UpdateMenu();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowViewport, MenuView_Rendering);

//----------------------------------------------------------------

bool cEditorWindowViewport::MenuView_ShowGrid(iWidget *apWidget, const cGuiMessageData& aData)
{
	cWidgetMenuItem* pItem = (cWidgetMenuItem*) apWidget;

	SetDrawGrid(!pItem->IsChecked());

	UpdateMenu();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowViewport, MenuView_ShowGrid);

//----------------------------------------------------------------

bool cEditorWindowViewport::MenuView_ShowAxes(iWidget *apWidget, const cGuiMessageData& aData)
{
	cWidgetMenuItem* pItem = (cWidgetMenuItem*) apWidget;

	SetDrawAxes(!pItem->IsChecked());

	UpdateMenu();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowViewport, MenuView_ShowAxes);

//----------------------------------------------------------------

void cEditorWindowViewport::UpdateMenu()
{
	if(mpMainMenu==NULL)
		return;

	if(mCamera.IsOrtho())
		mpMainMenuRenderModes[0]->SetEnabled(false);
	else
		mpMainMenuRenderModes[0]->SetEnabled(true);

	for(int i=0;i<2;++i)
		mpMainMenuRenderModes[i]->SetChecked(mRenderMode==i);

    mpMainMenuShowGrid->SetChecked(GetDrawGrid());
	mpMainMenuShowAxes->SetChecked(GetDrawAxes());
}

//----------------------------------------------------------------

bool cEditorWindowViewport::OnViewportUpdate(const cGuiMessageData& aData)
{
	if(IsFocused()==false || mpEditor->GetFlags(eEditorFlag_ViewModeActive)==false)
	{
		mCamera.SetTumbleActive(false);
		mCamera.SetTrackActive(false);
		mCamera.SetZoomActive(false);
	}

	mpEditor->SetFlags(eEditorFlag_ViewModeActive, 
			mpEditor->GetEngine()->GetInput()->GetKeyboard()->KeyIsDown(eKey_LeftAlt)
#if defined(__linux__)
			|| mpEditor->GetEngine()->GetInput()->GetKeyboard()->KeyIsDown(eKey_LeftSuper)
			|| mpEditor->GetEngine()->GetInput()->GetKeyboard()->KeyIsDown(eKey_Z)
#endif
	);

	return true;
}

//----------------------------------------------------------------

bool cEditorWindowViewport::OnViewportGotFocus(const cGuiMessageData& aData)
{
	return false;
}

//----------------------------------------------------------------

bool cEditorWindowViewport::OnViewportLostFocus(const cGuiMessageData& aData)
{
	return false;
}

//----------------------------------------------------------------

bool cEditorWindowViewport::OnViewportDraw(const cGuiMessageData& aData)
{
	//////////////////////////////////////////////
	// Draw red frame around selected viewport
	if(IsFocused()==false)
		return true;

	cVector3f vPos = mpBGFrame->GetGlobalPosition() + cVector3f(0,0,2);
	cVector2f vSize = mpBGFrame->GetSize();

	bool bViewMode =mpEditor->GetFlags(eEditorFlag_ViewModeActive);
	bool bViewLocked = mpEditor->GetFlags(eEditorFlag_ViewLocked);

	cColor color = (bViewMode && bViewLocked==false)?cColor(1,1,0,1):cColor(1,0,0,1);

	mpSet->DrawGfx(mpGfxRedFrame, vPos, cVector2f(vSize.x,2), color);
	mpSet->DrawGfx(mpGfxRedFrame, vPos + cVector3f(0,vSize.y-2,0), cVector2f(vSize.x,2), color);
	mpSet->DrawGfx(mpGfxRedFrame, vPos, cVector2f(2,vSize.y), color);
	mpSet->DrawGfx(mpGfxRedFrame, vPos + cVector3f(vSize.x-2,0,0), cVector2f(2,vSize.y), color);
	
	return true;
}

//----------------------------------------------------------------

bool cEditorWindowViewport::OnViewportMouseDown(const cGuiMessageData& aData)
{
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

		return true;
	}

	if(mpImgViewport->GetMouseIsOver()==false && mpSet->GetAttentionWidget()!=mpImgViewport)
		return false;

	mpEditor->SetViewportLocked(true);
	mpSet->SetAttentionWidget(mpImgViewport);

	bool bViewLocked = mpEditor->GetFlags(eEditorFlag_ViewLocked);
	bool bViewMode = mpEditor->GetFlags(eEditorFlag_ViewModeActive);
	bool bPlacing = mpEditor->GetFlags(eEditorFlag_Placing);
	bool bEditing =	mpEditor->GetFlags(eEditorFlag_Editing);

	bool bTumble = false;
	bool bTrack = false;
	bool bZoom = false;

	if(bViewMode)
	{
		if(bViewLocked==false)
		{
			bTumble = (aData.mlVal&eGuiMouseButton_Left)!=0;
			bTrack = (aData.mlVal&eGuiMouseButton_Middle)!=0;
			bZoom = (aData.mlVal&eGuiMouseButton_Right)!=0;
		}
	}
	else
	{
		if(bPlacing)
			bTumble = bViewMode;

		mpEditor->ViewportMouseDown(this, aData.mlVal);
	}

	mCamera.SetTumbleActive(bTumble);
	mCamera.SetTrackActive(bTrack);
	mCamera.SetZoomActive(bZoom);

	return true;
}

//----------------------------------------------------------------

bool cEditorWindowViewport::OnViewportMouseUp(const cGuiMessageData& aData)
{
	mpEditor->SetViewportLocked(false);
	mpSet->SetAttentionWidget(NULL);

	if(mpEditor->GetFlags(eEditorFlag_ViewLocked) || 
		mpEditor->GetFlags(eEditorFlag_ViewModeActive)==false)
		mpEditor->ViewportMouseUp(this, aData.mlVal);
	
	mCamera.SetTumbleActive(false);
	mCamera.SetZoomActive(false);
	mCamera.SetTrackActive(false);	

	return true;
}

//----------------------------------------------------------------

bool cEditorWindowViewport::OnViewportMouseMove(const cGuiMessageData& aData)
{
	if(IsFocused()==false && mpSet->GetAttentionWidget()==NULL)
	{
		mpEditor->SetFocusedViewport(this);
		return true;
	}

	if(mpSet->GetFocusedWidget()!=mpImgViewport || 
		mpEditor->GetFlags(eEditorFlag_PopUpActive))
		return false;

	return true;
}

//----------------------------------------------------------------

bool cEditorWindowViewport::OnViewportKeyPress(const cGuiMessageData& aData)
{
	if(IsFocused()==false)
		return false;

	return false;
}

//----------------------------------------------------------------

bool cEditorWindowViewport::OnViewportKeyRelease(const cGuiMessageData& aData)
{
	return false;
}

//----------------------------------------------------------------

void cEditorWindowViewport::OnInitLayout()
{
	cWidgetMenuItem* pItem = NULL;
	cWidgetMenuItem* pSubItem1 = NULL;
	cWidgetMenuItem* pSubItem2 = NULL;

	/////////////////////////////////////////////
	// Gfx Init
	mpGfxRedFrame = mpSet->GetGui()->CreateGfxFilledRect(cColor(1,1,1,1), eGuiMaterial_Diffuse);
	
	mpGfxPointerTumble = mpSet->GetGui()->CreateGfxImage("gui_def_pointer_tumble.tga",eGuiMaterial_Alpha);
	mpGfxPointerTrack = mpSet->GetGui()->CreateGfxImage("gui_def_pointer_track.tga",eGuiMaterial_Alpha);
	mpGfxPointerZoom = mpSet->GetGui()->CreateGfxImage("gui_def_pointer_zoom.tga",eGuiMaterial_Alpha);
	
	/////////////////////////////////////////////
	// Layout init
	mpBGFrame->SetDrawFrame(true);
	//mpBGFrame->AddCallback(eGuiMessage_MouseEnter, this, kGuiCallback(Viewport_OnMouseEnter));

	mpPresetLabel = mpSet->CreateWidgetLabel(cVector3f(5,30,1),cVector2f(50,20),_W(""), mpBGFrame);
	mpPresetLabel->SetDefaultFontColor(cColor(1,1));

	cVector2f vMenuSize = 0;
	// Menu Init
	if(mbAddViewMenu)
	{
		mpMainMenu = mpSet->CreateWidgetMainMenu(mpBGFrame);

		pItem = mpMainMenu->AddMenuItem(_W("View"));
		mpMainMenuView = pItem;

		pSubItem1 = pItem->AddMenuItem(_W("Render mode"));
		mpMainMenuRenderModes[0] = pSubItem1->AddMenuItem(_W("Shaded"));
		mpMainMenuRenderModes[1] = pSubItem1->AddMenuItem(_W("Wireframe"));

		for(int i=0;i<2;++i)
			mpMainMenuRenderModes[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MenuView_Rendering));

		
		pSubItem1 = pItem->AddMenuItem(_W("Presets"));

		pSubItem2 = pSubItem1->AddMenuItem(_W("Front"));
		pSubItem2->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MenuView_Presets));

		pSubItem2 = pSubItem1->AddMenuItem(_W("Right"));
		pSubItem2->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MenuView_Presets));

		pSubItem2 = pSubItem1->AddMenuItem(_W("Top"));
		pSubItem2->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MenuView_Presets));

		pSubItem2 = pSubItem1->AddMenuItem(_W("Perspective"));
		pSubItem2->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MenuView_Presets));

		mpMainMenuShowGrid = pItem->AddMenuItem(_W("Show Grid"));
		mpMainMenuShowGrid->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MenuView_ShowGrid));
		mpShortcutToggleGrid = mpMainMenuShowGrid->AddShortcut(eKeyModifier_None, eKey_G);
		mpShortcutToggleGrid->SetEnabled(false);


		mpMainMenuShowAxes = pItem->AddMenuItem(_W("Show Axes"));
		mpMainMenuShowAxes->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MenuView_ShowAxes));

		vMenuSize = mpMainMenu->GetSize();
	}
	
	// Render view init
	CreateGuiViewport(mpBGFrame);
	SetGuiViewportPos(cVector3f(0,vMenuSize.y,0.05f));
}

//----------------------------------------------------------------

void cEditorWindowViewport::OnLoadLevel()
{
}

//----------------------------------------------------------------

void cEditorWindowViewport::OnUpdate(float afTimeStep)
{
	UpdateMenu();
}

//-------------------------------------------------------

void cEditorWindowViewport::OnChangeSize()
{
	cVector2f vMenuSize = 0;
	if(mpMainMenu)
	{
		mpMainMenu->SetSize(cVector2f(mpBGFrame->GetSize().x,mpMainMenu->GetSize().y));
		vMenuSize = mpMainMenu->GetSize();
	}

	SetGuiViewportSize(mpBGFrame->GetSize() - cVector2f(0,vMenuSize.y));
}

//----------------------------------------------------------------

void cEditorWindowViewport::OnSetActive(bool abX)
{
	SetViewportActive(abX);
}

//----------------------------------------------------------------
