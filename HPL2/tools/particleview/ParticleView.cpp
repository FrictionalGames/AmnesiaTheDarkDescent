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

#include <hpl.h>

#include "../../tests/Common/SimpleCamera.h"

using namespace hpl;

cEngine *gpEngine=NULL;
cSimpleCamera *gpSimpleCamera=NULL;

tString gsParticleFile = "";

//--------------------------------------------------------------------------------

bool gbDrawAxes = false;
bool gbDrawGrid = false;
bool gbDrawBoundingBox = false;

bool gbDrawFloor = true;
bool gbDrawWalls = true;

int glCurrentAnimaiton = 0;

cParticleSystem *gpParticleSystem=NULL;
std::vector<cMeshEntity*> gvFloors;

std::vector<cLightSpot*> gvLights;
cLightBox * gpLightBox;

std::vector<iPhysicsBody*> gvFloorBodies;

cPostEffectComposite *gpPostEffectComp=NULL;


//--------------------------------------------------------------------------------

class cSimpleRenderCallback : public iRendererCallback
{
public:
	cSimpleRenderCallback()
	{

	};

	void OnPostSolidDraw(cRendererCallbackFunctions *apFunctions)
	{
		apFunctions->SetDepthTest(true);
		apFunctions->SetDepthWrite(false);
		apFunctions->SetBlendMode(eMaterialBlendMode_None);

		apFunctions->SetProgram(NULL);
		apFunctions->SetTextureRange(NULL, 0);
		apFunctions->SetMatrix(NULL);

		/*for(size_t i=0; i<gvLights.size(); ++i)
		{
			cLightSpot *pLight = gvLights[i];

            pLight->GetFrustum()->Draw(apFunctions->GetLowLevelGfx());
		}*/

		if(gbDrawBoundingBox && gpParticleSystem)
		{
			cBoundingVolume *pBV = gpParticleSystem->GetBoundingVolume();
			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(),pBV->GetMax(), cColor(1,1));
		}

		if(gbDrawGrid)
		{
			int lNum = 32;
			float fSize = 0.25f;
			float fStart = fSize *0.5f * (float)lNum + fSize;
			float fEnd = fSize *0.5f* (float)-lNum - fSize;
			float fY = -0.1;//gpFloor->GetWorldPosition().y + 0.05f;
			
			for(int i=-lNum/2; i<lNum/2+1;++i)
			{
				float fPos = fSize * (float)i;
				apFunctions->GetLowLevelGfx()->DrawLine(cVector3f(fPos,fY,fStart),cVector3f(fPos,fY,fEnd),cColor(0.5f,1));
				apFunctions->GetLowLevelGfx()->DrawLine(cVector3f(fStart,fY,fPos),cVector3f(fEnd,fY,fPos),cColor(0.5f,1));
			}
			
		}

		if(gbDrawAxes)
		{
			cVector3f vPosAdd = cVector3f(0, 0.01f, 0);
			apFunctions->SetDepthTest(false);
			apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(1,0,0)+vPosAdd,cColor(1,0,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(0,1,0)+vPosAdd,cColor(0,1,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(0,0,1)+vPosAdd,cColor(0,0,1,1));
			apFunctions->SetDepthTest(true);
		}
   	}
	
	void OnPostTranslucentDraw(cRendererCallbackFunctions *apFunctions)
	{

	}
	
	cWorld *mpWorld;
	iPhysicsWorld *mpPhysicsWorld;
	cVector3f mvDragPos;
};

//--------------------------------------------------------------------------------


class cMaterialData
{
public:
	tString msName;
	iTexture *mvMaterialTextures[eMaterialTexture_LastEnum];
	cMaterial *mpMat;
};

//--------------------------------------------------------------------------------

