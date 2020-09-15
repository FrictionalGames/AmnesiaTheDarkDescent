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

#ifndef ED_OBJ_FOG_AREA_H
#define ED_OBJ_FOG_AREA_H

#include "../Common/EdScnObject.h"
#include "../Common/EdEngineObject.h"

//---------------------------------------------------------------

class cIconObjectFogArea : public iIconObject
{
public:
	cIconObjectFogArea(iEdScnObject* apParent);
	~cIconObjectFogArea();

	bool Create(const tString& asName);
};

//---------------------------------------------------------------

#define FogAreaPropIdStart 50

enum eFogAreaBool
{
	eFogAreaBool_ShownBacksideWhenInside = FogAreaPropIdStart,
	eFogAreaBool_ShownBacksideWhenOutside,

	eFogAreaBool_LastEnum,
};

enum eFogAreaCol
{
	eFogAreaCol_Color = FogAreaPropIdStart,
	
	eFogAreaColor_LastEnum,
};

enum eFogAreaFloat
{
	eFogAreaFloat_Start = FogAreaPropIdStart,
	eFogAreaFloat_End, 
	eFogAreaFloat_FalloffExp,
	
	eFogAreaFloat_LastEnum,	
};

//---------------------------------------------------------------

class cTypeFogArea : public iEdScnObjType
{
public:
	cTypeFogArea();

protected:
	iEdObjectData* CreateTypeSpecificData();
};

//---------------------------------------------------------------

class cEdObjFogAreaData : public iEdScnObjData
{
public:
	cEdObjFogAreaData(iEdObjectType*);

protected:
	iEdObject* CreateTypeSpecificObject();
};

//---------------------------------------------------------------

class cEdObjFogArea : public iEdScnObject
{
public:
	cEdObjFogArea(iEdObjectData*);
	~cEdObjFogArea();

	bool SetProperty(int, const float);
	bool SetProperty(int, const bool);
	bool SetProperty(int, const cColor&);

	bool GetProperty(int, float&);
	bool GetProperty(int, bool&);
	bool GetProperty(int, cColor&);

	void SetColor(const cColor& aCol);
	const cColor& GetColor() { return mColor; }

	void SetStart(float afX);
	float GetStart() { return mfStart; }

	void SetEnd(float afX);
	float GetEnd() { return mfEnd; }

	void SetFalloffExp(float afX);
	float GetFalloffExp() { return mfFalloffExp; }

	void SetShownBacksideWhenOutside(bool abX);
	bool GetShownBacksideWhenOutside() { return mbShownBacksideWhenOutside; }

	void SetShownBacksideWhenInside(bool abX);
	bool GetShownBacksideWhenInside() { return mbShownBacksideWhenInside; }


	iEdEditPane* CreateEditPane();
	void OnDraw(const cModuleDrawData&);

protected:
	iEngineObject* CreateEngineObject();

	cColor mColor;
	float mfStart;
	float mfEnd;
	float mfFalloffExp;

	bool mbShownBacksideWhenOutside;
	bool mbShownBacksideWhenInside;
};

//---------------------------------------------------------------

#endif // ED_OBJ_FOG_AREA_H
