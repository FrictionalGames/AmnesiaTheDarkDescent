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

#include <sys/stat.h>
#include <dirent.h>
#include <sys/param.h>
#include <fstream>

#if USE_SDL2
#include "SDL2/SDL.h"
#else
#include "SDL/SDL.h"
#endif

#ifdef __linux__
#include <FL/fl_ask.H>
#include "binreloc.h"

#include <sys/types.h>
#endif
#include <unistd.h>
#include <sys/time.h>

#include <algorithm>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// FILE HANDLING
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	unsigned long cPlatform::GetFileSize(const tWString& asFileName)
	{
		struct stat statbuf;
		if (stat(cString::To8Char(asFileName).c_str(), &statbuf) == -1) {
			return 0;
		};
		return statbuf.st_size;
	}
	
	//-----------------------------------------------------------------------

	bool cPlatform::CopyFileToBuffer(const tWString& asFileName, void *apBuffer, unsigned long alSize)
	{
		FILE *pFile = OpenFile(asFileName, _W("r"));
		if (pFile==NULL) return false;
		fread(apBuffer, sizeof(char), alSize, pFile);
		
		fclose(pFile);
		return true;
	}

	//-----------------------------------------------------------------------

	bool cPlatform::FileExists(const tWString& asFileName)
	{
		struct stat statbuf;
		return (lstat(cString::To8Char(asFileName).c_str(), &statbuf) == 0);
	}

	//-----------------------------------------------------------------------

	void cPlatform::RemoveFile(const tWString& asFilePath)
	{
		int ret = unlink(cString::To8Char(asFilePath).c_str());
	}

	//-----------------------------------------------------------------------
