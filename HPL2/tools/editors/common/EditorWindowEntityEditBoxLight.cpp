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

#include "EditorWindowEntityEditBoxLight.h"

#include "EditorHelper.h"
#include "EditorWindowFactory.h"
#include "EditorWindowTextureBrowser.h"

#include "EntityWrapperLight.h"
#include "EntityWrapperLightSpot.h"
#include "EntityWrapperLightBox.h"

#include "EditorAction.h"

#include "EditorInput.h"

//------------------------------------------------------------

cEditorWindowEntityEditBoxLight::cEditorWindowEntityEditBoxLight(cEditorEditModeSelect* apEditMode, iEntityWrapperLight* apLight) : cEditorWindowEntityEditBox(apEditMode,apLight)
{
	mpLight = apLight;

	mpInpRadius = NULL;
	mpGroupShadows = NULL;

	// Box Light specific
	mpComboBoxBlendFunc = NULL;
}

//------------------------------------------------------------

cEditorWindowEntityEditBoxLight::~cEditorWindowEntityEditBoxLight()
{
}

//------------------------------------------------------------

void cEditorWindowEntityEditBoxLight::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	cWidgetTab* pTab;

	AddPropertyName(mpTabGeneral);
	AddPropertyActive(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);

	AddPropertyDiffuseColor(mpTabGeneral);


	switch(mpLight->GetLightType())
	{
	case eEditorEntityLightType_Box:
		pTab = mpTabs->AddTab(_W("Box"));
		AddPropertySetBox(pTab);
		break;
	case eEditorEntityLightType_Point:
		pTab = mpTabs->AddTab(_W("Point"));
		AddPropertySetPoint(pTab);
		break;
	case eEditorEntityLightType_Spot:
		pTab = mpTabs->AddTab(_W("Spot"));
		AddPropertySetSpot(pTab);
		break;
	default:
		break;
	}

	mpTabFlicker = mpTabs->AddTab(_W("Flicker"));
	AddPropertySetFlicker(mpTabFlicker);

	AddPropertyGobo(mpTabGeneral);
	AddPropertyFalloffMap(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y+5;
	mpInpActive->SetPosition(vPos);
	vPos.y += mpInpActive->GetSize().y+5;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y+5;
	mpGroupGobo->SetPosition(vPos);
	vPos.y += mpGroupGobo->GetSize().y + 5;
	mpGroupFalloff->SetPosition(vPos);
	vPos.y += mpGroupFalloff->GetSize().y + 5;
	mpGroupDiffuse->SetPosition(vPos);
}

//------------------------------------------------------------

void cEditorWindowEntityEditBoxLight::AddPropertyGobo(cWidgetTab *apParentTab)
{
	mpGroupGobo = mpSet->CreateWidgetDummy(0,apParentTab);

	cVector3f vPos = cVector3f(0,0,0.1f);


	mpInpGobo = CreateInputFile(vPos, _W("Gobo"), "", mpGroupGobo, 120);
	mpInpGobo->SetInitialPath(mpEditor->GetMainLookUpDir(eDir_Lights));
	mpInpGobo->SetBrowserType(eEditorResourceType_Texture);
	if(mpLight->GetLightType()==eEditorEntityLightType_Point)
		mpInpGobo->SetBrowserSubType(eEditorTextureResourceType_CubeMap);
	else
		mpInpGobo->SetBrowserSubType(eEditorTextureResourceType_2D);

	vPos.y += mpInpGobo->GetSize().y;

	mpInpGoboAnimMode = CreateInputEnum(vPos, _W("Anim Mode"), "", tWStringList(), mpGroupGobo, 75);
	mpInpGoboAnimMode->AddValue(_W("None"));
	mpInpGoboAnimMode->AddValue(_W("Loop"));
	mpInpGoboAnimMode->AddValue(_W("Oscillate"));

	vPos.x += mpInpGoboAnimMode->GetSize().x+5;

    mpInpGoboAnimFrameTime = CreateInputNumber(vPos, _W("Frame Time"), "", mpGroupGobo);
}

//------------------------------------------------------------

