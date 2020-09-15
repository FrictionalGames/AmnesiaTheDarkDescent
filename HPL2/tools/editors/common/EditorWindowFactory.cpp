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

#include "EditorWindowFactory.h"

#include "EditorWindowViewport.h"
#include "EditorWindowStaticObjects.h"
#include "EditorWindowEntities.h"
#include "EditorWindowSelect.h"
#include "EditorWindowLights.h"
#include "EditorWindowBodies.h"
#include "EditorWindowJoints.h"
#include "EditorWindowAreas.h"
#include "EditorWindowBillboards.h"
#include "EditorWindowSounds.h"
#include "EditorWindowParticleSystems.h"
#include "EditorWindowPrimitives.h"
#include "EditorWindowInputWorldName.h"

#include "EditorWindowEntityEditBoxStaticObject.h"
#include "EditorWindowEntityEditBoxLight.h"
#include "EditorWindowEntityEditBoxBodyShape.h"
#include "EditorWindowEntityEditBoxGroup.h"

#include "EditorWindowEntitySearch.h"

#include "EditorWindowLoaderStatus.h"

#include "EditorWindowTextureBrowser.h"

#include "EditorWindowMaterialEditor.h"

#include "EntityWrapper.h"

//--------------------------------------------------------

//////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////

//--------------------------------------------------------

cEditorWindowViewport* cEditorWindowFactory::CreateWindowViewport(iEditorBase* apEditor,
																  iFrameBuffer* apFB,
																  const cVector2l& avFBViewPos,
																  const cVector2l& avFBViewSize,
																  const cVector3f& avPosition, 
																  const cVector2f& avSize,
																  bool abAddViewMenu)
{
	////////////////////////////////////////
	// Create viewport and set parameters
	cEditorWindowViewport* pViewport = hplNew(cEditorWindowViewport,(apEditor, apFB, abAddViewMenu));
	pViewport->SetEngineViewportPositionAndSize(avFBViewPos,avFBViewSize);

	pViewport->Init();

	pViewport->SetPosition(avPosition);
	pViewport->SetSize(avSize);
	pViewport->UpdateViewport();

	apEditor->AddWindow(pViewport);

	return pViewport;
}

//--------------------------------------------------------

cEditorWindowStaticObjects* cEditorWindowFactory::CreateWindowStaticObjects(iEditorBase* apEditor, 
																			const cVector3f& avPosition,
																			const tWString& asBaseDir)
{
	/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowEntities* cEditorWindowFactory::CreateWindowEntities(iEditorBase* apEditor,
																  const cVector3f& avPosition,
																  const tWString& asBaseDir)
{
		/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowSelect* cEditorWindowFactory::CreateWindowSelect(iEditorBase* apEditor,
															  const cVector3f& avPosition)
{
		/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowEntityEditBox* cEditorWindowFactory::CreateEditBoxWindow(iEditorBase* apEditor,
																	  cEditorEditModeSelect* apEditMode, 
																	  const cVector3f& avPosition, 
																	  tEntityWrapperList alstEntities)
{	/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowLights* cEditorWindowFactory::CreateWindowLights(iEditorBase* apEditor, const cVector3f& avPosition)
{
		/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowBodies* cEditorWindowFactory::CreateWindowBodies(iEditorBase* apEditor, const cVector3f& avPosition)
{
	/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowJoints* cEditorWindowFactory::CreateWindowJoints( iEditorBase* apEditor,	const cVector3f& avPosition)
{
		/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowAreas* cEditorWindowFactory::CreateWindowAreas( iEditorBase* apEditor, const cVector3f& avPosition)
{
		/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowBillboards* cEditorWindowFactory::CreateWindowBillboards(iEditorBase* apEditor, const cVector3f& avPosition)
{
		/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowSounds* cEditorWindowFactory::CreateWindowSounds(iEditorBase* apEditor, const cVector3f& avPosition)
{
		/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowParticleSystems* cEditorWindowFactory::CreateWindowParticleSystems(iEditorBase* apEditor,
																	 const cVector3f& avPosition)
{
		/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowPrimitives* cEditorWindowFactory::CreateWindowPrimitives(iEditorBase* apEditor,
																	  const cVector3f& avPosition)
{
		/*
	cEditorWindowStaticObjects* pWindow = hplNew(cEditorWindowStaticObjects,(apEditor, asBaseDir) );

	pWindow->Init();
	
	pWindow->SetPosition(avPosition);

	apEditor->AddWindow(pWindow);

	return pWindow;
	*/
	return NULL;
}

//--------------------------------------------------------

cEditorWindowEntitySearch* cEditorWindowFactory::CreateSearchWindow(iEditorBase* apEditor, cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntitySearch* pWindow = hplNew( cEditorWindowEntitySearch, (apEditor));

	pWindow->Init();
	pWindow->SetActive(false);

	apEditor->AddWindow(pWindow);
	
	return pWindow;
}
//--------------------------------------------------------

cEditorWindowTextureBrowser* cEditorWindowFactory::CreateTextureBrowserWindow(iEditorBase* apEditor)
{
	return NULL;
	/*
	cEditorWindowTextureBrowser* pWindow = hplNew( cEditorWindowTextureBrowser(apEditor, ));

	pWindow->Init();

	cVector3f vCenter = cVector3f(apEditor->GetSet()->GetVirtualSize()*0.5f);
	vCenter.z = 30;

	pWindow->SetPosition(vCenter-cVector3f(400,260,0));

	apEditor->AddWindow(pWindow);

	return pWindow;*/
}

//--------------------------------------------------------

cEditorWindowLoaderStatus* cEditorWindowFactory::CreateLoaderStatusWindow(iEditorBase* apEditor)
{
	cEditorWindowLoaderStatus* pWindow = hplNew( cEditorWindowLoaderStatus, (apEditor));
	
	pWindow->Init();
	pWindow->SetCaption(_W("Loading"));
	
	apEditor->AddWindow(pWindow);

	return pWindow;
}

//--------------------------------------------------------


//--------------------------------------------------------

cEditorWindowMaterialEditor* cEditorWindowFactory::CreateMaterialEditor(iEditorBase* apEditor, iFrameBuffer* apFB)
{
	cEditorWindowMaterialEditor* pWindow = hplNew(cEditorWindowMaterialEditor,(apEditor,apFB));
	pWindow->Init();
	apEditor->AddWindow(pWindow);
	return pWindow;
}

//--------------------------------------------------------

void cEditorWindowFactory::DestroyEditorWindow(iEditorWindow *apWindow)
{
	if(apWindow==NULL)
		return;

	apWindow->GetEditor()->RemoveWindow(apWindow);

	hplDelete(apWindow);
}

//--------------------------------------------------------
