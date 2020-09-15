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

#ifndef HPLEDITOR_ENTITY_WRAPPER_PARTICLE_SYSTEM_H
#define HPLEDITOR_ENTITY_WRAPPER_PARTICLE_SYSTEM_H

#include "EntityWrapper.h"
#include "EngineEntity.h"

class cEditorWindowViewport;

//---------------------------------------------------------------

class cIconEntityPS : public iIconEntity
{
public:
	cIconEntityPS(iEntityWrapper* apParent);
	~cIconEntityPS();

	void Update();

private:
	bool ReCreatePS(const tString& asFile);
	void DestroyPS();
};

//---------------------------------------------------------------

#define ParticleSystemSPropIdStart 100

enum eParticleSystemStr
{
	eParticleSystemStr_File = ParticleSystemSPropIdStart,

	eParticleSystemStr_LastEnum,
};

enum eParticleSystemCol
{
	eParticleSystemCol_Color = ParticleSystemSPropIdStart,

	eParticleSystemCol_LastEnum,
};

enum eParticleSystemBool
{
	eParticleSystemBool_FadeAtDistance = ParticleSystemSPropIdStart,

	eParticleSystemBool_LastEnum,
};

enum eParticleSystemFloat
{
	eParticleSystemFloat_MinFadeDistanceStart = ParticleSystemSPropIdStart,
	eParticleSystemFloat_MaxFadeDistanceStart,
	eParticleSystemFloat_MinFadeDistanceEnd,
	eParticleSystemFloat_MaxFadeDistanceEnd,

	eParticleSystemFloat_LastEnum,
};

class cEntityWrapperTypeParticleSystem : public iEntityWrapperType
{
public:
	cEntityWrapperTypeParticleSystem();

protected:
	iEntityWrapperData* CreateSpecificData();
};

class cEntityWrapperDataParticleSystem : public iEntityWrapperData
{
public:
	cEntityWrapperDataParticleSystem(iEntityWrapperType*);

	iEntityWrapper* CreateSpecificEntity();
};

//---------------------------------------------------------------

class cEntityWrapperParticleSystem : public iEntityWrapper
{
	friend class cIconEntityPS;
public:
	cEntityWrapperParticleSystem(iEntityWrapperData*);
	virtual ~cEntityWrapperParticleSystem();

	bool SetProperty(int, const bool&);
	bool SetProperty(int, const float&);
	bool SetProperty(int, const tString&);
	bool SetProperty(int, const cColor&);

	bool GetProperty(int, bool&);
	bool GetProperty(int, float&);
	bool GetProperty(int, tString&);
	bool GetProperty(int, cColor&);

	bool EntitySpecificCheckCulled(cEditorClipPlane* apPlane);

	void OnSetCulled(bool abX);

	void SetVisible(bool abX);

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions,iEditorEditMode* apEditMode,
						bool abIsSelected, const cColor& aHighlightCol=cColor(1,1), const cColor& aDisabledCol=cColor(0.5f,1));

	//void SaveToElement(cXmlElement* apElement);

	cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);

	void SetFile(const tString& asFile);
	const tString& GetFile() { return msFile; }

	void SetColor(const cColor& aCol);
	void SetFadeAtDistance(bool abX);
	void SetMinFadeDistanceStart(float afX);
	void SetMinFadeDistanceEnd(float afX);
	void SetMaxFadeDistanceStart(float afX);
	void SetMaxFadeDistanceEnd(float afX);

	const cColor& GetColor() { return mColor; }
	bool GetFadeAtDistance() { return mbFadeAtDistance; }
	float GetMinFadeDistanceStart() { return mfMinFadeDistanceStart; }
	float GetMinFadeDistanceEnd() { return mfMinFadeDistanceEnd; }
	float GetMaxFadeDistanceStart() { return mfMaxFadeDistanceStart; }
	float GetMaxFadeDistanceEnd() { return mfMaxFadeDistanceEnd; }

	void UpdatePS();
protected:
	iEngineEntity* CreateSpecificEngineEntity();
	//////////////////////
	// Data
	bool mbTypeUpdated;
	bool mbDataUpdated;
	
	tString msFile;

	cColor mColor; 
	bool mbFadeAtDistance;

	float mfMinFadeDistanceStart;
	float mfMaxFadeDistanceStart;
	float mfMinFadeDistanceEnd; 
	float mfMaxFadeDistanceEnd;
};

//---------------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_PARTICLE_SYSTEM_H
