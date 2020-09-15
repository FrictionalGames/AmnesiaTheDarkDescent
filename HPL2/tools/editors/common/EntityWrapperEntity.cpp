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

#include "EntityWrapperEntity.h"

#include "EditorBaseClasses.h"
#include "EditorWorld.h"

#include "EditorWindowEntityEditBoxEntity.h"

#include "EditorWindowViewport.h"

#include "EngineEntity.h"
#include "EditorHelper.h"

#include "EditorEditModeEntities.h"

#include "EditorVar.h"

#include "EditorActionDynamicEntity.h"

//---------------------------------------------------------------------------

tString cEntityWrapperTypeEntity::msLastCheckedFile = "";
tString cEntityWrapperTypeEntity::msLastCheckedType = "";
tString cEntityWrapperTypeEntity::msLastCheckedSubType = "";

//---------------------------------------------------------------------------

cEntityWrapperTypeEntity::cEntityWrapperTypeEntity(cEditorUserClassSubType* apType) : iEntityWrapperTypeUserDefinedEntity(eEditorEntityType_Entity, _W("Entity"), "Entity", apType)
{
	AddInt(eEntityInt_FileIndex, "FileIndex", -1, ePropCopyStep_PreEnt);
	AddString(eEntityStr_Filename, "Filename", "", ePropCopyStep_PreEnt, false);
}

//---------------------------------------------------------------------------

tString cEntityWrapperTypeEntity::ToString()
{
	tString sSubType = GetUserSubTypeName();
	sSubType = (sSubType=="")?"" : "," + sSubType;
	return iEntityWrapperType::ToString() + " " + GetUserTypeName() + sSubType;
}

//------------------------------------------------------------------------------

