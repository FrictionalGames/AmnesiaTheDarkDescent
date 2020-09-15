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

#ifndef ED_WINDOW_HANDLER_H
#define ED_WINDOW_HANDLER_H

//---------------------------------------------------------------------

#include "../Common/EdModule.h"
#include "../Common/EdInput.h"

//---------------------------------------------------------------------

class cEdWindowHandler;
class iEdWindow;

class iEdMenu;

class cLayoutBlock;

class cEdInputBool;
class cEdInputText;
class cEdInputFile;
class cEdInputMaterial;
class cEdInputTexture;
class cEdInputSound;
class cEdInputNumber;
class cEdInputEnum;
class cEdInputVec2;
class cEdInputVec3;
class cEdInputColor;

class iEdEditMode;

class cEdPopUpTextureBrowser;
class cEdPopUpSoundBrowser;

//class cEdWindowFind;

//---------------------------------------------------------------------

enum eLayoutBlock
{
	eLayoutBlock_BG,

	eLayoutBlock_LastEnum
};

//---------------------------------------------------------------------

class iEdWindow : public iEdModule
{
public:
	iEdWindow(iEditor*, const tWString&);
	virtual ~iEdWindow();

	void SetDestroyOnSetInactive(bool);

	float GetMenuHeight();
	void SetMenu(iEdMenu*);
	void SetLayoutBlock(const cLayoutBlock*);

	bool IsActive();
	void SetActive(bool);

	void SetPosition(const cVector3f&);
	void SetSize(const cVector2f&);
	void SetWidth(float);
	void SetHeight(float);

	void SetUpdated();

	cGuiSet* GetGuiSet() { return mpSet; }
	iWidget* GetBG() { return mpBGWidget; }

	///////////////////////////////////////////////////////////
	// Editor inputs
	cEdInputBool*		CreateInputBool(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL);

	cEdInputText*		CreateInputString(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, float afBoxWidth=100);



	// File inputs
	//------------------------------------------------------------------------------------
	cEdInputFile*		CreateInputFile(const cVector3f& avPos, const tWString& asLabel, const tWString& asCategory, 
										iWidget* apParent=NULL, float afBoxWidth=100, const tWString& asInitPath=_W(""));

	cEdInputFile*		CreateInputMesh(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
										float afBoxWidth=100, const tWString& asInitPath=_W(""));

	cEdInputFile*		CreateInputAnim(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
										float afBoxWidth=100, const tWString& asInitPath=_W(""));

	cEdInputFile*		CreateInputEntFile(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
											float afBoxWidth=100, const tWString& asInitPath=_W(""));

	cEdInputFile*		CreateInputParticleSystem(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
													float afBoxWidth=100, const tWString& asInitPath=_W(""));

	cEdInputTexture*	CreateInputMaterial(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
											float afBoxWidth=100, const tWString& asInitPath=_W(""));

	cEdInputTexture*	CreateInputTexture(const cVector3f& avPos, const tWString& asLabel, eEdTexture aType, iWidget* apParent=NULL, 
											float afBoxWidth=100, const tWString& asInitPath=_W(""));
	cEdInputTexture*	CreateInputTexture1D(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
												float afBoxWidth=100, const tWString& asInitPath=_W(""));
	cEdInputTexture*	CreateInputTexture2D(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
												float afBoxWidth=100, const tWString& asInitPath=_W(""));
	cEdInputTexture*	CreateInputCubeMap(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
											float afBoxWidth=100, const tWString& asInitPath=_W(""));

	cEdInputSound*		CreateInputSound(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
											float afBoxWidth=100, const tWString& asInitPath=_W(""));
	//------------------------------------------------------------------------------------

	cEdInputNumber*		CreateInputNumber(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
											float afBoxWidth=50, float afNumericAdd=0);

	cEdInputEnum*		CreateInputEnum(const cVector3f& avPos, const tWString& asLabel, 
										iWidget* apParent=NULL, float afBoxWidth=100);

	cEdInputVec2*		CreateInputVec2(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
										float afBoxWidth=50, const tWStringList& alstLabels=tWStringList(), 
										eEdInpStyle aStyle=eEdInpStyle_RowLabelOnTop, float afNumericAdd=0);

	cEdInputVec3*		CreateInputVec3(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL, 
										float afBoxWidth=50, const tWStringList& alstLabels=tWStringList(), 
										eEdInpStyle aStyle=eEdInpStyle_RowLabelOnTop, float afNumericAdd=0);

	cEdInputColor*		CreateInputColor(const cVector3f& avPos, const tWString& asLabel, iWidget* apParent=NULL);

	void DestroyInput(iEdInput*);

	void DispatchMessage(eEdModuleMsg, void*);

	void AddShortcut(cGuiGlobalShortcut*);

protected:
	void SetUpInput(iEdInput*, const cVector3f&, eEdInpStyle aStyle=eEdInpStyle_RowLabelOnTop);
	void SetUpFileInput(cEdInputFile*, const cVector3f&, eEdInpStyle aStyle=eEdInpStyle_RowLabelOnTop, const tWString& asStartPath=_W(""));

	bool EdInputCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(EdInputCallback);

	virtual bool WindowSpecificInputCallback(iEdInput*) { return false; }

	virtual void OnInit();
	virtual void OnCreateLayout();

	virtual void OnSetActive(bool)					{}
	virtual void OnSetPosition(const cVector3f&)	{}
	virtual void OnSetSize(const cVector2f&)		{}

