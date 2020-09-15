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

#include "EntityWrapperBone.h"
#include "EditorWorld.h"
#include "EditorEditMode.h"
#include "EditorEditModeSelect.h"
#include "EditorHelper.h"
#include "EditorWindowViewport.h"
#include "EngineEntity.h"

#include "EditorWindowEntityEditBoxBone.h"

#include <algorithm>

//----------------------------------------------------------------------

cEntityWrapperTypeBone::cEntityWrapperTypeBone() : iEntityWrapperType(eEditorEntityType_Bone, _W("Bone"), "Bone")
{
	AddInt(eBoneInt_ParentBoneID, "ParentBoneID", -1, ePropCopyStep_PostDeployAll, false);

	GetPropBool(eObjBool_Active)->SetSaved(false);
	GetPropVec3f(eObjVec3f_Position)->SetSaved(false);
	GetPropVec3f(eObjVec3f_Rotation)->SetSaved(false);
	GetPropVec3f(eObjVec3f_Scale)->SetSaved(false);

	GetPropString(eObjStr_Tag)->SetSaved(false);

	mbCanTranslate = false;
	mbCanRotate = false;
	mScaleType = eScaleType_None;
	mbRenameable = false;
	mbDeletable = false;
	mbCloneable = false;

	mvAttachableTypes.push_back(eEditorEntityType_Light);
	mvAttachableTypes.push_back(eEditorEntityType_Billboard);
	mvAttachableTypes.push_back(eEditorEntityType_Sound);
	mvAttachableTypes.push_back(eEditorEntityType_ParticleSystem);
}

tIntList cEntityWrapperTypeBone::GetBoneIDs()
{
	tEntityWrapperList lstBones;
	lstBones.insert(lstBones.end(), mvBones.begin(), mvBones.end());

	tIntList lstIDs;
	cEditorHelper::GetIDsFromEntityList(lstBones, lstIDs);

	return lstIDs;
}

void cEntityWrapperTypeBone::AddBone(cEntityWrapperBone* apBone)
{
	if(apBone) mvBones.push_back(apBone);
}

void cEntityWrapperTypeBone::RemoveBone(cEntityWrapperBone* apBone)
{
	tBoneWrapperVecIt it = find(mvBones.begin(), mvBones.end(), apBone);
	mvBones.erase(it);
}

void cEntityWrapperTypeBone::ClearBones(bool abRemoveFromWorld)
{
	for(int i=0;i<(int)mvBones.size();++i)
		mpWorld->DestroyEntityWrapper(mvBones[i], abRemoveFromWorld);

	mvBones.clear();
}


iEntityWrapperData* cEntityWrapperTypeBone::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataBone,(this));
}


cEntityWrapperDataBone::cEntityWrapperDataBone(iEntityWrapperType* apType) : iEntityWrapperData(apType)
{
}

//----------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataBone::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperBone,(this));
}

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

cEntityWrapperBone::cEntityWrapperBone(iEntityWrapperData* apData) : iEntityWrapper(apData)
{
	mpParentBone = NULL;
	mpVBBone = NULL;
}

cEntityWrapperBone::~cEntityWrapperBone()
{
	if(mpVBBone)
		hplDelete(mpVBBone);
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

bool cEntityWrapperBone::SetProperty(int alPropID, const int& alX)
{
	switch(alPropID)
	{
	case eBoneInt_ParentBoneID:
		{
			cEntityWrapperBone* pEnt = (cEntityWrapperBone*)GetEditorWorld()->GetEntity(alX);
			SetParentBone(pEnt);
			break;
		}
	default:
		return iEntityWrapper::SetProperty(alPropID, alX);
	}

	return true;
}

bool cEntityWrapperBone::GetProperty(int alPropID, int& alX)
{
	switch(alPropID)
	{
	case eBoneInt_ParentBoneID:
		{
			int lID = -1;
			if(mpParentBone) lID = mpParentBone->GetID();
			alX = lID;
			break;
		}
	default:
		return iEntityWrapper::GetProperty(alPropID, alX);
	}
	
	return true;
}

//----------------------------------------------------------------------

bool cEntityWrapperBone::CheckRayIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT)
{
	bool bIntersect = false;
	float fMinT = 9999.0f;
	cVector3f vMinPos = 9999.0f;

	const cVector3f& vStart = apViewport->GetUnprojectedStart();
	const cVector3f& vEnd = apViewport->GetUnprojectedEnd();

	////////////////////////////////////
	// Check joint
	{
		float fT1, fT2;
		cVector3f vPos1, vPos2;
		if(cMath::CheckSphereLineIntersection(mvPosition, 0.01f, vStart, vEnd, &fT1, &vPos1, &fT2, &vPos2))
		{
			bIntersect=true;
			if(fT1<fMinT)
			{
				fMinT = fT1;
				vMinPos = vPos1;
			}
			if(fT2<fMinT)
			{
				fMinT = fT2;
				vMinPos = vPos2;
			}
		}
	}

	////////////////////////////////////
	// Check Links to children
	for(int i=0;i<(int)mvChildBones.size();++i)
	{
		float fT;
		cVector3f vPos;
		cEntityWrapperBone* pBone = mvChildBones[i];
		if(cMath::CheckLineTriVertexBufferIntersection(	vStart, vEnd, cMatrixf::Identity, pBone->GetBoneVB(), &vPos,
														&fT, NULL))
		{
			bIntersect=true;
			if(fT<fMinT)
			{
				fMinT = fT;
				vMinPos = vPos;
			}
		}
	}

	if(apPos)
		*apPos = vMinPos;
	if(apT)
		*apT = fMinT;

	return bIntersect;
}

//----------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperBone::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	return hplNew(cEditorWindowEntityEditBoxBone,(apEditMode, this));
}

