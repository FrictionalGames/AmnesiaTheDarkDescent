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

#include "../Common/EdWindowMaterialEditor.h"
#include "../Common/EdWorld.h"

#include "../Common/EdCamera.h"

#include "../Common/Editor.h"
#include "../Common/EdVar.h"
#include "../Common/DirectoryHandler.h"
#include "../Common/EdResource.h"

#include "../Common/HPLMaterial.h"

#include "../Common/EdMatEditorMenu.h"

#include <algorithm>


//------------------------------------------------------------------------------------

cCamModeMatEd::cCamModeMatEd(cEdCamera* apCam, int alIdx) : iCamMode(apCam, alIdx, _W("MatEditor"))
{
	AddCamController(hplNew(cCamControllerMatEd,(apCam)));

	AddRenderer(_W("Main"), eRenderer_Main);
}

//------------------------------------------------------------------------------------

cCamControllerMatEd::cCamControllerMatEd(cEdCamera* apCam) : iCamController(apCam, _W("MatEditor"))
{
	mbOrbiting = false;
	mbZooming = false;
	mbWheelUp = false;
	mbWheelDn = false;
}

//------------------------------------------------------------------------------------

bool cCamControllerMatEd::OnViewportMouseDown(const cViewportClick& aClick)
{
	if(aClick.mbLeft) 
		mbOrbiting = true;
	if(aClick.mbMid)
		mbZooming = true;
	if(aClick.mbWheelUp)
		mbWheelUp = true;
	if(aClick.mbWheelDn)
		mbWheelDn = true;

	return aClick.mbLeft || aClick.mbMid || aClick.mbWheelDn || aClick.mbWheelUp;
}

//------------------------------------------------------------------------------------

bool cCamControllerMatEd::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(aClick.mbLeft) 
		mbOrbiting = false;
	if(aClick.mbMid)
		mbZooming = false;
	

	return aClick.mbLeft || aClick.mbMid;
}

//------------------------------------------------------------------------------------

