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

#ifndef HPL_GPU_SHADER_H
#define HPL_GPU_SHADER_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"


namespace hpl {

	class iTexture;

	//---------------------------------------------------
	
	class cGpuShader_SamplerUnit
	{
	public:
		tString msName;
		int mlUnit;
	};
	
	//---------------------------------------------------

	class iGpuShader : public iResourceBase
	{
	public:
		iGpuShader(const tString& asName, const tWString& asFullPath, eGpuShaderType aType,eGpuProgramFormat aProgramFormat) : iResourceBase(asName, asFullPath,0){
			mProgramFormat = aProgramFormat; 
			mShaderType = aType;
		}
		virtual ~iGpuShader(){}

		static void SetLogDebugInformation(bool abX){mbDebugInfo = abX;}

		virtual bool SamplerNeedsTextureUnitSetup()=0;

		/**
		* Create a from a file. Used internally
		* \param asFile 
		* \param asEntry 
		* \return 
		*/
		virtual bool CreateFromFile(const tWString& asFile, const tString& asEntry="main", bool abPrintInfoIfFail=true)=0;
		virtual bool CreateFromString(const char *apStringData, const tString& asEntry="main", bool abPrintInfoIfFail=true)=0;
		
		eGpuProgramFormat GetFormat(){ return mProgramFormat;}
		eGpuShaderType GetType() { return mShaderType;}

		void AddSamplerUnit(const tString& asSamplerName, int alUnit);
		int GetSamplerUnitNum(){ return (int)mvSamplerUnits.size(); }
		cGpuShader_SamplerUnit* GetSamplerUnit(int alIdx){ return &mvSamplerUnits[alIdx];}

		void SetUserId(unsigned int alX){mlUserId = alX;}
		unsigned int GetUserId(){ return mlUserId;}

	protected:
		eGpuProgramFormat mProgramFormat;
		eGpuShaderType mShaderType;
		unsigned int mlUserId;

		std::vector<cGpuShader_SamplerUnit> mvSamplerUnits;

		static bool mbDebugInfo;
	};
};
#endif // HPL_GPU_SHADER_H
