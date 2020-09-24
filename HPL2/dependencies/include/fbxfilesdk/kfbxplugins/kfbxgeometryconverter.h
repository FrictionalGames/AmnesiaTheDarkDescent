/*!  \file kfbxgeometryconverter.h
*/

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXGEOMETRYCONVERTER_H
#define FBXFILESDK_KFBXPLUGINS_KFBXGEOMETRYCONVERTER_H

/**************************************************************************************

Copyright (C) 2001 - 2010 Autodesk, Inc. and/or its licensors.
All Rights Reserved.

The coded instructions, statements, computer programs, and/or related material 
(collectively the "Data") in these files contain unpublished information 
proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
Canada and United States of America federal copyright law and by international 
treaties. 

The Data may not be disclosed or distributed to third parties, in whole or in
part, without the prior written consent of Autodesk, Inc. ("Autodesk").

THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 

IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
OR DAMAGE. 

**************************************************************************************/
#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;
class KFbxScene;
class KFbxNode;
class KFbxNodeAttribute;
class KFbxGeometryBase;
class KFbxMesh;
class KFbxPatch;
class KFbxNurb;
class KFbxNurbsCurve;
class KFbxNurbsSurface;
class KFbxGeometry;
class KFbxCluster;
class KFbxWeightedMapping;
class KFbxLayerContainer;
class KFBXSurfaceEvaluator;
class KFbxVector4;

/** 
* This class provides the functionality to convert geometry nodes 
* attributes (KFbxMesh, KFbxNurb and KFbxPatch) and mainly focuses on the two
* major categories: Triangulation and conversion between NURBS and Patches surfaces. 
* \nosubgrouping
*/
class KFBX_DLL KFbxGeometryConverter
{
public:
    KFbxGeometryConverter(KFbxSdkManager* pManager);
    ~KFbxGeometryConverter();

    /** 
    * \name Triangulation
    */
    //@{
		/** Triangulate a mesh.
		  * \param pMesh     Pointer to the mesh to triangulate.
		  * \return          Pointer to the new triangulated mesh.
		  * \remarks         This method creates a new mesh, leaving the source mesh unchanged.
		  */
		KFbxMesh* TriangulateMesh(KFbxMesh const* pMesh);

		/** Triangulate a patch.
		  * \param pPatch     Pointer to the patch to triangulate.
		  * \return           Pointer to the new triangulated mesh.
		  * \remarks          The current deformations (skins & shapes) on the patch are also converted and applied to the
		  *                   resulting mesh.
		  */
		KFbxMesh* TriangulatePatch(KFbxPatch const* pPatch);

		/** Triangulate a nurb.
		  * \param pNurb     Pointer to the nurb to triangulate.
		  * \return          Pointer to the new triangulated mesh.
		  * \remarks         The current deformations (skins and shapes) on the nurb are also converted and applied to the
		  *                  resulting mesh.
		  */
		KFbxMesh* TriangulateNurb(KFbxNurb const* pNurb);

		/** Triangulate a mesh, patch or nurb contained in a node and preserve the
		  * skins and shapes animation channels.
		  * \param pNode     Pointer to the node containing the geometry to triangulate.
		  * \return          \c true on success, or \c false if the node attribute is not a mesh, a patch or a nurb.
		  * \remarks         See the remarks for functions TriangulateMesh(), TriangulatePatch() and TriangulateNurb().
		  */
		bool TriangulateInPlace(KFbxNode* pNode);

		/** Add an "alternate" geometry to the node.
		  * \param pNode                        Pointer to the node containing the geometry.
		  * \param pSrcGeom                     Pointer to the source geometry.
		  * \param pAltGeom                     Pointer to the alternate geometry.
		  * \param pSrcToAltWeightedMapping     Pointer to the weighted mapping table (optional).
		  * \param pConvertDeformations         Flag used only if parameter pSrcToAltWeightedMapping is a valid pointer to a weighted mapping table.
		  *                                     Set to \c true to convert deformations using the weighted mapping table.
		  * \return                             \c true on success, or \c false if the node attribute is not a mesh, a patch or a nurb.
		  * \remarks							  Although this method is publicly available, its only use in the FBX SDK is internal to some of
		  *                                     the conversion routines declared in this class.
		  */
		bool AddAlternateGeometry(KFbxNode* pNode, KFbxGeometry* pSrcGeom, KFbxGeometry* pAltGeom, KFbxWeightedMapping* pSrcToAltWeightedMapping, bool pConvertDeformations);

