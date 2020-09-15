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

#include "../Common/EdResource.h"
#include "../Common/Editor.h"

#include "../Common/EdWorld.h"

//--------------------------------------------------------------------------------------------

bool cEdLoader::LoadTexture(const tString& asFile, eTextureType aTex, iTexture** apTex)
{
	cTextureManager* pMgr = iEditor::GetInstance()->GetEngine()->GetResources()->GetTextureManager();
	iTexture* pTex = NULL;

	if(asFile.empty()==false)
	{
		switch(aTex)
		{
		case eTextureType_1D:
			pTex = pMgr->Create1D(asFile, true); break;
		case eTextureType_2D:
			pTex = pMgr->Create2D(asFile, true); break;
		case eTextureType_CubeMap:
			pTex = pMgr->CreateCubeMap(asFile, true); break;
		};
	}

	if(apTex)
		*apTex = pTex;
	else
	{
		if(pTex) pMgr->Destroy(pTex);
	}

	return pTex!=NULL;
}

//--------------------------------------------------------------------------------------------

bool cEdLoader::LoadTexture1D(const tString& asFile, iTexture** apTex)
{
	return LoadTexture(asFile, eTextureType_1D, apTex);
}

bool cEdLoader::LoadTexture2D(const tString& asFile, iTexture** apTex)
{
	return LoadTexture(asFile, eTextureType_2D, apTex);
}

bool cEdLoader::LoadCubeMap(const tString& asFile, iTexture** apTex)
{
	return LoadTexture(asFile, eTextureType_CubeMap, apTex);
}

//--------------------------------------------------------------------------------------------

bool cEdLoader::LoadMaterial(const tString& asFile, cMaterial** apMat)
{
	cMaterialManager* pMgr = iEditor::GetInstance()->GetEngine()->GetResources()->GetMaterialManager();
	cMaterial* pMat = NULL;

	if(cString::GetFileName(asFile).empty()==false)
		pMat = pMgr->CreateMaterial(asFile);

	if(apMat)
		*apMat = pMat;
	else
	{
		if(pMat) pMgr->Destroy(pMat);
	}

	return pMat!=NULL;
}


//--------------------------------------------------------------------------------------------

bool cEdLoader::LoadPS(const tString& asFile, cParticleSystem** apPS)
{
	cWorld* pWorld = iEditor::GetInstance()->GetWorld()->GetEngWorld();
	cParticleSystem* pPS = NULL;

	if(cString::GetFileName(asFile).empty()==false)
		pPS = pWorld->CreateParticleSystem("", asFile, 1);

	if(apPS)
		*apPS = pPS;
	else
		if(pPS) pWorld->DestroyParticleSystem(pPS);

	return pPS!=NULL;
}

//--------------------------------------------------------------------------------------------

bool cEdLoader::LoadSound(const tString& asFile, cSoundEntityData** apSnd)
{
	cSoundEntityManager* pMgr = iEditor::GetInstance()->GetEngine()->GetResources()->GetSoundEntityManager();
	cSoundEntityData* pSnd = NULL;

	if(cString::GetFileName(asFile).empty()==false)
		pSnd = pMgr->CreateSoundEntity(asFile);

	if(apSnd)
		*apSnd = pSnd;
	else
		if(pSnd) pMgr->Destroy(pSnd);

	return pSnd!=NULL;
}

//--------------------------------------------------------------------------------------------

bool cEdLoader::LoadModel(const tString& asFile, cMesh** apMesh)
{
	cMeshManager* pMgr = iEditor::GetInstance()->GetEngine()->GetResources()->GetMeshManager();
	cMesh* pMesh = NULL;

	if(cString::GetFileName(asFile).empty()==false)
		pMesh = pMgr->CreateMesh(asFile);

	if(apMesh)
		*apMesh = pMesh;
	else
		if(pMesh) pMgr->Destroy(pMesh);

	return pMesh!=NULL;
}

//--------------------------------------------------------------------------------------------

bool cEdLoader::LoadAnimModel(const tString& asFile, cAnimation** apAnim)
{
	cAnimationManager* pMgr = iEditor::GetInstance()->GetEngine()->GetResources()->GetAnimationManager();
	cAnimation* pAnim = NULL;

	if(cString::GetFileName(asFile).empty()==false)
		pAnim = pMgr->CreateAnimation(asFile);

	if(apAnim)
		*apAnim = pAnim;
	else
		if(pAnim) pMgr->Destroy(pAnim);

	return pAnim!=NULL;
}

//--------------------------------------------------------------------------------------------

bool cEdLoader::LoadEntity(const tString& asFile, cMeshEntity* apEnt)
{
	/*cMeshManager* pMgr = iEditor::GetInstance()->GetEngine()->GetResources()->GetMeshManager();
	cMesh* pMesh = NULL;

	if(cString::GetFileName(asFile).empty()==false)
		pMesh = pMgr->CreateMesh(asFile);

	if(apMesh)
		*apMesh = pMesh;
	else
		if(pMesh) pMgr->Destroy(pMesh);

	return pMesh!=NULL;
	*/

	return false;
}

//--------------------------------------------------------------------------------------------
