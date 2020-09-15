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

#ifndef ED_VAR_H
#define ED_VAR_H

#include "../common/StdAfx.h"
#include "../Common/EdInput.h"

using namespace hpl;

//--------------------------------------------------------------------------------

class iEdVar;
class iEdVarInput;
class cEdVarInstance;
class iEdClass;
class cEdClassInstance;
class iEdVarInputPanel;

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// VAR BASE
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class iEdVar
{
public:
	iEdVar(int aType);

	virtual bool Create(cXmlElement*);

	iEdVarInput* CreateInput(iEdWindow*, iWidget*, cEdVarInstance*);

	cEdVarInstance* CreateInstance();

	int GetType() { return mType; }
	const tWString& GetName() { return msName; }
	const tWString& GetDefaultValue() { return msDefaultValue; }
	const tWString& GetDescription() { return msDescription; }

	void SetExtraData(void* apData) { mpExtraData = apData; }
	void* GetExtraData() { return mpExtraData; }
	
protected:
	virtual iEdVarInput* CreateSpecificInput(iEdWindow*, 
												 iWidget*, 
												 cEdVarInstance*)=0;

	int mType;
	tWString msName;
	tWString msDefaultValue;
	tWString msDescription;

	void*	mpExtraData;
};

//--------------------------------------------------------------------------------

typedef std::vector<iEdVar*> tEdVarVec;

typedef std::list<iEdVar*> tEdVarList;

//--------------------------------------------------------------------------------

class iEdVarCreator
{
public:
	iEdVarCreator(const tString& asType) : msType(asType) {}
	const tString& GetType() { return msType; }
	virtual iEdVar* CreateVar()=0;
protected:
	tString msType;
};

//--------------------------------------------------------------------------------

typedef std::vector<iEdVarCreator*> tVarCreatorVec;

