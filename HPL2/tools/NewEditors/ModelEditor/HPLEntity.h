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

#ifndef ED_HPL_ENTITY_H
#define ED_HPL_ENTITY_H

//--------------------------------------------------------------------

#include "../Common/EdScnWorld.h"

//--------------------------------------------------------------------

class cHplEntity;

class cEdUserClassDefinition;
class cEdUserClass;
class cEdClassInstance;

//class cTypeSubMesh;
//class cTypeBone;
class cAnimationEventWrapper;
class cAnimationWrapper;

class cTypeBody;
class cTypeSubMesh;
class cTypeBone;

class cEdEntityLoader;
class cEdWindowPhysicsTest;

//--------------------------------------------------------------------

typedef std::vector<cAnimationEventWrapper> tAnimEventWrapperVec;
typedef tAnimEventWrapperVec::iterator tAnimEventWrapperVecIt;

typedef std::vector<cAnimationWrapper> tAnimWrapperVec;
typedef tAnimWrapperVec::iterator tAnimWrapperVecIt;

typedef std::list<cAnimationWrapper> tAnimWrapperList;
typedef tAnimWrapperList::iterator tAnimWrapperListIt;

typedef std::map<tWString, tWString> tVarValueMap;
typedef tVarValueMap::iterator		 tVarValueMapIt;

//--------------------------------------------------------------------

class cModelEditorSessionData : public iEdSessionData
{
public:
	cModelEditorSessionData();

	bool Load(cXmlElement*);
	bool Save(cXmlElement*);
};

//--------------------------------------------------------------------

class cCategoryDataHandlerMesh : public iEdScnObjCategoryDataHandler
{
public:
	cCategoryDataHandlerMesh(cHplEntity*);

	void Load(cXmlElement*);
	void Save(cXmlElement*);

	void SetBoneCategory(cEdScnObjCategory*);

protected:
	cHplEntity* mpEnt;
	cEdScnObjCategory* mpCatBone;
};

//--------------------------------------------------------------------

class cAnimationEventWrapper
{
public:
	cAnimationEventWrapper();

	int GetIndex() { return mlIndex; }

	void Load(cXmlElement* apElement);
	void Save(cXmlElement* apElement);

	void SetTime(float afX) { mfTime = afX; }
	float GetTime() { return mfTime; }

	void SetType(const tString& asX) { msType = asX; }
	const tString& GetType() { return msType; }

	void SetValue(const tString& asX) { msValue = asX; }
	const tString& GetValue() { return msValue; }

	bool IsValid();

protected:
	static int mlIndices;
	int mlIndex;
	float mfTime;
	tString msType;
	tString msValue;
};

//--------------------------------------------------------------------

class cAnimationWrapper
{
public:
	cAnimationWrapper();

	bool IsValid();

	void Load(cXmlElement* apElement);
	void Save(cXmlElement* apElement);

	void SetName(const tString& asName) { msName = asName; }
	const tString& GetName() { return msName; }

	void SetFile(const tString& asFile) { msFile = asFile; }
	const tString& GetFile() { return msFile; }

	void SetSpeed(float afX) { mfSpeed = afX; }
	float GetSpeed() { return mfSpeed; }

	void SetSpecialEventTime(float afX) { mfSpecialEventTime = afX; }
	float GetSpecialEventTime() { return mfSpecialEventTime; }

	tAnimEventWrapperVec& GetEvents() { return mvEvents; }
	cAnimationEventWrapper* GetEvent(int alIdx) { return &mvEvents[alIdx]; }
	int AddEvent();
	void RemoveEvent(int alIdx);
	void ClearEvents();

protected:
	tString msName;
	tString msFile;
	float mfSpeed;
	float mfSpecialEventTime;

	tAnimEventWrapperVec mvEvents;
};

//--------------------------------------------------------------------

class cAnimationCategoryDataHandler : public iEdScnObjCategoryDataHandler
{
public:
	void Load(cXmlElement*);
};

//--------------------------------------------------------------------

class cHplEntity : public iEdScnWorld
{
public:
	cHplEntity(iEditor*);
	~cHplEntity();

	cTypeSubMesh* GetTypeSubMesh() { return mpTypeSubMesh; }
	cTypeBone* GetTypeBone() { return mpTypeBone; }

	cEdUserClassDefinition* GetClassDefinition() { return mpClassDef; }

	void SetClass(cEdUserClass*);
	cEdClassInstance* GetClassInstance() { return mpClassInstance; }

	void SetAnimations(const tAnimWrapperVec& avAnims) { mvAnimations = avAnims; }
	const tAnimWrapperVec& GetAnimations() { return mvAnimations; }

	void SetMeshFromElement(cXmlElement* apMeshElement, cXmlElement* apBonesElement);

	iEdAction* CreateSetClassAction(cEdUserClass* apClass);
	iEdAction* CreateSetVariableValueAction(const tWString& asVarName, const tWString& asValue);

	iEdAction* CreateSetAnimationsAction(const tAnimWrapperVec& avAnimations);

	bool ToggleTestWindow();

	void CreateImportMeshDialog();

	iEdAction* CreateImportMeshAction(const tString&);

	cMeshEntity* CreateTestEntity(cWorld* apWorld, std::vector<iPhysicsJoint*>& avJoints);

protected:
	void OnInit();
	void OnReset();

	bool LoadContentsFromXmlDoc(iXmlDocument*);
	bool SaveContentsToXmlDoc(iXmlDocument*);

	bool ImportMesh(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ImportMesh);

	////////////////////////////////////////////////////////
	// Data
	cEdUserClassDefinition* mpClassDef;
	cEdClassInstance*		mpClassInstance;

	cTypeBody*		mpTypeBody;
	cTypeSubMesh*	mpTypeSubMesh;
	cTypeBone*		mpTypeBone;

	tAnimWrapperVec mvAnimations;

	cEdWindowPhysicsTest* mpActiveTestWindow;

	cEdEntityLoader* mpLoader;
};

//--------------------------------------------------------------------

#endif // ED_HPL_ENTITY_H
