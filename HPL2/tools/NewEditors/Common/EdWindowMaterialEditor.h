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

#ifndef ED_WINDOW_MATERIAL_EDITOR_H
#define ED_WINDOW_MATERIAL_EDITOR_H

#include "../Common/EdWindowHandler.h"
#include "../Common/EdViewport.h"
#include "../Common/EdVar.h"
#include "../Common/EdCamera.h"

class cEdWindowMaterialEditor;
class cTextureUnitWrapper;
class cTextureUnitPanel;
class cHPLMaterial;
class cHplTexture;

class iCamMode;

//-------------------------------------------------------------------

class cCamModeMatEd : public iCamMode
{
public:
	cCamModeMatEd(cEdCamera*, int);
protected:
	void OnSetActive(bool) {}
};

//-------------------------------------------------------------------

class cCamControllerMatEd : public iCamController
{
public:
	cCamControllerMatEd(cEdCamera*);

	bool OnViewportMouseDown(const cViewportClick&);
	bool OnViewportMouseUp(const cViewportClick&);

	bool IsActive() { return mbOrbiting || mbZooming || mbWheelUp || mbWheelDn; }
	bool IsLockedToGrid() { return false; }

	void LockToGrid(bool) {}

	void OnUpdate(const cViewportClick&);

protected:
	void UpdateInput() {}

	bool mbOrbiting;
	bool mbZooming;
	bool mbWheelUp;
	bool mbWheelDn;
};

//-------------------------------------------------------------------

class cTextureUnitPanel
{
	friend class cEdWindowMaterialEditor;
public:
	cTextureUnitPanel(cEdWindowMaterialEditor* apWin, eMaterialTexture aUnit);
	~cTextureUnitPanel();

	void Reset();
	void Update();

	iWidget* GetHandle() { return mpHandle; }
protected:
	bool PanelSpecificInputCallback(iEdInput* apInput);
	void UpdateTexture();

	cEdWindowMaterialEditor* mpWindow;

	eMaterialTexture mUnit;
	cHplTexture* mpTextureWrapper;

	cWidgetDummy* mpHandle;
	cWidgetLabel* mpLUnit;
	
	cWidgetDummy* mpInputs;
    cEdInputFile* mpInpFile;
	cEdInputEnum* mpInpAnimMode;
	cEdInputNumber* mpInpFrameTime;
	cEdInputBool* mpInpMipMaps;
	cEdInputEnum* mpInpWrap;
	cEdInputEnum* mpInpType;
	

	cWidgetFrame* mpFThumb;
	cWidgetImage* mpImgThumb;

	tWString msLastPath;
};

//-------------------------------------------------------------------

class cEdWindowMaterialEditor : public iEdViewport
{
	friend class cTextureUnitPanel;
	friend class cHPLMaterial;
public:
	cEdWindowMaterialEditor(iEditor* apEditor, const tWString& asMatFile=_W(""), cEdInputFile* apInput=NULL);
	~cEdWindowMaterialEditor();

	bool IsStandalone();

	cWidgetFrame* GetFrameUnits() { return mpFUnits; }

	cHPLMaterial* GetMaterial() { return mpMaterial; }

	void SetUpUnits();
	void SetUpPanels();
	void SetUpVars();

	void SetPreviewUpdated() { mbPreviewNeedsUpdate = true; }

	void DispatchMessage(eEdModuleMsg, void*);

protected:
	void CreateMaterial(const tString& asMat="");
	void UpdatePreview(cMaterial* apMat);

	bool FilePickerSave(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FilePickerSave);

	bool FilePickerLoad(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FilePickerLoad);

	bool ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ButtonPressed);

	void UpdateUVAnimInputs();

	void SetWindowCaption(const tWString& asCaption);

	iWidget* CreateBGWidget(const cVector3f&, const cVector2f&, iWidget*);

	void OnReset();
	void OnInit();
	void OnCreateLayout();
	void PostInitLayout();

	void OnSetActive(bool abX);

	void OnUpdate();

	bool OnViewportUpdate(const cGuiMessageData& aData);
	bool OnViewportMouseMove(const cGuiMessageData& aData);
	bool OnViewportMouseDown(const cGuiMessageData& aData);
	bool OnViewportMouseUp(const cGuiMessageData& aData);

	bool MenuCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MenuCallback);

	bool WindowSpecificInputCallback(iEdInput* apInput);

	/////////////////////////////////////////
	// World stuff
	cWorld* mpMatWorld;
	cMeshEntity* mpCurrentPreviewEntity;
	std::vector<cMeshEntity*> mvPreviewEntities;
	bool mbRotationEnabled;
	cMatrixf mmtxRotation;
	cLightPoint* mpFixedLight;
	cLightPoint* mpMovableLight;

	bool mbMoveLight;
	bool mbMoveCamera;

	/////////////////////////////////////////
	// GUI stuff

	// Menu
	cWidgetMenuItem* mpMenuNew;
	cWidgetMenuItem* mpMenuOpen;
	cWidgetMenuItem* mpMenuSave;
	cWidgetMenuItem* mpMenuSaveAs;
	cWidgetMenuItem* mpMenuExit;

	// General settings
    cEdInputEnum* mpInpMatType;
	cEdInputEnum* mpInpPhysicsMat;
	cEdInputBool* mpInpDepthTest;
	cEdInputNumber* mpInpValue;
	cEdInputEnum* mpInpBlendMode;

	// UV Animations
	cEdInputEnum* mpInpUVAnimation;
	cWidgetButton* mpBAddAnim;
	cWidgetButton* mpBRemAnim;
	cEdInputEnum* mpInpUVAnimType;
	cEdInputEnum* mpInpUVAnimAxis;
	cEdInputNumber* mpInpUVAnimSpeed;
	cEdInputNumber* mpInpUVAnimAmp;

	// Preview window
	cEdInputEnum* mpInpPreviewModel;
	cEdInputEnum* mpInpBGType;
	cEdInputFile* mpInpBGCubeMap;
	cEdInputColor* mpInpBGColor;
	cEdInputBool* mpInpRotateModel;
	cEdInputColor* mpInpFixedLightColor;
	cEdInputColor* mpInpMovableLightColor;

	// Texture units
	cWidgetFrame* mpFUnits;
	std::vector<cEdInputBool*>		mvUnitSwitches;

	// Material Variables
	cWidgetFrame* mpFMaterialVars;
	iEdVarInputPanel* mpInputPanel;

	cHPLMaterial* mpMaterial;

	bool mbPreviewNeedsUpdate;

	tWStringVec mvTempLoadedFiles;
	tWString msMatFilename;
	tWString msInitMatFile;

	tWString msLastTexturePath;
	
	cEdInputFile* mpDestInput;

	bool mbStandAlone;
	bool mbInitialized;

	bool mbTypeChanged;
};

//-------------------------------------------------------------------

#endif // ED_WINDOW_MATERIAL_EDITOR_H
