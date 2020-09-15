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

#include "../LevelEditor/HPLMap.h"
#include "../Common/EdScnObject.h"

#include "../Common/Editor.h"
#include "../Common/EdViewport.h"
#include "../Common/EdEditModeHandler.h"
#include "../Common/EdEditModeSelect.h"

#include "../Common/EdObjCompound.h"

#include "../LevelEditor/HPLMapActions.h"
#include "../Common/EdClipPlaneActions.h"

#include "../Common/EdResource.h"


bool GroupSortPredicate(cEdGroup* apGroup1, cEdGroup* apGroup2)
{
    return apGroup1->GetID() < apGroup2->GetID();
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cLevelEditorSessionData::cLevelEditorSessionData(iEdWorld* apWorld) : iEdSessionData(apWorld)
{
}

bool cLevelEditorSessionData::Load(cXmlElement* apElem)
{
	cXmlElement* pDataElem = apElem->GetFirstElement("EditorSession");
	if(pDataElem==NULL)
		return false;

	cXmlElement* pClipPlanes = pDataElem->GetFirstElement("ClipPlanes");
	if(pClipPlanes)
	{
		iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpWorld);
		cXmlNodeListIterator it = pClipPlanes->GetChildIterator();
		while(it.HasNext())
		{
			cXmlElement* pXmlPlane = it.Next()->ToElement();
			cEdClipPlane* pPlane = pWorld->AddClipPlane(pWorld->GetFreeClipPlaneID());

			pPlane->Load(pXmlPlane);
		}
	}

	return true;
}

bool cLevelEditorSessionData::Save(cXmlElement* apElem)
{
	iEditor* pEditor = mpWorld->GetEditor();
	cXmlElement* pDataElem = apElem->CreateChildElement("EditorSession");

	
	cXmlElement* pViewportsData = pDataElem->CreateChildElement("ViewportConfig");

	int lFocused = 0;
	iEdViewport* pFocusedViewport = pEditor->GetFocusedViewport();
	for(int i=0;i<pEditor->GetNumViewports(); ++i)
	{
		iEdViewport* pViewport = pEditor->GetViewport(i);
		if(pViewport==pFocusedViewport)
			lFocused = i;

		pViewport->SaveState(pViewportsData, NULL);
	}
	pViewportsData->SetAttributeInt("Focused", i);
	pViewportsData->SetAttributeBool("Enlarged", pFocusedViewport->IsEnlarged());

	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpWorld);
	const tClipPlaneMap& clipPlanes = pWorld->GetClipPlanes();
	if(clipPlanes.empty()==false)
	{
		cXmlElement* pXmlClipPlanes = pDataElem->CreateChildElement("ClipPlanes");
		tClipPlaneMap::const_iterator it = clipPlanes.begin();
		for(;it!=clipPlanes.end();++it)
		{
			cEdClipPlane* pPlane = it->second;
			
			pPlane->Save(pXmlClipPlanes);
		}

		//pXmlClipPlanes->SetAttributeInt("Focused");
	}
	

	return true;
}

cMapObjExtension::cMapObjExtension(iEdWorld* apWorld, iEdObject* apObj) : iEdObjExtension(apWorld, apObj)
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpObject);

	mpGroup = NULL;
	if(pObj) pMap->GetNullGroup()->AddObject(pObj);
}

cMapObjExtension::~cMapObjExtension()
{
	if(mpObject)
	{
		if(mpGroup)	SetGroup(NULL, true);
	}
		
}

void cMapObjExtension::SetGroup(cEdGroup* apGroup, bool abRemoveFromOld)
{
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpObject);
	if(abRemoveFromOld && mpGroup)
		mpGroup->RemoveObject(pObj);

	mpGroup = apGroup;
}

