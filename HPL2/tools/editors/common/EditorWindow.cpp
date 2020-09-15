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

#include "EditorWindow.h"

#include "EditorBaseClasses.h"
#include "EditorEditMode.h"
#include "EditorInput.h"
#include "SurfacePicker.h"

#include <algorithm>

//--------------------------------------------------------



//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

//////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////

//--------------------------------------------------------

iEditorWindow::iEditorWindow(iEditorBase* apEditor, const tString& asName)
{
	mpSet = apEditor->GetSet();
	mpSkin = apEditor->GetSkin();

	mpBGFrame = NULL;

	mbEnabled = true;
	mbVisible = true;

	mpEditor = apEditor;
	msName = asName;
	mvPos = 0;
}

//--------------------------------------------------------

iEditorWindow::~iEditorWindow()
{
    if (mpBGFrame)
        mpSet->DestroyWidget(mpBGFrame, true);
	STLDeleteAll(mlstInputs);
}

//--------------------------------------------------------

//////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////

//--------------------------------------------------------

void iEditorWindow::SetPosition(const cVector3f &avPosition)
{
	mvPos = avPosition;
	
	if(mpBGFrame)
		mpBGFrame->SetPosition(mvPos);
}

const cVector3f& iEditorWindow::GetPosition()
{
	return mvPos;
}

//--------------------------------------------------------

void iEditorWindow::SetSize(const cVector2f &avSize)
{
	mvSize = avSize;
	if(mpBGFrame)
		mpBGFrame->SetSize(mvSize);

	OnChangeSize();
}

const cVector2f& iEditorWindow::GetSize()
{
	if(mpBGFrame)
		mvSize = mpBGFrame->GetSize();

	return mvSize;
}

//--------------------------------------------------------

void iEditorWindow::SetEnabled(bool abX)
{
	mbEnabled = abX;

	if(mpBGFrame)
		mpBGFrame->SetEnabled(mbEnabled);
}

//--------------------------------------------------------

void iEditorWindow::SetVisible(bool abX)
{
	mbVisible = abX;

	if(mpBGFrame)
		mpBGFrame->SetVisible(mbVisible);
}

//--------------------------------------------------------

void iEditorWindow::SetActive(bool abX)
{
	SetEnabled(abX);
	SetVisible(abX);

	OnSetActive(abX);
}

//--------------------------------------------------------

bool iEditorWindow::IsActive()
{
	return mbEnabled && mbVisible;
}

//--------------------------------------------------------

void iEditorWindow::Init()
{
	OnInit();

	InitLayout();
	PostInitLayout();
}

//--------------------------------------------------------

//////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////

//--------------------------------------------------------

void iEditorWindow::InitLayout()
{
	mpBGFrame = mpSet->CreateWidgetFrame(mvPos, mvSize);
	mpBGFrame->SetText(cString::To16Char(msName));
	mpBGFrame->SetDrawBackground(true);
	mpBGFrame->SetBackGroundColor(cColor(0.82f, 0.81f, 0.79f,1));

	OnInitLayout();
}

//--------------------------------------------------------

bool iEditorWindow::EditorInputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	return WindowSpecificInputCallback((iEditorInput*)apWidget->GetUserData());
}
kGuiCallbackDeclaredFuncEnd(iEditorWindow, EditorInputCallback);

//--------------------------------------------------------

//--------------------------------------------------------

cEditorInputBool* iEditorWindow::CreateInputBool(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent)
{
	cEditorInputBool* pInput = hplNew(cEditorInputBool,(this, avPos, asLabel, asName, apParent));
	SetUpInput(pInput);

	return pInput;
}

//--------------------------------------------------------

cEditorInputEnum* iEditorWindow::CreateInputEnum(const cVector3f& avPos, const tWString& asLabel, const tString& asName, const tWStringList& alstValues, iWidget* apParent, float afBoxWidth)
{
	cEditorInputEnum* pInput = hplNew(cEditorInputEnum,(this, avPos, asLabel, asName, afBoxWidth, apParent));
	tWStringList::const_iterator it = alstValues.begin();
	for(;it!=alstValues.end();++it)
	{
		const tWString& sValue = *it;
		pInput->AddValue(sValue);
	}
	SetUpInput(pInput);

	return pInput;
}

