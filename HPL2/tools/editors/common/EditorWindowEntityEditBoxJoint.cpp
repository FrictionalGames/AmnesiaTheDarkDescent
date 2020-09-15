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

#include "EditorWindowEntityEditBoxJoint.h"

#include "EntityWrapperBody.h"
#include "EntityWrapperJoint.h"
#include "EntityWrapperJointBall.h"
#include "EntityWrapperJointHinge.h"
#include "EntityWrapperJointScrew.h"
#include "EntityWrapperJointSlider.h"

#include "EditorEditModeSelect.h"

#include "EditorWindowViewport.h"
#include "EditorWorld.h"

#include "EditorActionsBodies.h"

#include "EditorInput.h"


//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// JOINT EDIT BOX
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxJoint::cEditorWindowEntityEditBoxJoint(cEditorEditModeSelect* apEditMode,iEntityWrapperJoint* apObject) : cEditorWindowEntityEditBox(apEditMode,apObject)
{
	mpEntity = apObject;

	mpCurrentBodyHighlighter = NULL;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxJoint::~cEditorWindowEntityEditBoxJoint()
{
	
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxJoint::Create()
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
	switch(mpEntity->GetJointType())
	{
	case eEditorEntityJointType_Ball:
        mpTabSpecific = mpTabs->AddTab(_W("Ball Params"));
		AddPropertySetBall(mpTabSpecific);
		break;
	case eEditorEntityJointType_Hinge:
		mpTabSpecific = mpTabs->AddTab(_W("Hinge Params"));
		AddPropertySetHinge(mpTabSpecific);
		break;
	case eEditorEntityJointType_Screw:
		mpTabSpecific = mpTabs->AddTab(_W("Screw Params"));
		AddPropertySetScrewSlider(mpTabSpecific);
		break;
	case eEditorEntityJointType_Slider:
		mpTabSpecific = mpTabs->AddTab(_W("Slider Params"));
		AddPropertySetScrewSlider(mpTabSpecific);
		break;
	default:
		break;
	}

	mpTabJointSounds = mpTabs->AddTab(_W("Joint - Sounds"));
	AddPropertySetJointSounds(mpTabJointSounds);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxJoint::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);

	////////////////////////////////////////////
	// Joint base inputs
	mpCheckBoxStickyMinLimit->SetChecked(mpEntity->GetStickyMinLimit(), false);
	mpCheckBoxStickyMaxLimit->SetChecked(mpEntity->GetStickyMaxLimit(), false);

	mpInputMoveSound->SetText(cString::To16Char(mpEntity->GetMoveSound()));
	
	{
		tWString sValue = cString::To16Char(mpEntity->GetMoveType());
		for(int i=0;i<mpInpMoveType->GetNumValues();++i)
		{
			if(sValue==mpInpMoveType->GetEnumValue(i))
			{
				mpInpMoveType->SetValue(i, false);
				break;
			}
		}
	}

	mpInputMinMoveSpeed->SetNumericValue(mpEntity->GetMinMoveSpeed());
	mpInputMinMoveFreq->SetNumericValue(mpEntity->GetMinMoveFreq());
	mpInputMinMoveFreqSpeed->SetNumericValue(mpEntity->GetMinMoveFreqSpeed());
	mpInputMinMoveVolume->SetNumericValue(mpEntity->GetMinMoveVolume());
	
	mpInputMaxMoveSpeed->SetNumericValue(mpEntity->GetMaxMoveSpeed());
	mpInputMaxMoveFreq->SetNumericValue(mpEntity->GetMaxMoveFreq());
	mpInputMaxMoveFreqSpeed->SetNumericValue(mpEntity->GetMaxMoveFreqSpeed());
	mpInputMaxMoveVolume->SetNumericValue(mpEntity->GetMaxMoveVolume());
	
	mpInputMiddleMoveSpeed->SetNumericValue(mpEntity->GetMiddleMoveSpeed());
	mpInputMiddleMoveVolume->SetNumericValue(mpEntity->GetMiddleMoveVolume());

	mpInputMinLimitSound->SetText(cString::To16Char(mpEntity->GetMinLimitSound()));
	mpInputMaxLimitSound->SetText(cString::To16Char(mpEntity->GetMaxLimitSound()));

	mpInputMinLimitMinSpeed->SetNumericValue(mpEntity->GetMinLimitMinSpeed());
	mpInputMinLimitMaxSpeed->SetNumericValue(mpEntity->GetMinLimitMaxSpeed());
	mpInputMaxLimitMinSpeed->SetNumericValue(mpEntity->GetMaxLimitMinSpeed());
	mpInputMaxLimitMaxSpeed->SetNumericValue(mpEntity->GetMaxLimitMaxSpeed());

	mpInputLimitStepCount->SetNumericValue((float)mpEntity->GetLimitStepCount());

	mpCheckBoxBreakable->SetChecked(mpEntity->IsBreakable(), false);
	mpInputBreakForce->SetNumericValue(mpEntity->GetBreakForce());
	mpInputBreakSound->SetText(cString::To16Char(mpEntity->GetBreakSound()));

	mpCheckBoxCollideBodies->SetChecked(mpEntity->GetCollideBodies(), false);
	

	////////////////////////////////////////////
	// Specific inputs
	switch(mpEntity->GetJointType())
	{
	case eEditorEntityJointType_Ball:
		mpInputMaxConeAngle->SetNumericValue(((cEntityWrapperJointBall*)mpEntity)->GetMaxConeAngle());
		mpInputMaxTwistAngle->SetNumericValue(((cEntityWrapperJointBall*)mpEntity)->GetMaxTwistAngle());
		break;
	case eEditorEntityJointType_Hinge:
		mpInputMinAngle->SetNumericValue(((cEntityWrapperJointHinge*)mpEntity)->GetMinAngle());
		mpInputMaxAngle->SetNumericValue(((cEntityWrapperJointHinge*)mpEntity)->GetMaxAngle());
		break;
	case eEditorEntityJointType_Screw:
		mpInputMinDistance->SetNumericValue(((cEntityWrapperJointScrew*)mpEntity)->GetMinDistance());
		mpInputMaxDistance->SetNumericValue(((cEntityWrapperJointScrew*)mpEntity)->GetMaxDistance());
		break;
	case eEditorEntityJointType_Slider:
		mpInputMinDistance->SetNumericValue(((cEntityWrapperJointSlider*)mpEntity)->GetMinDistance());
		mpInputMaxDistance->SetNumericValue(((cEntityWrapperJointSlider*)mpEntity)->GetMaxDistance());
		break;
	default:
		break;
	}
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxJoint::AddPropertySetJointBase(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,30,0.1f);
	float fAddNearLabel = 18;
	float fAddNear = 20;
	float fAddFar = 25;

	////////////////////////////////////////////
	// Joint base inputs
	for(int i=0;i<2; ++i)
	{
		mvButtons[i] = mpSet->CreateWidgetButton( vPos + cVector3f(0,(float)i*30,0), cVector2f(160,25), _W(""), apParentTab);
		mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));
	}

	mvButtons[0]->SetText(_W("Set Parent Body"));
	mvButtons[1]->SetText(_W("Set Child Body"));

	vPos.y += 60;

	mpCheckBoxStickyMinLimit = mpSet->CreateWidgetCheckBox(vPos, 0, _W("Sticky Min Limit"), apParentTab);

	mpCheckBoxStickyMinLimit->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));
	
	vPos.y += fAddNear;

	mpCheckBoxStickyMaxLimit = mpSet->CreateWidgetCheckBox(vPos, 0, _W("Sticky Max Limit"), apParentTab);

	mpCheckBoxStickyMaxLimit->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));

	vPos.y += fAddFar;
	
	mpLabelLimitStepCount = mpSet->CreateWidgetLabel(vPos, 0, _W("Limit Step Count"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputLimitStepCount = mpSet->CreateWidgetTextBox(vPos, cVector2f(120,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,1);
	mpInputLimitStepCount->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputLimitStepCount->SetDecimals(0);


	vPos.y += fAddFar;
	
	mpCheckBoxCollideBodies = mpSet->CreateWidgetCheckBox(vPos, cVector2f(120,25), _W("Collide Bodies"), apParentTab);
	mpCheckBoxCollideBodies->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));


	vPos.y += fAddFar;
	
	mpCheckBoxBreakable = mpSet->CreateWidgetCheckBox(vPos, cVector2f(120,25), _W("Breakable"), apParentTab);
	mpCheckBoxBreakable->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));


	vPos.y += fAddNear;

	mpLabelBreakForce = mpSet->CreateWidgetLabel(vPos, 0, _W("Break Force"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputBreakForce = mpSet->CreateWidgetTextBox(vPos, cVector2f(120,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputBreakForce->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputBreakForce->SetDecimals(3);
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxJoint::AddPropertySetJointSounds(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,30,0.1f);
	float fAddNearLabel = 18;
	float fAddNear = 20;
	float fAddFar = 25;

	////////////////////////////////////////////
	// Joint sound inputs
	mpInpMoveType = CreateInputEnum(vPos, _W("Move Type"), "", tWStringList(), apParentTab);
	mpInpMoveType->AddValue(_W("Angular"));
	mpInpMoveType->AddValue(_W("Linear"));

	vPos.y += mpInpMoveType->GetSize().y;
    

	cWidgetLabel* pLabel = NULL;

	mpLabelMoveSound = mpSet->CreateWidgetLabel(vPos, 0, _W("Move Sound"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputMoveSound = mpSet->CreateWidgetTextBox(vPos, cVector2f(120,25), _W(""), apParentTab);
	mpInputMoveSound->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

	mpButtonMoveSoundBrowse = mpSet->CreateWidgetButton(vPos + cVector3f(123,0,0), cVector2f(20), _W("..."), apParentTab);

	mpButtonMoveSoundBrowse->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(BrowseButton_OnPressed));

	vPos.y += fAddFar;

	pLabel = mpSet->CreateWidgetLabel(vPos, 0, _W("Min"), apParentTab);


	vPos.y += fAddNearLabel;

	mpLabelMinMoveSpeed = mpSet->CreateWidgetLabel(vPos, 0, _W("Speed"), apParentTab);

	vPos.x += 75;
	mpLabelMinMoveFreq = mpSet->CreateWidgetLabel(vPos, 0, _W("Freq"), apParentTab);


	vPos.x = 15;
	vPos.y+=fAddNearLabel;

	mpInputMinMoveSpeed = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMinMoveSpeed->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMinMoveSpeed->SetDecimals(3);

	vPos.x += 75;
	mpInputMinMoveFreq = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMinMoveFreq->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMinMoveFreq->SetDecimals(3);


	vPos.x = 15;
	vPos.y += fAddFar;

	mpLabelMinMoveFreqSpeed = mpSet->CreateWidgetLabel(vPos, 0, _W("Freq Speed"), apParentTab);

	vPos.x += 75;
	mpLabelMinMoveVolume = mpSet->CreateWidgetLabel(vPos, 0, _W("Volume"), apParentTab);

	
	vPos.x = 15;
	vPos.y+=fAddNearLabel;

	mpInputMinMoveFreqSpeed = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMinMoveFreqSpeed->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMinMoveFreqSpeed->SetDecimals(3);

	vPos.x += 75;
	mpInputMinMoveVolume = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMinMoveVolume->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMinMoveVolume->SetDecimals(3);


	vPos.x = 15;
	vPos.y += fAddFar;

	pLabel = mpSet->CreateWidgetLabel(vPos, 0, _W("Max"), apParentTab);


	vPos.y += fAddNearLabel;


	mpLabelMaxMoveSpeed = mpSet->CreateWidgetLabel(vPos, 0, _W("Speed"), apParentTab);

	vPos.x += 75;
	mpLabelMaxMoveFreq = mpSet->CreateWidgetLabel(vPos, 0, _W("Freq"), apParentTab);


	vPos.x = 15;
	vPos.y+=fAddNearLabel;

	mpInputMaxMoveSpeed = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMaxMoveSpeed->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMaxMoveSpeed->SetDecimals(3);

	vPos.x += 75;
	mpInputMaxMoveFreq = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMaxMoveFreq->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMaxMoveFreq->SetDecimals(3);


	vPos.x = 15;
	vPos.y += fAddFar;

	mpLabelMaxMoveFreqSpeed = mpSet->CreateWidgetLabel(vPos, 0, _W("Freq Speed"), apParentTab);

	vPos.x += 75;
	mpLabelMaxMoveVolume = mpSet->CreateWidgetLabel(vPos, 0, _W("Volume"), apParentTab);

	
	vPos.x = 15;
	vPos.y+=fAddNearLabel;

	mpInputMaxMoveFreqSpeed = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMaxMoveFreqSpeed->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

	vPos.x += 75;
	mpInputMaxMoveVolume = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMaxMoveVolume->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));


	vPos.x = 15;
	vPos.y += fAddFar;

	pLabel = mpSet->CreateWidgetLabel(vPos, 0, _W("Middle"), apParentTab);


	vPos.y += fAddNearLabel;

	mpLabelMiddleMoveSpeed = mpSet->CreateWidgetLabel(vPos, 0, _W("Speed"), apParentTab);

	vPos.x += 75;
	mpLabelMiddleMoveVolume = mpSet->CreateWidgetLabel(vPos, 0, _W("Volume"), apParentTab);


	vPos.x =15;	
	vPos.y+=fAddNearLabel;

	mpInputMiddleMoveSpeed = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMiddleMoveSpeed->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

	vPos.x += 75;
	mpInputMiddleMoveVolume = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMiddleMoveVolume->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));


	vPos.x = 15;
	vPos.y += fAddFar;

	mpLabelBreakSound = mpSet->CreateWidgetLabel(vPos, 0, _W("Break Sound"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputBreakSound = mpSet->CreateWidgetTextBox(vPos, cVector2f(120,25), _W(""), apParentTab);
	mpInputBreakSound->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

	mpButtonBreakSoundBrowse = mpSet->CreateWidgetButton(vPos + cVector3f(123,0,0), cVector2f(20), _W("..."), apParentTab);

	mpButtonBreakSoundBrowse->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(BrowseButton_OnPressed));

	vPos.y += fAddFar;

	mpLabelMinLimitSound = mpSet->CreateWidgetLabel(vPos, 0, _W("Min Limit Sound"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputMinLimitSound = mpSet->CreateWidgetTextBox(vPos, cVector2f(120,25), _W(""), apParentTab);
	mpInputMinLimitSound->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

	mpButtonMinLimitSoundBrowse = mpSet->CreateWidgetButton(vPos + cVector3f(123,0,0), cVector2f(20), _W("..."), apParentTab);

	mpButtonMinLimitSoundBrowse->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(BrowseButton_OnPressed));

	vPos.y += fAddFar;

	mpLabelMinLimitMinSpeed = mpSet->CreateWidgetLabel(vPos, 0, _W("Min Speed"), apParentTab);

	vPos.x += 75;
	mpLabelMinLimitMaxSpeed = mpSet->CreateWidgetLabel(vPos, 0, _W("Max Speed"), apParentTab);


	vPos.x = 15;
	vPos.y+=fAddNearLabel;

	mpInputMinLimitMinSpeed = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMinLimitMinSpeed->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

	vPos.x += 75;
	mpInputMinLimitMaxSpeed = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMinLimitMaxSpeed->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));


	vPos.x = 15;
	vPos.y += fAddFar;

	mpLabelMaxLimitSound = mpSet->CreateWidgetLabel(vPos, 0, _W("Max Limit Sound"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputMaxLimitSound = mpSet->CreateWidgetTextBox(vPos, cVector2f(120,25), _W(""), apParentTab);
	mpInputMaxLimitSound->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

	mpButtonMaxLimitSoundBrowse = mpSet->CreateWidgetButton(vPos + cVector3f(123,0,0), cVector2f(20), _W("..."), apParentTab);

	mpButtonMaxLimitSoundBrowse->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(BrowseButton_OnPressed));

	vPos.y += fAddFar;

	mpLabelMinLimitMinSpeed = mpSet->CreateWidgetLabel(vPos, 0, _W("Min Speed"), apParentTab);

	vPos.x += 75;
	mpLabelMinLimitMaxSpeed = mpSet->CreateWidgetLabel(vPos, 0, _W("Max Speed"), apParentTab);


	vPos.x = 15;
	vPos.y+=fAddNearLabel;

	mpInputMaxLimitMinSpeed = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMaxLimitMinSpeed->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

	vPos.x += 75;
	mpInputMaxLimitMaxSpeed = mpSet->CreateWidgetTextBox(vPos, cVector2f(70,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric,0.5f);
	mpInputMaxLimitMaxSpeed->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxJoint::AddPropertySetBall(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,25,0.1f);
	float fAddNearLabel = 18;
	float fAddFar = 25;

	mpLabelMaxConeAngle = mpSet->CreateWidgetLabel(vPos, 0, _W("Max Cone Angle"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputMaxConeAngle = mpSet->CreateWidgetTextBox(vPos, cVector2f(100,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric);
	mpInputMaxConeAngle->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMaxConeAngle->SetDecimals(3);


	vPos.y += fAddFar;

	mpLabelMaxTwistAngle = mpSet->CreateWidgetLabel(vPos, 0, _W("Max Twist Angle"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputMaxTwistAngle = mpSet->CreateWidgetTextBox(vPos, cVector2f(100,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric);
	mpInputMaxTwistAngle->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMaxTwistAngle->SetDecimals(3);

}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxJoint::AddPropertySetHinge(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);
	float fAddFar = 25;
	float fAddNearLabel = 18;

	mpLabelMinAngle = mpSet->CreateWidgetLabel(vPos, 0, _W("Min Angle"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputMinAngle = mpSet->CreateWidgetTextBox(vPos, cVector2f(100,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric);
	mpInputMinAngle->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMinAngle->SetDecimals(3);


	vPos.y += fAddFar;

	mpLabelMaxAngle = mpSet->CreateWidgetLabel(vPos, 0, _W("Max Angle"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputMaxAngle = mpSet->CreateWidgetTextBox(vPos, cVector2f(100,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric);
	mpInputMaxAngle->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMaxAngle->SetDecimals(3);

}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxJoint::AddPropertySetScrewSlider(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);
	float fAddNearLabel = 18;
	float fAddNear = 25;
	float fAddFar = 45;


	mpLabelMinDistance = mpSet->CreateWidgetLabel(vPos, 0, _W("Min Distance"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputMinDistance = mpSet->CreateWidgetTextBox(vPos, cVector2f(100,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric);
	mpInputMinDistance->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMinDistance->SetDecimals(3);


	vPos.y += fAddFar;

	mpLabelMaxDistance = mpSet->CreateWidgetLabel(vPos, 0, _W("Max Distance"), apParentTab);

	vPos.y+=fAddNearLabel;
	mpInputMaxDistance = mpSet->CreateWidgetTextBox(vPos, cVector2f(100,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric);
	mpInputMaxDistance->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));
	mpInputMaxDistance->SetDecimals(3);


	vPos.y += fAddFar;

	/*for(int i=0; i<3; ++i)
	{
		mvInputPin[i] = mpSet->CreateWidgetTextBox(vPos, cVector2f(100,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric);
		mvInputPin[i]->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));


		vPos.y += fAddNear;
	}
	*/
}


//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxJoint::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lID = mpEntity->GetID();
	iEditorWorld* pLevel = mpEditor->GetEditorWorld();
	cResources* pRes = mpEditor->GetEngine()->GetResources();

	tString sFile = cString::To8Char(apWidget->GetText());

	cSoundEntityData* pSound = pRes->GetSoundEntityManager()->CreateSoundEntity(sFile);

	iEditorAction* pAction=NULL;

	//////////////////////////////////
	// Sticky Min Limit
	if(apWidget == mpCheckBoxStickyMinLimit)

	{
		pAction = mpEntity->CreateSetPropertyActionBool(eJointBool_StickyMinLimit, mpCheckBoxStickyMinLimit->IsChecked());
	}
	//////////////////////////////////
	// Sticky Min Limit
	if(apWidget == mpCheckBoxStickyMaxLimit)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eJointBool_StickyMaxLimit, mpCheckBoxStickyMaxLimit->IsChecked());
	}
	//////////////////////////////////
	// Move Sound
	else if(apWidget == mpInputMoveSound)
	{
		if(pSound==NULL && sFile!="")
		{
			apWidget->SetText(_W(""));
		}
		else
			pAction = mpEntity->CreateSetPropertyActionString(eJointStr_MoveSound, sFile);
	}
	//////////////////////////////////
	// Min Limit Sound
	else if(apWidget == mpInputMinLimitSound)
	{
		if(pSound==NULL && sFile!="")
		{
			apWidget->SetText(_W(""));
		}
		else
			pAction = mpEntity->CreateSetPropertyActionString(eJointStr_MinLimitSound, sFile);
	}
	//////////////////////////////////
	// Max Limit Sound
	else if(apWidget == mpInputMaxLimitSound)
	{
		if(pSound==NULL && sFile!="")
		{
			apWidget->SetText(_W(""));
		}
		else
			pAction = mpEntity->CreateSetPropertyActionString(eJointStr_MaxLimitSound, sFile);
	}
	//////////////////////////////////
	// Min Limit Min Speed
	else if(apWidget == mpInputMinLimitMinSpeed)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MinLimitMinSpeed, mpInputMinLimitMinSpeed->GetNumericValue());
	}
	//////////////////////////////////
	// Min Limit Max Speed
	else if(apWidget == mpInputMinLimitMaxSpeed)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MinLimitMaxSpeed, mpInputMinLimitMaxSpeed->GetNumericValue());
	}
	//////////////////////////////////
	// Max Limit Min Speed
	else if(apWidget == mpInputMaxLimitMinSpeed)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MaxLimitMinSpeed, mpInputMaxLimitMinSpeed->GetNumericValue());
	}
	//////////////////////////////////
	// Max Limit Max Speed
	else if(apWidget == mpInputMaxLimitMaxSpeed)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MaxLimitMaxSpeed, mpInputMaxLimitMaxSpeed->GetNumericValue());
	}
	//////////////////////////////////
	// Min Move Speed
	else if(apWidget == mpInputMinMoveSpeed)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MinMoveSpeed, mpInputMinMoveSpeed->GetNumericValue());
	}
	//////////////////////////////////
	// Min Move Freq
	else if(apWidget == mpInputMinMoveFreq)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MinMoveFreq, mpInputMinMoveFreq->GetNumericValue());
	}
	//////////////////////////////////
	// Min Move Freq Speed
	else if(apWidget == mpInputMinMoveFreqSpeed)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MinMoveFreqSpeed, mpInputMinMoveFreqSpeed->GetNumericValue());
	}
	//////////////////////////////////
	// Min Move Volume
	else if(apWidget == mpInputMinMoveVolume)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MinMoveVolume, mpInputMinMoveVolume->GetNumericValue());
	}
	//////////////////////////////////
	// Max Move Speed
	else if(apWidget == mpInputMaxMoveSpeed)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MaxMoveSpeed, mpInputMaxMoveSpeed->GetNumericValue());
	}
	//////////////////////////////////
	// Max Move Freq
	else if(apWidget == mpInputMaxMoveFreq)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MaxMoveFreq, mpInputMaxMoveFreq->GetNumericValue());
	}
	//////////////////////////////////
	// Max Move Freq Speed
	else if(apWidget == mpInputMaxMoveFreqSpeed)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MaxMoveFreqSpeed, mpInputMaxMoveFreqSpeed->GetNumericValue());
	}
	//////////////////////////////////
	// Max Move Volume
	else if(apWidget == mpInputMaxMoveVolume)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MaxMoveVolume, mpInputMaxMoveVolume->GetNumericValue());
	}
	//////////////////////////////////
	// Middle Move Speed
	else if(apWidget == mpInputMiddleMoveSpeed)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MiddleMoveSpeed, mpInputMiddleMoveSpeed->GetNumericValue());
	}
	//////////////////////////////////
	// Middle Move Volume
	else if(apWidget == mpInputMiddleMoveVolume)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_MiddleMoveVolume, mpInputMiddleMoveVolume->GetNumericValue());
	}
	//////////////////////////////////
	// Limit Step Count
	else if(apWidget == mpInputLimitStepCount)
	{
		pAction = mpEntity->CreateSetPropertyActionInt(eJointInt_LimitAutoSleepNumSteps, (int)mpInputLimitStepCount->GetNumericValue());
	}
	//////////////////////////////////
	// Collide bodies
	else if(apWidget == mpCheckBoxCollideBodies)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eJointBool_CollideBodies, mpCheckBoxCollideBodies->IsChecked());
	}
	//////////////////////////////////
	// Breakable
	else if(apWidget == mpCheckBoxBreakable)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eJointBool_Breakable, mpCheckBoxBreakable->IsChecked());
	}
	//////////////////////////////////
	// Break Force
	else if(apWidget == mpInputBreakForce)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointFloat_BreakForce, mpInputBreakForce->GetNumericValue());
	}
	//////////////////////////////////
	// Break Sound
	else if(apWidget == mpInputBreakSound)
	{
		if(pSound==NULL && sFile!="")
		{
			apWidget->SetText(_W(""));
		}
		else
			pAction = mpEntity->CreateSetPropertyActionString(eJointStr_BreakSound, sFile);
	}
	//////////////////////////////////
	// Ball : Max Cone Angle
	else if(apWidget == mpInputMaxConeAngle)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointBallFloat_MaxConeAngle, mpInputMaxConeAngle->GetNumericValue());
	}
	//////////////////////////////////
	// Ball : Max Twist Angle
	else if(apWidget == mpInputMaxTwistAngle)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointBallFloat_MaxTwistAngle, mpInputMaxTwistAngle->GetNumericValue());
	}
	/*
	//////////////////////////////////
	// Ball : Cone Pin
	else if(apWidget == mvInputConePin[0] || 
			apWidget == mvInputConePin[1] ||
			apWidget == mvInputConePin[2])
	{
		cVector3f vTemp;
		for(int i=0;i<3;++i)
		{
			vTemp.v[i] = mvInputConePin[i]->GetNumericValue();
		}

		pAction = hplNew( cEditorActionJointBallSetVector3Property,(lID, 
															  pLevel, 
															  eEditorJointBallVector3Property_ConePin, 
															  vTemp));
	}*/
	//////////////////////////////////
	// Hinge : Min Angle
	else if(apWidget == mpInputMinAngle)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointHingeFloat_MinAngle, mpInputMinAngle->GetNumericValue());
	}
	//////////////////////////////////
	// Hinge : Max Angle
	else if(apWidget == mpInputMaxAngle)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eJointHingeFloat_MaxAngle, mpInputMaxAngle->GetNumericValue());
	}
	//////////////////////////////////
	// Screw & Slider : Min Distance
	else if(apWidget == mpInputMinDistance)
	{
		int prop = -1;
		if(mpEntity->GetJointType()==eEditorEntityJointType_Screw)
			prop = eJointScrewFloat_MinDistance;
		else
			prop = eJointSliderFloat_MinDistance;

		pAction = mpEntity->CreateSetPropertyActionFloat(prop, mpInputMinDistance->GetNumericValue());
	}
	//////////////////////////////////
	// Screw & Slider : Max Distance
	else if(apWidget == mpInputMaxDistance)
	{
		int prop = -1;
		if(mpEntity->GetJointType()==eEditorEntityJointType_Screw)
			prop = eJointScrewFloat_MaxDistance;
		else
			prop = eJointSliderFloat_MaxDistance;

		pAction = mpEntity->CreateSetPropertyActionFloat(prop, mpInputMaxDistance->GetNumericValue());
	}
	/*
	//////////////////////////////////
	// Screw & Slider : Pin
	else if(apWidget == mvInputPin[0] || 
			apWidget == mvInputPin[1] ||
			apWidget == mvInputPin[2])
	{
		cVector3f vTemp;
		for(int i=0;i<3;++i)
		{
			vTemp.v[i] = mvInputPin[i]->GetNumericValue();
		}

		if(mpEntity->GetJointType()==eEditorEntityJointType_Screw)
		{
			pAction = hplNew( cEditorActionJointScrewSetVector3Property,(lID, 
																		 pLevel,
																		 eEditorJointScrewVector3Property_Pin,
																		 vTemp));
		}
		else
		{
			pAction = hplNew( cEditorActionJointSliderSetVector3Property,(lID, 
																		  pLevel,
																		  eEditorJointSliderVector3Property_Pin,
																		  vTemp));
		}
	}*/

	//////////////////////////////////
	// Execute action
	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxJoint, InputCallback);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxJoint::Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpCurrentBodyHighlighter = hplNew( cEntitySelectorHighlighter,(mpEditMode));
	mpCurrentBodyHighlighter->GetFilter()->SetTypeFilter(eEditorEntityType_Body, true);

	if(apWidget == mvButtons[0])
	{
		mpCurrentBodyHighlighter->SetMouseUpCallback(this, kGuiCallback(SetParentBodyCallback));
	}
	else if(apWidget == mvButtons[1])
	{
		mpCurrentBodyHighlighter->SetMouseUpCallback(this, kGuiCallback(SetChildBodyCallback));
	}

	mpEditMode->SetCurrentEntitySelector(mpCurrentBodyHighlighter);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxJoint, Button_OnPressed);


