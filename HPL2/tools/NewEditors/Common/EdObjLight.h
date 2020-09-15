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

#ifndef ED_OBJ_LIGHT_H
#define ED_OBJ_LIGHT_H

#include "../Common/EdScnObject.h"
#include "../Common/EdEngineObject.h"

//---------------------------------------------------------------------

class cEdWindowViewport;

class cEdObjBillboard;

class iShapeCreator;

//---------------------------------------------------------------------

class iIconObjectLight : public iIconObject
{
public:
	iIconObjectLight(iEdScnObject*, const tString&);
	~iIconObjectLight();

	void SetUpLight(iLight*);

	void Update();
};

//---------------------------------------------------------------------

#define LightPropIdStart 60

//////////////////////////////////////////////
// General Light properties
enum eLightInt
{
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

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// LIGHT TYPE
/////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class cTypeLight : public iEdScnObjType
{
	friend class iIconObjectLight;
public:
	cTypeLight();

	bool Init();

protected:
	iEdObjectData* CreateTypeSpecificData() { return NULL; }

};

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// LIGHT SUBTYPE
/////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class iLightSubType : public iEdScnObjType
{
public:
	iLightSubType(const tWString&, const tString&, iShapeCreator*);
	virtual ~iLightSubType();

	iShapeCreator* GetShapeCreator() { return mpShapeCreator; }

	virtual bool SetUpCreationData(iEdObjectData*)=0;

protected:
	iShapeCreator* mpShapeCreator;
};

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// LIGHT DATA
/////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class iEdObjLightData : public iEdScnObjData
{
public:
	iEdObjLightData(iEdObjectType*);

	void CopyFromObject(iEdObject*);
	void CopyToObject(iEdObject*,ePropStep);

	const tIntList& GetConnectedBBIDS();
protected:
	tIntList mlstConnectedBBIds;

};

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// LIGHT OBJECT
/////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class iEdObjLight : public iEdScnObject
{
public:
	iEdObjLight(iEdObjectData* apData);
	virtual ~iEdObjLight();

	virtual bool GetProperty(int, int&);
	virtual bool GetProperty(int, float&);
	virtual bool GetProperty(int, bool&);
	virtual bool GetProperty(int, tString&);
	virtual bool GetProperty(int, cColor&);

	virtual bool SetProperty(int, const int);
	virtual bool SetProperty(int, const float);
	virtual bool SetProperty(int, const bool);
	virtual bool SetProperty(int, const tString&);
	virtual bool SetProperty(int, const cColor&);

	//bool EntitySpecificCheckCulled(cEdClipPlane* apPlane);

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
	void AddConnectedBillboard(cEdObjBillboard* apBB);
	void RemoveConnectedBillboard(cEdObjBillboard* apBB);
	std::list<cEdObjBillboard*>& GetConnectedBillboards() { return mlstConnectedBBs; }

	void SaveToElement(cXmlElement* apElement);

	iEdEditPane* CreateEditPane();

protected:
	void OnDraw(const cModuleDrawData&);

	void OnSetVisible(bool abX) {}
	void OnSetCulled(bool abX) {}
	void OnSetActive(bool abX);

	bool OnUpdate(bool);
	///////////////////////////
	// To be implemented
	virtual void DrawLightTypeSpecific(const cModuleDrawData&)=0;
	

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

	std::list<cEdObjBillboard*> mlstPendingConnectedBBs;
	std::list<cEdObjBillboard*> mlstConnectedBBs;
};

//---------------------------------------------------------------------------

#endif // ED_OBJ_LIGHT_H
