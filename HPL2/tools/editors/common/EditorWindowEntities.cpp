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

#include "EditorWindowEntities.h"
#include "EditorEditModeEntities.h"
#include "EditorThumbnailBuilder.h"

#include "EditorGrid.h"
#include "EditorWorld.h"

#include "EditorIndex.h"

#include "EditorHelper.h"

//-------------------------------------------------------------------

cEditorObjectIndexEntryEntity::cEditorObjectIndexEntryEntity(cEditorObjectIndexDirEntities* apDir) : iEditorObjectIndexEntryMeshObject(apDir)
{
}

//-------------------------------------------------------------------

cMeshEntity* cEditorObjectIndexEntryEntity::CreateTempEntity(cWorld* apWorld)
{
	iEditorObjectIndex* pIndex = mpParentDir->GetIndex();
	iEditorBase* pEditor = pIndex->GetEditor();

	tString sSourceFile = cString::To8Char(GetFileNameFullPath());

	cMeshEntity* pEnt = pEditor->GetEngineEntityLoader()->LoadEntFile(-1, "TempObject_"+sSourceFile, sSourceFile, apWorld, false, false, false, false, false);
	
	pEnt->SetSourceFile(sSourceFile);
	pEnt->SetActive(false);
	pEnt->SetVisible(false);

	return pEnt;
}

//-------------------------------------------------------------------

cEditorObjectIndexDirEntities::cEditorObjectIndexDirEntities(cEditorObjectIndexEntities* apIndex, cEditorObjectIndexDirEntities* apParent) : iEditorObjectIndexDir(apIndex, apParent)
{
}

iEditorObjectIndexEntry* cEditorObjectIndexDirEntities::CreateEntry()
{
	return hplNew(cEditorObjectIndexEntryEntity,(this));
}

//-------------------------------------------------------------------

cEditorObjectIndexEntities::cEditorObjectIndexEntities(iEditorBase* apEditor,
																 const tWString& asBaseFolder) : iEditorObjectIndex(apEditor,
																													asBaseFolder,
																													_W("enp"),
																													"Entities",
																													"Entity",
																													true)
{
	AddFileFilter(_W("*.ent"));
}

//-------------------------------------------------------------------

iEditorObjectIndexDir* cEditorObjectIndexEntities::CreateDir(iEditorObjectIndexDir* apParent)
{
	return hplNew(cEditorObjectIndexDirEntities, (this, (cEditorObjectIndexDirEntities*)apParent));
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEditorWindowEntities::cEditorWindowEntities(cEditorEditModeEntities* apEditMode,
											 const tWStringVec& avBaseDirs) : cEditorWindowObjectBrowser(apEditMode,
																									 avBaseDirs,
																									 false)
{
	mbRotateUpdated = true;
}

cEditorWindowEntities::~cEditorWindowEntities()
{
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEditorWindowEntities::Reset()
{
	mpObjectList->SetSelectedItem(-1);
	UpdateObjectInfo();
	ResetInputs();
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEditorWindowEntities::OnSetActive(bool abX)
{
}

//-------------------------------------------------------------------

bool cEditorWindowEntities::Input_OnTextBoxEnter(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntities, Input_OnTextBoxEnter);

//-------------------------------------------------------------------

void cEditorWindowEntities::OnInitLayout()
{
	cEditorWindowObjectBrowser::OnInitLayout();
	mpBGFrame->SetSize(mpBGFrame->GetSize()+cVector2f(0,200));
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

void cEditorWindowEntities::ResetInputs()
{
}

//-------------------------------------------------------------------

iEditorObjectIndex* cEditorWindowEntities::CreateSpecificIndex(iEditorBase* apEditorBase, const tWString& asFolder)
{
	return hplNew(cEditorObjectIndexEntities,(apEditorBase, asFolder));
}

//-------------------------------------------------------------------

cMeshEntity* cEditorWindowEntities::CreatePreviewEntity(iEditorObjectIndexEntryMeshObject* apEntry)
{
	if(apEntry==NULL)
		return NULL;

	cResources* pRes = mpEditor->GetEngine()->GetResources();
	tWString sFullPath = pRes->GetFileSearcher()->GetFilePath(apEntry->GetFileName());
	cWorld* pWorld = mpEditor->GetEditorWorld()->GetWorld();
	cEditorEntityLoader* pLoader = mpEditor->GetEngineEntityLoader();
	
	cMeshEntity* pEntity = pLoader->LoadEntFile(0, "PreviewEntity", apEntry->GetFileName(), pWorld, false, false, false, false, false);
	
	return pEntity;
}



//-------------------------------------------------------------------