//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxJoint::BrowseButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	tGuiCallbackFunc pFunc;

	if(apWidget == mpButtonMoveSoundBrowse)
	{
		pFunc = kGuiCallback(FileMoveSoundCallback);
	}
	else if(apWidget == mpButtonBreakSoundBrowse)
	{
		pFunc = kGuiCallback(FileBreakSoundCallback);
	}
	else if(apWidget == mpButtonMinLimitSoundBrowse)
	{
		pFunc = kGuiCallback(FileMinLimitSoundCallback);
	}
	else if(apWidget == mpButtonMaxLimitSoundBrowse)
	{
		pFunc = kGuiCallback(FileMaxLimitSoundCallback);
	}

	mpEditor->ShowLoadFilePicker(mvLoadedFiles, msLastPath, this, pFunc, _W("Sounds"), tWStringList(1,_W("*.snt")));

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxJoint, BrowseButton_OnPressed);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxJoint::SetParentBodyCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	AttachBodyToJoint(eJointInt_ParentID);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxJoint, SetParentBodyCallback);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxJoint::SetChildBodyCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	AttachBodyToJoint(eJointInt_ChildID);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxJoint, SetChildBodyCallback);

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxJoint::AttachBodyToJoint(int alBodyType)
{
	if(mpCurrentBodyHighlighter==NULL)
		return;

	///////////////////////////////////////////////////////////
	// Get old and new bodies
	cEntityWrapperBody* pOldBody;
	cEntityWrapperBody* pOtherBody;
	switch(alBodyType)
	{
	case eJointInt_ParentID:
		pOldBody = mpEntity->GetParentBody();
		pOtherBody = mpEntity->GetChildBody();
		break;
	case eJointInt_ChildID:
		pOldBody = mpEntity->GetChildBody();
		pOtherBody = mpEntity->GetParentBody();
		break;
	default:
		break;
	}
	cEntityWrapperBody* pNewBody = (cEntityWrapperBody*)mpCurrentBodyHighlighter->GetPickedEntity();

	///////////////////////////////////////////////////////////
	// If old body and new body are the same, nothing to do
	if(pOldBody==pNewBody)
		return;

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
		
		iEditorAction* pAction = mpEntity->CreateSetPropertyActionInt(alBodyType, lBodyID);
		//iEditorAction* pAction = hplNew( cEditorActionJointAttachBody,(mpEntity->GetID(), mpEditMode->GetEditorWorld(), aBody, lBodyID));
		mpEditor->AddAction(pAction);
	}

	hplDelete(mpCurrentBodyHighlighter);
	mpCurrentBodyHighlighter = NULL;

	
	mpEditMode->SetCurrentEntitySelector(NULL);
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxJoint::FileMoveSoundCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		msLastPath = cString::GetFilePathW(mvLoadedFiles.front());
		tWString sFilename = cString::GetFileNameW(mvLoadedFiles.front());

		mpInputMoveSound->SetText(sFilename);
		InputCallback(mpInputMoveSound, cGuiMessageData());
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxJoint, FileMoveSoundCallback);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxJoint::FileBreakSoundCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		msLastPath = cString::GetFilePathW(mvLoadedFiles.front());
		tWString sFilename = cString::GetFileNameW(mvLoadedFiles.front());

		mpInputBreakSound->SetText(sFilename);
		InputCallback(mpInputBreakSound, cGuiMessageData());
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxJoint, FileBreakSoundCallback);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxJoint::FileMinLimitSoundCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		msLastPath = cString::GetFilePathW(mvLoadedFiles.front());
		tWString sFilename = cString::GetFileNameW(mvLoadedFiles.front());

		mpInputMinLimitSound->SetText(sFilename);
		InputCallback(mpInputMinLimitSound, cGuiMessageData());
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxJoint, FileMinLimitSoundCallback);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxJoint::FileMaxLimitSoundCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		msLastPath = cString::GetFilePathW(mvLoadedFiles.front());
		tWString sFilename = cString::GetFileNameW(mvLoadedFiles.front());

		mpInputMaxLimitSound->SetText(sFilename);
		InputCallback(mpInputMaxLimitSound, cGuiMessageData());
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxJoint, FileMaxLimitSoundCallback);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxJoint::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput)==true)
		return true;

	iEditorAction* pAction = NULL;
	int lID = mpEntity->GetID();
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	if(apInput==mpInpMoveType)
	{
		pAction = mpEntity->CreateSetPropertyActionString(eJointStr_MoveType, cString::To8Char(mpInpMoveType->GetEnumValue(mpInpMoveType->GetValue())));
		//pAction = hplNew(cEditorActionJointSetStringProperty,(lID, pWorld, eEditorJointStringProperty_MoveType, cString::To8Char(mpInpMoveType->GetEnumValue(mpInpMoveType->GetValue()))));
	}

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
