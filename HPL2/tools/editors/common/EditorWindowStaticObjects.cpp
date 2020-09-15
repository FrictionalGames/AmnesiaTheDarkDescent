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

#include "EditorWindowStaticObjects.h"
#include "EditorEditModeStaticObjects.h"

#include "EditorThumbnailBuilder.h"
#include "EditorGrid.h"
#include "EditorWorld.h"

//-------------------------------------------------------------------

cEditorObjectIndexEntryStaticObject::cEditorObjectIndexEntryStaticObject(cEditorObjectIndexDirStaticObjects* apDir) : iEditorObjectIndexEntryMeshObject(apDir)
{
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

cEditorObjectIndexDirStaticObjects::cEditorObjectIndexDirStaticObjects(cEditorObjectIndexStaticObjects* apIndex, cEditorObjectIndexDirStaticObjects* apParent) : iEditorObjectIndexDir(apIndex, apParent)
{
}

iEditorObjectIndexEntry* cEditorObjectIndexDirStaticObjects::CreateEntry()
{
	return hplNew(cEditorObjectIndexEntryStaticObject,(this));
}

//-------------------------------------------------------------------

//-------------------------------------------------------------------
//-------------------------------------------------------------------

cEditorObjectIndexStaticObjects::cEditorObjectIndexStaticObjects(iEditorBase* apEditor,
																 const tWString& asBaseFolder) : iEditorObjectIndex(apEditor,
																													asBaseFolder,
																													_W("sop"),
																													"StaticObjects",
																													"StaticObject", 
																													true)
{
	AddFileFilter(_W("*.dae"));
}

//-------------------------------------------------------------------

iEditorObjectIndexDir* cEditorObjectIndexStaticObjects::CreateDir(iEditorObjectIndexDir* apParent)
{
    return hplNew(cEditorObjectIndexDirStaticObjects,(this, (cEditorObjectIndexDirStaticObjects*)apParent));
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEditorWindowStaticObjects::cEditorWindowStaticObjects(cEditorEditModeStaticObjects* apEditMode,
													   const tWStringVec& avBaseDirs) : cEditorWindowObjectBrowser(apEditMode,avBaseDirs, true)
{
	mbScaleUpdated = true;
	mbRotateUpdated = true;
}

cEditorWindowStaticObjects::~cEditorWindowStaticObjects()
{
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------



//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEditorWindowStaticObjects::OnInitLayout()
{
	cEditorWindowObjectBrowser::OnInitLayout();
	mpBGFrame->SetSize(mpBGFrame->GetSize()+cVector2f(0,200));
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

void cEditorWindowStaticObjects::ResetInputs()
{
}


//-------------------------------------------------------------------

iEditorObjectIndex* cEditorWindowStaticObjects::CreateSpecificIndex(iEditorBase* apEditor, const tWString& asFolder)
{
	cEditorObjectIndexStaticObjects* pIndex = hplNew( cEditorObjectIndexStaticObjects,(apEditor, asFolder));

	return pIndex;
}

//-------------------------------------------------------------------

cMeshEntity* cEditorWindowStaticObjects::CreatePreviewEntity(iEditorObjectIndexEntryMeshObject* apEntry)
{
	if(apEntry==NULL)
		return NULL;

	cMeshEntity* pEnt = NULL;
	cMesh* pMesh = mpEditor->GetEngine()->GetResources()->GetMeshManager()->CreateMesh(cString::To8Char(apEntry->GetFileNameRelPath()));
	if(pMesh)
		pEnt = mpEditor->GetEditorWorld()->GetWorld()->CreateMeshEntity("PreviewEntity", pMesh);

	return pEnt;
}

//-------------------------------------------------------------------