void cEditorWindowEntityEditBoxLight::AddPropertyFalloffMap(cWidgetTab* apParentTab)
{
	mpGroupFalloff = mpSet->CreateWidgetDummy(0,apParentTab);

	mpInpFalloffMap = CreateInputFile(cVector3f(0,0,0.1f), _W("Falloff Map"), "", mpGroupFalloff,120);
	mpInpFalloffMap->SetInitialPath(mpEditor->GetMainLookUpDir(eDir_Lights));
	mpInpFalloffMap->SetBrowserSubType(eEditorTextureResourceType_1D);
}

//------------------------------------------------------------

void cEditorWindowEntityEditBoxLight::AddPropertyRadius(cWidgetTab* apParentTab)
{
	mpGroupRadius = mpSet->CreateWidgetDummy(0,apParentTab);

	mpInpRadius = CreateInputNumber(cVector3f(0,0,0.1f), _W("Radius"), "", mpGroupRadius, 50, 0.5f);
}

//------------------------------------------------------------

void cEditorWindowEntityEditBoxLight::AddPropertyDiffuseColor(cWidgetTab* apParentTab)
{
	cColor col = mpLight->GetDiffuseColor();

	mpGroupDiffuse= mpSet->CreateWidgetDummy(0,apParentTab);

	mpInpDiffuse = CreateInputColorFrame(cVector3f(0,0,0.1f), _W("Diffuse color"), "", mpGroupDiffuse);
}

//------------------------------------------------------------

//------------------------------------------------------------

void cEditorWindowEntityEditBoxLight::AddPropertyCastShadows(cWidgetTab* apParentTab)
{
	mpGroupShadows = mpSet->CreateWidgetDummy(0, apParentTab);

	cVector3f vPos = cVector3f(0,0,0.1f);

	mpInpCastShadows = CreateInputBool(vPos, _W("Cast Shadows"), "", mpGroupShadows);

	vPos.y += mpInpCastShadows->GetSize().y + 5;

	mpInpShadowRes = CreateInputEnum(vPos, _W("Shadow Resolution"), "", tWStringList(), mpGroupShadows);
	mpInpShadowRes->AddValue(_W("Low"));
	mpInpShadowRes->AddValue(_W("Medium"));
	mpInpShadowRes->AddValue(_W("High"));

	vPos.y += mpInpShadowRes->GetSize().y + 5;

	mpInpShadowAffectStatic = CreateInputBool(vPos, _W("Shadows affect static"), "", mpGroupShadows);
	vPos.y += mpInpShadowAffectStatic->GetSize().y + 5;
	mpInpShadowAffectDynamic = CreateInputBool(vPos, _W("Shadows affect dynamic"), "", mpGroupShadows);
}

//------------------------------------------------------------