//--------------------------------------------------------

cEditorInputText* iEditorWindow::CreateInputString(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent, float afBoxWidth)
{
	cEditorInputText* pInput = hplNew(cEditorInputText,(this, avPos, asLabel, asName, afBoxWidth, 1, false, false, apParent));
	pInput->SetLayoutStyle(eEditorInputLayoutStyle_ColumnLabelOnTop);
	SetUpInput(pInput);

	return pInput;
}

//--------------------------------------------------------

cEditorInputFile* iEditorWindow::CreateInputFile(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent, float afBoxWidth)
{
	cEditorInputFile* pInput = hplNew(cEditorInputFile,(this, avPos, asLabel, asName, afBoxWidth, apParent));
	pInput->SetLayoutStyle(eEditorInputLayoutStyle_ColumnLabelOnTop);
	SetUpInput(pInput);

	return pInput;
}

//--------------------------------------------------------

cEditorInputNumber* iEditorWindow::CreateInputNumber(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent, float afBoxWidth, float afNumericAdd)
{
	cEditorInputNumber* pInput = hplNew(cEditorInputNumber,(this, avPos, asLabel, asName, afBoxWidth, apParent, afNumericAdd));
	pInput->SetLayoutStyle(eEditorInputLayoutStyle_ColumnLabelOnTop);
	SetUpInput(pInput);

	return pInput;
}

//--------------------------------------------------------

cEditorInputVec2* iEditorWindow::CreateInputVec2(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent, float afBoxWidth, const tWStringList& alstLabels, eEditorInputLayoutStyle aStyle, float afNumericAdd)
{
	cEditorInputVec2* pInput = hplNew(cEditorInputVec2,(this, avPos, asLabel, asName, afBoxWidth, alstLabels, apParent, afNumericAdd));
	pInput->SetLayoutStyle(aStyle);
	SetUpInput(pInput);

	return pInput;
}

//--------------------------------------------------------

cEditorInputVec3* iEditorWindow::CreateInputVec3(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent, float afBoxWidth, const tWStringList& alstLabels, eEditorInputLayoutStyle aStyle, float afNumericAdd)
{
	cEditorInputVec3* pInput = hplNew(cEditorInputVec3,(this, avPos, asLabel, asName, afBoxWidth, alstLabels, apParent,afNumericAdd));
	pInput->SetLayoutStyle(aStyle);
	SetUpInput(pInput);

	return pInput;
}

//--------------------------------------------------------

cEditorInputColorFrame* iEditorWindow::CreateInputColorFrame(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent)
{
	cEditorInputColorFrame* pInput = hplNew(cEditorInputColorFrame,(this, avPos, asLabel, asName, apParent));
	pInput->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
	SetUpInput(pInput);

	return pInput;
}

//--------------------------------------------------------

void iEditorWindow::DestroyInput(iEditorInput* apInput)
{
	if(apInput)
	{
		mlstInputs.remove(apInput);
		hplDelete(apInput);
	}
}

//--------------------------------------------------------

void iEditorWindow::SetUpInput(iEditorInput* apInput)
{
	apInput->UpdateLayout();
	apInput->AddCallback(eEditorInputCallback_ValueEnter, this, kGuiCallback(EditorInputCallback));

	mlstInputs.push_back(apInput);
}

//--------------------------------------------------------
//--------------------------------------------------------

iEditModeWindow::iEditModeWindow(iEditorEditMode* apEditMode) : iEditorWindow(apEditMode->GetEditor(),
																				apEditMode->GetName() + " Edit Mode Window")
{
	mpEditMode = apEditMode;

}


//--------------------------------------------------------

iEditModeObjectCreatorWindow::iEditModeObjectCreatorWindow(iEditorEditMode* apEditMode) : iEditModeWindow(apEditMode)
{
	mvRotate = 0;
	mbRotateUpdated=false;

	mvScale = 1;
	mbScaleUpdated=false;

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

	this->mvPos = 0;
	this->mvRotate = 0;
	this->mvScale = 1;
}

