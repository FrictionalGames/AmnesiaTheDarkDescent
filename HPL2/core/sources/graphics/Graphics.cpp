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

#include "graphics/Graphics.h"

#include "engine/EngineTypes.h"
#include "engine/Updateable.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/Platform.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/MeshCreator.h"
#include "graphics/TextureCreator.h"
#include "graphics/DecalCreator.h"
#include "graphics/FrameBuffer.h"
#include "graphics/PostEffectComposite.h"
#include "graphics/PostEffect.h"
#include "graphics/MaterialType.h"
#include "graphics/Texture.h"
#include "graphics/GPUProgram.h"

#include "resources/LowLevelResources.h"
#include "resources/Resources.h"
#include "resources/GpuShaderManager.h"

#include "graphics/MaterialType_BasicSolid.h"
#include "graphics/MaterialType_BasicTranslucent.h"
#include "graphics/MaterialType_Water.h"
#include "graphics/MaterialType_Decal.h"

#include "graphics/PostEffect_Bloom.h"
#include "graphics/PostEffect_ColorConvTex.h"
#include "graphics/PostEffect_ImageTrail.h"
#include "graphics/PostEffect_RadialBlur.h"

#include "graphics/RendererDeferred.h"
#include "graphics/RendererWireFrame.h"
#include "graphics/RendererSimple.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGraphics::cGraphics(iLowLevelGraphics *apLowLevelGraphics, iLowLevelResources *apLowLevelResources) : iUpdateable("HPL_Graphics")
	{
		mpLowLevelGraphics = apLowLevelGraphics;
		mpLowLevelResources = apLowLevelResources;

		mpMeshCreator = NULL;
		mpTextureCreator = NULL;
		mpDecalCreator = NULL;
	}

	//-----------------------------------------------------------------------

	cGraphics::~cGraphics()
	{
		Log("Exiting Graphics Module\n");
		Log("--------------------------------------------------------\n");

		tMaterialTypeMapIt it = m_mapMaterialTypes.begin();
		for(;it != m_mapMaterialTypes.end(); ++it)
		{
			iMaterialType *pType = it->second;
			pType->DestroyData();
		}
		STLMapDeleteAll(m_mapMaterialTypes);
		cMaterial::SetDestroyTypeSpecifics(false); //Material types are destroyed! Remaining materials may not call!

		STLDeleteAll(mvPostEffectTypes);

		for(size_t i=0; i<mvRenderers.size(); ++i)
		{
			if(mvRenderers[i])
			{
				mvRenderers[i]->DestroyData();
				hplDelete(mvRenderers[i])
			}
		}
		mvRenderers.clear();

		STLDeleteAll(mlstPostEffectComposites);
		STLDeleteAll(mlstPostEffects);
		STLDeleteAll(mlstFrameBuffers);
		STLDeleteAll(mlstDepthStencilBuffers);
		STLDeleteAll(mlstGpuPrograms);
		STLDeleteAll(mlstTextures);
		
		hplDelete(mpMeshCreator);
		hplDelete(mpTextureCreator);
		hplDelete(mpDecalCreator);
		
		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cGraphics::Init(	int alWidth, int alHeight, int alDisplay, int alBpp, int abFullscreen, 
							int alMultisampling,eGpuProgramFormat aGpuProgramFormat,
							const tString &asWindowCaption, const cVector2l &avWindowPos,
							cResources* apResources,
							tFlag alHplSetupFlags)
	{
		Log("Initializing Graphics Module\n");
		Log("--------------------------------------------------------\n");
		
		mpResources = apResources;

		////////////////////////////////////////////////
		//Setup the graphic directories:
		apResources->AddResourceDir(_W("core/shaders"),false);
		apResources->AddResourceDir(_W("core/textures"),false);
		apResources->AddResourceDir(_W("core/models"),false);
		
		////////////////////////////////////////////////
		// LowLevel Init
		if(alHplSetupFlags & eHplSetup_Screen)
		{
			Log("Init lowlevel graphics: %dx%d disp:%d bpp:%d fs:%d ms:%d gpufmt:%d cap:'%s' pos:(%dx%d)\n",alWidth,alHeight,alDisplay,alBpp,abFullscreen,alMultisampling,aGpuProgramFormat, asWindowCaption.c_str(), avWindowPos.x,avWindowPos.y);
			mpLowLevelGraphics->Init(alWidth,alHeight,alDisplay,alBpp,abFullscreen,alMultisampling,aGpuProgramFormat,asWindowCaption,
									avWindowPos);
			mbScreenIsSetup = true;
		}
		else
		{
			mbScreenIsSetup = false;
		}
		

		////////////////////////////////////////////////
		// Create systems
		mpMeshCreator = hplNew( cMeshCreator,(mpLowLevelGraphics, apResources));
		mpTextureCreator  = hplNew( cTextureCreator,(mpLowLevelGraphics, apResources));
		mpDecalCreator = hplNew( cDecalCreator,(mpLowLevelGraphics, apResources));

		////////////////////////////////////////////////
		// Create Renderers
		if(alHplSetupFlags & eHplSetup_Screen)
		{
            ////////////////////////////////////////////////
            // Check feature support
            apResources->GetGpuShaderManager()->CheckFeatureSupport();

			mvRenderers.resize(eRenderer_LastEnum, NULL);

			mvRenderers[eRenderer_Main] = hplNew(cRendererDeferred, (this, apResources));
			mvRenderers[eRenderer_WireFrame] = hplNew(cRendererWireFrame, (this, apResources));
			mvRenderers[eRenderer_Simple] = hplNew(cRendererSimple, (this, apResources));

			for(size_t i=0; i<mvRenderers.size(); ++i)
			{
				if(mvRenderers[i])
				{
					if(mvRenderers[i]->LoadData()==false)
					{
						FatalError("Renderer #%d could not be initialized! Make sure your graphic card drivers are up to date. Check log file for more information.\n", i);
					}
				}
			}
		}
		else
		{
			
		}
		
		////////////////////////////////////////////////
		// Create Data
		if(alHplSetupFlags & eHplSetup_Screen)
		{
			////////////////////////////////////////////////
			//Add all the materials.
			Log(" Adding engine materials\n");

			AddMaterialType(hplNew( cMaterialType_SolidDiffuse, (this, apResources) ), "soliddiffuse");
			AddMaterialType(hplNew( cMaterialType_Translucent, (this, apResources) ), "translucent");
			AddMaterialType(hplNew( cMaterialType_Water, (this, apResources) ), "water");
			AddMaterialType(hplNew( cMaterialType_Decal, (this, apResources) ), "decal");


			////////////////////////////////////////////////
			//Add all the post effects
			Log(" Adding engine post effects\n");
			AddPostEffectType(hplNew( cPostEffectType_Bloom, (this, apResources)) );
			AddPostEffectType(hplNew( cPostEffectType_ColorConvTex, (this, apResources)) );
			AddPostEffectType(hplNew( cPostEffectType_ImageTrail, (this, apResources)) );
			AddPostEffectType(hplNew( cPostEffectType_RadialBlur, (this, apResources)) );
		}
		
		Log("--------------------------------------------------------\n\n");
		
		return true;
	}

	//-----------------------------------------------------------------------

	void cGraphics::Update(float afTimeStep)
	{
		for(size_t i=0; i< mvRenderers.size(); ++i)
		{
			iRenderer *pRenderer = mvRenderers[i];

			pRenderer->Update(afTimeStep);
		}
	}

	//-----------------------------------------------------------------------

	iRenderer* cGraphics::GetRenderer(eRenderer aType)
	{
		if(aType >= (int)mvRenderers.size()) return NULL;

		return mvRenderers[aType];
	}

	//-----------------------------------------------------------------------

	void cGraphics::ReloadRendererData()
	{
		for(size_t i=0; i< mvRenderers.size(); ++i)
		{
			iRenderer *pRenderer = mvRenderers[i];

			pRenderer->DestroyData();
			pRenderer->LoadData();
		}
	}

	//-----------------------------------------------------------------------
	
	iFrameBuffer* cGraphics::CreateFrameBuffer(const tString& asName)
	{
		iFrameBuffer* pFrameBuffer = mpLowLevelGraphics->CreateFrameBuffer(asName);
		if(pFrameBuffer == NULL)
		{
			Error("Could not create a frame buffer!\n");
			return NULL;
		}

		mlstFrameBuffers.push_back(pFrameBuffer);

		return pFrameBuffer;
	}
	
	void cGraphics::DestroyFrameBuffer(iFrameBuffer* apFrameBuffer)
	{
		STLFindAndDelete(mlstFrameBuffers,apFrameBuffer);
	}

	//-----------------------------------------------------------------------

	iFrameBuffer* cGraphics::GetTempFrameBuffer(const cVector2l& avSize, ePixelFormat aPixelFormat, int alIndex)
	{
		/////////////////////////
		// Try and find existing frame buffer
		for(size_t i=0; i<mvTempFrameBuffers.size(); ++i)
		{
			cTempFrameBuffer &tempBuffer = mvTempFrameBuffers[i];
			if(	tempBuffer.mvSize == avSize && tempBuffer.mPixelFormat == aPixelFormat && 
				tempBuffer.mlIndex == alIndex)
			{
				return tempBuffer.mpFrameBuffer;
			}
		}

		/////////////////////////
		// Create new buffer
		cTempFrameBuffer tempBuffer;
		tempBuffer.mvSize = avSize;
		tempBuffer.mPixelFormat = aPixelFormat;
		tempBuffer.mlIndex = alIndex;

		//Create texture
		tString sNameSuffix = cString::ToString(avSize.x)+"x"+cString::ToString(avSize.y)+":"+cString::ToString((int)aPixelFormat);
		iTexture *pTexture = CreateTexture("TempBufferTexture"+sNameSuffix, eTextureType_Rect, eTextureUsage_RenderTarget);
		pTexture->CreateFromRawData(cVector3l(avSize.x, avSize.y,0),aPixelFormat,NULL);
		pTexture->SetWrapSTR(eTextureWrap_ClampToEdge);

		//Create frame buffer
        iFrameBuffer *pFrameBuffer = CreateFrameBuffer("TempBuffer"+sNameSuffix);
		pFrameBuffer->SetTexture2D(0, pTexture);
		if(pFrameBuffer->CompileAndValidate()==false)
		{
			DestroyFrameBuffer(pFrameBuffer);
			DestroyTexture(pTexture);
			return NULL;
		}
		tempBuffer.mpFrameBuffer = pFrameBuffer;

		mvTempFrameBuffers.push_back(tempBuffer);

		return pFrameBuffer;
	}

	//-----------------------------------------------------------------------

	iDepthStencilBuffer* cGraphics::CreateDepthStencilBuffer(const cVector2l& avSize, int alDepthBits, int alStencilBits, bool abLookForMatchingFirst)
	{
		iDepthStencilBuffer* pBuffer = NULL;
		
		//////////////////////////////////////////
		// Check for matching
		if(abLookForMatchingFirst)
		{
			pBuffer = FindDepthStencilBuffer(avSize, alDepthBits, alStencilBits);
		}
		//////////////////////////////////////////
		// Create frame buffer and add to list
		if(pBuffer == NULL)
		{
			pBuffer = mpLowLevelGraphics->CreateDepthStencilBuffer(avSize,alDepthBits,alStencilBits);
			if(pBuffer == NULL)
			{
				Error("Could not create a depth stencil buffer size %dx%d, depthbits: %d stencilbits: %d\n",avSize.x, avSize.y,alDepthBits,alStencilBits);
				return NULL;
			}

			mlstDepthStencilBuffers.push_back(pBuffer);
		}
		
		//////////////////////////////////////////
		// Increase user count and return
		pBuffer->IncUserCount();

		return pBuffer;
	}

	//-----------------------------------------------------------------------

	iDepthStencilBuffer* cGraphics::FindDepthStencilBuffer(const cVector2l& avSize, int alMinDepthBits, int alMinStencilBits)
	{
		tDepthStencilBufferListIt it = mlstDepthStencilBuffers.begin();
		for(; it != mlstDepthStencilBuffers.end(); ++it)
		{
			iDepthStencilBuffer *pBuffer = *it;
            
			if(	pBuffer->GetSize() == avSize && 
				pBuffer->GetDepthBits() >= alMinDepthBits && 
				pBuffer->GetStencilBits() >= alMinStencilBits)
			{
				return pBuffer;
			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------

	void cGraphics::DestoroyDepthStencilBuffer(iDepthStencilBuffer* apBuffer)
	{
		apBuffer->DecUserCount();
		
		if(apBuffer->HasUsers()==false)
		{
			STLFindAndDelete(mlstDepthStencilBuffers,apBuffer);
		}
	}

	//-----------------------------------------------------------------------

	iTexture* cGraphics::CreateTexture(const tString &asName,eTextureType aType,   eTextureUsage aUsage)
	{	
		iTexture *pTexture = mpLowLevelGraphics->CreateTexture(asName,aType, aUsage);
		mlstTextures.push_back(pTexture);
		return pTexture;
	}

	void cGraphics::DestroyTexture(iTexture *apTexture)
	{
		STLFindAndDelete(mlstTextures, apTexture);
	}

	//-----------------------------------------------------------------------
	
	cPostEffectComposite* cGraphics::CreatePostEffectComposite()
	{
		cPostEffectComposite *pComposite = hplNew( cPostEffectComposite, (this) );
		mlstPostEffectComposites.push_back(pComposite);

		return pComposite;
	}
	
	void cGraphics::DestroyPostEffectComposite(cPostEffectComposite* apComposite)
	{
		STLFindAndDelete(mlstPostEffectComposites, apComposite);
	}

	//-----------------------------------------------------------------------

	void  cGraphics::AddPostEffectType(iPostEffectType *apPostEffectBase)
	{
		mvPostEffectTypes.push_back(apPostEffectBase);
	}

	//-----------------------------------------------------------------------

	iPostEffect* cGraphics::CreatePostEffect(iPostEffectParams *apParams)
	{
		iPostEffectType *pType = (iPostEffectType*)STLFindByName(mvPostEffectTypes, apParams->GetName());
		if(pType == NULL){
			Error("Could not find post effect type %s\n", apParams->GetName().c_str());
			return NULL;
		}

		iPostEffect *pPostEffect = pType->CreatePostEffect(apParams);
		pPostEffect->SetParams(apParams);

		mlstPostEffects.push_back(pPostEffect);

		return pPostEffect;
	}
	
	//-----------------------------------------------------------------------

	void cGraphics::DestroyPostEffect(iPostEffect* apPostEffect)
	{
		STLFindAndDelete(mlstPostEffects,apPostEffect);
	}

	//-----------------------------------------------------------------------

	iGpuProgram* cGraphics::CreateGpuProgram(const tString& asName)
	{
		iGpuProgram *pProgram = mpLowLevelGraphics->CreateGpuProgram(asName);
		pProgram->SetResources(mpResources);
		mlstGpuPrograms.push_back(pProgram);

		return pProgram;
	}

	iGpuProgram* cGraphics::CreateGpuProgramFromShaders(const tString& asName, const tString& asVtxShader,const tString& asFragShader,
														cParserVarContainer *apVarContainer)
	{
		iGpuShader *pVtxShader = mpResources->GetGpuShaderManager()->CreateShader(asVtxShader,eGpuShaderType_Vertex,apVarContainer);
		if(pVtxShader==NULL) return NULL;
		iGpuShader *pFragShader = mpResources->GetGpuShaderManager()->CreateShader(asFragShader,eGpuShaderType_Fragment,apVarContainer);
		if(pFragShader==NULL){
			mpResources->GetGpuShaderManager()->Destroy(pVtxShader);
			return NULL;
		}

		iGpuProgram *pProgram = CreateGpuProgram(asName);
		pProgram->SetShader(eGpuShaderType_Vertex, pVtxShader);
		pProgram->SetShader(eGpuShaderType_Fragment, pFragShader);
		pProgram->Link();

        return pProgram;		
	}
	
	void cGraphics::DestroyGpuProgram(iGpuProgram* apProgram)
	{
		STLFindAndDelete(mlstGpuPrograms, apProgram);
	}

	//-----------------------------------------------------------------------
	
	void cGraphics::AddMaterialType(iMaterialType *apType, const tString& asName)
	{
		apType->SetName(asName);
		apType->LoadData();
		m_mapMaterialTypes.insert(tMaterialTypeMap::value_type(asName, apType));
	}
	
	iMaterialType *cGraphics::GetMaterialType(const tString& asName)
	{
		tString sLowName = cString::ToLowerCase(asName);

		tMaterialTypeMapIt it = m_mapMaterialTypes.find(sLowName);
		if(it == m_mapMaterialTypes.end()) return NULL;

		return it->second;
	}

	tStringVec cGraphics::GetMaterialTypeNames()
	{
		tStringVec vNames;
		tMaterialTypeMapIt it = m_mapMaterialTypes.begin();
		for(;it!=m_mapMaterialTypes.end();++it)
		{
			vNames.push_back(it->first);
		}

		return vNames;
	}

	void cGraphics::ReloadMaterials()
	{
		tMaterialTypeMapIt it = m_mapMaterialTypes.begin();
		for(;it != m_mapMaterialTypes.end(); ++it)
		{
			iMaterialType *pType = it->second;
			pType->Reload();
		}
	}

	//-----------------------------------------------------------------------

}
