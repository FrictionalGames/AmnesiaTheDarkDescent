/*!  \file kfbxskin.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSKIN_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSKIN_H

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

#include <fbxfilesdk/kfbxplugins/kfbxdeformer.h>
#include <fbxfilesdk/kfbxmath/kfbxmatrix.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;
class KFbxGeometry;
class KFbxCluster;
class KFbxNode;

/** FBX SDK skin class
  * \nosubgrouping
  */

class KFBX_DLL KFbxSkin : public KFbxDeformer
{
    KFBXOBJECT_DECLARE(KFbxSkin,KFbxDeformer);

public:
    /** Set deformation accuracy.
      * \param pDeformAccuracy         value for deformation accuracy.
      */
    void SetDeformAccuracy(double pDeformAccuracy);

    /** Get deformation accuracy.
      * \return                        deformation accuracy value.
      */
    double GetDeformAccuracy() const;

    /** Set the geometry affected by this skin deformer.
      * \param pGeometry               Pointer to the geometry object to set.
      * \return                        \c true on success, \c false otherwise.
      */
    bool SetGeometry(KFbxGeometry* pGeometry);

    /** Get the geometry affected by this skin deformer.
      * \return                        a pointer to the geometry if set or NULL.
      */
    KFbxGeometry* GetGeometry();

    /** Add a cluster.
      * \param pCluster                Pointer to the cluster object to add.
      * \return                        \c true on success, \c false otherwise.
      */
    bool AddCluster(KFbxCluster* pCluster);

    /** Remove cluster at given index.
      * \param pCluster                Pointer to the cluster to remove from this skin deformer.
      * \return                        Pointer to cluster or \c NULL if pCluster is not owned by this skin deformer.
      */
    KFbxCluster* RemoveCluster(KFbxCluster* pCluster);

    /** Get the number of clusters.
      * \return                        Number of clusters that have been added to this object.
      */
    int GetClusterCount() const;

    /** Get cluster at given index.
      * \param pIndex                  Index of cluster.
      * \return                        Pointer to cluster or \c NULL if index is out of range.
      */
    KFbxCluster* GetCluster(int pIndex);

    /** Get cluster at given index.
      * \param pIndex                  Index of cluster.
      * \return                        Pointer to cluster or \c NULL if index is out of range.
      */
    KFbxCluster const* GetCluster(int pIndex) const;

    /** Get the type of the deformer.
      * \return                        Deformer type identifier.
      */
    EDeformerType GetDeformerType()  const {return eSKIN; };

	/** \enum ESkinningType Skinning type.
	* The skinning type decides which method will be used to do the skinning.
	*      - \e eRIGID                       Type eRIGID means rigid skinning, which means only one joint can influence each control point.
	*      - \e eLINEAR                      Type eLINEAR means the classic linear smooth skinning.
	*      - \e eDUALQUATERNION              Type eDUALQUATERNION means the dual quaternion smooth skinning.
	*      - \e eBLEND                       Type eBLEND means to blend classic linear and dual quaternion smooth skinning according to blend weights.
	*      - \e eSKINNINGTYPE_COUNT          eSKINNINGTYPE_COUNT represents the total number of skinning methods.
	*/
	typedef enum
	{
		eRIGID,
		eLINEAR,
		eDUALQUATERNION,
		eBLEND,
		eSKINNINGTYPE_COUNT
	} ESkinningType;

	/** Set the skinning type.
	* \param pMode     The skinning type.
	*/
	void SetSkinningType(ESkinningType pType);

	/** Get the skinning type.
	* \return     The skinning type.
	*/
	ESkinningType GetSkinningType() const;

	/**
	* \name Control Points
	* A skin has an array of indices to control points and associated blend weights.
	* The indices refer to the control points in the instance of class KFbxGeometry. 
	* The blend weights are the influence of the different skinning type over the
	* deformation effect of the indexed control points.
	*/
	//@{

	/** Add an element in both arrays of control point indices and blendWeights.
	* \param pIndex          The index of the control point.
	* \param pBlendWeight    The blend weight for this control point. The value should between 0 and 1. 
	*                        Any value that is less than 0 will be set to 0, any value that is greater than 1 will be set to 1.
	*                        0 means completely linear skinning, 1 means completely dual quaternion skinning,
	*                        a value between 0 and 1 means the weighted blending of the above two skinning methods.
	*/
	void AddControlPointIndex(int pIndex, double pBlendWeight = 0);

	/** Get the length of the arrays of control point indices and blend weights.
	* \return     Length of the arrays of control point indices and blend weights.
	*             Returns 0 if no control point indices have been added or the arrays have been reset.
	*/
	int GetControlPointIndicesCount() const;

	/** Get the array of control point indices.
	* \return     Pointer to the array of control point indices.
	*             \c NULL if no control point indices have been added or the array has been reset.
	*/
	int* GetControlPointIndices() const;

	/** Get the array of control point blend weights.
	* \return     Pointer to the array of control point blend weights.
	*             \c NULL if no control point indices have been added or the array has been reset.
	*/
	double* GetControlPointBlendWeights() const;

	/** Set the array size for the three arrays: the array of control point indices, the array of weights
	* and the array of blend weights.
	* \param pCount The new count.
	*/
	void SetControlPointIWCount(int pCount);

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
    KFbxSkin(KFbxSdkManager& pManager, char const* pName);

    virtual KStringList GetTypeFlags() const;

    // Skin deformer
    double mDeformAccuracy;
	ESkinningType mSkinningType;

	//Control points
	KArrayTemplate<int>     mControlPointIndices;
	KArrayTemplate<double>  mControlPointBlendWeights;

    friend class KFbxScene;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxSkin* HKFbxSkin;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSKIN_H