void cCamControllerMatEd::OnUpdate(const cViewportClick& aData)
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
	if(mbZooming)
	{
		mpCam->SetDistToTarget(mpCam->GetDistToTarget()+vRel.y);
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

//------------------------------------------------------------------------------------

cTextureUnitPanel::cTextureUnitPanel(cEdWindowMaterialEditor* apWin, eMaterialTexture aUnit)
{
	mpWindow = apWin;
	mUnit = aUnit;

	tString sLabel = mpWindow->GetEditor()->GetEngine()->GetResources()->GetMaterialManager()->GetTextureString(mUnit);

	mpTextureWrapper = mpWindow->GetMaterial()->GetTextureWrapper(mUnit);

	cGraphics* pGfx = mpWindow->GetEditor()->GetEngine()->GetGraphics();
	iWidget* pParent = mpWindow->GetFrameUnits();
	cGuiSet* pSet = mpWindow->GetGuiSet();

	mpHandle = pSet->CreateWidgetDummy(0, pParent);
	mpLUnit = pSet->CreateWidgetLabel(0, 0, cString::To16Char(sLabel), mpHandle);
	mpLUnit->SetDefaultFontSize(cVector2f(12));

	mpInputs = pSet->CreateWidgetDummy(cVector3f(10,10,0) + cVector3f(0,mpLUnit->GetSize().y,0), mpHandle);

	cVector3f vPos = 0;

	if(sLabel=="CubeMap")
		mpInpFile = apWin->CreateInputCubeMap(vPos, _W("File"), mpInputs);
	else
		mpInpFile = apWin->CreateInputTexture2D(vPos, _W("File"), mpInputs);

	mpInpFile->SetUserData(this);

	vPos.y += mpInpFile->GetSize().y; 

	cWidgetFrame* pFThumb = pSet->CreateWidgetFrame(cVector3f(150,15,-0.01f), cVector2f(100), false, mpInputs);
	pFThumb->SetBackgroundZ(0);
	pFThumb->SetDrawBackground(true);
	pFThumb->SetBackGroundColor(cColor(0,1));
	
	mpImgThumb = pSet->CreateWidgetImage("", cVector3f(0,0,0.01f), cVector2f(100), eGuiMaterial_Diffuse, false, pFThumb);

	mpInpWrap = mpWindow->CreateInputEnum(vPos, _W("Wrap Mode"), mpInputs);
	mpInpWrap->AddValue(_W("Repeat"));
	mpInpWrap->AddValue(_W("Clamp"));
	mpInpWrap->AddValue(_W("ClampToEdge"));
	mpInpWrap->AddValue(_W("ClampToBorder"));
	mpInpWrap->SetUserData(this);

	vPos.y += mpInpWrap->GetSize().y;

	mpInpMipMaps = mpWindow->CreateInputBool(vPos, _W("Use Mip Maps"), mpInputs);
	mpInpMipMaps->SetUserData(this);

	vPos.y += mpInpMipMaps->GetSize().y;

	mpInpAnimMode = mpWindow->CreateInputEnum(vPos, _W("Animation mode"), mpInputs);
	mpInpAnimMode->AddValue(_W("None"));
	mpInpAnimMode->AddValue(_W("Loop"));
	mpInpAnimMode->AddValue(_W("Oscillate"));
	mpInpAnimMode->SetUserData(this);

	vPos.y += mpInpAnimMode->GetSize().y;

	mpInpFrameTime = mpWindow->CreateInputNumber(vPos, _W("Frame time"), mpInputs);
	mpInpFrameTime->SetUserData(this);

	Reset();
}

//------------------------------------------------------------------------------------

cTextureUnitPanel::~cTextureUnitPanel()
{
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

void cTextureUnitPanel::Reset()
{
	cGuiGfxElement* pImg = mpImgThumb->GetImage();
	if(pImg)
		mpWindow->GetGuiSet()->GetGui()->DestroyGfx(pImg);
	mpImgThumb->SetImage(NULL);

	Update();
}

//------------------------------------------------------------------------------------

void cTextureUnitPanel::Update()
{
	cGui* pGui = mpWindow->GetGuiSet()->GetGui();
	if(mpTextureWrapper->IsValid()==false)
	{
		mpInpFile->SetValue(_W(""), false);
		mpImgThumb->SetImage(NULL);
	}
	else
	{
		const tWString& sTextureFile = mpTextureWrapper->GetFile();
		mpInpFile->SetValue(mpTextureWrapper->GetFile(), false, true, false, false);
		cGuiGfxElement* pImg = mpImgThumb->GetImage();
		iTexture* pOldTex = NULL;
		if(pImg)
			pOldTex = pImg->GetTexture(0);
		iTexture* pTex = mpTextureWrapper->GetEngTexture();
		if(pTex!=pOldTex)
		{
			if(pImg)
			{
				pGui->DestroyGfx(pImg);
			}
			
			pImg = pGui->CreateGfxTexture(pTex, false, eGuiMaterial_Alpha);
			mpImgThumb->SetImage(pImg);
		}
	}

	mpInpFile->SetInitialPath(mpWindow->msLastTexturePath);

	mpInpMipMaps->SetValue(mpTextureWrapper->GetUseMipMaps(), false);
	mpInpWrap->SetValue(mpWindow->GetEditor()->GetEngine()->GetResources()->GetMaterialManager()->GetWrap(mpTextureWrapper->GetWrap()), false);
		
	mpInpAnimMode->SetValue(cString::To16Char(mpTextureWrapper->GetAnimMode()), false);
	mpInpFrameTime->SetValue(mpTextureWrapper->GetFrameTime(), false);
}

//------------------------------------------------------------------------------------

void cTextureUnitPanel::UpdateTexture()
{
	mpTextureWrapper->Update();
}

//------------------------------------------------------------------------------------

bool cTextureUnitPanel::PanelSpecificInputCallback(iEdInput* apInput)
{
	cHPLMaterial* pMat = mpWindow->GetMaterial();
	cGraphics* pGfx = mpWindow->GetEditor()->GetEngine()->GetGraphics();
	if(apInput==mpInpFile)
	{
		mpTextureWrapper->SetFile(mpInpFile->GetValue());
		mpWindow->msLastTexturePath = mpInpFile->GetValue();
	}
	else if(apInput==mpInpMipMaps)
	{
		mpTextureWrapper->SetUseMipMaps(mpInpMipMaps->GetValue());
	}
	else if(apInput==mpInpWrap)
	{
		mpTextureWrapper->SetWrap(mpInpWrap->GetEnumValue(mpInpWrap->GetValue()));
	}
	else if(apInput==mpInpAnimMode)
	{
		mpTextureWrapper->SetAnimMode(mpInpAnimMode->GetEnumValue(mpInpAnimMode->GetValue()));
	}
	else if(apInput==mpInpFrameTime)
	{
		mpTextureWrapper->SetFrameTime(mpInpFrameTime->GetValue());
	}
	else
	{
		return false;
	}

	mpTextureWrapper->Update();
	mpWindow->SetPreviewUpdated();
	Update();

	return true;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------


cEdWindowMaterialEditor::cEdWindowMaterialEditor(iEditor* apEditor, const tWString& asMatFile, cEdInputFile* apInput) : iEdViewport(apEditor, asMatFile, NULL, NULL, true)
{
	///////////////////////////////////////////////////////////////
	// Set up world
	mpMatWorld = mpEditor->GetEngine()->GetScene()->CreateWorld("Mat");
	mpEngineViewport->SetWorld(mpMatWorld);

	mbStandAlone = apInput==NULL;
	if(IsStandalone())
		mpMaterial = static_cast<cHPLMaterial*>(apEditor->GetWorld());
	else
		mpMaterial = hplNew(cHPLMaterial,(apEditor));

	mpMaterial->SetMatEditorWindow(this);
	

	msLastTexturePath = _W("");

	mmtxRotation = cMatrixf::Identity;

	mpMatWorld->SetSkyBoxColor(cColor(0,0,1,1));
	mpMatWorld->SetSkyBoxActive(true);

	// Load models
	cResources* pRes = mpEditor->GetEngine()->GetResources();
	cMeshManager* pManager = pRes->GetMeshManager();
	cMesh* pMesh = NULL;
	pMesh = pManager->CreateMesh("mateditor_cube.dae");
	if(pMesh)
		mvPreviewEntities.push_back(mpMatWorld->CreateMeshEntity("PrevBox", pMesh));

	pMesh = pManager->CreateMesh("mateditor_cylinder.dae");
	if(pMesh)
		mvPreviewEntities.push_back(mpMatWorld->CreateMeshEntity("PrevCylinder", pMesh));

	pMesh = pManager->CreateMesh("mateditor_sphere.dae");
	if(pMesh)
		mvPreviewEntities.push_back(mpMatWorld->CreateMeshEntity("PrevSphere", pMesh));

	pMesh = pManager->CreateMesh("mateditor_plane.dae");
	if(pMesh)
		mvPreviewEntities.push_back(mpMatWorld->CreateMeshEntity("PrevPlane", pMesh));

	for(size_t i=0;i<mvPreviewEntities.size();++i)
	{
		mvPreviewEntities[i]->SetVisible(false);
		mvPreviewEntities[i]->SetActive(false);
	}
	mpCurrentPreviewEntity = NULL;

	mpFixedLight = mpMatWorld->CreateLightPoint();
	mpFixedLight->SetRadius(6);
	mpFixedLight->SetDiffuseColor(cColor(1,1));
	mpFixedLight->SetPosition(cVector3f(0,0,2.5f));

	mpMovableLight = mpMatWorld->CreateLightPoint();
	mpMovableLight->SetRadius(6);
	mpMovableLight->SetDiffuseColor(cColor(1,1));
	mpMovableLight->SetPosition(cVector3f(2.5f,0,0));

	mbMoveLight = false;
	mbMoveCamera = false;

	mbRotationEnabled = false;

	mpInputPanel = NULL;
	mpDestInput = apInput;

	if(mpDestInput)
		msInitMatFile = mpDestInput->GetValue();
	else
		msInitMatFile = asMatFile;

	SetMenu(hplNew(cEdMatEditorMenu,(this)));

	mbInitialized = false;

	mpCamera->AddCamMode(hplNew(cCamModeMatEd,(mpCamera, 0)));
	mpCamera->SetCameraPosition(cVector3f(10));
}

//------------------------------------------------------------------------------------

cEdWindowMaterialEditor::~cEdWindowMaterialEditor()
{
	UpdatePreview(NULL);

	if(IsStandalone()==false)
		hplDelete(mpMaterial);

	for(size_t i=0; i<mvUnitSwitches.size(); ++i)
	{
		cEdInputBool* pSwitch = mvUnitSwitches[i];
		cTextureUnitPanel* pPanel = static_cast<cTextureUnitPanel*>(pSwitch->GetUserData());
		hplDelete(pPanel);
	}

	if(mpInputPanel) hplDelete(mpInputPanel);

	mpEditor->GetEngine()->GetScene()->DestroyWorld(mpMatWorld);
}

//------------------------------------------------------------------------------------

bool cEdWindowMaterialEditor::IsStandalone()
{
	return mbStandAlone;
}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::SetUpUnits()
{
	for(int i=0; i<eMaterialTexture_LastEnum; ++i)
	{
		cEdInputBool* pSwitch = mvUnitSwitches[i];
		cTextureUnitPanel* pPanel = static_cast<cTextureUnitPanel*>(pSwitch->GetUserData());

		pSwitch->GetHandle()->SetEnabled(false);
		pSwitch->GetHandle()->SetVisible(false);

		pPanel->GetHandle()->SetEnabled(false);
		pPanel->GetHandle()->SetVisible(false);

		//pPanel->Update();
	}

	if(mpMaterial==NULL)
		return;
		
	iMaterialType* pType = mpMaterial->GetTypePointer();
	if(pType==NULL)
		return;

	///////////////////////////////////////////////////////////////////
	// Get used textures and enable unit panel switches accordingly
	// Got to sort used textures first
	std::list<eMaterialTexture> lstUsedTextures;
	std::list<eMaterialTexture>::iterator it;
	for(int i=0;i<pType->GetUsedTextureNum();++i)
	{
		eMaterialTexture index = pType->GetUsedTexture(i)->mType;
		lstUsedTextures.push_back(index);
	}
	lstUsedTextures.sort();

	it = lstUsedTextures.begin();
	cVector3f vSwitchPos = cVector3f(10,10,0.1f);
	for(; it!=lstUsedTextures.end(); ++it)
	{
		eMaterialTexture index = *it;
		bool bIsEnabled = mpMaterial->GetTextureWrapper(index)->IsEnabled();
		
		cEdInputBool* pSwitch = mvUnitSwitches[index];
		pSwitch->GetHandle()->SetEnabled(true);
		pSwitch->GetHandle()->SetVisible(true);
		pSwitch->SetValue(bIsEnabled, false);
		pSwitch->GetHandle()->SetPosition(vSwitchPos);

		vSwitchPos.y += pSwitch->GetHandle()->GetSize().y + 5;
	}
}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::SetUpPanels()
{
	cVector3f vPanelPos = cVector3f(0,0,0.1f);
	for(eMaterialTexture tex=(eMaterialTexture)0; tex<eMaterialTexture_LastEnum; tex=(eMaterialTexture)(tex+1))
	{
		cEdInputBool* pPanelSwitch = mvUnitSwitches[tex];

		bool bIsTextureEnabled = mpMaterial->GetTextureWrapper(tex)->IsEnabled();
		bool bIsShown = bIsTextureEnabled && pPanelSwitch->GetHandle()->IsVisible();
	
		cTextureUnitPanel* pPanel = static_cast<cTextureUnitPanel*>(pPanelSwitch->GetUserData());

		pPanel->GetHandle()->SetEnabled(bIsShown);
		pPanel->GetHandle()->SetVisible(bIsShown);
		pPanel->UpdateTexture();
		pPanel->Update();
		if(bIsShown)
		{
			pPanel->GetHandle()->SetPosition(vPanelPos);
			vPanelPos.y += pPanel->GetHandle()->GetSize().y + 10;
		}
	}
}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::SetUpVars()
{
	if(mbTypeChanged)
	{
		if(mpInputPanel)
			hplDelete(mpInputPanel);

		cEdClassInstance* pInstance = mpMaterial->GetClassInstance();
		if(pInstance==NULL)
			return;

		mpInputPanel = pInstance->CreateInputPanel(this, mpFMaterialVars, true);
	}
	
	if(mpInputPanel) mpInputPanel->Update();
}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::DispatchMessage(eEdModuleMsg aMsg, void* apData)
{
	if(IsStandalone()==false) mpMaterial->DispatchMessage(aMsg, apData);

	iEdViewport::DispatchMessage(aMsg, apData);
}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::OnReset()
{
	msMatFilename = _W("");
	
	mpInpDepthTest->SetValue(true, true, true, true);
	mpInpMatType->SetValue(0, true, true, true);
	mpInpPhysicsMat->SetValue(0, true, true, true);
	mpInpBlendMode->SetValue(0, true, true, true);

	for(size_t i=0;i<mvUnitSwitches.size();++i)
	{
		cEdInputBool* pSwitch = mvUnitSwitches[i];
		cTextureUnitPanel* pPanel = static_cast<cTextureUnitPanel*>(pSwitch->GetUserData());
		pPanel->Reset();
	}

	SetUpPanels();

	///////////////////////////////////////////////////////
	// Reset preview pane
	const tWString sCubeMapPath = mpEditor->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath("editor_cubemap_default.dds");
	mpInpBGCubeMap->SetValue(sCubeMapPath, true, true, true);
	mpInpPreviewModel->SetValue(0, true, true, true);
	mpInpRotateModel->SetValue(false, true, true, true);
	mpInpBGType->SetValue(0, true, true, true);
	mpInpBGColor->SetValue(cColor(0,0,1,1), true, true, true);

	mpInpFixedLightColor->SetValue(cColor(1), true, true, true);
	mpInpMovableLightColor->SetValue(cColor(1), true, true, true);

	mpMovableLight->SetPosition(cVector3f(2.5f,0,0));

	mpCamera->SetCameraPosition(cVector3f(0,0,2.5f));
	mpCamera->UpdateEngineCamera();

	SetPreviewUpdated();
	SetUpdated();
}

//------------------------------------------------------------------------------------

bool cEdWindowMaterialEditor::ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget==mpBAddAnim)
	{
		mpMaterial->AddUVAnim(eMaterialUvAnimation_Translate,0,0,eMaterialAnimationAxis_X);
	}
	else if(apWidget==mpBRemAnim)
	{
		mpMaterial->RemoveUVAnim(mpInpUVAnimation->GetValue());
	}

	SetUpdated();
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowMaterialEditor,ButtonPressed);

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::UpdateUVAnimInputs()
{
	int lSelectedAnim = mpInpUVAnimation->GetValue();
	cMaterialUvAnimation* pAnim = mpMaterial->GetUVAnim(lSelectedAnim);
	if(pAnim)
	{
		mpInpUVAnimType->SetValue(pAnim->mType,false);
		mpInpUVAnimAxis->SetValue(pAnim->mAxis,false);
		mpInpUVAnimSpeed->SetValue(pAnim->mfSpeed,false);
		mpInpUVAnimAmp->SetValue(pAnim->mfAmp,false);
	}
	else
	{
		mpInpUVAnimType->SetValue(-1,false);
		mpInpUVAnimAxis->SetValue(-1,false);
		mpInpUVAnimSpeed->SetValue(0,false);
		mpInpUVAnimAmp->SetValue(0,false);
	}

	mpInpUVAnimType->GetHandle()->SetEnabled(pAnim!=NULL);
	mpInpUVAnimAxis->GetHandle()->SetEnabled(pAnim!=NULL);
	mpInpUVAnimSpeed->GetHandle()->SetEnabled(pAnim!=NULL);
	mpInpUVAnimAmp->GetHandle()->SetEnabled(pAnim!=NULL);
}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::SetWindowCaption(const tWString& asCaption)
{
	if(IsStandalone())
		cPlatform::SetWindowCaption(cString::To8Char(asCaption));
	else
		GetBG()->SetText(asCaption);
}

//------------------------------------------------------------------------------------

iWidget* cEdWindowMaterialEditor::CreateBGWidget(const cVector3f& avPos, const cVector2f& avSize, iWidget* apParent)
{
	iWidget* pBG;
	if(IsStandalone())
	{
		cWidgetFrame* pFrame = mpSet->CreateWidgetFrame(avPos, avSize, false, apParent);
		pFrame->SetBackGroundColor(cColor(0.82f, 0.81f, 0.79f,1));
		pFrame->SetDrawBackground(true);

		pBG = pFrame;
	}

	else
	{
		pBG = mpSet->CreateWidgetWindow(eWidgetWindowButtonFlag_ButtonClose, avPos, avSize, _W(""), apParent);
		pBG->SetSize(cVector2f(950,768));
	}

	return pBG;
}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::CreateMaterial(const tString& asMat)
{
	SetPreviewUpdated();
}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::UpdatePreview(cMaterial* apMat)
{
	for(size_t i=0; i<mvPreviewEntities.size(); ++i)
	{
		cMesh* pMesh = mvPreviewEntities[i]->GetMesh();
		for(int j=0;j<pMesh->GetSubMeshNum();++j)
		{
			cSubMesh* pSubMesh = pMesh->GetSubMesh(j);
			pSubMesh->SetMaterial(apMat);
			if(apMat)apMat->IncUserCount();
		}
	}
}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::OnInit()
{

}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::OnCreateLayout()
{
	iEdWindow::OnCreateLayout();

	float fStartY = 5;
	iWidget* pParent = GetBG();

	cWidgetGroup* pGroup = NULL;

	////////////////////////////////////////////////
	// Preview
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(440,fStartY,0.1f), cVector2f(450,565), _W("Preview"), pParent);

		////////////////////////////////////////////////
		// Set up preview viewport
		CreateGuiViewport(pGroup);
		SetGuiViewportPos(cVector3f(10,10,0.1f));
		SetGuiViewportSize(cVector2f(430));

		iTexture* pTex = mpEditor->GetEngine()->GetGraphics()->CreateTexture("", eTextureType_2D, eTextureUsage_RenderTarget);
		pTex->SetWrapR(eTextureWrap_ClampToEdge);
		pTex->SetWrapS(eTextureWrap_ClampToEdge);
		pTex->CreateFromRawData(cVector3l(512,512,0), ePixelFormat_RGB, 0);

		iFrameBuffer* pFB = mpEditor->GetEngine()->GetGraphics()->CreateFrameBuffer("MaterialEditor");
		pFB->SetTexture2D(0, pTex);
		pFB->CompileAndValidate();
		SetFrameBuffer(pFB);

		SetEngineViewportPositionAndSize(0, 512);
		UpdateViewport();
		SetViewportActive(true);

		////////////////////////////////
		// Preview Controls

		// Preview Model selector
		mpInpPreviewModel = CreateInputEnum(cVector3f(10,15,0.1f) + cVector3f(0,GetGuiViewportSize().y,0), _W("Model"),pGroup);
		mpInpPreviewModel->AddValue(_W("Box"));
		mpInpPreviewModel->AddValue(_W("Cylinder"));
		mpInpPreviewModel->AddValue(_W("Sphere"));
		mpInpPreviewModel->AddValue(_W("Plane"));

		// Rotate Model
		mpInpRotateModel = CreateInputBool(mpInpPreviewModel->GetPosition() + cVector3f(mpInpPreviewModel->GetSize().x+10, 15, 0), _W("Rotate Model"), pGroup);

		// Background Type
		mpInpBGType = CreateInputEnum(mpInpPreviewModel->GetPosition()+ cVector3f(0,mpInpPreviewModel->GetSize().y + 10,0), _W("Background Type"), pGroup);
		mpInpBGType->AddValue(_W("CubeMap"));
		mpInpBGType->AddValue(_W("Flat"));

		// Background CubeMap
		mpInpBGCubeMap = CreateInputCubeMap(mpInpBGType->GetPosition() + cVector3f(mpInpBGType->GetSize().x + 10,0,0), _W("CubeMap file"), pGroup);
	
		// Background Color
		mpInpBGColor = CreateInputColor(mpInpBGCubeMap->GetPosition() + cVector3f(mpInpBGCubeMap->GetSize().x +10,15,0), _W("Background color"), pGroup);

		// Lights color
		mpInpFixedLightColor = CreateInputColor(mpInpBGType->GetPosition() + cVector3f(0,mpInpBGType->GetSize().y+10,0),_W("Fixed light color"), pGroup);
		mpInpMovableLightColor = CreateInputColor(mpInpFixedLightColor->GetPosition() + cVector3f(mpInpFixedLightColor->GetSize().x+10,0,0),_W("Movable light color"), pGroup);
	}



	cVector3f vPos;
	////////////////////////////////////////////////////////
	// General inputs
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,fStartY,0.1f), cVector2f(420, 85), _W("General"), pParent);

		vPos = cVector3f(10,10,0.1f);
		// Material Type
		mpInpMatType = CreateInputEnum(vPos, _W("Type:"), pGroup);
		
		// Add type names
		tStringVec vTypeNames = mpEditor->GetEngine()->GetGraphics()->GetMaterialTypeNames();
		for(size_t i=0;i<vTypeNames.size();++i)
			mpInpMatType->AddValue(cString::To16Char(vTypeNames[i]));

		vPos.y += mpInpMatType->GetSize().y;

		// Physisc material
		mpInpPhysicsMat = CreateInputEnum(vPos, _W("Physics Material"), pGroup);
		// Populate physics material data
		cSurfaceDataIterator it = mpEditor->GetEngine()->GetPhysics()->GetSurfaceDataIterator();
		while(it.HasNext())
		{
			cSurfaceData* pData = it.Next();
			mpInpPhysicsMat->AddValue(cString::To16Char(pData->GetName()));
		}

		vPos = cVector3f(130, 22, 0.1f);
		// Depth Test
		mpInpDepthTest = CreateInputBool(vPos, _W("Depth Test"), pGroup);

		vPos.y += mpInpDepthTest->GetSize().y;

		// Value (TODO: I still dont know what this is for)
		mpInpValue = CreateInputNumber(vPos, _W("Value"), pGroup);

		vPos = cVector3f(230,10, 0.1f);

		// Blend Mode
		mpInpBlendMode = CreateInputEnum(vPos, _W("Blend Mode"), pGroup);
		// Add blend modes
		for(int i=1;cHPLMaterial::gsBlendModeStrings[i]!="";++i)
			mpInpBlendMode->AddValue(cString::To16Char(cHPLMaterial::gsBlendModeStrings[i]));
	}

	vPos = cVector3f(10, mpInpPhysicsMat->GetPosition().y + mpInpPhysicsMat->GetSize().y+10,0.1f);

	////////////////////////////////////////
	// Animations
	{
		pGroup = mpSet->CreateWidgetGroup(pGroup->GetLocalPosition() + cVector3f(0, pGroup->GetSize().y+10,0), cVector2f(420,80), _W("UV Animations"), pParent);

		vPos = cVector3f(10,10,0.1f);

		// Animation selector
		mpInpUVAnimation = CreateInputEnum(vPos, _W("UV Animation"), pGroup);

		// Add anim button
		mpBAddAnim = mpSet->CreateWidgetButton(vPos + cVector3f(0, mpInpUVAnimation->GetSize().y + 5,0), cVector2f(50,20), _W("Create"), pGroup);
		mpBAddAnim->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(ButtonPressed));

		// Remove anim button
		mpBRemAnim = mpSet->CreateWidgetButton(vPos + cVector3f(55,mpInpUVAnimation->GetSize().y + 5,0), cVector2f(50,20), _W("Delete"), pGroup);
		mpBRemAnim->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(ButtonPressed));

		vPos.x += mpInpUVAnimation->GetSize().x + 30;

		// Anim type
		mpInpUVAnimType = CreateInputEnum(vPos, _W("Anim Type"), pGroup, 80);
		mpInpUVAnimType->AddValue(_W("Translate"));
		mpInpUVAnimType->AddValue(_W("Sin"));
		mpInpUVAnimType->AddValue(_W("Rotate"));

		vPos.x += mpInpUVAnimType->GetSize().x +5;

		// Axis
		mpInpUVAnimAxis = CreateInputEnum(vPos, _W("Axis"), pGroup, 40);
		mpInpUVAnimAxis->AddValue(_W("X"));
		mpInpUVAnimAxis->AddValue(_W("Y"));
		mpInpUVAnimAxis->AddValue(_W("Z"));

		vPos.x += mpInpUVAnimAxis->GetSize().x +5;

		// Speed
		mpInpUVAnimSpeed = CreateInputNumber(vPos, _W("Speed"), pGroup, 40, 0.1f);

		vPos.x += mpInpUVAnimSpeed->GetSize().x +5;

		// Sin Amplitude
		mpInpUVAnimAmp = CreateInputNumber(vPos, _W("Amplitude"), pGroup, 40, 0.1f);

		vPos.x += mpInpUVAnimAmp->GetSize().x +5;
	}


	pGroup = mpSet->CreateWidgetGroup(pGroup->GetLocalPosition() + cVector3f(0,pGroup->GetSize().y+10,0), cVector2f(420,380), _W("Texture Units"), pParent);

	////////////////////////////////////////////////////
	// Texture Unit Panels
	{
		mpFUnits = mpSet->CreateWidgetFrame(cVector3f(110,10,0.1f), cVector2f(300,340),false, pGroup, false, true);

		for(eMaterialTexture tex=(eMaterialTexture)0; tex<eMaterialTexture_LastEnum; tex=(eMaterialTexture)(tex+1))
		{
			// Texture Unit Panel itself
			cTextureUnitPanel* pPanel = hplNew( cTextureUnitPanel,(this, tex));
			pPanel->GetHandle()->SetVisible(false);
			pPanel->GetHandle()->SetEnabled(false);

			// Texture Unit switch
			cEdInputBool* pSwitch = CreateInputBool(0,pPanel->mpLUnit->GetText(), pGroup);
			pSwitch->GetHandle()->SetVisible(false);
			pSwitch->GetHandle()->SetEnabled(false);
			pSwitch->SetUserData(pPanel);

			mvUnitSwitches.push_back(pSwitch);
		}
	}

	//////////////////////////////////////////////////////////
	// Material specific vars
	{
		pGroup = mpSet->CreateWidgetGroup(pGroup->GetLocalPosition() + cVector3f(0,pGroup->GetSize().y+10,0), cVector2f(880, 90), _W("Material Specific Variables"), pParent);

		mpFMaterialVars = mpSet->CreateWidgetFrame(cVector3f(15,10,0.1f), cVector2f(860,75), false, pGroup, false, true);
	}

	mbInitialized = true;
}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::OnSetActive(bool abX)
{
	iEdViewport::OnSetActive(abX);

	if(abX)
	{
		if(IsStandalone()==false)
		{
			OnReset();

			iWidget* pBG = GetBG();
			cVector3f vPos = pBG->GetGlobalPosition();
			cVector3f vCenter = cVector3f(mpSet->GetVirtualSize()-pBG->GetSize())*0.5f;
			vPos.x = vCenter.x;
			vPos.y = vCenter.y;

			pBG->SetGlobalPosition(vPos);

			mpSet->SetFocusedWidget(GetBG());
		}
		//mpEditor->GetWindowHandler()->mlEdPopUpCount++;

		if(msInitMatFile.empty()==false)
			mpMaterial->LoadFromFile(msInitMatFile);
	}
	else
	{
		if(IsStandalone())
		{
			if(mbInitialized) mpEditor->GetEngine()->Exit();
		}
		else
		{
            if(mbInitialized) mpEditor->GetWindowHandler()->DestroyWindow(this);
		}
	}
}

