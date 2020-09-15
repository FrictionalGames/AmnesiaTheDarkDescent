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

#include "EntityWrapperSubMesh.h"
#include "EntityWrapperBody.h"
#include "EntityWrapperBone.h"

#include "EditorBaseClasses.h"
#include "EditorWindowViewport.h"
#include "../modeleditor/ModelEditorWorld.h"

#include "EditorHelper.h"

#include "EditorWindowEntityEditBoxSubMesh.h"

#include <algorithm>

//---------------------------------------------------------------------------

cEngineEntitySubMesh::cEngineEntitySubMesh(iEntityWrapper* apParent) : iEngineEntity(apParent)
{
}

bool cEngineEntitySubMesh::Create(const tString& asName)
{
	cEntityWrapperSubMesh* pParent = (cEntityWrapperSubMesh*)mpParent;
	cEntityWrapperTypeSubMesh* pType = (cEntityWrapperTypeSubMesh*)mpParent->GetType();

	cMeshEntity* pMesh = pType->GetMesh();
	if(pMesh==NULL)
		return false;

	int lSubMeshID = pParent->GetSubMeshID();
	mpEntity = pMesh->GetSubMeshEntity(lSubMeshID);

	iEntityWrapperData* pData = pParent->GetCreationData();
	if(mpEntity && pData)
	{
		cSubMeshEntity* pSubMesh = (cSubMeshEntity*)mpEntity;
		const cMatrixf& mtxSubMeshMatrix = pSubMesh->GetWorldMatrix();

		const tString& sName = pSubMesh->GetSubMesh()->GetName();
		pData->SetName(sName);
		mpParent->SetName(sName);
		pData->SetName(pSubMesh->GetSubMesh()->GetName());
		pData->SetVec3f(eObjVec3f_Position, mtxSubMeshMatrix.GetTranslation());
		pData->SetVec3f(eObjVec3f_Rotation, cMath::MatrixToEulerAngles(mtxSubMeshMatrix.GetRotation(), eEulerRotationOrder_XYZ));
		pData->SetVec3f(eObjVec3f_Scale, pSubMesh->GetCalcScale());
	}

	return IsCreated();
}

//---------------------------------------------------------------------------

bool cEngineEntitySubMesh::CheckRayIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, tVector3fVec* apTri, float* apT)
{
	if(iEngineEntity::CheckRayIntersect(apViewport, apPos, apTri)==false)
		return false;

	return cEditorHelper::CheckRaySubMeshEntityIntersect(apViewport->GetUnprojectedStart(), apViewport->GetUnprojectedEnd(), 
															(cSubMeshEntity*)mpEntity, apPos, apT, NULL, apTri);
}

//---------------------------------------------------------------------------

void cEngineEntitySubMesh::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, 
								bool abIsSelected, bool abIsActive, const cColor& aHighlightCol)
{
	if(abIsSelected==false)
		return;

	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	cSubMeshEntity* pSubMeshEntity = (cSubMeshEntity*)mpEntity;
	apFunctions->SetMatrix(pSubMeshEntity->GetModelMatrix(NULL));
	apFunctions->DrawWireFrame(pSubMeshEntity->GetVertexBuffer(), cColor(1,1,1,0.25f));

	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetDepthTest(false);
}

//---------------------------------------------------------------------------

