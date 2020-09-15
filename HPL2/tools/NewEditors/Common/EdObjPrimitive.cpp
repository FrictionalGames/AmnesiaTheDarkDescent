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

#include "../Common/EdObjPrimitive.h"

#include "../Common/EdResource.h"
#include "../Common/EdScnWorld.h"
#include "../Common/EdSurfacePicker.h"
#include "../Common/EdEditPanePrimitive.h"

#include "../Common/EdObjPrimitivePlane.h"

#include "../Common/EdEngineObject.h"
#include "../Common/EdWindowPrimitives.h"

//---------------------------------------------------------------------------

iPrimitiveMeshCreator::iPrimitiveMeshCreator()
{
}

//---------------------------------------------------------------------------

iEdObjectData* iPrimitiveMeshCreator::CreateData()
{
	return NULL;
	//return mpType->CreateData();
}

//---------------------------------------------------------------------------

void iPrimitiveMeshCreator::SetUpData(iEdWindow* apWindow, iEdObjectData* apData)
{
	cEdWindowPrimitives* pWindow = static_cast<cEdWindowPrimitives*>(apWindow);
	
	apData->SetString(ePrimitiveStr_Material, pWindow->GetMaterial());
	apData->SetBool(ePrimitiveBool_CastShadows, pWindow->GetCastShadows());
	apData->SetBool(ePrimitiveBool_Collides, pWindow->GetCollides());
}

//---------------------------------------------------------------------------


cTypePrimitive::cTypePrimitive(): iEdScnObjType(_W("Primitive"), "")
{
	AddString(ePrimitiveStr_Material, "Material", "", ePropStep_PreCreate);
	AddBool(ePrimitiveBool_CastShadows, "CastShadows", false);
	AddBool(ePrimitiveBool_Collides, "Collides");
}

//------------------------------------------------------------------------

bool cTypePrimitive::Init()
{
	bool bInit = iEdScnObjType::Init();
	bInit = bInit && AddChildType(hplNew(cTypePlane,()));

	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(GetWorld());
	cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();
	pPicker->RegisterSurfaceType(this);

	return bInit;
}

//------------------------------------------------------------------------

iPrimitiveSubType::iPrimitiveSubType(const tWString& asName, const tString& asXmlElement, iPrimitiveMeshCreator* apCreator) : iEdScnObjType(asName, asXmlElement)
{
	mpCreator = apCreator;
}

//------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

iEdScnObjPrimitive::iEdScnObjPrimitive(iEdObjectData* apData) : iEdScnObject(apData)
{
	mbCastShadows = false;
	mbCollides = true;
}

//------------------------------------------------------------------------

iEdScnObjPrimitive::~iEdScnObjPrimitive()
{
}

//------------------------------------------------------------------------

bool iEdScnObjPrimitive::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case ePrimitiveBool_CastShadows:
		abX = GetCastShadows();
		break;
	case ePrimitiveBool_Collides:
		abX = GetCollides();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, abX);
	}

	return true;
}


bool iEdScnObjPrimitive::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case ePrimitiveStr_Material:
		asX = GetMaterial();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, asX);
	}

	return true;
}

bool iEdScnObjPrimitive::SetProperty(int alPropID, const bool abX)
{
	switch(alPropID)
	{
	case ePrimitiveBool_CastShadows:
		SetCastShadows(abX);
		break;
	case ePrimitiveBool_Collides:
		SetCollides(abX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, abX);
	}

	return true;
}


bool iEdScnObjPrimitive::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case ePrimitiveStr_Material:
		SetMaterial(asX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, asX);
	}

	return true;
}

//------------------------------------------------------------------------

iEdEditPane* iEdScnObjPrimitive::CreateEditPane()
{
	return hplNew(cEdEditPanePrimitive,(this));
}

//------------------------------------------------------------------------

void iEdScnObjPrimitive::SetMaterial(const tString& asMaterial)
{
	if(cEdLoader::LoadMaterial(asMaterial, NULL))
	{
		msMaterial = asMaterial;

		iEngineMesh* pMesh = static_cast<iEngineMesh*>(mpEngObject);
		if(pMesh) pMesh->SetMaterial(asMaterial);
	}
	else
		msMaterial.clear();

	/*if(cEdHelper::LoadResourceFile(eEdResourceType_Material, asMaterial, NULL))
	{
		msMaterial = asMaterial;

		iEngineMesh* pMesh = (iEngineMesh*)mpEngObject;
		if(pMesh) pMesh->SetMaterial(asMaterial);
	}
	else
		msMaterial = "";
		*/
}

//------------------------------------------------------------------------

void iEdScnObjPrimitive::SetCastShadows(bool abX)
{
	mbCastShadows = abX;
	/*
	if(mpEngineEntity)
	{
		((cMeshEntity*)mpEngineEntity)->SetRenderFlagBit(eRenderableFlag_ShadowCaster, abX); 
	}*/
}

//------------------------------------------------------------------------

void iEdScnObjPrimitive::SetCollides(bool abX)
{
	mbCollides = abX;
}

//------------------------------------------------------------------------

bool iEdScnObjPrimitive::IsAffectedByDecal(bool abAffectsStaticObject, bool abAffectsPrimitive, bool abAffectsEntity)
{
	return abAffectsPrimitive && IsVisible() && IsCulledByClipPlanes()==false;
}

//------------------------------------------------------------------------

iEngineObject* iEdScnObjPrimitive::CreateEngineObject()
{
	return hplNew(cEngineGeneratedMesh,(this, CreatePrimitiveMesh()));
}

/*
void iEntityWrapperPrimitive::SaveToElement(cXmlElement* apElement)
{
	iEntityWrapper::SaveToElement(apElement);

	apElement->SetAttributeString("Material", mpWorld->GetEditor()->GetFilePathRelativeToWorkingDir(msMaterial));
	apElement->SetAttributeBool("CastShadows", mbCastShadows);
	apElement->SetAttributeInt("SubType", mPType);
	apElement->SetAttributeBool("Collides", mbCollides);
}
*/
//------------------------------------------------------------------------

