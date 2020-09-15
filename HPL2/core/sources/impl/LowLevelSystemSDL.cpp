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

//#include <vld.h>
//Use this to check for memory leaks!

#ifdef WIN32
#pragma comment(lib, "angelscript.lib")
#define UNICODE
#include <windows.h>
#include <shlobj.h>
#endif

#define _UNICODE

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fstream>
#include <string>

#include "impl/LowLevelSystemSDL.h"
#include "impl/SqScript.h"
#include "system/Platform.h"

#if USE_SDL2
#include "SDL2/SDL.h"
#else
#include "SDL/SDL.h"
#endif

#include "impl/scriptstring.h"

#include "system/String.h"

#ifndef WIN32
#include <clocale>
#include <langinfo.h>
#include <unistd.h>
#endif


// Include for using the versioning header
//#include "BuildID_HPL2_0.h"

// @todo redo this prototype to take a tStringVec instead

#ifndef IGNORE_HPL_MAIN
extern int hplMain(const hpl::tString &asCommandLine);

#ifdef WIN32
#include <windows.h>
int WINAPI WinMain(	HINSTANCE hInstance,  HINSTANCE hPrevInstance,LPSTR	lpCmdLine, int nCmdShow)
{
	return hplMain(lpCmdLine);
}
#else
int main(int argc, char *argv[])
{
#ifdef __linux__
	if(!std::setlocale(LC_CTYPE, "")) {
		fprintf(stderr, "Can't set the specified locale! Check LANG, LC_CTYPE, LC_ALL.\n");
		return 1;
	}
	char *charset = nl_langinfo(CODESET);
	bool utf8_mode = (strcasecmp(charset, "UTF-8") == 0);
	if (!utf8_mode) {
		fprintf(stderr, "UTF-8 Charset %s available.\nCurrent LANG is %s\nCharset: %s\n",
			utf8_mode ? "is" : "not",
			getenv("LANG"), charset);
	}
#endif

	bool cwd = false;
	hpl::tString cmdline = "";
	for (int i=1; i < argc; i++) {
        if (strcmp(argv[i], "-cwd") == 0) {
            cwd = true;
        } else if (strncmp(argv[i], "-psn", 4) == 0) {
            // skip "finder" process number
		} else {
			if (cmdline.length()>0) {
				cmdline.append(" ").append(argv[i]);
			} else {
				cmdline.append(argv[i]);
			}
		}
	}

	if (!cwd) {
        hpl::tString dataDir = hpl::cPlatform::GetDataDir();

        chdir(dataDir.c_str());
	}

	return hplMain(cmdline);
}
#endif
#endif





namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// LOG WRITER
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	static cLogWriter gLogWriter(_W("hpl.log"));
	static cLogWriter gUpdateLogWriter(_W("hpl_update.log"));

	//-----------------------------------------------------------------------
	
	cLogWriter::cLogWriter(const tWString& asFileName)
	{
		msFileName = asFileName;
		mpFile = NULL;
	}

	cLogWriter::~cLogWriter()
	{
		if(mpFile) fclose(mpFile);
	}

	void cLogWriter::Write(const tString& asMessage)
	{
		if(!mpFile) ReopenFile();
		
		if(mpFile)
		{
			fprintf(mpFile, "%s", asMessage.c_str());
			fflush(mpFile);
		}
	}

	void cLogWriter::Clear()
	{
		ReopenFile();
		if(mpFile) fflush(mpFile);
	}

	//-----------------------------------------------------------------------


	void cLogWriter::SetFileName(const tWString& asFile)
	{
		if(msFileName == asFile) return;

		msFileName = asFile;
		if(mpFile) ReopenFile();
	}

	//-----------------------------------------------------------------------


	void cLogWriter::ReopenFile()
	{
		if(mpFile) fclose(mpFile);
						
		#ifdef WIN32
			mpFile = _wfopen(msFileName.c_str(),_W("w"));
		#else
			mpFile = fopen(cString::To8Char(msFileName).c_str(),"w");
		#endif
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// LOG FUNCTIONS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	static tLogMessageCallbackFunc gpLogMessageCallbackFunc=NULL;

	//-----------------------------------------------------------------------
	
	void SetLogFile(const tWString &asFile)
	{
		gLogWriter.SetFileName(asFile);
	}

	//-----------------------------------------------------------------------

	void FatalError(const char* fmt,... )
	{
		char text[4096];
		va_list ap;	
		if (fmt == NULL)
			return;	
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "FATAL ERROR: ";
		sMess += text;
		gLogWriter.Write(sMess);

		if(gpLogMessageCallbackFunc) gpLogMessageCallbackFunc(eLogOutputType_FatalError, sMess.c_str());

#if defined(__APPLE__) || defined(__linux__)
#if !SDL_VERSION_ATLEAST(2, 0, 0)
		SDL_WM_GrabInput(SDL_GRAB_OFF);
#endif
		SDL_Quit();
#endif
		cPlatform::CreateMessageBox(eMsgBoxType_Error, _W("FATAL ERROR"), _W("%ls"), cString::To16Char(sMess).c_str());

		exit(1);
	}

	//-----------------------------------------------------------------------

	void Error(const char* fmt, ...)
	{
		char text[2048];
		va_list ap;	
		if (fmt == NULL)
			return;	
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "ERROR: ";
		sMess += text;
		gLogWriter.Write(sMess);

		if(gpLogMessageCallbackFunc) gpLogMessageCallbackFunc(eLogOutputType_Error, sMess.c_str());
	}

	//-----------------------------------------------------------------------


	void Warning(const char* fmt, ...)
	{
		char text[2048];
		va_list ap;	
		if (fmt == NULL)
			return;	
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "WARNING: ";
		sMess += text;
		gLogWriter.Write(sMess);

		if(gpLogMessageCallbackFunc) gpLogMessageCallbackFunc(eLogOutputType_Warning, sMess.c_str());
	}

	//-----------------------------------------------------------------------


	void Log(const char* fmt, ...)
	{
		char text[4096];
		va_list ap;	
		if (fmt == NULL)
			return;	
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "";
		sMess += text;
		gLogWriter.Write(sMess);

		if(gpLogMessageCallbackFunc) gpLogMessageCallbackFunc(eLogOutputType_Normal, sMess.c_str());
	}

	//-----------------------------------------------------------------------

	static bool gbUpdateLogIsActive;
	void SetUpdateLogFile(const tWString &asFile)
	{
		gUpdateLogWriter.SetFileName(asFile);
	}

	void ClearUpdateLogFile()
	{
		if(!gbUpdateLogIsActive) return;

		gUpdateLogWriter.Clear();
	}

	void SetUpdateLogActive(bool abX)
	{
		gbUpdateLogIsActive =abX;
	}

	bool GetUpdateLogActive()
	{
		return gbUpdateLogIsActive;
	}

	//-----------------------------------------------------------------------

	void LogUpdate(const char* fmt, ...)
	{
		if(!gbUpdateLogIsActive) return;

		char text[2048];
		va_list ap;	
		if (fmt == NULL)
			return;	
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "";
		sMess += text;
		gUpdateLogWriter.Write(sMess);
	}

	//-----------------------------------------------------------------------

	extern void SetLogMessageCallback(tLogMessageCallbackFunc apFunc)
	{
		gpLogMessageCallbackFunc = apFunc;
	}

	//-----------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////////////////
	// SCRIPT OUTPUT
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cScriptOutput::AddMessage(const asSMessageInfo *msg)
	{
		char sMess[1024];

		tString type = "ERR ";
		if( msg->type == asMSGTYPE_WARNING ) 
			type = "WARN";
		else if( msg->type == asMSGTYPE_INFORMATION ) 
			type = "INFO";

		sprintf(sMess,"%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type.c_str(), msg->message);

		msMessage += sMess;
	}
	//-----------------------------------------------------------------------

	void cScriptOutput::Display()
	{
		if(msMessage.size()>500)
		{
			while(msMessage.size() > 500)
			{
				tString sSub = msMessage.substr(0,500);
				msMessage = msMessage.substr(500);
				Log(sSub.c_str());
			}
			Log(msMessage.c_str());
		}
		else
		{
			Log(msMessage.c_str());
		}
	}
	//-----------------------------------------------------------------------

	void cScriptOutput::Clear()
	{
		msMessage = "";
	}
	
	//-----------------------------------------------------------------------

	
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelSystemSDL::cLowLevelSystemSDL()
	{
		mpScriptEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		if(mpScriptEngine==NULL)
		{
			Error("Failed to start angel script!\n");
		}
		
		mpScriptOutput = hplNew( cScriptOutput, () );
		mpScriptEngine->SetMessageCallback(asMETHOD(cScriptOutput,AddMessage), mpScriptOutput, asCALL_THISCALL);

		RegisterScriptString(mpScriptEngine);
	
		mlHandleCount = 0;

		Log("-------- THE HPL ENGINE LOG ------------\n");
		//Log("Engine build ID %s\n\n", 
		//	GetBuildID_HPL2_0());

		//const char* pASLibOptions = asGetLibraryOptions();
		//Log(" AngelScript options: '%s'\n", pASLibOptions);
	}

	//-----------------------------------------------------------------------

	cLowLevelSystemSDL::~cLowLevelSystemSDL()
	{
		/*Release all runnings contexts */
		
		mpScriptEngine->Release();
		hplDelete(mpScriptOutput);

		//perhaps not the best thing to skip :)
		//if(gpLogWriter)	hplDelete(gpLogWriter);
		//gpLogWriter = NULL;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	
	iScript* cLowLevelSystemSDL::CreateScript(const tString& asName)
	{
		return hplNew( cSqScript, (asName,mpScriptEngine,mpScriptOutput,mlHandleCount++) );
	}

	//-----------------------------------------------------------------------

	bool cLowLevelSystemSDL::AddScriptFunc(const tString& asFuncDecl, void* pFunc)
	{
		if(mpScriptEngine->RegisterGlobalFunction(asFuncDecl.c_str(),asFUNCTION(pFunc),asCALL_STDCALL)<0)
		{
			Error("Couldn't add func '%s'\n",asFuncDecl.c_str());
			return false;
		}

		return true;
	}
	
	//-----------------------------------------------------------------------

	bool cLowLevelSystemSDL::AddScriptVar(const tString& asVarDecl, void *pVar)
	{
		if(mpScriptEngine->RegisterGlobalProperty(asVarDecl.c_str(),pVar)<0)
		{
			Error("Couldn't add var '%s'\n",asVarDecl.c_str());
			return false;
		}

		return true;
	}
	
	//-----------------------------------------------------------------------
	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
}
