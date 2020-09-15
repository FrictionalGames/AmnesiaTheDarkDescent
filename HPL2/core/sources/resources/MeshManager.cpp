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

#include "resources/MeshManager.h"
#include "system/String.h"
#include "system/System.h"
#include "resources/Resources.h"
#include "graphics/Mesh.h"
#include "system/LowLevelSystem.h"
#include "resources/MeshLoaderHandler.h"
#include "resources/FileSearcher.h"
#include "graphics/SubMesh.h"
#include "graphics/VertexBuffer.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMeshManager::cMeshManager(cGraphics* apGraphic,cResources *apResources)
		: iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
							apResources->GetLowLevelSystem())
	{
		mpGraphics = apGraphic;
		mpResources = apResources;

		msFastloadMaterial = "";
		mbUseFastloadMaterial = false;
	}

	cMeshManager::~cMeshManager()
	{
		DestroyAll();

		Log(" Done with meshes\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMesh* cMeshManager::CreateMesh(const tString& asName, tMeshLoadFlag aFlag)
	{
		tWString sPath;
		cMesh* pMesh;
		tString asNewName;

		BeginLoad(asName);

		asNewName = asName;

		//If the file is missing an extension, search for an existing file.
		if(cString::GetFileExt(asNewName) == "")
		{
			bool bFound = false;
			tStringVec *pTypes = mpResources->GetMeshLoaderHandler()->GetSupportedTypes();
			for(size_t i=0; i< pTypes->size(); i++)
			{
				asNewName = cString::SetFileExt(asNewName, (*pTypes)[i]);
				tWString sPath = mpResources->GetFileSearcher()->GetFilePath(asNewName);
				if(sPath != _W(""))
				{
					bFound = true;
					break;
				}
			}

			if(bFound == false){
				Error("Couldn't find mesh file '%s' in any supported format!\n",asName.c_str());
				EndLoad();
				return NULL;
			}
		}

		pMesh = static_cast<cMesh*>(FindLoadedResource(asNewName,sPath));

		//An extra hackish check to load msh or anim
		//TODO: When the model is loaded, then it 
		/*if(pMesh==NULL && sPath==_W(""))
		{
			tString sExt = cString::GetFileExt(asNewName);
			tString sNewExt="";
			if(sExt=="dae") sNewExt = "msh";
			if(sExt=="dae_anim") sNewExt = "anim";
			
			if(sNewExt != "")
			{
				FindLoadedResource(cString::SetFileExt(asNewName, sNewExt), sPath);

				if(sPath!=_W("")) sPath = cString::SetFileExtW(sPath, cString::To16Char(sExt));
			}
		}*/

		if(pMesh==NULL && sPath!=_W(""))
		{
			pMesh = mpResources->GetMeshLoaderHandler()->LoadMesh(sPath,aFlag);
			if(pMesh == NULL)
			{
				EndLoad();
				return NULL;
			}

			AddResource(pMesh);
		}

		if(pMesh)pMesh->IncUserCount();
		else Error("Couldn't load mesh '%s'\n",asNewName.c_str());
		
		EndLoad();
		return pMesh;
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* cMeshManager::CreateVertexBufferFromMesh(const tString& asName, tVertexElementFlag alVtxToCopy)
	{
		cMesh *pMesh = CreateMesh(asName);
		if(pMesh==NULL) return NULL;

		iVertexBuffer *pVtxBuffer = pMesh->GetSubMesh(0)->GetVertexBuffer()->CreateCopy(eVertexBufferType_Hardware,
																						eVertexBufferUsageType_Static,
																						alVtxToCopy);
		Destroy(pMesh);
		return pVtxBuffer;
	}

	//-----------------------------------------------------------------------

	void cMeshManager::Unload(iResourceBase* apResource)
	{

	}
	//-----------------------------------------------------------------------

	void cMeshManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();

		if(apResource->HasUsers()==false){
			RemoveResource(apResource);
			hplDelete(apResource);
		}
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------
}
