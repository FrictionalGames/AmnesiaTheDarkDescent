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

#include "hpl.h"

#include "../../tests/Common/SimpleCamera.h"

using namespace hpl;

cEngine *gpEngine=NULL;
cSimpleCamera *gpSimpleCamera=NULL;

tString gsModelFile = "";

//--------------------------------------------------------------------------------

bool gbDrawPhysicsDebug = false;
bool gbDrawAxes = false;
bool gbDrawGrid = false;
bool gbDrawBoundingBox = false;
bool gbDrawSkeleton = false;
bool gbDrawSoundsPlaying= true;
bool gbRagdollActive=false;
int glAnimationState = 1; //0=stopped, 1=play, 2=paus
int glOnLoadAnimationState = 1;

bool gbRotateModel = false;
bool gbAmbientLight = true;
bool gbPhysicsActive = true;
bool gbShowInvalidVertices = false;

int glCurrentAnimaiton = 0;


cMeshEntity *gpFloor=NULL;
cMeshEntity *gpEntity=NULL;

std::vector<cLightSpot*> gvLights;
cLightBox * gpLightBox;

std::vector<iPhysicsBody*> gvBodies;
std::vector<iPhysicsJoint*> gvJoints;
std::vector<cParticleSystem*> gvParticleSystems;
std::vector<cBillboard*> gvBillboards;
std::vector<cSoundEntity*> gvSoundEntities;
std::vector<iLight*> gvModelLights;

iPhysicsBody* gpFloorBody;
std::vector<cMatrixf> gvBodyStartMatrix;

cPostEffectComposite *gpPostEffectComp=NULL;

//--------------------------------------------------------------------------------

class cSimpleObjectLoader : public cEntityLoader_Object
{
public:
	cSimpleObjectLoader(const tString &asName) : cEntityLoader_Object(asName)
	{
	}

	void BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars)
	{

	}
	
	void AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars)
	{
		gvBodies = mvBodies;
		
		gvBodyStartMatrix.resize(gvBodies.size());
		for(size_t i=0; i<gvBodies.size(); ++i)
		{
			gvBodyStartMatrix[i] = gvBodies[i]->GetWorldMatrix();
		}

		gvJoints = mvJoints;

		gvSoundEntities = mvSoundEntities;
		gvParticleSystems = mvParticleSystems;
		gvBillboards = mvBillboards;
		gvModelLights = mvLights;
	}
};


//--------------------------------------------------------------------------------

class cBodyPicker : public iPhysicsRayCallback
{
public:
	cBodyPicker()
	{
		Clear();
	}

	void Clear()
	{
		mfLastT = 100000.0f;
		mpPickedBody = NULL;
	}

	bool OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams)
	{
		if(pBody->GetMass()>0)
		{
			if(apParams->mfT < mfLastT)
			{
				mpPickedBody = pBody;
				mfLastT = apParams->mfT;
				mvPos = apParams->mvPoint;
				mfDist = apParams->mfDist;
			}
		}

		return true;
	}

	iPhysicsBody* mpPickedBody;
	cVector3f mvPos;
	cVector3f mvLocalPos;
	float mfLastT;
	float mfDist;
};

//--------------------------------------------------------------------------------

class cSimpleRenderCallback : public iRendererCallback
{
public:
	cSimpleRenderCallback()
	{

	};

	void DrawSkeletonRec(cRendererCallbackFunctions *apFunctions, cNode3D *apBoneState)
	{
		cVector3f vCameraSpacePos = cMath::MatrixMul(apFunctions->GetFrustum()->GetViewMatrix(), apBoneState->GetWorldPosition());		
		float fSize = cMath::Min(-0.01f * vCameraSpacePos.z, 0.03f);
		apFunctions->GetLowLevelGfx()->DrawSphere(apBoneState->GetWorldPosition(),fSize,cColor(1,0,0,1));
		
		//Draw bone bounding radii
		/*int lBoneIdx = gpEntity->GetBoneStateIndexFromPtr(static_cast<cBoneState*>(apBoneState));
		if(lBoneIdx >0)
		{
			float fRadius = gpEntity->GetMesh()->GetBoneBoundingRadius(lBoneIdx);
			apFunctions->GetLowLevelGfx()->DrawSphere(apBoneState->GetWorldPosition(),fRadius,cColor(0,1,0,1));
		}*/
		
		
		cNode3DIterator it = apBoneState->GetChildIterator();
		while(it.HasNext())
		{
			cNode3D *pChild = static_cast<cNode3D*>(it.Next());
			apFunctions->GetLowLevelGfx()->DrawLine(apBoneState->GetWorldPosition(), pChild->GetWorldPosition(), cColor(1,1));
			DrawSkeletonRec(apFunctions, pChild);
		}
	}
	
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

		if(gbShowInvalidVertices)
		{
			for(int i=0; i<gpEntity->GetSubMeshEntityNum(); ++i)
			{
				cSubMeshEntity *pSubEnt = gpEntity->GetSubMeshEntity(i);
				cSubMesh *pSubMesh = pSubEnt->GetSubMesh();
				iVertexBuffer *pVtxBuff = pSubMesh->GetVertexBuffer();
				
				tIntVec vVtxBindCount;
				vVtxBindCount.resize(pVtxBuff->GetVertexNum(), 0);

				for(int i=0; i<pSubMesh->GetVertexBonePairNum(); ++i)
				{
					cVertexBonePair *pPair = &pSubMesh->GetVertexBonePair(i);
					
                    vVtxBindCount[pPair->vtxIdx]++;
				}

				float *pPosArray = pVtxBuff->GetFloatArray(eVertexBufferElement_Position);
				float *pNrmArray = pVtxBuff->GetFloatArray(eVertexBufferElement_Normal);
				for(size_t i=0; i<vVtxBindCount.size(); ++i)
				{
                    int lCount = vVtxBindCount[i];
					if(lCount>0 && lCount<=4) continue;

					float *pPos = &pPosArray[i * pVtxBuff->GetElementNum(eVertexBufferElement_Position)];
					cVector3f vPos(pPos[0],pPos[1],pPos[2]);

					float *pNrm = &pNrmArray[i * pVtxBuff->GetElementNum(eVertexBufferElement_Normal)];
					cVector3f vNrm(pNrm[0],pNrm[1],pNrm[2]);
                    
					vPos = cMath::MatrixMul(pSubEnt->GetWorldMatrix(), vPos);
					cVector3f vCamPos = cMath::MatrixMul(gpSimpleCamera->GetCamera()->GetViewMatrix(), vPos);

					// Check if bad 
					bool bTooManyBinds = lCount > 4;

					float fSqrLen = vNrm.SqrLength();
					bool bBadNormal = fSqrLen < 0.97f || fSqrLen > 1.03;

					float fRadius = vCamPos.z * 0.02f;

					if(bTooManyBinds && bBadNormal)
						apFunctions->GetLowLevelGfx()->DrawSphere(vPos,fRadius, cColor(1,1,0));
					else if(bTooManyBinds)
						apFunctions->GetLowLevelGfx()->DrawSphere(vPos,fRadius, cColor(1,0,0));
					else if(bBadNormal)
						apFunctions->GetLowLevelGfx()->DrawSphere(vPos,fRadius, cColor(0,1,0));
				}
  			}	
		}

