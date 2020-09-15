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

#ifndef HPLEDITOR_EDITOR_VAR_H
#define HPLEDITOR_EDITOR_VAR_H

#include "../common/StdAfx.h"
#include "EditorInput.h"

using namespace hpl;

//--------------------------------------------------------------------------------

class iEditorVar;
class iEditorVarInput;
class cEditorVarInstance;
class iEditorClass;
class cEditorClassInstance;
class cEditorVarInputPanel;

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// VAR BASE
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class iEditorVar
{
public:
	iEditorVar(eVariableType aType);

	virtual bool Create(cXmlElement* apElement);

	iEditorVarInput* CreateInput(iEditorWindow* apWindow, 
								 iWidget* apParent, 
								 cEditorVarInstance* apVar);

	cEditorVarInstance* CreateInstance();

	eVariableType GetType() { return mType; }
	const tWString& GetName() { return msName; }
	const tWString& GetDefaultValue() { return msDefaultValue; }
	const tWString& GetDescription() { return msDescription; }

	void SetExtData(void* apData) { mpExtData = apData; }
	void* GetExtData() { return mpExtData; }
	
protected:
	virtual iEditorVarInput* CreateSpecificInput(iEditorWindow* apWindow, 
												 iWidget* apParent, 
												 cEditorVarInstance* apVar)=0;

	eVariableType mType;
	tWString msName;
	tWString msDefaultValue;
	tWString msDescription;

	void*	mpExtData;
};

//--------------------------------------------------------------------------------

typedef std::vector<iEditorVar*> tEditorVarVec;

typedef std::list<iEditorVar*> tEditorVarList;



//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// EDITOR VAR INPUT BASE
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class iEditorVarInput
{
public:
	iEditorVarInput(cEditorVarInstance* apVar, iEditorInput* apInput);
	virtual ~iEditorVarInput();

	virtual void FetchValueFromVar();
	virtual void CopyValueToVar();

	cEditorVarInstance* GetVar() { return mpVar; }
	iEditorInput* GetInput() { return mpInput; }

	void Update();

	void SetPanel(cEditorVarInputPanel* apPanel) { mpPanel = apPanel; }

protected:
	bool OnValueEnter(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnValueEnter);

	cEditorVarInstance* mpVar;
	iEditorInput* mpInput;
	cEditorVarInputPanel* mpPanel;
};

//---------------------------------------------------------------------------

typedef std::vector<iEditorVarInput*> tVarInputVec;