bool cEntityWrapperTypeEntity::IsAppropriateType(cXmlElement* apElement)
{
	if(iEntityWrapperType::IsAppropriateType(apElement)==false)
		return false;

	cResources* pRes = mpWorld->GetEditor()->GetEngine()->GetResources();

	int lFileIndex = apElement->GetAttributeInt("FileIndex", -1);
	tString sFilename;

	if(lFileIndex==-1)
	{
		sFilename = apElement->GetAttributeString("Filename");
		lFileIndex = mpWorld->AddFilenameToIndex("Entities", cString::To8Char(pRes->GetFileSearcher()->GetFilePath(sFilename)));
	}

	sFilename = mpWorld->GetFilenameFromIndex("Entities", lFileIndex);
	if(msLastCheckedFile!=sFilename)
	{
		iXmlDocument* pModelDoc = pRes->LoadXmlDocument(sFilename);
		if(pModelDoc)
		{
			cXmlElement* pUserVars = pModelDoc->GetFirstElement("UserDefinedVariables");
			if(pUserVars)
			{
				msLastCheckedFile = sFilename;
				msLastCheckedType = pUserVars->GetAttributeString("EntityType");
				msLastCheckedSubType = pUserVars->GetAttributeString("EntitySubType");
			}

			pRes->DestroyXmlDocument(pModelDoc);
		}
	}

	const tString& sType = GetUserTypeName();
	const tString& sSubType = GetUserSubTypeName();

	if(sType==msLastCheckedType)
	{
		if(sSubType!="")
		{
			if(sSubType==msLastCheckedSubType)
				return true;
		}
		else
		{
			if(msLastCheckedSubType!="")
				Log("Inconsistency found in file %s : no subtype %s in type %s\n", sFilename.c_str(), msLastCheckedSubType.c_str(), sType.c_str());
				
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------

bool cEntityWrapperTypeEntity::IsAppropriateDefaultType(cXmlElement* apElement)
{
	if(iEntityWrapperType::IsAppropriateType(apElement)==false)
		return false;

	cResources* pRes = mpWorld->GetEditor()->GetEngine()->GetResources();

	int lFileIndex = apElement->GetAttributeInt("FileIndex", -1);
	tString sFilename;

	if(lFileIndex==-1)
	{
		sFilename = apElement->GetAttributeString("Filename");
		lFileIndex = mpWorld->AddFilenameToIndex("Entities", cString::To8Char(pRes->GetFileSearcher()->GetFilePath(sFilename)));
	}

	sFilename = mpWorld->GetFilenameFromIndex("Entities", lFileIndex);
	if(msLastCheckedFile!=sFilename)
	{
		iXmlDocument* pModelDoc = pRes->LoadXmlDocument(sFilename);
		if(pModelDoc)
		{
			cXmlElement* pUserVars = pModelDoc->GetFirstElement("UserDefinedVariables");
			if(pUserVars)
			{
				msLastCheckedFile = sFilename;
				msLastCheckedType = pUserVars->GetAttributeString("EntityType");
				msLastCheckedSubType = pUserVars->GetAttributeString("EntitySubType");
			}

			pRes->DestroyXmlDocument(pModelDoc);
		}
	}

	if(mpUserType->GetDefinition()->GetType(msLastCheckedType)==NULL)
	{	
		mpWorld->SetShowLoadErrorPopUp();
		Log("Inconsistency found in file %s : no entity type %s is defined\n", sFilename.c_str(), msLastCheckedSubType.c_str());
		return true;
	}

	const tString& sType = GetUserTypeName();

	return sType==msLastCheckedType && mpUserType->IsDefaultSubType();
}

//------------------------------------------------------------------------------

const tString& cEntityWrapperTypeEntity::GetUserTypeName()
{
	return GetUserType()->GetParent()->GetName();
}

//------------------------------------------------------------------------------

const tString& cEntityWrapperTypeEntity::GetUserSubTypeName()
{
	return GetUserType()->GetName();
}

//------------------------------------------------------------------------------

iEditorVar* cEntityWrapperTypeEntity::GetLinkedEditorSetupVar(const tWString& asX, eVariableType aType)
{
	iEditorVar* pVar = GetUserType()->GetVariable(eEditorVarCategory_EditorSetup, asX);
	iEditorVar* pLinkedVar = NULL;
	if(pVar)
		pLinkedVar = GetUserType()->GetVariable(eEditorVarCategory_Instance, pVar->GetDefaultValue());

	if(pLinkedVar && pLinkedVar->GetType()==aType)
		return pLinkedVar;

	//Log("This is baaad\n");
	return NULL;
}

//------------------------------------------------------------------------------

iEntityWrapperData* cEntityWrapperTypeEntity::CreateSpecificData() 
{
	return hplNew(cEntityWrapperDataEntity,(this)); 
}

cEntityWrapperDataEntity::cEntityWrapperDataEntity(iEntityWrapperType* apType) : iEntityWrapperDataUserDefinedEntity(apType, 
																													eUserClassDefinition_Entity)
{
}

cEntityWrapperDataEntity::~cEntityWrapperDataEntity()
{
}

//---------------------------------------------------------------------------

bool cEntityWrapperDataEntity::Load(cXmlElement* apElement)
{
	if(iEntityWrapperDataUserDefinedEntity::Load(apElement)==false)
		return false;

	iEditorWorld* pWorld = mpType->GetWorld();
	int lFileIndex = GetInt(eEntityInt_FileIndex);
	if(lFileIndex==-1)
	{
		tWString sPath = pWorld->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(apElement->GetAttributeString("Filename"));
		lFileIndex = pWorld->AddFilenameToIndex("Entities", cString::To8Char(sPath));
		SetInt(eEntityInt_FileIndex, lFileIndex);
	}

	const tString& sFilename = pWorld->GetFilenameFromIndex("Entities",lFileIndex);
	if(sFilename=="")
	{
		pWorld->SetLoadErrorMessage(_W("File index out of bounds!"));
		return false;
	}

	SetString(eEntityStr_Filename, sFilename);

	return true;
}

//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataEntity::CreateSpecificEntity()
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// Check so the File Index is still valid ! (can become invalid after undoing a delete
	iEditorWorld* pWorld = mpType->GetWorld();

	int lFileIndex = GetInt(eEntityInt_FileIndex);
	const tString& sCurrentFile = GetString(eEntityStr_Filename);
	tWString sPath = pWorld->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(sCurrentFile);
	int lFileIndexForCurrentFile = pWorld->AddFilenameToIndex("Entities", cString::To8Char(sPath));

	// the If first index and the one we just retrieved are different, update first
	if(lFileIndex!=lFileIndexForCurrentFile)
		SetInt(eEntityInt_FileIndex, lFileIndexForCurrentFile);

	return hplNew(cEntityWrapperEntity,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEntityWrapperEntity::cEntityWrapperEntity(iEntityWrapperData* apData) : iEntityWrapperUserDefinedEntity(apData)												
{
	mbAffectedByDecal = true;
	/*
	if(mpMeshEntity)
	{
		if(apLights)
		{
			mvLights = *apLights;
			for(int i=0;i<(int)mvLights.size();++i)
			{
				iLight* pLight = mvLights[i];
				pLight->SetVisible(mpEditorWorld->GetTypeVisibility(eEditorEntityType_Light));
				mvLightLocalTransforms.push_back(pLight->GetWorldMatrix());
			}
		}
		SetCastShadows(false);
	}*/
}

cEntityWrapperEntity::~cEntityWrapperEntity()
{
	cWorld* pWorld = GetEditorWorld()->GetWorld();
	//pWorld->DestroyMeshEntity(mpMeshEntity);
	for(int i=0;i<(int)mvLights.size();++i)
	{
		pWorld->DestroyLight(mvLights[i]);
	}
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEntityWrapperEntity::GetProperty(int alPropID, int& alX)
{
	if(iEntityWrapper::GetProperty(alPropID, alX)==true)
		return true;

	switch(alPropID)
	{
	case eEntityInt_FileIndex:
		alX = GetFileIndex();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperEntity::GetProperty(int alPropID, tString& asX)
{
	if(iEntityWrapper::GetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case eEntityStr_Filename:
		asX = GetFilename();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperEntity::SetProperty(int alPropID, const int& alX)
{
	if(iEntityWrapper::SetProperty(alPropID, alX)==true)
		return true;

	switch(alPropID)
	{
	case eEntityInt_FileIndex:
		SetFileIndex(alX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperEntity::SetProperty(int alPropID, const tString& asX)
{
	if(iEntityWrapper::SetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case eEntityStr_Filename:
		SetFilename(asX);
		break;
	default:
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------- 

bool cEntityWrapperEntity::IsAffectedByDecal(bool abAffectsStaticObject, bool abAffectsPrimitive, bool abAffectsEntity)
{
	return abAffectsEntity && mbAffectedByDecal && 
		IsVisible() && IsActive() && IsCulledByClipPlanes()==false;
}

//---------------------------------------------------------------------------

void cEntityWrapperEntity::UpdateEntity()
{
	iEntityWrapper::UpdateEntity();
}

//---------------------------------------------------------------------------

iEditorAction* cEntityWrapperEntity::CreateSetPropertyActionString(int alPropID, const tString& asX)
{
	if(alPropID==eEntityStr_Filename)
	{
		return hplNew(cEditorActionEntitySetFilename,(GetEditorWorld(), mlID, asX));
	}
	else
		return iEntityWrapper::CreateSetPropertyActionString(alPropID, asX);
}

//---------------------------------------------------------------------------

void cEntityWrapperEntity::OnSetVar(const tWString& asName, const tWString& asValue)
{
	cEntityWrapperTypeEntity* pType = (cEntityWrapperTypeEntity*) mpType;

	// TODO: move this to a external script.
	// OR: check for special parameters in the class
	cEngineEntityLoadedMeshAggregate* pEnt = (cEngineEntityLoadedMeshAggregate*)mpEngineEntity;
	bool bValue = cString::ToBool(cString::To8Char(asValue).c_str(), false);

	iEditorVar* pAffectShadows = pType->GetLinkedEditorSetupVar(_W("AffectShadowsVar"), eVariableType_Bool);
	iEditorVar* pAffectLights = pType->GetLinkedEditorSetupVar(_W("AffectLightsVar"), eVariableType_Bool);
	iEditorVar* pAffectParticles = pType->GetLinkedEditorSetupVar(_W("AffectParticlesVar"), eVariableType_Bool);
	iEditorVar* pAffectBillboards = pType->GetLinkedEditorSetupVar(_W("AffectBillboardsVar"), eVariableType_Bool);
	iEditorVar* pIsAffectedByDecal = pType->GetLinkedEditorSetupVar(_W("AffectedByDecalVar"), eVariableType_Bool);
	
	bool bUpdateEnt = false;
	if(pAffectShadows && asName==pAffectShadows->GetName())
	{
		bUpdateEnt = true;
		pEnt->SetCastShadows(bValue);
	}
	if(pAffectLights && asName==pAffectLights->GetName())
	{
		bUpdateEnt = true;
		pEnt->SetLightsActive(bValue);
	}
	if(pAffectParticles && asName==pAffectParticles->GetName())
	{
		bUpdateEnt = true;
		pEnt->SetParticlesActive(bValue);
	}
	if(pAffectBillboards && asName==pAffectBillboards->GetName())
	{
		bUpdateEnt = true;
		pEnt->SetBillboardsActive(bValue);
	}
	if(pIsAffectedByDecal && asName==pIsAffectedByDecal->GetName())
	{
		mbAffectedByDecal = bValue;
	}

	if(bUpdateEnt) pEnt->Update();
}

void cEntityWrapperEntity::OnSetActive(bool abX)
{
	mpEngineEntity->Update();
}

//---------------------------------------------------------------------------

void cEntityWrapperEntity::SetFilename(const tString& asX)
{
	if(msFilename == asX)
		return;

	cEditorEditModeEntities* pEditMode = (cEditorEditModeEntities*)GetEditorWorld()->GetEditor()->GetEditMode("Entities");
	iEntityWrapperType* pType = pEditMode->GetTypeFromEntFile(asX);
	if(SetEntityType(pType)==false)
		return;

	msFilename = asX;

	if(mpEngineEntity==NULL)
		return;

	iEditorBase* pEditor = GetEditorWorld()->GetEditor();

	tWString sPath = pEditor->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(asX);
	int lFileIndex = GetEditorWorld()->AddFilenameToIndex("Entities", cString::To8Char(sPath));

	SetFileIndex(lFileIndex);

    CreateEngineEntity();
}

//---------------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperEntity::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = hplNew(cEditorWindowEntityEditBoxEntity,(apEditMode,this));

	return pEditBox;
}

//---------------------------------------------------------------------------

iEngineEntity* cEntityWrapperEntity::CreateSpecificEngineEntity()
{
	iEditorWorld* pWorld = GetEditorWorld();
	cResources* pRes = pWorld->GetEditor()->GetEngine()->GetResources();

	if(msFilename=="")
	{
		if(mlFileIndex!=-1)
		{
			msFilename = pWorld->GetFilenameFromIndex("Entities",mlFileIndex);
			mpData->SetString(eEntityStr_Filename, msFilename);
		}
		else
			Log("Error creating Entity named %s, ID %d\n", msName.c_str(), mlID);
	}
	
	return hplNew(cEngineEntityLoadedMeshAggregate,(this, msFilename));
}

//---------------------------------------------------------------------------

bool cEntityWrapperEntity::SetEntityType(iEntityWrapperType* apType)
{
	if(apType==NULL || 
		apType->GetID()!=eEditorEntityType_Entity)
		return false;

	if(mpType==apType)
		return true;

	mbTypeChanged = true;

	mpType = apType;
	iEntityWrapperData* pData = mpType->CreateData();
	pData->CopyToEntity(this, ePropCopyStep_PostEnt);
	hplDelete(pData);

	return true;
}

//---------------------------------------------------------------------------
