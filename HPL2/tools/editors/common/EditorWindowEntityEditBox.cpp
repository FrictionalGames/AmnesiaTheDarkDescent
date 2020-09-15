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

#include "EditorWindowEntityEditBox.h"

#include "EditorWorld.h"
#include "EditorGrid.h"
#include "EntityWrapper.h"

#include "EditorSelection.h"
#include "EditorActionSelection.h"
#include "EditorActionEntity.h"
#include "EditorHelper.h"

#include <algorithm>


//----------------------------------------------------------------------------

int cEditorWindowEntityEditBox::mLastEditedType = -1;
int cEditorWindowEntityEditBox::mCurrentEditedType = -1;
int cEditorWindowEntityEditBox::mlLastTabOpened = -1;

//----------------------------------------------------------------------------

cEditorWindowEntityEditBox::cEditorWindowEntityEditBox(cEditorEditModeSelect* apEditMode,iEntityWrapper* apEntity) : iEditModeWindow(apEditMode)
{
	mpEditMode = apEditMode;
	mpEntity = apEntity;
	if(mpEntity)
		mCurrentEditedType = mpEntity->GetTypeID();
	else
		mCurrentEditedType = -1;
    
	mpTabs = NULL;

	mpInpName = NULL;
	mpInpTag = NULL;
	mpInpActive = NULL;
	mpInpPosition = NULL;
	mpInpScale = NULL;
	mpInpRotation = NULL;

	mpChildHighlighter = NULL;
}

