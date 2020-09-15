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

#include "../Common/EdEditPaneLight.h"

#include "../Common/Editor.h"
#include "../Common/EdResource.h"

#include "../Common/EdObjLight.h"
#include "../Common/EdObjLightSpot.h"
#include "../Common/EdObjLightBox.h"

#include "../Common/EdAction.h"

//------------------------------------------------------------

cEdEditPaneLight::cEdEditPaneLight(iEdScnObject* apObject) : iEdScnObjEditPane(apObject)
{
	mpInpRadius = NULL;
	mpGroupShadows = NULL;

	// Box Light specific
	mpInpBoxBlendFunc = NULL;
}

//------------------------------------------------------------

cEdEditPaneLight::~cEdEditPaneLight()
{
}

//------------------------------------------------------------

void cEdEditPaneLight::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	cWidgetTab* pTab;

	AddPropertyName(mpTabGeneral);
	AddPropertyActive(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);

	AddPropertyDiffuseColor(mpTabGeneral);


	const tWString& sType = mpObject->GetTypeName();
	pTab = mpTabs->AddTab(sType);
	
	if(sType==_W("Box"))
		AddPropertySetBox(pTab);
	else if(sType==_W("Point"))
		AddPropertySetPoint(pTab);
	else if(sType==_W("Spot"))
		AddPropertySetSpot(pTab);

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

void cEdEditPaneLight::AddPropertyGobo(cWidgetTab *apParentTab)
{
	mpGroupGobo = mpSet->CreateWidgetDummy(0,apParentTab);

	cVector3f vPos = cVector3f(0,0,0.1f);


	if(mpObject->GetTypeName()!=_W("Point"))
        mpInpGobo = CreateInputTexture2D(vPos, _W("Gobo"), mpGroupGobo, 120, mpEditor->GetMainLookUpDir(eDir_Lights));
	else
		mpInpGobo = CreateInputCubeMap(vPos, _W("Gobo"), mpGroupGobo, 120, mpEditor->GetMainLookUpDir(eDir_Lights));
	
	vPos.y += mpInpGobo->GetSize().y;

	mpInpGoboAnimMode = CreateInputEnum(vPos, _W("Anim Mode"), mpGroupGobo, 75);
	mpInpGoboAnimMode->AddValue(_W("None"));
	mpInpGoboAnimMode->AddValue(_W("Loop"));
	mpInpGoboAnimMode->AddValue(_W("Oscillate"));

	vPos.x += mpInpGoboAnimMode->GetSize().x+5;

    mpInpGoboAnimFrameTime = CreateInputNumber(vPos, _W("Frame Time"),  mpGroupGobo);
}

//------------------------------------------------------------

void cEdEditPaneLight::AddPropertyFalloffMap(cWidgetTab* apParentTab)
{
	mpGroupFalloff = mpSet->CreateWidgetDummy(0,apParentTab);

	mpInpFalloffMap = CreateInputTexture1D(cVector3f(0,0,0.1f), _W("Falloff Map"), mpGroupFalloff, 
											120, mpEditor->GetMainLookUpDir(eDir_Lights));
}

//------------------------------------------------------------

void cEdEditPaneLight::AddPropertyRadius(cWidgetTab* apParentTab)
{
	mpGroupRadius = mpSet->CreateWidgetDummy(0,apParentTab);

	mpInpRadius = CreateInputNumber(cVector3f(0,0,0.1f), _W("Radius"),  mpGroupRadius, 50, 0.5f);
}

//------------------------------------------------------------

void cEdEditPaneLight::AddPropertyDiffuseColor(cWidgetTab* apParentTab)
{
	mpGroupDiffuse= mpSet->CreateWidgetDummy(0,apParentTab);

	mpInpDiffuse = CreateInputColor(cVector3f(0,0,0.1f), _W("Diffuse color"),  mpGroupDiffuse);
}

//------------------------------------------------------------

//------------------------------------------------------------

