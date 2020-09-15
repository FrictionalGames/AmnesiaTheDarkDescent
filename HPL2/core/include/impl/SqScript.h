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

#ifndef HPL_SQ_SCRIPT_H
#define HPL_SQ_SCRIPT_H

#include "system/Script.h"
#include "impl/LowLevelSystemSDL.h"
#include <angelscript.h>


namespace hpl {

	class cSqScript : public iScript
	{
	public:
		cSqScript(const tString& asName, asIScriptEngine *apScriptEngine,cScriptOutput *apScriptOutput, int alHandle); 
		~cSqScript();

		bool CreateFromFile(const tWString& asFileName, tString *apCompileMessages=NULL);

		int GetFuncHandle(const tString& asFunc);
		void AddArg(const tString& asArg);

		bool Run(const tString& asFuncLine);
		bool Run(int alHandle);

	private:
		asIScriptEngine *mpScriptEngine;
		cScriptOutput *mpScriptOutput;
        
		asIScriptContext *mpContext;
		asIScriptModule *mpModule;
		
		int mlHandle;
		tString msModuleName;

		char* LoadCharBuffer(const tWString& asFileName, int& alLength);
	};
};
#endif // HPL_SCRIPT_H
