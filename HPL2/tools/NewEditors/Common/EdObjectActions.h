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

#ifndef ED_OBJECT_ACTIONS_H
#define ED_OBJECT_ACTIONS_H

//------------------------------------------------------------------

#include "../Common/EdAction.h"

//------------------------------------------------------------------

class cEdActionCreateObject : public iEdActionWorldModifier
{
public:
	cEdActionCreateObject(iEdWorld*, iEdObjectData*);
	~cEdActionCreateObject();

	bool Init();

protected:
	void DoModify();
	void UndoModify();

	/////////////////////////////////////
	// Data
	iEdObjectData*	mpNewObjData;
};

//------------------------------------------------------------------

class cEdActionDeleteObjects : public iEdActionWorldModifier
{
public:
	cEdActionDeleteObjects(iEdWorld*, const tEdObjectList&);
	~cEdActionDeleteObjects();

	bool Init();

protected:
	void DoModify();
	void UndoModify();

	bool mbValid;
	tEdObjDataVec mvBackupObjData;
};

//------------------------------------------------------------------

class cEdActionCloneObjects : public iEdActionWorldModifier
{
public:
	cEdActionCloneObjects(iEdWorld*, const tEdObjectList&, const tWString& asPrefix=_W(""), const tWString& asSuffix=_W(""));
	~cEdActionCloneObjects();

	bool Init();

	const tEdObjectList& GetClones() { return mlstClones; }

protected:
	void DoModify();
	void UndoModify();

	bool mbValid;
	tIntVec mvObjectIDs;
	std::vector<tIntVec> mvClonedEntityIDs;

	tIntVec mvCloneIDs;

	tEdObjectList mlstClones;

	tWString msNamePrefix;
	tWString msNameSuffix;
};

//------------------------------------------------------------------

/////////////////////////////////////////////////////////
// SET OBJECT PROPERTY
/////////////////////////////////////////////////////////

//------------------------------------------------------------------

template<class T>
class cEdActionSetObjectProperty : public iEdActionWorldModifier
{
public:
	/**
	 * Constructor
	 */
	cEdActionSetObjectProperty(iEdWorld* apWorld, 
								int alObjID,
								iProp* apProp, const T& aValue) : iEdActionWorldModifier(_W("Set Property "),apWorld)
	{
		mlObjID = alObjID;
		mpProp = apProp;
		mNewValue = aValue;

		mbValidProperty = false;
	}

	//------------------------------------------------------------------

	/**
	 * Initializer
	 */
	bool Init()
	{
		if(mpProp==NULL)
		{
			Log("Error creating action: NULL property passed\n");

			return false;
		}

		msName += cString::To16Char(mpProp->GetName());

		iEdObject* pObj = GetObject(mlObjID);
		if(pObj==NULL)
		{
			Log("Error creating action: no objects to modify\n");

			return false;
		}

		// Fetch old value and check if the property exists in the object and if the current and next values are different
		mbValidProperty = pObj->GetProperty(mpProp->GetID(), mOldValue);
		if(mbValidProperty && mOldValue==mNewValue)
			mbValidProperty = false;

		return mbValidProperty;
	}

	//------------------------------------------------------------------

protected:
	virtual void DoModify()
	{
		Apply(mNewValue);
	}

	virtual void UndoModify()
	{
		Apply(mOldValue);
	}

	iEdObject* GetObject(int alID)
	{
		return GetObjectHelper(mpWorld,alID);
	}
	static iEdObject* GetObjectHelper(iEdWorld* apWorld, int alID)
	{
		return apWorld->GetObject(alID);
	}

	void Apply(const T& aX)
	{
		iEdObject* pObj = GetObject(mlObjID);
		pObj->SetProperty(mpProp->GetID(), aX);
		pObj->SetUpdated();

		mpWorld->GetEditor()->BroadcastModuleMsg(eEdModuleMsg_WorldObjModify, NULL);
	}

	void UpdateEntities()
	{

		/*
		tIntListIt itIDs = mlstObjIDs.begin();
		for(;itIDs!=mlstObjIDs.end();++itIDs)
		{
			int lID = *itIDs;
			iEdObject* pObj = GetObject(lID);
			//pObj->UpdateEntity();
		}
		*/
	}
	
	bool mbValidProperty;

	int mlObjID;
	iProp* mpProp;

	T mOldValue;
	T mNewValue;
};

//------------------------------------------------------------------

typedef cEdActionSetObjectProperty<int> cEdActionSetInt;
typedef cEdActionSetObjectProperty<float> cEdActionSetFloat;
typedef cEdActionSetObjectProperty<bool> cEdActionSetBool;
typedef cEdActionSetObjectProperty<tString> cEdActionSetString;
typedef cEdActionSetObjectProperty<cVector2f> cEdActionSetVec2f;
typedef cEdActionSetObjectProperty<cVector3f> cEdActionSetVec3f;
typedef cEdActionSetObjectProperty<cColor> cEdActionSetColor;

//------------------------------------------------------------------

