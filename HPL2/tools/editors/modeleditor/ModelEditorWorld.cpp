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

#include "ModelEditorWorld.h"

#include "../common/EditorBaseClasses.h"
#include "../common/EntityWrapperFactory.h"

#include "../common/EntityWrapperBodyShape.h"
#include "../common/EntityWrapperBody.h"
#include "../common/EntityWrapperJointBall.h"
#include "../common/EntityWrapperJointHinge.h"
#include "../common/EntityWrapperJointScrew.h"
#include "../common/EntityWrapperJointSlider.h"
#include "../common/EntityWrapperLightBox.h"
#include "../common/EntityWrapperLightPoint.h"
#include "../common/EntityWrapperLightSpot.h"
#include "../common/EntityWrapperBillboard.h"
#include "../common/EntityWrapperParticleSystem.h"
#include "../common/EntityWrapperSound.h"
#include "../common/EntityWrapperSubMesh.h"
#include "../common/EntityWrapperBone.h"

#include "../common/EditorHelper.h"

#include "../common/EditorUserClassDefinitionManager.h"

#include <algorithm>

int cAnimationEventWrapper::mlIndices = 0;
//--------------------------------------------------------------------

cAnimationEventWrapper::cAnimationEventWrapper()
{
	mlIndex = mlIndices++;
}

void cAnimationEventWrapper::Load(cXmlElement* apElement)
{
	SetTime(apElement->GetAttributeFloat("Time"));
	SetType(apElement->GetAttributeString("Type"));
	SetValue(apElement->GetAttributeString("Value"));
}

void cAnimationEventWrapper::Save(cXmlElement* apElement)
{
	apElement->SetAttributeFloat("Time",GetTime());
	apElement->SetAttributeString("Type",GetType());
	apElement->SetAttributeString("Value",GetValue());
}

bool cAnimationEventWrapper::IsValid()
{
	if(msType=="") return false;
	if(msValue=="" && msType!="Step") return false;

	return true;
}

//--------------------------------------------------------------------

cAnimationWrapper::cAnimationWrapper()
{
	mfSpeed = 1;
}

//--------------------------------------------------------------------

bool cAnimationWrapper::IsValid()
{
	if(msFile!="" && msName!="")
	{
		for(int i=0;i<(int)mvEvents.size();++i)
		{
			cAnimationEventWrapper& event = mvEvents[i];
			if(event.IsValid()==false)
				return false;
		}
	}
	else
		return false;
	
	return true;
}

void cAnimationWrapper::Load(cXmlElement* apElement)
{
	SetName(apElement->GetAttributeString("Name"));
	SetFile(apElement->GetAttributeString("File"));
	SetSpeed(apElement->GetAttributeFloat("Speed"));
	SetSpecialEventTime(apElement->GetAttributeFloat("SpecialEventTime"));

	cXmlNodeListIterator events = apElement->GetChildIterator();
	while(events.HasNext())
	{
		cXmlElement* pXmlEvent = events.Next()->ToElement();
		cAnimationEventWrapper event;
		event.Load(pXmlEvent);
		mvEvents.push_back(event);
	}
}

void cAnimationWrapper::Save(cXmlElement* apElement)
{
	apElement->SetAttributeString("Name",GetName());
	apElement->SetAttributeString("File",GetFile());
	apElement->SetAttributeFloat("Speed",GetSpeed());
	apElement->SetAttributeFloat("SpecialEventTime",GetSpecialEventTime());
	for(int i=0;i<(int)mvEvents.size();++i)
	{
		cAnimationEventWrapper* pEvent = &mvEvents[i];
		cXmlElement* pXmlEvent = apElement->CreateChildElement("Event");
		pEvent->Save(pXmlEvent);
	}
}

//--------------------------------------------------------------------

int cAnimationWrapper::AddEvent()
{
	mvEvents.push_back(cAnimationEventWrapper());
	return (int)mvEvents.size();
}

void cAnimationWrapper::RemoveEvent(int alIdx)
{
	cAnimationEventWrapper* event = &mvEvents[alIdx];
	std::vector<cAnimationEventWrapper> vTemp;
	for(int i=0;i<(int)mvEvents.size();++i)
		if(mvEvents[i].GetIndex()!=event->GetIndex())
			vTemp.push_back(mvEvents[i]);

	mvEvents = vTemp;
}
//--------------------------------------------------------------------

