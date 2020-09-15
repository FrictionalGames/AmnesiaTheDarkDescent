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

#include "EditorEditModeEntities.h"

#include "EditorBaseClasses.h"
#include "EditorActionHandler.h"

#include "EditorWindowFactory.h"
#include "EditorWindowEntities.h"

#include "EntityWrapperEntity.h"
#include "EditorWorld.h"

#include "../leveleditor/LevelEditor.h"

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEditorEditModeEntities::cEditorEditModeEntities(iEditorBase* apEditor,
												 iEditorWorld* apEditorWorld) : iEditorEditModeObjectCreator(apEditor, "Entities", apEditorWorld)
{
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

//-----------------------------------------------------------------

void cEditorEditModeEntities::DrawObjectPreview(cEditorWindowViewport* apViewport, cRendererCallbackFunctions *apFunctions, const cMatrixf& amtxTransform, bool abPreCreationActive)
{
	cEditorWindowEntities* pWin = (cEditorWindowEntities*)mpWindow;

	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	apFunctions->GetLowLevelGfx()->DrawSphere(mpEditor->GetPosOnGridFromMousePos(),0.1f,cColor(1,0,0,1));

	iEditorObjectIndexEntryMeshObject* pObj = pWin->GetSelectedObject();

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

iEntityWrapperType* cEditorEditModeEntities::GetType()
{
	cEditorWindowEntities* pWin = (cEditorWindowEntities*)mpWindow;
	iEditorObjectIndexEntry* pEntry =  pWin->GetSelectedObject();

	if(pEntry==NULL) return NULL;

	tString sFilename = cString::To8Char(pEntry->GetFileNameFullPath());

	return GetTypeFromEntFile(sFilename);
}

//-----------------------------------------------------------------

iEntityWrapperType* cEditorEditModeEntities::GetTypeFromEntFile(const tString& asFile)
{
	tString sType,sSubType;

	cResources* pRes = mpEditor->GetEngine()->GetResources();
	iXmlDocument* pModelDoc = pRes->LoadXmlDocument(asFile);
	if(pModelDoc==NULL)
		return NULL;

	cXmlElement* pUserVars = pModelDoc->GetFirstElement("UserDefinedVariables");
	if(pUserVars)
	{
		sType = pUserVars->GetAttributeString("EntityType");
		sSubType = pUserVars->GetAttributeString("EntitySubType");
	}
	pRes->DestroyXmlDocument(pModelDoc);

	if(sType=="")
	{
		cEntityWrapperTypeEntity* pType = (cEntityWrapperTypeEntity*)iEditorEditModeObjectCreator::GetType(0);

		Log("Using type %s for typeless entity file %s\n", pType->ToString().c_str(), asFile.c_str());
		return pType;
		
	}

	for(int i=0;i<GetTypeNum();++i)
	{
		cEntityWrapperTypeEntity* pType = (cEntityWrapperTypeEntity*)iEditorEditModeObjectCreator::GetType(i);
		cEditorUserClassSubType *pUserType = pType->GetUserType();
		cEditorUserClassType* pUserTypeParent = pUserType->GetParent();
		if(pUserTypeParent->GetName()==sType)
		{
			if(pUserType->GetName()==sSubType)
				return pType;
			else
			{
				if(pUserTypeParent->GetSubTypeNum()==1)
				{
					Log("Using type %s for badly saved file %s\n", pType->ToString().c_str(), asFile.c_str());
					return pType;
				}
			}
		}
	}

	Error("EditModeEntities : Could not find type %s %s, either %s is badly saved, or was saved using a different EntityTypes.cfg file.", sType.c_str(), sSubType.c_str(), asFile.c_str());
	cEntityWrapperTypeEntity* pType = (cEntityWrapperTypeEntity*)iEditorEditModeObjectCreator::GetType(0);

	Log("Using type %s instead\n", pType->ToString().c_str(), asFile.c_str());
	return pType;

	//return NULL;
}

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

iEditorWindow* cEditorEditModeEntities::CreateSpecificWindow()
{
	return hplNew(cEditorWindowEntities,(this, mpEditor->GetLookUpDirs(eDir_Entities)));
}

//-----------------------------------------------------------------

bool cEditorEditModeEntities::SetUpCreationData(iEntityWrapperData* apData)
{
	iEditorEditModeObjectCreator::SetUpCreationData(apData);

	cEditorWindowEntities* pWin = (cEditorWindowEntities*)mpWindow;
	iEditorObjectIndexEntry* pEntry =  pWin->GetSelectedObject();
	if(pEntry==NULL) return false;

	///////////////////////////////////////
	// Build name
	apData->SetName(pEntry->GetEntryName());

	///////////////////////////////////////
	// Set up filename
	tString sFilename = cString::To8Char(pEntry->GetFileNameFullPath());
	apData->SetString(eEntityStr_Filename, sFilename);

	///////////////////////////////////////
	// Set up file index
	int lFileIndex = mpEditorWorld->AddFilenameToIndex("Entities", sFilename);
	apData->SetInt(eEntityInt_FileIndex, lFileIndex);

	///////////////////////////////////////
	// Specific transforms
	if(mbCreateOnSurface==false)
		apData->SetVec3f(eObjVec3f_Rotation, pWin->GetRotate());

	return true;
}

//-----------------------------------------------------------------

void cEditorEditModeEntities::CreateTypes()
{
	cEditorUserClassDefinition* pDef = mpEditor->GetClassDefinitionManager()->GetDefinition(eUserClassDefinition_Entity);
	for(int i=0;i<pDef->GetTypeNum();++i)
	{
		cEditorUserClassType* pType = pDef->GetType(i);
		for(int j=0;j<pType->GetSubTypeNum();++j)
		{
			cEditorUserClassSubType* pSubType = pType->GetSubType(j);

			mvTypes.push_back(hplNew(cEntityWrapperTypeEntity, (pSubType)));
		}
	}
}

//-----------------------------------------------------------------
