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

#include "../Common/EdViewport.h"

#include "../Common/EdCamera.h"
#include "../Common/Editor.h"
#include "../Common/EdGrid.h"
#include "../Common/EdSelection.h"
#include "../Common/EdWorld.h"
#include "../Common/EdInputHandler.h"

//-------------------------------------------------------------
/*
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
*/

//---------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////
// VIEWPORT CLICK - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------

cViewportClick::cViewportClick(iEdViewport* apViewport, const cGuiMessageData& aData) : mpViewport(apViewport)
{
	mvLocalPos = mpViewport->GetMouseViewportPosition();
	mvRelPos = aData.mvRel;

	int lModifier = aData.mKeyPress.mlModifier;
	mbControl = (lModifier&eKeyModifier_Ctrl)!=0;
	mbShift	= (lModifier&eKeyModifier_Shift)!=0;
	mbAlt = (lModifier&eKeyModifier_Alt)!=0;

	mbLeft = (aData.mlVal&eGuiMouseButton_Left)!=0;
	mbMid = (aData.mlVal&eGuiMouseButton_Middle)!=0;
	mbRight = (aData.mlVal&eGuiMouseButton_Right)!=0;
	mbWheelUp = (aData.mlVal&eGuiMouseButton_WheelUp)!=0;
	mbWheelDn = (aData.mlVal&eGuiMouseButton_WheelDown)!=0;
}

//---------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// VIEWPORT CALLBACK - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------

cViewportCallback::cViewportCallback(iEdViewport* apViewport)
{
	mpViewport = apViewport;
	mpEditor = mpViewport->GetEditor();
}

//---------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// VIEWPORT CALLBACK - PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------

void cViewportCallback::OnPostSolidDraw(cRendererCallbackFunctions* apFunctions)
{
	apFunctions->SetMatrix(NULL);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	mpViewport->OnPostSolidDraw(apFunctions);

	//mpEditor->GetWorld()->GetSurfacePicker()->DrawDebug(apFunctions);

	/*

	if(mpViewport->GetDrawAxes())
	{
		apFunctions->SetBlendMode(eMaterialBlendMode_None);

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
//	tEditorClipPlaneVec& vClipPlanes = mpEditor->GetWorld()->GetClipPlanes();
//	for(int i=0;i<(int)vClipPlanes.size();++i)
//	{
//		vClipPlanes[i]->Draw(apFunctions, 0);
//	}

	apFunctions->GetLowLevelGfx()->DrawSphere(mpViewport->GetCamera()->GetTargetPosition(),0.05f, cColor(0,1,1,1));

	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(false);
	apFunctions->SetDepthWrite(false);

	//apFunctions->GetLowLevelGfx()->DrawSphere(mpEditor->,0.1f, cColor(0,1,0,1));
//	const cVector3f& vRefMousePos = mpViewport->GetVCamera()->GetTrackRefMousePos();
//	const cVector3f& vMouseNewPos = mpViewport->GetVCamera()->GetTrackNewMousePos();
//	cVector3f vMousePos = mpViewport->GetMouseWorldPosition();
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
	*/
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
}

//---------------------------------------------------------------------------------

void cViewportCallback::OnPostTranslucentDraw(cRendererCallbackFunctions* apFunctions)
{
	apFunctions->SetMatrix(NULL);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	mpViewport->OnPostTranslucentDraw(apFunctions);
}

//---------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////
// VIEWPORT - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------

int				iEdViewport::mlViewportCount = 0;
bool			iEdViewport::mbCamPlanesUpdated = true;
cVector2f		iEdViewport::mvCamPlanes = cVector2f(0.05f, 1000.0f);
iVertexBuffer*	iEdViewport::mpVBBillboard = NULL;
iGpuProgram*	iEdViewport::mpProgDrawSolid = NULL;

tVertexVec iEdViewport::mvArrowQuads[4] = 
{
	tVertexVec(4),
	tVertexVec(4),
	tVertexVec(4),
	tVertexVec(4)
};

//---------------------------------------------------------------------------------

