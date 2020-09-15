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

#ifndef HPLEDITOR_EDIT_MODE_H
#define HPLEDITOR_EDIT_MODE_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "EditorTypes.h"
#include "EditorUIPicker.h"

//------------------------------------------------------

class iEditorBase;
class iEditorAction;
class iEditorWindow;
class iEditorWorld;

class iEntityWrapperType;

class cEditorWindowViewport;

//------------------------------------------------------

//////////////////////////////////////////////////////////////
// iEditorEditMode
//  Base interface for edit modes. May hold a helper window, and provides means 
//  to work with viewport interactions
class iEditorEditMode
{
public:
	iEditorEditMode(iEditorBase* apEditor, const tString& asName);
	virtual ~iEditorEditMode();

	virtual void OnAdd() {}
	virtual void PostEditModesCreation() {}

	const tString& GetName() { return msName; }

	virtual void Reset(){}

	iEditorBase* GetEditor() { return mpEditor; }

	iEditorWindow* GetEditorWindow() { return mpWindow; }

	void SetCurrent(bool abX);

	iEditorWindow* CreateWindow();

	virtual void OnViewportMouseDown(int alButtons){}
	virtual void OnViewportMouseUp(int alButtons){}

	virtual void OnEditorUpdate(float afTimeStep){}

	virtual void OnSetCurrent(bool abX){}

	virtual void DrawPreGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos);
	virtual void DrawPostGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos);

protected:
	///////////////////////////////
	// Data
	iEditorBase* mpEditor;
	iEditorWindow* mpWindow;

	tString msName;

	virtual iEditorWindow* CreateSpecificWindow()=0;
};

//------------------------------------------------------

//////////////////////////////////////////////////////////////
// iEditorEditModeObjectCreator
//  Interface for edit modes that do create new map objects. May hold a helper window, and provides means 
//  to work with viewport interactions
class iEditorEditModeObjectCreator : public iEditorEditMode
{
public:
	iEditorEditModeObjectCreator(iEditorBase* apEditor, const tString& asName, iEditorWorld* apEditorWorld);
	virtual ~iEditorEditModeObjectCreator();

	void OnAdd();

	void SetCreateOnSurface(bool abX) { mbCreateOnSurface = abX; }
	bool GetCreateOnSurface() { return mbCreateOnSurface; }

	void SetAffectSurfaceType(int, bool);
	bool GetAffectSurfaceType(int);

	const cVector3f& GetUpVector();
	const cVector3f& GetRightVector();

	void OnEditorUpdate(float afTimeStep);
	virtual void OnViewportMouseDown(int alButtons);
	virtual void OnViewportMouseUp(int alButtons);

	virtual iEntityWrapperData* CreateObjectData();
	virtual iEditorAction* CreateObject(iEntityWrapperData* apData);

	virtual void DrawPostGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos);
	virtual void DrawObjectPreview(cEditorWindowViewport* apViewport, cRendererCallbackFunctions *apFunctions, const cMatrixf& amtxTransform, bool abPreCreationActive){}

	void SetSubType(int alX) { mlSubType = alX; OnSetSubType(alX); }
	int GetTypeNum();
	iEntityWrapperType* GetType(int alX);
	virtual iEntityWrapperType* GetType();

	void UpdateCreatorScale();
	void UpdateCreatorRotation();

	void UpdateLocalRotation();
protected:
	void OnSetCurrent(bool abX);

	const cVector3f& GetCreatorPosition();
	const cVector3f& GetCreatorRotation();
	const cVector3f& GetCreatorScale();

	virtual bool SetUpCreationData(iEntityWrapperData* apData);
	virtual void SetUpTypes();
	virtual void CreateTypes()=0;
	virtual void OnSetSubType(int alX) {}

	iEditorWorld* mpEditorWorld;
	std::vector<iEntityWrapperType*> mvTypes;

	bool mbPreCreationActive;

	int mlSubType;

	bool mbCreateOnSurface;
	tBoolVec mvAffectedSurfaceTypes;

	cVector3f mvUpVector;
	cVector3f mvRightVector;

	cVector3f mvCreatorPosition;
	cVector3f mvCreatorScale;

	cVector3f mvCreatorLocalRotation;
	cVector3f mvCreatorWorldRotation;
};

//------------------------------------------------------

#endif // HPLEDITOR_EDIT_MODE_H
