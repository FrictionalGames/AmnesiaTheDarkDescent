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

#include "../Common/EdMultiScnObjEditPane.h"
#include "../Common/Editor.h"

#include "../Common/EdSelection.h"


cEdMultiScnObjEditPane::cEdMultiScnObjEditPane(iEdObject* apObj) : iEdScnObjEditPane(apObj)
{
	mpBCreateCompound = NULL;
	mpLBNames = NULL;

	mpInpRelTranslation = NULL;
	mpInpRelScale = NULL;
	mpInpRelRotation = NULL;
}

cEdMultiScnObjEditPane::~cEdMultiScnObjEditPane()
{
	//if(mpChildHighlighter)
	//{
	//	hplDelete(mpChildHighlighter);
	//}
}

//----------------------------------------------------------------------------

void cEdMultiScnObjEditPane::Create()
{
	cVector3f vPos = cVector3f(10,10,1);

	iEdWorld* pWorld = mpEditor->GetWorld();
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpObject);

	cWidgetTab* pTab = mpTabs->AddTab(_W("General"));
	cVector2f vTabSize = pTab->GetSize();

	mpInpGlobalTransform = CreateInputBool(vPos, _W("Global Transform"), pTab);
	vPos.y += mpInpGlobalTransform->GetSize().y + 10;

	mpLBNames = mpSet->CreateWidgetListBox(vPos, cVector2f(vTabSize.x-20, 100), pTab);
	for(int i=0;i<pSelection->GetNumComponents(); ++i)
	{
		iEdScnObject* pObj = pSelection->GetComponent(i);
		cWidgetItem* pItem = mpLBNames->AddItem(pObj->GetName());
		pItem->SetUserData(pObj);
	}

	vPos.y += mpLBNames->GetSize().y + 10;

	if(pWorld->GetTypeByName(_W("Compound"))!=NULL)
	{
		mpBCreateCompound = mpSet->CreateWidgetButton(vPos, cVector2f(mpLBNames->GetSize().x, 25), _W("Create compound"), pTab);
		mpBCreateCompound->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(CreateCompound_OnPressed));
		AddShortcut(mpBCreateCompound->AddShortcut(0, eKey_B));

		vPos.y += mpBCreateCompound->GetSize().y+10;
	}


	if(pSelection->IsTranslateable())
	{
		AddRelTranslationInput(pTab);
		mpInpRelTranslation->SetPosition(vPos);
		vPos.y += mpInpRelTranslation->GetSize().y + 5;
	}
	if(pSelection->IsRotateable())
	{
		AddRelRotationInput(pTab);
		mpInpRelRotation->SetPosition(vPos);
		vPos.y += mpInpRelRotation->GetSize().y + 5;
	}
	if(pSelection->IsScalable())
	{
		AddRelScaleInput(pTab);
		mpInpRelScale->SetPosition(vPos);
		vPos.y += mpInpRelScale->GetSize().y + 5;
	}

	/*
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
//----------------------------------------------------------------------------
/*
void cEdMultiScnObjEditPane::AddPropertyName(cWidgetTab* apParentTab)
{
	mpInpName = CreateInputString(0, _W("Name"), apParentTab, 140);
	mpInpName->SetCanEdit(mpObject->IsRenameable());
}
*/

//----------------------------------------------------------------------------

void cEdMultiScnObjEditPane::AddRelTranslationInput(cWidgetTab* apParentTab)
{
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpObject);

	tWStringList lstLabels;
	lstLabels.push_back(_W("X"));
	lstLabels.push_back(_W("Y"));
	lstLabels.push_back(_W("Z"));

	mpInpRelTranslation = CreateInputVec3(0, _W("Relative Translation"),  apParentTab, 150, lstLabels, 
									eEdInpStyle_ColumnLabelOnTop);
	mpInpRelTranslation->SetDecimals(3);

	mvBaseTranslation = pObj->GetTranslation();
}

//----------------------------------------------------------------------------

void cEdMultiScnObjEditPane::AddRelRotationInput(cWidgetTab* apParentTab)
{
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpObject);

	tWStringList lstLabels;
	lstLabels.push_back(_W("X"));
	lstLabels.push_back(_W("Y"));
	lstLabels.push_back(_W("Z"));

	mpInpRelRotation = CreateInputVec3(0, _W("Relative Rotation"),  apParentTab, 150, lstLabels, 
									eEdInpStyle_ColumnLabelOnTop);
									//cMath::ToDeg(cEdSelection::GetRotateSnap()));
	mpInpRelRotation->SetDecimals(3);
	
	mvBaseRotation = pObj->GetRotation();
}

//----------------------------------------------------------------------------