void cAnimationWrapper::ClearEvents()
{
	mvEvents.clear();
}


//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

cModelEditorWorld::cModelEditorWorld(iEditorBase* apEditor) : iEditorWorld(apEditor, "Entity")
{
	AddEntityCategory("Entities", -1);
	AddEntityCategory("Mesh", eEditorEntityType_SubMesh);
	AddEntityCategory("Bones", eEditorEntityType_Bone);
	AddEntityCategory("Shapes", eEditorEntityType_BodyShape);
	AddEntityCategory("Bodies", eEditorEntityType_Body);
	AddEntityCategory("Joints", eEditorEntityType_Joint);
	AddEntityCategory("Animations", -1);
	
	mpTypeSubMesh = hplNew(cEntityWrapperTypeSubMesh,());
	mpTypeBone	  = hplNew(cEntityWrapperTypeBone,());
	AddEntityType(mpTypeSubMesh);
	AddEntityType(mpTypeBone);

	mpClass = NULL;
}

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

void cModelEditorWorld::Reset()
{
	mpTypeSubMesh->ClearMesh();

	mvAnimations.clear();
	
	/////////////////////////////////////////
	// Reset user defined variable values
	mmapTempValues.clear();
	cEditorUserClassDefinition* pDef = mpEditor->GetClassDefinitionManager()->GetDefinition(eUserClassDefinition_Entity);
	SetType(pDef->GetType(0)->GetSubType(0), false);

	iEditorWorld::Reset();
}

//------------------------------------------------------------------

cMeshEntity* cModelEditorWorld::GetMesh()
{
	return mpTypeSubMesh->GetMesh();
}

tString cModelEditorWorld::GetMeshFilename()
{
	return mpTypeSubMesh->GetMeshFilename();
}

void cModelEditorWorld::SetMeshFromElement(cXmlElement* apMeshElement, cXmlElement* apBonesElement)
{
	tEntityDataVec vSubMeshData, vBoneData;

	////////////////////////////////////////////////////////////
	// Get submesh data from the .ent file
	cXmlNodeListIterator itSubMeshes = apMeshElement->GetChildIterator();
	while(itSubMeshes.HasNext())
	{
		cXmlElement* pSubMesh = itSubMeshes.Next()->ToElement();

		iEntityWrapperData* pData = mpTypeSubMesh->CreateData();
		pData->Load(pSubMesh);

		vSubMeshData.push_back(pData);
	}

	////////////////////////////////////////////////////////////
	// Get bone data from the .ent file
	if(apBonesElement)
	{
		cXmlNodeListIterator itBones = apBonesElement->GetChildIterator();
		while(itBones.HasNext())
		{
			cXmlElement* pBone = itBones.Next()->ToElement();

			iEntityWrapperData* pData = mpTypeBone->CreateData();
			pData->Load(pBone);

			vBoneData.push_back(pData);
		}
	}

	////////////////////////////////////////////////////////////
	// Load mesh using the submesh and bone data loaded above
	// A comparison will take place and updates to data will come if necessary
	mpTypeSubMesh->SetMesh(apMeshElement->GetAttributeString("Filename"), true, 
							vSubMeshData, tIntList(), vBoneData, tIntList());	
}

//------------------------------------------------------------------

void cModelEditorWorld::SetAnimations(const tAnimWrapperVec& avAnims)
{
	IncModifications();
	mvAnimations = avAnims;
}

//------------------------------------------------------------------

void cModelEditorWorld::SetType(cEditorUserClassSubType* apType, bool abKeepValues)
{
	if(mpClass && mpClass->GetClass()==apType)
		return;

	if(mpClass)
	{
		if(abKeepValues)
			mpClass->SaveValuesToMap(mmapTempValues);

		hplDelete(mpClass);
		mpClass = NULL;
	}

	if(apType)
	{
		mpClass = apType->CreateInstance(eEditorVarCategory_Type);
		if(abKeepValues)
			mpClass->LoadValuesFromMap(mmapTempValues);
	}
}

//------------------------------------------------------------------

