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

#include "../Common/EdEditPanePrimitive.h"

#include "../Common/EdObjPrimitive.h"
#include "../Common/EdObjPrimitivePlane.h"

#include "../Common/EdResource.h"

//#include "EdEditModeSelect.h"

//#include "EditorActionsPrimitive.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPanePrimitive::cEdEditPanePrimitive(iEdScnObject* apObject) : iEdScnObjEditPane(apObject)
{
	mpInpAlignToWorld = NULL;
}

//----------------------------------------------------------------------------

cEdEditPanePrimitive::~cEdEditPanePrimitive()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPanePrimitive::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);
	AddPropertyScale(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y+5;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y+5;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y+5;
	mpInpScale->SetPosition(vPos);
	vPos.y += mpInpScale->GetSize().y+5;

	mpInpScale->SetLowerBound(true, 0);

	mpTabPrimitive = mpTabs->AddTab(_W("Primitive"));
    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddPrimitivePropertySet(mpTabPrimitive);
	
	
	if(mpObject->GetTypeName()==_W("Plane"))
	{
		mpTabPrimitiveType = mpTabs->AddTab(_W("Plane"));
		AddPrimitivePropertySetPlane(mpTabPrimitiveType);
	}
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPanePrimitive::AddPrimitivePropertySet(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);

	mpInpMaterial = CreateInputMaterial(vPos, _W("Material"),  apParentTab);

	vPos.y += mpInpMaterial->GetSize().y+15;

	mpInpCollides = CreateInputBool(vPos, _W("Collides"),  apParentTab);

	vPos.y += mpInpCollides->GetSize().y+10;

	mpInpCastShadows = CreateInputBool(vPos, _W("Cast Shadows"),  apParentTab);
}

//----------------------------------------------------------------------------

void cEdEditPanePrimitive::AddPrimitivePropertySetPlane(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);

	mpInpTileAmount = CreateInputVec3(vPos, _W("Tile amount"),  apParentTab, 50, tWStringList(), eEdInpStyle_RowLabelOnTop, 0.1f);
	vPos.y+= mpInpTileAmount->GetSize().y+10;
	mpInpTileOffset = CreateInputVec3(vPos, _W("Tile offset"),  apParentTab, 50, tWStringList(), eEdInpStyle_RowLabelOnTop, 0.1f);
	vPos.y+= mpInpTileOffset->GetSize().y+10;
	mpInpTextureAngle = CreateInputNumber(vPos, _W("Texture angle"),  apParentTab, 50, 15);
	vPos.y+= mpInpTextureAngle->GetSize().y+10;

	mpInpAlignToWorld = CreateInputBool(vPos, _W("Align To World Coords"),  apParentTab);
}

//----------------------------------------------------------------------------

void cEdEditPanePrimitive::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();

	iEdScnObjPrimitive* pPrimitive = static_cast<iEdScnObjPrimitive*>(mpObject);

	mpInpMaterial->SetValue(cString::To16Char(pPrimitive->GetMaterial()),false);
	mpInpCollides->SetValue(pPrimitive->GetCollides(), false);
	mpInpCastShadows->SetValue(pPrimitive->GetCastShadows(), false);

	if(mpObject->GetTypeName()==_W("Plane"))
	{
		cEdObjPlane* pPlane = static_cast<cEdObjPlane*>(mpObject);

		mpInpTileAmount->SetValue(pPlane->GetTileAmount(), false);
		mpInpTileOffset->SetValue(pPlane->GetTileOffset(), false);
		mpInpTextureAngle->SetValue(cMath::ToDeg(pPlane->GetTextureAngle()), false);
		mpInpAlignToWorld->SetValue(pPlane->GetAlignToWorld(), false);
	}
}

//----------------------------------------------------------------------------

bool cEdEditPanePrimitive::WindowSpecificInputCallback(iEdInput* apInput)
{
	if(iEdScnObjEditPane::WindowSpecificInputCallback(apInput))
		return true;

	iEdAction* pAction = NULL;

	if(apInput==mpInpAlignToWorld)
	{
		pAction = mpObject->CreateSetBoolAction(ePrimitivePlaneBool_AlignToWorld, mpInpAlignToWorld->GetValue());
	}
	else if(apInput==mpInpCastShadows)
	{
		pAction = mpObject->CreateSetBoolAction(ePrimitiveBool_CastShadows, mpInpCastShadows->GetValue());
	}
	else if(apInput==mpInpCollides)
	{
		pAction = mpObject->CreateSetBoolAction(ePrimitiveBool_Collides, mpInpCollides->GetValue());
	}
	else if(apInput==mpInpMaterial)
	{
		tString sFile = mpInpMaterial->GetValue8Char();
		if(sFile.empty() || cEdLoader::LoadTexture2D(sFile))
			pAction = mpObject->CreateSetStringAction(ePrimitiveStr_Material, sFile);
	}
	else if(apInput==mpInpTileAmount)
	{
		pAction = mpObject->CreateSetVector3fAction(ePrimitivePlaneVec3f_TileAmount, mpInpTileAmount->GetValue());
	}
	else if(apInput==mpInpTileOffset)
	{
		pAction = mpObject->CreateSetVector3fAction(ePrimitivePlaneVec3f_TileOffset, mpInpTileOffset->GetValue());
	}
	else if(apInput==mpInpTextureAngle)
	{
		pAction = mpObject->CreateSetFloatAction(ePrimitivePlaneFloat_TextureAngle, cMath::ToRad(mpInpTextureAngle->GetValue()));
	}

	AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
