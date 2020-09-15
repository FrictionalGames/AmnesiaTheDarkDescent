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

#ifndef HPL_TEXTURE_H
#define HPL_TEXTURE_H

#include "graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"

namespace hpl {

	class iLowLevelGraphics;
	class cBitmap;

	class iTexture : public iResourceBase, public iFrameBufferAttachment
	{
	public:
		iTexture(const tString& asName, const tWString& asFullPath, eTextureType aType, eTextureUsage aUsage, iLowLevelGraphics* apLowLevelGraphics)
			: iResourceBase(asName,asFullPath,0),
				mUsage(aUsage),
				mType(aType),
				mpLowLevelGraphics(apLowLevelGraphics),
				mbUseMipMaps(false), 
				mbIsCompressed(false),
				mlMemorySize(0),
				mPixelFormat(ePixelFormat_Unknown),
				mWrapS(eTextureWrap_Repeat), mWrapT(eTextureWrap_Repeat),mWrapR(eTextureWrap_Repeat),
				mfFrameTime(1), mAnimMode(eTextureAnimMode_Loop), mlSizeDownScaleLevel(0), mvMinDownScaleSize(16,16,16),
				mfAnisotropyDegree(1.0f),mFilter(eTextureFilter_Bilinear),
				mCompareMode(eTextureCompareMode_None),
				mCompareFunc(eTextureCompareFunc_LessOrEqual)
		{}

		virtual ~iTexture(){}

		bool Reload(){ return false;}
		void Unload(){}
		void Destroy(){}

		virtual bool CreateFromBitmap(cBitmap* pBmp)=0;
		virtual bool CreateAnimFromBitmapVec(std::vector<cBitmap*> *avBitmaps)=0;
		virtual bool CreateCubeFromBitmapVec(std::vector<cBitmap*> *avBitmaps)=0;
		virtual bool CreateFromRawData(const cVector3l &avSize,ePixelFormat aPixelFormat, unsigned char *apData)=0;
		
		virtual void SetRawData(	int alLevel, const cVector3l& avOffset, const cVector3l& avSize, 
									ePixelFormat aPixelFormat, void *apData)=0;

		virtual void Update(float afTimeStep)=0;

		inline ePixelFormat GetPixelFormat() const { return mPixelFormat;}
		
		inline const cVector3l& GetSize() const { return mvSize;}
		inline int GetWidth() const { return mvSize.x;}
		inline int GetHeight() const { return mvSize.y;}
		inline int GetDepth() const { return mvSize.z;}

		inline cVector2f GetSizeFloat2D() const { return cVector2f((float)mvSize.x, (float)mvSize.y);}
		inline cVector2l GetSizeInt2D() const { return cVector2l(mvSize.x, mvSize.y);}

		virtual void SetFilter(eTextureFilter aFilter)=0;
		virtual void SetAnisotropyDegree(float afX)=0;
		eTextureFilter GetFilter(){ return mFilter;}
		float GetAnisotropyDegree(float afX){ return mfAnisotropyDegree;}
		
		virtual void SetWrapS(eTextureWrap aMode)=0;
		virtual void SetWrapT(eTextureWrap aMode)=0;
		virtual void SetWrapR(eTextureWrap aMode)=0;
		virtual void SetWrapSTR(eTextureWrap aMode)=0;

		eTextureWrap GetWrapS(){ return mWrapS;}
		eTextureWrap GetWrapT(){ return mWrapT;}
		eTextureWrap GetWrapR(){ return mWrapR;}

		virtual void SetCompareMode(eTextureCompareMode aMode)=0;
		virtual void SetCompareFunc(eTextureCompareFunc aFunc)=0;

		eTextureCompareMode GetCompareMode(){ return mCompareMode;}
		eTextureCompareFunc GetCompareFunc(){ return mCompareFunc;}


		virtual void AutoGenerateMipmaps()=0;

		int GetMemorySize(){ return mlMemorySize;}

		void SetFrameTime(float afX){ mfFrameTime = afX;}
		float GetFrameTime(){ return mfFrameTime;}

		eTextureAnimMode GetAnimMode() { return mAnimMode;}
		void SetAnimMode(eTextureAnimMode aMode) {mAnimMode = aMode;};

		eTextureUsage GetUsage(){ return mUsage; }
		eTextureType GetType(){ return mType;}
		
		void SetUseMipMaps(bool abX){mbUseMipMaps = abX;} 
		bool UsesMipMaps(){ return mbUseMipMaps; }

		bool IsCompressed(){ return mbIsCompressed; }
		
		void SetSizeDownScaleLevel(unsigned int alLevel){mlSizeDownScaleLevel = alLevel;}
		void SetMinLevelSize(const cVector2l& avSize){ mvMinDownScaleSize = avSize;}

		eFrameBufferAttachment GetFrameBufferAttachmentType(){ return eFrameBufferAttachment_Texture;}
		
		virtual bool HasAnimation()=0;
		virtual void NextFrame()=0;
		virtual void PrevFrame()=0;
		virtual float GetT()=0;
		virtual float GetTimeCount()=0;
		virtual void SetTimeCount(float afX)=0;
		virtual int GetCurrentLowlevelHandle()=0;

	protected:
		eTextureUsage mUsage;
		eTextureType mType;

		cVector3l mvSize;

		int mlMemorySize;
		
		eTextureWrap mWrapS;
		eTextureWrap mWrapT;
		eTextureWrap mWrapR;
		eTextureFilter mFilter;
		float mfAnisotropyDegree;

		eTextureCompareMode mCompareMode;
		eTextureCompareFunc mCompareFunc;
		
		bool mbUseMipMaps;
		bool mbIsCompressed;
		ePixelFormat mPixelFormat;
		iLowLevelGraphics* mpLowLevelGraphics;
		float mfFrameTime;
		eTextureAnimMode mAnimMode;
		
		unsigned int mlSizeDownScaleLevel;
		cVector3l mvMinDownScaleSize;

	};
};
#endif // HPL_TEXTURE_H
