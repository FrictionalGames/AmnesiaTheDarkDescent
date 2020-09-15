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

#include "EntityWrapperFactory.h"

#include "EditorBaseClasses.h"
#include "EditorWorld.h"

#include "EntityWrapperStaticObject.h"
#include "EntityWrapperEntity.h"
#include "EntityWrapperSubMesh.h"
#include "EntityWrapperLightBox.h"
#include "EntityWrapperLightPoint.h"
#include "EntityWrapperLightSpot.h"
#include "EntityWrapperBodyShape.h"
#include "EntityWrapperBody.h"
#include "EntityWrapperJoint.h"
#include "EntityWrapperJointBall.h"
#include "EntityWrapperJointHinge.h"
#include "EntityWrapperJointScrew.h"
#include "EntityWrapperJointSlider.h"
#include "EntityWrapperArea.h"
#include "EntityWrapperBillboard.h"
#include "EntityWrapperSound.h"
#include "EntityWrapperParticleSystem.h"
#include "EntityWrapperPrimitivePlane.h"

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperFactory::CreateEntityWrapper(iEditorWorld* apEditorWorld, eEditorEntityType aType,
														   int alID, const tString& asName,
														   const tString& asFilename,
														   const cVector3f& avPosition,
														   const cVector3f& avRotation,
														   const cVector3f& avScale,
														   int alEntitySubType)
{
	iEntityWrapper* pEntityWrapper = NULL;

	/*

	switch(aType)
	{
	case eEditorEntityType_StaticObject:
		pEntityWrapper = hplNew( cEntityWrapperStaticObject, (apEditorWorld, alID, asName, asFilename));
		break;
	case eEditorEntityType_Entity:
		pEntityWrapper = hplNew( cEntityWrapperEntity, (apEditorWorld, alID, asName, asFilename));
		break;
	case eEditorEntityType_Light:
		switch(alEntitySubType)
		{
		case eEditorEntityLightType_Box:
			pEntityWrapper = hplNew(cEntityWrapperLightBox,(apEditorWorld, alID, asName));
			break;
		case eEditorEntityLightType_Point:
			pEntityWrapper = hplNew(cEntityWrapperLightPoint,(apEditorWorld, alID, asName));
			break;
		case eEditorEntityLightType_Spot:
			pEntityWrapper = hplNew(cEntityWrapperLightSpot,(apEditorWorld, alID, asName));
			break;
		default:
			break;
		}
		break;
	case eEditorEntityType_Area:
		pEntityWrapper = hplNew(cEntityWrapperArea,(apEditorWorld, alID, asName));
		break;
	case eEditorEntityType_Billboard:
		pEntityWrapper = hplNew(cEntityWrapperBillboard,(apEditorWorld, alID, asName));
		break;
	case eEditorEntityType_Sound:
		pEntityWrapper = hplNew(cEntityWrapperSound,(apEditorWorld, alID, asName));
		break;
	case eEditorEntityType_ParticleSystem:
		pEntityWrapper = hplNew(cEntityWrapperParticleSystem,(apEditorWorld, alID, asName));
		break;
	case eEditorEntityType_BodyShape:
		pEntityWrapper = hplNew(cEntityWrapperBodyShape,(apEditorWorld, alID, asName, (eEditorBodyShape)alEntitySubType));
		break;
	case eEditorEntityType_Body:
		pEntityWrapper = hplNew(cEntityWrapperBody,(apEditorWorld, alID, asName));
		break;
	case eEditorEntityType_Joint:
		switch(alEntitySubType)
		{
		case eEditorEntityJointType_Ball:
			pEntityWrapper = hplNew(cEntityWrapperJointBall,(apEditorWorld, alID, asName));
			break;
		case eEditorEntityJointType_Hinge:
			pEntityWrapper = hplNew(cEntityWrapperJointHinge,(apEditorWorld, alID, asName));
			break;
		case eEditorEntityJointType_Screw:
			pEntityWrapper = hplNew(cEntityWrapperJointScrew,(apEditorWorld, alID, asName));
			break;
		case eEditorEntityJointType_Slider:
			pEntityWrapper = hplNew(cEntityWrapperJointSlider,(apEditorWorld, alID, asName));
			break;
		default:
			break;
		}
		break;
	case eEditorEntityType_SubMesh:
		pEntityWrapper = hplNew(cEntityWrapperSubMesh,(apEditorWorld, alID, asFilename, alEntitySubType));
		break;
	}

	if(CheckIsCreated(pEntityWrapper)==false)
		return NULL;

	SetUpEntityWrapper(pEntityWrapper, avPosition, avRotation, avScale);

	apEditorWorld->AddObject(pEntityWrapper);
	*/
	return pEntityWrapper;
}

