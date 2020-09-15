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

#include "EditorInput.h"
#include "EditorWindow.h"


cVector2f iEditorInput::mvFontSize = cVector2f(12);

//--------------------------------------------------------

iEditorInput::iEditorInput(iEditorWindow* apWindow, 
						   const cVector3f& avPos, 
						   const tWString& asLabel,
						   const tString& asName, 
						   iWidget* apParent,
						   eEditorInputLayoutStyle aStyle)
{
	mpWindow = apWindow;
	mpSet = apWindow->GetSet();

	mLayoutStyle = aStyle;

	if(apParent==NULL)
		apParent = apWindow->GetBGFrame();
	
	mpHandle = mpSet->CreateWidgetDummy(avPos, apParent, asName);
	mpHandle->SetUserData(this);

	mfTabWidth = 0;

	mpUserData = NULL;
}

//--------------------------------------------------------

iEditorInput::~iEditorInput()
{
}

//--------------------------------------------------------

void iEditorInput::SetPosition(const cVector3f& avPos)
{
	mpHandle->SetPosition(avPos);
}

//--------------------------------------------------------

void iEditorInput::SetValue(const tWString& asX, bool abGenerateCallback, bool abCopyToInput)
{
	mpHandle->SetText(asX);

	if(abCopyToInput)
		CopyValueToInput();

	if(abGenerateCallback)
		mpHandle->ProcessMessage(eGuiMessage_TextBoxEnter, cGuiMessageData());
}

//--------------------------------------------------------

void iEditorInput::AddCallback(eEditorInputCallback aCallback, void* apCallbackObject, tGuiCallbackFunc apCallback)
{
	eGuiMessage msg;
	switch(aCallback)
	{
	case eEditorInputCallback_ValueEnter:
		msg = eGuiMessage_TextBoxEnter;
		break;
	case eEditorInputCallback_ValueChange:
		msg = eGuiMessage_TextChange;
		break;
	case eEditorInputCallback_Browse:
		msg = eGuiMessage_ButtonPressed;
		break;
	default:
		break;
	}

	mpHandle->AddCallback(msg, apCallbackObject, apCallback);
}

//--------------------------------------------------------

void iEditorInput::UpdateSize()
{
}

//--------------------------------------------------------

bool iEditorInput::ValueEnter(iWidget* apWidget, const cGuiMessageData& aData)
{
	UpdateValue();
	return mpHandle->ProcessMessage(eGuiMessage_TextBoxEnter, cGuiMessageData());
}
kGuiCallbackDeclaredFuncEnd(iEditorInput, ValueEnter);

//--------------------------------------------------------

iEditorInputLabeled::iEditorInputLabeled(iEditorWindow* apWindow,
										 const cVector3f& avPos,
										 const tWString& asLabel,
										 const tString& asName,
										 iWidget* apParent,
										 eEditorInputLayoutStyle aStyle) : iEditorInput(apWindow,avPos,asLabel,asName,apParent,aStyle)
{
	mpL = mpSet->CreateWidgetLabel(0,-1,asLabel, mpHandle);
	mpL->SetAutogenerateSize(true);
	mpL->SetDefaultFontSize(mvFontSize);
}

//--------------------------------------------------------

iEditorInputLabeled::~iEditorInputLabeled()
{
}

//--------------------------------------------------------

void iEditorInputLabeled::SetLabel(const tWString& asLabel)
{
	mpL->SetText(asLabel);
	UpdateLayout();
}

//--------------------------------------------------------

//--------------------------------------------------------
//--------------------------------------------------------

cEditorInputBool::cEditorInputBool(iEditorWindow* apWindow,
								   const cVector3f& avPos,
								   const tWString& asLabel,
								   const tString& asName,
								   iWidget* apParent) : iEditorInput(apWindow,avPos, asLabel, asName, apParent)
{
	mpChB = mpSet->CreateWidgetCheckBox(0,0, _W(""), mpHandle, asName);

	mpChB->AddCallback(eGuiMessage_CheckChange, (iEditorInput*)this, kGuiCallback(ValueEnter));
	mpChB->SetDefaultFontSize(mvFontSize);
	mpChB->SetText(asLabel);
}

//--------------------------------------------------------

cEditorInputBool::~cEditorInputBool()
{
}

