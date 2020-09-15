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

#include "EntityWrapperFogArea.h"
#include "EditorWorld.h"
#include "EditorWindowEntityEditBoxFogArea.h"

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// ICON ENTITY FOGAREA
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cIconEntityFogArea::cIconEntityFogArea(iEntityWrapper* apParent) : iIconEntity(apParent, "FogArea")
{
}

cIconEntityFogArea::~cIconEntityFogArea()
{
	if(mpEntity)
	{
		cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
		pWorld->DestroyFogArea((cFogArea*)mpEntity);
	}
}

//------------------------------------------------------------------------------

bool cIconEntityFogArea::Create(const tString& asName)
{
	iIconEntity::Create(asName);

	cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
	mpEntity = pWorld->CreateFogArea(asName);

	return IsCreated();
}

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// FOG AREA TYPE
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEntityWrapperTypeFogArea::cEntityWrapperTypeFogArea() : iEntityWrapperType(eEditorEntityType_FogArea, _W("FogArea"), "FogArea")
{
	AddColor(eFogAreaCol_Color, "Color");
	AddFloat(eFogAreaFloat_Start, "Start");
	AddFloat(eFogAreaFloat_End, "End", 10.0f);
	AddFloat(eFogAreaFloat_FalloffExp, "FalloffExp", 1.0f);

	AddBool(eFogAreaBool_ShownBacksideWhenOutside, "ShownBacksideWhenOutside", false);
	AddBool(eFogAreaBool_ShownBacksideWhenInside, "ShownBacksideWhenInside");
}

iEntityWrapperData* cEntityWrapperTypeFogArea::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataFogArea, (this));
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// FOG AREA DATA
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEntityWrapperDataFogArea::cEntityWrapperDataFogArea(iEntityWrapperType* apType) : iEntityWrapperData(apType)
{
}

//------------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataFogArea::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperFogArea,(this));
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// FOG AREA
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

cEntityWrapperFogArea::cEntityWrapperFogArea(iEntityWrapperData* apData) : iEntityWrapper(apData)
{
}

cEntityWrapperFogArea::~cEntityWrapperFogArea()
{
}

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

bool cEntityWrapperFogArea::GetProperty(int alPropID, bool& abX)
{
	if(iEntityWrapper::GetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case eFogAreaBool_ShownBacksideWhenOutside:
		abX = GetShownBacksideWhenOutside();
		break;
	case eFogAreaBool_ShownBacksideWhenInside:
		abX = GetShownBacksideWhenInside();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperFogArea::GetProperty(int alPropID, float& afX)
{
	if(iEntityWrapper::GetProperty(alPropID, afX)==true)
		return true;

	switch(alPropID)
	{
	case eFogAreaFloat_Start:
		afX = GetStart();
		break;
	case eFogAreaFloat_End:
		afX = GetEnd();
		break;
	case eFogAreaFloat_FalloffExp:
		afX = GetFalloffExp();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperFogArea::GetProperty(int alPropID, cColor& aX)
{
	if(iEntityWrapper::GetProperty(alPropID, aX)==true)
		return true;

	switch(alPropID)
	{
	case eFogAreaCol_Color:
		aX = GetColor();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperFogArea::SetProperty(int alPropID, const bool& abX)
{
	if(iEntityWrapper::SetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case eFogAreaBool_ShownBacksideWhenOutside:
		SetShownBacksideWhenOutside(abX);
		break;
	case eFogAreaBool_ShownBacksideWhenInside:
		SetShownBacksideWhenInside(abX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperFogArea::SetProperty(int alPropID, const float& afX)
{
	if(iEntityWrapper::SetProperty(alPropID, afX)==true)
		return true;

	switch(alPropID)
	{
	case eFogAreaFloat_Start:
		SetStart(afX);
		break;
	case eFogAreaFloat_End:
		SetEnd(afX);
		break;
	case eFogAreaFloat_FalloffExp:
		SetFalloffExp(afX);
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperFogArea::SetProperty(int alPropID, const cColor& aX)
{
	if(iEntityWrapper::SetProperty(alPropID, aX)==true)
		return true;

	switch(alPropID)
	{
	case eFogAreaCol_Color:
		SetColor(aX);
		break;
	default:
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------

void cEntityWrapperFogArea::SetColor(const cColor& aCol)
{
	mColor = aCol;

	((cFogArea*)mpEngineEntity->GetEntity())->SetColor(mColor);
}

//------------------------------------------------------------------------------

void cEntityWrapperFogArea::SetStart(float afX)
{
	mfStart = afX;

	((cFogArea*)mpEngineEntity->GetEntity())->SetStart(afX);
}

//------------------------------------------------------------------------------

void cEntityWrapperFogArea::SetEnd(float afX)
{
	mfEnd = afX;

	((cFogArea*)mpEngineEntity->GetEntity())->SetEnd(afX);
}

//------------------------------------------------------------------------------

void cEntityWrapperFogArea::SetFalloffExp(float afX)
{
	mfFalloffExp = afX;

	((cFogArea*)mpEngineEntity->GetEntity())->SetFalloffExp(afX);
}

//------------------------------------------------------------------------------

void cEntityWrapperFogArea::SetShownBacksideWhenInside(bool abX)
{
	mbShownBacksideWhenInside = abX;

	((cFogArea*)mpEngineEntity->GetEntity())->SetShowBacksideWhenInside(abX);
}

//------------------------------------------------------------------------------

void cEntityWrapperFogArea::SetShownBacksideWhenOutside(bool abX)
{
	mbShownBacksideWhenOutside = abX;

	((cFogArea*)mpEngineEntity->GetEntity())->SetShowBacksideWhenOutside(abX);
}

//------------------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperFogArea::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	return hplNew(cEditorWindowEntityEditFogArea,(apEditMode,this));
}

//------------------------------------------------------------------------------

void cEntityWrapperFogArea::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol)
{
	iEntityWrapper::Draw(apViewport, apFunctions, apEditMode, abIsSelected);
	if(mbSelected)
	{
		apFunctions->SetMatrix(&mmtxTransform);
		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(-0.5f, 0.5f, cColor(1,1));
	}
}

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

iEngineEntity* cEntityWrapperFogArea::CreateSpecificEngineEntity()
{
	return hplNew(cIconEntityFogArea,(this));
}

//------------------------------------------------------------------------------
