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

#ifndef HPL_SAVE_GAME_H
#define HPL_SAVE_GAME_H

#include <map>

#include "system/SystemTypes.h"
#include "system/SerializeClass.h"

class TiXmlElement;

#define kSaveData_LoadFromBegin(aClass) \
							__super::LoadFromSaveData(apSaveData); \
							cSaveData_##aClass *pData = static_cast<cSaveData_##aClass*>(apSaveData);

#define kSaveData_SaveToBegin(aClass) \
							__super::SaveToSaveData(apSaveData);\
							cSaveData_##aClass *pData = static_cast<cSaveData_##aClass*>(apSaveData);

#define kSaveData_SetupBegin(aClass) \
							__super::SaveDataSetup(apSaveObjectHandler,apGame);\
							cSaveData_##aClass *pData = static_cast<cSaveData_##aClass*>(mpSaveData); \
							const char *sClassNameString = #aClass;

#define kSaveData_BaseClass(aClass) class cSaveData_##aClass : public iSaveData
#define kSaveData_ChildClass(aParent, aChild) class cSaveData_##aChild : public cSaveData_##aParent

#define kSaveData_ClassInit(aClass) kSerializableClassInit(cSaveData_##aClass)

//////////////////////////////////////////////
//Helpers to copy data.
#define kSaveData_SaveTo(aVar) pData->aVar = aVar;
#define kSaveData_LoadFrom(aVar) aVar = pData->aVar;

#define kSaveData_SaveObject(aObject, aId) \
						if(aObject) \
							pData->aId = aObject->GetSaveObjectId(); \
						else \
							pData->aId = -1;

//Only used in setup:
#define kSaveData_LoadObject(aObject, aId,aClass) \
							if(pData->aId == -1) \
								aObject = NULL; \
							else {\
								aObject = static_cast<aClass>(apSaveObjectHandler->Get(pData->aId)); \
							}



//////////////////////////////////////////////
//Helpers to copy containers with SaveDataId
#define kSaveData_SaveIdList(aSrcList,aSrcIt,aDestList) \
							pData->aDestList.Clear(); \
							for(aSrcIt it = aSrcList.begin();it != aSrcList.end();++it) \
							{ \
								pData->aDestList.Add((*it)->GetSaveObjectId()); \
							}

//Only used in setup:
#define kSaveData_LoadIdList(aSrcList,aDestList, aClass) \
							{	cContainerListIterator<int> it = pData->aDestList.GetIterator(); \
							aSrcList.clear(); \
							while(it.HasNext()) \
							{ \
								int lId = it.Next(); \
								iSaveObject* pObject = apSaveObjectHandler->Get(lId); \
								if(pObject==NULL) \
								{  \
									continue;  \
								} \
								aSrcList.push_back(static_cast<aClass>(pObject)); \
							} \
							}



namespace hpl {

	//--------------------------------------------------------


	class cSaveObjectHandler;
	class iSaveObject;
	class cEngine;

	/**
	* This is data that is created by a SaveObject and the data can be loaded into the object.
	*/
	class iSaveData : public iSerializable
	{
	kSerializableClassInit(iSaveData)
	public:
		virtual ~iSaveData() {}
		int mlSaveDataId;

		/**
		  * Creates the SaveObject using previously saved objects and the data in this class.
		  */
		virtual iSaveObject* CreateSaveObject(cSaveObjectHandler *apSaveObjectHandler,cEngine *apGame)=0;

		/**
		  * The lower number the earlier it will be created.
		  */
        virtual int GetSaveCreatePrio()=0;
	};



	//--------------------------------------------------------

	/**
	* This is class is inherited by object that are to be saved.
	*/
	class iSaveObject
	{
	friend class cSaveObjectHandler;
	public:
		iSaveObject();
		~iSaveObject();

		/**
		* Get a unique id for this object.
		*/
		int GetSaveObjectId(){ return mlSaveObjectId;}

		/**
		* Save it's data to a SaveData
		*/
		virtual void SaveToSaveData(iSaveData *apSaveData);
		/**
		* Load it's data from a SaveData
		*/
		virtual void LoadFromSaveData(iSaveData *apSaveData);

		/**
		* Creates the SaveData that this class uses.
		*/
		virtual iSaveData* CreateSaveData()=0;

		/**
		* After all objects have been created, this function is called to enable setup.
		*/
		virtual void SaveDataSetup(cSaveObjectHandler *apSaveObjectHandler, cEngine *apGame);

		void SetIsSaved(bool abX){ mbIsSaved = abX;}
		bool IsSaved(){ return mbIsSaved;}

	protected:
		iSaveData *mpSaveData;
	private:
		int mlSaveObjectId;
		bool mbIsSaved;
		static int _mlGlobalIdCount;
	};

	//---------------------------------------------------------

	typedef std::multimap<int, iSaveObject*> tSaveObjectMap;
	typedef tSaveObjectMap::iterator tSaveObjectMapIt;

	typedef cSTLMapIterator<iSaveObject*,tSaveObjectMap,tSaveObjectMapIt> cSaveObjectIterator;

	/**
	* This store all the SaveObjects created at load time.
	*/
	class cSaveObjectHandler
	{
	public:
		cSaveObjectHandler();
		~cSaveObjectHandler();

	public:
		void Add(iSaveObject *pObject);

		iSaveObject* Get(int alId);

		cSaveObjectIterator GetIterator();

		void SetUpAll(cEngine *apGame);

		void Clear();
		size_t Size();

	private:

		tSaveObjectMap m_mapSaveObjects;
	};

	//---------------------------------------------------------

	typedef std::multimap<int, iSaveData*> tSaveDataMap;
	typedef tSaveDataMap::iterator tSaveDataMapIt;

	typedef cSTLMapIterator<iSaveData*,tSaveDataMap,tSaveDataMapIt> cSaveDataIterator;

	/**
	* Used to keep track of save data.
	*/
	class cSaveDataHandler : public iContainer
	{
	public:
		cSaveDataHandler();
		~cSaveDataHandler();

		void Add(iSaveData *pData);

		cSaveDataIterator GetIterator();

		void Clear();
		size_t Size();

	private:
		void AddVoidPtr(void **apPtr);
		void AddVoidClass(void *apClass);

		iContainerIterator* CreateIteratorPtr();

		tSaveDataMap m_mapSaveData;
	};

	//---------------------------------------------------------

};
#endif // HPL_SAVE_GAME_H
