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

#include "../ModelEditor/EdObjBone.h"

#include "../Common/Editor.h"
#include "../Common/EdViewport.h"

#include "../Common/EdObjectIcon.h"
//#include "../EditorWorld.h"
//#include "EdEditMode.h"
//#include "EdEditModeSelect.h"
//#include "EditorHelper.h"
//#include "EditorWindowViewport.h"
//#include "EngineEntity.h"

#include "../ModelEditor/EdEditPaneBone.h"

#include <algorithm>

//----------------------------------------------------------------------

cTypeBone::cTypeBone() : iEdScnObjType(_W("Bone"), "Bone")
{
	AddInt(eBoneInt_ParentBoneID, "ParentBoneID", -1, ePropStep_PostDeployAll, false);

	mbTranslateable = false;
	mbRotateable = false;
	mbScalable = false;

	mbRenameable = false;
	mbDeletable = false;
	mbCloneable = false;

	AddAttachableType(_W("Light"));
	AddAttachableType(_W("Billboard"));
	AddAttachableType(_W("Sound"));
	AddAttachableType(_W("Particle System"));
}

iEdObjectData* cTypeBone::CreateTypeSpecificData()
{
	return hplNew(cEdObjBoneData,(this));
}


cEdObjBoneData::cEdObjBoneData(iEdObjectType* apType) : iEdScnObjData(apType)
{
}

//----------------------------------------------------------------------

iEdObject* cEdObjBoneData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjBone,(this));
}

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

cEdObjBone::cEdObjBone(iEdObjectData* apData) : iEdScnObject(apData)
{
	mpParentBone = NULL;
	mpVBBone = NULL;
}

cEdObjBone::~cEdObjBone()
{
	if(mpVBBone)
		hplDelete(mpVBBone);
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

bool cEdObjBone::SetProperty(int alPropID, const int alX)
{
	switch(alPropID)
	{
	case eBoneInt_ParentBoneID:
		{
			cEdObjBone* pEnt = (cEdObjBone*)GetWorld()->GetObject(alX);
			SetParentBone(pEnt);
			break;
		}
	default:
		return iEdScnObject::SetProperty(alPropID, alX);
	}

	return true;
}

bool cEdObjBone::GetProperty(int alPropID, int& alX)
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
		return iEdScnObject::GetProperty(alPropID, alX);
	}
	
	return true;
}

//----------------------------------------------------------------------

bool cEdObjBone::CheckRayIntersect(iEdViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float *apT)
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
		if(cMath::CheckSphereLineIntersection(mvTranslation, 0.01f, vStart, vEnd, &fT1, &vPos1, &fT2, &vPos2))
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
	for(size_t i=0;i<mvChildBones.size();++i)
	{
		float fT;
		cVector3f vPos;
		cEdObjBone* pBone = mvChildBones[i];
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

iEdEditPane* cEdObjBone::CreateEditPane()
{
	return hplNew(cEdEditPaneBone,(this));
}

//----------------------------------------------------------------------

void cEdObjBone::SetParentBone(cEdObjBone* apBone)
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

void cEdObjBone::AddChildBone(cEdObjBone* apBone)
{
	if(apBone==NULL)
		return;

	for(size_t i=0;i<mvChildBones.size();++i)
	{
		if(apBone==mvChildBones[i])
			return;
	}

	mvChildBones.push_back(apBone);
}

//----------------------------------------------------------------------

void cEdObjBone::RemoveChildBone(cEdObjBone* apBone)
{
	if(apBone==NULL)
		return;

	tEdBoneVecIt it = find(mvChildBones.begin(), mvChildBones.end(), apBone);
	if(it!=mvChildBones.end())
		mvChildBones.erase(it);
}

//----------------------------------------------------------------------


void cEdObjBone::OnDraw(const cModuleDrawData& aData)
{
	//iEdScnObject::Draw(apViewport, apFunctions, apEditMode, abIsSelected);
	cColor colJoint = mbSelected?cColor(0,1,0,1):cColor(1,0,0,1);
	
	if(aData.mStep==eEdDrawStep_PostSolid)
	{
		aData.mpFunctions->SetDepthTest(false);
		aData.mpFunctions->SetDepthWrite(false);
		aData.mpFunctions->GetLowLevelGfx()->DrawSphere(mvTranslation, 0.01f, colJoint);

		DrawBone(aData.mpViewport, aData.mpFunctions, cColor(0.5f, 1));
	}
	else
	{
		if(mbSelected)
		{
			aData.mpFunctions->SetDepthTest(false);
			aData.mpFunctions->SetDepthWrite(false);
			cColor col = cColor(1,1);
			for(size_t i=0;i<mvChildBones.size();++i)
				mvChildBones[i]->DrawBone(aData.mpViewport, aData.mpFunctions, col);
		}
	}
}

void cEdObjBone::DrawBone(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cColor& aCol)
{
	apViewport->DrawSolidColorVertexBuffer(mpVBBone, NULL, aCol, apFunctions, true);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

void cEdObjBone::CreateLinkToParent()
{
	if(mpParentBone==NULL)
		return;

	cVector3f vUp = GetTranslation()-mpParentBone->GetTranslation();
	cVector3f vDirToParent = vUp*-1;
	float fLength = vDirToParent.Length();
	vDirToParent=vDirToParent/fLength;

	cMeshCreator* pCreator = GetWorld()->GetEditor()->GetEngine()->GetGraphics()->GetMeshCreator();
    cMesh* pMesh = pCreator->CreateCone("", cVector2f(0.01f, fLength), 4, "editor_rect.mat");
	
	mpVBBone = pMesh->GetSubMesh(0)->GetVertexBuffer()->CreateCopy(eVertexBufferType_Hardware, eVertexBufferUsageType_Dynamic,
																	eVertexElementFlag_Normal | eVertexElementFlag_Position | 
																	eVertexElementFlag_Color0 | eVertexElementFlag_Texture0);

	hplDelete(pMesh);

	cVector3f vRight, vFwd;
	cVector3f vWorldRight = cVector3f(1,0,0);

	vRight = vWorldRight - cMath::Vector3Cross(cMath::Vector3Dot(vWorldRight, vUp), vUp);
	vRight = cMath::Vector3Project(vRight, vWorldRight);
	vFwd = cMath::Vector3Cross(vRight, vUp);

	cMath::Vector3OrthonormalizeBasis(vUp, vRight, vFwd, vUp, vRight, vFwd);

	cMatrixf mtxRotation = cMath::MatrixInverse(cMath::MatrixUnitVectors(vRight, vUp, vFwd,0)).GetTranspose();
	mpVBBone->Transform(cMath::MatrixMul(GetTranslationMatrix(),cMath::MatrixMul(mtxRotation, cMath::MatrixTranslate(cVector3f(0,-fLength*0.5f,0)))));
}


//----------------------------------------------------------------------

cObjectIcon* cEdObjBone::CreateIcon()
{
	return hplNew( cObjectIcon,(this, "Sound"));
}

//----------------------------------------------------------------------
