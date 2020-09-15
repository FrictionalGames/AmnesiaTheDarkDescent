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

#include "../Common/EdWorld.h"

#include "../Common/Editor.h"
#include "../Common/EdAction.h"
#include "../Common/EdObjectActions.h"

#include <algorithm>



//-----------------------------------------------------------------------

static bool SortIndexedFiles(cIndexedFile* apFile1, cIndexedFile* apFile2)
{
	return apFile1->mlRefCount > apFile2->mlRefCount;
}

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// INDEXED FILE
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cFileIndex::cFileIndex(iEdWorld* apWorld, const tString& asName)
{
    mpWorld = apWorld;
	msName = asName;

	mlIDCounter = -1;
}

//-----------------------------------------------------------------------

cIndexedFile* cFileIndex::CreateIndexedFile(const tString& asFileName)
{
	//////////////////////////////////////////////////////////////////////////
	// Search for indexed file.
	tString sRelativePath = cIndexedFile::GetIndexedFilePathFromNormalPath(asFileName);
	cIndexedFile* pFile = GetIndexedFile(sRelativePath);
	if(pFile)
		return pFile;

	pFile = hplNew(cIndexedFile,(this, sRelativePath));

	pFile->mlID = ++mlIDCounter;
	mlstFiles.push_back(pFile);

	++mlIDCounter;

	return pFile;
}

cIndexedFile* cFileIndex::CreateIndexedFileFromElement(cXmlElement* apElem)
{
	if(apElem==NULL)
		return NULL;

	cIndexedFile* pFile = hplNew(cIndexedFile,(this, ""));
	pFile->Load(apElem);

	return pFile;
}

//-----------------------------------------------------------------------

cIndexedFile* cFileIndex::GetIndexedFile(const tString& asPath)
{
	if(asPath.empty())
		return NULL;

	tIndexedFileListIt it = mlstFiles.begin();
	for(;it!=mlstFiles.end();++it)
	{
		cIndexedFile* pFileInList = *it;

		if(asPath == pFileInList->msRelativePath)
			return pFileInList;
	}

	return NULL;
}

cIndexedFile* cFileIndex::GetIndexedFile(int alIdx, bool abSkipRedirect)
{
	if(abSkipRedirect==false && mmapRedirector.empty()==false)
	{
		int lID = alIdx;
		tIDRedirectMap::iterator it = mmapRedirector.find(alIdx);
		if(it!=mmapRedirector.end())
			lID = it->second;
		
		return GetIndexedFile(lID, true);
	}

	tIndexedFileListIt it = mlstFiles.begin();
	for(;it!=mlstFiles.end();++it)
	{
		cIndexedFile* pFileInList = *it;

		if(pFileInList->mlID == alIdx)
			return pFileInList;
	}

	return NULL;
}

//-----------------------------------------------------------------------

void cFileIndex::Load(cXmlElement* apParent)
{
	STLDeleteAll(mlstFiles);
	mlIDCounter = -1;

	cXmlElement* pIndexElem = apParent->GetFirstElement("FileIndex_" + msName);
	if(pIndexElem==NULL)
		return;
	
	cXmlNodeListIterator it = pIndexElem->GetChildIterator();
	while(it.HasNext())
	{
		cXmlElement* pFileElem = it.Next()->ToElement();

		cIndexedFile* pFile = CreateIndexedFileFromElement(pFileElem);
		if(pFile)
		{
			mlstFiles.push_back(pFile);

			if(mlIDCounter<pFile->mlID)
				mlIDCounter = pFile->mlID;
		}
	}
}

void cFileIndex::Save(cXmlElement* apParent)
{
	CleanUpFiles();

	tIndexedFileListIt itFiles = mlstFiles.begin();
	for(;itFiles!=mlstFiles.end();++itFiles)
	{
		cIndexedFile* pFile = *itFiles;

		if(pFile->mlRefCount==0)
			break;
	}

	tIndexedFileList lstCleanedFileList;
	lstCleanedFileList.insert(lstCleanedFileList.end(), mlstFiles.begin(), itFiles--);

	cXmlElement* pIndexElem = apParent->CreateChildElement("FileIndex_" + msName);
	pIndexElem->SetAttributeInt("NumOfFiles", (int)lstCleanedFileList.size());

	itFiles = lstCleanedFileList.begin();
	for(;itFiles!=lstCleanedFileList.end(); ++itFiles)
	{
		cIndexedFile* pFile = *itFiles;
		pFile->Save(pIndexElem);
	}
}

void cFileIndex::Import(cXmlElement* apParent)
{
	cXmlElement* pIndexElem = apParent->GetFirstElement("FileIndex_" + msName);
	if(pIndexElem==NULL)
		return;
	
	mmapRedirector.clear();

	cXmlNodeListIterator it = pIndexElem->GetChildIterator();
	while(it.HasNext())
	{
		cXmlElement* pFileElem = it.Next()->ToElement();

		cIndexedFile* pFile = CreateIndexedFileFromElement(pFileElem);
		if(pFile)
		{
			cIndexedFile* pFileInList = GetIndexedFile(pFile->msRelativePath);
			if(pFileInList==NULL)
			{
				mlstFiles.push_back(pFile);

				if(mlIDCounter<pFile->mlID)
					mlIDCounter = pFile->mlID;

				mmapRedirector.insert(tIDRedirectMap::value_type(pFile->mlID,pFile->mlID));
			}
			else
			{
				mmapRedirector.insert(tIDRedirectMap::value_type(pFile->mlID, pFileInList->mlID));

				hplDelete(pFile);
			}

		}
	}
}

void cFileIndex::PostImport()
{
	this->mmapRedirector.clear();
}

//-----------------------------------------------------------------------

void cFileIndex::CleanUpFiles()
{
	mlstFiles.sort(SortIndexedFiles);
	
	int i=0;
	tIndexedFileListIt it = mlstFiles.begin();
	for(;it!=mlstFiles.end();++it)
	{
		cIndexedFile* pFile = *it;

		pFile->mlID = i++;
	}

	if(i<mlIDCounter)
		mlIDCounter = i+1;
}

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// INDEXED FILE
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cIndexedFile::cIndexedFile(cFileIndex* apIndex, const tString& asFilename)
{
	mpIndex = apIndex;
	mlID = -1;
	msRelativePath = asFilename;
	mlRefCount = 0;
}

//-----------------------------------------------------------------------

void cIndexedFile::Load(cXmlElement* apElem)
{
	mlID = apElem->GetAttributeInt("Id", -1);
	msRelativePath = cIndexedFile::GetIndexedFilePathFromNormalPath(apElem->GetAttributeString("Path", ""));
}

void cIndexedFile::Save(cXmlElement* apElem)
{
	if(mlRefCount==0) return;

	cXmlElement* pXmlFile = apElem->CreateChildElement("File");

	pXmlFile->SetAttributeInt("Id", mlID);
	pXmlFile->SetAttributeString("Path", msRelativePath);
}

//-----------------------------------------------------------------------

tString cIndexedFile::GetIndexedFilePathFromNormalPath(const tString& asX)
{
	tString sRelativePath;

	if(asX.empty()==false)
	{
		iEditor* pEditor = iEditor::GetInstance();
		tWString sFullPath = pEditor->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(asX);

		sRelativePath = cString::To8Char(pEditor->GetPathRelToWD(sFullPath));
	}

	return sRelativePath;
}

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROPERTY WRAPPERS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iPropVal::iPropVal(iProp* apProp)
{
	mpProp = apProp;
}

void iPropVal::Save(cXmlElement* apElement)
{
	if(mpProp->IsSaved()) 
		SaveSpecific(apElement);
}

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROPERTY INT
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iPropVal* cPropInt::CreateValue()
{
	return hplNew(cPropValInt,(this));
}

cPropValInt::cPropValInt(iProp* apProp) : iPropVal(apProp)
{
	cPropInt* pProp = (cPropInt*)apProp;
	Set(pProp->GetDefault());
}

void cPropValInt::GetFromObject(iEdObject* apObj)
{
	apObj->GetProperty(mpProp->GetID(), mlVal);
}

void cPropValInt::SetToObject(iEdObject* apObj)
{
	apObj->SetProperty(mpProp->GetID(), mlVal);
}

