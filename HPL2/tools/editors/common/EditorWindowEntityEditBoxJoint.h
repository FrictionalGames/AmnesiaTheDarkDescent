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

#ifndef HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_BOX_JOINT_H
#define HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_BOX_JOINT_H

#include "../common/StdAfx.h"
using namespace hpl;

#include "EditorWindowEntityEditBox.h"
#include "EditorEditModeSelect.h"

//----------------------------------------------------------------------

class iEditorBase;
class iEntityWrapper;

class cEditorEditModeSelect;

class cEntityWrapperBody;
class iEntityWrapperJoint;

//enum eEditorJointAttachedBody;

//----------------------------------------------------------------------

class cEditorWindowEntityEditBoxJoint : public cEditorWindowEntityEditBox
{
public:
	cEditorWindowEntityEditBoxJoint(cEditorEditModeSelect* apEditMode,iEntityWrapperJoint* apObject);
	virtual ~cEditorWindowEntityEditBoxJoint();

	void Create();
protected:
	void OnUpdate(float afTimeStep);

	void AddPropertySetJointBase(cWidgetTab* apParentTab);
	void AddPropertySetJointSounds(cWidgetTab* apParentTab);
	void AddPropertySetBall(cWidgetTab* apParentTab);
	void AddPropertySetHinge(cWidgetTab* apParentTab);
	void AddPropertySetScrewSlider(cWidgetTab* apParentTab);

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Button_OnPressed);

	bool BrowseButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(BrowseButton_OnPressed);

	bool SetParentBodyCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SetParentBodyCallback);

	bool SetChildBodyCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SetChildBodyCallback);

	void AttachBodyToJoint(int alBody);

	bool FileMoveSoundCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FileMoveSoundCallback);

	bool FileMinLimitSoundCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FileMinLimitSoundCallback);

	bool FileMaxLimitSoundCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FileMaxLimitSoundCallback);

	bool FileBreakSoundCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FileBreakSoundCallback);

	bool WindowSpecificInputCallback(iEditorInput* apInput);


	//////////////////////////////////
	// Data
	cWidgetTab* mpTabGeneral;
	cWidgetTab* mpTabJointBase;
	cWidgetTab* mpTabJointSounds;
	cWidgetTab* mpTabSpecific;

	////////////////////////////////////
	// Joint Base
	cWidgetButton* mvButtons[2];

	cWidgetCheckBox* mpCheckBoxStickyMinLimit;
	cWidgetCheckBox* mpCheckBoxStickyMaxLimit;

	cWidgetLabel* mpLabelLimitStepCount;
	cWidgetTextBox* mpInputLimitStepCount;

	cWidgetCheckBox* mpCheckBoxCollideBodies;

	cWidgetCheckBox* mpCheckBoxBreakable;
	
	cWidgetLabel* mpLabelBreakForce;
	cWidgetTextBox* mpInputBreakForce;

	///////////////////////////////////
	// Sound related
	cWidgetLabel* mpLabelMoveSound;
	cWidgetTextBox* mpInputMoveSound;
	cWidgetButton* mpButtonMoveSoundBrowse;

	cEditorInputEnum* mpInpMoveType;

	cWidgetLabel* mpLabelMinMoveSpeed;
	cWidgetTextBox* mpInputMinMoveSpeed;

	cWidgetLabel* mpLabelMinMoveFreq;
	cWidgetTextBox* mpInputMinMoveFreq;

	cWidgetLabel* mpLabelMinMoveFreqSpeed;
	cWidgetTextBox* mpInputMinMoveFreqSpeed;

	cWidgetLabel* mpLabelMinMoveVolume;
	cWidgetTextBox* mpInputMinMoveVolume;

	cWidgetLabel* mpLabelMaxMoveSpeed;
	cWidgetTextBox* mpInputMaxMoveSpeed;

	cWidgetLabel* mpLabelMaxMoveFreq;
	cWidgetTextBox* mpInputMaxMoveFreq;

	cWidgetLabel* mpLabelMaxMoveFreqSpeed;
	cWidgetTextBox* mpInputMaxMoveFreqSpeed;

	cWidgetLabel* mpLabelMaxMoveVolume;
	cWidgetTextBox* mpInputMaxMoveVolume;

	cWidgetLabel* mpLabelMiddleMoveSpeed;
	cWidgetTextBox* mpInputMiddleMoveSpeed;

	cWidgetLabel* mpLabelMiddleMoveVolume;
	cWidgetTextBox* mpInputMiddleMoveVolume;

	cWidgetLabel* mpLabelMinLimitSound;
	cWidgetTextBox* mpInputMinLimitSound;
	cWidgetButton* mpButtonMinLimitSoundBrowse;

	cWidgetLabel* mpLabelMaxLimitSound;
	cWidgetTextBox* mpInputMaxLimitSound;
	cWidgetButton* mpButtonMaxLimitSoundBrowse;

	cWidgetLabel* mpLabelMinLimitMinSpeed;
	cWidgetTextBox* mpInputMinLimitMinSpeed;

	cWidgetLabel* mpLabelMinLimitMaxSpeed;
	cWidgetTextBox* mpInputMinLimitMaxSpeed;

	cWidgetLabel* mpLabelMaxLimitMinSpeed;
	cWidgetTextBox* mpInputMaxLimitMinSpeed;

	cWidgetLabel* mpLabelMaxLimitMaxSpeed;
	cWidgetTextBox* mpInputMaxLimitMaxSpeed;

	cWidgetLabel* mpLabelBreakSound;
	cWidgetTextBox* mpInputBreakSound;
	cWidgetButton* mpButtonBreakSoundBrowse;

	///////////////////////////////////
	// Ball
	cWidgetLabel* mpLabelMaxConeAngle;
	cWidgetTextBox* mpInputMaxConeAngle;

	cWidgetLabel* mpLabelMaxTwistAngle;
	cWidgetTextBox* mpInputMaxTwistAngle;

	cWidgetLabel* mpLabelConePin[4];
	cWidgetTextBox* mvInputConePin[3];

	///////////////////////////////////
	// Hinge
	cWidgetLabel* mpLabelMinAngle;
	cWidgetTextBox* mpInputMinAngle;

	cWidgetLabel* mpLabelMaxAngle;
	cWidgetTextBox* mpInputMaxAngle;

	///////////////////////////////////
	// Screw / Slider
	cWidgetLabel* mpLabelMinDistance;
	cWidgetTextBox* mpInputMinDistance;

	cWidgetLabel* mpLabelMaxDistance;
	cWidgetTextBox* mpInputMaxDistance;

	//cWidgetLabel* mpLabelPin[4];
	//cWidgetTextBox* mvInputPin[3];


	//////////////////////////////////	

	tWString msLastPath;
	tWStringVec mvLoadedFiles;

	iEntityWrapperJoint* mpEntity;

	cEntitySelectorHighlighter* mpCurrentBodyHighlighter;

	static int mlTabOnTopIndex;
};

//----------------------------------------------------------------------


#endif // HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_BOX_JOINT_H
