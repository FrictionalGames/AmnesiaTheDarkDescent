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

#ifndef HPL_GPU_PROGRAM_H
#define HPL_GPU_PROGRAM_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {

	//---------------------------------------------------

	class iGpuShader;
	class cResources;

	//---------------------------------------------------

	class iGpuProgram
	{
	public:
		iGpuProgram(const tString& asName,eGpuProgramFormat aProgramFormat);
		virtual ~iGpuProgram();

		const tString& GetName(){ return msName;}

		void SetShader(eGpuShaderType aType, iGpuShader *apShader);
		iGpuShader* GetShader(eGpuShaderType aType) { return mpShader[aType];}

		void SetAutoDestroyShaders(bool abX){ mbAutoDestroyShaders = abX;}
		bool GetAutoDestroyShaders(){ return mbAutoDestroyShaders;}

		void SetResources(cResources *apResources){ mpResources = apResources;}

		virtual bool Link()=0;

		virtual void Bind()=0;
		virtual void UnBind()=0;

		virtual bool CanAccessAPIMatrix()=0;
		
		virtual bool SetSamplerToUnit(const tString& asSamplerName, int alUnit)=0;

		virtual int GetVariableId(const tString& asName)=0;
		virtual bool GetVariableAsId(const tString& asName, int alId)=0;
		
		virtual bool SetInt(int alVarId, int alX)=0;
		virtual bool SetFloat(int alVarId, float afX)=0;
		virtual bool SetVec2f(int alVarId, float afX,float afY)=0;
		virtual bool SetVec3f(int alVarId, float afX,float afY,float afZ)=0;
		virtual bool SetVec4f(int alVarId, float afX,float afY,float afZ, float afW)=0;
		virtual bool SetMatrixf(int alVarId, const cMatrixf& mMtx)=0;
		virtual bool SetMatrixf(int alVarId, eGpuShaderMatrix mType, eGpuShaderMatrixOp mOp)=0;

        bool SetVec2f(int alVarId, const cVector2f avVec){return SetVec2f(alVarId,avVec.x, avVec.y);}
		
		bool SetVec3f(int alVarId, const cVector3f& avVec){ return SetVec3f(alVarId, avVec.x,avVec.y,avVec.z);}
		bool SetColor3f(int alVarId, const cColor &aCol){return SetVec3f(alVarId,aCol.r, aCol.g, aCol.b);}
		
		bool SetColor4f(int alVarId, const cColor &aCol){return SetVec4f(alVarId,aCol.r, aCol.g, aCol.b, aCol.a);}
		
		eGpuProgramFormat GetFormat(){ return mProgramFormat;}
		
		void SetUserId(unsigned int alX){mlUserId = alX;}
		unsigned int GetUserId(){ return mlUserId;}

	protected:
		tString msName;
		cResources *mpResources;
		unsigned int mlUserId;
		
		eGpuProgramFormat mProgramFormat;


		iGpuShader* mpShader[2];

		bool mbAutoDestroyShaders;
	};

	//---------------------------------------------------

};
#endif // HPL_GPU_PROGRAM_H


