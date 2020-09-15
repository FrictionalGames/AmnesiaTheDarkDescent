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

#ifndef HPLEDITOR_EDITOR_WINDOW_H
#define HPLEDITOR_EDITOR_WINDOW_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "EditorBaseClasses.h"
#include "EditorTypes.h"
#include "EditorVar.h"

//------------------------------------------------------

class iEditorBase;
class iEditMode;
class iWidgetContainer;
class iEditorInput;
class cEditorInputBool;
class cEditorInputNumber;
class cEditorInputEnum;
class cEditorInputText;
class cEditorInputFile;
class cEditorInputVec2;
class cEditorInputVec3;
class cEditorInputColor;
class cEditorInputColorFrame;

//------------------------------------------------------
//------------------------------------------------------

//------------------------------------------------------

class iEditorWindow
{
public:
	iEditorWindow(iEditorBase* apEditor,const tString& asName);
	virtual ~iEditorWindow();

	cGuiSet* GetSet() { return mpSet; }

	cWidgetFrame* GetBGFrame() { return mpBGFrame; }

	iEditorBase* GetEditor() { return mpEditor; }
	
	tString& GetName() { return msName; }

	void SetPosition(const cVector3f& avPosition);
	const cVector3f& GetPosition();

	void SetSize(const cVector2f& avSize);
	const cVector2f& GetSize();

	void SetEnabled(bool abX);
	bool IsEnabled() { return mbEnabled; }
	void SetVisible(bool abX);
	bool IsVisible() { return mbVisible; }

	void SetActive(bool abX);
	bool IsActive();


	void Init();
	void InitLayout();

	void Update(float afTimeStep) { OnUpdate(afTimeStep); }

	////////////////////////////////////////////
	// Editor Inputs
	cEditorInputBool* CreateInputBool(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent=NULL);
	cEditorInputText* CreateInputString(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent=NULL, float afBoxWidth=100);
	cEditorInputFile* CreateInputFile(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent=NULL, float afBoxWidth=75);
	cEditorInputNumber* CreateInputNumber(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent=NULL, float afBoxWidth=50, float afNumericAdd=0);
	cEditorInputEnum* CreateInputEnum(const cVector3f& avPos, const tWString& asLabel, const tString& asName, const tWStringList& alstValues, iWidget* apParent=NULL, float afBoxWidth=100);
	cEditorInputVec2* CreateInputVec2(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent=NULL, float afBoxWidth=50, const tWStringList& alstLabels=tWStringList(), eEditorInputLayoutStyle aStyle=eEditorInputLayoutStyle_RowLabelOnTop, float afNumericAdd=0);
	cEditorInputVec3* CreateInputVec3(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent=NULL, float afBoxWidth=50, const tWStringList& alstLabels=tWStringList(), eEditorInputLayoutStyle aStyle=eEditorInputLayoutStyle_RowLabelOnTop, float afNumericAdd=0);
	//cEditorInputColor* CreateInputColor(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent=NULL, float afBoxWidth=50);
	cEditorInputColorFrame* CreateInputColorFrame(const cVector3f& avPos, const tWString& asLabel, const tString& asName, iWidget* apParent=NULL);

	void DestroyInput(iEditorInput* apInput);

	virtual void OnWorldModify(){}
	virtual void OnSelectionChange(){}

protected:
	virtual void OnInit(){}
	virtual void OnInitLayout(){}
	virtual void OnLoadLevel(){}
	virtual void OnUpdate(float afTimeStep){}
	virtual void OnSetActive(bool abX){}

	virtual void OnChangeSize(){}

	virtual void PostInitLayout(){}

	void SetUpInput(iEditorInput* apInput);

	bool EditorInputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(EditorInputCallback);
	virtual bool WindowSpecificInputCallback(iEditorInput* apInput) { return true; }

    ////////////////////////////////
	// Data
	tString msName;

	cGuiSkin* mpSkin;
	cGuiSet* mpSet;

	iEditorBase* mpEditor;

	/////////////////////////////////
	// Window background
	cWidgetFrame* mpBGFrame;

	cVector3f mvPos;
	cVector2f mvSize;

	bool mbEnabled;
	bool mbVisible;

	std::list<iEditorInput*> mlstInputs;
};

//------------------------------------------------------

class iEditModeWindow : public iEditorWindow
{
public:
	iEditModeWindow(iEditorEditMode* apEditMode);

	iEditorEditMode* GetEditMode() { return mpEditMode; }

protected:
	iEditorEditMode* mpEditMode;
};

class iEditModeObjectCreatorWindow : public iEditModeWindow
{
public:
	iEditModeObjectCreatorWindow(iEditorEditMode* apEditMode);

	const cVector3f& GetRotate();
	void SetScaleUpdated() { mbScaleUpdated=true; }
	virtual void UpdateScale();

	const cVector3f& GetScale();
	void SetRotateUpdated() { mbRotateUpdated=true; }
	virtual void UpdateRotate();

protected:
	void AddCreateOnSurfaceControls(bool abAddNormalControls=false);
	void AddScaleControls();
	void AddRotateControls();

	void OnSetActive(bool abX);

	bool SurfaceButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SurfaceButton_OnPressed);
	bool WindowSpecificInputCallback(iEditorInput* apInput);

	cWidgetDummy* mpGSurface;
	cEditorInputBool* mpInpCreateOnSurface;
	std::vector<cWidgetButton*> mvBSurfaceTypes;

	cWidgetDummy* mpGScale;
	cEditorInputBool* mpInpRandScale;
	cEditorInputVec3* mpInpMinScale;
	cEditorInputVec3* mpInpMaxScale;

	cWidgetDummy* mpGRotate;
	cEditorInputBool* mpInpRandRotate;
	cEditorInputVec3* mpInpMinRotate;
	cEditorInputVec3* mpInpMaxRotate;

	cVector3f mvRotate;
	cVector3f mvScale;

	bool mbScaleUpdated;
	bool mbRotateUpdated;

	std::vector<cGuiGlobalShortcut*> mvShortcuts;
};


//------------------------------------------------------

class iEditorWindowPopUp : public iEditorWindow
{
public:
	iEditorWindowPopUp(iEditorBase* apEditor, const tString& asName, bool abGetEditorAttention=true, bool abGetGuiAttention=false, 
							bool abDestroyOnDisable=false, const cVector2f& avSize=cVector2f(800,600));

protected:
	bool Window_OnClose(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Window_OnClose);

	virtual void OnInitLayout();
	virtual void OnSetActive(bool abX);

	cVector2f mvSize;
	float mfZ;
	bool mbGetEditorAttention;
	bool mbGetGuiAttention;
	bool mbDestroyOnDisable;

	iWidget* mpPrevAttention;

	cWidgetWindow* mpWindow;
};

#endif //HPLEDITOR_EDITOR_WINDOW_H

