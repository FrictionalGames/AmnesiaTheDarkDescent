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

#ifndef HPL_GRAPHICS_H
#define HPL_GRAPHICS_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "engine/Updateable.h"

namespace hpl {

	class cResources;
	class iRenderer;
	class cPostEffectsComposite;
	class iPostEffectType;
	class iPostEffectParams;
	class iPostEffect;
	class iLowLevelResources;
	class iLowLevelGraphics;
	class cMeshCreator;
	class cTextureCreator;
	class cDecalCreator;
	class iFrameBuffer;
	class iDepthStencilBuffer;
	class iTexture;
	class iMaterialType;
	class cPostEffectComposite;
	class iGpuProgram;
	class cParserVarContainer;

	//------------------------------------------------------
	
	class cTempFrameBuffer
	{
	public:
		iFrameBuffer *mpFrameBuffer;
		cVector2l mvSize;
		ePixelFormat mPixelFormat;
		int mlIndex;
	};

	//------------------------------------------------------

	typedef std::list<iFrameBuffer*> tFrameBufferList;
	typedef tFrameBufferList::iterator tFrameBufferListIt;
	
	typedef std::list<iDepthStencilBuffer*> tDepthStencilBufferList;
	typedef tDepthStencilBufferList::iterator tDepthStencilBufferListIt;
	
	typedef std::list<iTexture*> tTextureList;
	typedef tTextureList::iterator tTextureListIt;

	typedef std::list<cPostEffectComposite*> tPostEffectCompositeList;
	typedef tPostEffectCompositeList::iterator tPostEffectCompositeListIt;

	typedef std::list<iPostEffect*> tPostEffectList;
	typedef tPostEffectList::iterator tPostEffectListIt;

	typedef std::map<tString, iMaterialType*> tMaterialTypeMap;
	typedef tMaterialTypeMap::iterator tMaterialTypeMapIt;

	//------------------------------------------------------

	class cGraphics : public iUpdateable
	{
	public:
		cGraphics(iLowLevelGraphics *apLowLevelGraphics,iLowLevelResources *apLowLevelResources);
		~cGraphics();

		bool Init(int alWidth, int alHeight, int alDisplay, int alBpp, int abFullscreen, int alMultisampling,
					eGpuProgramFormat aGpuProgramFormat,const tString &asWindowCaption, const cVector2l &avWindowPos,
					cResources* apResources, tFlag alHplSetupFlags);

		void Update(float afTimeStep);

		cVideoMode* GetValidVideoMode(int alX);
		
		iLowLevelGraphics* GetLowLevel(){ return mpLowLevelGraphics;}

		iRenderer* GetRenderer(eRenderer aType);
		void ReloadRendererData();
		
		iFrameBuffer* CreateFrameBuffer(const tString& asName);
		void DestroyFrameBuffer(iFrameBuffer* apFrameBuffer);

		iFrameBuffer* GetTempFrameBuffer(const cVector2l& avSize, ePixelFormat aPixelFormat, int alIndex);

		iDepthStencilBuffer* CreateDepthStencilBuffer(	const cVector2l& avSize, int alDepthBits, int alStencilBits,
														bool abLookForMatchingFirst);
		iDepthStencilBuffer* FindDepthStencilBuffer(const cVector2l& avSize, int alMinDepthBits, int alMinStencilBits);
		void DestoroyDepthStencilBuffer(iDepthStencilBuffer* apBuffer);
		
		iTexture* CreateTexture(const tString &asName,eTextureType aType,   eTextureUsage aUsage);
		void DestroyTexture(iTexture *apTexture);

		cPostEffectComposite* CreatePostEffectComposite();
		void DestroyPostEffectComposite(cPostEffectComposite* apComposite);
		
        void AddPostEffectType(iPostEffectType *apPostEffectBase);
		
		iPostEffect* CreatePostEffect(iPostEffectParams *apParams);
		void DestroyPostEffect(iPostEffect* apPostEffect);

		iGpuProgram* CreateGpuProgram(const tString& asName);
		iGpuProgram* CreateGpuProgramFromShaders(	const tString& asName, const tString& asVtxShader,const tString& asFragShader,
													cParserVarContainer *apVarContainer);
		void DestroyGpuProgram(iGpuProgram* apProgram);
            
		void AddMaterialType(iMaterialType *apType, const tString& asName);
		iMaterialType *GetMaterialType(const tString& asName);
		tStringVec GetMaterialTypeNames();
		void ReloadMaterials();
		
		cMeshCreator* GetMeshCreator(){return mpMeshCreator;}
		cTextureCreator* GetTextureCreator(){ return mpTextureCreator;}
		cDecalCreator* GetDecalCreator() {return mpDecalCreator;}
		
		bool GetScreenIsSetUp(){ return mbScreenIsSetup;}
	
	private:
		iLowLevelGraphics *mpLowLevelGraphics;
		iLowLevelResources *mpLowLevelResources;
		cMeshCreator *mpMeshCreator;
		cTextureCreator* mpTextureCreator;
		cDecalCreator* mpDecalCreator;
		cResources *mpResources;

		std::vector<cTempFrameBuffer> mvTempFrameBuffers;
		
		std::vector<iRenderer*> mvRenderers;
		std::vector<iPostEffectType*> mvPostEffectTypes;

		tFrameBufferList mlstFrameBuffers;
		tDepthStencilBufferList mlstDepthStencilBuffers;
		tTextureList mlstTextures;
		tPostEffectCompositeList mlstPostEffectComposites;
		tGpuProgramList mlstGpuPrograms;
		tMaterialTypeMap m_mapMaterialTypes;
		tPostEffectList mlstPostEffects;

		bool mbScreenIsSetup;
	};

};
#endif // HPL_GRAPHICS_H