		if(gbDrawBoundingBox)
		{
			//cBoundingVolume *pBV = gpEntity->GetBoundingVolume();
            //apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(),pBV->GetMax(), cColor(1,1));
			for(int i=0; i<gpEntity->GetSubMeshEntityNum(); ++i)
			{
				cSubMeshEntity *pSubEnt = gpEntity->GetSubMeshEntity(i);
				
				cBoundingVolume *pBV = pSubEnt->GetBoundingVolume();
				apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(),pBV->GetMax(), cColor(1,1));
				
				cVector3f vLocalCenter = (pBV->GetLocalMin() + pBV->GetLocalMax())/2.0f;

				cVector3f  vWorldCenter;
				/*vWorldCenter = cMath::MatrixMul(pSubEnt->GetWorldMatrix(), vLocalCenter);//pBV->GetLocalCenter());
				apFunctions->GetLowLevelGfx()->DrawSphere(vWorldCenter,0.01f, cColor(0,1,1));*/

				vWorldCenter = cMath::MatrixMul(pSubEnt->GetWorldMatrix(), pBV->GetLocalCenter());
				apFunctions->GetLowLevelGfx()->DrawSphere(vWorldCenter,0.01f, cColor(1,1,0));
			}
		}

		if(gbDrawSkeleton && gpEntity)
		{
			apFunctions->SetDepthTest(false);
			if(gpEntity->GetMesh()->GetSkeleton())
			{
				cNode3DIterator it = gpEntity->GetBoneStateRoot()->GetChildIterator();
				while(it.HasNext())
				{
					cNode3D *pChild = static_cast<cNode3D*>(it.Next());
					DrawSkeletonRec(apFunctions ,pChild);
				}
			}
			apFunctions->SetDepthTest(true);
		}


		if(gbPhysicsActive && mpBodyPicker->mpPickedBody)
		{
			//cBoundingVolume *pBV = mBodyPicker.mpPickedBody->GetBV();
			//mpLowLevelGraphics->DrawBoxMaxMin(pBV->GetMax(), pBV->GetMin(),cColor(1,0,1,1));

			apFunctions->GetLowLevelGfx()->DrawSphere(mpBodyPicker->mvPos,0.1f, cColor(1,0,0,1));
			apFunctions->GetLowLevelGfx()->DrawSphere(mvDragPos,0.1f, cColor(1,0,0,1));

			apFunctions->GetLowLevelGfx()->DrawLine(mpBodyPicker->mvPos, mvDragPos, cColor(1,1,1,1));
		}

		if(gbDrawGrid)
		{
			int lNum = 32;
			float fSize = 0.25f;
			float fStart = fSize *0.5f * (float)lNum + fSize;
			float fEnd = fSize *0.5f* (float)-lNum - fSize;
			float fY = 0;//gpFloor->GetWorldPosition().y + 0.05f;
			
			for(int i=-lNum/2; i<lNum/2+1;++i)
			{
				float fPos = fSize * (float)i;
				apFunctions->GetLowLevelGfx()->DrawLine(cVector3f(fPos,fY,fStart),cVector3f(fPos,fY,fEnd),cColor(0.5f,1));
				apFunctions->GetLowLevelGfx()->DrawLine(cVector3f(fStart,fY,fPos),cVector3f(fEnd,fY,fPos),cColor(0.5f,1));
			}
			
		}

		//apFunctions->GetLowLevelGfx()->DrawBoxMinMax(gpEntity->GetBoundingVolume()->GetMin(),gpEntity->GetBoundingVolume()->GetMax(),cColor(1,1,1,1));
		
		if(gbPhysicsActive && gbDrawPhysicsDebug)
		{
			mpPhysicsWorld->RenderDebugGeometry(apFunctions->GetLowLevelGfx(),cColor(1,1,1,1));

			apFunctions->SetDepthTest(false);
			for(size_t i=0;i<gvJoints.size(); ++i)
			{
				iPhysicsJoint *pJoint = gvJoints[i];
				if(mpPhysicsWorld->JointExists(pJoint)==false) continue;
                
				cVector3f vPivot = pJoint->GetPivotPoint();
				apFunctions->GetLowLevelGfx()->DrawSphere(vPivot,0.2f,cColor(1,0,0,1));
				apFunctions->GetLowLevelGfx()->DrawLine(vPivot,vPivot + pJoint->GetPinDir()*0.25 ,cColor(0,1,0,1));
			}
			apFunctions->SetDepthTest(true);
		}

		if(gbDrawAxes)
		{
			apFunctions->SetDepthTest(false);
			apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(1,0,0),cColor(1,0,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(0,1,0),cColor(0,1,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(0,0,1),cColor(0,0,1,1));
			apFunctions->SetDepthTest(true);
		}
   	}
	
	void OnPostTranslucentDraw(cRendererCallbackFunctions *apFunctions)
	{

	}
	
	cWorld *mpWorld;
	iPhysicsWorld *mpPhysicsWorld;
	cBodyPicker *mpBodyPicker;
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
		// Init some pointers
		mpCBLightVisible = NULL;
		mpCBLightCastShadows = NULL;
		mpCBPhysicsActive = NULL;
		mpCBRotateModel = NULL;
		
		////////////////////////////////
		// Rendering setup
		gpEngine->GetResources()->GetMaterialManager()->SetTextureFilter(eTextureFilter_Trilinear);
		gpEngine->GetResources()->GetMaterialManager()->SetTextureSizeDownScaleLevel(0);

		////////////////////////////////
		// Setup load
		gpEngine->GetResources()->AddEntityLoader(hplNew(cSimpleObjectLoader,("Object")),true);

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
		renderCallback.mpBodyPicker = &mBodyPicker;

		////////////////////////////////
		// Create meshes
		cMesh *pMesh =NULL;
		cMeshEntity *pBox = NULL;
				
		/////////////////
		//Floor
		pMesh = gpEngine->GetResources()->GetMeshManager()->CreateMesh("modelview_rect.dae");
		gpFloor = mpWorld->CreateMeshEntity("Floor",pMesh,true);
		gpFloor->SetMatrix(cMath::MatrixScale(6));
		gpFloor->SetRenderFlagBit(eRenderableFlag_ShadowCaster, true);
		//gpFloor->SetVisible(true);

		/*std::vector<iCollideShape*> vShapes;
		for(int i=0; i<1000; ++i)
		{
			cMatrixf mtxOffset;
			if(i%3==0) mtxOffset = cMath::MatrixTranslate(cVector3f(0,-0.05f,0));
			if(i%3==1) mtxOffset = cMath::MatrixTranslate(cVector3f(0,-0.35f,0));
			if(i%3==2) mtxOffset = cMath::MatrixTranslate(cVector3f(0,-0.15f,0));
			vShapes.push_back(mpPhysicsWorld->CreateBoxShape(cVector3f(12,0.1f, 12),&mtxOffset));
		}
		iCollideShape *pShape = mpPhysicsWorld->CreateCompundShape(vShapes);
		gpFloorBody = mpPhysicsWorld->CreateBody("Floor", pShape);*/

		cMatrixf mtxOffset = cMath::MatrixTranslate(cVector3f(0,-0.05f,0));
		iCollideShape *pShape = mpPhysicsWorld->CreateBoxShape(cVector3f(12,0.1f, 12),&mtxOffset);
		gpFloorBody = mpPhysicsWorld->CreateBody("Floor", pShape);
		
		
		/////////////////
		//Entity
		gpEntity = NULL;
		if(gsModelFile != "") 
			LoadModel(gsModelFile);
		else
			mpLowLevelGraphics->SetWindowCaption("ModelView - No model loaded!");

		/////////////////////////////////
		// Create Lights
		cLightSpot *pLightSpot=NULL;
		cLightPoint *pLightPoint=NULL;

		// Main spotlights
		for(int i=0; i<3; ++i)
		{
			cVector3f vPos;

			pLightSpot = mpWorld->CreateLightSpot("SpotLight"+cString::ToString(i),"");
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
		gpLightBox->SetVisible(gbAmbientLight);
		gpLightBox->SetSize(cVector3f(50.0f));
		gpLightBox->SetDiffuseColor(cColor(0.35f,1.0f));

			
		/////////////////////////////////
		// Compile world
		mpWorld->Compile(false);

		/////////////////////////////////
		// Load extra

		mpTexDiffuseNull = gpEngine->GetResources()->GetTextureManager()->Create2D("modelview_diffuse_null.jpg",true);
		mpTexNMapNull = gpEngine->GetResources()->GetTextureManager()->Create2D("modelview_nmap_null.jpg",true);
		
		/////////////////////////////////
		// Init variables
		msCurrentFilePath = _W("");
		mfModelRotation =0;
	}
	
	//--------------------------------------------------------------

	void SetupView()
	{
		gpEngine->GetInput()->GetLowLevel()->LockInput(false);
		gpEngine->GetInput()->GetLowLevel()->RelativeMouse(false);

		cRenderSettings *pSettings = gpSimpleCamera->GetViewport()->GetRenderSettings();
		gpSimpleCamera->GetViewport()->AddRendererCallback(&renderCallback);

		gpSimpleCamera->SetMouseMode(true);

		//////////////////////////
		//Settings
		pSettings->mbUseEdgeSmooth = true;

		//////////////////////////
		//Set up post effects
		cGraphics *pGraphics = gpEngine->GetGraphics();
		gpPostEffectComp = pGraphics->CreatePostEffectComposite();
		gpSimpleCamera->GetViewport()->SetPostEffectComposite(gpPostEffectComp);

		//Bloom
		cPostEffectParams_Bloom bloomParams;
		gpPostEffectComp->AddPostEffect(pGraphics->CreatePostEffect(&bloomParams), 10);
		//gpPostEffectComp->GetPostEffect(0)->SetActive(false);

		CreateGuiWindow();
	}

	//--------------------------------------------------------------

	void OnExit()
	{
		ResetEntitySubMeshes();
	}

	//--------------------------------------------------------------

	cMaterialData* GetMaterialData(const tString& asName, bool abShowErrorMess)
	{
		tString sProperName = cString::GetFileName(cString::SetFileExt(asName,""));

		for(size_t i=0; i< mvMaterialData.size(); ++i)
		{
			
			if(mvMaterialData[i].msName == sProperName) return &mvMaterialData[i];
		}
		
		if(abShowErrorMess)
		{
			cPlatform::CreateMessageBox(_W("Error!"),_W("Could not find material data for '%ls'\n"), cString::To16Char(asName).c_str());
		}

		return NULL;
	}

	tWStringVec GetTextureExtensions(eMaterialTexture aType)
	{
		tWStringVec vExt;
		switch(aType)
		{
		case eMaterialTexture_Diffuse:	vExt.push_back(_W(""));
										break;
		case eMaterialTexture_Alpha:	vExt.push_back(_W("alpha"));
										break;	
		case eMaterialTexture_NMap:		vExt.push_back(_W("nrm"));
										vExt.push_back(_W("bump"));
										break;	
		case eMaterialTexture_Height:	vExt.push_back(_W("height"));
										break;
		case eMaterialTexture_Illumination:		vExt.push_back(_W("illum"));
												break;
		case eMaterialTexture_Specular:	vExt.push_back(_W("spec"));
										break;
		case eMaterialTexture_CubeMap:	vExt.push_back( _W("cube"));
										break;
		case eMaterialTexture_CubeMapAlpha:	vExt.push_back( _W("cube_alpha"));
										break;
		}

		return vExt;
	}

	void CreateDefaultMaterial(cMesh *pMesh, const tString& asName)
	{
		/////////////////////////
		// Get the paths
		tWString sPath = gpEngine->GetResources()->GetFileSearcher()->GetFilePath(pMesh->GetName());		
		if(sPath == _W("")){
			FatalError("Could not find path for mesh file '%s'\n", pMesh->GetName().c_str());
		}
		
		tWString sDir = cString::GetFilePathW(sPath);
		tWString sMatPath = cString::SetFilePathW(cString::To16Char(asName), sDir);

		Log("Creating material file: '%s'\n", cString::To8Char(sMatPath).c_str());
		
		/////////////////////////
		// Create Xml document
		iXmlDocument *pXmlDoc = gpEngine->GetResources()->GetLowLevel()->CreateXmlDocument("Material");
		
		/////////////////////////
		// Main
		cXmlElement *pMainElem = pXmlDoc->CreateChildElement("Main");
		pMainElem->SetAttributeString("Type","SolidDiffuse");
		pMainElem->SetAttributeString("PhysicsMaterial","Default");
		pMainElem->SetAttributeString("DepthTest", "True");
		
		/////////////////////////
		// TextureUnits
		cXmlElement *pTexUnitsElem = pXmlDoc->CreateChildElement("TextureUnits");
		
		//Iterate the texture types
		for(int i=0; i<eMaterialTexture_LastEnum; ++i)
		{
			///////////////////////
			//Set up vars
			eMaterialTexture textureType = (eMaterialTexture)i;
			tString sElementName = gpEngine->GetResources()->GetMaterialManager()->GetTextureString(textureType);
			tWStringVec vExt = GetTextureExtensions(textureType);
			
			///////////////////////
			//Get the file name
			bool bFoundFile = false;
			tStringVec *apFileFormatsVec = gpEngine->GetResources()->GetBitmapLoaderHandler()->GetSupportedTypes();
			tWString sFilePath = _W("");
			for(tStringVecIt it = apFileFormatsVec->begin();it!=apFileFormatsVec->end();++it)
			{
				for(size_t j=0; j<vExt.size(); ++j)
				{
					sFilePath = cString::SetFileExtW(cString::To16Char(asName), _W(""));
					if(vExt[j] != _W("")) sFilePath += _W("_") + vExt[j];
					sFilePath =	cString::SetFilePathW(sFilePath, sDir);
					sFilePath = cString::SetFileExtW(sFilePath, cString::To16Char(*it));
					
					//Log("Test: '%s'\n",cString::To8Char(sFilePath).c_str());
					if(cPlatform::FileExists(sFilePath))
					{
						bFoundFile = true;
						break;
					}
				}
				if(bFoundFile) break;
			}
			if(bFoundFile==false)
			{
				Log("No file for '%s' texture type found\n",sElementName.c_str());
				continue;
			}

			///////////////////////
			//Create element
			cXmlElement *pTexElem = pTexUnitsElem->CreateChildElement(sElementName);
			pTexElem->SetAttributeString("File", cString::To8Char(cString::GetFileNameW(sFilePath)));
			if(textureType==eMaterialTexture_CubeMap)
				pTexElem->SetAttributeString("Type","cube");
			else
				pTexElem->SetAttributeString("Type","2D");
			pTexElem->SetAttributeString("Mipmaps","true");
			pTexElem->SetAttributeString("Wrap","Repeat");
			pTexElem->SetAttributeString("AnimMode","");
			pTexElem->SetAttributeString("AnimFrameTime","");
		}

		/////////////////////////
		// Variables
		cXmlElement *pSpecificVariablesElem = pXmlDoc->CreateChildElement("SpecificVariables");
		iMaterialType *pMatType = gpEngine->GetGraphics()->GetMaterialType("SolidDiffuse");
		if(pMatType)
		{
			for(int i=0; i<pMatType->GetUserVariableNum(); ++i)
			{
				cMaterialUserVariable* pUserVar = pMatType->GetUserVariable(i);
				cXmlElement *pVarElem = pSpecificVariablesElem->CreateChildElement("Var");
				
				pVarElem->SetAttributeString("Name", pUserVar->msName);
				pVarElem->SetAttributeString("Value", pUserVar->msValue);
			}
		}
		

		/////////////////////////
		// Save
		pXmlDoc->SaveToFile(sMatPath);
		hplDelete(pXmlDoc);

		gpEngine->GetResources()->AddResourceDir(sDir, false, "*.*");
	}
	
	//--------------------------------------------------------------
	
	bool DirectoryHasImageFile(const tWString& asPath, const tString& asMatName)
	{
		tWString sFileName = cString::To16Char(cString::SetFilePath(cString::SetFileExt(asMatName,""),""));
		tWString sFilePath = cString::SetFilePathW(sFileName, asPath);

		Log(" Searching for textures in dir: '%s'\n", cString::To8Char(asPath).c_str());
		tStringVec *apFileFormatsVec = gpEngine->GetResources()->GetBitmapLoaderHandler()->GetSupportedTypes();
		for(tStringVecIt it = apFileFormatsVec->begin();it!=apFileFormatsVec->end();++it)
		{
			tWString sNewName = cString::SetFileExtW(sFilePath,cString::To16Char(*it));
			
			if(cPlatform::FileExists(sNewName))
			{
				Log("  Found texture '%s'\n", cString::To8Char(sNewName).c_str());
				return true;
			}
		}
		
		return false;
	}

	//--------------------------------------------------------------
	
	void LoadMaterialData(cMesh *pMesh)
	{
		mvMaterialData.clear();

		tWString sMeshPath = cString::GetFilePathW(pMesh->GetFullPath());
		for(int i=0; i< pMesh->GetSubMeshNum(); ++i)
		{
			cSubMesh *pSubMesh = pMesh->GetSubMesh(i);
			cMaterial *pMat = pSubMesh->GetMaterial();
			tString sMatName = pSubMesh->GetMaterialName();

			Log("Loading material data for '%s'\n", pSubMesh->GetName().c_str());

			///////////////////////
			//If a material is found, then make sure that it is valid. Meaning:
			// - It is in the same directory has the mesh
			// - It is NOT in the same directory as much AND there is no image file in the directory either.
            if(pMat)
			{
				tWString sMathPath = cString::GetFilePathW(pMat->GetFullPath());
				if(sMathPath != sMeshPath && DirectoryHasImageFile(sMeshPath, sMatName))
				{
					Log(" Material path: '%s' and mesh path '%s' does not match! Valid textures also found in mesh path!\n",
						cString::To8Char(sMathPath).c_str(), cString::To8Char(sMeshPath).c_str());
					pSubMesh->SetMaterial(NULL);
					pMat = NULL;
				}
			}

			//////////////////////////
			//Check if some material data is already loaded
			cMaterialData *pExistingMatData = GetMaterialData(sMatName, false);
			if(pExistingMatData)
			{
				Log(" Already found material data!\n");
				if(pMat==NULL) 
				{
					pExistingMatData->mpMat->IncUserCount();
					pSubMesh->SetMaterial(pExistingMatData->mpMat);
				}
				continue;
			}
			
			mvMaterialData.push_back(cMaterialData());
			cMaterialData *pMatData = &mvMaterialData[mvMaterialData.size()-1];
			pMatData->msName =  cString::GetFileName(cString::SetFileExt(sMatName,""));

			//Log(" Create Mat data: '%s'\n", pMatData->msName.c_str());

			if(pMat)
			{
				Log(" Using existing material!\n");
				//CreateDefaultMaterial(pMesh, sMatName+"_temp");

				pMatData->mpMat = pMat;
				for(int j=0; j<eMaterialTexture_LastEnum; ++j)
				{
					pMatData->mvMaterialTextures[j] = pMat->GetTexture((eMaterialTexture)j);
				}
			}
			else
			{
				Log(" Create default material!\n");
				CreateDefaultMaterial(pMesh, sMatName);

				tString sMeshPath = cString::GetFilePath(cString::To8Char(pMesh->GetFullPath()));
				tString sMatPath = cString::SetFilePath(sMatName, sMeshPath);
				
				pMat = gpEngine->GetResources()->GetMaterialManager()->CreateMaterial(sMatPath);
				if(pMat==NULL) FatalError("Created material '%s' could not be loaded!\n",sMatPath.c_str());

				pMatData->mpMat = pMat;
				pSubMesh->SetMaterial(pMat);
				
				for(int j=0; j<eMaterialTexture_LastEnum; ++j)
				{
					pMatData->mvMaterialTextures[j] = pMat->GetTexture((eMaterialTexture)j);
				}
				
			}	
		}
	}

	//--------------------------------------------------------------

	void LoadModel(const tString& asFileName)
	{
		//////////////////////////////////////
		// Destroy stuff
		if(gpEntity != NULL)
		{
			ResetEntitySubMeshes();
			mpWorld->DestroyMeshEntity(gpEntity);

			for(size_t i=0; i<gvBodies.size(); ++i)
			{
				mpPhysicsWorld->DestroyBody(gvBodies[i]);
			}
			
			for(size_t i=0; i<gvSoundEntities.size(); ++i)
				mpWorld->DestroySoundEntity(gvSoundEntities[i]);
			
			for(size_t i=0; i<gvParticleSystems.size(); ++i)
				mpWorld->DestroyParticleSystem(gvParticleSystems[i]);
			
			for(size_t i=0; i<gvBillboards.size(); ++i)
				mpWorld->DestroyBillboard(gvBillboards[i]);

			for(size_t i=0; i<gvModelLights.size(); ++i)
				mpWorld->DestroyLight(gvModelLights[i]);

			gvJoints.clear();
			gvBodies.clear();
			gvBodyStartMatrix.clear();
			gvSoundEntities.clear();
			gvParticleSystems.clear();
			gvBillboards.clear();
			gvModelLights.clear();


			for(size_t i=0; i<mvMaterialData.size(); ++i)
			{
				cMaterialData& matData =mvMaterialData[i];
				//Log("UserCount%d: %d\n", i, matData.mpMat->GetUserCount());
			}
			mvMaterialData.clear();
		}
		gpEntity = NULL;
		cMesh *pMesh = NULL;

		tString sExt = cString::ToLowerCase(cString::GetFileExt(asFileName));

		///////////////////
		// Dae files
		if(sExt == "dae" || sExt == "dae_anim" || sExt == "msh" || sExt == "fbx")
		{
			pMesh = gpEngine->GetResources()->GetMeshManager()->CreateMesh(asFileName);
			if(pMesh==NULL) FatalError("Could not load '%s'\n", asFileName.c_str());
			gpEntity = mpWorld->CreateMeshEntity("Model",pMesh,false);
			gpEntity->SetRenderFlagBit(eRenderableFlag_ShadowCaster, true);
			
			cMatrixf mtxTransform = cMatrixf::Identity;
			//mtxTransform = cMath::MatrixScale(0.848f);
			//mtxTransform = cMath::MatrixRotate(cVector3f(0,kPi2f,0),eEulerRotationOrder_XYZ);
			//mtxTransform.SetTranslation(cVector3f(0,1,0));

			gpEntity->SetMatrix(mtxTransform);

			CreatePhysicaModelFromMesh(pMesh, gpEntity,gpEntity->GetWorldMatrix());
		}
		///////////////////
		// Entity files
		else if(sExt == "ent")
		{
			//NOte: because of strange stuff made in viewer, scale does not work well with physics!
			cMatrixf mtxTransform = cMatrixf::Identity;
			//mtxTransform = cMath::MatrixScale(0.3f);
			//mtxTransform = cMath::MatrixRotate(cVector3f(0,kPi2f,0),eEulerRotationOrder_XYZ);
			//mtxTransform.SetTranslation(cVector3f(0,1,0));
			
			gpEntity = (cMeshEntity*)mpWorld->CreateEntity("Model",mtxTransform,asFileName,-1,true, cVector3f(1.0f) );
			if(gpEntity==NULL) FatalError("Could not load '%s'\n", asFileName.c_str());
			pMesh = gpEntity->GetMesh();
		}

		gpFloor->SetPosition(cVector3f(0,gpEntity->GetBoundingVolume()->GetMin().y - 0.5f ,0));
		gpFloorBody->SetPosition(gpFloor->GetWorldPosition());
		
		if(pMesh)
		{
			LoadMaterialData(pMesh);
		}

		glOnLoadAnimationState = 1;
		glCurrentAnimaiton = 0;

		if(glAnimationState==1 && gpEntity->GetAnimationStateNum() > 0)
		{
			gpEntity->Play(glCurrentAnimaiton,true,true);
		}
		
		///////////////////////////////
		// If skeleton and no animation turn on skeleton physics
		if(pMesh->GetSkeleton() && gpEntity->GetAnimationStateNum()>0)
		{
			gbPhysicsActive = false;
		}

		if(mpCBPhysicsActive)
			mpCBPhysicsActive->SetChecked(gbPhysicsActive);
		else
			SetPhysicsActive(gbPhysicsActive);

		mpLowLevelGraphics->SetWindowCaption("ModelView - "+cString::GetFileName(asFileName));

		Log("--------- MODEL INFO ------------------\n");

		//gpEntity->SetNormalizeAnimationWeights(false);

		//MESH data
		Log(" MESH\n");
		Log("  Mesh path: '%s'\n", cString::To8Char(pMesh->GetFullPath()).c_str());
		Log("  Submeshes: %d\n", pMesh->GetSubMeshNum());
		int lTotalPolygons =0;
		for(int i=0; i<pMesh->GetSubMeshNum(); ++i)
		{
			cSubMeshEntity *pSubEnt = gpEntity->GetSubMeshEntity(i);
			cSubMesh *pSubMesh = pMesh->GetSubMesh(i);
			int lPolygons = pSubMesh->GetVertexBuffer()->GetVertexNum()/3;
			
			Log("   Name: '%s' \n", pSubMesh->GetName().c_str());
			Log("   Material: '%s' \n", pSubMesh->GetMaterialName().c_str());
			Log("   Polycount: %d\n", lPolygons);
			Log("   -----\n");
			
			lTotalPolygons += lPolygons;
		}
		Log("  Total Polycount: %d\n", lTotalPolygons);
		Log("  Nodes: %d\n", pMesh->GetNodeNum());
		for(int i=0; i<pMesh->GetNodeNum(); ++i)
		{
			cNode3D *pNode = pMesh->GetNode(i);
			cNode3D *pParent = pNode->GetParent();
			Log("   Name: '%s' Parent: '%s'\n",pNode->GetName().c_str(), pParent ? pParent->GetName().c_str() : "None");
			Log("    Local Transform: %s\n", pNode->GetLocalMatrix().ToString().c_str());
		}
		
		//ANIMATION DATA
        if(pMesh->GetAnimationNum() > 0)
		{
			Log("\n");

			Log(" ANIMATION\n");
			for(int i=0; i<pMesh->GetAnimationNum(); ++i)
			{
				cAnimation *pAnim = pMesh->GetAnimation(i);
				
				Log("  Name: '%s'\n", pAnim->GetName().c_str());
				Log("  Tracks: %d\n", pAnim->GetTrackNum());
				Log("  Length: %f\n", pAnim->GetLength());

				for(int track =0; track < pAnim->GetTrackNum(); ++track)
				{
					cAnimationTrack *pTrack = pAnim->GetTrack(track);
					Log("   Name: '%s' Keyframes: %d\n",pTrack->GetName().c_str(),pTrack->GetKeyFrameNum());

					//if(	cString::GetFirstStringPos(pTrack->GetName(),"joint5") >= 0 ||
					//	cString::GetFirstStringPos(pTrack->GetName(),"joint6") >= 0)
					if(false)
					{
						pTrack->Smooth(1.0f/30.0f * 4.0f, 0.5f, 4, true, true);
						for(int frame =0; frame <pTrack->GetKeyFrameNum(); ++frame)
						{
							cKeyFrame *pFrame = pTrack->GetKeyFrame(frame);
							Log("    %f (%s) | (%f %f %f %f)\n", pFrame->time,pFrame->trans.ToString().c_str(),
												pFrame->rotation.v.x, pFrame->rotation.v.y, pFrame->rotation.v.z,
												pFrame->rotation.w);
						}
						Log("-------\n\n");
					}
					else if(pTrack->GetKeyFrameNum()>0)
					{
						/*cKeyFrame *pFirst = pTrack->GetKeyFrame(0);
						for(int frame =1; frame <pTrack->GetKeyFrameNum(); ++frame)
						{
							cKeyFrame *pFrame = pTrack->GetKeyFrame(frame);
							pFrame->rotation = pFirst->rotation;
							pFrame->trans = pFirst->trans;
						}*/
					}
				}
				Log(" -----\n");


				Log(" - ANIM DEBUG:\n");
				//ANIMATION DEBUG
				/*cAnimationTrack *pKnee1 = pAnim->GetTrackByName("servant_grunt_A_joint5");
				cAnimationTrack *pKnee2 = pAnim->GetTrackByName("servant_grunt_A_joint6");
				pKnee1->Smooth(0.3f, 1.0f, 10, true, true);
				pKnee2->Smooth(0.3f, 1.0f, 10, true, true);
				cQuaternion qAvgRatio(0,0,0,0);
				float fCount =0;
				for(int frame =0; frame <pKnee1->GetKeyFrameNum(); ++frame)
				{
					cKeyFrame *pKnee1Frame = pKnee1->GetKeyFrame(frame);
					cKeyFrame *pKnee2Frame = pKnee2->GetKeyFrame(frame);
					Log("    %f (%f %f) vs (%f %f) ratio: (%f %f)\n", pKnee1Frame->time,
						pKnee1Frame->rotation.v.z, pKnee1Frame->rotation.w,
						pKnee2Frame->rotation.v.z, pKnee2Frame->rotation.w,
						pKnee1Frame->rotation.v.z / pKnee2Frame->rotation.v.z, pKnee1Frame->rotation.w / pKnee2Frame->rotation.w
						);
					qAvgRatio.v.x = pKnee2Frame->rotation.v.x==0 ? 0.0f : (qAvgRatio.v.x + (pKnee1Frame->rotation.v.x / pKnee2Frame->rotation.v.x) );
					qAvgRatio.v.y = pKnee2Frame->rotation.v.y==0 ? 0.0f : (qAvgRatio.v.y + (pKnee1Frame->rotation.v.y / pKnee2Frame->rotation.v.y) );
					qAvgRatio.v.z = pKnee2Frame->rotation.v.z==0 ? 0.0f : (qAvgRatio.v.z + (pKnee1Frame->rotation.v.z / pKnee2Frame->rotation.v.z) );
					qAvgRatio.w = qAvgRatio.w + (pKnee1Frame->rotation.w / pKnee2Frame->rotation.w);
					fCount++;
               	}
				qAvgRatio = qAvgRatio * (1.0f / fCount);
				Log("Count: %f AvgRatio: (%f %f %f %f)\n", fCount, qAvgRatio.v.x, qAvgRatio.v.y, qAvgRatio.v.z, qAvgRatio.w);
				for(int frame =0; frame <pKnee1->GetKeyFrameNum(); ++frame)
				{
					cKeyFrame *pKnee1Frame = pKnee1->GetKeyFrame(frame);
					cKeyFrame *pKnee2Frame = pKnee2->GetKeyFrame(frame);

					pKnee1Frame->rotation.v = pKnee2Frame->rotation.v;// * qAvgRatio.v;
					pKnee1Frame->rotation.w = pKnee2Frame->rotation.w;// * qAvgRatio.w;
					//pKnee1Frame->rotation.Normalize();

					Log("    %f (%f %f) vs (%f %f) ratio: (%f %f)\n", pKnee1Frame->time,
						pKnee1Frame->rotation.v.z, pKnee1Frame->rotation.w,
						pKnee2Frame->rotation.v.z, pKnee2Frame->rotation.w,
						pKnee1Frame->rotation.v.z / pKnee2Frame->rotation.v.z, pKnee1Frame->rotation.w / pKnee2Frame->rotation.w
						);
				}*/
				


				Log(" -----\n");
			}

			
		}

		

		


		//SKELETON DATA
		if(pMesh->GetSkeleton())
		{
			Log("\n");

			Log(" SKELETON\n");
			cSkeleton *pSkeleton = pMesh->GetSkeleton();
			Log("  Bones: %d\n", pSkeleton->GetBoneNum());
			for(int bone = 0; bone != pSkeleton->GetBoneNum(); ++bone)
			{
				cBone *pBone = pSkeleton->GetBoneByIndex(bone);

				Log("   Name: '%s' Pos: (%s)\n", pBone->GetName().c_str(), pBone->GetLocalTransform().GetTranslation().ToString().c_str());
				//Log("   BindPos: %s\n", pBone->GetLocalTransform().ToString().c_str());
			
				//Log(" -----\n");
			}
		}
		


		Log("---------------------------------------\n");

	}
	
	//--------------------------------------------------------------

	void CreatePhysicaModelFromMesh(cMesh *apMesh, cMeshEntity *apEntity, const cMatrixf& a_mtxTransform)
	{
		gvBodyStartMatrix.clear();

		for(int sub=0; sub<apMesh->GetSubMeshNum(); ++sub)
		{
			cSubMesh *pSubMesh = apMesh->GetSubMesh(sub);
			cSubMeshEntity *pSubEnt =  apEntity->GetSubMeshEntity(sub);
			iPhysicsBody* pBody=NULL;

			///////////////////
			// Mesh Shape
			if(	cString::Sub(pSubMesh->GetName(),0,9)== "_collider" ||
				cString::Sub(pSubMesh->GetName(),0,13)== "_charcollider")
			{
				iCollideShape *pShape = mpPhysicsWorld->CreateMeshShape(pSubMesh->GetVertexBuffer());

				pBody = mpPhysicsWorld->CreateBody("Body",pShape);

				pBody->SetMatrix(cMath::MatrixMul(a_mtxTransform, pSubMesh->GetLocalTransform()));
			}
			//////////////////
			// Normal Shape
			else
			{
				iCollideShape *pShape = pSubMesh->CreateCollideShape(mpPhysicsWorld);
				
				if(pShape != NULL)
				{
					pBody = mpPhysicsWorld->CreateBody("Body",pShape);
					pBody->SetMatrix(a_mtxTransform);
					pBody->AddChild(pSubEnt);
				}
			}
			if(pBody==NULL) continue;

			pBody->SetMass(0);
			
			gvBodies.push_back(pBody);

			gvBodyStartMatrix.push_back(pBody->GetWorldMatrix());
		}
	}

	//--------------------------------------------------------------

	void InitGuiAfterLoad()
	{	
		if(gpEntity==NULL) return;
	}

	//--------------------------------------------------------------

	void ResetEntitySubMeshes()
	{
		if(gpEntity == NULL) return;
		
		cMesh *pMesh = gpEntity->GetMesh();
		for(int i=0; i< pMesh->GetSubMeshNum(); ++i)
		{
			cSubMesh *pSubMesh = pMesh->GetSubMesh(i);
			cMaterial *pMat = pSubMesh->GetMaterial();

			cMaterialData *pMatData = GetMaterialData(pSubMesh->GetMaterialName(), true);
			if(pMatData == NULL) continue;

			for(int j=0; j<eMaterialTexture_LastEnum; ++j)
			{
				pMat->SetTexture((eMaterialTexture)j, pMatData->mvMaterialTextures[j]);				
			}
		}
	}
	
	//--------------------------------------------------------------

	void SetupMaterialTextureCheckBoxes()
	{
		if(gpEntity == NULL) return;

		cMesh *pMesh = gpEntity->GetMesh();

		//Reset all check boxes
		for(int j=0; j<eMaterialTexture_LastEnum; ++j)
		{
			mpCBMaterialTexture[j]->SetEnabled(false);
			mpCBMaterialTexture[j]->SetChecked(false);
		}

		for(int i=0; i< pMesh->GetSubMeshNum(); ++i)
		{
			cSubMesh *pSubMesh = pMesh->GetSubMesh(i);
			cMaterial *pMat = pSubMesh->GetMaterial();

			cMaterialData *pMatData = GetMaterialData(pSubMesh->GetMaterialName(), true);
			if(pMatData == NULL)continue;
			
			for(int j=0; j<eMaterialTexture_LastEnum; ++j)
			{
				//If any sub mesh has this texture, enable the check button.
				if(pMatData->mvMaterialTextures[j]) 
				{
					mpCBMaterialTexture[j]->SetEnabled(true);
					mpCBMaterialTexture[j]->SetChecked(true);
				}
			}
		}
	}

	//--------------------------------------------------------------

	void SetupAnimationComboBox()
	{
		if(gpEntity==NULL) return;
		mpCBAnimations->ClearItems();

		for(int i=0; i<gpEntity->GetAnimationStateNum(); ++i)
		{
			cAnimationState *pAnimation = gpEntity->GetAnimationState(i);
            
			mpCBAnimations->AddItem(cString::To16Char(pAnimation->GetName()).c_str());
		}

		if(gpEntity->GetAnimationStateNum() > 0)
			glCurrentAnimaiton = 0;
		else
			glCurrentAnimaiton = -1;
		mpCBAnimations->SetSelectedItem(glCurrentAnimaiton);
	}

	//--------------------------------------------------------------

	void SetRagdollActive(bool abX)
	{
		if(gpEntity==NULL) return;
		if(gpEntity->GetMesh()->GetSkeleton()==NULL) return;

		gbRagdollActive = abX;

		gpEntity->SetSkeletonPhysicsActive(gbRagdollActive);

		if(gbRagdollActive)
		{
			gpEntity->Stop();
			gpEntity->AlignBodiesToSkeleton(false);
		}
		else
		{
			if(gpEntity->GetAnimationStateNum() >0 && glCurrentAnimaiton>=0)
			{
				gpEntity->GetAnimationState(glCurrentAnimaiton)->SetActive(true);
			}
		}
	}

	//--------------------------------------------------------------

	void SetPhysicsActive(bool abX)
	{
		gbPhysicsActive = abX;

		if(gvBodies.empty()) return;

		SetRagdollActive(gbPhysicsActive);
		
		if(gbPhysicsActive)
		{
			for(size_t i=0; i<gvBodies.size(); ++i)
			{
				iPhysicsBody *pBody = gvBodies[i];
				pBody->Enable();
				pBody->SetActive(true);
			}
		}
		else
		{
			for(size_t i=0; i<gvBodies.size(); ++i)
			{
				iPhysicsBody *pBody = gvBodies[i];
				pBody->Enable();
				pBody->SetActive(false);
				pBody->SetLinearVelocity(0);
				pBody->SetAngularVelocity(0);
				//pBody->SetMatrix(gvBodyStartMatrix[i]);
			}
		}
		
		if(gpEntity) gpEntity->SetMatrix(cMatrixf::Identity);
		mfModelRotation =0;
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
		cVector2f vSize = cVector2f(290, 720);
		vGroupSize.x = vSize.x - 20;
		cVector3f vPos = cVector3f(pSet->GetVirtualSize().x - vSize.x - 3, 3, 0);
		mpOptionWindow = pSet->CreateWidgetWindow(0,vPos,vSize,_W("ModelView Toolbar") );
		
		///////////////////////////
		//Frame
		cWidgetFrame *pRootFrame = pSet->CreateWidgetFrame(cVector3f(5, 30, 0.1f), cVector2f(vSize.x-10, vSize.y-40), true, mpOptionWindow,false,true);

		vSize = cVector2f(vSize.x-50, 18);
		vPos = cVector3f(5, 5, 0.1f);

		///////////////////////////
		// Loading
		{
			//Group
			vGroupPos = cVector3f(5,10,0.1f);
			pGroup = pSet->CreateWidgetGroup(vPos,100,_W("Loading"),pRootFrame);

			//Load File
			pButton = pSet->CreateWidgetButton(vGroupPos,vSize,_W("Load Model"),pGroup);
			pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressLoadModel));
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
			pGroup = pSet->CreateWidgetGroup(vPos,100,_W("Environment"),pRootFrame);

			//Grid and Axes
			cVector2f vCheckSize = vSize; vCheckSize.x /= 2;
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos,vCheckSize,_W("Draw grid"),pGroup);
			pCheckBox->SetChecked(gbDrawGrid);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeDrawGrid));
			
			cVector3f vAddPos(vCheckSize.x, 0,0);
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos + vAddPos,vCheckSize,_W("Draw axes"),pGroup);
			pCheckBox->SetChecked(gbDrawAxes);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeDrawAxes));

			vGroupPos.y += 22;

			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos,vCheckSize,_W("Draw floor"),pGroup);
			pCheckBox->SetChecked(gpFloor->IsVisible());
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeDrawFloor));
			
			vGroupPos.y += 22;

			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos,vCheckSize,_W("Show bad vertices"),pGroup);
			pCheckBox->SetChecked(gbShowInvalidVertices);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeShowInvalidVertices));

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
		// Model
		{
			//Group
			vGroupPos = cVector3f(5,10,0.1f);
			pGroup = pSet->CreateWidgetGroup(vPos,100,_W("Model"),pRootFrame);

			//Rotate and show AABB
			cVector2f vCheckSize = vSize; vCheckSize.x /= 2;
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos,vCheckSize,_W("Rotate"),pGroup);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeRotateModel));
			mpCBRotateModel = pCheckBox;

			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos+cVector3f(vCheckSize.x,0,0) ,vCheckSize,_W("Show AABB"),pGroup);
			pCheckBox->SetChecked(gbDrawBoundingBox);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeShowAABB));


			//cVector3f vAddPos(vCheckSize.x, 0,0);
			//pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos + vAddPos,vCheckSize,_W("Draw axes"),pGroup);
			//pCheckBox->SetChecked(gbDrawAxes);
			//pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeDrawAxes));

			vGroupPos.y += 22;

			//Material Textures
			tWString sTextureName[] = {
				_W("Diffuse"),
				_W("NMap"),
				_W("Specular"),
				_W("Alpha"),
				_W("Height"),
				_W("Illumination"),
				_W("CubeMap"),
				_W("Refraction"),
				_W("DissolveAlpha"),
				_W("")
			};
			pLabel = pSet->CreateWidgetLabel(vGroupPos,vSize,_W("Material Textures:"),pGroup);
			vGroupPos.y += 20;
			cVector2f vMatSize = cVector2f(vSize.x/2 - 2, vSize.y);
			int lAddCount =0;
			for(int i=0; i<eMaterialTexture_LastEnum; ++i)
			{
				float fAdd = lAddCount % 2==1 ? vMatSize.x+4 : 0;
				pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos+cVector3f(fAdd+5,0,0),vMatSize,sTextureName[i],pGroup);
				pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeMaterialTexture));
				pCheckBox->SetUserValue(i);
				pCheckBox->SetEnabled(false);
				mpCBMaterialTexture[i] = pCheckBox;
				if(lAddCount % 2==1 || i==eMaterialTexture_LastEnum-1)vGroupPos.y += 20;
				lAddCount++;
			}
			vGroupPos.y += 2;

			//Group end
			vGroupSize.y = vGroupPos.y + 5;
			pGroup->SetSize(vGroupSize);
			vPos.y += vGroupSize.y + 15;

			SetupMaterialTextureCheckBoxes();
		}

		//////////////////////////
		//Animation
		{
			//Group
			vGroupPos = cVector3f(5,10,0.1f);
			pGroup = pSet->CreateWidgetGroup(vPos,100,_W("Animation"),pRootFrame);

			//Skeleton and add animation
			cVector2f vCheckSize = vSize; vCheckSize.x /= 2;
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos+cVector3f(vCheckSize.x*0, 0,0), vCheckSize,_W("Show Skeleton"),pGroup);
			pCheckBox->SetChecked(gbDrawSkeleton);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeDrawSkeleton));

			pButton = pSet->CreateWidgetButton(vGroupPos+cVector3f(vCheckSize.x, 0,0), vCheckSize,_W("Add anim"),pGroup);
			pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressAddAnimation));

			vGroupPos.y += 22;

			//Current animation label
			pLabel = pSet->CreateWidgetLabel(vGroupPos,vSize,_W("Current Animation:"),pGroup);
			vGroupPos.y += 18;

			//Current animation combo box
			pComboBox = pSet->CreateWidgetComboBox(vGroupPos,vSize,_W(""),pGroup);
			pComboBox->AddCallback(eGuiMessage_SelectionChange,this, kGuiCallback(ChangeAnimation));
			pComboBox->SetSelectedItem(-1);
			mpCBAnimations = pComboBox;

			vGroupPos.y += 26;

			//Stop, pause and play
			cVector2f vButtonSize = vSize; vButtonSize.x /= 3;
			pButton = pSet->CreateWidgetButton(vGroupPos+cVector3f(vButtonSize.x*0, 0,0), vButtonSize,_W("Stop"),pGroup);
			pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressStopButton));

			pButton = pSet->CreateWidgetButton(vGroupPos+cVector3f(vButtonSize.x*1, 0,0), vButtonSize,_W("Pause"),pGroup);
			pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressPauseButton));

			pButton = pSet->CreateWidgetButton(vGroupPos+cVector3f(vButtonSize.x*2, 0,0), vButtonSize,_W("Play"),pGroup);
			pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressPlayButton));

			vGroupPos.y += 22;


			//Group end
			vGroupSize.y = vGroupPos.y + 5;
			pGroup->SetSize(vGroupSize);
			vPos.y += vGroupSize.y + 15;

			SetupAnimationComboBox();
		}

		//////////////////////////
		//Physics
		{
			//Group
			vGroupPos = cVector3f(5,10,0.1f);
			pGroup = pSet->CreateWidgetGroup(vPos,100,_W("Physics"),pRootFrame);

			//Active and Debug
			cVector2f vCheckSize = vSize; vCheckSize.x /= 2;
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos,vCheckSize,_W("Active"),pGroup);
			pCheckBox->SetChecked(gbPhysicsActive);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangePhysicsActive));
			mpCBPhysicsActive = pCheckBox;

			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos+cVector3f(vCheckSize.x, 0,0), vCheckSize,_W("Draw Debug"),pGroup);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeDrawDebug));
			vGroupPos.y += 22;

			//Accuracy
			cVector2f vASize = vSize;
			vASize.x = vASize.x*0.5f;
			pLabel = pSet->CreateWidgetLabel(vGroupPos,vASize,_W("Accuracy:"),pGroup);
			pComboBox = pSet->CreateWidgetComboBox(vGroupPos+cVector3f(vASize.x,0,0),vASize,_W(""),pGroup);
			pComboBox->AddItem(_W("Low"));
			pComboBox->AddItem(_W("Medium"));
			pComboBox->AddItem(_W("High"));
			pComboBox->SetSelectedItem(mpPhysicsWorld->GetAccuracyLevel());
			pComboBox->AddCallback(eGuiMessage_SelectionChange,this, kGuiCallback(ChangeAccuracy));
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
			pGroup = pSet->CreateWidgetGroup(vPos,100,_W("Lights"),pRootFrame);

			//Ambient
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos,vSize,_W("Ambient lighting"),pGroup);
			pCheckBox->SetChecked(gbAmbientLight);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeAmbientLight));
			vGroupPos.y += 20;

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

			pSlider = pSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal,vAmbSlidePos,vAmbSlideSize,255,pGroup);
			pSlider->AddCallback(eGuiMessage_SliderMove,this, kGuiCallback(ChangeAmbientColor));
			pSlider->SetValue( (int)(gpLightBox->GetDiffuseColor().r * 255.0 + 0.5f));
			pLabel = pSet->CreateWidgetLabel(vGroupPos, vSize, _W("Intensity:"), pGroup);
			vGroupPos.y += 22;
			
			vGroupPos.y += 5;

			//Light Active
			cVector2f vASize = vSize;
			vASize.x = vASize.x*0.5f;
			pLabel = pSet->CreateWidgetLabel(vGroupPos,vASize,_W("Light:"),pGroup);
			pComboBox = pSet->CreateWidgetComboBox(vGroupPos+cVector3f(vASize.x,0,0),vASize,_W(""),pGroup);
			pComboBox->AddItem(_W("Key"));
			pComboBox->AddItem(_W("Fill"));
			pComboBox->AddItem(_W("Back"));
			pComboBox->AddCallback(eGuiMessage_SelectionChange,this, kGuiCallback(ChangeCurrentLight));
			vGroupPos.y += 22;

			//Visible
			cVector2f vCheckSize = vSize; vCheckSize.x /= 2;
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos,vCheckSize,_W("Visible"),pGroup);
			pCheckBox->SetChecked(gbDrawGrid);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeLightVisible));
			mpCBLightVisible = pCheckBox;
			
			//Shadows
			cVector3f vAddPos(vCheckSize.x, 0,0);
			pCheckBox = pSet->CreateWidgetCheckBox(vGroupPos + vAddPos,vCheckSize,_W("Shadows"),pGroup);
			pCheckBox->SetChecked(gbDrawAxes);
			pCheckBox->AddCallback(eGuiMessage_CheckChange,this, kGuiCallback(ChangeLightShadow));
			mpCBLightCastShadows = pCheckBox;

			vGroupPos.y += 22;

			//Color sliders
			cVector2f vSlideSize = vSize;
			vSlideSize.x -= 40;
			tWString sColorNames[] = {_W("Red:"),_W("Green:"),_W("Blue:"),_W("Spec:")};
			for(int i=0; i<4; ++i)
			{
				cVector3f vSlidePos = vGroupPos;
				vSlidePos.x += 40;
				pSlider = pSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal,vSlidePos,vSlideSize,255,pGroup);
				pSlider->AddCallback(eGuiMessage_SliderMove,this, kGuiCallback(ChangeLightColor));
				pSlider->SetUserValue(i);
				mpColorSlider[i] = pSlider;

				pLabel = pSet->CreateWidgetLabel(vGroupPos, vASize, sColorNames[i], pGroup);

				vGroupPos.y += vSlideSize.y+2;
			}

			pComboBox->SetSelectedItem(0);
			
			
					
			//Group end
			vGroupSize.y = vGroupPos.y + 5;
			pGroup->SetSize(vGroupSize);
			vPos.y += vGroupSize.y + 15;
		}
				
		
		InitGuiAfterLoad();
	}
	
	//--------------------------------------------------------------
	
	bool PressLoadModel(iWidget* apWidget,const cGuiMessageData& aData)
	{
		mvPickedFiles.clear();
		cGuiSet *pSet = gpSimpleCamera->GetSet();
		cGuiPopUpFilePicker* pPicker = pSet->CreatePopUpLoadFilePicker(mvPickedFiles,false,msCurrentFilePath,false, this, kGuiCallback(LoadModelFromFilePicker));
		int lCat = pPicker->AddCategory(_W("Models"),_W("*.dae"));
		pPicker->AddFilter(lCat,_W("*.ent"));
		pPicker->AddFilter(lCat,_W("*.fbx"));

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,PressLoadModel)  

	bool PressReload(iWidget* apWidget,const cGuiMessageData& aData)
	{
		if(gpEntity==NULL) return true;

		LoadModel(gsModelFile);
		SetupMaterialTextureCheckBoxes();
		SetupAnimationComboBox();
		InitGuiAfterLoad();

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,PressReload)  

	
	bool LoadModelFromFilePicker(iWidget* apWidget,const cGuiMessageData& aData)
	{
		if(mvPickedFiles.empty()) return true;

		tWString& sFilePath = mvPickedFiles[0];

		msCurrentFilePath = cString::GetFilePathW(sFilePath);
		gpEngine->GetResources()->AddResourceDir(msCurrentFilePath,false);
		
		gsModelFile = cString::To8Char(sFilePath);
		LoadModel(gsModelFile);  
		SetupMaterialTextureCheckBoxes();
		SetupAnimationComboBox();
		InitGuiAfterLoad();

		return true;		
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,LoadModelFromFilePicker)  


	//--------------------------------------------------------------
	
	bool ChangeDrawGrid(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gbDrawGrid = aData.mlVal == 1;
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeDrawGrid)  
	
	bool ChangeDrawAxes(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gbDrawAxes = aData.mlVal == 1;
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeDrawAxes)  

	bool ChangeDrawFloor(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gpFloor->SetVisible(aData.mlVal == 1);
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeDrawFloor)  

	bool ChangeShowInvalidVertices(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gbShowInvalidVertices = aData.mlVal == 1;
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeShowInvalidVertices)
	
	bool ChangeBackgroundColor(iWidget* apWidget,const cGuiMessageData& aData)
	{
		cColor gvBGColors[] = {cColor(0,1),cColor(0.5f,1),cColor(1,1) };

		mpWorld->SetSkyBoxColor(gvBGColors[aData.mlVal]);
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeBackgroundColor)

	bool ChangeAmbientLight(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gbAmbientLight = aData.mlVal == 1;
		gpLightBox->SetVisible(gbAmbientLight);
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeAmbientLight)  
	
	bool ChangeAmbientColor(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gpLightBox->SetDiffuseColor(cColor( ((float)aData.mlVal) / 255.0f, 1.0f ));
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeAmbientColor)  

	bool ChangeSSAOActive(iWidget* apWidget,const cGuiMessageData& aData)
	{
		cRenderSettings *pSettings = gpSimpleCamera->GetViewport()->GetRenderSettings();

		pSettings->mbSSAOActive = aData.mlVal == 1;

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeSSAOActive)

		
	bool ChangeBloomActive(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gpPostEffectComp->GetPostEffect(0)->SetActive(aData.mlVal == 1);

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeBloomActive)
	//--------------------------------------------------------------

	bool ChangeRotateModel(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gbRotateModel = aData.mlVal == 1;

		if(gbRotateModel) mpCBPhysicsActive->SetChecked(false);

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeRotateModel) 

	bool ChangeShowAABB(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gbDrawBoundingBox = aData.mlVal == 1;

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeShowAABB) 	

	
	bool ChangeMaterialTexture(iWidget* apWidget,const cGuiMessageData& aData)
	{
		if(gpEntity==NULL) return true;
		
		eMaterialTexture texType = (eMaterialTexture)apWidget->GetUserValue();
		
		cMesh *pMesh = gpEntity->GetMesh();
		for(int i=0; i< pMesh->GetSubMeshNum(); ++i)
		{
			cSubMesh *pSubMesh = pMesh->GetSubMesh(i);
			cMaterial *pMat = pSubMesh->GetMaterial();
			
			cMaterialData *pMatData = GetMaterialData(pSubMesh->GetMaterialName(), true);
			if(pMatData == NULL) continue;
			
			if(aData.mlVal == 1)
			{
				pMat->SetTexture(texType, pMatData->mvMaterialTextures[texType]);
			}
			else
			{
				if(texType == eMaterialTexture_Diffuse)		pMat->SetTexture(texType,mpTexDiffuseNull);
				else if(texType == eMaterialTexture_NMap)	pMat->SetTexture(texType,mpTexNMapNull);
				else										pMat->SetTexture(texType,NULL);
			}
			
			pMat->Compile();
		}

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeMaterialTexture) 
		


	//--------------------------------------------------------------

	void SetCurrentLight(int alIdx)
	{
		mlCurrentLight = alIdx;
		cLightSpot *pLight = gvLights[mlCurrentLight];

        mpCBLightCastShadows->SetChecked(pLight->GetCastShadows());
		mpCBLightVisible->SetChecked(pLight->IsVisible());

		cColor col = pLight->GetDiffuseColor();

		for(int i=0; i<4; ++i)
		{
			mpColorSlider[i]->SetValue( (int)(col.v[i]*255.0f + 0.5f) );
		}
	}

	bool ChangeCurrentLight(iWidget* apWidget,const cGuiMessageData& aData)
	{
		SetCurrentLight(aData.mlVal);
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeCurrentLight)
	
	bool ChangeLightVisible(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gvLights[mlCurrentLight]->SetVisible(aData.mlVal == 1);
		return true;
	}
	
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeLightVisible)
	
	bool ChangeLightShadow(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gvLights[mlCurrentLight]->SetCastShadows(aData.mlVal == 1);
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeLightShadow)
	
	bool ChangeLightColor(iWidget* apWidget,const cGuiMessageData& aData)
	{
		cColor aCol = gvLights[mlCurrentLight]->GetDiffuseColor();
		int lIdx = apWidget->GetUserValue();

		float fVal = ((float)aData.mlVal) / 255.0f;
		aCol.v[lIdx] = fVal;

		gvLights[mlCurrentLight]->SetDiffuseColor(aCol);
			
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeLightColor)
	

	//--------------------------------------------------------------
	
	bool ChangeDrawDebug(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gbDrawPhysicsDebug = aData.mlVal == 1;
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeDrawDebug)  

		
	bool ChangePhysicsActive(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gbPhysicsActive = aData.mlVal == 1;
		SetPhysicsActive(gbPhysicsActive);
		
		if(gbPhysicsActive) mpCBRotateModel->SetChecked(false);

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangePhysicsActive)  

	bool ChangeAccuracy(iWidget* apWidget,const cGuiMessageData& aData)
	{
		mpPhysicsWorld->SetAccuracyLevel((ePhysicsAccuracy)aData.mlVal);
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeAccuracy)

	//--------------------------------------------------------------
	
	bool ChangeDrawSkeleton(iWidget* apWidget,const cGuiMessageData& aData)
	{
		gbDrawSkeleton = aData.mlVal == 1;

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeDrawSkeleton)  

		
	bool PressAddAnimation(iWidget* apWidget,const cGuiMessageData& aData)
	{
		if(gpEntity == NULL) return true;

		mvPickedFiles.clear();
		cGuiSet *pSet = gpSimpleCamera->GetSet();
		cGuiPopUpFilePicker* pPicker = pSet->CreatePopUpLoadFilePicker(mvPickedFiles,false,msCurrentFilePath,false, this, kGuiCallback(LoadAnimationFromFilePicker));
		int lCat = pPicker->AddCategory(_W("Animation"),_W("*.dae"));
		pPicker->AddFilter(lCat,_W("*.dae_anim"));
		pPicker->AddFilter(lCat,_W("*.fbx"));

		
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,PressAddAnimation)  

	bool LoadAnimationFromFilePicker(iWidget* apWidget,const cGuiMessageData& aData)
	{
		if(mvPickedFiles.empty()) return true;

		tWString& sFilePath = mvPickedFiles[0];

		msCurrentFilePath = cString::GetFilePathW(sFilePath);
		gpEngine->GetResources()->AddResourceDir(msCurrentFilePath,false);

		tString sAnimFile = cString::To8Char(cString::GetFileNameW(sFilePath));
		
		cAnimation *pAnim = gpEngine->GetResources()->GetAnimationManager()->CreateAnimation(sAnimFile);
		if(pAnim == NULL)
		{
			cPlatform::CreateMessageBox(_W("Error"),_W("Could not load animation '%ls'"), sFilePath.c_str());
			return true;
		}

		gpEntity->AddAnimation(pAnim,sAnimFile,1.0f);

		int lAnimNum = gpEntity->GetAnimationStateNum();
		gpEntity->GetAnimationState(lAnimNum-1)->SetLoop(true);

		SetupAnimationComboBox();
		
		return true;		
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,LoadAnimationFromFilePicker)  
	
	bool ChangeAnimation(iWidget* apWidget,const cGuiMessageData& aData)
	{
		if(gpEntity == NULL) return true;
		if(gpEntity->GetAnimationStateNum() <=0) return true;
		
		glCurrentAnimaiton = aData.mlVal;
		
		if(glAnimationState != 0 && glCurrentAnimaiton > 0 && glCurrentAnimaiton < gpEntity->GetAnimationStateNum())
		{
			for(int i=0; i<gpEntity->GetAnimationStateNum(); ++i)
			{
				if(glAnimationState == 1)
					gpEntity->GetAnimationState(i)->SetSpeed(gpEntity->GetAnimationState(i)->GetBaseSpeed());
				else
					gpEntity->GetAnimationState(i)->SetSpeed(0);
				
				gpEntity->GetAnimationState(i)->SetActive(false);
			}
			
			gpEntity->GetAnimationState(glCurrentAnimaiton)->SetActive(true);
		}

		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,ChangeAnimation)

	
	bool PressStopButton(iWidget* apWidget,const cGuiMessageData& aData)
	{
		if(gpEntity == NULL) return true;
		if(gpEntity->GetAnimationStateNum() <=0) return true;

		glAnimationState =0;
		gpEntity->Stop();
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,PressStopButton)  

	bool PressPauseButton(iWidget* apWidget,const cGuiMessageData& aData)
	{
		if(gpEntity == NULL) return true;
		if(gpEntity->GetAnimationStateNum() <=0) return true;

		glAnimationState =2;
		if(gpEntity->GetAnimationStateNum() <= 0) return true;
		
		for(int i=0; i<gpEntity->GetAnimationStateNum(); ++i)
			gpEntity->GetAnimationState(i)->SetSpeed(0);
		
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,PressPauseButton)  

	bool PressPlayButton(iWidget* apWidget,const cGuiMessageData& aData)
	{
		if(gpEntity == NULL) return true;
		if(gpEntity->GetAnimationStateNum() <=0) return true;

		for(int i=0; i<gpEntity->GetAnimationStateNum(); ++i)
		{
			gpEntity->GetAnimationState(i)->SetSpeed(gpEntity->GetAnimationState(i)->GetBaseSpeed());
			gpEntity->GetAnimationState(i)->SetActive(false);
			gpEntity->GetAnimationState(i)->SetLoop(true);
		}

		gpEntity->GetAnimationState(glCurrentAnimaiton)->SetActive(true);

		glAnimationState =1;
		return true;
	}
	kGuiCallbackFuncEnd(cSimpleUpdate,PressPlayButton)  

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

			//////////////////////////////
			// Draw sounds playing
			if(gbDrawSoundsPlaying)
			{
				tStringVec vSoundNames;
				std::vector<cSoundEntry*> vEntries;

				cSoundHandler *pSoundHandler = gpEngine->GetSound()->GetSoundHandler();
				cMusicHandler *pMusicHandler = gpEngine->GetSound()->GetMusicHandler();

				//////////////////////////////
				//Sounds

				//Get names and entrys
				tSoundEntryList *pEntryList = pSoundHandler->GetEntryList();
				for(tSoundEntryListIt it = pEntryList->begin(); it != pEntryList->end();++it)
				{
					cSoundEntry *pEntry = *it;
					iSoundChannel *pSound = pEntry->GetChannel();
					vSoundNames.push_back(pSound->GetData()->GetName());
					vEntries.push_back(pEntry);
				}

				//Draw number of sounds
				gpSimpleCamera->GetSet()->DrawFont(gpSimpleCamera->GetFont(), cVector3f(5,fY,0),14,cColor(1,1),_W("Num of sounds: %d"),vSoundNames.size());
				fY+=16.0f;

				//Iterate sound entries and names
				int lRow=0, lCol=0;
				for(int i=0; i< (int)vSoundNames.size(); i++)
				{
					cSoundEntry *pEntry = vEntries[i];
					if(pEntry == NULL){
						lRow = 4;
						lCol =0;
						continue;
					}
					gpSimpleCamera->GetSet()->DrawFont(gpSimpleCamera->GetFont(),cVector3f(10 + (float)lCol*250,fY+(float)lRow*13,0),12,cColor(1,1),
						_W("%ls(%.2f)(%d) (%.2f/%.2f)"),
						cString::To16Char(vSoundNames[i]).c_str(),
						pEntry->GetChannel()->GetVolume(),
						pEntry->GetChannel()->GetPriority(),
						pEntry->GetChannel()->GetElapsedTime(),
						pEntry->GetChannel()->GetTotalTime()
						);

					lCol++;
					if(lCol == 3)
					{
						lCol =0;
						lRow++;
					}
				}
				if(vSoundNames.empty()==false) fY+=13.0f * lRow;	
			}
			
			//////////////////////////////
			// Draw entity properties
			if(gpEntity)
			{
				if(gpEntity->GetAnimationStateNum() >0)
				{
					cAnimationState *pAnimState = gpEntity->GetAnimationState(glCurrentAnimaiton);
					pSet->DrawFont(pFont,cVector3f(5,56,0),14,cColor(1,1),
										_W("Animationstate: %d, time: %.2f / %.2f"),
										glCurrentAnimaiton, pAnimState->GetTimePosition(), pAnimState->GetLength());

					/*for(int i=0; i<gpEntity->GetSubMeshEntityNum(); ++i)
					{
						cSubMeshEntity *pSubEnt = gpEntity->GetSubMeshEntity(i);

						pSet->DrawFont(pFont,cVector3f(5,72+i*16,0),14,cColor(1,1), _W("Sub: %s Mtx: %s"),
										cString::To16Char(pSubEnt->GetName()).c_str(),
										cString::To16Char(pSubEnt->GetWorldPosition().ToString()).c_str());
					}*/
				}
			}
		}
	}

	//--------------------------------------------------------------

	void Update(float afTimeStep)
	{
		//Debug when doing update debugg
		//Log("-----\n");
		//////////////////////////////////////////
		// Rotate model
		if(gbRotateModel && gpEntity)
		{
			mfModelRotation += kPif * 0.15f * afTimeStep;
            
			if(gvBodies.empty())
			{
				gpEntity->SetMatrix(cMath::MatrixRotateY(mfModelRotation));
			}
			else
			{
				for(size_t i=0; i<gvBodies.size(); ++i)
				{
					iPhysicsBody *pBody = gvBodies[i];
					cMatrixf mtxTrans = cMath::MatrixMul(cMath::MatrixRotateY(mfModelRotation), gvBodyStartMatrix[i]);
					pBody->SetMatrix(mtxTrans);
				}
			}
			
		}

		//////////////////////////////////////////
		// Body picking
		if(gbPhysicsActive && gvBodies.size()>0 && gpEngine->GetInput()->IsTriggerd("LeftMouse"))
		{
			cCamera *pCam = gpSimpleCamera->GetCamera();

			cVector2l vMousePosInt = gpEngine->GetInput()->GetMouse()->GetAbsPosition();
			cVector2f vMousePos = cVector2f((float)vMousePosInt.x, (float)vMousePosInt.y);

			if(mBodyPicker.mpPickedBody)
			{
				//Get pos of start.
				mBodyPicker.mvPos = cMath::MatrixMul(mBodyPicker.mpPickedBody->GetLocalMatrix(), 
					mBodyPicker.mvLocalPos);

				//Get Drag pos
				cVector3f vDir;
				pCam->UnProject(NULL,&vDir,vMousePos, mpLowLevelGraphics->GetScreenSizeFloat());
				renderCallback.mvDragPos = pCam->GetPosition() + vDir*mBodyPicker.mfDist;

				//Spring testing:
				cVector3f vForce = (renderCallback.mvDragPos - mBodyPicker.mvPos)*20 - 
					(mBodyPicker.mpPickedBody->GetLinearVelocity()*0.4f);


				mBodyPicker.mpPickedBody->AddForceAtPosition(vForce *  mBodyPicker.mpPickedBody->GetMass(),
					mBodyPicker.mvPos);
			}
			else
			{
				cVector3f vDir;
				pCam->UnProject(NULL,&vDir,vMousePos, mpLowLevelGraphics->GetScreenSizeFloat());
				cVector3f vOrigin = pCam->GetPosition();
				cVector3f vEnd = pCam->GetPosition() + vDir*100.0f;

				mBodyPicker.Clear();

				mpPhysicsWorld->CastRay(&mBodyPicker,vOrigin,vEnd,true,false,true);

				if(mBodyPicker.mpPickedBody)
				{
					mBodyPicker.mpPickedBody->SetAutoDisable(false);

					renderCallback.mvDragPos = mBodyPicker.mvPos;

					cMatrixf mtxInvWorld = cMath::MatrixInverse(mBodyPicker.mpPickedBody->GetLocalMatrix());
					mBodyPicker.mvLocalPos = cMath::MatrixMul(mtxInvWorld, mBodyPicker.mvPos);
				}
			}
		}
		else
		{
			if(mBodyPicker.mpPickedBody) mBodyPicker.mpPickedBody->SetAutoDisable(true);
			mBodyPicker.Clear();
		}		

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
	cBodyPicker mBodyPicker;

	
	cWidgetWindow *mpOptionWindow;

	tWStringVec mvPickedFiles;
	tWString msCurrentFilePath;

	float mfModelRotation;

	int mlCurrentLight;
	cWidgetCheckBox *mpCBLightVisible;
	cWidgetCheckBox *mpCBLightCastShadows;
	cWidgetCheckBox *mpCBPhysicsActive;
	cWidgetCheckBox *mpCBRotateModel;
	cWidgetSlider *mpColorSlider[4];
	cWidgetCheckBox *mpCBMaterialTexture[eMaterialTexture_LastEnum];
	cWidgetComboBox *mpCBAnimations;

	std::vector<cMaterialData> mvMaterialData;
	iTexture *mpTexDiffuseNull;
	iTexture *mpTexNMapNull;
};

