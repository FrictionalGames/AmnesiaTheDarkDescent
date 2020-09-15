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

#include "impl/VertexBufferOpenGL.h"

#include "system/LowLevelSystem.h"

#include "graphics/LowLevelGraphics.h"

#include "math/Math.h"

#include "impl/VertexBufferOGL_Array.h"
#include "impl/VertexBufferOGL_VBO.h"

#include <memory.h>

#include <GL/glew.h>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// GLOBAL FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	unsigned int GetGLArrayFromVertexElement(eVertexBufferElement aType)
	{
		switch(aType)
		{
		case eVertexBufferElement_Normal:			return GL_NORMAL_ARRAY;
		case eVertexBufferElement_Position:			return GL_VERTEX_ARRAY;
		case eVertexBufferElement_Color0:			return GL_COLOR_ARRAY;
		case eVertexBufferElement_Color1:			return GL_SECONDARY_COLOR_ARRAY;
		case eVertexBufferElement_Texture1Tangent:	return GL_TEXTURE_COORD_ARRAY;
		case eVertexBufferElement_Texture0:			return GL_TEXTURE_COORD_ARRAY;
		case eVertexBufferElement_Texture1:			return GL_TEXTURE_COORD_ARRAY;
		case eVertexBufferElement_Texture2:			return GL_TEXTURE_COORD_ARRAY;
		case eVertexBufferElement_Texture3:			return GL_TEXTURE_COORD_ARRAY;
		case eVertexBufferElement_Texture4:			return GL_TEXTURE_COORD_ARRAY;
		case eVertexBufferElement_User0:			return 0;
		case eVertexBufferElement_User1:			return 0;
		case eVertexBufferElement_User2:			return 0;
		case eVertexBufferElement_User3:			return 0;
		}

		return 0;
	}
	
	unsigned int  GetGLTypeFromVertexFormat(eVertexBufferElementFormat aFormat)
	{
		switch(aFormat)
		{
		case eVertexBufferElementFormat_Float:	return GL_FLOAT;
		case eVertexBufferElementFormat_Int:	return GL_INT;
		case eVertexBufferElementFormat_Byte:	return GL_UNSIGNED_BYTE;
		}

		return 0;
	}

	//-----------------------------------------------------------------------

	unsigned int GetDrawModeFromDrawType(eVertexBufferDrawType aDrawType)
	{
		switch(aDrawType)
		{
		case eVertexBufferDrawType_Tri:			return GL_TRIANGLES;
		case eVertexBufferDrawType_TriStrip:	return GL_TRIANGLE_STRIP;
		case eVertexBufferDrawType_TriFan:		return GL_TRIANGLE_FAN;

		case eVertexBufferDrawType_Quad:		return GL_QUADS;
		case eVertexBufferDrawType_QuadStrip:	return GL_QUAD_STRIP;
		
		case eVertexBufferDrawType_Line:		return GL_LINES;
		case eVertexBufferDrawType_LineStrip:	return GL_LINE_STRIP;
		case eVertexBufferDrawType_LineLoop:	return GL_LINE_LOOP;
		}

		return 0;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// ELEMENT
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVtxBufferGLElementArray::cVtxBufferGLElementArray(eVertexBufferElementFormat aFormat)
	{
		mlGLHandle =0;

		mpByteArray = NULL;
		mpIntArray = NULL;
		mpFloatArray = NULL;

		switch(aFormat)
		{
		case eVertexBufferElementFormat_Float:	mpFloatArray = hplNew(tFloatVec, ()); break;
		case eVertexBufferElementFormat_Int:	mpIntArray = hplNew(tIntVec, ());	break;
		case eVertexBufferElementFormat_Byte:	mpByteArray = hplNew(tByteVec, ());	break;
		}
	}
	
	cVtxBufferGLElementArray::~cVtxBufferGLElementArray()
	{
		if(mpByteArray)	hplDelete(mpByteArray);
		if(mpIntArray)	hplDelete(mpIntArray);
		if(mpFloatArray)hplDelete(mpFloatArray);
	}

	//-----------------------------------------------------------------------

	void cVtxBufferGLElementArray::Reserve(size_t alSize)
	{
		switch(mFormat)
		{
		case eVertexBufferElementFormat_Float:	mpFloatArray->reserve(alSize); break;
		case eVertexBufferElementFormat_Int:	mpIntArray->reserve(alSize); break;
		case eVertexBufferElementFormat_Byte:	mpByteArray->reserve(alSize); break;
		}
	}
	void cVtxBufferGLElementArray::Resize(size_t alSize)
	{
		switch(mFormat)
		{
		case eVertexBufferElementFormat_Float:	mpFloatArray->resize(alSize); break;
		case eVertexBufferElementFormat_Int:	mpIntArray->resize(alSize); break;
		case eVertexBufferElementFormat_Byte:	mpByteArray->resize(alSize); break;
		}
	}
	void cVtxBufferGLElementArray::PushBack(const void *apData)
	{
		switch(mFormat)
		{
		case eVertexBufferElementFormat_Float:	mpFloatArray->push_back( *((const float*)apData) ); break;
		case eVertexBufferElementFormat_Int:	mpIntArray->push_back( *((const int*)apData) ); break;
		case eVertexBufferElementFormat_Byte:	mpByteArray->push_back( *((const unsigned char*)apData) ); break;
		}
	}

	void* cVtxBufferGLElementArray::GetArrayPtr()
	{
		switch(mFormat)
		{
		case eVertexBufferElementFormat_Float:	return &(*mpFloatArray)[0];
		case eVertexBufferElementFormat_Int:	return &(*mpIntArray)[0];
		case eVertexBufferElementFormat_Byte:	return &(*mpByteArray)[0];
		}
		return NULL;
	}

	size_t cVtxBufferGLElementArray::Size()
	{
		switch(mFormat)
		{
		case eVertexBufferElementFormat_Float:	return mpFloatArray->size();
		case eVertexBufferElementFormat_Int:	return mpIntArray->size();
		case eVertexBufferElementFormat_Byte:	return mpByteArray->size();
		}
		return 0;
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iVertexBufferOpenGL::iVertexBufferOpenGL(	iLowLevelGraphics* apLowLevelGraphics,
												eVertexBufferType aType,
												eVertexBufferDrawType aDrawType,eVertexBufferUsageType aUsageType,
												int alReserveVtxSize,int alReserveIdxSize) :
	iVertexBuffer(apLowLevelGraphics, aType, aDrawType,aUsageType, alReserveVtxSize, alReserveIdxSize)
	{
		if(alReserveIdxSize>0)
			mvIndexArray.reserve(alReserveIdxSize);

		mbHasShadowDouble = false;

		mpLowLevelGraphics = apLowLevelGraphics;

		for(int i=0; i< eVertexBufferElement_LastEnum; ++i)	mvElementArrayIndex[i] =-1;
	}

	//----------------------------------------------------------------------

	iVertexBufferOpenGL::~iVertexBufferOpenGL()
	{
		STLDeleteAll(mvElementArrays);	
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	void iVertexBufferOpenGL::CreateElementArray(	eVertexBufferElement aType, eVertexBufferElementFormat aFormat,
													int alElementNum, int alProgramVarIndex)
	{
		tVertexElementFlag elementFlag = GetVertexElementFlagFromEnum(aType);
		if(elementFlag & mVertexFlags){
			Error("Vertex element of type %d already present in buffer %d!\n",aType,this);
			return;
		}
		mVertexFlags |= elementFlag;

		cVtxBufferGLElementArray *pElement = hplNew(cVtxBufferGLElementArray, (aFormat) );
		pElement->mType = aType;
		pElement->mFlag = elementFlag;
		pElement->mFormat = aFormat;
		pElement->mlElementNum = alElementNum;
		pElement->mlProgramVarIndex = alProgramVarIndex;
		 
		if(mlReservedVtxSize > 0) pElement->Reserve(alElementNum * mlReservedVtxSize);

		mvElementArrayIndex[aType] = (char)mvElementArrays.size();

		mvElementArrays.push_back(pElement);
	}
	//-----------------------------------------------------------------------

	void iVertexBufferOpenGL::AddVertexVec3f(eVertexBufferElement aType,const cVector3f& avVtx)
	{
		cVtxBufferGLElementArray *pElement = GetElementArray(aType);

		pElement->PushBack(&avVtx.x);
		pElement->PushBack(&avVtx.y);
		
		if(pElement->mlElementNum > 2)
			pElement->PushBack(&avVtx.z);
		if(pElement->mlElementNum > 3)
		{
			float lX = 1;
			pElement->PushBack(&lX);
		}
	}

	//-----------------------------------------------------------------------

	void iVertexBufferOpenGL::AddVertexVec4f(eVertexBufferElement aType,const cVector3f& avVtx, float afW)
	{
		cVtxBufferGLElementArray *pElement = GetElementArray(aType);

		pElement->PushBack(&avVtx.x);
		pElement->PushBack(&avVtx.y);
		pElement->PushBack(&avVtx.z);
		pElement->PushBack(&afW);
	}

	//-----------------------------------------------------------------------

	void iVertexBufferOpenGL::AddVertexColor(eVertexBufferElement aType,const cColor& aColor)
	{
		cVtxBufferGLElementArray *pElement = GetElementArray(aType);
        
		if(pElement->mFormat == eVertexBufferElementFormat_Float)
		{
			pElement->PushBack(&aColor.r);
			pElement->PushBack(&aColor.g);
			pElement->PushBack(&aColor.b);
			pElement->PushBack(&aColor.a);
		}
	}

	//-----------------------------------------------------------------------

	void iVertexBufferOpenGL::AddIndex(unsigned int alIndex)
	{
		mvIndexArray.push_back(alIndex);
	}

	//-----------------------------------------------------------------------

	cBoundingVolume iVertexBufferOpenGL::CreateBoundingVolume()
	{
		cBoundingVolume bv;

        if( (mVertexFlags & eVertexElementFlag_Position) == 0)
		{
			Warning("Could not create bounding volume from buffer %d  because no position element was present!\n",this);
			return bv;
		}

		cVtxBufferGLElementArray *pElement = GetElementArray(eVertexBufferElement_Position);
		if(pElement->mFormat != eVertexBufferElementFormat_Float)
		{
			Warning("Could not breate bounding volume since postion was not for format float in buffer %d!\n",this);
			return bv;
		}

		bv.AddArrayPoints((float*)pElement->GetArrayPtr(), GetVertexNum());
		bv.CreateFromPoints(pElement->mlElementNum);

		return bv;
	}

	//-----------------------------------------------------------------------

	bool iVertexBufferOpenGL::Compile(tVertexCompileFlag aFlags)
	{
		//////////////////////////////////////////
		//Create tangents
		if(aFlags & eVertexCompileFlag_CreateTangents)
		{
			CreateElementArray(eVertexBufferElement_Texture1Tangent, eVertexBufferElementFormat_Float, 4);

			cVtxBufferGLElementArray *pPosElement = GetElementArray(eVertexBufferElement_Position);
			cVtxBufferGLElementArray *pNormalElement = GetElementArray(eVertexBufferElement_Normal);
			cVtxBufferGLElementArray *pTextureElement = GetElementArray(eVertexBufferElement_Texture0);
			cVtxBufferGLElementArray *pTangentElement = GetElementArray(eVertexBufferElement_Texture1Tangent);

			pTangentElement->Resize(GetVertexNum()*4);
			
			cMath::CreateTriTangentVectors((float*)pTangentElement->GetArrayPtr(),
											&mvIndexArray[0], GetIndexNum(),

											(float*)pPosElement->GetArrayPtr(),
											pPosElement->mlElementNum,

											(float*)pTextureElement->GetArrayPtr(),
											(float*)pNormalElement->GetArrayPtr(),
											GetVertexNum()
				);
		}

		//////////////////////////////////////////
		//Compile implementation specific
		CompileSpecific();

		return true;
	}

	//-----------------------------------------------------------------------

	void iVertexBufferOpenGL::CreateShadowDouble(bool abUpdateData)
	{
		cVtxBufferGLElementArray *pPosElement = GetElementArray(eVertexBufferElement_Position);

		//Set to new size.
		int lSize = (int)pPosElement->Size();
		pPosElement->Reserve(lSize*2);

		float *pPosArray = (float*)pPosElement->GetArrayPtr();

		int lCount = lSize /4;
		int lZero =0;
		for(int i=0; i< lCount; i++)
		{
			pPosElement->PushBack(&pPosArray[i*4+0]);
			pPosElement->PushBack(&pPosArray[i*4+1]);
			pPosElement->PushBack(&pPosArray[i*4+2]);
			pPosElement->PushBack(&lZero);
		}

		mbHasShadowDouble = true;

		if(abUpdateData)
		{
			UpdateData(eVertexElementFlag_Position, false);
		}
	}

	//-----------------------------------------------------------------------

	void iVertexBufferOpenGL::Transform(const cMatrixf &a_mtxTransform)
	{
		///////////////
		//Get position
		float *pPosArray = (float*)GetElementArray(eVertexBufferElement_Position)->GetArrayPtr();
		
		///////////////
		//Get normal
		float *pNormalArray = NULL;
		cVtxBufferGLElementArray *pNormalElement = GetElementArray(eVertexBufferElement_Normal);
		if(pNormalElement) pNormalArray = (float*)pNormalElement->GetArrayPtr();
		
		///////////////
		//Get tangent
		float *pTangentArray = NULL;
		cVtxBufferGLElementArray *pTangentElement = GetElementArray(eVertexBufferElement_Texture1Tangent);
		if(pTangentElement)	pTangentArray = (float*)pTangentElement->GetArrayPtr();
	
		//////////////////////
		// Get variables
		int lVtxNum = GetVertexNum();

		cMatrixf mtxRot = a_mtxTransform.GetRotation();
		cMatrixf mtxNormalRot = cMath::MatrixInverse(mtxRot).GetTranspose();

		int lVtxStride = GetElementArray(eVertexBufferElement_Position)->mlElementNum;
		int lShadowDoubleOffset = GetVertexNum()*4;

		//////////////////////
		// Iterate and transform data
		for(int i=0; i<lVtxNum; i++)
		{
			//////////////////
			// Position
			if(pPosArray)
			{
				float* pPos = &pPosArray[i*lVtxStride];
				
				cVector3f vPos = cMath::MatrixMul(a_mtxTransform, cVector3f(pPos[0],pPos[1],pPos[2]));
				pPos[0] = vPos.x; pPos[1] = vPos.y; pPos[2] = vPos.z;

				if(mbHasShadowDouble){
					float* pExtraPos = &pPosArray[i*lVtxStride + lShadowDoubleOffset];
					pExtraPos[0] = vPos.x; pExtraPos[1] = vPos.y; pExtraPos[2] = vPos.z;
				}
			}
		
			//////////////////
			// Normals
			if(pNormalArray)
			{
				float* pNorm = &pNormalArray[i*3];

				cVector3f vNorm = cMath::MatrixMul3x3(mtxNormalRot, cVector3f(pNorm[0],pNorm[1],pNorm[2]));
				vNorm.Normalize();
				pNorm[0] = vNorm.x; pNorm[1] = vNorm.y; pNorm[2] = vNorm.z;
			}

			//////////////////
			// Tangents
			if(pTangentArray)
			{
				float* pTan = &pTangentArray[i*4];

				cVector3f vTan = cMath::MatrixMul3x3(mtxRot, cVector3f(pTan[0],pTan[1],pTan[2]));
				vTan.Normalize();
				pTan[0] = vTan.x; pTan[1] = vTan.y; pTan[2] = vTan.z;
			}
		}

		////////////////////////////
		//Update the data
		tVertexElementFlag vtxFlag = eVertexElementFlag_Position;
		if(pNormalArray) vtxFlag |= eVertexElementFlag_Normal;
		if(pTangentArray) vtxFlag |= eVertexElementFlag_Texture1;
		
		UpdateData(vtxFlag ,false);
	}

	//-----------------------------------------------------------------------

	int iVertexBufferOpenGL::GetElementNum(eVertexBufferElement aElement)
	{
		cVtxBufferGLElementArray *pElement = GetElementArray(aElement);
		if(pElement==NULL) return 0;	

		return pElement->mlElementNum;
	}

	//-----------------------------------------------------------------------

	eVertexBufferElementFormat iVertexBufferOpenGL::GetElementFormat(eVertexBufferElement aElement)
	{
		cVtxBufferGLElementArray *pElement = GetElementArray(aElement);
		if(pElement==NULL) return eVertexBufferElementFormat_LastEnum;	

		return pElement->mFormat;
	}

	//-----------------------------------------------------------------------

	int iVertexBufferOpenGL::GetElementProgramVarIndex(eVertexBufferElement aElement)
	{
		cVtxBufferGLElementArray *pElement = GetElementArray(aElement);
		if(pElement==NULL) return 0;	

		return pElement->mlProgramVarIndex;
	}

	//-----------------------------------------------------------------------

	float* iVertexBufferOpenGL::GetFloatArray(eVertexBufferElement aElement)
	{
		cVtxBufferGLElementArray *pElement = GetElementArray(aElement);
        if(pElement==NULL) return NULL;	
		
		return (float*)pElement->GetArrayPtr();
	}
	
	int* iVertexBufferOpenGL::GetIntArray(eVertexBufferElement aElement)
	{
		cVtxBufferGLElementArray *pElement = GetElementArray(aElement);
		if(pElement==NULL) return NULL;	

		return (int*)pElement->GetArrayPtr();
	}
	
	unsigned char* iVertexBufferOpenGL::GetByteArray(eVertexBufferElement aElement)
	{
		cVtxBufferGLElementArray *pElement = GetElementArray(aElement);
		if(pElement==NULL) return NULL;	

		return (unsigned char*)pElement->GetArrayPtr();
	}

	//-----------------------------------------------------------------------

	unsigned int* iVertexBufferOpenGL::GetIndices()
	{
		return &mvIndexArray[0];
	}

	//-----------------------------------------------------------------------

	void iVertexBufferOpenGL::ResizeArray(eVertexBufferElement aElement, int alSize)
	{
		cVtxBufferGLElementArray *pElement = GetElementArray(aElement);
		if(pElement==NULL) return;

		pElement->Resize(alSize);
	}

	//-----------------------------------------------------------------------

	void iVertexBufferOpenGL::ResizeIndices(int alSize)
	{
		mvIndexArray.resize(alSize);
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* iVertexBufferOpenGL::CreateCopy(	eVertexBufferType aType, eVertexBufferUsageType aUsageType,
													tVertexElementFlag alVtxToCopy)
	{
		if(alVtxToCopy == eFlagBit_All) alVtxToCopy = mVertexFlags;

		iVertexBufferOpenGL *pVtxBuff;
		if(mpLowLevelGraphics->GetCaps(eGraphicCaps_VertexBufferObject) && aType == eVertexBufferType_Hardware)
		{
			pVtxBuff = hplNew( cVertexBufferOGL_VBO, (mpLowLevelGraphics, mDrawType,aUsageType,GetIndexNum(),GetVertexNum()) );
		}
		else
		{
			pVtxBuff = hplNew( cVertexBufferOGL_Array, (mpLowLevelGraphics, mDrawType,aUsageType,GetIndexNum(),GetVertexNum()) );
		}

		//Copy the vertices to the new buffer.
		for(size_t i=0; i<mvElementArrays.size(); ++i)
		{
			cVtxBufferGLElementArray *pSrcElement = mvElementArrays[i];
			if( (pSrcElement->mFlag & alVtxToCopy) == 0) continue;
			
			pVtxBuff->CreateElementArray(pSrcElement->mType,pSrcElement->mFormat,pSrcElement->mlElementNum,pSrcElement->mlProgramVarIndex);
			cVtxBufferGLElementArray *pDestElement = pVtxBuff->GetElementArray(pSrcElement->mType);

			pDestElement->Resize(pSrcElement->Size());

			memcpy(	pDestElement->GetArrayPtr(), pSrcElement->GetArrayPtr(), 
					pSrcElement->Size() * GetVertexFormatByteSize(pSrcElement->mFormat));
		}

        /*for(int i=0; i < klNumOfVertexFlags; i++)
		{
			if( (kvVertexFlags[i] & mVertexFlags) && (kvVertexFlags[i] & alVtxToCopy))
			{
				int lElements = kvVertexElements[i];
				if(mbTangents && kvVertexFlags[i] == eVertexElementFlag_Texture1)
					lElements=4;

				pVtxBuff->ResizeArray(kvVertexFlags[i], (int)mvVertexArray[i].size());

				memcpy(pVtxBuff->GetArray(kvVertexFlags[i]),
						&mvVertexArray[i][0], mvVertexArray[i].size() * sizeof(float));
			}
		}*/

		//Copy indices to the new buffer
        pVtxBuff->ResizeIndices(GetIndexNum());
		memcpy(pVtxBuff->GetIndices(), GetIndices(), GetIndexNum() * sizeof(unsigned int) );

		pVtxBuff->mbTangents = mbTangents;
		pVtxBuff->mbHasShadowDouble = mbHasShadowDouble;

		pVtxBuff->Compile(0);

		return pVtxBuff;
	}

	//-----------------------------------------------------------------------

	int iVertexBufferOpenGL::GetVertexNum()
	{
		cVtxBufferGLElementArray *pPosElement = GetElementArray(eVertexBufferElement_Position);
		int lSize = (int)pPosElement->Size() / pPosElement->mlElementNum;

		//If there is a shadow double, just return the length of the first half.
		if(mbHasShadowDouble)	return lSize / 2;
		else					return lSize;
	}
	
	//-----------------------------------------------------------------------

	int iVertexBufferOpenGL::GetIndexNum()
	{
		return (int)mvIndexArray.size();
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	
	//-----------------------------------------------------------------------

}