typedef std::list<iEditorVarInput*> tVarInputList;
typedef tVarInputList::iterator tVarInputListIt;

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR BOOL + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEditorVarBool : public iEditorVar
{
public:
	cEditorVarBool();

	iEditorVarInput* CreateSpecificInput(iEditorWindow* apWindow, iWidget* apParent, cEditorVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEditorVarInputBool : public iEditorVarInput
{
public:
	cEditorVarInputBool(cEditorVarInstance* apVar, iEditorWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR FLOAT + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEditorVarFloat : public iEditorVar
{
public:
	cEditorVarFloat();

	iEditorVarInput* CreateSpecificInput(iEditorWindow* apWindow, iWidget* apParent, cEditorVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEditorVarInputFloat : public iEditorVarInput
{
public:
	cEditorVarInputFloat(cEditorVarInstance* apVar, iEditorWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR INT + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEditorVarInt : public iEditorVar
{
public:
	cEditorVarInt();

	iEditorVarInput* CreateSpecificInput(iEditorWindow* apWindow, iWidget* apParent, cEditorVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEditorVarInputInt : public iEditorVarInput
{
public:
	cEditorVarInputInt(cEditorVarInstance* apVar, iEditorWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR STRING + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEditorVarString : public iEditorVar
{
public:
	cEditorVarString();

	iEditorVarInput* CreateSpecificInput(iEditorWindow* apWindow, iWidget* apParent, cEditorVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEditorVarInputString : public iEditorVarInput
{
public:
	cEditorVarInputString(cEditorVarInstance* apVar, iEditorWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR VECTOR2F + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEditorVarVector2f : public iEditorVar
{
public:
	cEditorVarVector2f();

	iEditorVarInput* CreateSpecificInput(iEditorWindow* apWindow, iWidget* apParent, cEditorVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEditorVarInputVector2f : public iEditorVarInput
{
public:
	cEditorVarInputVector2f(cEditorVarInstance* apVar, iEditorWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR VECTOR3F + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEditorVarVector3f : public iEditorVar
{
public:
	cEditorVarVector3f();

	iEditorVarInput* CreateSpecificInput(iEditorWindow* apWindow, iWidget* apParent, cEditorVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEditorVarInputVector3f : public iEditorVarInput
{
public:
	cEditorVarInputVector3f(cEditorVarInstance* apVar, iEditorWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR COLOR + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEditorVarColor : public iEditorVar
{
public:
	cEditorVarColor();

	iEditorVarInput* CreateSpecificInput(iEditorWindow* apWindow, iWidget* apParent, cEditorVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEditorVarInputColor : public iEditorVarInput
{
public:
	cEditorVarInputColor(cEditorVarInstance* apVar, iEditorWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR ENUM + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEditorVarEnum : public iEditorVar
{
public:
	cEditorVarEnum();

	bool Create(cXmlElement* apElement);

	const tWStringVec& GetEnumValues() { return mvValues; }
	iEditorVarInput* CreateSpecificInput(iEditorWindow* apWindow, iWidget* apParent, cEditorVarInstance* apVar);

protected:
	tWStringVec mvValues;
};

//--------------------------------------------------------------------------------

class cEditorVarInputEnum : public iEditorVarInput
{
public:
	cEditorVarInputEnum(cEditorVarInstance* apVar, iEditorWindow* apWindow, iWidget* apParent);
};

//---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR FILE + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEditorVarFile : public iEditorVar
{
public:
	cEditorVarFile();

	bool Create(cXmlElement* apElement);

	iEditorVarInput* CreateSpecificInput(iEditorWindow* apWindow, iWidget* apParent, cEditorVarInstance* apVar);

	eEditorResourceType GetBrowserType() { return mResType; }

protected:
	eEditorResourceType mResType;
	tWStringList mlstExtensions;
private:
	eEditorResourceType GetBrowserTypeFromElement(cXmlElement* apElement);
};

//--------------------------------------------------------------------------------

class cEditorVarInputFile : public iEditorVarInput
{
public:
	cEditorVarInputFile(cEditorVarInstance* apVar, iEditorWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// VAR INSTANCE
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEditorVarInstance
{
public:
	cEditorVarInstance(iEditorVar* apVar);
	iEditorVar* GetVarType() { return mpVar; }

	const tWString& GetName();

	void SetValue(const tWString& asValue);
	const tWString& GetValue() { return msValue; }

	iEditorVarInput* CreateInput(iEditorWindow* apWindow, iWidget* apParent);
protected:
	iEditorVar* mpVar;
	tWString msValue;
};

//--------------------------------------------------------------------------------

typedef std::vector<cEditorVarInstance*> tEditorVarInstanceVec;

typedef std::map<tWString, tWString> tVarValueMap;
typedef tVarValueMap::iterator		tVarValueMapIt;

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

typedef std::vector<iEditorClass*> tEditorClassVec;

typedef std::map<tString, tEditorVarVec> tEditorVarGroup;

//--------------------------------------------------------------------------------

class iEditorClass
{
public:
	iEditorClass();
	virtual ~iEditorClass(){}

	virtual bool Create(void* apData)=0;

	const tString& GetName() { return msName; }

	virtual cEditorClassInstance* CreateInstance() { return NULL; }
	virtual iEditorVar* CreateClassSpecificVariableFromElement(cXmlElement* apElement);

	static bool AddVariablesFromElement(iEditorClass* apClass, tEditorVarVec& avVars, cXmlElement* apElement);

	static iEditorVar* CreateVariableFromElement(cXmlElement* apElement);
	static iEditorVar* CreateVariable(const tString& asType);

	static iEditorClass* GetClassByIdx(const tEditorClassVec& avClasses, int alIdx);
	static iEditorClass* GetClassByName(const tEditorClassVec& avClasses, const tString& asName);
	static void DumpVarsOnInstance(const tEditorVarVec& avVars, cEditorClassInstance* apInstance);
protected:
	tString msName;
};

//--------------------------------------------------------------------------------

class cEditorClassInstance
{
public:
	cEditorClassInstance(iEditorClass* apClass);
	~cEditorClassInstance();

	iEditorClass* GetClass() { return mpClass; }

	void AddVarInstance(cEditorVarInstance* apVar);

	int GetVarInstanceNum();
	cEditorVarInstance* GetVarInstance(int alX);
	cEditorVarInstance* GetVarInstance(const tWString& asName);

	void SetVarValue(const tWString& asName, const tWString& asValue);

	void Load(cXmlElement* apElement);
	void Save(cXmlElement* apElement);

	void LoadValuesFromMap(const tVarValueMap& amapValues);
	void SaveValuesToMap(tVarValueMap& amapValues);

	void LoadFromResourceVarsObject(cResourceVarsObject* apObject);

	cEditorClassInstance* CreateCopy();
	virtual cEditorClassInstance* CreateSpecificCopy() { return NULL; }
	virtual void CopyFromInstance(cEditorClassInstance* apInstance);

	virtual cEditorVarInputPanel* CreateInputPanel(iEditorWindow* apWindow, iWidget* apParent, bool abRows);

protected:
	iEditorClass* mpClass;
	tEditorVarInstanceVec mvVars;
};

//--------------------------------------------------------------------------------

typedef bool (*tEditorVarInputPanelCallback)(void*, iEditorVarInput*);

class cEditorVarInputPanel
{
	friend class iEditorVarInput;
public:
	cEditorVarInputPanel(cEditorClassInstance* apClass);
	virtual ~cEditorVarInputPanel();

	iWidget* GetHandle() { return mpHandle; }
    
	void SetDeployInputsOnRows(bool abX) { mbDeployInputsOnRows = abX; }
	virtual void Create(iEditorWindow* apWindow, iWidget* apWidget);
	void Update();

	void SetCallback(void*, tEditorVarInputPanelCallback);
	bool RunCallback(iEditorVarInput* apInput);
protected:
	virtual bool OnVarInputValueEnterCallback(iEditorVarInput* apInput) { return false; }

	cEditorClassInstance* mpClass;
	tVarInputVec mvInputs;
	iWidget* mpHandle;

	bool mbDeployInputsOnRows;

	void* mpCallbackObject;
	tEditorVarInputPanelCallback mpCallback;
};

//--------------------------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_CUSTOM_VAR_H