void cPropValInt::Load(cXmlElement* apElement)
{
	mlVal = apElement->GetAttributeInt(mpProp->GetName(), mlVal);
}

void cPropValInt::SaveSpecific(cXmlElement* apElement)
{
	apElement->SetAttributeInt(mpProp->GetName(), mlVal);
}

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROPERTY FLOAT
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iPropVal* cPropFloat::CreateValue()
{
	return hplNew(cPropValFloat,(this));
}

cPropValFloat::cPropValFloat(iProp* apProp) : iPropVal(apProp)
{
	cPropFloat* pProp = (cPropFloat*)apProp;
	Set(pProp->GetDefault());
}

void cPropValFloat::GetFromObject(iEdObject* apObj)
{
	apObj->GetProperty(mpProp->GetID(), mfVal);
}

void cPropValFloat::SetToObject(iEdObject* apObj)
{
	apObj->SetProperty(mpProp->GetID(), mfVal);
}

void cPropValFloat::Load(cXmlElement* apElement)
{
	mfVal = apElement->GetAttributeFloat(mpProp->GetName(), mfVal);
}

void cPropValFloat::SaveSpecific(cXmlElement* apElement)
{
	apElement->SetAttributeFloat(mpProp->GetName(), mfVal);
}

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROPERTY BOOL
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iPropVal* cPropBool::CreateValue()
{
	return hplNew(cPropValBool,(this));
}

cPropValBool::cPropValBool(iProp* apProp) : iPropVal(apProp)
{
	cPropBool* pProp = (cPropBool*)apProp;
	Set(pProp->GetDefault());
}

void cPropValBool::GetFromObject(iEdObject* apObj)
{
	apObj->GetProperty(mpProp->GetID(), mbVal);
}

void cPropValBool::SetToObject(iEdObject* apObj)
{
	apObj->SetProperty(mpProp->GetID(), mbVal);
}

void cPropValBool::Load(cXmlElement* apElement)
{
	mbVal = apElement->GetAttributeBool(mpProp->GetName(), mbVal);
}

void cPropValBool::SaveSpecific(cXmlElement* apElement)
{
	apElement->SetAttributeBool(mpProp->GetName(), mbVal);
}

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROPERTY STRING
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iPropVal* cPropStr::CreateValue()
{
	return hplNew(cPropValStr,(this));
}

cPropValStr::cPropValStr(iProp* apProp) : iPropVal(apProp)
{
	cPropStr* pProp = (cPropStr*)apProp;
	Set(pProp->GetDefault());
}

void cPropValStr::GetFromObject(iEdObject* apObj)
{
	apObj->GetProperty(mpProp->GetID(), msVal);
}

void cPropValStr::SetToObject(iEdObject* apObj)
{
	apObj->SetProperty(mpProp->GetID(), msVal);
}

void cPropValStr::Load(cXmlElement* apElement)
{
	msVal = apElement->GetAttributeString(mpProp->GetName(), msVal);
}

void cPropValStr::SaveSpecific(cXmlElement* apElement)
{
	apElement->SetAttributeString(mpProp->GetName(), msVal);
}

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROPERTY VEC2F
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iPropVal* cPropVec2f::CreateValue()
{
	return hplNew(cPropValVec2f,(this));
}

cPropValVec2f::cPropValVec2f(iProp* apProp) : iPropVal(apProp)
{
	cPropVec2f* pProp = (cPropVec2f*)apProp;
	Set(pProp->GetDefault());
}

void cPropValVec2f::GetFromObject(iEdObject* apObj)
{
	apObj->GetProperty(mpProp->GetID(), mvVal);
}

void cPropValVec2f::SetToObject(iEdObject* apObj)
{
	apObj->SetProperty(mpProp->GetID(), mvVal);
}

void cPropValVec2f::Load(cXmlElement* apElement)
{
	mvVal = apElement->GetAttributeVector2f(mpProp->GetName(), mvVal);
}

void cPropValVec2f::SaveSpecific(cXmlElement* apElement)
{
	apElement->SetAttributeVector2f(mpProp->GetName(), mvVal);
}

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROPERTY VEC3F
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iPropVal* cPropVec3f::CreateValue()
{
	return hplNew(cPropValVec3f,(this));
}

cPropValVec3f::cPropValVec3f(iProp* apProp) : iPropVal(apProp)
{
	cPropVec3f* pProp = (cPropVec3f*)apProp;
	Set(pProp->GetDefault());
}

void cPropValVec3f::GetFromObject(iEdObject* apObj)
{
	apObj->GetProperty(mpProp->GetID(), mvVal);
}

void cPropValVec3f::SetToObject(iEdObject* apObj)
{
	apObj->SetProperty(mpProp->GetID(), mvVal);
}

void cPropValVec3f::Load(cXmlElement* apElement)
{
	mvVal = apElement->GetAttributeVector3f(mpProp->GetName(), mvVal);
}

void cPropValVec3f::SaveSpecific(cXmlElement* apElement)
{
	apElement->SetAttributeVector3f(mpProp->GetName(), mvVal);
}

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROPERTY COLOR
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iPropVal* cPropCol::CreateValue()
{
	return hplNew(cPropValCol,(this));
}

cPropValCol::cPropValCol(iProp* apProp) : iPropVal(apProp)
{
	cPropCol* pProp = (cPropCol*)apProp;
	Set(pProp->GetDefault());
}

void cPropValCol::GetFromObject(iEdObject* apObj)
{
	apObj->GetProperty(mpProp->GetID(), mVal);
}

void cPropValCol::SetToObject(iEdObject* apObj)
{
	apObj->SetProperty(mpProp->GetID(), mVal);
}

void cPropValCol::Load(cXmlElement* apElement)
{
	mVal = apElement->GetAttributeColor(mpProp->GetName(), mVal);
}