/////////////////////////////////////////////////////////
// SET OBJECTS PROPERTY
/////////////////////////////////////////////////////////

//------------------------------------------------------------------

template<class T>
class cEdActionSetObjectsProperty : public iEdActionWorldModifier
{
public:
	/**
	 * Constructor
	 */
	cEdActionSetObjectsProperty(iEdWorld* apWorld, 
								const tIntList& alstObjIDs,
								const tString &asPropName, const T& aValue) : iEdActionWorldModifier(_W("Set multiple object Property ") + cString::To16Char(asPropName),apWorld)
	{
		mlstObjIDs = alstObjIDs;
		msPropName = asPropName;
		mNewValue = aValue;

		mbValidProperty = false;
	}

	//------------------------------------------------------------------

	/**
	 * Initializer
	 */
	bool Init()
	{
		if(msPropName.empty())
		{
			Log("Error creating action: nameless property passed\n");

			return false;
		}
		if(mlstObjIDs.empty())
		{
			Log("Error creating action: no objects to modify\n");

			return false;
		}

		bool bModifyingSingleEntity = (mlstObjIDs.size()==1);

		bool bNameSet = false;

		tIntListIt it = mlstObjIDs.begin();
		for(;it!=mlstObjIDs.end();++it)
		{
			// Fetch old value and check if the property exists in the object and if the current and next values are different
			iEdObject* pObj = GetObject(*it);
			iEdObjectType* pType = pObj->GetType();

			iProp* pProp = pType->GetPropByName(msPropName);
			if(pProp==NULL)
			{
				mbValidProperty = false;
				break;
			}

			int lPropID = pProp->GetID();
			mlstObjPropIDs.push_back(lPropID);
			
			T oldVal;
			pObj->GetProperty(lPropID, oldVal);

			if(bModifyingSingleEntity && oldVal==mNewValue)
				mbValidProperty = false;

			mlstOldValues.push_back(oldVal);
		}

		return mlstOldValues.size()==mlstObjIDs.size();
	}

	//------------------------------------------------------------------

	bool IsValid() { return mbValidProperty; }

	//------------------------------------------------------------------

protected:
	virtual void DoModify()
	{
		Apply(mNewValue);
	}

	virtual void UndoModify()
	{
		Apply(mlstOldValues);
	}

	iEdObject* GetObject(int alID)
	{
		return GetObjectHelper(mpWorld,alID);
	}
	static iEdObject* GetObjectHelper(iEdWorld* apWorld, int alID)
	{
		return apWorld->GetObject(alID);
	}

	void Apply(const T& aX)
	{
		tIntListIt itIDs = mlstObjIDs.begin();
		tIntListIt itPropIDs = mlstObjPropIDs.begin();
		for(;itIDs!=mlstObjIDs.end();++itIDs, ++itPropIDs)
		{
			int lID = *itIDs;
			int lPropID = *itPropIDs;
			iEdObject* pObj = GetObject(lID);
			pObj->SetProperty(lPropID, aX);
		}

		UpdateEntities();
	}

	void Apply(const std::list<T>& alstX)
	{
		typename std::list<T>::const_iterator itValues = mlstOldValues.begin();
		tIntListIt itIDs = mlstObjIDs.begin();
		tIntListIt itPropIDs = mlstObjPropIDs.begin();
		for(;itIDs!=mlstObjIDs.end();++itIDs, ++itPropIDs, ++itValues)
		{
			const T& oldValue = *itValues;
			int lID = *itIDs;
			int lPropID = *itPropIDs;
			iEdObject* pObj = GetObjectHelper(mpWorld, lID);
			pObj->SetProperty(lPropID, oldValue);
		}

		UpdateEntities();
	}

	void UpdateEntities()
	{
		tIntListIt itIDs = mlstObjIDs.begin();
		for(;itIDs!=mlstObjIDs.end();++itIDs)
		{
			int lID = *itIDs;
			iEdObject* pObj = GetObject(lID);
			pObj->SetUpdated();
		}
	}
	
	bool mbValidProperty;

	tIntList mlstObjIDs;
	tIntList mlstObjPropIDs;
	tString msPropName;

	std::list<T> mlstOldValues;
	T mNewValue;
};

//------------------------------------------------------------------

typedef cEdActionSetObjectsProperty<int>		cEdActionGroupSetInt;
typedef cEdActionSetObjectsProperty<float>		cEdActionGroupSetFloat;
typedef cEdActionSetObjectsProperty<bool>		cEdActionGroupSetBool;
typedef cEdActionSetObjectsProperty<tString>	cEdActionGroupSetString;
typedef cEdActionSetObjectsProperty<cVector2f>	cEdActionGroupSetVec2f;
typedef cEdActionSetObjectsProperty<cVector3f>	cEdActionGroupSetVec3f;
typedef cEdActionSetObjectsProperty<cColor>		cEdActionGroupSetColor;

//------------------------------------------------------------------

#endif // ED_OBJECT_ACTIONS_H