void cEditorWindowEntityEditBoxLight::AddPropertySetFlicker(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);

	mpInpFlickerActive = CreateInputBool(vPos, _W("Flicker Active"), "", apParentTab);
	
	///////////////////////////////////////
	// On Parameters
	vPos.y += mpInpFlickerActive->GetSize().y;
	mpGFlickerOn = mpSet->CreateWidgetDummy(vPos, apParentTab);

	vPos = cVector3f(0,0,0.1f);
	
	mpInpFlickerOnMinLength = CreateInputNumber(vPos, _W("On Min time"), "", mpGFlickerOn,50, 0.1f);
	mpInpFlickerOnMinLength->SetLowerBound(true, 0);
	vPos.x += mpInpFlickerOnMinLength->GetSize().x + 10;
	mpInpFlickerOnMaxLength = CreateInputNumber(vPos, _W("On Max time"), "", mpGFlickerOn, 50, 0.1f);
	mpInpFlickerOnMaxLength->SetLowerBound(true, 0);
	vPos.x=0;
	vPos.y += mpInpFlickerOnMinLength->GetSize().y + 10;
	// Time Params
	mpInpFlickerOnSound = CreateInputFile(vPos, _W("On Sound"), "", mpGFlickerOn);
	mpInpFlickerOnSound->SetBrowserType(eEditorResourceType_Sound);
	mpInpFlickerOnSound->SetInitialPath(mpEditor->GetMainLookUpDir(eDir_Sounds));
	vPos.y += mpInpFlickerOnSound->GetSize().y;
	mpInpFlickerOnPS = CreateInputFile(vPos, _W("On PS"), "", mpGFlickerOn);
	mpInpFlickerOnPS->SetBrowserType(eEditorResourceType_ParticleSystem);
	mpInpFlickerOnPS->SetInitialPath(mpEditor->GetMainLookUpDir(eDir_Particles));

	////////////////////////////////////////////
	// Off Parameters
	vPos = mpGFlickerOn->GetLocalPosition() + cVector3f(0,mpGFlickerOn->GetSize().y + 10,0);
	mpGFlickerOff = mpSet->CreateWidgetDummy(vPos, apParentTab);

	// Radius
	vPos = cVector3f(0,0,0.1f);
	mpInpFlickerOffMinLength = CreateInputNumber(vPos, _W("Off Min time"), "", mpGFlickerOff, 50, 0.1f);
	mpInpFlickerOffMinLength->SetLowerBound(true, 0);
	vPos.x += mpInpFlickerOffMinLength->GetSize().x + 10;
	mpInpFlickerOffMaxLength = CreateInputNumber(vPos, _W("Off Max time"), "", mpGFlickerOff, 50, 0.1f);
	mpInpFlickerOffMaxLength->SetLowerBound(true, 0);
	vPos.x=0;
	vPos.y += mpInpFlickerOffMinLength->GetSize().y + 10;
	// Time Params
	mpInpFlickerOffSound = CreateInputFile(vPos, _W("Off Sound"), "", mpGFlickerOff);
	mpInpFlickerOffSound->SetBrowserType(eEditorResourceType_Sound);
	mpInpFlickerOffSound->SetInitialPath(mpEditor->GetMainLookUpDir(eDir_Sounds));

	vPos.y += mpInpFlickerOffSound->GetSize().y;

	mpInpFlickerOffPS = CreateInputFile(vPos, _W("Off PS"), "", mpGFlickerOff);
	mpInpFlickerOffPS->SetBrowserType(eEditorResourceType_ParticleSystem);
	mpInpFlickerOffPS->SetInitialPath(mpEditor->GetMainLookUpDir(eDir_Particles));

	vPos.y += mpInpFlickerOffPS->GetSize().y + 10;

	mpInpFlickerOffRadius = CreateInputNumber(vPos, _W("Off Radius"), "", mpGFlickerOff, 50, 0.1f);
	vPos.y += mpInpFlickerOffRadius->GetSize().y + 10;
	mpInpFlickerOffColor = CreateInputColorFrame(vPos, _W("Off Color"), "", mpGFlickerOff);

	////////////////////////////////////////
	// Fade Parameters
	vPos = mpGFlickerOff->GetLocalPosition() + cVector3f(0,mpGFlickerOff->GetSize().y + 10,0);
	
	mpGFlickerFade = mpSet->CreateWidgetDummy(vPos, apParentTab);

	vPos = cVector3f(0,0,0.1f);

	mpInpFlickerFade = CreateInputBool(vPos, _W("Fade"), "", mpGFlickerFade);

	vPos.y += mpInpFlickerFade->GetSize().y;

	mpInpFlickerFadeOnMinLength = CreateInputNumber(vPos, _W("On Min time"), "", mpGFlickerFade,50, 0.1f);
	mpInpFlickerFadeOnMinLength->SetLowerBound(true, 0);
	vPos.x += mpInpFlickerFadeOnMinLength->GetSize().x+10;
	mpInpFlickerFadeOnMaxLength = CreateInputNumber(vPos, _W("On Max time"), "", mpGFlickerFade,50, 0.1f);
	mpInpFlickerFadeOnMaxLength->SetLowerBound(true, 0);
	
	vPos.x = 0;
	vPos.y += mpInpFlickerFadeOnMaxLength->GetSize().y+10;

	mpInpFlickerFadeOffMinLength = CreateInputNumber(vPos, _W("Off Min time"), "", mpGFlickerFade, 50, 0.1f);
	mpInpFlickerFadeOffMinLength->SetLowerBound(true, 0);
	vPos.x += mpInpFlickerFadeOffMinLength->GetSize().x+10;
	mpInpFlickerFadeOffMaxLength = CreateInputNumber(vPos, _W("Off Max time"), "", mpGFlickerFade, 50, 0.1f);
	mpInpFlickerFadeOffMaxLength->SetLowerBound(true, 0);
}

