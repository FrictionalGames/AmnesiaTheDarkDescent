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

#ifndef HPL_LIGHT_H
#define HPL_LIGHT_H

#include "scene/Entity3D.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/Renderable.h"

class TiXmlElement;

namespace hpl {
	
	//------------------------------------------

	class iLowLevelGraphics;
	class cRenderSettings;
	class cCamera;
	class cFrustum;
	class iGpuProgram;
	class iTexture;
	class cTextureManager;
	class cResources;
	class cFileSearcher;
	class cBillboard;
	class cSectorVisibilityContainer;
	class cWorld;
	class cVisibleRCNodeTracker;
	
	//------------------------------------------
	
	enum eLightType
	{
		eLightType_Point,
		eLightType_Spot,
		eLightType_Box,
		eLightType_LastEnum
	};

	enum eShadowVolumeType
	{
		eShadowVolumeType_None,
		eShadowVolumeType_ZPass,
		eShadowVolumeType_ZFail,
		eShadowVolumeType_LastEnum,
	};

	//------------------------------------------

	typedef std::map<iRenderable*, int> tShadowCasterCacheMap;
	typedef tShadowCasterCacheMap::iterator tShadowCasterCacheMapIt;

	//------------------------------------------

	class cLightBillboardConnection
	{
	public:
		cBillboard *mpBillboard;
		cColor mBaseColor;
	};

	//------------------------------------------

	class iLight : public iRenderable
	{
	#ifdef __GNUC__
		typedef iRenderable __super;
	#endif
	public:
		iLight(tString asName, cResources *apResources);
		virtual ~iLight();

		void UpdateLogic(float afTimeStep);

		bool CheckObjectIntersection(iRenderable *apObject);
		
		eLightType GetLightType(){ return mLightType;}

		iTexture *GetFalloffMap();
		void SetFalloffMap(iTexture* apTexture);
		
		void SetGoboTexture(iTexture *apTexture);
		iTexture* GetGoboTexture();

		///////////////////////////////
		//iEntity implementation
		tString GetEntityType(){ return "iLight";}

		virtual bool IsVisible();
		void OnChangeVisible();
		
		///////////////////////////////
		//Renderable implementation:
		cMaterial *GetMaterial(){ return NULL;}
		iVertexBuffer* GetVertexBuffer(){ return NULL;}

		eRenderableType GetRenderType(){ return eRenderableType_Light;}

		cBoundingVolume* GetBoundingVolume();

		int GetMatrixUpdateCount(){ return GetTransformUpdateCount();}

		cMatrixf* GetModelMatrix(cFrustum* apFrustum);

		inline void RenderShadow(iRenderable *apObject,cRenderSettings *apRenderSettings,iLowLevelGraphics *apLowLevelGraphics);

		void LoadXMLProperties(const tString asFile);

		void AttachBillboard(cBillboard *apBillboard, const cColor &aBaseColor);
		void RemoveBillboard(cBillboard *apBillboard);
		std::vector<cLightBillboardConnection>* GetBillboardVec(){ return &mvBillboards;}

		//////////////////////////
		//Shadow caster cache
		void AddShadowCaster(iRenderable *apObject);
		bool ShadowCasterIsValid(iRenderable *apObject);
		bool ShadowCastersAreUnchanged(const tRenderableVec &avObjects);
		void SetShadowCasterCacheFromVec(const tRenderableVec &avObjects);
		void ClearShadowCasterCache();

        //////////////////////////
		//Fading
		void FadeTo(const cColor& aCol, float afRadius, float afTime);
		void StopFading();
		bool IsFading();
		cColor GetDestColor(){ return mDestCol;}
		float GetDestRadius(){ return mfDestRadius;}


		//////////////////////////
		//FLickering
		void SetFlickerActive(bool abX);
		bool GetFlickerActive(){return mbFlickering;}

		void SetFlicker(const cColor& aOffCol, float afOffRadius,
			float afOnMinLength, float afOnMaxLength,const tString &asOnSound,const tString &asOnPS,
			float afOffMinLength, float afOffMaxLength,const tString &asOffSound,const tString &asOffPS,
			bool abFade,	float afOnFadeMinLength, float afOnFadeMaxLength, 
							float afOffFadeMinLength, float afOffFadeMaxLength);

		tString GetFlickerOffSound(){ return msFlickerOffSound;}
		tString GetFlickerOnSound(){ return msFlickerOnSound;}
		tString GetFlickerOffPS(){ return msFlickerOffPS;}
		tString GetFlickerOnPS(){ return msFlickerOnPS;}
		float GetFlickerOnMinLength(){ return mfFlickerOnMinLength;}
		float GetFlickerOffMinLength(){ return mfFlickerOffMinLength;}
		float GetFlickerOnMaxLength(){ return mfFlickerOnMaxLength;}
		float GetFlickerOffMaxLength(){ return mfFlickerOffMaxLength;}
		cColor GetFlickerOffColor(){ return mFlickerOffColor;}
		float GetFlickerOffRadius(){ return mfFlickerOffRadius;}
		bool GetFlickerFade(){ return mbFlickerFade;}
		float GetFlickerOnFadeMinLength(){ return mfFlickerOnFadeMinLength;}
		float GetFlickerOnFadeMaxLength(){ return mfFlickerOnFadeMaxLength;}
		float GetFlickerOffFadeMinLength(){ return mfFlickerOffFadeMinLength;}
		float GetFlickerOffFadeMaxLength(){ return mfFlickerOnFadeMaxLength;}

