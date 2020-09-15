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

#include "../ModelEditor/EdObjSubMesh.h"
//#include "EntityWrapperBody.h"
#include "../ModelEditor/EdObjBone.h"

#include "../Common/Editor.h"
#include "../Common/EdResource.h"

//#include "EditorBaseClasses.h"
#include "../Common/EdViewport.h"
#include "../ModelEditor/HplEntity.h"

#include "../Common/EdHelper.h"

#include "../ModelEditor/EdEditPaneSubMesh.h"

#include <algorithm>

//---------------------------------------------------------------------------

cEngineSubMesh::cEngineSubMesh(iEdScnObject* apParent) : iEngineObject(apParent)
{
}

bool cEngineSubMesh::Create(const tString& asName)
{
	cEdObjSubMesh* pParent = static_cast<cEdObjSubMesh*>(mpParent);
	cTypeSubMesh* pType = static_cast<cTypeSubMesh*>(mpParent->GetType());

	cMeshEntity* pMesh = pType->GetMesh();
	if(pMesh==NULL)
		return false;

	int lSubMeshID = pParent->GetSubMeshID();
	cSubMeshEntity* pSubMesh = pMesh->GetSubMeshEntity(lSubMeshID);
	pSubMesh->SetRenderableUserData(pParent);

	mpObject = pSubMesh;

	iEdObjectData* pData = pParent->GetCreatorData();
	if(mpObject && pData)
	{
		cSubMeshEntity* pSubMesh = static_cast<cSubMeshEntity*>(mpObject);
		const cMatrixf& mtxSubMeshMatrix = pSubMesh->GetWorldMatrix();

		tWString& sName = cString::To16Char(pSubMesh->GetSubMesh()->GetName());
		pData->SetName(sName);
		mpParent->SetName(sName);

		pData->SetVec3f(eScnVec3f_Translation, mtxSubMeshMatrix.GetTranslation());
		pData->SetVec3f(eScnVec3f_Rotation, cMath::MatrixToEulerAngles(mtxSubMeshMatrix.GetRotation(), eEulerRotationOrder_XYZ));
		pData->SetVec3f(eScnVec3f_Scale, pSubMesh->GetCalcScale());
	}

	return IsCreated();
}

//---------------------------------------------------------------------------

bool cEngineSubMesh::CheckRayIntersect(iEdViewport* apViewport, cVector3f* apPos, tVector3fVec* apTri, float* apT)
{
	if(iEngineObject::CheckRayIntersect(apViewport, apPos, apTri)==false)
		return false;

	cSubMeshEntity* pSubMesh = static_cast<cSubMeshEntity*>(mpObject);
	return cEdHelper::CheckRaySubMeshEntityIntersect(apViewport->GetUnprojectedStart(), apViewport->GetUnprojectedEnd(), 
															pSubMesh, apPos, apT, NULL, apTri);
}

//---------------------------------------------------------------------------

void cEngineSubMesh::Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions, 
								bool abIsSelected, bool abIsActive, const cColor& aHighlightCol)
{
	if(abIsSelected==false)
		return;

	cSubMeshEntity* pSubMeshEntity = static_cast<cSubMeshEntity*>(mpObject);
	apViewport->DrawSolidColorVertexBuffer(pSubMeshEntity->GetVertexBuffer(), 
											pSubMeshEntity->GetModelMatrix(NULL), 
											aHighlightCol, 
											apFunctions);
}

//---------------------------------------------------------------------------