void cModelEditorWorld::LoadWorldData(cXmlElement* apWorldDataElement)
{
	iEditorWorld::LoadWorldData(apWorldDataElement);

	cXmlElement* pXmlVariables = apWorldDataElement->GetParent()->GetFirstElement("UserDefinedVariables");
	if(pXmlVariables)
	{
		cEditorUserClassDefinition* pDef = mpEditor->GetClassDefinitionManager()->GetDefinition(eUserClassDefinition_Entity);

		tString sType = pXmlVariables->GetAttributeString("EntityType");
		tString sSubType = pXmlVariables->GetAttributeString("EntitySubType");

		bool bValid = false;
		cEditorUserClassType* pBaseType = pDef->GetType(sType);
		if (pBaseType)
		{
			bValid = true;
			cEditorUserClassSubType* pType = pBaseType->GetSubType(sSubType);
			SetType(pType);

			if(mpClass) mpClass->Load(pXmlVariables);
			else		bValid = false;
		}

		if (!bValid)
		{
			tString sMessage = "Model has invalid type : " + sType + " - " + sSubType;
			Error("%s\n", sMessage.c_str());
			mpEditor->ShowMessageBox(_W("Error"), cString::To16Char(sMessage), _W("OK"), _W(""), NULL, NULL);
		}
	}
}

//------------------------------------------------------------------

cXmlElement* cModelEditorWorld::GetWorldDataElement(iXmlDocument* apXmlDoc)
{
	return apXmlDoc->GetFirstElement("ModelData");
}

//------------------------------------------------------------------

cXmlElement* cModelEditorWorld::GetWorldObjectsElement(cXmlElement* apWorldDataElement)
{
	return apWorldDataElement;
}

//------------------------------------------------------------------

bool cModelEditorWorld::CustomCategorySaver(cXmlElement* apWorldObjectsElement)
{
	cXmlElement* pMeshElem = apWorldObjectsElement->GetFirstElement("Mesh");
	pMeshElem->SetAttributeString("Filename", 
		cString::To8Char(mpEditor->GetPathRelToWD(cString::To16Char(mpTypeSubMesh->GetMeshFilename()))));
	
	cXmlElement* pAnimElem = apWorldObjectsElement->GetFirstElement("Animations");
	{
		for(int i=0;i<(int)mvAnimations.size();++i)
		{
			cAnimationWrapper& pAnim = mvAnimations[i];
			cXmlElement* pXmlAnim = pAnimElem->CreateChildElement("Animation");
			pAnim.Save(pXmlAnim);
		}
	}
	

	return true;
}

bool cModelEditorWorld::CustomCategoryLoader(cXmlElement* apWorldObjectsElement, cXmlElement* apCategoryElement)
{
	if(apCategoryElement==NULL)
		return false;

	const tString& sCatName = apCategoryElement->GetValue();
	if(sCatName=="Mesh")
	{
		SetMeshFromElement(apCategoryElement, apWorldObjectsElement->GetFirstElement("Bones"));

		return true;
	}
	else if(sCatName=="Bones")
	{
		return true;
	}
	else if(sCatName=="Animations")
	{
		cXmlNodeListIterator animations = apCategoryElement->GetChildIterator();
		while(animations.HasNext())
		{
			cXmlElement* pElement = animations.Next()->ToElement();
			cAnimationWrapper animation;
			animation.Load(pElement);
			mvAnimations.push_back(animation);
		}

		return true;
	}

	return false;
}

//------------------------------------------------------------------

cXmlElement* cModelEditorWorld::CreateWorldDataElement(iXmlDocument* apXmlDoc)
{
	return apXmlDoc->CreateChildElement("ModelData");
}

//------------------------------------------------------------------

cXmlElement* cModelEditorWorld::CreateWorldObjectsElement(cXmlElement* apWorldDataElement)
{
	return apWorldDataElement;
}

//------------------------------------------------------------------

void cModelEditorWorld::SaveWorldData(cXmlElement* apWorldDataElement)
{
	cEditorUserClassSubType* pSubType = (cEditorUserClassSubType*)mpClass->GetClass();
	cEditorUserClassType* pType = pSubType->GetParent();

	cXmlElement* pXmlUserVars = apWorldDataElement->GetParent()->CreateChildElement("UserDefinedVariables");
	pXmlUserVars->SetAttributeString("EntityType", pType->GetName());
	pXmlUserVars->SetAttributeString("EntitySubType", pSubType->GetName());

	mpClass->Save(pXmlUserVars);
}

//------------------------------------------------------------------