void cEdEditPaneLight::AddPropertyCastShadows(cWidgetTab* apParentTab)
{
	mpGroupShadows = mpSet->CreateWidgetDummy(0, apParentTab);

	cVector3f vPos = cVector3f(0,0,0.1f);

	mpInpCastShadows = CreateInputBool(vPos, _W("Cast Shadows"),  mpGroupShadows);

	vPos.y += mpInpCastShadows->GetSize().y + 5;

	mpInpShadowRes = CreateInputEnum(vPos, _W("Shadow Resolution"), mpGroupShadows);
	mpInpShadowRes->AddValue(_W("Low"));
	mpInpShadowRes->AddValue(_W("Medium"));
	mpInpShadowRes->AddValue(_W("High"));

	vPos.y += mpInpShadowRes->GetSize().y + 5;

	mpInpShadowAffectStatic = CreateInputBool(vPos, _W("Shadows affect static"),  mpGroupShadows);
	vPos.y += mpInpShadowAffectStatic->GetSize().y + 5;
	mpInpShadowAffectDynamic = CreateInputBool(vPos, _W("Shadows affect dynamic"),  mpGroupShadows);
}

//------------------------------------------------------------

void cEdEditPaneLight::AddPropertySetFlicker(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);

	mpInpFlickerActive = CreateInputBool(vPos, _W("Flicker Active"),  apParentTab);
	
	///////////////////////////////////////
	// On Parameters
	vPos.y += mpInpFlickerActive->GetSize().y;
	mpGFlickerOn = mpSet->CreateWidgetDummy(vPos, apParentTab);

	vPos = cVector3f(0,0,0.1f);
	
	mpInpFlickerOnMinLength = CreateInputNumber(vPos, _W("On Min time"),  mpGFlickerOn,50, 0.1f);
	mpInpFlickerOnMinLength->SetLowerBound(true, 0);
	vPos.x += mpInpFlickerOnMinLength->GetSize().x + 10;
	mpInpFlickerOnMaxLength = CreateInputNumber(vPos, _W("On Max time"),  mpGFlickerOn, 50, 0.1f);
	mpInpFlickerOnMaxLength->SetLowerBound(true, 0);
	vPos.x=0;
	vPos.y += mpInpFlickerOnMinLength->GetSize().y + 10;
	// Time Params
	mpInpFlickerOnSound = CreateInputSound(vPos, _W("On Sound"), mpGFlickerOn);
	
	vPos.y += mpInpFlickerOnSound->GetSize().y;
	mpInpFlickerOnPS = CreateInputParticleSystem(vPos, _W("On PS"),  mpGFlickerOn, 100);
	
	////////////////////////////////////////////
	// Off Parameters
	vPos = mpGFlickerOn->GetLocalPosition() + cVector3f(0,mpGFlickerOn->GetSize().y + 10,0);
	mpGFlickerOff = mpSet->CreateWidgetDummy(vPos, apParentTab);

	// Radius
	vPos = cVector3f(0,0,0.1f);
	mpInpFlickerOffMinLength = CreateInputNumber(vPos, _W("Off Min time"),  mpGFlickerOff, 50, 0.1f);
	mpInpFlickerOffMinLength->SetLowerBound(true, 0);
	vPos.x += mpInpFlickerOffMinLength->GetSize().x + 10;
	mpInpFlickerOffMaxLength = CreateInputNumber(vPos, _W("Off Max time"),  mpGFlickerOff, 50, 0.1f);
	mpInpFlickerOffMaxLength->SetLowerBound(true, 0);
	vPos.x=0;
	vPos.y += mpInpFlickerOffMinLength->GetSize().y + 10;
	// Time Params
	mpInpFlickerOffSound = CreateInputSound(vPos, _W("Off Sound"),  mpGFlickerOff);

	vPos.y += mpInpFlickerOffSound->GetSize().y;

	mpInpFlickerOffPS = CreateInputParticleSystem(vPos, _W("Off PS"),  mpGFlickerOff);

	vPos.y += mpInpFlickerOffPS->GetSize().y + 10;

	mpInpFlickerOffRadius = CreateInputNumber(vPos, _W("Off Radius"),  mpGFlickerOff, 50, 0.1f);
	vPos.y += mpInpFlickerOffRadius->GetSize().y + 10;
	mpInpFlickerOffColor = CreateInputColor(vPos, _W("Off Color"),  mpGFlickerOff);

	////////////////////////////////////////
	// Fade Parameters
	vPos = mpGFlickerOff->GetLocalPosition() + cVector3f(0,mpGFlickerOff->GetSize().y + 10,0);
	
	mpGFlickerFade = mpSet->CreateWidgetDummy(vPos, apParentTab);

	vPos = cVector3f(0,0,0.1f);

	mpInpFlickerFade = CreateInputBool(vPos, _W("Fade"),  mpGFlickerFade);

	vPos.y += mpInpFlickerFade->GetSize().y;

	mpInpFlickerFadeOnMinLength = CreateInputNumber(vPos, _W("On Min time"),  mpGFlickerFade,50, 0.1f);
	mpInpFlickerFadeOnMinLength->SetLowerBound(true, 0);
	vPos.x += mpInpFlickerFadeOnMinLength->GetSize().x+10;
	mpInpFlickerFadeOnMaxLength = CreateInputNumber(vPos, _W("On Max time"),  mpGFlickerFade,50, 0.1f);
	mpInpFlickerFadeOnMaxLength->SetLowerBound(true, 0);
	
	vPos.x = 0;
	vPos.y += mpInpFlickerFadeOnMaxLength->GetSize().y+10;

	mpInpFlickerFadeOffMinLength = CreateInputNumber(vPos, _W("Off Min time"),  mpGFlickerFade, 50, 0.1f);
	mpInpFlickerFadeOffMinLength->SetLowerBound(true, 0);
	vPos.x += mpInpFlickerFadeOffMinLength->GetSize().x+10;
	mpInpFlickerFadeOffMaxLength = CreateInputNumber(vPos, _W("Off Max time"),  mpGFlickerFade, 50, 0.1f);
	mpInpFlickerFadeOffMaxLength->SetLowerBound(true, 0);
}

