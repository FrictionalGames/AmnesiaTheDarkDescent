/*!  \file kfbxcluster.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXCLUSTER_H
#define FBXFILESDK_KFBXPLUGINS_KFBXCLUSTER_H

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

#include <fbxfilesdk/kfbxplugins/kfbxsubdeformer.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>
#include <fbxfilesdk/kfbxmath/kfbxmatrix.h>

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;
class KFbxNode;
class KFbxCluster_internal;

/** Class for clusters (links). 
  * A cluster, or link, is an entity acting on a geometry (KFbxGeometry).
  * More precisely, the cluster acts on a subset of the geometry's control points.
  * For each control point that the cluster acts on, the intensity of the cluster's
  * action is modulated by a weight. The link mode (ELinkMode) specifies how
  * the weights are taken into account.
  *
  * The cluster's link node specifies the node (KFbxNode) that influences the
  * control points of the cluster. If the node is animated, the control points
  * will move accordingly.
  *
  * A cluster is usually part of a skin (see KFbxDeformer, KFbxSkin). For example,
  * imagine a mesh representing a humanoid, and imagine a skeleton made of bones. 
  * Each bone is represented by a node in FBX.
  * To bind the geometry to the nodes, 
  * we create a skin (KFbxSkin). The skin has many clusters, each one corresponding
  * to a bone.
  * Each node influences some control
  * points of the mesh. A node has a high influence on some of the points (high weight)
  * and lower influence on some other points (low weight). Some points of the mesh
  * are not affected at all by the bone, so they would not be part of the corresponding
  * cluster.
  *
  * 
  *
  *\nosubgrouping
  */
class KFBX_DLL KFbxCluster : public KFbxSubDeformer
{
    KFBXOBJECT_DECLARE(KFbxCluster,KFbxSubDeformer);

public:
    /**
      * \name General Functions
      */
    //@{
    /** Get the type of the sub deformer.
      * \return SubDeformer type identifier: eCLUSTER.
      */
    ESubDeformerType GetSubDeformerType() const {return eCLUSTER; };

    /** Restore the cluster to its initial state.
      * Calling this function will clear the following:
      * \li Pointer to linked node.
      * \li Pointer to associate model.
      * \li Control point indices and weights.
      * \li Transformation matrices.
      */
    void Reset();
    //@}
    /**
      * \name Link Mode, Link Node, Associate Model
      */
    //@{
    /** \enum ELinkMode Link modes.
      * The link mode sets how the link influences the position of a control
      * point and the relationship between the weights assigned to a control
      * point. The weights assigned to a control point are distributed among
      * the set of links associated with an instance of class KFbxGeometry.
      *      - \e eNORMALIZE     In mode eNORMALIZE, the sum of the weights assigned to a control point
      *                          is normalized to 1.0. Setting the associate model in this mode is not
      *                          relevant. The influence of the link is a function of the displacement of the
      *                          link node relative to the node containing the control points.
      *      - \e eADDITIVE      In mode eADDITIVE, the sum of the weights assigned to a control point
      *                          is kept as is. It is the only mode where setting the associate model is
      *                          relevant. The influence of the link is a function of the displacement of
      *                          the link node relative to the node containing the control points or,
      *                          if set, the associate model. The weight gives the proportional displacement
      *                          of a control point. For example, if the weight of a link over a control
      *                          point is set to 2.0, a displacement of the link node of 1 unit in the X
      *                          direction relative to the node containing the control points or, if set,
      *                          the associate model, triggers a displacement of the control point of 2
      *                          units in the same direction.
      *      - \e eTOTAL1        Mode eTOTAL1 is identical to mode eNORMALIZE except that the sum of the
      *                          weights assigned to a control point is not normalized and must equal 1.0.
      */
    typedef enum
    {
        eNORMALIZE,
        eADDITIVE,
        eTOTAL1
    } ELinkMode;

