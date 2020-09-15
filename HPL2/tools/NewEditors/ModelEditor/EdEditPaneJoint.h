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

#ifndef ED_EDIT_PANE_JOINT_H
#define ED_EDIT_PANE_JOINT_H

#include "../Common/StdAfx.h"
using namespace hpl;

#include "../Common/EdScnObjEditPane.h"
//#include "EdEditModeSelect.h"

//----------------------------------------------------------------------

//class iEditorBase;
//class iEntityWrapper;

//class cEdEditModeSelect;

//class cEntityWrapperBody;
//class iEntityWrapperJoint;

//enum eEditorJointAttachedBody;

//----------------------------------------------------------------------

class cEdEditPaneJoint : public iEdScnObjEditPane
{
public:
	cEdEditPaneJoint(iEdObject* apObject);
	virtual ~cEdEditPaneJoint();

	void Create();
protected:
	void OnUpdate();

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

	bool WindowSpecificInputCallback(iEdInput* apInput);


	//////////////////////////////////
	// Data
	cWidgetTab* mpTabGeneral;
	cWidgetTab* mpTabJointBase;
	cWidgetTab* mpTabJointSounds;
	cWidgetTab* mpTabSpecific;

	////////////////////////////////////
	// Joint Base
	cWidgetButton* mpBSetParentBody;
	cWidgetButton* mpBSetChildBody;
	cWidgetButton* mvButtons[2];

	cEdInputBool* mpInpStickyMinLimit;
	cEdInputBool* mpInpStickyMaxLimit;

	cEdInputNumber* mpInpLimitStepCount;

	cEdInputBool*	mpInpCollideBodies;
	cEdInputBool*	mpInpBreakable;
	cEdInputNumber*	mpInpBreakForce;

	///////////////////////////////////
	// Sound related
	cEdInputSound*	mpInpMoveSound;

	cEdInputEnum* mpInpMoveType;

	cEdInputNumber* mpInpMinMoveSpeed;
	cEdInputNumber* mpInpMinMoveFreq;
	cEdInputNumber* mpInpMinMoveFreqSpeed;
	cEdInputNumber* mpInpMinMoveVolume;

	cEdInputNumber* mpInpMaxMoveSpeed;
	cEdInputNumber* mpInpMaxMoveFreq;
	cEdInputNumber* mpInpMaxMoveFreqSpeed;
	cEdInputNumber* mpInpMaxMoveVolume;

	cEdInputNumber* mpInpMiddleMoveSpeed;
	cEdInputNumber* mpInpMiddleMoveVolume;

	cEdInputSound* mpInpMinLimitSound;
	cEdInputSound* mpInpMaxLimitSound;

	cEdInputNumber* mpInpMinLimitMinSpeed;
	cEdInputNumber* mpInpMinLimitMaxSpeed;

	cEdInputNumber* mpInpMaxLimitMinSpeed;
	cEdInputNumber* mpInpMaxLimitMaxSpeed;

	cEdInputSound* mpInpBreakSound;

	///////////////////////////////////
	// Ball
	cEdInputNumber* mpInpMaxConeAngle;
	cEdInputNumber* mpInpMaxTwistAngle;
	
	//cWidgetLabel* mpLabelConePin[4];
	//cWidgetTextBox* mvInputConePin[3];

	///////////////////////////////////
	// Hinge
	cEdInputNumber* mpInpMinAngle;
	cEdInputNumber* mpInpMaxAngle;
	
	///////////////////////////////////
	// Screw / Slider
	cEdInputNumber* mpInpMinDistance;
	cEdInputNumber* mpInpMaxDistance;
	
	//cWidgetLabel* mpLabelPin[4];
	//cWidgetTextBox* mvInputPin[3];

	//////////////////////////////////	

	//tWString msLastPath;
	//tWStringVec mvLoadedFiles;

	cEdSelectorHighlighter* mpCurrentBodyHighlighter;
};

//----------------------------------------------------------------------


#endif // ED_EDIT_PANE_JOINT_H
