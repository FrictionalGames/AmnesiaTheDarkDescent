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

#ifndef HPL_CGSHADER_H
#define HPL_CGSHADER_H

//#include <windows.h>
#include <GL/GLee.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GPUShader.h"

namespace hpl {

	class cCGShader : public iGpuShader
	{
	public:
		cCGShader(tString asName, CGcontext aContext,eGpuShaderType aType);
		~cCGShader();

		bool Reload();
		void Unload();
		void Destroy();

		tString GetProgramName(){ return msName; }

		bool CreateFromFile(const tString& asFile, const tString& asEntry="main");

		void Bind();
		void UnBind();

		int GetVariableId(const tString& asName);

		bool SetFloat(int alVarId, float afX);
		bool SetVec2f(int alVarId, float afX,float afY);
		bool SetVec3f(int alVarId, float afX,float afY,float afZ);
		bool SetVec4f(int alVarId, float afX,float afY,float afZ, float afW);

		bool SetMatrixf(int alVarId, const cMatrixf& mMtx);
		bool SetMatrixf(int alVarId, eGpuShaderMatrix mType, eGpuShaderMatrixOp mOp);

		bool SetTexture(int alVarId,iTexture* apTexture, bool abAutoDisable=true);
		bool SetTextureToUnit(int alUnit, iTexture* apTexture);

		
		/// CG SPECIFIC //////////////////////

		CGprogram GetProgram(){ return mProgram;}
		CGprofile GetProfile(){ return mProfile;}

		static void SetVProfile(tString asProfile) {
			msForceVP = asProfile;
		}
		static void SetFProfile(tString asProfile) {
			msForceFP = asProfile;
		}
		static tString &GetVProfile(){ return msForceVP; }
		static tString &GetFProfile(){ return msForceFP; }

	protected:
		CGcontext mContext;

		tString msName;
		tString msFile;
		tString msEntry;
		CGprogram mProgram;
		CGprofile mProfile;

		std::vector<CGparameter> mvParameters;

		CGparameter mvTexUnitParam[kMaxTextureUnits];
	

		static tString msForceFP;
		static tString msForceVP;
    };
};
#endif // HPL_CGSHADER_H
