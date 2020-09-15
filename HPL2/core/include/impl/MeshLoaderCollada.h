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

#ifndef HPL_MESH_LOADER_COLLADA_H
#define HPL_MESH_LOADER_COLLADA_H


#include "math/MathTypes.h"
#include "resources/MeshLoader.h"
#include "graphics/VertexBuffer.h"
#include "physics/PhysicsJoint.h"
#include "system/MemoryManager.h"

class TiXmlElement;

namespace hpl {

	class cMesh;
	class cNode3D;
	class iVertexBuffer;
	class cBone;
	class cAnimation;
	class cAnimationTrack;
	class cSkeleton;
	class cWorld;
	class cSector;
	class cMeshEntity;
	class iPhysicsBody;
	class cMeshJoint;
	class cSystem;
	class cMeshLoaderMSH;

	//////////////////////////////////////////////
	// HELP CLASSES

	class cColladaImage
	{
	public:
		tString msId;
		tString msName;
		
		tString msSource;
	};

	typedef std::vector<cColladaImage> tColladaImageVec;
	
	//------------------------------------------------
	
	class cColladaTexture
	{
	public:
		tString msId;
		tString msName;

		tString msImage;
	};

	typedef std::vector<cColladaTexture> tColladaTextureVec;
	
	//------------------------------------------------

	class cColladaMaterial
	{
	public:
		tString msId;
		tString msName;

		tString msTexture;
		cColor mDiffuseColor;
	};

	typedef std::vector<cColladaMaterial> tColladaMaterialVec;
	
	//------------------------------------------------
	
	class cColladaLight
	{
	public:
		tString msId;
		tString msName;
		tString msType;

		cColor mDiffuseColor;
		float mfAngle;
	};

	typedef std::vector<cColladaLight> tColladaLightVec;

	
	//------------------------------------------------

	class cColladaVtxArray
	{
	public:
		cColladaVtxArray() : mbIsInVertex(false){}	
	
		tString msId;
		tString msType;
		bool mbIsInVertex;

		tVector3fVec mvArray;
	};

	typedef std::vector<cColladaVtxArray> tColladaVtxArrayVec;

	class cColladaVtxIndex
	{
	public:
        int mlVtx;
		int mlNorm;
		int mlTex;
	};
	
	typedef std::vector<cColladaVtxIndex> tColladaVtxIndexVec;

	//------------------------------------------------


	class cColladaExtraVtx
	{
	public:
		cColladaExtraVtx(int alVtx,int alNorm,int alTex,int alNewVtx)
		{
			mlVtx = alVtx;
			mlNorm = alNorm;
			mlTex = alTex;
			mlNewVtx = alNewVtx;
		}

		cColladaExtraVtx(){}

		int mlVtx;
		int mlNorm;
		int mlTex;

		int mlNewVtx;

		bool Equals(const cColladaVtxIndex& aData)
		{
			if(mlVtx == aData.mlVtx && mlNorm == aData.mlNorm && mlTex == aData.mlTex)
			{
				return true;
			}

			return false;
		}
	};

	typedef std::list<cColladaExtraVtx> tColladaExtraVtxList;
	typedef tColladaExtraVtxList::iterator tColladaExtraVtxListIt;

	typedef std::vector<tColladaExtraVtxList> tColladaExtraVtxListVec;

	//------------------------------------------------

	class cColladaGeometry
	{
	public:
		cColladaGeometry() : mlPosArrayIdx(-1),mlNormArrayIdx(-1),mlTexArrayIdx(-1),
						mlPosIdxNum(-1),mlNormIdxNum(-1),mlTexIdxNum(-1)  {}

		void Clear()
		{
			mvIndices.clear();

			for(int i=0; i<(int)mvArrayVec.size();i++)
			{
				mvArrayVec[i].mvArray.clear();
			}
			/*mvVertexVec.clear();
			mvIndexVec.clear();
			mvExtraVtxVec.clear();*/
		}	

		tString msId;
		tString msName;	

		tVertexVec mvVertexVec;
		tUIntVec mvIndexVec;
		tColladaExtraVtxListVec mvExtraVtxVec;
		tFloatVec mvTangents;

		tString msMaterial;


