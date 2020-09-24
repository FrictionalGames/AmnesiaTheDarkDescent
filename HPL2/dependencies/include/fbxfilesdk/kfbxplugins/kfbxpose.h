/*!  \file kfbxpose.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXPOSE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXPOSE_H

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

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/kname.h>
#include <fbxfilesdk/kfbxmath/kfbxmatrix.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** This structure contains the description of a named pose.
  * KFbxPose contains one KFbxPoseInfo array to store all of FBX nodes and their transform matrix info.
  */
typedef struct
{
    //! Transform matrix of the node.
    KFbxMatrix  mMatrix;
    //! If true, the transform matrix above is defined in local coordinates.
    bool        mMatrixIsLocal;
    //! FBX node, which may be skeleton or geometry (skinned) node.
    KFbxNode*   mNode;

} KFbxPoseInfo;

class KFbxAnimStack;
class KFbxScene;
class KFbxUserNotification;
class KFbxPose;

typedef KArrayTemplate<KFbxNode*> NodeList;
typedef KArrayTemplate<KFbxPose*> PoseList;
typedef KArrayTemplate<KFbxPoseInfo*> PoseInfoList;

/** This class contains the description of a Pose and provide some methods to access Pose info in one FBX scene.
  * \nosubgrouping
  * The KFbxPose object can be setup to hold "Bind Pose" data or "Rest Pose" data.
  *
  * The Bind Pose holds the transformation (translation, rotation and scaling)
  * matrix of all the nodes implied in a link deformation. This includes the geometry
  * being deformed, the links deforming the geometry, and recursively all the
  * ancestors nodes of the link. The Bind Pose gives you the transformation of the nodes
  * at the moment of the binding operation when no deformation occurs.
  *
  * The Rest Pose is a snapshot of a node transformation. A Rest Pose can be used
  * to store the position of every node of a character at a certain point in
  * time. This pose can then be used as a reference position for animation tasks,
  * like editing walk cycles.
  *
  * One difference between the two modes is in the validation performed before
  * adding an item and the kind of matrix stored.
  *
  * In "Bind Pose" mode, the matrix is assumed to be defined in the global space,
  * while in "Rest Pose" the type of the matrix may be specified by the caller. So
  * local system matrices can be used. Actually, because there is one such flag for
  * each entry (KFbxPoseInfo), it is possible to have mixed types in a KFbxPose elements.
  * It is therefore the responsibility of the caller to check for the type of the retrieved
  * matrix and to do the appropriate conversions if required.
  *
  * The validation of the data to be added consists of the following steps:
  *
  *     \li If this KFbxPose object stores "Bind Poses", then
  *        add a KFbxPoseInfo only if the node is not already
  *        associated to another "Bind Pose". This check is done
  *        by visiting ALL the KFbxPose objects in the system.
  *
  *        The above test is only performed for the "Bind Pose" type. While
  *        the next one is always performed, no matter what kind of poses this
  *        KFbxPose object is setup to hold.
  *
  *     \li If a node is already inserted in the KFbxPose internal list,
  *        then the passed matrix MUST be equal to the one already stored.
  *        If this is not the case, the Add method will return -1, indicating
  *        that no new KFbxPoseInfo has been created.
  *
  * If the Add method succeeds, it will return the index of the KFbxPoseInfo
  * structure that as been created and held by the KFbxPose object.
  *
  * To ensure data integrity, the stored information can only be
  * accessed using the provided methods (read-only). If an entry needs to be
  * modified, the caller has to remove the KFbxPoseInfo item by calling Remove(i)
  * and then Add a new one.
  *
  * The internal list is not ordered and the search inside this list is linear
  * (from the first element to ... the first match or the end of the list).
  *
  */
class KFBX_DLL KFbxPose : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxPose,KFbxObject);

