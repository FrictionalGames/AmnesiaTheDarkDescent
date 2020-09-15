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

#include "resources/LanguageFile.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/FileSearcher.h"
#include "impl/tinyXML/tinyxml.h"

#include "resources/Resources.h"
#include "resources/FileSearcher.h"
#include "system/Platform.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cLanguageFile::cLanguageFile(cResources *apResources)
	{
		mpResources = apResources;
	}

	//-----------------------------------------------------------------------

	cLanguageFile::~cLanguageFile()
	{
		STLMapDeleteAll(m_mapCategories);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cLanguageFile::AddFromFile(const tWString& asFile, bool abAddResourceDirs, const tWString& asAltPath)
	{
		const bool bLog =false;
		TiXmlDocument *pDoc = hplNew(TiXmlDocument,() );

		tWString sPath = asFile;
		if (asAltPath.length() > 0 && cPlatform::FileExists(asAltPath + asFile)) {
			sPath = asAltPath + asFile;
		}

		FILE *pFile = cPlatform::OpenFile(sPath, _W("rb"));
		bool bRet = false;
		if (pFile!=NULL) {
			bRet = pDoc->LoadFile(pFile);
			fclose(pFile);
		}

		if (!bRet)
		{
			hplDelete(pDoc);
			Error("Couldn't load language file '%ls'\n",asFile.c_str());
			return false;
		}

		if(bLog) Log("Loading lang file '%ls'\n---------------------\n",asFile.c_str());

		TiXmlElement *pRootElem = pDoc->FirstChildElement();

		///////////////////////////
		//Iterate the resources
		if(abAddResourceDirs)
		{
			TiXmlElement *pResourceElem = pRootElem->FirstChildElement("RESOURCES");
			if(pResourceElem)
			{
				TiXmlElement *pDirElem = pResourceElem->FirstChildElement("Directory");
				for(; pDirElem != NULL; pDirElem = pDirElem->NextSiblingElement("Directory"))
				{
						tString sPath = pDirElem->Attribute("Path");
						if(sPath==""){
							continue;
						}

						bool bAddSubDirs = cString::ToBool(pDirElem->Attribute("AddSubDirs"), true);

						if(sPath[0]=='/' || sPath[0]=='\\') sPath = cString::Sub(sPath, 1);
#ifndef HPL_MINIMAL
						//Log("Adding lang path: '%s' %d\n", sPath.c_str(), bAddSubDirs);
						if (asAltPath.length()) {
							mpResources->AddResourceDir(asAltPath + cString::To16Char(sPath),bAddSubDirs);
						}
						mpResources->AddResourceDir(cString::To16Char(sPath),bAddSubDirs);
#endif
				}
			}
			else
			{
				Warning("No resources element found in '%ls'\n",asFile.c_str());
			}
		}

		
		///////////////////////////
		//Iterate the categories
        TiXmlElement *pCatElem = pRootElem->FirstChildElement("CATEGORY");
		for(; pCatElem != NULL; pCatElem = pCatElem->NextSiblingElement("CATEGORY"))
		{
			tString sCatName = pCatElem->Attribute("Name");

			/////////////////////////////////
			//Get the category or create if it does not exist
            cLanguageCategory *pCategory;
			tLanguageCategoryMapIt CatIt = m_mapCategories.find(sCatName);
			if(CatIt == m_mapCategories.end())
			{
				pCategory = hplNew( cLanguageCategory, () );
				m_mapCategories.insert(tLanguageCategoryMap::value_type(sCatName, pCategory));
				
				if(bLog) Log("Creating category '%s'\n",sCatName.c_str());
			}
			else
			{
				pCategory = CatIt->second;

				if(bLog) Log("Got existing category '%s'\n",sCatName.c_str());
			}

			
			///////////////////////////
            //Iterate the entries
			TiXmlElement *pEntryElem = pCatElem->FirstChildElement("Entry");
			for(; pEntryElem != NULL; pEntryElem = pEntryElem->NextSiblingElement("Entry"))
			{
				tString sEntryName = pEntryElem->Attribute("Name");
				
				///////////////////////////////////
				//Check if the entry already exists, if so, just continue.
				if(pCategory->m_mapEntries.find(sEntryName) != pCategory->m_mapEntries.end())
				{
					if(bLog) Log("Entry '%s' already exist!\n",sEntryName.c_str());

					continue;
				}
				
				////////////////////////
				//Create entry
				cLanguageEntry *pEntry = hplNew( cLanguageEntry, () );
				
				if(bLog) Log("Creating Entry '%s'\n",sEntryName.c_str());


				////////////////////////
				//Add Text
				if(pEntryElem->FirstChild())
				{
					TiXmlText *pTextNode = pEntryElem->FirstChild()->ToText();
					if(pTextNode)
					{
						//pEntry->msText = pTextNode->Value();
						//pEntry->msText = cString::ReplaceStringTo(pEntry->msText,"[br]","\n");

						tString sString = pTextNode->Value();
						pEntry->mwsText = _W("");

						//if(sCatName == "TEST") Log("String: '%s' %d\n",sString.c_str(),sString.size());

						for(size_t i=0; i< sString.length(); ++i)
						{
							unsigned char c = sString[i];
							if(c=='[')
							{
								bool bFoundCommand = true;
								tString sCommand = "";
								int lCount =1;

								while(sString[i+lCount] != ']' && i+lCount<sString.length() && lCount < 16)
								{
									sCommand += sString[i+lCount];
									lCount++;
								}

								if(sCommand=="br")
								{
									pEntry->mwsText += _W('\n');
								}
								else if(sCommand[0]=='u')
								{
									int lNum = cString::ToInt(sCommand.substr(1).c_str(),0);	
									pEntry->mwsText += (wchar_t)lNum;
								}
								else
								{
									bFoundCommand = false;
								}

								//Go forward or add [ to string
								if(bFoundCommand)
								{
									i += lCount;
								}
								else
								{
									pEntry->mwsText += sString[i];
								}
							}
							//Decode UTF-8!
							else if(c >= 128)
							{
								unsigned char c2 = sString[i+1];

								int lNum = c & 0x1f; // c AND 0001 1111
								lNum = lNum << 6;
								lNum = lNum | (c2 & 0x3f);// c AND 0011 1111

								pEntry->mwsText += (wchar_t)lNum;
								++i;
								//Log(" %d: (%x %x) -> %d\n",i,c,c2,(wchar_t)lNum);
							}
							else
							{
								//if(sCatName == "TEST") Log(" %d: %c | %d\n",i,c,c);
								pEntry->mwsText += c;
								//if(sCatName == "TEST") Log(" '%s'\n",cString::To8Char(pEntry->mwsText).c_str());
							}
						}

					}
				}
				//////////
				// No text found, add no text.
				else
				{
					pEntry->mwsText = _W("");
				}

				//if(sE == "Motion blur:") Log("After String: '%s'\n",cString::To8Char(pEntry->mwsText).c_str());

				std::pair<tLanguageEntryMap::iterator,bool> ret = pCategory->m_mapEntries.insert(tLanguageEntryMap::value_type(sEntryName,pEntry));
				if(ret.second==false){
					Warning("Language entry '%s' in category '%s' already exists!\n",sEntryName.c_str(), sCatName.c_str());
					hplDelete(pEntry);
				}
			}
		}

		hplDelete(pDoc);

		/*{
			FILE *pFile = fopen("raw_lang_text.txt","w+");
			
			tLanguageCategoryMapIt CatIt = m_mapCategories.begin();
			for(; CatIt != m_mapCategories.end(); ++CatIt)
			{
				cLanguageCategory *pCategory = CatIt->second;
				tLanguageEntryMapIt EntryIt = pCategory->m_mapEntries.begin();
                for(; EntryIt != pCategory->m_mapEntries.end(); ++EntryIt)				
				{
					cLanguageEntry *pEntry = EntryIt->second;
					fputs(cString::To8Char(pEntry->mwsText + _W("\n\n")).c_str(), pFile);
				}
			}
			
			fclose(pFile);
		}*/


		return true;
	}

	//-----------------------------------------------------------------------

	const tWString& cLanguageFile::Translate(const tString& asCat, const tString& asName)
	{
		tLanguageCategoryMapIt CatIt = m_mapCategories.find(asCat);
		if(CatIt == m_mapCategories.end())
		{
			Warning("Could not find language file category '%s'\n",asCat.c_str());
			return mwsEmpty;
		}
		
        cLanguageCategory *pCategory = CatIt->second;
		tLanguageEntryMapIt EntryIt = pCategory->m_mapEntries.find(asName);
		if(EntryIt == pCategory->m_mapEntries.end())
		{
			Warning("Could not find language file entry '%s'\n",asName.c_str());
			return mwsEmpty;
		}

		cLanguageEntry *pEntry = EntryIt->second;

		return pEntry->mwsText;
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	
	//-----------------------------------------------------------------------
}
