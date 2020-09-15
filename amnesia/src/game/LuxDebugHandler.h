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

#ifndef LUX_DEBUG_HANDLER_H
#define LUX_DEBUG_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxDebugMessage
{
public:
	tWString msText;
	float mfCount;
};

typedef std::list<cLuxDebugMessage> tLuxDebugMessageList;
typedef tLuxDebugMessageList::iterator tLuxDebugMessageListIt;

//----------------------------------------------

class cLuxDebugHandler : public iLuxUpdateable
{
public:	
	cLuxDebugHandler();
	~cLuxDebugHandler();

	void LoadUserConfig();
	void SaveUserConfig();
	
	void OnStart();
	void Update(float afTimeStep);
	void Reset();

	void OnMapEnter(cLuxMap *apMap);
	void OnMapLeave(cLuxMap *apMap);


	void SetDebugWindowActive(bool abActive);
	bool GetDebugWindowActive(){ return mbWindowActive;}
		
	void OnDraw(float afFrameTime);
	void RenderSolid(cRendererCallbackFunctions* apFunctions);

	void AddErrorOrWarningMessage(const tWString& asText);
	void AddMessage(const tWString& asText, bool abCheckForDuplicates);

	bool GetShowPlayerInfo(){ return mbShowPlayerInfo;}
	bool GetShowEntityInfo(){ return mbShowEntityInfo;}
	bool GetScriptDebugOn(){ return mbScriptDebugOn;}
	bool GetDisableFlashBacks(){ return mbDisableFlashBacks;}
	bool GetAllowQuickSave(){ return mbAllowQuickSave;}

	void SetFastForward(bool abX);
	bool GetFastForward(){ return mbFastForward;}

	void QuickReloadMap();

private:
	void CheckLineObjectIntersection(iRenderable *apObject, const cVector3f& avStart, const cVector3f& avEnd, cBoundingVolume *apBV);
	void IterateRenderableNode(iRenderableContainerNode *apNode, const cVector3f& avStart, const cVector3f& avEnd, cBoundingVolume *apBV);
	void UpdateInspectionMeshEntity(float afTimeStep);

	void CreateGuiWindow();
	void CreateScriptOutputWindow();
	void CreateScriptOutputWindowText(const tWString& asOutput);
	void UpdateMessages(float afTimeStep);

	void ShowScriptOutputWindow(const tWString& asName, const tString& asText);
	bool RecompileScript();
	void ReloadTranslations();
	void ReloadMap();
	void TestChangeMapSave();

	void LoadBatchLoadFile(const tWString& asFilePath);
	
    void DrawDynamicContainerDebugInfo();
	void OutputContainerContentsRec(iRenderableContainerNode *apNode, int alLevel);
	void CheckDynamicContainerBugsRec(iRenderableContainerNode *apNode, int alLevel);

	/////////////////////
	// GUI Callbacks
	bool ChangeDebugText(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ChangeDebugText);

	bool PressPrinfContDebugInfo(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressPrinfContDebugInfo);

	bool PressRebuildDynCont(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressRebuildDynCont);

	bool PressLevelReload(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressLevelReload);

	bool PressQuickLevelReload(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressQuickLevelReload);

	bool PressTestChangeMapSave(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressTestChangeMapSave);

	bool PressLoadWorld(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressLoadWorld);

	bool LoadWorldFromFilePicker(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(LoadWorldFromFilePicker);

	bool PressTelportPlayer(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressTelportPlayer);

	bool PressReloadInsanityEffect(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressReloadInsanityEffect);

	bool PressStartInsanityEffect(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressStartInsanityEffect);

	bool PressRecompileScript(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressRecompileScript);

	bool PressCloseScriptOutput(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressCloseScriptOutput);

	bool PressBatchLoad(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressBatchLoad);

	bool PressLoadBatchLoadFile(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressLoadBatchLoadFile);



	cGui *mpGui;

	cGuiSkin *mpGuiSkin;
	cGuiSet *mpGuiSet;

	cWidgetComboBox *mpCBInsanityEvents;
	cWidgetComboBox *mpCBPlayerStarts;

	cWidgetWindow *mpDebugWindow;

	cWidgetWindow *mpScriptOutputWindow;
	cWidgetFrame *mpScriptOutputFrame;

	cWidgetCheckBox *mpCBFastForward;
	
	tWidgetList mlstScriptOutputWidgets;

	bool mbShowFPS;
	bool mbShowSoundPlaying;
	bool mbShowPlayerInfo;
	bool mbShowEntityInfo;
	bool mbShowDebugMessages;
	bool mbScriptDebugOn;
	bool mbInspectionMode;
	bool mbDrawPhysics;

	bool mbAllowQuickSave;
    
	bool mbWindowActive;

	bool mbReloadFromCurrentPosition;
	bool mbDisableFlashBacks;

	bool mbFastForward;
	bool mbShowErrorMessages;

	cSubMeshEntity *mpInspectMeshEntity;

	bool mbFirstUpdateOnMap;

	tStringList m_lstBatchMaps;

	tWStringVec mvPickedFiles;
	tWString msCurrentFilePath;

	tLuxDebugMessageList mlstMessages;
	int mlTempCount;
};

//----------------------------------------------


#endif // LUX_DEBUG_HANDLER_H