class cSimpleUpdate : public iUpdateable
{
public:
	cSimpleUpdate() : iUpdateable("Simple3D")
	{
		mpLowLevelGraphics = gpEngine->GetGraphics()->GetLowLevel();
		
		////////////////////////////////
		// Rendering setup
		gpEngine->GetResources()->GetMaterialManager()->SetTextureFilter(eTextureFilter_Trilinear);
		gpEngine->GetResources()->GetMaterialManager()->SetTextureSizeDownScaleLevel(0);

		////////////////////////////////
		// Create world
		mpWorld = gpEngine->GetScene()->CreateWorld("Test");
		
		////////////////////////////////
		// Create physics world
		gpEngine->GetPhysics()->LoadSurfaceData("materials.cfg");

		mpPhysicsWorld = gpEngine->GetPhysics()->CreateWorld(true);
		mpPhysicsWorld->SetAccuracyLevel(ePhysicsAccuracy_Medium);
		mpPhysicsWorld->SetWorldSize(-300,300);
		mpPhysicsWorld->SetMaxTimeStep(1.0f / 60.0f);

		mpWorld->SetPhysicsWorld(mpPhysicsWorld);

		
		////////////////////////////////
		// Setup sky box
		mpWorld->SetSkyBoxColor(cColor(0.0f, 1.0f));
		mpWorld->SetSkyBoxActive(true);
		
		////////////////////////////////
		// Render callback
		renderCallback.mpWorld = mpWorld;
		renderCallback.mpPhysicsWorld = mpPhysicsWorld;
		
		////////////////////////////////
		// Create meshes
		cMesh *pMesh =NULL;
		cMeshEntity *pBox = NULL;
				
		/////////////////
		//Floor
		gvFloors.resize(4);
		
		pMesh = gpEngine->GetResources()->GetMeshManager()->CreateMesh("modelview_rect.dae");
		gvFloors[0] = mpWorld->CreateMeshEntity("Floor",pMesh,true);
		gvFloors[0]->SetMatrix(cMath::MatrixScale(6));
		gvFloors[0]->SetPosition(cVector3f(0,-0.2f,0));
		gvFloors[0]->SetVisible(gbDrawFloor);

		for(int i=0; i<3; ++i)
		{
			pMesh = gpEngine->GetResources()->GetMeshManager()->CreateMesh("modelview_rect.dae");
			cMeshEntity *pFloor = mpWorld->CreateMeshEntity("Floor",pMesh,true);;
			
			cVector3f vPos(0,6.0f-0.2f,0);
			cMatrixf mtxTrans = cMath::MatrixScale(6);
			mtxTrans = cMath::MatrixMul(cMath::MatrixRotateX(kPi2f),mtxTrans);
			
			if(i==0)
			{
				vPos.x -= 6;
				mtxTrans = cMath::MatrixMul(cMath::MatrixRotateY(kPi2f),mtxTrans);
			}
			if(i==1)
			{
				vPos.x += 6;
				mtxTrans = cMath::MatrixMul(cMath::MatrixRotateY(-kPi2f),mtxTrans);
			}
			if(i==2)
			{
				vPos.z -= 6;
			}
			
            
			mtxTrans.SetTranslation(vPos);
			pFloor->SetMatrix(mtxTrans);
			gvFloors[i+1] = pFloor;

			cMatrixf mtxOffset = cMath::MatrixTranslate(cVector3f(0,-0.05f,0));
			iCollideShape *pShape = mpPhysicsWorld->CreateBoxShape(cVector3f(12,0.1f, 12),&mtxOffset);
			iPhysicsBody *pBody = mpPhysicsWorld->CreateBody("Floor", pShape);

			pBody->SetMatrix(mtxTrans);
		}
		
		
		/////////////////
		//Particle System
		gpParticleSystem = NULL;
		if(gsParticleFile != "") 
			LoadParticle(gsParticleFile);
		else
			mpLowLevelGraphics->SetWindowCaption("ParticleView - No model loaded!");

		/////////////////////////////////
		// Create Lights
		cLightSpot *pLightSpot=NULL;
		cLightPoint *pLightPoint=NULL;

		// Main spotlights
		for(int i=0; i<3; ++i)
		{
			cVector3f vPos;

			pLightSpot = mpWorld->CreateLightSpot("SpotLight","");
			pLightSpot->SetDiffuseColor(cColor(1,1,1,1.0f));
			
			//Key
			if(i==0)
			{
				pLightSpot->SetCastShadows(true);
				pLightSpot->SetDiffuseColor(cColor(0.7f,0.9f));
				vPos = cVector3f(3,4,6);
			}
			//Fill
			else if(i==1)
			{
				pLightSpot->SetCastShadows(false);
				pLightSpot->SetDiffuseColor(cColor(0.3f,0.3f,0.3f,0.75f));
				vPos = cVector3f(6,4,1);
			}
			//Back
			else if(i==2)
			{
				pLightSpot->SetCastShadows(false);
				pLightSpot->SetDiffuseColor(cColor(0.3f,0.0f));
				pLightSpot->SetVisible(false);
				vPos = cVector3f(-1,4,-6);
			}
			
			//Rest
			pLightSpot->SetFOV(cMath::ToRad(110.0f));
			pLightSpot->SetAspect(1.0f);
			pLightSpot->SetRadius(vPos.Length() * 2);
			cVector3f vRot = cMath::GetAngleFromPoints3D(vPos,0);
			pLightSpot->SetMatrix(cMath::MatrixRotate(cVector3f(vRot.x,vRot.y,0),eEulerRotationOrder_XYZ));
			pLightSpot->SetPosition(vPos);
			pLightSpot->SetShadowMapResolution(eShadowMapResolution_High);


			gvLights.push_back(pLightSpot);
		}

		// Box light
		gpLightBox = mpWorld->CreateLightBox("BoxLight");
		gpLightBox->SetVisible(true);
		gpLightBox->SetSize(cVector3f(20.0f));
		gpLightBox->SetDiffuseColor(cColor(0.2f,1.0f));

			
		/////////////////////////////////
		// Compile world
		mpWorld->Compile(false);

		/////////////////////////////////
		// Init variables
		msCurrentFilePath = _W("");
	}
	