//--------------------------------------------------------

void cEditorInputBool::SetLabel(const tWString& asLabel)
{
	mpChB->SetText(asLabel);
	UpdateLayout();
}

//--------------------------------------------------------

void cEditorInputBool::UpdateValue()
{
	SetValue(mpChB->IsChecked(),false,false);
}

//--------------------------------------------------------

void cEditorInputBool::CopyValueToInput()
{
	bool bChecked = GetValue();
	mpChB->SetChecked(bChecked,false);
}

//--------------------------------------------------------

void cEditorInputBool::SetValue(bool abX, bool abGenerateCallback, bool abCopyToInput)
{
	tWString sValue = abX?_W("true"):_W("false");
	iEditorInput::SetValue(sValue, abGenerateCallback, abCopyToInput);
}

//--------------------------------------------------------

bool cEditorInputBool::GetValue()
{
	bool bChecked = cString::ToBool(cString::To8Char(iEditorInput::GetValue()).c_str(),0);
	return bChecked;
}

//--------------------------------------------------------
//--------------------------------------------------------

cEditorInputText::cEditorInputText(iEditorWindow* apWindow,
								   const cVector3f& avPos, 
								   const tWString& asLabel,
								   const tString& asName,
								   float afTextBoxWidth,
								   int alAmount, 
								   bool abNumeric,
                                   float afNumericAdd,
								   iWidget* apParent,
								   const tWStringList& alstInputLabels) : iEditorInputLabeled(apWindow, avPos, asLabel, asName, apParent)
{
	tWStringList::const_iterator itLabels = alstInputLabels.begin();
	for(int i=0;i<alAmount;++i,++itLabels)
	{
		// Setup Label
		tWString sLabel = (itLabels==alstInputLabels.end())?_W(""):*itLabels;
		cWidgetLabel* pL = mpSet->CreateWidgetLabel(0,0,sLabel, mpHandle);
		mvL.push_back(pL);
		pL->SetSize(cVector2f(pL->GetDefaultFontType()->GetLength(mvFontSize, sLabel.c_str()), mvFontSize.y));
		pL->SetDefaultFontSize(mvFontSize);

		// Setup TextBox control
		cWidgetTextBox* pTB = mpSet->CreateWidgetTextBox(0,
														 cVector2f(afTextBoxWidth,25),
														 _W(""), 
														 mpHandle, 
														 abNumeric?eWidgetTextBoxInputType_Numeric:eWidgetTextBoxInputType_Normal, 
														 afNumericAdd,
														 afNumericAdd!=0);

		mvTB.push_back(pTB);
		pTB->SetDefaultFontSize(mvFontSize);
		pTB->AddCallback(eGuiMessage_TextBoxEnter, (iEditorInput*)this, kGuiCallback(TBIdCheckerCallback));
		pTB->AddCallback(eGuiMessage_TextBoxEnter, (iEditorInput*)this, kGuiCallback(ValueEnter));
		mpSet->AddToTabOrder(pTB);
	}
	mlCallbackTBId = -1;
	/////////////////////////////
	// Default decimals to 3
	mlDecimals = -1;
	SetDecimals(3);
}

//--------------------------------------------------------

cEditorInputText::~cEditorInputText()
{
}

//--------------------------------------------------------

void cEditorInputText::SetLowerBound(bool abX, float afX)
{
	for(int i=0;i<(int)mvTB.size();++i)
		mvTB[i]->SetLowerBound(abX, afX);
}

void cEditorInputText::SetUpperBound(bool abX, float afX)
{
	for(int i=0;i<(int)mvTB.size();++i)
		mvTB[i]->SetUpperBound(abX, afX);
}

//--------------------------------------------------------

void cEditorInputText::SetDecimals(int alX)
{
	mlDecimals = alX;
	for(int i=0;i<(int)mvTB.size();++i)
		mvTB[i]->SetDecimals(alX);
}

//--------------------------------------------------------

void cEditorInputText::SetCanEdit(bool abX)
{
	for(int i=0;i<(int)mvTB.size();++i)
		mvTB[i]->SetCanEdit(abX);
}

//--------------------------------------------------------

void cEditorInputText::UpdateValue()
{
	tWString sValue = mvTB[0]->GetText();
	for(int i=1;i<(int)mvTB.size();++i)
	{
		sValue += _W(" ") + mvTB[i]->GetText();
	}
	SetValue(sValue,false,false);
}