void cEdMultiScnObjEditPane::AddRelScaleInput(cWidgetTab* apParentTab)
{
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpObject);

	tWStringList lstLabels;
	lstLabels.push_back(_W("X"));
	lstLabels.push_back(_W("Y"));
	lstLabels.push_back(_W("Z"));

	mpInpRelScale = CreateInputVec3(0, _W("Relative Scale"),  apParentTab, 150, lstLabels, 
									eEdInpStyle_ColumnLabelOnTop);
									//cEdSelection::GetScaleSnap());
	mpInpRelScale->SetDecimals(3);
	//mpInpScale->SetCanEdit(mpObject->CanScale());

	mvBaseScale = pObj->GetScale();
}

//----------------------------------------------------------------------------

bool cEdMultiScnObjEditPane::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdMultiScnObjEditPane, InputCallback);

//----------------------------------------------------------------------------

bool cEdMultiScnObjEditPane::CreateCompound_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpObject);
	mpEditor->AddAction(pSelection->CreateCompoundObjectAction());

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdMultiScnObjEditPane, CreateCompound_OnPressed);

//----------------------------------------------------------------------------

/*
void cEdMultiScnObjEditPane::OnCreateLayout()
{
	iEdPane::OnCreateLayout();
	cEdWindowSelect* pWin = static_cast<cEdWindowSelect*>(mpSelect->GetWindow());
	pWin->PositionEditPane(this);

	cVector3f vPos = cVector3f(5,5,0.1f);
	iEdScnObjType* pType = static_cast<iEdScnObjType*>(mpObject->GetType());
	for(int i=0; i<pType->GetNumManipulators(); ++i)
	{
		iScnObjManipulator* pManipulator = pType->GetManipulator(i);
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, 20, _W(""), GetBG(), true);
		pButton->SetUserData(pManipulator);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(SetManipulator));

		vPos.x += 22.5f;
	}

	mpTabs = mpSet->CreateWidgetTabFrame(cVector3f(5,30,0.1f), GetBG()->GetSize()-cVector2f(10,50),_W(""), GetBG());

	Create();
}
*/

//----------------------------------------------------------------------------

void cEdMultiScnObjEditPane::OnUpdate()
{
	if(mpObject==NULL)return;
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpObject);

	if(mpInpRelTranslation)
		mpInpRelTranslation->SetValue(pObj->GetTranslation()-mvBaseTranslation, false);
	/*if(mpInpName)
		mpInpName->SetValue(mpObject->GetName(),false);
	//if(mpInpTag)
	//	mpInpTag->SetValue(cString::To16Char(mpObject->GetTag()), false);
	if(mpInpActive)
		mpInpActive->SetValue(pObj->IsEnabled(), false);

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

	//if(mpChildHighlighter)
	//{
	/*	tWidgetListIt it = mlstBAttachFilters.begin();
		for(;it!=mlstBAttachFilters.end();++it)
		{
			cWidgetButton* pButton = static_cast<cWidgetButton*>(*it);
			//if(pButton)
			//	pButton->SetPressed(mpChildHighlighter->GetFilter()->GetTypeFilter((eEdEntityType)pButton->GetUserValue()), false);
		}
	}*/
}

//----------------------------------------------------------------------------

/*
void cEdMultiScnObjEditPane::OnSetActive(bool abX)
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
*/

//----------------------------------------------------------------------------

bool cEdMultiScnObjEditPane::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdAction* pAction = NULL;

	if(apInput==mpInpGlobalTransform)
	{
		cEdSelection* pSelection = mpSelect->GetSelection();
		pSelection->SetGlobalTransform(mpInpGlobalTransform->GetValue());
	}


	//iEdWorld* pWorld = mpEditor->GetWorld();

	/*
	if(apInput==mpInpName)
	{
		const tWString& sName = mpInpName->GetValue();
		//pAction = mpObject->CreateSetStringAction(
		//if(mpObject->GetName()!=sName && pWorld->IsNameAvailable(sName)==false)
		//	mpEditor->ShowMessageBox(_W("Warning"), _W("There is already an object with same name"), _W("OK"), _W(""), NULL, NULL);
		//else
		//	pAction = mpObject->CreateSetStringAction(eObjStr_Name, sName);
	}
	/*
	else if(apInput==mpInpTag)
	{
		tString sTag = cString::To8Char(mpInpTag->GetValue());
		pAction = mpObject->CreateSetStringAction(eObjStr_Tag, sTag);
	}
	*/
	/*
	else if(apInput==mpInpActive)
	{
		//pAction = mpObject->CreateSetBoolAction(eObjBool_Active, mpInpActive->GetValue());
	}
	else if(apInput==mpInpPosition)
	{
		pAction = mpSelect->GetSelection()->CreateSetVector3fAction("WorldPos", mpInpPosition->GetValue());
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
	*/

	return true;
}

//----------------------------------------------------------------------------
