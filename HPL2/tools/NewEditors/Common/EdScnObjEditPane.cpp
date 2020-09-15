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

#include "../Common/EdScnObjEditPane.h"
#include "../Common/Editor.h"

#include "../Common/EdWindowSelect.h"
#include "../Common/EdSelection.h"

#include "../Common/EdEditModeHandler.h"

#include <algorithm>

//-------------------------------------------------------------------------

cEdEditModeSelect* iEdScnObjEditPane::mpSelect = NULL;
int iEdScnObjEditPane::mLastEditedType = -1;
int iEdScnObjEditPane::mCurrentEditedType = -1;
int iEdScnObjEditPane::mlLastTabOpened = -1;
iScnObjManipulator* iEdScnObjEditPane::mpLastActiveManipulator = NULL;

//-------------------------------------------------------------------------

iEdScnObjEditPane::iEdScnObjEditPane(iEdObject* apObj) : iEdEditPane(apObj)
{
//	if(mpObject)
//		mCurrentEditedType = mpObject->GetTypeID();
//	else
//		mCurrentEditedType = -1;
    
	mpTabs = NULL;

	mpInpName = NULL;
	mpInpTag = NULL;
	mpInpActive = NULL;
	mpInpPosition = NULL;
	mpInpScale = NULL;
	mpInpRotation = NULL;

	mpChildHighlighter = NULL;

	mpBRemoveFromCompound = NULL;
}

iEdScnObjEditPane::~iEdScnObjEditPane()
{
	if(mpChildHighlighter)
	{
		hplDelete(mpChildHighlighter);
	}
}

//----------------------------------------------------------------------------

void iEdScnObjEditPane::SetUpGeneralTab()
{
	if(mpObject==NULL)
		return;

/*	cVector3f vPos = cVector3f(10,10,0.1f);
	if(mpInpName)
	{
		mpInpName->SetPosition(vPos);
		vPos.y += mpInpName->GetSize().y+5;
	}
	if(mpInpTag)
	{
		mpInpTag->SetPosition(vPos);
		vPos.y += mpInpTag->GetSize().y+5;
	}
	if(mpInpActive)
	{
		mpInpActive->SetPosition(vPos);
		vPos.y += mpInpActive->GetSize().y+5;
	}
	if(mpInpPosition)
	{
		mpInpPosition->SetPosition(vPos);
		vPos.y += mpInpPosition->GetSize().y+5;
	}
	if(mpInpScale)
	{
		mpInpScale->SetPosition(vPos);
		vPos.y += mpInpScale->GetSize().y+5;
	}
	if(mpInpRotation)
	{
		mpInpRotation->SetPosition(vPos);
		vPos.y += mpInpRotation->GetSize().y+5;
	}

	mvStartFramePos = vPos;*/
}

//----------------------------------------------------------------------------

void iEdScnObjEditPane::AddManipulatorButtons(const cVector3f& avPos, iWidget** apCurrentManipulatorButton)
{
	cVector3f vPos = avPos;
	eKey key[] =
	{
		eKey_Q,
		eKey_W,
		eKey_E,
		eKey_R,
		eKey_T,
		eKey_None
	};

	tWidgetListIt itButtons = mlstManipulatorButtons.begin();
	for(;itButtons!=mlstManipulatorButtons.end();++itButtons)
	{
		mpSet->DestroyWidget(*itButtons);
	}
	mlstManipulatorButtons.clear();

	int lKeyIndex = 0;
	iEdScnObjType* pType = static_cast<iEdScnObjType*>(mpObject->GetType());
	for(int i=0; i<pType->GetNumManipulators(); ++i)
	{
		iScnObjManipulator* pManipulator = pType->GetManipulator(i);
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, 20, _W(""), GetBG(), true);
		pButton->SetUserData(pManipulator);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(SetManipulator));

		if(key[lKeyIndex]!=eKey_None)
		{
			AddShortcut(pButton->AddShortcut(eKeyModifier_None, key[lKeyIndex]));
			lKeyIndex++;
		}

		if(*apCurrentManipulatorButton==NULL || pManipulator==mpLastActiveManipulator)
			*apCurrentManipulatorButton = pButton;

		mlstManipulatorButtons.push_back(pButton);

		vPos.x += 22.5f;
	}
}

