/****************************************************************************************
 
   Copyright (C) 2013 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxdeformationsevaluator.h
#ifndef _FBXSDK_UTILS_DEFORMATIONS_EVALUATOR_H_
#define _FBXSDK_UTILS_DEFORMATIONS_EVALUATOR_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

class FbxNode;
class FbxMesh;
class FbxTime;
class FbxAnimLayer;
class FbxPose;
class FbxCluster;
class FbxVector4;
class FbxAMatrix;
class FbxDualQuaternion;

class FBXSDK_DLL FbxDeformationsEvaluator
{
public:
    /** Configure this object to process the given mesh.
      * \param pNode Node object owner of the mesh.
      * \param pMesh Processed object.
      * \param pAnimLayerId Animation layer to be used during the evaluation.
      * \return \c true If \e pMesh is connected to \e pNode, the specified animation layer can be retrieved and 
      *         the internal data allocation succeeded.
      * \remarks The function will try to retrieve the animation layer connected to the Animation stack
      *          accessed from the current scene animation evaluator.
      *          \code
      *          FbxAnimStack* lStack = mNode->GetScene()->GetEvaluator()->GetContext();
      *          \endcode
      */
    bool Init(const FbxNode* pNode, const FbxMesh* pMesh, int pAnimLayerId = 0);

    /** If this object is properly configured, evaluates the shape deformation of the mesh at the given time.
      * \param pVertexArray The result of the evaluation.
      * \param pTime        Current time of the evaluation.
      
      * \return \c true if the function completed successfully and \c false in case of errors.
      * \remarks \e pVertexArray must be allocated and be of size: \e mMesh->GetControlPointCount().
      */
    bool ComputeShapeDeformation(FbxVector4* pVertexArray, const FbxTime& pTime);

    /** If this object is properly configured, evaluates the skin deformation of the received mesh at the given time.
      * \param pVertexArray The result of the evaluation.
      * \param pTime        Current time of the evaluation.
      * \param pGX          Local to World matrix to express the returned vertices in World space.
      * \param pPose        If defined, use the pose to evaluate the current transform.
      * \return \c true if the function completed successfully and \c false in case of errors.
      * \remarks \e pVertexArray must be allocated and be of size: \e mMesh->GetControlPointCount().
      */
    bool ComputeSkinDeformation(FbxVector4* pVertexArray, const FbxTime& pTime, FbxAMatrix* pGX = NULL, const FbxPose* pPose = NULL);

/*****************************************************************************************************************************
** WARNING! Anything beyond these lines is for internal use, may not be documented and is subject to change without notice! **
*****************************************************************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    FbxDeformationsEvaluator();
    virtual ~FbxDeformationsEvaluator();

private:
	void ComputeClusterDeformation(FbxVector4* pVertexArray, const FbxTime& pTime, const FbxAMatrix& pGX, FbxCluster* pCluster, FbxAMatrix& pVertexTransformMatrix, const FbxPose* pPose);
	void ComputeLinearDeformation(FbxVector4* pVertexArray, const FbxTime& pTime, const FbxAMatrix& pGX, const FbxPose* pPose);
	void ComputeDualQuaternionDeformation(FbxVector4* pVertexArray, const FbxTime& pTime, const FbxAMatrix& pGX, const FbxPose* pPose);

    void        Cleanup();

    bool                mIsConfigured;
    FbxNode*            mNode;
    FbxMesh*            mMesh;
    FbxAnimLayer*       mAnimLayer;

    int                 mVertexCount;
    FbxVector4*         mDstVertexArray;
    FbxVector4*         mVertexArrayLinear;
    FbxVector4*         mVertexArrayDQ;

    FbxAMatrix*         mClusterDeformation;
    double*             mClusterWeight;
    FbxDualQuaternion*  mDQClusterDeformation;
#endif /* !DOXYGEN_SHOULD_SKIP_THIS *****************************************************************************************/
};

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_UTILS_DEFORMATIONS_EVALUATOR_H_ */
