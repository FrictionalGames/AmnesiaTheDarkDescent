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

#include "hpl.h"

using namespace hpl;

cEngine *gpEngine=NULL;

#include "impl/tinyXML/tinyxml.h"

//------------------------------------------

int glTypeOfConv = 0; //0=to text, 1=to lang
bool bSkipTags = false;
tWString gsFilePath = _W("");

//------------------------------------------------

void ParseCommandLine(int argc, const char* argv[])
{
	for(int i=1; i<argc; ++i) //0=prog name
	{
		tString sArg = argv[i];

		//////////////////////////////
		// Type of conversion wanted
		if(i==1 && sArg == "-totxt")
		{
			glTypeOfConv = 0;
		}
		else if(i==1 && sArg == "-tolang")
		{
			glTypeOfConv = 1;
		}
		else if(i==2 && sArg == "-skiptags")
		{
			bSkipTags = true;
		}
		//////////////////////////////
		// The file path
		else
		{
			gsFilePath = cString::To16Char(sArg);
		}
	}
}

//------------------------------------------------

void StringToBinBuffer(cBinaryBuffer *pBinBuff, const tString& asString)
{
	for(size_t i=0; i<asString.size(); ++i)
	{
		pBinBuff->AddUnsignedChar(asString[i]);
	}
}

//------------------------------------------------

void WStringToLangCoding(cBinaryBuffer *pBinBuff, const tWString& asString)
{
	for(size_t i=0; i<asString.size(); ++i)
	{
		wchar_t lWChar = asString[i];
		
        if(lWChar == _W('\n'))
			StringToBinBuffer(pBinBuff, "[br]");
		else if(lWChar > 255)
			StringToBinBuffer(pBinBuff, "[u"+cString::ToString((int)lWChar)+"]");
		else
			pBinBuff->AddUnsignedChar((unsigned char)lWChar);

	}
}

//------------------------------------------------

#define         MASKBITS                0x3F
#define         MASKBYTE                0x80
#define         MASK2BYTES              0xC0
#define         MASK3BYTES              0xE0

void WStringToBinBuffer(cBinaryBuffer *pBinBuff, const tWString& asString)
{
	for(size_t i=0; i<asString.size(); ++i)
	{
		wchar_t lChar = asString[i];

		// 0xxxxxxx
		if(lChar < 0x80)
		{
			pBinBuff->AddUnsignedChar((unsigned char)lChar);
		}
		// 110xxxxx 10xxxxxx
		else if(lChar < 0x800)
		{
			pBinBuff->AddUnsignedChar((unsigned char)(MASK2BYTES | lChar >> 6));
			pBinBuff->AddUnsignedChar((unsigned char)(MASKBYTE | lChar & MASKBITS));
		}
		// 1110xxxx 10xxxxxx 10xxxxxx
		else if(lChar < 0x10000)
		{
			pBinBuff->AddUnsignedChar((unsigned char)(MASK3BYTES | lChar >> 12));
			pBinBuff->AddUnsignedChar((unsigned char)(MASKBYTE | lChar >> 6 & MASKBITS));
			pBinBuff->AddUnsignedChar((unsigned char)(MASKBYTE | lChar & MASKBITS));
		}
	}
}

//------------------------------------------------

void BinBufferToWString(cBinaryBuffer *pBinBuff, tWString& asString)
{
	std::vector<unsigned char> vChars;
	vChars.reserve(pBinBuff->GetSize());
	
	for(size_t i=0; i<pBinBuff->GetSize(); ++i)
	{
		vChars.push_back(pBinBuff->GetUnsignedChar());
	}
    
	for(size_t i=0; i<vChars.size(); )
	{
		wchar_t lChar;

		// 1110xxxx 10xxxxxx 10xxxxxx
		if((vChars[i] & MASK3BYTES) == MASK3BYTES)
		{
			lChar = ((vChars[i] & 0x0F) << 12) | (
				(vChars[i+1] & MASKBITS) << 6)
				| (vChars[i+2] & MASKBITS);
			i += 3;
		}
		// 110xxxxx 10xxxxxx
		else if((vChars[i] & MASK2BYTES) == MASK2BYTES)
		{
			lChar = ((vChars[i] & 0x1F) << 6) | (vChars[i+1] & MASKBITS);
			i += 2;
		}
		// 0xxxxxxx
		else if(vChars[i] < MASKBYTE)
		{
			lChar = vChars[i];
			i += 1;
		}
		asString += lChar;
	}
}

//------------------------------------------------

