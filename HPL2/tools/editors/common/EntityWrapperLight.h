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

#ifndef HPLEDITOR_ENTITY_WRAPPER_LIGHT_H
#define HPLEDITOR_ENTITY_WRAPPER_LIGHT_H

#include "EntityWrapper.h"
#include "EngineEntity.h"

class cEditorWindowViewport;

class cEntityWrapperBillboard;

//---------------------------------------------------------------------

class iIconEntityLight : public iIconEntity
{
public:
	iIconEntityLight(iEntityWrapper* apParent, const tString& asIconFile);
	~iIconEntityLight();

	void Update();
};

//---------------------------------------------------------------------

#define LightPropIdStart 60

//////////////////////////////////////////////
// General Light properties
enum eLightInt
{
	//eLightInt_GoboAnimMode = eObjInt_LastEnum,

	eLightInt_LastEnum = LightPropIdStart,
};


enum eLightBool
{
	eLightBool_FlickerActive = LightPropIdStart,
	eLightBool_FlickerFade,
	eLightBool_CastShadows,
	eLightBool_ShadowsAffectStatic,
	eLightBool_ShadowsAffectDynamic,

	eLightBool_LastEnum,
};

enum eLightCol
{
	eLightCol_Diffuse = LightPropIdStart,
	eLightCol_FlickerOff,

	eLightCol_LastEnum,
};

enum eLightFloat
{
	eLightFloat_Radius = LightPropIdStart,

	eLightFloat_GoboAnimFrameTime, 
	eLightFloat_FlickerOnMinLength,
	eLightFloat_FlickerOnMaxLength,
	eLightFloat_FlickerOffMinLength,
	eLightFloat_FlickerOffMaxLength,
	eLightFloat_FlickerOffRadius,
	eLightFloat_FlickerOnFadeMinLength,
	eLightFloat_FlickerOnFadeMaxLength,
	eLightFloat_FlickerOffFadeMinLength,
	eLightFloat_FlickerOffFadeMaxLength,
	
	eLightFloat_LastEnum,	
};

enum eLightStr
{
	eLightStr_Gobo = LightPropIdStart,
	eLightStr_GoboAnimMode,
	eLightStr_FalloffMap,
	eLightStr_FlickerOnSound,
	eLightStr_FlickerOffSound,
	eLightStr_FlickerOnPS,
	eLightStr_FlickerOffPS,
	eLightStr_ShadowResolution,

	eLightStr_LastEnum,
};

//---------------------------------------------------------------------

class iEntityWrapperDataLight : public iEntityWrapperData
{
public:
	iEntityWrapperDataLight(iEntityWrapperType* apType);

	void CopyFromEntity(iEntityWrapper* apEntity);
	void CopyToEntity(iEntityWrapper* apEntity, int alCopyFlags);

	const tIntList& GetConnectedBBIDS();
protected:
	tIntList mlstConnectedBBIds;

};

