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

#ifndef HPLEDITOR_USER_CLASS_DEFINITION_MANAGER_H
#define HPLEDITOR_USER_CLASS_DEFINITION_MANAGER_H

//------------------------------------------------------------------

#include "EditorVar.h"

//------------------------------------------------------------------

enum eEditorVarCategory
{
	eEditorVarCategory_EditorSetup	= 0x00000001,
	eEditorVarCategory_Type			= 0x00000002,
	eEditorVarCategory_Instance		= 0x00000004,
	
	eEditorVarCategory_LastEnum		= 3,
};

//------------------------------------------------------------------

class iEditorVar;
class cEditorUserClass;
class cEditorUserClassType;
class cEditorUserClassSubType;
class cEditorUserClassInstance;
class cEditorUserClassDefinition;
class cEditorUserClassDefinitionManager;

typedef std::map<tString, tEditorVarVec> tVarGroupMap;

//------------------------------------------------------------------

class cEditorVarGroup
{
public:
	cEditorVarGroup(const tString& asName);

	const tString& GetName() { return msName; }
private:
	tString msName;
};

typedef std::vector<cEditorVarGroup*> tVarGroupVec;

//------------------------------------------------------------------

class cEditorUserClass : public iEditorClass
{
public:
	cEditorUserClass(cEditorUserClassDefinition* apDefinition);
	virtual ~cEditorUserClass();

	cEditorUserClassDefinition* GetDefinition() { return mpDefinition; }

	bool Create(void* apData);

	iEditorVar* CreateClassSpecificVariableFromElement(cXmlElement* apElement);

	virtual iEditorVar* GetVariable(eEditorVarCategory, const tWString&);

	void SetIndex(int alX) { mlIndex = alX; }
	int GetIndex() { return mlIndex; }

	virtual void AddVariablesToInstance(eEditorVarCategory aCat, cEditorUserClassInstance* apInstance);

	virtual tEditorVarVec GetEditorSetupVars();
	virtual void DumpEditorSetupVars(tEditorVarVec&);

protected:
	int mlIndex;
	cEditorUserClassDefinition* mpDefinition;
	std::vector<tEditorVarVec> mvVariables;
};

//------------------------------------------------------------------

class cEditorUserClassBase : public cEditorUserClass
{
public:
	cEditorUserClassBase(cEditorUserClassDefinition* apDefinition);

protected:
};

//------------------------------------------------------------------



//------------------------------------------------------------------

class cEditorUserClassType : public cEditorUserClass
{
public:
	cEditorUserClassType(cEditorUserClassDefinition* apDefinition);
	~cEditorUserClassType();

	bool Create(void* apData);

	void AddVariablesToInstance(eEditorVarCategory aCat, cEditorUserClassInstance* apInstance);
	void DumpEditorSetupVars(tEditorVarVec&);

	iEditorVar* GetVariable(eEditorVarCategory, const tWString&);

	int GetSubTypeNum();
	cEditorUserClassSubType* GetSubType(int alX);
	cEditorUserClassSubType* GetSubType(const tString& asName);

	int GetDefaultSubTypeIndex() { return mlDefaultSubType; }
protected:

	iEditorClass* mpBaseClass;
	tEditorClassVec mvSubTypes;
	int mlDefaultSubType;
};

//------------------------------------------------------------------

class cEditorUserClassSubType : public cEditorUserClass
{
public:
	cEditorUserClassSubType(cEditorUserClassType* apParent);
	cEditorUserClassType* GetParent() { return mpParent; }

	cEditorClassInstance* CreateInstance(eEditorVarCategory aCat);
	void DumpEditorSetupVars(tEditorVarVec&);

	iEditorVar* GetVariable(eEditorVarCategory, const tWString&);

	bool IsDefaultSubType();

protected:

	cEditorUserClassType* mpParent;
};

//------------------------------------------------------------------

class cEditorUserClassInstance : public cEditorClassInstance
{
public:
	cEditorUserClassInstance(cEditorUserClassSubType* apClass, eEditorVarCategory aCat);
	eEditorVarCategory GetCategory() { return mCat; }

	cEditorClassInstance* CreateSpecificCopy();

	cEditorVarInputPanel* CreateInputPanel(iEditorWindow* apWindow, iWidget* apParent, bool abX);
protected:
	eEditorVarCategory mCat;
};

//------------------------------------------------------------------

class cEditorUserClassInputPanel : public cEditorVarInputPanel
{
public:
	cEditorUserClassInputPanel(cEditorUserClassInstance* apInstance);

	void Create(iEditorWindow* apWindow, iWidget* apWidget);
protected:
};

//------------------------------------------------------------------

class cEditorUserClassDefinition
{
public:
	cEditorUserClassDefinition(cEditorUserClassDefinitionManager* apManager);
	~cEditorUserClassDefinition();

	cEditorUserClassDefinitionManager* GetManager() { return mpManager; }

	bool Create(const tString& asFilename, int alLoadFlags);
	int GetLoadFlags() { return mlLoadFlags; }

	bool IsLoadableCategory(eEditorVarCategory aCat);

	cEditorUserClassBase* GetBaseClass(const tString& asName);

	int GetTypeNum();
	cEditorUserClassType* GetType(int);
	cEditorUserClassType* GetType(const tString& asName);

	int GetGroupNum();
	cEditorVarGroup* GetGroup(int alIdx);
	cEditorVarGroup* GetGroup(const tString& asName);
protected:
	int mlLoadFlags;

	cEditorUserClassDefinitionManager* mpManager;
	tEditorClassVec mvBaseClasses;
	tEditorClassVec mvTypes;

	std::vector<cEditorVarGroup*> mvGroups;
};

//------------------------------------------------------------------

typedef std::vector<cEditorUserClassDefinition*> tUserClassDefVec;

//------------------------------------------------------------------

class cEditorUserClassDefinitionManager
{
public:
	cEditorUserClassDefinitionManager(iEditorBase* apEditor);
	~cEditorUserClassDefinitionManager();

	iEditorBase* GetEditor() { return mpEditor; }

	void RegisterDefFilename(int alIndex, const tString& asName, int alLoadFlags);

	void Init();

	cEditorUserClassDefinition* GetDefinition(int alX);

	const tString& GetVarCategoryName(eEditorVarCategory aCat);

protected:
	iEditorBase* mpEditor;

	tStringVec mvVarCategoryNames;

	tIntVec		mvDefIndices;
	tStringVec	mvDefFilenames;
	tIntVec		mvLoadFlags;

	std::map<int, int> mmapIndices;
	tUserClassDefVec mvDefinitions;
};

//------------------------------------------------------------------

#endif // HPLEDITOR_USER_CLASS_DEFINITION_MANAGER_H