//------------------------------------------------------------

void cEditorWindowEntityEditBoxLight::AddPropertySetPoint(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,10,0.1f);
	AddPropertyRotation(apParentTab);
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y+10;

	AddPropertyRadius(apParentTab);
	mpGroupRadius->SetPosition(vPos);
}

//------------------------------------------------------------

void cEditorWindowEntityEditBoxLight::AddPropertySetBox(cWidgetTab* apParentTab)
{
	//AddPropertyRotation(apParentTab);
	AddPropertyScale(apParentTab);
	mpInpScale->SetLabel(_W("Size"));

	cVector3f vPos = cVector3f(10,10,0.1f);
	//mpInpRotation->SetPosition(vPos);
	//vPos.y += mpInpRotation->GetSize().y+5;
	mpInpScale->SetPosition(vPos);
	vPos.y += mpInpScale->GetSize().y+5;

	//mvLabelScale[3]->SetText(_W("Size"));

	mpLabelBlendFunc = mpSet->CreateWidgetLabel(vPos,0,_W("Blend Function"), apParentTab);

	vPos.y+= 15;
	mpComboBoxBlendFunc = mpSet->CreateWidgetComboBox(vPos,cVector2f(90,25),_W(""),apParentTab);
	mpComboBoxBlendFunc->AddItem(_W("Replace"));
	mpComboBoxBlendFunc->AddItem(_W("Add"));
	mpComboBoxBlendFunc->AddCallback(eGuiMessage_SelectionChange,this,kGuiCallback(InputCallback));
}

//------------------------------------------------------------

void cEditorWindowEntityEditBoxLight::AddPropertySetSpot(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,10,0.1f);
	AddPropertyRotation(apParentTab);
	mpInpRotation->SetPosition(vPos);

	vPos.y += mpInpRotation->GetSize().y + 5;

	AddPropertyCastShadows(apParentTab);
	mpGroupShadows->SetPosition(vPos);
	vPos.y += mpGroupShadows->GetSize().y + 5;

	AddPropertyRadius(apParentTab);
	mpGroupRadius->SetPosition(vPos);

	mpInpSpotNearClipPlane = CreateInputNumber(vPos + cVector3f(mpGroupRadius->GetSize().x+20,0,0), _W("Near Clip Plane"), "", apParentTab, 50, 0.1f);

	vPos.y += mpGroupRadius->GetSize().y +5;

	mpInpSpotFOV = CreateInputNumber(vPos, _W("FOV"), "", apParentTab, 50, 15);
	mpInpSpotFOV->SetDecimals(3);

	mpInpSpotAspect = CreateInputNumber(vPos + cVector3f(mpInpSpotFOV->GetSize().x+20,0,0), _W("Aspect"), "", apParentTab, 50, 0.1f);

	vPos.y += mpInpSpotFOV->GetSize().y + 10;

	mpInpSpotFalloffMap = CreateInputFile(vPos, _W("Spot Falloff Map"), "", apParentTab);
	mpInpSpotFalloffMap->SetInitialPath(mpEditor->GetMainLookUpDir(eDir_Lights));
	mpInpSpotFalloffMap->SetBrowserSubType(eEditorTextureResourceType_1D);
}

//------------------------------------------------------------