//-----------------------------------------------------------------------

#ifdef WIN32
	#include <Windows.h>
#endif

#include "../LuxBasePersonal.h"

#ifdef __APPLE__
namespace hpl {
	extern tString FindGameResources();
}
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
#if __APPLE__
	tWString sEditorDir = cPlatform::GetWorkingDir();
	sEditorDir = cString::AddSlashAtEndW(sEditorDir);
	
	tString gameDir = FindGameResources();
	if (gameDir.empty())
	{
		exit(1);
	}
#endif

	//iResourceBase::SetLogCreateAndDelete(true);
	//iGpuProgram::SetLogDebugInformation(true); 
	cRendererDeferred::SetGBufferType(eDeferredGBuffer_32Bit);
	cRendererDeferred::SetNumOfGBufferTextures(3);
	cRendererDeferred::SetSSAOLoaded(true);
	cRendererDeferred::SetEdgeSmoothLoaded(true);
	cRendererDeferred::SetShadowMapQuality(eShadowMapQuality_Medium);
	//cRendererDeferred::SetParallaxQuality(eParallaxQuality_Low);

	//iLowLevelGraphics::SetForceShaderModel3And4Off(true);
	tWString sPersonalDir = cString::ReplaceCharToW(cPlatform::GetSystemSpecialPath(eSystemPath_Personal), _W("\\"), _W("/"));