class iEntityWrapperTypeLight : public iEntityWrapperType
{
	friend class iIconEntityLight;
public:
	iEntityWrapperTypeLight(const tString& asElementString, int alSubType) : iEntityWrapperType(eEditorEntityType_Light, _W("Light"), asElementString),
																				mlSubType(alSubType)
	{
		AddBool(eLightBool_CastShadows, "CastShadows", false);
		AddString(eLightStr_ShadowResolution, "ShadowResolution", "High");
		AddBool(eLightBool_ShadowsAffectStatic, "ShadowsAffectStatic");
		AddBool(eLightBool_ShadowsAffectDynamic, "ShadowsAffectDynamic");

		AddFloat(eLightFloat_Radius, "Radius", 1.0f);
		AddString(eLightStr_FalloffMap, "FalloffMap");
		AddString(eLightStr_Gobo, "Gobo");
		AddString(eLightStr_GoboAnimMode, "GoboAnimMode", "None");
		AddFloat(eLightFloat_GoboAnimFrameTime, "GoboAnimFrameTime");
		AddColor(eLightCol_Diffuse, "DiffuseColor", cColor(1));

		AddBool(eLightBool_FlickerActive, "FlickerActive", false);
		AddFloat(eLightFloat_FlickerOnMinLength, "FlickerOnMinLength");
		AddFloat(eLightFloat_FlickerOnMaxLength, "FlickerOnMaxLength");
		AddString(eLightStr_FlickerOnPS, "FlickerOnPS");
		AddString(eLightStr_FlickerOnSound, "FlickerOnSound");

		AddFloat(eLightFloat_FlickerOffMinLength, "FlickerOffMinLength");
		AddFloat(eLightFloat_FlickerOffMaxLength, "FlickerOffMaxLength");
		AddString(eLightStr_FlickerOffPS, "FlickerOffPS");
		AddString(eLightStr_FlickerOffSound, "FlickerOffSound");
		AddColor(eLightCol_FlickerOff, "FlickerOffColor", cColor(0));
		AddFloat(eLightFloat_FlickerOffRadius, "FlickerOffRadius");

		AddBool(eLightBool_FlickerFade, "FlickerFade", false);
		AddFloat(eLightFloat_FlickerOnFadeMinLength, "FlickerOnFadeMinLength");
		AddFloat(eLightFloat_FlickerOnFadeMaxLength, "FlickerOnFadeMaxLength");
		AddFloat(eLightFloat_FlickerOffFadeMinLength, "FlickerOffFadeMinLength");
		AddFloat(eLightFloat_FlickerOffFadeMaxLength, "FlickerOffFadeMaxLength");
	}

	int GetLightType() { return mlSubType; }

	bool IsVisible() { return mbLightsVisible; }
	void SetVisible(bool abX);

	bool IsActive() { return mbLightsActive; }
	void SetActive(bool abX);
	

protected:

	int mlSubType;

	static bool mbLightsVisible;
	static bool mbLightsActive;
};

//---------------------------------------------------------------------

class iEntityWrapperLight : public iEntityWrapper
{
public:
	iEntityWrapperLight(iEntityWrapperData* apData);
	virtual ~iEntityWrapperLight();

	bool GetProperty(int, int&);
	bool GetProperty(int, float&);
	bool GetProperty(int, bool&);
	bool GetProperty(int, tString&);
	bool GetProperty(int, cColor&);

	bool SetProperty(int, const int&);
	bool SetProperty(int, const float&);
	bool SetProperty(int, const bool&);
	bool SetProperty(int, const tString&);
	bool SetProperty(int, const cColor&);

	int GetLightType() { return ((iEntityWrapperTypeLight*)mpType)->GetLightType(); }

	bool EntitySpecificCheckCulled(cEditorClipPlane* apPlane);

	bool GetCastShadows() { return mbCastShadows; }
	void SetCastShadows(bool abX);

	const tString& GetShadowResolution() { return msShadowResolution; }
	void SetShadowResolution(const tString& asX) { msShadowResolution = asX; }

	bool GetShadowsAffectStatic() { return mbShadowsAffectStatic; }
	void SetShadowsAffectStatic(bool abX);

	bool GetShadowsAffectDynamic() { return mbShadowsAffectDynamic; }
	void SetShadowsAffectDynamic(bool abX);

	virtual void SetGobo(const tString& asGoboFilename);
	const tString& GetGoboFilename() { return msGoboFilename; }

	const tString& GetGoboAnimMode() { return msGoboAnimMode; }
	void SetGoboAnimMode(const tString& asX);

	float GetGoboAnimFrameTime() { return mfGoboAnimFrameTime; }
	void SetGoboAnimFrameTime(float afX);

	virtual void SetRadius(float afRadius);
	float GetRadius() { return mfRadius; }

	void SetFalloffMap(const tString& asFalloffMap);
	const tString& GetFalloffMap() { return msFalloffMap; }
	
	void SetDiffuseColor(const cColor& aDiffuseColor);
	cColor GetDiffuseColor() { return mcolDiffuseColor; }


	//////////////////////////////////////////////////////
	// Flicker stuff
	bool GetFlickerActive() { return mbFlickerActive; }

