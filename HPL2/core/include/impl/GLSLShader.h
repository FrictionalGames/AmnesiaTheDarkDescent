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

#ifndef HPL_GLSL_SHADER_H
#define HPL_GLSL_SHADER_H

//#include <windows.h>
#include <GL/glew.h>

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GPUShader.h"

namespace hpl {

	//----------------------------------------------

	class iLowLevelGraphics;

	//----------------------------------------------
	class cGLSLShader : public iGpuShader
	{
	public:
		cGLSLShader(const tString& asName, eGpuShaderType aType, iLowLevelGraphics *apLowLevelGraphics);
		~cGLSLShader();

		bool Reload();
		void Unload();
		void Destroy();

		bool SamplerNeedsTextureUnitSetup(){ return true;}

		tString GetProgramName(){ return msName; }

		bool CreateFromFile(const tWString& asFile, const tString& asEntry="main", bool abPrintInfoIfFail=true);
		bool CreateFromString(const char *apStringData, const tString& asEntry="main", bool abPrintInfoIfFail=true);

		//GLSL Specific
		GLuint GetHandle(){ return mlHandle;}
	
	protected:
		void LogShaderInfoLog();
		void LogShaderCode(const char *apStringData);
		GLenum GetGLShaderType(eGpuShaderType aType);

		iLowLevelGraphics *mpLowLevelGraphics;
		
		GLuint mlHandle;
	};
};
#endif // HPL_GLSL_SHADER_H
