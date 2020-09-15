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

#include "../ModelEditor/HPLEntity.h"
#include "../Common/EdScnObject.h"

#include "../Common/Editor.h"

#include "../Common/EdEntityLoader.h"

#include "../Common/EdEditModeHandler.h"
#include "../Common/EdEditModeSelect.h"
#include "../Common/EdSelection.h"
#include "../Common/EdUserClassDefinition.h"
#include "../ModelEditor/EdHplEntityActions.h"

#include "../ModelEditor/EdObjBody.h"
#include "../ModelEditor/EdObjSubMesh.h"
#include "../ModelEditor/EdObjBone.h"

#include "../ModelEditor/EdWindowPhysicsTest.h"

//--------------------------------------------------------------------

int cAnimationEventWrapper::mlIndices = 0;

//--------------------------------------------------------------------

//--------------------------------------------------------------------

cCategoryDataHandlerMesh::cCategoryDataHandlerMesh(cHplEntity* apEnt) : iEdScnObjCategoryDataHandler(true)
{
	mpEnt = apEnt;
	mpCatBone = NULL;
}

void cCategoryDataHandlerMesh::Load(cXmlElement* apElement)
{
	mpEnt->SetMeshFromElement(apElement, mpCatBone->GetElement());
}

void cCategoryDataHandlerMesh::Save(cXmlElement* apElement)
{
	apElement->SetAttributeString("Filename", mpEnt->GetTypeSubMesh()->GetMeshFilename());
}

void cCategoryDataHandlerMesh::SetBoneCategory(cEdScnObjCategory* apCat)
{
	mpCatBone = apCat;
}

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
	if(msType=="" || msValue=="")
		return false;

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

