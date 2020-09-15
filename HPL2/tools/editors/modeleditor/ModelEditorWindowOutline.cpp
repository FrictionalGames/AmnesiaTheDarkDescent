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

#include "ModelEditorWindowOutline.h"

#include "../common/EntityWrapper.h"
#include "../common/EntityWrapperBodyShape.h"
#include "../common/EntityWrapperBody.h"
#include "../common/EntityWrapperJoint.h"
#include "../common/EditorWorld.h"

#include "../common/EditorActionEntity.h"
#include "../common/EditorActionsBodies.h"
#include "../common/EditorEditModeSelect.h"
#include "../common/EditorSelection.h"

#include "../common/EditorHelper.h"

#include "ModelEditor.h"

#include <algorithm>

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

cModelEditorWindowOutlineHelper::cModelEditorWindowOutlineHelper(cModelEditorWindowOutline* apWindow, tIntList& alstIDs) : iEditorWindowPopUp(apWindow->GetEditor(), 
																																			  "Attach helper window", 
																																			  true, true, true,
																																			  cVector2f(230,400)), 
																																			  mlstTempIDs(alstIDs)
{
	mpCallbackObject = NULL;
	mpCallback = NULL;
	mpFilter = hplNew(cUIPickFilter,("OutlineHelper"));
}

