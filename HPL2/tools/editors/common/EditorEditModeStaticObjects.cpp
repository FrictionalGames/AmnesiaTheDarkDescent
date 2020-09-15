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

#include "EditorEditModeStaticObjects.h"

#include "EditorWorld.h"

#include "EditorBaseClasses.h"
#include "EditorActionHandler.h"

#include "EditorWindowFactory.h"
#include "EditorWindowStaticObjects.h"

#include "EditorActionEntity.h"

#include "EntityWrapperStaticObject.h"

#include "../leveleditor/LevelEditor.h"


//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEditorEditModeStaticObjects::cEditorEditModeStaticObjects(iEditorBase* apEditor,
														   iEditorWorld* apEditorWorld) : iEditorEditModeObjectCreator(apEditor,
																													   "Static Objects",
																													   apEditorWorld)
{
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

void cEditorEditModeStaticObjects::DrawObjectPreview(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cMatrixf& amtxTransform, bool abPreCreationActive)
{
	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	apFunctions->GetLowLevelGfx()->DrawSphere(mpEditor->GetPosOnGridFromMousePos(),0.1f,cColor(1,0,0,1));

	cEditorWindowStaticObjects* pWin = (cEditorWindowStaticObjects*)mpWindow;
	iEditorObjectIndexEntryMeshObject* pObj = pWin->GetSelectedObject();

	if(pObj)
	{
		const cVector3f& vBVMin = pObj->GetBVMin();
		const cVector3f& vBVMax = pObj->GetBVMax();

		apFunctions->SetMatrix((cMatrixf*)&amtxTransform);
		cMeshEntity* pEnt = pWin->GetPreviewEntity();
		if(pEnt)
			pEnt->SetVisible(abPreCreationActive);

		if(abPreCreationActive)
		{
			apFunctions->SetDepthTestFunc(eDepthTestFunc_Greater);
			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vBVMin,vBVMax, cColor(1,0,0,0.6f));
			apFunctions->SetDepthTestFunc(eDepthTestFunc_Less);
			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vBVMin,vBVMax, cColor(0,1,0,0.6f));
			
			/////////////////////////////////////////
			// Draw Textured Mesh
			if(pEnt)
			{
				pEnt->SetMatrix(amtxTransform);
			}
		}
		else
		{
			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vBVMin,vBVMax, cColor(1,0.5f));
		}
	}

	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetMatrix(NULL);	
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

iEditorWindow* cEditorEditModeStaticObjects::CreateSpecificWindow()
{
	return hplNew(cEditorWindowStaticObjects,(this, mpEditor->GetLookUpDirs(eDir_StaticObjects)));
}

//-----------------------------------------------------------------

bool cEditorEditModeStaticObjects::SetUpCreationData(iEntityWrapperData* apData)
{
	iEditorEditModeObjectCreator::SetUpCreationData(apData);
	cEntityWrapperDataStaticObject* pData = (cEntityWrapperDataStaticObject*)apData;
	cEditorWindowStaticObjects* pWin = (cEditorWindowStaticObjects*)mpWindow;
	
	iEditorObjectIndexEntry* pEntry = pWin->GetSelectedObject();
	if(pEntry==NULL) return false;

	///////////////////////////////////////
	// Build name
	tWString sRelPath = pEntry->GetDir()->GetRelPath();
	if(sRelPath.empty()==false)
		sRelPath = cString::AddSlashAtEndW(sRelPath, _W('_'));
	tString sName = cString::To8Char(sRelPath) + pEntry->GetEntryName();
	pData->SetName(sName);

	///////////////////////////////////////
	// Set up filename
	tString sFilename = cString::To8Char(pEntry->GetFileNameFullPath());
	pData->SetString(eStaticObjectStr_Filename, sFilename);

	///////////////////////////////////////
	// Set up file index
	int lFileIndex = mpEditorWorld->AddFilenameToIndex("StaticObjects", sFilename);
	pData->SetInt(eStaticObjectInt_FileIndex, lFileIndex);

	return true;
}

//-----------------------------------------------------------------

void cEditorEditModeStaticObjects::OnEditorUpdate(float afTimeStep)
{
}

//-----------------------------------------------------------------

void cEditorEditModeStaticObjects::CreateTypes()
{
	mvTypes.push_back(hplNew(cEntityWrapperTypeStaticObject,()));
}
//-----------------------------------------------------------------

