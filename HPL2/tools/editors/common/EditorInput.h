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

#ifndef HPLEDITOR_EDITOR_INPUT_H
#define HPLEDITOR_EDITOR_INPUT_H

#include "StdAfx.h"
#include "EditorTypes.h"
#include "EditorBaseClasses.h"

using namespace hpl;
class iEditorWindow;

//------------------------------------------------------

////////////////////////////////////////////////////////
// INPUTS
////////////////////////////////////////////////////////

//------------------------------------------------------

class iEditorInput
{
public:
	iEditorInput(iEditorWindow* apWindow, 
				 const cVector3f& avPos, 
				 const tWString& asLabel, 
				 const tString& asName, 
				 iWidget* apParent=NULL,
				 eEditorInputLayoutStyle aStyle=eEditorInputLayoutStyle_ColumnLabelOnTop);
	virtual ~iEditorInput();

	iEditorWindow* GetWindow() { return mpWindow; }

	iWidget* GetHandle() { return mpHandle; }
	virtual iWidget* GetInputWidget(int alIdx=0)=0;

	const tString& GetName() { return mpHandle->GetName(); }
	const tWString& GetValue() { return mpHandle->GetText(); }

	virtual void SetValue(const tWString& asX, bool abGenerateCallback=true, bool abCopyToInput=true);
	virtual void UpdateValue()=0;
	virtual void CopyValueToInput()=0;

	void SetPosition(const cVector3f& avPos);
	const cVector3f& GetPosition() { return mpHandle->GetLocalPosition(); }

	virtual void SetLabel(const tWString& asLabel)=0;
	virtual void SetLayoutStyle(eEditorInputLayoutStyle aStyle) { mLayoutStyle = aStyle; }
	virtual void UpdateLayout(){ UpdateSize(); }

	void AddCallback(eEditorInputCallback aCallback, void* apCallbackObject, tGuiCallbackFunc apCallback);

	static void SetFontSize(const cVector2f& avSize) { mvFontSize = avSize; }
	static const cVector2f& GetFontSize() { return mvFontSize; }

	cVector2f GetSize() { return mpHandle->GetSize(); }
	void UpdateSize();

	void SetTabWidth(float afX) { mfTabWidth = afX; }

	void SetUserData(void* apData) { mpUserData = apData; }
	void* GetUserData() { return mpUserData; }

protected:
	bool ValueEnter(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(ValueEnter);

	cGuiSet* mpSet;
	iEditorWindow* mpWindow;
	cWidgetDummy* mpHandle;

	void* mpUserData;
	
	static cVector2f mvFontSize;
	eEditorInputLayoutStyle mLayoutStyle;
	float mfTabWidth;
};

//------------------------------------------------------

class iEditorInputLabeled : public iEditorInput
{
public:
	iEditorInputLabeled(iEditorWindow* apWindow, 
						const cVector3f& avPos, 
						const tWString& asLabel, 
						const tString& asName,
						iWidget* apParent=NULL,
						eEditorInputLayoutStyle aStyle=eEditorInputLayoutStyle_RowLabelOnTop);
	virtual ~iEditorInputLabeled();

	void SetLabel(const tWString& asLabel);

protected:
	cWidgetLabel* mpL;
};

//------------------------------------------------------

class cEditorInputBool : public iEditorInput
{
public:
	cEditorInputBool(iEditorWindow* apWindow, 
					 const cVector3f& avPos, 
					 const tWString& asLabel, 
					 const tString& asName, 
					 iWidget* apParent=NULL);
	virtual ~cEditorInputBool();

	void SetLabel(const tWString& asLabel);

	iWidget* GetInputWidget(int alX=0) { return mpChB; }

	void UpdateValue();
	void CopyValueToInput();

	void SetValue(bool abX, bool abGenerateCallback = true, bool abCopyToInput = true); 
	bool GetValue();

	void UpdateSize();

protected:
	cWidgetCheckBox* mpChB;
};

//----------------------------------------------------------

class cEditorInputText :public iEditorInputLabeled
{
public:
	cEditorInputText(iEditorWindow* apWindow, 
					 const cVector3f& avPos, 
					 const tWString& asLabel, 
					 const tString& asName, 
					 float afTextBoxWidth, 
					 int alAmount, 
					 bool abNumeric,
					 float afNumericAdd=0,
					 iWidget* apParent=NULL,
					 const tWStringList& alstLabels=tWStringList());
	virtual ~cEditorInputText();

	iWidget* GetInputWidget(int alIdx=0) { return mvTB[alIdx]; }

	void SetLowerBound(bool abX, float afX);
	void SetUpperBound(bool abX, float afX);

	void SetDecimals(int alX);

	void SetCanEdit(bool abX);

	void UpdateValue();
	void CopyValueToInput();

	void UpdateLayout();

	int GetModifiedTextBoxId() { return mlCallbackTBId; }
protected:
	bool TBIdCheckerCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(TBIdCheckerCallback);

	std::vector<cWidgetLabel*> mvL;
	std::vector<cWidgetTextBox*> mvTB;
	int mlDecimals;
	int mlCallbackTBId;
};

//------------------------------------------------------

class cEditorInputNumber : public cEditorInputText
{
public:
	cEditorInputNumber(iEditorWindow* apWindow,
					   const cVector3f& avPos,
					   const tWString& asLabel,
					   const tString& asName,
					   float afTextBoxWidth,
					   iWidget* apParent=NULL,
					   float afNumericAdd=0) : cEditorInputText(apWindow, avPos, asLabel, asName, afTextBoxWidth,1, true,afNumericAdd,apParent)
	{
	}
	
