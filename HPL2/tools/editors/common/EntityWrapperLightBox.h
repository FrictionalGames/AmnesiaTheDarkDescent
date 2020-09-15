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

#ifndef HPLEDITOR_ENTITY_WRAPPER_LIGHT_BOX_H
#define HPLEDITOR_ENTITY_WRAPPER_LIGHT_BOX_H

#include "EntityWrapperLight.h"

//---------------------------------------------------------------------

class cIconEntityLightBox : public iIconEntityLight
{
public:
	cIconEntityLightBox(iEntityWrapper* apParent);

	bool Create(const tString&);
};

//---------------------------------------------------------------------

#define LightBoxPropIdStart 80

enum eLightBoxInt
{
	eLightBoxInt_BlendFunc = LightBoxPropIdStart,
	
	eLightBoxInt_LastEnum,
};

enum eLightBoxVec3f
{
	eLightBoxVec3f_Size = LightBoxPropIdStart,

	eLightBoxVec3f_LastEnum
};

//---------------------------------------------------------------------

class cEntityWrapperTypeLightBox : public iEntityWrapperTypeLight
{
public:
	cEntityWrapperTypeLightBox();

	iEntityWrapperData* CreateSpecificData();
};

//---------------------------------------------------------------------

class cEntityWrapperDataLightBox : public iEntityWrapperDataLight
{
public:
	cEntityWrapperDataLightBox(iEntityWrapperType*);

protected:
	iEntityWrapper* CreateSpecificEntity();
};

//---------------------------------------------------------------------

class cEntityWrapperLightBox : public iEntityWrapperLight
{
public:
	cEntityWrapperLightBox(iEntityWrapperData*);
	~cEntityWrapperLightBox();

	bool SetProperty(int, const int&);
	bool SetProperty(int, const cVector3f&);

	bool GetProperty(int, int&);
	bool GetProperty(int, cVector3f&);

	void SetSize(const cVector3f& avSize);
	cVector3f& GetSize() { return mvSize; }

	void SetBlendFunc(eLightBoxBlendFunc aeFunc);
	eLightBoxBlendFunc GetBlendFunc() { return mBlendFunc; }

	void SetAbsScale(const cVector3f& avScale, int alAxis=-1);

	void DrawLightTypeSpecific(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected);

	void SaveToElement(cXmlElement* apElement);

protected:
	iEngineEntity* CreateSpecificEngineEntity();

	//////////////////////
	// Data
	cVector3f mvSize;
	eLightBoxBlendFunc mBlendFunc;
};

//---------------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_LIGHT_BOX_H
