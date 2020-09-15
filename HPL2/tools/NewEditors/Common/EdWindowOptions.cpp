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

#include "../Common/EdWindowOptions.h"

#include "../Common/Editor.h"
#include "../Common/EdWorld.h"
#include "../Common/EdWindowViewport.h"
#include "../Common/EdActionHandler.h"
#include "../Common/EdSelection.h"
#include "../Common/EdEngineObject.h"

//-----------------------------------------------------------------

iEdWindowOptions::iEdWindowOptions(iEditor* apEditor) : iEdPopUp(apEditor, _W("Preferences"))//, false, false, cVector2f(640,240))
{
}

//-----------------------------------------------------------------

void iEdWindowOptions::OnCreateLayout()
{
	iEdPopUp::OnCreateLayout();
	SetSize(cVector2f(640, 240));
	GetBG()->SetText(_W("Preferences"));

	cWidgetTabFrame* pFrame = mpSet->CreateWidgetTabFrame(cVector3f(10,35,0.1f), GetBG()->GetSize()-cVector2f(20,45), _W(""), GetBG());
	cWidgetTab* pTab = NULL;

	cVector3f vPos;

	/////////////////////////////////////////////////////////
	// General options
	{
		tWStringList lstLabels;
		lstLabels.push_back(_W("X"));
		lstLabels.push_back(_W("Y"));

		pTab = pFrame->AddTab(_W("General"));
		vPos = cVector3f(15,15,0.1f);

		// Resolution
		mpInpResolution = CreateInputVec2(vPos-cVector3f(0,3,0), _W("Resolution"),  pTab, 50,lstLabels, eEdInpStyle_RowLabelOnLeft);
		mpInpResolution->SetLowerBound(true, 640);

		// Undo stack
		vPos.x += mpInpResolution->GetSize().x + 15;
		mpInpUndoStackSize = CreateInputNumber(vPos-cVector3f(0,3,0), _W("Undo stack size"),  pTab, 50, 1);
		mpInpUndoStackSize->SetStyle(eEdInpStyle_RowLabelOnLeft);
		mpInpUndoStackSize->UpdateLayout();
		mpInpUndoStackSize->SetLowerBound(true, 0);

		// BG color
		vPos.x += mpInpUndoStackSize->GetSize().x + 15;
		mpInpBackgroundColor = CreateInputColor(vPos, _W("BG color"),  pTab);

		// Disabled mesh coverage
		vPos.x = 15;
		vPos.y += mpInpBackgroundColor->GetSize().y + 15;
		mpInpDisabledCoverage = CreateInputNumber(vPos, _W("Disabled mesh coverage"),  pTab, 50, 0.1f);
		mpInpDisabledCoverage->SetStyle(eEdInpStyle_RowLabelOnLeft);
		mpInpDisabledCoverage->UpdateLayout();
		mpInpDisabledCoverage->SetLowerBound(true, 0);
		mpInpDisabledCoverage->SetUpperBound(true, 1);
	}

	/////////////////////////////////////////////////////////
	// Performance options
	{
		iEdWorld* pWorld = mpEditor->GetWorld();

		pTab = pFrame->AddTab(_W("Performance"));
		vPos = cVector3f(15,15,0.1f);

		// Lights active
		mpInpLightsActive = CreateInputBool(vPos, _W("Lights Active"),  pTab);
		mpInpLightsActive->SetUserData(pWorld->GetTypeByName(_W("Light")));
		vPos.y += mpInpLightsActive->GetSize().y + 10;

		// Particles active
		mpInpPSActive = CreateInputBool(vPos, _W("Particle Systems Active"), pTab);
		mpInpPSActive->SetUserData(pWorld->GetTypeByName(_W("Particle System")));
		vPos.y += mpInpPSActive->GetSize().y + 10;
	}

	// World reflection
	mpInpWorldReflection = CreateInputBool(vPos, _W("World Reflection"),  pTab);
	vPos.y += mpInpWorldReflection->GetSize().y + 15;

	tWStringList lstPlaneLabels;
	lstPlaneLabels.push_back(_W("Near"));
	lstPlaneLabels.push_back(_W("Far"));
	mpInpCamPlanes = CreateInputVec2(vPos, _W("Camera Clip Planes:"),  pTab, 50, lstPlaneLabels, eEdInpStyle_RowLabelOnLeft, 1);
	
	vPos.y += mpInpCamPlanes->GetSize().y + 15;
	mpInpShowSkybox = CreateInputBool(vPos, _W("Show Skybox"),  pTab);
	vPos.y += mpInpShowSkybox->GetSize().y + 10;
	mpInpShowFog = CreateInputBool(vPos, _W("Show Fog"),  pTab);

	vPos.x = mpInpLightsActive->GetPosition().x + mpInpLightsActive->GetSize().x + 100;
	vPos.y = 15;
	mpInpTextureQuality = CreateInputEnum(vPos, _W("Texture Quality"), pTab);
	mpInpTextureQuality->SetStyle(eEdInpStyle_RowLabelOnLeft);
	mpInpTextureQuality->UpdateLayout();
	mpInpTextureQuality->AddValue(_W("High"));
	mpInpTextureQuality->AddValue(_W("Medium"));
	mpInpTextureQuality->AddValue(_W("Low"));

	/////////////////////////////////////////////////////////
	// Input options
	pTab = pFrame->AddTab(_W("Input"));
	vPos = cVector3f(15,15,0.1f);

	mpInpTumbleFactor = CreateInputNumber(vPos, _W("Tumble factor"),  pTab, 75, 0.001f);
	vPos.x += mpInpTumbleFactor->GetSize().x + 15;

	mpInpTrackFactor = CreateInputNumber(vPos, _W("Track factor(non LT cam)"),  pTab, 75, 0.01f);
	vPos.x += mpInpTrackFactor->GetSize().x + 15;

	mpInpZoomFactor = CreateInputNumber(vPos, _W("Zoom factor"),  pTab, 75, 0.001f);
	vPos.x += mpInpZoomFactor->GetSize().x + 15;

	mpInpMouseWheelZoom = CreateInputNumber(vPos, _W("Mouse wheel zoom"),  pTab, 75, 0.01f);
	vPos.x = 15;

	vPos.y += mpInpTumbleFactor->GetSize().y + 10;

	mpInpRotateSnap = CreateInputNumber(vPos, _W("Rotate snap"),  pTab, 50, 1);
	mpInpRotateSnap->SetLowerBound(true,0);
	mpInpRotateSnap->SetDecimals(3);

	vPos.x += mpInpRotateSnap->GetSize().x + 15;

	mpInpScaleSnap = CreateInputNumber(vPos, _W("Scale snap"),  pTab, 50, 0.25f);
	mpInpScaleSnap->SetLowerBound(true,0);
	mpInpRotateSnap->SetDecimals(3);
}

