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

#include "impl/VertexBufferOGL_VBO.h"

#include "system/LowLevelSystem.h"
#include "math/Math.h"

#include "impl/LowLevelGraphicsSDL.h"

#include <memory.h>

#include <GL/glew.h>


namespace hpl {

#define BUFFER_OFFSET(i) ((void*)(i*sizeof(float)))

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVertexBufferOGL_VBO::cVertexBufferOGL_VBO(iLowLevelGraphics* apLowLevelGraphics,
												eVertexBufferDrawType aDrawType,eVertexBufferUsageType aUsageType,
												int alReserveVtxSize,int alReserveIdxSize) :
	iVertexBufferOpenGL(apLowLevelGraphics,eVertexBufferType_Hardware,  aDrawType,aUsageType, alReserveVtxSize, alReserveIdxSize)
	{
		mlElementHandle =0;
	}

	//-----------------------------------------------------------------------

	cVertexBufferOGL_VBO::~cVertexBufferOGL_VBO()
	{
		;

		for(size_t i=0; i<mvElementArrays.size(); ++i)
		{
			cVtxBufferGLElementArray *pElement = mvElementArrays[i];
			
			glDeleteBuffersARB(1,(GLuint *)&pElement->mlGLHandle);
		}

		glDeleteBuffersARB(1,(GLuint *)&mlElementHandle);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cVertexBufferOGL_VBO::UpdateData(tVertexElementFlag aTypes, bool abIndices)
	{
		;

		GLenum usageType = GL_STATIC_DRAW_ARB;
		if(mUsageType== eVertexBufferUsageType_Dynamic) usageType = GL_DYNAMIC_DRAW_ARB;
		else if(mUsageType== eVertexBufferUsageType_Stream) usageType = GL_STREAM_DRAW_ARB;

		//Create the VBO vertex arrays
		for(size_t i=0; i<mvElementArrays.size(); ++i)
		{
			cVtxBufferGLElementArray *pElement = mvElementArrays[i];

			if( (aTypes & pElement->mFlag) && pElement->mlGLHandle >0)
			{
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, pElement->mlGLHandle);


				int lSize = (int)pElement->Size() * GetVertexFormatByteSize(pElement->mFormat);
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, lSize,	NULL, usageType);
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, lSize, pElement->GetArrayPtr(), usageType);
			}
		}
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

		//Create the VBO index array
		if(abIndices)
		{
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,mlElementHandle);

			//glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,GetIndexNum()*sizeof(unsigned int),
			//	NULL, usageType);

			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GetIndexNum()*sizeof(unsigned int),
				&mvIndexArray[0], usageType);

			//TODO: Same as with vertex, for stream and dynamic.

			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		}
	}
	
	//-----------------------------------------------------------------------

	void cVertexBufferOGL_VBO::Draw(eVertexBufferDrawType aDrawType)
	{
		;

		eVertexBufferDrawType drawType = aDrawType == eVertexBufferDrawType_LastEnum ? mDrawType : aDrawType;

		///////////////////////////////
		//Get the draw type
		GLenum mode = GetDrawModeFromDrawType(drawType);
	
		//////////////////////////////////
		//Bind and draw the buffer
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,mlElementHandle);

		int lSize = mlElementNum;
		if(mlElementNum<0) lSize = GetIndexNum();

		glDrawElements(mode,lSize,GL_UNSIGNED_INT, (char*) NULL);
		//glDrawRangeElements(mode,0,GetVertexNum(),lSize,GL_UNSIGNED_INT, NULL);

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
	}

	//-----------------------------------------------------------------------

	void cVertexBufferOGL_VBO::DrawIndices(unsigned int *apIndices, int alCount,eVertexBufferDrawType aDrawType)
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

	void cVertexBufferOGL_VBO::Bind()
	{
		;

		SetVertexStates();
	}

	//-----------------------------------------------------------------------

	void cVertexBufferOGL_VBO::UnBind()
	{
		;

		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

		for(size_t i=0; i<mvElementArrays.size(); ++i)
		{
			cVtxBufferGLElementArray *pElement = mvElementArrays[i];

			//Log("Unbinding %d handle %d, type: %d\n",i,pElement->mlGLHandle, pElement->mType);
			
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

	void cVertexBufferOGL_VBO::CompileSpecific()
	{
		;

		GLenum usageType = GL_STATIC_DRAW_ARB;
		if(mUsageType== eVertexBufferUsageType_Dynamic) usageType = GL_DYNAMIC_DRAW_ARB;
		else if(mUsageType== eVertexBufferUsageType_Stream) usageType = GL_STREAM_DRAW_ARB;

		//Create the VBO vertex arrays
		for(size_t i=0; i<mvElementArrays.size(); ++i)
		{
			cVtxBufferGLElementArray *pElement = mvElementArrays[i];
            
			glGenBuffersARB(1,(GLuint *)&pElement->mlGLHandle);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, pElement->mlGLHandle);

			glBufferDataARB(GL_ARRAY_BUFFER_ARB, pElement->Size() * GetVertexFormatByteSize(pElement->mFormat),
							pElement->GetArrayPtr(), usageType);

			//Log("Compiling handle %d, type: %d element num: %d\n",pElement->mlGLHandle, pElement->mType, pElement->mlElementNum);

			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

		}

		//Create the VBO index array
		glGenBuffersARB(1,(GLuint *)&mlElementHandle);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,mlElementHandle);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GetIndexNum()*sizeof(unsigned int),
			&mvIndexArray[0], usageType);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);

	}

	//-----------------------------------------------------------------------

	iVertexBufferOpenGL* cVertexBufferOGL_VBO::CreateDataCopy(tVertexElementFlag aFlags, eVertexBufferDrawType aDrawType,
																eVertexBufferUsageType aUsageType,
																int alReserveVtxSize,int alReserveIdxSize)
	{
		return hplNew(cVertexBufferOGL_VBO, (mpLowLevelGraphics,aDrawType,aUsageType,alReserveVtxSize,alReserveIdxSize));
	}

	//-----------------------------------------------------------------------

	void cVertexBufferOGL_VBO::SetVertexStates()
	{
		////////////////////////////////////////
		// Set all vertices except position
		bool bHadExtraTextureUnit = false;
		for(size_t i=0; i<mvElementArrays.size(); ++i)
		{
			cVtxBufferGLElementArray *pElement = mvElementArrays[i];
			if(pElement->mType == eVertexBufferElement_Position) continue;	//Make sure postion is called last...

			//Log("Binding %d handle %d, type: %d\n",i,pElement->mlGLHandle, pElement->mType);
			
			GLenum GLType = GetGLTypeFromVertexFormat(pElement->mFormat);
			int lSize = pElement->mlElementNum;

			int lTextureUnit = GetVertexElementTextureUnit(pElement->mType);
			if(lTextureUnit >=0) {
				if(lTextureUnit>0)bHadExtraTextureUnit = true;
				glClientActiveTextureARB(GL_TEXTURE0_ARB + lTextureUnit);
			}

			glEnableClientState( GetGLArrayFromVertexElement(pElement->mType) );
				
			glBindBufferARB(GL_ARRAY_BUFFER_ARB,pElement->mlGLHandle);
			
			switch(pElement->mType)
			{
			case eVertexBufferElement_Normal:
				//Log(" Normal\n");
				glNormalPointer(GLType, 0, (char*)NULL);
				break;
					
			case eVertexBufferElement_Color0:
				//Log(" Color\n");
				glColorPointer(lSize,GLType, 0, (char*)NULL);
				break;
			
			case eVertexBufferElement_Color1:
				//Log(" Color2\n");
				glSecondaryColorPointerEXT(lSize,GLType, 0, (char*)NULL);
				break;
			
			case eVertexBufferElement_Texture1Tangent:	
			case eVertexBufferElement_Texture0:			
			case eVertexBufferElement_Texture1:			
			case eVertexBufferElement_Texture2:			
			case eVertexBufferElement_Texture3:			
			case eVertexBufferElement_Texture4:	
				//Log(" Texture\n");
				glTexCoordPointer(lSize,GLType,0,(char*)NULL );
				break;					
			//TODO: User types
			}
		}
		if(bHadExtraTextureUnit) glClientActiveTextureARB(GL_TEXTURE0_ARB);

		
		////////////////////////////////////////
		// Set position vertex, so it is set last.
		for(size_t i=0; i<mvElementArrays.size(); ++i)
		{
			cVtxBufferGLElementArray *pElement = mvElementArrays[i];
			if(pElement->mType != eVertexBufferElement_Position) continue; //Only set position

			//Log("Binding %d handle %d, type: %d\n",i,pElement->mlGLHandle, pElement->mType);

			GLenum GLType = GetGLTypeFromVertexFormat(pElement->mFormat);
			int lSize = pElement->mlElementNum;

			glEnableClientState( GetGLArrayFromVertexElement(pElement->mType) );

			glBindBufferARB(GL_ARRAY_BUFFER_ARB,pElement->mlGLHandle);

			glVertexPointer(lSize,GLType, 0, (char*)NULL);
		}

		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
	}

	//-----------------------------------------------------------------------

}

