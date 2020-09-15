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

#include "../Common/EdUserClassDefinition.h"
#include "../Common/EdWindowHandler.h"

#include "../Common/Editor.h"
#include "../Common/EdScnObject.h"

//------------------------------------------------------------------------------

////////////////////////////////////////////
// Static helpers

static eEdVarCategory IdxToCat(int alX)
{
	return (eEdVarCategory)cMath::Pow2(alX);
}

static int CatToIdx(eEdVarCategory aCat)
{
	return cMath::Log2ToInt(aCat);
}

//------------------------------------------------------------------------------

cEdVarGroup::cEdVarGroup(const tString& asName)
{
	msName = asName;
}

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// USER CLASS
///////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

eEdVarCategory cEdUserClass::mInstanceCategory = eEdVarCategory_Type;

//------------------------------------------------------------------------------

cEdUserClass::cEdUserClass(cEdUserClassDefinition* apDef, cXmlElement* apElem)
{
	mpDefinition = apDef;
	mpElement = apElem;

	mvVariables.resize(eEdVarCategory_LastEnum);

	mbBaseClass = false;

	mpDefaultChild = NULL;
}

//------------------------------------------------------------------------------

cEdUserClass::~cEdUserClass()
{
	for(int i=0;i<eEdVarCategory_LastEnum;++i)
		STLDeleteAll(mvVariables[i]);
}

//------------------------------------------------------------------------------

const tWString& cEdUserClass::GetFullNameNoBaseClass()
{
	if(msFullNameNoBaseClass.empty())
	{
		msFullNameNoBaseClass = GetFullName();

		cEdUserClass* pBaseClass = GetBaseClass();
		if(pBaseClass) 
			msFullNameNoBaseClass = cString::SubW(msFullNameNoBaseClass, (int)pBaseClass->GetFullName().size()+1);
	}

	return msFullNameNoBaseClass;
}

cEdUserClass* cEdUserClass::GetBaseClass()
{
	cEdUserClass* pParent = static_cast<cEdUserClass*>(mpParent);
	if(IsBaseClass()==false)
	{
		if(pParent)
		{
			if(pParent->IsBaseClass())
				return pParent;
			else
				return pParent->GetBaseClass();
		}
		else
			return NULL;
	}
	else
	{
		if(pParent)
			return pParent;
		else
			return this;
	}

}

//------------------------------------------------------------------------------

bool cEdUserClass::Create()
{
	if(mpElement)
	{
		// Class name
		msName = cString::To16Char(mpElement->GetAttributeString("Name"));
		if(msName.empty())
		{
			mpDefinition->RaiseFatalError(_W("on creating User Class - unnamed class found!"));
			return false;
		}

		if(mpDefinition->GetClassByName(msName)!=NULL)
		{
			mpDefinition->RaiseFatalError(_W("on creating User Class ") + msName + _W(" - class with same name found!"));
			return false;
		}

		// Check class declaration
		const tString& sDeclaration = mpElement->GetValue();
		if(sDeclaration=="Class")
			mbBaseClass = false;
		else if(sDeclaration=="BaseClass")
			mbBaseClass = true;
		else
		{
			mpDefinition->RaiseFatalError( _W("on creating User Class \"") + msName + _W("\"\n - illegal declaration \"") + cString::To16Char(sDeclaration) + 
												_W("\"") );
			return false;
		}

		// Inheritance
		msParentName = cString::To16Char(mpElement->GetAttributeString("InheritsFrom"));
		if(msParentName==msName)
		{
			mpDefinition->RaiseFatalError(_W("on creating User Class \"") + msName + _W("\" - inheriting from itself!"));
			return false;
		}

		msDefaultChildName = cString::To16Char(mpElement->GetAttributeString("DefaultChild"));

		for(int cat=eEdVarCategory_Editor; cat<eEdVarCategory_LastEnum; ++cat)
		{
			const tString& sCatName = mpDefinition->GetVarCatName(static_cast<eEdVarCategory>(cat));
			cXmlElement* pVarCategory = mpElement->GetFirstElement(sCatName);
			if(pVarCategory==NULL)
				continue;

			AddVariablesFromElement(mvVariables[cat], pVarCategory);
		}
	}

	return true;
}

//------------------------------------------------------------------------------

