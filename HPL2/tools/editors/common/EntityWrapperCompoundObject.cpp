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

#include "EntityWrapperCompoundObject.h"

#include "EditorWindowEntityEditBoxCompound.h"

#include "EditorWorld.h"
#include "EditorGrid.h"
#include "EditorWindowViewport.h"

#include <algorithm>


//------------------------------------------------------------------------

cEntityWrapperTypeCompoundObject::cEntityWrapperTypeCompoundObject() : iEntityWrapperTypeAggregate(eEditorEntityType_Compound, _W("Compound"), "Compound", "Component")
{
}

//------------------------------------------------------------------------

iEntityWrapperData* cEntityWrapperTypeCompoundObject::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataCompoundObject,(this));
}


//------------------------------------------------------------------------

cEntityWrapperDataCompoundObject::cEntityWrapperDataCompoundObject(iEntityWrapperType* apType) : iEntityWrapperDataAggregate(apType)
{
}

cEntityWrapperDataCompoundObject::~cEntityWrapperDataCompoundObject()
{
}

//------------------------------------------------------------------------

/*
void cEntityWrapperDataCompoundObject::CopyFromEntity(iEntityWrapper* apEntity)
{
	iEntityWrapperData::CopyFromEntity(apEntity);

	mvEntitiesData.clear();

	iEntityWrapperDataCompoundObject* pEnt = (iEntityWrapperDataCompoundObject*)apEntity;
	tEntityWrapperList& lstEntities = (tEntityWrapperList&)pEnt->GetEntities();
	tEntityWrapperListIt it = lstEntities.begin();

	for(;it!=lstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		iEntityWrapperData* pData = pEnt->GetType()->CreateData();
		pData->CopyFromEntity(pEnt);

		mvEntitiesData.push_back(pData);
	}
}

//------------------------------------------------------------------------

void iEntityWrapperDataCompoundObject::CopyToEntity(iEntityWrapper* apEntity, int alCopyStep)
{
	iEntityWrapperData::CopyToEntity(apEntity, alCopyStep);

	if(alCopyStep==ePropCopyStep_PostDeployAll)
	{
		iEntityWrapperDataCompoundObject* pCompound = (iEntityWrapperDataCompoundObject*)apEntity;
		iEditorWorld* pWorld = mpType->GetWorld();
		for(int i=0;i<(int)mvEntitiesData.size();++i)
		{
			iEntityWrapperData* pData = mvEntitiesData[i];
			
			iEntityWrapper* pSubEntity = pWorld->GetEntity(pData->GetID());
			if(pSubEntity==NULL)
				pSubEntity = pWorld->CreateEntityWrapperFromData(pData);

			pCompound->AddEntity(pSubEntity);
		}
		pCompound->UpdateRelativeTransforms();
	}
}

//------------------------------------------------------------------------

bool iEntityWrapperDataCompoundObject::Load(cXmlElement* apElement)
{
	if(iEntityWrapperData::Load(apElement)==false)
		return false;

	iEditorWorld* pWorld = mpType->GetWorld();
	const tString& sComponentName = ((iEntityWrapperTypeCompoundObject*)mpType)->GetComponentElementName();

	cXmlNodeListIterator it = apElement->GetChildIterator();
	while(it.HasNext())
	{
		cXmlElement* pObj = it.Next()->ToElement();
		if(pObj->GetValue()==sComponentName)
			return false;
		iEntityWrapper* pEnt = pWorld->GetEntity(pObj->GetAttributeInt("ID"));
		if(pEnt)
		{
			iEntityWrapperData* pData = pEnt->CreateCopyData();
			mvEntitiesData.push_back(pData);
		}
	}

	return true;
}

//------------------------------------------------------------------------

bool iEntityWrapperDataCompoundObject::SaveSpecific(cXmlElement* apElement)
{
	if(iEntityWrapperData::SaveSpecific(apElement)==false)
		return false;

	const tString& sComponentName = ((iEntityWrapperTypeCompoundObject*)mpType)->GetComponentElementName();

	for(int i=0; i<(int)mvEntitiesData.size();++i)
		apElement->CreateChildElement(sComponentName)->SetAttributeInt("ID", mvEntitiesData[i]->GetID());

	return true;
}
*/
//------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataCompoundObject::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperCompoundObject,(this));
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEntityWrapperCompoundObject::cEntityWrapperCompoundObject(iEntityWrapperData* apData) : iEntityWrapperAggregate(apData)
{
}

//------------------------------------------------------------------------

cEntityWrapperCompoundObject::~cEntityWrapperCompoundObject()
{
}

//------------------------------------------------------------------------