//------------------------------------------------------------------------------------

void cEdWindowMaterialEditor::OnUpdate()
{
	iEdViewport::OnUpdate();

	/////////////////////////////////////////////////////
	// Update type, physics material and blend mode
	const tWString& sType = mpMaterial->GetType();
	const tWString& sMat = mpMaterial->GetPhysicsMat();
	const tWString& sBlendMode = mpMaterial->GetBlendMode();
	mpInpMatType->SetValue(sType, false);
	mpInpDepthTest->SetValue(mpMaterial->GetDepthTest(), false);
	mpInpPhysicsMat->SetValue(sMat, false);
	mpInpBlendMode->SetValue(sBlendMode,false);

	// Update panels
	SetUpUnits();
	SetUpPanels();
	if(mpInputPanel) mpInputPanel->Update();
	
	
	/////////////////////////////////////////////////////
	// Update animation stuff
	int lSelectedAnim = mpInpUVAnimation->GetValue();
	mpInpUVAnimation->ClearValues();
	for(int i=0;i<mpMaterial->GetUVAnimNum();++i)
	{
		cMaterialUvAnimation* pAnim = mpMaterial->GetUVAnim(i);
		mpInpUVAnimation->AddValue(_W("Anim") + cString::ToStringW((float)i,0));
	}
	if(lSelectedAnim==-1 || lSelectedAnim>=mpInpUVAnimation->GetNumValues())
		lSelectedAnim = mpInpUVAnimation->GetNumValues()-1;
	
	mpInpUVAnimation->SetValue(lSelectedAnim,false);
	UpdateUVAnimInputs();
	
	//////////////////////////////////////////////////////
	// Update preview controls
	//mpInpBGColor->SetValue(mpMatWorld->GetSkyBoxColor(), false);
	mpInpFixedLightColor->SetValue(mpFixedLight->GetDiffuseColor(), false);
	mpInpMovableLightColor->SetValue(mpMovableLight->GetDiffuseColor(), false);
}