void cAnimationWrapper::Save(cXmlElement* apParent)
{
	cXmlElement* pAnim = apParent->CreateChildElement("Animation");
	pAnim->SetAttributeString("Name",GetName());
	pAnim->SetAttributeString("File",GetFile());
	pAnim->SetAttributeFloat("Speed",GetSpeed());
	pAnim->SetAttributeFloat("SpecialEventTime",GetSpecialEventTime());
	for(size_t i=0;i<mvEvents.size();++i)
	{
		cAnimationEventWrapper* pEvent = &mvEvents[i];
		cXmlElement* pXmlEvent = pAnim->CreateChildElement("Event");
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

/////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cHplEntity::cHplEntity(iEditor* apEditor) : iEdScnWorld(apEditor, _W("HPL Entity"), _W("ent"), "Entity")
{
	mpClassDef = hplNew(cEdUserClassDefinition,("EntityClasses.def"));
	mpClassDef->RegisterVarCatName(eEdVarCategory_Type, "TypeVars");

	mpLoader = NULL;

	mpClassInstance = NULL;
	mpActiveTestWindow = NULL;
}

cHplEntity::~cHplEntity()
{
	hplDelete(mpClassDef);
	hplDelete(mpLoader);

	STLDeleteAll(mvCategories);
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cHplEntity::SetClass(cEdUserClass* apClass)
{
	if(apClass==NULL || mpClassInstance && mpClassInstance->GetClass()==apClass)
		return;

	tVarValueMap vTempValues;
	if(mpClassInstance) 
	{
		mpClassInstance->SaveValuesToMap(vTempValues);
		hplDelete(mpClassInstance);
	}

	mpClassInstance = apClass->CreateInstance(eEdVarCategory_Type);
	mpClassInstance->LoadValuesFromMap(vTempValues);

	mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldParamsChange, NULL);
}

void cHplEntity::SetMeshFromElement(cXmlElement* apMeshElement, cXmlElement* apBonesElement)
{
	tEdObjDataVec vSubMeshData, vBoneData;

	////////////////////////////////////////////////////////////
	// Get submesh data from the .ent file
	cXmlNodeListIterator itSubMeshes = apMeshElement->GetChildIterator();
	while(itSubMeshes.HasNext())
	{
		cXmlElement* pSubMesh = itSubMeshes.Next()->ToElement();

		iEdObjectData* pData = mpTypeSubMesh->CreateData();
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

			iEdObjectData* pData = mpTypeBone->CreateData();
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

iEdAction* cHplEntity::CreateSetClassAction(cEdUserClass* apClass)
{
	return hplNew(cEdActionHplEntitySetClass, (this, apClass));
}

iEdAction* cHplEntity::CreateSetVariableValueAction(const tWString& asVarName, const tWString& asValue)
{
	return hplNew(cEdActionHplEntitySetVarValue,(this, asVarName, asValue));
}

//-------------------------------------------------------------------

iEdAction* cHplEntity::CreateSetAnimationsAction(const tAnimWrapperVec& avAnimations)
{
	return hplNew(cEdActionHplEntitySetAnimations,(this, avAnimations));
}

//-------------------------------------------------------------------

bool cHplEntity::ToggleTestWindow()
{
	cEdWindowHandler* pHandler = mpEditor->GetWindowHandler();
	if(mpActiveTestWindow)
	{
		pHandler->DestroyWindow(mpActiveTestWindow);
		mpActiveTestWindow = NULL;		
	}
	else
	{
		mpActiveTestWindow = hplNew(cEdWindowPhysicsTest,(mpEditor, mpEditor->GetMainFrameBuffer()));
		pHandler->AddWindow(mpActiveTestWindow, eLayoutBlock_ViewportArea, true);
	}

	return mpActiveTestWindow!=NULL;
}

//-------------------------------------------------------------------

void cHplEntity::CreateImportMeshDialog()
{
	tWString sMeshFileName = cString::To16Char(mpTypeSubMesh->GetMeshFilename());
	mpEditor->GetWindowHandler()->CreateLoadDialog(mvDestFilenames, sMeshFileName, this, kGuiCallback(ImportMesh), _W("Meshes"), tWStringList(1, _W("*.dae")));
}

//-------------------------------------------------------------------

iEdAction* cHplEntity::CreateImportMeshAction(const tString& asX)
{
	return NULL;
}

//-------------------------------------------------------------------

cMeshEntity* cHplEntity::CreateTestEntity(cWorld* apWorld, std::vector<iPhysicsJoint*>& avJoints)
{
	const tString& sTestEntName = "TestEntity";

	if(apWorld->GetDynamicMeshEntity(sTestEntName))
		return NULL;

	cEngine* pEngine = mpEditor->GetEngine();
	cResources* pRes = pEngine->GetResources();
	iXmlDocument* pDoc = pRes->GetLowLevel()->CreateXmlDocument();
	if(SaveContentsToXmlDoc(pDoc)==false)
		return NULL;

	cMeshEntity* pTestEntity = mpLoader->LoadEntityFromElement(0, sTestEntName, pDoc, apWorld, "", _W(""),
																true, true, true, true, true);
	avJoints = mpLoader->GetJoints();
	pRes->DestroyXmlDocument(pDoc);

	return pTestEntity;
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cHplEntity::OnInit()
{
	mpLoader = hplNew(cEdEntityLoader,(mpEditor->GetEngine()->GetResources()));

	mpTypeBody = hplNew(cTypeBody,());
	mpTypeSubMesh = hplNew(cTypeSubMesh,());
	mpTypeBone = hplNew(cTypeBone,());

	AddType(mpTypeBody);
	AddType(mpTypeSubMesh);
	AddType(mpTypeBone);
	
	iEdScnWorld::OnInit();

	mpClassDef->Create();

	///////////////////////////
	// Register categories
	cEdScnObjCategory* pCat = NULL;
	pCat = CreateCategory("Entities");
	pCat->RegisterType(this, _W("Object"));

	pCat = CreateCategory("Mesh");
	pCat->RegisterType(this, _W("SubMesh"));
	pCat->SetBypassLoadObjects(true);

	cCategoryDataHandlerMesh* pMeshDataHandler = hplNew(cCategoryDataHandlerMesh,(this));
	pCat->SetDataHandler(pMeshDataHandler, true);

	pCat = CreateCategory("Bones");
	pCat->RegisterType(this, _W("Bone"));
	pCat->SetBypassLoadObjects(true);
	pMeshDataHandler->SetBoneCategory(pCat);

	pCat = CreateCategory("Shapes");
	pCat->RegisterType(this, _W("Shape"));

	pCat = CreateCategory("Bodies");
	pCat->RegisterType(this, _W("Body"));

	pCat = CreateCategory("Joints");
	pCat->RegisterType(this, _W("Joint"));

	/*
	tStringVec vCategoryNames;
	std::vector<tWStringVec> vCategoryTypes;

	vCategoryNames.push_back("Entities");
	vCategoryTypes.push_back(tWStringVec(1, _W("Object")));

	vCategoryNames.push_back("Mesh");
	vCategoryTypes.push_back(tWStringVec(1, _W("SubMesh")));

	vCategoryNames.push_back("Bones");
	vCategoryTypes.push_back(tWStringVec(1, _W("Bone")));

	vCategoryNames.push_back("Shapes");
	vCategoryTypes.push_back(tWStringVec(1, _W("Shape")));

	vCategoryNames.push_back("Bodies");
	vCategoryTypes.push_back(tWStringVec(1, _W("Body")));

	vCategoryNames.push_back("Joints");
	vCategoryTypes.push_back(tWStringVec(1, _W("Joint")));

	for(size_t i=0; i<vCategoryNames.size(); ++i)
	{
		const tString& sCategory = vCategoryNames[i];
		const tWStringVec vTypes = vCategoryTypes[i];

		cEdScnObjCategory* pCat = CreateCategory(sCategory);
		for(size_t j=0; j<vTypes.size(); ++j)
		{
			const tWString& sType = vTypes[j];
			if(pCat->RegisterType(this, sType)==false)
				FatalError("ScnWorld: Failed to register type '%ls' with save category '%s'", sType.c_str(), sCategory.c_str());
		}
	}
	*/
}

//-------------------------------------------------------------------

void cHplEntity::OnReset()
{
	iEdWorld::OnReset();

	mpTypeSubMesh->ClearMesh();
	
	mvAnimations.clear();

	SetClass(static_cast<cEdUserClass*>(mpClassDef->GetDefaultClass()));
}

//-------------------------------------------------------------------

bool cHplEntity::LoadContentsFromXmlDoc(iXmlDocument* apDoc)
{
	if(iEdScnWorld::LoadContentsFromXmlDoc(apDoc)==false)	
		return false;

	cXmlElement* pModelData = apDoc->GetFirstElement("ModelData");
	if(pModelData==NULL)
	{
		Error("ModelData element not found");
		return false;
	}

	LoadObjects(pModelData, tScnObjList());

	/////////////////////////////////////////////////////////////////////
	// Load animations
	cXmlElement* pAnimsElement = pModelData->GetFirstElement("Animations");
	if(pAnimsElement)
	{
		cXmlNodeListIterator itAnims = pAnimsElement->GetChildIterator();
		while(itAnims.HasNext())
		{
			cXmlElement* pAnimElement = itAnims.Next()->ToElement();
			cAnimationWrapper anim;
			anim.Load(pAnimElement);

			mvAnimations.push_back(anim);
		}
	}
	else
	{
		Error("No animations element found\n");
	}

	/////////////////////////////////////////////////////////////////////
	// Load user variables
	cXmlElement* pVarsElement = apDoc->GetFirstElement("UserDefinedVariables");
	if(pVarsElement)
	{
		tWString sTypeName = cString::To16Char(pVarsElement->GetAttributeString("EntityType"));

		// Backwards compatibility 'hack'
		tWString sSubTypeName = cString::To16Char(pVarsElement->GetAttributeString("EntitySubType"));
		if(sSubTypeName.empty()==false)
			sTypeName += _W(".") + sSubTypeName;

		tWString sSeparators = _W(".");
		tWStringVec sTypeNameVec;
		cString::GetStringVecW(sTypeName, sTypeNameVec, &sSeparators);

		int lVecLength = (int)sTypeNameVec.size();
		cEdUserClass* pClass = static_cast<cEdUserClass*>(mpClassDef->GetClassByName(sTypeNameVec[lVecLength-1]));
		if(pClass==NULL)
		{
			Error("Entity class not supported!!\n");
			pClass = static_cast<cEdUserClass*>(mpClassDef->GetClass(0));

			lVecLength = 0;
		}

		bool bCorrectType = true;
		cEdUserClass* pChild = pClass;
		for(int i=lVecLength-2; i>=0; --i)
		{
			cEdUserClass* pParent = static_cast<cEdUserClass*>(pChild->GetParent());
			const tWString& sParentTypeName = sTypeNameVec[i];

			if(pParent->GetName()!=sParentTypeName)
				bCorrectType = false;

			pChild = pParent;
		}

		if(bCorrectType==false)
			FatalError("AAARGH\n");

		SetClass(pClass);

		mpClassInstance->Load(pVarsElement);
	}
	else
	{
		Error("No user variables element found\n");
	}

	return true;
}

//-------------------------------------------------------------------

bool cHplEntity::SaveContentsToXmlDoc(iXmlDocument* apDoc)
{
	if(iEdScnWorld::SaveContentsToXmlDoc(apDoc)==false)
		return false;

	cXmlElement* pModelData = apDoc->CreateChildElement("ModelData");
	
	tScnObjList lstObjects;
	tEdObjectMapIt it = mmapObjects.begin();
	for(;it!=mmapObjects.end();++it)
	{
		iEdScnObject* pObj = static_cast<iEdScnObject*>(it->second);

		lstObjects.push_back(pObj);
	}

	SaveObjects(pModelData, lstObjects);

	for(size_t i=0;i<mvCategories.size(); ++i)
	{
		cEdScnObjCategory* pCat = mvCategories[i];

		pCat->PostObjectsSave();
	}

	///////////////////////////////////////////////////////////////
	// Save animations
	cXmlElement* pAnimsElement = pModelData->CreateChildElement("Animations");
	for(size_t i=0; i<mvAnimations.size(); ++i)
	{
		cAnimationWrapper& anim = mvAnimations[i];
		anim.Save(pAnimsElement);
	}

	///////////////////////////////////////////////////////////////
	// Save user variables
	cXmlElement* pVarsElement = apDoc->CreateChildElement("UserDefinedVariables");
	if(mpClassInstance)
	{
		cEdUserClass* pClass = static_cast<cEdUserClass*>(mpClassInstance->GetClass());
		const tWString& sEntityType = pClass->GetFullNameNoBaseClass();

		pVarsElement->SetAttribute("EntityType", cString::To8Char(sEntityType).c_str());
		mpClassInstance->Save(pVarsElement);
	}

	return true;
}

//-------------------------------------------------------------------

bool cHplEntity::ImportMesh(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal!=1) return true;

	tWString sFilenameFullPath = mvDestFilenames.front();
	tString sFilename = cString::To8Char(sFilenameFullPath);
	
	//if(msFilename.empty()) msFilename = cString::To16Char(cString::Sub(sFilename, 0, cString::GetLastStringPos(sFilename, "."))) + _W(".ent");
	
	cEdEditModeSelect* pMode = static_cast<cEdEditModeSelect*>(mpEditor->GetEditModeHandler()->GetMode(_W("Select")));
	cEdSelection* pSelection = pMode->GetSelection();

	cEdActionCompound* pAction = hplNew(cEdActionCompound,(_W("Add imported mesh")));
	pAction->AddAction(pSelection->CreateSelectObjectsAction(tScnObjList()));
	pAction->AddAction(hplNew(cEdActionHplEntityImportMesh, (this, sFilename)));

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cHplEntity, ImportMesh);

//-------------------------------------------------------------------

//-------------------------------------------------------------------
