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

#ifndef HPL_VERTEXBUFFER_OPENGL_H
#define HPL_VERTEXBUFFER_OPENGL_H

#include "graphics/VertexBuffer.h"


namespace hpl {

	//---------------------------------------------------------
	
	extern unsigned int GetGLArrayFromVertexElement(eVertexBufferElement aType);
	extern unsigned int GetGLTypeFromVertexFormat(eVertexBufferElementFormat aFormat);

	extern unsigned int GetDrawModeFromDrawType(eVertexBufferDrawType aDrawType);

	//---------------------------------------------------------
	
	class cVtxBufferGLElementArray
	{
	public:
		cVtxBufferGLElementArray(eVertexBufferElementFormat aFormat);
		~cVtxBufferGLElementArray();

		void Reserve(size_t alSize);
		void Resize(size_t alSize);
		void PushBack(const void *apData);
		size_t Size();

		void* GetArrayPtr();

		eVertexBufferElement mType;
		tVertexElementFlag mFlag;

		eVertexBufferElementFormat mFormat;

		int mlElementNum;

		int mlGLHandle;

		int mlProgramVarIndex;
		
		tByteVec* mpByteArray;
		tIntVec* mpIntArray;
		tFloatVec* mpFloatArray;
	};


	//---------------------------------------------------------
	
	class iVertexBufferOpenGL : public iVertexBuffer
	{
	public:
		iVertexBufferOpenGL(	iLowLevelGraphics* apLowLevelGraphics,
							eVertexBufferType aType,
							eVertexBufferDrawType aDrawType,eVertexBufferUsageType aUsageType,
							int alReserveVtxSize,int alReserveIdxSize);
		~iVertexBufferOpenGL();

		void CreateElementArray(	eVertexBufferElement aType, eVertexBufferElementFormat aFormat,
									int alElementNum, int alProgramVarIndex=0);

		void AddVertexVec3f(eVertexBufferElement aElement,const cVector3f& avVtx);
		void AddVertexVec4f(eVertexBufferElement aElement,const cVector3f& avVtx, float afW);
		void AddVertexColor(eVertexBufferElement aElement,const cColor& aColor);
		void AddIndex(unsigned int alIndex);

		bool Compile(tVertexCompileFlag aFlags);
		
		void CreateShadowDouble(bool abUpdateData);

		void Transform(const cMatrixf &mtxTransform);

		iVertexBuffer* CreateCopy(	eVertexBufferType aType, eVertexBufferUsageType aUsageType,
									tVertexElementFlag alVtxToCopy);

		cBoundingVolume CreateBoundingVolume();
		
		int GetVertexNum();
		int GetIndexNum();

		int GetElementNum(eVertexBufferElement aElement);
		eVertexBufferElementFormat GetElementFormat(eVertexBufferElement aElement);
		int GetElementProgramVarIndex(eVertexBufferElement aElement);

		float* GetFloatArray(eVertexBufferElement aElement);
		int* GetIntArray(eVertexBufferElement aElement);
		unsigned char* GetByteArray(eVertexBufferElement aElement);

		unsigned int* GetIndices();

		void ResizeArray(eVertexBufferElement aElement, int alSize);
		void ResizeIndices(int alSize);
		
	protected:
		virtual void CompileSpecific()=0;
		virtual iVertexBufferOpenGL* CreateDataCopy(tVertexElementFlag aFlags, eVertexBufferDrawType aDrawType,
													eVertexBufferUsageType aUsageType,
													int alReserveVtxSize,int alReserveIdxSize)=0;

		inline cVtxBufferGLElementArray* GetElementArray(eVertexBufferElement aElement)
		{
			int lIdx = mvElementArrayIndex[aElement];
			if(lIdx <0) return NULL;
			return mvElementArrays[ lIdx ]; 
		}
		
		char mvElementArrayIndex[eVertexBufferElement_LastEnum];
		std::vector<cVtxBufferGLElementArray*> mvElementArrays;
		
		tUIntVec mvIndexArray;

		bool mbHasShadowDouble;
	};

};
#endif // HPL_VERTEXBUFFER_OPENGL_H
