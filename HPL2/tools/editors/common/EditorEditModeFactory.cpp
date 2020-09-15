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

#include "EditorEditModeFactory.h"

#include "EditorEditModeStaticObjects.h"
#include "EditorEditModeEntities.h"
#include "EditorEditModeSelect.h"
#include "EditorEditModeLights.h"
#include "EditorEditModeBodies.h"
#include "EditorEditModeJoints.h"
#include "EditorEditModeAreas.h"
#include "EditorEditModeBillboards.h"
#include "EditorEditModeSounds.h"
#include "EditorEditModeParticleSystems.h"
#include "EditorEditModePrimitive.h"


#include "EditorWindowFactory.h"

#include "EditorEditModeDummy.h"
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

//---------------------------------------------------------------

cEditorEditModeDummy* cEditorEditModeFactory::CreateEditModeDummy(iEditorBase* apEditor)
{
	cEditorEditModeDummy* pEditMode = hplNew( cEditorEditModeDummy,(apEditor));

	return pEditMode;
}

//---------------------------------------------------------------

cEditorEditModeStaticObjects* cEditorEditModeFactory::CreateEditModeStaticObjects(iEditorBase* apEditor,
																				  iEditorWorld* apEditorWorld,
																				  const cVector3f& avEditorWindowPos)
{
//	cEditorWindowStaticObjects* pWindow = cEditorWindowFactory::CreateWindowStaticObjects(apEditor,
//																						  avEditorWindowPos,
//																						  apEditor->GetFolderFullPath(eEditorDir_StaticObjects));
//	pWindow->SetEnabled(false);
//	pWindow->SetVisible(false);

	cEditorEditModeStaticObjects* pEditMode = hplNew( cEditorEditModeStaticObjects, (apEditor,apEditorWorld));

	return pEditMode;
}

//---------------------------------------------------------------

cEditorEditModeEntities* cEditorEditModeFactory::CreateEditModeEntities(iEditorBase* apEditor,
																		iEditorWorld* apEditorWorld,
																		const cVector3f& avEditorWindowPos)
{
//	cEditorWindowEntities* pWindow = cEditorWindowFactory::CreateWindowEntities(apEditor,
//																				avEditorWindowPos,
//																				apEditor->GetFolderFullPath(eEditorDir_Entities));
//	pWindow->SetActive(false);

	cEditorEditModeEntities* pEditMode = hplNew( cEditorEditModeEntities,(apEditor, apEditorWorld));

	return pEditMode;
}

//---------------------------------------------------------------

cEditorEditModeSelect* cEditorEditModeFactory::CreateEditModeSelect(iEditorBase* apEditor,
																	iEditorWorld* apEditorWorld,
																	const cVector3f& avEditorWindowPos)
{
//	cEditorWindowSelect* pWindow = cEditorWindowFactory::CreateWindowSelect(apEditor,avEditorWindowPos);
//	pWindow->SetActive(false);

	cEditorEditModeSelect* pEditMode = hplNew( cEditorEditModeSelect, (apEditor, apEditorWorld));
	
	return pEditMode;
}


//---------------------------------------------------------------

cEditorEditModeLights* cEditorEditModeFactory::CreateEditModeLights(iEditorBase* apEditor,
																	iEditorWorld* apEditorWorld,
																	const cVector3f& avEditorWindowPos)
{
//	cEditorWindowLights* pWindow = cEditorWindowFactory::CreateWindowLights(apEditor, avEditorWindowPos);
//	pWindow->SetActive(false);

	cEditorEditModeLights* pEditMode = hplNew( cEditorEditModeLights, (apEditor,apEditorWorld));

//	pWindow->SetEditMode(pEditMode);

	return pEditMode;
}

//---------------------------------------------------------------

cEditorEditModeBodies* cEditorEditModeFactory::CreateEditModeBodies(iEditorBase* apEditor,
																	iEditorWorld* apEditorWorld,
																	const cVector3f& avEditorWindowPos)
{
//	cEditorWindowBodies* pWindow = cEditorWindowFactory::CreateWindowBodies(apEditor, avEditorWindowPos);
//	pWindow->SetActive(false);

	cEditorEditModeBodies* pEditMode = hplNew( cEditorEditModeBodies, (apEditor,apEditorWorld));

	return pEditMode;
}