void cEditorWindowEntityEditBoxLight::OnUpdate(float afTimeStep)
{
	if(mpLight==NULL)
		return;

	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);

	if(mpGroupGobo)
	{
		mpInpGobo->SetValue(cString::To16Char(mpLight->GetGoboFilename()),false);
		((iEditorInput*)mpInpGoboAnimMode)->SetValue(cString::To16Char(mpLight->GetGoboAnimMode()), false);
		mpInpGoboAnimFrameTime->SetValue(mpLight->GetGoboAnimFrameTime(), false);
	}
		
	if(mpGroupFalloff)
	{
		mpInpFalloffMap->SetValue(cString::To16Char(mpLight->GetFalloffMap()), false);
	}

	/////////////////////////////////////////////
	// Shadow options
	if(mpGroupShadows)
	{
		mpInpCastShadows->SetValue(mpLight->GetCastShadows(), false);
		tWString sValue = cString::To16Char(mpLight->GetShadowResolution());;
		for(int i=0;i<mpInpShadowRes->GetNumValues();++i)
		{
			if(sValue==mpInpShadowRes->GetEnumValue(i))
			{
				mpInpShadowRes->SetValue(i, false);
				break;
			}
		}	
		mpInpShadowAffectStatic->SetValue(mpLight->GetShadowsAffectStatic(),false);
		mpInpShadowAffectDynamic->SetValue(mpLight->GetShadowsAffectDynamic(),false);
	}

	mpInpDiffuse->SetValue(mpLight->GetDiffuseColor(), false);

	mpInpFlickerActive->SetValue(mpLight->GetFlickerActive(), false);
	
	mpInpFlickerOnMinLength->SetValue(mpLight->GetFlickerOnMinLength(), false);
	mpInpFlickerOnMaxLength->SetValue(mpLight->GetFlickerOnMaxLength(), false);
	mpInpFlickerOnSound->SetValue(cString::To16Char(mpLight->GetFlickerOnSound()), false);
	mpInpFlickerOnPS->SetValue(cString::To16Char(mpLight->GetFlickerOnPS()), false);

	mpInpFlickerOffMinLength->SetValue(mpLight->GetFlickerOffMinLength(), false);
	mpInpFlickerOffMaxLength->SetValue(mpLight->GetFlickerOffMaxLength(), false);
	mpInpFlickerOffSound->SetValue(cString::To16Char(mpLight->GetFlickerOffSound()), false);
	mpInpFlickerOffPS->SetValue(cString::To16Char(mpLight->GetFlickerOffPS()), false);

	mpInpFlickerOffRadius->SetValue(mpLight->GetFlickerOffRadius(), false);
	mpInpFlickerOffColor->SetValue(mpLight->GetFlickerOffColor(), false);
	
	mpInpFlickerFade->SetValue(mpLight->GetFlickerFade(), false);

	mpInpFlickerFadeOnMinLength->SetValue(mpLight->GetFlickerOnFadeMinLength(), false);
	mpInpFlickerFadeOnMaxLength->SetValue(mpLight->GetFlickerOnFadeMaxLength(), false);
	mpInpFlickerFadeOffMinLength->SetValue(mpLight->GetFlickerOffFadeMinLength(), false);
	mpInpFlickerFadeOffMaxLength->SetValue(mpLight->GetFlickerOffFadeMaxLength(), false);

	if(mpInpRadius) mpInpRadius->SetValue(mpLight->GetRadius(), false);

	if(mpComboBoxBlendFunc) mpComboBoxBlendFunc->SetSelectedItem( ((cEntityWrapperLightBox*)mpLight)->GetBlendFunc(),false,false );

	int lightType = mpLight->GetLightType();
	////////////
	// Spot
	if(lightType==eEditorEntityLightType_Spot)
	{
		cEntityWrapperLightSpot* pLight = (cEntityWrapperLightSpot*)mpLight;
		mpInpSpotFOV->SetValue(cMath::ToDeg(pLight->GetFOV()), false);
		mpInpSpotAspect->SetValue(pLight->GetAspect(), false);
		mpInpSpotNearClipPlane->SetValue(pLight->GetNearClipPlane(), false);
		mpInpSpotFalloffMap->SetValue(cString::To16Char(pLight->GetSpotFalloffMap()), false);
	}
}

//------------------------------------------------------------

