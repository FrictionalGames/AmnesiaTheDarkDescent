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

#include "../ModelEditor/EdEditPaneJoint.h"

#include "../ModelEditor/EdObjJointBall.h"
#include "../ModelEditor/EdObjJointHinge.h"
#include "../ModelEditor/EdObjJointScrew.h"
#include "../ModelEditor/EdObjJointSlider.h"

#include "../ModelEditor/EdObjBody.h"

#include "../Common/Editor.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// JOINT EDIT BOX
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneJoint::cEdEditPaneJoint(iEdObject* apObject) : iEdScnObjEditPane(apObject)
{
	mpCurrentBodyHighlighter = NULL;
}

//----------------------------------------------------------------------------

cEdEditPaneJoint::~cEdEditPaneJoint()
{
	
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneJoint::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabJointBase = mpTabs->AddTab(_W("Joint - Base"));
	
	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y;

    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddPropertySetJointBase(mpTabJointBase);
	const tWString& sType = mpObject->GetTypeName();
	if(sType==_W("Ball"))
	{
		mpTabSpecific = mpTabs->AddTab(_W("Ball Params"));
		AddPropertySetBall(mpTabSpecific);
	}
	else if(sType==_W("Hinge"))
	{
		mpTabSpecific = mpTabs->AddTab(_W("Hinge Params"));
		AddPropertySetHinge(mpTabSpecific);
	}
	else if(sType==_W("Screw"))
	{
		mpTabSpecific = mpTabs->AddTab(_W("Screw Params"));
		AddPropertySetScrewSlider(mpTabSpecific);
	}
	else if(sType==_W("Slider"))
	{
		mpTabSpecific = mpTabs->AddTab(_W("Slider Params"));
		AddPropertySetScrewSlider(mpTabSpecific);
	}

	mpTabJointSounds = mpTabs->AddTab(_W("Joint - Sounds"));
	AddPropertySetJointSounds(mpTabJointSounds);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneJoint::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();
	iEdObjJoint* pJoint = static_cast<iEdObjJoint*>(mpObject);

	////////////////////////////////////////////
	// Joint base inputs
	mpInpStickyMinLimit->SetValue(pJoint->GetStickyMinLimit(), false);
	mpInpStickyMaxLimit->SetValue(pJoint->GetStickyMaxLimit(), false);

	mpInpMoveType->SetValue(cString::To16Char(pJoint->GetMoveType()), false);
	mpInpMoveSound->SetValue(cString::To16Char(pJoint->GetMoveSound()), false);

	mpInpMinMoveSpeed->SetValue(pJoint->GetMinMoveSpeed(), false);
	mpInpMinMoveFreq->SetValue(pJoint->GetMinMoveFreq(), false);
	mpInpMinMoveFreqSpeed->SetValue(pJoint->GetMinMoveFreqSpeed(), false);
	mpInpMinMoveVolume->SetValue(pJoint->GetMinMoveVolume(), false);
	
	mpInpMaxMoveSpeed->SetValue(pJoint->GetMaxMoveSpeed(), false);
	mpInpMaxMoveFreq->SetValue(pJoint->GetMaxMoveFreq(), false);
	mpInpMaxMoveFreqSpeed->SetValue(pJoint->GetMaxMoveFreqSpeed(), false);
	mpInpMaxMoveVolume->SetValue(pJoint->GetMaxMoveVolume(), false);
	
	mpInpMiddleMoveSpeed->SetValue(pJoint->GetMiddleMoveSpeed(), false);
	mpInpMiddleMoveVolume->SetValue(pJoint->GetMiddleMoveVolume(), false);

	mpInpMinLimitSound->SetValue(cString::To16Char(pJoint->GetMinLimitSound()), false);
	mpInpMaxLimitSound->SetValue(cString::To16Char(pJoint->GetMaxLimitSound()), false);

	mpInpMinLimitMinSpeed->SetValue(pJoint->GetMinLimitMinSpeed(), false);
	mpInpMinLimitMaxSpeed->SetValue(pJoint->GetMinLimitMaxSpeed(), false);
	mpInpMaxLimitMinSpeed->SetValue(pJoint->GetMaxLimitMinSpeed(), false);
	mpInpMaxLimitMaxSpeed->SetValue(pJoint->GetMaxLimitMaxSpeed(), false);

	mpInpLimitStepCount->SetValue((float)pJoint->GetLimitStepCount(), false);

	mpInpBreakable->SetValue(pJoint->IsBreakable(), false);
	mpInpBreakForce->SetValue(pJoint->GetBreakForce(), false);
	mpInpBreakSound->SetValue(cString::To16Char(pJoint->GetBreakSound()), false);

	mpInpCollideBodies->SetValue(pJoint->GetCollideBodies(), false);
	
	////////////////////////////////////////////
	// Specific inputs
	const tWString& sType = pJoint->GetTypeName();
	if(sType==_W("Ball"))
	{
		cEdObjJointBall* pBall = static_cast<cEdObjJointBall*>(pJoint);
		mpInpMaxConeAngle->SetValue(pBall->GetMaxConeAngle(), false);
		mpInpMaxTwistAngle->SetValue(pBall->GetMaxTwistAngle(), false);
	}
	else if(sType==_W("Hinge"))
	{
		cEdObjJointHinge* pHinge = static_cast<cEdObjJointHinge*>(pJoint);
		mpInpMinAngle->SetValue(pHinge->GetMinAngle(), false);
		mpInpMaxAngle->SetValue(pHinge->GetMaxAngle(), false);
	}
	else if(sType==_W("Screw"))
	{
		cEdObjJointScrew* pScrew = static_cast<cEdObjJointScrew*>(pJoint);
		mpInpMinDistance->SetValue(pScrew->GetMinDistance(), false);
		mpInpMaxDistance->SetValue(pScrew->GetMaxDistance(), false);
	}
	else if(sType==_W("Slider"))
	{
		cEdObjJointSlider* pSlider = static_cast<cEdObjJointSlider*>(pJoint);
		mpInpMinDistance->SetValue(pSlider->GetMinDistance(), false);
		mpInpMaxDistance->SetValue(pSlider->GetMaxDistance(), false);
	}
}

