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

#ifndef HPL_MATERIAL_H
#define HPL_MATERIAL_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"

namespace hpl {

	//---------------------------------------------------

	class cGraphics;
	class cResources;
	class iTexture;
	class iGpuProgram;
	class iMaterialType;
	class cResourceVarsObject;

	//---------------------------------------------------
	
	class iMaterialVars
	{
	public:
		virtual ~iMaterialVars(){}
	};

	//---------------------------------------------------
	
	class cMaterialUvAnimation
	{
	public:
		cMaterialUvAnimation(eMaterialUvAnimation aType, float afSpeed, float afAmp, eMaterialAnimationAxis aAxis) :
							  mType(aType), mfSpeed(afSpeed), mfAmp(afAmp), mAxis(aAxis) {}

	    eMaterialUvAnimation mType;
		
		float mfSpeed;
		float mfAmp;

		eMaterialAnimationAxis mAxis;
	};

	//---------------------------------------------------
	
	class cMaterial : public iResourceBase 
	{
	friend class iMaterialType;
	public:
		cMaterial(const tString& asName, const tWString& asFullPath, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType);
		virtual ~cMaterial();

		void SetType(iMaterialType* apType);
		iMaterialType * GetType(){ return mpType; }

		void Compile();

		void SetTexture(eMaterialTexture aType, iTexture *apTexture);
		iTexture *GetTexture(eMaterialTexture aType);

		void SetVars(iMaterialVars *apVars){ mpVars = apVars;}
		iMaterialVars* GetVars(){ return mpVars;}
		cResourceVarsObject* GetVarsObject();
		void LoadVariablesFromVarsObject(cResourceVarsObject* apVarsObject);

		void SetAutoDestroyTextures(bool abX){ mbAutoDestroyTextures = abX;}

		void SetBlendMode(eMaterialBlendMode aBlendMode);
		void SetAlphaMode(eMaterialAlphaMode aAlphaMode);
		void SetDepthTest(bool abDepthTest);

		bool HasRefraction(){ return mbHasRefraction; }
		bool UseRefractionEdgeCheck(){ return mbUseRefractionEdgeCheck;}
		void SetHasRefraction(bool abX){ mbHasRefraction = abX; }
		void SetUseRefractionEdgeCheck(bool abX){ mbUseRefractionEdgeCheck = abX;}
		
		bool HasWorldReflection(){ return mbHasWorldReflection; }
		void SetHasWorldReflection(bool abX){ mbHasWorldReflection = abX; }
		void  SetWorldReflectionOcclusionTest(bool abX){ mbWorldReflectionOcclusionTest=abX;}
		void SetMaxReflectionDistance(float afX){ mfMaxReflectionDistance = afX;}
		bool  GetWorldReflectionOcclusionTest(){ return mbWorldReflectionOcclusionTest;}
		float GetMaxReflectionDistance(){ return mfMaxReflectionDistance;}

		void SetHasTranslucentIllumination(bool abX){ mbHasTranslucentIllumination = abX;}
		bool HasTranslucentIllumination(){ return mbHasTranslucentIllumination;}

		void SetLargeTransperantSurface(bool abX){ mbLargeTransperantSurface = abX;}
		bool GetLargeTransperantSurface(){ return mbLargeTransperantSurface;}

		bool GetUseAlphaDissolveFilter(){ return mbUseAlphaDissolveFilter;}
		void SetUseAlphaDissolveFilter(bool abX){ mbUseAlphaDissolveFilter = abX;}

		void SetAffectedByFog(bool abX){ mbAffectedByFog = abX;}
		bool GetAffectedByFog(){ return mbAffectedByFog;}
		
		inline iTexture* GetTextureInUnit(eMaterialRenderMode aRenderMode, int alUnit) const { return mvTextureInUnit[aRenderMode][alUnit];}
		inline iGpuProgram* GetProgram(char alSkeleton,eMaterialRenderMode aRenderMode) const { return mvPrograms[alSkeleton][aRenderMode];}
		inline eMaterialBlendMode GetBlendMode() const { return mBlendMode; }
		inline eMaterialAlphaMode GetAlphaMode() const { return mAlphaMode; }
		inline bool GetDepthTest() const { return mbDepthTest; }

