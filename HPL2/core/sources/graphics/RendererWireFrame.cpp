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

#include "graphics/RendererWireFrame.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/PreprocessParser.h"

#include "graphics/Graphics.h"
#include "graphics/Texture.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/Renderable.h"
#include "graphics/RenderList.h"
#include "graphics/Material.h"
#include "graphics/MaterialType.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"

#include "resources/Resources.h"
#include "graphics/GPUShader.h"
#include "graphics/GPUProgram.h"

#include "scene/Camera.h"
#include "scene/World.h"
#include "scene/RenderableContainer.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cRendererWireFrame::cRendererWireFrame(cGraphics *apGraphics,cResources* apResources) 
		: iRenderer("WireFrame",apGraphics, apResources,0)
	{
		////////////////////////////////////
		// Set up render specific things
		mbSetFrameBufferAtBeginRendering = true;
		mbClearFrameBufferAtBeginRendering = true;
	}

	//-----------------------------------------------------------------------

	cRendererWireFrame::~cRendererWireFrame()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cRendererWireFrame::LoadData()
	{
		return true;
	}

	//-----------------------------------------------------------------------

	
	void cRendererWireFrame::DestroyData()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cRendererWireFrame::CopyToFrameBuffer()
	{
		//Do Nothing
	}

	
	//-----------------------------------------------------------------------
	
	void cRendererWireFrame::SetupRenderList()
	{
		mpCurrentRenderList->Setup(mfCurrentFrameTime,mpCurrentFrustum);

		CheckForVisibleAndAddToList(mpCurrentWorld->GetRenderableContainer(eWorldContainerType_Static),0);
		CheckForVisibleAndAddToList(mpCurrentWorld->GetRenderableContainer(eWorldContainerType_Dynamic),0);

		mpCurrentRenderList->Compile(	eRenderListCompileFlag_Diffuse |
										eRenderListCompileFlag_Decal |
										eRenderListCompileFlag_Translucent);

	}

	//-----------------------------------------------------------------------
	
	void cRendererWireFrame::RenderObjects()
	{
		START_RENDER_PASS(WireFrame);

		////////////////////////////////////////////
		// Diffuse Objects
		SetDepthTest(true);
		SetDepthWrite(true);
		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_RGBA);

		SetTextureRange(NULL,0);

		int lCount =0;
		cRenderableVecIterator diffIt = mpCurrentRenderList->GetArrayIterator(eRenderListType_Diffuse);
		while(diffIt.HasNext())
		{
			iRenderable *pObject = diffIt.Next();
			cMaterial *pMaterial = pObject->GetMaterial();

			SetTexture(0,pMaterial->GetTexture(eMaterialTexture_Diffuse));
		
			SetMatrix(pObject->GetModelMatrixPtr());

			SetVertexBuffer(pObject->GetVertexBuffer());

			DrawCurrent(eVertexBufferDrawType_LineStrip);
			lCount++;
		}

		////////////////////////////////////////////
		// Decal Objects
		SetDepthWrite(false);

		cRenderableVecIterator decalIt = mpCurrentRenderList->GetArrayIterator(eRenderListType_Decal);
		while(decalIt.HasNext())
		{
			iRenderable *pObject = decalIt.Next();
			cMaterial *pMaterial = pObject->GetMaterial();

			SetBlendMode(pMaterial->GetBlendMode());

			SetTexture(0,pMaterial->GetTexture(eMaterialTexture_Diffuse));

			SetMatrix(pObject->GetModelMatrixPtr());

			SetVertexBuffer(pObject->GetVertexBuffer());

			DrawCurrent(eVertexBufferDrawType_LineStrip);
		}

		RunCallback(eRendererMessage_PostSolid);


		////////////////////////////////////////////
		// Trans Objects
		SetDepthWrite(false);
		
		cRenderableVecIterator transIt = mpCurrentRenderList->GetArrayIterator(eRenderListType_Translucent);
		while(transIt.HasNext())
		{
			iRenderable *pObject = transIt.Next();
			cMaterial *pMaterial = pObject->GetMaterial();

			pObject->UpdateGraphicsForViewport(mpCurrentFrustum, mfCurrentFrameTime);

			SetBlendMode(pMaterial->GetBlendMode());

			SetTexture(0,pMaterial->GetTexture(eMaterialTexture_Diffuse));

			SetMatrix(pObject->GetModelMatrix(mpCurrentFrustum));

			SetVertexBuffer(pObject->GetVertexBuffer());

			DrawCurrent(eVertexBufferDrawType_LineStrip);
		}
		
		RunCallback(eRendererMessage_PostTranslucent);
		
		
		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------
	
}
