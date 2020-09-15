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

#include "EntityWrapperStaticObject.h"

#include "EditorBaseClasses.h"
#include "EditorWorld.h"

#include "EditorWindowEntityEditBoxStaticObject.h"

#include "EditorWindowViewport.h"

#include "EditorHelper.h"

#include "EngineEntity.h"

//---------------------------------------------------------------------------

cEntityWrapperDataStaticObject::cEntityWrapperDataStaticObject(iEntityWrapperType* apType) : iEntityWrapperData(apType)
{
}

//---------------------------------------------------------------------------

bool cEntityWrapperDataStaticObject::Load(cXmlElement* apElement)
{
	if(iEntityWrapperData::Load(apElement)==false)
		return false;

	iEditorWorld* pWorld = mpType->GetWorld();
	int lFileIndex = GetInt(eStaticObjectInt_FileIndex);
	if(lFileIndex==-1)
	{
		tWString sPath = pWorld->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(apElement->GetAttributeString("Filename"));
		lFileIndex = pWorld->AddFilenameToIndex("StaticObjects", cString::To8Char(sPath));
		SetInt(eStaticObjectInt_FileIndex, lFileIndex);
	}

	const tString& sFilename = pWorld->GetFilenameFromIndex("StaticObjects",lFileIndex);
	if(sFilename=="")
	{
		pWorld->SetLoadErrorMessage(_W("File index out of bounds!"));
		return false;
	}

	SetString(eStaticObjectStr_Filename, sFilename);

	return true;
}

//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataStaticObject::CreateSpecificEntity()
{
	// Debug
	//Log("Creating static object\n");
	/////////////////////////////////////////////////////////////////////////////////////////
	// Check so the File Index is still valid ! (can become invalid after undoing a delete
	iEditorWorld* pWorld = mpType->GetWorld();

	int lFileIndex = GetInt(eStaticObjectInt_FileIndex);
	const tString& sCurrentFile = GetString(eStaticObjectStr_Filename);
	tWString sPath = pWorld->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(sCurrentFile);
	int lFileIndexForCurrentFile = pWorld->AddFilenameToIndex("StaticObjects", cString::To8Char(sPath));

	// the If first index and the one we just retrieved are different, update first
	if(lFileIndex!=lFileIndexForCurrentFile)
		SetInt(eStaticObjectInt_FileIndex, lFileIndexForCurrentFile);

	return hplNew(cEntityWrapperStaticObject,(this));
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEntityWrapperStaticObject::cEntityWrapperStaticObject(iEntityWrapperData* apData) : iEntityWrapper(apData)
{
}

cEntityWrapperStaticObject::~cEntityWrapperStaticObject()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEntityWrapperStaticObject::GetProperty(int alPropID, bool& abX)
{
	if(iEntityWrapper::GetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case eStaticObjectBool_Collides:
		abX = GetCollides();
		break;
	case eStaticObjectBool_CastShadows:
		abX = GetCastShadows();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperStaticObject::GetProperty(int alPropID, int& alX)
{
	if(iEntityWrapper::GetProperty(alPropID, alX)==true)
		return true;

	switch(alPropID)
	{
	case eStaticObjectInt_FileIndex:
		alX = GetFileIndex();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperStaticObject::GetProperty(int alPropID, tString& asX)
{
	if(iEntityWrapper::GetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case eStaticObjectStr_Filename:
		asX = GetFilename();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperStaticObject::SetProperty(int alPropID, const bool& abX)
{
	if(iEntityWrapper::SetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case eStaticObjectBool_Collides:
		SetCollides(abX);
		break;
	case eStaticObjectBool_CastShadows:
		SetCastShadows(abX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperStaticObject::SetProperty(int alPropID, const int& alX)
{
	if(iEntityWrapper::SetProperty(alPropID, alX)==true)
		return true;

	switch(alPropID)
	{
	case eStaticObjectInt_FileIndex:
		SetFileIndex(alX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperStaticObject::SetProperty(int alPropID, const tString& asX)
{
	if(iEntityWrapper::SetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case eStaticObjectStr_Filename:
		SetFilename(asX);
		break;
	default:
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------

void cEntityWrapperStaticObject::SetCastShadows(bool abX)
{
	mbCastShadows = abX;
	if(mpEngineEntity)
		((iEngineEntityMesh*)mpEngineEntity)->SetCastShadows(abX);
}

//---------------------------------------------------------------------------

void cEntityWrapperStaticObject::SetFilename(const tString& asX)
{
	if(msFilename == asX)
		return;

	msFilename = asX;

	if(mpEngineEntity==NULL)
		return;

	tWString sPath = GetEditorWorld()->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(asX);
	int lFileIndex = GetEditorWorld()->AddFilenameToIndex("StaticObjects", cString::To8Char(sPath));

	SetFileIndex(lFileIndex);

	CreateEngineEntity();
}

//---------------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperStaticObject::CreateEditBox( cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = hplNew(cEditorWindowEntityEditBoxStaticObject,(apEditMode,this));

	return pEditBox;
}

bool cEntityWrapperStaticObject::IsAffectedByDecal(bool abAffectsStaticObject, bool abAffectsPrimitive, bool abAffectsEntity)
{
	return abAffectsStaticObject && IsVisible() && IsCulledByClipPlanes()==false;
}

//---------------------------------------------------------------------------

iEngineEntity* cEntityWrapperStaticObject::CreateSpecificEngineEntity()
{
	iEditorWorld* pWorld = GetEditorWorld();
	cResources* pRes = pWorld->GetEditor()->GetEngine()->GetResources();

	if(msFilename=="")
	{
		if(mlFileIndex!=-1)
		{
			msFilename = pWorld->GetFilenameFromIndex("Entities",mlFileIndex);
			mpData->SetString(eStaticObjectStr_Filename, msFilename);
		}
		else
			Log("Error creating Entity named %s, ID %d\n", msName.c_str(), mlID);
	}

	return hplNew(cEngineEntityLoadedMesh,(this, msFilename));
}

//---------------------------------------------------------------------------
