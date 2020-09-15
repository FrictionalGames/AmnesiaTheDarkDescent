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

#ifndef ED_OBJ_ENTITY_H
#define ED_OBJ_ENTITY_H

//---------------------------------------------------------------

#include "../Common/StdAfx.h"
using namespace hpl;

//---------------------------------------------------------------

#include "../Common/EdScnObject.h"
#include "../Common/EdEngineObject.h"

//---------------------------------------------------------------

class cEdEntityLoader;

class iEdVar;

//---------------------------------------------------------------

#define EntityPropIdStart 40

enum eEntityInt
{
	eEntityInt_FileIndex = EntityPropIdStart,

	eEntityInt_LastEnum,
};

enum eEntityStr
{
	eEntityStr_Filename = EntityPropIdStart,

	eEntityStr_LastEnum,
};

//---------------------------------------------------------------

class cEngineLoadedMeshAggregate : public iEngineMesh
{
public:
	cEngineLoadedMeshAggregate(iEdScnObject* apParent, const tString& asFilename);
	~cEngineLoadedMeshAggregate();

	bool Create(const tString& asName);

	void Update();

	void SetLightsActive(bool abX) { mbLightsActive = abX; }
	void SetParticlesActive(bool abX) { mbParticleSystemsActive = abX; }
	void SetBillboardsActive(bool abX) { mbBillboardsActive = abX; }

protected:
	tString msFilename;

	bool mbLightsActive;
	bool mbParticleSystemsActive;
	bool mbBillboardsActive;

	tLightVec mvLights;
	std::vector<cParticleSystem*> mvParticleSystems;
	std::vector<cBillboard*> mvBillboards;
	std::vector<cSoundEntity*> mvSounds;
};

//---------------------------------------------------------------

class cTypeRootEntity : public iUserTypeRoot
{
public:
	cTypeRootEntity();
	~cTypeRootEntity();

	bool Init();

	iEdObjectType* GetTypeByXmlElement(cXmlElement*);

	cEdEntityLoader* GetEntityLoader() { return mpEntityLoader; }

	iEdObjectType* GetTypeFromEntFile(const tString& asFile);

	static cFileIndex* mpIndex;

protected:
	iUserTypeNode* CreateSpecificUserTypeNode(iEdClass*);

	iEdObjectData* CreateTypeSpecificData() { return NULL; }

	cEdEntityLoader* mpEntityLoader;
};

class cTypeEntity : public iUserTypeNode
{
public:
	cTypeEntity(iUserTypeRoot*, iEdClass*);

	iEdVar* GetAffectsShadowsVar();
	iEdVar* GetAffectsLightVar();
	iEdVar* GetAffectsParticlesVar();

	iEdVar* GetLinkedEditorSetupVar(const tWString&, eVariableType);

protected:
 	iEdObjectData* CreateTypeSpecificData();
};

//---------------------------------------------------------------

class cEdObjDataEntity : public iUserObjData
{
public:
	cEdObjDataEntity(iEdObjectType*);
	~cEdObjDataEntity();

	void CopyFromObject(iEdObject*);
	void CopyToObject(iEdObject*, ePropStep);

	bool LoadSpecificData(cXmlElement* apElement);

protected:
	iEdObject* CreateTypeSpecificObject();

	cIndexedFile* mpEntFile;
};

//---------------------------------------------------------------

class cEdObjEntity : public iUserObject
{
public:
	cEdObjEntity(iEdObjectData* apData);
	~cEdObjEntity();

	bool GetProperty(int, int&);
	bool GetProperty(int, tString&);

	bool SetProperty(int, const int);
	bool SetProperty(int, const tString&);

	cIndexedFile* GetEntFile() { return mpEntFile; }
	void SetEntFile(cIndexedFile*);

	void SetFilename(const tString& asFilename);

	bool GetTypeChanged() { return mbTypeChanged; }
	void SetTypeChanged(bool abX) { mbTypeChanged = abX; }

	iEdEditPane* CreateEditPane();

	bool IsAffectedByDecal(cEdSurfacePicker* apPicker);

	void UpdateEntity();

	//iEdAction* CreateSetStringAction(int alPropID, const tString& asX);
protected:
	iEngineObject* CreateEngineObject();

	void OnSetActive(bool abX);
	void OnSetVar(const tWString&, const tWString&);

	bool SetEntityType(iEdObjectType* apType);
	///////////////////////////
	// Data
	std::vector<iLight*> mvLights;
	std::vector<cMatrixf> mvLightLocalTransforms;

	bool mbAffectedByDecal;

	bool mbTypeChanged;

	cIndexedFile* mpEntFile;
};

//---------------------------------------------------------------

#endif // ED_OBJ_ENTITY_H
