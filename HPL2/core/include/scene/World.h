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

#ifndef HPL_WORLD_H
#define HPL_WORLD_H

#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "engine/EngineTypes.h"
#include "scene/SceneTypes.h"

class TiXmlElement;

namespace hpl {

	class cGraphics;
	class cResources;
	class cSound;
	class cPhysics;
	class cScene;
	class cSystem;
	class cAI;
	class cHaptic;

	class iCamera;
	class cCamera;
	class cNode3D;
	class iEntity3D;
	class cLightSpot;
	class cLightPoint;
	class cLightBox;
	class iLight;
	class cImageEntity;
	class cParticleManager;
	class cParticleSystem;
	class iScript;
	class cPortalContainer;
	class iRenderableContainer;
	class cMeshEntity;
	class cMesh;
	class cBillboard;
	class cBeam;
	class cGuiSetEntity;
	class iPhysicsWorld;
	class iPhysicsBody;
	class cSoundEntity;
	class cAINodeContainer;
	class cAStarHandler;
	class cAINodeGeneratorParams;
	class iVertexBuffer;
	class iTexture;
	class cGuiSet;
	class cRopeEntity;
	class iPhysicsRope;
	class cResourceVarsObject;
	class cFogArea;
	class cXmlElement;
	class cEntFile;
	class cDummyRenderable;
	

	//-------------------------------------------------------------------
	
	typedef std::list<cEntFile*> tEntFileList;
	typedef tEntFileList::iterator tEntFileListIt;

	//-------------------------------------------------------------------
	
	class cTempAiNode
	{
	public:
		cTempAiNode(const cVector3f& avPos, const tString& asName, int alID) : mvPos(avPos),msName(asName), mlID(alID) {}
		cVector3f mvPos;
		tString msName;
		int mlID;
	};
	
	typedef std::list<cTempAiNode> tTempAiNodeList;
	typedef std::list<cTempAiNode>::iterator tTempAiNodeListIt;

	class cTempNodeContainer
	{
	public:
		tString msName;
        tTempAiNodeList mlstNodes;		
	};

	typedef std::map<tString,cTempNodeContainer*> tTempNodeContainerMap;
	typedef std::map<tString,cTempNodeContainer*>::iterator tTempNodeContainerMapIt;

	//-------------------------------------------------------------------

	class cAreaEntity :public iSerializable
	{
		kSerializableClassInit(cAreaEntity)
	public:
		tString msName;
		tString msType;
		cMatrixf m_mtxTransform;
		cVector3f mvSize;
	};
	
	typedef std::map<tString, cAreaEntity*> tAreaEntityMap;
	typedef tAreaEntityMap::iterator tAreaEntityMapIt;

	//-------------------------------------------------------------------

	class cStartPosEntity : public iSerializable
	{
		kSerializableClassInit(cStartPosEntity)
	public:
		cStartPosEntity() {}
		cStartPosEntity(const tString& asName) : msName(asName){}

		cMatrixf& GetWorldMatrix(){ return m_mtxTransform;}
		cMatrixf& GetLocalMatrix(){ return m_mtxTransform;}
		void SetMatrix(const cMatrixf& a_mtxTrans){ m_mtxTransform = a_mtxTrans;}
		
		tString& GetName(){ return msName;}

		cMatrixf m_mtxTransform;
		tString msName;
	};
	
	typedef std::list<cStartPosEntity*> tStartPosEntityList;
	typedef std::list<cStartPosEntity*>::iterator tStartPosEntityListIt;

	//-------------------------------------------------------------------

	class cWorld
	{
	public:
		cWorld(tString asName,cGraphics *apGraphics,cResources *apResources,cSound* apSound,
					cPhysics *apPhysics, cScene *apScene,cSystem *apSystem, cAI *apAI,
					cHaptic *apHaptic);
		~cWorld();

		void DestroyAllEntities(tWorldDestroyAllFlag aFlags);

		tString GetName(){ return msName;}

		bool CreateFromFile(tString asFile);