//------------------------------------------------------------

void cEdEditPaneLight::AddPropertySetPoint(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,10,0.1f);
	AddPropertyRotation(apParentTab);
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y+10;

	AddPropertyRadius(apParentTab);
	mpGroupRadius->SetPosition(vPos);
}

//------------------------------------------------------------

void cEdEditPaneLight::AddPropertySetBox(cWidgetTab* apParentTab)
{
	AddPropertyScale(apParentTab);
	mpInpScale->SetLabel(_W("Size"));

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpScale->SetPosition(vPos);
	vPos.y += mpInpScale->GetSize().y+5;

	mpInpBoxBlendFunc = CreateInputEnum(vPos, _W("Blend Function"), apParentTab);
	mpInpBoxBlendFunc->AddValue(_W("Replace"));
	mpInpBoxBlendFunc->AddValue(_W("Add"));
}

//------------------------------------------------------------

void cEdEditPaneLight::AddPropertySetSpot(cWidgetTab* apParentTab)
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

	mpInpSpotNearClipPlane = CreateInputNumber(vPos + cVector3f(mpGroupRadius->GetSize().x+20,0,0), _W("Near Clip Plane"),  apParentTab, 50, 0.1f);

	vPos.y += mpGroupRadius->GetSize().y +5;

	mpInpSpotFOV = CreateInputNumber(vPos, _W("FOV"),  apParentTab, 50, 15);
	mpInpSpotFOV->SetDecimals(3);

	mpInpSpotAspect = CreateInputNumber(vPos + cVector3f(mpInpSpotFOV->GetSize().x+20,0,0), _W("Aspect"),  apParentTab, 50, 0.1f);

	vPos.y += mpInpSpotFOV->GetSize().y + 10;

	mpInpSpotFalloffMap = CreateInputTexture1D(vPos, _W("Spot Falloff Map"), apParentTab, 100, mpEditor->GetMainLookUpDir(eDir_Lights));
}

//------------------------------------------------------------