iEdViewport::iEdViewport(iEditor* apEditor, const tWString& asName, 
						 cWorld* apWorld, iFrameBuffer* apFB, bool abDestroyFBOnExit) : iEdPane(apEditor, _W("Viewport: ") + asName)
{
	++mlViewportCount;
	tString sViewportCount = "Viewport" + cString::ToString(mlViewportCount) + "_";

	mlStartMode = 0;

	mpEditor = apEditor;
	mpEngine = mpEditor->GetEngine();
	mpGfx = mpEngine->GetGraphics();

	mpCamera = hplNew(cEdCamera,(this));
	mpEngineViewport = mpEngine->GetScene()->CreateViewport(mpCamera->GetEngineCamera(), apWorld);
	SetViewportActive(false);

	mpImgViewport = NULL;
	mvViewportPos = 0;
	mvViewportSize = 0;
	mvEngineViewportPos = -1;
	mvEngineViewportSize = -1;
	mvUVStart = -1;
	mvUVSize = -1;

	mfRayEndDistance = 200;
	
	mbMousePositionUpdated = true;
	mbUnprojectionUpdated = true;

	mbDestroyFBOnExit = abDestroyFBOnExit;

	mpGrid = hplNew(cEdGrid, (this));

	////////////////////////////////////
	// Init renderer callback (grid,...)
	AddViewportCallback(hplNew(cViewportCallback,(this)));

	mpFB = NULL;
	SetFrameBuffer(apFB);

	if(mpVBBillboard==NULL)
	{
		//////////////////////////////////
		//Create quad vertex buffer
		mpVBBillboard = mpGfx->GetLowLevel()->CreateVertexBuffer(eVertexBufferType_Hardware, eVertexBufferDrawType_Quad, 
																	eVertexBufferUsageType_Dynamic);

		mpVBBillboard->CreateElementArray(eVertexBufferElement_Position, eVertexBufferElementFormat_Float,4);
		mpVBBillboard->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		mpVBBillboard->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);

		for(int i=0; i<4; ++i)
			mpVBBillboard->AddIndex(i);

		//////////////////////////////////
		//Set default values for pos and col
		for(int i=0;i<4;i++){
			mpVBBillboard->AddVertexVec3f(eVertexBufferElement_Position, 0);
			mpVBBillboard->AddVertexColor(eVertexBufferElement_Color0, cColor(1,1));
		}

		//////////////////////////////////
		//Fill with texture coords (will do for most particle systems)
		mpVBBillboard->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(1,1,0));
		mpVBBillboard->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0,1,0));
		mpVBBillboard->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0,0,0));
		mpVBBillboard->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(1,0,0));

		////////////////////////////////////
		// Compile vertex buffer
		mpVBBillboard->Compile(0);


		////////////////////////////////////
		// Create Draw solid program
		cParserVarContainer vars;
		mpProgDrawSolid = mpGfx->CreateGpuProgramFromShaders("ComboDrawProg", "flat_color_vtx.glsl", "flat_color_frag.glsl", &vars);
		mpProgDrawSolid->GetVariableId("gvColor");
	}
}

//-------------------------------------------------------------

iEdViewport::~iEdViewport()
{
	tRendererCallbackList* pRenderCallbacks = mpEngineViewport->GetRendererCallbackList();
	STLDeleteAll(*pRenderCallbacks);

	/*
	tRendererCallbackListIt itCallbacks = mlstOwnCallbacks.begin();
	for(;itCallbacks!=mlstOwnCallbacks.end(); ++itCallbacks)
	{
		iRendererCallback* pRenderCallback = *itCallbacks;
		pRenderCallbacks->remove(pRenderCallback);
	}
	mlstOwnCallbacks.clear();
	*/

	mpEngineViewport->SetFrameBuffer(NULL);
	mpEngine->GetScene()->DestroyViewport(mpEngineViewport);
	if(mpImgViewport) mpSet->DestroyWidget(mpImgViewport);

	hplDelete(mpGrid);
	hplDelete(mpCamera);

	if(mbDestroyFBOnExit)
	{
		mpGfx->GetLowLevel()->SetCurrentFrameBuffer(NULL);
		mpGfx->DestroyFrameBuffer(mpFB);
	}

	--mlViewportCount;
	if(mlViewportCount==0)
	{
		hplDelete(mpVBBillboard);
		mpVBBillboard = NULL;

		mpGfx->DestroyGpuProgram(mpProgDrawSolid);
		mpProgDrawSolid = NULL;
	}
}

//-------------------------------------------------------------

void iEdViewport::SetViewportActive(bool abX)
{
	mbViewportActive = abX;
	mpEngineViewport->SetActive(abX);
	mpEngineViewport->SetVisible(abX);
}