//--------------------------------------------------------

const cVector3f& iEditModeObjectCreatorWindow::GetRotate()
{
	UpdateRotate();

	return mvRotate;
}

void iEditModeObjectCreatorWindow::UpdateRotate()
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

const cVector3f& iEditModeObjectCreatorWindow::GetScale()
{
	UpdateScale();

	return mvScale;
}

void iEditModeObjectCreatorWindow::UpdateScale()
{
	if(mbScaleUpdated==false && (mpGRotate==NULL || mpInpRandScale->GetValue()==false))
		return;
	
	mbScaleUpdated=false;
	if(mpInpRandScale->GetValue())
		mvScale = cMath::RandRectVector3f(mpInpMinScale->GetValue(), mpInpMaxScale->GetValue());
	else
		mvScale = mpInpMinScale->GetValue();
}

//--------------------------------------------------------

void iEditModeObjectCreatorWindow::AddCreateOnSurfaceControls(bool abAddAverageNormal)
{
	if(mpGSurface)
		return;

	iEditorEditModeObjectCreator* pEditMode = (iEditorEditModeObjectCreator*)mpEditMode;

	mpGSurface = mpSet->CreateWidgetDummy(0, mpBGFrame); 
	mpInpCreateOnSurface = CreateInputBool(0, _W("Create on surface"), "", mpGSurface);
	cVector3f vPos = cVector3f(mpInpCreateOnSurface->GetSize().x+5,0,0.1f);
	tWString vStrings[] = { _W("StaticObjects"), _W("Primitives"), _W("Entities") };
	for(int i=0;i<eSurfaceType_LastEnum;++i)
	{
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, cVector2f(18), _W(""), mpGSurface);
		pButton->SetToggleable(true);
		pButton->SetUserValue(i);
		pButton->SetText(cString::SubW(vStrings[i], 0, 2));
		pButton->SetToolTip(vStrings[i]);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(SurfaceButton_OnPressed));
		pButton->SetPressed(pEditMode->GetAffectSurfaceType(i), false);

		vPos.x += pButton->GetSize().x+5;

		mvBSurfaceTypes.push_back(pButton);
	}
}

void iEditModeObjectCreatorWindow::AddRotateControls()
{
	mpGRotate = mpSet->CreateWidgetDummy(0, mpBGFrame); 
	mpInpRandRotate = CreateInputBool(0, _W("Rand. rotate"), "", mpGRotate);
	cVector3f vPos = mpInpRandRotate->GetSize().y+5;
	mpInpMinRotate = CreateInputVec3(vPos, _W("Min rotate"), "", mpGRotate, 50, tWStringList(), eEditorInputLayoutStyle_RowLabelOnTop, 15);
	vPos.y+=mpInpMinRotate->GetSize().y+5;
	mpInpMaxRotate = CreateInputVec3(vPos, _W("Max rotate"), "", mpGRotate, 50, tWStringList(), eEditorInputLayoutStyle_RowLabelOnTop, 15);

	mpInpMinRotate->SetValue(0, false);
	mpInpMaxRotate->SetValue(0, false);
	mpInpRandRotate->SetValue(false);

	if(mvShortcuts.empty())
	{
		mvShortcuts.push_back(mpSet->AddGlobalShortcut(0, eKey_Q, mpInpMinRotate->GetInputWidget(1), eGuiMessage_TextBoxValueDown));
		mvShortcuts.push_back(mpSet->AddGlobalShortcut(0, eKey_W, mpInpMinRotate->GetInputWidget(1), eGuiMessage_TextBoxValueUp));
	}
}

void iEditModeObjectCreatorWindow::AddScaleControls()
{
	mpGScale = mpSet->CreateWidgetDummy(0, mpBGFrame); 
	mpInpRandScale = CreateInputBool(0, _W("Rand. scale"), "", mpGScale);
	cVector3f vPos = mpInpRandScale->GetSize().y+5;
	mpInpMinScale = CreateInputVec3(vPos, _W("Min scale"), "", mpGScale, 50, tWStringList(), eEditorInputLayoutStyle_RowLabelOnTop, 0.25f);
	vPos.y+=mpInpMinScale->GetSize().y+5;
	mpInpMaxScale = CreateInputVec3(vPos, _W("Max scale"), "", mpGScale, 50, tWStringList(), eEditorInputLayoutStyle_RowLabelOnTop, 0.25f);

	mpInpMinScale->SetValue(1, false);
	mpInpMaxScale->SetValue(1, false);
	mpInpRandScale->SetValue(false);
}

