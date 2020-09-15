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

#include "../Common/EdObjLightBox.h"

#include "../Common/Editor.h"
#include "../Common/EdWorld.h"
#include "../Common/EdWindowViewport.h"

#include "../Common/BoxCreator.h"

//---------------------------------------------------------------------------

cIconBoxLight::cIconBoxLight(iEdScnObject* apParent) : iIconObjectLight(apParent, "Box")
{
}

bool cIconBoxLight::Create(const tString& asName)
{
	cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
	SetUpLight(pWorld->CreateLightBox(asName));

	return true;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// TYPE BOXLIGHT - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cTypeBoxLight::cTypeBoxLight() : iLightSubType(_W("Box"), "BoxLight", hplNew(cBoxCreator,(iEditor::GetInstance())))
{
	AddInt(eLightBoxInt_BlendFunc, "BlendFunc", eLightBoxBlendFunc_Replace);
	AddVec3f(eLightBoxVec3f_Size, "Size", 1);

	mbRotateable = false;
	mbScalable = false;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// TYPE BOXLIGHT - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cTypeBoxLight::SetUpCreationData(iEdObjectData* apData)
{
	cBoxCreator* pCreator = static_cast<cBoxCreator*>(mpShapeCreator);

	apData->SetVec3f(eScnVec3f_Translation, pCreator->GetBoxCenter());
	apData->SetVec3f(eLightBoxVec3f_Size, pCreator->GetBoxSize());

	return true;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// TYPE BOXLIGHT - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObjectData* cTypeBoxLight::CreateTypeSpecificData()
{
	return hplNew(cEdObjBoxLightData,(this));
}

//---------------------------------------------------------------------------

cEdObjBoxLightData::cEdObjBoxLightData(iEdObjectType* apType) : iEdObjLightData(apType)
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

iEdObject* cEdObjBoxLightData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjBoxLight,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjBoxLight::cEdObjBoxLight(iEdObjectData* apData) : iEdObjLight(apData)
{
}

//---------------------------------------------------------------------------

cEdObjBoxLight::~cEdObjBoxLight()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEdObjBoxLight::SetProperty(int alPropID, const int alX)
{
	switch(alPropID)
	{
	case eLightBoxInt_BlendFunc:
		SetBlendFunc((eLightBoxBlendFunc)alX);
		break;
	default:
		return iEdObjLight::SetProperty(alPropID, alX);
	}

	return true;
}

bool cEdObjBoxLight::SetProperty(int alPropID, const cVector3f& avX)
{
	switch(alPropID)
	{
	case eLightBoxVec3f_Size:
		SetSize(avX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, avX);
	}

	return true;
}

bool cEdObjBoxLight::GetProperty(int alPropID, int& alX)
{
	switch(alPropID)
	{
	case eLightBoxInt_BlendFunc:
		alX = GetBlendFunc();
		break;
	default:
		return iEdObjLight::GetProperty(alPropID, alX);
	}

	return true;
}

bool cEdObjBoxLight::GetProperty(int alPropID, cVector3f& avX)
{
	switch(alPropID)
	{
	case eLightBoxVec3f_Size:
		avX = GetSize();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, avX);
	}

	return true;
}

//---------------------------------------------------------------------------

void cEdObjBoxLight::DrawLightTypeSpecific(const cModuleDrawData& aData)
{
	aData.mpFunctions->GetLowLevelGfx()->DrawBoxMinMax(GetTranslation()-(mvSize*0.5f), GetTranslation()+(mvSize*0.5f),mcolDiffuseColor);
}


//---------------------------------------------------------------------------

void cEdObjBoxLight::SetSize(const cVector3f& avSize)
{
	mvSize = avSize;

	static_cast<cLightBox*>(mpEngObject->GetObject())->SetSize(avSize);
}

//---------------------------------------------------------------------------

void cEdObjBoxLight::SetBlendFunc(eLightBoxBlendFunc aFunc)
{
	mBlendFunc = aFunc;

	static_cast<cLightBox*>(mpEngObject->GetObject())->SetBlendFunc(aFunc);
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEngineObject* cEdObjBoxLight::CreateEngineObject()
{
	return hplNew( cIconBoxLight,(this));
}
