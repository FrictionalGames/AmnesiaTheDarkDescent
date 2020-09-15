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

#include "../Common/EdScnObjCreatorPane.h"
#include "../Common/EdEditModeScnObjCreator.h"

#include "../Common/Editor.h"
#include "../Common/EdScnObject.h"
#include "../Common/EdScnWorld.h"
#include "../Common/EdSurfacePicker.h"

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// EDIT MODE OBJECT CREATOR PANE - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

iEdScnObjCreatorPane::iEdScnObjCreatorPane(iEdEditMode* apMode) : iEdEditModePane(apMode)
{
	mbRotateUpdated=false;
	mbScaleUpdated=false;

	mpInpBaseName = NULL;

	mpInpSubType = NULL;

	mpGSurface = NULL;
	mpInpCreateOnSurface = NULL;

	mpGRotate = NULL;
	mpInpRandRotate = NULL;
	mpInpMinRotate = NULL;
	mpInpMaxRotate = NULL;

	mpGScale = NULL;
	mpInpRandScale = NULL;
	mpInpMinScale = NULL;
	mpInpMaxScale = NULL;

	mvPos = 0;
	mvRotate = 0;
	mvScale = 1;
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// EDIT MODE OBJECT CREATOR PANE - PUBLIC METHODS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

const cVector3f& iEdScnObjCreatorPane::GetRotate()
{
	UpdateRotate();

	return mvRotate;
}

//---------------------------------------------------------------------

void iEdScnObjCreatorPane::UpdateRotate()
{
	if(mbRotateUpdated==false && (mpGRotate==NULL || mpInpRandRotate->GetValue()==false))
		return;

	mbRotateUpdated=false;

	cVector3f vDegRotate;
	if(mpInpRandRotate->GetValue())
        vDegRotate = cMath::RandRectVector3f(mpInpMinRotate->GetValue(), mpInpMaxRotate->GetValue());
	else
		vDegRotate = mpInpMinRotate->GetValue();

	for(int i=0;i<3;++i)
		mvRotate.v[i] = cMath::ToRad(vDegRotate.v[i]);
}

//---------------------------------------------------------------------

const cVector3f& iEdScnObjCreatorPane::GetScale()
{
	UpdateScale();

	return mvScale;
}

//---------------------------------------------------------------------

void iEdScnObjCreatorPane::UpdateScale()
{
	if(mbScaleUpdated==false && (mpGRotate==NULL || mpInpRandScale->GetValue()==false))
		return;
	
	mbScaleUpdated=false;
	if(mpInpRandScale->GetValue())
		mvScale = cMath::RandRectVector3f(mpInpMinScale->GetValue(), mpInpMaxScale->GetValue());
	else
		mvScale = mpInpMinScale->GetValue();
}

//---------------------------------------------------------------------


//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// EDIT MODE OBJECT CREATOR PANE - PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------

void iEdScnObjCreatorPane::AddNameControls()
{
	if(mpInpBaseName)
		return;

	mpInpBaseName = CreateInputString(0, _W("Base Name"), GetBG());
}

//---------------------------------------------------------------------

void iEdScnObjCreatorPane::AddTypeControls()
{
	if(mpInpSubType)
		return;

	iEdEditModeScnObjCreator* pEditMode = static_cast<iEdEditModeScnObjCreator*>(mpEditMode);
	iEdObjectType* pType = pEditMode->GetMainType();

	if(pType==NULL)
		FatalError("Type for %ls is NULL. And that is bad!", pEditMode->GetName().c_str());

	mpInpSubType = CreateInputEnum(0, pType->GetName(), GetBG());

	for(int i=0; i<pType->GetNumChildren(); ++i)
	{
        iEdObjectType* pSubType = pType->GetChild(i);
		mpInpSubType->AddValue(pSubType->GetName(), pSubType);
	}
}

//---------------------------------------------------------------------

void iEdScnObjCreatorPane::AddCreateOnSurfaceControls(bool abAddAverageNormal)
{
	if(mpGSurface)
		return;

	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpEditor->GetWorld());
	iEdEditModeScnObjCreator* pEditMode = static_cast<iEdEditModeScnObjCreator*>(mpEditMode);
	cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();

	mpGSurface = mpSet->CreateWidgetDummy(0, GetBG()); 
	mpInpCreateOnSurface = CreateInputBool(0, _W("Create on surface"), mpGSurface);
	cVector3f vPos = cVector3f(mpInpCreateOnSurface->GetSize().x+5,0,0.1f);

	const tTypeFilterMap& mapSurfaceTypes = pPicker->GetAffectedTypes();
	tTypeFilterMap::const_iterator it = mapSurfaceTypes.begin();
	for(;it!=mapSurfaceTypes.end(); ++it)
	{
		iEdObjectType* pType = it->first;
		tWString sTypeName = cString::SubW(pType->GetName(), 0, 2);
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, cVector2f(18), sTypeName, mpGSurface);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(SurfaceButton_OnPressed));
		pButton->SetToggleable(true);
		pButton->SetUserData(pType);
		pButton->SetToolTip(pType->GetName());
		vPos.x += pButton->GetSize().x+5;

		pButton->SetPressed(it->second, false);

		mvBSurfaceTypes.push_back(pButton);
	}
}

