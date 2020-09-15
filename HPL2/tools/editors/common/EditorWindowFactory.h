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


#ifndef HPLEDITOR_EDITOR_WINDOW_FACTORY_H
#define HPLEDITOR_EDITOR_WINDOW_FACTORY_H

#include "../common/StdAfx.h"
using namespace hpl;

#include "EditorTypes.h"

//--------------------------------------------

class iEditorBase;
class iEditorWindow;

class cEditorWindowViewport;
class cEditorWindowStaticObjects;
class cEditorWindowEntities;
class cEditorWindowSelect;
class cEditorWindowEntityEditBox;
class cEditorWindowLights;
class cEditorWindowEntitySearch;
class cEditorWindowBodies;
class cEditorWindowJoints;
class cEditorWindowAreas;
class cEditorWindowBillboards;
class cEditorWindowSounds;
class cEditorWindowParticleSystems;
class cEditorWindowPrimitives;

class cEditorWindowInputWorldName;
class cEditorWindowTextureBrowser;
class cEditorWindowLoaderStatus;

class cEditorWindowMaterialEditor;

class cEditorEditModeSelect;

//--------------------------------------------

class cEditorWindowFactory
{
public:
	static cEditorWindowViewport* CreateWindowViewport(iEditorBase* apEditor,
													   iFrameBuffer* apFB,
													   const cVector2l& avFBViewPos,
													   const cVector2l& avFBViewSize,
													   const cVector3f& avPosition,
													   const cVector2f& avSize,
													   bool abAddViewMenu=true);

	///////////////////////////////////////////////////////////////
	// Edit Mode windows
	static cEditorWindowStaticObjects* CreateWindowStaticObjects(iEditorBase* apEditor,
																 const cVector3f& avPosition,
																 const tWString& asBaseDir);

	static cEditorWindowEntities* CreateWindowEntities(iEditorBase* apEditor,
													   const cVector3f& avPosition,
													   const tWString& asBaseDir);

	static cEditorWindowSelect* CreateWindowSelect(iEditorBase* apEditor,
												   const cVector3f& avPosition);

	static cEditorWindowEntityEditBox* CreateEditBoxWindow(iEditorBase* apEditor,
														   cEditorEditModeSelect* apEditMode,
														   const cVector3f& avPosition,
														   tEntityWrapperList alstEntities);

	static cEditorWindowLights* CreateWindowLights(iEditorBase* apEditor,
												   const cVector3f& avPosition);

	static cEditorWindowBodies* CreateWindowBodies(iEditorBase* apEditor,
													const cVector3f& avPosition);

	static cEditorWindowJoints* CreateWindowJoints(iEditorBase* apEditor,
													const cVector3f& avPosition);

	static cEditorWindowAreas* CreateWindowAreas(iEditorBase* apEditor,
												 const cVector3f& avPosition);

	static cEditorWindowBillboards* CreateWindowBillboards(iEditorBase* apEditor,
														   const cVector3f& avPosition);

	static cEditorWindowSounds* CreateWindowSounds(iEditorBase* apEditor,
												   const cVector3f& avPosition);

	static cEditorWindowParticleSystems* CreateWindowParticleSystems(iEditorBase* apEditor,
																	 const cVector3f& avPosition);

	static cEditorWindowPrimitives* CreateWindowPrimitives(iEditorBase* apEditor,
														   const cVector3f& avPosition);

	/////////////////////////////////////////////////////////
	// Utility windows

	static cEditorWindowEntitySearch* CreateSearchWindow(iEditorBase* apEditor,
														 cEditorEditModeSelect* apEditMode);

	static cEditorWindowTextureBrowser* CreateTextureBrowserWindow(iEditorBase* apEditor);

	static cEditorWindowLoaderStatus* CreateLoaderStatusWindow(iEditorBase* apEditor);

	static cEditorWindowMaterialEditor* CreateMaterialEditor(iEditorBase* apEditor, iFrameBuffer* apFB);

	static void DestroyEditorWindow(iEditorWindow* apWindow);

protected:
};

//--------------------------------------------

#endif //HPLEDITOR_EDITOR_WINDOW_FACTORY_H
