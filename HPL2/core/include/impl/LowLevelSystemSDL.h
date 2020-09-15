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

#ifndef HPL_LOWLEVELSYSTEM_SDL_H
#define HPL_LOWLEVELSYSTEM_SDL_H

#include "system/LowLevelSystem.h"
#include <angelscript.h>
#include <stdio.h>

namespace hpl {

	//------------------------------------------------------

	class cLogWriter
	{
	public:
		cLogWriter(const tWString& asDefaultFile);
		~cLogWriter();
		
		void Write(const tString& asMessage);
		void Clear();

		void SetFileName(const tWString& asFile);
		
	private:
		void ReopenFile();

		FILE *mpFile;
		tWString msFileName;
	};

	//------------------------------------------------------
	
	class cScriptOutput// : public  asIOutputStream
	{
	public:
		cScriptOutput() : msMessage("") {}
		~cScriptOutput(){}
		
		void AddMessage(const asSMessageInfo *msg);
		void Display();
		void Clear();

		tString &GetMessage(){ return msMessage;}

	private:
		tString msMessage;
	};

	//------------------------------------------------------


	class cLowLevelSystemSDL : public iLowLevelSystem
	{
	public:
		cLowLevelSystemSDL();
		~cLowLevelSystemSDL();

		iScript* CreateScript(const tString& asName);

		bool AddScriptFunc(const tString& asFuncDecl, void* pFunc);
		bool AddScriptVar(const tString& asVarDecl, void *pVar);

	private:
		asIScriptEngine *mpScriptEngine;
		cScriptOutput *mpScriptOutput;
		int mlHandleCount;
	};

	//------------------------------------------------------

};
#endif // HPL_LOWLEVELSYSTEM_SDL_H