bool cEditorWindowEntityEditBoxLight::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEditorAction* pAction=NULL;

	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	int lID = mpEntity->GetID();
	tString strFilename = cString::To8Char(apWidget->GetText());
	bool bEmptyString = (strFilename=="");

	
	///////////////////////////////////////////////
	// Boxlight Blend Func
	if(apWidget==mpComboBoxBlendFunc)
	{
		pAction = mpEntity->CreateSetPropertyActionInt(eLightBoxInt_BlendFunc, mpComboBoxBlendFunc->GetSelectedItem());
	}

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxLight,InputCallback);

//------------------------------------------------------------

bool cEditorWindowEntityEditBoxLight::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput))
		return true;

	iEditorAction* pAction = NULL;
	int lID = mpEntity->GetID();
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	tString strFilename;

	/////////////////////////////////////////////////////
	// General Inputs

	///////////////////////////////////
	// Gobo related
	if(apInput==mpInpGobo)
	{
		strFilename = cString::To8Char(mpInpGobo->GetValue());

		eEditorTextureResourceType texType = (mpLight->GetLightType()==eEditorEntityLightType_Point)?
											eEditorTextureResourceType_CubeMap :
											eEditorTextureResourceType_2D;

		
		if(strFilename=="" || cEditorHelper::LoadTextureResource(texType, strFilename, NULL))
		{
			pAction = mpEntity->CreateSetPropertyActionString(eLightStr_Gobo, strFilename);
		}
		else
			mpInpGobo->SetValue(_W(""), false);
	}
	// Gobo Anim Mode
	else if(apInput==mpInpGoboAnimMode)
	{
		pAction = mpEntity->CreateSetPropertyActionString(eLightStr_GoboAnimMode, cString::To8Char(mpInpGoboAnimMode->GetEnumValue(mpInpGoboAnimMode->GetValue())));
	}
	// Gobo Anim Frame Time
	else if(apInput==mpInpGoboAnimFrameTime)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightFloat_GoboAnimFrameTime, mpInpGoboAnimFrameTime->GetValue());
	}
	///////////////////////////////////
	// Falloff map file
	else if(apInput==mpInpFalloffMap)
	{
		strFilename = cString::To8Char(mpInpFalloffMap->GetValue());

		if(strFilename=="" || cEditorHelper::LoadTextureResource(eEditorTextureResourceType_1D, strFilename, NULL))
			pAction = mpEntity->CreateSetPropertyActionString(eLightStr_FalloffMap, strFilename);
		else
			mpInpFalloffMap->SetValue(_W(""), false);
	}

	///////////////////////////////////
	// Shadow related

	// Cast Shadows
	else if(apInput==mpInpCastShadows)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eLightBool_CastShadows, mpInpCastShadows->GetValue());
	}
	// Shadow Resolution
    else if(apInput==mpInpShadowRes)
	{
		pAction = mpEntity->CreateSetPropertyActionString(eLightStr_ShadowResolution, cString::To8Char(mpInpShadowRes->GetEnumValue(mpInpShadowRes->GetValue())));
	}
	// Shadow affect static
	else if(apInput==mpInpShadowAffectStatic)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eLightBool_ShadowsAffectStatic, mpInpShadowAffectStatic->GetValue());
	}
	// Shadow affect dynamic
	else if(apInput==mpInpShadowAffectDynamic)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eLightBool_ShadowsAffectDynamic, mpInpShadowAffectDynamic->GetValue());
	}
	///////////////////////////////////
	// Diffuse
	else if(apInput==mpInpDiffuse)
	{
		pAction = mpEntity->CreateSetPropertyActionColor(eLightCol_Diffuse, mpInpDiffuse->GetValue());
	}

	/////////////////////////////////
	// Radius
	else if(apInput==mpInpRadius)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightFloat_Radius, mpInpRadius->GetValue());
	}

	/////////////////////////////////
	// Flicker Related

	// Active
	else if(apInput==mpInpFlickerActive)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eLightBool_FlickerActive, mpInpFlickerActive->GetValue());
	}

	// On Min Length
	else if(apInput==mpInpFlickerOnMinLength)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightFloat_FlickerOnMinLength, mpInpFlickerOnMinLength->GetValue());
	}

	// On Max Length
	else if(apInput==mpInpFlickerOnMaxLength)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightFloat_FlickerOnMaxLength, mpInpFlickerOnMaxLength->GetValue());
	}

	// Off Min Length
	else if(apInput==mpInpFlickerOffMinLength)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightFloat_FlickerOffMinLength, mpInpFlickerOffMinLength->GetValue());
	}

	// Off Max Length
	else if(apInput==mpInpFlickerOffMaxLength)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightFloat_FlickerOffMaxLength, mpInpFlickerOffMaxLength->GetValue());
	}

	// On Sound
	else if(apInput==mpInpFlickerOnSound)
	{
		pAction = mpEntity->CreateSetPropertyActionString(eLightStr_FlickerOnSound, cString::To8Char(mpInpFlickerOnSound->GetValue()));
	}

	// On PS
	else if(apInput==mpInpFlickerOnPS)
	{
		pAction = mpEntity->CreateSetPropertyActionString(eLightStr_FlickerOnPS, cString::To8Char(mpInpFlickerOnPS->GetValue()));
	}

	// Off Sound
	else if(apInput==mpInpFlickerOffSound)
	{
		pAction = mpEntity->CreateSetPropertyActionString(eLightStr_FlickerOffSound, cString::To8Char(mpInpFlickerOffSound->GetValue()));
	}

	// Off PS
	else if(apInput==mpInpFlickerOffPS)
	{
		pAction = mpEntity->CreateSetPropertyActionString(eLightStr_FlickerOffPS, cString::To8Char(mpInpFlickerOffPS->GetValue()));
	}

	// Off Radius
	else if(apInput==mpInpFlickerOffRadius)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightFloat_FlickerOffRadius, mpInpFlickerOffRadius->GetValue());
	}

	// Off Color
	else if(apInput==mpInpFlickerOffColor)
	{
		pAction = mpEntity->CreateSetPropertyActionColor(eLightCol_FlickerOff, mpInpFlickerOffColor->GetValue());
	}

	// Flicker Fade active
	else if(apInput==mpInpFlickerFade)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eLightBool_FlickerFade, mpInpFlickerFade->GetValue());
	}

	// Fade On Min Length
	else if(apInput==mpInpFlickerFadeOnMinLength)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightFloat_FlickerOnFadeMinLength, mpInpFlickerFadeOnMinLength->GetValue());
	}

	// Fade On Max Length
	else if(apInput==mpInpFlickerFadeOnMaxLength)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightFloat_FlickerOnFadeMaxLength, mpInpFlickerFadeOnMaxLength->GetValue());
	}

	// Fade Off Min Length
	else if(apInput==mpInpFlickerFadeOffMinLength)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightFloat_FlickerOffFadeMinLength, mpInpFlickerFadeOffMinLength->GetValue());
	}

	// Fade Off Max Length
	else if(apInput==mpInpFlickerFadeOffMaxLength)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightFloat_FlickerOffFadeMaxLength, mpInpFlickerFadeOffMaxLength->GetValue());
	}

	////////////////////////////////////////////////////
	// Spot

	// FOV
	else if(apInput==mpInpSpotFOV)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightSpotFloat_FOV, cMath::ToRad(mpInpSpotFOV->GetValue()));
	}

	// Aspect
	else if(apInput==mpInpSpotAspect)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightSpotFloat_Aspect, mpInpSpotAspect->GetValue());
	}

	// Near Clip Plane
	else if(apInput==mpInpSpotNearClipPlane)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eLightSpotFloat_NearClipPlane, mpInpSpotNearClipPlane->GetValue());
	}

	// Spot Falloff Map
	else if(apInput==mpInpSpotFalloffMap)
	{
		strFilename = cString::To8Char(mpInpFalloffMap->GetValue());

		if(strFilename=="" || cEditorHelper::LoadTextureResource(eEditorTextureResourceType_1D, strFilename, NULL))
		{
			pAction = mpEntity->CreateSetPropertyActionString(eLightSpotStr_FalloffMap, cString::To8Char(mpInpSpotFalloffMap->GetValue()));
		}
		else
			mpInpFalloffMap->SetValue(_W(""), false);
	}

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------

