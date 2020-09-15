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

#include "EntityWrapperLightPoint.h"
#include "EntityWrapperLight.h"

#include "EditorWorld.h"
#include "EditorBaseClasses.h"

#include "EditorWindowViewport.h"

#include "EditorHelper.h"

//---------------------------------------------------------------------------

cIconEntityLightPoint::cIconEntityLightPoint(iEntityWrapper* apParent) : iIconEntityLight(apParent, "Point")
{
}

bool cIconEntityLightPoint::Create(const tString& asName)
{
	cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
	mpEntity = pWorld->CreateLightPoint(asName);

	return true;
}

cEntityWrapperTypeLightPoint::cEntityWrapperTypeLightPoint() : iEntityWrapperTypeLight("PointLight", eEditorEntityLightType_Point)
{
	mScaleType = eScaleType_None;
}

iEntityWrapperData* cEntityWrapperTypeLightPoint::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataLightPoint,(this));
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

cEntityWrapperDataLightPoint::cEntityWrapperDataLightPoint(iEntityWrapperType* apType)  : iEntityWrapperDataLight(apType)
{
	SetName("PointLight");
}

iEntityWrapper* cEntityWrapperDataLightPoint::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperLightPoint,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEntityWrapperLightPoint::cEntityWrapperLightPoint(iEntityWrapperData* apData) : iEntityWrapperLight(apData)/*apEditorWorld, 
																										  alID, asName,
																										  "LightPoint",
																										  (iLight*)CreateEngineEntity(apEditorWorld, alID, asName),
																										  eEditorEntityLightType_Point,
																										  true, eScaleType_None)*/
{
	//mpLight = (cLightPoint*)mpEngineEntity;

	//SetDiffuseColor(cColor(1,1));
}

//---------------------------------------------------------------------------

cEntityWrapperLightPoint::~cEntityWrapperLightPoint()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cEntityWrapperLightPoint::SetGobo(const tString& asGoboFilename)
{
	cResources* pRes = GetEditorWorld()->GetEditor()->GetEngine()->GetResources();
	
	iTexture* pTex = NULL;

	if(cEditorHelper::LoadTextureResource(eEditorTextureResourceType_CubeMap, msGoboFilename, &pTex))
		msGoboFilename = asGoboFilename;
	else
		msGoboFilename = "";

	((cLightPoint*)mpEngineEntity->GetEntity())->SetGoboTexture(pTex);	

}

//---------------------------------------------------------------------------

void cEntityWrapperLightPoint::DrawLightTypeSpecific(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, 
													 iEditorEditMode* apEditMode, bool abIsSelected)
{
	if(abIsSelected==false) return;
	
	apFunctions->GetLowLevelGfx()->DrawSphere(mvPosition, mfRadius, mcolDiffuseColor);
}

//---------------------------------------------------------------------------

/*
void cEntityWrapperLightPoint::SaveToElement(cXmlElement* apElement)
{
	apElement->SetValue("PointLight");

	cEntityWrapperLight::SaveToElement(apElement);
}
*/

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEngineEntity* cEntityWrapperLightPoint::CreateSpecificEngineEntity()
{
	return hplNew(cIconEntityLightPoint,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