public:
	/** Set the type of pose.
	  * \param pIsBindPose If true, type will be bind pose, else rest pose.
	  */
	void SetIsBindPose(bool pIsBindPose);

	/** Pose identifier flag.
	  * \return \c true if this object holds BindPose data.
	  */
	bool IsBindPose() const { return mType == 'b'; }

	/** Pose identifier flag.
	  * \return \c true if this object holds RestPose data.
	  */
	bool IsRestPose() const { return mType == 'r'; }

	/** Get number of stored items.
	  * \return The number of items stored.
	  */
	int GetCount() const { return mPoseInfo.GetCount(); }

	/** Stores the pose transformation for the given node.
	  * \param pNode pointer to the node for which the pose is stored.
	  * \param pMatrix Pose transform of the node.
	  * \param pLocalMatrix Flag to indicate if pMatrix is defined in Local or Global space.
	  * \param pMultipleBindPose Flag to indicate if multiple bind pose exist. If this is false, all matrix for one node should be same in different bind pose.
	  * \return -1 if the function failed or the index of the stored item.
	  */
	int Add(KFbxNode* pNode, const KFbxMatrix& pMatrix, bool pLocalMatrix = false, bool pMultipleBindPose = true);

	/** Remove the pIndexth item from the Pose object.
	  * \param pIndex Index of the item to be removed.
	  */
	void Remove(int pIndex);

	/** Get the node name.
	  * \param pIndex Index of the queried item.
	  * \return The node initial and current names.
	  * \remarks If the index is invalid an empty KName is returned.
	  */
	KName GetNodeName(int pIndex) const;

	/** Get the node.
	  * \param pIndex Index of the queried item.
	  * \return A pointer to the node referenced.
	  * \remarks If the index is invalid or no pointer to a node is set, returns NULL.
	  *  The returned pointer will become undefined if the KFbxPose object is destroyed.
	  */
	KFbxNode* GetNode(int pIndex) const;

	/** Get the transform matrix.
	  * \param pIndex Index of the queried item.
	  * \return A reference to the pose matrix.
	  * \remarks If the index is invalid a reference to an identity matrix is returned.
	  *  The reference will become undefined if the KFbxPose object is destroyed.
	  */
	const KFbxMatrix& GetMatrix(int pIndex)       const;

	/** Get the type of the matrix.
	  * \param pIndex Index of the queried item.
	  * \return \c true if the matrix is defined in the Local coordinate space and false otherwise.
	  * \remarks If the KFbxPose object is configured to hold BindPose data, this method will always return \c false.
	  */
	bool IsLocalMatrix(int pIndex) const;

	/**
	  * \name Search Section
	  */
	//@{
		/** This structure defines the strategy of comparing FBX node name.
		  * FBX node has an initial name and a current name (refer to KName). The structure defines which name to use when compare two nodes.
		  */
		enum KNameComponent {
			INITIALNAME_COMPONENT = 1,	//! use initial name when compare two nodes
			CURRENTNAME_COMPONENT = 2,	//! use current name when compare two nodes
			ALL_NAME_COMPONENTS   = 3	//! use both initial and current name when compare two nodes, it's true if one or both matched
		};

		/** Look in the KFbxPose object for the given node name.
		  * \param pNodeName Name of the node we are looking for.
		  * \param pCompareWhat Bitwise or of the following flags: INTIALNAME_COMPONENT, CURRENTNAME_COMPONENT
		  * \return -1 if the node is not in the list. Otherwise, the index of the corresponding KFbxPoseInfo element.
		  */
		int Find(const KName& pNodeName, char pCompareWhat = ALL_NAME_COMPONENTS) const;

		/** Look in the KFbxPose object for the given node.
		  * \param pNode the node we are looking for.
		  * \return -1 if the node is not in the list. Otherwise, the index of the corresponding KFbxPoseInfo element.
		  */
		int Find(const KFbxNode* pNode) const;
	//@}

	/**
	  * \name Utility Section
	  */
	//@{
		/** Get the list of Poses objects that contain the node with name pNodeName.
		  * This method will look in all the poses of all the scenes.
		  * \param pManager    The manager owning the poses and scenes.
		  * \param pNode       The node being explored.
		  * \param pPoseList   List of BindPoses/RestPoses that have the node.
		  * \param pIndex      List of indices of the nodes in the corresponding poses lists.
		  * \return \c true if the node belongs to at least one Pose (either a BindPose or a RestPose).
		  * \remarks The pPoseList and pIndex are filled by this method.
		  *  The elements of the returned list must not be deleted since they still belong to the scene.
		  */
		static bool GetPosesContaining(KFbxSdkManager& pManager, KFbxNode* pNode, PoseList& pPoseList, KArrayTemplate<int>& pIndex);

		/** Get the list of Poses objects that contain the node with name pNodeName.
		  * \param pScene     Scene owning the poses.
		  * \param pNode      The node being explored.
		  * \param pPoseList  List of BindPoses/RestPoses that have the node.
		  * \param pIndex     List of indices of the nodes in the corresponding poses lists.
		  * \return \c true if the node belongs to at least one Pose (either a BindPose or a RestPose).
		  * \remarks The pPoseList and pIndex are filled by this method.
		  *  The elements of the returned list must not be deleted since they still belong to the scene.
		  */
		static bool GetPosesContaining(KFbxScene* pScene, KFbxNode* pNode, PoseList& pPoseList, KArrayTemplate<int>& pIndex);

		/** Get the list of BindPose objects that contain the node with name pNodeName.
		  * This method will look in all the bind poses of all the scenes.
		  * \param pManager     The manager owning the poses.
		  * \param pNode        The node being explored.
		  * \param pPoseList    List of BindPoses that have the node.
		  * \param pIndex       List of indices of the nodes in the corresponding bind poses lists.
		  * \return \c true if the node belongs to at least one BindPose.
		  * \remarks The pPoseList and pIndex are filled by this method.
		  *  The elements of the returned list must not be deleted since they still belong to the scene.
		  */
		static bool GetBindPoseContaining(KFbxSdkManager& pManager, KFbxNode* pNode, PoseList& pPoseList, KArrayTemplate<int>& pIndex);

		/** Get the list of BindPose objects that contain the node with name pNodeName.
		  * \param pScene       The scene owning the poses.
		  * \param pNode        The node being explored.
		  * \param pPoseList    List of BindPoses that have the node.
		  * \param pIndex       List of indices of the nodes in the corresponding bind poses lists.
		  * \return \c true if the node belongs to at least one BindPose.
		  * \remarks The pPoseList and pIndex are filled by this method.
		  *  The elements of the returned list must not be deleted since they still belong to the scene.
		  */
		static bool GetBindPoseContaining(KFbxScene* pScene, KFbxNode* pNode, PoseList& pPoseList, KArrayTemplate<int>& pIndex);

		/** Get the list of RestPose objects that contain the node with name pNodeName.
		  * This method will look in all the bind poses of all the scenes.
		  * \param pManager     The manager owning the poses.
		  * \param pNode        The node being explored.
		  * \param pPoseList    List of RestPoses that have the node.
		  * \param pIndex       List of indices of the nodes in the corresponding rest poses lists.
		  * \return \c true if the node belongs to at least one RestPose.
		  * \remarks The pPoseList and pIndex are filled by this method.
		  *  The elements of the returned list must not be deleted since they still belong to the scene.
		  */
		static bool GetRestPoseContaining(KFbxSdkManager& pManager, KFbxNode* pNode, PoseList& pPoseList, KArrayTemplate<int>& pIndex);

		/** Get the list of RestPose objects that contain the node with name pNodeName.
		  * \param pScene       The scene owning the poses.
		  * \param pNode        The node being explored.
		  * \param pPoseList    List of RestPoses that have the node.
		  * \param pIndex       List of indices of the nodes in the corresponding rest poses lists.
		  * \return \c true if the node belongs to at least one RestPose.
		  * \remarks The pPoseList and pIndex are filled by this method.
		  *  The elements of the returned list must not be deleted since they still belong to the scene.
		  */
		static bool GetRestPoseContaining(KFbxScene* pScene, KFbxNode* pNode, PoseList& pPoseList, KArrayTemplate<int>& pIndex);

		/** Check this BindPose and report an error if all the conditions to a valid bind pose are not
		  * met. The conditions are:
		  *
		  * \li a) We are a BindPose.
		  * \li b) For every node in the bind pose, all their parent node are part of the bind pose.
		  * \li c) All the deforming nodes are part of the bind pose.
		  * \li d) All the parents of the deforming nodes are part of the bind pose.
		  * \li e) Each deformer relative matrix correspond to the deformer Inv(bindMatrix) * deformed Geometry bindMatrix.
		  *
		  * \param pRoot This node is used as the stop point when visiting the parents (cannot be NULL).
		  * \param pMatrixCmpTolerance Tolerance value when comparing the matrices.
		  * \return true if all the above conditions are met and false otherwise.
		  * \remarks If the returned value is false, querying for the error will return the reason of the failure.
		  *  As soon as one of the above conditions is not met, this method return ignoring any subsequent errors.
		  * Run the IsBindPoseVerbose if more details are needed.
		  */
		bool IsValidBindPose(KFbxNode* pRoot, double pMatrixCmpTolerance=0.0001);

		/** Same as IsValidBindPose() but slower because it will not stop as soon as a failure occurs. Instead,
		  * keeps running to accumulate the faulty nodes (stored in the appropriate array). It is then up to the
		  * caller to fill the UserNotification if desired.
		  *
		  * \param pRoot This node is used as the stop point when visiting the parents (cannot be NULL).
		  * \param pMissingAncestors Each ancestor missing from the BindPose is added to this list.
		  * \param pMissingDeformers Each deformer missing from the BindPose is added to this list.
		  * \param pMissingDeformersAncestors Each deformer ancestors missing from the BindPose is added to this list.
		  * \param pWrongMatrices Nodes that yield to a wrong matrix comparisons are added to this list.
		  * \param pMatrixCmpTolerance Tolerance value when comparing the matrices.
		  */
		bool IsValidBindPoseVerbose(KFbxNode* pRoot, NodeList& pMissingAncestors, NodeList& pMissingDeformers, NodeList& pMissingDeformersAncestors, NodeList& pWrongMatrices, double pMatrixCmpTolerance=0.0001);

		/** Same as IsValidBindPose() but slower because it will not stop as soon as a failure occurs. Instead,
		  * keeps running to accumulate the faulty nodes and send them directly to the UserNotification.
		  *
		  * \param pRoot This node is used as the stop point when visiting the parents (cannot be NULL).
		  * \param pUserNotification Pointer to the user notification where the messages will be accumulated.
		  * \param pMatrixCmpTolerance Tolerance value when comparing the matrices.
		  * \remarks If the pUserNotification parameter is NULL, this method will call IsValidBindPose().
		  */
		bool IsValidBindPoseVerbose(KFbxNode* pRoot, KFbxUserNotification* pUserNotification, double pMatrixCmpTolerance=0.0001);

	/**
	  * \name Error Management
	  */
	//@{
		/** Retrieve error object.
		  * \return Reference to error object.
		  */
		KError& GetError();

		/** \enum EError Error identifiers.
		  * - \e eERROR
		  * - \e eERROR_COUNT
		  */
		typedef enum
		{
			eERROR,
			eERROR_VALIDBINDPOSE_FAILURE_INVALIDOBJECT,
			eERROR_VALIDBINDPOSE_FAILURE_INVALIDROOT,
			eERROR_VALIDBINDPOSE_FAILURE_NOTALLANCESTORS_NODES,
			eERROR_VALIDBINDPOSE_FAILURE_NOTALLDEFORMING_NODES,
			eERROR_VALIDBINDPOSE_FAILURE_NOTALLANCESTORS_DEFNODES,
			eERROR_VALIDBINDPOSE_FAILURE_RELATIVEMATRIX,
			eERROR_COUNT
		} EError;

		/** Get last error code.
		  * \return Last error code.
		  */
		EError GetLastErrorID() const;

		/** Get last error string.
		  * \return Textual description of the last error.
		  */
		const char* GetLastErrorString() const;
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
protected:
    KFbxPose(KFbxSdkManager& pManager, char const* pName);

	virtual KFbxObject&	Copy(const KFbxObject& pObject);
    virtual void		Destruct(bool pRecursive, bool pDependents);
    virtual const char*	GetTypeName() const;

    //Returns false if pNode is already inserted in the list and the current matrix is different from the stored one. Also, if this pose is a rest pose, check if
    //pNode belongs to other BindPoses (accessed through the scene pointer). pPos will contains the index of the KFbxPoseInfo if the parameters are already stored in this object.
    bool				ValidateParams(const KFbxNode* pNode, const KFbxMatrix& pMatrix, int& pPos);

    bool				LocalValidateParams(const KFbxNode* pNode, const KFbxMatrix& pMatrix, int& pPos);
    static bool			GetSpecificPoseContaining(int poseType, KFbxScene* pScene, KFbxNode* pNode, PoseList& pPoseList, KArrayTemplate<int>& pIndex);

    KError				mError;

private:
    KFbxPoseInfo*		GetItem(int pIndex) const;
    bool				IsValidBindPoseCommon(KFbxNode* pRoot, NodeList* pMissingAncestors, NodeList* pMissingDeformers, NodeList* pMissingDeformersAncestors, NodeList* pWrongMatrices, double pMatrixCmpTolerance=0.0001);

    char				mType;
    PoseInfoList		mPoseInfo;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxPose* HKFbxPose;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXPOSE_H