void ConverLangToText()
{
	//////////////////////////
	// Load lang file
	cLanguageFile *pLangFile = hplNew(cLanguageFile, (gpEngine->GetResources()));
	printf("Loading file %s\n", cString::To8Char(gsFilePath).c_str());
	if(pLangFile->AddFromFile(cString::To8Char(gsFilePath), false)==false)
	{
		hplDelete(pLangFile);
		Error("Could not load '%s'!\n", cString::To8Char(gsFilePath).c_str());
		return;
	}

	//////////////////////////
	// Create bin buffer
	cBinaryBuffer binBuff;
    

	////////////////////////////
	//Iterate lang categories
	tLanguageCategoryMap* pCatMap = pLangFile->GetCategoryMap();
	for(tLanguageCategoryMapIt cat = pCatMap->begin(); cat != pCatMap->end(); ++cat)
	{
		cLanguageCategory *pCat = cat->second;
		if(bSkipTags==false)
		{
			WStringToBinBuffer(&binBuff, _W("###CATEGORY "));
			WStringToBinBuffer(&binBuff,  cString::To16Char(cat->first));
			WStringToBinBuffer(&binBuff, _W("\n"));
		}
		
		for(tLanguageEntryMapIt it = pCat->m_mapEntries.begin(); it != pCat->m_mapEntries.end(); ++it)
		{
			cLanguageEntry *pEntry = it->second;
			if(bSkipTags==false)
			{
				WStringToBinBuffer(&binBuff, _W("### "));
				WStringToBinBuffer(&binBuff, cString::To16Char(it->first));
				WStringToBinBuffer(&binBuff, _W("\n"));
			}
			WStringToBinBuffer(&binBuff, pEntry->mwsText);
			WStringToBinBuffer(&binBuff, _W("\n"));
		}
	}

	////////////////////////////
	//Save bin buffer
	hplDelete(pLangFile);

	tWString sOutputFile = cString::SetFileExtW(cString::GetFileNameW(gsFilePath), _W("txt"));
	printf("Saving to %s\n", cString::To8Char(sOutputFile).c_str());
	if(binBuff.Save(sOutputFile)==false)
	{
		Error("Could not save to '%s'\n", cString::To8Char(sOutputFile).c_str());
	}
}

//------------------------------------------------

void ConverTextToLang()
{
	//////////////////////////
	// Load text bin file
	cBinaryBuffer langBuff;
	printf("Loading file %s\n", cString::To8Char(gsFilePath).c_str());
	if(langBuff.Load(gsFilePath)==false)
	{
		Error("Could not load text file '%s'\n", cString::To8Char(gsFilePath).c_str());
		return;
	}
	
	//////////////////////////
	// Get Text as rows
	tWString sUnicodeData = _W("");
	BinBufferToWString(&langBuff, sUnicodeData);
	tWStringVec vRows;
	tWString sSepp =_W("\n");
	cString::GetStringWVec(sUnicodeData,vRows,&sSepp);
	
	
    cBinaryBuffer binBuff;

	//////////////////////////
	// Write rows to file
	bool bCategoryActive=false;
	bool bEntryActive=false;
	bool bTextActive=false;
	StringToBinBuffer(&binBuff,"<LANGUAGE>\n");
    for(size_t i=0; i<vRows.size(); ++i)
	{
		tWString sRow = vRows[i];

		/////////////////////////
		// Tag
		if(sRow.size() > 3 && cString::SubW(sRow,0,3)==_W("###"))
		{
			//Category
			if(cString::SubW(sRow,3,8)==_W("CATEGORY"))
			{
				if(bEntryActive) StringToBinBuffer(&binBuff, "</Entry>\n");
				if(bCategoryActive) StringToBinBuffer(&binBuff, "</CATEGORY>\n");

				tString sCatName = cString::To8Char(cString::SubW(sRow, 12,-1));
				StringToBinBuffer(&binBuff, "<CATEGORY Name=\""+sCatName+"\">\n");

				bEntryActive=false;
				bCategoryActive=true;
				bTextActive=false;
			}
			//Entry
			else
			{
				if(bEntryActive) StringToBinBuffer(&binBuff, "</Entry>\n");
				
				tString sEntryName = cString::To8Char(cString::SubW(sRow, 4,-1));
				StringToBinBuffer(&binBuff, "<Entry Name=\""+sEntryName+"\">");
				
				bEntryActive=true;
				bTextActive=false;
			}

		}
		/////////////////////////
		// Normal text
		else
		{
			if(bTextActive) StringToBinBuffer(&binBuff, "[br]");
			WStringToLangCoding(&binBuff, sRow);
			bTextActive=true;
		}

	}
	if(bEntryActive) StringToBinBuffer(&binBuff, "</Entry>\n");
	if(bCategoryActive) StringToBinBuffer(&binBuff, "</CATEGORY>");
	StringToBinBuffer(&binBuff,"\n</LANGUAGE>");

	//////////////////////////
	// Save buffer to file
	tWString sOutputFile = cString::SetFileExtW(cString::GetFileNameW(gsFilePath), _W("lang"));
	printf("Saving to %s\n", cString::To8Char(sOutputFile).c_str());
	if(binBuff.Save(sOutputFile)==false)
	{
		Error("Could not save to '%s'\n", cString::To8Char(sOutputFile).c_str());
	}
}

//------------------------------------------------


int main(int argc, const char* argv[])
{
	cEngineInitVars vars;
	gpEngine = CreateHPLEngine(eHplAPI_OpenGL, 0, &vars);
	
	ParseCommandLine(argc, argv);

	if(glTypeOfConv==0)
		ConverLangToText();
	else if(glTypeOfConv==1)
		ConverTextToLang();

	
	DestroyHPLEngine(gpEngine);
	
	return 0;
}
int hplMain(const tString &asCommandline){ return -1;}

