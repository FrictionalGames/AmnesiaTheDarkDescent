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

#include "EditorThumbnailBuilder.h"

#include "EditorBaseClasses.h"

//-------------------------------------------------------------------

cThumbnailRenderCallback::cThumbnailRenderCallback()
{
	mpObject = NULL;
}

void cThumbnailRenderCallback::OnPostSolidDraw(cRendererCallbackFunctions* apFunctions)
{
}

//-------------------------------------------------------------------

void cThumbnailRenderCallback::OnPostTranslucentDraw(cRendererCallbackFunctions* apFunctions)
{
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEditorThumbnailBuilder::cEditorThumbnailBuilder(iEditorBase* apEditor)
{
	mpEditor = apEditor;

	cEngine* pEngine = mpEditor->GetEngine();
	cGraphics* pGfx = pEngine->GetGraphics();
	cScene* pScene = pEngine->GetScene();

	cWorld* pWorld = mpEditor->GetTempWorld();//pScene->CreateWorld("ThumbnailWorld");
	pWorld->SetSkyBoxActive(true);
	pWorld->SetSkyBoxColor(cColor(0.2f,1));

	cCamera* pCamera = pScene->CreateCamera(eCameraMoveMode_Fly);
	pCamera->SetPitchLimits(0,0);

	iLight* pLight = NULL;

	///////////////////////////////////////////
	// Set up Lights
	pLight = pWorld->CreateLightPoint("ThumbCameraLight", "", false);
	pLight->SetDiffuseColor(cColor(1,1,1,0));
	pLight->SetCastShadows(false);
	pLight->SetVisible(true);

	pLight = pWorld->CreateLightPoint("ThumbKeyLight","",false);
	pLight->SetDiffuseColor(cColor(1,1,1,0));
	pLight->SetVisible(false);
	pLight->SetCastShadows(false);
	pLight->SetRadius(50);
	
	pLight = pWorld->CreateLightPoint("ThumbFillLight","",false);
	pLight->SetDiffuseColor(cColor(1,1,1,0));
	pLight->SetVisible(false);
	pLight->SetCastShadows(false);
	pLight->SetRadius(50);

	pLight = pWorld->CreateLightPoint("ThumbBackLight","",false);
	pLight->SetDiffuseColor(cColor(1,1,1,0));
	pLight->SetVisible(false);
	pLight->SetCastShadows(false);
	pLight->SetRadius(50);

	//////////////////////////////////////////
	// Set up render targets
	iTexture* pRenderTarget64 = pGfx->CreateTexture("Thumbnail", eTextureType_2D, eTextureUsage_RenderTarget);
	pRenderTarget64->SetWrapR(eTextureWrap_Clamp);
	pRenderTarget64->SetWrapS(eTextureWrap_Clamp);
	pRenderTarget64->CreateFromRawData(cVector3l(64,64,0), ePixelFormat_RGBA, NULL);

	mpFB64 = pGfx->CreateFrameBuffer("Thumbnail");
	mpFB64->SetTexture2D(0, pRenderTarget64);
	mpFB64->CompileAndValidate();

	mpRenderTarget128 = pGfx->CreateTexture("ThumbnailDest",eTextureType_2D, eTextureUsage_RenderTarget);
	mpRenderTarget128->SetWrapR(eTextureWrap_Clamp);
	mpRenderTarget128->SetWrapS(eTextureWrap_Clamp);
	mpRenderTarget128->CreateFromRawData(cVector3l(128,128,0), ePixelFormat_RGBA, NULL);

	mpFB128 = pGfx->CreateFrameBuffer("ThumbnailDestination");
	mpFB128->SetTexture2D(0,mpRenderTarget128);
	mpFB128->CompileAndValidate();

	mpViewport = pScene->CreateViewport(pCamera,pWorld,true);
	mpViewport->SetSize(cVector2l(128));
	mpViewport->SetFrameBuffer(mpFB128);
	mpViewport->SetActive(false);
	mpViewport->SetVisible(false);
	mpViewport->GetRenderSettings()->mClearColor = cColor(0,1);
	mpViewport->AddRendererCallback(&mThumbnailCallback);
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEditorThumbnailBuilder::BuildThumbnailFromMeshEntity(cMeshEntity* apEntity, const tWString& asDestName)
{
	cWorld* pOldWorld = apEntity->GetWorld();

	iLowLevelGraphics* pGfx = mpEditor->GetEngine()->GetGraphics()->GetLowLevel();
	cCamera* pCamera = mpViewport->GetCamera();
	cWorld* pWorld = mpViewport->GetWorld();
	iLight* pKeyLight = pWorld->GetLight("ThumbKeyLight");
	iLight* pFillLight = pWorld->GetLight("ThumbFillLight");
	iLight* pBackLight = pWorld->GetLight("ThumbBackLight");

	apEntity->SetWorld(pWorld);
	apEntity->SetPosition(0);
	apEntity->SetVisible(true);
	
	FocusCameraOnEntity(apEntity);

	pKeyLight->SetPosition(10);
	pFillLight->SetPosition(cVector3f(10,10,0));
	pBackLight->SetPosition(cVector3f(0,10,10));

	//////////////////////////////////////
	// Render 128x128 thumbnail
	mpViewport->GetRenderer()->Render(	1.0f/60.0f, 
										pCamera->GetFrustum(), 
										pWorld,
										mpViewport->GetRenderSettings(), 
										mpViewport->GetRenderTarget(), 
										false, 
										mpViewport->GetRendererCallbackList() );
	pGfx->WaitAndFinishRendering();
	
	mThumbnailCallback.mpObject = NULL;
	
	//////////////////////////////////
	// Now scale to 64x64
	pGfx->SetDepthTestActive(false);
	pGfx->SetDepthWriteActive(false);
	pGfx->SetBlendActive(false);

	pGfx->SetTexture(0, mpRenderTarget128);
	pGfx->SetOrthoProjection(128,-1000,1000);
	pGfx->SetIdentityMatrix(eMatrix_ModelView);
	pGfx->DrawQuad(0,128,cVector2f(0,1),cVector2f(1,0));
	pGfx->WaitAndFinishRendering();
	pGfx->SetTexture(0,NULL);
	
	/////////////////////////////////
	// Save to file
	cBitmap *pBmp = pGfx->CopyFrameBufferToBitmap();
	mpEditor->GetEngine()->GetResources()->GetBitmapLoaderHandler()->SaveBitmap(pBmp,GetThumbnailNameFromFileW(cString::To16Char(apEntity->GetSourceFile())),0);
	hplDelete(pBmp);

	//pGfx->SetCurrentFrameBuffer(NULL);
	//pGfx->SetDepthTestActive(true);
	//pGfx->SetDepthWriteActive(true);
	//pGfx->SetBlendActive(true);

	//pEnt->SetPosition(1000000.0f);
	apEntity->SetWorld(pOldWorld);
	apEntity->SetRenderFlagBit(eRenderableFlag_VisibleInNonReflection, false);
	apEntity->SetRenderFlagBit(eRenderableFlag_VisibleInReflection, false);
	apEntity->SetVisible(false);
}

//-------------------------------------------------------------------

void cEditorThumbnailBuilder::BuildThumbnailFromMesh(const tWString& asMeshFilename, const tWString& asDestName)
{
	iLowLevelGraphics* pGfx = mpEditor->GetEngine()->GetGraphics()->GetLowLevel();
	cCamera* pCamera = mpViewport->GetCamera();
	cWorld* pWorld = mpViewport->GetWorld();
	iLight* pKeyLight = pWorld->GetLight("ThumbKeyLight");
	iLight* pFillLight = pWorld->GetLight("ThumbFillLight");
	iLight* pBackLight = pWorld->GetLight("ThumbBackLight");

	tString sMeshFilename = cString::To8Char(asMeshFilename);

	cMesh* pMesh = mpEditor->GetEngine()->GetResources()->GetMeshManager()->CreateMesh(sMeshFilename);

	if(pMesh==NULL)
		return;

	tString sName = "ThumbnailObject_" + sMeshFilename;

	cMeshEntity* pEnt = pWorld->GetDynamicMeshEntity(sName);
	if(pEnt==NULL)
	{
		pEnt = pWorld->CreateMeshEntity(sName, pMesh, false);
		mlstThumbnailEntities.push_back(pEnt);
	}

	pEnt->SetPosition(0);
	pEnt->SetRenderFlagBit(eRenderableFlag_VisibleInNonReflection, true);
	pEnt->SetRenderFlagBit(eRenderableFlag_VisibleInReflection, true);
	pEnt->SetVisible(true);

	FocusCameraOnEntity(pEnt);

	pKeyLight->SetPosition(10);
	pFillLight->SetPosition(cVector3f(10,10,0));
	pBackLight->SetPosition(cVector3f(0,10,10));

	mThumbnailCallback.mpObject = pEnt;
	
	//////////////////////////////////////
	// Render 128x128 thumbnail
	mpViewport->GetRenderer()->Render(	1.0f/60.0f, 
										pCamera->GetFrustum(), 
										pWorld,
										mpViewport->GetRenderSettings(), 
										mpViewport->GetRenderTarget(), 
										false, 
										mpViewport->GetRendererCallbackList() );
	pGfx->WaitAndFinishRendering();
	
	mThumbnailCallback.mpObject = NULL;
	
	//////////////////////////////////
	// Now scale to 64x64
	pGfx->SetDepthTestActive(false);
	pGfx->SetDepthWriteActive(false);
	pGfx->SetBlendActive(false);

	pGfx->SetTexture(0, mpRenderTarget128);
	pGfx->SetOrthoProjection(128,-1000,1000);
	pGfx->SetIdentityMatrix(eMatrix_ModelView);
	pGfx->DrawQuad(0,128,cVector2f(0,1),cVector2f(1,0));
	pGfx->WaitAndFinishRendering();
	pGfx->SetTexture(0,NULL);
	
	/////////////////////////////////
	// Save to file
	cBitmap *pBmp = pGfx->CopyFrameBufferToBitmap();
	mpEditor->GetEngine()->GetResources()->GetBitmapLoaderHandler()->SaveBitmap(pBmp,GetThumbnailNameFromFileW(asMeshFilename),0);
	hplDelete(pBmp);

	pEnt->SetVisible(false);
}

//-------------------------------------------------------------------

void cEditorThumbnailBuilder::BuildThumbnailFromImage(const tWString& asImageFilename, const tWString& asDestName)
{
	cResources* pRes = mpEditor->GetEngine()->GetResources();
	cTextureManager* pManager = pRes->GetTextureManager();
	iTexture* pTex = pManager->Create2D(cString::To8Char(asImageFilename), true, eTextureType_2D);
	if(pTex==NULL)
		return;

	int lWidth = pTex->GetWidth();
	int lHeight = pTex->GetHeight();

	if(cMath::IsPow2(lWidth)== false || cMath::IsPow2(lHeight)==false)
		pTex = pManager->Create2D(cString::To8Char(asImageFilename), true, eTextureType_Rect);	

	iLowLevelGraphics* pGfx = mpEditor->GetEngine()->GetGraphics()->GetLowLevel();

	pGfx->SetTexture(0, pTex);
	pGfx->SetOrthoProjection(128,-1000,1000);
	pGfx->SetIdentityMatrix(eMatrix_ModelView);
	pGfx->SetBlendActive(true);
	pGfx->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
	pGfx->DrawQuad(0,128,cVector2f(0,0),cVector2f(1,1));
	pGfx->WaitAndFinishRendering();	
	
	pGfx->SetTexture(0,NULL);
	mpEditor->GetEngine()->GetResources()->GetTextureManager()->Destroy(pTex);

	/////////////////////////////////
	// Save to file
	tWString sDestinationFile = GetThumbnailNameFromFileW(asImageFilename);
	cBitmap *pBmp = pGfx->CopyFrameBufferToBitmap();
	mpEditor->GetEngine()->GetResources()->GetBitmapLoaderHandler()->SaveBitmap(pBmp,sDestinationFile,0);
	hplDelete(pBmp);

	pGfx->ClearFrameBuffer(eClearFrameBufferFlag_Color);
}

//-------------------------------------------------------------------

void cEditorThumbnailBuilder::SaveToFile(iTexture* apTexture)
{
	/////////////////////////////////
	// Save to file
	/*tWString sDestinationFile = GetThumbnailNameFromFileW(asImageFilename);
	cBitmap *pBmp = pGfx->CopyFrameBufferToBitmap();
	mpEditor->GetEngine()->GetResources()->GetBitmapLoaderHandler()->SaveBitmap(pBmp,sDestinationFile,0);
	hplDelete(pBmp);
*/
	//pGfx->ClearFrameBuffer(eClearFrameBufferFlag_Color);
}

//-------------------------------------------------------------------

tString cEditorThumbnailBuilder::GetThumbnailNameFromFile(const tWString& asFile)
{
	return cString::To8Char(GetThumbnailNameFromFileW(asFile));
}

tWString cEditorThumbnailBuilder::GetThumbnailNameFromFileW(const tWString& asFile)
{
	tWString sNormalizedFileString;
	mSha << asFile >> sNormalizedFileString << SHA1::reset;

	return mpEditor->GetThumbnailDir() + sNormalizedFileString + _W("_tmb.jpg");
}

//-------------------------------------------------------------------

void cEditorThumbnailBuilder::FocusCameraOnEntity(cMeshEntity *apEntity)
{
	cCamera *pCam = mpViewport->GetCamera();
	cMesh *pMesh = apEntity->GetMesh();
	cWorld* pWorld = mpViewport->GetWorld();
	iLight* pLight = pWorld->GetLight("ThumbCameraLight");

	cBoundingVolume meshBV;
	if(pMesh->GetSkeleton())
	{
		for(int i=0; i<pMesh->GetSubMeshNum(); ++i)
		{
			cSubMesh *pSubMesh = pMesh->GetSubMesh(i);
			iVertexBuffer *pVtxBuffer = pSubMesh->GetVertexBuffer();

			meshBV.AddArrayPoints(pVtxBuffer->GetFloatArray(eVertexBufferElement_Position), pVtxBuffer->GetVertexNum());
		}
		meshBV.CreateFromPoints(4);
		meshBV.SetTransform(apEntity->GetWorldMatrix());
	}
	else
	{
		meshBV = *apEntity->GetBoundingVolume();
	}
	

	/////////////////////////
	// Calculate direction most tris face
	cVector3f vVecSum =0;
	float fVecCount=0;
	for(int i=0; i<pMesh->GetSubMeshNum(); ++i)
	{	
		cSubMesh *pSubMesh = pMesh->GetSubMesh(i);
		cSubMeshEntity *pSubEnt = apEntity->GetSubMeshEntity(i);

		cMatrixf mtxTransform = pSubEnt->GetWorldMatrix();
		iVertexBuffer *pVtxBuffer = pSubMesh->GetVertexBuffer();

		VtxBufferAddNormals(mtxTransform, pVtxBuffer,vVecSum,fVecCount);
	}

	vVecSum.Normalize();
	
	/////////////////////////
	// Calculate position
	cVector3f vDir=1;
	if(vVecSum.x >= 0)	vDir.x = 1;
	else				vDir.x = -1;
	if(vVecSum.y >= 0)	vDir.y = 1;
	else				vDir.y = -1;
	if(vVecSum.z >= 0)	vDir.z = 1;
	else				vDir.z = -1;
	vDir.Normalize();

	float fDist = meshBV.GetSize().x;
	if(fDist < meshBV.GetSize().y) fDist = meshBV.GetSize().y;
	if(fDist < meshBV.GetSize().z) fDist = meshBV.GetSize().z;
	
	//Making the 2.0 larger might reduce some of the perspective distortion if needed
	fDist = sqrt(fDist*fDist)*2.0f;

    cVector3f vCamPos = meshBV.GetWorldCenter() + vDir * fDist;// + cVector3f(0,fYDir*fDist, 0);
	pCam->SetPosition(vCamPos);

	/////////////////////////
	// Calculate Direction
	cVector3f vAngels = cMath::GetAngleFromPoints3D(vCamPos, meshBV.GetWorldCenter());
	pCam->SetYaw(vAngels.y);
	if(vAngels.x > kPif) vAngels.x = vAngels.x - k2Pif;
	pCam->SetPitch(vAngels.x);


	/////////////////////////
	// Calculate FOV

	//Corners of the bounding box
	cVector3f vMin = meshBV.GetMin();
	cVector3f vMax = meshBV.GetMax();
	cVector3f vCorners[8] =
	{
			cVector3f(vMax.x,vMax.y,vMax.z),
			cVector3f(vMax.x,vMax.y,vMin.z),
			cVector3f(vMax.x,vMin.y,vMax.z),
			cVector3f(vMax.x,vMin.y,vMin.z),

			cVector3f(vMin.x,vMax.y,vMax.z),
			cVector3f(vMin.x,vMax.y,vMin.z),
			cVector3f(vMin.x,vMin.y,vMax.z),
			cVector3f(vMin.x,vMin.y,vMin.z),
	};

	// For each corner, check angle between it and the direction to the center
	// Use largest angle x 2 as FOV
	float fLargestAngle =0;
	cVector3f vDirToCenter = cMath::Vector3Normalize(meshBV.GetWorldCenter() - vCamPos);
	for(int i=0;i<8; ++i)
	{
		cVector3f vToCorner =  cMath::Vector3Normalize(vCorners[i] - vCamPos);
		
		float fAngle = cMath::Vector3Angle(vToCorner, vDirToCenter);
		if(fAngle > fLargestAngle) fLargestAngle = fAngle;
	}
	
	pCam->SetFOV(fLargestAngle*2);
	pCam->SetAspect(1); //<- this makes it looks stretch but since u render to a square it should be like this for you!
	
	/////////////////////////
	// Light
	pLight->SetRadius(fDist*4);
	pLight->SetPosition(vCamPos);
}

//-------------------------------------------------------------------

void cEditorThumbnailBuilder::VtxBufferAddNormals(const cMatrixf a_mtxTransform, iVertexBuffer *apVtxBuffer, cVector3f &avVecSum, float& afCount)
{
	float *pPosVec = apVtxBuffer->GetFloatArray(eVertexBufferElement_Position);
	unsigned int *pIdxVec = apVtxBuffer->GetIndices();
	int lIndexNum = apVtxBuffer->GetIndexNum();

	/////////////////////////////
	// Iterate triangles
	for(int tri=0; tri<lIndexNum; tri+=3)
	{
		///////////////////////
		//Get positions 
		cVector3f vPos[3];
		cVector3f vCenterPos=0;
		for(int i=0; i<3; ++i)
		{
			unsigned int lPosIdx = pIdxVec[tri+i];
			vPos[i].x = pPosVec[lPosIdx*4 + 0];
			vPos[i].y = pPosVec[lPosIdx*4 + 1];
			vPos[i].z = pPosVec[lPosIdx*4 + 2];

			vPos[i] = cMath::MatrixMul(a_mtxTransform,vPos[i]);
			vCenterPos += vPos[i];
		}
		vCenterPos = vCenterPos / 3.0f;

		///////////////////////
		//Calculate normal
		cVector3f vNormal = cMath::Vector3Cross(vPos[2] - vPos[0], vPos[1] - vPos[0]);
		vNormal.Normalize();

		//Use the area of poly to increase normal mul a bit.
		float fTriSide[3] = { cMath::Vector3Dist(vPos[0],vPos[1]),cMath::Vector3Dist(vPos[1],vPos[2]), cMath::Vector3Dist(vPos[2],vPos[0]) };
		float fS = (fTriSide[0]+fTriSide[1]+fTriSide[2]) / 2.0f;
		float fArea = sqrt(fS * (fS - fTriSide[0]) * (fS - fTriSide[1]) * (fS - fTriSide[2]));

		vNormal = vNormal * fArea;

		//Make sure that 2 times normals need to point down in order to view it from below.
		if(vNormal.y < 0) vNormal = vNormal * 0.45f;
		
		//In case both sides have equal amount of polygons
		if(vNormal.x < 0) vNormal = vNormal * 0.85f;
		else if(vNormal.z < 0) vNormal = vNormal * 0.8f;

		avVecSum += vNormal;
		afCount+=1;
	}
}

//-------------------------------------------------------------------

void cEditorThumbnailBuilder::PreBuild()
{
	iLowLevelGraphics* pGfx = mpEditor->GetEngine()->GetGraphics()->GetLowLevel();

	pGfx->SetCurrentFrameBuffer(mpFB128);
}

//-------------------------------------------------------------------

void cEditorThumbnailBuilder::PostBuild()
{
	iLowLevelGraphics* pGfx = mpEditor->GetEngine()->GetGraphics()->GetLowLevel();

	pGfx->SetCurrentFrameBuffer(NULL);

	pGfx->SetDepthTestActive(true);
	pGfx->SetDepthWriteActive(true);
	pGfx->SetBlendActive(true);
}

//-------------------------------------------------------------------

void cEditorThumbnailBuilder::CleanUp()
{
	cWorld* pWorld = mpViewport->GetWorld();
	tMeshEntityListIt itEntities = mlstThumbnailEntities.begin();
	for(;itEntities!=mlstThumbnailEntities.end();++itEntities)
	{
		pWorld->DestroyMeshEntity(*itEntities);
		//mpEditor->RemoveTempMeshEntity(*itEntities);
	}
	mlstThumbnailEntities.clear();
}

//-------------------------------------------------------------------

