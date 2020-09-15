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

#include "../ModelEditor/EdObjBody.h"

//#include "EditorBaseClasses.h"
//#include "EditorWorld.h"

#include "../ModelEditor/EdEditPaneBody.h"
#include "../ModelEditor/EdObjShape.h"

#include "../ModelEditor/EdObjBodyActions.h"

#include <algorithm>

//------------------------------------------------------------------------

cTypeBody::cTypeBody() : iBaseAggregateType(_W("Body"), "Body", "Shape")
{
	AddString(eBodyStr_Material, "Material");
	AddFloat(eBodyFloat_Mass, "Mass");
	AddFloat(eBodyFloat_AngularDamping, "AngularDamping");
	AddFloat(eBodyFloat_LinearDamping, "LinearDamping");
	AddFloat(eBodyFloat_MaxAngularSpeed, "MaxAngularSpeed");
	AddFloat(eBodyFloat_MaxLinearSpeed, "MaxLinearSpeed");
	AddFloat(eBodyFloat_BuoyancyDensityMul, "BuoyancyDensityMul", 1);

	AddBool(eBodyBool_BlockSound, "BlocksSound", false);
	AddBool(eBodyBool_ContinuousCollision, "ContinuousCollision", true);
	AddBool(eBodyBool_CanAttachCharacter, "CanAttachCharacter", true);
	AddBool(eBodyBool_PushedByCharacterGravity, "PushedByCharacterGravity", true);
	AddBool(eBodyBool_CollideCharacter, "CollideCharacter", true);
	AddBool(eBodyBool_CollideNonCharacter, "CollideNonCharacter", true);
	AddBool(eBodyBool_Volatile, "Volatile", false);
	AddBool(eBodyBool_UseSurfaceEffects, "UseSurfaceEffects", true);
	AddBool(eBodyBool_HasGravity, "HasGravity", true);

	
	AddAttachableType(_W("Light"));
	AddAttachableType(_W("Billboard"));
	AddAttachableType(_W("Sound"));
	AddAttachableType(_W("Particle System"));
	AddAttachableType(_W("SubMesh"));
	AddAttachableType(_W("Bone"));

	mbScalable = false;
}

iEdObjectData* cTypeBody::CreateTypeSpecificData()
{
	return hplNew(cEdObjBodyData,(this));
}

//---------------------------------------------------------------------------

cEdObjBodyData::cEdObjBodyData(iEdObjectType* apType) : iBaseAggregateObjData(apType)
{
}

//---------------------------------------------------------------------------

void cEdObjBodyData::CopyFromObject(iEdObject* apObj)
{
	iBaseAggregateObjData::CopyFromObject(apObj);
	cEdObjBody* pBody = (cEdObjBody*)apObj;

	mvParentJointIDs.clear();
	mvChildJointIDs.clear();
	
	const tEdJointVec& vParentJoints = pBody->GetParentJoints();
	for(size_t i=0;i<vParentJoints.size();++i)
	{
		iEdObjJoint* pJoint = vParentJoints[i];
		mvParentJointIDs.push_back(pJoint->GetID());
	}

	const tEdJointVec& vChildJoints = pBody->GetChildJoints();
	for(size_t i=0;i<vChildJoints.size();++i)
	{
		iEdObjJoint* pJoint = vChildJoints[i];
		mvChildJointIDs.push_back(pJoint->GetID());
	}
	
}

//---------------------------------------------------------------------------

void cEdObjBodyData::CopyToObject(iEdObject* apObj, ePropStep aStep)
{
	iBaseAggregateObjData::CopyToObject(apObj, aStep);
	cEdObjBody* pBody = static_cast<cEdObjBody*>(apObj);
	iEdWorld* pWorld = mpType->GetWorld();

	if(aStep==ePropStep_PostDeployAll)
	{
		for(size_t i=0;i<mvParentJointIDs.size();++i)
		{
			iEdObjJoint* pJoint = static_cast<iEdObjJoint*> (pWorld->GetObject(mvParentJointIDs[i]));
			pJoint->SetChildBody(pBody);
		}

		for(size_t i=0;i<mvChildJointIDs.size();++i)
		{
			iEdObjJoint* pJoint = static_cast<iEdObjJoint*> (pWorld->GetObject(mvChildJointIDs[i]));
			pJoint->SetParentBody(pBody);
		}
	}
}