//-------------------------------------------------------------

cCamera* iEdViewport::GetEngCamera()
{
	return mpCamera->GetEngineCamera();
}

//-------------------------------------------------------------

void iEdViewport::CreateGuiViewport(iWidget* apParentWidget)
{
	if(mpImgViewport) mpSet->DestroyWidget(mpImgViewport);
	float fMenuHeight = 22;

	mpImgViewport = mpSet->CreateWidgetImage("", mvViewportPos+cVector3f(0,fMenuHeight,0), mvViewportSize-fMenuHeight, eGuiMaterial_Alpha, false, apParentWidget);
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

void iEdViewport::SetRenderMode(eRenderer aMode)
{
	mRenderMode = aMode;
	
	iRenderer* pRenderer = mpGfx->GetRenderer(mRenderMode);
	mpEngineViewport->SetRenderer(pRenderer);
	SetClearColor(mpEditor->GetWorld()->GetBGColor());
}

//-------------------------------------------------------------

void iEdViewport::SetFrameBuffer(iFrameBuffer* apFB)
{
	if(apFB==NULL || mpFB==apFB) return;
	
	mpFB = apFB;
	iFrameBufferAttachment* pColorBuffer = mpFB->GetColorBuffer(0);
	if(pColorBuffer) mpRenderTarget = pColorBuffer->ToTexture();
	
	mpEngineViewport->SetFrameBuffer(mpFB);
	mbViewportNeedsUpdate = true;
}

//-------------------------------------------------------------

void iEdViewport::UpdateViewport()
{
	if(mpImgViewport==NULL || mbViewportNeedsUpdate==false)
		return;

	cGui* pGui = mpSet->GetGui();

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

void iEdViewport::SetGuiViewportPos(const cVector3f& avPos)
{
	if(mvViewportPos==avPos) return;
	mvViewportPos = avPos;

	if(mpImgViewport)
		mpImgViewport->SetPosition(mvViewportPos);
}

//-------------------------------------------------------------

void iEdViewport::SetGuiViewportSize(const cVector2f& avSize)
{
	if(mvViewportSize==avSize) return;
	mvViewportSize = avSize;

	mpCamera->GetEngineCamera()->SetAspect(avSize.x/avSize.y);

	if(mpImgViewport)
		mpImgViewport->SetSize(mvViewportSize);
}

//-------------------------------------------------------------

void iEdViewport::SetEngineViewportPositionAndSize(const cVector2l& avPos, const cVector2l& avSize)
{
	if(mvEngineViewportPos==avPos && 
		mvEngineViewportSize==avSize) return;

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

	mvUVSize =  vSizeFloat/vFBSizeFloat;
	mvUVEnd = mvUVStart + mvUVSize;

	/*for(int i=0; i<2; ++i)
	{
		mvUVStart.v[i] = cMath::Clamp(mvUVStart.v[i], 0.0f, 1.0f);
		mvUVEnd.v[i] = cMath::Clamp(mvUVEnd.v[i], 0.0f, 1.0f);
	}*/
	if(mvUVEnd.y>1.0f)
	{
		mvUVStart.y	-=1.0f;
		mvUVEnd.y	-=1.0f;
	}

	mbViewportNeedsUpdate = true;
}

//-------------------------------------------------------------

void iEdViewport::SetEngineViewportSize(const cVector2l& avSize)
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

const cVector2f& iEdViewport::GetMouseViewportPosition()
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

cVector2l iEdViewport::GetMouseViewportPositionInt()
{
	const cVector2f& vPos = GetMouseViewportPosition();

	return cVector2l((int)vPos.x, (int)vPos.y);
}

//-------------------------------------------------------------

const cVector3f& iEdViewport::GetUnprojectedStart()
{
	UpdateUnprojection();
	
	return mvUnprojectedStart;
}

//-------------------------------------------------------------

const cVector3f& iEdViewport::GetUnprojectedDir()
{
	UpdateUnprojection();
	
	return mvUnprojectedDir;
}

//-------------------------------------------------------------

const cVector3f& iEdViewport::GetUnprojectedEnd()
{
	UpdateUnprojection();

	return mvUnprojectedEnd;
}

cBoundingVolume* iEdViewport::GetRayBV()
{
	UpdateUnprojection();

	return &mRayBV;
}

//-------------------------------------------------------------

void iEdViewport::GetViewSpacePositionAndSize(const cVector3f& avWorldPosition, const cVector3f& avWorldSize, 
												   cVector3f& avViewSpacePos, cVector3f& avViewSpaceSize)
{
	mpCamera->GetCurrentCamMode()->GetViewSpacePositionAndSize(avWorldPosition, avWorldSize, avViewSpacePos, avViewSpaceSize);
}

//-------------------------------------------------------------

const cVector3f& iEdViewport::GetMouseWorldPosition(bool abForceUpdate, bool abCustomRayDist, float afCustomRayDist)
{
	if(mpEditor->GetInputHandler()->MouseHasMoved() || abForceUpdate || abCustomRayDist)
	{
		const cVector3f& vStart = GetUnprojectedStart();
		cVector3f vEnd = GetUnprojectedEnd();

		if(abCustomRayDist)
			vEnd = vStart + GetUnprojectedDir()*afCustomRayDist;

		cMath::CheckPlaneLineIntersection(mpGrid->GetPlane(), vStart, vEnd, &mvMouseWorldPos,NULL);
	}
	//if(mpEditorBase->GetFlags(eEdFlag_MouseMoved) || abForceUpdate || abCustomRayDist)
	//{
	//	const cVector3f& vStart = GetUnprojectedStart();
	//	cVector3f vEnd = GetUnprojectedEnd();

	//	if(abCustomRayDist)
	//		vEnd = vStart + GetUnprojectedDir()*afCustomRayDist;
		
	//	if(cMath::CheckPlaneLineIntersection(mpGrid->GetPlane(), vStart, vEnd, &mvMouseWorldPos,NULL)==false)
	//	{
	//		const cVector3f& vDir = GetUnprojectedDir();
	//		mvMouseWorldPos = mpGrid->GetProjectedPosOnPlane(vEnd + vDir*cMath::Abs(vEnd.v[mpGrid->GetPlaneNormal()]));
	//	}

	//}

	return mvMouseWorldPos;
}

//-------------------------------------------------------------

const cVector3f& iEdViewport::GetGridCenter()
{
	//switch(mCamera.GetCameraType())
	//{
	//case eEdCameraType_Perspective:
	//	return mCamera.GetCameraPosition();
	//default:
	//	return mCamera.GetTargetPosition();
	//}

	return mpCamera->GetTargetPosition();
}

//-------------------------------------------------------------

void iEdViewport::DrawBillboard(iTexture *apGfx, const cVector3f& avWorldPosition, const cVector2f& avSize, const cColor& aColor, 
									cRendererCallbackFunctions* apFunctions)
{
	apFunctions->SetModelViewMatrix(cMatrixf::Identity);

	/////////////////////////////////////////////////////////////////
	// Set up position and size on screen
	cVector3f vViewSpacePos;
	cVector3f vViewSpaceSize; 
	
	GetViewSpacePositionAndSize(avWorldPosition, avSize, vViewSpacePos, vViewSpaceSize);
	cVector2f vBBHalfSize = cVector2f(vViewSpaceSize.x, vViewSpaceSize.y)*0.5f;
	
	//////////////////////////
	// Update data in Vertex buffer
	int lVtxStride = mpVBBillboard->GetElementNum(eVertexBufferElement_Position);
	
	float *pPosArray = mpVBBillboard->GetFloatArray(eVertexBufferElement_Position);
	float *pColArray = mpVBBillboard->GetFloatArray(eVertexBufferElement_Color0);

	cVector3f vAdd[4] = 
	{
		cVector3f( vBBHalfSize.x,-vBBHalfSize.y,0),
		cVector3f(-vBBHalfSize.x,-vBBHalfSize.y,0),
		cVector3f(-vBBHalfSize.x, vBBHalfSize.y,0),
		cVector3f( vBBHalfSize.x, vBBHalfSize.y,0)
	};

	// Set position and color
	for(int i=0; i<4; ++i)
	{
		float *pPos = &pPosArray[i*lVtxStride];
		float *pCol = &pColArray[i*lVtxStride];

		cVector3f vVtxPos = vViewSpacePos + vAdd[i];

		pPos[0] = vVtxPos.x;
		pPos[1] = vVtxPos.y;
		pPos[2] = vVtxPos.z;

		pCol[0] = aColor.r;
		pCol[1] = aColor.g;
		pCol[2] = aColor.b;
		pCol[3] = aColor.a;
	}
	mpVBBillboard->UpdateData(eVertexElementFlag_Position | eVertexElementFlag_Color0, false);


	///////////////////////////////////////
	// Perform actual drawing
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(true);

	apFunctions->SetTexture(0, apGfx);
	apFunctions->SetAlphaMode(eMaterialAlphaMode_Trans);

	apFunctions->SetVertexBuffer(mpVBBillboard);
	apFunctions->DrawCurrent(eVertexBufferDrawType_Quad);
	apFunctions->SetVertexBuffer(NULL);

	apFunctions->SetTexture(0,NULL);
	apFunctions->SetAlphaMode(eMaterialAlphaMode_Solid);

	apFunctions->SetDepthTest(false);
	apFunctions->SetDepthWrite(false);

	apFunctions->SetMatrix(NULL);
}

//-------------------------------------------------------------

void iEdViewport::DrawSolidColorMesh(cMeshEntity* apMesh, const cColor& aCol, cRendererCallbackFunctions* apFunctions)
{
	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	apFunctions->SetTextureRange(NULL, 0);
	apFunctions->SetProgram(mpProgDrawSolid);
	mpProgDrawSolid->SetColor4f(0, aCol);

	for(int i=0;i<apMesh->GetSubMeshEntityNum();++i)
	{
		cSubMeshEntity* pSubMeshEntity = apMesh->GetSubMeshEntity(i);

		DrawSolidColorVertexBuffer(pSubMeshEntity->GetVertexBuffer(), pSubMeshEntity->GetModelMatrix(NULL), aCol, apFunctions, false);
	}

	apFunctions->SetMatrix(NULL);
	apFunctions->SetVertexBuffer(NULL);
	apFunctions->SetProgram(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
}

void iEdViewport::DrawSolidColorVertexBuffer(iVertexBuffer* apVB, cMatrixf* apTransform, const cColor& aCol, cRendererCallbackFunctions* apFunctions, bool abUsePreamble)
{
	if(abUsePreamble)
	{
		apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
		apFunctions->SetTextureRange(NULL, 0);
		apFunctions->SetProgram(mpProgDrawSolid);
		mpProgDrawSolid->SetColor4f(0, aCol);
	}

	apFunctions->SetMatrix(apTransform);
	apFunctions->SetVertexBuffer(apVB);
	apFunctions->DrawCurrent();

	if(abUsePreamble)
	{
		apFunctions->SetMatrix(NULL);
		apFunctions->SetVertexBuffer(NULL);
		apFunctions->SetProgram(NULL);
		apFunctions->SetBlendMode(eMaterialBlendMode_None);
	}
}

void iEdViewport::DrawArrow(cRendererCallbackFunctions* apFunctions, 
							const cMatrixf& amtxTransform, 
							float afLength, 
							bool abKeepConstantSize, 
							const cVector2f& avHeadSizeRatio, 
							const cColor& aCol,
							float afOrthoConstant, 
							float afPerspConstant)
{
	if(abKeepConstantSize)
	{
		cVector3f vViewPos;
		cVector3f vViewSize;
		GetViewSpacePositionAndSize(amtxTransform.GetTranslation(), 1, vViewPos, cVector3f());

		cCamera* pCam = GetEngCamera();

		switch(pCam->GetProjectionType())
		{
		case eProjectionType_Orthographic:
			afLength = pCam->GetOrthoViewSize().x * afOrthoConstant;
			break;
		case eProjectionType_Perspective:
			afLength = cMath::Abs(vViewPos.z)/afPerspConstant;
			break;
		}
	}

	float fY = avHeadSizeRatio.y * afLength;
	float fXZ = avHeadSizeRatio.x * afLength;

	mvArrowQuads[0][0].pos = cVector3f(fXZ,-fY,fXZ);
	mvArrowQuads[0][2].pos = 0;
	mvArrowQuads[0][3].pos = 0;
	mvArrowQuads[0][1].pos = cVector3f(-fXZ,-fY,fXZ);

	mvArrowQuads[1][0].pos = cVector3f(fXZ,-fY,-fXZ);
	mvArrowQuads[1][1].pos = 0;
	mvArrowQuads[1][2].pos = 0;
	mvArrowQuads[1][3].pos = cVector3f(-fXZ,-fY,-fXZ);

	mvArrowQuads[2][0].pos = cVector3f(fXZ,-fY,-fXZ);
	mvArrowQuads[2][2].pos = cVector3f(0,0,0);
	mvArrowQuads[2][3].pos = cVector3f(0,0,0);
	mvArrowQuads[2][1].pos = cVector3f(fXZ,-fY,fXZ);
	
	mvArrowQuads[3][0].pos = cVector3f(-fXZ,-fY,-fXZ);
	mvArrowQuads[3][1].pos = cVector3f(0,0,0);
	mvArrowQuads[3][2].pos = cVector3f(0,0,0);
	mvArrowQuads[3][3].pos = cVector3f(-fXZ,-fY,fXZ);

	cMatrixf mtxWorld = cMath::MatrixMul(amtxTransform, cMath::MatrixTranslate(cVector3f(0,afLength,0)));
	apFunctions->SetMatrix(&mtxWorld);
	
	apFunctions->GetLowLevelGfx()->DrawLine(cVector3f(0,-afLength,0), 0, aCol);
	for(int i=0; i<4;++i)
		apFunctions->GetLowLevelGfx()->DrawQuad(mvArrowQuads[i],aCol);

	apFunctions->SetMatrix(NULL);
}

//-------------------------------------------------------------

void iEdViewport::AddViewportCallback(iRendererCallback* apCallback)
{
	mpEngineViewport->AddRendererCallback(apCallback);
	mlstOwnCallbacks.push_back(apCallback);
}

//-------------------------------------------------------------

void iEdViewport::UpdateCameraPlanes()
{
	if(mbCamPlanesUpdated==false)
		return;

	mbCamPlanesUpdated = false;
	mpCamera->SetCameraPlanes(mvCamPlanes);
}

void iEdViewport::SetCamPlanes(const cVector2f& avX)
{
	if(mvCamPlanes==avX)
		return;

	mbCamPlanesUpdated = true;
	mvCamPlanes = avX;
}

//-------------------------------------------------------------

void iEdViewport::SetClearColor(const cColor& aX)
{
	cColor clearCol = aX;
	if(mRenderMode==eRenderer_Main)
		clearCol = cColor(0,1);
	
	mpEngineViewport->GetRenderSettings()->mClearColor = clearCol;
}

//-------------------------------------------------------------

void iEdViewport::SetFocused(bool abX)
{
	////////////////////////////////////////////
	// Set shortcuts status
	for(size_t i=0; i<mvShortcuts.size(); ++i)
	{
		cGuiGlobalShortcut* pShortcut = mvShortcuts[i];

		pShortcut->SetEnabled(mpBGWidget->IsEnabled() && abX);
	}
}

//----------------------------------------------------------------

bool iEdViewport::IsFocused()
{
	return mpEditor->GetFocusedViewport()==this;
}

//-------------------------------------------------------------

void iEdViewport::SetEnlarged(bool abX)
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

//-------------------------------------------------------------

bool iEdViewport::LoadState(cXmlElement* apElement)
{
	if(apElement==NULL || apElement->GetValue()!="Viewport")
		return false;

	//mpCamera->SetCameraMode(apElement->GetAttributeString("CamMode"));
	//mpCamera->GetCurrentCamMode()->SetCurrentController(apElement->GetAttributeString("CamController"));
	//mpCamera->GetCurrentCamMode()->SetCurrentRenderer(apElement->GetAttributeString("Renderer"));

	mpCamera->SetCameraPosition(apElement->GetAttributeVector3f("CameraPos"));
	mpCamera->SetTargetPosition(apElement->GetAttributeVector3f("CameraTarget"));

	mpGrid->SetPlaneNormal((ePlaneNormal)apElement->GetAttributeInt("GridPlane"));
	mpGrid->SetHeights(apElement->GetAttributeVector3f("GridHeights"));

	return true;
}

//-------------------------------------------------------------

bool iEdViewport::SaveState(cXmlElement* apElement, cXmlElement** apStateContainer)
{
	cXmlElement* pStateElem = apElement->CreateChildElement("Viewport");

	mpCamera->SaveState(pStateElem);

	pStateElem->SetAttributeInt("GridPlane", mpGrid->GetPlaneNormal());
	pStateElem->SetAttributeVector3f("GridHeights", mpGrid->GetHeights());

	if(apStateContainer)
		*apStateContainer = pStateElem;

	return true;
}

//-------------------------------------------------------------

bool iEdViewport::Viewport_OnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	// Big Hack - must fix somehow : make a broadcast func in Editor base or window handler.
	//UpdateCameraPlanes();

	if(IsFocused()==false)
		return false;

	mpCamera->GetCurrentController()->UpdateInput();

	return OnViewportUpdate(aData);
}
kGuiCallbackDeclaredFuncEnd(iEdViewport,Viewport_OnUpdate);

//-------------------------------------------------------------

bool iEdViewport::Viewport_OnGotFocus(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportGotFocus(aData);
}
kGuiCallbackDeclaredFuncEnd(iEdViewport,Viewport_OnGotFocus);

//-------------------------------------------------------------

bool iEdViewport::Viewport_OnLostFocus(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportLostFocus(aData);
}
kGuiCallbackDeclaredFuncEnd(iEdViewport,Viewport_OnLostFocus);

//-------------------------------------------------------------

bool iEdViewport::Viewport_OnDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportDraw(aData);
}
kGuiCallbackDeclaredFuncEnd(iEdViewport,Viewport_OnDraw);

//-------------------------------------------------------------

bool iEdViewport::Viewport_OnMouseMove(iWidget* apWidget, const cGuiMessageData& aData)
{
	mvMouseScreenPos = aData.mvPos;
	mbMousePositionUpdated = true;
	mbUnprojectionUpdated = true;

	mpCamera->GetCurrentController()->Update(cViewportClick(this, aData));

	mpEditor->BroadcastModuleMsg(eEdModuleMsg_ViewportMouseMove, NULL);

	return OnViewportMouseMove(aData);
}
kGuiCallbackDeclaredFuncEnd(iEdViewport,Viewport_OnMouseMove);

//-------------------------------------------------------------

bool iEdViewport::Viewport_OnMouseDown(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportMouseDown(aData);
}
kGuiCallbackDeclaredFuncEnd(iEdViewport,Viewport_OnMouseDown);

//-------------------------------------------------------------

bool iEdViewport::Viewport_OnMouseUp(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportMouseUp(aData);
}
kGuiCallbackDeclaredFuncEnd(iEdViewport,Viewport_OnMouseUp);

//-------------------------------------------------------------

bool iEdViewport::Viewport_OnKeyPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportKeyPress(aData);
}
kGuiCallbackDeclaredFuncEnd(iEdViewport,Viewport_OnKeyPress);