		/** Convert skins and shapes from source to destination geometry.
		  * \param pNode        Pointer to the node containing the geometry.
		  * \param pSrcGeom     Pointer to the source geometry.
		  * \param pDstGeom     Pointer to the destination geometry.
		  * \return             \c true on success, \c false otherwise.
		  * \remarks            Source and destination geometry must belong to the same node and must be linked by a geometry weighted map.
		  * \remarks			  Although this method is publicly available, its only use in the FBX SDK is internal to some of
		  *                     the conversion routines declared in this class.
		  */
		bool ConvertGeometryAnimation(KFbxNode* pNode, KFbxGeometry* pSrcGeom, KFbxGeometry* pDstGeom);

		/** Compute a "vertex-correspondence" table that helps passing from source to destination geometry.
		  * \param pSrcGeom                     Pointer to the source geometry.
		  * \param pDstGeom                     Pointer to the destination geometry.
		  * \param pSrcToDstWeightedMapping     Pointer to the weighted mapping table.
		  * \param pSwapUV                      Set to \c true to swap UVs.
		  * \return                             \c true on success, \c false if the function fails to compute the correspondence.
		  * \remarks                            Skins and shapes are also converted to fit the alternate geometry.
		  */
		bool ComputeGeometryControlPointsWeightedMapping(KFbxGeometry* pSrcGeom, KFbxGeometry* pDstGeom, KFbxWeightedMapping* pSrcToDstWeightedMapping, bool pSwapUV=false);
    //@}

    /** 
    * \name Geometry Conversion
    */
    //@{
		/** Convert from patch to nurb.
		  * \param pPatch     Pointer to the patch to convert.
		  * \return           Created nurb or \c NULL if the conversion fails.
		  * \remarks          The patch must be of type eBSPLINE, eBEZIER or eLINEAR.
		  */
		KFbxNurb* ConvertPatchToNurb(KFbxPatch *pPatch);

		/** Convert a patch contained in a node to a nurb. Use this function to preserve the patch's 
		  *   skins and shapes animation channels.
		  * \param pNode     Pointer to the node containing the patch.
		  * \return          \c true on success, \c false if the node attribute is not a patch.
		  * \remarks         The patch must be of type eBSPLINE, eBEZIER or eLINEAR.
		  */
		bool ConvertPatchToNurbInPlace(KFbxNode* pNode);

		/** Convert a patch to nurb surface.
		  * \param pPatch     Pointer to the patch to convert.
		  * \return           Created nurb surface or \c NULL if conversion fails.
		  * \remarks          The patch must be of type eBSPLINE, eBEZIER or eLINEAR.
		  */
		KFbxNurbsSurface* ConvertPatchToNurbsSurface(KFbxPatch *pPatch);

		/** Convert a patch contained in a node to a nurb surface. Use this function to preserve 
		  *   the patch's skins and shapes animation channels.
		  * \param pNode     Pointer to the node containing the patch.
		  * \return          \c true on success, \c false if the node attribute is not a patch.
		  * \remarks         The patch must be of type eBSPLINE, eBEZIER or eLINEAR.
		  */
		bool ConvertPatchToNurbsSurfaceInPlace(KFbxNode* pNode);

		/** Convert a KFbxNurb to a KFbxNurbsSurface
		  * \param pNurb     Pointer to the original nurb
		  * \return          A KFbxNurbsSurface that is equivalent to the original nurb.
		  */
		KFbxNurbsSurface* ConvertNurbToNurbsSurface( KFbxNurb* pNurb );

		/** Convert a KFbxNurbsSurface to a KFbxNurb
		  * \param pNurb     Pointer to the original nurbs surface
		  * \return          A KFbxNurb that is equivalent to the original nurbs surface.
		  */
		KFbxNurb* ConvertNurbsSurfaceToNurb( KFbxNurbsSurface* pNurb );

		/** Convert a nurb, contained in a node, to a nurbs surface. Use this function to preserve 
		  *   the nurb's skins and shapes animation channels.
		  * \param pNode     Pointer to the node containing the nurb.
		  * \return          \c true on success, \c false otherwise
		  */
		bool ConvertNurbToNurbsSurfaceInPlace(KFbxNode* pNode);