//---------------------------------------------------------------------------

iEdObject* cEdObjBodyData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjBody,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjBody::cEdObjBody(iEdObjectData* apData) : iBaseAggregateObject(apData)
												
{
	mfAngularDamping = 0;
	mfLinearDamping = 0;
	mfMass = 0;
	mfMaxAngularSpeed = 0;
	mfMaxLinearSpeed = 0;
	mfBuoyancyDensityMul = 1;

	mbBlocksSound = false;
	mbContinuousCollision = true;
	mbCanAttachCharacter = true;
	mbPushedByCharacterGravity = true;
	mbCollideCharacter = true;
	mbCollideNonCharacter = true;
	mbVolatile = false;
	mbUseSurfaceEffects = true;
	mbHasGravity = true;
}

cEdObjBody::~cEdObjBody()
{
	if(GetWorld()->IsClearing()==false)
		ClearJoints();
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEdObjBody::SetProperty(int alPropID, const float afX)
{
	switch(alPropID)
	{
	case eBodyFloat_Mass:
		SetMass(afX);			break;
	case eBodyFloat_AngularDamping:
		SetAngularDamping(afX);	break;
	case eBodyFloat_LinearDamping:
		SetLinearDamping(afX);	break;
	case eBodyFloat_MaxAngularSpeed:
		SetMaxAngularSpeed(afX);	break;
	case eBodyFloat_MaxLinearSpeed:
		SetMaxLinearSpeed(afX);		break;
	case eBodyFloat_BuoyancyDensityMul:
		SetBuoyancyDensityMul(afX);	break;
	default:
		return iEdScnObject::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjBody::SetProperty(int alPropID, const bool abX)
{
	switch(alPropID)
	{
	case eBodyBool_BlockSound:
		SetBlocksSound(abX);			break;
	case eBodyBool_ContinuousCollision:
		SetContinuousCollision(abX);	break;
	case eBodyBool_CanAttachCharacter:
		SetCanAttachCharacter(abX);	break;
	case eBodyBool_PushedByCharacterGravity:
		SetPushedByCharacterGravity(abX);	break;
	case eBodyBool_CollideCharacter:
		SetCollideCharacter(abX);		break;
	case eBodyBool_CollideNonCharacter:
		SetCollideNonCharacter(abX);	break;
	case eBodyBool_Volatile:
		SetVolatile(abX);	break;
	case eBodyBool_UseSurfaceEffects:
		SetUseSurfaceEffects(abX);	break;
	case eBodyBool_HasGravity:
		SetHasGravity(abX);	break;
	default:
		return iEdScnObject::SetProperty(alPropID, abX);
	}

	return true;
}

bool cEdObjBody::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eBodyStr_Material:
		SetMaterial(asX);	break;
	default:
		return iEdScnObject::SetProperty(alPropID, asX);
	}

	return true;
}

bool cEdObjBody::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eBodyFloat_Mass:
		afX = GetMass();			break;
	case eBodyFloat_AngularDamping:
		afX = GetAngularDamping();	break;
	case eBodyFloat_LinearDamping:
		afX = GetLinearDamping();	break;
	case eBodyFloat_MaxAngularSpeed:
		afX = GetMaxAngularSpeed();	break;
	case eBodyFloat_MaxLinearSpeed:
		afX = GetMaxLinearSpeed();		break;
	case eBodyFloat_BuoyancyDensityMul:
		afX = GetBuoyancyDensityMul();	break;
	default:
		return iEdScnObject::GetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjBody::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case eBodyBool_BlockSound:
		abX = BlocksSound();			break;
	case eBodyBool_ContinuousCollision:
		abX = HasContinuousCollision();	break;
	case eBodyBool_CanAttachCharacter:
		abX = CanAttachCharacter();	break;
	case eBodyBool_PushedByCharacterGravity:
		abX = IsPushedByCharacterGravity();	break;
	case eBodyBool_CollideCharacter:
		abX = CollidesCharacter();		break;
	case eBodyBool_CollideNonCharacter:
		abX = CollidesNonCharacter();	break;
	case eBodyBool_Volatile:
		abX = IsVolatile();	break;
	case eBodyBool_UseSurfaceEffects:
		abX = UsesSurfaceEffects();	break;
	case eBodyBool_HasGravity:
		abX = HasGravity();	break;
	default:
		return iEdScnObject::GetProperty(alPropID, abX);
	}

	return true;
}