#define COPY_BUFFSIZE 8192

	bool cPlatform::CloneFile(	const tWString& asSrcFileName,const tWString& asDestFileName,
		bool abFailIfExists)
	{
		std::ifstream IN (cString::To8Char(asSrcFileName).c_str(), std::ios::binary);
		std::ofstream OUT (cString::To8Char(asDestFileName).c_str(), std::ios::binary);
		OUT << IN.rdbuf();
		OUT.flush();
		return true;
	}

	//-----------------------------------------------------------------------

	bool cPlatform::CreateFolder(const tWString& asPath)
	{
		return mkdir(cString::To8Char(asPath).c_str(),0755) == 0;
	}
	
	//-----------------------------------------------------------------------

	bool cPlatform::RemoveFolder(const tWString& asPath, bool abDeleteAllFiles, bool abDeleteAllSubFolders)
	{
		////////////////////
		// Remove any files in the directory
		if(abDeleteAllFiles)
		{
			tWStringList lstFiles;
            FindFilesInDir(lstFiles,asPath,_W("*"), true);
			for(tWStringListIt it = lstFiles.begin(); it != lstFiles.end(); ++it)
			{
				tWString sFilePath = cString::SetFilePathW(*it, asPath);
				RemoveFile(sFilePath);
			}
		}
		
		////////////////////
		// Remove any sub folders in the directory
		if(abDeleteAllSubFolders)
		{
			tWStringList lstFolders;
			FindFoldersInDir(lstFolders, asPath,true, false);
			for(tWStringListIt it = lstFolders.begin(); it != lstFolders.end(); ++it)
			{
				tWString sFolderPath = cString::SetFilePathW(*it, asPath);
				RemoveFolder(sFolderPath, abDeleteAllFiles,abDeleteAllSubFolders);
			}
		}
		
		if(rmdir(cString::To8Char(asPath).c_str())!=0)
		{
//			wchar_t sTempString[2048];
//			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,sTempString,2048,NULL);
//			Error("Could not remove folder: '%s': %s",cString::To8Char(sPath).c_str(), cString::To8Char(sTempString).c_str());
			return false;
		}
		return true;
	}

	//-----------------------------------------------------------------------

	bool cPlatform::FolderExists(const tWString& asPath)
	{
		struct stat statbuf;
		if (stat(cString::To8Char(asPath).c_str(), &statbuf)!=0) {
            return false;
        } else {
            return S_ISDIR(statbuf.st_mode);
        }
	}

	//-----------------------------------------------------------------------

	tWString cPlatform::GetFullFilePath(const tWString& asFilePath)
	{
		char rpath[PATH_MAX];
		realpath(cString::To8Char(asFilePath).c_str(), rpath);
		tWString ret = cString::To16Char(tString(rpath)); 
		return ret;
	}
	
	//-----------------------------------------------------------------------
	
	FILE *cPlatform::OpenFile(const tWString& asFileName, const tWString asMode)
	{
		return fopen(cString::To8Char(asFileName).c_str(), cString::To8Char(asMode).c_str());
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
		struct tm pClock;
		struct stat attrib;
		stat(cString::To8Char(asFilePath).c_str(), &attrib);
		
		gmtime_r(&(attrib.st_mtime), &pClock);	// Get the last modified time and put it into the time structure
		
		cDate date = DateFromGMTime(&pClock);
		
		return date;
	}

	//-----------------------------------------------------------------------

	cDate cPlatform::FileCreationDate(const tWString& asFilePath)
	{
		struct tm pClock;

		struct stat attrib;
		stat(cString::To8Char(asFilePath).c_str(), &attrib);

		gmtime_r(&(attrib.st_ctime), &pClock);	// Get the last modified time and put it into the time structure

		cDate date = DateFromGMTime(&pClock);

		return date;
	}

	//-----------------------------------------------------------------------
	static inline int patiMatch (const wchar_t *pattern, const wchar_t *string) {
		switch (pattern[0])
		{
			case _W('\0'):
				return !string[0];
				
			case _W('*') :
				return patiMatch(pattern+1, string) || (string[0] && patiMatch(pattern, string+1));
				
			case _W('?') :
				return string[0] && patiMatch(pattern+1, string+1);
				
			default  :
				return (towupper(pattern[0]) == towupper(string[0])) && patiMatch(pattern+1, string+1);
      }
	}
	void cPlatform::FindFilesInDir(tWStringList &alstStrings,const tWString& asDir, const tWString& asMask, bool abAddHidden)
	{
		//Get the search string
		wchar_t sSpec[256];
		wchar_t end = asDir[asDir.size()-1];
		//The needed structs
		DIR *dirhandle;
		dirent *_entry;
		struct stat statbuff;
		tWString fileentry;
		
		if ((dirhandle = opendir(cString::To8Char(asDir).c_str()))==NULL) return;
		
		while ((_entry = readdir(dirhandle)) != NULL) {
			if (end==_W('/'))
				swprintf(sSpec,256,_W("%ls%s"),asDir.c_str(),_entry->d_name);
			else
				swprintf(sSpec,256,_W("%ls/%s"),asDir.c_str(),_entry->d_name);
			
			// skip unreadable
			if (stat(cString::To8Char(sSpec).c_str(),&statbuff) ==-1) continue;
			// skip directories
			if (S_ISDIR(statbuff.st_mode)) continue;
			
			fileentry.assign(cString::To16Char(_entry->d_name));
			
			if (!patiMatch(asMask.c_str(),fileentry.c_str())) continue;
			alstStrings.push_back(fileentry);
		}
		closedir(dirhandle);
		alstStrings.sort();
	}

	//-----------------------------------------------------------------------
	void cPlatform::FindFoldersInDir(tWStringList &alstStrings,const tWString& asDir, bool abAddHidden, bool abAddUpFolder)
	{
		//Get the search string
		char sSpec[256];
		tString sDir8 = cString::To8Char(asDir);
		char end = sDir8[sDir8.size()-1];
		
		if (end != '/') {
			sDir8 += "/";
		}
		
		//The needed structs
		DIR *dirhandle;
		dirent *_entry;
		struct stat statbuff;
		tWString fileentry;
		
		if ((dirhandle = opendir(cString::To8Char(asDir).c_str()))==NULL) return;
		
		while ((_entry = readdir(dirhandle)) != NULL) {
			snprintf(sSpec,256,"%s%s",sDir8.c_str(),_entry->d_name);
			
			// skip unreadable
			if (stat(sSpec,&statbuff) ==-1) continue;
			// skip non-directories
			if (!S_ISDIR(statbuff.st_mode)) continue;
			
			// add updir
			if (!abAddUpFolder && _entry->d_name[0] == '.' && _entry->d_name[1] == '.' && _entry->d_name[2] == '\0') continue;
			// add hidden
			if (!abAddHidden && _entry->d_name[0] == '.') continue;
			// Skip self
			if (_entry->d_name[0] == '.' && _entry->d_name[1]=='\0') continue;

			fileentry.assign(cString::To16Char(_entry->d_name));

			alstStrings.push_back(fileentry);
		}
		closedir(dirhandle);
		alstStrings.sort();
	}

	//-----------------------------------------------------------------------