cBoundingVolume* cEngineEntitySubMesh::GetPickBV(cEditorWindowViewport* apViewport)
{
	return mpEntity->GetBoundingVolume();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

cEntityWrapperTypeSubMesh::cEntityWrapperTypeSubMesh() : iEntityWrapperType(eEditorEntityType_SubMesh, _W("SubMesh"), "SubMesh")
{
	AddInt(eSubMeshInt_SubMeshID, "SubMeshID", -1, ePropCopyStep_PreEnt);
	AddString(eSubMeshStr_Material, "Material", "");

	mbDeletable = false;
	mbRenameable = false;
	mbCloneable = false;

	mpMesh = NULL;

	ClearMesh();
}

//---------------------------------------------------------------------------

bool cEntityWrapperTypeSubMesh::SetMesh(const tString& asFilename, bool abDeleteData,
										tEntityDataVec& avSubMeshData, const tIntList& alstSubMeshIDs,
										tEntityDataVec& avBoneData, const tIntList& alstBoneIDs)
{
	//////////////////////////////////////////////
	// First clean up any loaded mesh
	ClearMesh();


	// Try to create mesh
	iEditorBase* pEditor = mpWorld->GetEditor();
	cResources *pRes = pEditor->GetEngine()->GetResources();
	cMesh* pMesh = NULL;
	if(asFilename!="")
	{
		pRes->AddResourceDir(cString::GetFilePathW(cString::To16Char(asFilename)), false);
		pMesh = pRes->GetMeshManager()->CreateMesh(asFilename);
	}

	// If failed, show error message and return
	if(pMesh==NULL)
	{
		if(asFilename!="")
			pEditor->ShowMessageBox(_W("Warning"), _W("Could not load mesh file: ") + cString::To16Char(asFilename), _W("Ok"), _W(""), NULL, NULL);
		
		mpMesh = NULL;

		return false;
	}

	//////////////////////////////////////////////////////////
	// Now load all stuff related (submeshes + bones, if any)
	tEntityWrapperList lstMeshRelatedEnts;

	bool bSubMeshDataEmpty = avSubMeshData.empty();
	bool bSubMeshIDsEmpty = alstSubMeshIDs.empty();
	bool bBoneDataEmpty = avBoneData.empty();
	bool bBoneIDsEmpty = alstBoneIDs.empty();

	mpMesh = mpWorld->GetWorld()->CreateMeshEntity("ImportedMesh", pMesh);

	cSkeleton* pSkeleton = mpMesh->GetMesh()->GetSkeleton();
	bool bHasSkeleton = (pSkeleton!=NULL);


	// Set up submeshes from data passed as argument and/or data from the loaded mesh
	tIntList::const_iterator itSubMeshIDs = alstSubMeshIDs.begin();
	for(int i=0;i<mpMesh->GetSubMeshEntityNum();i++)
	{
		iEntityWrapperData* pData = NULL;

		if(bSubMeshDataEmpty || i>=(int)avSubMeshData.size())
		{
			pData = CreateData();
			int lID;
			if(bSubMeshIDsEmpty)
				lID = mpWorld->GetFreeID();
			else
			{
				lID = *itSubMeshIDs;
				++itSubMeshIDs;
			}

			pData->SetID(lID);
			pData->SetInt(eSubMeshInt_SubMeshID, i);
		}
		else
			pData = (cEntityWrapperDataSubMesh*)avSubMeshData[i];

		pData->SetName(mpMesh->GetSubMeshEntity(i)->GetSubMesh()->GetName());

		lstMeshRelatedEnts.push_back(mpWorld->CreateEntityWrapperFromData(pData));
	}

	// Set up bones
	if(bHasSkeleton)
	{
		mbCanTranslate = false;
		mbCanRotate = false;
		mScaleType = eScaleType_None;


		std::map<cBone*, int> mapBoneIDs;
		iEntityWrapperType* pBoneType = mpWorld->GetEntityTypeByID(eEditorEntityType_Bone);

		tIntList::const_iterator itBoneIDs = alstBoneIDs.begin();
		for(int i=0;i<pSkeleton->GetBoneNum();i++)
		{
			cBone* pBone = pSkeleton->GetBoneByIndex(i);
			
			iEntityWrapperData* pData = NULL;
			
			if(bBoneDataEmpty || i>=(int)avBoneData.size())
			{
				pData = pBoneType->CreateData();
				int lBoneID;
				if(bBoneIDsEmpty)
					lBoneID = mpWorld->GetFreeID();
				else
				{
					lBoneID = *itBoneIDs;
					++itBoneIDs;
				}

				pData->SetID(lBoneID);
				avBoneData.push_back(pData);
			}
			else
				pData = avBoneData[i];

			pData->SetName(pBone->GetName());
			pData->SetVec3f(eObjVec3f_Position, pBone->GetWorldTransform().GetTranslation());

			// Search for parent ID
			std::map<cBone*, int>::iterator itParents = mapBoneIDs.find(pBone->GetParent());
			int lParentBoneID = -1;
			if(itParents!=mapBoneIDs.end())
				lParentBoneID = itParents->second;
			
			pData->SetInt(eBoneInt_ParentBoneID, lParentBoneID);

			// Insert on map for future parent search
			mapBoneIDs.insert(std::pair<cBone*, int>(pBone, pData->GetID()));

			lstMeshRelatedEnts.push_back(mpWorld->CreateEntityWrapperFromData(pData));
		}
	}


	/////////////////////////////////////
	// Post creation setup
	tEntityWrapperListIt it = lstMeshRelatedEnts.begin();
	for(;it!=lstMeshRelatedEnts.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		pEnt->OnPostDeployAll(abDeleteData);
	}

	return true;
}

//---------------------------------------------------------------------------

void cEntityWrapperTypeSubMesh::ClearMesh()
{
	mbCanTranslate = true;
	mbCanRotate = true;
	mScaleType = eScaleType_Normal;

	if(mpMesh)
	{
		bool bRemoveStuffFromWorld = mpWorld->IsClearingEntities()==false;
		ClearSubMeshes(bRemoveStuffFromWorld);
		((cModelEditorWorld*)mpWorld)->GetBoneType()->ClearBones(bRemoveStuffFromWorld);

		mpWorld->GetWorld()->DestroyMeshEntity((cMeshEntity*)mpMesh);
		mpMesh = NULL;
	}
}

//---------------------------------------------------------------------------

tIntList cEntityWrapperTypeSubMesh::GetSubMeshIDs()
{
	tEntityWrapperList lstSubMeshes;
	lstSubMeshes.insert(lstSubMeshes.end(), mvSubMeshes.begin(), mvSubMeshes.end());

	tIntList lstIDs;
	cEditorHelper::GetIDsFromEntityList(lstSubMeshes, lstIDs);

	return lstIDs;
}

tString cEntityWrapperTypeSubMesh::GetMeshFilename()
{
	if(mpMesh) return cString::To8Char(mpMesh->GetMesh()->GetFullPath());

	return "";
}

void cEntityWrapperTypeSubMesh::AddSubMesh(cEntityWrapperSubMesh* apSubMesh)
{
	if(apSubMesh) mvSubMeshes.push_back(apSubMesh);
}

void cEntityWrapperTypeSubMesh::RemoveSubMesh(cEntityWrapperSubMesh* apSubMesh)
{
	tSubMeshWrapperVecIt it = find(mvSubMeshes.begin(), mvSubMeshes.end(), apSubMesh);
	if(it!=mvSubMeshes.end())
		mvSubMeshes.erase(it);
}

void cEntityWrapperTypeSubMesh::ClearSubMeshes(bool abRemoveFromWorld)
{
	for(int i=0;i<(int)mvSubMeshes.size();++i)
		mpWorld->DestroyEntityWrapper(mvSubMeshes[i], abRemoveFromWorld);

	mvSubMeshes.clear();
}

iEntityWrapperData* cEntityWrapperTypeSubMesh::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataSubMesh,(this));
}

