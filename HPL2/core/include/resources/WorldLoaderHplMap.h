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

#ifndef HPL_WORLD_LOADER_HPL_MAP_H
#define HPL_WORLD_LOADER_HPL_MAP_H

#include "resources/WorldLoader.h"

#include "resources/ResourcesTypes.h"
#include "scene/SceneTypes.h"
#include "graphics/GraphicsTypes.h"
#include "physics/PhysicsTypes.h"

namespace hpl {

	class iEntity3D;
	class cXmlElement;
	class iPhysicsWorld;
	class cRenderableContainer_BoxTree;
	class cSubMeshEntity;
	class iPhysicsMaterial;
	class cResourceVarsObject;
	class iPhysicsBody;

	//----------------------------------------

#if (defined(__ppc__) || defined(__PPC__)) && !defined(__LP64__)
	// PPC here
	#define MAP_CACHE_FORMAT_MAGIC_NUMBER		0xF4414550
#else
	// Only need to specialize PPC as 64bit and 32bit newton data is the same
	#define MAP_CACHE_FORMAT_MAGIC_NUMBER		0xF441451F
#endif

	#define MAP_CACHE_FORMAT_VERSION			10
	
	//----------------------------------------
	
	class cHplMapStaticUserData
	{
	public:
		cVector3f mvScale;
		bool mbCollides;
		bool mbCharCollider;
		bool mbCombine;
		bool mbVisible;
	};

	typedef std::list<cHplMapStaticUserData*> tHplMapStaticUserDataList;
	typedef tHplMapStaticUserDataList::iterator tHplMapStaticUserDataListIt;

	
	//----------------------------------------
	
	class cHplMapPhysicsObject
	{
	public:
		cSubMeshEntity *mpObject;
		iPhysicsMaterial *mpPhysicsMaterial;
		cHplMapStaticUserData *mpUserData;
		bool mbCharCollider;
	};

	//----------------------------------------

	class cHplMapShape
	{
	public:
		eCollideShapeType mType;
		cVector3f mvSize;
		cMatrixf m_mtxOffset;
	};

	typedef std::vector<cHplMapShape*> tHplMapShapeVec;
	typedef tHplMapShapeVec::iterator tHplMapShapeVecIt;

	//----------------------------------------
	
	class cHplMapShapeBody
	{
	public:
		cHplMapShapeBody();
		~cHplMapShapeBody();

		tString msMaterial;
		bool mbCharCollider;
		bool mbBlocksLight;
		cMatrixf m_mtxTransform;
		
		tHplMapShapeVec mvColliders;
	};
	
	typedef std::list<cHplMapShapeBody*> tHplMapShapeBodyList;
	typedef tHplMapShapeBodyList::iterator tHplMapShapeBodyListIt;

	//----------------------------------------

	
	class cWorldLoaderHplMap : public iWorldLoader
	{
	public:
		cWorldLoaderHplMap();
		~cWorldLoaderHplMap();

			
		cWorld* LoadWorld(const tWString& asFile, tWorldLoadFlag aFlags);

	private:
		void LoadCacheFile(const tWString& asFile);
		void SaveCacheFile(const tWString& asFile);

		void LoadFileIndicies(cXmlElement* apXmlContents);
			
		void LoadStaticObjects(cXmlElement* apXmlContents);		
		void BuildCombinedStaticMeshes(cRenderableContainer_BoxTree *apContainer);
		void CreateStaticObjectEntity(	cXmlElement* apElement, tMeshEntityList& alstMeshEntities,
										cRenderableContainer_BoxTree *apContainer);
		void CreatePrimitive(cXmlElement* apElement, tMeshEntityList& alstMeshEntities,
							cRenderableContainer_BoxTree *apContainer);
		void CreateDecal(	cXmlElement* apElement, tMeshEntityList& alstMeshEntities,
							cRenderableContainer_BoxTree *apDecalContainer);
		void CreateStaticObjectCombo(cXmlElement* apElement, tMeshEntityList& alstMeshEntities,
									cRenderableContainer_BoxTree *apDecalContainer);

		void IterateLeafNodesAndBuildMeshes(iRenderableContainerNode *apNode);
		void CombineAndCreateMeshesAndPhysics(tRenderableList *apObjectList);
		void CombineObjectsAndCreateMeshEntity(tRenderableVec &avObjects, int alFirstIdx, int alLastIdx);
		void CombineObjectsAndCreatePhysics(std::vector<cHplMapPhysicsObject> &avObjects, int alFirstIdx, int alLastIdx);

		void LoadEntities(cXmlElement* apXmlContents);
		void CreateLoadedEntity(cXmlElement* apElement, tEFL_LightBillboardConnectionList *apLightBillboardList);
		void CreateSubMeshShapeBodies(cSubMeshEntity *apSubEnt, const cMatrixf &a_mtxTransform, const cVector3f& avScale);
		void CreateShapeBody(cHplMapShapeBody* apShapeBody);

		void LoadEntity(const tString& asName, int alID, bool abActive,const cVector3f& avPos, const cVector3f& avRot, const cVector3f& avScale, cXmlElement* apElement);
		void LoadArea(const tString& asName, int alID, bool abActive,const cVector3f& avPos, const cVector3f& avRot,const cVector3f& avScale, cXmlElement* apElement);

		bool CheckTransformValidity(const tString& asName, const cVector3f& avPos, const cVector3f& avRot, const cVector3f& avScale);
		
		int mlCombinedMeshNameCount;
		int mlCombinedBodyNameCount;

		bool mbLoadedCache;

		tStringVec mvFileIndices_StaticObjects;
		tStringVec mvFileIndices_Entities;
		tStringVec mvFileIndices_Decals;

		int mlSortingTimeTotal;
		int mlCombineMeshTimeTotal;
		int mlCombineBodyTimeTotal;

		cWorld* mpCurrentWorld;
		iPhysicsWorld *mpCurrentPhysicsWorld;

		std::list<iPhysicsBody*> mlstStaticMeshBodies;
		tHplMapShapeBodyList mlstStaticShapeBodies;
		tMeshEntityList mlstStaticMeshEntities;
		
		int mlStaticMeshBodiesCreated;
		int mlStaticMeshEntitiesCreated;

		float *mpShortNegPosFloatTable;
		float *mpByteNegPosFloatTable;
		float *mpBytePosFloatTable;

		tWString msCacheFileExt;

		tWorldLoadFlag mlCurrentFlags;
		tHplMapStaticUserDataList mlstTempStaticUserData;
	};

};
#endif // HPL_WORLD_LOADER_HPL_MAP_H