//------------------------------------------------------------------------------------

bool cEdWindowMaterialEditor::OnViewportUpdate(const cGuiMessageData& aData)
{
	if(mbPreviewNeedsUpdate)
	{
		mbPreviewNeedsUpdate=false;
		UpdatePreview(mpMaterial->GetPreviewMaterial());
	}

	if(mbRotationEnabled)
	{
		mmtxRotation = cMath::MatrixMul(cMath::MatrixRotate(cVector3f(0,0.005f,0), eEulerRotationOrder_XYZ), mmtxRotation);
		mpCurrentPreviewEntity->SetMatrix(mmtxRotation);
	}
	return true;
}

//------------------------------------------------------------------------------------

bool cEdWindowMaterialEditor::OnViewportMouseMove(const cGuiMessageData& aData)
{
	cVector2f vRel = aData.mvRel*0.01f;

	if(mbMoveCamera)
	{
		mpCamera->GetCurrentController()->Update(cViewportClick(this, aData));
	}
	if(mbMoveLight)
	{
		cVector2f vSphCoords = cMath::GetSphericalCoordsFromPoint3D(0, mpMovableLight->GetWorldPosition());
		vSphCoords += cVector2f(vRel.y, -vRel.x);
		vSphCoords.x = cMath::Clamp(vSphCoords.x, 0.01f, kPif-0.01f);
		mpMovableLight->SetPosition(cMath::GetPoint3DFromSphericalCoords(0, 2.5f, vSphCoords));
	}

	return true;
}