    /** Set the link mode.
      * \param pMode     The link mode.
      * \remarks         All the links associated to an instance of class KFbxGeometry must have the same link mode.
      */
    void SetLinkMode(ELinkMode pMode);

    /** Get the link mode.
      * \return     The link mode.
      */
    ELinkMode GetLinkMode() const;

    /** Set the link node. The link node is the node which influences the displacement
      *                  of the control points. Typically, the link node is the bone a skin is
      *                  attached to.
      * \param pNode     The link node.     
      */
    void SetLink(KFbxNode const* pNode);

    /** Get the link node. The link node is the node which influences the displacement
      *              of the control points. Typically, the link node is the bone a skin is
      *              attached to.
      * \return      The link node or \c NULL if KFbxCluster::SetLink() has not been called before.
      */
    KFbxNode* GetLink();

    /** Get the link node (as const). The link node is the node which influences the displacement
      *              of the control points. Typically, the link node is the bone a skin is
      *              attached to.
      * \return      The link node or \c NULL if KFbxCluster::SetLink() has not been called before. 
      */
    KFbxNode const* GetLink() const;

    /** Set the associate model.
      * The associate model is optional. It is only relevant if the link mode
      * is of type eADDITIVE. If set, the associate model is the node used as a reference to
      *                  measure the relative displacement of the link node. Otherwise, the
      *                  displacement of the link node is measured relative to the node
      *                  containing the control points. Typically, the associate model node is
      *                  the parent of the bone a skin is attached to.
      * \param pNode     The associate model node.   
      */
    void SetAssociateModel(KFbxNode *pNode);

    /** Get the associate model.
      * The associate model is optional. It is only relevant if the link mode is of type
      * eADDITIVE. If set, the associate model is the node used as a reference to
      *              measure the relative displacement of the link node. Otherwise, the
      *              displacement of the link node is measured relative the the node
      *              containing the control points. Typically, the associate model node is
      *              the parent of the bone a skin is attached to.
      * \return      The associate model node or \c NULL if KFbxCluster::SetAssociateModel() has not been called before. 
      */
    KFbxNode* GetAssociateModel() const;
    //@}
    /**
      * \name Control Points
      * A link has an array of indices to control points and associated weights.
      * The indices refer to the control points in the instance of class KFbxGeometry
      * owning the link. The weights are the influence of the link node over the
      * displacement of the indexed control points.
      */
    //@{

    /** Add an element in both arrays of control point indices and weights.
      * \param pIndex          The index of the control point.
      * \param pWeight         The link weight for this control point.
      */
    void AddControlPointIndex(int pIndex, double pWeight);

    /** Get the length of the arrays of control point indices and weights.
      * \return     Length of the arrays of control point indices and weights.
      *             Returns 0 if no control point indices have been added or the arrays have been reset.
      */
    int GetControlPointIndicesCount() const;

    /** Get the array of control point indices.
      * \return     Pointer to the array of control point indices.
      *             \c NULL if no control point indices have been added or the array has been reset.
      */
    int* GetControlPointIndices() const;

    /** Get the array of control point weights.
      * \return     Pointer to the array of control point weights.
      *             \c NULL if no control point indices have been added or the array has been reset.
      */
    double* GetControlPointWeights() const;

    //@}

    /** Set the array size for the three arrays: the array of control point indices, the array of weights
	  * and the array of blend weights.
      * \param pCount The new count.
      */
	void SetControlPointIWCount(int pCount);

    /**
      * \name Transformation Matrices
      * A link has three transformation matrices:
      *      \li Transform refers to the global initial transform of the geometry node that contains the link node.
      *      \li TransformLink refers to global initial transform of the link node.
      *      \li TransformAssociateModel refers to the global initial transform of the associate model.
      *
	  * For example, given a mesh binding with several bones(links), Transform is the global transform 
	  * of the mesh at the binding moment, TransformLink is the global transform of the bone(link)
	  * at the binding moment, TransformAssociateModel is the global transform of the associate model 
	  * at the binding moment.
      */
    //@{

