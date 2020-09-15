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

#include "system/Platform.h"

#include "system/String.h"

#include "system/LowLevelSystem.h"

#define UNICODE
#include <windows.h>
#include <shlobj.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <io.h>
#include <direct.h>

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#else
#include "SDL/SDL.h"
#endif

#include "impl/TimerSDL.h"
#include "impl/ThreadWin32.h"
#include "impl/MutexWin32.h"

#include <algorithm>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// FILE HANDLING
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	unsigned long cPlatform::GetFileSize(const tWString& asFileName)
	{
		/*__stat64 fileStat; 
		int lErr = _wstat64( asFileName.c_str(), &fileStat ); 
		if (lErr != 0) return 0; 
		return (unsigned long)fileStat.st_size; */

		FILE *pFile = _wfopen(asFileName.c_str(),_W("rb"));
		if(pFile==NULL)	return 0;
		
		fseek(pFile,0,SEEK_END);
		long lFileSize = ftell(pFile);
		rewind(pFile);

		fclose(pFile);
		return (unsigned long)lFileSize;
	}
	
	//-----------------------------------------------------------------------

	bool cPlatform::CopyFileToBuffer(const tWString& asFileName, void *apBuffer, unsigned long alSize)
	{
		FILE *pFile = _wfopen(asFileName.c_str(),_W("rb"));
		if(pFile==NULL)	return false;
		
		fread(apBuffer, sizeof(char), alSize, pFile);
		
		fclose(pFile);
		return true;
	}

	//-----------------------------------------------------------------------

	bool cPlatform::FileExists(const tWString& asFileName)
	{
		FILE *f = _wfopen(asFileName.c_str(),_W("rb"));
		if(f==NULL)
		{
			return false;
		}

		fclose(f);
		return true;
	}

	//-----------------------------------------------------------------------

	void cPlatform::RemoveFile(const tWString& asFilePath)
	{
		_wremove(asFilePath.c_str());
	}

	//-----------------------------------------------------------------------

	bool cPlatform::CloneFile(	const tWString& asSrcFileName,const tWString& asDestFileName,
		bool abFailIfExists)
	{
		return CopyFile(asSrcFileName.c_str(),asDestFileName.c_str(),abFailIfExists)==TRUE;
	}

	//-----------------------------------------------------------------------

	bool cPlatform::CreateFolder(const tWString& asPath)
	{
		tWString sPath = cString::ReplaceCharToW(asPath,_W("/"), _W("\\"));

		return CreateDirectory(sPath.c_str(),NULL)==TRUE;
	}
	
	//-----------------------------------------------------------------------

	bool cPlatform::RemoveFolder(const tWString& asPath, bool abDeleteAllFiles, bool abDeleteAllSubFolders)
	{
		tWString sPath = cString::ReplaceCharToW(asPath,_W("/"), _W("\\"));

		////////////////////
		// Remove any files in the directory
		if(abDeleteAllFiles)
		{
			tWStringList lstFiles;
            FindFilesInDir(lstFiles,sPath,_W("*.*"), true);
			for(tWStringListIt it = lstFiles.begin(); it != lstFiles.end(); ++it)
			{
				tWString sFilePath = cString::SetFilePathW(*it, sPath);
				RemoveFile(sFilePath);
			}
		}

		////////////////////
		// Remove any sub folders in the directory
		if(abDeleteAllSubFolders)
		{
			tWStringList lstFolders;
			FindFoldersInDir(lstFolders, sPath,true, false);
			for(tWStringListIt it = lstFolders.begin(); it != lstFolders.end(); ++it)
			{
				tWString sFolderPath = cString::SetFilePathW(*it, sPath);
				RemoveFolder(sFolderPath, abDeleteAllFiles,abDeleteAllSubFolders);
			}
		}

		if(RemoveDirectory(sPath.c_str())!=TRUE)
		{
			wchar_t sTempString[2048];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,sTempString,2048,NULL);
			Error("Could not remove folder: '%s': %s",cString::To8Char(sPath).c_str(), cString::To8Char(sTempString).c_str());
			return false;
		}
		return true;

	}

	//-----------------------------------------------------------------------

	bool cPlatform::FolderExists(const tWString& asPath)
	{
		tWString sDir = cString::ReplaceCharToW(asPath, _W("/"), _W("\\"));
		DWORD lFileAttributes = GetFileAttributes(sDir.c_str());
		
		return (lFileAttributes != INVALID_FILE_ATTRIBUTES && (lFileAttributes & FILE_ATTRIBUTE_DIRECTORY) !=0);
	}

	//-----------------------------------------------------------------------

	tWString cPlatform::GetFullFilePath(const tWString& asFilePath)
	{
		wchar_t sOutFilePath[2048];
		int x = GetFullPathNameW(asFilePath.c_str(),2048,sOutFilePath,NULL);
		
		return sOutFilePath;
	}
	
	//-----------------------------------------------------------------------
	
	FILE *cPlatform::OpenFile(const tWString& asFileName, const tWString asMode)
	{
		return _wfopen(asFileName.c_str(), asMode.c_str());
	}

	//-----------------------------------------------------------------------

	static cDate DateFromGMTime(struct tm* apClock)
	{
		cDate date;

		date.seconds = apClock->tm_sec;
		date.minutes = apClock->tm_min;
		date.hours = apClock->tm_hour;
		date.month_day = apClock->tm_mday;
		date.month = apClock->tm_mon;
		date.year = 1900 + apClock->tm_year;
		date.week_day = apClock->tm_wday;
		date.year_day = apClock->tm_yday;

		return date;
	}

	cDate cPlatform::FileModifiedDate(const tWString& asFilePath)
	{
		struct tm* pClock;

		struct _stat attrib;
		_wstat(asFilePath.c_str(), &attrib);

		pClock = gmtime(&(attrib.st_mtime));	// Get the last modified time and put it into the time structure

		cDate date = DateFromGMTime(pClock);

		return date;
	}

	//-----------------------------------------------------------------------

	cDate cPlatform::FileCreationDate(const tWString& asFilePath)
	{
		struct tm* pClock;
		struct _stat attrib;
		_wstat(asFilePath.c_str(), &attrib);

		pClock = gmtime(&(attrib.st_ctime));	// Get the last modified time and put it into the time structure

		cDate date = DateFromGMTime(pClock);

		return date;
	}

	//-----------------------------------------------------------------------

	void cPlatform::FindFilesInDir(tWStringList &alstStrings,const tWString& asDir, const tWString& asMask, bool abAddHidden)
	{
		//these windows functions only works with "\".. sucks ...
		tWString sDir = cString::ReplaceCharToW(asDir,_W("/"),_W("\\"));

		int lLastMaskAsteriskPos = cString::GetLastStringPosW(asMask, _W("*"));
		tWString sMaskExt = cString::ToLowerCaseW(cString::SubW(asMask,lLastMaskAsteriskPos+2));
		bool bMaskNotNull = (sMaskExt!=_W(""));

		//Get the search string
		wchar_t sSpec[256];
		wchar_t end = sDir[sDir.size()-1];

		if(end == _W('\\') || end== _W('/'))
			swprintf(sSpec,256,_W("%s%s"),sDir.c_str(),asMask.c_str());
		else
		{
			//these windows functions only works with "\".. sucks ...
			swprintf(sSpec,256,_W("%s\\%s"),sDir.c_str(),asMask.c_str());
		}

		//The needed structs
		intptr_t lHandle;
		struct _wfinddata_t FileInfo;
		

		//Find the first file:
		lHandle = _wfindfirst(sSpec, &FileInfo );
		if(lHandle==-1L)return;

		//Check so it is not a subdir
		if((FileInfo.attrib & _A_SUBDIR)==0 && (abAddHidden || (FileInfo.attrib & _A_HIDDEN)==0) )
		{
			if(bMaskNotNull)
			{
				if(cString::ToLowerCaseW(cString::GetFileExtW(FileInfo.name))==sMaskExt) alstStrings.push_back(FileInfo.name);
			}
			else
				alstStrings.push_back(FileInfo.name);
		}

		//Get the other files.
		while( _wfindnext( lHandle, &FileInfo ) == 0 )
		{
			if((FileInfo.attrib & _A_SUBDIR)==0 && (abAddHidden || (FileInfo.attrib & _A_HIDDEN)==0) )
			{
				if(bMaskNotNull)
				{
					if(cString::ToLowerCaseW(cString::GetFileExtW(FileInfo.name))==sMaskExt) alstStrings.push_back(FileInfo.name);
				}
				else
					alstStrings.push_back(FileInfo.name);
			}
		}

		//Close the handle to release reosuces and unlock files
		_findclose(lHandle);
	}

	//-----------------------------------------------------------------------

	static bool IsGoodFolder(struct _wfinddata_t* pFileInfo, bool abAddHidden, bool abAddUpFolder)
	{
		if( (pFileInfo->attrib & _A_SUBDIR) ==0) return false;
		if( abAddHidden==false && (pFileInfo->attrib & _A_HIDDEN)) return false;

		tWString sName = pFileInfo->name;
		if( sName == _W("..") ) return abAddUpFolder;
		if( sName == _W(".") ) return false;

		return true;
	}

	void cPlatform::FindFoldersInDir(tWStringList &alstStrings,const tWString& asDir, bool abAddHidden, bool abAddUpFolder)
	{
		//these windows functions only works with "\".. sucks ...
		tWString sDir = cString::ReplaceCharToW(asDir,_W("/"),_W("\\"));

		//Get the search string
		wchar_t sSpec[256];
		wchar_t end = sDir[sDir.size()-1];

		if(end == _W('\\') || end== _W('/'))
			swprintf(sSpec,256,_W("%s%s"),sDir.c_str(),_W("*.*"));
		else
		{
			//these windows functions only works with "\".. sucks ...
			swprintf(sSpec,256,_W("%s\\%s"),sDir.c_str(),_W("*.*"));
		}

		//The needed structs
		intptr_t lHandle;
		struct _wfinddata_t FileInfo;

		//Find the first file:
		lHandle = _wfindfirst(sSpec, &FileInfo );
		if(lHandle==-1L)return;

		//Check so it IS a sub dir
		if( IsGoodFolder(&FileInfo, abAddHidden, abAddUpFolder))
		{
			alstStrings.push_back(FileInfo.name);
		}

		//Get the other files.
		while( _wfindnext( lHandle, &FileInfo ) == 0 )
		{
			if( IsGoodFolder(&FileInfo, abAddHidden, abAddUpFolder) )
			{
				alstStrings.push_back(FileInfo.name);
			}

		}
		
		//Close the handle to release reosuces and unlock files
		_findclose(lHandle);
	}

	//-----------------------------------------------------------------------

	tWString cPlatform::GetWorkingDir()
	{
		tWString sDir = tWString( _wgetcwd(NULL,0));

		return sDir;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// APPLICATION
	//////////////////////////////////////////////////////////////////////////


	//-----------------------------------------------------------------------

	unsigned long cPlatform::GetApplicationTime()
	{
		return SDL_GetTicks();
	}

	//-----------------------------------------------------------------------

	void cPlatform::Sleep ( const unsigned int alMillisecs )
	{
		SDL_Delay ( alMillisecs );
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// DIALOG
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cPlatform::CreateMessageBoxBase(eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, va_list ap)
	{
		wchar_t text[2048];

		if (fmt == NULL)
			return;	
		vswprintf(text, 2047, fmt, ap);

		tWString sMess = _W("");

		sMess += text;

		UINT lType = MB_OK;

		switch (eType)
		{
		case eMsgBoxType_Info:
			lType += MB_ICONINFORMATION;
			break;
		case eMsgBoxType_Error:
			lType += MB_ICONERROR;
			break;
		case eMsgBoxType_Warning:
			lType += MB_ICONWARNING;
			break;
		default:
			break;
		}

		MessageBox( NULL, sMess.c_str(), asCaption, lType );
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SYSTEM STATS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	ePlatform cPlatform::GetPlatform()
	{
		return ePlatform_Win32;
	}

	//-----------------------------------------------------------------------

	tString cPlatform::msName = "Win32";

	//-----------------------------------------------------------------------

	iTimer * cPlatform::CreateTimer()
	{
		return hplNew(cTimerSDL, () );
	}
	
	//-----------------------------------------------------------------------

	cDate cPlatform::GetDate()
	{
		time_t lTime;
		time(&lTime);

		struct tm* pClock;
		pClock = localtime(&lTime);

		return DateFromGMTime(pClock);		
	}

	//-----------------------------------------------------------------------

	void cPlatform::CopyTextToClipboard(const tWString &asText)
	{
		OpenClipboard(NULL);
		EmptyClipboard();

		HGLOBAL clipbuffer;
		wchar_t * pBuffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, (asText.size()+1) *sizeof(wchar_t));
		pBuffer = (wchar_t*)GlobalLock(clipbuffer);
		wcscpy(pBuffer, asText.c_str());
		//GlobalUnlock(clipbuffer);

		SetClipboardData(CF_UNICODETEXT, clipbuffer);

		GlobalUnlock(clipbuffer); 

		CloseClipboard();
	}

	//-----------------------------------------------------------------------

	tWString cPlatform::LoadTextFromClipboard()
	{
		tWString sText=_W("");
		OpenClipboard(NULL);

		HGLOBAL clipbuffer = GetClipboardData(CF_UNICODETEXT);

		wchar_t *pBuffer = (wchar_t*)GlobalLock(clipbuffer);

		if(pBuffer != NULL) sText = pBuffer;

		GlobalUnlock(clipbuffer); 

		CloseClipboard();

		return sText;
	}

	//-----------------------------------------------------------------------

	tWString cPlatform::GetSystemSpecialPath(eSystemPath aPathType)
	{
		tWString sOutput = _W("");
		
		///////////////////////////
		//Get the correction WIN32 varaible
		int type;
		switch(aPathType)
		{
		case eSystemPath_Personal:	type = CSIDL_PERSONAL;
			break;
		default: return _W("");
		}

		///////////////////////////
		//Get string
		TCHAR sPath[2048];
		if(SUCCEEDED(SHGetFolderPath(NULL, type | CSIDL_FLAG_CREATE, NULL,0,sPath))) 
		{
			sOutput = tWString(sPath);
		}
		else
		{
			return _W("");
		}
		
		///////////////////////////////////////
		//Make sure the last char is a separator
		if(	cString::GetLastCharW(sOutput) != _W("/") && 
			cString::GetLastCharW(sOutput) != _W("\\"))
		{
			sOutput += _W("/");
		}

		return sOutput;
	}

	//-----------------------------------------------------------------------

	unsigned long cPlatform::GetSystemAvailableDrives()
	{
		return _getdrives();
	}

	//-----------------------------------------------------------------------

	void cPlatform::GetAvailableVideoModes(tVideoModeVec &avDestVidModes, int alMinBpp, int alMinRefreshRate)
	{
		bool bSkipBppCheck = (alMinBpp==-1);
		bool bSkipRateCheck = (alMinRefreshRate==-1);

		avDestVidModes.clear();

		DEVMODE mode;
		int lModeCounter=0;
		while(EnumDisplaySettings(NULL, lModeCounter, &mode))
		{
			if((bSkipBppCheck || alMinBpp<=(int)mode.dmBitsPerPel) &&
				(bSkipRateCheck || alMinRefreshRate<=(int)mode.dmDisplayFrequency))
			{
				avDestVidModes.push_back(cVideoMode(cVector2l(mode.dmPelsWidth, mode.dmPelsHeight),
													mode.dmBitsPerPel,
													mode.dmDisplayFrequency));
			}

			++lModeCounter;
		}

		sort(avDestVidModes.begin(), avDestVidModes.end(), VideoComp());
	}

	tWString cPlatform::GetDisplayName(int alDisplay)
	{
		return _W("");
	}
	
	void cPlatform::GetDisplayResolution(int alDisplay, int& alHorizontal, int& alVertical)
	{
		if(alDisplay != 0) 
		{
			///////////
			// Only main monitor supported
			alHorizontal = -1;
			alVertical = -1;
		}
		else
		{
			///////////
			// Get current screen resolution
			RECT desktop;
			HWND pDesktop = GetDesktopWindow();
			GetWindowRect(pDesktop, &desktop);

			alHorizontal = desktop.right;
			alVertical = desktop.bottom;
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SYSTEM COMMANDS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cPlatform::OpenBrowserWindow ( const tWString& asURL )
	{
		ShellExecute ( NULL, _W("open"), asURL.c_str(), NULL, NULL, SW_SHOWNORMAL );
	}

	//-----------------------------------------------------------------------

	bool cPlatform::RunProgram( const tWString& asPath, const tWString& asParams )
	{
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		
		memset(&si,0,sizeof(si));
		si.cb= sizeof(si);
		si.wShowWindow = SW_SHOW;

		tWString sCommandLine = asPath + _W(" ") + asParams;

		return CreateProcess(asPath.c_str(), (LPWSTR)sCommandLine.c_str(), NULL, NULL, false, 0, NULL,NULL, &si,&pi)==TRUE;
	}

	//-----------------------------------------------------------------------

	bool cPlatform::OpenFileOnShell( const tWString& asPath )
	{
		int lStatusCode = (int)ShellExecute(NULL, _W("open"), asPath.c_str(), NULL, NULL, SW_SHOWMAXIMIZED);
		bool bRet = (lStatusCode > 32);

		tWString sMessage = _W("Cannot open file '") + asPath + _W("' - \n");
		switch(lStatusCode)
		{
		case SE_ERR_NOASSOC:
			sMessage += _W("No associated app found to open files of type: ") + cString::ToUpperCaseW(cString::GetFileExtW(asPath));
			break;
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			sMessage += _W("File not found");
			break;
		};

		if(bRet==false)
			cPlatform::CreateMessageBox(eMsgBoxType_Error, _W("Error"), sMessage.c_str());

		return bRet;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// THREADING
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iThread* cPlatform::CreateThread(iThreadClass* apThreadClass)
	{
		iThread* pThread = hplNew(cThreadWin32, ());
		pThread->SetThreadClass(apThreadClass);

		return pThread;
	}

	//-----------------------------------------------------------------------

	iMutex* cPlatform::CreateMutEx()
	{
		return hplNew(cMutexWin32, ());
	}


	//-----------------------------------------------------------------------



}
