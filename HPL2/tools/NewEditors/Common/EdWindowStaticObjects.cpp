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

#include "../Common/EdWindowStaticObjects.h"
#include "../Common/EdEditModeStaticObjects.h"

//#include "../Common/EdThumbnailBuilder.h"
#include "../Common/EdGrid.h"
#include "../Common/EdWorld.h"

#include "../Common/EdObjStaticObject.h"

//-------------------------------------------------------------------

cEdObjectIndexEntryStaticObject::cEdObjectIndexEntryStaticObject(cEdObjectIndexDirStaticObjects* apDir) : iEdObjectIndexEntryMeshObject(apDir)
{
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

cEdObjectIndexDirStaticObjects::cEdObjectIndexDirStaticObjects(cEdObjectIndexStaticObjects* apIndex, cEdObjectIndexDirStaticObjects* apParent) : iEdObjectIndexDir(apIndex, apParent)
{
}

iEdObjectIndexEntry* cEdObjectIndexDirStaticObjects::CreateEntry()
{
	return hplNew(cEdObjectIndexEntryStaticObject,(this));
}

//-------------------------------------------------------------------

//-------------------------------------------------------------------
//-------------------------------------------------------------------

cEdObjectIndexStaticObjects::cEdObjectIndexStaticObjects(iEditor* apEditor,
																 const tWString& asBaseFolder) : iEdObjectIndex(apEditor,
																													asBaseFolder,
																													_W("sop"),
																													"StaticObjects",
																													"StaticObject", 
																													true)
{
	AddFileFilter(_W("*.dae"));
}

//-------------------------------------------------------------------

iEdObjectIndexDir* cEdObjectIndexStaticObjects::CreateDir(iEdObjectIndexDir* apParent)
{
    return hplNew(cEdObjectIndexDirStaticObjects,(this, (cEdObjectIndexDirStaticObjects*)apParent));
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEdWindowStaticObjects::cEdWindowStaticObjects(iEdEditMode* apMode,
												const tWStringVec& avBaseDirs) : cEdWindowObjectBrowser(apMode,avBaseDirs, true)
{
	mbScaleUpdated = true;
	mbRotateUpdated = true;
}

cEdWindowStaticObjects::~cEdWindowStaticObjects()
{
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

bool cEdWindowStaticObjects::SetUpCreationData(iEdObjectData* apData)
{
	iEdObjectIndexEntry* pEntry = GetSelectedObject();
	if(pEntry==NULL) return false;

	iEdWorld* pWorld = mpEditor->GetWorld();

	///////////////////////////////////////
	// Build name
	tWString sRelPath = pEntry->GetDir()->GetRelPath();
	if(sRelPath.empty()==false)
		sRelPath = cString::AddSlashAtEndW(sRelPath, _W('_'));
	tWString sName = sRelPath + cString::To16Char(pEntry->GetEntryName());
	apData->SetName(sName);

	///////////////////////////////////////
	// Set up filename
	tString sFilename = cString::To8Char(pEntry->GetFileNameFullPath());
	apData->SetString(eStaticObjectStr_Filename, sFilename);

	///////////////////////////////////////
	// Set up file index
	cIndexedFile* pFile = cTypeStaticObject::mpIndex->CreateIndexedFile(sFilename);
	apData->SetInt(eStaticObjectInt_FileIndex, pFile->mlID);

	return true;
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEdWindowStaticObjects::OnCreateLayout()
{
	cEdWindowObjectBrowser::OnCreateLayout();
	SetHeight(GetBG()->GetSize().y+200);

	//////////////////////
	// Set up layout
	mpSelectionGroup->SetText(_W("Static objects"));
	mpObjectList->SetName("Static objects list");

	cVector3f vPos;
	cVector2f vSize;

	///////////////////////////////////////////////
	// Scale and Rotation inputs
	vPos = cVector3f(5,520,0.1f);

	AddScaleControls();
	mpGScale->SetPosition(vPos);
	AddRotateControls();
	vPos.y+= mpGScale->GetSize().y+10;
	mpGRotate->SetPosition(vPos);
}

//-------------------------------------------------------------------

//-------------------------------------------------------------------

void cEdWindowStaticObjects::ResetInputs()
{
}


//-------------------------------------------------------------------

iEdObjectIndex* cEdWindowStaticObjects::CreateSpecificIndex(iEditor* apEditor, const tWString& asFolder)
{
	cEdObjectIndexStaticObjects* pIndex = hplNew( cEdObjectIndexStaticObjects,(apEditor, asFolder));

	return pIndex;
}

//-------------------------------------------------------------------

cMeshEntity* cEdWindowStaticObjects::CreatePreviewEntity(iEdObjectIndexEntryMeshObject* apEntry)
{
	if(apEntry==NULL)
		return NULL;

	cMeshEntity* pEnt = NULL;
	cMesh* pMesh = mpEditor->GetEngine()->GetResources()->GetMeshManager()->CreateMesh(cString::To8Char(apEntry->GetFileNameRelPath()));
	if(pMesh)
	{
		pEnt = mpEditor->GetWorld()->GetEngWorld()->CreateMeshEntity("PreviewEntity", pMesh);
		Log("Created preview ent %p\n", pEnt);
	}

	return pEnt;
}

//-------------------------------------------------------------------


