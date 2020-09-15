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

#include "impl/GLSLShader.h"
#include "impl/SDLTexture.h"
#include "impl/LowLevelGraphicsSDL.h"
#include "system/LowLevelSystem.h"

#include "system/Platform.h"
#include "system/String.h"

#ifdef WIN32
#include <io.h>
#endif

namespace hpl{

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGLSLShader::cGLSLShader(const tString& asName,eGpuShaderType aType, iLowLevelGraphics *apLowLevelGraphics) 
				: iGpuShader(asName, _W(""), aType, eGpuProgramFormat_GLSL)
	{
		;

		mpLowLevelGraphics = apLowLevelGraphics;
		
		mlHandle = glCreateShader(GetGLShaderType(aType));
	}

	cGLSLShader::~cGLSLShader()
	{
		;

		glDeleteShader(mlHandle);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cGLSLShader::Reload(){return false;}
	void cGLSLShader::Unload(){}
	void cGLSLShader::Destroy(){}
	
	//-----------------------------------------------------------------------

	bool cGLSLShader::CreateFromFile(const tWString &asFile, const tString &asEntry, bool abPrintInfoIfFail)
	{
		///////////////////////////////////////
		//Load file
		FILE *pFile = cPlatform::OpenFile(asFile, _W("rb"));
		if(pFile == NULL)
		{
			Error("Could not open file %ls\n", asFile.c_str());
			return false;
		}
		SetFullPath(asFile);
		
		fseek(pFile,0,SEEK_END);
		int lFileSize = ftell(pFile);
		rewind(pFile);

		char *pBuffer = (char*) hplMalloc(sizeof(char)* lFileSize+1);

		fread(pBuffer, sizeof(GLchar), lFileSize, pFile);
		pBuffer[lFileSize] =0; //Zero at end so it is a proper string.
		
		fclose(pFile);

        bool bRet = CreateFromString(pBuffer, asEntry, abPrintInfoIfFail);

		hplFree(pBuffer);
		
		return bRet;
	}

	//-----------------------------------------------------------------------


	bool cGLSLShader::CreateFromString(const char *apStringData, const tString& asEntry, bool abPrintInfoIfFail)
	{
		;

		///////////////////////////////////////
		//Compile

		//const char *pConstBuffer = apStringData;
		//glShaderSource(mlHandle, 1, &pConstBuffer, NULL);
		glShaderSource(mlHandle, 1, &apStringData, NULL);

		glCompileShader(mlHandle);

		///////////////////////////////////////
		//Check for errors.
		GLint lStatus;
		glGetShaderiv(mlHandle,GL_COMPILE_STATUS,&lStatus);
		if(lStatus == GL_FALSE)
		{
			if(abPrintInfoIfFail)
			{
				Error("Failed to compile GLSL shader '%s'!\n",cString::To8Char(GetFullPath()).c_str());
				Log("Shader code:\n-------------------\n");
				
				LogShaderCode(apStringData);
	            
				
				Log("---------------------\n");
				Log("Compile log:\n");
				LogShaderInfoLog();
			}
			return false;
		}
		
		/////////////////////////////
		//If debug is on, then show code + compile log
		if(mbDebugInfo)
		{
			Log("Shader '%s' code:\n-------------------\n", msName.c_str());

			//LogShaderCode(apStringData);

			//Log("---------------------\n");
			Log("Compile log:\n");
			LogShaderInfoLog();
		}


		return true;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGLSLShader::LogShaderInfoLog()
	{
		;

		GLint infologLength = 0;
		GLsizei charsWritten  = 0;
		char *infoLog;

		glGetShaderiv(mlHandle, GL_INFO_LOG_LENGTH,&infologLength);

		if (infologLength > 0)
		{
			infoLog = (char *)hplMalloc(infologLength);
			glGetShaderInfoLog(mlHandle, infologLength, &charsWritten, infoLog);
			Log("---------------------\n");
			Log("%s\n",infoLog);
			Log("---------------------\n");
			hplFree(infoLog);
		}
	}

	//-----------------------------------------------------------------------

	void cGLSLShader::LogShaderCode(const char *apStringData)
	{
		int lRow =1;
		tString sRowCode ="";
		for(int i=0; apStringData[i]!=0; ++i)
		{
			char lChar = apStringData[i];
			if(lChar=='\r') continue; //Can skip this sign.

			if(lChar == '\n')
			{
				Log("[%04d] %s\n",lRow, sRowCode.c_str());
				sRowCode.resize(0);
				lRow++;
			}
			else
			{
				sRowCode +=lChar;
			}
		}
	}

	//-----------------------------------------------------------------------

	GLenum cGLSLShader::GetGLShaderType(eGpuShaderType aType)
	{
		switch(aType)
		{
		case eGpuShaderType_Fragment:	return GL_FRAGMENT_SHADER;
		case eGpuShaderType_Vertex:		return GL_VERTEX_SHADER;
		}

		return GL_VERTEX_SHADER;
	}
	
	//-----------------------------------------------------------------------

}