//--------------------------------------------------------

void cEditorInputText::CopyValueToInput()
{
	if(mvTB.size()==1)
		mvTB[0]->SetText(GetValue());
	else
	{
		tWStringVec vStrings;
		tWString vSeparator = _W(" ");
		vStrings = cString::GetStringVecW(GetValue(), vStrings, &vSeparator);

		for(int i=0;i<(int)vStrings.size() && i<(int)mvTB.size();++i)
		{
			mvTB[i]->SetText(vStrings[i]);
		}
	}
}

//--------------------------------------------------------

void cEditorInputText::UpdateLayout()
{
	cVector3f vPos = 0;
	cVector2f vLabelSize = mpL->GetSize();
	cVector2f vInputSize = mvTB[0]->GetSize();
	if(mLayoutStyle==eEditorInputLayoutStyle_RowLabelOnLeft ||
		mLayoutStyle==eEditorInputLayoutStyle_ColumnLabelOnLeft)
	{
		mpL->SetPosition(cVector3f(0,(vInputSize.y-vLabelSize.y)*0.5f,0));
        vPos.x += vLabelSize.x + 5;
		if(mfTabWidth>0)
		{
			for(int i=0;;++i)
			{
				float fTab = i*mfTabWidth;
				if(vPos.x<fTab)
				{
					vPos.x = fTab;
					break;
				}
			}
		}
	}
	else
	{
		mpL->SetPosition(0);
		vPos.y += vLabelSize.y;
	}
	
	float fMaxLabelLength = 0;
	for(int i=0;i<(int)mvL.size();++i)
		if(fMaxLabelLength<mvL[i]->GetSize().x)
			fMaxLabelLength = mvL[i]->GetSize().x;

	for(int i=0;i<(int)mvL.size();++i)
	{
		cVector3f vLabelPos = vPos + cVector3f(0,(vInputSize.y-vLabelSize.y)*0.5f,0);
		cVector3f vInputPos;

        iWidget* pL = mvL[i];
		iWidget* pTB = mvTB[i];

		switch(mLayoutStyle)
		{
		case eEditorInputLayoutStyle_RowLabelOnLeft:
		case eEditorInputLayoutStyle_RowLabelOnTop:
			vInputPos = vPos + cVector3f(pL->GetSize().x,0,0);
			
			vPos = vInputPos + cVector3f(pTB->GetSize().x,0,0);
			break;
		case eEditorInputLayoutStyle_ColumnLabelOnLeft:
		case eEditorInputLayoutStyle_ColumnLabelOnTop:
			vInputPos = vPos + cVector3f(fMaxLabelLength,0,0);
			
			vPos.y = vInputPos.y + pTB->GetSize().y;
			break;
		}

		pL->SetPosition(vLabelPos);
		pTB->SetPosition(vInputPos);
	}

	UpdateSize();
}

//--------------------------------------------------------

bool cEditorInputText::TBIdCheckerCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	for(int i=0;i<(int)mvTB.size();++i)
	{
		if(apWidget==mvTB[i])
		{
			mlCallbackTBId = i;
			break;
		}
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorInputText, TBIdCheckerCallback);

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

cEditorInputFile::cEditorInputFile(iEditorWindow* apWindow,
								   const cVector3f& avPos, 
								   const tWString& asLabel, 
								   const tString& asName, 
								   float afTextBoxWidth,
								   iWidget* apParent) : cEditorInputText(apWindow,
																			   avPos,
																			   asLabel, 
																			   asName, 
																			   afTextBoxWidth,
																			   1, false, 0,
																			   apParent)
{
	iWidget* pTB = mvTB.back();
	const cVector3f& vPos = pTB->GetLocalPosition();
	const cVector2f& vSize = pTB->GetSize();
	mpBBrowse = mpSet->CreateWidgetButton(0, cVector2f(20), _W("..."), mpHandle, false, asName+"Browse");

	mpBNew = mpSet->CreateWidgetButton(0, cVector2f(20), _W("+"), mpHandle, false, asName+"New");

	mpBBrowse->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(BrowseButton_OnPressed));
	mpBNew->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(NewButton_OnPressed));

	mbShowPath = false;

	SetBrowserType(eEditorResourceType_Texture);
	mlBrowserSubtype = eEditorTextureResourceType_LastEnum;

	SetInitialPath(apWindow->GetEditor()->GetWorkingDir());
}