//----------------------------------------------------------------------------

void iEdScnObjEditPane::AddCompoundDetachButton(cWidgetTab* apParentTab)
{
	iEdScnObject* pObject = static_cast<iEdScnObject*>(mpObject);
	if(pObject->GetCompoundObject()==NULL)
		return;

	mpBRemoveFromCompound = mpSet->CreateWidgetButton(0, cVector2f(apParentTab->GetSize().x -20, 25), _W("Detach from Compound"), apParentTab);
	mpBRemoveFromCompound->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
}

//----------------------------------------------------------------------------

void iEdScnObjEditPane::AddPropertyName(cWidgetTab* apParentTab)
{
	mpInpName = CreateInputString(0, _W("Name"), apParentTab, 140);
	//mpInpName->SetCanEdit(mpObject->IsRenameable());
	mpInpName->GetInputWidget(0)->SetEnabled(mpObject->IsRenameable());
}

//----------------------------------------------------------------------------

void iEdScnObjEditPane::AddPropertyPosition(cWidgetTab* apParentTab)
{
	tWStringList lstLabels;
	lstLabels.push_back(_W("X"));
	lstLabels.push_back(_W("Y"));
	lstLabels.push_back(_W("Z"));

	mpInpPosition = CreateInputVec3(0, _W("Position"),  apParentTab, 150, lstLabels, 
									eEdInpStyle_ColumnLabelOnTop);// mpEditor->GetGrid()->GetSnapSeparation());
	mpInpPosition->SetDecimals(3);
	//mpInpPosition->SetCanEdit(mpObject->CanTranslate());
}

//----------------------------------------------------------------------------

void iEdScnObjEditPane::AddPropertyRotation(cWidgetTab* apParentTab)
{
	tWStringList lstLabels;
	lstLabels.push_back(_W("X"));
	lstLabels.push_back(_W("Y"));
	lstLabels.push_back(_W("Z"));

	mpInpRotation = CreateInputVec3(0, _W("Rotation"),  apParentTab, 150, lstLabels, 
									eEdInpStyle_ColumnLabelOnTop);
									//cMath::ToDeg(cEdSelection::GetRotateSnap()));
	mpInpRotation->SetDecimals(3);
	//mpInpRotation->SetCanEdit(mpObject->CanRotate());
}

//----------------------------------------------------------------------------

void iEdScnObjEditPane::AddPropertyScale(cWidgetTab* apParentTab)
{
	tWStringList lstLabels;
	lstLabels.push_back(_W("X"));
	lstLabels.push_back(_W("Y"));
	lstLabels.push_back(_W("Z"));

	mpInpScale = CreateInputVec3(0, _W("Scale"),  apParentTab, 150, lstLabels, 
									eEdInpStyle_ColumnLabelOnTop);
									//cEdSelection::GetScaleSnap());
	mpInpScale->SetDecimals(3);
	//mpInpScale->SetCanEdit(mpObject->CanScale());
}

//----------------------------------------------------------------------------

void iEdScnObjEditPane::AddPropertyActive(cWidgetTab* apParentTab)
{
	mpInpActive = CreateInputBool(0, _W("Active"),  apParentTab);
}

//----------------------------------------------------------------------------

void iEdScnObjEditPane::AddPropertySetAttachments(cWidgetTab* apParentTab)
{
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpObject);
	cVector3f vPos = cVector3f(15,30,0.1f);

	mpChildHighlighter = hplNew(cEdSelectorHighlighter,(mpSelect));
	mpChildHighlighter->SetMouseUpCallback(this, kGuiCallback(Child_OnPick));

	iEdWorld* pWorld = mpEditor->GetWorld();

	const tScnObjTypeVec& vFilters = pObj->GetAttachableTypes();
	for(size_t i=0; i<vFilters.size(); ++i)
	{
		iEdObjectType* pType = vFilters[i];
		tWString sShortTypeName = cString::SubW(pType->GetName(),0,2);

		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, 20, sShortTypeName, apParentTab);
		pButton->SetUserData(pType);
		pButton->SetToolTip(pType->GetName());
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
		pButton->SetToggleable(true);
		pButton->SetPressed(true);

		vPos.x+= 25;
		if(vPos.x >160)
		{
			vPos.y += 25;
			vPos.x = 15;
		}

		mlstBAttachFilters.push_back(pButton);
	}

	vPos.y += 25;
	vPos.x = 15;
	mpBEditAttachments = mpSet->CreateWidgetButton(vPos, cVector2f(160,25), _W("Attach"), apParentTab);

	mpBEditAttachments->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	vPos.y += 28;
	mpBClearAttachments = mpSet->CreateWidgetButton(vPos, cVector2f(160, 25), _W("Clear attachments"), apParentTab);
	mpBClearAttachments->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
}