//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperFactory::CreateEntityWrapperPrimitive(iEditorWorld* apEditorWorld, int alID, const tString& asName, tVector3fList& alstParameters)
{
	if(apEditorWorld==NULL) return NULL;

	cEntityWrapperPrimitive* pEnt = hplNew(cEntityWrapperPrimitivePlane,(apEditorWorld, alID, ""));
	if(pEnt) pEnt->CreatePrimitiveFromParameters(alstParameters);

	if(CheckIsCreated(pEnt)==false)
		return NULL;

	apEditorWorld->AddObject(pEnt);

	return pEnt;
}

//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperFactory::CreateEntityWrapperFromEntityWrapperData(iEditorWorld* apEditorWorld, iEntityWrapperData* apData)
{
	if(apData==NULL || apEditorWorld==NULL)
		return NULL;

	iEntityWrapper* pEnt = NULL;

	if(apData->mEntityType==eEditorEntityType_Primitive)
	{
		cEntityWrapperDataPrimitive* pData = (cEntityWrapperDataPrimitive*)apData;
		pEnt = cEntityWrapperFactory::CreateEntityWrapperPrimitive(apEditorWorld, pData->mlID, pData->msName, pData->mlstParameters);
	}
	else
		pEnt = cEntityWrapperFactory::CreateEntityWrapper(apEditorWorld, apData->mEntityType, apData->mlID, apData->msName, apData->msFilename, 0,0,1, apData->mlSubType);
	
	if(pEnt) pEnt->LoadFromEntityWrapperData(apData);

	return pEnt;
}

//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperFactory::CreateEntityWrapperFromXMLElement(iEditorWorld* apEditorWorld, cXmlElement* apElement)
{

	//////////////////////////////////////////////////////////////////////////
	// Returns a properly created Entity Wrapper based on data in apElement
	// Everytime a new entity type is added, this method needs to be updated
	// (of course this is not the best way to do this)

	iEntityWrapper* pEnt = NULL;

	int lType = apElement->GetAttributeInt("Type");
	
	int lSubType = apElement->GetAttributeInt("SubType",-1);
	// Backward compatibility
	if(lSubType==-1)
		lSubType = apElement->GetAttributeInt("LightType",-1);
	if(lSubType==-1)
		lSubType = apElement->GetAttributeInt("JointType",-1);
	if(lSubType==-1)
		lSubType = apElement->GetAttributeInt("SubMeshID",-1);


	int lID = apElement->GetAttributeInt("ID", -1);
	if(lID==-1)
		lID = apEditorWorld->GetFreeID();

	tString sFilename = apElement->GetAttributeString("Filename");
	// Backward compatibility
	if(sFilename=="")
		sFilename = apElement->GetAttributeString("MeshFilename");
	if(sFilename=="")
		sFilename = apElement->GetAttributeString("EntityFilename");

    if(lType==eEditorEntityType_Primitive)
	{
		cXmlNodeListIterator itParams = apElement->GetFirstElement("Parameters")->GetChildIterator();
		tVector3fList lstParameters;
		while(itParams.HasNext())
		{
			cXmlElement* pParam = itParams.Next()->ToElement();

			lstParameters.push_back(pParam->GetAttributeVector3f("Value"));
		}
		pEnt = cEntityWrapperFactory::CreateEntityWrapperPrimitive(apEditorWorld, lID, "", lstParameters);
	}
	else
        pEnt = cEntityWrapperFactory::CreateEntityWrapper(apEditorWorld, (eEditorEntityType)lType, lID , "", sFilename, 0,0,1, lSubType);

	if(pEnt) pEnt->Load(apElement);

	return pEnt;

}

//----------------------------------------------------------------------------



//---------------------------------------------------------------------------

void cEntityWrapperFactory::DestroyEntityWrapper(iEntityWrapper* apEntity, bool abRemoveFromWorld)
{
	if(apEntity==NULL)
		return;

	iEditorWorld* pWorld = apEntity->GetEditorWorld();
	if(abRemoveFromWorld && pWorld) pWorld->RemoveObject(apEntity);
	
	hplDelete(apEntity);
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cEntityWrapperFactory::SetUpEntityWrapper(iEntityWrapper* apEntity, const cVector3f& avPosition, const cVector3f& avRotation, const cVector3f& avScale)
{
	if(apEntity==NULL)
		return;

	apEntity->SetAbsPosition(avPosition);
	apEntity->SetAbsRotation(avRotation);
	apEntity->SetAbsScale(avScale);

	apEntity->UpdateEntity();
}

//---------------------------------------------------------------------------

bool cEntityWrapperFactory::CheckIsCreated(iEntityWrapper* apEntity)
{
	if(apEntity==NULL || apEntity->IsCreated()==false)
	{
		if(apEntity) hplDelete(apEntity);
		return false;
	}

	return true;
}

