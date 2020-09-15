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

#include "resources/EntityLoader_Object.h"

#include "system/String.h"
#include "scene/World.h"
#include "system/LowLevelSystem.h"


#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsJoint.h"
#include "physics/PhysicsController.h"
#include "physics/CollideShape.h"
#include "physics/PhysicsJointBall.h"
#include "physics/PhysicsJointHinge.h"
#include "physics/PhysicsJointScrew.h"
#include "physics/PhysicsJointSlider.h"

#include "math/Math.h"

#include "resources/MeshManager.h"
#include "resources/MaterialManager.h"
#include "resources/AnimationManager.h"
#include "resources/MeshLoaderHandler.h"
#include "resources/FileSearcher.h"
#include "resources/XmlDocument.h"
#include "resources/EngineFileLoading.h"

#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/Animation.h"
#include "graphics/BoneState.h"
#include "scene/AnimationState.h"
#include "scene/MeshEntity.h"
#include "scene/Node3D.h"
#include "scene/SoundEntity.h"
#include "scene/BillBoard.h"
#include "scene/ParticleSystem.h"

#include "scene/Light.h"

#include "haptic/Haptic.h"
#include "haptic/LowLevelHaptic.h"
#include "haptic/HapticShape.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	typedef std::multimap<int,iCollideShape*> tLoaderCollideShapeMap;
	typedef tLoaderCollideShapeMap::iterator tLoaderCollideShapeMapIt;

	typedef std::multimap<int,iPhysicsBody*> tLoaderPhysicsBodyMap;
	typedef tLoaderPhysicsBodyMap::iterator tLoaderPhysicsBodyMapIt;

	//-----------------------------------------------------------------------

	static iCollideShape* GetBodyShape(cXmlElement *apBodyElem,iPhysicsWorld *apPhysicsWorld, tLoaderCollideShapeMap &a_setShapes)
	{
		////////////////////////////////////////
		// Get shapes for body
		tCollideShapeVec vShapes;
		cXmlNodeListIterator boundShapeIt = apBodyElem->GetChildIterator();
		while(boundShapeIt.HasNext())
		{
			cXmlElement *pShapeElem = boundShapeIt.Next()->ToElement();
			int lShapeID = pShapeElem->GetAttributeInt("ID");

			tLoaderCollideShapeMapIt it = a_setShapes.find(lShapeID);
			if(it != a_setShapes.end())
			{
				vShapes.push_back(it->second);
				a_setShapes.erase(it);
			}
		}
		
		////////////////////////////////////////
		// Create final shape
		if(vShapes.empty()) return NULL;
        if(vShapes.size()==1) return vShapes[0];

		iCollideShape *pCompound = apPhysicsWorld->CreateCompundShape(vShapes);
		return pCompound;
	}

	//-----------------------------------------------------------------------

	eCollideShapeType ToCollideShape(const tString& asType)
	{
		tString sLowType = cString::ToLowerCase(asType);

		if(sLowType == "box") return eCollideShapeType_Box;
		if(sLowType == "cylinder") return eCollideShapeType_Cylinder;
		if(sLowType == "sphere") return eCollideShapeType_Sphere;
		if(sLowType == "capsule") return eCollideShapeType_Capsule;

		Error("CollideShape '%s' does not exist!\n", asType.c_str());

		return eCollideShapeType_Null;
	}

	static iCollideShape* CreateCollideShape(cXmlElement *apShapeElem, iPhysicsWorld *apPhysicsWorld, const cVector3f &avScale)
	{
		eCollideShapeType type = ToCollideShape(apShapeElem->GetAttributeString("ShapeType"));
		cVector3f vSize = apShapeElem->GetAttributeVector3f("Scale") * avScale;
		cVector3f vPos = apShapeElem->GetAttributeVector3f("RelativeTranslation") * avScale;
		cVector3f vRot = apShapeElem->GetAttributeVector3f("RelativeRotation");

		cMatrixf mtxOffset = cMath::MatrixRotate(vRot,eEulerRotationOrder_XYZ);
		mtxOffset.SetTranslation(vPos);

		switch(type)
		{
		case eCollideShapeType_Box: 
			return apPhysicsWorld->CreateBoxShape(vSize,&mtxOffset);
		case eCollideShapeType_Sphere: 
			return apPhysicsWorld->CreateSphereShape(vSize,&mtxOffset);
		case eCollideShapeType_Cylinder: 
			mtxOffset = cMath::MatrixMul(mtxOffset, cMath::MatrixRotateZ(kPi2f));
			return apPhysicsWorld->CreateCylinderShape(vSize.x,vSize.y,&mtxOffset);
		case eCollideShapeType_Capsule: 
			mtxOffset = cMath::MatrixMul(mtxOffset, cMath::MatrixRotateZ(kPi2f));
			return apPhysicsWorld->CreateCapsuleShape(vSize.x,vSize.y,&mtxOffset);
		}

		return NULL;
	}

	//-----------------------------------------------------------------------
	
	static iPhysicsBody * FindBody(int alID, tLoaderPhysicsBodyMap &a_setBodies)
	{
		tLoaderPhysicsBodyMapIt it = a_setBodies.find(alID);
		if(it == a_setBodies.end()) return NULL;
        
		return it->second;
	}

	static ePhysicsJointType ToJointType(const tString& asType)
	{
		tString sLowType = cString::ToLowerCase(asType);

        if(sLowType == "jointhinge")	return ePhysicsJointType_Hinge;
		if(sLowType == "jointball")		return ePhysicsJointType_Ball;
		if(sLowType == "jointslider")	return ePhysicsJointType_Slider;
		if(sLowType == "joinscrew")		return ePhysicsJointType_Screw;

		Error("Joint type '%s' does not exist!\n", asType.c_str());

		return ePhysicsJointType_Ball;
	}

	static iPhysicsJoint* CreateJoint(	const tString& asEntityName, 
										cXmlElement *apJointElem, iPhysicsWorld *apPhysicsWorld,
										tLoaderPhysicsBodyMap &a_setBodies,
										const cMatrixf& a_mtxTransform,
										const cVector3f& avScale)
	{
		
		/////////////////////////////
		//Get pin direction and pivot and transform according to entity
		cVector3f vPivot = apJointElem->GetAttributeVector3f("WorldPos") * avScale;
		cVector3f vPinDir = apJointElem->GetAttributeVector3f("PinDir");

		vPivot = cMath::MatrixMul(a_mtxTransform, vPivot);
		vPinDir = cMath::MatrixMul3x3(a_mtxTransform, vPinDir);
		
		/////////////////////////////
		//Name and type
		tString sJointName = asEntityName + "_" + apJointElem->GetAttributeString("Name");

		ePhysicsJointType jointType = ToJointType(apJointElem->GetValue());


		/////////////////////////////
		//Get the bodies
		int lParentID = apJointElem->GetAttributeInt("ConnectedParentBodyID");
		int lChildID = apJointElem->GetAttributeInt("ConnectedChildBodyID");

		iPhysicsBody *pParentBody = lParentID > 0 ? FindBody(lParentID,a_setBodies) : NULL;
		iPhysicsBody *pChildBody = FindBody(lChildID,a_setBodies);
		
		if(pChildBody==NULL)
		{
			Error("Could not find child body with ID %d for joint '%s'\n", lChildID, sJointName.c_str());
			return NULL;
		}
		
		///////////////////////////
		// Hinge
		if(jointType == ePhysicsJointType_Hinge)
		{
			iPhysicsJointHinge *pJoint = apPhysicsWorld->CreateJointHinge(sJointName,vPivot,vPinDir,pParentBody,pChildBody);

			pJoint->SetMinAngle(cMath::ToRad(apJointElem->GetAttributeFloat("MinAngle")));
			pJoint->SetMaxAngle(cMath::ToRad(apJointElem->GetAttributeFloat("MaxAngle")));

			return pJoint;
		}
		///////////////////////////
		// Ball
		else if(jointType == ePhysicsJointType_Ball)
		{
			iPhysicsJointBall *pJoint = apPhysicsWorld->CreateJointBall(sJointName,vPivot,vPinDir,pParentBody,pChildBody);

			pJoint->SetConeLimits(	cMath::ToRad(apJointElem->GetAttributeFloat("MaxConeAngle")),
									cMath::ToRad(apJointElem->GetAttributeFloat("MaxTwistAngle")));

			return pJoint;
		}
		///////////////////////////
		// Slider
		else if(jointType == ePhysicsJointType_Slider)
		{
			iPhysicsJointSlider *pJoint = apPhysicsWorld->CreateJointSlider(sJointName, vPivot,vPinDir,pParentBody,pChildBody);

			pJoint->SetMinDistance(apJointElem->GetAttributeFloat("MinDistance"));
			pJoint->SetMaxDistance(apJointElem->GetAttributeFloat("MaxDistance"));

			return pJoint;
		}
		///////////////////////////
		// Screw
		else if(jointType == ePhysicsJointType_Screw)
		{
			iPhysicsJointScrew *pJoint = apPhysicsWorld->CreateJointScrew(sJointName,vPivot,vPinDir,pParentBody,pChildBody);

			pJoint->SetMinDistance(apJointElem->GetAttributeFloat("MinDistance"));
			pJoint->SetMaxDistance(apJointElem->GetAttributeFloat("MaxDistance"));

			return pJoint;
		}


		return NULL;	
	}

	
	//-----------------------------------------------------------------------

	static cMatrixf GetMatrixFromVectors(const cVector3f& avPos, const cVector3f& avRot, const cVector3f& avScale)
	{
		cMatrixf mtxOut = cMath::MatrixScale(avScale);
		mtxOut = cMath::MatrixMul(cMath::MatrixRotate(avRot, eEulerRotationOrder_XYZ), mtxOut);
		mtxOut.SetTranslation(avPos);

		return mtxOut;
	}


	//-----------------------------------------------------------------------

	eAnimationEventType ToAnimEventType(const tString& asType)
	{
		tString sLowType = cString::ToLowerCase(asType);

        if(sLowType == "playsound")	return eAnimationEventType_PlaySound;
		if(sLowType == "step")		return eAnimationEventType_Step;

		Error("No animation event named '%s'\n", asType.c_str());
		return eAnimationEventType_LastEnum;
	}
	
	//-----------------------------------------------------------------------


	iEntity3D* cEntityLoader_Object::Load(	const tString &asName, int alID, bool abActive, cXmlElement *apRootElem, 
											const cMatrixf &a_mtxTransform, const cVector3f &avScale, 
											cWorld *apWorld, const tString &asFileName, const tWString &asFullPath, cResourceVarsObject *apInstanceVars)
	{
		////////////////////////////////////////	
		// Init
		mvBodies.clear();
		mvJoints.clear();

		mvBodyExtraData.clear();

		mvHapticShapes.clear();
		
		mvParticleSystems.clear();
		mvBillboards.clear();
		mvSoundEntities.clear();
		mvLights.clear();
		mvBeams.clear();

		tEntity3DList lstEntities;
		
		mpEntity = NULL;
		mpMesh = NULL;

		msFileName = asFileName; 
		mlID = alID;
		mbActive = abActive;
		mvScale = avScale;

		mbNodeAnimation = false;

		iPhysicsWorld *pPhysicsWorld = apWorld->GetPhysicsWorld();

		////////////////////////////////////////	
		// Load ModelData
		cXmlElement* pModelDataElem = apRootElem->GetFirstElement("ModelData");
		if(pModelDataElem==NULL){
			Error("Couldn't load element ModelData"); return NULL;
		}

		//msName = cString::ToString(pModelDataElem->Attribute("Name"),"");
		//msSubType = cString::ToString(pModelDataElem->Attribute("Subtype"),"");

		//////////////////////////////
		// Before load virtual call.
		BeforeLoad(apRootElem,a_mtxTransform,apWorld,apInstanceVars);


		////////////////////////////////////////	
		// Load Mesh and create entity
		cXmlElement *pMeshElem =NULL;
		{
			pMeshElem  = pModelDataElem->GetFirstElement("Mesh");
			if(pMeshElem==NULL){
				Error("Couldn't load element Mesh"); return NULL;
			}

			// Create mesh
			tString sMeshFile = pMeshElem->GetAttributeString("Filename");
			if(cString::GetFilePath(sMeshFile).size() < 1)
			{
				sMeshFile = cString::SetFilePath(sMeshFile, cString::To8Char(cString::GetFilePathW(asFullPath) ) );
			}
			//Log("Mesh: '%s'\n",sMeshFile.c_str());
			mpMesh = apWorld->GetResources()->GetMeshManager()->CreateMesh(sMeshFile);
			if(mpMesh==NULL) return NULL;

			//Create entity
			mpEntity = apWorld->CreateMeshEntity(asName, mpMesh, mbLoadAsStatic);
			
			if(mpMesh->GetSkeleton()!=NULL)
				mpEntity->SetMatrix(cMath::MatrixScale(mvScale));

			//Set entity properties
			//TODO...
			mpEntity->SetRenderFlagBit(eRenderableFlag_ShadowCaster,true); //<- Temp
		}

		////////////////////////////////////////	
		// Load sub meshes
		{
			bool bHasSkeleton = mpMesh->GetSkeleton()!=NULL;
			cXmlNodeListIterator submeshIt = pMeshElem->GetChildIterator();
			while(submeshIt.HasNext())
			{
				cXmlElement *pSubMeshElem = submeshIt.Next()->ToElement();

				//////////////////////////
				// Load the sub entity
				tString sName = pSubMeshElem->GetAttributeString("Name");
				//tString sMaterialFile = cString::ToString(pSubMeshElem->Attribute("MaterialFile"),"");

				cSubMeshEntity *pSubEntity = mpEntity->GetSubMeshEntityName(sName);
				if(pSubEntity==NULL)
				{
					Warning("Sub mesh '%s' does not exist in mesh '%s'!\n",sName.c_str(), mpMesh->GetName().c_str());
					continue;
				}
				if(bHasSkeleton==false)
					lstEntities.push_back(pSubEntity);

				
				//////////////////////////
				// Get transform matrix
				if(bHasSkeleton==false)
				{
					cMatrixf mtxLocalTransform = GetMatrixFromVectors(	pSubMeshElem->GetAttributeVector3f("WorldPos")*mvScale,
																		pSubMeshElem->GetAttributeVector3f("Rotation"),
																		pSubMeshElem->GetAttributeVector3f("Scale")*mvScale);
					
					//mtxLocalTransform = cMath::MatrixMul(mtxLocalTransform, cMath::MatrixScale(mvScale));

					pSubEntity->SetWorldMatrix(mtxLocalTransform);
				}
				
				//////////////////////////
				// Set the variables
				int lID = pSubMeshElem->GetAttributeInt("ID",-1);
				if(lID < 0) lID = pSubMeshElem->GetAttributeInt("SubMeshID"); //To support older files!
				
				pSubEntity->SetUniqueID(lID);


				//////////////////////////
				// Set material
				//TODO:
				/*if(sMaterialFile != "")
				{
				cMaterial *pMaterial = apWorld->GetResources()->GetMaterialManager()->CreateMaterial(sMaterialFile);
				if(pMaterial)
				{
				pSubEntity->SetCustomMaterial(pMaterial);
				}
				}*/
			}
		}

		////////////////////////////////////////	
		// Animations
		cXmlElement *pAnimationsElem  = pModelDataElem->GetFirstElement("Animations");
		if(mbLoadAnimations && mpEntity && pAnimationsElem)
		{
			cXmlNodeListIterator animElemIt = pAnimationsElem->GetChildIterator();
			while(animElemIt.HasNext())
			{
				cXmlElement *pAnimElem = animElemIt.Next()->ToElement();
				
				tString sFile = pAnimElem->GetAttributeString("File");
				tString sName = pAnimElem->GetAttributeString("Name");
				float fSpeed = pAnimElem->GetAttributeFloat("Speed",1.0f);
				float fSpecialEventTime = pAnimElem->GetAttributeFloat("SpecialEventTime",0.0f);
				
				if(cString::GetFilePath(sFile).length() <= 1)
					sFile = cString::SetFilePath(sFile, cString::To8Char(cString::GetFilePathW(asFullPath) ) );

				cAnimation *pAnim = apWorld->GetResources()->GetAnimationManager()->CreateAnimation(sFile);
				
				if(pAnim)
				{
					cAnimationState *pState = mpEntity->AddAnimation(pAnim, sName,fSpeed);
					pState->SetSpecialEventTime(fSpecialEventTime);

					///////////////////////////////
					// Load events
					cXmlNodeListIterator eventElemIt = pAnimElem->GetChildIterator();
					while(eventElemIt.HasNext())
					{
						cXmlElement *pEventElem = eventElemIt.Next()->ToElement();

                        cAnimationEvent *pEvent = pState->CreateEvent();
						pEvent->mfTime = pEventElem->GetAttributeFloat("Time");
						pEvent->mType = ToAnimEventType(pEventElem->GetAttributeString("Type"));
						pEvent->msValue = pEventElem->GetAttributeString("Value");
					}

				}
			}
		}


		////////////////////////////////////////	
		// Load World entities
		{
			//List that contain light and billboard connections
			tEFL_LightBillboardConnectionList lstLightBillboardListConnections;

            cXmlElement *pEntitiesElem  = pModelDataElem->GetFirstElement("Entities");
			if(pEntitiesElem)
			{
				////////////////////////
				//Iterate entities
				cXmlNodeListIterator entityIt = pEntitiesElem->GetChildIterator();
				while(entityIt.HasNext())
				{
					cXmlElement *pEntityElem = entityIt.Next()->ToElement();
					const tString& sEntityType = pEntityElem->GetValue();
					iEntity3D *pEntity = NULL;

					/////////////////////////
					// Particle System
					if(sEntityType == "ParticleSystem")
					{
						if(mbLoadParticleSystems)
						{
							cParticleSystem *pPS = cEngineFileLoading::LoadParticleSystem(pEntityElem,asName +"_", apWorld);
							if(pPS)	mvParticleSystems.push_back(pPS);
							pEntity = pPS;
						}
					}
					/////////////////////////
					// Billboard
					else if(sEntityType == "Billboard")
					{
						if(mbLoadBillboards)
						{
							cBillboard *pBillboard = cEngineFileLoading::LoadBillboard(pEntityElem,asName +"_", apWorld, apWorld->GetResources(), mbLoadAsStatic,
																						&lstLightBillboardListConnections);
							if(pBillboard)	mvBillboards.push_back(pBillboard);
							pEntity = pBillboard;
						}
					}
					/////////////////////////
					// Sound
					else if(sEntityType == "Sound")
					{
						if(mbLoadSounds)
						{
							cSoundEntity *pSound = cEngineFileLoading::LoadSound(pEntityElem,asName +"_", apWorld);
							if(pSound)	mvSoundEntities.push_back(pSound);
							pEntity = pSound;
						}
					}
                    /////////////////////////
					// Light
					else if(cString::GetLastStringPos(sEntityType,"Light")>0)
					{
						if(mbLoadLights)
						{
							iLight *pLight = cEngineFileLoading::LoadLight(pEntityElem,asName +"_", apWorld, apWorld->GetResources(),mbLoadAsStatic);
							if(pLight)	mvLights.push_back(pLight);
							pEntity = pLight;
						}
					}
					/////////////////////////
					// Unknown
					else
					{
						Error("Entity world entity type '%s' is unknown!\n", sEntityType.c_str());
					}

					/////////////////////////
					// Add to list and scale!
					if(pEntity)
					{
						//Scale the local position accoringly!
						cVector3f vPos = pEntity->GetLocalPosition();
						pEntity->SetPosition(vPos * mvScale);
						
						lstEntities.push_back(pEntity);
					}
				}
			}

			/////////////////////////////////////
			//Set up light and billboard connections
			tEFL_LightBillboardConnectionListIt connIt = lstLightBillboardListConnections.begin();
			for(; connIt != lstLightBillboardListConnections.end(); ++connIt)
			{
				cEFL_LightBillboardConnection& lightConnect = *connIt;

				cBillboard *pBB = GetBillboardFromID(lightConnect.msBillboardID);
				iLight *pLight = GetLightFromName(lightConnect.msLightName);

				if(pLight==NULL)
				{
					Warning("Light with name '%s' does not exist!",lightConnect.msLightName.c_str());
					continue;
				}
				if(pBB==NULL)
				{
					Warning("Billboard with id '%d' does not exist!",lightConnect.msBillboardID);
					continue;
				}

				pLight->AttachBillboard(pBB, pBB->GetColor());
			}
		}

		////////////////////////////////////////	
		// Copy entity list to other list
		tEntity3DList lstTempEntities = lstEntities;


		////////////////////////////////////////	
		// Load Bones
		tNodeStateMap mapBoneStates;
		if(mpMesh->GetSkeleton())
		{
			cXmlElement *pBonesElem  = pModelDataElem->GetFirstElement("Bones");
			if(pBonesElem)
			{
				//////////////////////////
				// Iterate bones
				cXmlNodeListIterator boneIt = pBonesElem->GetChildIterator();
				while(boneIt.HasNext())
				{
					cXmlElement *pBoneElem = boneIt.Next()->ToElement();

					int lID = pBoneElem->GetAttributeInt("ID");
					tString sName = pBoneElem->GetAttributeString("Name");

					cBoneState *pBoneState = mpEntity->GetBoneStateFromName(sName);
					if(pBoneState==NULL){
						Error("Could not find bone '%s' in model '%s'\n", sName.c_str(), cString::To8Char(asFullPath).c_str());
						continue;
					}

					/////////////////////////////
					//Add bones to list
                    mapBoneStates.insert(tNodeStateMap::value_type(lID, pBoneState));
					
					/////////////////////////////
					//Add children to bone
					LoadAndAttachChildren(pBoneElem, NULL, pBoneState, lstTempEntities, mapBoneStates, true, false);
				}
			}
		}
		


		////////////////////////////////////////	
		// Load Shapes
		tLoaderCollideShapeMap setShapes;

		if(pPhysicsWorld)
		{
			cXmlElement *pShapesElem  = pModelDataElem->GetFirstElement("Shapes");
			if(pShapesElem)
			{
				cXmlNodeListIterator shapeIt = pShapesElem->GetChildIterator();
				while(shapeIt.HasNext())
				{
					cXmlElement *pBodyShapeElem = shapeIt.Next()->ToElement();

					iCollideShape *pCollideShape = CreateCollideShape(pBodyShapeElem, pPhysicsWorld, mvScale);
					if(pCollideShape == NULL) continue;
					int lID = pBodyShapeElem->GetAttributeInt("ID");

					setShapes.insert(tLoaderCollideShapeMap::value_type(lID, pCollideShape));
				}
			}
		}
			

		////////////////////////////////////////	
		// Load Bodies
		tLoaderPhysicsBodyMap setBodies;

		if(pPhysicsWorld)
		{
			////////////////////////
			//Iterate the bodies elements
			cXmlElement *pBodiesElem  = pModelDataElem->GetFirstElement("Bodies");
			if(pBodiesElem)
			{
				cXmlNodeListIterator bodyIt = pBodiesElem->GetChildIterator();
				while(bodyIt.HasNext())
				{
					/////////////////////
					// Init
					cXmlElement *pBodyElem = bodyIt.Next()->ToElement();
					int lID = pBodyElem->GetAttributeInt("ID");
					tString sBodyName = pBodyElem->GetAttributeString("Name");

					/////////////////////
					// Get shape
					iCollideShape *pShape = GetBodyShape(pBodyElem,pPhysicsWorld,setShapes);
					if(pShape==NULL){
						Error("No shapes found for body '%s'\n", sBodyName.c_str());
						continue;
					}

					/////////////////////
					// Create body and set up properties
					iPhysicsBody *pBody = pPhysicsWorld->CreateBody(asName +"_"+ sBodyName,pShape);
					SetBodyProperties(pBody, pBodyElem);

					//Material
					iPhysicsMaterial *pPhysicsMat = pPhysicsWorld->GetMaterialFromName(pBodyElem->GetAttributeString("Material"));
					if(pPhysicsMat) pBody->SetMaterial(pPhysicsMat);

					setBodies.insert(tLoaderPhysicsBodyMap::value_type(lID, pBody));
					mvBodies.push_back(pBody);

					/////////////////////
					// Add extra properties
					size_t lIdx = mvBodies.size() -1;
					mvBodyExtraData.push_back(cEntityBodyExtraData());

					mvBodyExtraData[lIdx].m_mtxLocalTransform = pBody->GetLocalMatrix();

					/////////////////////
					// Attach children
					LoadAndAttachChildren(pBodyElem, pBody, NULL, lstTempEntities, mapBoneStates, true, true);
				}
			}

			////////////////////////
			//If no bones got attached to bodies, then add the entire entity to be attached. (Luis: this comment does not make much sense given the code below, not sure what the point is)
			if(mpMesh->GetSkeleton())
			{
				if(mapBoneStates.size() != mpEntity->GetBoneStateNum())
				{
					Error("Loading entity %s: Skeletons in mesh file (%ls) and .ent file (%ls) differ! Probably caused by .ent not being up to date with mesh\n", 
						asName.c_str(), 
						mpMesh->GetFullPath().c_str(), 
						static_cast<iXmlDocument*>(apRootElem)->GetPath().c_str());
				}
				else
					lstTempEntities.push_back(mpEntity);
			}

			////////////////////////
			//Destroy any left over shapes
			tLoaderCollideShapeMapIt shapeSetIt = setShapes.begin();
			for(; shapeSetIt != setShapes.end(); ++shapeSetIt)
			{
				pPhysicsWorld->DestroyShape(shapeSetIt->second);
			}
		}
		
		////////////////////////////////////////	
		// Add all remaining entities directly to first body
		if(mvBodies.empty()==false)
		{
			iPhysicsBody *pMainBody = mvBodies[0];
			cMatrixf mtxInvParent = cMath::MatrixInverse(pMainBody->GetLocalMatrix());

			for(tEntity3DListIt it = lstTempEntities.begin(); it != lstTempEntities.end(); ++it)
			{
				iEntity3D *pEntity = *it;

				if(pEntity->GetEntityType() == "SubMesh" && mpMesh->GetSkeleton() == false && mpMesh->GetAnimationNum() > 0)
				{
					mbNodeAnimation = true;
					continue;
				}
				else
				{
					AttachEntityChild(pMainBody, mtxInvParent, pEntity);
				}
			}

			if(mbNodeAnimation)
			{
				/////////////
				// Attach the whole entity to the body
				AttachEntityChild(pMainBody, mtxInvParent, mpEntity);
			}
		}


		////////////////////////////////////////	
		// Set matrix on entity if there are no bodies.
		if(mvBodies.size()<=0)
		{
			if(mpMesh->GetSkeleton())
				mpEntity->SetMatrix(cMath::MatrixMul(a_mtxTransform, cMath::MatrixScale(avScale)) );
			else
				mpEntity->SetMatrix(a_mtxTransform);
			
			//to make sure everything is in place.
			mpEntity->UpdateLogic(0);
		}
		else
		{
			for(size_t i=0;i<mvBodies.size(); ++i)
			{
				iPhysicsBody *pBody = mvBodies[i];
				
				pBody->SetMatrix(cMath::MatrixMul(a_mtxTransform,pBody->GetLocalMatrix()));
			}
		}

		if(pPhysicsWorld)
		////////////////////////////////////////	
		// Load Joints
		{
			cXmlElement *JointsElem  = pModelDataElem->GetFirstElement("Joints");
			if(JointsElem)
			{
				cXmlNodeListIterator jointIt = JointsElem->GetChildIterator();
				while(jointIt.HasNext())
				{
					cXmlElement *pJointElem = jointIt.Next()->ToElement();


					iPhysicsJoint *pJoint = CreateJoint(asName,pJointElem,pPhysicsWorld,setBodies, a_mtxTransform, mvScale);
					if(pJoint)
					{
						SetJointProperties(pJoint,pJointElem, apWorld);

						mvJoints.push_back(pJoint);
					}
				}
			}
		}

		////////////////////////////////////////	
		// Reset the unique ID (will mess with other entities otherwise)
		// Do not want to do this with bodies!
		for(tEntity3DListIt it = lstEntities.begin(); it != lstEntities.end(); ++it)
		{
			iEntity3D *pEntity = *it;
			pEntity->SetUniqueID(-1);
		}


		////////////////////////////////////////	
		// Final setup, start animation or go to ragdoll mode is possible
		if(mpEntity->GetAnimationStateNum() >0)
		{
			mpEntity->Play(0, true, true);
		}
		else if(mpEntity->GetMesh()->GetSkeleton() && mvBodies.size() >0)
		{
			mpEntity->SetSkeletonPhysicsActive(true);
		}
		
		////////////////////////////////////////	
		// Load user variables();
		LoadUserVariables(apRootElem);
		
		// After load virtual call.
		// This is where the user adds extra stuff.
		AfterLoad(apRootElem,a_mtxTransform,apWorld,apInstanceVars);

		return mpEntity;
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cEntityLoader_Object::AttachEntityChild(iEntity3D *apParent, const cMatrixf& a_mtxInvParent, iEntity3D *apChild)
	{
		//Calculate local matrix
		cMatrixf mtxLocalTransform = cMath::MatrixMul(a_mtxInvParent, apChild->GetWorldMatrix());
		apChild->SetMatrix(mtxLocalTransform);

		//Add the entity to parent and remove it from any other hierarchy it might be in.
		if(apChild->GetEntityParent()) 
			apChild->GetEntityParent()->RemoveChild(apChild);
		apParent->AddChild(apChild);
	}

	//-----------------------------------------------------------------------

	void cEntityLoader_Object::AttachBoneChild(cBoneState *apBoneState, const cMatrixf& a_mtxInvParent, iEntity3D *apChild)
	{
		//Calculate local matrix
		cMatrixf mtxLocalTransform = cMath::MatrixMul(a_mtxInvParent, apChild->GetWorldMatrix());

		apChild->SetMatrix(mtxLocalTransform);

		//Add the entity to parent and remove it from any other hierarchy it might be in.
		if(apChild->GetParent()) 
			apChild->GetParent()->RemoveEntity(apChild);
		apBoneState->AddEntity(apChild);	
	}

	//-----------------------------------------------------------------------

	void cEntityLoader_Object::AttachBoneToBody(iPhysicsBody *apParentBody, const cMatrixf& a_mtxInvParent, cBoneState *apBoneState)
	{
		cMatrixf mtxInvBone = cMath::MatrixInverse(apBoneState->GetWorldMatrix());
		cMatrixf mtxBoneToBody = cMath::MatrixMul(mtxInvBone, apParentBody->GetWorldMatrix());

		//Attaching work differently for bones since they rely on base matrix and animations.
		//This sets up the basic properties and then the skeletal physics needs to be turned on the mesh entity.
		apBoneState->SetBody(apParentBody);
		apBoneState->SetBodyMatrix(mtxBoneToBody);

		apParentBody->SetIsRagDoll(true);
		//apParentBody->SetCollideRagDoll(false);

		/////////////////////////////////////
		//Create collider body
		/*
		iPhysicsBody *pColliderBody = mpPhysicsWorld->CreateBody(apEntity->GetName()+"_collider_"+pBoneState->GetName(),pShape);
		pColliderBody->SetMass(0);
		pColliderBody->SetActive(false);
		pColliderBody->SetCollideCharacter(false);

		pBoneState->SetColliderBody(pColliderBody);
		*/
	}
	
	//-----------------------------------------------------------------------
	
	void cEntityLoader_Object::LoadAndAttachChildren(	cXmlElement *apMainElem, iEntity3D *apEntityParent, cBoneState *apBoneStateParent, 
														tEntity3DList& a_lstChildList, tNodeStateMap &a_mapBoneStates,
														bool abRemoveAttachedChild, bool abIsBody)
	{
		cXmlElement *pChildrenElem  = apMainElem->GetFirstElement("Children");
		if(pChildrenElem==NULL) return;

		cMatrixf mtxInvParent;
		if(apEntityParent)
			mtxInvParent = cMath::MatrixInverse(apEntityParent->GetLocalMatrix());
		else
			mtxInvParent = cMath::MatrixInverse(apBoneStateParent->GetWorldMatrix());

		///////////////////////////////
		//Iterate the children elements
		cXmlNodeListIterator childIt = pChildrenElem->GetChildIterator();
		while(childIt.HasNext())
		{
			cXmlElement *pChildElem = childIt.Next()->ToElement();

			int lID = pChildElem->GetAttributeInt("ID");
			
			//////////////////////////////////
			// Search for child entity
			bool bFound = false;
			for(tEntity3DListIt it = a_lstChildList.begin(); it != a_lstChildList.end(); ++it)
			{
				//////////////////////////
				//If found attach and remove from list if set
				iEntity3D *pEntity = *it;
				if(pEntity->GetUniqueID() != lID) continue;

				//Attach
				if(apEntityParent)
				{
					if(pEntity->GetEntityType() == "SubMesh" && mpMesh->GetSkeleton() == false && mpMesh->GetAnimationNum() > 0)
					{
						mbNodeAnimation = true;
						continue;
					}
					else
					{
						AttachEntityChild(apEntityParent, mtxInvParent, pEntity);
					}
				}
				else
					AttachBoneChild(apBoneStateParent, mtxInvParent, pEntity);
								
				//Final stuff				
				if(abRemoveAttachedChild) a_lstChildList.erase(it);
				bFound = true;
				break;
			}

			//////////////////////////////////
			// Search for child bone
			if(abIsBody && bFound==false)
			{
				tNodeStateMapIt it = a_mapBoneStates.find(lID);
				if(it != a_mapBoneStates.end())
				{
					iPhysicsBody *pParentBody = static_cast<iPhysicsBody*>(apEntityParent);

					cBoneState *pBoneState = it->second;
					AttachBoneToBody(pParentBody, mtxInvParent, pBoneState);
					bFound = true;

					a_mapBoneStates.erase(it);
				}
			}


			if(bFound == false)
			{
				if(apEntityParent)
					Warning("Could not find child with unique ID %d to attach to entity '%s' in '%s'\n", lID, apEntityParent->GetName().c_str(), msFileName.c_str());
				else
					Warning("Could not find child with unique ID %d to attach to bone '%s' in '%s'\n", lID, apBoneStateParent->GetName().c_str(), msFileName.c_str());
			}
		}
	}

	//-----------------------------------------------------------------------

	cBillboard* cEntityLoader_Object::GetBillboardFromID(int alID)
	{
        for(size_t i=0; i<mvBillboards.size(); ++i)
		{
			cBillboard *pBB = mvBillboards[i];
			if(pBB->GetUniqueID() == alID) return pBB;
		}
		return NULL;
	}
	
	iLight* cEntityLoader_Object::GetLightFromName(const tString& asName)
	{
		for(size_t i=0; i<mvLights.size(); ++i)
		{
			iLight *pLight = mvLights[i];
			if(pLight->GetName() == asName) return pLight;
		}
		return NULL;
	}

	//-----------------------------------------------------------------------

	void cEntityLoader_Object::SetBodyProperties(iPhysicsBody *apBody, cXmlElement *apElem)
	{
		apBody->SetMatrix(GetMatrixFromVectors(	apElem->GetAttributeVector3f("WorldPos") * mvScale,
												apElem->GetAttributeVector3f("Rotation"), 
												1.0f) 
												);

		apBody->SetMass(apElem->GetAttributeFloat("Mass",1.0f));
		
		apBody->SetAngularDamping(apElem->GetAttributeFloat("AngularDamping"));
		apBody->SetLinearDamping(apElem->GetAttributeFloat("LinearDamping"));

		apBody->SetBlocksSound(apElem->GetAttributeBool("BlocksSound",false));
		apBody->SetCollideCharacter(apElem->GetAttributeBool("CollideCharacter",true));
		apBody->SetCollide(apElem->GetAttributeBool("CollideNonCharacter",true));

		apBody->SetGravity(apElem->GetAttributeBool("HasGravity",true));
		apBody->SetBuoyancyDensityMul(apElem->GetAttributeFloat("BuoyancyDensityMul",1.0));

		apBody->SetMaxAngularSpeed(apElem->GetAttributeFloat("MaxAngularSpeed",0));
		apBody->SetMaxLinearSpeed(apElem->GetAttributeFloat("MaxLinearSpeed",0));

		apBody->SetContinuousCollision(apElem->GetAttributeBool("ContinuousCollision",true));

		apBody->SetPushedByCharacterGravity(apElem->GetAttributeBool("PushedByCharacterGravity",false));

		apBody->SetVolatile(apElem->GetAttributeBool("Volatile",false));

		apBody->SetUseSurfaceEffects(apElem->GetAttributeBool("UseSurfaceEffects",true));
	
		apBody->SetGravityCanAttachCharacter(apElem->GetAttributeBool("CanAttachCharacter",false));

		apBody->SetUniqueID(apElem->GetAttributeInt("ID",-1));
	}

	//-----------------------------------------------------------------------

	void cEntityLoader_Object::SetJointProperties(iPhysicsJoint *pJoint, cXmlElement *apJointElem, cWorld *apWorld)
	{
		tString t = apJointElem->GetAttributeString("MoveSound","");
		pJoint->SetMoveSound(t);
		pJoint->SetMinMoveSpeed(apJointElem->GetAttributeFloat("MinMoveSpeed",0.5f));
		pJoint->SetMinMoveFreq(apJointElem->GetAttributeFloat("MinMoveFreq",0.9f));
		pJoint->SetMinMoveVolume(apJointElem->GetAttributeFloat("MinMoveVolume",0.3f));
		pJoint->SetMinMoveFreqSpeed(apJointElem->GetAttributeFloat("MinMoveFreqSpeed",0.9f));
		pJoint->SetMaxMoveFreq(apJointElem->GetAttributeFloat("MaxMoveFreq",1.1f));
		pJoint->SetMaxMoveVolume(apJointElem->GetAttributeFloat("MaxMoveVolume",1.0f));
		pJoint->SetMaxMoveFreqSpeed(apJointElem->GetAttributeFloat("MaxMoveFreqSpeed",1.1f));
		pJoint->SetMiddleMoveSpeed(apJointElem->GetAttributeFloat("MiddleMoveSpeed",1.0f));
		pJoint->SetMiddleMoveVolume(apJointElem->GetAttributeFloat("MiddleMoveVolume",1.0f));
		pJoint->SetMoveSpeedType(cString::ToLowerCase(apJointElem->GetAttributeString("MoveType","Linear")) == "angular" ? 
									ePhysicsJointSpeed_Angular : 	ePhysicsJointSpeed_Linear);
		
		pJoint->SetStickyMinLimit(apJointElem->GetAttributeBool("StickyMinLimit",false));
		pJoint->SetStickyMaxLimit(apJointElem->GetAttributeBool("StickyMaxLimit",false));

		pJoint->SetBreakable(apJointElem->GetAttributeBool("Breakable",false));
		pJoint->SetBreakForce(apJointElem->GetAttributeFloat("BreakForce",1000));
		pJoint->SetBreakSound(apJointElem->GetAttributeString("BreakSound",""));

		pJoint->SetLimitAutoSleep(apJointElem->GetAttributeBool("LimitAutoSleep",false));
		pJoint->SetLimitAutoSleepDist(apJointElem->GetAttributeFloat("LimitAutoSleepDist",0.02f));
		pJoint->SetLimitAutoSleepNumSteps(apJointElem->GetAttributeInt("LimitAutoSleepNumSteps",10));
		
		pJoint->SetCollideBodies(apJointElem->GetAttributeBool("CollideBodies",true));
		
		pJoint->GetMaxLimit()->msSound = apJointElem->GetAttributeString("MaxLimitSound","");
		pJoint->GetMaxLimit()->mfMaxSpeed = apJointElem->GetAttributeFloat("MaxLimitMaxSpeed",10.0f);
		pJoint->GetMaxLimit()->mfMinSpeed = apJointElem->GetAttributeFloat("MaxLimit_MinSpeed",20.0f);
		if(pJoint->GetMaxLimit()->mfMaxSpeed <=0) pJoint->GetMaxLimit()->mfMaxSpeed = 0.01f;

		pJoint->GetMinLimit()->msSound = apJointElem->GetAttributeString("MinLimitSound","");
		pJoint->GetMinLimit()->mfMaxSpeed = apJointElem->GetAttributeFloat("MinLimitMaxSpeed",10.0f);
		pJoint->GetMinLimit()->mfMinSpeed = apJointElem->GetAttributeFloat("MinLimitMinSpeed",20.0f);
		if(pJoint->GetMinLimit()->mfMaxSpeed <=0) pJoint->GetMaxLimit()->mfMaxSpeed = 0.01f;

		pJoint->SetUniqueID(apJointElem->GetAttributeInt("ID",-1));


		/////////////////////////////
		//Load all controllers
		//TODO!
		/*TiXmlElement *pControllerElem = pJointElem->FirstChildElement("Controller");
		for(; pControllerElem != NULL; pControllerElem = pControllerElem->NextSiblingElement("Controller"))
		{
			LoadController(pJoint,apWorld->GetPhysicsWorld(),pControllerElem);
		}*/
	}

	//-----------------------------------------------------------------------
	
	ePhysicsControllerType GetControllerType(const char* apString)
	{
		if(apString == NULL) return ePhysicsControllerType_LastEnum;

		tString sName = apString;

		if(sName == "Pid") return ePhysicsControllerType_Pid;
		else if(sName == "Spring") return ePhysicsControllerType_Spring;
		
		return ePhysicsControllerType_LastEnum;
	}

	/////////////////////////

	static ePhysicsControllerInput GetControllerInput(const char* apString)
	{
		if(apString == NULL) return ePhysicsControllerInput_LastEnum;

		tString sName = apString;

		if(sName == "JointAngle") return ePhysicsControllerInput_JointAngle;
		else if(sName == "JointDist") return ePhysicsControllerInput_JointDist; 
		else if(sName == "LinearSpeed") return ePhysicsControllerInput_LinearSpeed;
		else if(sName == "AngularSpeed")  return ePhysicsControllerInput_AngularSpeed;
		
		return ePhysicsControllerInput_LastEnum;
	}

	/////////////////////////

	static ePhysicsControllerOutput GetControllerOutput(const char* apString)
	{
		if(apString == NULL) return ePhysicsControllerOutput_LastEnum;

		tString sName = apString;

		if(sName == "Force") return ePhysicsControllerOutput_Force;
		else if(sName == "Torque") return ePhysicsControllerOutput_Torque; 
		
		return ePhysicsControllerOutput_LastEnum;
	}

	/////////////////////////

	static ePhysicsControllerAxis GetControllerAxis(const char* apString)
	{
		if(apString == NULL) return ePhysicsControllerAxis_LastEnum;

		tString sName = apString;

		if(sName == "X") return ePhysicsControllerAxis_X;
		else if(sName == "Y") return ePhysicsControllerAxis_Y; 
		else if(sName == "Z") return ePhysicsControllerAxis_Z; 

		return ePhysicsControllerAxis_LastEnum;
	}

	/////////////////////////

	static ePhysicsControllerEnd GetControllerEnd(const char* apString)
	{
		if(apString == NULL) return ePhysicsControllerEnd_Null;

		tString sName = apString;

		if(sName == "OnMax") return ePhysicsControllerEnd_OnMax;
		else if(sName == "OnMin") return ePhysicsControllerEnd_OnMin; 
		else if(sName == "OnDest") return ePhysicsControllerEnd_OnDest; 

		return ePhysicsControllerEnd_Null;
	}

	/////////////////////////


	void cEntityLoader_Object::LoadController(iPhysicsJoint* apJoint,iPhysicsWorld *apPhysicsWorld, 
												TiXmlElement *apElem)
	{
		//////////////////////////////
		// Get the properties
		/*tString sName = cString::ToString(apElem->Attribute("Name"),"");
		bool bActive = cString::ToBool(apElem->Attribute("Active"),false);

		ePhysicsControllerType CtrlType = GetControllerType(apElem->Attribute("Type"));
		float fA= cString::ToFloat(apElem->Attribute("A"),0);
		float fB= cString::ToFloat(apElem->Attribute("B"),0);
		float fC= cString::ToFloat(apElem->Attribute("C"),0);
		int lIntegralSize = cString::ToInt(apElem->Attribute("IntegralSize"),1);

		ePhysicsControllerInput CtrlInput = GetControllerInput(apElem->Attribute("Input"));
		ePhysicsControllerAxis CtrlInputAxis = GetControllerAxis(apElem->Attribute("InputAxis"));
		float fDestValue = cString::ToFloat(apElem->Attribute("DestValue"),0);
		float fMaxOutput = cString::ToFloat(apElem->Attribute("MaxOutput"),0);

		ePhysicsControllerOutput CtrlOutput = GetControllerOutput(apElem->Attribute("Output"));
		ePhysicsControllerAxis CtrlOutputAxis = GetControllerAxis(apElem->Attribute("OutputAxis"));
		bool bMulMassWithOutput = cString::ToBool(apElem->Attribute("MulMassWithOutput"),false);

		ePhysicsControllerEnd CtrlEnd = GetControllerEnd(apElem->Attribute("EndType"));
		tString sNextCtrl = cString::ToString(apElem->Attribute("NextController"),"");
		
		bool bLogInfo = cString::ToBool(apElem->Attribute("LogInfo"),false);

		//Convert degrees to radians.
		if(CtrlInput == ePhysicsControllerInput_JointAngle) {
			fDestValue = cMath::ToRad(fDestValue);
		}

		//////////////////////////////
		// Create the controller
		iPhysicsController *pController =apPhysicsWorld->CreateController(sName);

		pController->SetType(CtrlType);

		pController->SetA(fA);
		pController->SetB(fB);
		pController->SetC(fC);

		pController->SetPidIntegralSize(lIntegralSize);

		pController->SetActive(bActive);
		pController->SetInputType(CtrlInput,CtrlInputAxis);
		pController->SetDestValue(fDestValue);
        
		pController->SetOutputType(CtrlOutput,CtrlOutputAxis);
		pController->SetMaxOutput(fMaxOutput);
		pController->SetMulMassWithOutput(bMulMassWithOutput);

		pController->SetEndType(CtrlEnd);
		pController->SetNextController(sNextCtrl);

		pController->SetLogInfo(bLogInfo);

		apJoint->AddController(pController);

		//Log("Controller: %s active: %d val: %f %f %f input: %d %d output: %d %d\n",
		//	sName.c_str(),bActive, fA,fB,fC, (int)CtrlInput, (int)CtrlInputAxis, (int)CtrlOutput,(int)CtrlOutputAxis);*/
	}

	//-----------------------------------------------------------------------

	eAnimationEventType cEntityLoader_Object::GetAnimationEventType(const char* apString)
	{
		if(apString == NULL) return eAnimationEventType_LastEnum;

        tString sName = apString;
		sName = cString::ToLowerCase(sName);

		if(sName == "playsound")
		{
			return eAnimationEventType_PlaySound;
		}

		Warning("Animation event type '%s' does not exist!\n",apString);
		return eAnimationEventType_LastEnum;
	}
	
	//-----------------------------------------------------------------------

	void cEntityLoader_Object::LoadUserVariables(cXmlElement *apRootElem)
	{
		cXmlElement *pVarRootElem = apRootElem->GetFirstElement("UserDefinedVariables");
		if(pVarRootElem==NULL){
			Warning("Can not find a use variable root element!\n");
			return;
		}

		msEntityType = pVarRootElem->GetAttributeString("EntityType");
		msEntitySubType = pVarRootElem->GetAttributeString("EntitySubType");

		LoadVariables(pVarRootElem);
	}
	
	//-----------------------------------------------------------------------
}