//--------------------------------------------------------

cEditorInputFile::~cEditorInputFile()
{
}

//--------------------------------------------------------

void cEditorInputFile::SetBrowserType(eEditorResourceType aType)
{
	mBrowserType = aType;
	bool bIsMaterial = mBrowserType==eEditorResourceType_Material;

	mpBNew->SetEnabled(bIsMaterial);
	mpBNew->SetVisible(bIsMaterial);
}

//--------------------------------------------------------

void cEditorInputFile::UpdateLayout()
{
	cEditorInputText::UpdateLayout();

	iWidget* pTB = mvTB.back();
	const cVector3f& vPos = pTB->GetLocalPosition();
	const cVector2f& vSize = pTB->GetSize();
	mpBBrowse->SetPosition(vPos+cVector3f(vSize.x+5,0,0));
	mpBNew->SetPosition(mpBBrowse->GetLocalPosition()+cVector3f(mpBBrowse->GetSize().x+5,0,0));

	UpdateSize();
}

//--------------------------------------------------------

void cEditorInputFile::SetShowPath(bool abX)
{
	if(mbShowPath==abX)
		return;

	mbShowPath = abX;
	
	CopyValueToInput();
}

//--------------------------------------------------------

void cEditorInputFile::SetValue(const tWString& asX, bool abGenerateCallback, bool abCopyToInput)
{
	msFilename = asX;
	msFullPath = mpWindow->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(cString::To8Char(asX));
	iEditorInput::SetValue(asX, abGenerateCallback, abCopyToInput);
}

void cEditorInputFile::SetInitialPath(const tWString& asPath)
{
	//Log("Setting EditorInputFile Initial path: %ls\n", asPath.c_str());
	msInitialPath = cString::GetFilePathW(asPath);
	//Log("\tPath string is: %ls, checking if exists...", msInitialPath.c_str());
	if(cPlatform::FolderExists(msInitialPath)==false)
	{
		msInitialPath = cString::GetFilePathW(mpWindow->GetEditor()->GetWorkingDir());
	//	Log("Does not exist, setting %ls instead\n", msInitialPath.c_str());
	}
	//else
	//	Log("Yup\n");
}

void cEditorInputFile::AddCustomFileType(const tWString& asType)
{
	 mlstCustomFileTypes.push_back(_W("*.") + asType);
}

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

void cEditorInputFile::CopyValueToInput()
{
	tWString sText;
	if(mbShowPath)
		sText = msFilename;
	else
		sText = cString::GetFileNameW(msFilename);

	mvTB[0]->SetText(sText);
}

//--------------------------------------------------------