void cPropValCol::SaveSpecific(cXmlElement* apElement)
{
	apElement->SetAttributeColor(mpProp->GetName(), mVal);
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// TYPENODE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iEdObjectType::iEdObjectType(const tWString& asName, const tString& asXmlElementName)
{
	msName = asName;
	msXmlElementName = asXmlElementName;
	
	mpParentType = NULL;
	mpWorld = NULL;

	mvProperties.resize(ePropStep_LastEnum);

	mbCloneable = true;
	mbDeletable = true;
	mbRenameable = true;

	mbClearingObjects = false;
}

iEdObjectType::~iEdObjectType()
{
	for(size_t i=0; i<mvProperties.size(); ++i)
	{
		tPropVec& vProps = mvProperties[i];
		STLDeleteAll(vProps);
		vProps.clear();
	}
	mvProperties.clear();

	STLDeleteAll(mvChildTypes);
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// TYPENODE - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool iEdObjectType::PostCreateSetUp()
{
	msFullName.clear();

	bool bRet = true;

	for(int i=0; i<GetNumChildren(); ++i)
	{
		iEdObjectType* pSubType = GetChild(i);
		bRet = bRet && pSubType->PostCreateSetUp();
	}

	return bRet;
}

//-----------------------------------------------------------------------

tWString iEdObjectType::GetFullName()
{
	if(msFullName.empty())
	{
		if(mpParentType)
			msFullName = mpParentType->GetFullName() + _W(".");

		msFullName += GetName();
	}

	return msFullName;
}

//-----------------------------------------------------------------------

bool iEdObjectType::IsA(iEdObjectType* apType)
{
	if(this==apType) return true;
	if(mpParentType) return mpParentType->IsA(apType);

	return false;
}

bool iEdObjectType::IsA(int alID)
{
	if(mlID==alID)
		return true;
	else if(mpParentType)
		return mpParentType->IsA(alID);
	else
		return false;
}

bool iEdObjectType::IsA(const tWString& asTypeName)
{
	if(msName==asTypeName)
		return true;
	else if(mpParentType)
		return mpParentType->IsA(asTypeName);
	else
		return false;
}

//-----------------------------------------------------------------------

iEdObjectType* iEdObjectType::GetMostRestrictiveType(iEdObjectType* apType)
{
	if(apType==NULL || apType->IsA(this))
		return this;

	const tIntVec& vThisID = GetID();
	const tIntVec& vOtherTypeID = apType->GetID();
	tIntVec vCommonID;

	int lMinSize = cMath::Min((int)vThisID.size(), (int)vOtherTypeID.size());

	for(int i=0; i<lMinSize; ++i)
	{
		if(vThisID[i]==vOtherTypeID[i])
			vCommonID.push_back(vThisID[i]);
		else
			break;
	}

	if(vCommonID.empty())
		return mpWorld->GetRootType();

	return mpWorld->GetTypeByID(vCommonID);
}

//-----------------------------------------------------------------------

iEdObjectType* iEdObjectType::GetTypeByName(const tWString& asName)
{
	int lFirstDotPos = cString::GetFirstStringPosW(asName, _W("."));
	tWString sNamePiece = cString::SubW(asName, 0, lFirstDotPos);
	tWString sRestingName = cString::SubW(asName, lFirstDotPos+1);

	if(lFirstDotPos==-1 && msName==asName)
		return this;

	tEdObjTypeMapIt it = mmapChildTypes.find(sNamePiece);
	if(it!=mmapChildTypes.end())
	{
		iEdObjectType* pSubType = it->second;
		return pSubType->GetTypeByName(sRestingName);
	}

	return NULL;
}

iEdObjectType* iEdObjectType::GetTypeByID(const tIntVec& avID)
{
	if(avID.empty())
		return this;

	int lChildID = avID.front();
	tIntVec vID = tIntVec(++(avID.begin()), avID.end());

	iEdObjectType* pType = GetChild(lChildID);
	if(pType) return pType->GetTypeByID(vID);

	return NULL;
}

iEdObjectType* iEdObjectType::GetTypeByXmlElement(cXmlElement* apElement)
{
	if(apElement->GetValue()==msXmlElementName)
		return this;

	for(int i=0; i<GetNumChildren(); ++i)
	{
		iEdObjectType* pSubType = GetChild(i);
		iEdObjectType* pType = pSubType->GetTypeByXmlElement(apElement);

		if(pType) return pType;
	}

	return NULL;
}

iEdObjectType* iEdObjectType::GetMainType()
{
	if(mpParentType==NULL || mpParentType==mpWorld->GetRootType())
		return this;
	else
	{
		if(mpParentType)
			return mpParentType->GetMainType();
	}

	return NULL;
}

//-----------------------------------------------------------------------

void iEdObjectType::LogTypeHierarchyHelper(tWString& asX, int alLevel)
{
	if(alLevel==0)
		asX += _W('*');

	for(int i=0;i<alLevel;++i)
		asX += _W("\t|");

	asX += _W("- ") + GetName() + _W("(");

	for(size_t i = 0; i<mvID.size(); ++i)
	{
		asX += _W(" ") + cString::ToStringW(mvID[i]);
	}
	asX += _W(")\n");

	for(int i=0;i<GetNumChildren();++i)
	{
		iEdObjectType* pType = GetChild(i);
		pType->LogTypeHierarchyHelper(asX, alLevel+1);
	}
}

//-----------------------------------------------------------------------

const tEdObjTypeVec& iEdObjectType::GetChildren()
{
	return mvChildTypes;
}

//-----------------------------------------------------------------------

iEdObjectData* iEdObjectType::CreateData()
{
	iEdObjectData* pData = CreateTypeSpecificData();
	if(pData) CreateValuesInData(pData);

	return pData;
}

//-----------------------------------------------------------------------

void iEdObjectType::AddObject(iEdObject* apObj)
{
	mlstObjects.push_back(apObj);
}

void iEdObjectType::RemoveObject(iEdObject* apObj)
{
	if(mbClearingObjects==false)
		mlstObjects.remove(apObj);
}

int iEdObjectType::GetObjectNum()
{
	int lNum = (int)mlstObjects.size();

	for(int i=0;i<GetNumChildren(); ++i)
	{
		iEdObjectType* pSubType = GetChild(i);

		lNum += pSubType->GetObjectNum();
	}

	return lNum;
}

tEdObjectList iEdObjectType::GetObjects()
{
	tEdObjectList lstObjects;
	GetObjectsHelper(lstObjects);

	return lstObjects;
}

void iEdObjectType::GetObjectsHelper(tEdObjectList& alstObjects)
{
	alstObjects.insert(alstObjects.end(), mlstObjects.begin(), mlstObjects.end());
	for(int i=0;i<GetNumChildren(); ++i)
	{
		iEdObjectType* pSubType = GetChild(i);

		pSubType->GetObjectsHelper(alstObjects);
	}
}

//-----------------------------------------------------------------------

void iEdObjectType::SetChildObjectsUpdated()
{
	tEdObjectListIt it = mlstObjects.begin();
	for(;it!=mlstObjects.end(); ++it)
	{
		iEdObject* pObj = *it;

		pObj->SetUpdated();
	}

	for(int i=0;i<GetNumChildren();++i)
	{
		iEdObjectType* pSubType = GetChild(i);

		pSubType->SetChildObjectsUpdated();
	}
}

//-----------------------------------------------------------------------

tIntList iEdObjectType::GetObjectIDs()
{
	tEdObjectList lstObjects = GetObjects();
	tIntList lstIDs;
	tEdObjectListIt it = lstObjects.begin();
	for(;it!=lstObjects.end(); ++it)
	{
		iEdObject* pObj = *it;

		lstIDs.push_back(pObj->GetID());
	}

	return lstIDs;
}

void iEdObjectType::ClearObjects()
{
	mbClearingObjects = true;

	tEdObjectListIt it = mlstObjects.begin();
	for(;it!=mlstObjects.end();++it)
	{
		iEdObject* pObj = *it;

		mpWorld->DestroyObj(pObj);
	}

	mlstObjects.clear();

	mbClearingObjects = false;

	for(int i=0;i<GetNumChildren();++i)
	{
		iEdObjectType* pSubType = GetChild(i);

		pSubType->ClearObjects();
	}
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// TYPENODE - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool iEdObjectType::AddChildType(iEdObjectType* apType)
{
	if(apType)
	{
		apType->SetWorld(mpWorld);

		if(apType->Init())
		{
			apType->SetParent(this, (int)mvChildTypes.size());

			mvChildTypes.push_back(apType);
			mmapChildTypes.insert(tEdObjTypeMap::value_type(apType->GetName(), apType));

			return true;
		}
	}

	if(apType)
	{
		Error("Could not add Type %ls\n", apType->GetName().c_str());
		hplDelete(apType);
	}
	else
		Log("Warning: tried to add NULL type!\n");

	return false;
}

//-----------------------------------------------------------------------

int iEdObjectType::GetNumChildren()
{
	return (int)mvChildTypes.size();
}

iEdObjectType* iEdObjectType::GetChild(int alNum)
{
	if(alNum<0 || alNum >=GetNumChildren()) return NULL;

	return mvChildTypes[alNum];
}

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

void iEdObjectType::AddInt(int alPropID, const tString& asName, int alDefault, ePropStep aOrder, bool abSave)
{
	AddProperty(hplNew(cPropInt,(this, alPropID, asName, alDefault, abSave)), aOrder);
}

//-----------------------------------------------------------------------

void iEdObjectType::AddFloat(int alPropID, const tString& asName, float afDefault, ePropStep aOrder, bool abSave)
{
	AddProperty(hplNew(cPropFloat,(this, alPropID, asName, afDefault, abSave)), aOrder);
}

//-----------------------------------------------------------------------

void iEdObjectType::AddBool(int alPropID, const tString& asName, bool abDefault, ePropStep aOrder, bool abSave)
{
	AddProperty(hplNew(cPropBool,(this, alPropID, asName, abDefault, abSave)), aOrder);
}

//-----------------------------------------------------------------------

void iEdObjectType::AddString(int alPropID, const tString& asName, const tString& asDefault, ePropStep aOrder, bool abSave)
{
	AddProperty(hplNew(cPropStr,(this, alPropID, asName, asDefault, abSave)), aOrder);
}

//-----------------------------------------------------------------------

void iEdObjectType::AddVec2f(int alPropID, const tString& asName, const cVector2f& avDefault, ePropStep aOrder, bool abSave)
{
	AddProperty(hplNew(cPropVec2f,(this, alPropID, asName, avDefault, abSave)), aOrder);
}

//-----------------------------------------------------------------------

void iEdObjectType::AddVec3f(int alPropID, const tString& asName, const cVector3f& avDefault, ePropStep aOrder, bool abSave)
{
	AddProperty(hplNew(cPropVec3f,(this, alPropID, asName, avDefault, abSave)), aOrder);
}

//-----------------------------------------------------------------------

void iEdObjectType::AddColor(int alPropID, const tString& asName, const cColor& aDefault, ePropStep aOrder, bool abSave)
{
	AddProperty(hplNew(cPropCol,(this, alPropID, asName, aDefault, abSave)), aOrder);
}

//-----------------------------------------------------------------------

void iEdObjectType::AddProperty(iProp* apProp, ePropStep aOrder)
{
	mvProperties[aOrder].push_back(apProp);
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

iProp* iEdObjectType::GetPropByTypeAndID(int alVarType, int alPropID)
{
	for(size_t i=0; i<mvProperties.size(); ++i)
	{
		const tPropVec& vProps = mvProperties[i];
		for(size_t j=0; j<vProps.size(); ++j)
		{
			iProp* pProp = vProps[j];
			if(pProp->GetVarType()==alVarType &&
				pProp->GetID()==alPropID)
				return pProp;
		}
	}
	if(mpParentType)
		return mpParentType->GetPropByTypeAndID(alVarType, alPropID);

	return NULL;
}

//-----------------------------------------------------------------------

iProp* iEdObjectType::GetPropByName(const tString& asName)
{
	for(size_t i=0; i<mvProperties.size(); ++i)
	{
		const tPropVec& vProps = mvProperties[i];
		for(size_t j=0; j<vProps.size(); ++j)
		{
			iProp* pProp = vProps[j];
			if(pProp->GetName()==asName)
				return pProp;
		}
	}

	if(mpParentType)
		return mpParentType->GetPropByName(asName);

	return NULL;
}

//-----------------------------------------------------------------------

bool iEdObjectType::HasProp(const tString& asName)
{
	return GetPropByName(asName)!=NULL;
}

//-----------------------------------------------------------------------

void iEdObjectType::CreateValuesInData(iEdObjectData* apData)
{
	////////////////////////////////////////////////////////////////////
	// Let the properties cascade from the top of the hierarchy down
	if(mpParentType)
		mpParentType->CreateValuesInData(apData);

	////////////////////////////////////////////////
	// Dump each typenode's properties
	for(size_t i=0; i<mvProperties.size(); ++i)
	{
		tPropVec& vProps = mvProperties[i];
		ePropStep order = static_cast<ePropStep>(i);

		for(size_t j=0; j<vProps.size(); ++j)
		{
			iProp* pProp = vProps[j];
			apData->AddValue(order, pProp->CreateValue());
		}
	}
}

//-----------------------------------------------------------------------

void iEdObjectType::SetParent(iEdObjectType* apParent, int alIdx)
{
	mvID = apParent->GetID();
	mvID.push_back(alIdx);

	mpParentType = apParent;
	mpWorld = apParent->GetWorld();

	// Update children IDs
	for(int i=0; i<GetNumChildren(); ++i)
	{
		iEdObjectType* pSubType = GetChild(i);
		pSubType->SetParent(this, i);
	}
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// OBJECT DATA - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iEdObjectData::iEdObjectData(iEdObjectType* apType, bool abSaveID, bool abSaveName) : mpType(apType), mbSaveID(abSaveID), mbSaveName(abSaveName)
{
	mlID = -1;
	msName = cString::To16Char(apType->GetXmlElementName());

	mvValues.resize(ePropStep_LastEnum);

	mpExtension = apType->GetWorld()->CreateObjExtension(NULL);
}

iEdObjectData::~iEdObjectData()
{
	for(size_t i=0; i<mvValues.size(); ++i)
	{
		tPropValVec& vValues = mvValues[i];
		STLDeleteAll(vValues);
		vValues.clear();
	}
	mvValues.clear();

	if(mpExtension) hplDelete(mpExtension);
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// OBJECT DATA - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iEdObjectData::CopyFromObject(iEdObject* apObj)
{
	////////////////////////////////////////////
	// Get ID and name
	mlID = apObj->GetID();
	msName = apObj->GetName();
	////////////////////////////////////////////
	// Get property values from Object
	for(size_t i=0;i<mvValues.size(); ++i)
	{
		tPropValVec& vValues = mvValues[i];
		for(size_t j=0; j<vValues.size(); ++j)
		{
			iPropVal* pVal = vValues[j];
			pVal->GetFromObject(apObj);
		}
	}
}

//-----------------------------------------------------------------------

void iEdObjectData::CopyToObject(iEdObject* apObj, ePropStep aOrder)
{
	////////////////////////////////////////////
	// Set ID and name
	apObj->SetID(mlID);
	apObj->SetName(msName);
	////////////////////////////////////////////
	// Set property values to Object
	tPropValVec& vValues = mvValues[aOrder];
	for(size_t i=0; i<vValues.size(); ++i)
	{
		iPropVal* pVal = vValues[i];
		pVal->SetToObject(apObj);
	}

	////////////////////////////////////////////
	// If all stuff has been deployed to the map already
	if(aOrder==ePropStep_PostDeployAll)
	{
	}
}

//-----------------------------------------------------------------------

bool iEdObjectData::Load(cXmlElement* apElem)
{
	///////////////////////////////////////////
	// Load ID and name
	mlID = apElem->GetAttributeInt("ID", -1);
	msName = cString::To16Char(apElem->GetAttributeString("Name"));

	///////////////////////////////////////////
	// Load values from XML element
	for(size_t i=0; i<mvValues.size(); ++i)
	{
		tPropValVec& vValues = mvValues[i];
		for(size_t j=0; j<vValues.size(); ++j)
		{
			iPropVal* pVal = vValues[j];
			pVal->Load(apElem);
		}
	}

	return LoadSpecificData(apElem);
}

//-----------------------------------------------------------------------

bool iEdObjectData::Save(cXmlElement* apParentElem)
{
	cXmlElement* pObjElem = apParentElem->CreateChildElement(mpType->GetXmlElementName());
	//mpType->GetWorld()->SaveDataCallback(this, pElement);

	///////////////////////////////////////////
	// Save ID and name
	if(mbSaveID)	pObjElem->SetAttributeInt("ID", mlID);
	if(mbSaveName)	pObjElem->SetAttributeString("Name", cString::To8Char(msName));

	///////////////////////////////////////////
	// Save values to XML element
	for(size_t i=0; i<mvValues.size(); ++i)
	{
		tPropValVec& vValues = mvValues[i];
		for(size_t j=0; j<vValues.size(); ++j)
		{
			iPropVal* pVal = vValues[j];

			pVal->Save(pObjElem);
		}
	}

	if(SaveSpecificData(pObjElem)==false)
	{
		apParentElem->DestroyChild(pObjElem);
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------

iEdObject* iEdObjectData::CreateObject()
{
	iEdObject* pObject = CreateTypeSpecificObject();
	
	if(pObject==NULL || pObject->Create()==false)
	{
		Error("Could not create object %ls of type %ls, with ID: %d\n", 
				msName.c_str(), GetType()->GetFullName().c_str(), mlID);

		if(pObject)
		{
			hplDelete(pObject);
			pObject = NULL;
		}
	}

	mpType->AddObject(pObject);

	return pObject;
}


void iEdObjectData::SetInt(int alID, int alX)
{
	cPropValInt* pVal = (cPropValInt*)GetValue(alID, eVariableType_Int);
	if(pVal)
		pVal->Set(alX);
}

void iEdObjectData::SetFloat(int alID, float afX)
{
	cPropValFloat* pVal = (cPropValFloat*)GetValue(alID, eVariableType_Float);
	if(pVal)
		pVal->Set(afX);
}

void iEdObjectData::SetBool(int alID, bool abX)
{
	cPropValBool* pProp = (cPropValBool*)GetValue(alID, eVariableType_Bool);
	if(pProp)
		pProp->Set(abX);
}

void iEdObjectData::SetString(int alID, const tString& asX)
{
	cPropValStr* pProp = (cPropValStr*)GetValue(alID, eVariableType_String);
	if(pProp)
		pProp->Set(asX);
}

void iEdObjectData::SetVec2f(int alID, const cVector2f& avX)
{
	cPropValVec2f* pProp = (cPropValVec2f*)GetValue(alID, eVariableType_Vec2);
	if(pProp)
		pProp->Set(avX);
}

void iEdObjectData::SetVec3f(int alID, const cVector3f& avX)
{
	cPropValVec3f* pProp = (cPropValVec3f*)GetValue(alID, eVariableType_Vec3);
	if(pProp)
		pProp->Set(avX);
}

void iEdObjectData::SetColor(int alID, const cColor& aX)
{
	cPropValCol* pProp = (cPropValCol*)GetValue(alID, eVariableType_Color);
	if(pProp)
		pProp->Set(aX);
}

int iEdObjectData::GetInt(int alID)
{
	cPropValInt* pProp = (cPropValInt*)GetValue(alID, eVariableType_Int);
	if(pProp)return pProp->Get();
	return 0;
}

float iEdObjectData::GetFloat(int alID)
{
	cPropValFloat* pProp = (cPropValFloat*)GetValue(alID, eVariableType_Float);
	if(pProp) return pProp->Get();
	return 0;
}

bool iEdObjectData::GetBool(int alID)
{
	cPropValBool* pProp = (cPropValBool*)GetValue(alID, eVariableType_Bool);
	if(pProp)	return pProp->Get();
	else		return false;
}

//----------------------------------

//Variables that are to be returned, incase Prop Data is not found.
static const tString gsNullString ="";
static const cVector2f gvNullVector2(0);
static const cVector3f gvNullVector3(0);
static const cColor gNullColor(0,0);

//----------------------------------

const tString& iEdObjectData::GetString(int alID)
{
	cPropValStr* pProp = (cPropValStr*)GetValue(alID, eVariableType_String);
	if(pProp)	return pProp->Get();
	else		return gsNullString;
}

const cVector2f& iEdObjectData::GetVec2f(int alID)
{
	cPropValVec2f* pProp = (cPropValVec2f*)GetValue(alID, eVariableType_Vec2);
	if(pProp)	return pProp->Get();
	else		return gvNullVector2;
}

const cVector3f& iEdObjectData::GetVec3f(int alID)
{
	cPropValVec3f* pProp = (cPropValVec3f*)GetValue(alID, eVariableType_Vec3);
	if(pProp)	return pProp->Get();
	else		return gvNullVector3;
}

const cColor& iEdObjectData::GetColor(int alID)
{
	cPropValCol* pProp = (cPropValCol*)GetValue(alID, eVariableType_Color);
	if(pProp)	return pProp->Get();
	else		return gNullColor;
}

iProp* iEdObjectData::GetProperty(int alID, int aType)
{
	iPropVal* pVal = GetValue(alID, aType);

	if(pVal)
		return pVal->GetProperty();

	return NULL;
}

iPropVal* iEdObjectData::GetValue(int alID, int aType)
{
	///////////////////////////////////////////
	// Search for value and return it
	for(int i=0;i<::ePropStep_LastEnum; ++i)
	{
		tPropValVec& vValues = mvValues[i];
		//tPropValVecIt it = lstValues.begin();
		for(size_t i=0;i<vValues.size();++i)
		{
			iPropVal* pVal = vValues[i];
			iProp* pProp = pVal->GetProperty();
			if(pProp->GetID()==alID && pProp->GetVarType()==aType)
				return pVal;
		}
	}

	return NULL;
}


//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// OBJECT DATA - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iEdObjectData::AddValue(ePropStep aOrder, iPropVal* apVal)
{
	if(apVal==NULL) return;

	mvValues[aOrder].push_back(apVal);
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// OBJECT - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iEdObject::iEdObject(iEdObjectData* apData)
{
	mpType = NULL;

	mpData = apData;
	if(mpData)
		mpType = mpData->GetType();

	mbUpdated = false;

	mpExtension = NULL;
	if(apData && apData->GetExtension()) mpExtension = apData->GetExtension()->CreateCopy(this);

	//Log("EdObject created - %p\n", this);
}

iEdObject::~iEdObject()
{
	if(mpType) mpType->RemoveObject(this);

	iEdWorld* pWorld = mpType?GetWorld():NULL;
	if(pWorld && pWorld->IsClearing()==false)
		pWorld->RemUpdatedObject(this);
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// OBJECT - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool iEdObject::Create()
{
	if(mpData==NULL)
		return false;

	mpData->CopyToObject(this, ePropStep_PreCreate);

	if(OnCreate()==false)
		return false;

	mpData->CopyToObject(this, ePropStep_PostCreate);

	return true;
}

//-----------------------------------------------------------------------

void iEdObject::PostDeployAll(bool abDeleteData)
{
	if(mpData)
		mpData->CopyToObject(this, ePropStep_PostDeployAll);

	OnPostDeployAll(abDeleteData);
	if(mpExtension) mpExtension->OnPostDeployAll();

	if(abDeleteData)
		hplDelete(mpData);

	mpData = NULL;
}

//-----------------------------------------------------------------------

bool iEdObject::Update(bool abForceUpdate)
{
	//Log("Update - Object %ls - mbUpdated %d\n", GetName().c_str(), mbUpdated);
	if(mbUpdated || abForceUpdate)
	{
		if(OnUpdate(abForceUpdate))
		{
			GetWorld()->GetEditor()->BroadcastModuleMsg(eEdModuleMsg_WorldObjModify, NULL);
			SetUpdated(false);

			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------

void iEdObject::SetExtension(iEdObjExtension* apObj)
{
	if(mpExtension) hplDelete(mpExtension);
	mpExtension = apObj;
}

//-----------------------------------------------------------------------

iEdObjectData* iEdObject::CreateCopyData()
{
	iEdObjectData* pData = mpType->CreateData();
	pData->CopyFromObject(this);

	return pData;
}

tIntVec iEdObject::GetCloneIDs()
{
	return tIntVec(1, GetWorld()->GetFreeID());
}

iEdObject* iEdObject::Clone(const tIntVec& avIDs, bool abCallPostDestroy, bool abDestroyData, const tWString& asNamePrefix, const tWString& asNameSuffix)
{
	iEdObjectData* pCloneData = CreateCopyData();

	pCloneData->SetID(avIDs.back());
	pCloneData->SetName(GetWorld()->GenerateValidName(asNamePrefix + msName + asNameSuffix, false));

	SetUpCloneData(pCloneData, avIDs);
	
	iEdObject* pClone = GetWorld()->CreateObjFromData(pCloneData);
	if(abCallPostDestroy)
		pClone->PostDeployAll(abDestroyData);

	return pClone;
}

//-----------------------------------------------------------------------

bool iEdObject::Save(cXmlElement* apParentElement)
{
	if(apParentElement==NULL)
	{
		Error("Failed saving of entity named %s of type %s - no parent element found\n", 
				GetName().c_str(), mpType->GetXmlElementName().c_str());
		return false;
	}

	iEdObjectData* pData = CreateCopyData();
	if(pData->Save(apParentElement)==false)
	{
		Error("Failed saving of entity named %s of type %s\n", 
				pData->GetName().c_str(), mpType->GetXmlElementName().c_str());
		return false;
	}

	hplDelete(pData);

	return true;
}

//-----------------------------------------------------------------------

bool iEdObject::HasProperty(const tString& asName)
{
	return mpType->HasProp(asName);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetIntAction(int alPropID, int alX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Int, alPropID);
	cEdActionSetInt* pAction = hplNew(cEdActionSetInt,(GetWorld(), GetID(), pProp, alX));
	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetFloatAction(int alPropID, float afX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Float, alPropID);
	cEdActionSetFloat* pAction = hplNew(cEdActionSetFloat,(GetWorld(), GetID(), pProp, afX));

	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetBoolAction(int alPropID, bool abX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Bool, alPropID);
	cEdActionSetBool* pAction = hplNew(cEdActionSetBool,(GetWorld(), GetID(), pProp, abX));
	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetStringAction(int alPropID, const tString& asX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_String, alPropID);
	cEdActionSetString* pAction = hplNew(cEdActionSetString,(GetWorld(), GetID(), pProp, asX));
	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetVector2fAction(int alPropID, const cVector2f& avX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Vec2, alPropID);
	cEdActionSetVec2f* pAction = hplNew(cEdActionSetVec2f,(GetWorld(), GetID(), pProp, avX));
	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetVector3fAction(int alPropID, const cVector3f& avX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Vec3, alPropID);
	cEdActionSetVec3f* pAction = hplNew(cEdActionSetVec3f,(GetWorld(), GetID(), pProp, avX));
	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetColorAction(int alPropID, const cColor& aX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Color, alPropID);
	cEdActionSetColor* pAction = hplNew(cEdActionSetColor,(GetWorld(), GetID(), pProp, aX));
	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// OBJECT - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iEdAction* iEdObject::SetUpAction(iEdAction* apAction)
{
	/*if(apAction && apAction->Init()==false)
	{
		Error("Entity %s with type %ls created invalid action \"%ls\"\n", 
			GetName().c_str(), GetType()->GetFullName().c_str(), apAction->GetName().c_str());
		hplDelete(apAction);

		return NULL;
	}
	*/

	return apAction;
}

//-----------------------------------------------------------------------

void iEdObject::SetUpdated(bool abX)
{
	if(mbUpdated==abX)
		return;

	mbUpdated = abX;
	if(abX)
		GetWorld()->AddUpdatedObject(this);
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// OBJECT EXTENSION - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iEdWorld* iEdObjExtension::mpWorld = NULL;

//-----------------------------------------------------------------------

iEdObjExtension::iEdObjExtension(iEdWorld* apWorld, iEdObject* apObj)
{
	if(mpWorld==NULL) mpWorld = apWorld;
	mpObject = apObj;

	if(mpObject) mpObject->SetExtension(this);
}

iEdObjExtension::~iEdObjExtension()
{
}

//-----------------------------------------------------------------------

iEdObjExtension* iEdObjExtension::CreateCopy(iEdObject* apObj)
{
	iEdObjExtension* pExt = mpWorld->CreateObjExtension(apObj);

	pExt->Copy(this);

	return pExt;
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// WORLD - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

iEdWorld::iEdWorld(iEditor* apEditor, const tWString& asName, const tWString& asFileExt, const tString& asXmlValue, iEdObjectType* apRootType) : iEdModule(apEditor, asName)
{
	msFileExt = asFileExt;
	msXmlValue = asXmlValue;

	SetRootType(apRootType);

	mlModificationStamp = 0;
	mlObjIDCounter = -1;
	mBGColor = cColor(0.3f, 1);

	mpLastAddedObject = NULL;

	mbClearing = false;

	mpSessionData = NULL;

	mpWorld = NULL;
	mpTempWorld = NULL;
}

iEdWorld::~iEdWorld()
{
	OnReset();

	if(mpSessionData) hplDelete(mpSessionData);
	if(mpRootType) hplDelete(mpRootType);
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// WORLD - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iEdWorld::Compile()
{
	
}

void iEdWorld::SetBGColor(const cColor& aCol)
{
	mBGColor = aCol;
	mpWorld->SetSkyBoxColor(mBGColor);

	mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldParamsChange, NULL);
}

void iEdWorld::SetAmbientLightEnabled(bool abX)
{
	mpAmbientLight->SetVisible(abX);
}

void iEdWorld::SetPointLightEnabled(bool abX)
{
	mpPointLight->SetVisible(abX);
}

bool iEdWorld::GetAmbientLightEnabled()
{
	return mpAmbientLight?mpAmbientLight->IsVisible() : false;
}

bool iEdWorld::GetPointLightEnabled()
{
	return mpPointLight?mpPointLight->IsVisible() : false;
}

//-----------------------------------------------------------------------

bool iEdWorld::AddType(iEdObjectType* apType)
{
	return mpRootType->AddChildType(apType);
}

//-----------------------------------------------------------------------

iEdObjectType* iEdWorld::GetTypeByName(const tWString& asName)
{
	if(asName.empty()) return NULL;

	return mpRootType? 
			mpRootType->GetTypeByName(asName) : 
			NULL;
}

iEdObjectType* iEdWorld::GetTypeByID(const tIntVec& avID)
{
	if(avID.empty()) return NULL;

	return mpRootType? 
			mpRootType->GetTypeByID(avID) : 
			NULL;
}

iEdObjectType* iEdWorld::GetTypeByXmlElement(cXmlElement* apElement)
{
	if(apElement==NULL)	return NULL;

	iEdObjectType* pType =	mpRootType? 
										mpRootType->GetTypeByXmlElement(apElement) : 
										NULL;
	if(pType==NULL)
		Error("Not proper type found for loading element with value %s\n", apElement->GetValue().c_str());

	return pType;
}

//-----------------------------------------------------------------------

void iEdWorld::LogTypeHierarchy()
{
	if(mpRootType==NULL)
		return;

	Log("Object type tree: \n");

	tWString sHierarchy;
	mpRootType->LogTypeHierarchyHelper(sHierarchy, 0);

	Log("%ls\n", sHierarchy.c_str());
}

//-----------------------------------------------------------------------

bool iEdWorld::IsIDAvailable(int alID)
{
	if(alID<0)
		return false;

	return (GetObject(alID)==NULL);
}

//-----------------------------------------------------------------------

bool iEdWorld::IsNameAvailable(const tWString& asName)
{
	return GetObject(asName)==NULL;
}

//-----------------------------------------------------------------------

tWString iEdWorld::GenerateValidName(const tWString& asName, bool abTreatWholeNameAsBase)
{
	tWString sBaseName;
	tWString sGeneratedName;

	//size_t lNumericSuffixLength = 3;
	int lNumericSuffix = 0;

	if(abTreatWholeNameAsBase)
		sBaseName = asName;
	else
	{
		int lIndex;

		tString sNumericSuffix = cString::GetNumericSuffix( cString::To8Char(asName), &lIndex);

		if(lIndex==-1)
		{
			sBaseName = asName;
		}
		else
		{
			// If the suffix is preceded by a '_' character, consider it as valid
			if(asName.at(lIndex-1)==_W('_'))
			{
				lNumericSuffix = cString::ToInt(sNumericSuffix.c_str(), 0);
			}
			else
				lIndex = 0;
			
			sBaseName = cString::SubW(asName, 0, lIndex-1);
		}
	}
	
	do
	{
		sGeneratedName = sBaseName + _W("_") + cString::ToStringW(++lNumericSuffix);
	}
	while(IsNameAvailable(sGeneratedName)==false);
	
	return sGeneratedName;
}

//-----------------------------------------------------------------------

iEdObject* iEdWorld::CreateObjFromData(iEdObjectData* apData)
{
	if(apData==NULL) return NULL;
	
	// Check if the ID is taken, give a new one if so
	int lDataID = apData->GetID();
	if(IsIDAvailable(lDataID)==false)
	{
		int lNewID = GetFreeID();
		// If ID is not the default one, add a redirector
		if(lDataID!=-1) mmapIDRedirections.insert(tIDRedirectMap::value_type(lDataID, lNewID));

		apData->SetID(lNewID);
	}

	// Same with name
	if(IsNameAvailable(apData->GetName())==false)
	{
		tWString sName = GenerateValidName(apData->GetName(), false);
		apData->SetName(sName);
	}

	iEdObject* pObj = apData->CreateObject();
	if(AddObject(pObj)==false)
	{
		Log("Error creating object with ID %i and name %ls from object data\n", apData->GetID(), apData->GetName().c_str());
		hplDelete(pObj);
		hplDelete(apData);

		pObj = NULL;
	}

	//Log("Object %p created - %ls\n", pObj, pObj->GetName().c_str());

	return pObj;
}

//-----------------------------------------------------------------------

iEdObject* iEdWorld::CreateObjFromXmlElement(cXmlElement* apElement)
{
	iEdObjectType* pType = GetTypeByXmlElement(apElement);
	if(pType==NULL)
		return NULL;

	iEdObjectData* pData = pType->CreateData();
	if(pData==NULL)
	{
		FatalError("Error creating object from XML - type '%ls' - NULL data created\n", pType->GetFullName().c_str());
	}
	else if(pData->Load(apElement)==false)
	{
		Log("Error creating object from XML - type '%ls', ID %d\n", pType->GetFullName().c_str(), pData->GetID());
		hplDelete(pData);

		pData = NULL;
	}

	return CreateObjFromData(pData);
}

//-----------------------------------------------------------------------

void iEdWorld::DestroyObj(iEdObject* apObj)
{
	if(apObj==NULL) return;

	apObj->OnDestroy();

	RemoveObject(apObj);

	hplDelete(apObj);
}

//-----------------------------------------------------------------------

bool iEdWorld::AddObject(iEdObject* apObj)
{
	if(apObj==NULL)	return false;
	
	int lID = apObj->GetID();
	const tWString& sName = apObj->GetName();

	mmapObjects.insert(tEdObjectMap::value_type(lID, apObj));
	mmapObjectNames.insert(tEdObjectNameMap::value_type(sName, apObj));

	

	// Call on add stuff.
	//apObj->OnAddToWorld();

	//Log("Added entity with key %d - map size:%d\n", apObj->GetID(), mmapObjects.size());

	if(mlObjIDCounter < apObj->GetID())
		mlObjIDCounter = apObj->GetID();

	mpLastAddedObject = apObj;

	mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldAddRemObject, NULL);

	return true;
}

bool iEdWorld::RemoveObject(iEdObject* apObj)
{
	if(apObj==NULL || mbClearing)
		return false;

	bool bRemoved = (mmapObjects.erase(apObj->GetID()) != 0);
	mmapObjectNames.erase(apObj->GetName());

	if(bRemoved)
	{
		if(mpLastAddedObject==apObj) mpLastAddedObject = NULL;

		mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldAddRemObject, NULL);
	}

	return bRemoved;
}

//-----------------------------------------------------------------------

void iEdWorld::ClearObjects()
{
	mbClearing=true;

	tEdObjectMapIt it = mmapObjects.begin();
	for(;it!=mmapObjects.end(); ++it)
	{
		DestroyObj(it->second);
	}

	mmapObjects.clear();
	mmapObjectNames.clear();

	mlstUpdatedObjects.clear();

	mbClearing=false;
}

//-----------------------------------------------------------------------

void iEdWorld::CreateLoadWorldDialog()
{
	tWString sStartPath = msFilename;
	if(sStartPath.empty())
		sStartPath = msLastUsedPath;

	mpEditor->GetWindowHandler()->CreateLoadDialog(mvDestFilenames, sStartPath, 
													this, kGuiCallback(LoadWorldCallback), 
													msName, tWStringList(1,_W("*.")+msFileExt));
}

void iEdWorld::CreateSaveWorldDialog()
{
	tWString sStartPath = msFilename;
	if(sStartPath.empty())
		sStartPath = msLastUsedPath;

	mpEditor->GetWindowHandler()->CreateSaveDialog(msFilename, sStartPath, 
													this, kGuiCallback(SaveWorldCallback), 
													msName, _W("*.")+msFileExt);
}

//-----------------------------------------------------------------------

bool iEdWorld::LoadFromFile(const tWString& asFileName)
{
	mpEditor->Reset();

	msFilename = asFileName;
	cString::SetFileExtW(msFilename, msFileExt);

	cResources* pRes = mpEditor->GetEngine()->GetResources();
	pRes->AddResourceDir(cString::GetFilePathW(msFilename), false);

	iXmlDocument* pDoc = pRes->GetLowLevel()->CreateXmlDocument();
	if(pDoc->CreateFromFile(asFileName)==false)
	{
		Error("Could not load file %ls\n", asFileName.c_str());

		pRes->DestroyXmlDocument(pDoc);

		return false;
	}

	if(mpSessionData) mpSessionData->Load(pDoc);

	bool bLoadOK = LoadContentsFromXmlDoc(pDoc);
	if(bLoadOK)
	{
		PostDeployObjects(true);

		msLastUsedPath = cString::GetFilePathW(asFileName);

		mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldLoad, NULL);
	}
	else
	{
		mpEditor->Reset();
	}

	pRes->DestroyXmlDocument(pDoc);

	return bLoadOK;
}

//-----------------------------------------------------------------------

bool iEdWorld::SaveToFile(const tWString& asFileName)
{
	msFilename = asFileName;
	cString::SetFileExtW(msFilename, msFileExt);

	iXmlDocument* pDoc = mpEditor->GetEngine()->GetResources()->GetLowLevel()->CreateXmlDocument(msXmlValue);
	pDoc->SetPath(msFilename);

	if(mpSessionData) mpSessionData->Save(pDoc);
	
	bool bSaveOK = SaveContentsToXmlDoc(pDoc);

	if(bSaveOK && pDoc->Save())
	{
		mlLastSavedModificationStamp = mlModificationStamp;

		msLastUsedPath = cString::GetFilePathW(asFileName);

		mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldSave, NULL);
		OnWorldParamsChange();
	}

	hplDelete(pDoc);

	return bSaveOK;
}

bool iEdWorld::Save()
{
	if(msFilename.empty())
	{
		CreateSaveWorldDialog();
		return true;
	}
	else
		return SaveToFile(msFilename);
}

//-----------------------------------------------------------------------

void iEdWorld::AddRecentFile(const tWString& asX)
{
	if(asX.empty() || cString::GetFileExtW(asX)!=msFileExt) return;

	RemRecentFile(asX);
	mlstRecentFiles.push_back(asX);

	while(mlstRecentFiles.size()>=10)
		mlstRecentFiles.pop_front();
}

void iEdWorld::RemRecentFile(const tWString& asX)
{
	mlstRecentFiles.remove(asX);
}

const tWStringList& iEdWorld::GetRecentFiles()
{
	return mlstRecentFiles;
}

//-----------------------------------------------------------------------

bool iEdWorld::LoadFileIndices(cXmlElement* apElement)
{
	if(apElement==NULL)
		return false;

	tFileIndexMap::iterator it = mmapFileIndices.begin();
	for(;it!=mmapFileIndices.end();++it)
	{
		cFileIndex* pIndex = it->second;

		pIndex->Load(apElement);
	}

	return true;
}

bool iEdWorld::SaveFileIndices(cXmlElement* apElement)
{
	if(apElement==NULL)
		return false;

	tIndexedFileMapIt it = mmapFileIndices.begin();
	for(;it!=mmapFileIndices.end(); ++it)
	{
		cFileIndex* pIndex = it->second;
		pIndex->Save(apElement);
	}

	return false;
}

//-----------------------------------------------------------------------

int iEdWorld::Modify()
{
	mlModificationStamp++;

	return mlModificationStamp;
}

bool iEdWorld::Unmodify(int alStamp)
{
	if(mlModificationStamp==alStamp)
	{
		mlModificationStamp--;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------

bool iEdWorld::IsModified()
{
	return mlModificationStamp!=mlLastSavedModificationStamp;
}

//-----------------------------------------------------------------------

iEdObject* iEdWorld::GetObject(int alID)
{
	if(alID<0 || alID>mlObjIDCounter)
		return NULL;

	if(mmapIDRedirections.empty()==false)
	{
		// TODO:
	}
	tEdObjectMapIt it = mmapObjects.find(alID);

	if(it!=mmapObjects.end())
		return it->second;

	return NULL;
}

iEdObject* iEdWorld::GetObject(const tWString& asName)
{
	tEdObjectNameMapIt it = mmapObjectNames.find(asName);

	if(it!=mmapObjectNames.end())
		return it->second;

	return NULL;
}

//-----------------------------------------------------------------------

bool iEdWorld::HasObject(int alID)
{
	return GetObject(alID) != NULL;
}

bool iEdWorld::HasObject(const tWString& asName)
{
	return GetObject(asName) != NULL;
}

bool iEdWorld::HasObject(iEdObject* apObj)
{
    if(apObj==NULL) return false;

	iEdObject* pObj = GetObject(apObj->GetID());
	if(pObj==apObj)
		return true;

	return false;
	/*
	tEdObjectMapIt it = mmapObjects.begin();
	for(; it!=mmapObjects.end(); ++it)
	{
		if(it->second==apObj)
			return true;
	}

	return false;
	*/
}

//-----------------------------------------------------------------------

void iEdWorld::AddUpdatedObject(iEdObject* apObj)
{
	RemUpdatedObject(apObj);
	mlstUpdatedObjects.push_back(apObj);
}

void iEdWorld::RemUpdatedObject(iEdObject* apObj)
{
	mlstUpdatedObjects.remove(apObj);
}

//-----------------------------------------------------------------------

cFileIndex* iEdWorld::CreateFileIndex(const tString& asName)
{
	tIndexedFileMapIt it = mmapFileIndices.find(asName);
	if(it!=mmapFileIndices.end())
		return it->second;

	cFileIndex* pIndex = hplNew(cFileIndex,(this, asName));
	mmapFileIndices.insert(tFileIndexMap::value_type(asName, pIndex));

	return pIndex;
}

//-----------------------------------------------------------------------

/*
cIndexedFile* iEdWorld::GetIndexedFile(const tString& asIndexName, int alIdx)
{
	if(alIdx==-1) return NULL;

	tIndexedFileMapIt it = mmapFileIndices.find(asIndexName);
	if(it==mmapFileIndices.end())
		return NULL;

	tIndexedFileList& lstFile = it->second;
	tIndexedFileListIt itList = lstFile.begin();
	for(int i=0; itList!=lstFile.end(); ++itList, ++i)
	{
		if(i==alIdx)
		{
			cIndexedFile* pFile = *itList;

			return pFile;
		}
	}

	return NULL;
}
*/

//-----------------------------------------------------------------------

int iEdWorld::GetFreeID()
{
	return ++mlObjIDCounter;
}

//-----------------------------------------------------------------------

iEdAction* iEdWorld::CreateActionCreateObject(iEdObjectData* apNewObjData)
{
	return hplNew(cEdActionCreateObject,(this, apNewObjData));
}

//-----------------------------------------------------------------------

iEdAction* iEdWorld::CreateActionDeleteObjects(const tEdObjectList& alstObjectsToDelete)
{
	return hplNew(cEdActionDeleteObjects,(this, alstObjectsToDelete));
}

iEdAction* iEdWorld::CreateActionDeleteObject(iEdObject* apObj)
{
	return CreateActionDeleteObjects(tEdObjectList(1, apObj));
}

//-----------------------------------------------------------------------

iEdAction* iEdWorld::CreateActionCloneObjects(const tEdObjectList& alstObjectsToClone, const tWString& asPrefix, const tWString& asSuffix)
{
	return hplNew(cEdActionCloneObjects,(this, alstObjectsToClone, asPrefix, asSuffix));
}

iEdAction* iEdWorld::CreateActionCloneObject(iEdObject* apObj, const tWString& asPrefix, const tWString& asSuffix)
{
	return CreateActionCloneObjects(tEdObjectList(1,apObj), asPrefix, asSuffix);
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// WORLD - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool iEdWorld::LoadWorldCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
		LoadFromFile(mvDestFilenames.front());
	return true;
}
kGuiCallbackDeclaredFuncEnd(iEdWorld, LoadWorldCallback);

bool iEdWorld::SaveWorldCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
		SaveToFile(msFilename);
	return true;
}
kGuiCallbackDeclaredFuncEnd(iEdWorld, SaveWorldCallback);

//-----------------------------------------------------------------------

bool iEdWorld::LoadContentsFromXmlDoc(iXmlDocument*)
{
	return true;
}

bool iEdWorld::SaveContentsToXmlDoc(iXmlDocument* apDoc)
{
	if(apDoc==NULL)
		return false;

	return true;
}

//-----------------------------------------------------------------------

void iEdWorld::OnLoadLocalConfig(cConfigFile* apCfg)
{
	//////////////////////////////////
	// Recent Files
	int i=0;
	tWString sRecentFile = apCfg->GetStringW("General", "RecentFile"+cString::ToString(++i), _W(""));
	while(sRecentFile.empty()==false && i<=10)
	{
		AddRecentFile(sRecentFile);
		sRecentFile = apCfg->GetStringW("General", "RecentFile"+cString::ToString(++i), _W(""));
	}
}

void iEdWorld::OnSaveLocalConfig(cConfigFile* apCfg)
{
	tWStringListIt it = mlstRecentFiles.begin();
	int i=1;
	for(;it!=mlstRecentFiles.end(); ++it)
	{
		tString sFilename = cString::To8Char(*it);
		apCfg->SetString("General", "RecentFile"+cString::ToString(i++), sFilename);
	}
	while(i<=10)
	{
		apCfg->SetString("General", "RecentFile"+cString::ToString(i++), "");
	}
}

//-----------------------------------------------------------------------

void iEdWorld::OnInit()
{
	cScene* pScene = mpEditor->GetEngine()->GetScene();
	mpWorld = pScene->CreateWorld(cString::To8Char(msName));
	mpWorld->SetSkyBoxActive(true);

	//////////////////////////////////////////////
	// Global light creation
	mpAmbientLight = mpWorld->CreateLightBox("Ambient");
	mpAmbientLight->SetPosition(0);
	mpAmbientLight->SetDiffuseColor(cColor(0.4f,1));
	mpAmbientLight->SetSize(500);
	mpAmbientLight->SetBlendFunc(eLightBoxBlendFunc_Add);
	mpAmbientLight->SetBoxLightPrio(1);

	mpPointLight = mpWorld->CreateLightPoint("Point");
	mpPointLight->SetPosition(cVector3f(0,100,0));
	mpPointLight->SetRadius(500);
	mpPointLight->SetDiffuseColor(cColor(1,1));


	///////////////////////////////////////////
	// Temp world
	mpTempWorld = pScene->CreateWorld("TempWorld");

	if(mpRootType->PostCreateSetUp()==false)
		FatalError("EdWorld::Compile - Error occured postbuilding Object type hierarchy");

	LogTypeHierarchy();

	mpSessionData = CreateSessionData();
}

//-----------------------------------------------------------------------

void iEdWorld::OnReset()
{
	msFilename.clear();

	mlModificationStamp = 0;
	mlLastSavedModificationStamp = mlModificationStamp;

	mmapIDRedirections.clear();

	mlObjIDCounter = -1;

	SetBGColor(cColor(0.3f, 1));
	
	ClearObjects();
}

//-----------------------------------------------------------------------

void iEdWorld::OnUpdate()
{
	if(msFileToLoadOnUpdate.empty()==false)
	{
		LoadFromFile(msFileToLoadOnUpdate);
		msFileToLoadOnUpdate.clear();
	}

	if(mlstUpdatedObjects.empty())
		return;

	//Log("-------------EdWorld::OnUpdate starts. Size %d\n", mlstUpdatedObjects.size());
	
	tEdObjectList lstPendingObjects;
	tEdObjectListIt it = mlstUpdatedObjects.begin();
	for(;it!=mlstUpdatedObjects.end();++it)
	{
		iEdObject* pObj = *it;
		if(pObj->Update()==false)
		{
			lstPendingObjects.push_back(pObj);
		}
	}

	mlstUpdatedObjects = lstPendingObjects;

	// We insert stuff at the back cos mlstUpdatedObjects might have been updated in the world update.
	//mlstUpdatedObjects.insert(mlstUpdatedObjects.end(), lstPendingObjects.begin(), lstPendingObjects.end());

	//Log("-------------EdWorld::OnUpdate Ends. Size %d\n\n", mlstUpdatedObjects.size());
}

//-----------------------------------------------------------------------

void iEdWorld::OnWorldLoad()
{
	AddRecentFile(msFilename);
}

void iEdWorld::OnWorldSave()
{
	AddRecentFile(msFilename);
}

//-----------------------------------------------------------------------

void iEdWorld::OnWorldParamsChange()
{
}

void iEdWorld::OnWorldObjModify()
{
	OnWorldParamsChange();
}

//-----------------------------------------------------------------------

void iEdWorld::OnDraw(const cModuleDrawData& aData)
{
	if(aData.mpViewport->GetEngineViewport()->GetWorld()!=mpWorld)
		return;

	tEdObjectMapIt it = mmapObjects.begin();
	for(;it!=mmapObjects.end(); ++it)
	{
		iEdObject* pObj = it->second;
		pObj->OnDraw(aData);
	}
}

//-----------------------------------------------------------------------

void iEdWorld::PostDeployObjects(bool abDestroyData)
{
	tEdObjectMapIt it = mmapObjects.begin();
	for(;it!=mmapObjects.end(); ++it)
	{
		iEdObject* pObj = it->second;
		pObj->PostDeployAll(abDestroyData);
	}
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// WORLD - PRIVATE METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iEdWorld::SetRootType(iEdObjectType* apType)
{
	if(apType==NULL)
	{
		FatalError("Setting NULL root to world");
		return;
	}

	if(apType->Init()==false)
	{
		Error("Failed to set root type %ls\n", apType->GetName().c_str());
		return;
	}

	mpRootType = apType;
	mpRootType->SetWorld(this);
}

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
