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

#include "../ModelEditor/EdWindowOutline.h"
#include "../ModelEditor/HPLEntity.h"

#include "../ModelEditor/EdObjShape.h"
#include "../ModelEditor/EdObjBody.h"
#include "../ModelEditor/EdObjJoint.h"

#include "../Common/Editor.h"
#include "../Common/EdScnObject.h"
#include "../Common/EdUIPicker.h"

#include "../Common/EdEditModeHandler.h"
#include "../Common/EdEditModeSelect.h"
#include "../Common/EdSelection.h"
//#include "../common/EditorActionEntity.h"
//#include "../common/EditorActionsBodies.h"
//#include "../common/EdEditModeSelect.h"
//#include "../common/EditorSelection.h"

//#include "../common/EditorHelper.h"


#include <algorithm>        
  
//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

/*
cEdWindowOutlineHelper::cEdWindowOutlineHelper(cEdWindowOutline* apWindow, tIntList& alstIDs) : iEdPopUp(apWindow->GetEditor(), 
																										 _W("Attach helper window")), 
																										  mlstTempIDs(alstIDs)
{
	mpCallbackObject = NULL;
	mpCallback = NULL;
	mpFilter = hplNew(cUIPickFilter,("OutlineHelper"));
}

cEdWindowOutlineHelper::~cEdWindowOutlineHelper()
{
	hplDelete(mpFilter);
}

//------------------------------------------------------------------------------------

void cEdWindowOutlineHelper::SetAllowedTypes(const tIntVec& avTypes)
{
	//for(size_t i=0;i<avTypes.size();++i)
	//	mpFilter->SetTypeFilter(avTypes[i], true);

	PopulateList();
	UpdateHighlightedItems();
}

//------------------------------------------------------------------------------------

void cEdWindowOutlineHelper::ExcludeIDs(tIntList& alstIDs)
{
	tIntListIt it = alstIDs.begin();
}

//------------------------------------------------------------------------------------

void cEdWindowOutlineHelper::SetCallback(void *apCallbackObject, tGuiCallbackFunc apCallback)
{
	mpCallbackObject = apCallbackObject;
	mpCallback = apCallback;
}

//------------------------------------------------------------------------------------

void cEdWindowOutlineHelper::OnCreateLayout()
{
	iEdPopUp::OnCreateLayout();
	GetBG()->SetText(_W("Attach"));

	cVector3f vPos = cVector3f(10,30, 0.1f);

	mpListObjects = mpSet->CreateWidgetListBox(vPos+cVector3f(0,0,1), cVector2f(210, 340), GetBG());

	mpListObjects->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(List_OnSelectionChange));
	mpListObjects->SetAllowMultiSelection(true);

	vPos.y += mpListObjects->GetSize().y+5;

	for(int i=0;i<2;i++)
	{
		mvButtons[i] = mpSet->CreateWidgetButton(vPos + cVector3f(10.05f+i*45,0,0), cVector2f(40,25), _W(""), GetBG());

		mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));
	}
	mvButtons[0]->SetText(_W("OK"));
	mvButtons[1]->SetText(_W("Cancel"));
}

//------------------------------------------------------------------------------------

void cEdWindowOutlineHelper::OnSetActive(bool abX)
{
	iEdPopUp::OnSetActive(abX);
}

//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------

bool cEdWindowOutlineHelper::List_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	mlstTempIDs.clear();
	for(int i=0;i<mpListObjects->GetMultiSelectionNum();++i)
	{
		int lIndex = mpListObjects->GetMultiSelection(i);

		mlstTempIDs.push_back(mvIDList[lIndex]);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowOutlineHelper,List_OnSelectionChange);

//------------------------------------------------------------------------------------

void cEdWindowOutlineHelper::PopulateList()
{
	mvIDList.clear();
	mpListObjects->ClearItems();

	const tEdObjectMap& mapObjects = mpEditor->GetWorld()->GetObjects();

	tEdObjectMap::const_iterator it = mapObjects.begin();
	for(;it!=mapObjects.end();++it)
	{
		iEdObject* pObj = it->second;
		//if(pEnt->EntitySpecificFilterCheck(false, mpFilter->GetTypeFilter(pEnt->GetTypeID())))
		//{
		//	mpListObjects->AddItem(pEnt->GetName());
		//	mvIDList.push_back(pEnt->GetID());
		//}
	}
}

//------------------------------------------------------------------------------------

void cEdWindowOutlineHelper::UpdateHighlightedItems()
{
	mpListObjects->ClearSelection();
	for(int i=0;i<(int)mvIDList.size();++i)
	{
		//Log("i==%d - Testing id: %d\n", i, mvIDList[i]);
		if(find(mlstTempIDs.begin(), mlstTempIDs.end(), mvIDList[i])!=mlstTempIDs.end())
		{
			mpListObjects->SetSelectedItem(i, false, false, eListBoxSelectType_Select, false);
			//Log("Selecting id: %d\n", mvIDList[i]);
		}
	}
}

//------------------------------------------------------------------------------------

void cEdWindowOutlineHelper::SetMultiSelectionEnabled(bool abX)
{
	mpListObjects->SetAllowMultiSelection(abX);
}*/

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

