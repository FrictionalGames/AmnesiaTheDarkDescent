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

#include "impl/CGShader.h"
#include "impl/SDLTexture.h"
#include "system/LowLevelSystem.h"

#include "system/String.h"

namespace hpl{

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	tString cCGShader::msForceFP = "AUTO";
	tString cCGShader::msForceVP = "AUTO";

	cCGShader::cCGShader(tString asName,CGcontext aContext,eGpuShaderType aType) 
	: iGpuShader(asName, aType, eGpuProgramFormat_CG)
	{
		mContext = aContext;

		mProgram = NULL;

		#define CG_CHECK(type, p) else if (msForce##type == #p) { \
			mProfile = cgGLIsProfileSupported(CG_PROFILE_##p) ? CG_PROFILE_##p : CG_PROFILE_UNKNOWN; \
		}
		if(mShaderType == eGpuShaderType_Vertex)
		{
			if (msForceVP == "AUTO") {
				mProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
			} CG_CHECK(VP,ARBVP1)
			CG_CHECK(VP,VP40)
			CG_CHECK(VP,VP30)
			CG_CHECK(VP,VP20)
			CG_CHECK(VP,GLSLV) else {
				Log("Forced VP %s unknown\n",msForceVP.c_str());
				mProfile = CG_PROFILE_UNKNOWN;
			}
		}
		else
		{
			if (msForceFP == "AUTO") {
				mProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
			} CG_CHECK(FP,ARBFP1)
			CG_CHECK(FP,FP40)
			CG_CHECK(FP,FP30)
			CG_CHECK(FP,FP20)
			CG_CHECK(FP,GLSLF) else {
				Log("Forced FP %s unknown\n",msForceFP.c_str());
				mProfile = CG_PROFILE_UNKNOWN;
			}
		}
		#undef CG_CHECK
		if(mbDebugInfo)
		{
			if(mProfile == CG_PROFILE_UNKNOWN)
				Log(" '%s' using profile: UKNOWN\n",asName.c_str());
			else
				Log(" '%s' using profile: '%s'\n",asName.c_str(),cgGetProfileString(mProfile));
		}

		for(int i=0; i< kMaxTextureUnits; ++i)
		{
			mvTexUnitParam[i] = NULL;
		}

		cgGLSetOptimalOptions(mProfile);
	}

	cCGShader::~cCGShader()
	{
		if(mProgram) cgDestroyProgram(mProgram);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cCGShader::Reload()
	{
		return false;
	}
	
	//-----------------------------------------------------------------------

	void cCGShader::Unload()
	{
	}
	
	//-----------------------------------------------------------------------

	void cCGShader::Destroy()
	{
	}
	
	//-----------------------------------------------------------------------

	bool cCGShader::CreateFromFile(const tString &asFile, const tString &asEntry)
	{
		if(mProfile == CG_PROFILE_UNKNOWN)
		{
			CGerror err = cgGetError();
			Log("Could not find a working profile for cg file '%s'!\n",msName.c_str());
			if (err != CG_NO_ERROR)Log(" %s\n", cgGetErrorString(err));
			return false;
		}

		//Log("Creating CG prog\n");

        if(mShaderType == eGpuShaderType_Vertex)
		{
			mProgram = cgCreateProgramFromFile(	mContext, CG_SOURCE, asFile.c_str(),mProfile,
												asEntry.c_str(),NULL);
		}
		else
		{
			if(false)//GLEE_ATI_draw_buffers)
			{
				const char * vArgs[] = {"-po", "ATI_draw_buffers",0};
				mProgram = cgCreateProgramFromFile(	mContext, CG_SOURCE, asFile.c_str(),mProfile,
													asEntry.c_str(),&vArgs[0]);
			}
			else
			{
				mProgram = cgCreateProgramFromFile(	mContext, CG_SOURCE, asFile.c_str(),mProfile,
													asEntry.c_str(),0);
			}
		}
		
		//Log("getting CG errors\n");

		CGerror err = cgGetError();
		if (err != CG_NO_ERROR)
		{
			Log(" %s\n", cgGetErrorString(err));
			const char* pString = cgGetLastListing(mContext);
			int lIdx=0;
			int lLastNewLine=0;
			
			Log(" -----------------------------------\n");
			while(pString[lIdx]!=0)//true)
			{
				Log("%c",pString[lIdx]);
				/*if(pString[lIdx]==0)
				{
					tString sStr = cString::Sub(pString, lLastNewLine,(lIdx-lLastNewLine));
					Log(" %s",sStr.c_str());
					break;
				}
				else if(pString[lIdx]=='\n')
				{
					tString sStr = cString::Sub(pString, lLastNewLine,(lIdx-lLastNewLine));
					Log(" %s\n",sStr.c_str());
					lLastNewLine = lIdx+1;
				}*/
				lIdx++;
			}
			Log(" -----------------------------------\n");
			
			Log("Error loading: '%s'!\n",asFile.c_str());
			return false;
		}
		
		if(mProgram==NULL){
			Log("Error loading: '%s'!\n",asFile.c_str());
			return false;
		}

		//Log("Loading CG program\n");

		cgGLLoadProgram(mProgram);

		err = cgGetError();
		if (err != CG_NO_ERROR)
		{
			Log(" %s\n", cgGetErrorString(err));
			
			cgDestroyProgram(mProgram);
			mProgram = NULL;

			return false;
		}


		msFile = asFile;
		msEntry = asEntry;

		///////////////////////////////
		//Look for texture units.
		//Log("File: %s\n", msFile.c_str());
		int lCount =0;
		CGparameter Param = cgGetFirstParameter(mProgram, CG_PROGRAM);
		for(; Param != NULL; Param = cgGetNextParameter(Param))
		{
			//Check if it is a texture type
			CGparameterclass paramClass = cgGetParameterClass(Param);
			if(	paramClass != CG_PARAMETERCLASS_SAMPLER)
			{
				continue;
			}
			
			//Get the unit number
			int lUnit = lCount;
			const char *pSemantic = cgGetParameterSemantic(Param);
			if(pSemantic)
			{
				lUnit =	cString::ToInt(cString::Sub(pSemantic,7).c_str(),0);
			}
			tString sName = cgGetParameterName(Param);
			//Log(" Texture %d: %s\n",lUnit,sName.c_str());

			mvTexUnitParam[lUnit] = Param;

			++lCount;
		}
		
		return true;
	}

	//-----------------------------------------------------------------------

	void cCGShader::Bind()
	{
		cgGLBindProgram(mProgram);
		cgGLEnableProfile(mProfile);
	}

	//-----------------------------------------------------------------------

	void cCGShader::UnBind()
	{
		cgGLDisableProfile(mProfile);
	}

	//-----------------------------------------------------------------------

	int cCGShader::GetVariableId(const tString& asName)
	{
		CGparameter Param = cgGetNamedParameter(mProgram, asName.c_str());
		if(Param==NULL) return -1;

		mvParameters.push_back(Param);

		return (int)mvParameters.size()-1;
	}
	
	//-----------------------------------------------------------------------
	
	bool  cCGShader::SetFloat(int alVarId, float afX)
	{
		if(alVarId<0 || alVarId >= (int)mvParameters.size()) return false;
		CGparameter& Param = mvParameters[alVarId];
		
		cgGLSetParameter1f(Param, afX);
		return true;
	}

	//-----------------------------------------------------------------------

	bool  cCGShader::SetVec2f(int alVarId, float afX,float afY)
	{
		if(alVarId<0 || alVarId >= (int)mvParameters.size()) return false;
		CGparameter& Param = mvParameters[alVarId];

		cgGLSetParameter2f(Param, afX, afY);
		return true;
	}
	
	//-----------------------------------------------------------------------

	bool  cCGShader::SetVec3f(int alVarId, float afX,float afY,float afZ)
	{
		if(alVarId<0 || alVarId >= (int)mvParameters.size()) return false;
		CGparameter& Param = mvParameters[alVarId];

		cgGLSetParameter3f(Param, afX, afY, afZ);
		return true;
	}
	
	//-----------------------------------------------------------------------
	
	bool  cCGShader::SetVec4f(int alVarId, float afX,float afY,float afZ, float afW)
	{
		if(alVarId<0 || alVarId >= (int)mvParameters.size()) return false;
		CGparameter& Param = mvParameters[alVarId];

		cgGLSetParameter4f(Param, afX, afY, afZ, afW);
		return true;
	}

	//-----------------------------------------------------------------------

	bool cCGShader::SetMatrixf(int alVarId, const cMatrixf& mMtx)
	{
		if(alVarId<0 || alVarId >= (int)mvParameters.size()) return false;
		CGparameter& Param = mvParameters[alVarId];
		
		cgGLSetMatrixParameterfr(Param,&mMtx.m[0][0]);

		return true;
	}
	
	//-----------------------------------------------------------------------

	bool cCGShader::SetMatrixf(int alVarId, eGpuShaderMatrix mType, 
		eGpuShaderMatrixOp mOp)
	{
		if(alVarId<0 || alVarId >= (int)mvParameters.size()) return false;
		CGparameter& Param = mvParameters[alVarId];

        CGGLenum MtxType;
		CGGLenum OpType;
		switch(mType){
			case eGpuShaderMatrix_View:				MtxType=CG_GL_MODELVIEW_MATRIX;break;
			case eGpuShaderMatrix_Projection:		MtxType=CG_GL_PROJECTION_MATRIX;break;
			case eGpuShaderMatrix_Texture:			MtxType=CG_GL_TEXTURE_MATRIX;break;
			case eGpuShaderMatrix_ViewProjection:	MtxType=CG_GL_MODELVIEW_PROJECTION_MATRIX;break;
		}
		switch(mOp){
			case eGpuShaderMatrixOp_Identity:		OpType=CG_GL_MATRIX_IDENTITY; break;
			case eGpuShaderMatrixOp_Inverse:		OpType=CG_GL_MATRIX_INVERSE; break;
			case eGpuShaderMatrixOp_Transpose:		OpType=CG_GL_MATRIX_TRANSPOSE; break;
			case eGpuShaderMatrixOp_InverseTranspose:OpType=CG_GL_MATRIX_INVERSE_TRANSPOSE; break;
		}

        cgGLSetStateMatrixParameter(Param,MtxType, OpType);
		return true;
	}

	//-----------------------------------------------------------------------

	bool cCGShader::SetTexture(int alVarId,iTexture* apTexture, bool abAutoDisable)
	{
		CGtype textureType;

		if(alVarId<0 || alVarId >= (int)mvParameters.size()) return false;
		CGparameter& Param = mvParameters[alVarId];

    	if(apTexture)
		{
			switch(apTexture->GetType())
			{
				case eTextureType_1D:			textureType = CG_SAMPLER1D; break;
				case eTextureType_2D:			textureType = CG_SAMPLER2D; break;
				case eTextureType_CubeMap:		textureType = CG_SAMPLERCUBE; break;
				case eTextureType_Rect:			textureType = CG_SAMPLERRECT; break;
			}
			
			cSDLTexture* pSDLTex = static_cast<cSDLTexture*>(apTexture);

			cgGLSetTextureParameter(Param, pSDLTex->GetTextureHandle());
			cgGLEnableTextureParameter(Param);
		}
		else
		{
				cgGLDisableTextureParameter(Param);
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cCGShader::SetTextureToUnit(int alUnit, iTexture* apTexture)
	{
		if(mvTexUnitParam[alUnit]==NULL || alUnit >= kMaxTextureUnits) return false;

		cSDLTexture* pSDLTex = static_cast<cSDLTexture*>(apTexture);
		
		if(apTexture)
		{
			cgGLSetTextureParameter(mvTexUnitParam[alUnit], pSDLTex->GetTextureHandle());
			cgGLEnableTextureParameter(mvTexUnitParam[alUnit]);
		}
		else
		{
			cgGLDisableTextureParameter(mvTexUnitParam[alUnit]);
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
