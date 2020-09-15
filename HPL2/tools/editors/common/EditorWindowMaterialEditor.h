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

#ifndef HPLEDITOR_EDITOR_WINDOW_MATERIAL_EDITOR_H
#define HPLEDITOR_EDITOR_WINDOW_MATERIAL_EDITOR_H

#include "EditorWindow.h"
#include "EditorViewport.h"
#include "EditorVar.h"

class cEditorWindowMaterialEditor;
class cTextureUnitWrapper;
class cTextureUnitPanel;
class cMaterialWrapper;

//-------------------------------------------------------------------

class cEditorClassMaterial : public iEditorClass
{
public:
	cEditorClassMaterial(cEditorWindowMaterialEditor* apWindow);

	bool Create(void* apData);

	cEditorClassInstance* CreateInstance();

protected:

	cEditorWindowMaterialEditor* mpWindow;
	tEditorVarVec mvVariables;
};

//-------------------------------------------------------------------

class cTextureWrapper
{
	friend class cMaterialWrapper;
public:
	cTextureWrapper(cMaterialWrapper* apMat, eMaterialTexture aUnit);
	~cTextureWrapper();

	iTexture* GetTexture() { return mpTexture; }

	void Reset();

	void Load(cXmlElement* apElement);
	void Save(cXmlElement* apElement);

	void Reload();
	void Update();

	void CreateFromTexture(iTexture* apTexture);

	bool CheckFileIsUpdated();

	void SetEnabled(bool abX);
	void SetNeedsReload() { mbNeedsReload = true; }
	void SetUpdated() {mbUpdated = true; }

	bool IsValid() { return mbValid; }
	bool IsEnabled() { return mbEnabled; }

	const tWString& GetFile() { return msFile; }
	const tString& GetWrap() { return msWrap; }
	const tString& GetAnimMode() { return msAnimMode; }
	float GetFrameTime() { return mfFrameTime; }
	const tString& GetType() { return msType; }

	bool GetCompressed() { return mbCompressed; }
	bool GetUseMipMaps() { return mbMipMaps; }

	void SetFile(const tWString& asX);
	void SetWrap(const tWString& asX);
	void SetAnimMode(const tWString& asX);
	void SetFrameTime(float afX);

	void SetCompressed(bool abX);
	void SetUseMipMaps(bool abX);

protected:
	static eTextureType GetTextureTypeFromBitmap(cBitmap* apBmp);
	cMaterialWrapper* mpMat;
	eMaterialTexture mUnit;

	bool mbValid;
	bool mbEnabled;
	bool mbNeedsReload;
	bool mbUpdated;

	tWString msFile;
	bool mbMipMaps;
	bool mbCompressed;
	tString msWrap;
	tString msType;
	tString msAnimMode;
	float mfFrameTime;

	iTexture* mpTexture;
	cDate mTimeStamp;
};

//-------------------------------------------------------------------

class cMaterialWrapper
{
	friend class cTextureWrapper;
public:
	cMaterialWrapper(cEditorWindowMaterialEditor* apWindow);
	~cMaterialWrapper();

	bool IsSaveable();

	void Reset();

	void Load(const tWString& asFilename);
	void Save(const tWString& asFilename);

	cMaterial* GetPreviewMaterial();

	cTextureWrapper* GetTextureWrapper(eMaterialTexture aUnit) { return mvTextures[aUnit]; }

	cEditorClassInstance* GetClass() { return mpClass; }

	void SetType(const tWString& asType);
	void SetPhysicsMaterial(const tWString& asMat);
	void SetDepthTest(bool abX);
	void SetUseAlpha(bool abX);
	void SetBlendMode(const tWString& asMode);
	void SetHeightMapScale(float afX);
	void SetHeightMapBias(float afX);

	void AddUVAnim(eMaterialUvAnimation aType, float afSpeed, float afAmp, eMaterialAnimationAxis aAxis);
	void RemoveUVAnim(int alIdx);
	cMaterialUvAnimation* GetUVAnim(int alIdx);
	int GetUVAnimNum() { return (int)mvUVAnimations.size(); }
	void ClearUVAnims();

	const tWString& GetType() { return msType; }
	const tWString& GetPhysicsMat() { return msPhysicsMat; }
	bool GetDepthTest() { return mbDepthTest; }
	bool GetUseAlpha() { return mbUseAlpha; }
	const tWString& GetBlendMode() { return msBlendMode; }

	iMaterialType* GetTypePointer();
	void SetTextureUnitEnabled(eMaterialTexture aIdx, bool abX) { mvTextures[aIdx]->mbEnabled = abX; }
	void SetTextureUnitUpdated(eMaterialTexture aIdx) { mvTextures[aIdx]->mbUpdated = true; }
	void SetTextureUnitNeedsReload(eMaterialTexture aIdx) { mvTextures[aIdx]->mbNeedsReload = true; }

	void UpdateMaterialInMemory(const tString& asName);

	bool ReloadTextures();


	void SetUpdated() { mbPreviewUpdated = true; }

protected:
	cEditorWindowMaterialEditor* mpMatEditor;

