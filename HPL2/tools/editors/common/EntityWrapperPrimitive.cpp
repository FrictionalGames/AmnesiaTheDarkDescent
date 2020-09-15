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

#include "EntityWrapperPrimitive.h"

#include "EditorHelper.h"
#include "EditorWorld.h"
#include "EditorWindowEntityEditBoxPrimitive.h"

//#include "EntityWrapperPrimitivePlane.h"

#include "EngineEntity.h"

//------------------------------------------------------------------------

iEntityWrapperTypePrimitive::iEntityWrapperTypePrimitive(const tString& asElementString, int alSubType): iEntityWrapperType(eEditorEntityType_Primitive, _W("Primitive"), asElementString)
{
	mlSubType = alSubType;

	AddString(ePrimitiveStr_Material, "Material", "", ePropCopyStep_PreEnt);
	AddBool(ePrimitiveBool_CastShadows, "CastShadows", false);
	AddBool(ePrimitiveBool_Collides, "Collides");
}

//------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

iEntityWrapperPrimitive::iEntityWrapperPrimitive(iEntityWrapperData* apData) : iEntityWrapper(apData)
{
	mbCastShadows = false;
	mbCollides = true;
}

//------------------------------------------------------------------------

iEntityWrapperPrimitive::~iEntityWrapperPrimitive()
{
}

//------------------------------------------------------------------------

bool iEntityWrapperPrimitive::GetProperty(int alPropID, bool& abX)
{
	if(iEntityWrapper::GetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case ePrimitiveBool_CastShadows:
		abX = GetCastShadows();
		break;
	case ePrimitiveBool_Collides:
		abX = GetCollides();
		break;
	default:
		return false;
	}

	return true;
}


bool iEntityWrapperPrimitive::GetProperty(int alPropID, tString& asX)
{
	if(iEntityWrapper::GetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case ePrimitiveStr_Material:
		asX = GetMaterial();
		break;
	default:
		return false;
	}

	return true;
}

bool iEntityWrapperPrimitive::SetProperty(int alPropID, const bool& abX)
{
	if(iEntityWrapper::SetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case ePrimitiveBool_CastShadows:
		SetCastShadows(abX);
		break;
	case ePrimitiveBool_Collides:
		SetCollides(abX);
		break;
	default:
		return false;
	}

	return true;
}


bool iEntityWrapperPrimitive::SetProperty(int alPropID, const tString& asX)
{
	if(iEntityWrapper::SetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case ePrimitiveStr_Material:
		SetMaterial(asX);
		break;
	default:
		return false;
	}

	return true;
}

//------------------------------------------------------------------------

cEditorWindowEntityEditBox* iEntityWrapperPrimitive::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = hplNew(cEditorWindowEntityEditBoxPrimitive,(apEditMode,this));

	return pEditBox;
}

//------------------------------------------------------------------------

void iEntityWrapperPrimitive::SetMaterial(const tString& asMaterial)
{
	cMaterial* pMat = NULL;
	if(cEditorHelper::LoadResourceFile(eEditorResourceType_Material, asMaterial, NULL))
	{
		msMaterial = asMaterial;

		iEngineEntityMesh* pMesh = (iEngineEntityMesh*)mpEngineEntity;
		if(pMesh) pMesh->SetMaterial(asMaterial);
	}
	else
		msMaterial = "";
}

//------------------------------------------------------------------------

void iEntityWrapperPrimitive::SetCastShadows(bool abX)
{
	mbCastShadows = abX;
	/*
	if(mpEngineEntity)
	{
		((cMeshEntity*)mpEngineEntity)->SetRenderFlagBit(eRenderableFlag_ShadowCaster, abX); 
	}*/
}

//------------------------------------------------------------------------

void iEntityWrapperPrimitive::SetCollides(bool abX)
{
	mbCollides = abX;
}

//------------------------------------------------------------------------

bool iEntityWrapperPrimitive::IsAffectedByDecal(bool abAffectsStaticObject, bool abAffectsPrimitive, bool abAffectsEntity)
{
	return abAffectsPrimitive && IsVisible() && IsCulledByClipPlanes()==false;
}

//------------------------------------------------------------------------

iEngineEntity* iEntityWrapperPrimitive::CreateSpecificEngineEntity()
{
	return hplNew(cEngineEntityGeneratedMesh,(this, CreatePrimitiveMesh()));
}

/*
void iEntityWrapperPrimitive::SaveToElement(cXmlElement* apElement)
{
	iEntityWrapper::SaveToElement(apElement);

	apElement->SetAttributeString("Material", mpEditorWorld->GetEditor()->GetFilePathRelativeToWorkingDir(msMaterial));
	apElement->SetAttributeBool("CastShadows", mbCastShadows);
	apElement->SetAttributeInt("SubType", mPType);
	apElement->SetAttributeBool("Collides", mbCollides);
}
*/
//------------------------------------------------------------------------