bool cEditorInputFile::BrowseButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	tWString sCatName;
	tWStringList lstCatString;
	iEditorBase* pEditor = mpWindow->GetEditor();

	mvTempLoadedFiles.clear();
	if(msFilename==_W(""))
		msTempLoadedFile=msInitialPath;
	else
	{
		msTempLoadedFile = mpWindow->GetEditor()->GetPathRelToWD(msFilename);
		if(cPlatform::FileExists(msTempLoadedFile)==false && cPlatform::FolderExists(msTempLoadedFile)==false)
		{
			if (cPlatform::FileExists(msFilename) || cPlatform::FolderExists(msFilename))
			{
				msTempLoadedFile = msFilename;
			}
			else
			{
				//Log("TempLoadedFile does not exist:%ls\n", msTempLoadedFile.c_str());
				msTempLoadedFile = msInitialPath;
			}
		}
	}

	//Log("Browser initial path: %ls\n", msTempLoadedFile.c_str());

	if(mlstCustomFileTypes.empty()==false)
	{
		pEditor->ShowLoadFilePicker(mvTempLoadedFiles, cString::GetFilePathW(msTempLoadedFile), this, kGuiCallback(Browser_OnOkay), _W("Custom"), mlstCustomFileTypes);
	}
	else
	{
		//////////////////////////////////////////
		// TODO: Texture browser please
		switch(mBrowserType)
		{
		case eEditorResourceType_Material:
			lstCatString.push_back(_W("mat"));
			pEditor->ShowTextureBrowser((eEditorTextureResourceType)mlBrowserSubtype, _W(""), msTempLoadedFile, this, kGuiCallback(Browser_OnOkay), lstCatString);
			break;
		case eEditorResourceType_Texture:
		{
			lstCatString.push_back(_W("jpg"));
			lstCatString.push_back(_W("bmp"));
			lstCatString.push_back(_W("png"));
			lstCatString.push_back(_W("gif"));
			lstCatString.push_back(_W("dds"));
			lstCatString.push_back(_W("tga"));
			pEditor->ShowTextureBrowser((eEditorTextureResourceType)mlBrowserSubtype, _W(""), msTempLoadedFile, this, kGuiCallback(Browser_OnOkay), lstCatString);
			break;
		}
		case eEditorResourceType_ParticleSystem:
			sCatName = _W("Particle Systems");
			lstCatString.push_back(_W("*.ps"));
			pEditor->ShowLoadFilePicker(mvTempLoadedFiles, cString::GetFilePathW(msTempLoadedFile), this, kGuiCallback(Browser_OnOkay), sCatName, lstCatString);
			break;
		case eEditorResourceType_Model:
			sCatName = _W("Models");
			lstCatString.push_back(_W("*.dae"));
			lstCatString.push_back(_W("*.fbx"));
			pEditor->ShowLoadFilePicker(mvTempLoadedFiles, cString::GetFilePathW(msTempLoadedFile), this, kGuiCallback(Browser_OnOkay), sCatName, lstCatString);
			break;
		case eEditorResourceType_ModelAnim:
			sCatName = _W("Animations");
			lstCatString.push_back(_W("*.dae_anim"));
			lstCatString.push_back(_W("*.fbx"));
			pEditor->ShowLoadFilePicker(mvTempLoadedFiles, cString::GetFilePathW(msTempLoadedFile), this, kGuiCallback(Browser_OnOkay), sCatName, lstCatString);
			break;
		case eEditorResourceType_Sound:
			pEditor->ShowSoundBrowser(cString::GetFilePathW(msTempLoadedFile), msTempLoadedFile, this, kGuiCallback(Browser_OnOkay));
			break;
		case eEditorResourceType_EntityFile:
			sCatName = _W("Entities");
			lstCatString.push_back(_W("*.ent"));
			pEditor->ShowLoadFilePicker(mvTempLoadedFiles, cString::GetFilePathW(msTempLoadedFile), this, kGuiCallback(Browser_OnOkay), sCatName, lstCatString);
			break;
		}
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorInputFile, BrowseButton_OnPressed);

//--------------------------------------------------------

bool cEditorInputFile::Browser_OnOkay(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		tWString sFile;
		if(mvTempLoadedFiles.empty()==false)
			sFile = mvTempLoadedFiles.front();
		else
			sFile = msTempLoadedFile;
		
		msFilename = sFile;
		msFullPath = sFile;

		SetValue(sFile, true, true);
		return true;
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorInputFile, Browser_OnOkay);

//--------------------------------------------------------

bool cEditorInputFile::NewButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpWindow->GetEditor()->ShowMaterialEditor(this);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorInputFile, NewButton_OnPressed);

//--------------------------------------------------------
//--------------------------------------------------------

//--------------------------------------------------------
//--------------------------------------------------------

void cEditorInputNumber::SetValue(float afX, bool abGenerateCallback, bool abCopyToInput)
{
	tWString sValue = cString::ToStringW(afX,mlDecimals,true);
	iEditorInput::SetValue(sValue, abGenerateCallback, abCopyToInput);
}

//--------------------------------------------------------

float cEditorInputNumber::GetValue()
{
	return mvTB[0]->GetNumericValue();
}

//--------------------------------------------------------
//--------------------------------------------------------

void cEditorInputVec2::SetValue(const cVector2f& avX, bool abGenerateCallback, bool abCopyToInput)
{
	tWString sValue;

	for(int i=0;i<4;++i)
	{
		sValue += cString::ToStringW(avX.v[i],mlDecimals,true) + _W(" ");
	}
	iEditorInput::SetValue(sValue, abGenerateCallback, abCopyToInput);
}

//--------------------------------------------------------

