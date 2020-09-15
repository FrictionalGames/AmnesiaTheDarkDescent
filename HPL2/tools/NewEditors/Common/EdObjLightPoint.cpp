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

#include "../Common/EdObjLightPoint.h"

#include "../Common/EdWorld.h"
#include "../Common/Editor.h"

#include "../Common/EdWindowViewport.h"

#include "../Common/SphereCreator.h"
#include "../Common/EdResource.h"

//---------------------------------------------------------------------------

cIconPointLight::cIconPointLight(iEdScnObject* apParent) : iIconObjectLight(apParent, "Point")
{
}

bool cIconPointLight::Create(const tString& asName)
{
	cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
	SetUpLight(pWorld->CreateLightPoint(asName));

	return true;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// POINTLIGHT TYPE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cTypePointLight::cTypePointLight() : iLightSubType(_W("Point"), "PointLight", hplNew(cSphereCreator,(iEditor::GetInstance())))
{
	mbScalable = false;
}


//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// POINTLIGHT TYPE - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cTypePointLight::SetUpCreationData(iEdObjectData* apData)
{
	cSphereCreator* pCreator = static_cast<cSphereCreator*>(mpShapeCreator);

	apData->SetFloat(eLightFloat_Radius, pCreator->GetRadius());
	apData->SetVec3f(eScnVec3f_Translation, pCreator->GetSphereCenter());

	return true;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// POINTLIGHT TYPE - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObjectData* cTypePointLight::CreateTypeSpecificData()
{
	return hplNew(cEdObjPointLightData,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// POINTLIGHT DATA - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjPointLightData::cEdObjPointLightData(iEdObjectType* apType)  : iEdObjLightData(apType)
{
	SetName(_W("PointLight"));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// POINTLIGHT DATA - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObject* cEdObjPointLightData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjPointLight,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// POINTLIGHT - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjPointLight::cEdObjPointLight(iEdObjectData* apData) : iEdObjLight(apData)
{
}

//---------------------------------------------------------------------------

cEdObjPointLight::~cEdObjPointLight()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// POINTLIGHT - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cEdObjPointLight::SetGobo(const tString& asGoboFilename)
{
	cResources* pRes = GetWorld()->GetEditor()->GetEngine()->GetResources();
	
	iTexture* pTex = NULL;
	if(cEdLoader::LoadCubeMap(asGoboFilename, &pTex))
		msGoboFilename = asGoboFilename;
	else
		msGoboFilename.clear();

	static_cast<cLightPoint*>(mpEngObject->GetObject())->SetGoboTexture(pTex);	

}

//---------------------------------------------------------------------------

void cEdObjPointLight::DrawLightTypeSpecific(const cModuleDrawData& aData)
{
	aData.mpFunctions->GetLowLevelGfx()->DrawSphere(GetTranslation(), mfRadius, mcolDiffuseColor);
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// POINTLIGHT - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEngineObject* cEdObjPointLight::CreateEngineObject()
{
	return hplNew(cIconPointLight,(this));
}

//---------------------------------------------------------------------------

