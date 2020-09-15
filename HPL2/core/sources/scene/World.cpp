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

#include "scene/World.h"

#include "impl/tinyXML/tinyxml.h"

#include "system/Script.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"

#include "math/Math.h"
#include "math/MathTypes.h"

#include "engine/Engine.h"

#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/MeshCreator.h"
#include "graphics/VertexBuffer.h"


#include "resources/ParticleManager.h"
#include "resources/ScriptManager.h"
#include "resources/MaterialManager.h"
#include "resources/TextureManager.h"
#include "resources/EntFileManager.h"
#include "resources/FileSearcher.h"
#include "resources/Resources.h"
#include "resources/SoundEntityManager.h"
#include "resources/LowLevelResources.h"
#include "resources/XmlDocument.h"

#include "scene/Scene.h"
#include "scene/Node3D.h"
#include "scene/LightPoint.h"
#include "scene/LightSpot.h"
#include "scene/LightBox.h"
#include "scene/MeshEntity.h"
#include "scene/SoundEntity.h"
#include "scene/ParticleEmitter.h"
#include "scene/ParticleSystem.h"
#include "scene/BillBoard.h"
#include "scene/Beam.h"
#include "scene/GuiSetEntity.h"
#include "scene/RopeEntity.h"
#include "scene/FogArea.h"
#include "scene/RenderableContainer_List.h"
#include "scene/RenderableContainer_BoxTree.h"
#include "scene/RenderableContainer_DynBoxTree.h"
#include "scene/DummyRenderable.h"

#include "system/System.h"
#include "system/Platform.h"

#include "sound/SoundEntityData.h"
#include "sound/Sound.h"
#include "sound/SoundHandler.h"

#include "physics/Physics.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsJoint.h"

#include "ai/AI.h"
#include "ai/AINodeContainer.h"
#include "ai/AINodeGenerator.h"
#include "ai/AStar.h"

