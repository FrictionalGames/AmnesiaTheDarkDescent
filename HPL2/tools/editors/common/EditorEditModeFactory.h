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

#ifndef HPLEDITOR_EDITOR_EDIT_MODE_FACTORY_H
#define HPLEDITOR_EDITOR_EDIT_MODE_FACTORY_H


#include "../common/StdAfx.h"

using namespace hpl;

//---------------------------------------------------------------

class iEditorBase;
class iEditorEditMode;

class iEditorWorld;

class cEditorEditModeDummy;
class cEditorEditModeStaticObjects;
class cEditorEditModeEntities;
class cEditorEditModeSelect;
class cEditorEditModeLights;
class cEditorEditModeBodies;
class cEditorEditModeJoints;
class cEditorEditModeAreas;
class cEditorEditModeBillboards;
class cEditorEditModeSounds;
class cEditorEditModeParticleSystems;
class cEditorEditModePrimitives;

//---------------------------------------------------------------

class cEditorEditModeFactory
{
public:
	static cEditorEditModeDummy* CreateEditModeDummy(iEditorBase* apEditor);

	static cEditorEditModeStaticObjects* CreateEditModeStaticObjects(iEditorBase* apEditor, 
																	 iEditorWorld* apEditorWorld,
																	 const cVector3f& avEditorWindowPos);

	static cEditorEditModeEntities* CreateEditModeEntities(iEditorBase* apEditor,
														   iEditorWorld* apEditorWorld,
														   const cVector3f& avEditorWindowPos);
	
	static cEditorEditModeSelect* CreateEditModeSelect(iEditorBase* apEditor, 
													   iEditorWorld* apEditorWorld,
													   const cVector3f& avEditorWindowPos);

	static cEditorEditModeLights* CreateEditModeLights(iEditorBase* apEditor,
													   iEditorWorld* apEditorWorld,
													   const cVector3f& avEditorWindowPos);

	static cEditorEditModeBodies* CreateEditModeBodies(iEditorBase* apEditor,
														iEditorWorld* apEditorWorld,
														const cVector3f& avEditorWindowPos);

	static cEditorEditModeJoints* CreateEditModeJoints(iEditorBase* apEditor,
													   iEditorWorld* apEditorWorld,
													   const cVector3f& avEditorWindowPos);

	static cEditorEditModeAreas* CreateEditModeAreas(iEditorBase* apEditor,
													 iEditorWorld* apEditorWorld,
													 const cVector3f& avEditorWindowPos);

	static cEditorEditModeBillboards* CreateEditModeBillboards(iEditorBase* apEditor,
															   iEditorWorld* apEditorWorld,
															   const cVector3f& avEditorWindowPos);

	static cEditorEditModeSounds* CreateEditModeSounds(iEditorBase* apEditor,
													   iEditorWorld* apEditorWorld,
													   const cVector3f& avEditorWindowPos);

	static cEditorEditModeParticleSystems* CreateEditModeParticleSystems(iEditorBase* apEditor,
																		 iEditorWorld* apEditorWorld,
																		 const cVector3f& avEditorWindowPos);

	static cEditorEditModePrimitives* CreateEditModePrimitives(iEditorBase* apEditor,
															   iEditorWorld* apEditorWorld,
															   const cVector3f& avEditorWindowPos);

	static void DestroyEditMode(iEditorEditMode* apEditMode);

protected:
};

//---------------------------------------------------------------

#endif //HPLEDITOR_EDITOR_EDIT_MODE_FACTORY_H