cVector2f cEditorInputVec2::GetValue()
{
	cVector2f vTemp;
	for(int i=0;i<2;++i)
	{
        vTemp.v[i] = mvTB[i]->GetNumericValue();
	}

	return vTemp;
}

//--------------------------------------------------------
//--------------------------------------------------------

void cEditorInputVec3::SetValue(const cVector3f& avX, bool abGenerateCallback , bool abCopyToInput)
{
	tWString sValue;

	for(int i=0;i<3;++i)
	{
		sValue += cString::ToStringW(avX.v[i],mlDecimals,true) + _W(" ");
	}
	iEditorInput::SetValue(sValue, abGenerateCallback, abCopyToInput);
}

//--------------------------------------------------------

cVector3f cEditorInputVec3::GetValue()
{
	cVector3f vTemp;
	for(int i=0;i<3;++i)
	{
        vTemp.v[i] = mvTB[i]->GetNumericValue();
	}

	return vTemp;
}

//--------------------------------------------------------
//--------------------------------------------------------
/*
void cEditorInputColor::SetValue(const cColor& aX, bool abGenerateCallback, bool abCopyToInput)
{
	tWString sValue;

	for(int i=0;i<4;++i)
	{
		sValue += cString::ToStringW(aX.v[i],-1,true) + _W(" ");
	}
	iEditorInput::SetValue(sValue, abGenerateCallback, abCopyToInput);
}

//--------------------------------------------------------

cColor cEditorInputColor::GetValue()
{
	cColor value;
	for(int i=0;i<4;++i)
	{
        value.v[i] = mvTB[i]->GetNumericValue();
	}
    
	return value;
}

//--------------------------------------------------------
//--------------------------------------------------------
*/
//--------------------------------------------------------

cEditorInputEnum::cEditorInputEnum(iEditorWindow* apWindow,
								   const cVector3f& avPos,
								   const tWString& asLabel,
								   const tString& asName,
								   float afComboBoxWidth,
								   iWidget* apParent) : iEditorInputLabeled(apWindow, avPos, asLabel, asName, apParent)
{
	mpCB = mpSet->CreateWidgetComboBox(0, cVector2f(afComboBoxWidth,25), _W(""), mpHandle);
	mpCB->AddCallback(eGuiMessage_SelectionChange, (iEditorInput*)this, kGuiCallback(ValueEnter));
	mpCB->SetDefaultFontSize(mvFontSize);
	mpCB->SetClipActive(false);
}

//--------------------------------------------------------

void cEditorInputEnum::AddValue(const tWString& asX)
{
	mpCB->AddItem(asX);
}

//--------------------------------------------------------

void cEditorInputEnum::RemoveValue(int alIndex)
{
	mpCB->RemoveItem(alIndex);
}

//--------------------------------------------------------

void cEditorInputEnum::ClearValues()
{
	mpCB->ClearItems();
	UpdateValue();
}

//--------------------------------------------------------

void cEditorInputEnum::SetMaxShownItems(int alNum)
{
	mpCB->SetMaxShownItems(alNum);
}

//--------------------------------------------------------
//--------------------------------------------------------

//--------------------------------------------------------

void cEditorInputEnum::UpdateValue()
{
	SetValue(mpCB->GetSelectedItem(),false,false);
}
//--------------------------------------------------------

void cEditorInputEnum::CopyValueToInput()
{
	tWString sValue = cString::ToLowerCaseW(iEditorInput::GetValue());
	int lIndex=-1;
	for(int i=0;i<mpCB->GetItemNum();++i)
	{
		if(sValue==cString::ToLowerCaseW(mpCB->GetItemText(i)))
		{
			lIndex=i;
			break;
		}
	}
	
	mpCB->SetSelectedItem(lIndex,false,false);
}

//--------------------------------------------------------

void cEditorInputEnum::UpdateLayout()
{
	cVector3f vPos = 0;
	cVector2f vLabelSize = mpL->GetSize();
	cVector2f vInputSize = mpCB->GetSize();

	if(mLayoutStyle==eEditorInputLayoutStyle_RowLabelOnLeft ||
		mLayoutStyle==eEditorInputLayoutStyle_ColumnLabelOnLeft)
	{
		mpL->SetPosition(cVector3f(0,(vInputSize.y-vLabelSize.y)*0.5f,0));
        vPos.x += vLabelSize.x + 5;
		if(mfTabWidth>0)
		{
			for(int i=0;;++i)
			{
				float fTab = i*mfTabWidth;
				if(vPos.x<fTab)
				{
					vPos.x = fTab;
					break;
				}
			}
		}
	}
	else
	{
		mpL->SetPosition(0);
		vPos.y += vLabelSize.y;
	}
	
	mpCB->SetPosition(vPos);

	UpdateSize();
}