#ifdef USERDIR_RESOURCES
	tWString sUserResourceDir = sPersonalDir + PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT PERSONAL_RESOURCES;
#endif

	// Create the base personal folders
	tWStringVec vDirs;
#ifdef USERDIR_RESOURCES
	SetupBaseDirs(vDirs, _W("HPL2"), _W(""), true);
#else
	SetupBaseDirs(vDirs, _W("HPL2"));
#endif
	CreateBaseDirs(vDirs, sPersonalDir);

	SetLogFile(sPersonalDir + PERSONAL_RELATIVEROOT _W("HPL2/modelview.log"));

	//Init the game engine
	cEngineInitVars vars;
	vars.mGraphics.mvScreenSize.x = 1024;
	vars.mGraphics.mvScreenSize.y = 768;
	vars.mGraphics.mbFullscreen = false;
	vars.mGraphics.msWindowCaption = "ModelView - Initalizing...";
	//vars.mGraphics.mvWindowPosition = cVector2l(0,0);
	gpEngine = CreateHPLEngine(eHplAPI_OpenGL, eHplSetup_All, &vars);
	gpEngine->SetLimitFPS(false);
	gpEngine->GetGraphics()->GetLowLevel()->SetVsyncActive(false);
	gpEngine->SetWaitIfAppOutOfFocus(true);
	

	if(asCommandline != "")
	{
		gsModelFile = asCommandline;
		gsModelFile = cString::ReplaceCharTo(gsModelFile,"\"","");

		tString sModelDir = cString::GetFilePath(gsModelFile);
		tWString sDir = cString::To16Char(sModelDir);
		if(sDir != _W(""))
			gpEngine->GetResources()->AddResourceDir(sDir,false);
	}
	
	//Add resources
#ifdef USERDIR_RESOURCES
	gpEngine->GetResources()->LoadResourceDirsFile("resources.cfg", sUserResourceDir);
#else
	gpEngine->GetResources()->LoadResourceDirsFile("resources.cfg");
#endif
#ifdef __APPLE__
	gpEngine->GetResources()->AddResourceDir(sEditorDir + _W("viewer/"), true);
#endif
	
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