//----------------------------------------------------------------------------

void cEdEditPaneJoint::AddPropertySetJointBase(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,10,0.1f);
	float fSmallSep = 5;
	float fLargeSep = 10;

	////////////////////////////////////////////
	// Joint base inputs
	for(int i=0;i<2; ++i)
	{
		mvButtons[i] = mpSet->CreateWidgetButton( vPos + cVector3f(0,(float)i*30,0), cVector2f(160,25), _W(""), apParentTab, true);
		mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));
	}

	mvButtons[0]->SetText(_W("Set Parent Body"));
	mvButtons[1]->SetText(_W("Set Child Body"));

	vPos.y += 60;

	mpInpStickyMinLimit = CreateInputBool(vPos, _W("Sticky Min Limit"), apParentTab);
	vPos.y += mpInpStickyMinLimit->GetSize().y+fSmallSep;

	mpInpStickyMaxLimit = CreateInputBool(vPos, _W("Sticky Max Limit"), apParentTab);
	vPos.y += mpInpStickyMaxLimit->GetSize().y+fLargeSep;

	mpInpLimitStepCount = CreateInputNumber(vPos, _W("Limit Step Count"), apParentTab, 50, 1);
	mpInpLimitStepCount->SetDecimals(0);
	vPos.y += mpInpLimitStepCount->GetSize().y + fLargeSep;

	mpInpCollideBodies = CreateInputBool(vPos, _W("Collide Bodies"), apParentTab);
	vPos.y += mpInpCollideBodies->GetSize().y + fLargeSep;

	mpInpBreakable = CreateInputBool(vPos, _W("Breakable"), apParentTab);
	vPos.y += mpInpBreakable->GetSize().y + fSmallSep;

	mpInpBreakForce = CreateInputNumber(vPos, _W("Break Force"), apParentTab, 50, 0.5f);
}

//----------------------------------------------------------------------------