/*
tIntVec iEntityWrapperCompoundObject::GetCloneIDs()
{
	tIntVec vIDs;

	iEditorWorld* pWorld = GetEditorWorld();
	for(int i=0;i<(int)mlstEntities.size()+1;++i)
		vIDs.push_back(pWorld->GetFreeID());

	return vIDs;
}

iEntityWrapper* iEntityWrapperCompoundObject::SetUpCloneData(iEntityWrapperData* apData, const tIntVec& avIDs)
{
	iEntityWrapperDataCompoundObject* pCloneData = (iEntityWrapperDataCompoundObject*)apData;

	tEntityWrapperListIt itComponents = mlstEntities.begin();
	for(int i=0;itComponents!=mlstEntities.end();++itComponents,++i)
	{
		iEntityWrapper* pComponent = *itComponents;
		iEntityWrapper* pComponentClone = pComponent->Clone(tIntVec(1,avIDs[i]));
		pComponentClone->UpdateEntity();
	}
	mbComponentsUpdated = false;

	iEditorWorld* pWorld = GetEditorWorld();
	for(int i=0;i<(int)avIDs.size()-1;++i)
	{
		iEntityWrapper* pEnt = pWorld->GetEntity(avIDs[i]);
		iEntityWrapperData* pData = pEnt->CreateCopyData();

		pCloneData->GetEntitiesData().push_back(pData);
	}
}

//------------------------------------------------------------------------

bool cEntityWrapperCompoundObject::CheckRayIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT)
{
	////////////////////////////////////////////////////
	// Check intersection against all component shapes
	bool bIntersect = false;

	//float fMinSqrDistance = 999999999.0f;
	cVector3f vIntersectionPos;
	cVector3f vMinIntersectionPos;
	float fMinT = 99999.0f;
	const cVector3f& vRayStart = apViewport->GetUnprojectedStart();

	// Iterate through components
	tEntityWrapperListIt it = mlstEntities.begin();
	for(;it!=mlstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		float fT;
		// Check intersection with each
		if(pEnt->CheckRayIntersect(apViewport, &vIntersectionPos, NULL, &fT))
		{
			bIntersect = true;
			if(fT<fMinT)
			{
				fMinT = fT;
				vMinIntersectionPos = vIntersectionPos;
			}
		}
	}

	// If there is intersection, return minimum distance intersection (if possible)
	if(bIntersect)
	{
		if(apPos) *apPos = vMinIntersectionPos;
		if(apT) *apT = fMinT;
	}

	return bIntersect;
}

//------------------------------------------------------------------------

bool cEntityWrapperCompoundObject::Check2DBoxIntersect(cEditorWindowViewport* apViewport, const cRect2l& aBox, cVector3f* apPos)
{
	////////////////////////////////////////////////////
	// Check intersection against all component shapes
	bool bIntersect = false;

	float fMinDistance = 99999999.0f;
	cVector3f vMinIntersectionPos;
	const cVector3f& vRayStart = apViewport->GetUnprojectedStart();

	// Iterate through components
	tEntityWrapperListIt it = mlstEntities.begin();
	for(;it!=mlstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;

		// Check intersection with each
		cVector3f vIntersectionPos;
		if(pEnt->Check2DBoxIntersect(apViewport, aBox, &vIntersectionPos))
		{
			bIntersect = true;

			float fDistance = cMath::Vector3DistSqr(vRayStart, vIntersectionPos);
			if(fDistance < fMinDistance)
			{
				fMinDistance = fDistance;
				vMinIntersectionPos = vIntersectionPos;
			}
		}
	}

	// If there is intersection, return minimum distance intersection (if possible)
	if(bIntersect && apPos)
		*apPos = vMinIntersectionPos;

	return bIntersect;
}
*/

//------------------------------------------------------------------------

void cEntityWrapperCompoundObject::OnAddComponent(iEntityWrapper* apEntity)
{
	apEntity->SetCompoundObject(this);
}

//------------------------------------------------------------------------

void cEntityWrapperCompoundObject::OnRemoveComponent(iEntityWrapper* apEntity)
{
	apEntity->SetCompoundObject(NULL,false);
}

//------------------------------------------------------------------------


void cEntityWrapperCompoundObject::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected,const cColor& aHighlightCol, const cColor& aDisabledCol)
{
	if(abIsSelected==false)
		return;

	for(tEntityWrapperListIt it = mlstComponents.begin();it!=mlstComponents.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		cBoundingVolume* pBV = pEnt->GetPickBV(NULL);

		pEnt->Draw(apViewport, apFunctions, apEditMode, abIsSelected, cColor(1,0,0,1));

		//apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(), pBV->GetMax(), cColor(1,0,0,1));
	}

}

//------------------------------------------------------------------------

//------------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperCompoundObject::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = hplNew(cEditorWindowEntityEditBoxCompound,(apEditMode,this));

	return pEditBox;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