		//THe following are ONLY used when loading from a geometry element.!!
		tColladaVtxArrayVec mvArrayVec;
		tColladaVtxIndexVec mvIndices;
		
		int mlPosIdxNum; //The position in the triangle element
		int mlNormIdxNum; //for eternal use only
		int mlTexIdxNum;

		int mlPosArrayIdx; //The index for array containing positions
		int mlNormArrayIdx; //The index for array containing normals
		int mlTexArrayIdx; //The index for array containing texcoords
	};

	typedef std::vector<cColladaGeometry> tColladaGeometryVec;
	
	//------------------------------------------------
	
	class cColladaJointPair
	{
	public:
		cColladaJointPair(){}
		cColladaJointPair(int alJoint, int alWeight)
		{
			mlJoint = alJoint;
			mlWeight = alWeight;
		}

		int mlJoint;
		int mlWeight;
	};

	typedef std::list<cColladaJointPair> tColladaJointPairList;
	typedef tColladaJointPairList::iterator tColladaJointPairListIt;

	typedef std::vector<tColladaJointPairList> tColladaJointPairListVec;

	class cColladaController
	{
	public:
		cColladaController() : mlJointPairIdx(-1), mlWeightPairIdx(-1){}

		tString msTarget;
		tString msId;

		cMatrixf m_mtxBindShapeMatrix;
		
		int mlJointPairIdx;
		int mlWeightPairIdx;
		
		tStringVec mvJoints;
		tFloatVec mvWeights;
		tMatrixfVec mvMatrices;

		tColladaJointPairListVec mvPairs;
	};

	typedef std::vector<cColladaController> tColladaControllerVec;

	//------------------------------------------------
	
	class cColladaChannel
	{
	public:
		tString msId;
		tString msTarget;
		tString msSource;
	};

	typedef std::vector<cColladaChannel> tColladaChannelVec;

	class cColladaSampler
	{
	public:
		tString msId;
		tString msTimeArray;
		tString msValueArray;

		tString msTarget;
	};
	
	typedef std::vector<cColladaSampler> tColladaSamplerVec;
	
	class cColladaAnimSource
	{
	public:
		tString msId;
		tFloatVec mvValues;
	};

	typedef std::vector<cColladaAnimSource> tColladaAnimSourceVec;


	class cColladaAnimation
	{
	public:
		tString msId;
		//tString msName;
		tString msTargetNode;

		tColladaChannelVec mvChannels;
		tColladaSamplerVec mvSamplers;
        
		tColladaAnimSourceVec mvSources;

		tFloatVec* GetSourceVec(const tString& asId)
		{
			for(size_t i=0; i< mvSources.size(); i++)
			{
				if(mvSources[i].msId == asId)
				{
					return &mvSources[i].mvValues;
				}
			}

			return NULL;
		}
	};

	typedef std::vector<cColladaAnimation> tColladaAnimationVec;


	//------------------------------------------------
	
	class cColladaTransform
	{
	public:
		tString msSid;
		tString msType;
		tFloatVec mvValues;
	};

	typedef std::list<cColladaTransform> tColladaTransformList;
	typedef tColladaTransformList::iterator tColladaTransformListIt;


	class cColladaNode;
	typedef std::list<cColladaNode*> tColladaNodeList;
	typedef tColladaNodeList::iterator tColladaNodeListIt;

	class cColladaNode
	{
	public:
		cColladaNode() : mlCount(0), pParent(NULL), mvScale(1,1,1), msInstanceMaterial("") {}
		
		tString msId;
		tString msName;
		tString msSid;
		tString msType;

		tString msInstanceMaterial;

		tString msSource;
		bool mbSourceIsFile;

		cMatrixf m_mtxTransform;
		cMatrixf m_mtxWorldTransform;

		cColladaNode *pParent;

		cVector3f mvScale;

		int mlCount;

		tColladaNodeList mlstChildren;

		tColladaTransformList mlstTransforms;

		/*void DeleteChildren()
		{
			STLDeleteAll(mlstChildren);
		}*/

		cColladaNode* CreateChild()
		{
			cColladaNode *pNode = hplNew( cColladaNode, () );
			mlstChildren.push_back(pNode);
			pNode->pParent = this;
			return pNode;
		}