void cEdEditPaneLight::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();

	iEdObjLight* pLight = static_cast<iEdObjLight*>(mpObject);

	if(mpGroupGobo)
	{
		mpInpGobo->SetValue(cString::To16Char(pLight->GetGoboFilename()),false);
		mpInpGoboAnimMode->SetValue(cString::To16Char(pLight->GetGoboAnimMode()), false);
		mpInpGoboAnimFrameTime->SetValue(pLight->GetGoboAnimFrameTime(), false);
	}
		
	if(mpGroupFalloff)
	{
		mpInpFalloffMap->SetValue(cString::To16Char(pLight->GetFalloffMap()), false);
	}

	/////////////////////////////////////////////
	// Shadow options
	if(mpGroupShadows)
	{
		mpInpCastShadows->SetValue(pLight->GetCastShadows(), false);
		tWString sValue = cString::To16Char(pLight->GetShadowResolution());;
		for(int i=0;i<mpInpShadowRes->GetNumValues();++i)
		{
			if(sValue==mpInpShadowRes->GetEnumValue(i))
			{
				mpInpShadowRes->SetValue(i, false);
				break;
			}
		}	
		mpInpShadowAffectStatic->SetValue(pLight->GetShadowsAffectStatic(),false);
		mpInpShadowAffectDynamic->SetValue(pLight->GetShadowsAffectDynamic(),false);
	}

	mpInpDiffuse->SetValue(pLight->GetDiffuseColor(), false);

	mpInpFlickerActive->SetValue(pLight->GetFlickerActive(), false);
	
	mpInpFlickerOnMinLength->SetValue(pLight->GetFlickerOnMinLength(), false);
	mpInpFlickerOnMaxLength->SetValue(pLight->GetFlickerOnMaxLength(), false);
	mpInpFlickerOnSound->SetValue(cString::To16Char(pLight->GetFlickerOnSound()), false);
	mpInpFlickerOnPS->SetValue(cString::To16Char(pLight->GetFlickerOnPS()), false);

	mpInpFlickerOffMinLength->SetValue(pLight->GetFlickerOffMinLength(), false);
	mpInpFlickerOffMaxLength->SetValue(pLight->GetFlickerOffMaxLength(), false);
	mpInpFlickerOffSound->SetValue(cString::To16Char(pLight->GetFlickerOffSound()), false);
	mpInpFlickerOffPS->SetValue(cString::To16Char(pLight->GetFlickerOffPS()), false);

	mpInpFlickerOffRadius->SetValue(pLight->GetFlickerOffRadius(), false);
	mpInpFlickerOffColor->SetValue(pLight->GetFlickerOffColor(), false);
	
	mpInpFlickerFade->SetValue(pLight->GetFlickerFade(), false);

	mpInpFlickerFadeOnMinLength->SetValue(pLight->GetFlickerOnFadeMinLength(), false);
	mpInpFlickerFadeOnMaxLength->SetValue(pLight->GetFlickerOnFadeMaxLength(), false);
	mpInpFlickerFadeOffMinLength->SetValue(pLight->GetFlickerOffFadeMinLength(), false);
	mpInpFlickerFadeOffMaxLength->SetValue(pLight->GetFlickerOffFadeMaxLength(), false);

	if(mpInpRadius) mpInpRadius->SetValue(pLight->GetRadius(), false);

	// TODO : MOVE THIS TO SUBCLASSES
	
	if(mpInpBoxBlendFunc)
	{
		cEdObjBoxLight* pLight = static_cast<cEdObjBoxLight*>(mpObject);

		mpInpScale->SetValue(pLight->GetSize(), false);
		mpInpBoxBlendFunc->SetValue(pLight->GetBlendFunc(), false);
	}

	////////////
	// Spot
	if(mpObject->GetTypeName()==_W("Spot"))
	{
		cEdObjSpotLight* pLight = static_cast<cEdObjSpotLight*>(mpObject);

		mpInpSpotFOV->SetValue(cMath::ToDeg(pLight->GetFOV()), false);
		mpInpSpotAspect->SetValue(pLight->GetAspect(), false);
		mpInpSpotNearClipPlane->SetValue(pLight->GetNearClipPlane(), false);
		mpInpSpotFalloffMap->SetValue(cString::To16Char(pLight->GetSpotFalloffMap()), false);
	}
}

//------------------------------------------------------------