cEdWindowOutline::cEdWindowOutline(iEditor* apEditor) : iEdPopUp(apEditor, _W("Outline"))
{
	mpHelperPane = NULL;
	mpFilter =NULL;

	mbHideHelperPane = false;

	mvSizeNormal = cVector2f(360, 400);
	mvSizeOpenPane = mvSizeNormal + cVector2f(210,0);

	mpCallback = NULL;
}

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

void cEdWindowOutline::AddButtonDetach()
{
	mpBDetach = mpSet->CreateWidgetButton(0, cVector2f(75,25), _W("Detach"), mpGroupPanel);
	mpBDetach->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	iEdScnObject* pObj = GetSelectedObject();
	if(pObj->GetParent()==NULL)
		mpBDetach->SetEnabled(false);
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::AddButtonAttachParent()
{
	mpBAttachParent = mpSet->CreateWidgetButton(0, cVector2f(100,25), _W("Attach Parent"), mpGroupPanel);
	mpBAttachParent->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::AddButtonAttachChild()
{
	mpBAttachChild = mpSet->CreateWidgetButton(cVector3f(0, 60, 0),cVector2f(100,25) , _W("Attach Child"), mpGroupPanel);
	mpBAttachChild->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::AddButtonEditAttachments()
{
	mpBEditAttachments = mpSet->CreateWidgetButton(0, cVector2f(100,25), _W("Edit attachments"), mpGroupPanel);
	mpBEditAttachments->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::AddButtonDetachParent()
{
	mpBDetachParent = mpSet->CreateWidgetButton(cVector3f(0,30,0), cVector2f(100,25),  _W("Detach Parent"), mpGroupPanel);
	mpBDetachParent->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	iEdObjJoint* pJoint = static_cast<iEdObjJoint*>(GetSelectedObject());
	if(pJoint->GetParentBody()==NULL)
		mpBDetachParent->SetEnabled(false);
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::AddButtonDetachChild()
{
	mpBDetachChild = mpSet->CreateWidgetButton(cVector3f(0,90,0), cVector2f(100,25), _W("Detach Child"), mpGroupPanel);
	mpBDetachChild->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	iEdObjJoint* pJoint = static_cast<iEdObjJoint*>(GetSelectedObject());
	if(pJoint->GetChildBody()==NULL)
		mpBDetachChild->SetEnabled(false);
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::ClearEditPanel()
{
	tWidgetList lstButtons = mpGroupPanel->GetChildren();
	tWidgetListIt it = lstButtons.begin();
	for(;it!=lstButtons.end(); ++it)
	{
		iWidget* pButton = *it;
		mpSet->DestroyWidget(pButton);
	}

	mpBDetach = NULL;
	mpBAttachParent = NULL;
	mpBAttachChild = NULL;
	mpBEditAttachments = NULL;
	mpBDetachParent = NULL;
	mpBDetachChild = NULL;
}

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

void cEdWindowOutline::PopulateList()
{
	//mbHideConnectedShapes = mpCheckBoxHideShapes->IsChecked();
	bool bHideConnectedShapes = mpInpHideShapes->GetValue();

	mlstAddedChildren.clear();
	mpListObjects->ClearItems();

	cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
	const tEdObjectMap& mapObjects = pEnt->GetObjects();

	tEdObjectMap::const_iterator it = mapObjects.begin();
    for(;it!=mapObjects.end(); ++it)
	{
		iEdScnObject* pObj = static_cast<iEdScnObject*>(it->second);
		pObj->AddToOutline(mpListObjects, 0, mlstAddedChildren);
	}

	UpdateHighlightedItems();
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::UpdateHighlightedItems()
{
	mpListObjects->ClearSelection();

	cEdEditModeSelect* pSelect = static_cast<cEdEditModeSelect*>(mpEditor->GetEditModeHandler()->GetMode(_W("Select")));
	cEdSelection* pSelection = pSelect->GetSelection();

	if(pSelection->HasComponents()==false)
		return;

	for(int i=0; i<this->mpListObjects->GetItemNum(); ++i)
	{
		cWidgetItem* pItem = mpListObjects->GetItem(i);
		iEdScnObject* pObj = static_cast<iEdScnObject*>(pItem->GetUserData());

		if(pObj && pObj->IsSelected()) mpListObjects->SetSelectedItem(i, false, false, eListBoxSelectType_Select, false);
	}
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::UpdateEditPanel()
{
	ClearEditPanel();

	iEdScnObject* pObj = GetSelectedObject();
	if(pObj==NULL) return;

	iEdWorld* pWorld = mpEditor->GetWorld();

	iEdObjectType* pTypeJoint = pWorld->GetTypeByName(_W("Joint"));
	iEdObjectType* pTypeBody = pWorld->GetTypeByName(_W("Body"));
	iEdObjectType* pTypeBone = pWorld->GetTypeByName(_W("Bone"));

	if(pObj->IsA(pTypeJoint))
	{
		AddButtonAttachChild();
		AddButtonAttachParent();
		AddButtonDetachChild();
		AddButtonDetachParent();
	}
	else if(pObj->IsA(pTypeBody) || pObj->IsA(pTypeBone))
	{
		AddButtonEditAttachments();
	}
	else if(pObj->IsAttachable() && pObj->GetParent()!=NULL)
	{
		AddButtonDetach();
	}
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::OnSetActive(bool abX)
{
	iEdPopUp::OnSetActive(abX);

	if(abX)
	{
		PopulateList();
		UpdateHighlightedItems();
		UpdateEditPanel();
	}
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::OnCreateLayout()
{
	iEdPopUp::OnCreateLayout();
	GetBG()->SetSize(cVector2f(360, 400));
	GetBG()->SetText(_W("Outline"));

	cVector3f vPos = cVector3f(15,30,0.1f);

	mpInpHideShapes = CreateInputBool(vPos, _W("Hide connected shapes"), GetBG());
	vPos.y += mpInpHideShapes->GetSize().y+5;

	mpListObjects = mpSet->CreateWidgetListBox(vPos+cVector3f(0,0,1), cVector2f(200, 340), GetBG());

	mpListObjects->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(InputCallback));
	mpListObjects->SetAllowMultiSelection(true);
	mpListObjects->SetDefaultFontSize(12);

	vPos = cVector3f(mpListObjects->GetLocalPosition().x+mpListObjects->GetSize().x+10,30,0.1f);

	mpGroupPanel = mpSet->CreateWidgetDummy(vPos, GetBG());

	mpBAttachChild = NULL;
	mpBDetachChild = NULL;
	mpBAttachParent = NULL;
	mpBDetachParent = NULL;
	mpBDetach = NULL;
	mpBEditAttachments = NULL;
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::OnUpdate()
{
	iEdPopUp::OnUpdate();

	mpInpHideShapes->SetValue(iEdObjShape::mbHideConnectedShapes, false);

	if(mbHideHelperPane)
	{
		mbHideHelperPane = false;
		HideHelperPane();
	}

	UpdateEditPanel();

	//if(IsActive()) 
	//{
		//PopulateList();
		/*
		if(mpEditor->GetWorld()->GetNumModifications()!=mlModificationStamp)
			PopulateList();

		UpdateHighlightedItems();
		UpdateEditPanel();
		*/
	//}
}

void cEdWindowOutline::OnWorldAddRemObject()
{
	PopulateList();
}

void cEdWindowOutline::OnWorldObjModify()
{
	PopulateList();
}

void cEdWindowOutline::OnWorldSelectChange()
{
	UpdateHighlightedItems();

	UpdateEditPanel();
}

//------------------------------------------------------------------------------------

bool cEdWindowOutline::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	tScnObjList lstSelectedObjects;
	for(int i=0; i<mpListObjects->GetMultiSelectionNum(); ++i)
	{
		int lIndex = mpListObjects->GetMultiSelection(i);
		cWidgetItem* pSelectedItem = mpListObjects->GetItem(lIndex);
		iEdScnObject* pObj = static_cast<iEdScnObject*>(pSelectedItem->GetUserData());
		if(pObj==NULL) continue;

		lstSelectedObjects.push_back(pObj);
	}

	iEdAction* pAction = NULL;
	iEdScnObject* pObj = GetSelectedObject();
	tScnObjList lstObjects;
	tGuiCallbackFunc pCallback = NULL;

	bool bShowHelper = false;

	if(apWidget==mpListObjects)
	{
		cEdEditModeSelect* pEditMode = static_cast<cEdEditModeSelect*>(mpEditor->GetEditModeHandler()->GetMode(_W("Select")));
		cEdSelection* pSelection = pEditMode->GetSelection();

		pAction = pSelection->CreateSelectObjectsAction(lstSelectedObjects);
	}

	else if(apWidget==mpBDetach)
	{
		iEdScnObject* pParent = pObj->GetParent();
		tScnObjList lstChildren = pParent->GetChildren();
		lstChildren.remove(pObj);

		pAction = pParent->CreateAddChildrenAction(lstChildren);

		SetUpdated();
	}
	else if(apWidget==mpBAttachParent)
	{
		bShowHelper = true;
		pCallback = kGuiCallback(AttachParentBodyCallback);

		iEdObjJoint* pJoint = static_cast<iEdObjJoint*>(pObj);
		iEdScnObject* pParent = pJoint->GetParentBody();
		if(pParent)
			lstObjects.push_back(pParent);
	}
	else if(apWidget==mpBDetachParent)
	{
		pAction = pObj->CreateSetIntAction(eJointInt_ParentID, -1);
	}
	else if(apWidget==mpBAttachChild)
	{
		bShowHelper = true;
		pCallback = kGuiCallback(AttachChildBodyCallback);

		iEdObjJoint* pJoint = static_cast<iEdObjJoint*>(pObj);
		iEdScnObject* pChild = pJoint->GetChildBody();
		if(pChild)
			lstObjects.push_back(pChild);
	}
	else if(apWidget==mpBDetachChild)
	{
		pAction = pObj->CreateSetIntAction(eJointInt_ChildID, -1);
	}
	else if(apWidget==mpBEditAttachments)
	{
		bShowHelper = true;
		pCallback = kGuiCallback(SetAttachmentsCallback);
		
		lstObjects = pObj->GetChildren();
	}
	else if(apWidget==mpHelperList)
	{
		mlstHelperObjects.clear();
		for(int i=0; i<mpHelperList->GetMultiSelectionNum(); ++i)
		{
			int lIdx = mpHelperList->GetMultiSelection(i);
			iEdScnObject* pObj = static_cast<iEdScnObject*>(mpHelperList->GetItem(lIdx)->GetUserData());

			mlstHelperObjects.push_back(pObj);
		}

	}
	/*
    iEntityWrapper* pEnt = GetSelectedEntity();
	tIntList lstIDs;
	bool bCreateHelper=false;
	bool bHelperMultiSelect=false;
	tGuiCallbackFunc pCallback = NULL;
	
	int lExcludedID = -1;
	//if(mpListObjects->GetLastPerformedAction()==eListBoxActionType_SingleDeselect)
	//	lExcludedID = mvIDList[mpListObjects->GetDeselectedItem()];
			
	for(int i=0;i<mpListObjects->GetMultiSelectionNum();++i)
	{
		int lIndex = mpListObjects->GetMultiSelection(i);
		int lTempID = mvIDList[lIndex];
		if(lTempID!=lExcludedID)
			lstIDs.push_back(lTempID);
	}

	mlstTempIDs.clear();
	
	cEdWindowOutlineHelper* pHelper = NULL;
	iEditorAction* pAction = NULL;

	if(apWidget==mpCheckBoxHideShapes)
	{
		PopulateList();
	}
	
	else if(apWidget==mpBDetach)
	{
		//if(pEnt) 
		//	pAction = hplNew(cEditorActionEntityDetachChildren,(mpEditor->GetWorld(), pEnt->GetParent()->GetID(), lstIDs));
	}
	else if(apWidget==mpBAttachParent)
	{
		//bCreateHelper = true;
		//pCallback = kGuiCallback(AttachParentBodyCallback);
	}
	else if(apWidget==mpBAttachChild)
	{
		//iEntityWrapperJoint* pJoint = (iEntityWrapperJoint*) pEnt;
		//if(pJoint->GetChildBody())
		//	mlstTempIDs.push_back(pJoint->GetChildBody()->GetID());

		bCreateHelper = true;
		pCallback = kGuiCallback(AttachChildBodyCallback);
	}
	else if(apWidget==mpBEditAttachments)
	{
		//cEditorHelper::GetIDsFromEntityList(pEnt->GetChildren(), mlstTempIDs);

		//bCreateHelper = true;
		//bHelperMultiSelect = true;
		//pCallback = kGuiCallback(SetAttachmentsCallback);
	}
	else if(apWidget==mpBDetachParent)
	{
		pAction = pEnt->CreateSetPropertyActionInt(eJointInt_ParentID, -1);
			//hplNew(cEditorActionJointAttachBody,(pEnt->GetID(), mpEditor->GetWorld(), eEditorJointAttachedBody_Parent, -1));
	}
	else if(apWidget==mpBDetachChild)
	{
		pAction = pEnt->CreateSetPropertyActionInt(eJointInt_ChildID, -1);
		//pAction = hplNew(cEditorActionJointAttachBody,(pEnt->GetID(), mpEditor->GetWorld(), eEditorJointAttachedBody_Child, -1));
	}

	if(bCreateHelper)
	{
		pHelper = CreateHelperWindow(mlstTempIDs, pCallback, bHelperMultiSelect);
		pHelper->SetPosition(mpWindow->GetGlobalPosition() + cVector3f(mpWindow->GetSize().x,0,0));
		pHelper->SetAllowedTypes(pEnt->GetAttachableTypes());
	}

	mpEditor->AddAction(pAction);
	*/

	if(bShowHelper)
	{
		ShowHelperPane(pObj->GetAttachableTypes(), lstObjects, pCallback, true);
	}

	//GetBG()->SetSize(GetBG()->GetSize() + cVector2f(200, 0));

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowOutline, InputCallback);

//------------------------------------------------------------------------------------

bool cEdWindowOutline::AttachParentBodyCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdObjJoint* pJoint = static_cast<iEdObjJoint*>(GetSelectedObject());

	if(pJoint==NULL || pJoint->IsA(_W("Joint"))==false)
		return true;

	cEdObjBody* pBody = NULL;
	if(mlstHelperObjects.empty()==false)
		pBody = static_cast<cEdObjBody*>(mlstHelperObjects.front());

	mpEditor->AddAction(pJoint->CreateSetParentBodyAction(pBody));
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowOutline, AttachParentBodyCallback);

//------------------------------------------------------------------------------------

bool cEdWindowOutline::AttachChildBodyCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdObjJoint* pJoint = static_cast<iEdObjJoint*>(GetSelectedObject());

	if(pJoint==NULL || pJoint->IsA(_W("Joint"))==false)
		return true;

	cEdObjBody* pBody = NULL;
	if(mlstHelperObjects.empty()==false)
		pBody = static_cast<cEdObjBody*>(mlstHelperObjects.front());

	mpEditor->AddAction(pJoint->CreateSetChildBodyAction(pBody));

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowOutline, AttachChildBodyCallback);

//------------------------------------------------------------------------------------

bool cEdWindowOutline::SetAttachmentsCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdScnObject* pObj = GetSelectedObject();
	if(pObj)
		mpEditor->AddAction(pObj->CreateAddChildrenAction(mlstHelperObjects));
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowOutline, SetAttachmentsCallback);

//------------------------------------------------------------------------------------

iEdScnObject* cEdWindowOutline::GetSelectedObject()
{
	iEdScnObject* pObj = NULL;

	for(int i=0;i<mpListObjects->GetMultiSelectionNum();++i)
	{
		int lIdx = mpListObjects->GetMultiSelection(i);
		cWidgetItem* pSelectedItem = mpListObjects->GetItem(lIdx);
		iEdScnObject* pSelectedObj = static_cast<iEdScnObject*>(pSelectedItem->GetUserData());

		if(pObj==NULL) pObj = pSelectedObj;
		else
			if(pObj!=pSelectedObj)
				return NULL;
	}

	return pObj;
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::PushEditButtonsState()
{
	bool* vButtonState[] = 
	{
		&mbAttachChildEnabled,
		&mbAttachParentEnabled,
		&mbDetachChildEnabled,
		&mbDetachParentEnabled,
		&mbDetachEnabled,
		&mbEditAttachmentsEnabled,
	};
	iWidget* vButtons[] =
	{
		mpBAttachChild,
		mpBAttachParent,
		mpBDetachChild,
		mpBDetachParent,
		mpBDetach,
		mpBEditAttachments
	};

	for(int i=0; i<6;++i)
	{
		iWidget* pButton = vButtons[i];
		if(pButton)
		{
			bool* pButtonState = vButtonState[i];
			*pButtonState = pButton->IsEnabled();
		}
	}
}

void cEdWindowOutline::PopEditButtonsState()
{
	bool* vButtonState[] = 
	{
		&mbAttachChildEnabled,
		&mbAttachParentEnabled,
		&mbDetachChildEnabled,
		&mbDetachParentEnabled,
		&mbDetachEnabled,
		&mbEditAttachmentsEnabled,
	};
	iWidget* vButtons[] =
	{
		mpBAttachChild,
		mpBAttachParent,
		mpBDetachChild,
		mpBDetachParent,
		mpBDetach,
		mpBEditAttachments
	};

	for(int i=0; i<6;++i)
	{
		iWidget* pButton = vButtons[i];
		if(pButton)
		{
			bool bButtonState = *vButtonState[i];
			pButton->SetEnabled(bButtonState);
		}
	}
}

void cEdWindowOutline::SetEditButtonsEnabled(bool abX)
{
	iWidget* vButtons[] =
	{
		mpBAttachChild,
		mpBAttachParent,
		mpBDetachChild,
		mpBDetachParent,
		mpBDetach,
		mpBEditAttachments
	};
	for(int i=0; i<6;++i)
	{
		iWidget* pButton = vButtons[i];
		if(pButton)
		{
			pButton->SetEnabled(abX);
		}
	}
}

void cEdWindowOutline::ShowHelperPane(const tScnObjTypeVec& avAttachableTypes, const tScnObjList& alstObjects, tGuiCallbackFunc apCallback, bool abMulti)
{
	HideHelperPane();

	PushEditButtonsState();
	SetEditButtonsEnabled(false);

	GetBG()->SetSize(mvSizeOpenPane);

	mpCallback = apCallback;

	mpHelperPane = mpSet->CreateWidgetDummy(mpGroupPanel->GetLocalPosition() + cVector3f(mpGroupPanel->GetSize().x + 10, 0, 0), GetBG());

	cVector3f vPos = cVector3f(0,0,1);
	
	mpHelperList = mpSet->CreateWidgetListBox(vPos, cVector2f(210, 330), mpHelperPane);
	mpHelperList->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(InputCallback));
	mpHelperList->SetAllowMultiSelection(abMulti);
	mpHelperList->SetDefaultFontSize(12);

	vPos.y += mpHelperList->GetSize().y+5;

	tWString vButtonStrings[] = { _W("OK"), _W("Cancel") };
	for(int i=0;i<2;i++)
	{
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos + cVector3f(30+i*55.0f,0,0), 
															cVector2f(50,25), 
															vButtonStrings[i], mpHelperPane);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(HelperButton_OnPressed));

		mvHelperButtons[i] = pButton;
	}

	mlstHelperObjects = alstObjects;
	mpFilter = hplNew(cUIPickFilter,("Niano"));
	mpFilter->SetAllPass(false);
	for(size_t i=0;i<avAttachableTypes.size();++i)
	{
		mpFilter->SetTypeFilter(avAttachableTypes[i], true);
	}
	
	PopulateHelperList();

	//mpFilter
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::HideHelperPane()
{
	GetBG()->SetSize(mvSizeNormal);

	if(mpHelperPane)
	{
		mpSet->DestroyWidget(mpHelperPane, true);
		mpHelperPane = NULL;
	}
	if(mpFilter)
	{
		hplDelete(mpFilter);
		mpFilter = NULL;
	}
}

//------------------------------------------------------------------------------------

void cEdWindowOutline::PopulateHelperList()
{
	mpHelperList->ClearItems();

	cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
	const tEdObjectMap& mapObjects = pEnt->GetObjects();

	tEdObjectMap::const_iterator it = mapObjects.begin();
    for(;it!=mapObjects.end(); ++it)
	{
		iEdScnObject* pObj = static_cast<iEdScnObject*>(it->second);

		if(mpFilter->Passes(pObj))
		{
			cWidgetItem* pItem = mpHelperList->AddItem(pObj->GetName());
			pItem->SetUserData(pObj);

			if(find(mlstHelperObjects.begin(), mlstHelperObjects.end(), pObj)!=mlstHelperObjects.end())
			{
				mpHelperList->SetSelectedItem(mpHelperList->GetItemNum()-1, false, false, eListBoxSelectType_Select, false);
			}
		}
	}
}

//------------------------------------------------------------------------------------

bool cEdWindowOutline::WindowSpecificInputCallback(iEdInput* apInput)
{
	if(apInput==mpInpHideShapes)
	{
		iEdObjShape::mbHideConnectedShapes = mpInpHideShapes->GetValue();
		PopulateList();
	}
	else
		return iEdPopUp::WindowSpecificInputCallback(apInput);

	return true;
}

//------------------------------------------------------------------------------------

bool cEdWindowOutline::HelperButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget==mvHelperButtons[0])
	{
		if(mpCallback)
			mpCallback(this, NULL, cGuiMessageData());
	}
	else if(apWidget==mvHelperButtons[1])
	{
	}

	mbHideHelperPane = true;
	SetUpdated();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowOutline,HelperButton_OnPressed);

