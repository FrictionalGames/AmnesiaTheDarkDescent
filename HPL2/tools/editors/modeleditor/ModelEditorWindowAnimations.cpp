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

#include "ModelEditorWindowAnimations.h"
#include "ModelEditor.h"
#include "ModelEditorWorld.h"

#include <algorithm>

//------------------------------------------------------------------------------------

cModelEditorWindowAnimations::cModelEditorWindowAnimations(cModelEditor* apEditor) : iEditorWindowPopUp(apEditor, "Animation edition window", true, true, false, cVector2f(650,450))
{
	mpEditor = apEditor;
}

//------------------------------------------------------------------------------------

void cModelEditorWindowAnimations::OnSetActive(bool abX)
{
	iEditorWindowPopUp::OnSetActive(abX);
	if(abX)
	{
		mpEditor->SetFlags(eModelEditorFlag_TestWindowActive, false);

		mvTempAnimations = ((cModelEditorWorld*)mpEditor->GetEditorWorld())->GetAnimations();
		mpEditor->SetLayoutNeedsUpdate(true);
	}
}

//------------------------------------------------------------------------------------

void cModelEditorWindowAnimations::OnInitLayout()
{
	iEditorWindowPopUp::OnInitLayout();
	mpWindow->SetText(_W("Edit Animations"));

	//////////////////////////////////////////////////
	// List of anims
	cWidgetLabel* pLabel = mpSet->CreateWidgetLabel(cVector3f(15,40,0.1f),0,_W("Animations"), mpWindow);

	mpListAnimations = mpSet->CreateWidgetListBox(pLabel->GetLocalPosition()+cVector3f(0,25,1),cVector2f(200,270), mpWindow);

	mpListAnimations->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(InputCallback));

	///////////////////////////////////////////
	// Add animation button
	mpBAddAnimation = mpSet->CreateWidgetButton(mpListAnimations->GetLocalPosition() + cVector3f(0,275,0), cVector2f(100,25), _W("Add"), mpWindow);

	mpBAddAnimation->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	///////////////////////////////////////////
	// Remove animation button
	mpBRemAnimation = mpSet->CreateWidgetButton(mpListAnimations->GetLocalPosition() + cVector3f(110,275,0), cVector2f(100,25), _W("Remove"), mpWindow);

	mpBRemAnimation->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	///////////////////////////////////////////
	// OK button
	mpBOK = mpSet->CreateWidgetButton(cVector3f(200,410,0.1f), cVector2f(100,25),_W("OK"), mpWindow);

	mpBOK->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	///////////////////////////////////////////
	// Cancel button
	mpBCancel = mpSet->CreateWidgetButton(cVector3f(330,410,0.1f), cVector2f(100,25),_W("Cancel"), mpWindow);

	mpBCancel->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	cWidgetGroup* pGroupAnim = mpSet->CreateWidgetGroup(cVector3f(240,60,0.1f), cVector2f(400,300), _W("Animation Data"), mpWindow);


	mpInpAnimName = CreateInputString(cVector3f(10,10,0.1f),_W("Name"), "", pGroupAnim);
	mpInpAnimFile = CreateInputFile(mpInpAnimName->GetPosition() + cVector3f(mpInpAnimName->GetSize().x+20,0,0),_W("File"), "", pGroupAnim);
	mpInpAnimFile->SetBrowserType(eEditorResourceType_ModelAnim);
	mpInpAnimSpeed = CreateInputNumber(mpInpAnimName->GetPosition() + cVector3f(0,mpInpAnimName->GetSize().y+10,0),_W("Speed"),"", pGroupAnim);
	mpInpAnimSpecialEventTime = CreateInputNumber(mpInpAnimFile->GetPosition() + cVector3f(0,mpInpAnimFile->GetSize().y+10,0),_W("Special Event Time"), "", pGroupAnim);

	mpBAddEvent = mpSet->CreateWidgetButton();

	mpBAddEvent->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	mpBRemEvent = mpSet->CreateWidgetButton();

	mpBRemEvent->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	mpLEvent = mpSet->CreateWidgetLabel(cVector3f(15,120,0.1f),0,_W("Events"), pGroupAnim);

	mpListEvents = mpSet->CreateWidgetListBox(mpLEvent->GetLocalPosition()+cVector3f(0,25,1),cVector2f(110,100), pGroupAnim);

	mpListEvents->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(InputCallback));

	mpBAddEvent = mpSet->CreateWidgetButton(mpListEvents->GetLocalPosition() + cVector3f(0,110,2), cVector2f(55,25),_W("Add"), pGroupAnim);

	mpBAddEvent->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	mpBRemEvent = mpSet->CreateWidgetButton(mpListEvents->GetLocalPosition() + cVector3f(60,110,2), cVector2f(55,25),_W("Rem"), pGroupAnim);

	mpBRemEvent->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	mpGEvent =  mpSet->CreateWidgetGroup(mpListEvents->GetLocalPosition() + cVector3f(130,0,0.1f), cVector2f(200,130), _W("Event"), pGroupAnim);

	/////////////////////////////////
	// Animation Event Attributes
	mpInpEventTime = CreateInputNumber(cVector3f(15,15,0.1f), _W("Time"),"", mpGEvent); 
	mpInpEventType = CreateInputEnum(mpInpEventTime->GetPosition() + cVector2f(0,mpInpEventTime->GetSize().y), _W("Type"), "", tWStringList(), mpGEvent);
	mpInpEventType->AddValue(_W("PlaySound"));
	mpInpEventType->AddValue(_W("Step"));
	mpInpEventValue = CreateInputString(mpInpEventType->GetPosition() + cVector2f(0,mpInpEventType->GetSize().y), _W("Value"), "", mpGEvent);
}

