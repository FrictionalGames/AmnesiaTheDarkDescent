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

#include "../Common/EdObjStaticObject.h"

#include "../Common/Editor.h"
#include "../Common/EdScnWorld.h"
#include "../Common/EdSurfacePicker.h"

#include "../Common/EdEditPaneStaticObject.h"

#include "../Common/EdWindowViewport.h"

//#include "../Common/EdHelper.h"

#include "../Common/EdEngineObject.h"

//---------------------------------------------------------------------------

cFileIndex* cTypeStaticObject::mpIndex = NULL;

//---------------------------------------------------------------------------

cTypeStaticObject::cTypeStaticObject() : iEdScnObjType(_W("Static Object"), "StaticObject")
{
	AddBool(eStaticObjectBool_Collides, "Collides");
	AddBool(eStaticObjectBool_CastShadows, "CastShadows");

	AddInt(eStaticObjectInt_FileIndex, "FileIndex", -1, ePropStep_PreCreate);
	AddString(eStaticObjectStr_Filename, "Filename", "", ePropStep_PreCreate, false);
}

bool cTypeStaticObject::Init()
{
	if(iEdScnObjType::Init()==false)
		return false;

	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(GetWorld());
	cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();
	pPicker->RegisterSurfaceType(this);

	if(mpIndex==NULL)
		mpIndex = pWorld->CreateFileIndex("StaticObjects");

	return true;
}
//---------------------------------------------------------------------------

iEdObjectData* cTypeStaticObject::CreateTypeSpecificData()
{
	return hplNew(cEdObjStaticObjectData,(this));
}

//---------------------------------------------------------------------------

cEdObjStaticObjectData::cEdObjStaticObjectData(iEdObjectType* apType) : iEdScnObjData(apType)
{
	mpMeshFile = NULL;
}

//---------------------------------------------------------------------------

void cEdObjStaticObjectData::CopyToObject(iEdObject* apObj, ePropStep aOrder)
{
	iEdScnObjData::CopyToObject(apObj, aOrder);
}

//---------------------------------------------------------------------------

void cEdObjStaticObjectData::CopyFromObject(iEdObject* apObj)
{
	iEdScnObjData::CopyFromObject(apObj);

	cEdObjStaticObject* pObj = static_cast<cEdObjStaticObject*>(apObj);

	mpMeshFile = pObj->GetMeshFile();

	SetInt(eStaticObjectInt_FileIndex, mpMeshFile?mpMeshFile->mlID:-1);

	//SetString(eStaticObjectStr_Filename, mpMeshFile->msRelativePath);
}

//---------------------------------------------------------------------------

bool cEdObjStaticObjectData::LoadSpecificData(cXmlElement* apElement)
{
	if(iEdScnObjData::LoadSpecificData(apElement)==false)
		return false;

	iEdWorld* pWorld = mpType->GetWorld();
	int lFileIndex = GetInt(eStaticObjectInt_FileIndex);
	if(lFileIndex==-1)
		mpMeshFile = cTypeStaticObject::mpIndex->CreateIndexedFile(apElement->GetAttributeString("Filename"));
	else
		mpMeshFile = cTypeStaticObject::mpIndex->GetIndexedFile(lFileIndex);

	if(mpMeshFile==NULL)
	{
		//pWorld->SetLoadErrorMessage(_W("Mesh file not found!"));
		return false;
	}

	SetString(eStaticObjectStr_Filename, mpMeshFile->msRelativePath);

	return true;
}

/*
bool cEdObjStaticObjectData::SaveSpecificData(cXmlElement* apElement)
{
	if(iEdScnObjData::SaveSpecificData(apElement)==false)
		return false;

	iEdWorld* pWorld = mpType->GetWorld();
	int lFileIndex = GetInt(eStaticObjectInt_FileIndex);
	if(lFileIndex==-1)
		mpMeshFile = cTypeStaticObject::mpIndex->CreateIndexedFile(apElement->GetAttributeString("Filename"));
	else
		mpMeshFile = cTypeStaticObject::mpIndex->GetIndexedFile(lFileIndex);

	if(mpMeshFile==NULL)
	{
		//pWorld->SetLoadErrorMessage(_W("Mesh file not found!"));
		return false;
	}

	SetString(eStaticObjectStr_Filename, mpMeshFile->msRelativePath);

	return true;
}
*/

//---------------------------------------------------------------------------