		void SetFilePath(const tWString& asFile){ msFilePath = asFile;}
		const tWString& GetFilePath(){ return msFilePath;}

		void SetActive(bool abX) {mbActive = abX;}
		inline bool IsActive()  const { return mbActive;}

		void Update(float afTimeStep);

		void PreUpdate(float afTotalTime, float afTimeStep);

		cVector3f GetWorldSize(){ return mvWorldSize;}

		void SetIsSoundEmitter(bool abX){ mbIsSoundEmitter = abX;}
		bool IsSoundEmitter(){ return mbIsSoundEmitter;}

		iRenderableContainer* GetRenderableContainer(eWorldContainerType aType);
		
		cPhysics* GetPhysics(){ return mpPhysics;}
		cResources* GetResources(){ return mpResources;}
		cSound* GetSound(){ return mpSound;}
		cSystem* GetSystem(){ return mpSystem;}
		cHaptic* GetHaptic(){ return mpHaptic;}

		iEntity3D* CreateEntity(const tString& asName, const cMatrixf &a_mtxTransform, 
								const tString& asFile, int alID = -1, bool abActive=true,
								const cVector3f &avScale=cVector3f(1),
								cResourceVarsObject *apInstanceVars=NULL,
								bool abSkipNonStaticEntity=false);
								
		/**
		 * Call this when all things have been added to set up things like physics world size.
		 **/
		void Compile(bool abCalcPhysicsWorldSize);


		///// PHYSICS ////////////////////////////////

		void SetPhysicsWorld(iPhysicsWorld *apWorld, bool abAutoDelete=true);
		iPhysicsWorld* GetPhysicsWorld();

		///// SKYBOX ////////////////////////////////

		void SetSkyBox(iTexture *apTexture, bool abAutoDestroy);
		void SetSkyBoxActive(bool abX);
		void SetSkyBoxColor(const cColor& aColor);

		iTexture* GetSkyBoxTexture(){return mpSkyBoxTexture;}
		iVertexBuffer *GetSkyBoxVertexBuffer(){ return mpSkyBoxVtxBuffer;}
		bool GetSkyBoxActive(){ return mbSkyBoxActive;}
		cColor GetSkyBoxColor(){ return mSkyBoxColor;}
        
		///// FOG ////////////////////////////////

		void SetFogActive(bool abX){ mbFogActive = abX;}
		void SetFogStart(float afX){ mfFogStart = afX;}
		void SetFogEnd(float afX){ mfFogEnd = afX;}
		void SetFogFalloffExp(float afX){ mfFogFalloffExp = afX;}
		void SetFogColor(const cColor& aCol){ mFogColor = aCol; }
		void SetFogCulling(bool abX) {mbFogCulling=abX;}

		bool GetFogActive(){ return mbFogActive;}
		float GetFogStart(){ return mfFogStart;}
		float GetFogEnd(){ return mfFogEnd;}
		float GetFogFalloffExp(){ return mfFogFalloffExp;}
		const cColor& GetFogColor(){ return mFogColor; }
		bool GetFogCulling() { return mbFogCulling;}

		///// AREA ////////////////////////////////
		
		cAreaEntity* CreateAreaEntity(const tString &asName);
		cAreaEntity* GetAreaEntity(const tString &asName);
		tAreaEntityMap* GetAreaEntityMap(){return &m_mapAreaEntities;}

		///// MESH ENTITY METHODS ////////////////////
		
		cMeshEntity* CreateMeshEntity(const tString &asName,cMesh *apMesh, bool abStatic=false);
		void DestroyMeshEntity(cMeshEntity* apMesh);
		cMeshEntity* GetDynamicMeshEntity(const tString& asName);
		
		cMeshEntityIterator GetDynamicMeshEntityIterator();
		cMeshEntityIterator GetStaticMeshEntityIterator();
		
		void DrawMeshBoundingBoxes(const cColor &aColor, bool abStatic);
		
		///// LIGHT METHODS ////////////////////

