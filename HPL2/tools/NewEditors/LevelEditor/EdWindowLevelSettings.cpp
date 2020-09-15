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

#include "../LevelEditor/EdWindowLevelSettings.h"

//#include "LevelEditorActions.h"
#include "../LevelEditor/HPLMap.h"
//#include "../Common/EntityWrapperDecal.h"
//#include "../Common/EdAction.h"

#include "../Common/Editor.h"

//---------------------------------------------------------------------------------

cEdWindowLevelSettings::cEdWindowLevelSettings(iEditor* apEditor) : iEdPopUp(apEditor,_W("LevelSettings"))
{	

}

cEdWindowLevelSettings::~cEdWindowLevelSettings()
{
}

//---------------------------------------------------------------------------------

void cEdWindowLevelSettings::OnUpdate()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpEditor->GetWorld());

	////////////////////////////////////////////////////////
	// SkyBox parameters
	mpInpSkyboxActive->SetValue(pMap->GetSkyboxActive(), false);
	mpInpSkyboxTexture->SetValue(cString::To16Char(pMap->GetSkyboxTexture()), false);
	mpInpSkyboxColor->SetValue(pMap->GetSkyboxColor(), false);

	////////////////////////////////////////////////////////
	// Global Fog parameters
	mpInpFogActive->SetValue(pMap->GetFogActive(), false);
	mpInpFogCulling->SetValue(pMap->GetFogCulling(), false);
	mpInpFogStart->SetValue(pMap->GetFogStart(), false);
	mpInpFogEnd->SetValue(pMap->GetFogEnd(), false);
	mpInpFogFallOffExp->SetValue(pMap->GetFogFalloffExp(), false);
	mpInpFogColor->SetValue(pMap->GetFogColor(), false);

	////////////////////////////////////////////////////////
	// Global Decal parameters
	//mpInpGlobalMaxDecalTris->SetValue((float)cEntityWrapperTypeDecal::GetGlobalMaxTriangles(), false);
}

//---------------------------------------------------------------------------------

void cEdWindowLevelSettings::OnCreateLayout()
{
	iEdPopUp::OnCreateLayout();
	GetBG()->SetText(_W("Level Settings"));
	cVector2f vSize = cVector2f(300,250);
	SetSize(vSize);

	cWidgetTabFrame* pTabFrame = mpSet->CreateWidgetTabFrame(cVector3f(10,35,0.1f), vSize-cVector2f(20,40), _W(""), GetBG());
	
	////////////////////////////////////////////////////////
	// SkyBox parameters tab
	cWidgetTab* pTab = pTabFrame->AddTab(_W("SkyBox"));
	cVector3f vPos = cVector3f(10,10,0.1f);

	mpInpSkyboxActive = CreateInputBool(vPos, _W("Active"), pTab);
	vPos.y += mpInpSkyboxActive->GetSize().y +10;
	mpInpSkyboxColor = CreateInputColor(vPos, _W("Color"), pTab);
	mpInpSkyboxTexture = CreateInputTexture(vPos + cVector3f(mpInpSkyboxColor->GetSize().x+20,-4,0), _W("Texture"), eEdTexture_CubeMap, pTab);
	mpInpSkyboxTexture->SetStyle(eEdInpStyle_RowLabelOnLeft);
	mpInpSkyboxTexture->UpdateLayout();
	
	////////////////////////////////////////////////////////
	// Global Fog parameters tab
	pTab = pTabFrame->AddTab(_W("Fog"));
	vPos = cVector3f(10,10,0.1f);

	mpInpFogActive = CreateInputBool(vPos, _W("Active"), pTab);
	vPos.y += mpInpFogActive->GetSize().y +10;
	mpInpFogCulling = CreateInputBool(vPos, _W("Culling"), pTab);

	vPos = mpInpFogActive->GetPosition() + cVector3f(mpInpFogActive->GetSize().x+30,-4,0);
	mpInpFogStart = CreateInputNumber(vPos, _W("Start"), pTab, 50, 1);
	mpInpFogStart->SetStyle(eEdInpStyle_RowLabelOnLeft);
	mpInpFogStart->UpdateLayout();
	mpInpFogEnd = CreateInputNumber(vPos + cVector3f(mpInpFogStart->GetSize().x+10,0,0), _W("End"), pTab, 50, 1);
	mpInpFogEnd->SetStyle(eEdInpStyle_RowLabelOnLeft);
	mpInpFogEnd->UpdateLayout();
	vPos.y += mpInpFogEnd->GetSize().y +5;
	mpInpFogFallOffExp = CreateInputNumber(vPos, _W("Falloff Exp"), pTab, 50, 0.1f);
	mpInpFogFallOffExp->SetStyle(eEdInpStyle_RowLabelOnLeft);
	mpInpFogFallOffExp->UpdateLayout();

	vPos.y += mpInpFogFallOffExp->GetSize().y +5;
	mpInpFogColor = CreateInputColor(vPos, _W("Color"), pTab);

	////////////////////////////////////////////////////////
	// Global decal parameters tab
	pTab = pTabFrame->AddTab(_W("Decals"));
	vPos = cVector3f(10,10,0.1f);

	mpInpGlobalMaxDecalTris = CreateInputNumber(vPos, _W("Global Max Triangles per decal"), pTab, 70, 10.0f);
	mpInpGlobalMaxDecalTris->SetLowerBound(true, 0.0f);

	vPos.x += mpInpGlobalMaxDecalTris->GetSize().x + 15;
	mpBResetDecals = mpSet->CreateWidgetButton(vPos, cVector2f(100,20), _W("Reset created decals"), pTab);
	mpBResetDecals->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ResetDecals));

}