void cEdEditPaneJoint::AddPropertySetJointSounds(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,10,0.1f);
	float fSmallSep = 5;
	float fLargeSep = 10;

	////////////////////////////////////////////
	// Joint sound inputs
	mpInpMoveType = CreateInputEnum(vPos, _W("Move Type"), apParentTab);
	mpInpMoveType->AddValue(_W("Angular"));
	mpInpMoveType->AddValue(_W("Linear"));

	vPos.y += mpInpMoveType->GetSize().y + fLargeSep;

	mpInpMoveSound = CreateInputSound(vPos, _W("Move Sound"), apParentTab);
	vPos.y += mpInpMoveSound->GetSize().y + fLargeSep;

	// Min
	cWidgetLabel* pLabel = mpSet->CreateWidgetLabel(vPos, 0, _W("Min"), apParentTab);
    
	vPos.y += 18;

	mpInpMinMoveSpeed = CreateInputNumber(vPos, _W("Speed"), apParentTab);
	mpInpMinMoveFreq = CreateInputNumber(vPos + cVector3f(mpInpMinMoveSpeed->GetSize().x+5,0,0), _W("Freq"), apParentTab);

	vPos.y += mpInpMinMoveSpeed->GetSize().y + fSmallSep;

	mpInpMinMoveFreqSpeed = CreateInputNumber(vPos, _W("Freq Speed"), apParentTab, 50, 0.5f);
	mpInpMinMoveVolume = CreateInputNumber(vPos + cVector3f(mpInpMinMoveFreqSpeed->GetSize().x+5,0,0), _W("Volume"), apParentTab, 50, 0.5f);

	vPos.y += mpInpMinMoveFreqSpeed->GetSize().y + fLargeSep;

	// Max
	pLabel = mpSet->CreateWidgetLabel(vPos, 0, _W("Max"), apParentTab);

	vPos.y += 18;

	mpInpMaxMoveSpeed = CreateInputNumber(vPos, _W("Speed"), apParentTab);
	mpInpMaxMoveFreq = CreateInputNumber(vPos + cVector3f(mpInpMaxMoveSpeed->GetSize().x+5,0,0), _W("Freq"), apParentTab);

	vPos.y += mpInpMaxMoveSpeed->GetSize().y + fSmallSep;

	mpInpMaxMoveFreqSpeed = CreateInputNumber(vPos, _W("Freq Speed"), apParentTab, 50, 0.5f);
	mpInpMaxMoveVolume = CreateInputNumber(vPos + cVector3f(mpInpMaxMoveFreqSpeed->GetSize().x+5,0,0), _W("Volume"), apParentTab, 50, 0.5f);

	vPos.y += mpInpMaxMoveFreqSpeed->GetSize().y + fLargeSep;

	// Middle
	pLabel = mpSet->CreateWidgetLabel(vPos, 0, _W("Middle"), apParentTab);

	vPos.y += 18;
	
	mpInpMiddleMoveSpeed = CreateInputNumber(vPos, _W("Speed"), apParentTab, 50, 0.5f);
	mpInpMiddleMoveVolume = CreateInputNumber(vPos + cVector3f(mpInpMiddleMoveSpeed->GetSize().x+5,0,0), _W("Volume"), apParentTab, 50, 0.5f); 

	vPos.y += mpInpMiddleMoveSpeed->GetSize().y + fSmallSep;

	// Break
	mpInpBreakSound = CreateInputSound(vPos, _W("Break Sound"), apParentTab);

	vPos.y += mpInpBreakSound->GetSize().y + fLargeSep;

	// Min Limit
	mpInpMinLimitSound = CreateInputSound(vPos, _W("Min Limit Sound"), apParentTab);

	vPos.y += mpInpMinLimitSound->GetSize().y + fSmallSep;

	mpInpMinLimitMinSpeed = CreateInputNumber(vPos, _W("Min Speed"), apParentTab, 50, 0.5f);
	mpInpMinLimitMaxSpeed = CreateInputNumber(vPos + cVector3f(mpInpMinLimitMinSpeed->GetSize().x + 5, 0, 0), _W("Max Speed"), apParentTab, 50, 0.5f);

	vPos.y += mpInpMinLimitMinSpeed->GetSize().y + fLargeSep;

	// Max Limit
	mpInpMaxLimitSound = CreateInputSound(vPos, _W("Max Limit Sound"), apParentTab);

	vPos.y += mpInpMaxLimitSound->GetSize().y + fSmallSep;

	mpInpMaxLimitMinSpeed = CreateInputNumber(vPos, _W("Min Speed"), apParentTab, 50, 0.5f);
	mpInpMaxLimitMaxSpeed = CreateInputNumber(vPos + cVector3f(mpInpMinLimitMinSpeed->GetSize().x + 5, 0, 0), _W("Max Speed"), apParentTab, 50, 0.5f);
}

//----------------------------------------------------------------------------

