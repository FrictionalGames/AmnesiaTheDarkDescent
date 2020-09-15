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

#include "EntityWrapperBody.h"

#include "EditorBaseClasses.h"
#include "EditorWorld.h"

#include "EditorWindowEntityEditBoxBody.h"

#include "EntityWrapperBodyShape.h"

#include <algorithm>

//------------------------------------------------------------------------

cEntityWrapperTypeBody::cEntityWrapperTypeBody() : iEntityWrapperTypeAggregate(eEditorEntityType_Body, _W("Body"), "Body", "Shape", false)
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

	mvAttachableTypes.push_back(eEditorEntityType_Light);
	mvAttachableTypes.push_back(eEditorEntityType_Billboard);
	mvAttachableTypes.push_back(eEditorEntityType_Sound);
	mvAttachableTypes.push_back(eEditorEntityType_ParticleSystem);
	mvAttachableTypes.push_back(eEditorEntityType_SubMesh);
	mvAttachableTypes.push_back(eEditorEntityType_Bone);
}

iEntityWrapperData* cEntityWrapperTypeBody::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataBody,(this));
}

//---------------------------------------------------------------------------

cEntityWrapperDataBody::cEntityWrapperDataBody(iEntityWrapperType* apType) : iEntityWrapperDataAggregate(apType)
{
}

//---------------------------------------------------------------------------

void cEntityWrapperDataBody::CopyFromEntity(iEntityWrapper* apEnt)
{
	iEntityWrapperDataAggregate::CopyFromEntity(apEnt);
	cEntityWrapperBody* pBody = (cEntityWrapperBody*)apEnt;

	//const tJointWrapperVec& vJoints[] = { pBody->GetParentJoints(), pBody->GetChildJoints() };

	mvParentJointIDs.clear();
	mvChildJointIDs.clear();

	/*
	const tJointWrapperVec* vJoints[] = { &pBody->GetParentJoints(), &pBody->GetChildJoints() };
	tIntVec* vJointIDs[] = { &mvParentJointIDs, &mvChildJointIDs };

	for(int i=0;i<2; ++i)
	{
		const tJointWrapperVec& vSrcJoints = *vJoints[i];
		tIntVec* vDstJointIDs = vJointIDs[i];
		vDstJointIDs->clear();
		for(int j=0;j<(int)vSrcJoints.size();++j)
		{
			iEntityWrapperJoint* pJoint = vSrcJoints[i];
			vDstJointIDs->push_back(pJoint->GetID());
		}
	}
	*/

	
	const tJointWrapperVec& vParentJoints = pBody->GetParentJoints();
	for(int i=0;i<(int)vParentJoints.size();++i)
	{
		iEntityWrapperJoint* pJoint = vParentJoints[i];
		mvParentJointIDs.push_back(pJoint->GetID());
	}

	const tJointWrapperVec& vChildJoints = pBody->GetChildJoints();
	for(int i=0;i<(int)vChildJoints.size();++i)
	{
		iEntityWrapperJoint* pJoint = vChildJoints[i];
		mvChildJointIDs.push_back(pJoint->GetID());
	}
	
}

//---------------------------------------------------------------------------

void cEntityWrapperDataBody::CopyToEntity(iEntityWrapper* apEnt, int alCopyStep)
{
	iEntityWrapperDataAggregate::CopyToEntity(apEnt, alCopyStep);
	cEntityWrapperBody* pBody = (cEntityWrapperBody*)apEnt;
	iEditorWorld* pWorld = mpType->GetWorld();

	if(alCopyStep==ePropCopyStep_PostDeployAll)
	{
		for(int i=0;i<(int)mvParentJointIDs.size();++i)
		{
			iEntityWrapperJoint* pJoint = (iEntityWrapperJoint*) pWorld->GetEntity(mvParentJointIDs[i]);
			pJoint->SetChildBody(pBody);
		}

		for(int i=0;i<(int)mvChildJointIDs.size();++i)
		{
			iEntityWrapperJoint* pJoint = (iEntityWrapperJoint*) pWorld->GetEntity(mvChildJointIDs[i]);
			pJoint->SetParentBody(pBody);
		}
	}
}

//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataBody::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperBody,(this));
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEntityWrapperBody::cEntityWrapperBody(iEntityWrapperData* apData) : iEntityWrapperAggregate(apData)
												
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

cEntityWrapperBody::~cEntityWrapperBody()
{
	if(GetEditorWorld()->IsClearingEntities()==false)
		ClearJoints();
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEntityWrapperBody::SetProperty(int alPropID, const float& afX)
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
		return iEntityWrapper::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEntityWrapperBody::SetProperty(int alPropID, const bool& abX)
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
		return iEntityWrapper::SetProperty(alPropID, abX);
	}

	return true;
}