	float GetFlickerOnMinLength() { return mfFlickerOnMinLength; }
	float GetFlickerOnMaxLength() { return mfFlickerOnMaxLength; }
	const tString& GetFlickerOnSound() { return msFlickerOnSound; }
	const tString& GetFlickerOnPS() { return msFlickerOnPS; }
	
	float GetFlickerOffMinLength() { return mfFlickerOffMinLength; }
	float GetFlickerOffMaxLength() { return mfFlickerOffMaxLength; }
	float GetFlickerOffRadius() { return mfFlickerOffRadius; }
	cColor GetFlickerOffColor() { return mcolFlickerOffColor; }
	const tString& GetFlickerOffSound() { return msFlickerOffSound; }
	const tString& GetFlickerOffPS() { return msFlickerOffPS; }
	
	bool GetFlickerFade() { return mbFlickerFade; }

	float GetFlickerOnFadeMinLength() { return mfFlickerOnFadeMinLength; }
	float GetFlickerOnFadeMaxLength() { return mfFlickerOnFadeMaxLength; }
	float GetFlickerOffFadeMinLength() { return mfFlickerOffFadeMinLength; }
	float GetFlickerOffFadeMaxLength() { return mfFlickerOffFadeMaxLength; }


	void SetFlickerActive(bool abX);

	void SetFlickerOnMinLength(float afX);
	void SetFlickerOnMaxLength(float afX);
	void SetFlickerOnSound(const tString& asStr);
	void SetFlickerOnPS(const tString& asStr);


	void SetFlickerOffMinLength(float afX);
	void SetFlickerOffMaxLength(float afX);
	void SetFlickerOffSound(const tString& asStr);
	void SetFlickerOffPS(const tString& asStr);
	void SetFlickerOffRadius(float afX);
	void SetFlickerOffColor(const cColor& aCol);

	void SetFlickerFade(bool abX);
	
	void SetFlickerOnFadeMinLength(float afX);
	void SetFlickerOnFadeMaxLength(float afX);
	void SetFlickerOffFadeMinLength(float afX);
	void SetFlickerOffFadeMaxLength(float afX);

	void UpdateFlickerParams();

	//////////////////////////////////////////////////
	// Billboard connection
	void AddConnectedBillboard(cEntityWrapperBillboard* apBB);
	void RemoveConnectedBillboard(cEntityWrapperBillboard* apBB);
	std::list<cEntityWrapperBillboard*>& GetConnectedBillboards() { return mlstConnectedBBs; }
	

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol);

	void SaveToElement(cXmlElement* apElement);

	cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);

protected:
	void OnSetVisible(bool abX) {}
	void OnSetCulled(bool abX) {}
	void OnSetActive(bool abX);
	///////////////////////////
	// To be implemented
	virtual void DrawLightTypeSpecific(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelectedc)=0;
	

	//////////////////////
	// Data

	bool mbCastShadows;
	tString msShadowResolution;
	bool mbShadowsAffectStatic;
	bool mbShadowsAffectDynamic;

	tString msGoboFilename;
	tString msGoboAnimMode;
	float mfGoboAnimFrameTime;

	float mfRadius;

	cColor mcolDiffuseColor;

	tString msFalloffMap;

	//////////////////////
	// Flicker stuff
	bool mbFlickerUpdated;
	bool mbFlickerActive;

	float mfFlickerOnMinLength;
	float mfFlickerOnMaxLength;
	tString msFlickerOnSound;
	tString msFlickerOnPS;

	float mfFlickerOffMinLength;
	float mfFlickerOffMaxLength;
	tString msFlickerOffSound;
	tString msFlickerOffPS;
	float mfFlickerOffRadius;
	cColor mcolFlickerOffColor;

	bool mbFlickerFade;
	
	float mfFlickerOnFadeMinLength;
	float mfFlickerOnFadeMaxLength;
	float mfFlickerOffFadeMinLength;
	float mfFlickerOffFadeMaxLength;

	std::list<cEntityWrapperBillboard*> mlstConnectedBBs;
};

//---------------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_STATIC_OBJECT_H