		void SetPhysicsMaterial(const tString & asPhysicsMaterial){ msPhysicsMaterial = asPhysicsMaterial;}
		const tString& GetPhysicsMaterial(){ return msPhysicsMaterial;}

		void UpdateBeforeRendering(float afTimeStep);

		inline int GetRenderFrameCount() const { return mlRenderFrameCount;}
		inline void SetRenderFrameCount(const int alCount) { mlRenderFrameCount = alCount;}

		inline bool GetHasSpecificSettings(eMaterialRenderMode aMode) const{ return mbHasSpecificSettings[aMode];}
		void SetHasSpecificSettings(eMaterialRenderMode aMode,bool abX){ mbHasSpecificSettings[aMode] = abX;}

		inline bool HasObjectSpecificsSettings(eMaterialRenderMode aMode)const { return  mbHasObjectSpecificsSettings[aMode];}
		void SetHasObjectSpecificsSettings(eMaterialRenderMode aMode,bool abX){ mbHasObjectSpecificsSettings[aMode] = abX;}

		//Animation
		void AddUvAnimation(eMaterialUvAnimation aType, float afSpeed, float afAmp, eMaterialAnimationAxis aAxis);
		int GetUvAnimationNum(){ return (int)mvUvAnimations.size();}
		cMaterialUvAnimation *GetUvAnimation(int alIdx){ return &mvUvAnimations[alIdx]; }
		inline bool HasUvAnimation()const { return mbHasUvAnimation; }
		inline const cMatrixf& GetUvMatrix() const { return m_mtxUV;}
		void ClearUvAnimations();

		/**
		 * This is used so that materials do not call type specific things after types have been destroyed!
		 * Shall only be set by graphics!
		 */
		static void SetDestroyTypeSpecifics(bool abX){ mbDestroyTypeSpecifics = abX; }
		static bool GetDestroyTypeSpecifics(){ return mbDestroyTypeSpecifics; }

		//resources stuff.
		bool Reload(){ return false;}
		void Unload(){}
		void Destroy(){}
		
	private:
		void UpdateAnimations(float afTimeStep);

		cGraphics *mpGraphics;
		cResources *mpResources;

		iMaterialType *mpType;

		iMaterialVars *mpVars;

		bool mbAutoDestroyTextures;

		bool mbHasSpecificSettings[eMaterialRenderMode_LastEnum];
		bool mbHasObjectSpecificsSettings[eMaterialRenderMode_LastEnum];

		eMaterialBlendMode mBlendMode;
		eMaterialAlphaMode mAlphaMode;
		bool mbDepthTest;

		bool mbHasRefraction;
		int mlRefractionTextureUnit;
		bool mbUseRefractionEdgeCheck;

		bool mbHasWorldReflection;
		int mlWorldReflectionTextureUnit;
		bool mbWorldReflectionOcclusionTest;
		float mfMaxReflectionDistance;

		bool mbHasTranslucentIllumination;

		bool mbLargeTransperantSurface;

		bool mbAffectedByFog;

		bool mbUseAlphaDissolveFilter;

		iGpuProgram *mvPrograms[2][eMaterialRenderMode_LastEnum]; //[2] == If it has skeleton or not.
		iTexture* mvTextures[eMaterialTexture_LastEnum];
		iTexture* mvTextureInUnit[eMaterialRenderMode_LastEnum][kMaxTextureUnits];

		std::vector<cMaterialUvAnimation> mvUvAnimations;
		bool mbHasUvAnimation;
		cMatrixf m_mtxUV;
		float mfAnimTime;
		
		int mlRenderFrameCount;
	
		tString msPhysicsMaterial;

		static bool mbDestroyTypeSpecifics;
	};

	//---------------------------------------------------

};
#endif // HPL_MATERIAL_H
