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

#ifndef ED_OBJ_LIGHT_BOX_H
#define ED_OBJ_LIGHT_BOX_H

//---------------------------------------------------------------------

#include "../Common/EdObjLight.h"

//---------------------------------------------------------------------

class cIconBoxLight : public iIconObjectLight
{
public:
	cIconBoxLight(iEdScnObject*);

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

class cTypeBoxLight : public iLightSubType
{
public:
	cTypeBoxLight();

	bool SetUpCreationData(iEdObjectData*);

protected:
	iEdObjectData* CreateTypeSpecificData();


};

//---------------------------------------------------------------------

class cEdObjBoxLightData : public iEdObjLightData
{
public:
	cEdObjBoxLightData(iEdObjectType*);

protected:
	iEdObject* CreateTypeSpecificObject();
};

//---------------------------------------------------------------------

class cEdObjBoxLight : public iEdObjLight
{
public:
	cEdObjBoxLight(iEdObjectData*);
	~cEdObjBoxLight();

	bool SetProperty(int, const int);
	bool SetProperty(int, const cVector3f&);

	bool GetProperty(int, int&);
	bool GetProperty(int, cVector3f&);

	void SetSize(const cVector3f& avSize);
	cVector3f& GetSize() { return mvSize; }

	void SetBlendFunc(eLightBoxBlendFunc aeFunc);
	eLightBoxBlendFunc GetBlendFunc() { return mBlendFunc; }

	void SaveToElement(cXmlElement* apElement);

protected:
	iEngineObject* CreateEngineObject();

	void DrawLightTypeSpecific(const cModuleDrawData&);

	//////////////////////
	// Data
	cVector3f mvSize;
	eLightBoxBlendFunc mBlendFunc;
};

//---------------------------------------------------------------------

#endif // ED_OBJ_LIGHT_BOX_H