void cMapObjExtension::SetCombo(cLevelEditorStaticObjectCombo* apCombo, bool abRemoveFromOld)
{
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpObject);
	if(abRemoveFromOld && mpCombo)
		mpCombo->RemoveObject(pObj);

	mpCombo = apCombo;
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cHplMap::cHplMap(iEditor* apEditor) : iEdScnWorld(apEditor, _W("HPL Map"), _W("map"), "Map")
{
	mbGroupsChanged = false;

	mpNullGroup = NULL;
	mpNullGroup = AddGroup(0, false);
	mpNullGroup->SetName("None");

	mpLastAddedCombo = NULL;
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cLevelEditorStaticObjectCombo* cHplMap::CreateStaticObjectCombo(int alComboID)
{
	if(GetStaticObjectCombo(alComboID)!=NULL)
		return NULL;

	cLevelEditorStaticObjectCombo* pCombo = hplNew(cLevelEditorStaticObjectCombo,(this, alComboID));

	return pCombo;
}

//-------------------------------------------------------------------------

bool cHplMap::AddStaticObjectCombo(cLevelEditorStaticObjectCombo* apCombo)
{
	if(apCombo && GetStaticObjectCombo(apCombo->GetID())==NULL)
	{
		mlstStaticObjectCombos.push_back(apCombo);
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------

void cHplMap::RemoveStaticObjectCombo(int alID)
{
	cLevelEditorStaticObjectCombo* pCombo = GetStaticObjectCombo(alID);

	if(pCombo)
	{
		mlstStaticObjectCombos.remove(pCombo);
		hplDelete(pCombo);
	}
}

//-------------------------------------------------------------------------

void cHplMap::ClearStaticObjectCombos()
{
	STLDeleteAll(mlstStaticObjectCombos);
}

//-------------------------------------------------------------------------

cLevelEditorStaticObjectCombo* cHplMap::GetStaticObjectCombo(int alComboID)
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

int cHplMap::GetStaticObjectComboNum() 
{ 
	return (int)mlstStaticObjectCombos.size(); 
}

//-------------------------------------------------------------------------

cEdGroup* cHplMap::AddGroup(int alID, bool abAddToList)
{
	if(GetGroupByID(alID)!=NULL)
		return NULL;

	cEdGroup* pGroup = hplNew(cEdGroup,(this, alID, ""));
	if(abAddToList)
	{
		mlstGroups.push_back(pGroup);
		mlstGroups.sort(GroupSortPredicate);
	}

	mbGroupsChanged = true;
	
	return pGroup;
}

void cHplMap::RemoveGroup(int alID)
{
	cEdGroup* pGroup = GetGroupByID(alID);
	if(pGroup)
	{
		mlstGroups.remove(pGroup);
		hplDelete(pGroup);

		mbGroupsChanged = true;
	}
}

//-------------------------------------------------------------------------

const tGroupList& cHplMap::GetGroups()
{
	return mlstGroups;
}

cEdGroup* cHplMap::GetGroupByID(int alID)
{
	if(mpNullGroup && alID==mpNullGroup->GetID()) return NULL;

	tGroupListIt it = mlstGroups.begin();
	for(;it!=mlstGroups.end(); ++it)
	{
		cEdGroup* pGroup = *it;
		if(pGroup->GetID()==alID)
			return pGroup;
	}

	return NULL;
}

cEdGroup* cHplMap::GetGroup(int alIdx)
{
	int i=0;
	tGroupListIt it = mlstGroups.begin();
	for(;it!=mlstGroups.end(); ++it, ++i)
	{
		cEdGroup* pGroup = *it;
		if(i==alIdx)
			return pGroup;
	}

	return NULL;
}

int cHplMap::GetNumGroups()
{
	return (int)mlstGroups.size();
}

//-------------------------------------------------------------------------

iEdAction* cHplMap::CreateAddComboAction()
{
	return hplNew(cEdActionMapAddStaticObjectCombo,(this));
}

//-------------------------------------------------------------------------

iEdAction* cHplMap::CreateRemoveComboAction(int alID)
{
	return hplNew(cEdActionMapRemoveStaticObjectCombo,(this, alID));
}

//-------------------------------------------------------------------------

iEdAction* cHplMap::CreateAddGroupAction()
{
	return hplNew(cEdActionMapAddGroup,(this));
}

//-------------------------------------------------------------------------

iEdAction* cHplMap::CreateRemoveGroupAction(int alID)
{
	return hplNew(cEdActionMapDeleteGroup,(this, alID));
}

//-------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEdSessionData* cHplMap::CreateSessionData()
{
	return hplNew(cLevelEditorSessionData,(this));
}

//-------------------------------------------------------------------

bool cHplMap::LoadContentsFromXmlDoc(iXmlDocument* apDoc)
{
	cXmlElement* pMapDataElement = apDoc->GetFirstElement("MapData");
	
	SetFogActive(pMapDataElement->GetAttributeBool("FogActive"));
	SetFogCulling(pMapDataElement->GetAttributeBool("FogCulling"));
	SetFogColor(pMapDataElement->GetAttributeColor("FogColor"));
	SetFogStart(pMapDataElement->GetAttributeFloat("FogStart"));
	SetFogEnd(pMapDataElement->GetAttributeFloat("FogEnd"));
	SetFogFalloffExp(pMapDataElement->GetAttributeFloat("FogFalloffExp"));

	SetSkyboxActive(pMapDataElement->GetAttributeBool("SkyBoxActive"));
	SetSkyboxColor(pMapDataElement->GetAttributeColor("SkyBoxColor"));
	SetSkyboxTexture(pMapDataElement->GetAttributeString("SkyBoxTexture"));

	cXmlElement* pMapContentsElement = pMapDataElement->GetFirstElement("MapContents");
	if(pMapContentsElement==NULL)
	{
		Error("MapContents element not found");
		return false;
	}

	LoadFileIndices(pMapContentsElement);

	LoadObjects(pMapContentsElement, tScnObjList());

	//////////////////////////////////////////////////////
	// Load static object combos
	int lMaxComboID = -1;
	cXmlElement* pXmlCombos = pMapContentsElement->GetFirstElement("StaticObjectCombos");
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


	return true;
}

//-------------------------------------------------------------------

bool cHplMap::SaveContentsToXmlDoc(iXmlDocument* apDoc)
{
	// Save file indices
	// Go through categories
	// Save objects in each category

	cXmlElement* pMapDataElement = apDoc->CreateChildElement("MapData");

	pMapDataElement->SetAttributeBool("FogActive", GetFogActive());
	pMapDataElement->SetAttributeBool("FogCulling", GetFogCulling());
	pMapDataElement->SetAttributeColor("FogColor", GetFogColor());
	pMapDataElement->SetAttributeFloat("FogStart", GetFogStart());
	pMapDataElement->SetAttributeFloat("FogEnd", GetFogEnd());
	pMapDataElement->SetAttributeFloat("FogFalloffExp", GetFogFalloffExp());

	pMapDataElement->SetAttributeBool("SkyBoxActive", GetSkyboxActive());
	pMapDataElement->SetAttributeColor("SkyBoxColor", GetSkyboxColor());
	pMapDataElement->SetAttributeString("SkyBoxTexture", GetSkyboxTexture());

	cXmlElement* pMapContentsElement = pMapDataElement->CreateChildElement("MapContents");
	SaveFileIndices(pMapContentsElement);

	tScnObjList lstObjects;
	tEdObjectMapIt it = mmapObjects.begin();
	for(;it!=mmapObjects.end();++it)
	{
		iEdScnObject* pObj = static_cast<iEdScnObject*>(it->second);

		lstObjects.push_back(pObj);
	}

	SaveObjects(pMapContentsElement, lstObjects);

	////////////////////////////////////////////////////////
	// Save Static object Combinations
	cXmlElement* pXmlCombos = pMapContentsElement->CreateChildElement("StaticObjectCombos");
	tStaticObjectComboList::const_iterator itCombos = mlstStaticObjectCombos.begin();
	for(;itCombos!=mlstStaticObjectCombos.end();++itCombos)
	{
		cLevelEditorStaticObjectCombo* pCombo = *itCombos;

		pCombo->Save(pXmlCombos);
	}

	/////////////////////////////////////////////////////////
	// Save default .hps
	tWString sScriptFile = apDoc->GetPath();
	sScriptFile = cString::SetFileExtW(sScriptFile, _W("hps"));
	cPlatform::CloneFile(_W("editor/default.hps"), sScriptFile,	true);

	return true;
}

//-------------------------------------------------------------------

void cHplMap::OnInit()
{
	AddType(hplNew(cTypeCompound,()));

	iEdScnWorld::OnInit();

	///////////////////////////
	// Register categories
	cEdScnObjCategory* pCat = NULL;
	pCat = CreateCategory("StaticObjects");
	pCat->RegisterType(this, _W("Static Object"));

	pCat = CreateCategory("Primitives");
	pCat->RegisterType(this, _W("Primitive"));

	pCat = CreateCategory("Decals");
	pCat->RegisterType(this, _W("Decal"));

	pCat = CreateCategory("Entities");
	pCat->RegisterType(this, _W("Object"));

	pCat = CreateCategory("Misc");
	pCat->RegisterType(this, _W("Compound"));
}

//-------------------------------------------------------------------

void cHplMap::OnReset()
{
	iEdScnWorld::OnReset();

	STLDeleteAll(mlstStaticObjectCombos);
	mlComboID = -1;

	STLDeleteAll(mlstGroups);
	mlGroupID = 0;
}

//-------------------------------------------------------------------

void cHplMap::OnUpdate()
{
	iEdScnWorld::OnUpdate();

	if(mbGroupsChanged)
	{
		mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldParamsChange, NULL);
		mbGroupsChanged = false;
	}
}

//-------------------------------------------------------------------