cBoundingVolume* cEngineSubMesh::GetPickBV(iEdViewport* apViewport)
{
	return mpObject->GetBoundingVolume();
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// TYPE SUBMESH - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

typedef std::map<cBone*, int>	tBoneIdMap;
typedef tBoneIdMap::iterator	tBoneIdMapIt;

//---------------------------------------------------------------------------

cTypeSubMesh::cTypeSubMesh() : iEdScnObjType(_W("SubMesh"), "SubMesh")
{
	AddInt(eSubMeshInt_SubMeshID, "SubMeshID", -1, ePropStep_PreCreate);
	AddString(eSubMeshStr_Material, "Material", "");

	mbDeletable = false;
	mbRenameable = false;
	mbCloneable = false;

	mpMesh = NULL;

	ClearMesh();
}

//---------------------------------------------------------------------------

bool cTypeSubMesh::SetMesh(const tString& asFilename, bool abDeleteData,
							tEdObjDataVec& avSubMeshData, const tIntList& alstSubMeshIDs,
							tEdObjDataVec& avBoneData, const tIntList& alstBoneIDs)
{
	//////////////////////////////////////////////
	// First clean up any loaded mesh
	ClearMesh();


	// Try to create mesh
	iEditor* pEditor = mpWorld->GetEditor();
	cResources *pRes = pEditor->GetEngine()->GetResources();
	cMesh* pMesh = NULL;

	cEdLoader::LoadModel(asFilename, &pMesh);
	//if(asFilename.empty())
	//{
	//	pRes->AddResourceDir(cString::GetFilePathW(cString::To16Char(asFilename)), false);
	//	pMesh = pRes->GetMeshManager()->CreateMesh(asFilename);
	//}

	// If failed, show error message and return
	if(pMesh==NULL)
	{
		//if(asFilename!="")
		//	pEditor->ShowMessageBox(_W("Warning"), _W("Could not load mesh file: ") + cString::To16Char(asFilename), _W("Ok"), _W(""), NULL, NULL);
		
		mpMesh = NULL;

		return false;
	}

	//////////////////////////////////////////////////////////
	// Now load all stuff related (submeshes + bones, if any)
	tEdObjectList lstMeshRelatedEnts;

	bool bSubMeshDataEmpty = avSubMeshData.empty();
	bool bSubMeshIDsEmpty = alstSubMeshIDs.empty();
	bool bBoneDataEmpty = avBoneData.empty();
	bool bBoneIDsEmpty = alstBoneIDs.empty();

	mpMesh = mpWorld->GetEngWorld()->CreateMeshEntity("ImportedMesh", pMesh);

	cSkeleton* pSkeleton = mpMesh->GetMesh()->GetSkeleton();
	bool bHasSkeleton = (pSkeleton!=NULL);


	// Set up submeshes from data passed as argument and/or data from the loaded mesh
	tIntList::const_iterator itSubMeshIDs = alstSubMeshIDs.begin();
	for(int i=0;i<mpMesh->GetSubMeshEntityNum();i++)
	{
		cSubMeshEntity* pSubMeshEnt = mpMesh->GetSubMeshEntity(i);
		tWString sSubMeshName = cString::To16Char(pSubMeshEnt->GetSubMesh()->GetName());

		iEdObjectData* pData = NULL;

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
			pData->SetName(sSubMeshName);
		}
		else
			pData = GetDataForName(sSubMeshName, avSubMeshData);

		lstMeshRelatedEnts.push_back(mpWorld->CreateObjFromData(pData));
	}

	// Set up bones
	if(bHasSkeleton)
	{
		mbTranslateable = false;
		mbRotateable = false;
		mbScalable = false;

		std::map<tWString, cBone*> mapBoneNames;
		tBoneIdMap mapBoneIDs;
		iEdObjectType* pBoneType = mpWorld->GetTypeByName(_W("Bone"));

		tIntList::const_iterator itBoneIDs = alstBoneIDs.begin();
		
		for(int i=0;i<pSkeleton->GetBoneNum();i++)
		{
			cBone* pBone = pSkeleton->GetBoneByIndex(i);
			tWString sBoneName = cString::To16Char(pBone->GetName());

			iEdObjectData* pData = NULL;
			
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
				pData->SetName(sBoneName);
				
				avBoneData.push_back(pData);
			}
			else
				pData = GetDataForName(sBoneName, avBoneData);

			pData->SetVec3f(eScnVec3f_Translation, pBone->GetWorldTransform().GetTranslation());

			// Search for parent ID
			tBoneIdMapIt itParents = mapBoneIDs.find(pBone->GetParent());
			int lParentBoneID = -1;
			if(itParents!=mapBoneIDs.end())
				lParentBoneID = itParents->second;
			
			pData->SetInt(eBoneInt_ParentBoneID, lParentBoneID);

			// Insert on map for future parent search
			mapBoneIDs.insert(tBoneIdMap::value_type(pBone, pData->GetID()));

			lstMeshRelatedEnts.push_back(mpWorld->CreateObjFromData(pData));
		}
	}


	/////////////////////////////////////
	// Post creation setup
	tEdObjectListIt it = lstMeshRelatedEnts.begin();
	for(;it!=lstMeshRelatedEnts.end();++it)
	{
		iEdObject* pObj = *it;
		pObj->PostDeployAll(abDeleteData);
	}

	return true;
}

//---------------------------------------------------------------------------