typedef std::map<tString, iEdVarCreator*> tVarCreatorMap;
typedef tVarCreatorMap::iterator tVarCreatorMapIt;

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// EDITOR VAR INPUT BASE
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class iEdVarInput
{
public:
	iEdVarInput(cEdVarInstance* apVar, iEdInput* apInput);
	virtual ~iEdVarInput();

	virtual void FetchValueFromVar();
	virtual void CopyValueToVar();

	cEdVarInstance* GetVar() { return mpVar; }
	iEdInput* GetInput() { return mpInput; }

	void Update();

	void SetPanel(iEdVarInputPanel* apPanel) { mpPanel = apPanel; }

protected:
	bool OnValueEnter(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(OnValueEnter);

	cEdVarInstance* mpVar;
	iEdInput* mpInput;
	iEdVarInputPanel* mpPanel;
};

//---------------------------------------------------------------------------

typedef std::vector<iEdVarInput*> tVarInputVec;

typedef std::list<iEdVarInput*> tVarInputList;
typedef tVarInputList::iterator tVarInputListIt;

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR BOOL + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEdVarBool : public iEdVar
{
public:
	cEdVarBool();

	iEdVarInput* CreateSpecificInput(iEdWindow* apWindow, iWidget* apParent, cEdVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEdVarBoolCreator : public iEdVarCreator
{
public:
	cEdVarBoolCreator() : iEdVarCreator("Bool") {}
	iEdVar* CreateVar() { return hplNew(cEdVarBool,()); }
};

//--------------------------------------------------------------------------------

class cEdVarInputBool : public iEdVarInput
{
public:
	cEdVarInputBool(cEdVarInstance* apVar, iEdWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR FLOAT + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEdVarFloat : public iEdVar
{
public:
	cEdVarFloat();

	iEdVarInput* CreateSpecificInput(iEdWindow* apWindow, iWidget* apParent, cEdVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEdVarFloatCreator : public iEdVarCreator
{
public:
	cEdVarFloatCreator() : iEdVarCreator("Float") {}
	iEdVar* CreateVar() { return hplNew(cEdVarFloat,()); }
};

//--------------------------------------------------------------------------------

class cEdVarInputFloat : public iEdVarInput
{
public:
	cEdVarInputFloat(cEdVarInstance* apVar, iEdWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR INT + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEdVarInt : public iEdVar
{
public:
	cEdVarInt();

	iEdVarInput* CreateSpecificInput(iEdWindow* apWindow, iWidget* apParent, cEdVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEdVarIntCreator : public iEdVarCreator
{
public:
	cEdVarIntCreator() : iEdVarCreator("Int") {}
	iEdVar* CreateVar() { return hplNew(cEdVarInt,()); }
};

//--------------------------------------------------------------------------------

class cEdVarInputInt : public iEdVarInput
{
public:
	cEdVarInputInt(cEdVarInstance* apVar, iEdWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR STRING + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEdVarString : public iEdVar
{
public:
	cEdVarString();

	iEdVarInput* CreateSpecificInput(iEdWindow* apWindow, iWidget* apParent, cEdVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEdVarStringCreator : public iEdVarCreator
{
public:
	cEdVarStringCreator() : iEdVarCreator("String") {}
	iEdVar* CreateVar() { return hplNew(cEdVarString,()); }
};

//--------------------------------------------------------------------------------

class cEdVarInputString : public iEdVarInput
{
public:
	cEdVarInputString(cEdVarInstance* apVar, iEdWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR VECTOR2F + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEdVarVector2f : public iEdVar
{
public:
	cEdVarVector2f();

	iEdVarInput* CreateSpecificInput(iEdWindow* apWindow, iWidget* apParent, cEdVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEdVarVector2fCreator : public iEdVarCreator
{
public:
	cEdVarVector2fCreator() : iEdVarCreator("Vector2f") {}
	iEdVar* CreateVar() { return hplNew(cEdVarVector2f,()); }
};

//--------------------------------------------------------------------------------

class cEdVarInputVector2f : public iEdVarInput
{
public:
	cEdVarInputVector2f(cEdVarInstance* apVar, iEdWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR VECTOR3F + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEdVarVector3f : public iEdVar
{
public:
	cEdVarVector3f();

	iEdVarInput* CreateSpecificInput(iEdWindow* apWindow, iWidget* apParent, cEdVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEdVarVector3fCreator : public iEdVarCreator
{
public:
	cEdVarVector3fCreator() : iEdVarCreator("Vector3f") {}
	iEdVar* CreateVar() { return hplNew(cEdVarVector3f,()); }
};

//--------------------------------------------------------------------------------

class cEdVarInputVector3f : public iEdVarInput
{
public:
	cEdVarInputVector3f(cEdVarInstance* apVar, iEdWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR COLOR + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEdVarColor : public iEdVar
{
public:
	cEdVarColor();

	iEdVarInput* CreateSpecificInput(iEdWindow* apWindow, iWidget* apParent, cEdVarInstance* apVar);
};

//--------------------------------------------------------------------------------

class cEdVarColorCreator : public iEdVarCreator
{
public:
	cEdVarColorCreator() : iEdVarCreator("Color") {}
	iEdVar* CreateVar() { return hplNew(cEdVarColor,()); }
};

//--------------------------------------------------------------------------------

class cEdVarInputColor : public iEdVarInput
{
public:
	cEdVarInputColor(cEdVarInstance* apVar, iEdWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR ENUM + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEdVarEnum : public iEdVar
{
public:
	cEdVarEnum();

	bool Create(cXmlElement* apElement);

	const tWStringVec& GetEnumValues() { return mvValues; }
	iEdVarInput* CreateSpecificInput(iEdWindow* apWindow, iWidget* apParent, cEdVarInstance* apVar);

protected:
	tWStringVec mvValues;
};

//--------------------------------------------------------------------------------

class cEdVarEnumCreator : public iEdVarCreator
{
public:
	cEdVarEnumCreator() : iEdVarCreator("Enum") {}
	iEdVar* CreateVar() { return hplNew(cEdVarEnum,()); }
};

//--------------------------------------------------------------------------------

class cEdVarInputEnum : public iEdVarInput
{
public:
	cEdVarInputEnum(cEdVarInstance* apVar, iEdWindow* apWindow, iWidget* apParent);
};

//---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CUSTOM VAR FILE + INPUT
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEdVarFile : public iEdVar
{
public:
	cEdVarFile();

	bool Create(cXmlElement* apElement);

	iEdVarInput* CreateSpecificInput(iEdWindow* apWindow, iWidget* apParent, cEdVarInstance* apVar);

	//eEdResourceType GetBrowserType() { return mResType; }

protected:
	//eEdResourceType mResType;
	tWStringList mlstExtensions;
private:
	//eEdResourceType GetBrowserTypeFromElement(cXmlElement* apElement);
};

//--------------------------------------------------------------------------------

class cEdVarFileCreator : public iEdVarCreator
{
public:
	cEdVarFileCreator() : iEdVarCreator("File") {}
	iEdVar* CreateVar() { return hplNew(cEdVarFile,()); }
};

//--------------------------------------------------------------------------------

class cEdVarInputFile : public iEdVarInput
{
public:
	cEdVarInputFile(cEdVarInstance* apVar, iEdWindow* apWindow, iWidget* apParent);
};

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// VAR INSTANCE
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEdVarInstance
{
public:
	cEdVarInstance(iEdVar* apVar);
	iEdVar* GetVarType() { return mpVar; }

	const tWString& GetName();

	bool SetValue(const tWString& asValue);
	const tWString& GetValue() { return msValue; }

	iEdVarInput* CreateInput(iEdWindow* apWindow, iWidget* apParent);
protected:
	iEdVar* mpVar;
	tWString msValue;
};

//--------------------------------------------------------------------------------

typedef std::vector<cEdVarInstance*> tEdVarInstanceVec;

typedef std::map<tWString, iEdVar*>	 tEdVarMap;

typedef std::map<tWString, tWString> tVarValueMap;
typedef tVarValueMap::iterator		tVarValueMapIt;

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

typedef std::vector<iEdClass*> tEdClassVec;

typedef std::map<tString, tEdVarVec> tEdVarGroup;

//--------------------------------------------------------------------------------

class iEdClass
{
public:
	iEdClass();
	virtual ~iEdClass();

	virtual bool Create()=0;

	const tWString& GetName()	{ return msName; }
	virtual const tWString& GetFullName();
	iEdClass* GetParent()		{ return mpParent; }

	cEdClassInstance* CreateInstance();
	
	iEdVar* CreateVariable(const tString&);

	virtual iEdVar* CreateVariableFromElement(cXmlElement*);
	//virtual iEdVar* CreateClassSpecificVariableFromElement(cXmlElement* apElement);

	bool AddVariablesFromElement(tEdVarVec& avVars, cXmlElement* apElement);

	iEdVar* GetVariable(const tEdVarVec&, const tWString&);

	//static iEdVar* CreateVariableFromElement(cXmlElement* apElement);
	//static iEdVar* CreateVariable(const tString& asType);

	//static iEdClass* GetClassByIdx(const tEditorClassVec& avClasses, int alIdx);
	//static iEdClass* GetClassByName(const tEditorClassVec& avClasses, const tString& asName);
	//static void DumpVarsOnInstance(const tEdVarVec& avVars, cEdClassInstance* apInstance);

	virtual void AddChild(iEdClass*);

	virtual iEdVarInputPanel* CreateInputPanel(cEdClassInstance*);

	int GetNumChildren();
	iEdClass* GetChild(int);
protected:
	virtual void AddVariablesToInstance(cEdClassInstance*)=0;
	void CreateVariableInstances(cEdClassInstance*);

	tWString msName;
	tWString msFullName;
	tEdVarMap mmapVars;

	iEdClass* mpParent;
	tEdClassVec mvChildren;

private:
	void SetParent(iEdClass* apParent) { mpParent = apParent; }

	static void AddVarCreator(iEdVarCreator*, bool abDefault=false);

	static int mlCreatedClasses;
	static tVarCreatorMap mmapVarCreators;
	static iEdVarCreator* mpDefaultVarCreator;
};

//--------------------------------------------------------------------------------

class cEdClassInstance
{
public:
	cEdClassInstance(iEdClass* apClass);
	~cEdClassInstance();

	iEdClass* GetClass() { return mpClass; }

	void AddVarInstance(cEdVarInstance* apVar);

	int GetVarInstanceNum();
	cEdVarInstance* GetVarInstance(int alX);
	cEdVarInstance* GetVarInstance(const tWString& asName);

	bool SetVarValue(const tWString&, const tWString&);

	void Load(cXmlElement* apElement);
	void Save(cXmlElement* apElement);

	void LoadValuesFromMap(const tVarValueMap& amapValues);
	void SaveValuesToMap(tVarValueMap& amapValues);

	void LoadFromResourceVarsObject(cResourceVarsObject* apObject);

	cEdClassInstance* CreateCopy();
	virtual cEdClassInstance* CreateSpecificCopy() { return NULL; }
	virtual void CopyFromInstance(cEdClassInstance* apInstance);

	virtual iEdVarInputPanel* CreateInputPanel(iEdWindow* apWindow, iWidget* apParent, bool abRows);

protected:
	iEdClass* mpClass;
	tEdVarInstanceVec mvVars;
};

//--------------------------------------------------------------------------------

typedef bool (*tEdVarInputPanelCallback)(void*, iEdVarInput*);

class iEdVarInputPanel
{
	friend class iEdVarInput;
public:
	iEdVarInputPanel(cEdClassInstance* apClass);
	virtual ~iEdVarInputPanel();

	iWidget* GetHandle() { return mpHandle; }
    
	void SetDeployInputsOnRows(bool abX) { mbDeployInputsOnRows = abX; }
	virtual void Create(iEdWindow* apWindow, iWidget* apWidget);
	void Update();

	void SetCallback(void*, tEdVarInputPanelCallback);
	bool RunCallback(iEdVarInput* apInput);
protected:
	virtual bool OnVarInputValueEnterCallback(iEdVarInput* apInput) { return false; }

	cEdClassInstance* mpClass;
	tVarInputVec mvInputs;
	iWidget* mpHandle;

	bool mbDeployInputsOnRows;

	void* mpCallbackObject;
	tEdVarInputPanelCallback mpCallback;
};

//--------------------------------------------------------------------------------

#endif // ED_VAR_H
