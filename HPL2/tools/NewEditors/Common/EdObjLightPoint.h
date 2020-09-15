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

#ifndef ED_OBJ_LIGHT_POINT_H
#define ED_OBJ_LIGHT_POINT_H

//---------------------------------------------------------------

#include "../Common/EdObjLight.h"

//---------------------------------------------------------------

class cIconPointLight : public iIconObjectLight
{
public:
	cIconPointLight(iEdScnObject*);

	bool Create(const tString& asName);
};

//---------------------------------------------------------------------

class cTypePointLight : public iLightSubType
{
public:
	cTypePointLight(); 

	bool SetUpCreationData(iEdObjectData*);

protected:

	iEdObjectData* CreateTypeSpecificData();
};

//---------------------------------------------------------------------

class cEdObjPointLightData : public iEdObjLightData
{
public:
	cEdObjPointLightData(iEdObjectType*);

protected:
	iEdObject* CreateTypeSpecificObject();
};

//---------------------------------------------------------------------

class cEdObjPointLight : public iEdObjLight
{
public:
	cEdObjPointLight(iEdObjectData*);
	~cEdObjPointLight();

	void SetGobo(const tString& asGoboFilename);

	void DrawLightTypeSpecific(const cModuleDrawData&);

protected:
	iEngineObject* CreateEngineObject();
	//////////////////////
	// Data
};

//---------------------------------------------------------------------

#endif // ED_OBJ_LIGHT_POINT_H
