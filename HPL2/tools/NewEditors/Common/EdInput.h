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

#ifndef ED_INPUT_H
#define ED_INPUT_H

#include "../Common/StdAfx.h"
#include "../Common/EditorTypes.h"
//#include "../Common/Editor.h"

using namespace hpl;

//------------------------------------------------------

class iEdWindow;

//------------------------------------------------------

enum eEdInputType
{
	eEdInputType_Bool,
	eEdInputType_String,
	eEdInputType_File,
	eEdInputType_Number,
	eEdInputType_Vec2,
	eEdInputType_Vec3,
	eEdInputType_Enum,

	eEdInputType_LastEnum,
};

enum eEdInpStyle
{
	eEdInpStyle_RowLabelOnLeft,
	eEdInpStyle_RowLabelOnTop,
	eEdInpStyle_ColumnLabelOnLeft,
	eEdInpStyle_ColumnLabelOnTop,

	eEdInpStyle_LastEnum,
};

enum eEdInputCallback
{
	eEdInputCallback_ValueEnter,
	eEdInputCallback_ValueChange,
	eEdInputCallback_Browse,

	eEdInputCallback_LastEnum,
};


//------------------------------------------------------

////////////////////////////////////////////////////////
// INPUT BASE
////////////////////////////////////////////////////////

//------------------------------------------------------

class iEdInput
{
public:
	iEdInput(iEdWindow* apWindow, 
				iWidget* apParent=NULL,
				eEdInpStyle aStyle=eEdInpStyle_ColumnLabelOnTop);
	virtual ~iEdInput();

	iEdWindow* GetWindow() { return mpWindow; }

	iWidget* GetHandle() { return mpHandle; }
	virtual iWidget* GetInputWidget(int)=0;

	const tString& GetName() { return mpHandle->GetName(); }
	const tWString& GetValue() { return mpHandle->GetText(); }
	tString GetValue8Char() { return cString::To8Char(mpHandle->GetText()); }

	virtual void SetValue(const tWString& asX, bool abGenerateCallback=true, bool abCopyToInput=true, bool abSkipChecks=false);
	virtual void UpdateValue()=0;
	virtual void CopyValueToInput()=0;

	void SetPosition(const cVector3f& avPos);
	const cVector3f& GetPosition() { return mpHandle->GetLocalPosition(); }

	virtual void SetLabel(const tWString& asLabel)=0;
	virtual void SetStyle(eEdInpStyle aStyle) { mStyle = aStyle; }
	virtual void UpdateLayout(){ UpdateSize(); }

	void AddCallback(eEdInputCallback aCallback, void* apCallbackObject, tGuiCallbackFunc apCallback);

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

	iEdWindow* mpWindow;
	cWidgetDummy* mpHandle;
	eEdInpStyle mStyle;
	float mfTabWidth;

	void* mpUserData;

	static cGuiSet* mpSet;
	static cVector2f mvFontSize;
};

//------------------------------------------------------

typedef std::list<iEdInput*> tInputList;
typedef tInputList::iterator tInputListIt;

//------------------------------------------------------

////////////////////////////////////////////////////////
// LABELED INPUT BASE
////////////////////////////////////////////////////////

//------------------------------------------------------

class iEdInputLabeled : public iEdInput
{
public:
	iEdInputLabeled(iEdWindow* apWindow, 
						const tWString& asLabel, 
						iWidget* apParent=NULL,
						eEdInpStyle aStyle=eEdInpStyle_RowLabelOnTop);
	virtual ~iEdInputLabeled();

	void SetLabel(const tWString& asLabel);

protected:
	cWidgetLabel* mpL;
};

//------------------------------------------------------

////////////////////////////////////////////////////////
// BOOL INPUT
////////////////////////////////////////////////////////

//------------------------------------------------------

class cEdInputBool : public iEdInput
{
public:
	cEdInputBool(iEdWindow* apWindow, 
					 const tWString& asLabel, 
					 iWidget* apParent=NULL);
	virtual ~cEdInputBool();

	void SetLabel(const tWString& asLabel);