//Old code for compiling all to one array.
//Log("Compiling VBO..\n");

/*int lNum = 0;
int lSize = 0;
int lNumSize = 0;
//Deterimine the number of arrays and the size.
Log("calc size: ");
for(int i=0;i< klNumOfVertexFlags; i++)
{
if(mVertexFlags & kvVertexFlags[i]){
lNum++;

//if(lSize != -1 && lSize != mvTempVertexArray[i].size()){
//	return false;
//}

lSize += (int)mvTempVertexArray[i].size();

lNumSize = (int)mvTempVertexArray[i].size()/GetElementNum(kvVertexFlags[i]);

Log(" %d (%d * %d),",lSize,lNumSize, GetElementNum(kvVertexFlags[i]));
}
}
Log("\n");

//Get memory for the main array
mvVertexArray.resize(lSize);

//Copy the temp arrays into the main one
int lPos =0;

Log("Copy: \n");
for(int i=0;i< klNumOfVertexFlags; i++)
{
if(mVertexFlags & kvVertexFlags[i])
{
int lArraySize = (int)mvTempVertexArray[i].size();

Log(" %d (%d) -> %d \n",i,lArraySize,lPos );

memcpy(&mvVertexArray[lPos], &(mvTempVertexArray[i][0]), sizeof(float)*lArraySize);

mvArrayOffset[i] = lPos;

lPos += lArraySize;
mvTempVertexArray[i].clear();
}
}

Log("Array:\n");
for(int i=0;i< klNumOfVertexFlags; i++)
{
if(mVertexFlags & kvVertexFlags[i])
{
int lOffset =  mvArrayOffset[i];
int lElemNum = GetElementNum(kvVertexFlags[i]);

for(int j=0;j<lNumSize;j++)
{
Log("(");

for(int k=0;k<lElemNum;k++)
{
Log(" %.1f,",mvVertexArray[lOffset + j*lElemNum + k]);
}

Log(") ");
}

Log("\n");
}
}*/