cEdClassInstance* cEdUserClass::CreateInstance(eEdVarCategory aCat)
{
	mInstanceCategory = aCat;
	cEdClassInstance* pInstance = hplNew(cEdUserClassInstance, (this, aCat));
	CreateVariableInstances(pInstance);

	return pInstance;
}

//------------------------------------------------------------------------------

iEdVar* cEdUserClass::GetVariable(eEdVarCategory aCat, const tWString& asName)
{
	return iEdClass::GetVariable(mvVariables[aCat], asName);
}

//------------------------------------------------------------------------------

void cEdUserClass::SetUpHierarchy()
{
	if(msParentName.empty())
		return;

	cEdUserClass* pParent = static_cast<cEdUserClass*>(mpDefinition->GetClassByName(msParentName));
	if(pParent==NULL)
	{
		mpDefinition->RaiseFatalError(_W("on setting up hierarchy for class \"") + msName + 
										_W("\" - Parent class \"") + msParentName + _W("\" not found!"));
	}
	else
	{
		if(IsBaseClass() && pParent->IsBaseClass()==false)
		{
			mpDefinition->RaiseFatalError(_W("on setting up hierarchy for class \"") + msName + 
										_W("\" - Base class cannot inherit from a non-base class!"));
		}
		else
		{
			pParent->AddChild(this);
		}
	}
}

//------------------------------------------------------------------------------

void cEdUserClass::AddChild(iEdClass* apChild)
{
	iEdClass::AddChild(apChild);
	if(apChild)
	{
		if(msDefaultChildName==apChild->GetName() || mpDefaultChild==NULL)
			mpDefaultChild = static_cast<cEdUserClass*>(apChild);
	}
}

//------------------------------------------------------------------------------

cEdUserClass* cEdUserClass::GetDefaultChildClass()
{
	if(mpDefaultChild)
		return mpDefaultChild->GetDefaultChildClass();

	return this;
}

//------------------------------------------------------------------------------

iEdVarInputPanel* cEdUserClass::CreateInputPanel(cEdClassInstance* apInstance)
{
	cEdUserClassInstance* pInstance = static_cast<cEdUserClassInstance*>(apInstance);
	if(pInstance->GetCategory()==eEdVarCategory_Type)
		return hplNew(cEdUserClassInputPanel,(pInstance));

	return iEdClass::CreateInputPanel(apInstance);
}

//------------------------------------------------------------------------------

void cEdUserClass::AddVariablesToInstance(cEdClassInstance* apInstance)
{
	const tEdVarVec& vVars = mvVariables[mInstanceCategory];
	for(size_t i=0; i<vVars.size(); ++i)
	{
		iEdVar* pVar = vVars[i];
		apInstance->AddVarInstance(pVar->CreateInstance());
	}
}

//------------------------------------------------------------------------------

/*
iEdVar* cEdUserClass::CreateClassSpecificVariableFromElement(cXmlElement* apElement)
{
	if(apElement==NULL)
		return NULL;

	tString sGroupName = "Uncategorized";

	if(apElement->GetValue()=="Group")
	{
		sGroupName = apElement->GetAttributeString("Name");

		tEdVarVec vTempVars;
		AddVariablesFromElement(this, vTempVars, apElement);
		if(vTempVars.empty()==false)
		{
			cEdVarGroup* pGroup = mpDefinition->GetGroup(sGroupName);
			for(int i=0;i<(int)vTempVars.size();++i)
			{
				iEdVar* pVar = vTempVars[i];
				pVar->SetExtData(pGroup);
			}
			int lIdx = CatToIdx(eEdVarCategory_Type);
			mvVariables[lIdx].insert(mvVariables[lIdx].end(), vTempVars.begin(), vTempVars.end());
		}
	}
	
	return CreateVariableFromElement(apElement);
}

//------------------------------------------------------------------------------

iEdVar* cEdUserClass::GetVariable(eEdVarCategory aCat, const tWString& asName)
{
	int lIdx = CatToIdx(aCat);
	tEdVarVec& vVars = mvVariables[lIdx];
	for(int i=0;i<(int)vVars.size();++i)
	{
		iEdVar* pVar = vVars[i];
		if(pVar->GetName()==asName)
			return pVar;
	}

	return NULL;
}

//------------------------------------------------------------------------------

*/
/*
void cEdUserClass::AddVariablesToInstance(eEdVarCategory aCat, cEdUserClassInstance* apInstance)
{
	int lIdx = CatToIdx(aCat);
	tEdVarVec& vVars = mvVariables[lIdx];
	for(int i=0;i<(int)vVars.size();++i)
	{
		iEdVar* pVar = vVars[i];
		cEdVarInstance* pVarInstance = pVar->CreateInstance();
		
		apInstance->AddVarInstance(pVarInstance);
	}
}
*/

