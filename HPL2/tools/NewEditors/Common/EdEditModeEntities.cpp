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

#include "../Common/EdEditModeEntities.h"

#include "../Common/Editor.h"
#include "../Common/EdActionHandler.h"

#include "../Common/EdWindowEntities.h"

#include "../Common/EdObjEntity.h"
#include "../Common/EdWorld.h"

#include "../LevelEditor/LevelEditor.h"

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEdEditModeEntities::cEdEditModeEntities(iEditor* apEditor) : iEdEditModeScnObjCreator(apEditor, _W("Entities"))
{
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

//-----------------------------------------------------------------

void cEdEditModeEntities::DrawObjectPreview(iEdViewport* apViewport, cRendererCallbackFunctions *apFunctions, const cMatrixf& amtxTransform, bool abPreCreationActive)
{
	cEdWindowEntities* pWin = (cEdWindowEntities*)mpWindow;

	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	//apFunctions->GetLowLevelGfx()->DrawSphere(mpEditor->GetPosOnGridFromMousePos(),0.1f,cColor(1,0,0,1));

	iEdObjectIndexEntryMeshObject* pObj = pWin->GetSelectedObject();

	if(pObj)
	{
		const cVector3f& vBVMin = pObj->GetBVMin();
		const cVector3f& vBVMax = pObj->GetBVMax();		

		apFunctions->SetMatrix((cMatrixf*)&amtxTransform);
		cMeshEntity* pEnt = pWin->GetPreviewEntity();
		if(pEnt)
		{
			pEnt->SetMatrix(amtxTransform);
			pEnt->SetVisible(abPreCreationActive);

		}

		if(abPreCreationActive)
		{
			apFunctions->SetDepthTestFunc(eDepthTestFunc_Greater);
			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vBVMin,vBVMax, cColor(1,0,0,0.6f));
			apFunctions->SetDepthTestFunc(eDepthTestFunc_Less);
			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vBVMin,vBVMax, cColor(0,1,0,0.6f));
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

iEdObjectType* cEdEditModeEntities::GetSelectedType()
{
	cEdWindowEntities* pWin = static_cast<cEdWindowEntities*>(mpWindow);
	iEdObjectIndexEntry* pEntry =  pWin->GetSelectedObject();
	if(pEntry==NULL) return NULL;

	tString sFilename = cString::To8Char(pEntry->GetFileNameFullPath());

	return static_cast<cTypeRootEntity*>(mpType)->GetTypeFromEntFile(sFilename);
}

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

iEdWindow* cEdEditModeEntities::CreateWindow()
{
	return hplNew(cEdWindowEntities,(this, mpEditor->GetLookUpDirs(eDir_Entities)));
}

//-----------------------------------------------------------------

/*
bool cEdEditModeEntities::SetUpCreationData(iEdObjectData* apData)
{
	if(iEdEditModeScnObjCreator::SetUpCreationData(apData)==false)
		return false;

	cEdWindowEntities* pWin = (cEdWindowEntities*)mpWindow;
	iEdObjectIndexEntry* pEntry =  pWin->GetSelectedObject();
	if(pEntry==NULL) return false;

	///////////////////////////////////////
	// Build name
	//apData->SetName(pEntry->GetEntryName());

	///////////////////////////////////////
	// Set up filename
	tString sFilename = cString::To8Char(pEntry->GetFileNameFullPath());
	//apData->SetString(eEntityStr_Filename, sFilename);

	///////////////////////////////////////
	// Set up file index
	//int lFileIndex = GetWorld()->AddFilenameToIndex("Entities", sFilename);
	//apData->SetInt(eEntityInt_FileIndex, lFileIndex);

	///////////////////////////////////////
	// Specific transforms
	//if(mbCreateOnSurface==false)
	//	apData->SetVec3f(eScnVec3f_Rotation, pWin->GetRotate());

	return true;
}
*/

//-----------------------------------------------------------------

iEdObjectType* cEdEditModeEntities::CreateType()
{
	return hplNew(cTypeRootEntity, ());
}

//-----------------------------------------------------------------
