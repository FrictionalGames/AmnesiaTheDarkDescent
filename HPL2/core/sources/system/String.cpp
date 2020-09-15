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

#include "system/String.h"
#include <stdlib.h>
#include <string.h>

#include "system/LowLevelSystem.h"

#if USE_SDL2
#include "SDL2/SDL_stdinc.h"
#else
#include "SDL/SDL_stdinc.h"
#endif

#if defined WIN32
#define SIZEOF_WCHAR 2
#else
#define SIZEOF_WCHAR 4
#endif

SDL_COMPILE_TIME_ASSERT(wchar_t, sizeof(wchar_t) == SIZEOF_WCHAR);

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	
	//-----------------------------------------------------------------------

	tWString cString::To16Char(const tString &asString)
	{
		if ( asString == "" ) return L"";

		tWString wsTemp;

		size_t needed = mbstowcs(NULL,&asString[0],0);
		wsTemp.resize(needed);
		mbstowcs(&wsTemp[0],&asString[0],needed);

		return wsTemp;
	}

	//-----------------------------------------------------------------------
	
	tString cString::To8Char(const tWString &awsString)
	{
        tString sTemp;
		
		#ifdef WIN32
			sTemp.resize(awsString.size());
			for(size_t i=0; i<awsString.length(); ++i)
			{
				if(awsString[i]>255)	sTemp[i] = '_';
				else					sTemp[i] = (unsigned char)awsString[i];
			}
		#else
			size_t needed = wcstombs(NULL,&awsString[0],0);
			if (needed == (size_t)-1) {
				return "";
			}
			sTemp.resize(needed);
			wcstombs(&sTemp[0],&awsString[0],needed);
		#endif

		return sTemp;
	}
    
    tWString cString::UTF8ToWChar(const tString& asString)
    {
        tWString sTemp;

        const char *pCur = asString.c_str();
        const char *pEnd = pCur + asString.size();

        while (pCur < pEnd) {
            Uint32 uTemp;
            if ( (*pCur & 0x80) == 0 ) {
                uTemp = pCur[0];
                pCur+=1;
            }
            else if ( (*pCur & 0xe0) == 0xc0 ) {
                uTemp = (pCur[0] & 0x1f) << 6 | 
                        (pCur[1] & 0x3f);
                pCur+=2;
            }
            else if ( (*pCur & 0xf0) == 0xe0 ) {
                uTemp = (pCur[0] & 0x0f) << 12 | 
                        (pCur[1] & 0x3f) << 6 | 
                        (pCur[2] & 0x3f);
                pCur+=3;
            }
#if SIZEOF_WCHAR == 4
            else if ( (*pCur & 0xf8) == 0xf0 ) {
                uTemp = (pCur[0] & 0x07) << 18 | 
                        (pCur[1] & 0x3f) << 12 | 
                        (pCur[2] & 0x3f) << 6 | 
                        (pCur[3] & 0x3f);
                pCur+=4;
            }
            else if ( (*pCur & 0xfc) == 0xf8 ) {
                uTemp = (pCur[0] & 0x03) << 24 | 
                        (pCur[1] & 0x3f) << 18 | 
                        (pCur[2] & 0x3f) << 12 | 
                        (pCur[3] & 0x3f) << 6 | 
                        (pCur[4] & 0x3f);
                pCur+=5;
            }
            else if ( (*pCur & 0xfe) == 0xfc ) {
                uTemp = (pCur[0] & 0x01) << 30 | 
                        (pCur[1] & 0x3f) << 24 | 
                        (pCur[2] & 0x3f) << 18 | 
                        (pCur[3] & 0x3f) << 12 | 
                        (pCur[4] & 0x3f) << 6 | 
                        (pCur[5] & 0x3f);
                pCur+=6;
            }
#endif
            sTemp.push_back((wchar_t)uTemp);
        }

        return sTemp;
    }

	//-----------------------------------------------------------------------

	tString cString::S16BitToUTF8(const tWString& awsString)
	{
		tString sTemp;
		for(size_t i=0; i<awsString.size(); ++i)
		{
			Uint32 lWChar = awsString[i];
			tString sBuf;

			if(lWChar < 0x80)
			{
				sBuf = char(lWChar);
			}
			else if(lWChar < 0x800)
			{
				sBuf.push_back(((lWChar & 0x7C0) >> 6) | 0xC0);
				sBuf.push_back((lWChar & 0x3F) | 0x80);
			}
			else if(lWChar < 0x10000)
			{
				sBuf.push_back(((lWChar & 0xF000) >> 12) | 0xE0);
				sBuf.push_back(((lWChar & 0xFC0) >> 6) | 0x80);
				sBuf.push_back(((lWChar & 0x3F) | 0x80));
			}
#if SIZEOF_WCHAR == 4
			else if(lWChar < 0x200000)
			{
				sBuf.push_back(((lWChar & 0x1C0000) >> 18) | 0xF0);
				sBuf.push_back(((lWChar & 0x3F000) >> 12) | 0x80);
				sBuf.push_back(((lWChar & 0xFC0) >> 6) | 0x80);
				sBuf.push_back(((lWChar & 0x3F) | 0x80));
			}
			else if(lWChar < 0x4000000)
			{
				sBuf.push_back(((lWChar & 0x3000000) >> 24) | 0xF8);
				sBuf.push_back(((lWChar & 0xFC000) >> 18) | 0x80);
				sBuf.push_back(((lWChar & 0x3F000) >> 12) | 0x80);
				sBuf.push_back(((lWChar & 0xFC0) >> 6) | 0x80);
				sBuf.push_back(((lWChar & 0x3F) | 0x80));
			}
			else if(lWChar < 0x80000000)
			{
				sBuf.push_back(((lWChar & 0x40000000) >> 30) | 0xFC);
				sBuf.push_back(((lWChar & 0x3F000000) >> 24) | 0x80);
				sBuf.push_back(((lWChar & 0xFC000) >> 18) | 0x80);
				sBuf.push_back(((lWChar & 0x3F000) >> 12) | 0x80);
				sBuf.push_back(((lWChar & 0xFC0) >> 6) | 0x80);
				sBuf.push_back(((lWChar & 0x3F) | 0x80));
			}
#endif
			sTemp += sBuf;
		}

		return sTemp;
	}

	//-----------------------------------------------------------------------

	tWString cString::Get16BitFromArray(const tString &asArray)
	{
		// TODOD: needs to be rewritten to be more portal as wchar_t is not ALWAYS 2 bytes. (it's 4 on linux and os x)
		tIntVec vVals;
		GetIntVec(asArray,vVals,NULL);

		tWString wsString=_W(""); wsString.resize(vVals.size());

		for(size_t i=0; i< vVals.size(); ++i)
		{
			wsString[i] = (wchar_t)vVals[i];
		}

		return wsString;
	}
	
	//-----------------------------------------------------------------------

	tString cString::Sub(const tString& asString,int alStart,int alCount)
	{
		int lStringSize = (int)asString.length();
		if(alStart >= lStringSize) return "";
		if(alStart + alCount > lStringSize) alCount = lStringSize - alStart;

		if(alCount<0)	return asString.substr(alStart);
		else			return asString.substr(alStart, alCount);
	}
	tWString cString::SubW(const tWString& asString,int alStart,int alCount)
	{
		int lStringSize = (int)asString.length();
		if(lStringSize==0) return _W("");
		if(alStart >= lStringSize) return _W("");
		if(alStart + alCount > lStringSize) alCount = lStringSize - alStart;

		if(alCount<0)	return asString.substr(alStart);
		else			return asString.substr(alStart, alCount);
	}

	//-----------------------------------------------------------------------
	
	//Get the file extension of a string
	tString cString::GetFileExt(const tString& aString)
	{
		tString sFileName = GetFileName(aString);
		int pos = GetLastStringPos(sFileName,".");
		
		if(pos<0)
			return "";
		else
			return sFileName.substr(pos+1);
	}

	tWString cString::GetFileExtW(const tWString& aString)
	{
		tWString sFileName = GetFileNameW(aString);
		int pos = GetLastStringPosW(sFileName,_W("."));
		
		if(pos<0)
			return _W("");
		else
			return sFileName.substr(pos+1);
	}


	//-----------------------------------------------------------------------

	tString cString::ToLowerCase(const tString& aString)
	{
		tString sTemp;
		sTemp.resize(aString.size());
		for(int i=0;i<(int)aString.size();i++)
		{
			sTemp[i] = tolower(aString[i]);
		}
		return sTemp;
	}

	tWString cString::ToLowerCaseW(const tWString& aString)
	{
		tWString sTemp;
		sTemp.resize(aString.size());
		for(int i=0;i<(int)aString.size();i++)
		{
			sTemp[i] = tolower(aString[i]);
		}
		return sTemp;
	}
	
	//-----------------------------------------------------------------------

	tString cString::ToUpperCase(const tString& aString)
	{
		tString sTemp;
		sTemp.resize(aString.size());
		for(int i=0;i<(int)aString.size();i++)
		{
			sTemp[i] = toupper(aString[i]);
		}
		return sTemp;
	}

	tWString cString::ToUpperCaseW(const tWString& aString)
	{
		tWString sTemp;
		sTemp.resize(aString.size());
		for(int i=0;i<(int)aString.size();i++)
		{
			sTemp[i] = toupper(aString[i]);
		}
		return sTemp;
	}

	//-----------------------------------------------------------------------


	//Set the file extension
	tString cString::SetFileExt(const tString&  aString,const tString&  aExt)
	{
		tString sExt = aExt;
		tString sOutput = aString;
		if(sExt.substr(0,1)==".")sExt = sExt.substr(1);
		if(GetFileExt(aString)!="")
		{
			sOutput = aString.substr(0,GetLastStringPos(aString,"."));
		}

		if(sExt!="")
			sOutput = sOutput + "." + sExt;

		return sOutput;
	}

	tWString cString::SetFileExtW(const tWString&  aString,const tWString&  aExt)
	{
		tWString sExt = aExt;
		tWString sOutput = aString;
		if(sExt.substr(0,1)==_W("."))sExt = sExt.substr(1);
		if(GetFileExtW(aString)!=_W(""))
		{
			sOutput = aString.substr(0,GetLastStringPosW(aString,_W(".")));
		}

		if(sExt!=_W(""))
			sOutput = sOutput + _W(".") + sExt;

		return sOutput;
	}

	//-----------------------------------------------------------------------

	tString cString::SetFilePath(const tString& aString,const tString& aPath)
	{
		tString sSepp="";
		if(GetLastChar(aPath)!="/" && GetLastChar(aPath)!="\\")
			sSepp ="/";

		return aPath +sSepp+ GetFileName(aString);
	}
	
	tWString cString::SetFilePathW(const tWString& aString,const tWString& aPath)
	{
		tWString sSepp=_W("");
		if(GetLastCharW(aPath)!=_W("/") && GetLastCharW(aPath)!=_W("\\"))
			sSepp =_W("/");

		return aPath +sSepp+ GetFileNameW(aString);
	}

	//-----------------------------------------------------------------------

	tString cString::GetRelativePath(const tString& aPath, const tString& aRelativeToPath)
	{
		///////////////
		// Split both paths into folders
		tStringVec vPathFolders, vRelativeFolders;
		tString sSep = "/\\";
		GetStringVec(aPath, vPathFolders, &sSep);
		GetStringVec(aRelativeToPath, vRelativeFolders, &sSep);

		/////////
		// Find difference
		int lDifferenceAt = 0;
		size_t lPathSize = vPathFolders.size();
		size_t lRelativeSize = vRelativeFolders.size();
		size_t lCount = lPathSize < lRelativeSize ? lPathSize : lRelativeSize;

		for(size_t i = 0; i < lCount; ++i)
		{
			if(vPathFolders[i] != vRelativeFolders[i])
			{
				///////
				// Found a difference
				lDifferenceAt = i;
				break;
			}
			else
			{
				//////
				// Update index in case of end of vector
				lDifferenceAt = i + 1;
			}
		}

		if(lDifferenceAt == 0)
		{
			//////////
			// The files are no different partitions, theres no relative path
			return aPath;
		}

		///////////
		// Create relative path
		tString sRelativePath = "";

		size_t lBackUp = lRelativeSize - lDifferenceAt;
		for(size_t i = 0; i < lBackUp; ++i)
		{
			// Move back a step
			sRelativePath += "../";
		}

		for(size_t i = lDifferenceAt; i < lPathSize; ++i)
		{
			// Add the relaitve part
			sRelativePath += vPathFolders[i] + "/";
		}

		return sRelativePath;
	}

	tWString cString::GetRelativePathW(const tWString& aPath, const tWString& aRelativeToPath)
	{
		///////////////
		// Split both paths into folders
		tWStringVec vPathFolders, vRelativeFolders;
		tWString sSep = _W("/\\");
		GetStringVecW(aPath, vPathFolders, &sSep);
		GetStringVecW(aRelativeToPath, vRelativeFolders, &sSep);

		/////////
		// Find difference
		int lDifferenceAt = 0;
		size_t lPathSize = vPathFolders.size();
		size_t lRelativeSize = vRelativeFolders.size();
		size_t lCount = lPathSize < lRelativeSize ? lPathSize : lRelativeSize;

		for(size_t i = 0; i < lCount; ++i)
		{
			if(vPathFolders[i] != vRelativeFolders[i])
			{
				///////
				// Found a difference
				lDifferenceAt = i;
				break;
			}
			else
			{
				//////
				// Update index in case this is end of vector
				lDifferenceAt = i + 1;
			}
		}

		if(lDifferenceAt == 0)
		{
			//////////
			// The files are no different partitions, theres no relative path
			return aPath;
		}

		///////////
		// Create relative path
		tWString sRelativePath = _W("");

		size_t lBackUp = lRelativeSize - lDifferenceAt;
		for(size_t i = 0; i < lBackUp; ++i)
		{
			// Move back a step
			sRelativePath += _W("../");
		}

		for(size_t i = lDifferenceAt; i < lPathSize; ++i)
		{
			// Add the relaitve part
			sRelativePath += vPathFolders[i] + _W("/");
		}

		return sRelativePath;
	}
		
	//-----------------------------------------------------------------------

	tString cString::AddSlashAtEnd(const tString& asPath,char alSlash)
	{
		if(asPath.size()==0) return "";

		char lLastChar = asPath[asPath.size()-1];
		if(lLastChar == '/' || lLastChar == '\\') return asPath;

		return asPath + alSlash;
	}

	tWString cString::AddSlashAtEndW(const tWString& asPath,wchar_t alSlash)
	{
		if(asPath.size()==0) return _W("");

		wchar_t lLastChar = asPath[asPath.size()-1];
		if(lLastChar == _W('/') || lLastChar == _W('\\')) return asPath;

		return asPath + alSlash;
	}

	//-----------------------------------------------------------------------

	tString cString::RemoveSlashAtEnd(const tString& asPath)
	{
		if(asPath.size()==0) return "";

		char lLastChar = asPath[asPath.size()-1];
		if(lLastChar == '/' || lLastChar == '\\') return cString::Sub(asPath,0, asPath.size()-1);

		return asPath;
	}

	tWString cString::RemoveSlashAtEndW(const tWString& asPath)
	{
		if(asPath.size()==0) return _W("");

		wchar_t lLastChar = asPath[asPath.size()-1];
		if(lLastChar == _W('/') || lLastChar == _W('\\')) return cString::SubW(asPath,0, asPath.size()-1);

		return asPath;
	}

	//-----------------------------------------------------------------------


	//Gets the filename in a path
	tString cString::GetFileName(const tString&  aString)
	{
		int pos1 = GetLastStringPos(aString,"\\");
		int pos2 = GetLastStringPos(aString,"/");
		int pos = pos1>pos2 ? pos1 : pos2;

		if(pos<0)
			return aString;
		else
			return aString.substr(pos+1);
	}
	tWString cString::GetFileNameW(const tWString&  aString)
	{
		int pos1 = GetLastStringPosW(aString,_W("\\"));
		int pos2 = GetLastStringPosW(aString,_W("/"));
		int pos = pos1>pos2 ? pos1 : pos2;

		if(pos<0)
			return aString;
		else
			return aString.substr(pos+1);
	}

	//-----------------------------------------------------------------------

	tString cString::GetFilePath(const tString& aString)
	{
		if(GetLastStringPos(aString,".")<0)return aString;

		int pos1 = GetLastStringPos(aString,"\\");
		int pos2 = GetLastStringPos(aString,"/");
		int pos = pos1>pos2 ? pos1 : pos2;

		if(pos<0)
			return "";
		else
			return aString.substr(0, pos+1);

	}

	tWString cString::GetFilePathW(const tWString& aString)
	{
		if(GetLastStringPosW(aString,_W("."))<0) return aString;

		int pos1 = GetLastStringPosW(aString,_W("\\"));
		int pos2 = GetLastStringPosW(aString,_W("/"));
		int pos = pos1>pos2 ? pos1 : pos2;

		if(pos<0)
			return _W("");
		else
			return aString.substr(0, pos+1);

	}

	//-----------------------------------------------------------------------

	tString cString::ReplaceCharTo(const tString& aString, const tString& asOldChar,const tString& asNewChar)
	{
		if(asNewChar !="")
		{
			tString sNewString = aString;
			
			for(int i=0;i<(int)aString.size();i++)
			{
				if(aString[i] == asOldChar[0]) sNewString[i] = asNewChar[0];
			}
			return sNewString;
		}
		else
		{
			tString sNewString;
			sNewString.reserve(aString.size());

			for(int i=0;i<(int)aString.size();i++)
			{
				if(aString[i] != asOldChar[0]) sNewString.push_back(aString[i]);
			}
			return sNewString;
		}
	}

	tWString cString::ReplaceCharToW(const tWString& aString, const tWString& asOldChar,const tWString& asNewChar)
	{
		if(asNewChar !=_W(""))
		{
			tWString sNewString  = aString;
			for(int i=0;i<(int)aString.size();i++)
			{
				if(aString[i] == asOldChar[0]) sNewString[i] = asNewChar[0];
			}
			return sNewString;
		}
		else
		{
			tWString sNewString;
			sNewString.reserve(aString.size());

			for(int i=0;i<(int)aString.size();i++)
			{
				if(aString[i] != asOldChar[0]) sNewString.push_back(aString[i]);
			}
			return sNewString;
		}
	}

	//-----------------------------------------------------------------------

	tString cString::ReplaceStringTo(const tString& asString, const tString& asOldString,const tString& asNewString)
	{
		tString sNewString = "";
		
		for(size_t i=0;i<asString.size();i++)
		{
			bool bFound = true;
			//Search for old string
			if(asString.size() >= i + asOldString.size())
			{
				for(size_t j=0; j<asOldString.size(); ++j)
				{
					if(asString[i+j] != asOldString[j]) {
						bFound = false;
						break;
					}
				}
			}
			else
			{
				bFound = false;
			}
			
            //Insert new string
			if(bFound)
			{
				sNewString += asNewString;
				i += asOldString.size()-1;
			}
			//Just add the character
			else {
				sNewString += asString[i];
			}
		}
		return sNewString;
	}

	//-----------------------------------------------------------------------

	//gets the last char in the string
	tString cString::GetLastChar(const tString& aString)
	{
		if(aString.size()==0) return "";
		return aString.substr(aString.size()-1); 
	}

	tWString cString::GetLastCharW(const tWString& aString)
	{
		if(aString.size()==0) return _W("");
		return aString.substr(aString.size()-1); 
	}

	//-----------------------------------------------------------------------

	tString cString::ToString(const char* asString,tString asDefault)
	{
		if(asString==NULL)return asDefault;

		return asString;
	}
	
	//-----------------------------------------------------------------------

	int cString::ToInt(const char* asString,int alDefault)
	{
		if(asString==NULL)return alDefault;

		return atoi(asString);
	}

	//-----------------------------------------------------------------------

	float cString::ToFloat(const char* asString,float afDefault)
	{
		if(asString==NULL)return afDefault;

		return (float)atof(asString);
	}

	//-----------------------------------------------------------------------

	bool cString::ToBool(const char* asString, bool abDefault)
	{
		if(asString==NULL)return abDefault;

		tString asTempString = ToLowerCase(asString);
		return asTempString == "true"?true:false;
	}

	//-----------------------------------------------------------------------

	cColor cString::ToColor(const char* asString, const cColor& aDefault)
	{
		if(asString==NULL)return aDefault;

		tFloatVec vValues;

		GetFloatVec(asString,vValues,NULL);
        
		if(vValues.size() != 4) return aDefault;

		return cColor(vValues[0],vValues[1],vValues[2],vValues[3]);
	}
	
	//-----------------------------------------------------------------------

	cVector2f cString::ToVector2f(const char* asString, const cVector2f& avDefault)
	{
		if(asString==NULL) return avDefault;

		tFloatVec vValues;

		GetFloatVec(asString,vValues,NULL);

		if(vValues.size() != 2) return avDefault;

		return cVector2f(vValues[0],vValues[1]);
	}

	//-----------------------------------------------------------------------

	cVector3f cString::ToVector3f(const char* asString, const cVector3f& avDefault)
	{
		if(asString==NULL) return avDefault;

		tFloatVec vValues;

		GetFloatVec(asString,vValues,NULL);

		if(vValues.size() != 3) return avDefault;

		return cVector3f(vValues[0],vValues[1],vValues[2]);
	}

	//-----------------------------------------------------------------------

	cVector2l cString::ToVector2l(const char* asString, const cVector2l& avDefault)
	{
		if(asString==NULL) return avDefault;
		
		tIntVec vValues;

		GetIntVec(asString,vValues,NULL);

		if(vValues.size() != 2) return avDefault;

		return cVector2l(vValues[0],vValues[1]);
	}
	
	//-----------------------------------------------------------------------

	cVector3l cString::ToVector3l(const char* asString, const cVector3l& avDefault)
	{
		if(asString==NULL) return avDefault;
		
		tIntVec vValues;

		GetIntVec(asString,vValues,NULL);

		if(vValues.size() != 3) return avDefault;

		return cVector3l(vValues[0],vValues[1],vValues[2]);
	}

	//-----------------------------------------------------------------------

	cMatrixf cString::ToMatrixf(const char* asString, const cMatrixf& a_mtxDefault)
	{
		if(asString==NULL) return a_mtxDefault;

		tFloatVec vValues;

		GetFloatVec(asString,vValues,NULL);

		if(vValues.size() != 16) return a_mtxDefault;

		return cMatrixf(vValues[0],vValues[1],vValues[2],vValues[3],
						vValues[4],vValues[5],vValues[6],vValues[7],
						vValues[8],vValues[9],vValues[10],vValues[11],
						vValues[12],vValues[13],vValues[14],vValues[15]);
	}

	//-----------------------------------------------------------------------

	tIntVec& cString::GetIntVec(const tString &asData, tIntVec& avVec,tString *apSeparators)
	{
		tStringVec mvStr;
		GetStringVec(asData, mvStr, apSeparators);

		for(int i=0;i<(int)mvStr.size();i++)
		{
			avVec.push_back(ToInt(mvStr[i].c_str(),0) );
		}

		return avVec;
	}

	//-----------------------------------------------------------------------

	tUIntVec& cString::GetUIntVec(const tString &asData, tUIntVec& avVec,tString *apSeparators)
	{
		tStringVec mvStr;
		GetStringVec(asData, mvStr, apSeparators);

		for(int i=0;i<(int)mvStr.size();i++)
		{
			avVec.push_back(ToInt(mvStr[i].c_str(),0) );
		}

		return avVec;
	}


	//-----------------------------------------------------------------------

	tFloatVec& cString::GetFloatVec(const tString &asData, tFloatVec& avVec,tString *apSeparators)
	{
		tStringVec mvStr;
		GetStringVec(asData, mvStr,apSeparators);

		for(int i=0;i<(int)mvStr.size();i++)
		{
			avVec.push_back(ToFloat(mvStr[i].c_str(),0) );
		}

		return avVec;
	}
	
	//-----------------------------------------------------------------------

	tString cString::ToString(int alX, int alPaddingZeros)
	{
		char buff[256];
		
        sprintf(buff, "%0*d" , alPaddingZeros, alX);

		return buff;
	}

	tString cString::ToString(unsigned int alX, int alPaddingZeros)
	{
		char buff[256];

		sprintf(buff, "%0*u" , alPaddingZeros, alX);

		return buff;
	}

	tString cString::ToString(unsigned long alX, int alPaddingZeros)
	{
		char buff[256];

		sprintf(buff, "%0*lu", alPaddingZeros, alX);

		return buff;
	}

	tString cString::ToString(float afX, int alNumDecimals, bool abRemoveTrailingZeros)
	{
		char buff[256];
		if(alNumDecimals<0) alNumDecimals = 20;

		////////////////////////////////////////////////////////////
		// Print the float into a string, using a given precision
		sprintf(buff, "%.*f", alNumDecimals, afX);
		
		//////////////////////////////////////////
		// Clean up decimal part
		//  Search for '.' char
		char *p = strchr (buff,'.');
		if(p != NULL) 
		{
			// Check if there are less decimals than what precision states
			while(alNumDecimals >= 0) 
			{
				alNumDecimals--;
				if(*p == '\0')    
					break;
				p++;
			}
			
			///////////////////////
			// Truncate string
			*p-- = '\0';
		
			///////////////////////////////////////
			// Remove trailing zeros on demand
			if(abRemoveTrailingZeros)
			{
				// Overwrite any '0' chars with the null character
				while(*p == '0')
					*p-- = '\0';
				
				// If all decimals were zero, remove "."
				if(*p == '.')
					*p = '\0';
			}
		}
		
		return buff;
	}
	
	//-----------------------------------------------------------------------

	tWString cString::ToStringW(int alX, int alPaddingZeros)
	{
		wchar_t buff[256];

		tWString sFormat = _W("%0*d");

#ifdef WIN32
		swprintf(buff, sFormat.c_str(), alPaddingZeros, alX);
#else
		swprintf(buff, 256, sFormat.c_str(), alPaddingZeros, alX);
#endif

		return buff;
	}

	tWString cString::ToStringW(unsigned long alX, int alPaddingZeros)
	{
		wchar_t buff[256];

		tWString sFormat = _W("%0*ld");

#ifdef WIN32
		swprintf(buff, sFormat.c_str(), alPaddingZeros, alX);
#else
		swprintf(buff, 256, sFormat.c_str(), alPaddingZeros, alX);
#endif

		return buff;
	}

	tWString cString::ToStringW(float afX, int alNumDecimals, bool abRemoveTrailingZeros)
	{
		wchar_t buff[256];
		if(alNumDecimals<0) alNumDecimals = 20;

		////////////////////////////////////////////////////////////
		// Print the float into a wstring, using a given precision
		tWString sFormat = _W("%.*f");
#ifdef WIN32
		swprintf(buff, sFormat.c_str(), alNumDecimals ,afX);
#else
		swprintf(buff, 256, sFormat.c_str(), alNumDecimals, afX);
#endif

		//////////////////////////////////////////
		// Clean up decimal part
		//  Search for '.' char
		wchar_t *p = wcschr (buff, _W('.'));
		if(p != NULL) 
		{
			// Check if there are less decimals than what precision states
			while(alNumDecimals >= 0) 
			{
				alNumDecimals--;
				if(*p == _W('\0'))
					break;
				p++;
			}
			
			///////////////////////
			// Truncate string
			*p-- = _W('\0');
		
			///////////////////////////////////////
			// Remove trailing zeros on demand
			if(abRemoveTrailingZeros)
			{
				// Overwrite any '0' chars with the null character
				while(*p == _W('0'))
					*p-- = _W('\0');
				
				// If all decimals were zero, remove "."
				if(*p == _W('.'))
					*p = _W('\0');
			}
		}

		return buff;
	}

	//-----------------------------------------------------------------------

	tStringVec& cString::GetStringVec(const tString &asData, tStringVec& avVec,tString *apSeparators)
	{
		tString str = "";
		bool start = false;
		tString c = "";

		for(int i=0;i<(int)asData.length();i++)
		{
			c = asData.substr(i,1);
			bool bNewWord = false;
			
			//Check if the current char is a separator
			if(apSeparators)
			{
				for(size_t j=0; j< apSeparators->size(); j++)
				{
					if((*apSeparators)[j] == c[0])
					{
						bNewWord = true;
						break;
					}
				}
			}
			else
			{
				if(c[0]==' ' || c[0]=='\n' || c[0]=='\r' || c[0]=='\t' || c[0]==',')
				{
					bNewWord = true;	
				}
			}
			
			if(bNewWord)
			{
				if(start)
				{
					start = false;
					avVec.push_back(str);
					str = "";
				}
			}
			else
			{
				start = true;
				str +=c;
				if(i==asData.length()-1)avVec.push_back(str);
			}
		}	
	
		return avVec;
	}

	//-----------------------------------------------------------------------

	tWStringVec& cString::GetStringVecW(const tWString &asData, tWStringVec& avVec,tWString *apSeparators)
	{
		tWString str = _W("");
		bool start = false;
		wchar_t c = ' ';

		for(int i=0;i<(int)asData.length();i++)
		{
			c = asData[i];
			bool bNewWord = false;

			//Check if the current char is a separator
			if(apSeparators)
			{
				for(size_t j=0; j< apSeparators->size(); j++)
				{
					if((*apSeparators)[j] == c)
					{
						bNewWord = true;
						break;
					}
				}
			}
			else
			{
				if(c==' ' || c=='\n' || c=='\r' || c=='\t' || c==',')
				{
					bNewWord = true;	
				}
			}

			if(bNewWord)
			{
				if(start)
				{
					start = false;
					avVec.push_back(str);
					str = _W("");
				}
			}
			else
			{
				start = true;
				str +=c;
				if(i==asData.length()-1)avVec.push_back(str);
			}
		}	

		return avVec;
	}

	//-----------------------------------------------------------------------

	tWStringVec& cString::GetStringWVec(const tWString &asData, tWStringVec& avVec,tWString *apSeparators)
	{
		tWString str = _W("");
		bool start = false;
		tWString c = _W("");

		for(int i=0;i<(int)asData.length();i++)
		{
			c = asData.substr(i,1);
			bool bNewWord = false;
			
			//Check if the current char is a separator
			if(apSeparators)
			{
				for(size_t j=0; j< apSeparators->size(); j++)
				{
					if((*apSeparators)[j] == c[0])
					{
						bNewWord = true;
						break;
					}
				}
			}
			else
			{
				if(c[0]==_W(' ') || c[0]==_W('\n') || c[0]==_W('\t') || c[0]==_W(','))
				{
					bNewWord = true;	
				}
			}
			
			if(bNewWord)
			{
				if(start)
				{
					start = false;
					avVec.push_back(str);
					str = _W("");
				}
			}
			else
			{
				start = true;
				str +=c;
				if(i==asData.length()-1)avVec.push_back(str);
			}
		}	
	
		return avVec;
	}

	//-----------------------------------------------------------------------

	///Helper
	//returns first char in a string
	int cString::GetFirstStringPos(const tString& aString, const tString&  aChar)
	{	
		int pos=-1;
		for(int i=0;i<(int)aString.size();i++)
		{
			if(aString.substr(i,aChar.size())==aChar)
			{
				pos=i;
				break;
			}
		}
		return pos;
	}

	///Helper
	//returns first char in a string
	int cString::GetFirstStringPosW(const tWString& aString, const tWString&  aChar)
	{	
		int pos=-1;
		for(int i=0;i<(int)aString.size();i++)
		{
			if(aString.substr(i,aChar.size())==aChar)
			{
				pos=i;
				break;
			}
		}
		return pos;
	}


	///Helper
	//returns last char in a string
	int cString::GetLastStringPos(const tString& aString, const tString&  aChar)
	{	
		int pos=-1;
		for(int i=0;i<(int)aString.size();i++)
		{
			if(aString.substr(i,aChar.size())==aChar)
				pos=i;
		}
		return pos;
	}

	///Helper
	//returns last char in a string
	int cString::GetLastStringPosW(const tWString& aString, const tWString&  aChar)
	{	
		int pos=-1;
		for(int i=0;i<(int)aString.size();i++)
		{
			if(aString.substr(i,aChar.size())==aChar)
				pos=i;
		}
		return pos;
	}

	//-----------------------------------------------------------------------

	void cString::UIntStringToArray(unsigned int *apArray, const char* apString,int alSize)
	{
		char vTempChar[20];
		int lTempCharCount=0;

		int lArrayCount=0;
		int lStringCount =0;

		while(lArrayCount < alSize)
		{
			char c= apString[lStringCount];

			//if a space is found, convert the previous characters to a float.
			if(c == ' ' || c == 0)
			{
				if(lTempCharCount>0)
				{
					vTempChar[lTempCharCount] =0;
					apArray[lArrayCount] = (unsigned int) atoi(vTempChar);
					
					lTempCharCount=0;
					lArrayCount++;
				}
			}
			//If character is not a space, add to temp char.
			else
			{
				vTempChar[lTempCharCount] = c;
				lTempCharCount++;
			}

			lStringCount++;
		}
	}

	//-----------------------------------------------------------------------

	void cString::FloatStringToArray(float *apArray, const char* apString,int alSize)
	{
		char vTempChar[20];
		int lTempCharCount=0;

		int lArrayCount=0;
		int lStringCount =0;

		while(lArrayCount < alSize)
		{
			char c= apString[lStringCount];
			//if a space is found, convert the previous characters to a float.
			if(c == ' ' || c == 0)
			{
				if(lTempCharCount>0)
				{
					vTempChar[lTempCharCount] =0;
					apArray[lArrayCount] = (float)atof(vTempChar);

					lTempCharCount=0;
					lArrayCount++;
				}
			}
			//If character is not a space, add to temp char.
			else
			{
				vTempChar[lTempCharCount] = apString[lStringCount];
				lTempCharCount++;
			}

			lStringCount++;
		}
	}
	
	//-----------------------------------------------------------------------

	int cString::CountCharsInString(const tString& aString, const tString& aChar)
	{
		int lCount = 0;

		for(int i=0;i<(int)aString.size();++i)
		{
			if(aString[i]==aChar[0])
			{
				++lCount;
			}
		}

		return lCount;

	}

	//-----------------------------------------------------------------------

	
	int cString::CountCharsInStringW(const tWString& aString, const tWString& aChar)
	{
		int lCount = 0;

		for(int i=0;i<(int)aString.size();++i)
		{
			if(aString[i]==aChar[0])
			{
				++lCount;
			}
		}

		return lCount;

	}

	//-----------------------------------------------------------------------

	//ALGO FROM: http://www.azillionmonkeys.com/qed/lHash.html
	// by Paul Hsieh
	
	#define Get16Bits(data) (*((const wchar_t *) (data)))

	unsigned int cString::GetHash(const tString& asStr)
	{
		unsigned int lLen = (unsigned int)asStr.size();
		const char *pData = asStr.c_str();
		unsigned int lHash = lLen, lTemp;
		int lRem;

		if (asStr.empty()) return 0;

		lRem = lLen & 3;
		lLen >>= 2;

		//////////////////////////////////
		//Main loop
		for (;lLen > 0; lLen--)
		{
			lHash  += Get16Bits(pData);
			lTemp    = ( Get16Bits(pData+2) << 11) ^ lHash;
			lHash   = (lHash << 16) ^ lTemp;
			pData  += 2*sizeof (wchar_t);
			lHash  += lHash >> 11;
		}

		//////////////////////////////////
		// Handle end cases
		switch (lRem) 
		{
		case 3: 
			lHash += Get16Bits(pData);
			lHash ^= lHash << 16;
			lHash ^= pData[sizeof (wchar_t)] << 18;
			lHash += lHash >> 11;
			break;
		case 2: 
			lHash += Get16Bits(pData);
			lHash ^= lHash << 11;
			lHash += lHash >> 17;
			break;
		case 1: 
			lHash += *pData;
			lHash ^= lHash << 10;
			lHash += lHash >> 1;
		}

		//////////////////////////////////
		// Force "avalanching" of final 127 bits
		lHash ^= lHash << 3;
		lHash += lHash >> 5;
		lHash ^= lHash << 4;
		lHash += lHash >> 17;
		lHash ^= lHash << 25;
		lHash += lHash >> 6;

		return lHash;
	}
	
	//-----------------------------------------------------------------------
	
	unsigned int cString::GetHashW(const tWString& asStr)
	{
		unsigned int lLen = (unsigned int)asStr.size();
		const wchar_t *pData = asStr.c_str();
		unsigned int lHash = lLen, lTemp;
		int lRem;

		if (asStr.empty()) return 0;

		lRem = lLen & 1;
		lLen >>= 1;

		//////////////////////////////////
		//Main loop
		for (;lLen > 0; lLen--)
		{
			lHash  += pData[0];
			lTemp    = ( pData[1] << 11) ^ lHash;
			lHash   = (lHash << 16) ^ lTemp;
			pData  += 2;
			lHash  += lHash >> 11;
		}

		//////////////////////////////////
		// Handle end cases
		if(lRem) 
		{
			lHash += *pData;
			lHash ^= lHash << 10;
			lHash += lHash >> 1;
		}

		//////////////////////////////////
		// Force "avalanching" of final 127 bits
		lHash ^= lHash << 3;
		lHash += lHash >> 5;
		lHash ^= lHash << 4;
		lHash += lHash >> 17;
		lHash ^= lHash << 25;
		lHash += lHash >> 6;

		return lHash;
	}

	//-----------------------------------------------------------------------

	tString cString::GetNumericSuffix(const tString& aString, int* apIndex)
	{
		const char* str = aString.c_str();
		int lIndex=-1;
		int lStringSize=(int)aString.size();
		for(int i=lStringSize-1;i>=0;--i)
		{
			if(str[i]<'0' || str[i]>'9')
			{
				lIndex = ++i;
				if(lIndex>=lStringSize) lIndex=-1;
				break;
			}
		}
		if(apIndex)
			*apIndex=lIndex;

		if(lIndex!=-1)
			return Sub(aString,lIndex);
		else
			return "";
	}

	//-----------------------------------------------------------------------

	tWString cString::GetNumericSuffixW(const tWString& aString, int* apIndex)
	{
		const wchar_t* str = aString.c_str();
		int lIndex=-1;
		int lStringSize=(int)aString.size();
		for(int i=lStringSize-1;i>=0;--i)
		{
			if(str[i]<_W('0') || str[i]>_W('9'))
			{
				lIndex = ++i;
				if(lIndex>=lStringSize) lIndex=-1;
				break;
			}
		}
		if(apIndex)
			*apIndex=lIndex;

		if(lIndex!=-1)
			return SubW(aString,lIndex);
		else
			return _W("");
	}

	//-----------------------------------------------------------------------
	
	static tString EncodeTextStringDataChunk(int alChunk)
	{
		char vChars[7];
		vChars[6] = 0; //string terminator

		#ifdef BIGENDIAN
			vChars[3] = (unsigned char)((alChunk>>0 ) & 0x000000FF );
			vChars[2] = (unsigned char)((alChunk>>8 ) & 0x000000FF );
			vChars[1] = (unsigned char)((alChunk>>16) & 0x000000FF );
			vChars[0] = (unsigned char)((alChunk>>24) & 0x000000FF );
		#else
			//Need to be in separate order! (because of little endian)
			vChars[0] = (unsigned char)((alChunk>>0 ) & 0x000000FF );
			vChars[1] = (unsigned char)((alChunk>>8 ) & 0x000000FF );
			vChars[2] = (unsigned char)((alChunk>>16) & 0x000000FF );
			vChars[3] = (unsigned char)((alChunk>>24) & 0x000000FF );
		#endif

		//Save the padding for higher bytes
		vChars[4] = (vChars[2]>>2) & 0x30;
		vChars[5] = (vChars[3]>>2) & 0x30;

		//Save the padding for lower bytes
		vChars[4] |=  (vChars[0] >> 4) & 0x0C; 
		vChars[5] |=  (vChars[1] >> 4) & 0x0C; 

		//Convert to printable characters
		for(int i=0;i<6; ++i)
		{
			vChars[i] &= 63;
			if(vChars[i] != 63) vChars[i] += 64;
		}

		return tString(vChars);
	}

	void cString::EncodeDataToTextString(const void *apData, size_t alSize, tString &asOutput)
	{
		if(alSize %4 != 0) return;//Need to be dividable by 4!

		size_t lCount = alSize;
		asOutput ="";
		const int *pDataChunk = (int*)apData;

		while(lCount > 0)
		{
			int lChunk = *((int*)pDataChunk);

			asOutput += EncodeTextStringDataChunk(lChunk);

			++pDataChunk;
			lCount -= 4;
		}
	}
	
	//-----------------------------------------------------------------------
	
	//Chars must contain 6 characters!
	static int DecodeTextStringDataChunk(const char* apCharChunk)
	{
		int lFront = *((int*)&apCharChunk[0]);
		int lBack = *((short*)&apCharChunk[4]);

		//Clear spaces
		lFront &= 0x3F3F3F3F; 

		//Set ones in spaces
		lFront |= ((lBack << 18) | (lBack << 4)) & 0xC0C0C0C0;

		return lFront;
	}

	void cString::DecodeDataFromTextString(const tString &asStr, void *apDest, size_t alSize)
	{
		if(alSize %4 != 0) return;				//Need to be dividable by 4!
		if(asStr.size() < (alSize/4)*6) return; //String not large enough!

		const char *pDataChunk = asStr.c_str();
		int *pDestChunk = (int*)apDest;
		size_t lCount = alSize;

		while(lCount>0)
		{
			*pDestChunk = DecodeTextStringDataChunk(pDataChunk);

			pDataChunk += 6;
			++pDestChunk;
			lCount -= 4;
		}
	}

	//-----------------------------------------------------------------------

	tString cString::GetFormatOptions(const tString& asDataType, int alZeroesOnLeft, int alNumDecimals)
	{
		tString sFormat = "%";
		if(alZeroesOnLeft)
			sFormat += "0" + cString::ToString(alZeroesOnLeft);
		if(alNumDecimals>=0)
			sFormat += "." + cString::ToString(alNumDecimals);
		sFormat += asDataType;

		return sFormat;
	}

	tWString cString::GetFormatOptionsW(const tString& asDataType, int alZeroesOnLeft, int alNumDecimals)
	{
		tString sTemp = GetFormatOptions(asDataType, alZeroesOnLeft, alNumDecimals);

		return cString::To16Char(sTemp);
	}

	//-----------------------------------------------------------------------

}