//------------------------------------------------------------------------------

/*
tEditorVarVec cEdUserClass::GetEditorSetupVars()
{
	tEditorVarVec vEditorSetupVars;
	DumpEditorSetupVars(vEditorSetupVars);

	return vEditorSetupVars;
}
*/

/*
void cEdUserClass::DumpEditorSetupVars(tEditorVarVec& avVars)
{
	int lIdx = CatToIdx(eEditorVarCategory_EditorSetup);
	avVars.insert(avVars.end(), mvVariables[lIdx].begin(), mvVariables[lIdx].end());
}
*/

/*

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// USER CLASS TYPE
///////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEdUserClassType::cEdUserClassType(cEdUserClassDefinition* apDefinition) : cEdUserClass(apDefinition)
{
}

//------------------------------------------------------------------------------

cEdUserClassType::~cEdUserClassType()
{
	STLDeleteAll(this-
	STLDeleteAll(mvClasses);
}

//------------------------------------------------------------------------------

bool cEdUserClassType::Create(void* apData)
{
	if(cEdUserClass::Create(apData)==false)
		return false;

	cXmlElement* pElement = (cXmlElement*)apData;

	tString sBaseClassName = pElement->GetAttributeString("BaseClass");
	mpBaseClass = mpDefinition->GetBaseClass(sBaseClassName);

	tString sDefaultSubType = pElement->GetAttributeString("Default");
	mlDefaultSubType = 0;

	cXmlElement* pSubTypes = pElement->GetFirstElement("SubTypes");
	if(pSubTypes)
	{
		cXmlNodeListIterator it = pSubTypes->GetChildIterator();
		int i=0;
		while(it.HasNext())
		{
			cXmlElement* pSubTypeData = it.Next()->ToElement();
			cEdUserClassSubType* pSubType = hplNew(cEdUserClassSubType, (this));
			if(pSubType->Create(pSubTypeData)==false)
			{
				hplDelete(pSubType);
				return false;
			}
			pSubType->SetIndex(i++);
			mvSubTypes.push_back(pSubType);

			//////////////////////////////////
			// Set default subtype index
			if(pSubType->GetName()==sDefaultSubType)
				mlDefaultSubType = (int)mvSubTypes.size()-1;
		}
	}
	else
	{
		cEdUserClassSubType* pSubType = hplNew(cEdUserClassSubType, (this));
		mvSubTypes.push_back(pSubType);
	}

	return true;
}

//------------------------------------------------------------------------------

void cEdUserClassType::AddVariablesToInstance(eEditorVarCategory aCat, cEdUserClassInstance* apInstance)
{
	if(mpBaseClass)
		((cEdUserClassBase*)mpBaseClass)->AddVariablesToInstance(aCat, apInstance);

	cEdUserClass::AddVariablesToInstance(aCat, apInstance);
}

//------------------------------------------------------------------------------

void cEdUserClassType::DumpEditorSetupVars(tEditorVarVec& avVars)
{
	if(mpBaseClass) ((cEdUserClassBase*)mpBaseClass)->DumpEditorSetupVars(avVars);

	cEdUserClass::DumpEditorSetupVars(avVars);
}

//------------------------------------------------------------------------------

iEditorVar* cEdUserClassType::GetVariable(eEditorVarCategory aCat, const tWString& asName)
{
	iEditorVar* pVar = cEdUserClass::GetVariable(aCat, asName);
	if(pVar)
		return pVar;

	if(mpBaseClass) 
		return ((cEdUserClassBase*)mpBaseClass)->GetVariable(aCat, asName);

	return NULL;
}

//------------------------------------------------------------------------------

int cEdUserClassType::GetSubTypeNum()
{
	return (int)mvSubTypes.size();
}

*/
//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// USER CLASS INSTANCE - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEdUserClassInstance::cEdUserClassInstance(iEdClass* apClass, eEdVarCategory aCat) : cEdClassInstance(apClass)
{
	mCat = aCat;
}

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// USER CLASS INSTANCE - PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

