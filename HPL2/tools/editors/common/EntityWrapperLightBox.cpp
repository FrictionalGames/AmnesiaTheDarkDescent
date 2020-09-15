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

#include "EntityWrapperLightBox.h"
#include "EntityWrapperLight.h"

#include "EditorWorld.h"

#include "EditorWindowViewport.h"

//---------------------------------------------------------------------------

cIconEntityLightBox::cIconEntityLightBox(iEntityWrapper* apParent) : iIconEntityLight(apParent, "Box")
{
}

bool cIconEntityLightBox::Create(const tString& asName)
{
	cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();

	mpEntity = pWorld->CreateLightBox(asName);

	return true;
}

//---------------------------------------------------------------------------

cEntityWrapperTypeLightBox::cEntityWrapperTypeLightBox() : iEntityWrapperTypeLight("BoxLight", eEditorEntityLightType_Box)
{
	AddInt(eLightBoxInt_BlendFunc, "BlendFunc", eLightBoxBlendFunc_Replace);
	AddVec3f(eLightBoxVec3f_Size, "Size", 1);
}

iEntityWrapperData* cEntityWrapperTypeLightBox::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataLightBox,(this));
}

//---------------------------------------------------------------------------

cEntityWrapperDataLightBox::cEntityWrapperDataLightBox(iEntityWrapperType* apType) : iEntityWrapperDataLight(apType)
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataLightBox::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperLightBox,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEntityWrapperLightBox::cEntityWrapperLightBox(iEntityWrapperData* apData) : iEntityWrapperLight(apData)/*apEditorWorld, 
																									  alID, asName,
																									  "LightBox",
																									  (iLight*)CreateEngineEntity(apEditorWorld, alID, asName), eEditorEntityLightType_Box,
																									  true, eScaleType_Normal)
																									  */
{
}

//---------------------------------------------------------------------------

cEntityWrapperLightBox::~cEntityWrapperLightBox()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEntityWrapperLightBox::SetProperty(int alPropID, const int& alX)
{
	switch(alPropID)
	{
	case eLightBoxInt_BlendFunc:
		SetBlendFunc((eLightBoxBlendFunc)alX);
		break;
	default:
		return iEntityWrapperLight::SetProperty(alPropID, alX);
	}

	return true;
}

bool cEntityWrapperLightBox::SetProperty(int alPropID, const cVector3f& avX)
{
	switch(alPropID)
	{
	case eLightBoxVec3f_Size:
		SetSize(avX);
		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, avX);
	}

	return true;
}

bool cEntityWrapperLightBox::GetProperty(int alPropID, int& alX)
{
	switch(alPropID)
	{
	case eLightBoxInt_BlendFunc:
		alX = GetBlendFunc();
		break;
	default:
		return iEntityWrapperLight::GetProperty(alPropID, alX);
	}

	return true;
}

bool cEntityWrapperLightBox::GetProperty(int alPropID, cVector3f& avX)
{
	switch(alPropID)
	{
	case eLightBoxVec3f_Size:
		avX = GetSize();
		break;
	default:
		return iEntityWrapper::GetProperty(alPropID, avX);
	}

	return true;
}

//---------------------------------------------------------------------------

void cEntityWrapperLightBox::DrawLightTypeSpecific(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, 
												   iEditorEditMode* apEditMode, bool abIsSelected)
{
	if(abIsSelected==false) return;

	apFunctions->GetLowLevelGfx()->DrawBoxMinMax(mvPosition-(mvSize*0.5f), mvPosition+(mvSize*0.5f),mcolDiffuseColor);
}


//---------------------------------------------------------------------------

void cEntityWrapperLightBox::SetSize(const cVector3f& avSize)
{
	mvSize = avSize;

	((cLightBox*)mpEngineEntity->GetEntity())->SetSize(avSize);
}

//---------------------------------------------------------------------------

void cEntityWrapperLightBox::SetBlendFunc(eLightBoxBlendFunc aFunc)
{
	mBlendFunc = aFunc;

	((cLightBox*)mpEngineEntity->GetEntity())->SetBlendFunc(aFunc);
}

//---------------------------------------------------------------------------

void cEntityWrapperLightBox::SetAbsScale(const cVector3f& avScale, int alAxis)
{
	iEntityWrapper::SetAbsScale(avScale);

	if(mpEngineEntity) SetSize(mvScale);
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEngineEntity* cEntityWrapperLightBox::CreateSpecificEngineEntity()
{
	return hplNew( cIconEntityLightBox,(this));
}
