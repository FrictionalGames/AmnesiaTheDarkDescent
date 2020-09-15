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

#ifndef HPL_LANGUAGE_FILE_H
#define HPL_LANGUAGE_FILE_H

#include <map>
#include "system/SystemTypes.h"

namespace hpl {

	class cResources;

	//--------------------------------

	class cLanguageEntry
	{
	public:
		tWString mwsText;		
	};

	typedef std::map<tString, cLanguageEntry*> tLanguageEntryMap;
	typedef tLanguageEntryMap::iterator tLanguageEntryMapIt;

	//--------------------------------

	class cLanguageCategory
	{
	public:
		~cLanguageCategory(){
			STLMapDeleteAll(m_mapEntries);
		}

		tLanguageEntryMap m_mapEntries;
	};

	typedef std::map<tString, cLanguageCategory*> tLanguageCategoryMap;
	typedef tLanguageCategoryMap::iterator tLanguageCategoryMapIt;

	//--------------------------------

	class cLanguageFile
	{
	public:
		cLanguageFile(cResources *apResources);
		~cLanguageFile();
		
		bool AddFromFile(const tWString& asFile, bool abAddResourceDirs, const tWString& asAltPath = _W(""));
		
		const tWString& Translate(const tString& asCat, const tString& asName);

		tLanguageCategoryMap* GetCategoryMap(){ return &m_mapCategories;}
        
	private:
		tLanguageCategoryMap m_mapCategories;	
		tWString mwsEmpty;

		cResources *mpResources;
	};

};
#endif // HPL_LANGUAGE_FILE_H