void cEdEditPaneJoint::AddPropertySetBall(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,10,0.1f);
	float fLargeSep = 10;

	mpInpMaxConeAngle = CreateInputNumber(vPos, _W("Max Cone Angle"), apParentTab, 50, 15);
	vPos.y += mpInpMaxConeAngle->GetSize().y + fLargeSep;

	mpInpMaxTwistAngle = CreateInputNumber(vPos, _W("Max Twist Angle"), apParentTab, 50, 15);
}

//----------------------------------------------------------------------------

void cEdEditPaneJoint::AddPropertySetHinge(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,10,0.1f);
	float fLargeSep = 10;

	mpInpMinAngle = CreateInputNumber(vPos, _W("Min Angle"), apParentTab, 50, 15);
	vPos.y += mpInpMinAngle->GetSize().y + fLargeSep;

	mpInpMaxAngle = CreateInputNumber(vPos, _W("Max Angle"), apParentTab, 50, 15);
}

//----------------------------------------------------------------------------

void cEdEditPaneJoint::AddPropertySetScrewSlider(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,10,0.1f);
	float fLargeSep = 10;

	mpInpMinDistance = CreateInputNumber(vPos, _W("Min Angle"), apParentTab, 50, 0.1f);
	vPos.y += mpInpMinDistance->GetSize().y + fLargeSep;

	mpInpMaxDistance = CreateInputNumber(vPos, _W("Max Angle"), apParentTab, 50, 0.1f);
}

//----------------------------------------------------------------------------

bool cEdEditPaneJoint::Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mpCurrentBodyHighlighter)
	{
		cWidgetButton* pButton = static_cast<cWidgetButton*>(apWidget);
		bool bPressed = pButton->IsPressed();
		pButton->SetPressed(!bPressed, false);

		return true;
	}

	mpCurrentBodyHighlighter = hplNew(cEdSelectorHighlighter,(mpSelect));
	cUIPickFilter* pFilter = mpCurrentBodyHighlighter->GetFilter();
	iEdScnObject* pJoint = static_cast<iEdScnObject*>(mpObject);
	const tScnObjTypeVec& vAttachableTypes = pJoint->GetAttachableTypes();
	for(size_t i=0;i<vAttachableTypes.size();++i)
	{
		pFilter->SetTypeFilter(vAttachableTypes[i], true);
	}

	if(apWidget == mvButtons[0])
	{
		mpCurrentBodyHighlighter->SetMouseUpCallback(this, kGuiCallback(SetParentBodyCallback));
	}
	else if(apWidget == mvButtons[1])
	{
		mpCurrentBodyHighlighter->SetMouseUpCallback(this, kGuiCallback(SetChildBodyCallback));
	}

	mpSelect->SetCurrentSelector(mpCurrentBodyHighlighter);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneJoint, Button_OnPressed);

//----------------------------------------------------------------------------

bool cEdEditPaneJoint::SetParentBodyCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	AttachBodyToJoint(eJointInt_ParentID);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneJoint, SetParentBodyCallback);

//----------------------------------------------------------------------------

bool cEdEditPaneJoint::SetChildBodyCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	AttachBodyToJoint(eJointInt_ChildID);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneJoint, SetChildBodyCallback);

//----------------------------------------------------------------------------

void cEdEditPaneJoint::AttachBodyToJoint(int alBodyType)
{
	cWidgetButton* pButton = NULL;

	if(mpCurrentBodyHighlighter)
	{
		iEdObjJoint* pJoint = static_cast<iEdObjJoint*>(mpObject);

		///////////////////////////////////////////////////////////
		// Get old and new bodies
		cEdObjBody* pOldBody = NULL;
		cEdObjBody* pOtherBody = NULL;
		switch(alBodyType)
		{
		case eJointInt_ParentID:
			pButton = mvButtons[0];
			pOldBody = pJoint->GetParentBody();
			pOtherBody = pJoint->GetChildBody();
			break;
		case eJointInt_ChildID:
			pButton = mvButtons[1];
			pOldBody = pJoint->GetChildBody();
			pOtherBody = pJoint->GetParentBody();
			break;
		}
		
		cEdObjBody* pNewBody = static_cast<cEdObjBody*>(mpCurrentBodyHighlighter->GetPickedEntity());

		///////////////////////////////////////////////////////////
		// If old body and new body are the same, nothing to do
		if(pOldBody!=pNewBody)
		{

			///////////////////////////////////////////////////////////
			// If parent and child bodies are the same, show message and exit
			if(pNewBody!=NULL && pNewBody==pOtherBody)
			{
				mpSet->CreatePopUpMessageBox( _W("Warning"), _W("Parent and child bodies must be different"), _W("Continue"), _W(""), this, NULL );
			}
			else
			{
				int lBodyID = -1;
				if(pNewBody) lBodyID = pNewBody->GetID();
				
				iEdAction* pAction = mpObject->CreateSetIntAction(alBodyType, lBodyID);
				mpEditor->AddAction(pAction);
			}
		}
	}

	mpSelect->SetCurrentSelector(NULL);

	hplDelete(mpCurrentBodyHighlighter);
	mpCurrentBodyHighlighter = NULL;

	if(pButton) pButton->SetPressed(false,false);
}