void cTypeSubMesh::ClearMesh()
{
	mbTranslateable = true;
	mbRotateable = true;
	//mScaleType = eScaleType_Normal;

	if(mpMesh)
	{
		cHplEntity* pEnt = static_cast<cHplEntity*>(mpWorld);
		cMeshEntity* pMeshEnt = static_cast<cMeshEntity*>(mpMesh);

		ClearObjects();
		pEnt->GetTypeBone()->ClearObjects();

		pEnt->GetEngWorld()->DestroyMeshEntity(pMeshEnt);
		mpMesh = NULL;
	}
}

//---------------------------------------------------------------------------

tString cTypeSubMesh::GetMeshFilename()
{
	if(mpMesh) return cString::To8Char(mpMesh->GetMesh()->GetFullPath());

	return "";
}

iEdObjectData* cTypeSubMesh::CreateTypeSpecificData()
{
	return hplNew(cEdObjSubMeshData,(this));
}

//---------------------------------------------------------------------------

iEdObjectData* cTypeSubMesh::GetDataForName(const tWString& asName, const tEdObjDataVec& avBoneData)
{
	for(size_t i=0; i<avBoneData.size(); ++i)
	{
		iEdObjectData* pData = avBoneData[i];

		if(pData->GetName()==asName)
			return pData;
	}

	return NULL;
}

//---------------------------------------------------------------------------

cEdObjSubMeshData::cEdObjSubMeshData(iEdObjectType* apType) : iEdScnObjData(apType)
{
}

//---------------------------------------------------------------------------

iEdObject* cEdObjSubMeshData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjSubMesh,(this));
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEdObjSubMesh::cEdObjSubMesh(iEdObjectData* apData) : iEdScnObject(apData)
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

		mbRotateable = bHasNoSkeleton;
		mbTranslateable = bHasNoSkeleton;
		mScaleType = bHasNoSkeleton?eScaleType_Normal:eScaleType_None;
	}
	*/
}

cEdObjSubMesh::~cEdObjSubMesh()
{
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

bool cEdObjSubMesh::SetProperty(int alPropID, const int alX)
{
	switch(alPropID)
	{
	case eSubMeshInt_SubMeshID:
		mlSubMeshID = alX;		break;
	default:
		return iEdScnObject::SetProperty(alPropID, alX);
	}

	return true;
}

bool cEdObjSubMesh::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eSubMeshStr_Material:
		SetMaterialFile(asX);	break;
	default:
		return iEdScnObject::SetProperty(alPropID, asX);
	}

	return true;
}

bool cEdObjSubMesh::GetProperty(int alPropID, int& alX)
{
	switch(alPropID)
	{
	case eSubMeshInt_SubMeshID:
		alX = GetSubMeshID();	break;
	default:
		return iEdScnObject::GetProperty(alPropID, alX);
	}

	return true;
}

bool cEdObjSubMesh::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eSubMeshStr_Material:
		asX = GetMaterialFile();	break;
	default:
		return iEdScnObject::GetProperty(alPropID, asX);
	}

	return true;
}


/*
bool cEdObjSubMesh::CheckRayAccurateIntersection(const cVector3f& avRayStart, const cVector3f& avRayEnd, cVector3f* apIntersection, tVector3fVec* apTriangle)
{
	return cEditorHelper::CheckRaySubMeshEntityIntersect(avRayStart, avRayEnd, mpSubMeshEntity, apIntersection, NULL, NULL, apTriangle);
}
*/

//-------------------------------------------------------------------

/*
void cEdObjSubMesh::SaveToElement(cXmlElement* apElement)
{
	apElement->SetValue("SubMesh");
	iEdScnObject::SaveToElement(apElement);

	apElement->SetAttributeInt("SubMeshID", mlSubMeshID);
}
*/
//-------------------------------------------------------------------

iEdEditPane* cEdObjSubMesh::CreateEditPane()
{
	iEdEditPane* pEditBox = hplNew(cEdEditPaneSubMesh,(this));

	return pEditBox;
}

void cEdObjSubMesh::SetMaterialFile(const tString& asMatFile)
{
	cMaterial* pMat = NULL;
	cEdLoader::LoadMaterial(asMatFile, &pMat);

	if(pMat==NULL)
		msMatFile = "";
	else
		msMatFile = asMatFile;

	if(mpEngObject) 
		static_cast<cSubMeshEntity*>(mpEngObject->GetObject())->SetCustomMaterial(pMat);
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEngineObject* cEdObjSubMesh::CreateEngineObject()
{
	return hplNew(cEngineSubMesh, (this));
}

//-------------------------------------------------------------------