		cLightPoint* CreateLightPoint(const tString &asName="",const tString &asGobo="", bool abStatic=false);
		cLightSpot* CreateLightSpot(const tString &asName="", const tString &asGobo="", bool abStatic=false);
		cLightBox* CreateLightBox(const tString &asName="", bool abStatic=false);
		void DestroyLight(iLight* apLight);
		iLight* GetLight(const tString& asName);
		iLight* GetLightFromUniqueID(int alID);

		tLightList * GetLightList(){ return &mlstLights;}

		cLightListIterator GetLightIterator(){ return cLightListIterator(&mlstLights);}

		///// BILLBOARD METHODS ////////////////////

		cBillboard* CreateBillboard(const tString& asName, const cVector2f& avSize,eBillboardType aType,const tString& asMaterial="",bool abStatic=false);
		void DestroyBillboard(cBillboard* apObject);
		cBillboard* GetBillboard(const tString& asName);
		cBillboard* GetBillboardFromUniqueID(int alID);
		cBillboardIterator GetBillboardIterator();

		///// BEAM METHODS ////////////////////

		cBeam* CreateBeam(const tString& asName, bool abStatic=false);
		void DestroyBeam(cBeam* apObject);
		cBeam* GetBeam(const tString& asName);
		cBeam* GetBeamFromUniqueID(int alID);
		cBeamIterator GetBeamIterator();

		///// PARTICLE METHODS ////////////////////

		cParticleSystem* CreateParticleSystem(	const tString& asName,const tString& asType, const cVector3f& avSize, bool abRemoveWhenDead=true);
		cParticleSystem* CreateParticleSystem(	const tString& asName,const tString& asDataName, cXmlElement* apElement, const cVector3f& avSize);
		void DestroyParticleSystem(cParticleSystem* apPS);
		cParticleSystem* GetParticleSystem(const tString& asName);
		cParticleSystem* GetParticleSystemFromUniqueID(int alID);
		bool ParticleSystemExists(cParticleSystem* apPS);

		void DestroyAllParticleSystems();

		cParticleSystemIterator GetParticleSystemIterator(){ return cParticleSystemIterator(&mlstParticleSystems);}

		///// GUISET ENTITY METHODS ////////////////////
		
		cGuiSetEntity* CreateGuiSetEntity(const tString& asName, cGuiSet *apSet, bool abStatic=false);
		void DestroyGuiSetEntity(cGuiSetEntity* apObject);
		cGuiSetEntity* GetGuiSetEntity(const tString& asName);
		cGuiSetEntity* GetGuiSetEntityFromUniqueID(int alID);
		cGuiSetEntityIterator GetGuiSetEntityIterator();

		///// ROPE ENTITY METHODS ////////////////////
		
		cRopeEntity* CreateRopeEntity(const tString& asName, iPhysicsRope *apRope, int alMaxSegments);
		void DestroyRopeEntity(cRopeEntity* apRope);
		cRopeEntity* GetRopeEntity(const tString& asName);
		cRopeEntity* GetRopeEntityFromUniqueID(int alID);
		cRopeEntityIterator GetRopeEntityIterator();

		///// FOG AREA METHODS ////////////////////
		cFogArea* CreateFogArea(const tString& asName, bool abStatic=false);
		void DestroyFogArea(cFogArea* apRope);
		cFogArea* GetFogArea(const tString& asName);
		cFogArea* GetFogAreaFromUniqueID(int alID);
		cFogAreaIterator GetFogAreaIterator();

		///// SOUND ENTITY METHODS ////////////////////

		cSoundEntity* CreateSoundEntity(const tString &asName,const tString &asSoundEntity, 
										bool abRemoveWhenOver);
		void DestroySoundEntity(cSoundEntity* apEntity);
		cSoundEntity* GetSoundEntity(const tString& asName);
		cSoundEntity* GetSoundEntityFromUniqueID(int alID);
		void DestroyAllSoundEntities();
		bool SoundEntityExists(cSoundEntity* apEntity, int alCreationID);

		cSoundEntityIterator GetSoundEntityIterator(){ return cSoundEntityIterator(&mlstSoundEntities);}