//----------------------------------------------------------------------------

bool iEdScnObjEditPane::SetManipulator(iWidget* apWidget, const cGuiMessageData& aData)
{
	iScnObjManipulator* pManipulator = static_cast<iScnObjManipulator*>(apWidget->GetUserData());
	
	mpSelect->SetCurrentManipulator(pManipulator);
	mpLastActiveManipulator = pManipulator;

	tWidgetListIt it = mlstManipulatorButtons.begin();
	for(;it!=mlstManipulatorButtons.end(); ++it)
	{
		cWidgetButton* pButton = static_cast<cWidgetButton*>(*it);

		pButton->SetPressed(pButton==apWidget, false);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEdScnObjEditPane, SetManipulator);


//----------------------------------------------------------------------------

bool iEdScnObjEditPane::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lID = mpObject->GetID();
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpObject);
	iEdAction* pAction = NULL;
	iEdWorld* pWorld = mpEditor->GetWorld();

	if(apWidget == mpBEditAttachments)
	{
		// TODO : This is quite temp, but works now
		if(mpSelect->GetCurrentSelector()==mpChildHighlighter)
		{
			mpSelect->SetCurrentSelector(NULL);
			mpBEditAttachments->SetText(_W("Attach"));
			if(mlstAttachedChildren!=pObj->GetChildren())
			{
				pAction = pObj->CreateAddChildrenAction(mlstAttachedChildren);
				//tIntList lstChildIDs;
				//cEdHelper::GetIDsFromEntityList(mlstAttachedChildren, lstChildIDs);
				//pAction = hplNew( cEdActionEntityAttachChildren,(pWorld, lID, lstChildIDs) );
			}

			mpChildHighlighter->Reset();
			mlstAttachedChildren.clear();
		}
		else
		{
			if(mpChildHighlighter->IsAtLeastOneFilterActive())
			{
				mpBEditAttachments->SetText(_W("Done"));
				mpSelect->SetCurrentSelector(mpChildHighlighter);
				mlstAttachedChildren = pObj->GetChildren();
				mpChildHighlighter->SetHighlightedEntities(mlstAttachedChildren);
			}
			else
			{
				//mpEditor->ShowMessageBox(_W("Warning"), _W("You must activate at least one filter"), _W("OK"),_W(""),NULL,NULL);
			}
		}
	}
	else if(apWidget==mpBClearAttachments)
	{
		pAction = pObj->CreateAddChildrenAction(tScnObjList());
	}
	else
	{
		tWidgetListIt it = mlstBAttachFilters.begin();
		for(;it!=mlstBAttachFilters.end();++it)
		{
			cWidgetButton* pButton = static_cast<cWidgetButton*>(*it);
			cUIPickFilter* pFilter = mpChildHighlighter->GetFilter();
			iEdObjectType* pType = static_cast<iEdObjectType*>(pButton->GetUserData());
			if(apWidget==pButton)
			{
				pFilter->SetTypeFilter(pType, pButton->IsPressed());
				if(mpChildHighlighter->IsAtLeastOneFilterActive()==false)
				{
					//mpEditor->ShowMessageBox(_W("Warning"), _W("You must activate at least one filter"), _W("OK"),_W(""),NULL,NULL);
					pFilter->SetTypeFilter(pType, true);
					//mpChildHighlighter->GetFilter()->SetTypeFilter((eEdEntityType)i, true);
				}

				SetUpdated();

				//mpEditor->SetLayoutNeedsUpdate(true);
			}
		}
	}

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEdScnObjEditPane, InputCallback);

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void iEdScnObjEditPane::OnCreateLayout()
{
	iEdPane::OnCreateLayout();
	cEdWindowSelect* pWin = static_cast<cEdWindowSelect*>(mpSelect->GetWindow());
	pWin->PositionEditPane(this);

	iWidget* pCurrentManipulatorButton = NULL;

	cVector3f vPos = cVector3f(5,5,0.1f);

	AddManipulatorButtons(vPos, &pCurrentManipulatorButton);

	mpTabs = mpSet->CreateWidgetTabFrame(cVector3f(5,30,0.1f), GetBG()->GetSize()-cVector2f(10,50),_W(""), GetBG());

	Create();

	if(pCurrentManipulatorButton)
		SetManipulator(pCurrentManipulatorButton, cGuiMessageData());
	else
		mpSelect->SetCurrentManipulator(NULL);
}

