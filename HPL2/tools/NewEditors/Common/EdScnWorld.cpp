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

#include "../Common/EdScnWorld.h"
#include "../Common/EdScnObject.h"
#include "../Common/EdScnWorldActions.h"

#include "../Common/Editor.h"
#include "../Common/EdViewport.h"
#include "../Common/EdEditModeHandler.h"
#include "../Common/EdEditModeSelect.h"

#include "../Common/EdObjCompound.h"

#include "../Common/EdClipPlaneActions.h"

#include "../Common/EdResource.h"

#include "../Common/EdSurfacePicker.h"
#include "../Common/EdSelection.h"


//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// CATEGORY - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEdScnObjCategory::cEdScnObjCategory(const tString& asName)
{
	msName = asName;
	mpElement = NULL;

	mbDestroyHandler = false;
	mpDataHandler = NULL;

	mbBypassLoadObjects = false;
}

cEdScnObjCategory::~cEdScnObjCategory()
{
	ClearDataHandler();
}

void cEdScnObjCategory::SetDataHandler(iEdScnObjCategoryDataHandler* apHandler, bool abDestroyHandler)
{
	ClearDataHandler();
	

	mbDestroyHandler = abDestroyHandler;
	mpDataHandler = apHandler;
}

void cEdScnObjCategory::ClearDataHandler()
{
	if(mbDestroyHandler && mpDataHandler)
	{
		hplDelete(mpDataHandler);
		mpDataHandler = NULL;
	}
}

bool cEdScnObjCategory::RegisterType(iEdScnWorld* apWorld, const tWString& asTypeName)
{
	iEdScnObjType* pType = static_cast<iEdScnObjType*>(apWorld->GetTypeByName(asTypeName));
	if(pType==NULL)
	{
		FatalError("ScnWorld: Failed to register type '%ls' with save category '%s'", asTypeName.c_str(), msName.c_str());
		return false;
	}

	pType->SetCategory(this);
	return true;
}

void cEdScnObjCategory::PreObjectsSave(cXmlElement* apParentElement)
{
	mpElement = apParentElement->CreateChildElement(msName);
	if(mpDataHandler) mpDataHandler->Save(mpElement);
}

void cEdScnObjCategory::PostObjectsSave()
{
	mpElement = NULL;
}

void cEdScnObjCategory::PreObjectsLoad(cXmlElement* apParentElement)
{
	mpElement = apParentElement->GetFirstElement(msName);
}

void cEdScnObjCategory::Load(iEdScnWorld* apWorld, tScnObjList& alstLoadedObjects)
{
	if(mpElement==NULL)
	{
		Error("Category %s not found, skipping...\n", msName.c_str());
		return;
	}

	if(mpDataHandler) mpDataHandler->Load(mpElement);

	if(mpDataHandler==NULL || mbBypassLoadObjects==false)
	{
		cXmlNodeListIterator it = mpElement->GetChildIterator();
		while(it.HasNext())
		{
			cXmlElement* pObjElement = it.Next()->ToElement();
			iEdScnObject* pObj = static_cast<iEdScnObject*>(apWorld->CreateObjFromXmlElement(pObjElement));
			if(pObj==NULL)
				Error("Loading object in scene\n");
			else
                alstLoadedObjects.push_back(pObj);
		}
	}
}

void cEdScnObjCategory::PostObjectsLoad()
{
	PostObjectsSave();
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// SCENE WORLD - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEdScnWorld::iEdScnWorld(iEditor* apEditor, 
						 const tWString& asName, const tWString& asFileExt, const tString& asXmlElementName) : iEdWorld(apEditor, asName, asFileExt, asXmlElementName, hplNew(cEdScnObjRoot,()))
{
	mpSurfacePicker = hplNew(cEdSurfacePicker,(this));
}

