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

#ifndef ED_EDIT_MODE_H
#define ED_EDIT_MODE_H

//---------------------------------------------------------------------------

#include "../common/EdModule.h"

//---------------------------------------------------------------------------

class iEditor;
class iEdWorld;
class iEdWindow;
class cEdWindowViewport;
class cViewportClick;
class iEdAction;
class cEdEditModeHandler;

class iEdObjectType;
class iEdObjectData;


//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// EDITMODE BASE CLASS
/////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class iEdEditMode : public iEdModule
{
	friend class cEdEditModeHandler;
public:
	iEdEditMode(iEditor*, const tWString&);
	virtual ~iEdEditMode();

	iEdWindow* GetWindow()			{ return mpWindow; }
	iEdWorld*  GetWorld();

	cGuiGfxElement* GetIcon()		{ return mpIcon; }

	void SetActive(bool);
	bool IsActive();

	virtual bool IsBusy()=0;
	
protected:
	virtual cGuiGfxElement* CreateIcon();

	virtual void OnSetActive(bool){}

	virtual void OnCreateLayout();

	virtual void OnViewportMouseDown(const cViewportClick&)=0;
	virtual void OnViewportMouseUp(const cViewportClick&)=0;

	virtual void OnDraw(const cModuleDrawData&) {}

	virtual iEdWindow* CreateWindow()=0;

	//////////////////////////////////////////////////////
	// Data
	iEdWindow* mpWindow;
	cGuiGfxElement* mpIcon;
};

//---------------------------------------------------------------------------

typedef std::list<iEdEditMode*> tEditModeList;
typedef tEditModeList::iterator	tEditModeListIt;

typedef std::vector<iEdEditMode*> tEditModeVec;

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// OBJECT CREATOR MODE CLASS
/////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class iEdEditModeObjectCreator : public iEdEditMode
{
public:
	iEdEditModeObjectCreator(iEditor*, const tWString&);
	virtual ~iEdEditModeObjectCreator();

	iEdObjectType* GetMainType();
	void SetSelectedType(int);
	virtual iEdObjectType* GetSelectedType();

	void SetBaseName(const tWString&);
	virtual iEdObjectData* CreateObjectData();

	bool IsBusy();

protected:
	void OnInit();	
	//void OnUpdate();

	virtual void OnViewportMouseDown(const cViewportClick&);
	virtual void OnViewportMouseUp(const cViewportClick&);

	void OnSetActive(bool);

	virtual iEdObjectType* CreateType()=0;
	virtual void OnSetSubType(int) {};

	virtual bool SetUpCreationData(iEdObjectData* apData)=0;

	bool mbPreCreationActive;

	int mlSubType;
	iEdObjectType* mpType;

	tWString msBaseName;
};

//---------------------------------------------------------------------------

#endif // ED_EDIT_MODE_H