	iWidget* GetInputWidget(int alX=0) { return mpChB; }

	void UpdateValue();
	void CopyValueToInput();

	void SetValue(bool abX, bool abGenerateCallback = true, bool abCopyToInput = true, bool abSkipChecks = false); 
	bool GetValue();

	void UpdateSize();

protected:
	cWidgetCheckBox* mpChB;
};

//------------------------------------------------------

////////////////////////////////////////////////////////
// TEXT INPUT
////////////////////////////////////////////////////////

//------------------------------------------------------

class cEdInputText :public iEdInputLabeled
{
public:
	cEdInputText(iEdWindow* apWindow, 
					 const tWString& asLabel, 
					 float afTextBoxWidth, 
					 int alAmount, 
					 bool abNumeric,
					 float afNumericAdd=0,
					 iWidget* apParent=NULL,
					 const tWStringList& alstLabels=tWStringList());
	virtual ~cEdInputText();

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

////////////////////////////////////////////////////////
// NUMBER INPUT
////////////////////////////////////////////////////////

//------------------------------------------------------

class cEdInputNumber : public cEdInputText
{
public:
	cEdInputNumber(iEdWindow* apWindow,
					   const tWString& asLabel,
					   float afTextBoxWidth,
					   iWidget* apParent=NULL,
					   float afNumericAdd=0) : cEdInputText(apWindow, asLabel, afTextBoxWidth, 1, true, afNumericAdd, apParent)
	{
	}
	
	void SetValue(float afX, bool abGenerateCallback = true, bool abCopyToInput = true, bool abSkipChecks = false); 
	float GetValue();

protected:
};

//------------------------------------------------------

////////////////////////////////////////////////////////
// VEC2 INPUT
////////////////////////////////////////////////////////

//------------------------------------------------------

class cEdInputVec2 : public cEdInputText
{
public:
	cEdInputVec2(iEdWindow* apWindow,
					 const tWString& asLabel,
					 float afTextBoxWidth,
					 const tWStringList& alstLabels=tWStringList(),
					 iWidget* apParent=NULL,
					 float afNumericAdd=0);

	void SetValue(const cVector2f& avX, bool abGenerateCallback = true, bool abCopyToInput = true, bool abSkipChecks = false); 
	cVector2f GetValue();
};

//------------------------------------------------------

////////////////////////////////////////////////////////
// VEC3 INPUT
////////////////////////////////////////////////////////

//------------------------------------------------------

class cEdInputVec3 : public cEdInputText
{
public:
	cEdInputVec3(iEdWindow* apWindow,
					 const tWString& asLabel,
					 float afTextBoxWidth,
					 const tWStringList& alstLabels=tWStringList(),
					 iWidget* apParent=NULL,
					 float afNumericAdd=0);

	void SetValue(const cVector3f& avX, bool abGenerateCallback = true, bool abCopyToInput = true, bool abSkipChecks = false); 
	cVector3f GetValue();
};

//------------------------------------------------------

////////////////////////////////////////////////////////
// FILE INPUT BASE
////////////////////////////////////////////////////////

//------------------------------------------------------

class cEdInputFile : public cEdInputText
{
public:
	cEdInputFile(iEdWindow* apWindow,
					const tWString& asLabel,
					float afTextBoxWidth,
					const tWString& asCat,
					iWidget* apParent=NULL);
	virtual ~cEdInputFile();

	void UpdateLayout();

	void SetShowPath(bool abX);
	bool GetShowPath() { return mbShowPath; }

	void SetValue(const tWString& asX, bool abGenerateCallback=true, bool abCopyToInput=true, bool abSkipChecks = false, bool abCheckValidFile=true); 

	const tWString& GetFullPath() { return msFullPath; }

	void SetInitialPath(const tWString&);

	void AddFileType(const tWString&);
protected:
	virtual bool CheckFileIsValid(const tWString&);
	void CopyValueToInput();