cEditorWindowEntityEditBox::~cEditorWindowEntityEditBox()
{
	if(mpChildHighlighter)
	{
		hplDelete(mpChildHighlighter);
	}
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBox::SetUpGeneralTab()
{
	if(mpEntity==NULL)
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

void cEditorWindowEntityEditBox::AddPropertyName(cWidgetTab* apParentTab)
{
	mpInpName = CreateInputString(0, _W("Name"),"", apParentTab, 140);
	mpInpName->SetCanEdit(mpEntity->IsRenameable());
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBox::AddPropertyPosition(cWidgetTab* apParentTab)
{
	tWStringList lstLabels;
	lstLabels.push_back(_W("X"));
	lstLabels.push_back(_W("Y"));
	lstLabels.push_back(_W("Z"));

	mpInpPosition = CreateInputVec3(0, _W("Position"), "", apParentTab, 150, lstLabels, 
									eEditorInputLayoutStyle_ColumnLabelOnTop, mpEditor->GetGrid()->GetSnapSeparation());
	mpInpPosition->SetDecimals(3);
	mpInpPosition->SetCanEdit(mpEntity->CanTranslate());
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBox::AddPropertyRotation(cWidgetTab* apParentTab)
{
	tWStringList lstLabels;
	lstLabels.push_back(_W("X"));
	lstLabels.push_back(_W("Y"));
	lstLabels.push_back(_W("Z"));

	mpInpRotation = CreateInputVec3(0, _W("Rotation"), "", apParentTab, 150, lstLabels, 
									eEditorInputLayoutStyle_ColumnLabelOnTop,
									cMath::ToDeg(cEditorSelection::GetRotateSnap()));
	mpInpRotation->SetDecimals(3);
	mpInpRotation->SetCanEdit(mpEntity->CanRotate());
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBox::AddPropertyScale(cWidgetTab* apParentTab)
{
	tWStringList lstLabels;
	lstLabels.push_back(_W("X"));
	lstLabels.push_back(_W("Y"));
	lstLabels.push_back(_W("Z"));

	mpInpScale = CreateInputVec3(0, _W("Scale"), "", apParentTab, 150, lstLabels, 
									eEditorInputLayoutStyle_ColumnLabelOnTop, 
									cEditorSelection::GetScaleSnap());
	mpInpScale->SetDecimals(3);
	mpInpScale->SetCanEdit(mpEntity->CanScale());
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBox::AddPropertyActive(cWidgetTab* apParentTab)
{
	mpInpActive = CreateInputBool(0, _W("Active"), "", apParentTab);
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBox::AddPropertySetAttachments(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,30,0.1f);

	mpChildHighlighter = hplNew(cEntitySelectorHighlighter,(mpEditMode));
	mpChildHighlighter->SetMouseUpCallback(this, kGuiCallback(Child_OnPick));

	const tIntVec& vFilters = mpEntity->GetAttachableTypes();
	for(int i=0;i<(int)vFilters.size();++i)
	{
		iEntityWrapperType* pType = this->mpEditor->GetEditorWorld()->GetEntityTypeByID(vFilters[i]);
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, 20, _W(""), apParentTab);
		pButton->SetToggleable(true);
		pButton->SetUserValue(pType->GetID());
		pButton->SetText(cString::SubW(pType->GetName(),0,2));
		pButton->SetToolTip(pType->GetName());
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
		pButton->SetPressed(true);
		vPos.x+= 25;
		if(vPos.x >160)
		{
			vPos.y += 25;
			vPos.x = 15;
		}

		mvBAttachFilters.push_back(pButton);
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

bool cEditorWindowEntityEditBox::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lID = mpEntity->GetID();
	iEditorAction* pAction = NULL;
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	if(apWidget == mpBEditAttachments)
	{
		// TODO : This is quite temp, but works now
		if(mpEditMode->GetCurrentEntitySelector()==mpChildHighlighter)
		{
			mpEditMode->SetCurrentEntitySelector(NULL);
			mpBEditAttachments->SetText(_W("Attach"));
			if(mlstAttachedChildren!=mpEntity->GetChildren())
			{
				tIntList lstChildIDs;
				cEditorHelper::GetIDsFromEntityList(mlstAttachedChildren, lstChildIDs);
				pAction = hplNew( cEditorActionEntityAttachChildren,(pWorld, lID, lstChildIDs) );
			}

			mpChildHighlighter->Reset();
			mlstAttachedChildren.clear();
		}
		else
		{
			if(mpChildHighlighter->IsAtLeastOneFilterActive())
			{
				mpBEditAttachments->SetText(_W("Done"));
				mpEditMode->SetCurrentEntitySelector(mpChildHighlighter);
				mlstAttachedChildren = mpEntity->GetChildren();
				mpChildHighlighter->SetHighlightedEntities(mlstAttachedChildren);
			}
			else
			{
				mpEditor->ShowMessageBox(_W("Warning"), _W("You must activate at least one filter"), _W("OK"),_W(""),NULL,NULL);
			}
		}
	}
	else if(apWidget==mpBClearAttachments)
	{
		pAction = hplNew( cEditorActionEntityAttachChildren, (pWorld, lID, tIntList()));
	}
	else
	{
		for(int i=0;i<(int)mvBAttachFilters.size();++i)
		{
			cWidgetButton* pButton = mvBAttachFilters[i];
			if(apWidget==pButton)
			{
				mpChildHighlighter->GetFilter()->SetTypeFilter((eEditorEntityType)pButton->GetUserValue(), pButton->IsPressed());
				if(mpChildHighlighter->IsAtLeastOneFilterActive()==false)
				{
					mpEditor->ShowMessageBox(_W("Warning"), _W("You must activate at least one filter"), _W("OK"),_W(""),NULL,NULL);
					mpChildHighlighter->GetFilter()->SetTypeFilter((eEditorEntityType)i, true);
				}

				mpEditor->SetLayoutNeedsUpdate(true);
			}
		}
	}

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBox, InputCallback);

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void cEditorWindowEntityEditBox::OnInitLayout()
{
	mpBGFrame->SetSize(cVector2f(200,620));

	mpTabs = mpSet->CreateWidgetTabFrame(cVector3f(5,10,0.1f),cVector2f(190,600),_W(""), mpBGFrame);

	Create();
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBox::OnUpdate(float afTimeStep)
{
	if(mpEntity==NULL)return;

	if(mpInpName)
		mpInpName->SetValue(cString::To16Char(mpEntity->GetName()),false);
	if(mpInpTag)
		mpInpTag->SetValue(cString::To16Char(mpEntity->GetTag()), false);
	if(mpInpActive)
		mpInpActive->SetValue(mpEntity->IsActive(), false);

	if(mpInpPosition)
		mpInpPosition->SetValue(mpEntity->GetPosition(), false);
	if(mpInpRotation)
	{
		cVector3f vRot = mpEntity->GetRotation();
		vRot.x = cMath::ToDeg(vRot.x);
		vRot.y = cMath::ToDeg(vRot.y);
		vRot.z = cMath::ToDeg(vRot.z);
		mpInpRotation->SetValue(vRot, false);
	}
	if(mpInpScale)
		mpInpScale->SetValue(mpEntity->GetScale(), false);

	if(mpChildHighlighter)
	{
		for(int i=0;i<(int)mvBAttachFilters.size();++i)
		{
			cWidgetButton* pButton = mvBAttachFilters[i];
			if(pButton)
				pButton->SetPressed(mpChildHighlighter->GetFilter()->GetTypeFilter((eEditorEntityType)pButton->GetUserValue()), false);
		}
	}
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBox::OnSetActive(bool abX)
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

bool cEditorWindowEntityEditBox::Child_OnPick(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEntityWrapper* pEnt = mpChildHighlighter->GetPickedEntity();
	if(pEnt==NULL)
		return InputCallback(mpBEditAttachments, cGuiMessageData());

	tEntityWrapperListIt it = find(mlstAttachedChildren.begin(), mlstAttachedChildren.end(), pEnt);
	if(it!=mlstAttachedChildren.end())
	{
		mpChildHighlighter->RemoveHighlightedEntity(*it);
		mlstAttachedChildren.erase(it);
	}
	else
	{
		mpChildHighlighter->AddHighlightedEntity(pEnt);
		mlstAttachedChildren.push_back(pEnt);
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBox,Child_OnPick);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBox::WindowSpecificInputCallback(iEditorInput* apInput)
{
	tIntList lstEntityIDs;
	lstEntityIDs.push_back(mpEntity->GetID());
	iEditorAction* pAction = NULL;
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	if(apInput==mpInpName)
	{
		tString sName = cString::To8Char(mpInpName->GetValue());
		if(mpEntity->GetName()!=sName && pWorld->IsNameAvailable(sName)==false)
			mpEditor->ShowMessageBox(_W("Warning"), _W("There is already an object with same name"), _W("OK"), _W(""), NULL, NULL);
		else
            pAction = mpEntity->CreateSetPropertyActionString(eObjStr_Name, sName);
	}
	else if(apInput==mpInpTag)
	{
		tString sTag = cString::To8Char(mpInpTag->GetValue());
		pAction = mpEntity->CreateSetPropertyActionString(eObjStr_Tag, sTag);
	}
	else if(apInput==mpInpActive)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eObjBool_Active, mpInpActive->GetValue());
	}
	else if(apInput==mpInpPosition)
	{
		pAction = mpEntity->CreateModifyPositionAction(mpEditMode, mpInpPosition->GetValue());
	}
	else if(apInput==mpInpScale)
	{
		pAction = mpEntity->CreateModifyScaleAction(mpEditMode, mpInpScale->GetValue());
	}
	else if(apInput==mpInpRotation)
	{
		cVector3f vRot = mpInpRotation->GetValue();
		vRot.x = cMath::ToRad(vRot.x);
		vRot.y = cMath::ToRad(vRot.y);
		vRot.z = cMath::ToRad(vRot.z);

		pAction = mpEntity->CreateModifyRotationAction(mpEditMode, vRot);
	}
	else
		return false;

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxUserDefinedEntity::cEditorWindowEntityEditBoxUserDefinedEntity(cEditorEditModeSelect* apEditMode, iEntityWrapperUserDefinedEntity* apVarEntity) : cEditorWindowEntityEditBox(apEditMode, apVarEntity)
{
	mpEntity = apVarEntity;

	mpInputPanel = NULL;
	mbRefreshInputs = false;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxUserDefinedEntity::~cEditorWindowEntityEditBoxUserDefinedEntity()
{
	hplDelete(mpInputPanel);
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxUserDefinedEntity::AddInputs(cWidgetTab* apParentTab, const cVector3f& avStartPos)
{
	cVector3f vPos = cVector3f(4,4,0.1f);
	mpFVars = mpSet->CreateWidgetFrame(vPos, apParentTab->GetSize()-cVector2f(12,40), false, apParentTab);

	mpInputPanel = mpEntity->GetClass()->CreateInputPanel(this, mpFVars, false);
	mpInputPanel->SetCallback(this, VarInputCallbackStaticHelper);
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxUserDefinedEntity::VarInputCallback(iEditorVarInput* apInput)
{
	iEditorAction* pAction = mpEntity->CreateActionSetUserVariable(apInput->GetVar()->GetName(), apInput->GetInput()->GetValue());
	mpEditor->AddAction(pAction);

	return true;
}

bool cEditorWindowEntityEditBoxUserDefinedEntity::VarInputCallbackStaticHelper(void* apWin, iEditorVarInput* apInput)
{
	cEditorWindowEntityEditBoxUserDefinedEntity* pWin = (cEditorWindowEntityEditBoxUserDefinedEntity*)apWin;
	return pWin->VarInputCallback(apInput);
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxUserDefinedEntity::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);

	if(mbRefreshInputs)
	{
		mbRefreshInputs=false;
		hplDelete(mpInputPanel);

		mpInputPanel = mpEntity->GetClass()->CreateInputPanel(this, mpFVars, false);
		mpInputPanel->SetCallback(this, VarInputCallbackStaticHelper);
	}

	mpInputPanel->Update();
}

//----------------------------------------------------------------------------