iEdScnWorld::~iEdScnWorld()
{
	hplDelete(mpSurfacePicker);
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEdScnObject* iEdScnWorld::GetScnObject(int alID)
{
	return static_cast<iEdScnObject*>(GetObject(alID));
}

//-------------------------------------------------------------------

bool iEdScnWorld::GetFogCulling()
{
	return mpWorld->GetFogCulling();
}

//-------------------------------------------------------------------

float iEdScnWorld::GetFogStart()
{
	return mpWorld->GetFogStart();
}

//-------------------------------------------------------------------

float iEdScnWorld::GetFogEnd()
{
	return mpWorld->GetFogEnd();
}

//-------------------------------------------------------------------

float iEdScnWorld::GetFogFalloffExp()
{
	return mpWorld->GetFogFalloffExp();
}

//-------------------------------------------------------------------

const cColor& iEdScnWorld::GetFogColor()
{
	return mpWorld->GetFogColor();
}

//----------------------------------------------------------------------------

void iEdScnWorld::SetShowFog(bool abX)
{
	mbShowFog = abX;
	SetFogActive(GetFogActive());
}

//----------------------------------------------------------------------------

void iEdScnWorld::SetFogActive(bool abX)
{
	mbFogActive = abX;
	mpWorld->SetFogActive(abX && mbShowFog);
}

void iEdScnWorld::SetFogCulling(bool abX)
{
	mpWorld->SetFogCulling(abX);
}

void iEdScnWorld::SetFogColor(const cColor& aX)
{
	mpWorld->SetFogColor(aX);
}

void iEdScnWorld::SetFogStart(float afX)
{
	mpWorld->SetFogStart(afX);
}

void iEdScnWorld::SetFogEnd(float afX)
{
	mpWorld->SetFogEnd(afX);
}

void iEdScnWorld::SetFogFalloffExp(float afX)
{
	mpWorld->SetFogFalloffExp(afX);
}

//-------------------------------------------------------------------

void iEdScnWorld::SetShowSkybox(bool abX)
{
	mbShowSkybox = abX;
	mbSkyboxUpdated = true;

	UpdateSkybox();
}

void iEdScnWorld::SetSkyboxActive(bool abX)
{
	mbSkyboxActive = abX;
	mbSkyboxUpdated = true;

	UpdateSkybox();
}

void iEdScnWorld::SetSkyboxColor(const cColor& aX)
{
	mSkyboxColor = aX;
	mbSkyboxUpdated = true;

	UpdateSkybox();
}

void iEdScnWorld::SetSkyboxTexture(const tString& asX)
{
	msSkyboxTexture = asX;
	mbSkyboxUpdated = true;

	UpdateSkybox();
}

//-------------------------------------------------------------------

int iEdScnWorld::GetFreeClipPlaneID()
{
	return ++mlClipPlaneCounter;
}

cEdClipPlane* iEdScnWorld::AddClipPlane(int alID)
{
	if(GetClipPlane(alID)!=NULL)
		return NULL;

	cEdClipPlane* pPlane = hplNew(cEdClipPlane,(this, alID));
	mmapClipPlanes.insert(tClipPlaneMap::value_type(alID, pPlane));
	mbNumClipPlanesChanged = true;
	return pPlane;
}

void iEdScnWorld::RemoveClipPlane(int alID)
{
	cEdClipPlane* pPlane = GetClipPlane(alID);
	if(pPlane)
	{
		mbNumClipPlanesChanged = true;

		mmapClipPlanes.erase(alID);
		hplDelete(pPlane);
	}
}

const tClipPlaneMap& iEdScnWorld::GetClipPlanes()
{
	return mmapClipPlanes;
}

cEdClipPlane* iEdScnWorld::GetClipPlane(int alID)
{
	tClipPlaneMapIt it = mmapClipPlanes.find(alID);

	if(it!=mmapClipPlanes.end())
		return it->second;

	return NULL;
}

//-------------------------------------------------------------------

iEdAction* iEdScnWorld::CreateSetFogBoolAction(eFogBoolProp aProp, bool abX)
{
	return hplNew(cEdActionWorldSetFogBool,(this, aProp, abX));
}

iEdAction* iEdScnWorld::CreateSetFogFloatAction(eFogFloatProp aProp, float afX)
{
	return hplNew(cEdActionWorldSetFogFloat,(this, aProp, afX));
}

iEdAction* iEdScnWorld::CreateSetFogColorAction(const cColor& aX)
{
	return hplNew(cEdActionWorldSetFogColor,(this, aX));
}

iEdAction* iEdScnWorld::CreateSetSkyboxActiveAction(bool abX)
{
	return hplNew(cEdActionWorldSetSkyboxActive,(this, abX));
}

iEdAction* iEdScnWorld::CreateSetSkyboxColorAction(const cColor& aX)
{
	return hplNew(cEdActionWorldSetSkyboxColor,(this, aX));
}

iEdAction* iEdScnWorld::CreateSetSkyboxTextureAction(const tString& asX)
{
	return hplNew(cEdActionWorldSetSkyboxTexture,(this, asX));
}

//-------------------------------------------------------------------

iEdAction* iEdScnWorld::CreateAddClipPlaneAction()
{
	return hplNew(cEdActionClipPlaneAdd,(this));
}

iEdAction* iEdScnWorld::CreateRemoveClipPlaneAction(int alID)
{
	return hplNew(cEdActionClipPlaneRemove,(this, alID));
}

//-------------------------------------------------------------------

cEdScnObjCategory* iEdScnWorld::CreateCategory(const tString& asName)
{
	cEdScnObjCategory* pCat = GetCategory(asName);
	if(pCat)
		return pCat;

	pCat = hplNew(cEdScnObjCategory,(asName));
	mvCategories.push_back(pCat);

	return pCat;
}

cEdScnObjCategory* iEdScnWorld::GetCategory(const tString& asName)
{
	for(size_t i=0; i<mvCategories.size(); ++i)
	{
		cEdScnObjCategory* pCat = mvCategories[i];

		if(pCat->GetName()==asName)
			return pCat;
	}

	return NULL;
}

bool iEdScnWorld::LoadObjects(cXmlElement* apParentElement, tScnObjList& alstObjects)
{
	alstObjects.clear();

	for(size_t i=0;i<mvCategories.size(); ++i)
	{
		cEdScnObjCategory* pCat = mvCategories[i];
		pCat->PreObjectsLoad(apParentElement);
	}

	for(size_t i=0;i<mvCategories.size(); ++i)
	{
		cEdScnObjCategory* pCat = mvCategories[i];
		pCat->Load(this, alstObjects);
	}

	for(size_t i=0;i<mvCategories.size(); ++i)
	{
		cEdScnObjCategory* pCat = mvCategories[i];
		pCat->PostObjectsLoad();
	}

	return true;
}

bool iEdScnWorld::SaveObjects(cXmlElement* apParentElement, const tScnObjList& alstObjects)
{
	bool bSuccess = true;

	for(size_t i=0;i<mvCategories.size(); ++i)
	{
		cEdScnObjCategory* pCat = mvCategories[i];

		pCat->PreObjectsSave(apParentElement);
	}

	tScnObjList::const_iterator it = alstObjects.begin();
	for(;it!=alstObjects.end(); ++it)
	{
		iEdScnObject* pObj = static_cast<iEdScnObject*>(*it);

		cEdScnObjCategory* pCat = pObj->GetCategory();
		if(pObj->Save(pCat->GetElement())==false)
		{
			Log("Error saving object '%ls'\n", pObj->GetName().c_str());
			bSuccess = false;
		}
	}

	for(size_t i=0;i<mvCategories.size(); ++i)
	{
		cEdScnObjCategory* pCat = mvCategories[i];

		pCat->PostObjectsSave();
	}

	return bSuccess;
}

//-------------------------------------------------------------------

void iEdScnWorld::CreateImportDialog()
{
	mpEditor->GetWindowHandler()->CreateLoadDialog(mvDestFilenames, msLastUsedPath, 
													this, kGuiCallback(ImportObjectsCallback), 
													_W("Exported Objects"), tWStringList(1,_W("*.expobj")));
}

void iEdScnWorld::CreateExportDialog()
{
	cEdEditModeSelect* pSelect = static_cast<cEdEditModeSelect*>(mpEditor->GetEditModeHandler()->GetMode(_W("Select")));
	if(pSelect->GetSelection()->HasComponents()==false)
		return;

	mpEditor->GetWindowHandler()->CreateSaveDialog(msExportFilename, msLastUsedPath, 
													this, kGuiCallback(ExportObjectsCallback), 
													_W("Exported Objects"), _W("*.expobj"));
}


void iEdScnWorld::ImportObjects(const tWString& asX, tScnObjList& alstObjects)
{
	cResources* pRes = mpEditor->GetEngine()->GetResources();
	iXmlDocument* pDoc = pRes->LoadXmlDocument(cString::To8Char(asX));
	if(pDoc==NULL)
		return;

	if(pDoc->GetValue()!="ExportedMapObjects")
	{
		pRes->DestroyXmlDocument(pDoc);

		return;
	}

	std::map<cFileIndex*, tIDRedirectMap> mapFileIndexRedirectors;
	tIDRedirectMap mapObjectRedirector;

	tFileIndexMap::iterator it = mmapFileIndices.begin();
	for(;it!=mmapFileIndices.end();++it)
	{
		cFileIndex* pIndex = it->second;
		pIndex->Import(pDoc);
	}

	LoadObjects(pDoc, alstObjects);

	tScnObjListIt itObjects = alstObjects.begin();
	for(;itObjects!=alstObjects.end();++itObjects)
	{
		iEdScnObject* pObj = *itObjects;
		pObj->PostDeployAll(true);
	}

	it = mmapFileIndices.begin();
	for(;it!=mmapFileIndices.end();++it)
	{
		cFileIndex* pIndex = it->second;
		pIndex->PostImport();
	}
	
	//LoadObjects(pDoc);

	/*
	for(size_t i=0;i<vIndexStrings.size();++i)
	{
		tString& sIndex = vIndexStrings[i];
		cXmlElement* pIndex = pXmlObjectsData->GetFirstElement("FileIndex_" + sIndex);
		if(pIndex)
		{
			cXmlNodeListIterator it = pIndex->GetChildIterator();
			tIDRedirector& mapRedirector = mapFileIndexRedirectors[vElementStrings[i]];
			tStringVec& vFiles = mmapFileIndices[sIndex];

			while(it.HasNext())
			{
				cXmlElement* pFile = it.Next()->ToElement();
				int lOldIdx = pFile->GetAttributeInt("Id");
				tString sFilename = GetFileRelativePath(pFile->GetAttributeString("Path"));
				int lNewIdx = AddFilenameToIndex(vFiles, sFilename);
				mapRedirector.insert(std::pair<int,int>(lOldIdx, lNewIdx));
			}
		}
	}
	*/
	
	/*
	for(int i=0; i<(int)mvEntityCategories.size(); ++i)
	{
		//////////////////////////////////////////////////////////////////
		// Iterate through category strings and all entities in each
		cXmlElement* pCurrentXmlCategory = pXmlObjectsData->GetFirstElement(mvEntityCategories[i]);

		if(pCurrentXmlCategory==NULL)
			continue;

		cXmlNodeListIterator it = pCurrentXmlCategory->GetChildIterator();
		while(it.HasNext())
		{
			iEntityWrapper* pEnt = NULL;
			cXmlElement* pXmlEntity = it.Next()->ToElement();
			tString sVal = pXmlEntity->GetValue();

			int lFileIndex=pXmlEntity->GetAttributeInt("FileIndex",-1);
			if(lFileIndex!=-1)
				pXmlEntity->SetAttributeInt("FileIndex", mapFileIndexRedirectors[sVal][lFileIndex]);

			int lNewID = GetFreeID();
			int lID = pXmlEntity->GetAttributeInt("ID", lNewID);
			tString sName = pXmlEntity->GetAttributeString("Name", "");
			if(IsNameAvailable(sName)==false)
				pXmlEntity->SetAttributeString("Name", GenerateName(sName));

			mapIDRedirectors.insert(std::pair<int, int>(lID, lNewID));
			pXmlEntity->SetAttributeInt("ID", lNewID);

			pEnt = CreateEntityWrapperFromXMLElement(pXmlEntity);
			alstImportedIDs.push_back(lNewID);
		}
	}

	tIntListIt it = alstImportedIDs.begin();
	for(;it!=alstImportedIDs.end();++it)
	{
		iEntityWrapper* pEnt = GetEntity(*it);
		//pEnt->PostCreateAllLoadedObjects();
		//pEnt->PostCreationCleanUp();
	}
	*/

	pRes->DestroyXmlDocument(pDoc);
}


void iEdScnWorld::ExportObjects(const tWString& asFile, const tScnObjList& alstObjects)
{
	iXmlDocument* pDoc = mpEditor->GetEngine()->GetResources()->GetLowLevel()->CreateXmlDocument("ExportedMapObjects");
	SaveFileIndices(pDoc);
	SaveObjects(pDoc, alstObjects);

	pDoc->SaveToFile(asFile);
}

//-------------------------------------------------------------------

bool iEdScnWorld::ImportObjectsCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		cEdEditModeSelect* pSelect = static_cast<cEdEditModeSelect*>(mpEditor->GetEditModeHandler()->GetMode(_W("Select")));
		tScnObjList lstImportedObjects;
		ImportObjects(mvDestFilenames.front(), lstImportedObjects);

		mpEditor->AddAction(pSelect->GetSelection()->CreateSelectObjectsAction(lstImportedObjects));
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(iEdScnWorld, ImportObjectsCallback);

bool iEdScnWorld::ExportObjectsCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		cEdEditModeSelect* pSelect = static_cast<cEdEditModeSelect*>(mpEditor->GetEditModeHandler()->GetMode(_W("Select")));
		ExportObjects(msExportFilename, pSelect->GetSelection()->GetComponents());
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(iEdScnWorld, ExportObjectsCallback);

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void iEdScnWorld::OnReset()
{
	iEdWorld::OnReset();

	SetShowFog(true);
	SetFogActive(false);
	SetFogCulling(true);
	SetFogStart(0);
	SetFogEnd(20);
	SetFogFalloffExp(1);
	SetFogColor(cColor(1));

	SetShowSkybox(true);
	SetSkyboxTexture("");
	SetSkyboxColor(cColor(1));
	SetSkyboxActive(false);

	STLMapDeleteAll(mmapClipPlanes);
	mlClipPlaneCounter = -1;
}

//-------------------------------------------------------------------

void iEdScnWorld::OnUpdate()
{
	iEdWorld::OnUpdate();

	if(mbNumClipPlanesChanged)
	{
		mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldParamsChange, NULL);
		mbNumClipPlanesChanged = false;
	}
}

//-------------------------------------------------------------------

void iEdScnWorld::OnDraw(const cModuleDrawData& aData)
{
	iEdWorld::OnDraw(aData);

	tClipPlaneMapIt it = mmapClipPlanes.begin();
	for(;it!=mmapClipPlanes.end(); ++it)
	{
		cEdClipPlane* pPlane = it->second;
		pPlane->Draw(aData.mpFunctions, aData.mpViewport->GetGridCenter());
	}
}

//-------------------------------------------------------------------

void iEdScnWorld::OnViewportMouseMove()
{
	mpSurfacePicker->SetUpdated();
}

//-------------------------------------------------------------------

void iEdScnWorld::UpdateSkybox()
{
	if(mbSkyboxUpdated)
	{
		mbSkyboxUpdated = false;
		iTexture* pTex = NULL;
		cColor col = mBGColor;

		if(GetSkyboxActive() && mbShowSkybox)
		{
			cEdLoader::LoadCubeMap(GetSkyboxTexture(), &pTex);
			col = GetSkyboxColor();
		}

		mpWorld->SetSkyBox(NULL, false);
		mpWorld->SetSkyBoxColor(col);
	}
}

//-------------------------------------------------------------------