	//--------------------------------------------------------------

	void SetupView()
	{
		gpEngine->GetInput()->GetLowLevel()->LockInput(false);

		cRenderSettings *pSettings = gpSimpleCamera->GetViewport()->GetRenderSettings();
		gpSimpleCamera->GetViewport()->AddRendererCallback(&renderCallback);
		
		gpSimpleCamera->SetMouseMode(true);

		//////////////////////////
		//Set up post effects
		cGraphics *pGraphics = gpEngine->GetGraphics();
		gpPostEffectComp = pGraphics->CreatePostEffectComposite();
		gpSimpleCamera->GetViewport()->SetPostEffectComposite(gpPostEffectComp);

		//Bloom
		cPostEffectParams_Bloom bloomParams;
		gpPostEffectComp->AddPostEffect(pGraphics->CreatePostEffect(&bloomParams));

		CreateGuiWindow();
	}

	//--------------------------------------------------------------

	void OnExit()
	{
	}

	//--------------------------------------------------------------

	void LoadParticle(const tString& asFileName)
	{
		if(gpParticleSystem != NULL)
		{
			mpWorld->DestroyParticleSystem(gpParticleSystem);
			gpEngine->GetResources()->GetParticleManager()->DestroyUnused(0);
		}
		gpParticleSystem = mpWorld->CreateParticleSystem("temp",asFileName,1);
		gpParticleSystem->SetPosition(cVector3f(0,3,0));
		

		Log("--------- PARTICLE SYSTEM INFO --------\n");

		

		Log("---------------------------------------\n");

	}
	
	//--------------------------------------------------------------

	void InitGuiAfterLoad()
	{	
		
	}
	
	//--------------------------------------------------------------

	void CreateGuiWindow()
	{
		cWidgetCheckBox *pCheckBox=NULL;
		cWidgetButton *pButton = NULL;
		cWidgetComboBox *pComboBox = NULL;
		cWidgetLabel *pLabel = NULL;
		cWidgetGroup *pGroup = NULL;
		cWidgetSlider *pSlider = NULL;

		cGuiSet *pSet = gpSimpleCamera->GetSet();

		cRenderSettings *pSettings = gpSimpleCamera->GetViewport()->GetRenderSettings();


		cVector3f vGroupPos =0;
		cVector2f vGroupSize =0;

		///////////////////////////
		//Window
		cVector2f vSize = cVector2f(250, 762);
		vGroupSize.x = vSize.x - 20;
		cVector3f vPos = cVector3f(pSet->GetVirtualSize().x - vSize.x - 3, 3, 0);
		mpOptionWindow = pSet->CreateWidgetWindow(0,vPos,vSize,_W("ParticleView Toolbar") );
		
		vSize = cVector2f(vSize.x-30, 18);
		vPos = cVector3f(10, 30, 0.1f);

		///////////////////////////
		// Loading
		{
			//Group
			vGroupPos = cVector3f(5,10,0.1f);
			pGroup = pSet->CreateWidgetGroup(vPos,100,_W("Loading"),mpOptionWindow);

			//Load File
			pButton = pSet->CreateWidgetButton(vGroupPos,vSize,_W("Load Particle"),pGroup);
			pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressLoadParticle));
			vGroupPos.y += 22;