    /** Set matrix associated with the node containing the link.
      * \param pMatrix     Transformation matrix.
      */
    void SetTransformMatrix(const KFbxXMatrix& pMatrix);

    /** Get matrix associated with the node containing the link.
      * \param pMatrix     Transformation matrix to be filled with appropriate data.
      * \return            Input parameter matrix filled with appropriate data.
      */
    KFbxXMatrix& GetTransformMatrix(KFbxXMatrix& pMatrix) const;

    /** Set matrix associated with the link node.
      * \param pMatrix     Transformation matrix.
      */
    void SetTransformLinkMatrix(const KFbxXMatrix& pMatrix);

    /** Get matrix associated with the link node.
      * \param pMatrix     Transformation matrix to be filled with appropriate data..
      * \return            Input parameter matrix filled with appropriate data.
      */
    KFbxXMatrix& GetTransformLinkMatrix(KFbxXMatrix& pMatrix) const;

    /** Set matrix associated with the associate model.
      * \param pMatrix     Transformation matrix.
      */
    void SetTransformAssociateModelMatrix(const KFbxXMatrix& pMatrix);

    /** Get matrix associated with the associate model.
      * \param pMatrix     Transformation matrix to be filled with appropriate data..
      * \return            Input parameter matrix filled with appropriate data.
      */
    KFbxXMatrix& GetTransformAssociateModelMatrix(KFbxXMatrix& pMatrix) const;

    /** Set matrix associated with the parent node.
      * \param pMatrix     Transformation matrix.
      */
    void SetTransformParentMatrix(const KFbxXMatrix& pMatrix);

    /** Get matrix associated with the parent node.
      * \param pMatrix     Transformation matrix to be filled with appropriate data..
      * \return            Input parameter matrix filled with appropriate data.
      */
    KFbxXMatrix& GetTransformParentMatrix(KFbxXMatrix& pMatrix) const;

    /** Get the Transform Parent set flag value.
      * \return           \c true if transform matrix associated with parent node is set.
      */
    bool IsTransformParentSet() const { return mIsTransformParentSet; }

    //@}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

    virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
    KFbxCluster(KFbxSdkManager& pManager, char const* pName);

    virtual void Construct(const KFbxCluster* pFrom);
    virtual bool ConstructProperties( bool pForceSet );

    virtual KStringList GetTypeFlags() const;

    /**
      * \name User Data
      * Service functions to store and retrieve extra data about the link.
      * Only used by the Maya plugin, should move somewhere out of here...
      */
    //@{

  public:
    /** Set user data.
      * \param pUserDataID Identifier of user data.
      * \param pUserData User data.
      */
    void SetUserData(KString pUserDataID, KString pUserData);

    //! Get the user data identifier.
    KString GetUserDataID () const;

    //! Get the user data.
    KString GetUserData () const;

    //! Get the user data by identifier.
    KString GetUserData (KString pUserDataID) const;

    //@}

protected:

    //  Cluster deformer
    ELinkMode               mLinkMode;
    KString                 mUserDataID;
    KString                 mUserData;
    KArrayTemplate<int>     mControlPointIndices;
    KArrayTemplate<double>  mControlPointWeights;
    KFbxMatrix              mTransform;
    KFbxMatrix              mTransformLink;
    KFbxMatrix              mTransformAssociate;
    KFbxMatrix              mTransformParent;
    bool                    mIsTransformParentSet;

public:
    // For pre version 6 support
    KString                 mBeforeVersion6LinkName;
    KString                 mBeforeVersion6AssociateModelName;

protected:
    // Properties
    KFbxTypedProperty<fbxReference> SrcModel;
    KFbxTypedProperty<fbxReference> SrcModelReference;

    friend class KFbxScene;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxCluster* HKFbxCluster;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXCLUSTER_H