//----------------------------------------------------------------------------

bool cEdEditPaneJoint::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdAction* pAction = NULL;

	if(apInput==mpInpMoveType)
	{
		pAction = mpObject->CreateSetStringAction(eJointStr_MoveType, cString::To8Char(mpInpMoveType->GetSelectedEnumValue()));
	}
	//////////////////////////////////
	// Sticky Min Limit
	else if(apInput == mpInpStickyMinLimit)
	{
		pAction = mpObject->CreateSetBoolAction(eJointBool_StickyMinLimit, mpInpStickyMinLimit->GetValue());
	}
	//////////////////////////////////
	// Sticky Max Limit
	else if(apInput == mpInpStickyMaxLimit)
	{
		pAction = mpObject->CreateSetBoolAction(eJointBool_StickyMaxLimit, mpInpStickyMaxLimit->GetValue());
	}
	//////////////////////////////////
	// Move Sound
	else if(apInput == mpInpMoveSound)
	{
		pAction = mpObject->CreateSetStringAction(eJointStr_MoveSound, mpInpMoveSound->GetValue8Char());
	}
	//////////////////////////////////
	// Min Limit Sound
	else if(apInput == mpInpMinLimitSound)
	{
		pAction = mpObject->CreateSetStringAction(eJointStr_MoveSound, mpInpMoveSound->GetValue8Char());
	}
	//////////////////////////////////
	// Max Limit Sound
	else if(apInput == mpInpMaxLimitSound)
	{
		pAction = mpObject->CreateSetStringAction(eJointStr_MaxLimitSound, mpInpMaxLimitSound->GetValue8Char());
	}
	//////////////////////////////////
	// Min Limit Min Speed
	else if(apInput == mpInpMinLimitMinSpeed)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MinLimitMinSpeed, mpInpMinLimitMinSpeed->GetValue());
	}
	//////////////////////////////////
	// Min Limit Max Speed
	else if(apInput == mpInpMinLimitMaxSpeed)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MinLimitMaxSpeed, mpInpMinLimitMaxSpeed->GetValue());
	}
	//////////////////////////////////
	// Max Limit Min Speed
	else if(apInput == mpInpMaxLimitMinSpeed)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MaxLimitMinSpeed, mpInpMaxLimitMinSpeed->GetValue());
	}
	//////////////////////////////////
	// Max Limit Max Speed
	else if(apInput == mpInpMaxLimitMaxSpeed)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MaxLimitMaxSpeed, mpInpMaxLimitMaxSpeed->GetValue());
	}
	//////////////////////////////////
	// Min Move Speed
	else if(apInput == mpInpMinMoveSpeed)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MinMoveSpeed, mpInpMinMoveSpeed->GetValue());
	}
	//////////////////////////////////
	// Min Move Freq
	else if(apInput == mpInpMinMoveFreq)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MinMoveFreq, mpInpMinMoveFreq->GetValue());
	}
	//////////////////////////////////
	// Min Move Freq Speed
	else if(apInput == mpInpMinMoveFreqSpeed)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MinMoveFreqSpeed, mpInpMinMoveFreqSpeed->GetValue());
	}
	//////////////////////////////////
	// Min Move Volume
	else if(apInput == mpInpMinMoveVolume)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MinMoveVolume, mpInpMinMoveVolume->GetValue());
	}
	//////////////////////////////////
	// Max Move Speed
	else if(apInput == mpInpMaxMoveSpeed)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MaxMoveSpeed, mpInpMaxMoveSpeed->GetValue());
	}
	//////////////////////////////////
	// Max Move Freq
	else if(apInput == mpInpMaxMoveFreq)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MaxMoveFreq, mpInpMaxMoveFreq->GetValue());
	}
	//////////////////////////////////
	// Max Move Freq Speed
	else if(apInput == mpInpMaxMoveFreqSpeed)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MaxMoveFreqSpeed, mpInpMaxMoveFreqSpeed->GetValue());
	}
	//////////////////////////////////
	// Max Move Volume
	else if(apInput == mpInpMaxMoveVolume)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MaxMoveVolume, mpInpMaxMoveVolume->GetValue());
	}
	//////////////////////////////////
	// Middle Move Speed
	else if(apInput == mpInpMiddleMoveSpeed)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MiddleMoveSpeed, mpInpMiddleMoveSpeed->GetValue());
	}
	//////////////////////////////////
	// Middle Move Volume
	else if(apInput == mpInpMiddleMoveVolume)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_MiddleMoveVolume, mpInpMiddleMoveVolume->GetValue());
	}
	//////////////////////////////////
	// Limit Step Count
	else if(apInput == mpInpLimitStepCount)
	{
		pAction = mpObject->CreateSetIntAction(eJointInt_LimitAutoSleepNumSteps, (int)mpInpLimitStepCount->GetValue());
	}
	//////////////////////////////////
	// Collide bodies
	else if(apInput == mpInpCollideBodies)
	{
		pAction = mpObject->CreateSetBoolAction(eJointBool_CollideBodies, mpInpCollideBodies->GetValue());
	}
	//////////////////////////////////
	// Breakable
	else if(apInput == mpInpBreakable)
	{
		pAction = mpObject->CreateSetBoolAction(eJointBool_Breakable, mpInpBreakable->GetValue());
	}
	//////////////////////////////////
	// Break Force
	else if(apInput == mpInpBreakForce)
	{
		pAction = mpObject->CreateSetFloatAction(eJointFloat_BreakForce, mpInpBreakForce->GetValue());
	}
	//////////////////////////////////
	// Break Sound
	else if(apInput == mpInpBreakSound)
	{
		pAction = mpObject->CreateSetStringAction(eJointStr_BreakSound, mpInpBreakSound->GetValue8Char());
	}
	//////////////////////////////////
	// Ball : Max Cone Angle
	else if(apInput == mpInpMaxConeAngle)
	{
		pAction = mpObject->CreateSetFloatAction(eJointBallFloat_MaxConeAngle, mpInpMaxConeAngle->GetValue());
	}
	//////////////////////////////////
	// Ball : Max Twist Angle
	else if(apInput == mpInpMaxTwistAngle)
	{
		pAction = mpObject->CreateSetFloatAction(eJointBallFloat_MaxTwistAngle, mpInpMaxTwistAngle->GetValue());
	}	
	//////////////////////////////////
	// Hinge : Min Angle
	else if(apInput == mpInpMinAngle)
	{
		pAction = mpObject->CreateSetFloatAction(eJointHingeFloat_MinAngle, mpInpMinAngle->GetValue());
	}
	//////////////////////////////////
	// Hinge : Max Angle
	else if(apInput == mpInpMaxAngle)
	{
		pAction = mpObject->CreateSetFloatAction(eJointHingeFloat_MaxAngle, mpInpMaxAngle->GetValue());
	}
	//////////////////////////////////
	// Screw & Slider : Min Distance
	else if(apInput == mpInpMinDistance)
	{
		int prop = -1;
		
		if(mpObject->GetTypeName()==_W("Screw"))
			prop = eJointScrewFloat_MinDistance;
		else
			prop = eJointSliderFloat_MinDistance;
			
		pAction = mpObject->CreateSetFloatAction(prop, mpInpMinDistance->GetValue());
	}
	//////////////////////////////////
	// Screw & Slider : Max Distance
	else if(apInput == mpInpMaxDistance)
	{
		int prop = -1;
		
		if(mpObject->GetTypeName()==_W("Screw"))
			prop = eJointScrewFloat_MaxDistance;
		else
			prop = eJointSliderFloat_MaxDistance;

		pAction = mpObject->CreateSetFloatAction(prop, mpInpMaxDistance->GetValue());
	}
	else
	{
		return iEdScnObjEditPane::WindowSpecificInputCallback(apInput);
	}

	AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
