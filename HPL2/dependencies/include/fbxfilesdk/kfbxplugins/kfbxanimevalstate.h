/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxanimevalstate.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXEVALUATION_STATE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXEVALUATION_STATE_H

#include <fbxfilesdk/kfbxplugins/kfbxnode.h>
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KMBTransform;
class KFbxNodeEvalState;
struct KFbxAnimEvalState_prv;

/** This class hold results from animation evaluations. To clear an evaluation state for re-use, it is possible to invalidate
  * or to reset it. For the same scene with the same objects, invalidating an evaluation state is the quickest way to clear
  * an evaluation state object for re-use because it only zeroes all the entries. A reset will delete all the entries.
  * Unless the scene changes, for performance purposes it is recommended to invalidate evaluation states instead of resetting them.
  * 
  * \internal
  * \see KFbxAnimEvaluator
  */
class KFBX_DLL KFbxAnimEvalState
{
public:
    /** Constructor.
      */
    KFbxAnimEvalState();

    /** Get the time associated with this evaluation state.
      * \return The time associated with this evaluation state.
      */
    KTime GetTime() const;

    /** Reset an evaluation state by deleting the cache it contains. This will remove all entries in the cache.
      */
    void Reset();

    /** Invalidate an evaluation state by zeroing the cache it contains, and changing its associated time. All
	  * node and property entries will remain in the list, but will become in an not-evaluated state.
      * \param pTime The time at which the evaluation state should be set after the invalidation.
      */
    void Invalidate(KTime pTime);

    /** Invalidate a node evaluation state.
      * \param pNode The node that needs to be re-evaluated in next evaluation.
      */
	void InvalidateNode(KFbxNode* pNode);

    /** Invalidate a property evaluation state.
      * \param pProperty The property that needs to be re-evaluated in next evaluation.
      */
	void InvalidateProperty(KFbxProperty& pProperty);

    /** Get node transform evaluation result from the evaluation state.
      * \param pNode The node for which the value was stored.
      * \param pDirectIndex Index to retrieve the information in the evaluation state node list, to speed up performance. (Use -1 if index is unknown).
      * \param pNeedEval The function will set this parameter to \c true if the value in the state needs a re-evaluation.
      * \return The global or local matrix transform for the specified node.
      */
	KFbxNodeEvalState* GetNodeTransform(KFbxNode* pNode, int& pDirectIndex, bool& pNeedEval);

	/** Set the evaluation state of a node to "evaluated".
	  * Each node in this evaluation state can become "evaluated", which means this evaluation state contains information 
	  * about this node. Please remember that an evaluation state contains a time, so evaluated nodes must match that time.
      *	Calling this method will set the flag to true for this node, as if it was already being evaluated.
	  * \param pDirectIndex The index of the node in the evaluation state node list. 
	  *                     You may call GetNodeTransform to retrieve this index.
	  * \remarks Not all nodes get evaluated at the same time, it is only true for nodes that were evaluated by the evaluator.
	  *          So in most cases, an evaluation state contains both evaluated and not-evaluated nodes. 
	  *			 Sometimes, if you do not want a node to get evaluated, you can set it as "evaluated".
	  *          A good example is if you create a camera manipulator, you probably do not want the system to replace 
	  *			 the camera on every redraw, so you can set it as evaluated and then it won't get re-evaluated.
	  */
	void SetNodeEvaluated(int pDirectIndex);

    /** Get a property evaluation result from the evaluation state.
      * \param pProperty The property for which the value was stored.
      * \param pDirectIndex Index to retrieve the information in the evaluation state property list, to speed up performance. (Use -1 if index is unknown).
	  * \param pNeedEval The function will set this parameter to \c true if the value in the state needs a re-evaluation.
	  * \param pScene The FBX scene used for evaluation.
      * \return The result value that was stored.
      * \remarks This function is not well suited for real-time applications, since it
      *          performs a find in the array. But it is reliable and simple to use.
      */
    KFbxAnimCurveNode* GetPropertyValue(KFbxProperty& pProperty, int& pDirectIndex, bool& pNeedEval, KFbxScene* pScene);

	/** Set the evaluation state of a property to "evaluated".
	* Each property in this evaluation state can become "evaluated", which means this evaluation state contains information 
	* about this property. Please remember that an evaluation state contains a time, so evaluated properties must match that time.
	* Calling this method will set the flag to true for this property, as if it was already being evaluated.
	* \param pDirectIndex The index of the property in the evaluation state property list. 
	*                     You may call GetPropertyValue to retrieve this index.
	* \remarks Not all properties get evaluated at the same time, it is only true for properties that were evaluated by the evaluator.
	*          So in most cases, an evaluation state contains both evaluated and not-evaluated properties. 
	*		   Sometimes, if you do not want a property to get evaluated, you can set it as "evaluated".
	*/
	void SetPropertyEvaluated(int pDirectIndex);

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//    Anything beyond these lines may not be documented accurately and is 
//     subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    virtual ~KFbxAnimEvalState();

private:
    KFbxAnimEvalState_prv* mData;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 
};

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//    Anything beyond these lines may not be documented accurately and is 
//     subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/** This class hold results for node evaluation.
  * \nosubgrouping
  */
class KFBX_DLL KFbxNodeEvalState
{
public:
	KFbxNodeEvalState(KFbxNode* pNode);

	/** mLT is used to hold result value of LclTranslation property from node evaluation.
	*/
	KFbxVector4         mLT;
	/** mLR is used to hold result value of LclRotation property from node evaluation.
	*/
	KFbxVector4         mLR;
	/** mLS is used to hold result value of LclScaling property from node evaluation.
	*/
	KFbxVector4         mLS;

	/** mLX is used to hold result local transform matrix from node evaluation.
	* Pivots, offsets, pre/post rotation and all other transforms are taken into consideration.
	*/
	KFbxXMatrix			mLX;
	/** mGX is used to hold result global transform matrix from node evaluation.
	* Pivots, offsets, pre/post rotation and all other transforms are taken into consideration.
	*/	
	KFbxXMatrix			mGX;
	/** mMBX is used to hold the corresponding KMBTransform of the node.
	* This KMBTransform takes all transform-related info, including pivots, offsets, pre/post rotation, rotation order, limits, etc.
	* The evaluation is actually done through the utility functions of KMBTransform.
	*/
	KMBTransform*		mMBX;

	int					mParentIndex, mTargetIndex, mTargetUpIndex;
	KFbxAnimCurveNode*	mCurveNode[3];
	KFbxAnimLayer*		mLayer;
};

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif /* FBXFILESDK_KFBXPLUGINS_KFBXEVALUATION_STATE_H */
