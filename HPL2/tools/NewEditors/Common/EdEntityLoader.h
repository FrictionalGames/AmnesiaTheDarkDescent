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

#ifndef ED_ENTITY_LOADER_H
#define ED_ENTITY_LOADER_H

//---------------------------------------------------------------

#include "../Common/StdAfx.h"
using namespace hpl;

//---------------------------------------------------------------

class iEditor;

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////
// cEdEntityLoader
//	Class derived from EntityLoader_Object used to load Entities in editors 
//	(mainly for physics test in ModelEditor)
//	Can choose what portions of the entity to load at a time (bodies, animations, ...)
class cEdEntityLoader : public cEntityLoader_Object
{
public:
	cEdEntityLoader(cResources*);

	void BeforeLoad(cXmlElement*, const cMatrixf&,cWorld*, cResourceVarsObject*){}
	
	void AfterLoad(cXmlElement*, const cMatrixf&,cWorld*, cResourceVarsObject*){}

	cMeshEntity* LoadEntFile(int , const tString&, const tString&, 
							 cWorld*,
							 bool abLoadAnims=false,
							 bool abLoadParticles=false,
							 bool abLoadBillboards=false,
							 bool abLoadSounds=false,
							 bool abLoadLights=true);

	cMeshEntity* LoadEntityFromElement(int, const tString&, cXmlElement*, 
									   cWorld*, const tString& asFilename="", const tWString& asFullPath=_W(""), 
									   bool abLoadAnims=false,
									   bool abLoadParticles=false,
									   bool abLoadBillboards=false,
									   bool abLoadSounds=false,
									   bool abLoadLights=true);

	tLightVec&						GetLights()				{ return mvLights; }
	std::vector<cParticleSystem*>&	GetParticleSystems()	{ return mvParticleSystems; }
	std::vector<cBillboard*>&		GetBillboards()			{ return mvBillboards; }
	std::vector<iPhysicsJoint*>&	GetJoints()				{ return mvJoints; }
	std::vector<cSoundEntity*>&		GetSounds()				{ return mvSoundEntities; }

	cResources* mpResources;
};

//---------------------------------------------------------------

#endif // ED_ENTITY_LOADER_H