iEdObject* cEdObjStaticObjectData::CreateTypeSpecificObject()
{
	// Debug
	//Log("Creating static object\n");
	/////////////////////////////////////////////////////////////////////////////////////////
	// Check so the File Index is still valid ! (can become invalid after undoing a delete
	/*

	int lFileIndex = GetInt(eStaticObjectInt_FileIndex);
	const tString& sCurrentFile = GetString(eStaticObjectStr_Filename);
	tWString sPath = pWorld->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(sCurrentFile);
	int lFileIndexForCurrentFile = pWorld->AddFilenameToIndex("StaticObjects", cString::To8Char(sPath));

	// the If first index and the one we just retrieved are different, update first
	if(lFileIndex!=lFileIndexForCurrentFile)
		SetInt(eStaticObjectInt_FileIndex, lFileIndexForCurrentFile);

		*/

	return hplNew(cEdObjStaticObject,(this));
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjStaticObject::cEdObjStaticObject(iEdObjectData* apData) : iEdScnObject(apData)
{
	mpMeshFile = NULL;
}

cEdObjStaticObject::~cEdObjStaticObject()
{
	SetMeshFile(NULL);
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEdObjStaticObject::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case eStaticObjectBool_Collides:
		abX = GetCollides();	break;
	case eStaticObjectBool_CastShadows:
		abX = GetCastShadows();		break;
	default:
		return iEdScnObject::GetProperty(alPropID, abX);
	}

	return true;
}

bool cEdObjStaticObject::GetProperty(int alPropID, int& alX)
{
	switch(alPropID)
	{
	case eStaticObjectInt_FileIndex:
		alX = mpMeshFile? mpMeshFile->mlID : -1;
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, alX);
	}

	return true;
}

bool cEdObjStaticObject::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eStaticObjectStr_Filename:
		asX = mpMeshFile?mpMeshFile->msRelativePath : "";
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, asX);
	}

	return true;
}

bool cEdObjStaticObject::SetProperty(int alPropID, const bool abX)
{
	switch(alPropID)
	{
	case eStaticObjectBool_Collides:
		SetCollides(abX); break;
	case eStaticObjectBool_CastShadows:
		SetCastShadows(abX); break;
	default:
		return iEdScnObject::SetProperty(alPropID, abX);
	}

	return true;
}

bool cEdObjStaticObject::SetProperty(int alPropID, const int alX)
{
	switch(alPropID)
	{
	case eStaticObjectInt_FileIndex:
		//SetFileIndex(alX); break;
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, alX);
	}

	return true;
}

bool cEdObjStaticObject::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eStaticObjectStr_Filename:
		SetFilename(asX);	break;
	default:
		return iEdScnObject::SetProperty(alPropID, asX);
	}

	return true;
}

//---------------------------------------------------------------------------

void cEdObjStaticObject::SetCastShadows(bool abX)
{
	mbCastShadows = abX;
	if(mpEngObject)
		((iEngineMesh*)mpEngObject)->SetCastShadows(abX);
}

//---------------------------------------------------------------------------

void cEdObjStaticObject::SetMeshFile(cIndexedFile* apRes)
{
	if(mpMeshFile==apRes)
		return;

	if(mpMeshFile) mpMeshFile->Release();

	mpMeshFile = apRes;

	if(mpMeshFile) mpMeshFile->Assign();
}

//---------------------------------------------------------------------------

void cEdObjStaticObject::SetFilename(const tString& asX)
{
	cIndexedFile* pTemp = cTypeStaticObject::mpIndex->CreateIndexedFile(asX);

	SetMeshFile(pTemp);

	if(mpEngObject)
	{
		hplDelete(mpEngObject);
		mpEngObject=NULL;
	}

	SetUpEngineObject();
}

//---------------------------------------------------------------------------


iEdEditPane* cEdObjStaticObject::CreateEditPane()
{
	return hplNew(cEdEditPaneStaticObject,(this));
}

//---------------------------------------------------------------------------

bool cEdObjStaticObject::IsAffectedByDecal(bool abAffectsStaticObject, bool abAffectsPrimitive, bool abAffectsEntity)
{
	return abAffectsStaticObject && IsVisible() && IsCulledByClipPlanes()==false;
}

//---------------------------------------------------------------------------

iEngineObject* cEdObjStaticObject::CreateEngineObject()
{
	if(mpMeshFile==NULL)
		return NULL;

	return hplNew(cEngineLoadedMesh,(this, mpMeshFile->msRelativePath));

	/*if(msFilename=="")
	{
		if(mlFileIndex!=-1)
		{
			msFilename = pWorld->GetFilenameFromIndex("Entities",mlFileIndex);
			mpData->SetString(eStaticObjectStr_Filename, msFilename);
		}
		else
			Log("Error creating Entity named %s, ID %d\n", msName.c_str(), mlID);
	}

	return hplNew(cEngineLoadedMesh,(this, msFilename));
	*/
}

//---------------------------------------------------------------------------