//------------------------------------------------------------------------------------

void cModelEditorWindowAnimations::OnUpdate(float afTimeStep)
{
	int lSelectedItem = mpListAnimations->GetSelectedItem();
	mpListAnimations->ClearItems();
	for(int i=0;i<(int)mvTempAnimations.size();++i)
		mpListAnimations->AddItem(mvTempAnimations[i].GetName());

	mpListAnimations->SetSelectedItem(lSelectedItem);
}

//------------------------------------------------------------------------------------

bool cModelEditorWindowAnimations::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget==mpBAddAnimation)
	{
		mvTempAnimations.push_back(cAnimationWrapper());
		cAnimationWrapper& anim = mvTempAnimations.back();

		tString sName;
		int i=(int)mvTempAnimations.size();
		do
		{
			sName = "Animation "+cString::ToString(i++);
		}
		while (CheckAnimNameAvailable(sName)==false);

		anim.SetName(sName);

		mpEditor->SetLayoutNeedsUpdate(true);
	}
	else if(apWidget==mpBRemAnimation)
	{
		int lSelectedItem = mpListAnimations->GetSelectedItem();
		if(lSelectedItem!=-1)
		{
			cAnimationWrapper* anim = &mvTempAnimations[lSelectedItem];
			std::vector<cAnimationWrapper> vTemp;
			for(int i=0;i<(int)mvTempAnimations.size();++i)
				if(mvTempAnimations[i].GetName()!=anim->GetName())
					vTemp.push_back(mvTempAnimations[i]);

			mvTempAnimations = vTemp;
			mpEditor->SetLayoutNeedsUpdate(true);
		}
	}
	else if(apWidget==mpBAddEvent)
	{
		int lSelectedAnim = mpListAnimations->GetSelectedItem();

		cAnimationWrapper* pAnim = &mvTempAnimations[lSelectedAnim];

		pAnim->AddEvent();
		mpEditor->SetLayoutNeedsUpdate(true);
	}
	else if(apWidget==mpBRemEvent)
	{
		int lSelectedAnim = mpListAnimations->GetSelectedItem();

		cAnimationWrapper* pAnim = &mvTempAnimations[lSelectedAnim];

		pAnim->RemoveEvent(this->mpListEvents->GetSelectedItem());
		mpEditor->SetLayoutNeedsUpdate(true);
	}
	else if(apWidget==mpBOK)
	{
		bool bAnimationsValid = true;
		tAnimWrapperVec vTemp;
		for(int i=0;i<(int)mvTempAnimations.size();++i)
		{
			cAnimationWrapper& pAnim = mvTempAnimations[i];
			if(pAnim.IsValid())
				vTemp.push_back(pAnim);
			else
			{
				bAnimationsValid = false;
				break;
			}
		}

		if(bAnimationsValid)
		{
			((cModelEditorWorld*)mpEditor->GetEditorWorld())->SetAnimations(vTemp);
			SetActive(false);
		}
		else
			mpEditor->ShowMessageBox(_W("Warning!"), _W("Animations were not saved due to errors (blank fields)"), _W("OK"),_W(""), NULL, NULL);
	}
	else if(apWidget==mpBCancel)
	{
		SetActive(false);
	}
	else if(apWidget==mpListAnimations)
	{
		UpdateAnimInputs();
	}
	else if(apWidget==mpListEvents)
	{
		UpdateEventInputs();
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorWindowAnimations,InputCallback);

//------------------------------------------------------------------------------------