		/** Convert a nurb contained in a node to a nurbs surface. Use this function to preserve 
		  *   the nurb's skins and shapes animation channels.
		  * \param pNode     Pointer to the node containing the nurbs surface.
		  * \return          \c true on success, \c false otherwise
		  */
		bool ConvertNurbsSurfaceToNurbInPlace(KFbxNode* pNode);
    //@}

    /** 
    * \name Nurb UV and Links Swapping
    */
    //@{
		/** Flip UV and/or skin clusters of a nurb.
		  * \param pNurb             Pointer to the Source nurb.
		  * \param pSwapUV           Set to \c true to swap the UVs.
		  * \param pSwapClusters     Set to \c true to swap the control point indices of clusters.
		  * \return                  A flipped kFbxNurb, or \c NULL if the function fails.
		  */
		KFbxNurb* FlipNurb(KFbxNurb* pNurb, bool pSwapUV, bool pSwapClusters);

		/** Flip UV and/or skin clusters of a nurb surface.
		  * \param pNurb             Pointer to the Source nurb surface.
		  * \param pSwapUV           Set to \c true to swap the UVs.
		  * \param pSwapClusters     Set to \c true to swap the control point indices of clusters.
		  * \return                  A flipped kFbxNurbSurface, or \c NULL if the function fails.
		  */
		KFbxNurbsSurface* FlipNurbsSurface(KFbxNurbsSurface* pNurb, bool pSwapUV, bool pSwapClusters);
    //@}

    /** 
    * \name Normals By Polygon Vertex Emulation
    */
    //@{
		/** Emulate normals by polygon vertex mode for a mesh.
		  * \param pMesh     Pointer to the mesh object.
		  * \return          \c true on success, \c false if the number of normals in the 
		  *                  mesh and in its associated shapes don't match the number of polygon
		  *                  vertices.
		  * \remarks         For applications that only supports normals by control points, 
		  *                  this function duplicates control points to equal the 
		  *                  number of polygon vertices. skins and shapes are also converted.
		  *                  As preconditions:
		  *                       -# polygons must have been created
		  *                       -# the number of normals in the mesh and in its associated shapes must match the 
		  *                          number of polygon vertices.
		  */
		bool EmulateNormalsByPolygonVertex(KFbxMesh* pMesh);

		/** Create edge smoothing information from polygon-vertex mapped normals.
		  * Existing smoothing information is removed and edge data is created if
		  * none exists on the mesh.
		  * \param pMesh     The mesh used to generate edge smoothing.
		  * \return          \c true on success, \c false otherwise.
		  * \remarks         The edge smoothing data is placed on Layer 0 of the mesh.
		  *                  Normals do not need to be on Layer 0, since the first layer with
		  *                  per polygon vertex normals is used.
		  */
		bool ComputeEdgeSmoothingFromNormals( KFbxMesh* pMesh ) const;

		/** Convert edge smoothing to polygon smoothing group.
		  * Existing smoothing information is replaced.
		  * 
		  * \param pMesh     The mesh that contains the smoothing to be converted.
		  * \param pIndex    The index of the layer smoothing to be converted.
		  * \return          \c true on success, \c false otherwise.
		  * \remarks         The smoothing group is bitwise.  Each bit of the integer represents
		  *                  one smoothing group.  Therefore, there is a maximum of 32 smoothing groups.
		  */
		bool ComputePolygonSmoothingFromEdgeSmoothing( KFbxMesh* pMesh, int pIndex=0 ) const;

		/** Convert polygon smoothing group to edge smoothing.
		  * Existing smoothing information is replaced.
		  * 
		  * \param pMesh     The mesh that contains the smoothing to be converted.
		  * \param pIndex    The index of the layer smoothing to be converted
		  * \return          \c true on success, \c false otherwise.
		  */
		bool ComputeEdgeSmoothingFromPolygonSmoothing( KFbxMesh* pMesh, int pIndex=0 ) const;
    //@}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//    Anything beyond these lines may not be documented accurately and is 
//     subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
public:
	/** Replace node attribute with new one.
	  * \param pNode Pointer to the node which node attribute will be replaced.
	  * \param pNewNodeAttr new node attribute.
	  * \remark a)node attribute of all instance object will also be replaced;
	  *         b)the old node attribute will be destroyed.
	  */
	void ReplaceNodeAttribute(KFbxNode* pNode, KFbxNodeAttribute* pNewNodeAttr);

