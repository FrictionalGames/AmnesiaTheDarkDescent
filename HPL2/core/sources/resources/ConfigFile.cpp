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

#include "resources/ConfigFile.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/FileSearcher.h"
#include "impl/tinyXML/tinyxml.h"

#include "resources/BinaryBuffer.h"
#include "system/Platform.h"

#include <stdio.h>

namespace hpl {

	#define kCRCKey (0x16AF2C1D)

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cConfigFile::cConfigFile(const tWString& asFile, const tWString& asAltPath)
	{
		if (asAltPath.length() && cPlatform::FileExists(asAltPath + asFile)) {
			msFile = asAltPath + asFile;
		} else {
			msFile = asFile;
		}
		mpXmlDoc = hplNew( TiXmlDocument,() );
		//mpFileSearcher = apFileSearcher;

		mbUseCRC = false;
	}

	cConfigFile::~cConfigFile()
	{
		hplDelete(mpXmlDoc);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cConfigFile::Load()
	{
		////////////////////////////////////
		// CRC Loading
		if(mbUseCRC)
		{
			/////////////////////////////////
			// Load 
			cBinaryBuffer buff;
			if(buff.Load(msFile)==false)
			{
				Error("Unable to open binary config file '%s'!\n", cString::To8Char(msFile).c_str());
				return false;
			}

			/////////////////////////////////
			// CRC
			if(buff.CheckInternalCRC(kCRCKey)==false)
			{
				Error("CRC check for binary config file '%s' failed!\n", cString::To8Char(msFile).c_str());
				return false;
			}

			/////////////////////////////////
			// Get XML
			mpXmlDoc->Parse(buff.GetDataPointerAtCurrentPos());

			if(mpXmlDoc->Error())
			{
				Error("Parsing binary config file '%s' failed!\n", cString::To8Char(msFile).c_str());
				return false;
			}

			return true;
		}
		////////////////////////////////////
		// Normal load
		else
		{
			FILE *pFile = cPlatform::OpenFile(msFile, _W("rb"));

			if(pFile==NULL)
			{
				Error("Unable to open config file '%s'! Invalid filepointer returned!\n", cString::To8Char(msFile).c_str());
				return false;
			}
		
			bool bRet = mpXmlDoc->LoadFile(pFile);

			if(pFile) fclose(pFile);
			return bRet;
		}
	}

	//-----------------------------------------------------------------------

	bool cConfigFile::Save()
	{
		////////////////////////////////////
		// CRC Loading
		if(mbUseCRC)
		{
			/////////////////////////////
			// Get the data
			tString sData;
			sData << *mpXmlDoc;

			/////////////////////////////
			// Compress the data
			cBinaryBuffer buff;

			buff.AddCRC_Begin();

			buff.AddCharArray(sData.c_str(), sData.size()+1);
			
			buff.AddCRC_End(kCRCKey);

			/////////////////////////////
			// Save the file
			if(buff.Save(msFile)==false)
			{
				Error("Unable to save bin config file '%s'!\n", cString::To8Char(msFile).c_str());
				return false;
			}

			return true;
		}
		////////////////////////////////////
		// Normal load
		else
		{
			#ifdef WIN32
						tWString sFile = cString::ReplaceCharToW(msFile, _W("/"), _W("\\"));
						FILE *pFile = _wfopen(sFile.c_str(),_W("w+"));
			#else
						FILE *pFile = fopen(cString::To8Char(msFile).c_str(),"w+");
			#endif

			if(pFile==NULL)
			{
				Error("Unable to save config file '%s'! Invalid filepointer returned!\n", cString::To8Char(msFile).c_str());
				return false;
			}

			bool bRet = mpXmlDoc->SaveFile(pFile);

			if(pFile) fclose(pFile);

			return bRet;
		}
		
	}

	//-----------------------------------------------------------------------

	void cConfigFile::SetString(const tString& asLevel, const tString& asName, const tString& asVal)
	{
		TiXmlElement *pLevelElem = mpXmlDoc->FirstChildElement(asLevel.c_str());
		
		if(pLevelElem==NULL){
			TiXmlElement *pNodeChild = hplNew( TiXmlElement, (asLevel.c_str()) );
			pLevelElem = static_cast<TiXmlElement*>(mpXmlDoc->InsertEndChild(*pNodeChild));
			hplDelete(pNodeChild);
		}
		
		pLevelElem->SetAttribute(asName.c_str(),asVal.c_str());
	}
	//-----------------------------------------------------------------------

	void cConfigFile::SetInt(const tString& asLevel, const tString& asName, int alVal)
	{
		char sBuffer[40];
		sprintf(sBuffer,"%d",alVal);

		SetString(asLevel,asName,sBuffer);
	}
	
	//-----------------------------------------------------------------------
	
	void cConfigFile::SetFloat(const tString& asLevel, const tString& asName, float afVal)
	{
		char sBuffer[40];
		sprintf(sBuffer,"%f",afVal);

		SetString(asLevel,asName,sBuffer);
	}
	
	//-----------------------------------------------------------------------

	void cConfigFile::SetBool(const tString& asLevel, const tString& asName, bool abVal)
	{
		SetString(asLevel,asName,abVal?"true":"false");
	}

	void cConfigFile::SetVector2f(const tString& asLevel, const tString& asName, const cVector2f& avVal)
	{
		tString sVal =	cString::ToString(avVal.x) + " " +
						cString::ToString(avVal.y);
		SetString(asLevel,asName, sVal.c_str());
	}
	void cConfigFile::SetVector3f(const tString& asLevel, const tString& asName, const cVector3f& avVal)
	{
		tString sVal =	cString::ToString(avVal.x) + " " +
						cString::ToString(avVal.y) + " " +
						cString::ToString(avVal.z);
		SetString(asLevel,asName, sVal.c_str());
	}
	void cConfigFile::SetVector2l(const tString& asLevel, const tString& asName, const cVector2l& avVal)
	{
		tString sVal =	cString::ToString(avVal.x) + " " +
						cString::ToString(avVal.y);
		SetString(asLevel,asName, sVal.c_str());
	}
	void cConfigFile::SetVector3l(const tString& asLevel, const tString& asName, const cVector3l& avVal)
	{
		tString sVal =	cString::ToString(avVal.x) + " " +
						cString::ToString(avVal.y) + " " +
						cString::ToString(avVal.z);
		SetString(asLevel,asName, sVal.c_str());
	}

	//-----------------------------------------------------------------------

	tString cConfigFile::GetString(const tString& asLevel, const tString& asName, const tString& asDefault)
	{
		const char *sVal = GetCharArray(asLevel,asName);
		if(sVal==NULL){
			return asDefault;
		}

		return sVal;
	}

	tWString cConfigFile::GetStringW(const tString& asLevel, const tString& asName, const tWString& asDefault)
	{
		tString str = GetString(asLevel,asName,cString::To8Char(asDefault));

		return cString::To16Char(str);
	}

	int cConfigFile::GetInt(const tString& asLevel, const tString& asName, int alDefault)
	{
		const char *sVal = GetCharArray(asLevel,asName);
		if(sVal==NULL) return alDefault;
		
		return cString::ToInt(sVal,alDefault);
	}
	
	float cConfigFile::GetFloat(const tString& asLevel, const tString& asName,float afDefault)
	{
		const char *sVal = GetCharArray(asLevel,asName);
		if(sVal==NULL) return afDefault;

		return cString::ToFloat(sVal,afDefault);
	}

	bool cConfigFile::GetBool(const tString& asLevel, const tString& asName, bool abDefault)
	{
		const char *sVal = GetCharArray(asLevel,asName);
		if(sVal==NULL) return abDefault;
        
		return cString::ToBool(sVal,abDefault);
	}

	//-----------------------------------------------------------------------
	
	cVector2f cConfigFile::GetVector2f(const tString& asLevel, const tString& asName, const cVector2f& avDefault)
	{
		return cString::ToVector2f(GetCharArray(asLevel,asName),avDefault);
	}
	
	cVector3f cConfigFile::GetVector3f(const tString& asLevel, const tString& asName, const cVector3f& avDefault)
	{
		return cString::ToVector3f(GetCharArray(asLevel,asName),avDefault);
	}

	//-----------------------------------------------------------------------

	cVector2l cConfigFile::GetVector2l(const tString& asLevel, const tString& asName, const cVector2l& avDefault)
	{
		return cString::ToVector2l(GetCharArray(asLevel,asName),avDefault);
	}

	cVector3l cConfigFile::GetVector3l(const tString& asLevel, const tString& asName, const cVector3l& avDefault)
	{
		return cString::ToVector3l(GetCharArray(asLevel,asName),avDefault);
	}

	//-----------------------------------------------------------------------


	cColor cConfigFile::GetColor(const tString& asLevel, const tString& asName, const cColor& aDefault)
	{
		return cString::ToColor(GetCharArray(asLevel,asName),aDefault);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	const char* cConfigFile::GetCharArray(tString asLevel, tString asName)
	{
		TiXmlElement *pLevelElem = mpXmlDoc->FirstChildElement(asLevel.c_str());
		if(pLevelElem==NULL){
			return NULL;
		}
		
		return pLevelElem->Attribute(asName.c_str());
	}

    //-----------------------------------------------------------------------
}