//----------------------------------------------------------------------------

void iEdScnObjEditPane::OnUpdate()
{
	if(mpObject==NULL)return;
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpObject);

	if(mpInpName)
		mpInpName->SetValue(mpObject->GetName(),false);
	//if(mpInpTag)
	//	mpInpTag->SetValue(cString::To16Char(mpObject->GetTag()), false);
	if(mpInpActive)
		mpInpActive->SetValue(pObj->IsActive(), false);

	if(mpInpPosition)
		mpInpPosition->SetValue(pObj->GetTranslation(), false);
	if(mpInpRotation)
	{
		cVector3f vRot = pObj->GetRotation();

		vRot.x = cMath::ToDeg(vRot.x);
		vRot.y = cMath::ToDeg(vRot.y);
		vRot.z = cMath::ToDeg(vRot.z);
		mpInpRotation->SetValue(vRot, false);
	}
	if(mpInpScale)
		mpInpScale->SetValue(pObj->GetScale(), false);

	if(mpChildHighlighter)
	{
		tWidgetListIt it = mlstBAttachFilters.begin();
		cUIPickFilter* pFilter = mpChildHighlighter->GetFilter();
		for(;it!=mlstBAttachFilters.end();++it)
		{
			cWidgetButton* pButton = static_cast<cWidgetButton*>(*it);
			if(pButton)
				pButton->SetPressed(pFilter->GetTypeFilter(static_cast<iEdObjectType*>(pButton->GetUserData())), false);
		}
	}
}

//----------------------------------------------------------------------------

void iEdScnObjEditPane::OnSetActive(bool abX)
{
	if(abX)
	{
		if(mCurrentEditedType!=-1 && 
			mLastEditedType==mCurrentEditedType)
			mpTabs->SetTabOnTopByIndex(mlLastTabOpened);
	}
	else
	{
		mLastEditedType = mCurrentEditedType;
		mlLastTabOpened = mpTabs?mpTabs->GetTabOnTopIndex():-1;
	}
}

//----------------------------------------------------------------------------

