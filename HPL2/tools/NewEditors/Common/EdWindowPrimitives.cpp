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

#include "../Common/EdWindowPrimitives.h"
#include "../Common/EdEditModePrimitives.h"

#include "../Common/EditorTypes.h"
#include "../Common/EdInput.h"
#include "../Common/EdWorld.h"

#include "../Common/EdObjPrimitivePlane.h"

//-------------------------------------------------------------------

static tString gsTypeDefaultMaterial[] = 
{
	"editor_primitive_plane",
};
static tString gsPrimitiveMaterial;

//-------------------------------------------------------------------


//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEdWindowPrimitives::cEdWindowPrimitives(iEdEditMode* apMode) : iEdScnObjCreatorPane(apMode)
{
	mpCurrentType = NULL;
}

cEdWindowPrimitives::~cEdWindowPrimitives()
{
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

bool cEdWindowPrimitives::SetUpCreationData(iEdObjectData* apData)
{
	apData->SetString(ePrimitiveStr_Material, mpInputMaterial->GetValue8Char());
	apData->SetVec3f(ePrimitivePlaneVec3f_TileAmount, mpInputTileAmount->GetValue());
	apData->SetVec3f(ePrimitivePlaneVec3f_TileOffset, mpInputTileOffset->GetValue());
	apData->SetFloat(ePrimitivePlaneFloat_TextureAngle, cMath::ToRad(mpInpTextureAngle->GetValue()));
	apData->SetBool(ePrimitivePlaneBool_AlignToWorld, mpInputAlignToWorld->GetValue());

	return true;
}

const tString& cEdWindowPrimitives::GetMaterial()
{
	gsPrimitiveMaterial = mpInputMaterial->GetValue8Char();
	//if(gsPrimitiveMaterial=="")
	//	gsPrimitiveMaterial = gsTypeDefaultMaterial[GetPrimitiveType()];

	return gsPrimitiveMaterial;
}

//-------------------------------------------------------------------

cVector3f cEdWindowPrimitives::GetTileAmount()
{
	return mpInputTileAmount->GetValue();
}

//-------------------------------------------------------------------

cVector3f cEdWindowPrimitives::GetTileOffset()
{
	return mpInputTileOffset->GetValue();
}

//-------------------------------------------------------------------

float cEdWindowPrimitives::GetTextureAngle()
{
	return mpInpTextureAngle->GetValue();
}

//-------------------------------------------------------------------

bool cEdWindowPrimitives::GetAlignToWorld()
{
	return mpInputAlignToWorld->GetValue();
}

//-------------------------------------------------------------------

bool cEdWindowPrimitives::GetCollides()
{
	return mpInpCollides->GetValue();
}

bool cEdWindowPrimitives::GetCastShadows()
{
	return mpInputCastShadows->GetValue();
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEdWindowPrimitives::OnCreateLayout()
{
	iEdPane::OnCreateLayout();
	SetHeight(600.0f);

	cVector3f vPos = cVector3f(10,10,0.1f);
	cVector2f vSize;

	///////////////////////////////////////////////////
	// Primitive Selection
	AddTypeControls();
	mpInpSubType->SetPosition(vPos);
	
	vPos.y += mpInpSubType->GetSize().y + 15;

	mpInputMaterial = CreateInputMaterial(vPos, _W("Material"), GetBG());
	
	vPos.y += mpInputMaterial->GetSize().y + 15;

	mpInputTileAmount = CreateInputVec3(vPos, _W("Tile Amount"), GetBG());
	
	vPos.y += mpInputTileAmount->GetSize().y + 15;

	mpInputTileOffset = CreateInputVec3(vPos, _W("Tile Offset"), GetBG());
	
	vPos.y += mpInputTileOffset->GetSize().y + 15;

	mpInpTextureAngle = CreateInputNumber(vPos, _W("Texture Angle"),  GetBG(), 50, 15);

	vPos.y += mpInpTextureAngle->GetSize().y + 15;

	mpInputAlignToWorld = CreateInputBool(vPos, _W("Align to World Coordinates"), GetBG());
	
	vPos.y += mpInputAlignToWorld->GetSize().y + 15;

	mpInputCastShadows = CreateInputBool(vPos, _W("Cast Shadows"), GetBG());
	
	vPos.y += mpInputCastShadows->GetSize().y + 15;

	mpInpCollides = CreateInputBool(vPos, _W("Collides"), GetBG());
}


void cEdWindowPrimitives::OnReset()
{
	mpInpSubType->SetValue(0, true, true, true);

	mpInputMaterial->SetValue(_W(""),false);
	mpInputTileAmount->SetValue(1,false,true);
	mpInputTileOffset->SetValue(0,false, true);
	mpInpTextureAngle->SetValue(0,false);
	mpInputAlignToWorld->SetValue(false,false,true);
	mpInputCastShadows->SetValue(false,false,true);
	mpInpCollides->SetValue(true,false,true);
}

//-------------------------------------------------------------------

bool cEdWindowPrimitives::WindowSpecificInputCallback(iEdInput* apInput)
{
	if(apInput==mpInpSubType)
	{
		cEdEditModePrimitives* pMode = static_cast<cEdEditModePrimitives*>(mpEditMode);
		pMode->SetCurrentType(static_cast<iPrimitiveSubType*>(mpInpSubType->GetSelectedUserData()));
	}
	else
		return iEdScnObjCreatorPane::WindowSpecificInputCallback(apInput);

	return true;
}


//-------------------------------------------------------------------

//-------------------------------------------------------------------
//-------------------------------------------------------------------


