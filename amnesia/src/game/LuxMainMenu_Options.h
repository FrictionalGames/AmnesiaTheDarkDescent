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

#ifndef LUX_MAIN_MENU_OPTIONS_H
#define LUX_MAIN_MENU_OPTIONS_H

//----------------------------------------------

#include "LuxMainMenu.h"

//----------------------------------------------

class cLuxOption_ExtData
{
public:
	cLuxOption_ExtData(bool abNeedsRestart, const tWString& asMessage) { mbNeedsRestart = abNeedsRestart; msMessage = asMessage; }

	bool mbNeedsRestart;
	tWString msMessage;
};

typedef std::vector<cLuxOption_ExtData*> tOptionDataVec;
typedef tOptionDataVec::iterator		 tOptionDataVecIt;

//----------------------------------------------

class cLuxMainMenu_Options : public iLuxMainMenuWindow
{
public:	
	cLuxMainMenu_Options(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin);
	~cLuxMainMenu_Options();
	
	void CreateGui();

	void ExitPressed();
	
private:
	void OnSetActive(bool abX);

	void CreateMainGui();

	void AddGameOptions(cWidgetTab* apTab);
	void AddGraphicsOptions(cWidgetTab* apTab);
	void AddInputOptions(cWidgetTab* apTab);
	void AddSoundOptions(cWidgetTab* apTab);

	void AddBasicGfxOptions(cWidgetDummy* apDummy);
	void AddAdvancedGfxOptions(cWidgetDummy* apDummy);

	void SetInputValues(cResourceVarsObject& pObj);

	void SetUpInput(cWidgetLabel* apLabel, iWidget* apInput, bool abNeedsRestart, const tWString& asMessage);
	cLuxOption_ExtData* AddOptionData(bool abNeedsRestart, const tWString& asMessage);
	void SetCurrentTipWidget(iWidget* apWidget);

	void ApplyChanges();

	float GetGamma() { return GetSliderValue(mpSGamma, mfGammaMin, mfGammaMax); }
	float GetSensitivity() { return GetSliderValue(mpSMouseSensitivity, mfMouseSensitivityMin, mfMouseSensitivityMax); }
#ifdef USE_GAMEPAD
	float GetGamepadLookSensitivity() { return GetSliderValue(mpSGamepadLookSensitivity, mfGamepadLookSensitivityMin, mfGamepadLookSensitivityMax); }
#endif
	float GetVolume() { return GetSliderValue(mpSVolume, mfVolumeMin, mfVolumeMax); }

	void SetGammaLabelString(float afX);
	void SetSensitivityLabelString(float afX);
	void SetGamepadLookSensitivityLabelString(float afX);
	void SetVolumeLabelString(float afX);

	bool ChangeLanguage(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ChangeLanguage);

	void PopulateLanguageList();
	
	void PopulateSoundDevices();

	////////////////////////
	// Slider value helpers
	void SetUpSlider(cWidgetSlider* apSlider, float afMinValue, float afMaxValue, float afStepValue, 
						tGuiCallbackFunc apCallback=NULL, cWidgetLabel** apValueDisplay=NULL);
	void SetSliderValue(cWidgetSlider* apSlider, float afValue, bool abGenCallback, float afMinValue, float afMaxValue);
	float GetSliderValue(cWidgetSlider* apSlider, float afMinValue, float afMaxValue);

	void SetSliderLabelString(cWidgetLabel* apLabel, float afValue,
							  float afMinValue, float afMaxValue,
							  const tWString& asMin=_W(""), const tWString& asMax=_W(""));

	////////////////////////
	// Persistence of vars
	void DumpInitialValues(cResourceVarsObject& aObj);
	void DumpCurrentValues(cResourceVarsObject& aObj);

	////////////////////////
	// Callbacks
	bool TabFrame_OnPageChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(TabFrame_OnPageChange);


	void SetTabNavigation(cWidgetTab* apTab, bool abSetFocus);

	////////////////////////
	// Properties
	cVector2f mvWindowSize;
	bool mbShowCommentary;
	
	///////////////////////
	// Data and variables
	iWidget *mpShoulderHint[2];

	tWStringVec mvLangFiles;

	cWidgetTab *mpTabGame;
	cWidgetTab *mpTabGraphics;
	cWidgetTab *mpTabInput;
	cWidgetTab *mpTabSound;

	cWidgetButton* mpBOK;
	cWidgetButton* mpBCancel;

	// Tip Label
	cWidgetLabel *mpLTip;

	// Game
	cWidgetCheckBox *mpChBShowSubtitles;
	cWidgetCheckBox *mpChBShowEffectSubtitles;
	cWidgetComboBox *mpCBLanguage;
	cWidgetCheckBox *mpChBShowHints;
	cWidgetCheckBox *mpChBShowDeathHints;
	cWidgetCheckBox *mpChBShowCrosshair;
	cWidgetComboBox *mpCBFocusIconStyle;
	cWidgetCheckBox *mpChBShowCommentary;
	
