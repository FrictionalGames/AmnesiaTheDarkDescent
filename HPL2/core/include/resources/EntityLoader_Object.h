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

#ifndef HPL_ENTITY_LOADER_OBJECT_H
#define HPL_ENTITY_LOADER_OBJECT_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

#include "resources/Resources.h"

class TiXmlElement;

namespace hpl {

	class iPhysicsBody;
	class iPhysicsJoint;
	class iPhysicsWorld;
	class cMesh;
	class cMeshEntity;
	class cParticleSystem;
	class cBillboard;
	class cBeam;
	class cSoundEntity;
	class iLight;
	class iHapticShape;
	class cBoneState;

	//--------------------------------------------

	class cEntityBodyExtraData
	{
	public:
		cMatrixf m_mtxLocalTransform;
	};


	//--------------------------------------------
	
	class cEntityLoader_Object : public iEntityLoader
	{
	public:
		cEntityLoader_Object(const tString &asName) : iEntityLoader(asName)
		{
			mbLoadAnimations=true;
			mbLoadParticleSystems=true;
			mbLoadBillboards=true;
			mbLoadSounds=true;
			mbLoadLights=true;
			mbLoadAsStatic = false;
		}
		virtual ~cEntityLoader_Object(){}

        iEntity3D* Load(const tString &asName, int alID, bool abActive, cXmlElement *apRootElem, 
						const cMatrixf &a_mtxTransform, const cVector3f &avScale, 
						cWorld *apWorld, const tString &asFileName, const tWString &asFullPath, cResourceVarsObject *apInstanceVars);		

	protected:
		virtual void BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars)=0;
		virtual void AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars)=0;
		
		void AttachEntityChild(iEntity3D *apParent, const cMatrixf& a_mtxInvParent, iEntity3D *apChild);
		void AttachBoneChild(cBoneState *apBoneState, const cMatrixf& a_mtxInvParent, iEntity3D *apChild);
		void AttachBoneToBody(iPhysicsBody *apParentBody, const cMatrixf& a_mtxInvParent, cBoneState *apBoneState);
		void LoadAndAttachChildren(cXmlElement *apMainElem, iEntity3D *apEntityParent, cBoneState *apBoneStateParent, 
            						std::list<iEntity3D*>& a_lstChildList, tNodeStateMap &a_mapBoneStates,
									bool abRemoveAttachedChild, bool abIsBody);

		cBillboard* GetBillboardFromID(int alID);
		iLight* GetLightFromName(const tString& asName);

		void SetBodyProperties(iPhysicsBody *apBody, cXmlElement *apPhysicsElem);
		void SetJointProperties(iPhysicsJoint *apJoint, cXmlElement *apJointElem,cWorld *apWorld);

		void LoadController(iPhysicsJoint *apJoint,iPhysicsWorld *apPhysicsWorld, TiXmlElement *apElem);

		eAnimationEventType GetAnimationEventType(const char* apString);

		void LoadUserVariables(cXmlElement *apRootElem);
		
		tString msSubType;
		int mlID;
		bool mbActive;
		cVector3f mvScale;

		bool mbNodeAnimation;

		tString msFileName;

		tString msEntityType;
		tString msEntitySubType;

		std::vector<iPhysicsBody*> mvBodies;
		std::vector<iPhysicsJoint*> mvJoints;

		std::vector<cEntityBodyExtraData> mvBodyExtraData;

		std::vector<iHapticShape*> mvHapticShapes;
		
		std::vector<iLight*> mvLights;
		std::vector<cParticleSystem*> mvParticleSystems;
		std::vector<cBillboard*> mvBillboards;
		std::vector<cBeam*> mvBeams;
		std::vector<cSoundEntity*> mvSoundEntities;
		
		cMeshEntity *mpEntity;
		cMesh *mpMesh;

		bool mbLoadAnimations;
		bool mbLoadParticleSystems;
		bool mbLoadBillboards;
		bool mbLoadSounds;
		bool mbLoadLights;
		bool mbLoadAsStatic;
	};

};
#endif // HPL_ENTITY_LOADER_OBJECT_H