		cColor GetFlickerOnColor(){ return mFlickerOnColor;}
		float GetFlickerOnRadius(){ return mfFlickerOnRadius;}

		//////////////////////////
		//Properties
		const cColor& GetDiffuseColor(){ return mDiffuseColor; }
		void SetDiffuseColor(cColor aColor);
		
		const cColor&  GetDefaultDiffuseColor(){ return mDefaultDiffuseColor;}
		void SetDefaultDiffuseColor(const cColor& aColor) { mDefaultDiffuseColor = aColor; }
		
		const cColor& GetSpecularColor(){ return mSpecularColor; }
		void SetSpecularColor(cColor aColor){ mSpecularColor = aColor; }

		bool GetCastShadows(){ return mbCastShadows;}
		void SetCastShadows(bool afX){ mbCastShadows = afX;}

		tObjectVariabilityFlag GetShadowCastersAffected(){ return mlShadowCastersAffected;}
		void SetShadowCastersAffected(tObjectVariabilityFlag alX){ mlShadowCastersAffected = alX;}

		inline eShadowMapResolution GetShadowMapResolution() const{ return mShadowMapResolution;}
		inline void SetShadowMapResolution(eShadowMapResolution aQuality){ mShadowMapResolution  = aQuality;}

		inline float GetShadowMapBlurAmount() const{ return mfShadowMapBlurAmount;}
		inline void SetShadowMapBlurAmount(float afX){ mfShadowMapBlurAmount  = afX;}

		inline bool GetOcclusionCullShadowCasters() const{ return mbOcclusionCullShadowCasters;}
		inline void SetOcclusionCullShadowCasters(bool abX){ mbOcclusionCullShadowCasters  = abX;}

		inline cVisibleRCNodeTracker * GetVisibleNodeTracker(){ return mpVisibleNodeTracker;}

		float GetShadowMapBiasMul(){ return mfShadowMapBiasMul;}
		float GetShadowMapSlopeScaleBiasMul(){ return mfShadowMapSlopeScaleBiasMul;}
		void SetShadowMapBiasMul(float afX){ mfShadowMapBiasMul = afX;}
		void SetShadowMapSlopeScaleBiasMul(float afX){ mfShadowMapSlopeScaleBiasMul = afX;}
		
		virtual void SetRadius(float afX);
		float GetRadius(){return mfRadius;}


		float GetSourceRadius(){ return mfSourceRadius;}
		void SetSourceRadius(float afX){ mfSourceRadius = afX;}

		void UpdateLight(float afTimeStep);

		void SetWorld(cWorld *apWorld){ mpWorld = apWorld;}


	protected:
		void OnFlickerOff();
		void OnFlickerOn();
		void OnSetDiffuse();

        virtual void ExtraXMLProperties(TiXmlElement *apMainElem){}
		virtual void UpdateBoundingVolume()=0;
		
		eLightType mLightType;

		cTextureManager *mpTextureManager;
		cFileSearcher *mpFileSearcher;
		cWorld *mpWorld;

		iTexture *mpFalloffMap;
		
		iTexture *mpGoboTexture;

		eShadowMapResolution mShadowMapResolution;
		float mfShadowMapBlurAmount;
		bool mbOcclusionCullShadowCasters;

		cVisibleRCNodeTracker *mpVisibleNodeTracker;

		std::vector<cLightBillboardConnection> mvBillboards;

		cColor mDiffuseColor;
		cColor mDefaultDiffuseColor;

		cColor mSpecularColor;
		float mfSourceRadius;
		float mfRadius;

		bool mbCastShadows;
		tObjectVariabilityFlag mlShadowCastersAffected;

		tShadowCasterCacheMap m_mapShadowCasterCache;

		float mfShadowMapBiasMul;
		float mfShadowMapSlopeScaleBiasMul;

		///////////////////////////
		//Fading.
		cColor mColAdd;
		float mfRadiusAdd;
		cColor mDestCol;
		float mfDestRadius;
		float mfFadeTime;

		///////////////////////////
		//Flicker
		bool mbFlickering;
		tString msFlickerOffSound;
		tString msFlickerOnSound;
		tString msFlickerOffPS;
		tString msFlickerOnPS;
		float mfFlickerOnMinLength;
		float mfFlickerOffMinLength;
		float mfFlickerOnMaxLength;
		float mfFlickerOffMaxLength;
		cColor mFlickerOffColor;
		float mfFlickerOffRadius;
		bool mbFlickerFade;
		float mfFlickerOnFadeMinLength;
		float mfFlickerOnFadeMaxLength;
		float mfFlickerOffFadeMinLength;
		float mfFlickerOffFadeMaxLength;

		cColor mFlickerOnColor;
		float mfFlickerOnRadius;

		bool mbFlickerOn;
		float mfFlickerTime;
		float mfFlickerStateLength;
	};

	typedef std::list<iLight*> tLightList;
	typedef tLightList::iterator tLightListIt;
};
#endif // HPL_LIGHT_H
