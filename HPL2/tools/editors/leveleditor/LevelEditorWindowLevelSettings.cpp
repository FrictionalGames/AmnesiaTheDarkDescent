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

#include "LevelEditorWindowLevelSettings.h"
#include "LevelEditorActions.h"
#include "../common/EditorWorld.h"
#include "../common/EntityWrapperDecal.h"
#include "../common/EditorAction.h"

//---------------------------------------------------------------------------------

cLevelEditorWindowLevelSettings::cLevelEditorWindowLevelSettings(iEditorBase* apEditor) : iEditorWindowPopUp(apEditor, 
																											"LevelSettings", 
																											true, 
																											false, 
																											true, 
																											cVector2f(300,150))
{	

}

cLevelEditorWindowLevelSettings::~cLevelEditorWindowLevelSettings()
{
}

//---------------------------------------------------------------------------------

void cLevelEditorWindowLevelSettings::OnUpdate(float afTimeStep)
{
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	////////////////////////////////////////////////////////
	// SkyBox parameters
	mpInpSkyboxActive->SetValue(pWorld->GetSkyboxActive(), false);
	mpInpSkyboxTexture->SetValue(cString::To16Char(pWorld->GetSkyboxTexture()), false);
	mpInpSkyboxColor->SetValue(pWorld->GetSkyboxColor(), false);

	////////////////////////////////////////////////////////
	// Global Fog parameters
	mpInpFogActive->SetValue(pWorld->GetFogActive(), false);
	mpInpFogCulling->SetValue(pWorld->GetFogCulling(), false);
	mpInpFogStart->SetValue(pWorld->GetFogStart(), false);
	mpInpFogEnd->SetValue(pWorld->GetFogEnd(), false);
	mpInpFogFallOffExp->SetValue(pWorld->GetFogFalloffExp(), false);
	mpInpFogColor->SetValue(pWorld->GetFogColor(), false);

	////////////////////////////////////////////////////////
	// Global Decal parameters
	mpInpGlobalMaxDecalTris->SetValue((float)cEntityWrapperTypeDecal::GetGlobalMaxTriangles(), false);
}

//---------------------------------------------------------------------------------

void cLevelEditorWindowLevelSettings::OnInitLayout()
{
	iEditorWindowPopUp::OnInitLayout();
	mpWindow->SetText(_W("Level Settings"));

	cWidgetTabFrame* pTabFrame = mpSet->CreateWidgetTabFrame(cVector3f(10,35,0.1f), mvSize-cVector2f(20,40), _W(""), mpWindow);
	
	////////////////////////////////////////////////////////
	// SkyBox parameters tab
	cWidgetTab* pTab = pTabFrame->AddTab(_W("SkyBox"));
	cVector3f vPos = cVector3f(10,10,0.1f);

	mpInpSkyboxActive = CreateInputBool(vPos, _W("Active"), "", pTab);
	vPos.y += mpInpSkyboxActive->GetSize().y +10;
	mpInpSkyboxColor = CreateInputColorFrame(vPos, _W("Color"), "", pTab);
	mpInpSkyboxTexture = CreateInputFile(vPos + cVector3f(mpInpSkyboxColor->GetSize().x+20,-4,0), _W("Texture"), "", pTab);
	mpInpSkyboxTexture->SetBrowserType(eEditorResourceType_Texture);
	mpInpSkyboxTexture->SetBrowserSubType(eEditorTextureResourceType_CubeMap);
	mpInpSkyboxTexture->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
	mpInpSkyboxTexture->UpdateLayout();
	
	////////////////////////////////////////////////////////
	// Global Fog parameters tab
	pTab = pTabFrame->AddTab(_W("Fog"));
	vPos = cVector3f(10,10,0.1f);

	mpInpFogActive = CreateInputBool(vPos, _W("Active"), "", pTab);
	vPos.y += mpInpFogActive->GetSize().y +10;
	mpInpFogCulling = CreateInputBool(vPos, _W("Culling"), "", pTab);

	vPos = mpInpFogActive->GetPosition() + cVector3f(mpInpFogActive->GetSize().x+30,-4,0);
	mpInpFogStart = CreateInputNumber(vPos, _W("Start"), "", pTab, 50, 1);
	mpInpFogStart->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
	mpInpFogStart->UpdateLayout();
	mpInpFogEnd = CreateInputNumber(vPos + cVector3f(mpInpFogStart->GetSize().x+10,0,0), _W("End"), "", pTab, 50, 1);
	mpInpFogEnd->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
	mpInpFogEnd->UpdateLayout();
	vPos.y += mpInpFogEnd->GetSize().y +5;
	mpInpFogFallOffExp = CreateInputNumber(vPos, _W("Falloff Exp"), "", pTab, 50, 0.1f);
	mpInpFogFallOffExp->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
	mpInpFogFallOffExp->UpdateLayout();

	vPos.y += mpInpFogFallOffExp->GetSize().y +5;
	mpInpFogColor = CreateInputColorFrame(vPos, _W("Color"), "", pTab);

	////////////////////////////////////////////////////////
	// Global decal parameters tab
	pTab = pTabFrame->AddTab(_W("Decals"));
	vPos = cVector3f(10,10,0.1f);

	mpInpGlobalMaxDecalTris = CreateInputNumber(vPos, _W("Global Max Triangles per decal"), "", pTab, 70, 10.0f);
	mpInpGlobalMaxDecalTris->SetLowerBound(true, 0.0f);

	vPos.x += mpInpGlobalMaxDecalTris->GetSize().x + 15;
	mpBResetDecals = mpSet->CreateWidgetButton(vPos, cVector2f(100,20), _W("Reset created decals"), pTab);
	mpBResetDecals->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ResetDecals));

}

