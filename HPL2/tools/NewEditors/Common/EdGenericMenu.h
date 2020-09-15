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

#ifndef ED_GENERIC_MENU_H
#define ED_GENERIC_MENU_H

#include "../Common/EdMenu.h"

//----------------------------------------------------------

class cEdGenericMenu : public iEdMenu
{
public:
	cEdGenericMenu(iEdWindow*);

protected:
	void AddNew(cWidgetMenuItem*, int alMod=eKeyModifier_Ctrl, eKey aKey=eKey_N);
	void AddOpen(cWidgetMenuItem*, int alMod=eKeyModifier_Ctrl, eKey aKey=eKey_O);
	void AddSave(cWidgetMenuItem*, int alMod=eKeyModifier_Ctrl, eKey aKey=eKey_S);
	void AddSaveAs(cWidgetMenuItem*, int alMod=eKeyModifier_None, eKey aKey=eKey_None);
	void AddRecentFiles(cWidgetMenuItem*);
	void AddExit(cWidgetMenuItem*, int alMod=eKeyModifier_Alt, eKey aKey=eKey_F4);

	void AddActionControls(cWidgetMenuItem*);
	void AddPreferences(cWidgetMenuItem*);

	void OnActionExec();
	void OnWorldLoad();
	void OnWorldSave();

	virtual void UpdateActionControls();
	virtual void UpdateRecentFiles();

	bool New(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(New);
	bool Open(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(Open);
	bool Save(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(Save);
	bool SaveAs(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(SaveAs);
	bool Quit(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(Quit);

	bool OpenRecentFile(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(OpenRecentFile);

	bool Undo(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(Undo);
	bool Redo(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(Redo);

	bool Preferences(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(Preferences);

	virtual void New();
	virtual void Open();
	virtual void Save();
	virtual void SaveAs();
	virtual void Quit();


	////////////////////////////////////////////////////////
	// Data
	cWidgetMenuItem* mpRecent;

	cWidgetMenuItem* mpUndo;
	cWidgetMenuItem* mpRedo;
};

#endif // ED_GENERIC_MENU_H
