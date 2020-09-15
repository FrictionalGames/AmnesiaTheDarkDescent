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

#include "../Common/EdObjArea.h"

#include "../Common/EdWorld.h"
#include "../Common/Editor.h"
#include "../Common/EdResource.h"

#include "../Common/EdObjectIcon.h"

#include "../Common/EdEditPaneArea.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

cTypeRootArea::cTypeRootArea() : iUserTypeRoot(_W("Area"), "Area", "AreaClasses.def")
{
	RegisterVarCatName(eEdVarCategory_Editor, "EditorSetupVars");
	RegisterVarCatName(eEdVarCategory_Instance, "Vars");

	AddBool(eScnBool_Active, "Active");
	AddString(eAreaStr_Mesh, "Mesh", "");
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

iEdObjectType* cTypeRootArea::GetTypeByXmlElement(cXmlElement* apElem)
{
	if(iUserTypeRoot::GetTypeByXmlElement(apElem)==NULL)
		return NULL;

	tString sAreaType = apElem->GetAttributeString("AreaType", "");

	return GetTypeByName(cString::To16Char(sAreaType));
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------


iUserTypeNode* cTypeRootArea::CreateSpecificUserTypeNode(iEdClass* apClass)
{
	return hplNew(cTypeArea,(this, apClass));
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

cTypeArea::cTypeArea(iUserTypeRoot* apRoot, iEdClass* apClass) : iUserTypeNode(apRoot, apClass)
{
	mColor = cColor(1,0,1,1);
	mbShowOrientation = false;
	mbDrawAsSphere = false;
	mvDefaultSize = 1;
	mbRotateable = true;

	msIcon = "Area";


	/*////////////////////////////////////////
	// Set up defaults
	mColor = cColor(1,0,1,1);
	mbShowOrientation = false;
	mbDrawAsSphere = false;
	mvDefaultSize = 1;
	mbCanRotate = true;
	mScaleType = eScaleType_Normal;

	msIcon = "Area";
	*/

	/////////////////////////////////////////////////////////////////////
	// Retrieve type characteristics

	cEdUserClass* pClass = static_cast<cEdUserClass*>(apClass);
	cEdClassInstance* pClassInst = pClass->CreateInstance(eEdVarCategory_Editor);
	
	tString sColor, sShowOrientation, sSphere, sDefaultSize, sIcon, sAllowRotate, sAllowScale;

	for(int i=0;i<pClassInst->GetVarInstanceNum();++i)
	{
		cEdVarInstance* pVar = pClassInst->GetVarInstance(i);
		const tWString& sName = pVar->GetName();
		tString sValue = cString::To8Char(pVar->GetValue());

		if(sName==_W("Color"))
			mColor = cString::ToColor(sValue.c_str(), mColor);

		else if(sName==_W("ShowOrientation"))
			mbShowOrientation = cString::ToBool(sValue.c_str(), mbShowOrientation);

		else if(sName==_W("Sphere"))
			mbDrawAsSphere = cString::ToBool(sValue.c_str(), mbDrawAsSphere);

		else if(sName==_W("DefaultSize"))
			mvDefaultSize = cString::ToVector3f(sValue.c_str(), mvDefaultSize);

		else if(sName==_W("AllowRotate"))
			mbRotateable = cString::ToBool(sValue.c_str(), mbRotateable);

		else if(sName==_W("AllowScale"))
		{
			//bool bAllowScale = cString::ToBool(sValue.c_str(), true);
			//mScaleType = bAllowScale? mScaleType : eScaleType_None;
		}

		else if(sName==_W("Icon"))
			msIcon = sValue==""? msIcon : sValue;
	}
	hplDelete(pClassInst);
}

//------------------------------------------------------------------------------

/*
tString cEdObjTypeArea::ToString()
{
	return iEdObjectType::ToString() + " " + GetAreaType();
}
*/

//------------------------------------------------------------------------------

/*
bool cEdObjTypeArea::IsAppropriateType(cXmlElement* apElement)
{
	if(iEdObjectType::IsAppropriateType(apElement)==false)
		return false;

	return apElement->GetAttributeString("AreaType")==GetAreaType();
}
*/

//------------------------------------------------------------------------------

/*
const tString& cEdObjTypeArea::GetAreaType()
{
	// Name is contained in parent type
	return mpUserType->GetParent()->GetName();
}
*/

iEdObjectData* cTypeArea::CreateTypeSpecificData()
{
	return hplNew(cEdObjAreaData,(this));
}

//------------------------------------------------------------------------------

iEdObject* cEdObjAreaData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjArea,(this));
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

cEdObjAreaData::cEdObjAreaData(iEdObjectType* apType) : iUserObjData(apType)
{
	SetName(apType->GetName() + _W("Area"));
}

//------------------------------------------------------------------------------

bool cEdObjAreaData::SaveSpecificData(cXmlElement* apElem)
{
	if(iUserObjData::SaveSpecificData(apElem)==false)
		return false;

	apElem->SetAttributeString("AreaType", cString::To8Char(mpType->GetName()));

	return true;
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEdObjArea::cEdObjArea(iEdObjectData* apData) : iUserObject(apData)
{
	mpMesh = NULL;
}

//------------------------------------------------------------------------------

cEdObjArea::~cEdObjArea()
{
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

bool cEdObjArea::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eAreaStr_Mesh:
		SetMeshFile(asX);
		break;
	case eAreaStr_Type:
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, asX);
	}

	return true;
}

//------------------------------------------------------------------------------

bool cEdObjArea::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eAreaStr_Mesh:
		asX = GetMeshFile();
		break;
	case eAreaStr_Type:
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, asX);
	}

	return true;
}