cModelEditorWindowOutlineHelper::~cModelEditorWindowOutlineHelper()
{
	hplDelete(mpFilter);
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutlineHelper::SetAllowedTypes(const tIntVec& avTypes)
{
	for(int i=0;i<(int)avTypes.size();++i)
		mpFilter->SetTypeFilter(avTypes[i], true);

	PopulateList();
	UpdateHighlightedItems();
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutlineHelper::ExcludeIDs(tIntList& alstIDs)
{
	tIntListIt it = alstIDs.begin();
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutlineHelper::SetCallback(void *apCallbackObject, tGuiCallbackFunc apCallback)
{
	mpCallbackObject = apCallbackObject;
	mpCallback = apCallback;
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutlineHelper::OnInitLayout()
{
	iEditorWindowPopUp::OnInitLayout();
	mpWindow->SetText(_W("Attach"));

	cVector3f vPos = cVector3f(10,30, 0.1f);

	mpListObjects = mpSet->CreateWidgetListBox(vPos+cVector3f(0,0,1), cVector2f(210, 340), mpWindow);

	mpListObjects->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(List_OnSelectionChange));
	mpListObjects->SetAllowMultiSelection(true);

	vPos.y += mpListObjects->GetSize().y+5;

	for(int i=0;i<2;i++)
	{
		mvButtons[i] = mpSet->CreateWidgetButton(vPos + cVector3f(10.05f+i*45,0,0), cVector2f(40,25), _W(""), mpWindow);

		mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));
	}
	mvButtons[0]->SetText(_W("OK"));
	mvButtons[1]->SetText(_W("Cancel"));
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutlineHelper::OnSetActive(bool abX)
{
	iEditorWindowPopUp::OnSetActive(abX);
}

//------------------------------------------------------------------------------------

bool cModelEditorWindowOutlineHelper::Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget==mvButtons[0])
	{
		if(mpCallbackObject && mpCallback)
		{
			mpCallback(mpCallbackObject, NULL, cGuiMessageData());
		}
	}
	else if(apWidget==mvButtons[1])
	{
	}

	SetActive(false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorWindowOutlineHelper,Button_OnPressed);

//------------------------------------------------------------------------------------

bool cModelEditorWindowOutlineHelper::List_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	mlstTempIDs.clear();
	for(int i=0;i<this->mpListObjects->GetMultiSelectionNum();++i)
	{
		int lIndex = mpListObjects->GetMultiSelection(i);

		mlstTempIDs.push_back(mvIDList[lIndex]);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorWindowOutlineHelper,List_OnSelectionChange);

//------------------------------------------------------------------------------------

void cModelEditorWindowOutlineHelper::PopulateList()
{
	mvIDList.clear();
	mpListObjects->ClearItems();

	tEntityWrapperMap& mapEntities = mpEditor->GetEditorWorld()->GetEntities();

	tEntityWrapperMapIt itEntities = mapEntities.begin();
	for(;itEntities!=mapEntities.end();++itEntities)
	{
		iEntityWrapper* pEnt = itEntities->second;
		if(pEnt->EntitySpecificFilterCheck(false, mpFilter->GetTypeFilter(pEnt->GetTypeID())))
		{
			mpListObjects->AddItem(pEnt->GetName());
			mvIDList.push_back(pEnt->GetID());
		}
	}
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutlineHelper::UpdateHighlightedItems()
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

void cModelEditorWindowOutlineHelper::SetMultiSelectionEnabled(bool abX)
{
	mpListObjects->SetAllowMultiSelection(abX);
}

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

cModelEditorWindowOutline::cModelEditorWindowOutline(iEditorBase* apEditor) : iEditorWindowPopUp(apEditor, "Outline Window", false, false, false, cVector2f(360, 400))
{
	mbHideConnectedShapes = false;
}

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::AddButtonDetach()
{
	mpButtonDetach = mpSet->CreateWidgetButton(0, cVector2f(75,25), _W("Detach"), mpGroupPanel);
	mpButtonDetach->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	iEntityWrapper* pEnt = GetSelectedEntity();
	if(pEnt->GetParent()==NULL)
		mpButtonDetach->SetEnabled(false);
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::AddButtonAttachParent()
{
	mpButtonAttachParent = mpSet->CreateWidgetButton(0, cVector2f(100,25), _W("Attach Parent"), mpGroupPanel);
	mpButtonAttachParent->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::AddButtonAttachChild()
{
	mpButtonAttachChild = mpSet->CreateWidgetButton(cVector3f(0, 60, 0),cVector2f(100,25) , _W("Attach Child"), mpGroupPanel);
	mpButtonAttachChild->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::AddButtonEditAttachments()
{
	mpButtonEditAttachments = mpSet->CreateWidgetButton(0, cVector2f(100,25), _W("Edit attachments"), mpGroupPanel);
	mpButtonEditAttachments->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::AddButtonDetachParent()
{
	mpButtonDetachParent = mpSet->CreateWidgetButton(cVector3f(0,30,0), cVector2f(100,25),  _W("Detach Parent"), mpGroupPanel);
	mpButtonDetachParent->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	iEntityWrapperJoint* pJoint = (iEntityWrapperJoint*)GetSelectedEntity();
	if(pJoint->GetParentBody()==NULL)
		mpButtonDetachParent->SetEnabled(false);
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::AddButtonDetachChild()
{
	mpButtonDetachChild = mpSet->CreateWidgetButton(cVector3f(0,90,0), cVector2f(100,25), _W("Detach Child"), mpGroupPanel);
	mpButtonDetachChild->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	iEntityWrapperJoint* pJoint = (iEntityWrapperJoint*)GetSelectedEntity();
	if(pJoint->GetChildBody()==NULL)
		mpButtonDetachChild->SetEnabled(false);
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::ClearEditPanel()
{
	std::vector<iWidget*> vWidgets;
	if(mpButtonDetach)
	{
		vWidgets.push_back(mpButtonDetach);
		mpButtonDetach = NULL;
	}
	if(mpButtonAttachParent)
	{
		vWidgets.push_back(mpButtonAttachParent);
		mpButtonAttachParent = NULL;
	}
	if(mpButtonAttachChild)
	{
		vWidgets.push_back(mpButtonAttachChild);
		mpButtonAttachChild = NULL;
	}
	if(mpButtonEditAttachments)
	{
		vWidgets.push_back(mpButtonEditAttachments);
		mpButtonEditAttachments = NULL;
	}
	if(mpButtonDetachParent)
	{
		vWidgets.push_back(mpButtonDetachParent);
		mpButtonDetachParent = NULL;
	}
	if(mpButtonDetachChild)
	{
		vWidgets.push_back(mpButtonDetachChild);
		mpButtonDetachChild = NULL;
	}
	

	for(int i=0;i<(int)vWidgets.size();++i)
	{
		mpSet->DestroyWidget(vWidgets[i]);
	}
}

//------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::PopulateList()
{
	mlModificationStamp = mpEditor->GetEditorWorld()->GetNumModifications();

	mbHideConnectedShapes = mpCheckBoxHideShapes->IsChecked();

	mvIDList.clear();
	mlstAddedChildren.clear();
	mpListObjects->ClearItems();

	tEntityWrapperMap& mapEntities = mpEditor->GetEditorWorld()->GetEntities();
	mvIDList.reserve(mapEntities.size());

	tEntityWrapperMapIt itEntities = mapEntities.begin();
	for(;itEntities!=mapEntities.end();++itEntities)
	{
		iEntityWrapper* pEnt = itEntities->second;
		if(pEnt->GetParent()==NULL)
		{
			AddObjectToList(pEnt, 0);
		}
	}
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::UpdateHighlightedItems()
{
	mpListObjects->ClearSelection();

	cEditorSelection* pSelection = mpEditor->GetSelection();
	if(pSelection->GetNumEntities()==0)
		return;

	for(int i=0;i<(int)mvIDList.size();++i)
	{
		if(pSelection->HasEntity(mvIDList[i]))
			mpListObjects->SetSelectedItem(i,false, false, eListBoxSelectType_Select, false);
	}
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::UpdateEditPanel()
{
	ClearEditPanel();

	iEntityWrapper* pEnt = GetSelectedEntity();
	if(pEnt==NULL) return;

	switch(pEnt->GetTypeID())
	{
	case eEditorEntityType_Joint:
		AddButtonAttachChild();
		AddButtonAttachParent();
		AddButtonDetachChild();
		AddButtonDetachParent();
		break;
	case eEditorEntityType_Body:
	case eEditorEntityType_Bone:
		AddButtonEditAttachments();
		break;
	case eEditorEntityType_Light:
	case eEditorEntityType_SubMesh:
	case eEditorEntityType_ParticleSystem:
	case eEditorEntityType_Sound:
	case eEditorEntityType_Billboard:
		AddButtonDetach();
		break;
	}
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::AddObjectToList(iEntityWrapper* apEntity, int alLevel)
{
	if(apEntity==NULL)
		return;

	int lID = apEntity->GetID();
	int type = apEntity->GetTypeID();
	if(type==eEditorEntityType_BodyShape)
	{
		cEntityWrapperBodyShape* pShape = (cEntityWrapperBodyShape*)apEntity;
		if(alLevel==0 && pShape->GetParentBody()!=NULL)
			return;
	}

	tIntListIt it = find(mlstAddedChildren.begin(), mlstAddedChildren.end(), lID);
	if(it==mlstAddedChildren.end())
	{
		tString sName = apEntity->GetName();
		for(int i=0;i<alLevel;++i)
			sName = "   " + sName;

		mpListObjects->AddItem(sName);
		mvIDList.push_back(lID);
		mlstAddedChildren.push_back(lID);

		if(type==eEditorEntityType_Joint)
		{
			iEntityWrapperJoint* pJoint = (iEntityWrapperJoint*)apEntity;
			cEntityWrapperBody* vBodies[] = { pJoint->GetParentBody(), pJoint->GetChildBody() };
			
			tString sJointConnectedEntryPrefix[] = { "Parent: ", "Child: " };

            for(int i=0;i<2;++i)
			{
				tString sEntryText;
				for(int j=0;j<=alLevel;++j)
					sEntryText = "   " + sEntryText;

				int lID = -1;
				tString sBodyName = "None";
				if(vBodies[i])
				{
					lID = vBodies[i]->GetID();
					sBodyName = vBodies[i]->GetName();
				}

				sEntryText += sJointConnectedEntryPrefix[i] + sBodyName;
				mpListObjects->AddItem(sEntryText);
				mvIDList.push_back(lID);
			}
		}
		else if(type==eEditorEntityType_Body)
		{
			cEntityWrapperBody* pBody = (cEntityWrapperBody*)apEntity;
			if(mbHideConnectedShapes==false)
			{
				const tEntityWrapperList& lstShapes = pBody->GetComponents();
				tEntityWrapperList::const_iterator itShapes = lstShapes.begin();
				for(;itShapes!=lstShapes.end();++itShapes)
				{
					iEntityWrapper* pShape = *itShapes;
					AddObjectToList(pShape, alLevel+1);
				}
			}
		}

		tEntityWrapperList& lstChildren = apEntity->GetChildren();
		tEntityWrapperListIt itChildren = lstChildren.begin();
		for(;itChildren!=lstChildren.end();++itChildren)
		{
			iEntityWrapper* pChild = *itChildren;
			AddObjectToList(pChild, alLevel+1);
		}
	}
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::OnSetActive(bool abX)
{
	iEditorWindowPopUp::OnSetActive(abX);

	if(abX)
	{
		PopulateList();
		UpdateHighlightedItems();
		UpdateEditPanel();
	}
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::OnInitLayout()
{
	iEditorWindowPopUp::OnInitLayout();
	mpWindow->SetText(_W("Outline"));

	cVector3f vPos = cVector3f(15,30,0.1f);

	mpCheckBoxHideShapes = mpSet->CreateWidgetCheckBox(vPos, 0, _W("Hide connected shapes"), mpWindow);

	mpCheckBoxHideShapes->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));

	vPos.y += mpCheckBoxHideShapes->GetSize().y+5;

	mpListObjects = mpSet->CreateWidgetListBox(vPos+cVector3f(0,0,1), cVector2f(200, 340), mpWindow);

	mpListObjects->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(InputCallback));
	mpListObjects->SetAllowMultiSelection(true);

	vPos = cVector3f(mpListObjects->GetLocalPosition().x+mpListObjects->GetSize().x+10,30,0.1f);

	mpGroupPanel = mpSet->CreateWidgetDummy(vPos, mpWindow);


	mpButtonAttachChild = NULL;
	mpButtonDetachChild = NULL;
	mpButtonAttachParent = NULL;
	mpButtonDetachParent = NULL;
	mpButtonDetach = NULL;
	mpButtonEditAttachments = NULL;
}

//------------------------------------------------------------------------------------

void cModelEditorWindowOutline::OnUpdate(float afTimeStep)
{
	iEditorWindowPopUp::OnUpdate(afTimeStep);

	if(IsActive()) 
	{
		if(mpEditor->GetEditorWorld()->GetNumModifications()!=mlModificationStamp)
			PopulateList();

		UpdateHighlightedItems();
		UpdateEditPanel();
	}
}

//------------------------------------------------------------------------------------

bool cModelEditorWindowOutline::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
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
	
	cModelEditorWindowOutlineHelper* pHelper = NULL;
	iEditorAction* pAction = NULL;

	if(apWidget==mpCheckBoxHideShapes)
	{
		PopulateList();
	}
	else if(apWidget==mpListObjects)
	{
		cEditorEditModeSelect* pEditMode = (cEditorEditModeSelect*)mpEditor->GetEditMode("Select");
	
		pAction = pEditMode->CreateSelectEntityAction(lstIDs, eSelectActionType_Select);
	}
	else if(apWidget==mpButtonDetach)
	{
		if(pEnt) 
			pAction = hplNew(cEditorActionEntityDetachChildren,(mpEditor->GetEditorWorld(), pEnt->GetParent()->GetID(), lstIDs));
	}
	else if(apWidget==mpButtonAttachParent)
	{
		bCreateHelper = true;
		pCallback = kGuiCallback(AttachParentBodyCallback);
	}
	else if(apWidget==mpButtonAttachChild)
	{
		iEntityWrapperJoint* pJoint = (iEntityWrapperJoint*) pEnt;
		if(pJoint->GetChildBody())
			mlstTempIDs.push_back(pJoint->GetChildBody()->GetID());

		bCreateHelper = true;
		pCallback = kGuiCallback(AttachChildBodyCallback);
	}
	else if(apWidget==mpButtonEditAttachments)
	{
		cEditorHelper::GetIDsFromEntityList(pEnt->GetChildren(), mlstTempIDs);

		bCreateHelper = true;
		bHelperMultiSelect = true;
		pCallback = kGuiCallback(SetAttachmentsCallback);
	}
	else if(apWidget==mpButtonDetachParent)
	{
		pAction = pEnt->CreateSetPropertyActionInt(eJointInt_ParentID, -1);
			//hplNew(cEditorActionJointAttachBody,(pEnt->GetID(), mpEditor->GetEditorWorld(), eEditorJointAttachedBody_Parent, -1));
	}
	else if(apWidget==mpButtonDetachChild)
	{
		pAction = pEnt->CreateSetPropertyActionInt(eJointInt_ChildID, -1);
		//pAction = hplNew(cEditorActionJointAttachBody,(pEnt->GetID(), mpEditor->GetEditorWorld(), eEditorJointAttachedBody_Child, -1));
	}

	if(bCreateHelper)
	{
		pHelper = CreateHelperWindow(mlstTempIDs, pCallback, bHelperMultiSelect);
		pHelper->SetPosition(mpWindow->GetGlobalPosition() + cVector3f(mpWindow->GetSize().x,0,0));
		pHelper->SetAllowedTypes(pEnt->GetAttachableTypes());
	}

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorWindowOutline, InputCallback);

//------------------------------------------------------------------------------------

bool cModelEditorWindowOutline::AttachParentBodyCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEntityWrapper* pJoint = GetSelectedEntity();

	if(pJoint->GetTypeID()==eEditorEntityType_Joint)
	{
		int lBodyID = -1;
		if(mlstTempIDs.empty()==false) lBodyID = mlstTempIDs.front();
		mpEditor->AddAction(pJoint->CreateSetPropertyActionInt(eJointInt_ParentID, lBodyID));
		//mpEditor->AddAction(hplNew(cEditorActionJointAttachBody,(pJoint->GetID(), mpEditor->GetEditorWorld(), eEditorJointAttachedBody_Parent, lBodyID)));
	}
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorWindowOutline, AttachParentBodyCallback);

//------------------------------------------------------------------------------------

bool cModelEditorWindowOutline::AttachChildBodyCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEntityWrapper* pJoint = GetSelectedEntity();

	if(pJoint->GetTypeID()==eEditorEntityType_Joint)
	{
		int lBodyID = -1;
		if(mlstTempIDs.empty()==false) lBodyID = mlstTempIDs.front();
		mpEditor->AddAction(pJoint->CreateSetPropertyActionInt(eJointInt_ChildID, lBodyID));
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorWindowOutline, AttachChildBodyCallback);

//------------------------------------------------------------------------------------

bool cModelEditorWindowOutline::SetAttachmentsCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEntityWrapper* pEnt = GetSelectedEntity();

	if(pEnt)
	{
		mpEditor->AddAction(hplNew(cEditorActionEntityAttachChildren,(mpEditor->GetEditorWorld(), pEnt->GetID(), mlstTempIDs)));
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorWindowOutline, SetAttachmentsCallback);

//------------------------------------------------------------------------------------

cModelEditorWindowOutlineHelper* cModelEditorWindowOutline::CreateHelperWindow(tIntList& alstIDs, tGuiCallbackFunc apCallback, bool abMulti)
{
	cModelEditorWindowOutlineHelper* pHelper = hplNew(cModelEditorWindowOutlineHelper,(this, alstIDs) );
	pHelper->Init();
	mpEditor->AddWindow(pHelper);
	pHelper->SetCallback(this, apCallback);
	pHelper->SetActive(true);
	pHelper->SetMultiSelectionEnabled(abMulti);
	pHelper->SetPosition(mvPos + cVector3f(mpWindow->GetSize().x,0,10));

	return pHelper;
}

//------------------------------------------------------------------------------------

iEntityWrapper* cModelEditorWindowOutline::GetSelectedEntity()
{
	iEntityWrapper* pEnt = NULL;
	int lID=-1;
	bool bAllSameEntity = true;

	int lSize = mpListObjects->GetMultiSelectionNum();
	for(int i=0;i<lSize;++i)
	{
		int lIdx = mpListObjects->GetMultiSelection(i);
		if(lID!=-1 && lID!= mvIDList[lIdx])
		{
			bAllSameEntity = false;
			break;
		}
		lID = mvIDList[lIdx];
	}

	if(bAllSameEntity)
		pEnt = mpEditor->GetEditorWorld()->GetEntity(lID);

	return pEnt;
}

//------------------------------------------------------------------------------------
