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

#include "impl/VertexBufferOGL_Array.h"

#include "system/LowLevelSystem.h"
#include "math/Math.h"

#include "impl/LowLevelGraphicsSDL.h"

#include <GL/glew.h>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVertexBufferOGL_Array::cVertexBufferOGL_Array(	iLowLevelGraphics* apLowLevelGraphics,
										eVertexBufferDrawType aDrawType,eVertexBufferUsageType aUsageType,
										int alReserveVtxSize,int alReserveIdxSize) :
	iVertexBufferOpenGL(apLowLevelGraphics,eVertexBufferType_Software, aDrawType,aUsageType, alReserveVtxSize, alReserveIdxSize)
	{
		
	}

	cVertexBufferOGL_Array::~cVertexBufferOGL_Array()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cVertexBufferOGL_Array::UpdateData(tVertexElementFlag aTypes, bool abIndices)
	{
		
	}
	//-----------------------------------------------------------------------

	void cVertexBufferOGL_Array::Draw(eVertexBufferDrawType aDrawType)
	{
		;

		eVertexBufferDrawType drawType = aDrawType == eVertexBufferDrawType_LastEnum ? mDrawType : aDrawType;

		///////////////////////////////
		//Get the draw type
		GLenum mode = GetDrawModeFromDrawType(drawType);
		
		int lSize = mlElementNum;
		if(mlElementNum<0) lSize = GetIndexNum();
		
		glDrawElements(mode,lSize,GL_UNSIGNED_INT, &mvIndexArray[0]);
	}

	void cVertexBufferOGL_Array::DrawIndices(	unsigned int *apIndices, int alCount,
												eVertexBufferDrawType aDrawType)
	{
		;

		eVertexBufferDrawType drawType = aDrawType == eVertexBufferDrawType_LastEnum ? mDrawType : aDrawType;

		///////////////////////////////
		//Get the draw type
		GLenum mode = GetDrawModeFromDrawType(drawType);

		//////////////////////////////////
		//Bind and draw the buffer
		glDrawElements(mode, alCount, GL_UNSIGNED_INT, apIndices);
	}


	//-----------------------------------------------------------------------

	void cVertexBufferOGL_Array::Bind()
	{
		;

		SetVertexStates();
	}

	//-----------------------------------------------------------------------

	void cVertexBufferOGL_Array::UnBind()
	{
		;

		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

		for(size_t i=0; i<mvElementArrays.size(); ++i)
		{
			cVtxBufferGLElementArray *pElement = mvElementArrays[i];

			int lTextureUnit = GetVertexElementTextureUnit(pElement->mType);
			if(lTextureUnit >=0) glClientActiveTextureARB(GL_TEXTURE0_ARB + lTextureUnit);

			glDisableClientState( GetGLArrayFromVertexElement(pElement->mType) );
		}
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cVertexBufferOGL_Array::CompileSpecific()
	{
	}

	//-----------------------------------------------------------------------

	iVertexBufferOpenGL* cVertexBufferOGL_Array::CreateDataCopy(tVertexElementFlag aFlags, eVertexBufferDrawType aDrawType,
																eVertexBufferUsageType aUsageType,
																int alReserveVtxSize,int alReserveIdxSize)
	{
		return hplNew(cVertexBufferOGL_Array, (mpLowLevelGraphics,aDrawType,aUsageType,alReserveVtxSize,alReserveIdxSize));
	}

	//-----------------------------------------------------------------------

	void cVertexBufferOGL_Array::SetVertexStates()
	{
		for(size_t i=0; i<mvElementArrays.size(); ++i)
		{
			cVtxBufferGLElementArray *pElement = mvElementArrays[i];

			GLenum GLType = GetGLTypeFromVertexFormat(pElement->mFormat);
			int lSize = pElement->mlElementNum;

			int lTextureUnit = GetVertexElementTextureUnit(pElement->mType);
			if(lTextureUnit >=0) glClientActiveTextureARB(GL_TEXTURE0_ARB + lTextureUnit);

			glEnableClientState( GetGLArrayFromVertexElement(pElement->mType) );

			switch(pElement->mType)
			{
			case eVertexBufferElement_Normal:
				glNormalPointer(GLType, 0, pElement->GetArrayPtr());
				break;

			case eVertexBufferElement_Position:	
				glVertexPointer(lSize,GLType, 0, pElement->GetArrayPtr());
				break;

			case eVertexBufferElement_Color0:
				glColorPointer(lSize,GLType, 0, pElement->GetArrayPtr());
				break;

			case eVertexBufferElement_Color1:
				glSecondaryColorPointerEXT(lSize,GLType, 0, pElement->GetArrayPtr());
				break;

			case eVertexBufferElement_Texture1Tangent:	
			case eVertexBufferElement_Texture0:			
			case eVertexBufferElement_Texture1:			
			case eVertexBufferElement_Texture2:			
			case eVertexBufferElement_Texture3:			
			case eVertexBufferElement_Texture4:			
				glTexCoordPointer(lSize,GLType,0, pElement->GetArrayPtr());
				break;					
			//TODO: User types
			}
		}
		glClientActiveTextureARB(GL_TEXTURE0_ARB);

		/*/// POSITION /////////////////////////
		if(aFlags & eVertexElementFlag_Position){
			glEnableClientState(GL_VERTEX_ARRAY );
			int idx = cMath::Log2ToInt(eVertexElementFlag_Position);
			glVertexPointer(kvVertexElements[idx],GL_FLOAT, sizeof(float)*kvVertexElements[idx], &mvVertexArray[idx][0]);
		}
		else
		{
			glDisableClientState(GL_VERTEX_ARRAY );
		}

		/// COLOR 0 /////////////////////////
		if(aFlags & eVertexElementFlag_Color0)
		{
			glEnableClientState(GL_COLOR_ARRAY );
			int idx = cMath::Log2ToInt(eVertexElementFlag_Color0);
			glColorPointer(kvVertexElements[idx],GL_FLOAT, sizeof(float)*kvVertexElements[idx], &mvVertexArray[idx][0]);
		}
		else
		{
			glDisableClientState(GL_COLOR_ARRAY );
		}

		/// NORMAL /////////////////////////
		if(aFlags & eVertexElementFlag_Normal)
		{
			glEnableClientState(GL_NORMAL_ARRAY );
			glNormalPointer(GL_FLOAT, sizeof(float)*3, &mvVertexArray[cMath::Log2ToInt(eVertexElementFlag_Normal)][0]);
		}
		else
		{
			glDisableClientState(GL_NORMAL_ARRAY );
		}

		/// TEXTURE 0 /////////////////////////
		if(aFlags & eVertexElementFlag_Texture0)
		{
			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
			int idx =  cMath::Log2ToInt(eVertexElementFlag_Texture0);
			glTexCoordPointer(kvVertexElements[idx],GL_FLOAT,sizeof(float)*kvVertexElements[idx],&mvVertexArray[idx][0] );
		}
		else {
			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		}

		/// TEXTURE 1 /////////////////////////
		if(aFlags & eVertexElementFlag_Texture1){
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
			int idx =  cMath::Log2ToInt(eVertexElementFlag_Texture1);

			if(mbTangents)
				glTexCoordPointer(4,GL_FLOAT,sizeof(float)*4,&mvVertexArray[idx][0] );
			else
				glTexCoordPointer(kvVertexElements[idx],GL_FLOAT,sizeof(float)*kvVertexElements[idx],&mvVertexArray[idx][0] );
		}
		else {
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		}

		/// TEXTURE 2 /////////////////////////
		if(aFlags & eVertexElementFlag_Texture2){
			glClientActiveTextureARB(GL_TEXTURE2_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
			int idx =  cMath::Log2ToInt(eVertexElementFlag_Texture2);
			glTexCoordPointer(kvVertexElements[idx],GL_FLOAT,sizeof(float)*kvVertexElements[idx],&mvVertexArray[idx][0] );
		}
		else {
			glClientActiveTextureARB(GL_TEXTURE2_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		}

		/// TEXTURE 3 /////////////////////////
		if(aFlags & eVertexElementFlag_Texture3){
			glClientActiveTextureARB(GL_TEXTURE3_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
			int idx =  cMath::Log2ToInt(eVertexElementFlag_Texture3);
			glTexCoordPointer(kvVertexElements[idx],GL_FLOAT,sizeof(float)*kvVertexElements[idx],&mvVertexArray[idx][0] );
		}
		else {
			glClientActiveTextureARB(GL_TEXTURE3_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		}

		/// TEXTURE 4 /////////////////////////
		if(aFlags & eVertexElementFlag_Texture4){
			glClientActiveTextureARB(GL_TEXTURE4_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
			int idx =  cMath::Log2ToInt(eVertexElementFlag_Texture4);
			glTexCoordPointer(kvVertexElements[idx],GL_FLOAT,sizeof(float)*kvVertexElements[idx],&mvVertexArray[idx][0] );
		}
		else {
			glClientActiveTextureARB(GL_TEXTURE4_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		}*/
	}

	//-----------------------------------------------------------------------

}