	/** Add a "triangulated mesh" geometry to the node.
	  * \param pNode Pointer to the node containing the geometry.
	  * \param pUVStepCoeff Coefficient factor for the U/V steps. Must be >= 1.
	  * \return \c true on success, \c false if the node attribute is not a mesh, 
	  * a patch or a nurb.
	  * \remarks The remarks relative to functions TriangulateMesh(), TriangulatePatch()
	  * , TriangulateNurb() and TriangulateInPlace() are applicable.
	  */
	bool AddTriangulatedMeshGeometry(KFbxNode* pNode, int pUVStepCoeff);

	/** Split Mesh Per Material.
	  * Each split mesh only has a single material on it.
	  * \param pMesh     The mesh that contains the smoothing to be converted.    
	  * \return          \c true on success, \c false otherwise.
	  * \remarks          It will work only on mesh that have material mapped "per-face" (Mapping Mode is KFbxLayerElement::eBY_POLYGON).
	  *                   It does NOT work on meshes with material mapped per-vertex/per-edge/etc.
	  *                   It will create as many meshes on output that there are materials applied to it. 
	  *                   If one mesh have some polygons with material A, some polygons with material B, 
	  *                   and some polygons with NO material, it should create 3 meshes after calling this function.
	  *                   The newly created meshes should be attached to the same KFbxNode that hold the original KFbxMesh.
	  *                   The original KFbxMesh STAY UNCHANGED.
	  *                   Now, the new mesh will have Normals, UVs, vertex color, material and textures.
	  */
	bool SplitMeshPerMaterial(KFbxMesh* pMesh);

	/** Split all the mesh in the scene.
	  * \param pScene    each mesh in the scene will be split.
	  * \return          \c true on success, \c false otherwise.
	  */
	bool SplitMeshesPerMaterial(KFbxScene* pScene);

	/** Create LayerElement, and copy settings from pRefMesh to pNewMesh.
	  * \param pNewMesh    new mesh to create layerElement.
	  * \param pRefMesh    reference mesh, to copy layerElement settings from it. 
	  * \return          \c true on success, \c false otherwise.
	  */
	bool CreateAndCopyLayerElement(KFbxMesh *pNewMesh, KFbxMesh *pRefMesh);

	/** Set Normals, UVs and Vertex Color, when building new mesh.
	  * \remarks          If MappingMode is KFbxLayerElement::eBY_POLYGON,
	  *                   only pIsEndPolygon/pPolygonIndex are meaningful here,
	  *                   pIsSearched/pPolyPointIndex/pLoopIndex are not used here.
	  */
	bool SetLayerElements(KFbxMesh *pNewMesh, KFbxMesh *pMesh, int pPolygonIndex, int pPolyPointIndex, int pLoopIndex, bool pIsSearched, bool pIsEndPolygon);

	/** Triangulate a mesh(support without holes or with holes simple polygon)
	  * \param pMesh     Pointer to the mesh to triangulate.
	  * \param pStatus   The status is \c true if the triangulation is successful for the whole mesh.
	  * \return          Pointer to the new triangulated mesh.
	  * \remarks         This method creates a new mesh, leaving the source mesh unchanged.
	  */
	KFbxMesh* TriangulateMeshAdvance(KFbxMesh const* pMesh, bool& pStatus);

	/** TriangulatePolygon          Triangulation of the current polygon 
	  * \param pNewMesh             The mesh to triangulate
	  * \param pControlPoints       Control points on the old mesh
	  * \param pVertexIndices       Vertex indices on the old mesh
	  * \param pPolyVertexIndices   Vertex indices for the current polygon
	  * \param pStartVertexIndex    Polygon Start Index from vertex indices on the old mesh
	  * \param pNormal              Face normal of the polygon
	  * \param pMaterialIndex       Material index of the polygon
	  * \param pPolygonSize         Vertex number of the polygon
	  * \param polygonIndex         Index o the polygon
	  * \return int                 Number of triangles after the triangulation
	  */
	K_DEPRECATED int TriangulatePolygon(KFbxMesh *pNewMesh, KFbxVector4 *pControlPoints, int* pVertexIndices, int * pPolyVertexIndices, int pStartVertexIndex, KFbxVector4& pNormal, int pMaterialIndex, int pPolygonSize, int polygonIndex);

protected:
    bool ComputePatchToMeshControlPointsWeightedMapping(KFbxPatch* pSrcPatch, KFbxMesh* pDstMesh, KFbxWeightedMapping* pMapping, bool pSwapUV=false);
    bool ComputeNurbToMeshControlPointsWeightedMapping(KFbxNurbsSurface* pSrcNurb, KFbxMesh* pDstMesh, KFbxWeightedMapping* pMapping, bool pRescaleUVs=false, bool pSwapUV=false);