//---------------------------------------------------------------------------------

void cEdWindowLevelSettings::OnSetActive(bool abX)
{
	iEdPopUp::OnSetActive(abX);

	if(abX)
		OnUpdate();
}

//---------------------------------------------------------------------------------

bool cEdWindowLevelSettings::WindowSpecificInputCallback(iEdInput* apInput)
{
	cHplMap* pMap = static_cast<cHplMap*>(mpEditor->GetWorld());
	iEdAction* pAction = NULL;

	if(apInput==mpInpSkyboxActive)
		pAction = pMap->CreateSetSkyboxActiveAction(mpInpSkyboxActive->GetValue());
		//pAction = hplNew(cEdActionLevelSetSkyboxActive,(pWorld, mpInpSkyboxActive->GetValue()));
	else if(apInput==mpInpSkyboxTexture)
		pAction = pMap->CreateSetSkyboxTextureAction(mpInpSkyboxTexture->GetValue8Char());
		//pAction = hplNew(cEdActionLevelSetSkyboxTexture,(pWorld, cString::To8Char(mpInpSkyboxTexture->GetValue())));
	else if(apInput==mpInpSkyboxColor)
		pAction = pMap->CreateSetSkyboxColorAction(mpInpSkyboxColor->GetValue());
		//pAction = hplNew(cEdActionLevelSetSkyboxColor,(pWorld, mpInpSkyboxColor->GetValue()));
	else if(apInput==mpInpFogActive)
		pAction = pMap->CreateSetFogBoolAction(eFogBoolProp_Active, mpInpFogActive->GetValue());
		//pAction = hplNew(cLevelEditorActionSetFogBoolProperty,(pWorld, eFogBoolProp_Active, mpInpFogActive->GetValue()));
	else if(apInput==mpInpFogCulling)
		pAction = pMap->CreateSetFogBoolAction(eFogBoolProp_Culling, mpInpFogCulling->GetValue());
		//pAction = hplNew(cLevelEditorActionSetFogBoolProperty,(pWorld, eFogBoolProp_Culling, mpInpFogCulling->GetValue()));
	else if(apInput==mpInpFogStart)
		pAction = pMap->CreateSetFogFloatAction(eFogFloatProp_Start, mpInpFogStart->GetValue());
		//pAction = hplNew(cLevelEditorActionSetFogFloatProperty,(pWorld, eFogFloatProp_Start, mpInpFogStart->GetValue()));
	else if(apInput==mpInpFogEnd)
		pAction = pMap->CreateSetFogFloatAction(eFogFloatProp_End, mpInpFogEnd->GetValue());
		//pAction = hplNew(cLevelEditorActionSetFogFloatProperty,(pWorld, eFogFloatProp_End, mpInpFogEnd->GetValue()));
	else if(apInput==mpInpFogFallOffExp)
		pAction = pMap->CreateSetFogFloatAction(eFogFloatProp_FalloffExp, mpInpFogFallOffExp->GetValue());
		//pAction = hplNew(cLevelEditorActionSetFogFloatProperty,(pWorld, eFogFloatProp_FalloffExp, mpInpFogFallOffExp->GetValue()));
	else if(apInput==mpInpFogColor)
		pAction = pMap->CreateSetFogColorAction(mpInpFogColor->GetValue());
		//pAction = hplNew(cLevelEditorActionSetFogColor,(pWorld, mpInpFogColor->GetValue()));

//	else if(apInput==mpInpGlobalMaxDecalTris)
//		cEntityWrapperTypeDecal::SetGlobalMaxTriangles((int)mpInpGlobalMaxDecalTris->GetValue());
		//pAction = hplNew(cLevelEditorActionSetGlobalMaxDecalTris, (pWorld, (int)mpInpGlobalMaxDecalTris->GetValue()));


    mpEditor->AddAction(pAction);

	return true;
}

//---------------------------------------------------------------------------------

bool cEdWindowLevelSettings::ResetDecals(iWidget* apWidget, const cGuiMessageData& aData)
{
	/*
	iEdWorld* pWorld = mpEditor->GetWorld();
	iEdObjectType* pType = pWorld->GetTypeByName(_W("Decals"));
	iProp* pPropMaxTris = pType->GetPropByName("MaxTriangles");

	tIntList lstDecalIDs;

	////////////////////////////////////////////////
	// Filter decals from other entities
	const tEdObjectMap& mapObject = pWorld->GetObjects();
	tEdObjectMap::const_iterator it = mapObject.begin();
	for(;it!=mapObject.end();++it)
	{
		iEdObject* pObj = it->second;
		if(pObj->IsA(pType->GetTopID()==false)
			continue;

		lstDecalIDs.push_back(pObj->GetID());
	}

	if(lstDecalIDs.empty())
		return true;

	iEditorAction* pAction = hplNew(cEditorActionObjectSetProperty<int>,(pWorld, lstDecalIDs, pPropMaxTris, -1)); 
	mpEditor->AddAction(pAction);
	*/

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowLevelSettings, ResetDecals);

//---------------------------------------------------------------------------------