	// Graphics;
	cWidgetDummy	*mpDBasicGfxOptions;
	cWidgetComboBox *mpCBResolution;
	cWidgetCheckBox *mpChBFullScreen;
	cWidgetCheckBox *mpChBVSync;
//	cWidgetCheckBox *mpChBAdaptiveVSync;

	cWidgetComboBox *mpCBTextureSizeLevel;

	cWidgetButton	*mpBToggleShowGfxOptions;
	cWidgetDummy	*mpDAdvancedGfxOptions;
	cWidgetComboBox *mpCBTextureFilter;

	cWidgetCheckBox *mpChBShadows;
	cWidgetComboBox *mpCBShadowQuality;
	cWidgetComboBox *mpCBShadowRes;

	cWidgetCheckBox *mpChBWorldReflection;
	cWidgetCheckBox *mpChBRefraction;

	cWidgetComboBox *mpCBAnisotropy;
	cWidgetCheckBox *mpChBParallax;
	cWidgetComboBox *mpCBParallaxQuality;
	
	cWidgetCheckBox *mpChEdgeSmooth;

	cWidgetCheckBox	*mpChBBloom;
	cWidgetCheckBox *mpChBImageTrail;
	cWidgetCheckBox *mpChBSepia;
	cWidgetCheckBox *mpChBRadialBlur;
	cWidgetCheckBox *mpChBInsanity;

	cWidgetCheckBox *mpChBSSAO;
	cWidgetComboBox *mpCBSSAOSamples;
	cWidgetComboBox *mpCBSSAOResolution;

	cWidgetLabel	*mpLGamma;
	cWidgetSlider	*mpSGamma;

	iWidget* mpLastFocusedWidget;
	bool mbHideWindow;

	// Input
	cWidgetCheckBox *mpChBInvertMouse;
	cWidgetCheckBox *mpChBSmoothMouse;

	cWidgetLabel	*mpLMouseSensitivity;
	cWidgetSlider	*mpSMouseSensitivity;

#ifdef USE_GAMEPAD
	cWidgetCheckBox *mpChBGamepadInvertLook;
	cWidgetLabel	*mpLGamepadLookSensitivity;
	cWidgetSlider	*mpSGamepadLookSensitivity;
#endif

	cWidgetButton	*mpBKeyConfig;

	// Sound
	cWidgetLabel  *mpLVolume;
	cWidgetSlider *mpSVolume;

	cWidgetLabel  *mpLSndDevice;
	cWidgetComboBox *mpCBSndDevice;

	float mfGammaMin;
	float mfGammaMax;
	float mfGammaStep;

	float mfMouseSensitivityMin;
	float mfMouseSensitivityMax;
	float mfMouseSensitivityStep;
	
#ifdef USE_GAMEPAD
	float mfGamepadLookSensitivityMin;
	float mfGamepadLookSensitivityMax;
	float mfGamepadLookSensitivityStep;
#endif

	float mfVolumeMin;
	float mfVolumeMax;
	float mfVolumeStep;

	// Old values
	tString msOldLanguage;
	float mfOldGamma;
	float mfOldSensitivity;
	float mfOldVolume;

	bool mbTipFadeRestart;
	bool mbTipWidgetUpdated;
	bool mbTipTextReset;
	iWidget* mpCurrentTipWidget;
	tOptionDataVec mvOptionData;

	bool mbSettingInitialValues;

	bool mbKeyConfigOpen;

	tVideoModeVec mvScreenSizes;

	tSoundDeviceVec mvSoundDevices;

	// Initial config
	static cResourceVarsObject mInitialValues;
	static cResourceVarsObject mCurrentValues;

	///////////////////////
	// Widget callbacks
	bool Window_OnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Window_OnUpdate);

	bool Option_OnMouseOver(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Option_OnMouseOver);
	
	bool Option_OnChangeValue(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Option_OnChangeValue);	

	bool GammaSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(GammaSlider_OnMove);

	bool MouseSensitivitySlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MouseSensitivitySlider_OnMove);

#ifdef USE_GAMEPAD
	bool GamepadLookSensitivitySlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(GamepadLookSensitivitySlider_OnMove);
#endif

	bool SoundSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SoundSlider_OnMove);

	bool PressToggleShowGfxOptions(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressToggleShowGfxOptions);

	bool PressKeyConfig(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressKeyConfig);

	bool PressOK(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressOK);

	bool PressCancel(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressCancel);

	bool UIPressCancel(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIPressCancel);

	bool MessageBoxCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MessageBoxCallback);
};

//----------------------------------------------


#endif // LUX_MAIN_MENU_OPTIONS_H
