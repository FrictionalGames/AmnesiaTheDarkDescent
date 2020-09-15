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

#ifndef ED_USER_CLASS_DEFINITION_H
#define ED_USER_CLASS_DEFINITION_H

//------------------------------------------------------------------

#include "../Common/EdVar.h"

//------------------------------------------------------------------

enum eEdVarCategory
{
	eEdVarCategory_Editor,
	eEdVarCategory_Type,
	eEdVarCategory_Instance,
	
	eEdVarCategory_LastEnum
};

//------------------------------------------------------------------

class iEdVar;
class cEdUserClass;
class cEdUserClassInstance;
class cEdUserClassDefinition;

typedef std::map<tString, tEdVarVec> tVarGroupMap;

//------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// VAR GROUP
////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

class cEdVarGroup
{
public:
	cEdVarGroup(const tString&);

	const tString& GetName() { return msName; }
private:
	tString msName;
};

//------------------------------------------------------------------

typedef std::vector<cEdVarGroup*> tVarGroupVec;

//------------------------------------------------------------------

class cEdUserClass : public iEdClass
{
public:
	cEdUserClass(cEdUserClassDefinition*, cXmlElement*);
	virtual ~cEdUserClass();

	const tWString& GetFullNameNoBaseClass();

	bool IsBaseClass() { return mbBaseClass; }

	cEdUserClass* GetBaseClass();

	cEdUserClassDefinition* GetDefinition() { return mpDefinition; }

	bool Create();

	cEdClassInstance* CreateInstance(eEdVarCategory);

	iEdVar* GetVariable(eEdVarCategory, const tWString&);

	//iEdVar* CreateVariableFromElement(cXmlElement*);
	//iEdVar* CreateClassSpecificVariableFromElement(cXmlElement* apElement) { return NULL; }

	void SetUpHierarchy();

	void AddChild(iEdClass*);

	cEdUserClass* GetDefaultChildClass();

	iEdVarInputPanel* CreateInputPanel(cEdClassInstance*);

protected:
	void AddVariablesToInstance(cEdClassInstance*);

	cEdUserClassDefinition* mpDefinition;

	cXmlElement* mpElement;
	tWString msParentName;
	tWString msDefaultChildName;

	cEdUserClass* mpDefaultChild;

	std::vector<tEdVarVec> mvVariables;

	bool mbBaseClass;
	tWString msFullNameNoBaseClass;

	static eEdVarCategory mInstanceCategory;
};

//------------------------------------------------------------------

class cEdUserClassInstance : public cEdClassInstance
{
public:
	cEdUserClassInstance(iEdClass* apClass, eEdVarCategory aCat);
	eEdVarCategory GetCategory() { return mCat; }

	//cEdClassInstance* CreateSpecificCopy();

	iEdVarInputPanel* CreateInputPanel(iEdWindow* apWindow, iWidget* apParent, bool abRows);
protected:
	eEdVarCategory mCat;
};

//------------------------------------------------------------------

class cEdUserClassInputPanel : public iEdVarInputPanel
{
public:
	cEdUserClassInputPanel(cEdUserClassInstance* apInstance);

	void Create(iEdWindow*, iWidget*);
protected:
};

//------------------------------------------------------------------

class cEdUserClassDefinition
{
	friend class cEdUserClass;
public:
	cEdUserClassDefinition(const tString&);
	~cEdUserClassDefinition();

	void RegisterVarCatName(eEdVarCategory, const tString&);
	const tString& GetVarCatName(eEdVarCategory aCat) { return mvCatNames[aCat]; }

	bool Create();

	cEdUserClass* GetClassByName(const tWString&);
	cEdUserClass* GetDefaultClass() { return mpDefaultClass; }

	int GetNumClasses();
	cEdUserClass* GetClass(int);

	int GetGroupNum();
	cEdVarGroup* GetGroup(int);
	cEdVarGroup* GetGroup(const tString&);
protected:
	void RaiseFatalError(const tWString&);
	void SetDefaultClass(cEdUserClass*);

	////////////////////////////////////////////////
	// Data
	tString msDefFile;

	tEdClassVec mvClassPool;
	tEdClassVec mvClasses;
	cEdUserClass* mpDefaultClass;

	tVarGroupVec mvGroups;

	tStringVec mvCatNames;
};

//------------------------------------------------------------------

#endif // ED_USER_CLASS_DEFINITION_H
