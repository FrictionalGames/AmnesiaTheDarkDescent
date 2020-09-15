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

#ifndef LUX_STATIC_PROP_H
#define LUX_STATIC_PROP_H


//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxMap;

//----------------------------------------------
// ATTENTION!
// The entity classes is only be used if for some reason 
// static props needs to be turned of or what not.
//----------------------------------------------

class iLuxStaticProp_SaveData : public iSerializable
{
	kSerializableClassInit(iLuxEntity_SaveData)
public:
	tString msName;
	int mlID;

	tString msFileName;
	cMatrixf m_mtxOnLoadTransform;
};

//----------------------------------------------

class cLuxStaticProp
{
friend class cLuxStaticPropLoader;
public:
	cLuxStaticProp(const tString &asName, int alID);
	~cLuxStaticProp();
	
private:
	tString msName;
	int mlID;

	cLuxMap *mpMap;
	tString msFileName;
	cMatrixf m_mtxOnLoadTransform;

	cMeshEntity *mpMeshEntity;
	
	std::vector<iPhysicsBody*> mvBodies;
	std::vector<iPhysicsJoint*> mvJoints;

	std::vector<iLight*> mvLights;
	std::vector<cParticleSystem*> mvParticleSystems;
	std::vector<cBillboard*> mvBillboards;
	std::vector<cBeam*> mvBeams;
	std::vector<cSoundEntity*> mvSoundEntities;
};


//----------------------------------------------

class cLuxStaticPropLoader : public cEntityLoader_Object
{
public:
	cLuxStaticPropLoader(const tString& asName);
	virtual ~cLuxStaticPropLoader(){}

	void BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);
	void AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);
};

//----------------------------------------------

#endif // LUX_STATIC_PROP_H