//------------------------------------------------------------------------------------

bool cEdWindowMaterialEditor::OnViewportMouseDown(const cGuiMessageData& aData)
{
	cViewportClick click(this, aData);

	if(mpCamera->GetCurrentController()->OnViewportMouseDown(click))
	{
		mbMoveCamera = true;
	}

	if(aData.mlVal&eGuiMouseButton_Right)
	{
		mbMoveLight = true;
	}

	return true;
}

//------------------------------------------------------------------------------------

bool cEdWindowMaterialEditor::OnViewportMouseUp(const cGuiMessageData& aData)
{
	cViewportClick click(this, aData);

	if(mpCamera->GetCurrentController()->OnViewportMouseUp(click))
	{
		mbMoveCamera = false;
	}
	if(click.mbRight)
	{
		mbMoveLight = false;
	}

	return true;
}

//------------------------------------------------------------------------------------

bool cEdWindowMaterialEditor::WindowSpecificInputCallback(iEdInput* apInput)
{
	cTextureUnitPanel* pPanel = static_cast<cTextureUnitPanel*>(apInput->GetUserData());
	if(pPanel && pPanel->PanelSpecificInputCallback(apInput)==true)
		return true;

	/////////////////////////////////////
	// Material type
	if(apInput==mpInpMatType)
	{
		mpMaterial->SetType(mpInpMatType->GetSelectedEnumValue());
		mbTypeChanged = true;

		SetUpUnits();
		SetUpPanels();
		SetUpVars();

		mbTypeChanged = false;
	}
	/////////////////////////////////////
	// Depth Test
	else if(apInput==mpInpDepthTest)
	{
		mpMaterial->SetDepthTest(mpInpDepthTest->GetValue());
	}
	/////////////////////////////////////
	// Physics material
	else if(apInput==mpInpPhysicsMat)
	{
		mpMaterial->SetPhysicsMaterial(mpInpPhysicsMat->GetSelectedEnumValue());
	}
	/////////////////////////////////////
	// Blend mode
	else if(apInput==mpInpBlendMode)
	{
		mpMaterial->SetBlendMode(mpInpBlendMode->GetSelectedEnumValue());
	}
	/////////////////////////////////////
	// UV Animation
	else if(apInput==mpInpUVAnimation)
	{
		UpdateUVAnimInputs();
	}
	/////////////////////////////////////
	// UV Animation Type
	else if(apInput==mpInpUVAnimType)
	{
		cMaterialUvAnimation* pAnim = mpMaterial->GetUVAnim(mpInpUVAnimation->GetValue());
		if(pAnim)
			pAnim->mType = (eMaterialUvAnimation)mpInpUVAnimType->GetValue();
		mpMaterial->SetUpdated();
	}
	/////////////////////////////////////
	// UV Animation Axis
	else if(apInput==mpInpUVAnimAxis)
	{
		cMaterialUvAnimation* pAnim = mpMaterial->GetUVAnim(mpInpUVAnimation->GetValue());
		if(pAnim)
			pAnim->mAxis = (eMaterialAnimationAxis)mpInpUVAnimAxis->GetValue();
		mpMaterial->SetUpdated();
	}
	/////////////////////////////////////
	// UV Animation Speed
	else if(apInput==mpInpUVAnimSpeed)
	{
		cMaterialUvAnimation* pAnim = mpMaterial->GetUVAnim(mpInpUVAnimation->GetValue());
		if(pAnim)
			pAnim->mfSpeed = mpInpUVAnimSpeed->GetValue();
		mpMaterial->SetUpdated();
	}
	/////////////////////////////////////
	// UV Animation Amplitude
	else if(apInput==mpInpUVAnimAmp)
	{
		cMaterialUvAnimation* pAnim = mpMaterial->GetUVAnim(mpInpUVAnimation->GetValue());
		if(pAnim)
			pAnim->mfAmp = mpInpUVAnimAmp->GetValue();
		mpMaterial->SetUpdated();
	}
	/////////////////////////////////////
	// Preview Model
	else if(apInput==mpInpPreviewModel)
	{
		if(mpCurrentPreviewEntity)
		{
			mpCurrentPreviewEntity->SetVisible(false);
			mpCurrentPreviewEntity->SetActive(false);
		}

		mpCurrentPreviewEntity = mvPreviewEntities[mpInpPreviewModel->GetValue()];
		mpCurrentPreviewEntity->SetVisible(true);
		mpCurrentPreviewEntity->SetActive(true);
	}
	/////////////////////////////////////
	// Background Type
	else if(apInput==mpInpBGType)
	{
		// CubeMap
		if(mpInpBGType->GetValue()==0)
		{
			iTexture* pTexture = NULL;
			if(cEdLoader::LoadCubeMap(mpInpBGCubeMap->GetValue8Char(), &pTexture))
			{
				mpMatWorld->SetSkyBox(pTexture,true);
				mpMatWorld->SetSkyBoxColor(cColor(1));
			}
			else
				mpInpBGType->SetValue(1);
		}
		// Flat
		else
		{
			mpMatWorld->SetSkyBox(NULL, true);
			mpMatWorld->SetSkyBoxColor(mpInpBGColor->GetValue());
		}
	}
	/////////////////////////////////////
	// Background Color
	else if(apInput==mpInpBGColor)
	{
		if(mpInpBGType->GetValue()==1)
			mpMatWorld->SetSkyBoxColor(mpInpBGColor->GetValue());
	}
	/////////////////////////////////////
	// Background CubeMap
	else if(apInput==mpInpBGCubeMap)
	{
		mpMatWorld->SetSkyBoxColor(mpInpBGColor->GetValue());
	}
	/////////////////////////////////////
	// Preview Model Rotate
	else if(apInput==mpInpRotateModel)
	{
		mbRotationEnabled = mpInpRotateModel->GetValue();
	}
	/////////////////////////////////////
	// Fixed Light Color
	else if(apInput==mpInpFixedLightColor)
	{
		mpFixedLight->SetDiffuseColor(mpInpFixedLightColor->GetValue());
	}
	/////////////////////////////////////
	// Movable Light Color
	else if(apInput==mpInpMovableLightColor)
	{
		mpMovableLight->SetDiffuseColor(mpInpMovableLightColor->GetValue());
	}
	else
	{
		/////////////////////////////////////
		// Texture Panel Inputs
		bool bInputInTexturePanels=false;
		for(size_t i=0;i<mvUnitSwitches.size();++i)
		{
			cEdInputBool* pSwitch = mvUnitSwitches[i];

			if(apInput==pSwitch)
			{
				cHplTexture* pTexWrapper = mpMaterial->GetTextureWrapper((eMaterialTexture)i);
				pTexWrapper->SetEnabled(pSwitch->GetValue());

				bInputInTexturePanels=true;
				break;
			}
		}
		if(bInputInTexturePanels==false)
			mpMaterial->SetUpdated();
	}

	SetUpdated();
	SetPreviewUpdated();

	return true;
}

//------------------------------------------------------------------------------------