        cColladaTransform* GetTransform(const tString &asSid)
		{
			tColladaTransformListIt it = mlstTransforms.begin();
			for(; it != mlstTransforms.end(); it++)
			{
				 cColladaTransform &Trans = *it;
				 if(Trans.msSid == asSid)
				 {
					 return &Trans;
				 }
			}
			return NULL;
		}
	};

	class cColladaScene
	{
	public:
		cColladaScene()
		{
			mRoot.m_mtxTransform = cMatrixf::Identity;
			mRoot.m_mtxWorldTransform = cMatrixf::Identity;

			mfDeltaTime =0;
		}

		~cColladaScene()
		{
			//mRoot.DeleteChildren();
			STLDeleteAll(mlstNodes);
		}

		void ResetNodes()
		{
			STLDeleteAll(mlstNodes);
			mRoot.mlstChildren.clear();
		}

		cColladaNode* GetNode(const tString asId)
		{
			tColladaNodeListIt it = mlstNodes.begin();
			for(; it != mlstNodes.end();++it)
			{	
				cColladaNode *pNode = *it;
				if(pNode->msId == asId || pNode->msName == asId)
				{
					return pNode;
				}
			}

			return NULL;
		}

		cColladaNode* GetNodeFromSource(const tString asSource)
		{
			tColladaNodeListIt it = mlstNodes.begin();
			for(; it != mlstNodes.end();++it)
			{	
				cColladaNode *pNode = *it;
				if(pNode->msSource == asSource)
				{
					return pNode;
				}
			}

			return NULL;
		}

		float mfStartTime;
		float mfEndTime;
		float mfDeltaTime;

		cColladaNode mRoot;

		tColladaNodeList mlstNodes;
	};

	//------------------------------------------------
	
	class cMeshLoaderCollada : public iMeshLoader
	{
	public:
		cMeshLoaderCollada(iLowLevelGraphics *apLowLevelGraphics, cMeshLoaderMSH *apMeshLoaderMSH, bool abLoadAndSaveMSHFormat);
		~cMeshLoaderCollada();

		void SetLoadAndSaveMSHFormat(bool abX){ mbLoadAndSaveMSHFormat = abX;} //Needed for tools!

		cMesh* LoadMesh(const tWString& asFile, tMeshLoadFlag aFlags);
		bool SaveMesh(cMesh* apMesh,const tWString& asFile){return false;}

		cAnimation* LoadAnimation(const tWString& asFile);
		bool SaveAnimation(cAnimation* apAnimation, const tWString& asFile){return false;}

		static void SetConvertUnit(bool abX) { mbConvertUnit = abX; }
		static bool GetConvertUnit() { return mbConvertUnit; }

	private:
		cMeshLoaderMSH *mpMeshLoaderMSH;
		bool mbLoadAndSaveMSHFormat;

		float mfUnitScale;
		bool mbZToY;
		cMatrixf m_mtxZToY;
		static bool mbConvertUnit;

		tWorldLoadFlag mFlags;

		void CleanUpNonAnimatedNodes(cAnimation *apAnimation, cSkeleton *apSkeleton, cColladaScene* apColladaScene );

		tString GetParentName(cColladaNode *apNode, tColladaGeometryVec *apColladaGeometries);

		void CreateMeshJoint(cMeshJoint* apJoint,ePhysicsJointType aJointType,cBoundingVolume &aBV, 
							tStringVec &avStrings,cColladaNode* apNode,cColladaScene &aColladaScene,
							tColladaGeometryVec &avColladaGeom);

		void CreateHierarchyNodes(cMesh *apMesh, cNode3D* mpParentNode, 
			cColladaNode* apColladaNode, 
			tColladaGeometryVec &avColladaGeom);

		cColladaGeometry* GetGeometry(const tString& asId, tColladaGeometryVec &avGeomVec);
		cColladaLight* GetLight(const tString& asId, tColladaLightVec &avLightVec);

		
		cAnimationTrack* CreateAnimTrack(cAnimation *apAnimation , cSkeleton *apSkeleton,
						cColladaAnimation &aAnim, cColladaScene *apScene);