bool cEdEditPaneLight::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdAction* pAction = NULL;
	
	tString strFilename;

	/////////////////////////////////////////////////////
	// General Inputs

	///////////////////////////////////
	// Gobo related
	if(apInput==mpInpGobo)
	{
		strFilename = mpInpGobo->GetValue8Char();
		pAction = mpObject->CreateSetStringAction(eLightStr_Gobo, strFilename);
	}
	// Gobo Anim Mode
	else if(apInput==mpInpGoboAnimMode)
	{
		pAction = mpObject->CreateSetStringAction(eLightStr_GoboAnimMode, cString::To8Char(mpInpGoboAnimMode->GetSelectedEnumValue()));
	}
	// Gobo Anim Frame Time
	else if(apInput==mpInpGoboAnimFrameTime)
	{
		pAction = mpObject->CreateSetFloatAction(eLightFloat_GoboAnimFrameTime, mpInpGoboAnimFrameTime->GetValue());
	}
	///////////////////////////////////
	// Falloff map file
	else if(apInput==mpInpFalloffMap)
	{
		strFilename = mpInpFalloffMap->GetValue8Char();
		pAction = mpObject->CreateSetStringAction(eLightStr_FalloffMap, strFilename);
	}

	///////////////////////////////////
	// Shadow related

	// Cast Shadows
	else if(apInput==mpInpCastShadows)
	{
		pAction = mpObject->CreateSetBoolAction(eLightBool_CastShadows, mpInpCastShadows->GetValue());
	}
	// Shadow Resolution
    else if(apInput==mpInpShadowRes)
	{
		pAction = mpObject->CreateSetStringAction(eLightStr_ShadowResolution, cString::To8Char(mpInpShadowRes->GetSelectedEnumValue()));
	}
	// Shadow affect static
	else if(apInput==mpInpShadowAffectStatic)
	{
		pAction = mpObject->CreateSetBoolAction(eLightBool_ShadowsAffectStatic, mpInpShadowAffectStatic->GetValue());
	}
	// Shadow affect dynamic
	else if(apInput==mpInpShadowAffectDynamic)
	{
		pAction = mpObject->CreateSetBoolAction(eLightBool_ShadowsAffectDynamic, mpInpShadowAffectDynamic->GetValue());
	}
	///////////////////////////////////
	// Diffuse
	else if(apInput==mpInpDiffuse)
	{
		pAction = mpObject->CreateSetColorAction(eLightCol_Diffuse, mpInpDiffuse->GetValue());
	}

	/////////////////////////////////
	// Radius
	else if(apInput==mpInpRadius)
	{
		pAction = mpObject->CreateSetFloatAction(eLightFloat_Radius, mpInpRadius->GetValue());
	}

	/////////////////////////////////
	// Flicker Related

	// Active
	else if(apInput==mpInpFlickerActive)
	{
		pAction = mpObject->CreateSetBoolAction(eLightBool_FlickerActive, mpInpFlickerActive->GetValue());
	}

	// On Min Length
	else if(apInput==mpInpFlickerOnMinLength)
	{
		pAction = mpObject->CreateSetFloatAction(eLightFloat_FlickerOnMinLength, mpInpFlickerOnMinLength->GetValue());
	}

	// On Max Length
	else if(apInput==mpInpFlickerOnMaxLength)
	{
		pAction = mpObject->CreateSetFloatAction(eLightFloat_FlickerOnMaxLength, mpInpFlickerOnMaxLength->GetValue());
	}

	// Off Min Length
	else if(apInput==mpInpFlickerOffMinLength)
	{
		pAction = mpObject->CreateSetFloatAction(eLightFloat_FlickerOffMinLength, mpInpFlickerOffMinLength->GetValue());
	}

	// Off Max Length
	else if(apInput==mpInpFlickerOffMaxLength)
	{
		pAction = mpObject->CreateSetFloatAction(eLightFloat_FlickerOffMaxLength, mpInpFlickerOffMaxLength->GetValue());
	}

	// On Sound
	else if(apInput==mpInpFlickerOnSound)
	{
		pAction = mpObject->CreateSetStringAction(eLightStr_FlickerOnSound, mpInpFlickerOnSound->GetValue8Char());
	}

	// On PS
	else if(apInput==mpInpFlickerOnPS)
	{
		pAction = mpObject->CreateSetStringAction(eLightStr_FlickerOnPS, mpInpFlickerOnPS->GetValue8Char());
	}

	// Off Sound
	else if(apInput==mpInpFlickerOffSound)
	{
		pAction = mpObject->CreateSetStringAction(eLightStr_FlickerOffSound, mpInpFlickerOffSound->GetValue8Char());
	}

	// Off PS
	else if(apInput==mpInpFlickerOffPS)
	{
		pAction = mpObject->CreateSetStringAction(eLightStr_FlickerOffPS, mpInpFlickerOffPS->GetValue8Char());
	}

	// Off Radius
	else if(apInput==mpInpFlickerOffRadius)
	{
		pAction = mpObject->CreateSetFloatAction(eLightFloat_FlickerOffRadius, mpInpFlickerOffRadius->GetValue());
	}

	// Off Color
	else if(apInput==mpInpFlickerOffColor)
	{
		pAction = mpObject->CreateSetColorAction(eLightCol_FlickerOff, mpInpFlickerOffColor->GetValue());
	}

	// Flicker Fade active
	else if(apInput==mpInpFlickerFade)
	{
		pAction = mpObject->CreateSetBoolAction(eLightBool_FlickerFade, mpInpFlickerFade->GetValue());
	}

	// Fade On Min Length
	else if(apInput==mpInpFlickerFadeOnMinLength)
	{
		pAction = mpObject->CreateSetFloatAction(eLightFloat_FlickerOnFadeMinLength, mpInpFlickerFadeOnMinLength->GetValue());
	}

	// Fade On Max Length
	else if(apInput==mpInpFlickerFadeOnMaxLength)
	{
		pAction = mpObject->CreateSetFloatAction(eLightFloat_FlickerOnFadeMaxLength, mpInpFlickerFadeOnMaxLength->GetValue());
	}

	// Fade Off Min Length
	else if(apInput==mpInpFlickerFadeOffMinLength)
	{
		pAction = mpObject->CreateSetFloatAction(eLightFloat_FlickerOffFadeMinLength, mpInpFlickerFadeOffMinLength->GetValue());
	}

	// Fade Off Max Length
	else if(apInput==mpInpFlickerFadeOffMaxLength)
	{
		pAction = mpObject->CreateSetFloatAction(eLightFloat_FlickerOffFadeMaxLength, mpInpFlickerFadeOffMaxLength->GetValue());
	}

	////////////////////////////////////////////////////
	// Box
	else if(apInput==mpInpScale)
	{
		pAction = mpObject->CreateSetVector3fAction(eLightBoxVec3f_Size, mpInpScale->GetValue());
	}
	else if(apInput==mpInpBoxBlendFunc)
	{
		pAction = mpObject->CreateSetIntAction(eLightBoxInt_BlendFunc, mpInpBoxBlendFunc->GetValue());
	}

	////////////////////////////////////////////////////
	// Spot

	// FOV
	else if(apInput==mpInpSpotFOV)
	{
		pAction = mpObject->CreateSetFloatAction(eLightSpotFloat_FOV, cMath::ToRad(mpInpSpotFOV->GetValue()));
	}

	// Aspect
	else if(apInput==mpInpSpotAspect)
	{
		pAction = mpObject->CreateSetFloatAction(eLightSpotFloat_Aspect, mpInpSpotAspect->GetValue());
	}

	// Near Clip Plane
	else if(apInput==mpInpSpotNearClipPlane)
	{
		pAction = mpObject->CreateSetFloatAction(eLightSpotFloat_NearClipPlane, mpInpSpotNearClipPlane->GetValue());
	}

	// Spot Falloff Map
	else if(apInput==mpInpSpotFalloffMap)
	{
		strFilename = mpInpFalloffMap->GetValue8Char();
		pAction = mpObject->CreateSetStringAction(eLightSpotStr_FalloffMap, strFilename);
	}
	else
		return iEdScnObjEditPane::WindowSpecificInputCallback(apInput);

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------