	tWString msType;
	tWString msPhysicsMat;
	bool mbDepthTest;
	bool mbUseAlpha;
	tWString msBlendMode;

	std::vector<cTextureWrapper*> mvTextures;
	std::vector<iTexture*> mvDefaultTextures;

	std::vector<cMaterialUvAnimation> mvUVAnimations;

	cEditorClassMaterial* mpMatClass;
	cEditorClassInstance* mpClass;

	bool mbPreviewUpdated;
	bool mbTypeChanged;
	cMaterial* mpPreviewMat;

	bool mbUpdateMaterialInMemory;
};

//-------------------------------------------------------------------

class cTextureUnitPanel
{
	friend class cEditorWindowMaterialEditor;
public:
	cTextureUnitPanel(cEditorWindowMaterialEditor* apWin, eMaterialTexture aUnit);
	~cTextureUnitPanel();

	void Reset();
	void Update();

	iWidget* GetHandle() { return mpHandle; }
protected:
	bool PanelSpecificInputCallback(iEditorInput* apInput);
	void UpdateTexture();

	cEditorWindowMaterialEditor* mpWindow;

	eMaterialTexture mUnit;
	cTextureWrapper* mpTextureWrapper;

	cWidgetDummy* mpHandle;
	cWidgetLabel* mpLUnit;
	
	cWidgetDummy* mpInputs;
    cEditorInputFile* mpInpFile;
	cEditorInputEnum* mpInpAnimMode;
	cEditorInputNumber* mpInpFrameTime;
	cEditorInputBool* mpInpMipMaps;
	cEditorInputEnum* mpInpWrap;
	cEditorInputEnum* mpInpType;
	

	cWidgetFrame* mpFThumb;
	cWidgetImage* mpImgThumb;

	tWString msLastPath;
};

//-------------------------------------------------------------------

class cEditorWindowMaterialEditor : public iEditorWindowPopUp, public iEditorViewport
{
	friend class cTextureUnitPanel;
public:
	cEditorWindowMaterialEditor(iEditorBase* apEditor, iFrameBuffer* apFB, const tWString& asMatFile=_W(""), cEditorInputFile* apInput=NULL, bool abStandAlone=false);
	~cEditorWindowMaterialEditor();

	cWidgetFrame* GetFrameUnits() { return mpFUnits; }

	cMaterialWrapper* GetMaterial() { return mpMaterial; }

	void SetUpUnits();
	void SetUpPanels();
	void SetUpVars();

	void Command_New();
	void Command_Open();
	void Command_Save();
	void Command_SaveAs();

	void Reset();

	bool LoadMaterial(const tWString& asFilename);
	void SaveMaterial(const tWString& asFilename);

	void SetPreviewUpdated() { mbPreviewNeedsUpdate = true; }

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


	void OnInit();
	void OnInitLayout();
	void PostInitLayout();

	void OnSetActive(bool abX);

	void OnUpdate(float afTimeStep);

	bool OnViewportUpdate(const cGuiMessageData& aData);
	bool OnViewportMouseMove(const cGuiMessageData& aData);
	bool OnViewportMouseDown(const cGuiMessageData& aData);
	bool OnViewportMouseUp(const cGuiMessageData& aData);

	bool MenuCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MenuCallback);

	bool WindowSpecificInputCallback(iEditorInput* apInput);

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
    cEditorInputEnum* mpInpMatType;
	cEditorInputEnum* mpInpPhysicsMat;
	cEditorInputBool* mpInpDepthTest;
	cEditorInputNumber* mpInpValue;
	cEditorInputEnum* mpInpBlendMode;

	// UV Animations
	cEditorInputEnum* mpInpUVAnimation;
	cWidgetButton* mpBAddAnim;
	cWidgetButton* mpBRemAnim;
	cEditorInputEnum* mpInpUVAnimType;
	cEditorInputEnum* mpInpUVAnimAxis;
	cEditorInputNumber* mpInpUVAnimSpeed;
	cEditorInputNumber* mpInpUVAnimAmp;

	// Preview window
	cEditorInputEnum* mpInpPreviewModel;
	cEditorInputEnum* mpInpBGType;
	cEditorInputFile* mpInpBGCubeMap;
	cEditorInputColorFrame* mpInpBGColor;
	cEditorInputBool* mpInpRotateModel;
	cEditorInputColorFrame* mpInpFixedLightColor;
	cEditorInputColorFrame* mpInpMovableLightColor;

	// Texture units
	cWidgetFrame* mpFUnits;
	std::vector<cEditorInputBool*> mvUnitSwitches;
	std::vector<cTextureUnitPanel*> mvUnitPanels;

	// Material Variables
	cWidgetFrame* mpFMaterialVars;
	cEditorVarInputPanel* mpInputPanel;

	cMaterialWrapper* mpMaterial;

	bool mbPreviewNeedsUpdate;

	tWStringVec mvTempLoadedFiles;
	tWString msMatFilename;
	tWString msInitMatFile;

	tWString msLastTexturePath;
	
	cEditorInputFile* mpDestInput;

	bool mbStandAlone;
};

//-------------------------------------------------------------------

#endif // HPLEDITOR_EDITOR_WINDOW_MATERIAL_EDITOR_H