		void CalcLocalMatrixAfterControllerBindMatrixRec(cBone* apBone, cMatrixf a_mtxParentGlobal, int alDepth);

		void CreateSkeletonBone(cColladaNode* apColladaNode, cBone* apParentBone);

		iVertexBuffer *CreateVertexBuffer(cColladaGeometry & aGeometry, 
											eVertexBufferUsageType aUsageType);
											//tColladaExtraVtxListVec &vExtraVtxVec);

		bool FillStructures(const tWString &asFile,
							tColladaImageVec *apColladaImageVec,
							tColladaTextureVec *apColladaTextureVec,
							tColladaMaterialVec *apColladaMaterialVec,
							tColladaLightVec *apColladaLightVec,
							tColladaGeometryVec *apColladaGeometryVec,
							tColladaControllerVec *apColladaControllerVec,
							tColladaAnimationVec *apColladaAnimVec,
							cColladaScene *apColladaScene,
							bool abCache);

		bool SaveStructures(const tWString &asFile,
							tColladaImageVec *apColladaImageVec,
							tColladaTextureVec *apColladaTextureVec,
							tColladaMaterialVec *apColladaMaterialVec,
							tColladaLightVec *apColladaLightVec,
							tColladaGeometryVec *apColladaGeometryVec,
							tColladaControllerVec *apColladaControllerVec,
							tColladaAnimationVec *apColladaAnimVec,
							cColladaScene *apColladaScene);

		bool LoadStructures(const tWString &asFile,
							tColladaImageVec *apColladaImageVec,
							tColladaTextureVec *apColladaTextureVec,
							tColladaMaterialVec *apColladaMaterialVec,
							tColladaLightVec *apColladaLightVec,
							tColladaGeometryVec *apColladaGeometryVec,
							tColladaControllerVec *apColladaControllerVec,
							tColladaAnimationVec *apColladaAnimVec,
							cColladaScene *apColladaScene);

		void LoadColladaScene(TiXmlElement* apRootElem,cColladaNode *apParentNode, cColladaScene *apScene,
								tColladaLightVec *apColladaLightVec);

		void LoadAnimations(TiXmlElement* apRootElem, tColladaAnimationVec &avAnimations,
								cColladaScene *apColladaScene);

		void LoadImages(TiXmlElement* apRootElem, tColladaImageVec &avColladaImageVec);
		
		void LoadTextures(TiXmlElement* apRootElem, tColladaTextureVec &avColladaTextureVec);

		void LoadMaterials(TiXmlElement* apRootElem, tColladaMaterialVec &avColladaMaterialVec);
		
		void LoadLights(TiXmlElement* apRootElem, tColladaLightVec &avColladaLightVec);

		void LoadGeometry(TiXmlElement* apRootElem, tColladaGeometryVec &avColladaGeometryVec);
		void LoadVertexData(TiXmlElement* apSourceElem, tVector3fVec &avVtxVec);
		
		void LoadControllers(TiXmlElement* apRootElem, tColladaControllerVec &avColladaControllerVec,
							tColladaGeometryVec *apColladaGeometryVec);
		void LoadJointData(TiXmlElement* apSourceElem, cColladaController &aController);

		//Helpers
		void SplitVertices(cColladaGeometry &aGeometry,tColladaExtraVtxListVec &avExtraVtxVec, 
							tVertexVec &avVertexVec, tUIntVec &avIndexVec);


		void FillVertexVec(const char* apChars,tVector3fVec &avVtxVec, int alElements, int alVtxCount);
		tString GetTopString(const tString asPath);
		tString GetMaterialTextureFile(const tString &asMaterial, tColladaMaterialVec &avColladaMaterialVec,
																tColladaTextureVec &avColladaTextureVec,
																tColladaImageVec &avColladaImageVec);

		cVector3f GetVectorPos(const cVector3f &avVec);
		cVector3f GetVectorNormal(const cVector3f &avVec);
		cMatrixf GetMatrix(const cMatrixf &amtxA);
		cVector3f GetVectorPosFromPtr(float *apVec);
		cVector3f GetVectorRotationFromPtr(float *apVec);
		cVector3f GetVectorScaleFromPtr(float *apVec);
		
	};

};
#endif // HPL_MESH_LOADER_COLLADA_H