bool cEntityWrapperBody::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eBodyStr_Material:
		SetMaterial(asX);	break;
	default:
		return iEntityWrapper::SetProperty(alPropID, asX);
	}

	return true;
}

bool cEntityWrapperBody::GetProperty(int alPropID, float& afX)
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
		return iEntityWrapper::GetProperty(alPropID, afX);
	}

	return true;
}

bool cEntityWrapperBody::GetProperty(int alPropID, bool& abX)
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
		return iEntityWrapper::GetProperty(alPropID, abX);
	}

	return true;
}

bool cEntityWrapperBody::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eBodyStr_Material:
		asX = GetMaterial();	break;
	default:
		return iEntityWrapper::GetProperty(alPropID, asX);
	}

	return true;
}

//---------------------------------------------------------------------------

void cEntityWrapperBody::Draw(	cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions,
									iEditorEditMode* apEditMode,bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol)
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

//---------------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperBody::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = (cEditorWindowEntityEditBox*)hplNew(cEditorWindowEntityEditBoxBody, (apEditMode, this));

	return pEditBox;
}

//---------------------------------------------------------------------------

void cEntityWrapperBody::AddParentJoint(iEntityWrapperJoint* apJoint)
{
	tJointWrapperVecIt it = find(this->mvParentJoints.begin(), mvParentJoints.end(), apJoint);
	if(it==mvParentJoints.end())
		mvParentJoints.push_back(apJoint);
}

//---------------------------------------------------------------------------

void cEntityWrapperBody::AddChildJoint(iEntityWrapperJoint* apJoint)
{
	tJointWrapperVecIt it = find(this->mvChildJoints.begin(), mvChildJoints.end(), apJoint);
	if(it==mvChildJoints.end())
		mvChildJoints.push_back(apJoint);
}

//---------------------------------------------------------------------------

void cEntityWrapperBody::RemoveJoint(iEntityWrapperJoint* apJoint, bool abModifyJoint)
{
	if(apJoint==NULL)
		return;

	tJointWrapperVec* vJointsToSearchIn[] = { &mvParentJoints, &mvChildJoints };

	for(int i=0;i<2;++i)
	{
		tJointWrapperVec& vJoints = *vJointsToSearchIn[i];
		tJointWrapperVecIt it = find(vJoints.begin(), vJoints.end(), apJoint);
		if(it!=vJoints.end())
		{
			vJoints.erase(it);
			RemoveJointHelper(apJoint, abModifyJoint);
			
			break;
		}
	}
}

void cEntityWrapperBody::RemoveJointHelper(iEntityWrapperJoint* apJoint, bool abModifyJoint)
{
	if(abModifyJoint)
	{
		if(apJoint->GetParentBody()==this) apJoint->SetParentBody(NULL, false);
		if(apJoint->GetChildBody()==this) apJoint->SetChildBody(NULL, false);
	}
}

//---------------------------------------------------------------------------

void cEntityWrapperBody::ClearJoints()
{
	tJointWrapperVec vJointsToClear;
	vJointsToClear.insert(vJointsToClear.end(), mvParentJoints.begin(), mvParentJoints.end());
	vJointsToClear.insert(vJointsToClear.end(), mvChildJoints.begin(), mvChildJoints.end());

	mvParentJoints.clear();
	mvChildJoints.clear();

	for(int i=0;i<(int)vJointsToClear.size();++i)
	{
		iEntityWrapperJoint* pJoint = vJointsToClear[i];
		RemoveJointHelper(pJoint, true);
	}
}

//---------------------------------------------------------------------------

void cEntityWrapperBody::OnAddComponent(iEntityWrapper* apEntity)
{
	cEntityWrapperBodyShape* pShape = (cEntityWrapperBodyShape*)apEntity;
	pShape->SetParentBody(this);
}

//---------------------------------------------------------------------------

void cEntityWrapperBody::OnRemoveComponent(iEntityWrapper* apEntity)
{
	cEntityWrapperBodyShape* pShape = (cEntityWrapperBodyShape*)apEntity;
	pShape->SetParentBody(NULL);
}

//---------------------------------------------------------------------------

void cEntityWrapperBody::OnUpdateRelativeTransform(iEntityWrapper* apEntity, const cMatrixf& amtxRelTransform)
{
	cEntityWrapperBodyShape* pShape = (cEntityWrapperBodyShape*)apEntity;
	pShape->SetRelativeMatrix(amtxRelTransform);
}

//---------------------------------------------------------------------------


