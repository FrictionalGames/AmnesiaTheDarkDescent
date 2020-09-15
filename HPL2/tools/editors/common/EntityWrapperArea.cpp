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

#include "EntityWrapperArea.h"
#include "EditorEditModeAreas.h"

#include "EditorWorld.h"
#include "EditorBaseClasses.h"
#include "EditorHelper.h"

#include "EditorSelection.h"
#include "EntityIcon.h"

#include "EditorWindowViewport.h"

#include "EditorWindowEntityEditBoxArea.h"

//------------------------------------------------------------------------------

cEntityWrapperTypeArea::cEntityWrapperTypeArea(cEditorUserClassSubType* apType) : iEntityWrapperTypeUserDefinedEntity(eEditorEntityType_Area, 
																													  _W("Area"), "Area", 
																													  apType)
{
	AddString(eAreaStr_Mesh, "Mesh", "");
	AddString(eAreaStr_Type, "AreaType", GetAreaType());

	////////////////////////////////////////
	// Set up defaults
	mColor = cColor(1,0,1,1);
	mbShowOrientation = false;
	mbDrawAsSphere = false;
	mvDefaultSize = 1;
	mbCanRotate = true;
	mScaleType = eScaleType_Normal;

	msIcon = "Area";

	/////////////////////////////////////////////////////////////////////
	// Retrieve type characteristics
	tEditorVarVec vEditorSetupVars = apType->GetEditorSetupVars();
	tString sColor, sShowOrientation, sSphere, sDefaultSize, sIcon, sAllowRotate, sAllowScale;

	for(int i=0;i<(int)vEditorSetupVars.size();++i)
	{
		iEditorVar* pVar = vEditorSetupVars[i];
		const tWString& sName = pVar->GetName();
		tString sValue = cString::To8Char(pVar->GetDefaultValue());;

		if(sName==_W("Color"))
			mColor = cString::ToColor(sValue.c_str(), mColor);

		else if(sName==_W("ShowOrientation"))
			mbShowOrientation = cString::ToBool(sValue.c_str(), mbShowOrientation);

		else if(sName==_W("Sphere"))
			mbDrawAsSphere = cString::ToBool(sValue.c_str(), mbDrawAsSphere);

		else if(sName==_W("DefaultSize"))
			mvDefaultSize = cString::ToVector3f(sValue.c_str(), mvDefaultSize);

		else if(sName==_W("AllowRotate"))
			mbCanRotate = cString::ToBool(sValue.c_str(), mbCanRotate);

		else if(sName==_W("AllowScale"))
		{
			bool bAllowScale = cString::ToBool(sValue.c_str(), true);
			mScaleType = bAllowScale? mScaleType : eScaleType_None;
		}

		else if(sName==_W("Icon"))
			msIcon = sValue==""? msIcon : sValue;
	}
}

//------------------------------------------------------------------------------

tString cEntityWrapperTypeArea::ToString()
{
	return iEntityWrapperType::ToString() + " " + GetAreaType();
}

//------------------------------------------------------------------------------

bool cEntityWrapperTypeArea::IsAppropriateType(cXmlElement* apElement)
{
	if(iEntityWrapperType::IsAppropriateType(apElement)==false)
		return false;

	return apElement->GetAttributeString("AreaType")==GetAreaType();
}

//------------------------------------------------------------------------------

const tString& cEntityWrapperTypeArea::GetAreaType()
{
	// Name is contained in parent type
	return mpUserType->GetParent()->GetName();
}

iEntityWrapperData* cEntityWrapperTypeArea::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataArea,(this));
}

