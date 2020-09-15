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

#ifndef HPL_POSTEFFECT_H
#define HPL_POSTEFFECT_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {

	class cGraphics;
	class cResources;

	class iLowLevelGraphics;
	class iTexture;
	class iFrameBuffer;
	class iGpuProgram;

	class cPostEffectComposite;
	class iPostEffect;
	

	//------------------------------------------

	#define kPostEffectParamsClassInit(aClass)							\
		void CopyTo(iPostEffectParams* apDestParams) {					\
			aClass *pCastParams = static_cast< aClass *>(apDestParams);	\
			*pCastParams = *this;										\
		}																\
		void LoadFrom(iPostEffectParams* apSrcParams) {				\
			aClass *pCastParams = static_cast< aClass *>(apSrcParams);	\
			*this = *pCastParams;										\
		}		
		

	//------------------------------------------
	
	class iPostEffectParams
	{
	public:
		iPostEffectParams(const tString& asName) : msName(asName){}
		virtual ~iPostEffectParams() { }
		const tString& GetName(){ return msName;}

		virtual void CopyTo(iPostEffectParams* apDestParams)=0;
		virtual void LoadFrom(iPostEffectParams* apSrcParams)=0;
		
	private:
		tString msName;
	};

	//------------------------------------------
	
	class iPostEffectType
	{
	public:
		iPostEffectType(const tString& asName, cGraphics *apGraphics, cResources *apResources);
		virtual ~iPostEffectType();

		const tString& GetName(){ return msName;}

		virtual iPostEffect *CreatePostEffect(iPostEffectParams *apParams)=0;
	
	protected:
		cGraphics *mpGraphics;
		cResources *mpResources;

		tString msName;
	};
	
	//------------------------------------------

	class iPostEffect
	{
	public:
		iPostEffect(cGraphics *apGraphics, cResources *apResources, iPostEffectType *apType);
		virtual ~iPostEffect();

		iTexture* Render(cPostEffectComposite *apComposite, iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer, bool abLastEffect);

		/** SetDisabled - Method to disable the Effect completely, meaning IsActive will always return false even
		 * after a SetActive(true) call
		 *
		 * \param abX 
		 */
		void SetDisabled(bool abX) { mbDisabled = abX; }
		bool IsDisabled() { return mbDisabled; }

		void SetActive(bool abX);
		bool IsActive(){ return mbDisabled==false && mbActive;}

		void SetParams(iPostEffectParams *apSrcParams);
		void GetParams(iPostEffectParams *apDestParams);

		virtual void Reset(){}

	protected:
		virtual void OnSetActive(bool abX){}
		virtual void OnSetParams()=0;
		virtual iPostEffectParams *GetTypeSpecificParams()=0;
		virtual iTexture* RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer)=0;
		
		/**
		 * Very important! Only set this if the contents of the final buffer does not matter!
		 * This function will set the frame buffer if the post effect is last!
		 */
		void SetFinalFrameBuffer(iFrameBuffer *apOutputBuffer);

		void GetTextureUvPosAndSize(const cVector2f& avTexSize,cVector2f& avUvPos,  cVector2f& avUvSize);

		void SetFrameBuffer(iFrameBuffer *apFrameBuffer);
		void DrawQuad(	const cVector3f& avPos,  const cVector2f& avSize, iTexture *apTexture, bool abFlipY);
		void DrawQuad(	const cVector3f& avPos,  const cVector2f& avSize, iTexture *apTexture0, iTexture *apTexture1, 
						bool abFlipY0,bool abFlipY1);
		
		cGraphics *mpGraphics;
		cResources *mpResources;
		iLowLevelGraphics* mpLowLevelGraphics;

		iPostEffectType *mpType;

		bool mbDisabled;
		bool mbActive;
		
		cPostEffectComposite *mpCurrentComposite;
		bool mbIsLastEffect;

		bool mbFinalFrameBufferUsed;
	};

	//------------------------------------------

};
#endif // HPL_POSTEFFECT_H