bool cEdObjBody::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eBodyStr_Material:
		asX = GetMaterial();	break;
	default:
		return iEdScnObject::GetProperty(alPropID, asX);
	}

	return true;
}

//---------------------------------------------------------------------------

void cEdObjBody::OnDraw(const cModuleDrawData& aData)
{
	if(aData.mStep==eEdDrawStep_PostTrans) return;

	if(mbSelected)
	{
		DrawLinks(aData);

		int lParentSize = mvParentJoints.size();
		int lChildSize = mvChildJoints.size();
		int lMaxSize = cMath::Max(lParentSize, lChildSize);
		for(int i=0; i<lMaxSize; ++i)
		{
			if(i<lParentSize) aData.mpFunctions->GetLowLevelGfx()->DrawLine(GetTranslation(), mvParentJoints[i]->GetTranslation(), cColor(1,0,0,1));
			if(i<lChildSize) aData.mpFunctions->GetLowLevelGfx()->DrawLine(GetTranslation(), mvChildJoints[i]->GetTranslation(), cColor(0,1,0,1));
		}
	}
}

/*
void cEdObjBody::Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions,
									iEdEditMode* apEditMode,bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol)
{
	iEntityWrapper::Draw(apViewport, apFunctions, apEditMode, abIsSelected);
	if(abIsSelected)
	{
		apFunctions->SetBlendMode(eMaterialBlendMode_None);
		apFunctions->SetProgram(NULL);
		apFunctions->SetTextureRange(NULL,0);
		apFunctions->SetDepthTest(true);
		apFunctions->SetDepthWrite(true);

		apFunctions->SetMatrix(NULL);

		tEntityWrapperListIt itShapes = mlstComponents.begin();
		for(;itShapes!=mlstComponents.end();++itShapes)
		{
			iEntityWrapper* pShape = *itShapes;
			//apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pShape->GetPickBV()->GetMin(), pShape->GetPickBV()->GetMax(), cColor(1));
			pShape->Draw(apViewport, apFunctions, apEditMode, true, cColor(1,1,1,1));
		}
		
		apFunctions->SetMatrix(NULL);
		apFunctions->GetLowLevelGfx()->DrawSphere(mvPosition, 0.2f, cColor(1,0,0,1));
	}
}
*/

//---------------------------------------------------------------------------

/*
void cEdObjBody::OnDraw(const cModuleDrawData& aData)
{

}
*/

//---------------------------------------------------------------------------

iEdEditPane* cEdObjBody::CreateEditPane()
{
	return hplNew(cEdEditPaneBody,(this));
}

//---------------------------------------------------------------------------

void cEdObjBody::AddParentJoint(iEdObjJoint* apJoint)
{
	tEdJointVecIt it = find(this->mvParentJoints.begin(), mvParentJoints.end(), apJoint);
	if(it==mvParentJoints.end())
		mvParentJoints.push_back(apJoint);
}

//---------------------------------------------------------------------------

void cEdObjBody::AddChildJoint(iEdObjJoint* apJoint)
{
	tEdJointVecIt it = find(mvChildJoints.begin(), mvChildJoints.end(), apJoint);
	if(it==mvChildJoints.end())
		mvChildJoints.push_back(apJoint);
}

//---------------------------------------------------------------------------

void cEdObjBody::RemoveJoint(iEdObjJoint* apJoint, bool abModifyJoint)
{
	if(apJoint==NULL)
		return;

	tEdJointVec* vJointsToSearchIn[] = { &mvParentJoints, &mvChildJoints };

	for(int i=0;i<2;++i)
	{
		tEdJointVec& vJoints = *vJointsToSearchIn[i];
		tEdJointVecIt it = find(vJoints.begin(), vJoints.end(), apJoint);
		if(it!=vJoints.end())
		{
			vJoints.erase(it);
			RemoveJointHelper(apJoint, abModifyJoint);
			
			break;
		}
	}
}

