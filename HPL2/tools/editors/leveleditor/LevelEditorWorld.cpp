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

#include "LevelEditorWorld.h"

#include "../common/DirectoryHandler.h"
#include "../common/EntityWrapper.h"
#include "../common/EntityWrapperDecal.h"

#include "../common/EditorBaseClasses.h"
#include "../common/EditorHelper.h"


//-------------------------------------------------------------------------

cLevelEditorWorld::cLevelEditorWorld(iEditorBase* apEditor) : iEditorWorld(apEditor, "Level")
{
	AddEntityCategory("StaticObjects", eEditorEntityType_StaticObject);
	AddEntityCategory("Primitives", eEditorEntityType_Primitive);
	AddEntityCategory("Decals", eEditorEntityType_Decal);
	AddEntityCategory("Entities", -1);
	AddEntityCategory("Misc", eEditorEntityType_Compound);

	AddFileIndex("StaticObjects", eEditorEntityType_StaticObject);
	AddFileIndex("Entities", eEditorEntityType_Entity);
	AddFileIndex("Decals", eEditorEntityType_Decal);
}

//-------------------------------------------------------------------------

cLevelEditorWorld::~cLevelEditorWorld()
{
	ClearStaticObjectCombos();
}

//-------------------------------------------------------------------------

cXmlElement* cLevelEditorWorld::GetWorldDataElement(iXmlDocument* apXmlDoc)
{
	return apXmlDoc->GetFirstElement("MapData");
}

//-------------------------------------------------------------------------

cXmlElement* cLevelEditorWorld::GetWorldObjectsElement(cXmlElement* apWorldDataElement)
{
	return apWorldDataElement->GetFirstElement("MapContents");
}

//-------------------------------------------------------------------------

void cLevelEditorWorld::LoadWorldObjects(cXmlElement* apWorldObjectsElement)
{
	iEditorWorld::LoadWorldObjects(apWorldObjectsElement);

	cXmlElement* pMapData = apWorldObjectsElement->GetParent()->ToElement();

	//////////////////////////////////////////////////////
	// Load skybox params
	SetSkyboxActive(pMapData->GetAttributeBool("SkyBoxActive", false));
	SetSkyboxTexture(pMapData->GetAttributeString("SkyBoxTexture"));
	SetSkyboxColor(pMapData->GetAttributeColor("SkyBoxColor", cColor(1)));

	//////////////////////////////////////////////////////
	// Load global fog params
	SetFogActive(pMapData->GetAttributeBool("FogActive", false));
	SetFogCulling(pMapData->GetAttributeBool("FogCulling", true));
	SetFogStart(pMapData->GetAttributeFloat("FogStart", 0));
	SetFogEnd(pMapData->GetAttributeFloat("FogEnd", 20));
	SetFogFalloffExp(pMapData->GetAttributeFloat("FogFalloffExp", 1));
	SetFogColor(pMapData->GetAttributeColor("FogColor", cColor(1)));

	//////////////////////////////////////////////////////
	// Load global Max triangles per decal
	cEntityWrapperTypeDecal::SetGlobalMaxTriangles(pMapData->GetAttributeInt("GlobalDecalMaxTris", cEntityWrapperTypeDecal::GetGlobalMaxTriangles()));

	//////////////////////////////////////////////////////
	// Load static object combos
	int lMaxComboID = -1;
	cXmlElement* pXmlCombos = apWorldObjectsElement->GetFirstElement("StaticObjectCombos");
	if(pXmlCombos)
	{
		cXmlNodeListIterator it = pXmlCombos->GetChildIterator();
		while(it.HasNext())
		{
			cXmlElement* pXmlCombo = it.Next()->ToElement();

			cLevelEditorStaticObjectCombo* pCombo = CreateStaticObjectCombo(-1);
			if(pCombo->Load(pXmlCombo))
			{
				AddStaticObjectCombo(pCombo);

				if(lMaxComboID<pCombo->GetID())
					lMaxComboID = pCombo->GetID();
			}
		}
	}
	mlComboID = lMaxComboID+1;
}