//---------------------------------------------------------------------------

cEntityWrapperDataSubMesh::cEntityWrapperDataSubMesh(iEntityWrapperType* apType) : iEntityWrapperData(apType)
{
}

//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataSubMesh::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperSubMesh,(this));
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEntityWrapperSubMesh::cEntityWrapperSubMesh(iEntityWrapperData* apData) : iEntityWrapper(apData)
{
	/*mpSubMeshEntity = (cSubMeshEntity*)mpEngineEntity;
	mlSubMeshID = alSubMeshID;

	if(mpEngineEntity)
	{
		cMatrixf& mtxEntityMatrix = mpEngineEntity->GetWorldMatrix();

		msSubMeshName = mpSubMeshEntity->GetSubMesh()->GetName();
		mvLocalTranslation = mtxEntityMatrix.GetTranslation();
		mvLocalRotation = cMath::MatrixToEulerAngles(mtxEntityMatrix.GetRotation(), eEulerRotationOrder_XYZ);
		mvLocalScale = mpSubMeshEntity->GetCalcScale();

		cSkeleton* pSkeleton = (cSkeleton*)mpSubMeshEntity->GetUserData();
		bool bHasNoSkeleton = (pSkeleton==NULL);

		mbCanRotate = bHasNoSkeleton;
		mbCanTranslate = bHasNoSkeleton;
		mScaleType = bHasNoSkeleton?eScaleType_Normal:eScaleType_None;
	}
	*/
}

