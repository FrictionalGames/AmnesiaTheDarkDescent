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

#include "../Common/EdWindowEntities.h"
#include "../Common/EdEditModeEntities.h"
#include "../Common/EdThumbnailBuilder.h"

#include "../Common/EdGrid.h"
#include "../Common/EdWorld.h"

#include "../Common/EdIndex.h"
#include "../Common/EdWorld.h"
#include "../Common/EdObjEntity.h"

#include "../Common/EdEntityLoader.h"

//#include "EditorHelper.h"

cEdEntityLoader* cEdObjectIndexEntities::mpLoader = NULL;

//-------------------------------------------------------------------

cEdObjectIndexEntryEntity::cEdObjectIndexEntryEntity(cEdObjectIndexDirEntities* apDir) : iEdObjectIndexEntryMeshObject(apDir)
{
}

//-------------------------------------------------------------------

cMeshEntity* cEdObjectIndexEntryEntity::CreateTempObject(cWorld* apWorld)
{
	cEdObjectIndexEntities* pIndex = static_cast<cEdObjectIndexEntities*>(mpParentDir->GetIndex());
	iEditor* pEditor = pIndex->GetEditor();

	tString sSourceFile = cString::To8Char(GetFileNameFullPath());

	cMeshEntity* pEnt = pIndex->mpLoader->LoadEntFile(-1, "TempObject_"+sSourceFile, sSourceFile, apWorld, false, false, false, false, false);
	
	pEnt->SetSourceFile(sSourceFile);
	pEnt->SetActive(false);
	pEnt->SetVisible(false);

	return pEnt;
}

//-------------------------------------------------------------------

cEdObjectIndexDirEntities::cEdObjectIndexDirEntities(cEdObjectIndexEntities* apIndex, cEdObjectIndexDirEntities* apParent) : iEdObjectIndexDir(apIndex, apParent)
{
}

iEdObjectIndexEntry* cEdObjectIndexDirEntities::CreateEntry()
{
	return hplNew(cEdObjectIndexEntryEntity,(this));
}

//-------------------------------------------------------------------

cEdObjectIndexEntities::cEdObjectIndexEntities(iEditor* apEditor,
												const tWString& asBaseFolder,
												cEdEntityLoader* apLoader) : iEdObjectIndex(apEditor,
																								asBaseFolder,
																								_W("enp"),
																								"Entities",
																								"Entity",
																								true)
{
	AddFileFilter(_W("*.ent"));

	if(mpLoader==NULL)
        mpLoader = apLoader;
}

//-------------------------------------------------------------------

iEdObjectIndexDir* cEdObjectIndexEntities::CreateDir(iEdObjectIndexDir* apParent)
{
	return hplNew(cEdObjectIndexDirEntities, (this, (cEdObjectIndexDirEntities*)apParent));
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEdWindowEntities::cEdWindowEntities(cEdEditModeEntities* apEditMode,
											 const tWStringVec& avBaseDirs) : cEdWindowObjectBrowser(apEditMode,
																									 avBaseDirs,
																									 false)
{
	mbRotateUpdated = true;
}

cEdWindowEntities::~cEdWindowEntities()
{
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

bool cEdWindowEntities::SetUpCreationData(iEdObjectData* apData)
{
	iEdObjectIndexEntry* pEntry = GetSelectedObject();
	if(pEntry==NULL) return false;

	///////////////////////////////////////
	// Build name
	apData->SetName(cString::To16Char(pEntry->GetEntryName()));

	///////////////////////////////////////
	// Set up filename
	tString sFilename = cString::To8Char(pEntry->GetFileNameFullPath());
	apData->SetString(eEntityStr_Filename, sFilename);

	///////////////////////////////////////
	// Set up file index
	/*
	cIndexedFile* pFile = mpEditor->GetWorld()->CreateIndexedFile("Entities", sFilename);
	cEdObjDataEntity* pData = static_cast<cEdObjDataEntity*>(apData);
	apData->SetInt(eEntityInt_FileIndex, pFile->mlID);
	*/

	///////////////////////////////////////
	// Specific transforms
	if(mpInpCreateOnSurface->GetValue()==false)
		apData->SetVec3f(eScnVec3f_Rotation, GetRotate());

	return true;
}

/*
void cEdWindowEntities::Reset()
{
	mpObjectList->SetSelectedItem(-1);
	UpdateObjectInfo();
	ResetInputs();
}
*/

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEdWindowEntities::OnSetActive(bool abX)
{
}

//-------------------------------------------------------------------

void cEdWindowEntities::OnCreateLayout()
{
	cEdWindowObjectBrowser::OnCreateLayout();
	SetHeight(GetBG()->GetSize().y+200);
	//////////////////////
	// Set up layout
	mpSelectionGroup->SetText(_W("Entities"));

	cVector3f vPos = cVector3f(5,515,0.1f);
	cVector2f vSize;

	///////////////////////////////////////////////
	// Scale and Rotation inputs
	AddCreateOnSurfaceControls();
	mpGSurface->SetPosition(vPos);
	vPos.y+= mpGSurface->GetSize().y +10;

	AddScaleControls();
	mpGScale->SetPosition(vPos);
	vPos.y+= mpGScale->GetSize().y +10;
	AddRotateControls();
	mpGRotate->SetPosition(vPos);

	//AddRotateControls();
	//AddScaleControls();

	BuildObjectSetList();
}

//-------------------------------------------------------------------

iEdObjectIndex* cEdWindowEntities::CreateSpecificIndex(iEditor* apEditorBase, const tWString& asFolder)
{
	cEdEditModeEntities* pMode = static_cast<cEdEditModeEntities*>(mpEditMode);
	cTypeRootEntity* pType = static_cast<cTypeRootEntity*>(pMode->GetMainType());
	
	return hplNew(cEdObjectIndexEntities,(apEditorBase, asFolder, pType->GetEntityLoader()));
}

//-------------------------------------------------------------------

cMeshEntity* cEdWindowEntities::CreatePreviewEntity(iEdObjectIndexEntryMeshObject* apEntry)
{
	if(apEntry==NULL)
		return NULL;

	cResources* pRes = mpEditor->GetEngine()->GetResources();
	tWString sFullPath = pRes->GetFileSearcher()->GetFilePath(apEntry->GetFileName());
	cWorld* pWorld = mpEditor->GetWorld()->GetEngWorld();
	//cEdEntityLoader* pLoader = mpEditor->GetEngineEntityLoader();
	
	//cMeshEntity* pEntity = pLoader->LoadEntFile(0, "PreviewEntity", apEntry->GetFileName(), pWorld, false, false, false, false, false);
	
	//return pEntity;

	return NULL;
}



//-------------------------------------------------------------------
