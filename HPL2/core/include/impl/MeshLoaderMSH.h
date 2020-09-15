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

#ifndef HPL_MESH_LOADER_MSH_H
#define HPL_MESH_LOADER_MSH_H

#include "resources/MeshLoader.h"

namespace hpl {

	class cMesh;
	class cNode3D;
	class iVertexBuffer;
	class cBinaryBuffer;
	class iVertexBuffer;
	class cBone;
	class cAnimation;
	class cNode3D;

	//----------------------------------------------------------

	#define MSH_FORMAT_MAGIC_NUMBER		0x76034569
	#define MSH_FORMAT_VERSION			7

	//----------------------------------------------------------
	
	class cMeshLoaderMSH : public iMeshLoader
	{
	public:
		cMeshLoaderMSH(iLowLevelGraphics *apLowLevelGraphics);
		~cMeshLoaderMSH();

		cMesh* LoadMesh(const tWString& asFile, tMeshLoadFlag aFlags);
		bool SaveMesh(cMesh* apMesh,const tWString& asFile);

		cWorld* LoadWorld(const tWString& asFile, cScene* apScene,tWorldLoadFlag aFlags);

		cAnimation* LoadAnimation(const tWString& asFile);
		bool SaveAnimation(cAnimation* apAnimation, const tWString& asFile);

	private:
		void AddAnimation(cAnimation *apAnimation, cBinaryBuffer* apBuffer);
		cAnimation* GetAnimation(cBinaryBuffer* apBuffer, const tWString &asFullPath);

		void AddNodeToBuffer(cNode3D *apNode, cBinaryBuffer* apBuffer, int alLevel);
		void GetNodeFromBuffer(cNode3D *apParentNode, cMesh *apMesh, cBinaryBuffer* apBuffer, int alLevel);

		void AddBoneToBuffer(cBone *apBone, cBinaryBuffer* apBuffer, int alLevel);
		void GetBoneFromBuffer(cBone *apParentBone, cBinaryBuffer* apBuffer, int alLevel);

		void* GetVertexBufferWithFormat(iVertexBuffer *apVtxBuffer, eVertexBufferElement aElement, eVertexBufferElementFormat aFormat);
		void AddBinaryBufferDataWithFormat(cBinaryBuffer* apBuffer, void *apSrcData, size_t alSize, eVertexBufferElementFormat aFormat);
		void GetBinaryBufferDataWithFormat(cBinaryBuffer* apBuffer, void *apDestData, size_t alSize, eVertexBufferElementFormat aFormat);
	};

};
#endif // HPL_MESH_LOADER_MSH_H