			//Reload File
			pButton = pSet->CreateWidgetButton(vGroupPos,vSize,_W("Reload"),pGroup);
			pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressReload));
			vGroupPos.y += 22;

			//Group end
			vGroupSize.y = vGroupPos.y + 5;
			pGroup->SetSize(vGroupSize);
			vPos.y += vGroupSize.y + 15;
		}

		///////////////////////////
		// Environment
		{
			//Group
			vGroupPos = cVector3f(5,10,0.1f);
			pGroup = pSet->CreateWidgetGroup(vPos,100,_W("Environment"),mpOptionWindow);

			//Walls and Floor
			cVector2f vCheckSize = vSize; vCheckSize.x /= 2;
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos,vCheckSize,_W("Draw Floor"),pGroup);
			pCheckBox->SetChecked(gbDrawFloor);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeDrawFloor));

			cVector3f vAddPos(vCheckSize.x, 0,0);
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos + vAddPos,vCheckSize,_W("Draw Walls"),pGroup);
			pCheckBox->SetChecked(gbDrawWalls);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeDrawWalls));

			vGroupPos.y += 22;


			//Grid and Axes
			vCheckSize = vSize; vCheckSize.x /= 2;
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos,vCheckSize,_W("Draw grid"),pGroup);
			pCheckBox->SetChecked(gbDrawGrid);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeDrawGrid));
			
			vAddPos = cVector3f(vCheckSize.x, 0,0);
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos + vAddPos,vCheckSize,_W("Draw axes"),pGroup);
			pCheckBox->SetChecked(gbDrawAxes);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeDrawAxes));

			vGroupPos.y += 22;

			//Background color
			cVector2f vASize = vSize;
			vASize.x = vASize.x*0.5f;
			pLabel = pSet->CreateWidgetLabel(vGroupPos,vASize,_W("Background:"),pGroup);
			pComboBox = pSet->CreateWidgetComboBox(vGroupPos+cVector3f(vASize.x,0,0),vASize,_W(""),pGroup);
			pComboBox->AddItem(_W("Black"));
			pComboBox->AddItem(_W("Grey"));
			pComboBox->AddItem(_W("White"));
			pComboBox->SetSelectedItem(0);
			pComboBox->AddCallback(eGuiMessage_SelectionChange,this, kGuiCallback(ChangeBackgroundColor));
			vGroupPos.y += 22;
			
			//Group end
			vGroupSize.y = vGroupPos.y + 5;
			pGroup->SetSize(vGroupSize);
			vPos.y += vGroupSize.y + 15;
		}

		///////////////////////////
		// Lights
		{
			//Group
			vGroupPos = cVector3f(5,10,0.1f);
			pGroup = pSet->CreateWidgetGroup(vPos,100,_W("Lights"),mpOptionWindow);

			//SSAO
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos,vSize,_W("SSAO"),pGroup);
			pCheckBox->SetChecked(pSettings->mbSSAOActive);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeSSAOActive));
			vGroupPos.y += 22;

			//Bloom
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos,vSize,_W("Bloom"),pGroup);
			pCheckBox->SetChecked(gpPostEffectComp->GetPostEffect(0)->IsActive());
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeBloomActive));
			vGroupPos.y += 22;

			//Ambient Color
			cVector3f vAmbSlidePos = vGroupPos; vAmbSlidePos.x += 55;
			cVector2f vAmbSlideSize = vSize; vAmbSlideSize.x -= 55;

			pSlider = pSet->CreateWidgetSlider(eWidgetSliderOrientation_Horisontal,vAmbSlidePos,vAmbSlideSize,255,pGroup);
			pSlider->AddCallback(eGuiMessage_SliderMove,this, kGuiCallback(ChangeAmbientColor));
			pSlider->SetValue( (int)(gpLightBox->GetDiffuseColor().r * 255.0 + 0.5f));
			pLabel = pSet->CreateWidgetLabel(vGroupPos, vSize, _W("Ambient:"), pGroup);
			vGroupPos.y += 22;
			
			vGroupPos.y += 5;

			//Group end
			vGroupSize.y = vGroupPos.y + 5;
			pGroup->SetSize(vGroupSize);
			vPos.y += vGroupSize.y + 15;
		}
				
		
		InitGuiAfterLoad();
	}
	
	//--------------------------------------------------------------
	
	bool PressLoadParticle(iWidget* apWidget,cGuiMessageData& aData)
	{
		mvPickedFiles.clear();
		cGuiSet *pSet = gpSimpleCamera->GetSet();
		cGuiPopUpFilePicker* pPicker = pSet->CreatePopUpLoadFilePicker(mvPickedFiles,false,msCurrentFilePath,false, this, kGuiCallback(LoadModelFromFilePicker));
		int lCat = pPicker->AddCategory(_W("Particles"),_W("*.ps"));
		
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,PressLoadParticle)  

	bool PressReload(iWidget* apWidget,cGuiMessageData& aData)
	{
		LoadParticle(gsParticleFile);
		InitGuiAfterLoad();

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,PressReload)  

	
	bool LoadModelFromFilePicker(iWidget* apWidget,cGuiMessageData& aData)
	{
		if(mvPickedFiles.empty()) return true;

		tWString& sFilePath = mvPickedFiles[0];

		msCurrentFilePath = cString::GetFilePathW(sFilePath);
		gpEngine->GetResources()->AddResourceDir(msCurrentFilePath,false);
		
		gsParticleFile = cString::To8Char(sFilePath);
		LoadParticle(gsParticleFile);  
		InitGuiAfterLoad();

		return true;		
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,LoadModelFromFilePicker)  


	//--------------------------------------------------------------

	bool ChangeDrawFloor(iWidget* apWidget,cGuiMessageData& aData)
	{
		if(gvFloors.size() < 1) return true;
		
		gvFloors[0]->SetVisible(aData.mlVal == 1);
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeDrawFloor)

	bool ChangeDrawWalls(iWidget* apWidget,cGuiMessageData& aData)
	{
		for(size_t i=1; i< gvFloors.size(); ++i)
		{
			gvFloors[i]->SetVisible(aData.mlVal == 1);
		}

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeDrawWalls)

	
	bool ChangeDrawGrid(iWidget* apWidget,cGuiMessageData& aData)
	{
		gbDrawGrid = aData.mlVal == 1;
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeDrawGrid)  
	
	bool ChangeDrawAxes(iWidget* apWidget,cGuiMessageData& aData)
	{
		gbDrawAxes = aData.mlVal == 1;
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeDrawAxes)  
	
	bool ChangeBackgroundColor(iWidget* apWidget,cGuiMessageData& aData)
	{
		cColor gvBGColors[] = {cColor(0,1),cColor(0.5f,1),cColor(1,1) };

		mpWorld->SetSkyBoxColor(gvBGColors[aData.mlVal]);
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeBackgroundColor)

	bool ChangeAmbientColor(iWidget* apWidget,cGuiMessageData& aData)
	{
		gpLightBox->SetDiffuseColor(cColor( ((float)aData.mlVal) / 255.0f, 1.0f ));
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeAmbientColor)  

	bool ChangeSSAOActive(iWidget* apWidget,cGuiMessageData& aData)
	{
		cRenderSettings *pSettings = gpSimpleCamera->GetViewport()->GetRenderSettings();

		pSettings->mbSSAOActive = aData.mlVal == 1;

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeSSAOActive)

		
	bool ChangeBloomActive(iWidget* apWidget,cGuiMessageData& aData)
	{
		gpPostEffectComp->GetPostEffect(0)->SetActive(aData.mlVal == 1);

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeBloomActive)
	//--------------------------------------------------------------

	bool ChangeShowAABB(iWidget* apWidget,cGuiMessageData& aData)
	{
		gbDrawBoundingBox = aData.mlVal == 1;

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeShowAABB) 	

	//--------------------------------------------------------------

	void OnDraw(float afFrameTime)
	{
		float fY = 30;

		//return;
		if(gpSimpleCamera->GetFont())
		{
			cGuiSet *pSet = gpSimpleCamera->GetSet();
			iFontData *pFont = gpSimpleCamera->GetFont();


			int lTexMem = gpEngine->GetResources()->GetTextureManager()->GetMemoryUsage()/1024;
			pSet->DrawFont(pFont,cVector3f(5,fY,0),14,cColor(1,1),	_W("Texture memory usage: %d kb / %.1f Mb"),
							lTexMem, ((float)lTexMem) / 1024.0);
			fY += 16;
			
			//pSet->DrawFont(pFont,cVector3f(5,50,0),14,cColor(1,1),
			//	_W("Angle: %f"),gvJoints[0]->GetAngle());

			
		}
	}

	//--------------------------------------------------------------

	void Update(float afTimeStep)
	{
		//Debug when doing update debugg
		//Log("-----\n");
		//////////////////////////////////////////
		
	}


	//--------------------------------------------------------------


	~cSimpleUpdate()
	{
	
	}

