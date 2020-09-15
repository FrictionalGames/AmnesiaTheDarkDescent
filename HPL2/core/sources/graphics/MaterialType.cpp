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

#include "graphics/MaterialType.h"

#include "system/LowLevelSystem.h"
#include "system/PreprocessParser.h"

#include "graphics/Graphics.h"
#include "graphics/GPUShader.h"
#include "graphics/GPUProgram.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/ProgramComboManager.h"

#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/GpuShaderManager.h"

#include "math/Math.h"

#include "system/String.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iMaterialType::iMaterialType(cGraphics *apGraphics, cResources *apResources)
	{
		mpGraphics = apGraphics;
		mpResources = apResources;

		mbIsTranslucent = false;
		mbIsDecal = false;
		
		for(int i=0; i<eMaterialRenderMode_LastEnum; ++i)
		{
			mbHasTypeSpecifics[i] = false;
		}
		
		//Need to do this to support NULL materials (that do not use graphical stuff)
		if(mpGraphics && mpResources)
			mpProgramManager = hplNew( cProgramComboManager, ("",mpGraphics, mpResources,eMaterialRenderMode_LastEnum ));
		else
			mpProgramManager = NULL;
	}

	iMaterialType::~iMaterialType()
	{
		if(mpProgramManager) hplDelete( mpProgramManager );
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void iMaterialType::SetName(const tString& asName)
	{ 
		msName = asName;
		mpProgramManager->SetName(asName);
	}
	
	//-----------------------------------------------------------------------

	cMaterialUserVariable* iMaterialType::GetUserVariable(int alIdx)
	{
		if(alIdx>=0 && alIdx<GetUserVariableNum())
			return &mvUserVariables[alIdx];

		return NULL;
	}

	cMaterialUserVariable* iMaterialType::GetUserVariable(const tString& asName)
	{
		for(int i=0;i<GetUserVariableNum();++i)
		{
			cMaterialUserVariable* pVar = &mvUserVariables[i];
			if(pVar->msName==asName)
				return pVar;
		}
		
		return NULL;
	}

	//-----------------------------------------------------------------------

	void iMaterialType::Reload()
	{
		DestroyData();
		LoadData();
	}
	
	//-----------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iMaterialType::AddUsedTexture(eMaterialTexture aType)
	{
		cMaterialUsedTexture usedTexture;
		usedTexture.mType = aType;
		mvUsedTextures.push_back( usedTexture );
	}

	//-----------------------------------------------------------------------

	void iMaterialType::AddVar(const tString& asName, eVariableType aType, const tString& asDefaultValue, const tString& asDesc, 
							   const tStringVec& avEnumValues)
	{
		cMaterialUserVariable userVariable;
		userVariable.msName = asName;
		userVariable.mType = aType;
		userVariable.msValue = asDefaultValue;
		userVariable.msDescription = asDesc;
		userVariable.mvEnumValues = avEnumValues;
		mvUserVariables.push_back(userVariable);
	}

	void iMaterialType::AddVarBool(const tString& asName, bool abDefaultValue, const tString& asDesc)
	{
		AddVar(asName, eVariableType_Bool, abDefaultValue ? "true" : "false", asDesc);
	}

	void iMaterialType::AddVarInt(const tString& asName, int alDefaultValue, const tString& asDesc)
	{
		AddVar(asName, eVariableType_Int, cString::ToString(alDefaultValue), asDesc);
	}

	void iMaterialType::AddVarFloat(const tString& asName, float afDefaultValue, const tString& asDesc)
	{
		AddVar(asName, eVariableType_Float, cString::ToString(afDefaultValue, -1, true), asDesc);
	}

	void iMaterialType::AddVarVec2(const tString& asName, const cVector2f& avDefaultValue, const tString& asDesc)
	{
		tString sValue = cString::ToString(avDefaultValue.x, -1, true) + " " + 
						 cString::ToString(avDefaultValue.y, -1, true);
		AddVar(asName, eVariableType_Vec2, sValue, asDesc);
	}

	void iMaterialType::AddVarVec3(const tString& asName, const cVector3f& avDefaultValue, const tString& asDesc)
	{
		tString sValue = cString::ToString(avDefaultValue.x, -1, true) + " " + 
						 cString::ToString(avDefaultValue.y, -1, true) + " " + 
						 cString::ToString(avDefaultValue.z, -1, true);
		AddVar(asName, eVariableType_Vec3, sValue, asDesc);
	}

	void iMaterialType::AddVarColor(const tString& asName, const cColor& aDefaultValue, const tString& asDesc)
	{
		tString sValue = cString::ToString(aDefaultValue.r, -1, true) + " " + 
						 cString::ToString(aDefaultValue.g, -1, true) + " " + 
						 cString::ToString(aDefaultValue.b, -1, true) + " " + 
						 cString::ToString(aDefaultValue.a, -1, true);
		AddVar(asName, eVariableType_Color, sValue, asDesc);
	}

	void iMaterialType::AddVarString(const tString& asName, const tString& asDefaultValue, const tString& asDesc)
	{
		AddVar(asName, eVariableType_String, asDefaultValue, asDesc);
	}

	void iMaterialType::AddVarEnum(const tString& asName, const tString& asDefaultValue, const tStringVec& avEnumValues, const tString& asDesc)
	{
		AddVar(asName, eVariableType_Enum, asDefaultValue, asDesc, avEnumValues);
	}

	//-----------------------------------------------------------------------
}