cEntityWrapperSubMesh::~cEntityWrapperSubMesh()
{
	cEntityWrapperTypeSubMesh* pType = (cEntityWrapperTypeSubMesh*)mpType;
	pType->RemoveSubMesh(this);
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

bool cEntityWrapperSubMesh::SetProperty(int alPropID, const int& alX)
{
	switch(alPropID)
	{
	case eSubMeshInt_SubMeshID:
		mlSubMeshID = alX;		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, alX);
	}

	return true;
}

bool cEntityWrapperSubMesh::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eSubMeshStr_Material:
		SetMaterialFile(asX);	break;
	default:
		return iEntityWrapper::SetProperty(alPropID, asX);
	}

	return true;
}

bool cEntityWrapperSubMesh::GetProperty(int alPropID, int& alX)
{
	switch(alPropID)
	{
	case eSubMeshInt_SubMeshID:
		alX = GetSubMeshID();	break;
	default:
		return iEntityWrapper::GetProperty(alPropID, alX);
	}

	return true;
}

bool cEntityWrapperSubMesh::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eSubMeshStr_Material:
		asX = GetMaterialFile();	break;
	default:
		return iEntityWrapper::GetProperty(alPropID, asX);
	}

	return true;
}


/*
bool cEntityWrapperSubMesh::CheckRayAccurateIntersection(const cVector3f& avRayStart, const cVector3f& avRayEnd, cVector3f* apIntersection, tVector3fVec* apTriangle)
{
	return cEditorHelper::CheckRaySubMeshEntityIntersect(avRayStart, avRayEnd, mpSubMeshEntity, apIntersection, NULL, NULL, apTriangle);
}
*/

//-------------------------------------------------------------------

/*
void cEntityWrapperSubMesh::SaveToElement(cXmlElement* apElement)
{
	apElement->SetValue("SubMesh");
	iEntityWrapper::SaveToElement(apElement);

	apElement->SetAttributeInt("SubMeshID", mlSubMeshID);
}
*/
//-------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperSubMesh::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = hplNew(cEditorWindowEntityEditBoxSubMesh,(apEditMode,this));

	return pEditBox;
}

void cEntityWrapperSubMesh::SetMaterialFile(const tString& asMatFile)
{
	cMaterialManager* pManager = GetEditorWorld()->GetEditor()->GetEngine()->GetResources()->GetMaterialManager();
	cMaterial* pMat = pManager->CreateMaterial(asMatFile);

	if(pMat==NULL)
		msMatFile = "";
	else
		msMatFile = asMatFile;

	if(mpEngineEntity) 
		((cSubMeshEntity*)mpEngineEntity->GetEntity())->SetCustomMaterial(pMat);
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEntityWrapperSubMesh::OnAddToWorld()
{
	cEntityWrapperTypeSubMesh* pType = (cEntityWrapperTypeSubMesh*)mpType;
	pType->AddSubMesh(this);
}

iEngineEntity* cEntityWrapperSubMesh::CreateSpecificEngineEntity()
{
	return hplNew(cEngineEntitySubMesh, (this));
}

/*
iEntity3D* cEntityWrapperSubMesh::CreateEngineEntity(iEditorWorld* apEditorWorld, const tString& asFilename, int alSubMeshID, int alID)
{
	cMeshEntity* pMeshEntity = apEditorWorld->GetWorld()->GetDynamicMeshEntity("ImportedMesh");
	if(pMeshEntity==NULL)
	{
		cResources* pRes = apEditorWorld->GetEditor()->GetEngine()->GetResources();

		cMesh* pMesh = pRes->GetMeshManager()->CreateMesh(asFilename);

		if(pMesh==NULL)
			return NULL;

		pMeshEntity = apEditorWorld->GetWorld()->CreateMeshEntity("ImportedMesh", pMesh);
	}

	cSkeleton* pSkeleton = pMeshEntity->GetMesh()->GetSkeleton();

	cSubMeshEntity* pSubMeshEntity = pMeshEntity->GetSubMeshEntity(alSubMeshID);
	pSubMeshEntity->SetUserData(pSkeleton);

	return pSubMeshEntity;
}
*/

//-------------------------------------------------------------------