void cEdObjBody::RemoveJointHelper(iEdObjJoint* apJoint, bool abModifyJoint)
{
	if(abModifyJoint)
	{
		if(apJoint->GetParentBody()==this) apJoint->SetParentBody(NULL, false);
		if(apJoint->GetChildBody()==this) apJoint->SetChildBody(NULL, false);
	}
}

//---------------------------------------------------------------------------

void cEdObjBody::ClearJoints()
{
	tEdJointVec vJointsToClear;
	vJointsToClear.insert(vJointsToClear.end(), mvParentJoints.begin(), mvParentJoints.end());
	vJointsToClear.insert(vJointsToClear.end(), mvChildJoints.begin(), mvChildJoints.end());

	mvParentJoints.clear();
	mvChildJoints.clear();

	for(size_t i=0;i<vJointsToClear.size();++i)
	{
		iEdObjJoint* pJoint = vJointsToClear[i];
		RemoveJointHelper(pJoint, true);
	}
}

//---------------------------------------------------------------------------

iEdAction* cEdObjBody::CreateAddComponentsAction(const tScnObjList& alstObjects)
{
	return hplNew(cEdActionBodyAddShapes,(this, alstObjects));
}

iEdAction* cEdObjBody::CreateRemoveComponentsAction(const tScnObjList& alstObjects)
{
	return hplNew(cEdActionBodyRemoveShapes,(alstObjects));
}

//---------------------------------------------------------------------------

void cEdObjBody::AddToOutlineSpecific(cWidgetListBox* apList, int alLevel, tScnObjList& alstAddedChildren)
{
	tScnObjList::const_iterator it = mlstComponents.begin();
	for(;it!=mlstComponents.end();++it)
	{
		iEdScnObject* pShape = *it;
		pShape->AddToOutline(apList, alLevel+1, alstAddedChildren); 
	}
}

//---------------------------------------------------------------------------

void cEdObjBody::DrawObject(const cModuleDrawData& aData, bool abHighlight, const cColor& aHighlightCol)
{
	tScnObjList::const_iterator it = mlstComponents.begin();
	for(;it!=mlstComponents.end();++it)
	{
		iEdScnObject* pShape = *it;
		pShape->DrawObject(aData, abHighlight, aHighlightCol);
	}
}

//---------------------------------------------------------------------------

void cEdObjBody::OnAddComponent(iEdScnObject* apObject)
{
	iEdObjShape* pShape = static_cast<iEdObjShape*>(apObject);
	pShape->SetParentBody(this);
}

//---------------------------------------------------------------------------

void cEdObjBody::OnRemoveComponent(iEdScnObject* apObject)
{
	iEdObjShape* pShape = static_cast<iEdObjShape*>(apObject);
	pShape->SetParentBody(NULL);
}

//---------------------------------------------------------------------------

void cEdObjBody::UpdateComponents()
{
	mbUpdatingComponents = true;

	mlstRelWorldMatrices.clear();

	cVector3f vMax = -999999999.9f;
	cVector3f vMin = 999999999.9f;

	tScnObjListIt it = mlstComponents.begin();
	for(;it!=mlstComponents.end(); ++it)
	{
		iEdScnObject* pObj = *it;

		cMath::ExpandAABB(vMin, vMax, pObj->GetTranslation(), pObj->GetTranslation());
	}

	SetTranslation((vMin+vMax)*0.5f);

	mmtxInvWorldMatrix	= cMath::MatrixInverse(GetWorldMatrix());

	it = mlstComponents.begin();
	for(;it!=mlstComponents.end(); ++it)
	{
		iEdObjShape* pObj = static_cast<iEdObjShape*>(*it);
		
		cMatrixf mtxRelMatrix = pObj->GetRelativeWorldMatrix(mmtxInvWorldMatrix);

		mlstRelWorldMatrices.push_back(mtxRelMatrix);
		pObj->SetRelativeMatrix(mlstRelWorldMatrices.back());
	}

	mbUpdatingComponents = false;
}