//-----------------------------------------------------------------

/*
void cEdWindowOptions::PostInitLayout()
{
	OnUpdate(0);
}
*/

//-----------------------------------------------------------------

void iEdWindowOptions::OnUpdate()
{
	iEdWorld* pWorld = mpEditor->GetWorld();

	cVector2f vScreenSize = mpSet->GetVirtualSize();

	mpInpResolution->SetValue(mpSet->GetVirtualSize(), false);
	mpInpBackgroundColor->SetValue(pWorld->GetBGColor(), false);
	mpInpDisabledCoverage->SetValue(iEngineMesh::GetDisabledCoverage(), false);
	//mpInpUndoStackSize->SetValue((float)mpEditor->GetActionHandler()->GetMaxUndoSize(), false);

	cEdInputBool* pTypeActive[] = { mpInpLightsActive, mpInpPSActive };
	for(int i=0;i<2;++i)
	{
		cEdInputBool* pInput = pTypeActive[i];
		iEdScnObjType* pType = static_cast<iEdScnObjType*>(pInput->GetUserData());

		pInput->SetValue(pType->IsEnabled(), false);
	}

	/*
	{
		mpInpWorldReflection->SetValue(mpEditor->GetWorldReflectionActive(), false);
		mpInpShowSkybox->SetValue(pWorld->GetShowSkybox(), false);
		mpInpShowFog->SetValue(pWorld->GetShowFog(), false);

		mpInpTextureQuality->SetValue(cString::ToInt(mpEditor->GetSetting("TexQuality").c_str(), 0), false);
	}
	*/

	mpInpCamPlanes->SetValue(cEdWindowViewport::GetCamPlanes(), false);

	/*
	mpInpTumbleFactor->SetValue(cString::ToFloat(mpEditor->GetSetting("TumbleFactor").c_str(), 0.005f), false);
	mpInpTrackFactor->SetValue(cString::ToFloat(mpEditor->GetSetting("TrackFactor").c_str(), 0.01f), false);
	mpInpZoomFactor->SetValue(cString::ToFloat(mpEditor->GetSetting("ZoomFactor").c_str(), 0.001f), false);
	mpInpMouseWheelZoom->SetValue(cString::ToFloat(mpEditor->GetSetting("MouseWheelZoom").c_str(), 0.1f), false);
	mpInpRotateSnap->SetValue(cMath::ToDeg(cEdSelection::GetRotateSnap()), false);
	mpInpScaleSnap->SetValue(cEdSelection::GetScaleSnap(), false);
	*/
}

