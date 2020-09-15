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

#include "../ModelEditor/EdWindowAnimations.h"
#include "../ModelEditor/ModelEditor.h"
#include "../ModelEditor/HPLEntity.h"

#include <algorithm>

//------------------------------------------------------------------------------------

cEdWindowAnimations::cEdWindowAnimations(iEditor* apEditor) : iEdPopUp(apEditor, _W("Animations"))
{
}

//------------------------------------------------------------------------------------

void cEdWindowAnimations::OnSetActive(bool abX)
{
	iEdPopUp::OnSetActive(abX);
	if(abX)
	{
		cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
		const tAnimWrapperVec& vAnimations = pEnt->GetAnimations();

		mlstTempAnimations.clear();
		mlstTempAnimations.insert(mlstTempAnimations.end(), 
									vAnimations.begin(), vAnimations.end());
	}
}

//------------------------------------------------------------------------------------

void cEdWindowAnimations::OnCreateLayout()
{
	iEdPopUp::OnCreateLayout();
	GetBG()->SetText(_W("Edit Animations"));
	GetBG()->SetSize(cVector2f(650,460));

	//////////////////////////////////////////////////
	// List of anims
	cWidgetLabel* pLabel = mpSet->CreateWidgetLabel(cVector3f(15,40,0.1f),0,_W("Animations"), GetBG());

	mpListAnimations = mpSet->CreateWidgetListBox(pLabel->GetLocalPosition()+cVector3f(0,25,1),cVector2f(200,270), GetBG());
	mpListAnimations->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(InputCallback));

	///////////////////////////////////////////
	// Add animation button
	mpBAddAnimation = mpSet->CreateWidgetButton(mpListAnimations->GetLocalPosition() + cVector3f(0,275,0), cVector2f(100,25), _W("Add"), GetBG());

	mpBAddAnimation->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	///////////////////////////////////////////
	// Remove animation button
	mpBRemAnimation = mpSet->CreateWidgetButton(mpListAnimations->GetLocalPosition() + cVector3f(110,275,0), cVector2f(100,25), _W("Remove"), GetBG());

	mpBRemAnimation->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	///////////////////////////////////////////
	// OK button
	mpBOK = mpSet->CreateWidgetButton(cVector3f(200,410,0.1f), cVector2f(100,25),_W("OK"), GetBG());

	mpBOK->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	///////////////////////////////////////////
	// Cancel button
	mpBCancel = mpSet->CreateWidgetButton(cVector3f(330,410,0.1f), cVector2f(100,25),_W("Cancel"), GetBG());

	mpBCancel->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(InputCallback));

	cWidgetGroup* pGroupAnim = mpSet->CreateWidgetGroup(cVector3f(240,60,0.1f), cVector2f(400,300), _W("Animation Data"), GetBG());


	mpInpAnimName = CreateInputString(cVector3f(10,10,0.1f),_W("Name"), pGroupAnim);
	mpInpAnimFile = CreateInputAnim(mpInpAnimName->GetPosition() + cVector3f(mpInpAnimName->GetSize().x+20,0,0),_W("File"), pGroupAnim);

	mpInpAnimSpeed = CreateInputNumber(mpInpAnimName->GetPosition() + cVector3f(0,mpInpAnimName->GetSize().y+10,0),_W("Speed"), pGroupAnim);
	mpInpAnimSpecialEventTime = CreateInputNumber(mpInpAnimFile->GetPosition() + cVector3f(0,mpInpAnimFile->GetSize().y+10,0),_W("Special Event Time"), pGroupAnim);

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
	mpInpEventTime = CreateInputNumber(cVector3f(15,15,0.1f), _W("Time"), mpGEvent); 
	mpInpEventType = CreateInputEnum(mpInpEventTime->GetPosition() + cVector2f(0,mpInpEventTime->GetSize().y), _W("Type"), mpGEvent);
	mpInpEventType->AddValue(_W("PlaySound"));
	mpInpEventType->AddValue(_W("Step"));
	mpInpEventValue = CreateInputString(mpInpEventType->GetPosition() + cVector2f(0,mpInpEventType->GetSize().y), _W("Value"), mpGEvent);
}

//------------------------------------------------------------------------------------

void cEdWindowAnimations::OnUpdate()
{
	iEdPopUp::OnUpdate();

	int lSelectedItem = mpListAnimations->GetSelectedItem();

	mpListAnimations->ClearItems();
	tAnimWrapperListIt it = mlstTempAnimations.begin();
	for(;it!=mlstTempAnimations.end(); ++it)
	{
		cAnimationWrapper& anim = *it;

		cWidgetItem* pItem = mpListAnimations->AddItem(anim.GetName());
		pItem->SetUserData(&anim);
	}

	mpListAnimations->SetSelectedItem(lSelectedItem);
}

//------------------------------------------------------------------------------------