//----------------------------------------------------------------------

void cEntityWrapperBone::SetParentBone(cEntityWrapperBone* apBone)
{
	if(mpParentBone!=NULL && mpParentBone==apBone)
		return;

	if(mpParentBone)
		mpParentBone->RemoveChildBone(this);

	mpParentBone = apBone;

	if(mpParentBone)
		mpParentBone->AddChildBone(this);

	CreateLinkToParent();
}

//----------------------------------------------------------------------

void cEntityWrapperBone::AddChildBone(cEntityWrapperBone* apBone)
{
	if(apBone==NULL)
		return;

	for(int i=0;i<(int)mvChildBones.size();++i)
	{
		if(apBone==mvChildBones[i])
			return;
	}

	mvChildBones.push_back(apBone);
}

//----------------------------------------------------------------------

void cEntityWrapperBone::RemoveChildBone(cEntityWrapperBone* apBone)
{
	if(apBone==NULL)
		return;

	std::vector<cEntityWrapperBone*>::iterator it = find(mvChildBones.begin(), mvChildBones.end(), apBone);
	if(it!=mvChildBones.end())
		mvChildBones.erase(it);
}

//----------------------------------------------------------------------

void cEntityWrapperBone::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, 
							  iEditorEditMode* apEditMode, bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol)
{
	iEntityWrapper::Draw(apViewport, apFunctions, apEditMode, abIsSelected);
	cColor colJoint = abIsSelected?cColor(0,1,0,1):cColor(1,0,0,1);

	apFunctions->SetDepthTest(false);
	apFunctions->SetDepthWrite(false);
	apFunctions->GetLowLevelGfx()->DrawSphere(mvPosition, 0.01f, colJoint);
	DrawBone(apViewport, apFunctions, apEditMode, abIsSelected);
	for(int i=0;i<(int)mvChildBones.size();++i)
		mvChildBones[i]->DrawBone(apViewport, apFunctions, apEditMode, abIsSelected);
}

void cEntityWrapperBone::DrawBone(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected)
{
	if(mpVBBone)
	{
		apFunctions->SetProgram(NULL);
		apFunctions->SetTextureRange(NULL,0);
		apFunctions->SetMatrix(NULL);

		apFunctions->SetTexture(0, NULL);
			
		apFunctions->SetVertexBuffer(mpVBBone);
		apFunctions->DrawCurrent();
	}
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------


void cEntityWrapperBone::OnSetSelected(bool abX)
{
	iEntityWrapper::OnSetSelected(abX);
	for(int i=0;i<(int)this->mvChildBones.size();++i)
	{
		cEntityWrapperBone* pChildBone = mvChildBones[i];
        pChildBone->SetBoneVBColor(abX?cColor(1,1):cColor(0.3f,1));
	}
}

//----------------------------------------------------------------------


void cEntityWrapperBone::SetBoneVBColor(const cColor& aCol)
{
	if(mpVBBone==NULL)
		return;

	float *fColArray = mpVBBone->GetFloatArray(eVertexBufferElement_Color0);
	int lStride = mpVBBone->GetElementNum(eVertexBufferElement_Color0);

	for(int i=0;i<mpVBBone->GetVertexNum();++i)
	{
		fColArray[i*lStride] = aCol.r;
		fColArray[i*lStride+1] = aCol.g;
		fColArray[i*lStride+2] = aCol.b;
		fColArray[i*lStride+3] = aCol.a;
	}

	mpVBBone->UpdateData(eVertexElementFlag_Color0, false);
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

void cEntityWrapperBone::CreateLinkToParent()
{
	if(mpParentBone==NULL)
		return;

	cVector3f vDirToParent = mpParentBone->GetPosition()-mvPosition;
	float fLength = vDirToParent.Length();
	vDirToParent=vDirToParent/fLength;

	cMeshCreator* pCreator = GetEditorWorld()->GetEditor()->GetEngine()->GetGraphics()->GetMeshCreator();
    cMesh* pMesh = pCreator->CreateCone("", cVector2f(0.01f, fLength), 4, "editor_rect.mat");
	
	mpVBBone = pMesh->GetSubMesh(0)->GetVertexBuffer()->CreateCopy(eVertexBufferType_Hardware, eVertexBufferUsageType_Dynamic,
																	eVertexElementFlag_Normal | eVertexElementFlag_Position | 
																	eVertexElementFlag_Color0 | eVertexElementFlag_Texture0);

	hplDelete(pMesh);

	cVector3f vUp = mvPosition-mpParentBone->GetPosition();

	cVector3f vRight, vFwd;
	cVector3f vWorldRight = cVector3f(1,0,0);

	vRight = vWorldRight - cMath::Vector3Cross(cMath::Vector3Dot(vWorldRight, vUp), vUp);
	vRight = cMath::Vector3Project(vRight, vWorldRight);
	vFwd = cMath::Vector3Cross(vRight, vUp);

	cMath::Vector3OrthonormalizeBasis(vUp, vRight, vFwd, vUp, vRight, vFwd);

	cMatrixf mtxRotation = cMath::MatrixInverse(cMath::MatrixUnitVectors(vRight, vUp, vFwd,0)).GetTranspose();
	mpVBBone->Transform(cMath::MatrixMul(cMath::MatrixTranslate(mvPosition),cMath::MatrixMul(mtxRotation, cMath::MatrixTranslate(cVector3f(0,-fLength*0.5f,0)))));

	SetBoneVBColor(cColor(0.3f,1));
}

//----------------------------------------------------------------------

void cEntityWrapperBone::OnAddToWorld()
{
	cEntityWrapperTypeBone* pType = (cEntityWrapperTypeBone*)mpType;
	pType->AddBone(this);
}

//----------------------------------------------------------------------