//--------------------------------------------------------

int cEditorInputEnum::GetNumValues()
{
	return mpCB->GetItemNum();
}

//--------------------------------------------------------

void cEditorInputEnum::SetValue(int alX, bool abGenerateCallback, bool abCopyToInput)
{
	tWString sValue = mpCB->GetItemText(alX);
	iEditorInput::SetValue(sValue, abGenerateCallback, abCopyToInput);
}

//--------------------------------------------------------

int cEditorInputEnum::GetValue()
{
	return mpCB->GetSelectedItem();
}

//--------------------------------------------------------

const tWString& cEditorInputEnum::GetEnumValue(int alIndex)
{
	return mpCB->GetItemText(alIndex);
}

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

cEditorInputColorFrame::cEditorInputColorFrame(iEditorWindow* apWindow, 
											   const cVector3f& avPos, 
											   const tWString& asLabel, 
											   const tString& asName, 
											   iWidget* apParent) : iEditorInputLabeled(apWindow, avPos, asLabel, asName, apParent, eEditorInputLayoutStyle_RowLabelOnLeft)
{
	mCol = cColor(1,1);

	mpFColor = mpSet->CreateWidgetFrame(0, cVector2f(15), true, mpHandle);
	mpFColor->AddCallback(eGuiMessage_MouseUp, this, kGuiCallback(Frame_OnClick));
	mpFColor->SetDrawBackground(true);
	mpFColor->SetBackgroundZ(0);
	mpFColor->SetBackGroundColor(mCol);
}

//--------------------------------------------------------

void cEditorInputColorFrame::UpdateLayout()
{
	cVector2f vSize = mpL->GetSize();
	vSize.x += 10;
	if(mfTabWidth>0)
	{
		for(int i=0;;++i)
		{
			float fTab = i*mfTabWidth;
			if(vSize.x<fTab)
			{
				vSize.x = fTab;
				break;
			}
		}
	}
	mpFColor->SetPosition(cVector3f(vSize.x,0,0.1f));

	UpdateSize();
}

//--------------------------------------------------------

bool cEditorInputColorFrame::Frame_OnClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	if((aData.mlVal&eGuiMouseButton_Left)==0)
		return false;

	mpSet->CreatePopUpColorPicker(&mCol,apWidget->GetGlobalPosition()+cVector3f(-50,0,20),this,kGuiCallback(ColorPicker_OnOK));
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorInputColorFrame, Frame_OnClick);

//--------------------------------------------------------

bool cEditorInputColorFrame::ColorPicker_OnOK(iWidget* apWidget, const cGuiMessageData& aData)
{
	SetValue(mCol, true, true);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorInputColorFrame, ColorPicker_OnOK);
//--------------------------------------------------------

void cEditorInputColorFrame::UpdateValue()
{
	SetValue(mCol);
}

//--------------------------------------------------------

void cEditorInputColorFrame::CopyValueToInput()
{
	const tWString& sValue = iEditorInput::GetValue();
	mCol = cString::ToColor(cString::To8Char(sValue).c_str(), cColor(1));
	mpFColor->SetBackGroundColor(cColor(mCol.r, mCol.g, mCol.b, 1));
}
//--------------------------------------------------------

const cColor& cEditorInputColorFrame::GetValue()
{
	return mCol;
}

//--------------------------------------------------------

void cEditorInputColorFrame::SetValue(const cColor& aCol, bool abGenerateCallback, bool abCopyToInput)
{
	tWString sValue = cString::ToStringW(aCol.v[0],-1,true);
	for(int i=1;i<4;++i)
	{
		sValue += _W(" ") + cString::ToStringW(aCol.v[i],-1,true);
	}
	iEditorInput::SetValue(sValue, abGenerateCallback, abCopyToInput);
}

//--------------------------------------------------------