/*
cEdClassInstance* cEdUserClassInstance::CreateSpecificCopy()
{
	//return (static_cast<cEdUse(cEdUserClassSubType*)mpClass)->CreateInstance(mCat);
}
*/

//------------------------------------------------------------------------------

iEdVarInputPanel* cEdUserClassInstance::CreateInputPanel(iEdWindow* apWindow, iWidget* apParent, bool abRows)
{
	iEdVarInputPanel* pPanel = NULL;
	if(mCat==eEdVarCategory_Type)
		pPanel = hplNew(cEdUserClassInputPanel,(this));
	else
		return cEdClassInstance::CreateInputPanel(apWindow, apParent, abRows);

	pPanel->SetDeployInputsOnRows(abRows);

	pPanel->Create(apWindow, apParent);

	return pPanel;
}

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// USER CLASS INPUT PANEL - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEdUserClassInputPanel::cEdUserClassInputPanel(cEdUserClassInstance* apInstance) : iEdVarInputPanel(apInstance)
{
}

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// USER CLASS INPUT PANEL - PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------


void cEdUserClassInputPanel::Create(iEdWindow* apWindow, iWidget* apWidget)
{
	cGuiSet* pSet = apWindow->GetGuiSet();
	cEdUserClassInstance* pClass = static_cast<cEdUserClassInstance*>(mpClass);

	if(pClass->GetCategory()==eEdVarCategory_Type)
	{
		cWidgetTabFrame* pTabFrame = pSet->CreateWidgetTabFrame(0,apWidget->GetSize()-4, _W(""), apWidget);

		cEdUserClassInstance* pClass = (cEdUserClassInstance*)mpClass;
		tVector3fVec vPositions;

		for(int i=0;i<pClass->GetVarInstanceNum();++i)
		{
			cWidgetTab* pTab = NULL;
			tWString sGroupName = _W("Uncategorized");

			cEdVarInstance* pVar = pClass->GetVarInstance(i);
			iEdVar* pVarType = pVar->GetVarType();
			cEdVarGroup* pGroup = static_cast<cEdVarGroup*>(pVarType->GetExtraData());
			if(pGroup)
				sGroupName = cString::To16Char(pGroup->GetName());

			pTab = pTabFrame->GetTab(sGroupName);
			if(pTab==NULL)
			{
				pTab = pTabFrame->AddTab(sGroupName);
				vPositions.push_back(cVector3f(10,10,0.1f));
			}

			iEdVarInput* pInput = pVar->CreateInput(apWindow, pTab);
			pInput->SetPanel(this);
			pInput->GetInput()->SetPosition(vPositions[pTab->GetIndex()]);
			pInput->GetInput()->SetTabWidth(0.4f*pTab->GetSize().x);
			pInput->GetInput()->UpdateLayout();

			mvInputs.push_back(pInput);

			vPositions[pTab->GetIndex()].y += pInput->GetInput()->GetSize().y + 5;
		}

		mpHandle = pTabFrame;
	}
	else
	{
		iEdVarInputPanel::Create(apWindow, apWidget);
	}
}


//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// USER CLASS DEFINITION - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEdUserClassDefinition::cEdUserClassDefinition(const tString& asDefFile)
{
	msDefFile = asDefFile;

	mvCatNames.resize(eEdVarCategory_LastEnum);

	mpDefaultClass = NULL;
}

//------------------------------------------------------------------------------

cEdUserClassDefinition::~cEdUserClassDefinition()
{
	STLDeleteAll(mvClassPool);
	STLDeleteAll(mvGroups);
}

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// USER CLASS DEFINITION - PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

void cEdUserClassDefinition::RegisterVarCatName(eEdVarCategory aCat, const tString& asName)
{
	if(aCat==eEdVarCategory_LastEnum) return;
	
	mvCatNames[aCat] = asName;
}

//------------------------------------------------------------------------------

