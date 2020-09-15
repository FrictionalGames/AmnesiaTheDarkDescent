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

#include "../Common/EdObjFogArea.h"

#include "../Common/EdWorld.h"
#include "../Common/EdEditPaneFogArea.h"

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// ICON ENTITY FOGAREA
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cIconObjectFogArea::cIconObjectFogArea(iEdScnObject* apParent) : iIconObject(apParent, "FogArea")
{
}

cIconObjectFogArea::~cIconObjectFogArea()
{
	if(mpObject)
	{
		cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
		pWorld->DestroyFogArea((cFogArea*)mpObject);
	}
}

//------------------------------------------------------------------------------

bool cIconObjectFogArea::Create(const tString& asName)
{
	iIconObject::Create(asName);

	cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
	cFogArea* pFog = pWorld->CreateFogArea(asName);
	//pFog->SetRenderableUserData(pFog);

	mpObject = pFog;

	return IsCreated();
}

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// FOG AREA TYPE
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cTypeFogArea::cTypeFogArea() : iEdScnObjType(_W("FogArea"), "FogArea")
{
	AddColor(eFogAreaCol_Color, "Color");
	AddFloat(eFogAreaFloat_Start, "Start");
	AddFloat(eFogAreaFloat_End, "End", 10.0f);
	AddFloat(eFogAreaFloat_FalloffExp, "FalloffExp", 1.0f);

	AddBool(eFogAreaBool_ShownBacksideWhenOutside, "ShownBacksideWhenOutside", false);
	AddBool(eFogAreaBool_ShownBacksideWhenInside, "ShownBacksideWhenInside");
}

iEdObjectData* cTypeFogArea::CreateTypeSpecificData()
{
	return hplNew(cEdObjFogAreaData, (this));
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// FOG AREA DATA
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEdObjFogAreaData::cEdObjFogAreaData(iEdObjectType* apType) : iEdScnObjData(apType)
{
}

//------------------------------------------------------------------------------

iEdObject* cEdObjFogAreaData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjFogArea,(this));
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// FOG AREA
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

cEdObjFogArea::cEdObjFogArea(iEdObjectData* apData) : iEdScnObject(apData)
{
}

cEdObjFogArea::~cEdObjFogArea()
{
}

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

bool cEdObjFogArea::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case eFogAreaBool_ShownBacksideWhenOutside:
		abX = GetShownBacksideWhenOutside();
		break;
	case eFogAreaBool_ShownBacksideWhenInside:
		abX = GetShownBacksideWhenInside();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, abX);
	}

	return true;
}

bool cEdObjFogArea::GetProperty(int alPropID, float& afX)
{
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
		return iEdScnObject::GetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjFogArea::GetProperty(int alPropID, cColor& aX)
{
	switch(alPropID)
	{
	case eFogAreaCol_Color:
		aX = GetColor();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, aX);
	}

	return true;
}

bool cEdObjFogArea::SetProperty(int alPropID, const bool abX)
{
	switch(alPropID)
	{
	case eFogAreaBool_ShownBacksideWhenOutside:
		SetShownBacksideWhenOutside(abX);
		break;
	case eFogAreaBool_ShownBacksideWhenInside:
		SetShownBacksideWhenInside(abX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, abX);
	}

	return true;
}

bool cEdObjFogArea::SetProperty(int alPropID, const float afX)
{
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
		return iEdScnObject::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjFogArea::SetProperty(int alPropID, const cColor& aX)
{
	switch(alPropID)
	{
	case eFogAreaCol_Color:
		SetColor(aX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, aX);
	}

	return true;
}

//------------------------------------------------------------------------------

void cEdObjFogArea::SetColor(const cColor& aCol)
{
	mColor = aCol;

	static_cast<cFogArea*>(mpEngObject->GetObject())->SetColor(mColor);
}

//------------------------------------------------------------------------------

void cEdObjFogArea::SetStart(float afX)
{
	mfStart = afX;

	static_cast<cFogArea*>(mpEngObject->GetObject())->SetStart(afX);
}

//------------------------------------------------------------------------------

void cEdObjFogArea::SetEnd(float afX)
{
	mfEnd = afX;

	static_cast<cFogArea*>(mpEngObject->GetObject())->SetEnd(afX);
}

//------------------------------------------------------------------------------

void cEdObjFogArea::SetFalloffExp(float afX)
{
	mfFalloffExp = afX;

	static_cast<cFogArea*>(mpEngObject->GetObject())->SetFalloffExp(afX);
}

//------------------------------------------------------------------------------

void cEdObjFogArea::SetShownBacksideWhenInside(bool abX)
{
	mbShownBacksideWhenInside = abX;

	static_cast<cFogArea*>(mpEngObject->GetObject())->SetShowBacksideWhenInside(abX);
}

//------------------------------------------------------------------------------

void cEdObjFogArea::SetShownBacksideWhenOutside(bool abX)
{
	mbShownBacksideWhenOutside = abX;

	static_cast<cFogArea*>(mpEngObject->GetObject())->SetShowBacksideWhenOutside(abX);
}

//------------------------------------------------------------------------------

iEdEditPane* cEdObjFogArea::CreateEditPane()
{
	return hplNew(cEdEditPaneFogArea,(this));
}

//------------------------------------------------------------------------------

void cEdObjFogArea::OnDraw(const cModuleDrawData& aData)
{
	iEdScnObject::OnDraw(aData);
	if(mbSelected==false)
		return;

	aData.mpFunctions->SetMatrix(&mmtxTransform);
	aData.mpFunctions->GetLowLevelGfx()->DrawBoxMinMax(-0.5f, 0.5f, cColor(1,1));
}

//------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

iEngineObject* cEdObjFogArea::CreateEngineObject()
{
	return hplNew(cIconObjectFogArea,(this));
}

//------------------------------------------------------------------------------
