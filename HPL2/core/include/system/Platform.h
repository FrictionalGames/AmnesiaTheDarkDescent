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

#ifndef HPL_PLATFORM_H
#define HPL_PLATFORM_H

#include "system/SystemTypes.h"

#include <cstdarg>

namespace hpl {

	class iTimer;
	class iThread;
	class iThreadClass;
	class iMutex;

	//-----------------------------------------

    struct VideoComp : public std::binary_function<cVideoMode, cVideoMode, bool> {
        bool operator() (const cVideoMode& aVM1, const cVideoMode& aVM2) const;
    };

	//-----------------------------------------

	class cPlatform
	{
	public:

		//////////////////////////////////////////////////////
		////////// FILE HANDLING /////////////////////////////
		//////////////////////////////////////////////////////

		static unsigned long GetFileSize(const tWString& asFileName);
		static bool CopyFileToBuffer(const tWString& asFileName, void *apBuffer, unsigned long alSize);

		static bool FileExists(const tWString& asFileName);
		static void RemoveFile(const tWString& asFileName);
		static bool CloneFile(const tWString& asSrcFileName,const tWString& asDestFileName,	bool abFailIfExists);
		static bool CreateFolder(const tWString& asPath);
		static bool RemoveFolder(const tWString& asPath, bool abDeleteAllFiles, bool abDeleteAllSubFolders);
		static bool FolderExists(const tWString& asPath);
		static tWString GetFullFilePath(const tWString& asFilePath);
		static FILE *OpenFile(const tWString& asFileName, const tWString asMode);
		
		static cDate FileModifiedDate(const tWString& asFilePath);
		static cDate FileCreationDate(const tWString& asFilePath);

		/**
		* Returns a list of files in a dir
		* \param &alstStrings list where the files are saved
		* \param asDir Directory
		* \param asMask Mask to be used, for example "*.*" to search for all kinds of files.
		* \param abAddHidden If hidden files should be added
		*/
		static void FindFilesInDir(tWStringList &alstStrings, const tWString& asDir, const tWString& asMask, bool abAddHidden=false);

		/**
		* Returns a list of folders in a dir
		* \param &alstStrings list where the folders are saved
		* \param asDir Directory
		* \param abAddHidden If hidden folders should be added
		* \param abAddUpFolder If the ".." folder should be added
		*/
		static void FindFoldersInDir(tWStringList &alstStrings, const tWString& asDir, bool abAddHidden, bool abAddUpFolder=false);


        static tString GetDataDir();
		static tWString GetWorkingDir();
		

		//////////////////////////////////////////////////////
		////////// APPLICATION ///////////////////////////////
		//////////////////////////////////////////////////////

		static unsigned long GetApplicationTime();
		static void Sleep (unsigned int alMilliSecs);

		//////////////////////////////////////////////////////
		////////// DIALOG ////////////////////////////////////
		//////////////////////////////////////////////////////
		
		static void CreateMessageBox( eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, ...);
		static void CreateMessageBox( const wchar_t* asCaption, const wchar_t* fmt, ...);
		static void CreateMessageBox( const wchar_t* asCaption, const wchar_t *fmt, va_list ap);
		static void CreateMessageBox( eMsgBoxType eType, const wchar_t* asCaption, const wchar_t *fmt, va_list ap);

		
		//////////////////////////////////////////////////////
		////////// SYSTEM DATA //////////////////////////////
		//////////////////////////////////////////////////////

		static ePlatform GetPlatform();
		static const tString& GetPlatformName() { return msName; }

		static iTimer * CreateTimer();
		
		static cDate GetDate();

		static void CopyTextToClipboard(const tWString &asText);
		static tWString LoadTextFromClipboard();

		static tWString GetSystemSpecialPath(eSystemPath aPathType);

		static unsigned long GetSystemAvailableDrives();

		static void GetAvailableVideoModes(tVideoModeVec& avDestVidModes, int alMinBpp=-1, int alMinRefreshRate=-1);
		
		static tWString GetDisplayName(int alDisplay);

		static void GetDisplayResolution(int alDisplay, int& alHorizontal, int& alVertical);

		//////////////////////////////////////////////////////
		////////// SYSTEM COMMANDS ///////////////////////////
		//////////////////////////////////////////////////////
		
		static void OpenBrowserWindow ( const tWString& asURL );

		static bool RunProgram( const tWString& asPath, const tWString& asParams );

		static bool OpenFileOnShell(const tWString& asPath);

		//////////////////////////////////////////////////////
		////////// THREADING /////////////////////////////////
		//////////////////////////////////////////////////////

		static iThread* CreateThread(iThreadClass* apThreadClass);

		static iMutex* CreateMutEx(); // If you name this method CreateMutex strange stuff will happen :S
	
	private:
        static void CreateMessageBoxBase(eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, va_list ap);

        static tString msName;
	};

};
#endif // HPL_PLATFORM_H
