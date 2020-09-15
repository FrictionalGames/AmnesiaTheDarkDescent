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

#ifndef HPL_VERTEXBUFFER_H
#define HPL_VERTEXBUFFER_H

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "math/BoundingVolume.h"

namespace hpl {
	
	class cBoundingVolume;
	class iLowLevelGraphics;

	//-----------------------------------------------------

	class iVertexBuffer
	{
	public:
		iVertexBuffer(iLowLevelGraphics* apLowLevelGraphics,
			eVertexBufferType aType, 
			eVertexBufferDrawType aDrawType,eVertexBufferUsageType aUsageType,
			int alReserveVtxSize,int alReserveIdxSize) :
			mType(aType), mVertexFlags(0),
			mpLowLevelGraphics(apLowLevelGraphics),
			mDrawType(aDrawType), mUsageType(aUsageType), 
			mlReservedVtxSize(alReserveVtxSize), mlReservedIdxSize(alReserveIdxSize),
			mlElementNum(-1) {}
		
		virtual ~iVertexBuffer(){}

		inline eVertexBufferType GetType() const { return mType; }

		virtual void CreateElementArray(	eVertexBufferElement aType, eVertexBufferElementFormat aFormat,
											int alElementNum, int alProgramVarIndex=0)=0;
		
		virtual void AddVertexVec3f(eVertexBufferElement aElement,const cVector3f& avVtx)=0;
		virtual void AddVertexVec4f(eVertexBufferElement aElement,const cVector3f& avVtx, float afW)=0;
		virtual void AddVertexColor(eVertexBufferElement aElement,const cColor& aColor)=0;
		virtual void AddIndex(unsigned int alIndex)=0;
		
		virtual bool Compile(tVertexCompileFlag aFlags)=0;
		virtual void UpdateData(tVertexElementFlag aTypes, bool abIndices)=0;
		
		/**
		* This creates a double of the vertex array with w=0.
		* \param abUpdateData if the hardware buffer should be updated as well.
		*/
		virtual void CreateShadowDouble(bool abUpdateData)=0;

		/**
		 * Transform the entire buffer with transform.
		*/
		virtual void Transform(const cMatrixf &mtxTransform)=0;

		virtual void Draw(eVertexBufferDrawType aDrawType = eVertexBufferDrawType_LastEnum)=0;
		virtual void DrawIndices(	unsigned int *apIndices, int alCount,
									eVertexBufferDrawType aDrawType = eVertexBufferDrawType_LastEnum)=0;

		virtual void Bind()=0;
		virtual void UnBind()=0;

        virtual iVertexBuffer* CreateCopy(	eVertexBufferType aType,eVertexBufferUsageType aUsageType,
											tVertexElementFlag alVtxToCopy)=0;
			
        virtual cBoundingVolume CreateBoundingVolume()=0;

        virtual int GetElementNum(eVertexBufferElement aElement)=0;		
		virtual eVertexBufferElementFormat GetElementFormat(eVertexBufferElement aElement)=0;
		virtual int GetElementProgramVarIndex(eVertexBufferElement aElement)=0;
		virtual float* GetFloatArray(eVertexBufferElement aElement)=0;
		virtual int* GetIntArray(eVertexBufferElement aElement)=0;
		virtual unsigned char* GetByteArray(eVertexBufferElement aElement)=0;
		
		virtual unsigned int* GetIndices()=0;
		
		virtual int GetVertexNum()=0;
		virtual int GetIndexNum()=0;

		/**
		 * Resizes an array to a custom size, the size is number of elements and NOT number of vertices.
		 */
		virtual void ResizeArray(eVertexBufferElement aElement, int alSize)=0;
		virtual void ResizeIndices(int alSize)=0;
        		
		/**
		 * Set the number of of elements to draw.
		 * \param alNum If < 0, draw all indices.
		 */
		void SetElementNum(int alNum){ mlElementNum = alNum;}
		int GetElementNum(){ return mlElementNum;}

		tVertexElementFlag GetVertexElementFlags(){ return mVertexFlags;}
	
	protected:
		iLowLevelGraphics* mpLowLevelGraphics;

		eVertexBufferType mType;
		tVertexElementFlag mVertexFlags;
		eVertexBufferDrawType mDrawType;
		eVertexBufferUsageType mUsageType;
		
		int mlReservedVtxSize;
		int mlReservedIdxSize;

		int mlElementNum;

		bool mbTangents;
	};

};
#endif // HPL_RENDERER3D_H