	bool BrowseButton_OnPressed(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(BrowseButton_OnPressed);

	bool Browser_OnOkay(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(Browser_OnOkay);

	virtual void ShowBrowser();

	/////////////////////////////////
	// Data

	cWidgetButton* mpBBrowse;
	
	tWStringVec mvTempLoadedFiles;
	tWString msTempLoadedFile;
	tWString msInitialPath;

	tWString msCategory;
	tWStringList mlstFileTypes;

	tWString msFilename;
	tWString msFullPath;
	bool mbShowPath;
};

//------------------------------------------------------

class cEdInputTexture : public cEdInputFile
{
public:
	cEdInputTexture(iEdWindow* apWindow,
					const tWString& asLabel,
					float afTextBoxWidth,
					bool abMaterialBrowser, eEdTexture aType,
					iWidget* apParent=NULL);

protected:
	void UpdateLayout();

	bool CheckFileIsValid(const tWString&);
	void ShowBrowser();

	bool NewButton_OnPressed(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(NewButton_OnPressed);

	bool mbMaterialBrowser;
	eEdTexture mType;
	cWidgetButton* mpBNew;
};

class cEdInputModel : public cEdInputFile
{
public:
	cEdInputModel(iEdWindow*, const tWString&, bool, float, iWidget* apWidget=NULL);

protected:
	bool CheckFileIsValid(const tWString&);
};

class cEdInputEntity : public cEdInputFile
{
public:
	cEdInputEntity(iEdWindow*, const tWString&, float, iWidget* apWidget=NULL);

protected:
	bool CheckFileIsValid(const tWString&);
};

class cEdInputSound : public cEdInputFile
{
public:
	cEdInputSound(iEdWindow* apWindow, const tWString& asLabel, float afTextBoxWidth, iWidget* apWidget=NULL);

protected:
	bool CheckFileIsValid(const tWString&);
	void ShowBrowser();
};

class cEdInputParticleSystem : public cEdInputFile
{
public:
	cEdInputParticleSystem(iEdWindow* apWindow, const tWString& asLabel, float afTextBoxWidth, iWidget* apWidget=NULL);

protected:
	bool CheckFileIsValid(const tWString&);
	//void ShowBrowser();
};

//------------------------------------------------------

////////////////////////////////////////////////////////
// ENUM INPUT
////////////////////////////////////////////////////////

//------------------------------------------------------

class cEdInputEnum : public iEdInputLabeled
{
public:
	cEdInputEnum(iEdWindow* apWindow,
					const tWString& asLabel, 
					float afComboBoxWidth,
					iWidget* apParent=NULL);

	iWidget* GetInputWidget(int alIdx=0) { return mpCB; }

	int  AddValue(const tWString&, void* apData=NULL);
	void RemoveValue(int alIndex);
	void ClearValues();

	bool IsClearing() { return mbClearing; }

	int GetNumValues();
	void SetValue(int, bool abGenerateCallback=true, bool abCopyToInput=true, bool abSkipChecks = false); 
	void SetValue(const tWString&, bool abGenerateCallback=true, bool abCopyToInput=true, bool abSkipChecks = false); 
	int GetValue();
	
	const tWString& GetEnumValue(int alIndex);
	const tWString& GetSelectedEnumValue();
	tString GetSelectedEnumValue8Char();

	void* GetUserData(int alIndex);
	void* GetSelectedUserData();

	void SetMaxShownItems(int alNum);

	void UpdateValue();
	void CopyValueToInput();

	void UpdateLayout();
protected:

	cWidgetComboBox* mpCB;
	bool mbClearing;
};

//------------------------------------------------------

////////////////////////////////////////////////////////
// COLOR INPUT
////////////////////////////////////////////////////////

//------------------------------------------------------

class cEdInputColor : public iEdInputLabeled
{
public:
	cEdInputColor(iEdWindow* apWindow, 
					const tWString& asLabel, 
					iWidget* apParent=NULL);

	iWidget* GetInputWidget(int alIdx=0) { return mpFColor; }

	void SetValue(const cColor& aCol, bool abGenerateCallback=true, bool abCopyToInput=true, bool abSkipChecks = false); 
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

#endif // ED_INPUT_H
