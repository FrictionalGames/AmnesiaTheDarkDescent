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

#ifndef HPL_GLSL_PROGRAM_H
#define HPL_GLSL_PROGRAM_H

//#include <windows.h>

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GPUShader.h"

#include "graphics/GPUProgram.h"

#include <GL/glew.h>

namespace hpl {

	//-----------------------------------------------------

	class cGLSLParam
	{
	public:
		cGLSLParam() : mlId(-1), msName(""){}
		cGLSLParam(GLint alId, const tString& asName) : mlId(alId), msName(asName){}
		
		tString msName;
		GLint mlId;
	};

	//-----------------------------------------------------

	class cGLSLProgram : public iGpuProgram
	{
	public:
		cGLSLProgram(const tString& asName);
		~cGLSLProgram();

		bool Link();

		void Bind();
		void UnBind();

		bool CanAccessAPIMatrix(){ return true;}
		bool SamplerNeedsTextureUnitSetup(){ return true; }

		/**
		 * Binds and unbinds program so must not be done during rendering.
		 */
		bool SetSamplerToUnit(const tString& asSamplerName, int alUnit);

		int GetVariableId(const tString& asName);
		bool GetVariableAsId(const tString& asName, int alId);

		bool SetInt(int alVarId, int alX);
		bool SetFloat(int alVarId, float afX);
		bool SetVec2f(int alVarId, float afX,float afY);
		bool SetVec3f(int alVarId, float afX,float afY,float afZ);
		bool SetVec4f(int alVarId, float afX,float afY,float afZ, float afW);

		bool SetMatrixf(int alVarId, const cMatrixf& aMtx);
		bool SetMatrixf(int alVarId, eGpuShaderMatrix aType, eGpuShaderMatrixOp aOp);
		
	private:
		void LogProgramInfoLog();

		GLuint mlHandle;
		static int mlCurrentProgram;

		std::vector<cGLSLParam> mvParameters;
    };
};
#endif // HPL_GLSL_PROGRAM_H