public:
	iLowLevelGraphics* mpLowLevelGraphics;
	cWorld* mpWorld;
	iPhysicsWorld *mpPhysicsWorld;
	
	cSimpleRenderCallback renderCallback;
		
	cWidgetWindow *mpOptionWindow;

	tWStringVec mvPickedFiles;
	tWString msCurrentFilePath;
};

//-----------------------------------------------------------------------

#ifdef WIN32
	#include <Windows.h>
#endif

int hplMain(const tString &asCommandline)
{
//To allow drag and drop:
#ifdef WIN32
	TCHAR buffer[MAX_PATH];
	HMODULE module = GetModuleHandle(NULL);
	GetModuleFileName(module, buffer,MAX_PATH);
	tString sDir = cString::GetFilePath(buffer);
	SetCurrentDirectory(sDir.c_str());
#endif

	//iResourceBase::SetLogCreateAndDelete(true);
	//iGpuProgram::SetLogDebugInformation(true); 
	cRendererDeferred::SetGBufferType(eDeferredGBuffer_32Bit);
	cRendererDeferred::SetNumOfGBufferTextures(3);
	cRendererDeferred::SetSSAOLoaded(true);

	SetLogFile(_W("particleview.log"));
	
	//Init the game engine
	cEngineInitVars vars;
	vars.mGraphics.mvScreenSize.x = 1024;
	vars.mGraphics.mvScreenSize.y = 768;
	vars.mGraphics.mbFullscreen = false;
	vars.mGraphics.msWindowCaption = "ParticleView - Initalizing...";
	gpEngine = CreateHPLEngine(eHplAPI_OpenGL, eHplSetup_All, &vars);
	gpEngine->SetLimitFPS(false);
	gpEngine->GetGraphics()->GetLowLevel()->SetVsyncActive(false);
	gpEngine->SetWaitIfAppOutOfFocus(true);
	


	if(asCommandline != "")
	{
		Log("Command line: %s\n", asCommandline.c_str());
		gsParticleFile = asCommandline;
		gsParticleFile = cString::ReplaceCharTo(gsParticleFile,"\"","");

		tString sModelDir = cString::GetFilePath(gsParticleFile);
		tWString sDir = cString::To16Char(sModelDir);
		if(sDir != _W(""))
			gpEngine->GetResources()->AddResourceDir(sDir,false);
	}
	
	//Add resources
	gpEngine->GetResources()->LoadResourceDirsFile("resources.cfg");
	
	//Add updates
	cSimpleUpdate Update;
	gpEngine->GetUpdater()->AddUpdate("Default", &Update);
	
	gpSimpleCamera = hplNew(cSimpleCamera, (Update.GetName(),gpEngine, Update.mpWorld, 10, cVector3f(0,0,9), true) );
	
	gpEngine->GetUpdater()->AddUpdate("Default", gpSimpleCamera);

	Update.SetupView();

	//Run the engine
	gpEngine->Run();

	
	hplDelete (gpSimpleCamera);

	//Delete the engine
	DestroyHPLEngine(gpEngine);

	cMemoryManager::LogResults();

	return 0;
}