//------------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataArea::CreateSpecificEntity()
{
	return hplNew( cEntityWrapperArea,(this));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

cEntityWrapperDataArea::cEntityWrapperDataArea(iEntityWrapperType* apType) : iEntityWrapperDataUserDefinedEntity(apType, eUserClassDefinition_Area)
{
	cEntityWrapperTypeArea* pType = (cEntityWrapperTypeArea*)apType;
	SetName(pType->GetAreaType()+"Area");
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEntityWrapperArea::cEntityWrapperArea(iEntityWrapperData* apData) : iEntityWrapperUserDefinedEntity(apData)
{
	mpMesh = NULL;

	mbTypeChanged = false;
}

//------------------------------------------------------------------------------

cEntityWrapperArea::~cEntityWrapperArea()
{
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

bool cEntityWrapperArea::SetProperty(int alPropID, const tString& asX)
{
	if(iEntityWrapper::SetProperty(alPropID, asX))
		return true;

	switch(alPropID)
	{
	case eAreaStr_Mesh:
		SetMeshFile(asX);
		break;
	case eAreaStr_Type:
		break;
	default:
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------

bool cEntityWrapperArea::GetProperty(int alPropID, tString& asX)
{
	if(iEntityWrapper::GetProperty(alPropID, asX))
		return true;

	switch(alPropID)
	{
	case eAreaStr_Mesh:
		asX = GetMeshFile();
		break;
	case eAreaStr_Type:
		break;
	default:
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------

void cEntityWrapperArea::UpdateEntity()
{
	iEntityWrapperUserDefinedEntity::UpdateEntity();
	
	if(mpMesh)
	{
		cMatrixf mtxRT = cMath::MatrixMul(mmtxTranslate,
											mmtxRotate);
		mpMesh->SetMatrix(mtxRT);
	}
}

//------------------------------------------------------------------------------

const tString& cEntityWrapperArea::GetAreaType()
{
	return ((cEntityWrapperTypeArea*)mpType)->GetAreaType();
}

//------------------------------------------------------------------------------

void cEntityWrapperArea::SetAreaType(const tString& asX)
{
	cEditorEditModeAreas* pEditMode = (cEditorEditModeAreas*)GetEditorWorld()->GetEditor()->GetEditMode("Areas");
	
	for(int i=0;i<pEditMode->GetTypeNum(); ++i)
	{
		cEntityWrapperTypeArea* pType = (cEntityWrapperTypeArea*)pEditMode->GetType(i);
		if(pType->GetAreaType()==asX)
		{
			mpType = pType;
			iEntityWrapperData* pData = mpType->CreateData();
			pData->CopyToEntity(this, ePropCopyStep_PostEnt);
			hplDelete(pData);

			mbTypeChanged = true;

			break;
		}
	}
}

//------------------------------------------------------------------------------

void cEntityWrapperArea::SetMeshFile(const tString& asX)
{
	////////////////////////////////////////////
	// Try to load a mesh for the area
	cResources* pRes = GetEditorWorld()->GetEditor()->GetEngine()->GetResources();
	cMesh* pMesh = NULL;
	if(asX!="")
		pMesh = pRes->GetMeshManager()->CreateMesh(asX);

	if(pMesh==NULL && asX!="")
		return;

	////////////////////////////////////////////
	// Destroy previous entity
	msMeshFile = asX;

	cWorld* pWorld = GetEditorWorld()->GetWorld();
	if(mpMesh)
	{
		pWorld->DestroyMeshEntity(mpMesh);
		mpMesh = NULL;
	}

	if(pMesh==NULL)
		return;

	////////////////////////////////////////////
	// Create a new one using the new mesh and 
	// set it up with a custom material
	cMaterialManager* pMatMgr = pRes->GetMaterialManager();

	mpMesh = pWorld->CreateMeshEntity(msName, pMesh, false);
	mpMesh->SetVisible(false);
	for(int i=0;i<mpMesh->GetSubMeshEntityNum();++i)
	{
		cSubMeshEntity* pSubMeshEnt = mpMesh->GetSubMeshEntity(i);
		pSubMeshEnt->SetCustomMaterial(pMatMgr->CreateMaterial("editor_area_mesh.mat"));
	}

	mbEntityUpdated=true;
	UpdateEntity();
}

//------------------------------------------------------------------------------

void cEntityWrapperArea::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode,
								bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol)
{
	iEntityWrapper::Draw(apViewport, apFunctions, apEditMode, abIsSelected, aHighlightCol, cColor(1,0,0,1));
	cEntityWrapperTypeArea* pType = (cEntityWrapperTypeArea*)mpType;
	
	cColor col;
	if(abIsSelected)
		col = aHighlightCol;
	else
		col = pType->GetColor();

	cMatrixf mtxRT = cMath::MatrixMul(cMath::MatrixTranslate(mvPosition), cMath::MatrixRotate(mvRotation, eEulerRotationOrder_XYZ));

	apFunctions->SetMatrix(&mtxRT);

	apFunctions->GetLowLevelGfx()->DrawLine(cVector3f(-0.05f,0,0), cVector3f(0.05f,0,0), col);
	apFunctions->GetLowLevelGfx()->DrawLine(cVector3f(0,-0.05f,0), cVector3f(0,0.05f,0), col);
	apFunctions->GetLowLevelGfx()->DrawLine(cVector3f(0,0,-0.05f), cVector3f(0,0,0.05f), col);

	cVector3f vHalfScale;
	if(pType->IsScalable())
		vHalfScale = mvScale*0.5f;
	else
		vHalfScale = pType->GetDefaultSize()*0.5f;


	if(pType->GetDrawAsSphere())
	{
		vHalfScale = vHalfScale*0.5f;
		apFunctions->GetLowLevelGfx()->DrawSphere(0, vHalfScale.x, col);
	}
	else
	{
		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vHalfScale*-1, vHalfScale, col);
	}

	if(pType->GetShowOrientation())
	{
		cVector3f vLength = vHalfScale-0.1f;
		for(int i=0;i<3;++i)
		{
			cVector3f vDir = 0;
			cColor col = 0;
			vDir.v[i] = 1;
			vDir = vDir*vLength;
			col.v[i] = 1;
			apFunctions->GetLowLevelGfx()->DrawLine(0 , vDir, col);
			cEditorHelper::DrawPyramid(apFunctions, vDir, vDir+vDir*0.1f, 0.025f, col);
		}
	}

	if(mpMesh)
		mpMesh->SetVisible(mbSelected);

	apFunctions->SetMatrix(NULL);
}

//------------------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperArea::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = hplNew(cEditorWindowEntityEditBoxArea,(apEditMode,this));

	return pEditBox;
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEntityIcon* cEntityWrapperArea::CreateIcon()
{
	cEntityWrapperTypeArea* pType = (cEntityWrapperTypeArea*)mpType;
	return hplNew( cEntityIcon, (this, pType->GetIcon()));

	return NULL;
}

//------------------------------------------------------------------------------