#include "haptic/Haptic.h"
#include "haptic/LowLevelHaptic.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWorld::cWorld(tString asName,cGraphics *apGraphics,cResources *apResources,cSound* apSound,
						cPhysics *apPhysics, cScene *apScene,cSystem *apSystem, cAI *apAI,
						cHaptic *apHaptic)
	{
		mpGraphics = apGraphics;
		mpResources = apResources;
		mpSound = apSound;
		mpPhysics = apPhysics;
		mpScene = apScene;
		mpSystem =apSystem;
		mpAI = apAI;
		mpHaptic = apHaptic;

		mpRootNode = hplNew( cNode3D, () );

		msName=asName;

		mbActive = true;

		mAmbientColor=cColor(0,0);

		mbIsSoundEmitter = false;

		mlSoundCreationIDCount =0;

		//TODO: Have the container type as param and create.
		mpRenderableContainer[eWorldContainerType_Static] = hplNew( cRenderableContainer_BoxTree, () );
		mpRenderableContainer[eWorldContainerType_Dynamic] = hplNew( cRenderableContainer_DynBoxTree, () );

		mpPhysicsWorld = NULL;
		mbAutoDeletePhysicsWorld = false;

		//////////////////////////////
		//Sky box
		mpSkyBoxVtxBuffer = mpGraphics->GetMeshCreator()->CreateSkyBoxVertexBuffer(1);
		mpSkyBoxTexture = NULL;
		mbAutoDestroySkybox = false;
		mbSkyBoxActive = false;
		mSkyBoxColor = cColor(1,1);

		//////////////////////////////
		//Fog
		mbFogActive = false;
		mfFogStart =0;
		mfFogEnd = 10;
		mfFogFalloffExp = 1;
		mFogColor = cColor(1,1);
		mbFogCulling = true;


		msFilePath = _W("");
	}

	//-----------------------------------------------------------------------

	cWorld::~cWorld()
	{
		if(mpSkyBoxVtxBuffer) hplDelete(mpSkyBoxVtxBuffer);
		if(mpSkyBoxTexture && mbAutoDestroySkybox)
		{
			mpResources->GetTextureManager()->Destroy(mpSkyBoxTexture);
		}

		for(tEntFileListIt it = mlstEntFileCache.begin(); it != mlstEntFileCache.end(); ++it)
		{
			mpResources->GetEntFileManager()->Destroy(*it);
		}

		DestroyAllEntities(0);
		
		for(int i=0; i<2; ++i)
		{
			if(mpRenderableContainer[i]) hplDelete(mpRenderableContainer[i]);
		}

		hplDelete(mpRootNode);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWorld::DestroyAllEntities(tWorldDestroyAllFlag aFlags)
	{
		if( (aFlags & eWorldDestroyAllFlag_SkipStaticEntities)==0)
			STLDeleteAll(mlstStaticMeshEntities);
		
		STLDeleteAll(mlstDynamicMeshEntities);
		STLDeleteAll(mlstLights);
		STLDeleteAll(mlstBillboards);
		STLDeleteAll(mlstBeams);
		STLDeleteAll(mlstParticleSystems);
		STLDeleteAll(mlstGuiSetEntities);
		STLDeleteAll(mlstRopeEntities);
		STLDeleteAll(mlstFogAreas);
		STLDeleteAll(mlstStartPosEntities);
		STLMapDeleteAll(m_mapAreaEntities);


		STLDeleteAll(mlstAINodeContainers);
		STLDeleteAll(mlstAStarHandlers);
		STLMapDeleteAll(m_mapTempNodes);

		if( (aFlags & eWorldDestroyAllFlag_SkipPhysics)==0)
		{
			if(mpPhysicsWorld && mbAutoDeletePhysicsWorld)
				mpPhysics->DestroyWorld(mpPhysicsWorld);
		}

		//So that bodies can stop sound entities on destruction.
		STLDeleteAll(mlstSoundEntities);

	}

	//-----------------------------------------------------------------------

	void cWorld::Update(float afTimeStep)
	{
		START_TIMING(Physics);
		if(mpPhysicsWorld) mpPhysicsWorld->Update(afTimeStep);
		STOP_TIMING(Physics);


		START_TIMING(Entities);
		UpdateEntities(afTimeStep);
		STOP_TIMING(Entities);
		
		START_TIMING(Particles);
		UpdateParticles(afTimeStep);
		STOP_TIMING(Particles);

		START_TIMING(Lights);
		UpdateLights(afTimeStep);
		STOP_TIMING(Lights);

		START_TIMING(SoundEntities);
		UpdateSoundEntities(afTimeStep);
		STOP_TIMING(SoundEntities);
	}

	//-----------------------------------------------------------------------

	void cWorld::PreUpdate(float afTotalTime, float afTimeStep)
	{
		mpSound->GetSoundHandler()->SetSilent(true);

		while(afTotalTime>0)
		{
			if(mpPhysicsWorld) mpPhysicsWorld->Update(afTimeStep);
			UpdateParticles(afTimeStep);

			afTotalTime -= afTimeStep;
		}

		mpSound->GetSoundHandler()->SetSilent(false);
	}

	//-----------------------------------------------------------------------

	iRenderableContainer* cWorld::GetRenderableContainer(eWorldContainerType aType)
	{
		return mpRenderableContainer[aType];
	}

	//-----------------------------------------------------------------------

	void cWorld::SetPhysicsWorld(iPhysicsWorld *apWorld, bool abAutoDelete)
	{
		mpPhysicsWorld = apWorld;
		mbAutoDeletePhysicsWorld = abAutoDelete;
		if(mpPhysicsWorld)
			mpPhysicsWorld->SetWorld(this);
	}

	iPhysicsWorld* cWorld::GetPhysicsWorld()
	{
		return mpPhysicsWorld;
	}

	//-----------------------------------------------------------------------

	static void CheckMinMaxUpdate(cVector3f &avMin,cVector3f &avMax,
							const cVector3f &avLocalMin,const cVector3f &avLocalMax)
	{
		if(avMin.x > avLocalMin.x) avMin.x = avLocalMin.x;
		if(avMax.x < avLocalMax.x) avMax.x = avLocalMax.x;

		if(avMin.y > avLocalMin.y) avMin.y = avLocalMin.y;
		if(avMax.y < avLocalMax.y) avMax.y = avLocalMax.y;

		if(avMin.z > avLocalMin.z) avMin.z = avLocalMin.z;
		if(avMax.z < avLocalMax.z) avMax.z = avLocalMax.z;
	}

	//-----------------------------------------------------------------------

	void cWorld::Compile(bool abCalcPhysicsWorldSize)
	{
		for(int i=0; i<2; ++i)
			if(mpRenderableContainer[i]) mpRenderableContainer[i]->Compile();

		if(mpPhysicsWorld && abCalcPhysicsWorldSize)
		{
			iRenderableContainerNode *pStaticRoot = mpRenderableContainer[eWorldContainerType_Static]->GetRoot();

			//Create a 10 m border around the world too
			cVector3f vMin = pStaticRoot->GetMin() - cVector3f(10,10,10);
			cVector3f vMax = pStaticRoot->GetMax() + cVector3f(10,10,10);

			mpPhysicsWorld->SetWorldSize(vMin, vMax);
		}
	}

	//-----------------------------------------------------------------------

	void cWorld::SetSkyBox(iTexture *apTexture, bool abAutoDestroy)
	{
		if(mpSkyBoxTexture && mbAutoDestroySkybox)
		{
			mpResources->GetTextureManager()->Destroy(mpSkyBoxTexture);
		}

		mbAutoDestroySkybox = abAutoDestroy;
		mpSkyBoxTexture = apTexture;
		if(mpSkyBoxTexture)
		{
			mpSkyBoxTexture->SetWrapS(eTextureWrap_ClampToEdge);
			mpSkyBoxTexture->SetWrapT(eTextureWrap_ClampToEdge);
		}
	}

	void cWorld::SetSkyBoxActive(bool abX)
	{
		mbSkyBoxActive = abX;
	}

	void cWorld::SetSkyBoxColor(const cColor& aColor)
	{
		if(mSkyBoxColor == aColor) return;

		mSkyBoxColor = aColor;

		float *pColors = mpSkyBoxVtxBuffer->GetFloatArray(eVertexBufferElement_Color0);
		int lNum = mpSkyBoxVtxBuffer->GetVertexNum();
		for(int i=0; i<lNum;++i)
		{
			pColors[0] = mSkyBoxColor.r;
			pColors[1] = mSkyBoxColor.g;
			pColors[2] = mSkyBoxColor.b;
			pColors[3] = mSkyBoxColor.a;
			pColors+=4;
		}

		mpSkyBoxVtxBuffer->UpdateData(eVertexElementFlag_Color0,false);
	}
	//-----------------------------------------------------------------------

	cAreaEntity* cWorld::CreateAreaEntity(const tString &asName)
	{
		cAreaEntity *pArea = hplNew( cAreaEntity, () );
		pArea->msName = asName;
		m_mapAreaEntities.insert(tAreaEntityMap::value_type(asName, pArea));
		return pArea;
	}

	cAreaEntity* cWorld::GetAreaEntity(const tString &asName)
	{
		tAreaEntityMapIt it = m_mapAreaEntities.find(asName);
		if(it== m_mapAreaEntities.end()) return NULL;

		return it->second;
	}

	//-----------------------------------------------------------------------

	iEntity3D* cWorld::CreateEntity(const tString& asName, const cMatrixf &a_mtxTransform,const tString& asFile, int alID, bool abActive,
									const cVector3f &avScale,
									cResourceVarsObject *apInstanceVars, bool abSkipNonStaticEntity)
	{
		iEntity3D *pEntity = NULL;
		
		cEntFile *pEntFile = mpResources->GetEntFileManager()->CreateEntFile(asFile);
		if(pEntFile==NULL) return NULL;

		mlstEntFileCache.push_back(pEntFile);
		
		tString sEntityType = "";
		iXmlDocument *pDoc = pEntFile->GetXmlDoc();

		//////////////////////////////////
		// Get Root element
		cXmlElement *pVarRootElem = pDoc->GetFirstElement("UserDefinedVariables");
		if(pVarRootElem==NULL){
			Warning("Can not find a UserDefinedVariables element in '%s'. Using default entity type\n", asFile.c_str());
		}
		else
		{
			sEntityType = pVarRootElem->GetAttributeString("EntityType");
		}

		
		//////////////////////////////////
		// Get Loader and load data
		iEntityLoader *pLoader = mpResources->GetEntityLoader(sEntityType);
		if(pLoader)
		{
			if(abSkipNonStaticEntity==false || pLoader->GetCreatesStaticEntity())
			{
				pEntity = pLoader->Load(asName,alID, abActive, pDoc,a_mtxTransform, avScale, this,pEntFile->GetName(),pEntFile->GetFullPath(), apInstanceVars);
				if(pEntity) pEntity->SetSourceFile(pEntFile->GetName());
			}
		}
		else
		{
			Error("Couldn't find loader for type '%s' in file '%s'\n",sEntityType.c_str(),pEntFile->GetName().c_str());
		}
		
		return pEntity;
	}

	//-----------------------------------------------------------------------

	cMeshEntity* cWorld::CreateMeshEntity(const tString &asName,cMesh *apMesh, bool abStatic)
	{
		cMeshEntity* pMeshEntity = hplNew( cMeshEntity, (asName,apMesh,mpResources->GetMaterialManager(),
														mpResources->GetMeshManager(), 
														mpResources->GetAnimationManager()) );

		//////////////////////////////
		//Put in entity list
		if(abStatic)
			mlstStaticMeshEntities.push_back(pMeshEntity);
		else
			mlstDynamicMeshEntities.push_back(pMeshEntity);


		//////////////////////////////
		// Add submeshes to renderable container
		for(int i=0; i<pMeshEntity->GetSubMeshEntityNum(); ++i) 
		{
			cSubMeshEntity *pSubEntity = pMeshEntity->GetSubMeshEntity(i);
			if(pSubEntity->GetSubMesh()->IsCollideShape()) continue; //Collide shapes are never rendered!
            
			pSubEntity->SetStatic(abStatic);
			AddRenderableToContainer(pSubEntity);
		}
		
		pMeshEntity->SetWorld(this);

		return pMeshEntity;
	}

	//-----------------------------------------------------------------------

	void cWorld::DestroyMeshEntity(cMeshEntity* apMesh)
	{
		if(apMesh==NULL) return;

		for(int i=0; i<apMesh->GetSubMeshEntityNum(); ++i) 
		{
			RemoveRenderableFromContainer(apMesh->GetSubMeshEntity(i));
		}

		if(apMesh->IsStatic())
			STLFindAndDelete(mlstStaticMeshEntities,apMesh);
		else
			STLFindAndDelete(mlstDynamicMeshEntities,apMesh);
	}

	//-----------------------------------------------------------------------

	cMeshEntity* cWorld::GetDynamicMeshEntity(const tString& asName)
	{
		 return (cMeshEntity*)STLFindByName(mlstDynamicMeshEntities, asName);
	}

	//-----------------------------------------------------------------------

	cMeshEntityIterator cWorld::GetDynamicMeshEntityIterator()
	{
		return cMeshEntityIterator(&mlstDynamicMeshEntities);
	}

	cMeshEntityIterator cWorld::GetStaticMeshEntityIterator()
	{
		return cMeshEntityIterator(&mlstStaticMeshEntities);
	}
	
	//-----------------------------------------------------------------------

	void cWorld::DrawMeshBoundingBoxes(const cColor &aColor, bool abStatic)
	{
		tMeshEntityListIt It=mlstDynamicMeshEntities.begin();
		for(;It != mlstDynamicMeshEntities.end();++It)
		{
			cMeshEntity *pEntity = *It;

			if(abStatic==false && pEntity->IsStatic()) continue;

			cBoundingVolume *pBV = pEntity->GetBoundingVolume();
			mpGraphics->GetLowLevel()->DrawBoxMinMax(pBV->GetMin(), pBV->GetMax(), aColor);
		}
	}

	//-----------------------------------------------------------------------


	cLightPoint* cWorld::CreateLightPoint(const tString &asName,const tString &asGobo,bool abStatic)
	{
		cLightPoint* pLight = hplNew( cLightPoint, (asName,mpResources) );
		mlstLights.push_back(pLight);

		if(asGobo != "")
		{
			iTexture *pTexture = mpResources->GetTextureManager()->CreateCubeMap(asGobo,true);
			if(pTexture!=NULL)
				pLight->SetGoboTexture(pTexture);
			else
				Warning("Couldn't load gobo texture '%s' for light '%s'",asGobo.c_str(), asName.c_str());
		}

		pLight->SetStatic(abStatic);
		AddRenderableToContainer(pLight);
		
		pLight->SetWorld(this);

		return pLight;
	}

	//-----------------------------------------------------------------------

	cLightSpot* cWorld::CreateLightSpot(const tString &asName, const tString &asGobo,
										bool abStatic)
	{
		cLightSpot* pLight = hplNew( cLightSpot, (asName,mpResources) );
		mlstLights.push_back(pLight);

		if(asGobo != "")
		{
			iTexture *pTexture = mpResources->GetTextureManager()->Create2D(asGobo,true);
			if(pTexture!=NULL)
				pLight->SetGoboTexture(pTexture);
			else
				Warning("Couldn't load gobo texture '%s' for light '%s'",asGobo.c_str(), asName.c_str());
		}

		pLight->SetStatic(abStatic);
		AddRenderableToContainer(pLight);
		
		pLight->SetWorld(this);
		
		return pLight;
	}

	//-----------------------------------------------------------------------

	cLightBox* cWorld::CreateLightBox(const tString &asName,bool abStatic)
	{
		cLightBox* pLight = hplNew( cLightBox, (asName,mpResources) );
		mlstLights.push_back(pLight);

		pLight->SetStatic(abStatic);
		AddRenderableToContainer(pLight);
		
		pLight->SetWorld(this);

		return pLight;
	}

	//-----------------------------------------------------------------------

	void cWorld::DestroyLight(iLight* apLight)
	{
		RemoveRenderableFromContainer(apLight);

		STLFindAndDelete(mlstLights, apLight);
	}

	//-----------------------------------------------------------------------

	iLight* cWorld::GetLight(const tString& asName)
	{
		tLightListIt LightIt=mlstLights.begin();
		for(;LightIt !=mlstLights.end();++LightIt)
		{
			if((*LightIt)->GetName() == asName){
				return *LightIt;
			}
		}
		return NULL;
	}

	iLight* cWorld::GetLightFromUniqueID(int alID)
	{
		tLightListIt LightIt=mlstLights.begin();
		for(;LightIt !=mlstLights.end();++LightIt)
		{
			if((*LightIt)->GetUniqueID() == alID){
				return *LightIt;
			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------

	cBillboard* cWorld::CreateBillboard(const tString& asName, const cVector2f& avSize,eBillboardType aType,
										const tString& asMaterial, bool abStatic)
	{
		cBillboard* pBillboard = hplNew( cBillboard, (asName, avSize,aType,mpResources,mpGraphics) );
		mlstBillboards.push_back(pBillboard);

		if(asMaterial!="")
		{
			cMaterial *pMat = mpResources->GetMaterialManager()->CreateMaterial(asMaterial);
			pBillboard->SetMaterial(pMat);
		}

		pBillboard->SetStatic(abStatic);
		AddRenderableToContainer(pBillboard);
		
		return pBillboard;
	}
	//-----------------------------------------------------------------------

	void cWorld::DestroyBillboard(cBillboard* apObject)
	{
		RemoveRenderableFromContainer(apObject);

		STLFindAndDelete(mlstBillboards, apObject);
	}

	//-----------------------------------------------------------------------

	cBillboard* cWorld::GetBillboard(const tString& asName)
	{
		return (cBillboard*)STLFindByName(mlstBillboards,asName);
	}

	cBillboard* cWorld::GetBillboardFromUniqueID(int alID)
	{
		tBillboardListIt BillboardIt=mlstBillboards.begin();
		for(;BillboardIt !=mlstBillboards.end();++BillboardIt)
		{
			if((*BillboardIt)->GetUniqueID() == alID){
				return *BillboardIt;
			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------

	cBillboardIterator cWorld::GetBillboardIterator()
	{
		return cBillboardIterator(&mlstBillboards);
	}

	//-----------------------------------------------------------------------


	cBeam* cWorld::CreateBeam(const tString& asName, bool abStatic)
	{
		cBeam* pBeam = hplNew( cBeam, (asName,mpResources,mpGraphics) );
		mlstBeams.push_back(pBeam);

		pBeam->SetStatic(abStatic);
		AddRenderableToContainer(pBeam);
			
		
		return pBeam;
	}
	//-----------------------------------------------------------------------

	void cWorld::DestroyBeam(cBeam* apObject)
	{
		RemoveRenderableFromContainer(apObject);

		STLFindAndDelete(mlstBeams, apObject);
	}

	//-----------------------------------------------------------------------

	cBeam* cWorld::GetBeam(const tString& asName)
	{
		return (cBeam*)STLFindByName(mlstBeams,asName);
	}

	cBeam* cWorld::GetBeamFromUniqueID(int alID)
	{
		for(tBeamListIt BeamIt=mlstBeams.begin();BeamIt !=mlstBeams.end();++BeamIt)
		{
			if((*BeamIt)->GetUniqueID() == alID) return *BeamIt;
		}
		return NULL;
	}

	//-----------------------------------------------------------------------

	cBeamIterator cWorld::GetBeamIterator()
	{
		return cBeamIterator(&mlstBeams);
	}


	//-----------------------------------------------------------------------

	cParticleSystem* cWorld::CreateParticleSystem(const tString& asName,const tString& asType,const cVector3f& avSize, bool abRemoveWhenDead)
	{
		cParticleSystem* pPS = mpResources->GetParticleManager()->CreatePS(asName,asType, avSize);
		if(pPS == NULL){
			Error("Couldn't create particle system '%s' of type '%s'\n",asName.c_str(), asType.c_str());
			return NULL;
		}

		//Log("Created particle system '%s' of type '%s'\n",asName.c_str(), asType.c_str());
		if(false)//asName== "candlestick02_1_ParticleSystem_1")
		{
			for(int i=0; i< pPS->GetEmitterNum();++i)
			{
				iParticleEmitter *pPE = pPS->GetEmitter(i);
				pPE->SetRenderFlagBit(eRenderableFlag_ContainerDebug, true);
			}
		}

		pPS->SetRemoveWhenDead(abRemoveWhenDead);

		//Add the emitters contained in the system.
		//Do not add the system itself.
		for(int i=0; i< pPS->GetEmitterNum();++i)
		{
			iParticleEmitter *pPE = pPS->GetEmitter(i);

			AddRenderableToContainer(pPE);

			pPE->SetWorld(this);
		}

		mlstParticleSystems.push_back(pPS);

		//Log("Created particle system '%s'\n",asType.c_str());

		return pPS;
	}

	//-----------------------------------------------------------------------

	cParticleSystem* cWorld::CreateParticleSystem(const tString& asName, const tString& asDataName, cXmlElement* apElement, const cVector3f& avSize)
	{
		cParticleSystem* pPS = mpResources->GetParticleManager()->CreatePS(asName,asDataName, apElement, avSize);
		if(pPS == NULL){
			Error("Couldn't create particle system '%s' of type '%s'\n",asName.c_str(), asDataName.c_str());
			return NULL;
		}

		//Log("Created particle system '%s' of type '%s'\n",asName.c_str(), asType.c_str());

		//Add the emitters contained in the system.
		//Do not add the system itself.
		for(int i=0; i< pPS->GetEmitterNum();++i)
		{
			iParticleEmitter *pPE = pPS->GetEmitter(i);

			AddRenderableToContainer(pPE);

			pPE->SetWorld(this);
		}

		mlstParticleSystems.push_back(pPS);

		//Log("Created particle system '%s'\n",asType.c_str());

		return pPS;
	}

	//-----------------------------------------------------------------------

	void cWorld::DestroyParticleSystem(cParticleSystem* apPS)
	{
		if(apPS==NULL)return;

		for(int i=0; i< apPS->GetEmitterNum();++i)
		{
			iParticleEmitter *pPE = apPS->GetEmitter(i);

			RemoveRenderableFromContainer(pPE);
		}

		STLFindAndDelete(mlstParticleSystems, apPS);
	}

	//-----------------------------------------------------------------------

	cParticleSystem* cWorld::GetParticleSystem(const tString& asName)
	{
		return (cParticleSystem*)STLFindByName(mlstParticleSystems,asName);
	}

	cParticleSystem* cWorld::GetParticleSystemFromUniqueID(int alID)
	{
		for(tParticleSystemListIt PSIt=mlstParticleSystems.begin();PSIt !=mlstParticleSystems.end();++PSIt)
		{
			if((*PSIt)->GetUniqueID() == alID) return *PSIt;
		}
		return NULL;
	}

	//-----------------------------------------------------------------------

	bool cWorld::ParticleSystemExists(cParticleSystem* apPS)
	{
		tParticleSystemListIt it = mlstParticleSystems.begin();
		for(; it != mlstParticleSystems.end(); ++it)
		{
			if(apPS == *it) return true;
		}
		return false;
	}

	void cWorld::DestroyAllParticleSystems()
	{
		tParticleSystemListIt it = mlstParticleSystems.begin();
		for(; it != mlstParticleSystems.end(); ++it)
		{
			cParticleSystem *pPS = *it;
			
			for(int i=0; i< pPS->GetEmitterNum();++i)
			{
				iParticleEmitter *pPE = pPS->GetEmitter(i);

				RemoveRenderableFromContainer(pPE);
			}
			hplDelete(pPS);
		}
		mlstParticleSystems.clear();
	}

	//-----------------------------------------------------------------------

	cGuiSetEntity* cWorld::CreateGuiSetEntity(const tString& asName, cGuiSet *apSet, bool abStatic)
	{
		cGuiSetEntity *pSetEntity = hplNew( cGuiSetEntity, (asName, apSet) );
		mlstGuiSetEntities.push_back(pSetEntity);

		pSetEntity->SetStatic(abStatic);
		AddRenderableToContainer(pSetEntity);
		
		return pSetEntity;
	}
	
	void cWorld::DestroyGuiSetEntity(cGuiSetEntity* apObject)
	{
		//TODO...RemoveRenderableFromContainer(...), etc

		STLFindAndDelete(mlstGuiSetEntities, apObject);
	}
	
	cGuiSetEntity* cWorld::GetGuiSetEntity(const tString& asName)
	{
		return static_cast<cGuiSetEntity*>(STLFindByName(mlstGuiSetEntities, asName));
	}

	cGuiSetEntity* cWorld::GetGuiSetEntityFromUniqueID(int alID)
	{
		for(tGuiSetEntityListIt it=mlstGuiSetEntities.begin();it !=mlstGuiSetEntities.end();++it)
		{
			if((*it)->GetUniqueID() == alID) return *it;
		}
		return NULL;
	}
	
	cGuiSetEntityIterator cWorld::GetGuiSetEntityIterator()
	{
		return cGuiSetEntityIterator(&mlstGuiSetEntities);
	}
	
	//-----------------------------------------------------------------------

	cRopeEntity* cWorld::CreateRopeEntity(const tString& asName, iPhysicsRope *apRope, int alMaxSegments)
	{
		cRopeEntity *pRope = hplNew( cRopeEntity, (asName, mpResources, mpGraphics, apRope, alMaxSegments));
		mlstRopeEntities.push_back(pRope);

		AddRenderableToContainer(pRope);
		
		return pRope;
	}

	void cWorld::DestroyRopeEntity(cRopeEntity* apRope)
	{
		RemoveRenderableFromContainer(apRope);

		STLFindAndDelete(mlstRopeEntities, apRope);
	}

	cRopeEntity* cWorld::GetRopeEntity(const tString& asName)
	{
		return static_cast<cRopeEntity*>(STLFindByName(mlstRopeEntities, asName));
	}

	cRopeEntity* cWorld::GetRopeEntityFromUniqueID(int alID)
	{
		for(tRopeEntityListIt it=mlstRopeEntities.begin();it !=mlstRopeEntities.end();++it)
		{
			if((*it)->GetUniqueID() == alID) return *it;
		}
		return NULL;
	}

	cRopeEntityIterator cWorld::GetRopeEntityIterator()
	{
		return cRopeEntityIterator(&mlstRopeEntities);
	}

	//-----------------------------------------------------------------------

	cFogArea* cWorld::CreateFogArea(const tString& asName, bool abStatic)
	{
		cFogArea *pFog = hplNew( cFogArea, (asName, mpResources));
		mlstFogAreas.push_back(pFog);
		pFog->SetStatic(abStatic);


		AddRenderableToContainer(pFog);

		return pFog;
	}

	void cWorld::DestroyFogArea(cFogArea* apRope)
	{
		RemoveRenderableFromContainer(apRope);

		STLFindAndDelete(mlstFogAreas, apRope);
	}

	cFogArea* cWorld::GetFogArea(const tString& asName)
	{
		return static_cast<cFogArea*>(STLFindByName(mlstFogAreas, asName));
	}

	cFogArea* cWorld::GetFogAreaFromUniqueID(int alID)
	{
		for(tFogAreaListIt it=mlstFogAreas.begin();it !=mlstFogAreas.end();++it)
		{
			if((*it)->GetUniqueID() == alID) return *it;
		}
		return NULL;
	}

	cFogAreaIterator cWorld::GetFogAreaIterator()
	{
		return cFogAreaIterator(&mlstFogAreas);
	}

	//-----------------------------------------------------------------------

	cSoundEntity* cWorld::CreateSoundEntity(const tString &asName,const tString &asSoundEntity,
											bool abRemoveWhenOver)
	{
		cSoundEntityData *pData = mpResources->GetSoundEntityManager()->CreateSoundEntity(asSoundEntity);
		if(pData==NULL){
			Error("Cannot find sound entity '%s'\n",asSoundEntity.c_str());
			return NULL;
		}

		cSoundEntity *pSound = hplNew( cSoundEntity, (asName,pData, mpResources->GetSoundEntityManager(), this, mpSound->GetSoundHandler(),
														abRemoveWhenOver,mlSoundCreationIDCount++));
		/*cSoundEntity *pSound = NULL;
		if(mlstSoundEntityPool.empty())
		{
			pSound = hplNew( cSoundEntity, (asName,pData,
											mpResources->GetSoundEntityManager(),
											this,
											mpSound->GetSoundHandler(),abRemoveWhenOver,mlSoundCreationIDCount++));
		}
		else
		{
			pSound = mlstSoundEntityPool.back();
			pSound->Setup(asName, pData, abRemoveWhenOver);
			mlstSoundEntityPool.pop_back();
		}*/

		mlstSoundEntities.push_back(pSound);

		return pSound;
	}

	void cWorld::DestroySoundEntity(cSoundEntity* apEntity)
	{
		//STLFindAndDelete(mlstSoundEntities,apEntity);
		
		//Only delete if found! (as it is possible that the sound entity is not longer a valid pointer!
		tSoundEntityListIt it= mlstSoundEntities.begin();
		for(; it != mlstSoundEntities.end(); ++it)
		{
			cSoundEntity *pSound = *it;
			if(pSound == apEntity)
			{
				mlstSoundEntities.erase(it);
				hplDelete(pSound);
				//mlstSoundEntityPool.push_back(apEntity);
				break;
			}
		}
	}

	void cWorld::DestroyAllSoundEntities()
	{
		//Make sure no body has any sound entity
		if(mpPhysicsWorld)
		{
			cPhysicsBodyIterator bodyIt = mpPhysicsWorld->GetBodyIterator();
			while(bodyIt.HasNext())
			{
				iPhysicsBody *pBody = bodyIt.Next();
				pBody->SetScrapeSoundEntity(NULL);
				pBody->SetRollSoundEntity(NULL);
			}

			cPhysicsJointIterator jointIt = mpPhysicsWorld->GetJointIterator();
			while(jointIt.HasNext())
			{
				iPhysicsJoint *pJoint = jointIt.Next();
				pJoint->SetSound(NULL);
			}
		}
		
		//Destroy all sound entities
		STLDeleteAll(mlstSoundEntities);
		mlstSoundEntities.clear();
	}


	cSoundEntity* cWorld::GetSoundEntity(const tString& asName)
	{
		return (cSoundEntity*)STLFindByName(mlstSoundEntities,asName);
	}

	cSoundEntity* cWorld::GetSoundEntityFromUniqueID(int alID)
	{
		for(tSoundEntityListIt it=mlstSoundEntities.begin();it !=mlstSoundEntities.end();++it)
		{
			if((*it)->GetUniqueID() == alID) return *it;
		}
		return NULL;
	}

	bool cWorld::SoundEntityExists(cSoundEntity* apEntity, int alCreationID)
	{
		tSoundEntityListIt it= mlstSoundEntities.begin();
		tSoundEntityListIt end = mlstSoundEntities.end();
		for(; it != end; ++it)
		{
			cSoundEntity *pTestSound = *it;
			if(*it == apEntity)
			{
				if(alCreationID==pTestSound->GetCreationID())	return true;
				else											return false;
			}
		}

		return false;
	}

	//-----------------------------------------------------------------------

	cStartPosEntity* cWorld::CreateStartPos(const tString &asName)
	{
		cStartPosEntity *pStartPos = hplNew( cStartPosEntity, (asName) );

		mlstStartPosEntities.push_back(pStartPos);

		return pStartPos;
	}

	cStartPosEntity* cWorld::GetStartPosEntity(const tString &asName)
	{
		return (cStartPosEntity*)STLFindByName(mlstStartPosEntities,asName);
	}

	cStartPosEntity* cWorld::GetFirstStartPosEntity()
	{
		if(mlstStartPosEntities.empty()) return NULL;

		return mlstStartPosEntities.front();
	}


	//-----------------------------------------------------------------------

	void  cWorld::GenerateAINodes(cAINodeGeneratorParams *apParams)
	{
		mpAI->GetNodeGenerator()->Generate(this, apParams);
	}

	//-----------------------------------------------------------------------

	cAINodeContainer* cWorld::CreateAINodeContainer(const tString &asName, 
								const tString &asNodeName,
								const cVector3f &avSize,
								bool abNodeIsAtCenter,
								int alMinEdges, int alMaxEdges, float afMaxEdgeDistance,float afMaxHeight)
	{
		cAINodeContainer* pContainer=NULL;

		//unsigned long lStartTime = mpSystem->GetLowLevel()->GetTime();

		//////////////////////////////////
		//See if the container is allready loaded.
		tAINodeContainerListIt it = mlstAINodeContainers.begin();
		for(; it != mlstAINodeContainers.end(); ++it)
		{
			cAINodeContainer* pCont = *it;
			if(pCont->GetName() == asName)
			{
				pContainer = pCont;
			}
		}
		

		//////////////////////////////////
		//Get file name
		cFileSearcher *pFileSearcher = mpResources->GetFileSearcher();
		tWString sMapPath = GetFilePath();

		tWString sAiFileName = cString::SetFileExtW(sMapPath,_W(""));
		sAiFileName += _W("_")+cString::To16Char(asName);
		sAiFileName = cString::SetFileExtW(sAiFileName,_W("nodes"));

		//////////////////////////////////
		//If there is no container created, create it.
		if(pContainer == NULL)
		{
			tTempNodeContainerMapIt ContIt = m_mapTempNodes.find(asNodeName);
			if(ContIt == m_mapTempNodes.end()){
				Warning("AI node type '%s' does not exist!\n",asNodeName.c_str());
				return NULL;
			}
			cTempNodeContainer *pTempCont = ContIt->second;

			pContainer = hplNew( cAINodeContainer, (asName,asNodeName,this,avSize) );
			mlstAINodeContainers.push_back(pContainer);

			//Set properties
			pContainer->SetMinEdges(alMinEdges);
			pContainer->SetMaxEdges(alMaxEdges);
			pContainer->SetMaxEdgeDistance(afMaxEdgeDistance);
			pContainer->SetMaxHeight(afMaxHeight);
			pContainer->SetNodeIsAtCenter(abNodeIsAtCenter);

			//Reserve space for the incoming nodes.
			pContainer->ReserveSpace(pTempCont->mlstNodes.size());

            //Add nodes to container
			tTempAiNodeListIt NodeIt = pTempCont->mlstNodes.begin();
			for(; NodeIt != pTempCont->mlstNodes.end(); ++NodeIt)
			{
				cTempAiNode& pNode = *NodeIt;
				pContainer->AddNode(pNode.msName,pNode.mlID,pNode.mvPos,NULL);
			}
			
			bool bLoadedFromFile=false;
			if(cPlatform::FileExists(sAiFileName))
			{
				cDate dateMapFile = cPlatform::FileModifiedDate(sMapPath);
				cDate dateAIFile = cPlatform::FileModifiedDate(sAiFileName);

				if(dateAIFile > dateMapFile || cResources::GetForceCacheLoadingAndSkipSaving())
				{
					bLoadedFromFile = true;
					pContainer->LoadFromFile(sAiFileName);
				}
			}
			
			if(bLoadedFromFile==false)
			{
				Log("Rebuilding node connections and saving to '%s'\n",cString::To8Char(sAiFileName).c_str());

				//Compile
				pContainer->Compile();

				//Save to disk
				if(cResources::GetForceCacheLoadingAndSkipSaving()==false)
				{
					pContainer->SaveToFile(sAiFileName);
				}
			}
		}
		
		//unsigned long lTime = mpSystem->GetLowLevel()->GetTime() - lStartTime;
		//Log("Creating ai nodes took: %d\n",lTime);


		return pContainer;
	}

	//-----------------------------------------------------------------------

	cAStarHandler* cWorld::CreateAStarHandler(cAINodeContainer* apContainer)
	{
		cAStarHandler *pAStar = hplNew( cAStarHandler, (apContainer) );

		mlstAStarHandlers.push_back(pAStar);

		return pAStar;
	}

	void cWorld::DestroyAStarHandler(cAStarHandler* apHandler)
	{
		STLFindAndDelete(mlstAStarHandlers, apHandler);
	}

	//-----------------------------------------------------------------------

	void cWorld::AddAINode(const tString &asName, int alID, const tString &asType, const cVector3f &avPosition)
	{
		cTempNodeContainer *pContainer = NULL;
		tTempNodeContainerMapIt it = m_mapTempNodes.find(asType);
		if(it != m_mapTempNodes.end()){
			pContainer = it->second;
		}

		if(pContainer==NULL){
			pContainer = hplNew( cTempNodeContainer, () );
			m_mapTempNodes.insert(tTempNodeContainerMap::value_type(asType,pContainer));
		}

		pContainer->mlstNodes.push_back(cTempAiNode(avPosition,asName, alID));
	}

	//-----------------------------------------------------------------------

	tTempAiNodeList* cWorld::GetAINodeList(const tString &asType)
	{
		cTempNodeContainer *pContainer = NULL;
		tTempNodeContainerMapIt it = m_mapTempNodes.find(asType);
		if(it != m_mapTempNodes.end()){
			pContainer = it->second;
		}

		if(pContainer==NULL){
			pContainer = hplNew( cTempNodeContainer, () );
			m_mapTempNodes.insert(tTempNodeContainerMap::value_type(asType,pContainer));
		}

		return &pContainer->mlstNodes;
	}

	//-----------------------------------------------------------------------

	bool cWorld::CreateFromFile(tString asFile)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	cDummyRenderable* cWorld::CreateDummyRenderable(const tString& asName, bool abStatic)
	{
		cDummyRenderable *pDummy = hplNew( cDummyRenderable, (asName));
		mlstDummyRenderables.push_back(pDummy);
		pDummy->SetStatic(abStatic);

		AddRenderableToContainer(pDummy);

		return pDummy;
	}

	void cWorld::DestroyDummyRenderable(cDummyRenderable* apDummy)
	{
		RemoveRenderableFromContainer(apDummy);

		STLFindAndDelete(mlstDummyRenderables, apDummy);
	}

	cDummyRenderable* cWorld::GetDummyRenderable(const tString& asName)
	{
		return static_cast<cDummyRenderable*>(STLFindByName(mlstDummyRenderables, asName));
	}

	cDummyRenderable* cWorld::GetDummyRenderableFromUniqueID(int alID)
	{
		for(tDummyRenderableListIt it=mlstDummyRenderables.begin();it !=mlstDummyRenderables.end();++it)
		{
			if((*it)->GetUniqueID() == alID) return *it;
		}
		return NULL;
	}

	cDummyRenderableIterator cWorld::GetDummyRenderableIterator()
	{
		return cDummyRenderableIterator(&mlstDummyRenderables);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void cWorld::AddRenderableToContainer(iRenderable *apObject)
	{
		if(apObject->IsStatic())
			mpRenderableContainer[eWorldContainerType_Static]->Add(apObject);
		else
			mpRenderableContainer[eWorldContainerType_Dynamic]->Add(apObject);
	}
	
	//-----------------------------------------------------------------------

	void cWorld::RemoveRenderableFromContainer(iRenderable *apObject)
	{
		if(apObject->IsStatic())	mpRenderableContainer[eWorldContainerType_Static]->Remove(apObject);
		else						mpRenderableContainer[eWorldContainerType_Dynamic]->Remove(apObject);
	}

	//-----------------------------------------------------------------------

	void cWorld::UpdateParticles(float afTimeStep)
	{
		tParticleSystemListIt it = mlstParticleSystems.begin();

		while(it != mlstParticleSystems.end())
		{
			cParticleSystem *pPS = *it;

			pPS->UpdateLogic(afTimeStep);

			//Check if the system is alive, else destroy
			if(pPS->GetRemoveWhenDead() && pPS->IsDead())
			{
                it = mlstParticleSystems.erase(it);
				for(int i=0; i< pPS->GetEmitterNum();++i)
				{
					RemoveRenderableFromContainer(pPS->GetEmitter(i));
				}
				hplDelete(pPS);
			}
			else
			{
				it++;
			}
		}
	}
	//-----------------------------------------------------------------------


	void cWorld::UpdateEntities(float afTimeStep)
	{
		//static size_t lLastSize = 0;
		//bool bRenderDebug = lLastSize != mlstDynamicMeshEntities.size() && mlstDynamicMeshEntities.size()>=2;
		//if(mlstDynamicMeshEntities.size()>=2) lLastSize = mlstDynamicMeshEntities.size();
        	
		tMeshEntityListIt MeshIt = mlstDynamicMeshEntities.begin();
		tMeshEntityListIt endIt =mlstDynamicMeshEntities.end();
		//if(bRenderDebug)Log("----\n");
		for(;MeshIt != endIt;MeshIt++)
		{
			cMeshEntity *pEntity = *MeshIt;

			if(pEntity->IsActive()){
				//if(pEntity->IsStatic()==false) START_TIMING_EX(pEntity->GetName().c_str(),entity);
				pEntity->UpdateLogic(afTimeStep);
				//if(bRenderDebug) Log("Enitity '%s'. Pos: (%s), Matrix: (%s)\n", pEntity->GetName().c_str(), pEntity->GetWorldPosition().ToString().c_str(),
																			//pEntity->GetWorldMatrix().ToString().c_str());
				//if(pEntity->IsStatic()==false) STOP_TIMING(entity);
			}
		}
		//if(bRenderDebug)Log("----\n");
	}

	//-----------------------------------------------------------------------

	void cWorld::UpdateLights(float afTimeStep)
	{
		tLightListIt it = mlstLights.begin();

		while(it != mlstLights.end())
		{
			iLight *pLight = *it;

			if(pLight->IsActive()) pLight->UpdateLogic(afTimeStep);

			++it;
		}
	}

	//-----------------------------------------------------------------------

	void cWorld::UpdateSoundEntities(float afTimeStep)
	{
		tSoundEntityListIt it = mlstSoundEntities.begin();

		while(it != mlstSoundEntities.end())
		{
			cSoundEntity *pSound = *it;

			if(pSound->IsActive()) pSound->UpdateLogic(afTimeStep);

			//Check if the system is stopped, else destroy
			if(pSound->IsStopped() && pSound->GetRemoveWhenOver())
			{
				it =  mlstSoundEntities.erase(it);
				//mlstSoundEntityPool.push_back(pSound);
				hplDelete(pSound);
			}
			else
			{
				it++;
			}
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------

	kBeginSerializeBase(cAreaEntity)
	kSerializeVar(msName, eSerializeType_String)
	kSerializeVar(msType, eSerializeType_String)
	kSerializeVar(m_mtxTransform, eSerializeType_Matrixf)
	kSerializeVar(mvSize, eSerializeType_Vector3f)
	kEndSerialize()

	//-------------------------------------------------------------------

	kBeginSerializeBase(cStartPosEntity)
	kSerializeVar(msName, eSerializeType_String)
	kSerializeVar(m_mtxTransform, eSerializeType_Matrixf)
	kEndSerialize()

	
	//-----------------------------------------------------------------------


}