//-------------------------------------------------------------

bool iEdViewport::Viewport_OnKeyRelease(iWidget* apWidget, const cGuiMessageData& aData)
{
	return OnViewportKeyRelease(aData);
}
kGuiCallbackDeclaredFuncEnd(iEdViewport,Viewport_OnKeyRelease);

//-------------------------------------------------------------

void iEdViewport::UpdateUnprojection()
{
	if(mbUnprojectionUpdated==false) return;

	const cVector2f& vMouseViewportPos = GetMouseViewportPosition();
	const cVector2f& vViewportSize = GetGuiViewportSize();
	
	if(cMath::CheckPointInRectIntersection(vMouseViewportPos, cRect2f(0,0,vViewportSize.x, vViewportSize.y)))
	{
		cCamera* pCam = mpCamera->GetEngineCamera();
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

void iEdViewport::OnSetCamMode(iCamMode* apMode)
{
	mpEditor->BroadcastModuleMsg(eEdModuleMsg_ViewportParamsChange, this);
}

//-------------------------------------------------------------

void iEdViewport::OnReset()
{
	// TODO: fix so that this is not needed. It must be some uninitialized var in camera.
	//Log("prereset Camera pos %s - target %s\n", mpCamera->GetCameraPosition().ToFileString().c_str(), mpCamera->GetTargetPosition().ToFileString().c_str());
	mpCamera->GetCameraPosition();
	mpCamera->GetTargetPosition();
	//mpCamera->SetCameraPosition(-1);
	//mpCamera->SetTargetPosition(-1);
	mpCamera->SetCameraMode(mlStartMode);
	//Log("postreset Camera pos %s - target %s\n", mpCamera->GetCameraPosition().ToFileString().c_str(), mpCamera->GetTargetPosition().ToFileString().c_str());
}

//-------------------------------------------------------------

void iEdViewport::OnSetActive(bool abX)
{
	iEdPane::OnSetActive(abX);

	SetViewportActive(abX);
}


//-------------------------------------------------------------