//------------------------------------------------------------------------------

void cEdObjArea::SetMeshFile(const tString& asX)
{
	////////////////////////////////////////////
	// Try to load a mesh for the area
	cResources* pRes = GetWorld()->GetEditor()->GetEngine()->GetResources();
	cMesh* pMesh = NULL;
	cEdLoader::LoadModel(asX, &pMesh);

	if(pMesh==NULL && asX.empty()==false)
		return;

	////////////////////////////////////////////
	// Destroy previous entity
	msMeshFile = asX;

	cWorld* pWorld = GetWorld()->GetEngWorld();
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

	mpMesh = pWorld->CreateMeshEntity(cString::To8Char(msName), pMesh, false);
	mpMesh->SetVisible(false);
	for(int i=0;i<mpMesh->GetSubMeshEntityNum();++i)
	{
		cSubMeshEntity* pSubMeshEnt = mpMesh->GetSubMeshEntity(i);
		pSubMeshEnt->SetCustomMaterial(pMatMgr->CreateMaterial("editor_area_mesh.mat"));
	}
}

//------------------------------------------------------------------------------

iEdEditPane* cEdObjArea::CreateEditPane()
{
	return hplNew(cEdEditPaneArea,(this));
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

void cEdObjArea::OnDraw(const cModuleDrawData& aData)
{
	iEdScnObject::OnDraw(aData);

	cTypeArea* pType = static_cast<cTypeArea*>(mpType);
	
	cColor col;
	if(mbSelected)
		col = cColor(1);
	else
		col = pType->GetColor();

	cMatrixf mtxRT = cMath::MatrixMul(GetTranslationMatrix(), GetRotationMatrix());

	aData.mpFunctions->SetDepthTest(true);
	aData.mpFunctions->SetDepthWrite(false);
	aData.mpFunctions->SetMatrix(&mtxRT);

	aData.mpFunctions->GetLowLevelGfx()->DrawLine(cVector3f(-0.05f,0,0), cVector3f(0.05f,0,0), col);
	aData.mpFunctions->GetLowLevelGfx()->DrawLine(cVector3f(0,-0.05f,0), cVector3f(0,0.05f,0), col);
	aData.mpFunctions->GetLowLevelGfx()->DrawLine(cVector3f(0,0,-0.05f), cVector3f(0,0,0.05f), col);

	cVector3f vHalfScale;
	if(pType->IsScalable())
		vHalfScale = mvScale*0.5f;
	else
		vHalfScale = pType->GetDefaultSize()*0.5f;


	if(pType->GetDrawAsSphere())
	{
		vHalfScale = vHalfScale*0.5f;
		aData.mpFunctions->GetLowLevelGfx()->DrawSphere(0, vHalfScale.x, col);
	}
	else
	{
		aData.mpFunctions->GetLowLevelGfx()->DrawBoxMinMax(vHalfScale*-1, vHalfScale, col);
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
			aData.mpFunctions->GetLowLevelGfx()->DrawLine(0 , vDir, col);
			//cEdHelper::DrawPyramid(apFunctions, vDir, vDir+vDir*0.1f, 0.025f, col);
		}
	}

	if(mpMesh)
		mpMesh->SetVisible(mbSelected);

	aData.mpFunctions->SetMatrix(NULL);

}

//------------------------------------------------------------------------------

bool cEdObjArea::OnUpdate(bool abForce)
{
	if(mpMesh)
	{
		cMatrixf mtxRT = cMath::MatrixMul(GetTranslationMatrix(), GetRotationMatrix());
		mpMesh->SetMatrix(mtxRT);
	}

	return true;
}

//------------------------------------------------------------------------------

cObjectIcon* cEdObjArea::CreateIcon()
{
	cTypeArea* pType = static_cast<cTypeArea*>(mpType);
	return hplNew( cObjectIcon, (this, pType->GetIcon()));
}

//------------------------------------------------------------------------------