//-----------------------------------------------------------------

bool iEdWindowOptions::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdWorld* pWorld = mpEditor->GetWorld();
	if(apInput==mpInpResolution)
	{
		cVector2f vScreenSize = mpInpResolution->GetValue();
		//mpEditor->mpConfigHandler->mlScreenWidth = vScreenSize.x;
		//mpEditor->mpConfigHandler->mlScreenHeight = vScreenSize.y;
		//mpEditor->SetSettingValue("ScreenWidth", cString::ToString(vScreenSize.x));
		//mpEditor->SetSettingValue("ScreenHeight", cString::ToString(vScreenSize.y));
	}

	//else if(apInput==mpInpBackgroundColor)
	//	pWorld->SetBGDefaultColor(mpInpBackgroundColor->GetValue());

	else if(apInput==mpInpDisabledCoverage)
	{
		//iEngineObjectMesh::SetDisabledCoverage(mpInpDisabledCoverage->GetValue());
		//pWorld->SetVisibilityUpdated();
		//pWorld->UpdateVisibility();		
	}

	else if(apInput==mpInpUndoStackSize)
		mpEditor->GetActionHandler()->SetMaxUndoStackSize((size_t)mpInpUndoStackSize->GetValue());

	else if(apInput==mpInpLightsActive || apInput==mpInpPSActive)
	{
		cEdInputBool* pBool = static_cast<cEdInputBool*>(apInput);
		iEdScnObjType* pType = static_cast<iEdScnObjType*>(pBool->GetUserData());
		pType->SetEnabled(pBool->GetValue());
	}

	/*
	else if(apInput==mpInpWorldReflection)
		mpEditor->SetWorldReflectionActive(mpInpWorldReflection->GetValue());
	*/

	else if(apInput==mpInpCamPlanes)
		cEdWindowViewport::SetCamPlanes(mpInpCamPlanes->GetValue());

	/*
	else if(apInput==mpInpShowFog)
		pWorld->SetShowFog(mpInpShowFog->GetValue());

	else if(apInput==mpInpShowSkybox)
		pWorld->SetShowSkybox(mpInpShowSkybox->GetValue());
		*/

	/*
	else if(apInput==mpInpTextureQuality)
		mpEditor->SetSettingValue("TexQuality", cString::ToString(mpInpTextureQuality->GetValue()));

	else if(apInput==mpInpTumbleFactor)
		//mpEditor->SetSettingValue("TumbleFactor", cString::ToString(mpInpTumbleFactor->GetValue()));

	else if(apInput==mpInpTrackFactor)
		//mpEditor->SetSettingValue("TrackFactor", cString::ToString(mpInpTrackFactor->GetValue()));

	else if(apInput==mpInpZoomFactor)
		mpEditor->SetSettingValue("ZoomFactor", cString::ToString(mpInpZoomFactor->GetValue()));

	else if(apInput==mpInpMouseWheelZoom)
		mpEditor->SetSettingValue("MouseWheelZoom", cString::ToString(mpInpMouseWheelZoom->GetValue()));

	else if(apInput==mpInpRotateSnap)
		cEdSelection::SetRotateSnap(cMath::ToRad(mpInpRotateSnap->GetValue()));

	else if(apInput==mpInpScaleSnap)
		cEdSelection::SetScaleSnap(mpInpScaleSnap->GetValue());
	*/
	

	//tViewportVec& vViewports = mpEditor->GetViewports();
	//for(int i=0;i<(int)vViewports.size();++i)
	//{
	//	cEdWindowViewport* pViewport = vViewports[i];
	//	pViewport->GetVCamera()->FetchSettings();
	//}

	//mpEditor->SetLayoutNeedsUpdate(true);

	return true;		
}

//-----------------------------------------------------------------