bool cEdUserClassDefinition::Create()
{
	bool bValid = false;
	for(size_t i=0; i<mvCatNames.size(); ++i)
	{
		if(mvCatNames[i].empty()==false)
		{
			bValid = true;
			break;
		}
	}

	if(bValid==false)
	{
		FatalError("Class Definition %s with no registered category names!", msDefFile.c_str());
		return false;
	}


	cResources* pRes = iEditor::GetInstance()->GetEngine()->GetResources();
	iXmlDocument* pDoc = pRes->LoadXmlDocument(msDefFile);

	////////////////////////////////////////////
	// Check XML validity
	if(pDoc==NULL)
	{
		RaiseFatalError(_W("failed compilation. Check log for details."));

		return false;
	}

	cXmlElement* pClassesElement = pDoc->GetFirstElement("Classes");
	if(pClassesElement==NULL)
	{
		RaiseFatalError(_W("failed compilation.\"Classes\" element not found."));

		return false;
	}
		
	bool bSuccessfullyCreated = true;

	////////////////////////////////////////////
	// Create Classes
	cXmlNodeListIterator it = pClassesElement->GetChildIterator();
	while(it.HasNext())
	{
		cXmlElement* pXmlClass = it.Next()->ToElement();

		cEdUserClass* pClass = hplNew(cEdUserClass,(this, pXmlClass));
		if(pClass->Create()==false)
		{
			bSuccessfullyCreated = false;
			hplDelete(pClass);
			break;
		}
		mvClassPool.push_back(pClass);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Build hierarchy
	for(size_t i=0; i<mvClassPool.size(); ++i)
	{
		cEdUserClass* pClass = static_cast<cEdUserClass*>(mvClassPool[i]);
		pClass->SetUpHierarchy();
	}

	// Once the hierarchy tree is set up, choose classes to hang from the def root
	// These should be non-base classes that are either orphan or parent is a base class
	for(size_t i=0; i<mvClassPool.size(); ++i)
	{
		cEdUserClass* pClass = static_cast<cEdUserClass*>(mvClassPool[i]);
		if(pClass->IsBaseClass())
			continue;

		cEdUserClass* pParent = static_cast<cEdUserClass*>(pClass->GetParent());
		if(pParent==NULL || pParent->IsBaseClass())
		{
			if(mpDefaultClass==NULL) mpDefaultClass = pClass->GetDefaultChildClass();
			mvClasses.push_back(pClass);
		}
	}

	pRes->DestroyXmlDocument(pDoc);

	return bSuccessfullyCreated;
}

//------------------------------------------------------------------------------

cEdUserClass* cEdUserClassDefinition::GetClassByName(const tWString& asName)
{
	for(size_t i=0; i<mvClassPool.size(); ++i)
	{
		iEdClass* pClass = mvClassPool[i];

		if(pClass->GetName()==asName)
			return static_cast<cEdUserClass*>(pClass);
	}

	return NULL;
}

//------------------------------------------------------------------------------

int cEdUserClassDefinition::GetNumClasses()
{
	return (int)mvClasses.size();
}

cEdUserClass* cEdUserClassDefinition::GetClass(int alIdx)
{
	if(alIdx<0 || alIdx>=GetNumClasses())
		return NULL;

	return static_cast<cEdUserClass*>(mvClasses[alIdx]);
}


//------------------------------------------------------------------------------

int cEdUserClassDefinition::GetGroupNum()
{
	return (int)mvGroups.size();
}

//------------------------------------------------------------------------------

cEdVarGroup* cEdUserClassDefinition::GetGroup(int alIdx)
{
	if(alIdx<0 || alIdx>GetGroupNum())
		return NULL;

	return mvGroups[alIdx];
}

//------------------------------------------------------------------------------

cEdVarGroup* cEdUserClassDefinition::GetGroup(const tString& asName)
{
	cEdVarGroup* pGroup = (cEdVarGroup*) STLFindByName(mvGroups, asName);

	if(pGroup==NULL)
	{
		pGroup = hplNew(cEdVarGroup,(asName));
		mvGroups.push_back(pGroup);
	}

	return pGroup;
}

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// USER CLASS DEFINITION - PROTECTED METHODS
///////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

void cEdUserClassDefinition::RaiseFatalError(const tWString& asError)
{
	FatalError("Definition \"%s\" %ls", msDefFile.c_str(), asError.c_str());
}

//------------------------------------------------------------------------------
