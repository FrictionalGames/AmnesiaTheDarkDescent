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

#include "../LevelEditor/EdLowerToolbarLevel.h"

#include "../Common/Editor.h"

#include "../Common/EdActionHandler.h"

#include "../Common/EdWindowViewport.h"
#include "../Common/EdSelection.h"
#include "../Common/EdGrid.h"
#include "../Common/EdClipPlane.h"
#include "../LevelEditor/HPLMap.h"
//#include "../Common/EditorActionMisc.h"

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

cEdLowerToolbarLevel::cEdLowerToolbarLevel(iEditor* apEditor) : iEdPaneLowerToolbar(apEditor)
{	
	mpGClipPlanes = NULL;

	mbClipPlaneAdded = false;
	mbClipPlaneRemoved = false;
}

//---------------------------------------------------------------

cEdLowerToolbarLevel::~cEdLowerToolbarLevel()
{
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

void cEdLowerToolbarLevel::OnCreateLayout()
{
	iEdPaneLowerToolbar::OnCreateLayout();

	cVector3f vPos = cVector3f(5,5,0.1f);

	iWidget* pHandle = AddGridControls();
	pHandle->SetPosition(vPos);

	vPos.x += pHandle->GetSize().x;
	
	pHandle = this->AddLightingControls();
	pHandle->SetPosition(vPos);
	
	vPos.x += pHandle->GetSize().x;

	pHandle = AddViewportControls();
	pHandle->SetPosition(vPos);

	vPos.x += pHandle->GetSize().x;

	pHandle = AddClipPlaneControls();
	pHandle->SetPosition(vPos);
	//////////////////////////////
	// Set up layout
	//mpEditor->GetWindowHandler()->GetLayoutBlock(eLayoutBlock_ViewportArea);
	//SetSize(cVector2f(mpEditor->GetLayoutVec2f(eLayoutVec2_ViewportAreaSize).x-4,50));
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

iWidget* cEdLowerToolbarLevel::AddClipPlaneControls()
{
	mpGClipPlanes = mpSet->CreateWidgetGroup(0, cVector2f(275,43), _W("Clip planes"), GetBG());
	mpGClipPlanes->SetDefaultFontSize(iEdInput::GetFontSize());

	mpInpClipPlanes = CreateInputEnum(cVector3f(5,0,0.1f), _W(""), mpGClipPlanes,50);
	mpInpClipPlanes->SetMaxShownItems(3);
	mpBAddClipPlane = mpSet->CreateWidgetButton(cVector3f(60, 5, 0.1f), cVector2f(15), _W("+"), mpGClipPlanes);

	mpBAddClipPlane->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
	mpBRemClipPlane = mpSet->CreateWidgetButton(cVector3f(60, 21, 0.1f), cVector2f(15), _W("-"), mpGClipPlanes);
	mpBRemClipPlane->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	// Buttons
	mpBClipNormalSwitch = mpSet->CreateWidgetButton(cVector3f(100,10,0.1f), 25, _W(""), mpGClipPlanes);
	mpBClipNormalSwitch->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
	mpBClipNormalSwitch->SetToolTip(_W("Cycle through clip plane alignments"));
	mpBClipNormalSwitch->SetToolTipEnabled(true);

	mpInpClipPlaneHeight = CreateInputNumber(cVector3f(130,5,0.1f), _W("Height"),  mpGClipPlanes, 50, 0.25f);
	mpInpClipPlaneHeight->GetInputWidget()->AddShortcut(0, eKey_O, eGuiMessage_TextBoxValueUp);
	mpInpClipPlaneHeight->GetInputWidget()->AddShortcut(0, eKey_L, eGuiMessage_TextBoxValueDown);

	mpBClipPlaneCullSide = mpSet->CreateWidgetButton(cVector3f(185,5,0.1f), 25, _W(""), mpGClipPlanes);
	mpBClipPlaneCullSide->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
	mpBClipPlaneCullSide->SetToolTip(_W("Set culled plane side"));
	mpBClipPlaneCullSide->SetToolTipEnabled(true);

	mpInpClipPlaneActive = CreateInputBool(cVector3f(215,5,0.1f), _W("Active"),  mpGClipPlanes);

	return mpGClipPlanes;
}

//---------------------------------------------------------------

void cEdLowerToolbarLevel::SetFocusedClipPlane(int alX)
{
	if(mpInpClipPlanes)
		mpInpClipPlanes->SetValue(alX);
}

//---------------------------------------------------------------

int cEdLowerToolbarLevel::GetFocusedClipPlane()
{
	if(mpInpClipPlanes)
		return mpInpClipPlanes->GetValue();

	return -1;
}

//---------------------------------------------------------------

//---------------------------------------------------------------

void cEdLowerToolbarLevel::OnUpdate()
{
	iEdPaneLowerToolbar::OnUpdate();

	cHplMap* pMap = static_cast<cHplMap*>(mpEditor->GetWorld());

	if(mpGClipPlanes)
	{
		cEdClipPlane* pSelectedPlane = static_cast<cEdClipPlane*>(mpInpClipPlanes->GetSelectedUserData());

		if(mbClipPlaneAdded || mbClipPlaneRemoved)
		{
			int lSelectedItem = -1;
			int lLastSelected = mpInpClipPlanes->GetValue();

			mpInpClipPlanes->ClearValues();

			const tClipPlaneMap& mapClipPlanes = pMap->GetClipPlanes();
			tClipPlaneMap::const_iterator it = mapClipPlanes.begin();
			for(;it!=mapClipPlanes.end(); ++it)
			{
				cEdClipPlane* pPlane = it->second;

				int lItemID = mpInpClipPlanes->AddValue(cString::ToStringW(pPlane->GetID()), pPlane);
				if(pPlane==pSelectedPlane)
					lSelectedItem = lItemID;
			}
			if(mbClipPlaneAdded)
			{
				lSelectedItem = mpInpClipPlanes->GetNumValues()-1;
			}
			else if(mbClipPlaneRemoved && lSelectedItem>0)
			{
				lSelectedItem--;
			}

			mpInpClipPlanes->SetValue(lSelectedItem, false);

			mbClipPlaneAdded = false;
			mbClipPlaneRemoved = false;
		}

		UpdateClipPlaneControls();
	}
}

//---------------------------------------------------------------

void cEdLowerToolbarLevel::OnReset()
{
	iEdPaneLowerToolbar::OnReset();

	mbClipPlaneRemoved = true;
}

void cEdLowerToolbarLevel::OnWorldParamsChange()
{
	iEdPaneLowerToolbar::OnWorldParamsChange();
	cHplMap* pMap = static_cast<cHplMap*>(mpEditor->GetWorld());
	if(pMap->ClipPlaneNumChanged())
		mbClipPlaneAdded = true;
}

//---------------------------------------------------------------

bool cEdLowerToolbarLevel::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdWorld* pWorld = mpEditor->GetWorld();

	iEdAction* pAction = NULL;

	cEdClipPlane* pClipPlane = static_cast<cEdClipPlane*>(mpInpClipPlanes->GetSelectedUserData());
	
	if(apInput==mpInpClipPlanes)
	{
		UpdateClipPlaneControls();
	}
	else if(apInput==mpInpClipPlaneHeight)
	{
		if(pClipPlane)
		{
			cVector3f vHeight = pClipPlane->GetHeights();
			vHeight.v[pClipPlane->GetPlaneNormal()] = mpInpClipPlaneHeight->GetValue();
			
			pAction = pClipPlane->CreateSetHeightsAction(vHeight);
		}
	}
	else if(apInput==mpInpClipPlaneActive)
	{
		if(pClipPlane)
		{
			pAction = pClipPlane->CreateSetActiveAction(mpInpClipPlaneActive->GetValue());
		}
	}
	else
	{
		return iEdPaneLowerToolbar::WindowSpecificInputCallback(apInput);
	}

	mpEditor->AddAction(pAction);

	return true;
}

bool cEdLowerToolbarLevel::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(iEdPaneLowerToolbar::InputCallback(apWidget, aData)==true)
		return true;

	cHplMap* pMap = static_cast<cHplMap*>(mpEditor->GetWorld());

	cEdClipPlane* pSelectedClipPlane = NULL;
	if(mpGClipPlanes)
		pSelectedClipPlane = static_cast<cEdClipPlane*>(mpInpClipPlanes->GetSelectedUserData());

	iEdAction* pAction = NULL;

	if(apWidget==mpBAddClipPlane)
	{
		mbClipPlaneAdded = true;
		pAction = pMap->CreateAddClipPlaneAction();
	}
	else if(apWidget==mpBRemClipPlane)
	{
		if(pSelectedClipPlane)
		{
			mbClipPlaneRemoved = true;
			pAction = pMap->CreateRemoveClipPlaneAction(pSelectedClipPlane->GetID());
		}
	}

	else if(apWidget==mpBClipNormalSwitch)
	{
		if(pSelectedClipPlane)
		{
			int currentPlane = (int)pSelectedClipPlane->GetPlaneNormal();
			++currentPlane;

			pAction = pSelectedClipPlane->CreateSetNormalAction(currentPlane);
		}
	}
	else if(apWidget==mpBClipPlaneCullSide)
	{
		if(pSelectedClipPlane)
		{
			bool bPositiveCulling = pSelectedClipPlane->GetCullingOnPositiveSide();
			pAction = pSelectedClipPlane->CreateSetCullSideAction(!bPositiveCulling);
		}
	}
	
	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdLowerToolbarLevel,InputCallback);

//---------------------------------------------------------------

void cEdLowerToolbarLevel::UpdateClipPlaneControls()
{
	if(mpGClipPlanes)
	{
		cEdClipPlane* pPlane = static_cast<cEdClipPlane*>(mpInpClipPlanes->GetSelectedUserData());

		bool bEnabled = pPlane!=NULL;
		mpInpClipPlaneHeight->GetHandle()->SetEnabled(bEnabled);
		mpBClipPlaneCullSide->SetEnabled(bEnabled);
		mpBClipNormalSwitch->SetEnabled(bEnabled);
		mpInpClipPlaneActive->GetHandle()->SetEnabled(bEnabled);
		
		if(pPlane)
		{
			mpInpClipPlaneHeight->SetValue(pPlane->GetHeight(),false);
			mpBClipNormalSwitch->SetText(pPlane->GetPlaneString());
			mpBClipPlaneCullSide->SetText(pPlane->GetCullingOnPositiveSide()?_W("Pos"):_W("Neg"));
			mpBClipNormalSwitch->SetText(pPlane->GetPlaneString());
			mpInpClipPlaneActive->SetValue(pPlane->IsActive(), false);
		}
	}

}

//---------------------------------------------------------------

//---------------------------------------------------------------