    void InitializeWeightInControlPoints(KFbxGeometryBase* pGeometry);
    void InitializeWeightInNormals(KFbxLayerContainer* pLayerContainer);
    void TriangulateContinuousSurface(KFbxMesh* pMesh, KFBXSurfaceEvaluator* pSurface, kUInt pPointCountX, kUInt pPointCountY, bool ClockWise=false);
    void CheckForZeroWeightInShape(KFbxGeometry *pGeometry);
    KFbxMesh* CreateMeshFromParametricSurface(KFbxGeometry const* pGeometry);
    KFbxNurb* CreateNurbFromPatch(KFbxPatch* pPatch);
    KFbxNurbsSurface* CreateNurbsSurfaceFromPatch(KFbxPatch* pPatch);

    void ConvertShapes(KFbxGeometry const* pSource, KFbxGeometry* pDestination, KFBXSurfaceEvaluator* pEvaluator, int pUCount, int pVCount);
    void ConvertShapes(KFbxGeometry const* pSource, KFbxGeometry* pDestination, KFbxWeightedMapping* pSourceToDestinationMapping);
    void ConvertClusters(KFbxGeometry const* pSource, KFbxGeometry* pDestination, KFbxWeightedMapping* pSourceToDestinationMapping);
    void ConvertClusters(KArrayTemplate<KFbxCluster*> const& pSourceClusters, int pSourceControlPointsCount, KArrayTemplate<KFbxCluster*>& pDestinationClusters, int pDestinationControlPointsCount, KFbxWeightedMapping* pSourceToDestinationMapping);
    void BuildClusterToSourceMapping(KArrayTemplate<KFbxCluster*> const& pSourceClusters, KFbxWeightedMapping* pClusterToSourceMapping);
    void CheckClusterToSourceMapping(KFbxWeightedMapping* pClusterToSourceMapping);
    void ConvertCluster(int pSourceClusterIndex, KFbxWeightedMapping* pClusterToSourceMapping, KFbxWeightedMapping* pSourceToDestinationMapping, KFbxCluster* pDestinationCluster);
    void DuplicateControlPoints(KArrayTemplate<KFbxVector4>& pControlPoints, KArrayTemplate<int>& pPolygonVertices);
    void UpdatePolygon(KFbxMesh *pNewMesh, KFbxMesh const *pRefMesh, int pPolygonIndex, int* pNewIndex, int &pVerticeIndexMeshTriangulated, int &pPolygonIndexMeshTriangulated);
    void UpdatePolygon(KFbxMesh *pNewMesh, KFbxMesh const *pRefMesh, int pPolygonIndex, int* pNewIndex, int &pVerticeIndexMeshTriangulated, int &pPolygonIndexMeshTriangulated, int pTriangleNum);
    void ResizePolygon(KFbxMesh *pNewMesh, int pNewCountVertices = 0, int pNewCountPolygons =0, bool pClearFlag = true);

    template <class T1, class T2> void ConvertNurbs(T1* pNewNurb, T2* pOldNurb);

    bool CopyAnimationCurves(KFbxNode* pNode, KFbxGeometry* pNewGeometry);
    bool FlipNurbsCurve(KFbxNurbsCurve* pCurve) const;
    void FlipControlPoints(KFbxGeometryBase* pPoints, int pUCount, int pVCount) const;
    bool ConvertMaterialReferenceMode(KFbxMesh* pMeshRef) const;
    void RevertMaterialReferenceModeConversion(KFbxMesh* pMeshRef) const;

    KFbxSdkManager* mManager;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXGEOMETRYCONVERTER_H

