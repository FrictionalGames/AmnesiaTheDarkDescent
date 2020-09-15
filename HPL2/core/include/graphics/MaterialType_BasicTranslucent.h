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

#ifndef HPL_MATERIAL_TYPE_BASIC_TRANSLUCENT_H
#define HPL_MATERIAL_TYPE_BASIC_TRANSLUCENT_H

#include "graphics/MaterialType.h"
#include "graphics/Material.h"

namespace hpl {

	//---------------------------------------------------
	// TRANSLUCENT
	//---------------------------------------------------

	//--------------------------------------------------

	class cMaterialType_Translucent_Vars : public iMaterialVars
	{
	public:
		cMaterialType_Translucent_Vars() :	mbRefraction(false), mbRefractionEdgeCheck(true), mbRefractionNormals(false), mfRefractionScale(0.1f), 
											mfFrenselBias(0.2f), mfFrenselPow(8.0f) {}
		~cMaterialType_Translucent_Vars(){}

		bool mbRefraction;
		bool mbRefractionEdgeCheck;
		bool mbRefractionNormals;
		float mfRefractionScale;
		float mfFrenselBias;
		float mfFrenselPow;
		float mfRimLightMul;
		float mfRimLightPow;
		bool mbAffectedByLightLevel;
	};

	//--------------------------------------------------

	class cMaterialType_Translucent : public iMaterialType
	{
	public:
		cMaterialType_Translucent(cGraphics *apGraphics, cResources *apResources);
		~cMaterialType_Translucent();

		void DestroyProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, char alSkeleton);

		bool SupportsHWSkinning(){ return false; }

		iTexture* GetTextureForUnit(cMaterial *apMaterial,eMaterialRenderMode aRenderMode, int alUnit);
		iTexture* GetSpecialTexture(cMaterial *apMaterial, eMaterialRenderMode aRenderMode,iRenderer *apRenderer, int alUnit);

		iGpuProgram* GetGpuProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, char alSkeleton);

		void SetupTypeSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram,iRenderer *apRenderer);
		void SetupMaterialSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, cMaterial *apMaterial,iRenderer *apRenderer);
		void SetupObjectSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, iRenderable *apObject,iRenderer *apRenderer);

		iMaterialVars* CreateSpecificVariables();
		void LoadVariables(cMaterial *apMaterial, cResourceVarsObject *apVars);
		void GetVariableValues(cMaterial *apMaterial, cResourceVarsObject *apVars);

		void CompileMaterialSpecifics(cMaterial *apMaterial);
	
	private:
		void LoadData();
		void DestroyData();

		cProgramComboManager *mpBlendProgramManager[5];
	};

	//---------------------------------------------------

};
#endif // HPL_MATERIAL_TYPE_BASIC_TRANSLUCENT_H
