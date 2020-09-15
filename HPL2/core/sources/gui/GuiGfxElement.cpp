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

#include "gui/GuiGfxElement.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/ImageManager.h"
#include "graphics/FrameSubImage.h"
#include "graphics/FrameBitmap.h"

#include "graphics/Texture.h"

#include "gui/Gui.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// ANIMATION
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGuiGfxAnimation::AddFrame(int alNum)
	{
		mvFrames.push_back(alNum);
	}
	void cGuiGfxAnimation::SetType(eGuiGfxAnimationType aType)
	{
		mType = aType;
	}
	void cGuiGfxAnimation::SetFrameLength(float afLength)
	{
		mfFrameLength = afLength;
	}
		
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGuiGfxElement::cGuiGfxElement(cGui* apGui)
	{
		mpGui = apGui;

		mpResources = mpGui->GetResources();

		/////////////////////////////
		// Set up vertices
		mvVtx.resize(4);

		for(int i=0; i<4; ++i)
		{
			mvVtx[i].pos =0;
			mvVtx[i].tex =0;
			mvVtx[i].col = cColor(1,1);
		}

		//Position
		mvVtx[1].pos.x = 1;
		mvVtx[2].pos.x = 1;
		mvVtx[2].pos.y = 1;
		mvVtx[3].pos.y = 1;

		//Texture coordinate
		mvVtx[1].tex.x = 1;
		mvVtx[2].tex.x = 1;
		mvVtx[2].tex.y = 1;
		mvVtx[3].tex.y = 1;

		mvImageSize = 0;

		mvOffset =0;
		mvActiveSize =0;

		mbDestroyTexture = true;

		////////////////////////////
		//Set up textures
		for(int i=0; i<kMaxGuiTextures; ++i)
		{
			mvTextures[i] = NULL;
			mvImages[i] = NULL;
		}

		mlTextureNum =0;
		mlCurrentAnimation = 0;
		mfCurrentFrame =0;
		mbForwardAnim = true;
		mlActiveImage =0;
		mbAnimationPaused = false;

		mpMaterial = NULL;

		mbFlipUvYAxis = false;

		mbFlushed = false;

		mlImageUpdateCount = -1;
	}

	//---------------------------------------------------

	cGuiGfxElement::~cGuiGfxElement()
	{
		STLDeleteAll(mvAnimations);

		////////////////////////////////
		// Delete all textures / Images
		if(mvImageBufferVec.size()>0)
		{
			for(int i=0; i< (int)mvImageBufferVec.size(); ++i)
			{
				mpResources->GetImageManager()->Destroy(mvImageBufferVec[i]);
			}
		}
		else
		{
			for(int i=0; i<mlTextureNum; ++i)
			{
				if(mvImages[i])
				{
					mpResources->GetImageManager()->Destroy(mvImages[i]);
				}
				else if(mvTextures[i] && mbDestroyTexture)
				{
					mpResources->GetTextureManager()->Destroy(mvTextures[i]);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGuiGfxElement::Update(float afTimeStep)
	{

		//////////////////////////////////////
		//Update image animations
		if(mvAnimations.empty() || mbAnimationPaused) return;
        if(mlCurrentAnimation >= (int)mvAnimations.size()) return;

		cGuiGfxAnimation* pAnim = mvAnimations[mlCurrentAnimation];

		int lFrame = 0;
		//////////////////////////////////
		// Non random animation update
		if(pAnim->mType != eGuiGfxAnimationType_Random)
		{
			mfCurrentFrame += afTimeStep * (1.0f/pAnim->mfFrameLength);
			lFrame = (int) mfCurrentFrame;
			if(lFrame >= (int)mvImageBufferVec.size())
			{
				//Log("Over max!\n");
				//Stop at end
				if(pAnim->mType == eGuiGfxAnimationType_StopAtEnd) 
				{
					lFrame = (int)mvImageBufferVec.size()-1;
					mfCurrentFrame = (float)lFrame;
				}
				//Loop
				else if(pAnim->mType == eGuiGfxAnimationType_Loop)
				{
					lFrame =0;
					mfCurrentFrame = 0;
				}
				//Oscillate
				else if(pAnim->mType == eGuiGfxAnimationType_Oscillate)
				{
					lFrame =1;
					mfCurrentFrame = 1;
					mbForwardAnim = !mbForwardAnim;
				}
			}
			//Log("Frame %d %f actual_frame: %d size: %d\n",lFrame,mfCurrentFrame,((int)mvImageBufferVec.size()-1) - lFrame,mvImageBufferVec.size());
			
			//Oscillate fix
			if(mbForwardAnim== false && pAnim->mType == eGuiGfxAnimationType_Oscillate)
			{
				lFrame = ((int)mvImageBufferVec.size()-1) - lFrame;
			}
		}
		//////////////////////////////////
		// Random animation update
		else if(mvImageBufferVec.size() > 1)
		{
			float fPrev = mfCurrentFrame;
			mfCurrentFrame += afTimeStep * (1.0f/pAnim->mfFrameLength);
			lFrame = (int) mfCurrentFrame;
			if((int)mfCurrentFrame != (int)fPrev)
			{
				int lPrev = (int)fPrev;
				do
				{
					lFrame = cMath::RandRectl(0, (int)mvImageBufferVec.size()-1);
				}
				while(lFrame == lPrev);

				mfCurrentFrame = (float)lFrame;
			}
		}

		//////////////////////////////////
		// Set new image
		if(lFrame != mlActiveImage)
		{
			mlActiveImage = lFrame;
			SetImage(mvImageBufferVec[mlActiveImage],0);
		}
	}
	
	//-----------------------------------------------------------------------
	
	void cGuiGfxElement::AddImage(cFrameSubImage* apImage)
	{
		SetImage(apImage, mlTextureNum);
		
		mvActiveSize = GetImageSize();

		++mlTextureNum;
	}

	//---------------------------------------------------

	void cGuiGfxElement::AddTexture(iTexture* apTexture, const cVector2f& avStartUV, const cVector2f& avEndUV)
	{
		mvTextures[mlTextureNum] = apTexture;

		if(mlTextureNum==0)
		{
			cVector2f vSize = cVector2f((float)apTexture->GetWidth(), (float)apTexture->GetHeight());

			mvImageSize.x = vSize.x * (avEndUV.x - avStartUV.x);
			mvImageSize.y = vSize.y * (avEndUV.y - avStartUV.y);

			if(apTexture->GetType() == eTextureType_Rect)
			{
				//If a rect, give new texture coordinates
				
				mvVtx[0].tex.x = avStartUV.x * vSize.x;
				mvVtx[0].tex.y = avStartUV.y * vSize.y;

				mvVtx[1].tex.x = avEndUV.x * vSize.x;
				mvVtx[1].tex.y = avStartUV.y * vSize.y;

				mvVtx[2].tex.x = avEndUV.x * vSize.x;
				mvVtx[2].tex.y = avEndUV.y * vSize.y;
				
				mvVtx[3].tex.x = avStartUV.x * vSize.x;
				mvVtx[3].tex.y = avEndUV.y * vSize.y;
			}
			else
			{
				mvVtx[0].tex.x = avStartUV.x;
				mvVtx[0].tex.y = avStartUV.y;

				mvVtx[1].tex.x = avEndUV.x;
				mvVtx[1].tex.y = avStartUV.y;

				mvVtx[2].tex.x = avEndUV.x;
				mvVtx[2].tex.y = avEndUV.y;
				
				mvVtx[3].tex.x = avStartUV.x;
				mvVtx[3].tex.y = avEndUV.y;

			}

			if(mbFlipUvYAxis) SetFlipUvYAxis(mbFlipUvYAxis);
		}

		mvActiveSize = GetImageSize();

		++mlTextureNum;
	}

	//---------------------------------------------------

	void cGuiGfxElement::AddImageToBuffer(cFrameSubImage* apImage)
	{
		if(mvImageBufferVec.size()==0)
		{
			SetImage(apImage,0);
		}

		mvImageBufferVec.push_back(apImage);
	}

	//---------------------------------------------------

	cGuiGfxAnimation* cGuiGfxElement::CreateAnimtion(const tString& asName)
	{
		cGuiGfxAnimation *pAnimation = hplNew( cGuiGfxAnimation, () );
		pAnimation->msName = asName;

		mvAnimations.push_back(pAnimation);
        		
		return pAnimation;
	}

	//---------------------------------------------------

	void cGuiGfxElement::PlayAnimation(int alNum)
	{
		if(mlCurrentAnimation == alNum) return;

		mlCurrentAnimation = alNum;

		mfCurrentFrame =0;
		mbForwardAnim = true;
		mlActiveImage =0;
		SetImage(mvImageBufferVec[mlActiveImage],0);
	}

	void cGuiGfxElement::SetAnimationTime(float afTime)
	{
        if(mlCurrentAnimation>=0)
			mfCurrentFrame = afTime / mvAnimations[mlCurrentAnimation]->mfFrameLength;
		else
			mfCurrentFrame = afTime;
	}

	//---------------------------------------------------

	void cGuiGfxElement::SetMaterial(iGuiMaterial *apMat)
	{
		mpMaterial = apMat;
	}

	//---------------------------------------------------

	void cGuiGfxElement::SetColor(const cColor &aColor)
	{
		for(int i=0; i<4; ++i)	mvVtx[i].col = aColor;
	}

	//-----------------------------------------------------------------------

	void cGuiGfxElement::SetFlipUvYAxis(bool abX)
	{
		mbFlipUvYAxis = abX;

		////////////////////////////////////
		//Image
        if(mvImages[0])
		{
			const tVertexVec& vImageVtx = mvImages[0]->GetVertexVec();
			for(int i=0; i<4; ++i) mvVtx[i].tex = vImageVtx[i].tex;
			if(mbFlipUvYAxis)
			{
				mvVtx[0].tex.y = mvVtx[2].tex.y;
				mvVtx[1].tex.y = mvVtx[3].tex.y;
				mvVtx[2].tex.y = mvVtx[0].tex.y;
				mvVtx[3].tex.y = mvVtx[1].tex.y;
			}
		}
		////////////////////////////////////
		//Texture
		else if(mvTextures[0])
		{
			if(mbFlipUvYAxis)
			{
				float fOldY2 = mvVtx[2].tex.y;
				float fOldY3 = mvVtx[3].tex.y;

				mvVtx[2].tex.y = mvVtx[1].tex.y;
				mvVtx[3].tex.y = mvVtx[0].tex.y;
				
				mvVtx[0].tex.y = fOldY3;
				mvVtx[1].tex.y = fOldY2;
			}
			else
			{
				float fOldY0 = mvVtx[0].tex.y;
				float fOldY1 = mvVtx[1].tex.y;
				
				mvVtx[0].tex.y = mvVtx[3].tex.y;
				mvVtx[1].tex.y = mvVtx[2].tex.y;
				
				mvVtx[2].tex.y = fOldY1;
				mvVtx[3].tex.y = fOldY0;
			}
		
		}

	}

	//-----------------------------------------------------------------------

	cVector2f cGuiGfxElement::GetImageSize()
	{
		return mvImageSize;
	}

	//-----------------------------------------------------------------------

	void cGuiGfxElement::Flush()
	{
		//Check if element uses images.
		cFrameSubImage *pMainImage = mvImages[0];
		if(pMainImage == NULL) return;

		//Flush all images
		for(int i=0; i<mlTextureNum; ++i)
		{
			cFrameSubImage *pImage = mvImages[i];
			if(pImage) pImage->Flush();

		}
		
		//Check if the image has been changed, if so update UV.
		if(mlImageUpdateCount != pMainImage->GetUpdateCount())
		{
			const tVertexVec& vImageVtx = pMainImage->GetVertexVec();
			for(int i=0; i<4; ++i) mvVtx[i].tex = vImageVtx[i].tex;	
			if(mbFlipUvYAxis) SetFlipUvYAxis(mbFlipUvYAxis);

			mlImageUpdateCount = pMainImage->GetUpdateCount();
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGuiGfxElement::SetImage(cFrameSubImage* apImage, int alNum)
	{
		//Set image and texture (for sorting)
		mvImages[alNum] = apImage;
		mvTextures[alNum] = apImage->GetTexture();

		//Get texture coords
		//Log("Image tex: '%s' Texture: %d\n", apImage->GetName().c_str(), mvTextures[alNum]);
		const tVertexVec& vImageVtx = apImage->GetVertexVec();
		for(int i=0; i<4; ++i) {
			mvVtx[i].tex = vImageVtx[i].tex;
			//Log("  (%s)", mvVtx[i].tex.ToString().c_str());
		}
		//Log("\n");

		if(alNum==0)
		{
			mvImageSize.x = (float)apImage->GetWidth();
			mvImageSize.y = (float)apImage->GetHeight();

			if(mbFlipUvYAxis) SetFlipUvYAxis(mbFlipUvYAxis);
		}
	}

	//-----------------------------------------------------------------------

}
