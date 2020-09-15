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

#include "../Common/EdInput.h"
#include "../Common/EdWindowHandler.h"

#include "../Common/Editor.h"

#include "../Common/EdResource.h"

#include "../Common/EdPopUpTextureBrowser.h"
#include "../Common/EdPopUpSoundBrowser.h"
#include "../Common/EdWindowMaterialEditor.h"


//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// ED INPUT BASE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cGuiSet* iEdInput::mpSet = NULL;
cVector2f iEdInput::mvFontSize = cVector2f(12);

//------------------------------------------------------------------------

iEdInput::iEdInput(iEdWindow* apWindow, 
				   iWidget* apParent,
				   eEdInpStyle aStyle)
{
	mpWindow = apWindow;
	if(mpSet==NULL) mpSet = mpWindow->GetGuiSet();

	mStyle = aStyle;

	if(apParent==NULL)
		apParent = apWindow->GetBG();
	
	mpHandle = mpSet->CreateWidgetDummy(0, apParent);
	mpHandle->SetUserData(this);

	mfTabWidth = 0;

	mpUserData = NULL;
}

//------------------------------------------------------------------------

iEdInput::~iEdInput()
{
	mpSet->DestroyWidget(mpHandle, true);
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// ED INPUT BASE - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void iEdInput::SetPosition(const cVector3f& avPos)
{
	mpHandle->SetPosition(avPos);
}

//--------------------------------------------------------

void iEdInput::SetValue(const tWString& asX, bool abGenerateCallback, bool abCopyToInput, bool abSkipChecks)
{
	mpHandle->SetText(asX);

	if(abCopyToInput)
		CopyValueToInput();

	if(abGenerateCallback)
		mpHandle->ProcessMessage(eGuiMessage_TextBoxEnter, cGuiMessageData(), abSkipChecks, abSkipChecks);
}

//--------------------------------------------------------

void iEdInput::AddCallback(eEdInputCallback aCallback, void* apCallbackObject, tGuiCallbackFunc apCallback)
{
	eGuiMessage msg;
	switch(aCallback)
	{
	case eEdInputCallback_ValueEnter:
		msg = eGuiMessage_TextBoxEnter;
		break;
	case eEdInputCallback_ValueChange:
		msg = eGuiMessage_TextChange;
		break;
	case eEdInputCallback_Browse:
		msg = eGuiMessage_ButtonPressed;
		break;
	}

	mpHandle->AddCallback(msg, apCallbackObject, apCallback);
}

//--------------------------------------------------------

void iEdInput::UpdateSize()
{
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// ED INPUT BASE - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

bool iEdInput::ValueEnter(iWidget* apWidget, const cGuiMessageData& aData)
{
	UpdateValue();
	return mpHandle->ProcessMessage(eGuiMessage_TextBoxEnter, cGuiMessageData());
}
kGuiCallbackDeclaredFuncEnd(iEdInput, ValueEnter);

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// ED LABELED INPUT BASE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

iEdInputLabeled::iEdInputLabeled(iEdWindow* apWindow, const tWString& asLabel,
								 iWidget* apParent, eEdInpStyle aStyle) : iEdInput(apWindow, apParent, aStyle)
{
	mpL = mpSet->CreateWidgetLabel(0, -1, asLabel, mpHandle);
	mpL->SetAutogenerateSize(true);
	mpL->SetDefaultFontSize(mvFontSize);
}

//--------------------------------------------------------

iEdInputLabeled::~iEdInputLabeled()
{
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// ED LABELED INPUT BASE - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void iEdInputLabeled::SetLabel(const tWString& asLabel)
{
	mpL->SetText(asLabel);
	UpdateLayout();
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT BOOL - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputBool::cEdInputBool(iEdWindow* apWindow, 
						   const tWString& asLabel,
						   iWidget* apParent) : iEdInput(apWindow, apParent)
{
	mpChB = mpSet->CreateWidgetCheckBox(0,0, asLabel, mpHandle);

	mpChB->AddCallback(eGuiMessage_CheckChange, static_cast<iEdInput*>(this), kGuiCallback(ValueEnter));
	mpChB->SetDefaultFontSize(mvFontSize);
}

//------------------------------------------------------------------------

cEdInputBool::~cEdInputBool()
{
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT BOOL - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEdInputBool::SetLabel(const tWString& asLabel)
{
	mpChB->SetText(asLabel);
	UpdateLayout();
}

//------------------------------------------------------------------------

void cEdInputBool::UpdateValue()
{
	SetValue(mpChB->IsChecked(),false,false);
}

//------------------------------------------------------------------------

void cEdInputBool::CopyValueToInput()
{
	bool bChecked = GetValue();
	mpChB->SetChecked(bChecked,false);
}

//------------------------------------------------------------------------

void cEdInputBool::SetValue(bool abX, bool abGenerateCallback, bool abCopyToInput, bool abSkipChecks)
{
	tWString sValue = abX?_W("true"):_W("false");
	iEdInput::SetValue(sValue, abGenerateCallback, abCopyToInput, abSkipChecks);
}

//------------------------------------------------------------------------

bool cEdInputBool::GetValue()
{
	return cString::ToBool(cString::To8Char(iEdInput::GetValue()).c_str(), false);
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT TEXT - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputText::cEdInputText(iEdWindow* apWindow, const tWString& asLabel, 
						   float afTextBoxWidth,int alTBAmount, bool abNumeric,float afNumericAdd,
						   iWidget* apParent, const tWStringList& alstInputLabels) : iEdInputLabeled(apWindow, asLabel, apParent)
{
	iFontData* pFont = mpHandle->GetDefaultFontType();
	tWStringList::const_iterator itLabels = alstInputLabels.begin();

	for(int i=0; i<alTBAmount; ++i,++itLabels)
	{
		///////////////////////////////////
		// Setup Label
		tWString sLabel;
		if(itLabels!=alstInputLabels.end())
			sLabel = *itLabels;

		cWidgetLabel* pL = mpSet->CreateWidgetLabel(0,0,sLabel, mpHandle);
		mvL.push_back(pL);

		pL->SetDefaultFontSize(mvFontSize);

		cVector2f vLabelSize;
		vLabelSize.x = pFont->GetLength(mvFontSize, sLabel.c_str());
		vLabelSize.y = mvFontSize.y;

		pL->SetSize(vLabelSize);

		cVector2f vTextBoxSize = cVector2f(afTextBoxWidth, 25);
		eWidgetTextBoxInputType tbType = abNumeric? eWidgetTextBoxInputType_Numeric : eWidgetTextBoxInputType_Normal;

		// Setup TextBox control
		cWidgetTextBox* pTB = mpSet->CreateWidgetTextBox(0,
														 vTextBoxSize,
														 _W(""), 
														 mpHandle, 
														 tbType, 
														 afNumericAdd,
														 afNumericAdd!=0);

		mvTB.push_back(pTB);

		pTB->SetDefaultFontSize(mvFontSize);
		pTB->AddCallback(eGuiMessage_TextBoxEnter, static_cast<iEdInput*>(this), kGuiCallback(TBIdCheckerCallback));
		pTB->AddCallback(eGuiMessage_TextBoxEnter, static_cast<iEdInput*>(this), kGuiCallback(ValueEnter));
		mpSet->AddToTabOrder(pTB);
	}
	mlCallbackTBId = -1;
	/////////////////////////////
	// Default decimals to 3
	mlDecimals = 0;
	SetDecimals(3);
}

//--------------------------------------------------------

cEdInputText::~cEdInputText()
{
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT TEXT - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEdInputText::SetLowerBound(bool abX, float afX)
{
	for(int i=0;i<(int)mvTB.size();++i)
		mvTB[i]->SetLowerBound(abX, afX);
}

void cEdInputText::SetUpperBound(bool abX, float afX)
{
	for(int i=0;i<(int)mvTB.size();++i)
		mvTB[i]->SetUpperBound(abX, afX);
}

//--------------------------------------------------------

void cEdInputText::SetDecimals(int alX)
{
	if(mlDecimals==alX) return;

	mlDecimals = alX;
	for(size_t i=0; i<mvTB.size(); ++i)
		mvTB[i]->SetDecimals(alX);
}

//--------------------------------------------------------

void cEdInputText::SetCanEdit(bool abX)
{
	for(size_t i=0; i<mvTB.size(); ++i)
		mvTB[i]->SetCanEdit(abX);
}

//--------------------------------------------------------

void cEdInputText::UpdateValue()
{
	tWString sValue = mvTB[0]->GetText();
	for(size_t i=1; i<mvTB.size(); ++i)
	{
		sValue += _W(" ") + mvTB[i]->GetText();
	}
	SetValue(sValue,false,false);
}

//--------------------------------------------------------

void cEdInputText::CopyValueToInput()
{
	if(mvTB.size()==1)
	{
		mvTB[0]->SetText(GetValue());
	}
	else
	{
		tWStringVec vStrings;
		tWString vSeparator = _W(" ");
		vStrings = cString::GetStringVecW(GetValue(), vStrings, &vSeparator);

		for(size_t i=0; i<vStrings.size() && i<mvTB.size(); ++i)
		{
			mvTB[i]->SetText(vStrings[i]);
		}
	}
}

//--------------------------------------------------------

void cEdInputText::UpdateLayout()
{
	cVector3f vPos = 0;
	cVector2f vLabelSize = mpL->GetSize();
	cVector2f vInputSize = mvTB[0]->GetSize();
	if(mStyle==eEdInpStyle_RowLabelOnLeft ||
		mStyle==eEdInpStyle_ColumnLabelOnLeft)
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
	for(size_t i=0; i<mvL.size(); ++i)
		if(fMaxLabelLength<mvL[i]->GetSize().x)
			fMaxLabelLength = mvL[i]->GetSize().x;

	for(size_t i=0; i<mvL.size(); ++i)
	{
		cVector3f vLabelPos = vPos + cVector3f(0,(vInputSize.y-vLabelSize.y)*0.5f,0);
		cVector3f vInputPos;

        iWidget* pL = mvL[i];
		iWidget* pTB = mvTB[i];

		switch(mStyle)
		{
		case eEdInpStyle_RowLabelOnLeft:
		case eEdInpStyle_RowLabelOnTop:
			vInputPos = vPos + cVector3f(pL->GetSize().x,0,0);
			
			vPos = vInputPos + cVector3f(pTB->GetSize().x,0,0);
			break;
		case eEdInpStyle_ColumnLabelOnLeft:
		case eEdInpStyle_ColumnLabelOnTop:
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

bool cEdInputText::TBIdCheckerCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	for(size_t i=0; i<mvTB.size(); ++i)
	{
		if(apWidget==mvTB[i])
		{
			mlCallbackTBId = (int)i;
			break;
		}
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdInputText, TBIdCheckerCallback);

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT FILE BASE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputFile::cEdInputFile(iEdWindow* apWindow,
						   const tWString& asLabel, 
						   float afTextBoxWidth,
						   const tWString& asCat,
						   iWidget* apParent) : cEdInputText(apWindow,
																asLabel, 
																afTextBoxWidth,
																1, false, 0,
																apParent)
{
	msCategory = asCat;

	iWidget* pTB = mvTB.back();
	const cVector3f& vPos = pTB->GetLocalPosition();
	const cVector2f& vSize = pTB->GetSize();
	mpBBrowse = mpSet->CreateWidgetButton(0, cVector2f(20), _W("..."), mpHandle, false);

	mpBBrowse->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(BrowseButton_OnPressed));
	
	mbShowPath = false;

	SetInitialPath(apWindow->GetEditor()->GetWorkingDir());
}

//--------------------------------------------------------

cEdInputFile::~cEdInputFile()
{
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT FILE BASE - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEdInputFile::UpdateLayout()
{
	cEdInputText::UpdateLayout();

	iWidget* pTB = mvTB.back();
	const cVector3f& vPos = pTB->GetLocalPosition();
	const cVector2f& vSize = pTB->GetSize();
	mpBBrowse->SetPosition(vPos+cVector3f(vSize.x+5,0,0));
	
	UpdateSize();
}

//--------------------------------------------------------

void cEdInputFile::SetShowPath(bool abX)
{
	if(mbShowPath==abX)
		return;

	mbShowPath = abX;
	
	CopyValueToInput();
}

//--------------------------------------------------------

void cEdInputFile::SetValue(const tWString& asX, bool abGenerateCallback, bool abCopyToInput, bool abSkipChecks, bool abCheckValidFile)
{
	tString sFile = cString::To8Char(cString::GetFileNameW(asX));
	tWString sFullPath = mpWindow->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(sFile);

	if(abCheckValidFile && CheckFileIsValid(sFullPath)==false)
	{
		abCopyToInput = true;
		msFullPath.clear();
		msFilename.clear();
	}
	else
	{
		msFilename = asX;
		msFullPath = sFullPath;
	}

	iEdInput::SetValue(msFilename, abGenerateCallback, abCopyToInput, abSkipChecks);
}

//------------------------------------------------------------------------

void cEdInputFile::SetInitialPath(const tWString& asPath)
{
	msInitialPath = cString::GetFilePathW(asPath);

	if(cPlatform::FolderExists(msInitialPath)==false)
		msInitialPath = cString::GetFilePathW(mpWindow->GetEditor()->GetWorkingDir());
}

//------------------------------------------------------------------------

void cEdInputFile::AddFileType(const tWString& asType)
{
	 mlstFileTypes.push_back(_W("*.") + asType);
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT FILE BASE - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

bool cEdInputFile::CheckFileIsValid(const tWString& asFile)
{
	return cPlatform::FileExists(asFile);
}

//------------------------------------------------------------------------

void cEdInputFile::CopyValueToInput()
{
	tWString sText;
	if(mbShowPath)
		sText = msFilename;
	else
		sText = cString::GetFileNameW(msFilename);

	mvTB[0]->SetText(sText);
}

//------------------------------------------------------------------------

bool cEdInputFile::BrowseButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	mvTempLoadedFiles.clear();
	ShowBrowser();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdInputFile, BrowseButton_OnPressed);

//------------------------------------------------------------------------

bool cEdInputFile::Browser_OnOkay(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		if(mvTempLoadedFiles.empty()==false)
			msTempLoadedFile = mvTempLoadedFiles.front();

		msFullPath = msTempLoadedFile;
		
		SetValue(msTempLoadedFile, true, true);
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdInputFile, Browser_OnOkay);

//------------------------------------------------------------------------

void cEdInputFile::ShowBrowser()
{
	cEdWindowHandler* pHandler = mpWindow->GetEditor()->GetWindowHandler();

	cGuiPopUpFilePicker* pPicker = pHandler->CreateLoadDialog(mvTempLoadedFiles, msInitialPath, 
																this, kGuiCallback(Browser_OnOkay), 
																msCategory, mlstFileTypes);
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT MODEL - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputModel::cEdInputModel(iEdWindow* apWindow, const tWString& asLabel, bool abAnim, float afTextBoxWidth, iWidget* apParent) : cEdInputFile(apWindow, asLabel, afTextBoxWidth, abAnim?_W("Animations"):_W("Models"), apParent)
{
	tWString sType = _W("dae");
	if(abAnim)
		sType += _W("_anim");
	
	AddFileType(sType);
}

//------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////
// INPUT MODEL - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

bool cEdInputModel::CheckFileIsValid(const tWString& asFile)
{
	tString sFile = cString::To8Char(asFile);

	return cEdLoader::LoadModel(sFile);
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT ENTITY - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputEntity::cEdInputEntity(iEdWindow* apWindow, const tWString& asLabel, float afTextBoxWidth, iWidget* apParent) : cEdInputFile(apWindow, asLabel, afTextBoxWidth, _W("Entities"), apParent)
{
	AddFileType(_W("ent"));
}

//------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////
// INPUT ENTITY - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

bool cEdInputEntity::CheckFileIsValid(const tWString& asFile)
{
	tString sFile = cString::To8Char(asFile);

	return cEdLoader::LoadEntity(sFile);
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT TEXTURE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputTexture::cEdInputTexture(iEdWindow* apWindow, const tWString& asLabel, 
								 float afTextBoxWidth, 
								 bool abMaterialBrowser, eEdTexture aType, 
								 iWidget* apParent) : cEdInputFile(apWindow, asLabel, afTextBoxWidth, _W("Textures"), apParent)
{
	mbMaterialBrowser = abMaterialBrowser;
	mType = aType;

	mpBNew = NULL;
	if(mbMaterialBrowser)
	{
		mpBNew = mpSet->CreateWidgetButton(0, cVector2f(20), _W("+"), mpHandle, false);
		mpBNew->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(NewButton_OnPressed));
	}
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT TEXTURE - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEdInputTexture::UpdateLayout()
{
	cEdInputFile::UpdateLayout();
    if(mpBNew)
	{
		mpBNew->SetPosition(mpBBrowse->GetLocalPosition()+cVector3f(mpBBrowse->GetSize().x+5,0,0));

		UpdateSize();
	}
}

bool cEdInputTexture::CheckFileIsValid(const tWString& asFile)
{
	tString sFile = cString::To8Char(asFile);

	if(mbMaterialBrowser)
		return cEdLoader::LoadMaterial(sFile);
	else
		return cEdLoader::LoadTexture(sFile, (eTextureType)mType);
}

//------------------------------------------------------------------------

void cEdInputTexture::ShowBrowser()
{
	iEditor* pEditor = mpWindow->GetEditor();
	cEdWindowHandler* pHandler = pEditor->GetWindowHandler();

	pHandler->AddPopUp(hplNew(cEdPopUpTextureBrowser,(pEditor, mbMaterialBrowser, mType, msInitialPath, msTempLoadedFile, this, kGuiCallback(Browser_OnOkay))));
}

//------------------------------------------------------------------------

bool cEdInputTexture::NewButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEditor* pEditor = mpWindow->GetEditor();
	cEdWindowMaterialEditor* pMatEditor = hplNew(cEdWindowMaterialEditor,(pEditor, _W(""), this));

	pEditor->GetWindowHandler()->AddPopUp(pMatEditor);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdInputTexture, NewButton_OnPressed);

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT SOUND - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputSound::cEdInputSound(iEdWindow* apWindow, const tWString& asLabel, float afTextBoxWidth, iWidget* apParent) : cEdInputFile(apWindow, asLabel, afTextBoxWidth, _W("Sounds"), apParent)
{
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT SOUND - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

bool cEdInputSound::CheckFileIsValid(const tWString& asFile)
{
	tString sFile = cString::To8Char(asFile);

	return cEdLoader::LoadSound(sFile);
}

//------------------------------------------------------------------------

void cEdInputSound::ShowBrowser()
{
	iEditor* pEditor = mpWindow->GetEditor();
	cEdWindowHandler* pHandler = pEditor->GetWindowHandler();

	pHandler->AddPopUp(hplNew(cEdPopUpSoundBrowser,(pEditor, msInitialPath, msTempLoadedFile, this, kGuiCallback(Browser_OnOkay))));
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT PS - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputParticleSystem::cEdInputParticleSystem(iEdWindow* apWindow, const tWString& asLabel, float afTextBoxWidth, iWidget* apParent) : cEdInputFile(apWindow, asLabel, afTextBoxWidth, _W("Sounds"), apParent)
{
	AddFileType(_W("ps"));
}

//------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////
// INPUT PS - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

bool cEdInputParticleSystem::CheckFileIsValid(const tWString& asFile)
{
	tString sFile = cString::To8Char(asFile);

	return cEdLoader::LoadPS(sFile);
}

//------------------------------------------------------------------------
/*
void cEdInputParticleSystem::ShowBrowser()
{
	iEditor* pEditor = mpWindow->GetEditor();
	cEdWindowHandler* pHandler = pEditor->GetWindowHandler();



	pHandler->AddPopUp(hplNew(cEdPopUpSoundBrowser,(pEditor, msInitialPath, msTempLoadedFile, this, kGuiCallback(Browser_OnOkay))));
}
*/

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT NUMBER - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEdInputNumber::SetValue(float afX, bool abGenerateCallback, bool abCopyToInput, bool abSkipChecks)
{
	tWString sValue = cString::ToStringW(afX,mlDecimals,true);
	iEdInput::SetValue(sValue, abGenerateCallback, abCopyToInput, abSkipChecks);
}

//------------------------------------------------------------------------

float cEdInputNumber::GetValue()
{
	return mvTB[0]->GetNumericValue();
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT VEC2 - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputVec2::cEdInputVec2(iEdWindow* apWindow, const tWString& asLabel, 
						   float afTextBoxWidth, const tWStringList& alstLabels, 
						   iWidget* apParent, float afNumericAdd) : cEdInputText(apWindow, asLabel, afTextBoxWidth, 
																				 2, true, afNumericAdd, apParent, alstLabels)
{
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT VEC2 - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEdInputVec2::SetValue(const cVector2f& avX, bool abGenerateCallback, bool abCopyToInput, bool abSkipChecks)
{
	tWString sValue;

	for(int i=0;i<4;++i)
	{
		sValue += cString::ToStringW(avX.v[i],mlDecimals,true) + _W(" ");
	}
	iEdInput::SetValue(sValue, abGenerateCallback, abCopyToInput, abSkipChecks);
}

//------------------------------------------------------------------------

cVector2f cEdInputVec2::GetValue()
{
	cVector2f vTemp;
	for(int i=0;i<2;++i)
	{
        vTemp.v[i] = mvTB[i]->GetNumericValue();
	}

	return vTemp;
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT VEC3 - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputVec3::cEdInputVec3(iEdWindow* apWindow, const tWString& asLabel, 
						   float afTextBoxWidth, const tWStringList& alstLabels, 
						   iWidget* apParent, float afNumericAdd) : cEdInputText(apWindow, asLabel, afTextBoxWidth, 
																				 3, true, afNumericAdd, apParent, alstLabels)
{
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT VEC3 - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEdInputVec3::SetValue(const cVector3f& avX, bool abGenerateCallback , bool abCopyToInput, bool abSkipChecks)
{
	tWString sValue;

	for(int i=0;i<3;++i)
	{
		sValue += cString::ToStringW(avX.v[i],mlDecimals,true) + _W(" ");
	}
	iEdInput::SetValue(sValue, abGenerateCallback, abCopyToInput, abSkipChecks);
}

//------------------------------------------------------------------------

cVector3f cEdInputVec3::GetValue()
{
	cVector3f vTemp;
	for(int i=0;i<3;++i)
	{
        vTemp.v[i] = mvTB[i]->GetNumericValue();
	}

	return vTemp;
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT ENUM - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputEnum::cEdInputEnum(iEdWindow* apWindow,
						   const tWString& asLabel,
						   float afComboBoxWidth,
						   iWidget* apParent) : iEdInputLabeled(apWindow, asLabel, apParent)
{
	mpCB = mpSet->CreateWidgetComboBox(0, cVector2f(afComboBoxWidth,25), _W(""), mpHandle);
	mpCB->AddCallback(eGuiMessage_SelectionChange, (iEdInput*)this, kGuiCallback(ValueEnter));
	mpCB->SetDefaultFontSize(mvFontSize);
	mpCB->SetClipActive(false);

	mbClearing = false;
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT ENUM - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

int cEdInputEnum::AddValue(const tWString& asX, void *apData)
{
	cWidgetItem* pItem = mpCB->AddItem(asX);
	pItem->SetUserData(apData);

	return pItem->GetIndex();
}

//------------------------------------------------------------------------

void cEdInputEnum::RemoveValue(int alIndex)
{
	mpCB->RemoveItem(alIndex);
}

//------------------------------------------------------------------------

void cEdInputEnum::ClearValues()
{
	mbClearing = true;

	mpCB->SetCallbacksDisabled(true);
	mpCB->ClearItems();
	UpdateValue();
	mpCB->SetCallbacksDisabled(false);

	mbClearing = false;
}

//------------------------------------------------------------------------

void cEdInputEnum::SetMaxShownItems(int alNum)
{
	mpCB->SetMaxShownItems(alNum);
}

//------------------------------------------------------------------------

void cEdInputEnum::UpdateValue()
{
	SetValue(mpCB->GetSelectedItem(),false,false);
}
//------------------------------------------------------------------------

void cEdInputEnum::CopyValueToInput()
{
	tWString sValue = cString::ToLowerCaseW(iEdInput::GetValue());
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

//------------------------------------------------------------------------

void cEdInputEnum::UpdateLayout()
{
	cVector3f vPos = 0;
	cVector2f vLabelSize = mpL->GetSize();
	cVector2f vInputSize = mpCB->GetSize();

	if(mStyle==eEdInpStyle_RowLabelOnLeft ||
		mStyle==eEdInpStyle_ColumnLabelOnLeft)
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

//------------------------------------------------------------------------

int cEdInputEnum::GetNumValues()
{
	return mpCB->GetItemNum();
}

//------------------------------------------------------------------------

void cEdInputEnum::SetValue(int alX, bool abGenerateCallback, bool abCopyToInput, bool abSkipChecks)
{
	tWString sValue = mpCB->GetItemText(alX);
	iEdInput::SetValue(sValue, mbClearing==false && abGenerateCallback, abCopyToInput, abSkipChecks);
}

//------------------------------------------------------------------------

void cEdInputEnum::SetValue(const tWString& asX, bool abGenerateCallback, bool abCopyToInput, bool abSkipChecks)
{
	int lIdx = 0;
	for(int i=0;i<GetNumValues(); ++i)
	{
		const tWString& sValue = GetEnumValue(i);
		if(sValue==asX)
		{
			lIdx = i;
			break;
		}
	}

	SetValue(lIdx, abGenerateCallback, abCopyToInput, abSkipChecks);
}

//------------------------------------------------------------------------

int cEdInputEnum::GetValue()
{
	return mpCB->GetSelectedItem();
}

//------------------------------------------------------------------------

const tWString& cEdInputEnum::GetEnumValue(int alIndex)
{
	return mpCB->GetItemText(alIndex);
}

//------------------------------------------------------------------------

const tWString& cEdInputEnum::GetSelectedEnumValue()
{
	return GetEnumValue(GetValue());
}

tString cEdInputEnum::GetSelectedEnumValue8Char()
{
	return cString::To8Char(GetSelectedEnumValue());
}

//------------------------------------------------------------------------

void* cEdInputEnum::GetUserData(int alIndex)
{
	cWidgetItem* pItem = mpCB->GetItem(alIndex);
	if(pItem) return pItem->GetUserData();

	return NULL;
}

void* cEdInputEnum::GetSelectedUserData()
{
	return GetUserData(GetValue());
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT COLOR - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdInputColor::cEdInputColor(iEdWindow* apWindow,
							 const tWString& asLabel, 
							 iWidget* apParent) : iEdInputLabeled(apWindow, asLabel, apParent, eEdInpStyle_RowLabelOnLeft)
{
	mCol = cColor(1,1);

	mpFColor = mpSet->CreateWidgetFrame(0, cVector2f(15), true, mpHandle);
	mpFColor->AddCallback(eGuiMessage_MouseUp, this, kGuiCallback(Frame_OnClick));
	mpFColor->SetDrawBackground(true);
	mpFColor->SetBackgroundZ(0);
	mpFColor->SetBackGroundColor(mCol);
}

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT COLOR - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEdInputColor::UpdateLayout()
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

//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// INPUT COLOR - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

bool cEdInputColor::Frame_OnClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	if((aData.mlVal&eGuiMouseButton_Left)==0)
		return false;

	mpSet->CreatePopUpColorPicker(&mCol,apWidget->GetGlobalPosition()+cVector3f(-50,0,20),this,kGuiCallback(ColorPicker_OnOK));
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdInputColor, Frame_OnClick);

//--------------------------------------------------------

bool cEdInputColor::ColorPicker_OnOK(iWidget* apWidget, const cGuiMessageData& aData)
{
	SetValue(mCol, true, true);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdInputColor, ColorPicker_OnOK);
//--------------------------------------------------------

void cEdInputColor::UpdateValue()
{
	SetValue(mCol);
}

//--------------------------------------------------------

void cEdInputColor::CopyValueToInput()
{
	const tWString& sValue = iEdInput::GetValue();
	mCol = cString::ToColor(cString::To8Char(sValue).c_str(), cColor(1));
	mpFColor->SetBackGroundColor(cColor(mCol.r, mCol.g, mCol.b, 1));
}
//--------------------------------------------------------

const cColor& cEdInputColor::GetValue()
{
	return mCol;
}

//--------------------------------------------------------

void cEdInputColor::SetValue(const cColor& aCol, bool abGenerateCallback, bool abCopyToInput, bool abSkipChecks)
{
	tWString sValue = cString::ToStringW(aCol.v[0],-1,true);
	for(int i=1;i<4;++i)
	{
		sValue += _W(" ") + cString::ToStringW(aCol.v[i],-1,true);
	}
	iEdInput::SetValue(sValue, abGenerateCallback, abCopyToInput, abSkipChecks);
}

//--------------------------------------------------------
