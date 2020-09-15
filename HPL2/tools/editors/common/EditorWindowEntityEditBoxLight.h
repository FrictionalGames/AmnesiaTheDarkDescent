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

#ifndef HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_BOX_LIGHT_H
#define HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_BOX_LIGHT_H

#include "EditorWindowEntityEditBox.h"


//--------------------------------------------------------------------

class iEntityWrapperLight;

//--------------------------------------------------------------------

class cEditorWindowEntityEditBoxLight : public cEditorWindowEntityEditBox
{
public:
	cEditorWindowEntityEditBoxLight(cEditorEditModeSelect* apEditMode,iEntityWrapperLight* apLight);
	~cEditorWindowEntityEditBoxLight();

	void Create();

protected:
	void OnUpdate(float afTimeStep);

	void AddPropertyGobo(cWidgetTab* apParentTab);
	void AddPropertyFalloffMap(cWidgetTab* apParentTab);
	void AddPropertyRadius(cWidgetTab* apParentTab);
	void AddPropertyDiffuseColor(cWidgetTab* apParentTab);
	void AddPropertyCastShadows(cWidgetTab* apParentTab);

	void AddPropertySetFlicker(cWidgetTab* apParentTab);
	
	void AddPropertySetBox(cWidgetTab* apParentTab);
	void AddPropertySetPoint(cWidgetTab* apParentTab);
	void AddPropertySetSpot(cWidgetTab* apParentTab);

	
	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);
	

	bool WindowSpecificInputCallback(iEditorInput* apInput);

	///////////////////////////////////////////////////
	// Data
	iEntityWrapperLight* mpLight;

	tWString msTempTextureFilename;
	tWStringVec mvLoadedFiles;

	cColor mDiffuseCol;
	cColor mSpecularCol;
	cColor mFlickerOffCol;

	// Layout
	cWidgetTab* mpTabGeneral;
	cWidgetTab* mpTabFlicker;
	cWidgetTab* mpTabLightParameters;

	cWidgetDummy* mpGroupRadius;
	cEditorInputNumber* mpInpRadius;

	cWidgetDummy* mpGroupGobo;
	cEditorInputFile* mpInpGobo;
	cEditorInputEnum* mpInpGoboAnimMode;
	cEditorInputNumber* mpInpGoboAnimFrameTime;

	cWidgetDummy* mpGroupFalloff;
	cEditorInputFile* mpInpFalloffMap;

	cEditorInputColorFrame* mpInpDiffuse;

	cWidgetDummy* mpGroupDiffuse;
	cWidgetLabel* mpLabelDiffuseColor;
	cWidgetLabel* mpLabelDiffuseColorExp;
	cWidgetFrame* mpFrameDiffuseColor;

	cWidgetDummy* mpGroupShadows;
	cEditorInputBool* mpInpCastShadows;
	cEditorInputEnum* mpInpShadowRes;
	cEditorInputBool* mpInpShadowAffectStatic;
	cEditorInputBool* mpInpShadowAffectDynamic;

	///////////////////////////////
	// Flicker stuff
	cEditorInputBool* mpInpFlickerActive;

	// On params
	cWidgetDummy* mpGFlickerOn;
	cEditorInputNumber* mpInpFlickerOnMinLength;
	cEditorInputNumber* mpInpFlickerOnMaxLength;

	cEditorInputFile* mpInpFlickerOnSound;
	cEditorInputFile* mpInpFlickerOnPS;

	// Off params
	cWidgetDummy* mpGFlickerOff;

	cEditorInputNumber* mpInpFlickerOffMinLength;
	cEditorInputNumber* mpInpFlickerOffMaxLength;
	
	cEditorInputFile* mpInpFlickerOffSound;
	cEditorInputFile* mpInpFlickerOffPS;

	cEditorInputNumber* mpInpFlickerOffRadius;
	cEditorInputColorFrame* mpInpFlickerOffColor;

	// Fade
	cWidgetDummy* mpGFlickerFade;

	cEditorInputBool* mpInpFlickerFade;

	cEditorInputNumber* mpInpFlickerFadeOnMinLength;
	cEditorInputNumber* mpInpFlickerFadeOnMaxLength;
	cEditorInputNumber* mpInpFlickerFadeOffMinLength;
	cEditorInputNumber* mpInpFlickerFadeOffMaxLength;

	/////////////////////////////////
	// Box Light specific
	cWidgetLabel* mpLabelBlendFunc;
	cWidgetComboBox* mpComboBoxBlendFunc;

	cWidgetLabel* mvLabelBoxSize[4];
	cWidgetTextBox* mvInputBoxSize[3];

	cEditorInputEnum* mpInpBoxBlendFunc;
	cEditorInputVec3* mpInpBoxSize;

	///////////////////////////////////
	// Spot Light specific
	cEditorInputNumber* mpInpSpotFOV;
	cEditorInputNumber* mpInpSpotAspect;
	cEditorInputNumber* mpInpSpotNearClipPlane;
	cEditorInputFile* mpInpSpotFalloffMap;
};



#endif // HPLEDITOR_EDITOR_WINDOW_EDIT_BOX_LIGHT_H
