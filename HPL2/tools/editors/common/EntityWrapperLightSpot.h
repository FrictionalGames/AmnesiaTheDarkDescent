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

#ifndef HPLEDITOR_ENTITY_WRAPPER_LIGHT_SPOT_H
#define HPLEDITOR_ENTITY_WRAPPER_LIGHT_SPOT_H

#include "EntityWrapperLight.h"


class cEditorWindowViewport;

//---------------------------------------------------------------------------

class cIconEntityLightSpot : public iIconEntityLight
{
public:
	cIconEntityLightSpot(iEntityWrapper* apParent);

	bool Create(const tString& asName);
};

//---------------------------------------------------------------------------

#define LightSpotPropIdStart 90

//////////////////////////////////////
// Specific Spotlight properties
enum eLightSpotFloat
{
	eLightSpotFloat_FOV = LightSpotPropIdStart,
	eLightSpotFloat_Aspect,
	eLightSpotFloat_NearClipPlane,
	
	eLightSpotFloat_LastEnum,	
};

enum eLightSpotStr
{
	eLightSpotStr_FalloffMap = LightSpotPropIdStart,
	
	eLightSpotString_LastEnum,
};

//---------------------------------------------------------------

class cEntityWrapperTypeLightSpot : public iEntityWrapperTypeLight
{
public:
	cEntityWrapperTypeLightSpot();

protected:
	iEntityWrapperData* CreateSpecificData();
};

//---------------------------------------------------------------

class cEntityWrapperDataLightSpot : public iEntityWrapperDataLight
{
public:
	cEntityWrapperDataLightSpot(iEntityWrapperType*);

protected:
	iEntityWrapper* CreateSpecificEntity();
};

//---------------------------------------------------------------

class cEntityWrapperLightSpot : public iEntityWrapperLight
{
public:
	cEntityWrapperLightSpot(iEntityWrapperData*);
	~cEntityWrapperLightSpot();

	bool SetProperty(int, const float&);
	bool SetProperty(int, const tString&);
	bool GetProperty(int, float&);
	bool GetProperty(int, tString&);

	void SetFOV(float afAngle);
	inline float GetFOV() const { return mfFOV;}

	void SetAspect(float afAngle);
	float GetAspect() { return mfAspect;}

	void SetNearClipPlane(float afX);
	float GetNearClipPlane() { return mfNearClipPlane;}
	
	void SetRadius(float afX);
		
	tString& GetSpotFalloffMap() { return msSpotFalloffMap; }
	void SetSpotFalloffMap(const tString& asFalloffMap);

	// iEntityWrapperLight Implementation

	void DrawLightTypeSpecific(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected);

protected:
	iEngineEntity* CreateSpecificEngineEntity();
	//////////////////////
	// Data
	tString msSpotFalloffMap;

	float mfFOV;
	float mfAspect;
	float mfNearClipPlane;
};

//---------------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_LIGHT_SPOT_H