bool iEdScnObjEditPane::Child_OnPick(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpChildHighlighter->GetPickedEntity());
	if(pObj==NULL)
		return InputCallback(mpBEditAttachments, cGuiMessageData());

	tScnObjListIt it = find(mlstAttachedChildren.begin(), mlstAttachedChildren.end(), pObj);
	if(it!=mlstAttachedChildren.end())
	{
		mpChildHighlighter->RemoveHighlightedEntity(*it);
		mlstAttachedChildren.erase(it);
	}
	else
	{
		mpChildHighlighter->AddHighlightedEntity(pObj);
		mlstAttachedChildren.push_back(pObj);
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(iEdScnObjEditPane,Child_OnPick);

//----------------------------------------------------------------------------

bool iEdScnObjEditPane::WindowSpecificInputCallback(iEdInput* apInput)
{
	tIntList lstEntityIDs;
	lstEntityIDs.push_back(mpObject->GetID());
	iEdAction* pAction = NULL;
	iEdWorld* pWorld = mpEditor->GetWorld();

	if(apInput==mpInpName)
	{
		const tWString& sName = mpInpName->GetValue();
		//pAction = mpObject->CreateSetStringAction(
		//if(mpObject->GetName()!=sName && pWorld->IsNameAvailable(sName)==false)
		//	mpEditor->ShowMessageBox(_W("Warning"), _W("There is already an object with same name"), _W("OK"), _W(""), NULL, NULL);
		//else
		pAction = mpObject->CreateSetStringAction(eObjStr_Name, cString::To8Char(sName));
	}
	/*
	else if(apInput==mpInpTag)
	{
		tString sTag = cString::To8Char(mpInpTag->GetValue());
		pAction = mpObject->CreateSetStringAction(eObjStr_Tag, sTag);
	}
	*/
	else if(apInput==mpInpActive)
	{
		pAction = mpObject->CreateSetBoolAction(eScnBool_Active, mpInpActive->GetValue());
	}
	else if(apInput==mpInpPosition)
	{
		pAction = mpObject->CreateSetVector3fAction(eScnVec3f_Translation, mpInpPosition->GetValue());
		//pAction = mpObject->CreateSetVector3fAction(eScnVec3f_Translation, mpInpPosition->GetValue());
	}
	else if(apInput==mpInpScale)
	{
		pAction = mpObject->CreateSetVector3fAction(eScnVec3f_Scale, mpInpScale->GetValue());
	}
	else if(apInput==mpInpRotation)
	{
		cVector3f vRot = mpInpRotation->GetValue();
		vRot.x = cMath::ToRad(vRot.x);
		vRot.y = cMath::ToRad(vRot.y);
		vRot.z = cMath::ToRad(vRot.z);

		pAction = mpObject->CreateSetVector3fAction(eScnVec3f_Rotation, vRot);
	}
	else
		return false;

	AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

iEdUserObjEditPane::iEdUserObjEditPane(iEdScnObject* apObject) : iEdScnObjEditPane(apObject)
{
	mpInputPanel = NULL;
	mbRefreshInputs = false;
}

//----------------------------------------------------------------------------

iEdUserObjEditPane::~iEdUserObjEditPane()
{
	hplDelete(mpInputPanel);
}

//----------------------------------------------------------------------------

void iEdUserObjEditPane::AddInputs(cWidgetTab* apParentTab, const cVector3f& avStartPos)
{
	cVector3f vPos = cVector3f(4,4,0.1f);
	mpFVars = mpSet->CreateWidgetFrame(vPos, apParentTab->GetSize()-cVector2f(12,40), false, apParentTab);

	iUserObject* pUsrObj = static_cast<iUserObject*>(mpObject);

	mpInputPanel = pUsrObj->GetClassInstance()->CreateInputPanel(this, mpFVars, false);
	mpInputPanel->SetCallback(this, VarInputCallbackStaticHelper);
}

//----------------------------------------------------------------------------

bool iEdUserObjEditPane::VarInputCallback(iEdVarInput* apInput)
{
	iUserObject* pUsrObj = static_cast<iUserObject*>(mpObject);

	iEdAction* pAction = pUsrObj->CreateSetUserValueAction(apInput->GetVar()->GetName(), apInput->GetInput()->GetValue());
	AddAction(pAction);

	SetUpdated();

	return true;
}

bool iEdUserObjEditPane::VarInputCallbackStaticHelper(void* apWin, iEdVarInput* apInput)
{
	iEdUserObjEditPane* pWin = static_cast<iEdUserObjEditPane*>(apWin);
	return pWin->VarInputCallback(apInput);
}

//----------------------------------------------------------------------------

void iEdUserObjEditPane::OnWorldObjModify()
{
	SetUpdated();
}

//----------------------------------------------------------------------------

void iEdUserObjEditPane::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();

	iUserObject* pUsrObj = static_cast<iUserObject*>(mpObject);
	if(pUsrObj->HasChangedType())
	{
		SetRefreshInputs();

		iWidget* pCurrentManipulatorButton=NULL;
		AddManipulatorButtons(cVector3f(5,5,0.1f), &pCurrentManipulatorButton);

		if(pCurrentManipulatorButton)
			SetManipulator(pCurrentManipulatorButton, cGuiMessageData());
		else
			mpSelect->SetCurrentManipulator(NULL);

		pUsrObj->SetTypeChanged(false);
	}

	if(mbRefreshInputs)
	{
		mbRefreshInputs=false;
		hplDelete(mpInputPanel);

		mpInputPanel = pUsrObj->GetClassInstance()->CreateInputPanel(this, mpFVars, false);
		mpInputPanel->SetCallback(this, VarInputCallbackStaticHelper);
	}

	mpInputPanel->Update();
}


//----------------------------------------------------------------------------