#ifdef __linux__
    tString cPlatform::GetDataDir()
    {
        tString temp;
        BrInitError error;
		if (!br_init (&error)) {
			// Log non-fatal error
			printf("*** BinReloc failed to initialize. Error: %d\n", error);
		} else {
			char *exedir;
			exedir = br_find_exe_dir(NULL);
			if (exedir) {
				temp = exedir;
				free(exedir);
			}
		}
        return temp;
    }
#endif

	//-----------------------------------------------------------------------

	tWString cPlatform::GetWorkingDir()
	{
		char buff[MAXPATHLEN];
		getcwd(buff, MAXPATHLEN);
		return cString::To16Char(tString(buff));
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// DIALOG
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
#ifdef __APPLE__
	void OSXAlertBox(eMsgBoxType eType, tString caption, tString message);
#endif

	void cPlatform::CreateMessageBoxBase(eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, va_list ap)
	{
		wchar_t text[2048];

		if (fmt == NULL)
			return;	
		vswprintf(text, 2047, fmt, ap);

		tWString sMess = _W("");

		sMess += text;

#if defined(__APPLE__) && (HPL_MINIMAL || !SDL_VERSION_ATLEAST(2,0,0))
		OSXAlertBox(eType, cString::To8Char(asCaption), cString::To8Char(sMess));
#elif SDL_VERSION_ATLEAST(2,0,0)
		Uint32 type = SDL_MESSAGEBOX_WARNING;
		switch (eType) {
			case eMsgBoxType_Info: type = SDL_MESSAGEBOX_INFORMATION; break;
			case eMsgBoxType_Error: type = SDL_MESSAGEBOX_ERROR; break;
			case eMsgBoxType_Warning: type = SDL_MESSAGEBOX_WARNING; break;
		}
		tString caption = cString::To8Char(asCaption);
		tString message = cString::To8Char(sMess);
		SDL_MessageBoxButtonData buttons[] = {
			{
				SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,
				1,
				"Dismiss"
			}
		};
		SDL_MessageBoxData data = {
			type,
			NULL,
			caption.c_str(),
			message.c_str(),
			1,
			buttons,
			NULL
		};
		int pressed = -1;
		SDL_ShowMessageBox(&data, &pressed);
#else
		// Linux/X11 implementation
		SDL_GrabMode cur = SDL_WM_GrabInput(SDL_GRAB_QUERY);
		if (cur == SDL_GRAB_ON) {
			SDL_WM_GrabInput(SDL_GRAB_OFF);
		}
		fl_alert("%s", cString::To8Char(asCaption + tWString(_W("\n\n")) + sMess).c_str());
		if (cur == SDL_GRAB_ON) {
			SDL_WM_GrabInput(SDL_GRAB_ON);
		}
#endif
	}


	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SYSTEM STATS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	ePlatform cPlatform::GetPlatform()
	{
#if defined(__linux__)
		return ePlatform_Linux;
#elif defined(__APPLE__)
        return ePlatform_Mac;
#else
#error Platfom Unsupported
#endif
	}

	//-----------------------------------------------------------------------
#if defined(__linux__) && defined(__LP64__)
	tString cPlatform::msName = "Linux x86_64";
#elif defined(__linux__)
    tString cPlatform::msName = "Linux x86";
#elif defined(__APPLE__) && (defined(__PPC__) || defined(__ppc__))
    tString cPlatform::msName = "Mac OS X PowerPC";
#elif defined(__APPLE__) && defined(__LP64__)
	tString cPlatform::msName = "Mac OS X x86_64";
#elif defined(__APPLE__)
	tString cPlatform::msName = "Mac OS X x86";
#else
#error Platform Unsupported
#endif

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

	tWString cPlatform::GetSystemSpecialPath(eSystemPath aPathType)
	{
		switch (aPathType)
		{
			case eSystemPath_Personal: {
				const char *home = getenv("HOME");
				tWString sDir = cString::To16Char(tString(home));
				if (cString::GetLastCharW(sDir) != _W("/")) {
					sDir += _W("/");
				}
				return sDir;
			}
			default:
				return _W("");
		}
	}

	//-----------------------------------------------------------------------

	unsigned long cPlatform::GetSystemAvailableDrives()
	{
		/* @THOMAS Mac and Linux do not have an equivilent of Drives everything is underone tree */
		return 0x0;
	}

	//////////////////////////////////////////////////////////////////////////
	// SYSTEM COMMANDS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	void OSXLaunchURL(tString url);
	void cPlatform::OpenBrowserWindow ( const tWString& asURL )
	{
#if defined(__APPLE__)
		OSXLaunchURL(cString::To8Char(asURL));
#else
		pid_t pID = fork();
		if (pID == 0) {// child
			execlp("xdg-open", "xdg-open", cString::To8Char(asURL).c_str(), (char *)0);
			exit(1);
		} else if (pID < 0) { // Failed
			Error("Could not Open URL %ls\n", asURL.c_str());
		} else { // Parent
			Log("Opened URL %ls\n", asURL.c_str());
		}
#endif
	}

    bool OSXOpenFile(tString path);
	bool cPlatform::OpenFileOnShell( const tWString& asPath )
	{
#if defined(__APPLE__)
        return OSXOpenFile(cString::To8Char(asPath));
#else
		pid_t pID = fork();
		if (pID == 0) {// child
			execlp("/bin/bash", "xdg-open", "xdg-open", cString::To8Char(asPath).c_str(), (char *)0);
			exit(1);
		} else if (pID < 0) { // Failed
			Error("Could not Open File %ls\n", asPath.c_str());
			return false;
		} else { // Parent
			Log("Opened File %ls\n", asPath.c_str());
			return true;
		}
#endif
	}

	bool OSXRunProgram(tString path, tString args);
	bool cPlatform::RunProgram( const tWString& asPath, const tWString& asParams )
	{
        bool ret = true;
#if defined(__APPLE__)
		ret = OSXRunProgram(cString::To8Char(asPath), cString::To8Char(asParams));
#else
		pid_t pID = fork();
		if (pID == 0) {// child
			tStringVec tvArgs;
			tString sSepp = " ";
			cString::GetStringVec( cString::To8Char(asParams), tvArgs, &sSepp);

			char **argv = (char **)hplMalloc(sizeof(char *) * (tvArgs.size()+2));
			if (argv) {
				tString sTemp = cString::To8Char(asPath);
				argv[0] = const_cast <char *> (sTemp.c_str());
				size_t ai=1;
				for (size_t i = 0; i < tvArgs.size(); ++i,++ai) {
					argv[ai] = const_cast <char*> (tvArgs[i].c_str());
				}
				argv[ai] = (char *)0;

				execvp(sTemp.c_str(), argv);
			}
			exit(1);
		} else if (pID < 0) { // Failed
			Error("Could not Launch program %ls\n", asPath.c_str());
            ret = false;
		} else { // Parent
			Log("Launched Program %ls\n", asPath.c_str());
		}
#endif
        return ret;
	}
}

