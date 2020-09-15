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

#include "../Common/EdObjEntity.h"

#include "../Common/Editor.h"
#include "../Common/EdScnWorld.h"
#include "../Common/EdSurfacePicker.h"

#include "../Common/EdEntityLoader.h"
#include "../Common/EdEngineObject.h"

#include "../Common/EdEditPaneEntity.h"
#include "../Common/EdWindowViewport.h"

//#include "EditorHelper.h"

//#include "../Common/EdEditModeEntities.h"
#include "../Common/EdVar.h"

//#include "EditorActionDynamicEntity.h"


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// LOADED MESH AGGREGATE ENTITY - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEngineLoadedMeshAggregate::cEngineLoadedMeshAggregate(iEdScnObject* apParent, const tString& asFilename) : iEngineMesh(apParent)
{
	msFilename = asFilename;
	mbLightsActive = true;
	mbParticleSystemsActive = true;
}

cEngineLoadedMeshAggregate::~cEngineLoadedMeshAggregate()
{
	cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();

	for(int i=0;i<(int)mvLights.size();++i)
		pWorld->DestroyLight(mvLights[i]);

	for(int i=0;i<(int)mvBillboards.size();++i)
		pWorld->DestroyBillboard(mvBillboards[i]);

	for(int i=0;i<(int)mvParticleSystems.size();++i)
		pWorld->DestroyParticleSystem(mvParticleSystems[i]);

	for(int i=0;i<(int)mvSounds.size();++i)
		pWorld->DestroySoundEntity(mvSounds[i]);	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// MESH AGGREGATE ENTITY - PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cEngineLoadedMeshAggregate::Create(const tString& asName)
{
	cEdObjEntity* pParent = static_cast<cEdObjEntity*>(mpParent);
	cTypeEntity* pType = static_cast<cTypeEntity*>(pParent->GetType());
	cTypeRootEntity* pEntType = static_cast<cTypeRootEntity*>(pType->GetUserTypeRoot());

	cEdEntityLoader* pLoader = pEntType->GetEntityLoader();

	iEdWorld* pWorld = mpParent->GetWorld();

	mpObject = pLoader->LoadEntFile(mpParent->GetID(), asName, msFilename, pWorld->GetEngWorld(), false, true, true, false, true);
	mvLights = pLoader->GetLights();
	mvBillboards = pLoader->GetBillboards();
	mvParticleSystems = pLoader->GetParticleSystems();
	mvSounds = pLoader->GetSounds();

	for(size_t i=0;i<mvLights.size();++i)
		mpObject->AddChild(mvLights[i]);
	for(size_t i=0;i<mvBillboards.size();++i)
		mpObject->AddChild(mvBillboards[i]);
	for(size_t i=0;i<mvParticleSystems.size();++i)
		mpObject->AddChild(mvParticleSystems[i]);
	for(size_t i=0;i<mvSounds.size();++i)
		mpObject->AddChild(mvSounds[i]);

	SetUpMesh();

	return IsCreated();
}

//-----------------------------------------------------------------------

void cEngineLoadedMeshAggregate::Update()
{
	iEngineMesh::Update();

	bool bEnabled = mpParent->IsEnabled();
	bool bVisible = mpParent->IsVisible();

	iEdWorld* pWorld = mpParent->GetWorld();
	//bool bLightsVisible = pWorld->GetTypeVisibility(eEdEntityType_Light);
	//bool bPSVisible = pWorld->GetTypeVisibility(eEdEntityType_ParticleSystem);

	//for(int i=0;i<(int)mvLights.size();++i)
	//	mvLights[i]->SetVisible(mbLightsActive && bLightsVisible && bActive && bVisible);
	//for(int i=0;i<(int)mvParticleSystems.size();++i)
	//	mvParticleSystems[i]->SetVisible(mbParticleSystemsActive && bPSVisible && bActive && bVisible);
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ENTITY ROOT TYPE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cFileIndex* cTypeRootEntity::mpIndex = NULL;

//---------------------------------------------------------------------------

cTypeRootEntity::cTypeRootEntity() : iUserTypeRoot(_W("Entity"), "Entity", "EntityClasses.def")
{
	// Properties
	AddBool(eScnBool_Active, "Active");
	AddInt(eEntityInt_FileIndex, "FileIndex", -1, ePropStep_PreCreate);
	AddString(eEntityStr_Filename, "Filename", "", ePropStep_PreCreate, false);

	RegisterVarCatName(eEdVarCategory_Editor, "EditorSetupVars");
	RegisterVarCatName(eEdVarCategory_Type, "TypeVars");
	RegisterVarCatName(eEdVarCategory_Instance, "InstanceVars");

	mpEntityLoader = NULL;
}

cTypeRootEntity::~cTypeRootEntity()
{
	if(mpEntityLoader) hplDelete(mpEntityLoader);
}



//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ENTITY ROOT TYPE - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cTypeRootEntity::Init()
{
	mpEntityLoader = hplNew(cEdEntityLoader,(mpWorld->GetEditor()->GetEngine()->GetResources()));
	bool bInit = iUserTypeRoot::Init();

	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(GetWorld());
	cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();
	pPicker->RegisterSurfaceType(this);

	if(mpIndex==NULL)
		mpIndex = pWorld->CreateFileIndex("Entities");

	return bInit;
}

//---------------------------------------------------------------------------

iEdObjectType* cTypeRootEntity::GetTypeByXmlElement(cXmlElement* apElem)
{
	if(iUserTypeRoot::GetTypeByXmlElement(apElem)==NULL)
		return NULL;

	int lFileIndex = apElem->GetAttributeInt("FileIndex");
	cIndexedFile* pFile = mpIndex->GetIndexedFile(lFileIndex);

	iEdObjectType* pType = GetTypeFromEntFile(pFile->msRelativePath);

	return pType;
}

//---------------------------------------------------------------------------

iEdObjectType* cTypeRootEntity::GetTypeFromEntFile(const tString& asFile)
{
	tString sEntityType = cString::To8Char(GetName());

	cResources* pRes = GetWorld()->GetEditor()->GetEngine()->GetResources();
	iXmlDocument* pModelDoc = pRes->LoadXmlDocument(asFile);
	if(pModelDoc==NULL)
		return NULL;

	cXmlElement* pUserVars = pModelDoc->GetFirstElement("UserDefinedVariables");
	if(pUserVars)
	{
		tString sType = pUserVars->GetAttributeString("EntityType");
		
		if(sType.empty()==false)
			sEntityType += "." + sType;

		tString sSubType = pUserVars->GetAttributeString("EntitySubType");
		if(sSubType.empty()==false)
			sEntityType += "." + sSubType;
	}
	pRes->DestroyXmlDocument(pModelDoc);

	return GetTypeByName(cString::To16Char(sEntityType));
	/*
	if(sEntityType.empty())
	{
		cTypeEntity* pType = static_cast<cTypeEntity*>
		cEdObjTypeEntity* pType = (cEdObjTypeEntity*)iEdEditModeObjectCreator::GetType(0);

		Log("Using type %s for typeless entity file %s\n", pType->ToString().c_str(), asFile.c_str());
		return pType;
		
	}

	/*
	for(int i=0;i<GetTypeNum();++i)
	{
		cEdObjTypeEntity* pType = (cEdObjTypeEntity*)iEdEditModeObjectCreator::GetType(i);
		cEdUserClassSubType *pUserType = pType->GetUserType();
		cEdUserClassType* pUserTypeParent = pUserType->GetParent();
		if(pUserTypeParent->GetName()==sType)
		{
			if(pUserType->GetName()==sSubType)
				return pType;
			else
			{
				if(pUserTypeParent->GetSubTypeNum()==1)
				{
					Log("Using type %s for badly saved file %s\n", pType->ToString().c_str(), asFile.c_str());
					return pType;
				}
			}
		}
	}

	Error("EditModeEntities : Could not find type %s %s, either %s is badly saved, or was saved using a different EntityTypes.cfg file.", sType.c_str(), sSubType.c_str(), asFile.c_str());
	cEdObjTypeEntity* pType = (cEdObjTypeEntity*)iEdEditModeObjectCreator::GetType(0);

	Log("Using type %s instead\n", pType->ToString().c_str(), asFile.c_str());
	return pType;

	//return NULL;
	*/
}


//---------------------------------------------------------------------------

iUserTypeNode* cTypeRootEntity::CreateSpecificUserTypeNode(iEdClass* apClass)
{
	return hplNew(cTypeEntity,(this, apClass));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ENTITY TYPE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cTypeEntity::cTypeEntity(iUserTypeRoot* apRoot, iEdClass* apClass) : iUserTypeNode(apRoot, apClass)
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ENTITY TYPE - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

iEdVar* cTypeEntity::GetLinkedEditorSetupVar(const tWString& asX, eVariableType aType)
{
	cEdUserClass* pClass = static_cast<cEdUserClass*>(mpClass);
	iEdVar* pVar = pClass->GetVariable(eEdVarCategory_Editor, asX);
	iEdVar* pLinkedVar = NULL;
	if(pVar)
		pLinkedVar = pClass->GetVariable(eEdVarCategory_Instance, pVar->GetDefaultValue());

	if(pLinkedVar && pLinkedVar->GetType()==aType)
		return pLinkedVar;

	//Log("This is baaad\n");
	return NULL;
}

//------------------------------------------------------------------------------

iEdObjectData* cTypeEntity::CreateTypeSpecificData() 
{
	return hplNew(cEdObjDataEntity,(this)); 
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ENTITY DATA - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjDataEntity::cEdObjDataEntity(iEdObjectType* apType) : iUserObjData(apType)
{
}

cEdObjDataEntity::~cEdObjDataEntity()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ENTITY DATA - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------


void cEdObjDataEntity::CopyToObject(iEdObject* apObj, ePropStep aOrder)
{
	iUserObjData::CopyToObject(apObj, aOrder);
}

//---------------------------------------------------------------------------

void cEdObjDataEntity::CopyFromObject(iEdObject* apObj)
{
	iUserObjData::CopyFromObject(apObj);

	cEdObjEntity* pObj = static_cast<cEdObjEntity*>(apObj);

	mpEntFile = pObj->GetEntFile();

	SetInt(eEntityInt_FileIndex, mpEntFile?mpEntFile->mlID:-1);
	//SetString(eEntityStr_Filename, mpEntFile->msRelativePath);
}

bool cEdObjDataEntity::LoadSpecificData(cXmlElement* apElement)
{
	if(iUserObjData::LoadSpecificData(apElement)==false)
		return false;

	iEdWorld* pWorld = mpType->GetWorld();
	int lFileIndex = GetInt(eEntityInt_FileIndex);
	if(lFileIndex==-1)
	{
		mpEntFile = cTypeRootEntity::mpIndex->CreateIndexedFile(apElement->GetAttributeString("Filename"));
		SetInt(eEntityInt_FileIndex, mpEntFile->mlID);
	}
	else
		mpEntFile = cTypeRootEntity::mpIndex->GetIndexedFile(lFileIndex);

	if(mpEntFile==NULL)
	{
		//pWorld->SetLoadErrorMessage(_W("Mesh file not found!"));
		return false;
	}

	SetString(eEntityStr_Filename, mpEntFile->msRelativePath);

	return true;
}

//---------------------------------------------------------------------------

iEdObject* cEdObjDataEntity::CreateTypeSpecificObject()
{
	/*
	/////////////////////////////////////////////////////////////////////////////////////////
	// Check so the File Index is still valid ! (can become invalid after undoing a delete
	iEdWorld* pWorld = mpType->GetWorld();

	int lFileIndex = GetInt(eEntityInt_FileIndex);
	const tString& sCurrentFile = GetString(eEntityStr_Filename);
	tWString sPath = pWorld->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(sCurrentFile);
	int lFileIndexForCurrentFile = pWorld->AddFilenameToIndex("Entities", cString::To8Char(sPath));

	// the If first index and the one we just retrieved are different, update first
	if(lFileIndex!=lFileIndexForCurrentFile)
		SetInt(eEntityInt_FileIndex, lFileIndexForCurrentFile);
	*/

	return hplNew(cEdObjEntity,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ENTITY OBJECT - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjEntity::cEdObjEntity(iEdObjectData* apData) : iUserObject(apData)												
{
	mbAffectedByDecal = true;
	mpEntFile = NULL;
}

cEdObjEntity::~cEdObjEntity()
{
	SetEntFile(NULL);
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ENTITY OBJECT - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEdObjEntity::GetProperty(int alPropID, int& alX)
{
	switch(alPropID)
	{
	case eEntityInt_FileIndex:
		alX = mpEntFile==NULL?-1:mpEntFile->mlID;
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, alX);
	}

	return true;
}

bool cEdObjEntity::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eEntityStr_Filename:
		asX = mpEntFile==NULL?"" : mpEntFile->msRelativePath;
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, asX);
	}

	return true;
}

bool cEdObjEntity::SetProperty(int alPropID, const int alX)
{
	switch(alPropID)
	{
	case eEntityInt_FileIndex:
		//SetFileIndex(alX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, alX);
	}

	return true;
}

bool cEdObjEntity::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eEntityStr_Filename:
		SetFilename(asX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, asX);
	}

	return true;
}

//--------------------------------------------------------------------------- 

/*
iEdAction* cEdObjEntity::CreateSetStringAction(int alPropID, const tString& asX)
{
	if(alPropID==eEntityStr_Filename)
	{
		return hplNew(cEdActionEntitySetFilename,(GetEdWorld(), mlID, asX));
	}
	else
		return iEdObject::CreateSetStringAction(alPropID, asX);
}
*/

//---------------------------------------------------------------------------

void cEdObjEntity::OnSetVar(const tWString& asName, const tWString& asValue)
{
	cTypeEntity* pType = static_cast<cTypeEntity*>(mpType);

	cEngineLoadedMeshAggregate* pObj = static_cast<cEngineLoadedMeshAggregate*>(mpEngObject);
	bool bValue = cString::ToBool(cString::To8Char(asValue).c_str(), false);

	iEdVar* pAffectShadows = pType->GetLinkedEditorSetupVar(_W("AffectShadowsVar"), eVariableType_Bool);
	iEdVar* pAffectLights = pType->GetLinkedEditorSetupVar(_W("AffectLightsVar"), eVariableType_Bool);
	iEdVar* pAffectParticles = pType->GetLinkedEditorSetupVar(_W("AffectParticlesVar"), eVariableType_Bool);
	iEdVar* pAffectBillboards = pType->GetLinkedEditorSetupVar(_W("AffectBillboardsVar"), eVariableType_Bool);
	iEdVar* pIsAffectedByDecal = pType->GetLinkedEditorSetupVar(_W("AffectedByDecalVar"), eVariableType_Bool);

	bool bUpdateEnt = false;
	if(pAffectShadows && asName==pAffectShadows->GetName())
	{
		bUpdateEnt = true;
		pObj->SetCastShadows(bValue);
	}
	if(pAffectLights && asName==pAffectLights->GetName())
	{
		bUpdateEnt = true;
		pObj->SetLightsActive(bValue);
	}
	if(pAffectParticles && asName==pAffectParticles->GetName())
	{
		bUpdateEnt = true;
		pObj->SetParticlesActive(bValue);
	}
	if(pAffectBillboards && asName==pAffectBillboards->GetName())
	{
		bUpdateEnt = true;
		pObj->SetBillboardsActive(bValue);
	}
	if(pIsAffectedByDecal && asName==pIsAffectedByDecal->GetName())
	{
		mbAffectedByDecal = bValue;
	}

	if(bUpdateEnt) pObj->Update();
}

void cEdObjEntity::OnSetActive(bool abX)
{
	mpEngObject->Update();
}

//---------------------------------------------------------------------------

void cEdObjEntity::SetEntFile(cIndexedFile* apRes)
{
	if(mpEntFile==apRes)
		return;

	if(mpEntFile) mpEntFile->Release();

	mpEntFile = apRes;

	if(mpEntFile) mpEntFile->Assign();
}

//---------------------------------------------------------------------------

void cEdObjEntity::SetFilename(const tString& asX)
{
	if(mpEntFile && mpEntFile->msRelativePath==asX)
		return;

	cTypeRootEntity* pRootType = static_cast<cTypeRootEntity*>(GetMainType());
	iEdObjectType* pNewType = pRootType->GetTypeFromEntFile(asX);

	cIndexedFile* pTemp = cTypeRootEntity::mpIndex->CreateIndexedFile(asX);

	SetEntFile(pTemp);

	if(mpEngObject)
	{
		hplDelete(mpEngObject);
		mpEngObject=NULL;
	}

	SetUpEngineObject();
	SetEntityType(pNewType);
}

//---------------------------------------------------------------------------

iEdEditPane* cEdObjEntity::CreateEditPane()
{
	return hplNew(cEdEditPaneEntity,(this));
}

//---------------------------------------------------------------------------

bool cEdObjEntity::IsAffectedByDecal(cEdSurfacePicker* apPicker)
{
	if(iEdScnObject::IsAffectedByDecal(apPicker)==false)
		return false;

	return mbAffectedByDecal;
}

//---------------------------------------------------------------------------

iEngineObject* cEdObjEntity::CreateEngineObject()
{
	if(mpEntFile==NULL)
		return NULL;

	return hplNew(cEngineLoadedMeshAggregate,(this, mpEntFile->msRelativePath));
	
	/*
	if(msFilename=="")
	{
/*		if(mlFileIndex!=-1)
		{
			msFilename = pWorld->GetFilenameFromIndex("Entities",mlFileIndex);
			mpData->SetString(eEntityStr_Filename, msFilename);
		}
		else
			Log("Error creating Entity named %s, ID %d\n", msName.c_str(), mlID);
	}
	*/
	
	//return hplNew(cEngineLoadedMeshAggregate,(this, msFilename));
}

//---------------------------------------------------------------------------

bool cEdObjEntity::SetEntityType(iEdObjectType* apType)
{
	if(apType==NULL || mpType==apType)
		return false;

	mpType->RemoveObject(this);

	mbTypeChanged = true;

	mpType = apType;
	mpType->AddObject(this);

	iEdObjectData* pData = mpType->CreateData();
	pData->CopyToObject(this, ePropStep_PostCreate);
	hplDelete(pData);

	return true;
}

//---------------------------------------------------------------------------