//---------------------------------------------------------------------------------

void cLevelEditorWindowLevelSettings::OnSetActive(bool abX)
{
	iEditorWindowPopUp::OnSetActive(abX);

	if(abX)
		OnUpdate(0);
}

//---------------------------------------------------------------------------------

bool cLevelEditorWindowLevelSettings::WindowSpecificInputCallback(iEditorInput* apInput)
{
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	iEditorAction* pAction = NULL;

	if(apInput==mpInpSkyboxActive)
		pAction = hplNew(cLevelEditorActionSetSkyboxActive,(pWorld, mpInpSkyboxActive->GetValue()));
	else if(apInput==mpInpSkyboxTexture)
		pAction = hplNew(cLevelEditorActionSetSkyboxTexture,(pWorld, cString::To8Char(mpInpSkyboxTexture->GetValue())));
	else if(apInput==mpInpSkyboxColor)
		pAction = hplNew(cLevelEditorActionSetSkyboxColor,(pWorld, mpInpSkyboxColor->GetValue()));

	else if(apInput==mpInpFogActive)
		pAction = hplNew(cLevelEditorActionSetFogBoolProperty,(pWorld, eFogBoolProp_Active, mpInpFogActive->GetValue()));
	else if(apInput==mpInpFogCulling)
		pAction = hplNew(cLevelEditorActionSetFogBoolProperty,(pWorld, eFogBoolProp_Culling, mpInpFogCulling->GetValue()));
	else if(apInput==mpInpFogStart)
		pAction = hplNew(cLevelEditorActionSetFogFloatProperty,(pWorld, eFogFloatProp_Start, mpInpFogStart->GetValue()));
	else if(apInput==mpInpFogEnd)
		pAction = hplNew(cLevelEditorActionSetFogFloatProperty,(pWorld, eFogFloatProp_End, mpInpFogEnd->GetValue()));
	else if(apInput==mpInpFogFallOffExp)
		pAction = hplNew(cLevelEditorActionSetFogFloatProperty,(pWorld, eFogFloatProp_FalloffExp, mpInpFogFallOffExp->GetValue()));
	else if(apInput==mpInpFogColor)
		pAction = hplNew(cLevelEditorActionSetFogColor,(pWorld, mpInpFogColor->GetValue()));

	else if(apInput==mpInpGlobalMaxDecalTris)
		cEntityWrapperTypeDecal::SetGlobalMaxTriangles((int)mpInpGlobalMaxDecalTris->GetValue());
		//pAction = hplNew(cLevelEditorActionSetGlobalMaxDecalTris, (pWorld, (int)mpInpGlobalMaxDecalTris->GetValue()));


    mpEditor->AddAction(pAction);

	return true;
}

//---------------------------------------------------------------------------------

bool cLevelEditorWindowLevelSettings::ResetDecals(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	iEntityWrapperType* pDecalType = pWorld->GetEntityTypeByID(eEditorEntityType_Decal);
	iProp* pPropMaxTris = pDecalType->GetPropInt(eDecalInt_MaxTris);

	tIntList lstDecalIDs;

	////////////////////////////////////////////////
	// Filter decals from other entities
	const tEntityWrapperMap& mapEntities = pWorld->GetEntities();
	tEntityWrapperMap::const_iterator it = mapEntities.begin();
	for(;it!=mapEntities.end();++it)
	{
		iEntityWrapper* pEnt = it->second;
		if(pEnt->GetTypeID()!=eEditorEntityType_Decal)
			continue;

		lstDecalIDs.push_back(pEnt->GetID());
	}

	if(lstDecalIDs.empty())
		return true;

	iEditorAction* pAction = hplNew(cEditorActionObjectSetProperty<int>,(pWorld, lstDecalIDs, pPropMaxTris, -1)); 
	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditorWindowLevelSettings, ResetDecals);

//---------------------------------------------------------------------------------