	virtual iWidget* CreateBGWidget(const cVector3f&, const cVector2f&, iWidget*)=0;

	////////////////////////////////////////
	// Data
	bool mbUpdated;

	bool mbDestroyOnSetInactive;

	bool mbLayoutCreated;

	static cGuiSet* mpSet;
	iWidget* mpBGWidget;

	iEdMenu* mpMenu;

	const cLayoutBlock* mpLayoutBlock;

	tInputList mlstInputs;

	std::vector<cGuiGlobalShortcut*> mvShortcuts;
};

//---------------------------------------------------------------------

typedef std::list<iEdWindow*>	tEdWindowList;
typedef tEdWindowList::iterator tEdWindowListIt;

//---------------------------------------------------------------------

class iEdPane : public iEdWindow
{
public:
	iEdPane(iEditor*, const tWString&);
protected:
	iWidget* CreateBGWidget(const cVector3f&, const cVector2f&, iWidget*);
};

//---------------------------------------------------------------------

class iEdPopUp : public iEdWindow
{
public:
	iEdPopUp(iEditor*, const tWString&);
protected:
	iWidget* CreateBGWidget(const cVector3f&, const cVector2f&, iWidget*);
	void OnSetActive(bool);

	bool PopUpOnClose(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(PopUpOnClose);
};

//---------------------------------------------------------------------

class iEdEditModePane : public iEdPane
{
public:
	iEdEditModePane(iEdEditMode*);

	iEdEditMode* GetEditMode() { return mpEditMode; }

protected:
	iEdEditMode* mpEditMode;
};

//---------------------------------------------------------------------

class cLayoutBlock
{
public:
	cLayoutBlock(int alID, const cVector3f& avPos, const cVector2f& avSize, iWidget* apParent)
	{
		mlID = alID;
		mvPosition = avPos;
		mvSize = avSize;
		mpParent = apParent;
	}

	int mlID;
	cVector3f mvPosition;
	cVector2f mvSize;
	iWidget* mpParent;
};

//---------------------------------------------------------------------

typedef std::list<cLayoutBlock> tLayoutBlockList;
typedef tLayoutBlockList::iterator tLayoutBlockListIt;

//---------------------------------------------------------------------

class cEdWindowHandler : public iEdModule
{
	friend class iEdWindow;
public:
	cEdWindowHandler(iEditor*);
	~cEdWindowHandler();

	void AddWindow(iEdWindow*, int alLayoutBlockID=-1, bool abActivate=true);
	void AddBackground(iEdWindow*);
	void AddMainWindow(iEdWindow*, int alLayoutBlockID);
	void AddPopUp(iEdWindow*, bool abActivate=true);

	void DestroyWindow(iEdWindow*);

	iEdWindow* GetBackground() { return mpBackground; }
	iEdWindow* GetWindow(const tWString&);

	const cLayoutBlock* AddLayoutBlock(int alID, 
										bool abRelX, bool abRelY, const cVector3f& avPos, 
										bool abRelWidth, bool abRelHeight, const cVector2f& avSize, 
										iWidget* apParent=NULL);
	const cLayoutBlock* GetLayoutBlock(int);




	////////////////////////////////////////////////////
	// Pop Up handling
	cGuiPopUpFilePicker* CreateLoadDialog(tWStringVec& avDestFilenames, const tWString& asStartPath, 
											void* apCallbackObject, tGuiCallbackFunc apCallback, 
											const tWString& asCategoryName, const tWStringList& alstCategoryStrings);

	cGuiPopUpFilePicker* CreateSaveDialog(tWString& asFilename, const tWString& asStartPath, 
											void* apCallbackObject, tGuiCallbackFunc apCallback,
											const tWString& asCategoryName, const tWString& asFilter);

	/*
	cEdPopUpTextureBrowser* CreateTextureBrowser(bool abMaterialBrowser, eEdTexture aType, const tWString& asStartPath, tWString& asFile,
													void* apCallbackObject, tGuiCallbackFunc apCallback);
	cEdPopUpSoundBrowser*	CreateSoundBrowser(const tWString& asStartPath, tWString& asFile,
												void* apCallbackObject, tGuiCallbackFunc apCallback);
												*/


	//cEdWindowFind* CreateFindDialog();

	bool IsPopUpActive();

	void DispatchMessage(eEdModuleMsg, void*);

protected:
	void BroadcastMsgToWindows(eEdModuleMsg, void*);

	void CreatePendingWindows();
	
	void OnCreateLayout();
	void OnUpdate();

	void OnWorldParamsChange();

	void AddWindowToCreateList(iEdWindow*);
	void AddWindowToActivateList(iEdWindow*);
	void AddWindowToUpdateList(iEdWindow*);
	void AddWindowToDestroyList(iEdWindow*);

	bool PopUpOnClose(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(PopUpOnClose);

	tEdWindowList mlstWindows;
	tEdWindowList mlstWindowsToCreate;
	tEdWindowList mlstWindowsToActivate;
	tEdWindowList mlstWindowsToUpdate;
	tEdWindowList mlstWindowsToDestroy;

	tEdWindowList mlstPopups;

	iEdWindow* mpBackground;

	tLayoutBlockList mlstLayoutBlocks;

	int mlEdPopUpCount;
};

//---------------------------------------------------------------------

#endif // ED_WINDOW_HANDLER_H
