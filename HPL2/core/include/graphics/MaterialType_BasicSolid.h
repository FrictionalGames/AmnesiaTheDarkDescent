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

#ifndef HPL_MATERIAL_TYPE_BASIC_SURFACES_H
#define HPL_MATERIAL_TYPE_BASIC_SURFACES_H

#include "graphics/MaterialType.h"
#include "graphics/Material.h"

namespace hpl {

	//---------------------------------------------------

	class iMaterialVars;
	
	//---------------------------------------------------
	// SOLID BASE
	//---------------------------------------------------
	
	class iMaterialType_SolidBase : public iMaterialType
	{
	public:
		iMaterialType_SolidBase(cGraphics *apGraphics, cResources *apResources);
		~iMaterialType_SolidBase();

		void DestroyProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, char alSkeleton);
			
		bool SupportsHWSkinning(){ return true; }
		
		void CreateGlobalPrograms();

		iMaterialVars* CreateSpecificVariables() { return NULL; }
		void LoadVariables(cMaterial *apMaterial, cResourceVarsObject *apVars);
		void GetVariableValues(cMaterial *apMaterial, cResourceVarsObject* apVars);

		void CompileMaterialSpecifics(cMaterial *apMaterial);
		
	protected:
		virtual void CompileSolidSpecifics(cMaterial *apMaterial){}

		virtual void LoadSpecificData()=0;

		void LoadData();
		void DestroyData();

		bool mbIsGlobalDataCreator;
		static bool mbGlobalDataCreated;
		static cProgramComboManager* mpGlobalProgramManager;
		//[skeleton][uv animation]

		iTexture *mpDissolveTexture;

		static float mfVirtualPositionAddScale;
	};

	//---------------------------------------------------
	// SOLID DIFFUSE 
	//---------------------------------------------------

	class cMaterialType_SolidDiffuse_Vars : public iMaterialVars
	{
	public:
		cMaterialType_SolidDiffuse_Vars() : mfHeightMapScale(0.05f), mfHeightMapBias(0.0f), mbAlphaDissolveFilter(false) {}
		~cMaterialType_SolidDiffuse_Vars(){}

		float mfHeightMapScale;
		float mfHeightMapBias;
		float mfFrenselBias;
		float mfFrenselPow;
		bool mbAlphaDissolveFilter;
	};

	//---------------------------------------------------

	class cMaterialType_SolidDiffuse : public iMaterialType_SolidBase
	{
	public:
		cMaterialType_SolidDiffuse(cGraphics *apGraphics, cResources *apResources);
		~cMaterialType_SolidDiffuse();

		bool SupportsHWSkinning(){ return true; }

		iTexture* GetTextureForUnit(cMaterial *apMaterial,eMaterialRenderMode aRenderMode, int alUnit);
		iTexture* GetSpecialTexture(cMaterial *apMaterial, eMaterialRenderMode aRenderMode,iRenderer *apRenderer, int alUnit);
		
		iGpuProgram* GetGpuProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, char alSkeleton);

		void SetupTypeSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, iRenderer *apRenderer);
		void SetupMaterialSpecificData(	eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, cMaterial *apMaterial,
										iRenderer *apRenderer);
		void SetupObjectSpecificData(	eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, iRenderable *apObject,
										iRenderer *apRenderer);

		iMaterialVars* CreateSpecificVariables();
		void LoadVariables(cMaterial *apMaterial, cResourceVarsObject *apVars);
		void GetVariableValues(cMaterial *apMaterial, cResourceVarsObject *apVars);
	
	private:
		void CompileSolidSpecifics(cMaterial *apMaterial);

		void LoadSpecificData();
	};

	//---------------------------------------------------

};
#endif // HPL_MATERIAL_TYPE_BASIC_SURFACES_H
