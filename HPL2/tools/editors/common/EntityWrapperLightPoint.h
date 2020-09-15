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

#ifndef HPLEDITOR_ENTITY_WRAPPER_LIGHT_POINT_H
#define HPLEDITOR_ENTITY_WRAPPER_LIGHT_POINT_H

#include "EntityWrapperLight.h"

//---------------------------------------------------------------

class cIconEntityLightPoint : public iIconEntityLight
{
public:
	cIconEntityLightPoint(iEntityWrapper* apParent);

	bool Create(const tString& asName);
};

//---------------------------------------------------------------------

class cEntityWrapperTypeLightPoint : public iEntityWrapperTypeLight
{
public:
	cEntityWrapperTypeLightPoint(); 

	iEntityWrapperData* CreateSpecificData();
};

//---------------------------------------------------------------------

class cEntityWrapperDataLightPoint : public iEntityWrapperDataLight
{
public:
	cEntityWrapperDataLightPoint(iEntityWrapperType*);

protected:
	iEntityWrapper* CreateSpecificEntity();
};

//---------------------------------------------------------------------

class cEntityWrapperLightPoint : public iEntityWrapperLight
{
public:
	cEntityWrapperLightPoint(iEntityWrapperData*);
	~cEntityWrapperLightPoint();

	void SetGobo(const tString& asGoboFilename);

	void DrawLightTypeSpecific(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected);

protected:
	iEngineEntity* CreateSpecificEngineEntity();
	//////////////////////
	// Data
};

//---------------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_LIGHT_POINT_H