bool cEdWindowAnimations::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	cWidgetItem* pItem = mpListAnimations->GetItem(mpListAnimations->GetSelectedItem());
	cAnimationWrapper* pAnim = pItem ? static_cast<cAnimationWrapper*>(pItem->GetUserData()) : NULL;

	if(apWidget==mpBAddAnimation)
	{
		cAnimationWrapper anim;

		tString sName;
		size_t i= mlstTempAnimations.size();
		do
		{
			sName = "Animation " + cString::ToString((int)i++);
		}
		while(CheckAnimNameAvailable(sName)==false);

		anim.SetName(sName);

		mlstTempAnimations.push_back(anim);

		SetUpdated();
	}
	else if(apWidget==mpBRemAnimation)
	{
		if(pAnim)
		{
			tAnimWrapperListIt it = mlstTempAnimations.begin();
			while(it!=mlstTempAnimations.end())
			{
				cAnimationWrapper& anim = *it;
				if(pAnim->GetName()==anim.GetName())
					it = mlstTempAnimations.erase(it);
				else
					++it;
			}
			SetUpdated();
		}
	}
	else if(apWidget==mpBAddEvent)
	{
		if(pAnim)
		{
			pAnim->AddEvent();
            SetUpdated();
		}
	}
	else if(apWidget==mpBRemEvent)
	{
		if(pAnim)
		{
			pAnim->RemoveEvent(this->mpListEvents->GetSelectedItem());
			SetUpdated();
		}
	}
	else if(apWidget==mpBOK)
	{
		bool bAnimationsValid = true;
		tAnimWrapperVec vValidAnimations;
		tAnimWrapperListIt it = mlstTempAnimations.begin();
		for(;it!=mlstTempAnimations.end();++it)
		{
			cAnimationWrapper& anim = *it;
			if(anim.IsValid())
				vValidAnimations.push_back(anim);
			else
			{
				bAnimationsValid = false;
				break;
			}
		}

		if(bAnimationsValid)
		{
			cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
			mpEditor->AddAction(pEnt->CreateSetAnimationsAction(vValidAnimations));
			SetActive(false);
		}
		//else
		//	mpEditor->ShowMessageBox(_W("Warning!"), _W("Animations were not saved due to errors (blank fields)"), _W("OK"),_W(""), NULL, NULL);
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
kGuiCallbackDeclaredFuncEnd(cEdWindowAnimations,InputCallback);

//------------------------------------------------------------------------------------

bool cEdWindowAnimations::WindowSpecificInputCallback(iEdInput* apInput)
{
	cWidgetItem* pItem = mpListAnimations->GetItem(mpListAnimations->GetSelectedItem());
	cAnimationWrapper* pAnim = pItem ? static_cast<cAnimationWrapper*>(pItem->GetUserData()) : NULL;

	cWidgetItem* pEventItem = mpListEvents->GetItem(mpListEvents->GetSelectedItem());
	cAnimationEventWrapper* pEvent = pEventItem ? static_cast<cAnimationEventWrapper*>(pEventItem->GetUserData()) : NULL;

	if(apInput==mpInpAnimName)
	{
		pAnim->SetName(cString::To8Char(mpInpAnimName->GetValue()));
		SetUpdated();
	}
	else if(apInput==mpInpAnimFile)
	{
		pAnim->SetFile(mpInpAnimFile->GetValue8Char());		
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
		pEvent->SetValue(mpInpEventValue->GetValue8Char());
	}

	return true;
}

//------------------------------------------------------------------------------------

bool cEdWindowAnimations::CheckAnimNameAvailable(const tString& asName)
{
	/*
	for(int i=0;i<(int)mvTempAnimations.size();++i)
	{
		if(asName==mvTempAnimations[i].GetName())
			return false;
	}
	*/

	return true;
}

//------------------------------------------------------------------------------------

void cEdWindowAnimations::UpdateAnimInputs()
{
	cWidgetItem* pItem = mpListAnimations->GetItem(mpListAnimations->GetSelectedItem());
	cAnimationWrapper* pAnim = pItem ? static_cast<cAnimationWrapper*>(pItem->GetUserData()) : NULL;

	tString sName = "";
	tString sFile = "";
	float fSpeed = 0;
	float fSpecialEventTime = 0;

	bool bValidAnimation = (pAnim!=NULL);
	tAnimEventWrapperVec vTempEvents;

	if(bValidAnimation)
	{
		sName = pAnim->GetName();
		sFile = pAnim->GetFile();
		fSpeed = pAnim->GetSpeed();
		fSpecialEventTime = pAnim->GetSpecialEventTime();

		vTempEvents = pAnim->GetEvents();
	}

	mpInpAnimName->SetValue(cString::To16Char(sName), false);
	mpInpAnimFile->SetValue(cString::To16Char(sFile), false, true, false, false);
	mpInpAnimSpeed->SetValue(fSpeed, false);
	mpInpAnimSpecialEventTime->SetValue(fSpecialEventTime, false);

	mpListEvents->ClearItems();
	for(size_t i=0;i<vTempEvents.size();++i)
	{
		cAnimationEventWrapper& event = vTempEvents[i];
		cWidgetItem* pEventItem = mpListEvents->AddItem(_W("Event ") + cString::ToStringW((int)i+1));
		pEventItem->SetUserData(&event);
	}

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

void cEdWindowAnimations::UpdateEventInputs()
{
	cWidgetItem* pEventItem = mpListEvents->GetItem(mpListEvents->GetSelectedItem());
	cAnimationEventWrapper* pEvent = pEventItem ? static_cast<cAnimationEventWrapper*>(pEventItem->GetUserData()) : NULL;

	float fEventTime = 0;
	tString sType = "";
	tString sValue = "";
	bool bValidEvent = (pEvent!=NULL);

	if(bValidEvent)
	{
		fEventTime = pEvent->GetTime();
		sType = pEvent->GetType();
		sValue = pEvent->GetValue();
	}

	mpInpEventTime->SetValue(fEventTime, false);
	mpInpEventType->SetValue(cString::To16Char(sType), false);
	mpInpEventValue->SetValue(cString::To16Char(sValue), false);

	mpGEvent->SetEnabled(bValidEvent);
}

//------------------------------------------------------------------------------------