void iEditModeObjectCreatorWindow::OnSetActive(bool abX)
{
	for(int i=0;i<(int)mvShortcuts.size();++i)
	{
		mvShortcuts[i]->SetEnabled(abX);
	}
}

bool iEditModeObjectCreatorWindow::SurfaceButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEditorEditModeObjectCreator* pEditMode = (iEditorEditModeObjectCreator*)mpEditMode;
	for(int i=0;i<(int)mvBSurfaceTypes.size();++i)
	{
		cWidgetButton* pButton = mvBSurfaceTypes[i];
		if(apWidget==pButton)
		{
			pEditMode->SetAffectSurfaceType(pButton->GetUserValue(), pButton->IsPressed());
			break;
		}
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditModeObjectCreatorWindow, SurfaceButton_OnPressed);


bool iEditModeObjectCreatorWindow::WindowSpecificInputCallback(iEditorInput* apInput)
{
	iEditorEditModeObjectCreator* pEditMode = (iEditorEditModeObjectCreator*)mpEditMode;
	if(apInput==mpInpCreateOnSurface)
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

//--------------------------------------------------------

//////////////////////////////////////////////////////////////
// POP UP
//////////////////////////////////////////////////////////////

//--------------------------------------------------------

iEditorWindowPopUp::iEditorWindowPopUp(iEditorBase* apEditor, 
									   const tString& asName, 
									   bool abGetEditorAttention, bool abGetGuiAttention, bool abDestroyOnDisable,
									   const cVector2f& avSize) : iEditorWindow(apEditor, asName)
{
	mvSize = avSize;
	mbGetEditorAttention = abGetEditorAttention;
	mbGetGuiAttention = abGetGuiAttention;
	mbDestroyOnDisable = abDestroyOnDisable;
}

//--------------------------------------------------------

void iEditorWindowPopUp::OnInitLayout()
{
	mpBGFrame->SetClipActive(false);
	mpBGFrame->SetSize(0);

	mpWindow = mpSet->CreateWidgetWindow(eWidgetWindowButtonFlag_ButtonClose, 0, mvSize,_W(""), mpBGFrame);
	mpWindow->AddCallback(eGuiMessage_WindowClose, this, kGuiCallback(Window_OnClose));
	mpWindow->SetCloseButtonDisablesWindow(false);
	mpWindow->SetEscapeKeyClosesWindow(true);
}
//--------------------------------------------------------

void iEditorWindowPopUp::OnSetActive(bool abX)
{
	mpEditor->SetFlags(eEditorFlag_PopUpActive, abX);

	if(abX)
	{
		cVector3f vPos = cVector3f(mpSet->GetVirtualSize() - mvSize)*0.5f;
		SetPosition(vPos);
		mpWindow->SetPosition(0);

		if(mbGetEditorAttention) 
		{
		}
		if(mbGetGuiAttention) 
		{
			mpPrevAttention = mpSet->GetAttentionWidget();
			mpSet->SetAttentionWidget(mpBGFrame);
		}

		mpSet->SetFocusedWidget(mpWindow);
	}
	else
	{
		if(mbGetEditorAttention)
		{
		}
		if(mbGetGuiAttention) 
		{
			if(mpSet->GetAttentionWidget()==mpBGFrame) mpSet->SetAttentionWidget(mpPrevAttention);
		}
		if(mbDestroyOnDisable)
			mpEditor->DestroyPopUpWindow(this);
	}
}

//--------------------------------------------------------

bool iEditorWindowPopUp::Window_OnClose(iWidget* apWidget, const cGuiMessageData& aData)
{
	SetActive(false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorWindowPopUp,Window_OnClose);

//--------------------------------------------------------