//---------------------------------------------------------------

cEditorEditModeJoints* cEditorEditModeFactory::CreateEditModeJoints(iEditorBase* apEditor,
																	iEditorWorld* apEditorWorld,
																	const cVector3f& avEditorWindowPos)
{
//	cEditorWindowJoints* pWindow = cEditorWindowFactory::CreateWindowJoints(apEditor, avEditorWindowPos);
//	pWindow->SetActive(false);

	cEditorEditModeJoints* pEditMode = hplNew( cEditorEditModeJoints, (apEditor,apEditorWorld));

	return pEditMode;
}

//---------------------------------------------------------------

cEditorEditModeAreas* cEditorEditModeFactory::CreateEditModeAreas(iEditorBase* apEditor,
																  iEditorWorld* apEditorWorld,
																  const cVector3f& avEditorWindowPos)
{
//	cEditorWindowAreas* pWindow = cEditorWindowFactory::CreateWindowAreas(apEditor, avEditorWindowPos);
//	pWindow->SetActive(false);

	cEditorEditModeAreas* pEditMode = hplNew( cEditorEditModeAreas,(apEditor, apEditorWorld));

//	pWindow->SetEditMode(pEditMode);

	return pEditMode;
}


//---------------------------------------------------------------

cEditorEditModeBillboards* cEditorEditModeFactory::CreateEditModeBillboards(iEditorBase* apEditor, iEditorWorld* apEditorWorld, const cVector3f& avEditorWindowPos)
{
//	cEditorWindowBillboards* pWindow = cEditorWindowFactory::CreateWindowBillboards(apEditor, avEditorWindowPos);
//	pWindow->SetActive(false);

	cEditorEditModeBillboards* pEditMode = hplNew( cEditorEditModeBillboards,(apEditor, apEditorWorld));

	return pEditMode;
}

//---------------------------------------------------------------

cEditorEditModeSounds* cEditorEditModeFactory::CreateEditModeSounds(iEditorBase* apEditor, iEditorWorld* apEditorWorld, const cVector3f& avEditorWindowPos)
{
//	cEditorWindowSounds* pWindow = cEditorWindowFactory::CreateWindowSounds(apEditor, avEditorWindowPos);
//	pWindow->SetActive(false);

	cEditorEditModeSounds* pEditMode = hplNew( cEditorEditModeSounds,(apEditor, apEditorWorld));

	return pEditMode;
}

//---------------------------------------------------------------

cEditorEditModeParticleSystems* cEditorEditModeFactory::CreateEditModeParticleSystems(iEditorBase* apEditor,
																		 iEditorWorld* apEditorWorld,
																		 const cVector3f& avEditorWindowPos)
{
//	cEditorWindowParticleSystems* pWindow = cEditorWindowFactory::CreateWindowParticleSystems(apEditor, avEditorWindowPos);
//	pWindow->SetActive(false);

	cEditorEditModeParticleSystems* pEditMode = hplNew( cEditorEditModeParticleSystems,(apEditor, apEditorWorld));

	return pEditMode;
}

//---------------------------------------------------------------

cEditorEditModePrimitives* cEditorEditModeFactory::CreateEditModePrimitives(iEditorBase* apEditor,
																			iEditorWorld* apEditorWorld,
																			const cVector3f& avEditorWindowPos)
{
//	cEditorWindowPrimitives* pWindow = cEditorWindowFactory::CreateWindowPrimitives(apEditor, avEditorWindowPos);
//	pWindow->SetActive(false);

	cEditorEditModePrimitives* pEditMode = hplNew( cEditorEditModePrimitives,(apEditor, apEditorWorld));

	return pEditMode;
}

//---------------------------------------------------------------

void cEditorEditModeFactory::DestroyEditMode(iEditorEditMode* apEditMode)
{
	if(apEditMode==NULL)
		return;

	hplDelete(apEditMode);
}

//---------------------------------------------------------------


