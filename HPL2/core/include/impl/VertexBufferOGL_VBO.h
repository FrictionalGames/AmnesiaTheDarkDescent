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

#ifndef HPL_VERTEXBUFFER_OGL_VBO_H
#define HPL_VERTEXBUFFER_OGL_VBO_H

#include "impl/VertexBufferOpenGL.h"


namespace hpl {
	
	class cVertexBufferOGL_VBO : public iVertexBufferOpenGL
	{
	public:
		cVertexBufferOGL_VBO(	iLowLevelGraphics* apLowLevelGraphics,
								eVertexBufferDrawType aDrawType,eVertexBufferUsageType aUsageType,
								int alReserveVtxSize,int alReserveIdxSize);
		~cVertexBufferOGL_VBO();

		void UpdateData(tVertexElementFlag aTypes, bool abIndices);
				
		void Draw(eVertexBufferDrawType aDrawType = eVertexBufferDrawType_LastEnum);
		void DrawIndices(unsigned int *apIndices, int alCount,
						eVertexBufferDrawType aDrawType = eVertexBufferDrawType_LastEnum);
		
		void Bind();
		void UnBind();
	
	private:
		void CompileSpecific();
		iVertexBufferOpenGL* CreateDataCopy(tVertexElementFlag aFlags, eVertexBufferDrawType aDrawType,
											eVertexBufferUsageType aUsageType,
											int alReserveVtxSize,int alReserveIdxSize);

		void SetVertexStates();

		unsigned int mlElementHandle;
	};

};
#endif // HPL_RENDERER3D_OGL_VBO_H
