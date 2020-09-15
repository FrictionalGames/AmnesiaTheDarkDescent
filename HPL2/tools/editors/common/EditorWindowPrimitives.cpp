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

#include "EditorWindowPrimitives.h"
#include "EditorEditModePrimitive.h"

#include "EditorTypes.h"
#include "EditorInput.h"
#include "EntityWrapper.h"

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

cEditorWindowPrimitives::cEditorWindowPrimitives(cEditorEditModePrimitives* apEditMode) : iEditModeObjectCreatorWindow(apEditMode)
{
}

cEditorWindowPrimitives::~cEditorWindowPrimitives()
{
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

//-------------------------------------------------------------------

//-------------------------------------------------------------------

void cEditorWindowPrimitives::Reset()
{
	ResetInputs();
}

//-------------------------------------------------------------------

const tString& cEditorWindowPrimitives::GetMaterial()
{
	gsPrimitiveMaterial = cString::To8Char(mpInputMaterial->GetValue());
	//if(gsPrimitiveMaterial=="")
	//	gsPrimitiveMaterial = gsTypeDefaultMaterial[GetPrimitiveType()];

	return gsPrimitiveMaterial;
}

//-------------------------------------------------------------------

cVector3f cEditorWindowPrimitives::GetTileAmount()
{
	return mpInputTileAmount->GetValue();
}

//-------------------------------------------------------------------

cVector3f cEditorWindowPrimitives::GetTileOffset()
{
	return mpInputTileOffset->GetValue();
}

//-------------------------------------------------------------------

float cEditorWindowPrimitives::GetTextureAngle()
{
	return mpInpTextureAngle->GetValue();
}

//-------------------------------------------------------------------

bool cEditorWindowPrimitives::GetAlignToWorld()
{
	return mpInputAlignToWorld->GetValue();
}

//-------------------------------------------------------------------

bool cEditorWindowPrimitives::GetCollides()
{
	return mpInpCollides->GetValue();
}

bool cEditorWindowPrimitives::GetCastShadows()
{
	return mpInputCastShadows->GetValue();
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

//-------------------------------------------------------------------

void cEditorWindowPrimitives::OnInitLayout()
{
	//////////////////////
	// Set up layout
    mpBGFrame->SetSize(cVector2f(200,600));

	cVector3f vPos = cVector3f(5,5,1);
	cVector2f vSize;

	///////////////////////////////////////////////////
	// Primitive Selection
	mpInpPrimitiveType = CreateInputEnum(vPos, _W("Type"), "", tWStringList(), mpBGFrame);

	//mpPrimitiveSelection = mpSet->CreateWidgetComboBox(vPos,cVector2f(190,13),_W(""), mpBGFrame);
	cEditorEditModePrimitives* pEditMode = (cEditorEditModePrimitives*)mpEditMode;
	for(int i=0; i<pEditMode->GetTypeNum(); ++i)
	{
		iEntityWrapperType* pType = pEditMode->GetType(i);
		mpInpPrimitiveType->AddValue(cString::To16Char(pType->GetXmlElementName()));
	}

	vPos.y += mpInpPrimitiveType->GetSize().y + 15;

	mpInputMaterial = CreateInputFile(vPos, _W("Material"), "InputMaterial", mpBGFrame);
	mpInputMaterial->SetBrowserType(eEditorResourceType_Material);

	vPos.y += mpInputMaterial->GetSize().y + 15;

	mpInputTileAmount = CreateInputVec3(vPos, _W("Tile Amount"), "InputTileAmount", mpBGFrame);
	
	vPos.y += mpInputTileAmount->GetSize().y + 15;

	mpInputTileOffset = CreateInputVec3(vPos, _W("Tile Offset"), "InputTileOffset", mpBGFrame);
	
	vPos.y += mpInputTileOffset->GetSize().y + 15;

	mpInpTextureAngle = CreateInputNumber(vPos, _W("Texture Angle"), "", mpBGFrame, 50, 15);

	vPos.y += mpInpTextureAngle->GetSize().y + 15;

	mpInputAlignToWorld = CreateInputBool(vPos, _W("Align to World Coordinates"), "InputAlignToWorld", mpBGFrame);
	
	vPos.y += mpInputAlignToWorld->GetSize().y + 15;

	mpInputCastShadows = CreateInputBool(vPos, _W("Cast Shadows"), "InputCastShadows", mpBGFrame);
	
	vPos.y += mpInputCastShadows->GetSize().y + 15;

	mpInpCollides = CreateInputBool(vPos, _W("Collides"), "", mpBGFrame);


	mpInpPrimitiveType->SetValue(0);
	mpInputMaterial->SetValue(_W(""),false);
	mpInputTileAmount->SetValue(1,false,true);
	mpInputTileOffset->SetValue(0,false, true);
	mpInpTextureAngle->SetValue(0,false);
	mpInputAlignToWorld->SetValue(false,false,true);
	mpInputCastShadows->SetValue(false,false,true);
	mpInpCollides->SetValue(true,false,true);
}

void cEditorWindowPrimitives::OnSetActive(bool abX)
{
	if(abX)
		Reset();
}

//-------------------------------------------------------------------

void cEditorWindowPrimitives::ResetInputs()
{
	
}


//-------------------------------------------------------------------

bool cEditorWindowPrimitives::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(apInput==mpInpPrimitiveType)
		((cEditorEditModePrimitives*)mpEditMode)->SetPrimitiveType(mpInpPrimitiveType->GetValue());

	return true;
}


//-------------------------------------------------------------------

//-------------------------------------------------------------------
//-------------------------------------------------------------------