//---------------------------------------------------------------------

void iEdScnObjCreatorPane::AddRotateControls()
{
	cGuiSet* pSet = mpEditor->GetGuiSet();

	mpGRotate = pSet->CreateWidgetDummy(0, GetBG()); 
	mpInpRandRotate = CreateInputBool(0, _W("Rand. rotate"),  mpGRotate);
	cVector3f vPos = mpInpRandRotate->GetSize().y+5;
	mpInpMinRotate = CreateInputVec3(vPos, _W("Min rotate"),  mpGRotate, 50, tWStringList(), eEdInpStyle_RowLabelOnTop, 15);
	vPos.y+=mpInpMinRotate->GetSize().y+5;
	mpInpMaxRotate = CreateInputVec3(vPos, _W("Max rotate"),  mpGRotate, 50, tWStringList(), eEdInpStyle_RowLabelOnTop, 15);

	mpInpMinRotate->SetValue(0, false);
	mpInpMaxRotate->SetValue(0, false);
	mpInpRandRotate->SetValue(false);

	if(mvShortcuts.empty())
	{
		iWidget* pInputRotateY = mpInpMinRotate->GetInputWidget(1);
		AddShortcut(pInputRotateY->AddShortcut(0, eKey_Q, eGuiMessage_TextBoxValueDown));
		AddShortcut(pInputRotateY->AddShortcut(0, eKey_W, eGuiMessage_TextBoxValueUp));
	}
}

//---------------------------------------------------------------------

void iEdScnObjCreatorPane::AddScaleControls()
{
	mpGScale = mpEditor->GetGuiSet()->CreateWidgetDummy(0, GetBG()); 
	mpInpRandScale = CreateInputBool(0, _W("Rand. scale"),  mpGScale);
	cVector3f vPos = mpInpRandScale->GetSize().y+5;
	mpInpMinScale = CreateInputVec3(vPos, _W("Min scale"),  mpGScale, 50, tWStringList(), eEdInpStyle_RowLabelOnTop, 0.25f);
	vPos.y+=mpInpMinScale->GetSize().y+5;
	mpInpMaxScale = CreateInputVec3(vPos, _W("Max scale"),  mpGScale, 50, tWStringList(), eEdInpStyle_RowLabelOnTop, 0.25f);

	mpInpMinScale->SetValue(1, false);
	mpInpMaxScale->SetValue(1, false);
	mpInpRandScale->SetValue(false);
}

//---------------------------------------------------------------------

bool iEdScnObjCreatorPane::SurfaceButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdEditModeScnObjCreator* pEditMode = static_cast<iEdEditModeScnObjCreator*>(mpEditMode);
	for(size_t i=0;i<mvBSurfaceTypes.size();++i)
	{
		cWidgetButton* pButton = mvBSurfaceTypes[i];
		if(apWidget==pButton)
		{
			iEdObjectType* pType = static_cast<iEdObjectType*>(pButton->GetUserData());
			pEditMode->SetAffectSurfaceType(pType, pButton->IsPressed());
			break;
		}
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEdScnObjCreatorPane, SurfaceButton_OnPressed);

//---------------------------------------------------------------------

bool iEdScnObjCreatorPane::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdEditModeScnObjCreator* pEditMode = static_cast<iEdEditModeScnObjCreator*>(mpEditMode);

	if(apInput==mpInpSubType)
		pEditMode->SetSelectedType(mpInpSubType->GetValue());

	else if(apInput==mpInpBaseName)
		pEditMode->SetBaseName(mpInpBaseName->GetValue());

	else if(apInput==mpInpCreateOnSurface)
		pEditMode->SetCreateOnSurface(mpInpCreateOnSurface->GetValue());

	else if(apInput==mpInpRandRotate)
	{
		mpInpMaxRotate->GetHandle()->SetEnabled(mpInpRandRotate->GetValue());

		pEditMode->UpdateLocalRotation();
	}
	else if(apInput==mpInpMinRotate || apInput==mpInpMaxRotate)
	{
		mbRotateUpdated = true;

		pEditMode->UpdateLocalRotation();
	}
	else if(apInput==mpInpRandScale)
	{
		mpInpMaxScale->GetHandle()->SetEnabled(mpInpRandScale->GetValue());

		pEditMode->UpdateCreatorScale();
	}
	else if(apInput==mpInpMinScale || apInput==mpInpMaxScale)
	{
		mbScaleUpdated = true;

		pEditMode->UpdateCreatorScale();
	}
	else
		return false;

	return true;
}

//---------------------------------------------------------------------

void iEdScnObjCreatorPane::OnReset()
{
	if(mpInpSubType) mpInpSubType->SetValue(0, true, true, true);
}
