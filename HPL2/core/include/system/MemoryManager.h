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

#ifndef HPL_MEMORY_MANAGER_H
#define HPL_MEMORY_MANAGER_H

#include <map>
#include <string>

namespace hpl {

	//------------------------------------
	
	class cAllocatedPointer
	{
	public:
		cAllocatedPointer(void *apData, const std::string& asFile, int alLine, size_t alMemory);

		std::string msFile;
		int mlLine;
		size_t mlMemory;
		void *mpData;
	};

	//------------------------------------

	typedef std::map<void*, cAllocatedPointer> tAllocatedPointerMap;
	typedef tAllocatedPointerMap::iterator tAllocatedPointerMapIt;
	
	//------------------------------------

	class cMemoryManager
	{
	public:

		static void* AddPointer(const cAllocatedPointer& aAllocatedPointer);
		static void* UpdatePointer(void *apOldData, const cAllocatedPointer& aNewAllocatedPointer);

		static bool RemovePointer(void *apData,const char* apFileString, int alLine);

		/**
		 * Checks if data is valid, and can even be used on sub data (like pData + x )
		 */
		static bool IsValid(void *apData);

		static void LogResults();
		
		static 	tAllocatedPointerMap m_mapPointers;
		static size_t mlTotalMemoryUsage;

		static bool mbLogDeletion;

		template<class T>
		static T* DeleteAndReturn(T* apData)
		{ 
			delete apData;
			return apData;
		}

		template<class T>
		static T* DeleteArrayAndReturn(T* apData)
		{ 
			delete [] apData;
			return apData;
		}

		template<class T>
		static T* FreeAndReturn(T* apData)
		{ 
			free(apData);
			return apData;
		}

		static void SetLogCreation(bool abX);
		static bool GetLogCreation(){ return mbLogCreation;}

		static int GetCreationCount(){ return mlCreationCount;}

	private:
		static bool mbLogCreation;
		static int mlCreationCount;
	};

	//------------------------------------

#ifdef MEMORY_MANAGER_ACTIVE
    
	#define hplNew(classType, constructor) \
			( classType *)hpl::cMemoryManager::AddPointer(hpl::cAllocatedPointer(new classType constructor ,__FILE__,__LINE__,sizeof(classType)))

	#define hplNewArray(classType, amount) \
			( classType *) hpl::cMemoryManager::AddPointer(hpl::cAllocatedPointer(new classType [ amount ] ,__FILE__,__LINE__,amount * sizeof(classType)))

	#define hplMalloc(amount) \
			hpl::cMemoryManager::AddPointer(hpl::cAllocatedPointer(malloc( amount ) ,__FILE__,__LINE__,amount))

	#define hplRealloc(data, amount) \
		hpl::cMemoryManager::UpdatePointer(data, hpl::cAllocatedPointer(realloc( data, amount ) ,__FILE__,__LINE__,amount))


	#define hplDelete(data){ \
			hpl::cMemoryManager::RemovePointer(hpl::cMemoryManager::DeleteAndReturn(data),__FILE__,__LINE__); \
			}//delete data;
				
		
	#define hplDeleteArray(data){ \
			hpl::cMemoryManager::RemovePointer(hpl::cMemoryManager::DeleteArrayAndReturn(data),__FILE__,__LINE__); \
			}//delete [] data;

	#define hplFree(data){ \
			hpl::cMemoryManager::RemovePointer(hpl::cMemoryManager::FreeAndReturn(data),__FILE__,__LINE__); \
			}//free(data);

		
#else
	#define hplNew(classType, constructor) \
			new classType constructor 
	
	#define hplNewArray(classType, amount) \
			new classType [ amount ] 
	
	#define hplMalloc(amount) \
			malloc( amount )

	#define hplRealloc(data, amount) \
			realloc( data, amount )
	
	#define hplDelete(data) \
		delete data;

	#define hplDeleteArray(data) \
		delete [] data;

	#define hplFree(data) \
		free(data);

#endif

	//------------------------------------


};
#endif // HPL_MEMORY_MANAGER_H