//-------------------------------------------------------------------------

cXmlElement* cLevelEditorWorld::CreateWorldDataElement(iXmlDocument* apXmlDoc)
{
	return apXmlDoc->CreateChildElement("MapData");
}

//-------------------------------------------------------------------------

cXmlElement* cLevelEditorWorld::CreateWorldObjectsElement(cXmlElement* apWorldDataElement)
{

	return apWorldDataElement->CreateChildElement("MapContents");
}

//-------------------------------------------------------------------------

void cLevelEditorWorld::SaveWorldObjects(cXmlElement* apWorldObjectsElement, tEntityWrapperList& alstEnts)
{
	iEditorWorld::SaveWorldObjects(apWorldObjectsElement, alstEnts);

	//////////////////////////////////////////////////////
	// Save skybox params
	cXmlElement* pMapData = apWorldObjectsElement->GetParent()->ToElement();
	pMapData->SetAttributeBool("SkyBoxActive", GetSkyboxActive());
	pMapData->SetAttributeString("SkyBoxTexture", cString::To8Char(mpEditor->GetPathRelToWD(GetSkyboxTexture())));
	pMapData->SetAttributeColor("SkyBoxColor", GetSkyboxColor());

	//////////////////////////////////////////////////////
	// Save global fog params
	pMapData->SetAttributeBool("FogActive", GetFogActive());
	pMapData->SetAttributeBool("FogCulling", GetFogCulling());
	pMapData->SetAttributeFloat("FogStart", GetFogStart());
	pMapData->SetAttributeFloat("FogEnd", GetFogEnd());
	pMapData->SetAttributeFloat("FogFalloffExp", GetFogFalloffExp());
	pMapData->SetAttributeColor("FogColor", GetFogColor());

	//////////////////////////////////////////////////////
	// Save global Decal params
	pMapData->SetAttributeInt("GlobalDecalMaxTris", cEntityWrapperTypeDecal::GetGlobalMaxTriangles());

	////////////////////////////////////////////////////////
	// Save Static object Combinations
	cXmlElement* pXmlCombos = apWorldObjectsElement->CreateChildElement("StaticObjectCombos");
	tStaticObjectComboList::const_iterator itCombos = mlstStaticObjectCombos.begin();
	for(;itCombos!=mlstStaticObjectCombos.end();++itCombos)
	{
		cLevelEditorStaticObjectCombo* pCombo = *itCombos;

		pCombo->Save(pXmlCombos);
	}
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------

void cLevelEditorWorld::Reset()
{
	iEditorWorld::Reset();

	mlComboID = 0;
	ClearStaticObjectCombos();
}

//-------------------------------------------------------------------------

cLevelEditorStaticObjectCombo* cLevelEditorWorld::CreateStaticObjectCombo(int alComboID)
{
	if(GetStaticObjectCombo(alComboID)!=NULL)
		return NULL;

	cLevelEditorStaticObjectCombo* pCombo = hplNew(cLevelEditorStaticObjectCombo,(this, alComboID));

	return pCombo;
}

//-------------------------------------------------------------------------

bool cLevelEditorWorld::AddStaticObjectCombo(cLevelEditorStaticObjectCombo* apCombo)
{
	if(apCombo && GetStaticObjectCombo(apCombo->GetID())==NULL)
	{
		mlstStaticObjectCombos.push_back(apCombo);
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------

void cLevelEditorWorld::RemoveStaticObjectCombo(int alID)
{
	cLevelEditorStaticObjectCombo* pCombo = GetStaticObjectCombo(alID);

	if(pCombo)
	{
		mlstStaticObjectCombos.remove(pCombo);
		hplDelete(pCombo);
	}
}

//-------------------------------------------------------------------------

void cLevelEditorWorld::ClearStaticObjectCombos()
{
	STLDeleteAll(mlstStaticObjectCombos);
}

//-------------------------------------------------------------------------

cLevelEditorStaticObjectCombo* cLevelEditorWorld::GetStaticObjectCombo(int alComboID)
{
	tStaticObjectComboListIt it = mlstStaticObjectCombos.begin();
	for(;it!=mlstStaticObjectCombos.end();++it)
	{
		cLevelEditorStaticObjectCombo* pCombo = *it;
		if(pCombo->GetID() == alComboID)
			return pCombo;
	}

	return NULL;
}

//-------------------------------------------------------------------------

int cLevelEditorWorld::GetStaticObjectComboNum() 
{ 
	return (int)mlstStaticObjectCombos.size(); 
}

//-------------------------------------------------------------------------

void cLevelEditorWorld::CreateDataCallback(iEntityWrapperData* apData)
{
	apData->SetExtData(hplNew(cLevelEditorEntityExtData,()));
}

//-------------------------------------------------------------------------

void cLevelEditorWorld::CopyDataFromEntityCallback(iEntityWrapperData* apData, iEntityWrapper* apEnt)
{
	cLevelEditorEntityExtData* pDataExtData = (cLevelEditorEntityExtData*)apData->GetExtData();
	cLevelEditorEntityExtData* pEntExtData = (cLevelEditorEntityExtData*)apEnt->GetEntityExtData();
	if(pEntExtData==NULL)
		return;

	pDataExtData->mlComboID = pEntExtData->mlComboID;
	pDataExtData->mlGroupID = pEntExtData->mlGroupID;
}

//-------------------------------------------------------------------------

void cLevelEditorWorld::CopyDataToEntityCallback(iEntityWrapperData* apData, iEntityWrapper* apEnt, int alCopyStep)
{
	if(alCopyStep == ePropCopyStep_PostDeployAll)
	{
		void* pData = apEnt->GetEntityExtData();
		if(pData)
		{
			hplDelete(pData);
			apEnt->SetEntityExtData(NULL);
		}

		cLevelEditorEntityExtData* pExtData = (cLevelEditorEntityExtData*)apData->GetExtData();
		cLevelEditorEntityExtData* pCopyExtData = hplNew(cLevelEditorEntityExtData,());

		pCopyExtData->mlComboID = pExtData->mlComboID;
		pCopyExtData->mlGroupID = pExtData->mlGroupID;

		apEnt->SetEntityExtData(pCopyExtData);

		cLevelEditorStaticObjectCombo* pCombo = GetStaticObjectCombo(pExtData->mlComboID);
		if(pCombo)
			pCombo->AddObject(apEnt);
	}
}

//-------------------------------------------------------------------------

void cLevelEditorWorld::DestroyDataCallback(iEntityWrapperData* apData)
{
	cLevelEditorEntityExtData* pDataExtData = (cLevelEditorEntityExtData*)apData->GetExtData();
	if(pDataExtData)
		hplDelete(pDataExtData);
}

//-------------------------------------------------------------------------

void cLevelEditorWorld::SaveDataCallback(iEntityWrapperData* apData, cXmlElement* apElement)
{
	cLevelEditorEntityExtData* pDataExtData = (cLevelEditorEntityExtData*)apData->GetExtData();
	if(pDataExtData)
	{
		apElement->SetAttributeInt("Group", pDataExtData->mlGroupID);
	}
}

//-------------------------------------------------------------------------

void cLevelEditorWorld::LoadDataCallback(iEntityWrapperData* apData, cXmlElement* apElement)
{
	cLevelEditorEntityExtData* pDataExtData = (cLevelEditorEntityExtData*)apData->GetExtData();
	if(pDataExtData)
	{
		pDataExtData->mlGroupID = apElement->GetAttributeInt("Group", 0);
	}
}

//-------------------------------------------------------------------------

void cLevelEditorWorld::DestroyEntityWrapperCallback(iEntityWrapper* apEnt)
{
	cLevelEditorEntityExtData* pExtData = (cLevelEditorEntityExtData*)apEnt->GetEntityExtData();
	if(pExtData)
	{
		cLevelEditorStaticObjectCombo* pCombo = GetStaticObjectCombo(pExtData->mlComboID);
		if(pCombo) pCombo->RemoveObject(apEnt);

		hplDelete(pExtData);
	}
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
