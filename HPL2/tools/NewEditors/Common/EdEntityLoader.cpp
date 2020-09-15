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

#include "../Common/EdEntityLoader.h"

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEdEntityLoader::cEdEntityLoader(cResources* apResources) : cEntityLoader_Object("EditorEntityLoader")
{
	Log("Created Entity Loader\n");
	mpResources = apResources;
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cMeshEntity* cEdEntityLoader::LoadEntFile(int alID, const tString& asName, 
										  const tString& asFilename, cWorld* apWorld, 
										  bool abLoadAnims, bool abLoadParticles, 
										  bool abLoadBillboards, bool abLoadSounds, bool abLoadLights)
{
	tWString sFullPath = mpResources->GetFileSearcher()->GetFilePath(asFilename);

	cMeshEntity* pEntity = NULL;
	cXmlElement* pXmlEntity = NULL;

	iXmlDocument* pDoc = mpResources->LoadXmlDocument(asFilename);
	if(pDoc==NULL)
		return NULL;

	pEntity = LoadEntityFromElement(alID, 
									asName,
									pDoc,
									apWorld,
									asFilename,
									sFullPath,
									abLoadAnims,
									abLoadParticles,
									abLoadBillboards,
									abLoadSounds,
									abLoadLights);

	
	mpResources->DestroyXmlDocument(pDoc);

	return pEntity;
}
//-----------------------------------------------------------------------

cMeshEntity* cEdEntityLoader::LoadEntityFromElement(int alID, const tString& asName, cXmlElement* apElement,
														cWorld* apWorld, const tString& asFilename, const tWString& asFullPath, 
														bool abLoadAnims, bool abLoadParticles, bool abLoadBillboards, bool abLoadSounds, bool abLoadLights)
{
	mbLoadAnimations = abLoadAnims;
	mbLoadParticleSystems = abLoadParticles;
	mbLoadBillboards = abLoadBillboards;
	mbLoadSounds = abLoadSounds;
	mbLoadLights = abLoadLights;

	cMeshEntity* pEntity = (cMeshEntity*) Load(asName,
											   alID,
											   true,
											   apElement,
											   cMatrixf::Identity,
											   cVector3f(1),
											   apWorld,
											   asFilename, 
											   asFullPath, 
											   NULL);


	return pEntity;
}

//-----------------------------------------------------------------------