	void SetValue(float afX, bool abGenerateCallback = true, bool abCopyToInput = true);
	float GetValue();

protected:
};

//------------------------------------------------------

class cEditorInputVec2 : public cEditorInputText
{
public:
	cEditorInputVec2(iEditorWindow* apWindow,
					 const cVector3f& avPos,
					 const tWString& asLabel,
					 const tString& asName,
					 float afTextBoxWidth,
					 const tWStringList& alstLabels=tWStringList(),
					 iWidget* apParent=NULL,
					   float afNumericAdd=0) : cEditorInputText(apWindow, avPos, asLabel, asName, afTextBoxWidth,2, true,afNumericAdd,apParent, alstLabels)
	{
	}

	void SetValue(const cVector2f& avX, bool abGenerateCallback = true, bool abCopyToInput = true); 
	cVector2f GetValue();
};

//------------------------------------------------------

class cEditorInputVec3 : public cEditorInputText
{
public:
	cEditorInputVec3(iEditorWindow* apWindow,
					 const cVector3f& avPos,
					 const tWString& asLabel,
					 const tString& asName,
					 float afTextBoxWidth,
					 const tWStringList& alstLabels=tWStringList(),
					 iWidget* apParent=NULL,
					 float afNumericAdd=0): cEditorInputText(apWindow, avPos, asLabel, asName, afTextBoxWidth,3, true, afNumericAdd, apParent, alstLabels)
	{
	}

	void SetValue(const cVector3f& avX, bool abGenerateCallback = true, bool abCopyToInput = true); 
	cVector3f GetValue();
};

//------------------------------------------------------

class cEditorInputFile : public cEditorInputText
{
public:
	cEditorInputFile(iEditorWindow* apWindow,
					 const cVector3f& avPos, 
					 const tWString& asLabel, 
					 const tString& asName, 
					 float afTextBoxWidth, 
					 iWidget* apParent=NULL);
	virtual ~cEditorInputFile();

	void SetBrowserType(eEditorResourceType aType);
	void SetBrowserSubType(int alX) { mlBrowserSubtype = alX; }

	void UpdateLayout();

	void SetShowPath(bool abX);
	bool GetShowPath() { return mbShowPath; }

	void SetValue(const tWString& asX, bool abGenerateCallback=true, bool abCopyToInput=true);

	const tWString& GetFullPath() { return msFullPath; }

	void SetInitialPath(const tWString& asPath);

	void AddCustomFileType(const tWString& asType);
protected:
	void CopyValueToInput();

	bool BrowseButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(BrowseButton_OnPressed);

	bool Browser_OnOkay(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Browser_OnOkay);

	bool NewButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(NewButton_OnPressed);


	/////////////////////////////////
	// File browsing
	eEditorResourceType mBrowserType;
	int mlBrowserSubtype;

	cWidgetButton* mpBBrowse;
	cWidgetButton* mpBNew;
	tWStringVec mvTempLoadedFiles;
	tWString msTempLoadedFile;
	tWString msInitialPath;

	tWStringList mlstCustomFileTypes;

	tWString msFilename;
	tWString msFullPath;
	bool mbShowPath;
};

//------------------------------------------------------

class cEditorInputEnum : public iEditorInputLabeled
{
public:
	cEditorInputEnum(iEditorWindow* apWindow,
					 const cVector3f& avPos,
					 const tWString& asLabel, 
					 const tString& asName,
					 float afComboBoxWidth,
					 iWidget* apParent=NULL);

	iWidget* GetInputWidget(int alIdx=0) { return mpCB; }

	void AddValue(const tWString& asX);
	void RemoveValue(int alIndex);
	void ClearValues();

	int GetNumValues();
	void SetValue(int alX, bool abGenerateCallback=true, bool abCopyToInput=true);
	int GetValue();
	//void SetSelectedValue();
	//int GetSelectedValue();
	//int GetSelectedValueIndex() { return 0; }
	const tWString& GetEnumValue(int alIndex);

	void SetMaxShownItems(int alNum);

	void UpdateValue();
	void CopyValueToInput();

	void UpdateLayout();
protected:

	cWidgetComboBox* mpCB;
};

//------------------------------------------------------

class cEditorInputColorFrame : public iEditorInputLabeled
{
public:
	cEditorInputColorFrame(iEditorWindow* apWindow, 
						   const cVector3f& avPos, 
						   const tWString& asLabel, 
						   const tString& asName, 
						   iWidget* apParent=NULL);

	iWidget* GetInputWidget(int alIdx=0) { return mpFColor; }

	void SetValue(const cColor& aCol, bool abGenerateCallback=true, bool abCopyToInput=true);
	const cColor& GetValue();

	void UpdateValue();
	void CopyValueToInput();

	void UpdateLayout();
protected:
	bool Frame_OnClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Frame_OnClick);
	bool ColorPicker_OnOK(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ColorPicker_OnOK);

	cWidgetFrame* mpFColor;
	cColor mCol;
};

//------------------------------------------------------



#endif // HPLEDITOR_EDITOR_INPUT_H