		///// START POS ENTITY METHODS ////////////////
		
		cStartPosEntity* CreateStartPos(const tString &asName);
		cStartPosEntity* GetStartPosEntity(const tString &asName);
		cStartPosEntity* GetFirstStartPosEntity();

		///// AI NODE METHODS ////////////////

		void GenerateAINodes(cAINodeGeneratorParams *apParams);

		cAINodeContainer* CreateAINodeContainer(const tString &asName, 
											const tString &asNodeName, 
											const cVector3f &avSize,
											bool abNodeIsAtCenter, 
											int alMinEdges, int alMaxEdges, float afMaxEdgeDistance,
											float afMaxHeight);

		cAStarHandler* CreateAStarHandler(cAINodeContainer* apContainer);
		void DestroyAStarHandler(cAStarHandler* apHandler);
        
		void AddAINode(const tString &asName, int alID, const tString &asType, const cVector3f &avPosition);
		tTempAiNodeList* GetAINodeList(const tString &asType);


		/// NODE METHODS //////////////////////
		// Remove this for the time being, not need it seems.
		//cNode3D* GetRootNode(){ return mpRootNode; }

		///// DUMMY RENDERABLE METHODS ////////////////////
		cDummyRenderable* CreateDummyRenderable(const tString& asName, bool abStatic=false);
		void DestroyDummyRenderable(cDummyRenderable* apDummy);
		cDummyRenderable* GetDummyRenderable(const tString& asName);
		cDummyRenderable* GetDummyRenderableFromUniqueID(int alID);
		cDummyRenderableIterator GetDummyRenderableIterator();


	private:
		void AddRenderableToContainer(iRenderable *apObject);
		void RemoveRenderableFromContainer(iRenderable *apObject);
		
		void UpdateEntities(float afTimeStep);
		void UpdateParticles(float afTimeStep);
		void UpdateLights(float afTimeStep);
		void UpdateSoundEntities(float afTimeStep);

		tString msName;
		tWString msFilePath;
		bool mbActive;

		cGraphics *mpGraphics;
		cSound* mpSound;
		cResources *mpResources;
		cPhysics *mpPhysics;
		cScene *mpScene;
		cSystem *mpSystem;
		cAI *mpAI;
		cHaptic *mpHaptic;

		iPhysicsWorld *mpPhysicsWorld;
		bool mbAutoDeletePhysicsWorld;

		bool mbIsSoundEmitter;
		
		cVector3f mvWorldSize;

		iRenderableContainer* mpRenderableContainer[2];

		iVertexBuffer* mpSkyBoxVtxBuffer;
		iTexture* mpSkyBoxTexture;
		bool mbAutoDestroySkybox;
		bool mbSkyBoxActive;
		cColor mSkyBoxColor;

		bool mbFogActive;
		bool mbFogCulling;
		float mfFogStart;
		float mfFogEnd;
		float mfFogFalloffExp;
		cColor mFogColor;

		tLightList mlstLights;
		tMeshEntityList mlstDynamicMeshEntities;
		tMeshEntityList mlstStaticMeshEntities;
		tBillboardList mlstBillboards;
		tBeamList mlstBeams;
		tParticleSystemList mlstParticleSystems;
		tGuiSetEntityList mlstGuiSetEntities;
		tRopeEntityList mlstRopeEntities;
		tSoundEntityList mlstSoundEntities;
		tStartPosEntityList mlstStartPosEntities;
		tAreaEntityMap m_mapAreaEntities;
		tFogAreaList mlstFogAreas;
		tDummyRenderableList mlstDummyRenderables;

		int mlSoundCreationIDCount;

		//tSoundEntityList mlstSoundEntityPool;<-Debugging

		tEntFileList mlstEntFileCache;
		
		tAINodeContainerList mlstAINodeContainers;
		tAStarHandlerList mlstAStarHandlers;
		tTempNodeContainerMap m_mapTempNodes;

		cNode3D* mpRootNode;

		tString msMapName;
		cColor mAmbientColor;
	};

};
#endif // HPL_WOLRD_H