bool cModelEditorWindowAnimations::WindowSpecificInputCallback(iEditorInput* apInput)
{
	int lSelectedItem = mpListAnimations->GetSelectedItem();
	cAnimationWrapper* pAnim = NULL;
	cAnimationEventWrapper* pEvent = NULL;
	if(lSelectedItem!=-1)
	{
		pAnim = &mvTempAnimations[lSelectedItem];
		int lSelectedEvent = mpListEvents->GetSelectedItem();
		pEvent = pAnim->GetEvent(lSelectedEvent);
	}

	if(apInput==mpInpAnimName)
	{
		pAnim->SetName(cString::To8Char(mpInpAnimName->GetValue()));
		mpEditor->SetLayoutNeedsUpdate(true);
	}
	else if(apInput==mpInpAnimFile)
	{
		pAnim->SetFile(cString::To8Char(mpInpAnimFile->GetValue()));		
	}
	else if(apInput==mpInpAnimSpeed)
	{
		pAnim->SetSpeed(mpInpAnimSpeed->GetValue());
	}
	else if(apInput==mpInpAnimSpecialEventTime)
	{
		pAnim->SetSpecialEventTime(mpInpAnimSpecialEventTime->GetValue());
	}
	else if(apInput==mpInpEventTime)
	{
		pEvent->SetTime(mpInpEventTime->GetValue());
	}
	else if(apInput==mpInpEventType)
	{
		pEvent->SetType(cString::To8Char(mpInpEventType->GetEnumValue(mpInpEventType->GetValue())));
	}
	else if(apInput==mpInpEventValue)
	{
		pEvent->SetValue(cString::To8Char(mpInpEventValue->GetValue()));
	}
	

	return true;
}

//------------------------------------------------------------------------------------

bool cModelEditorWindowAnimations::CheckAnimNameAvailable(const tString& asName)
{
	for(int i=0;i<(int)mvTempAnimations.size();++i)
	{
		if(asName==mvTempAnimations[i].GetName())
			return false;
	}

	return true;
}

//------------------------------------------------------------------------------------

void cModelEditorWindowAnimations::UpdateAnimInputs()
{
	int lSelectedItem = mpListAnimations->GetSelectedItem();
	tString sName = "";
	tString sFile = "";
	float fSpeed = 0;
	float fSpecialEventTime = 0;
	bool bValidAnimation = (lSelectedItem!=-1);
	std::vector<cAnimationEventWrapper> vTempEvents;

	if(bValidAnimation)
	{
		cAnimationWrapper* pAnim = &mvTempAnimations[lSelectedItem];
		sName = pAnim->GetName();
		sFile = pAnim->GetFile();
		fSpeed = pAnim->GetSpeed();
		fSpecialEventTime = pAnim->GetSpecialEventTime();

		vTempEvents = pAnim->GetEvents();
	}

	mpInpAnimName->SetValue(cString::To16Char(sName), false);
	mpInpAnimFile->SetValue(cString::To16Char(sFile), false);
	mpInpAnimSpeed->SetValue(fSpeed, false);
	mpInpAnimSpecialEventTime->SetValue(fSpecialEventTime, false);

	mpListEvents->ClearItems();
	for(int i=0;i<(int)vTempEvents.size();++i)
		mpListEvents->AddItem(_W("Event ") + cString::ToStringW(i+1));

	mpBAddEvent->SetEnabled(bValidAnimation);
	mpBRemEvent->SetEnabled(bValidAnimation);

	mpInpAnimName->GetHandle()->SetEnabled(bValidAnimation);
	mpInpAnimFile->GetHandle()->SetEnabled(bValidAnimation);
	mpInpAnimSpeed->GetHandle()->SetEnabled(bValidAnimation);
	mpInpAnimSpecialEventTime->GetHandle()->SetEnabled(bValidAnimation);

	mpLEvent->SetEnabled(bValidAnimation);
	mpListEvents->SetEnabled(bValidAnimation);

	UpdateEventInputs();
}

//------------------------------------------------------------------------------------

void cModelEditorWindowAnimations::UpdateEventInputs()
{
	int lSelectedAnim = mpListAnimations->GetSelectedItem();
	cAnimationWrapper* pAnim =NULL;
	if(lSelectedAnim!=-1)
	pAnim = &mvTempAnimations[mpListAnimations->GetSelectedItem()];
	
	int lSelectedItem = mpListEvents->GetSelectedItem();
	float fEventTime = 0;
	tString sType = "";
	tString sValue = "";
	bool bValidEvent = (pAnim!=NULL && lSelectedItem!=-1);

	if(bValidEvent)
	{
		cAnimationEventWrapper* pEvent = pAnim->GetEvent(lSelectedItem);
		fEventTime = pEvent->GetTime();
		sType = pEvent->GetType();
		sValue = pEvent->GetValue();
	}

	mpInpEventTime->SetValue(fEventTime, false);
	((iEditorInput*)mpInpEventType)->SetValue(cString::To16Char(sType), false);
	mpInpEventValue->SetValue(cString::To16Char(sValue), false);

	mpGEvent->SetEnabled(bValidEvent);
	/*mpInpEventTime->GetHandle()->SetEnabled(bValidEvent);
	mpInpEventType->GetHandle()->SetEnabled(bValidEvent);
	mpInpEventValue->GetHandle()->SetEnabled(bValidEvent);*/
}

//------------------------------------------------------------------------------------

