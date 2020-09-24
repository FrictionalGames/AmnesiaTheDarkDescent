/*!  \file kfbxanimevaluator.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXANIMEVALUATOR_H
#define FBXFILESDK_KFBXPLUGINS_KFBXANIMEVALUATOR_H

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
#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxplugins/kfbxanimevalstate.h>
#include <fbxfilesdk/kfbxplugins/kfbxanimstack.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

struct KFbxAnimEvaluator_prv;

/** The principal interface for animation evaluators. The animation evaluator is used to compute node transforms
  * and property values at specific times during an animation. Evaluators simplify the process of computing transform 
  * matrices by taking into account all of the parameters, such as pre- and post-rotations.
  * This class is abstract so that SDK users can implement their own evaluator if needed. The default evaluator used
  * by the FBX SDK is a KFbxAnimEvalClassic. The default evaluator can be queried with the function 
  * KFbxScene::GetEvaluator(), and can be changed using KFbxScene::SetEvaluator().
  *
  * When working with scene nodes, the evaluator will always return an affine transform matrix that contains the
  * translation, rotation and scale of that node.
  *
  * When working with object properties, the evaluator will always return a structure that can contain as many components
  * as the property can have. For example, an RGB color property would return a structure containing 3 channels. The 
  * class KFbxAnimCurveNode is used as a data container to store those values, because it can handle as many channels as 
  * needed, even if the property is not a real curve node .
  *
  * Below is a typical usage of the evaluator class to retrieve the global transform matrix of each node in a scene:
  * \code
  * //Here we assume the user already imported a scene...
  * for( int i = 0, c = MyScene->GetMemberCount(KFbxNode::ClassId); i < c; ++i )
  * {
  *     KFbxNode* CurrentNode = MyScene->GetMember(KFbxNode::ClassId, i);
  *     KFbxXMatrix& NodeGlobalTransform = MyScene->GetEvaluator()->GetNodeGlobalTransform(CurrentNode);
  * }
  *
  * //There is an equivalent call to retrieve a node's global transform, which is exactly the same as calling Scene->GetEvaluator() :
  * KFbxXMatrix& NodeGlobalTransform = CurrentNode->EvaluateGlobalTransform();
  * \endcode
  *
  * Another typical usage of the evaluator class, but this time to retrieve the value of an animated color property on a material:
  * \code
  * //Assuming the user imported a scene with objects and materials...
  * KFbxAnimCurveNode& Color = MyScene->GetEvaluator()->GetPropertyValue(MyMaterial->GetDiffuseColor());
  *
  * //The first parameter represent the index of the component for the property.
  * //The second parameter value of GetChannelValue is the default value in case the component is not animated.
  * float Red = Color.GetChannelValue<float>(0, 0.0f);
  * float Green = Color.GetChannelValue<float>(1, 0.0f);
  * float Blue = Color.GetChannelValue<float>(2, 0.0f);
  * \endcode
  *
  * \note Note that all the methods to retrieve global/local matrices as well as property values returns references. 
  * This is important for performance purposes, to prevent an extra memory copy.
  * \see KFbxScene, KFbxAnimEvalClassic, KFbxAnimCurveNode
  */
class KFBX_DLL KFbxAnimEvaluator : public KFbxObject
{
    KFBXOBJECT_DECLARE_ABSTRACT(KFbxAnimEvaluator, KFbxObject);

public:
    /** Set the evaluator context, which represent which animation stack should be evaluated. When no context is specified, the FBX SDK
	  * will try to automatically pick the first animation stack available in the scene and set it as the current context. If no animation
	  * stack are available, the evaluator will not be able to evaluate the scene's animation. Only one context can be evaluated at a time per evaluator.
      * \param pAnimStack The animation stack to evaluate when using this evaluator.
	  * \remark When changing the current context, the evaluator will automatically clear any animation evaluation cache present in memory.
	  * \see KFbxAnimStack
      */
    void SetContext(KFbxAnimStack* pAnimStack);

    /** Get the current evaluator context.
	  * \return The current animation stack used by the evaluator.
      * \remarks When no context is specified, the FBX SDK will try to automatically pick the first animation stack available
	  * in the scene and set it as the current context.
      */
	KFbxAnimStack* GetContext();

    /** Returns a node's global transformation matrix at the specified time. The node's translation, rotation and scaling limits are taken into consideration.
      * \param pNode The node to evaluate.
      * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	  * \param pPivotSet The pivot set to take into account
	  * \param pApplyTarget Applies the necessary transform to align into the target node
	  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
	  * \return The resulting global transform of the specified node at the specified time.
      */
	KFbxXMatrix& GetNodeGlobalTransform(KFbxNode* pNode, KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);

    /** Returns a node's local transformation matrix at the specified time. The node's translation, rotation and scaling limits are taken into consideration.
      * \param pNode The node to evaluate.
      * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	  * \param pPivotSet The pivot set to take into account
	  * \param pApplyTarget Applies the necessary transform to align into the target node
	  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
	  * \return The resulting local transform of the specified node for the specified time.
	  * \remarks The local transform matrix is calculated in this way: ParentGlobal.Inverse * Global, all transforms such as pre/post rotation are taken into consideration.
	  * This will return a different value than LclTranslation, LclRotation and LclScaling at the specified time. To evaluate these properties separately
	  * without taking pre/post rotation, pivots and offsets into consideration, please use GetNodeLocalTranslation(), GetNodeLocalRotation() and GetNodeLocalScaling().
      */
    KFbxXMatrix& GetNodeLocalTransform(KFbxNode* pNode, KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);

	/** Returns the value of a node's LclTranslation property at the specified time. 
	  * No pivot, offsets, or any other transform is taken into consideration. The translation limit is applied.
	  * \param pNode The transform node to evaluate.
	  * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	  * \param pPivotSet The pivot set to take into account
	  * \param pApplyTarget Applies the necessary transform to align into the target node
	  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
	  * \return The resulting value of LclTranslation property of the specified node at the specified time.
	  */
	KFbxVector4& GetNodeLocalTranslation(KFbxNode* pNode, KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);

	/** Returns the value of a node's LclRotation property at the specified time. 
	  * No pre/post rotation, rotation pivot, rotation offset or any other transform is taken into consideration. The rotation limit is applied.
	  * \param pNode The transform node to evaluate.
	  * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	  * \param pPivotSet The pivot set to take into account
	  * \param pApplyTarget Applies the necessary transform to align into the target node
	  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
	  * \return The resulting value of LclRotation property of the specified node at the specified time.
	  */
	KFbxVector4& GetNodeLocalRotation(KFbxNode* pNode, KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);

	/** Returns the value of a node's LclScaling property at the specified time. 
	  * No scaling pivot, scaling offset or any other transform is taken into consideration. The scaling limit is applied.
	  * \param pNode The transform node to evaluate.
	  * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	  * \param pPivotSet The pivot set to take into account
	  * \param pApplyTarget Applies the necessary transform to align into the target node
	  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
	  * \return The resulting value of LclScaling property of the specified node at the specified time.
	  */
	KFbxVector4& GetNodeLocalScaling(KFbxNode* pNode, KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);


	/** Get a property's value at the specified time.
      * \param pProperty The property to evaluate.
      * \param pTime The time used for evaluate.
	  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
      * \return The property value at the specified time stored in a curve node structure for easier access of complex types.
      */
    KFbxAnimCurveNode& GetPropertyValue(KFbxProperty& pProperty, KTime pTime, bool pForceEval=false);

    /** Validate if the given time value is within animation stack time span range.
      * \param pTime The time value to validate.
      * \return The new validated time, clamped by the animation stack time span range.
	  * \remarks If no animation stack are found, time zero is returned. This function is not used by the evaluator itself.
      */
    KTime ValidateTime(KTime pTime);

    /** A faster version of GetNodeGlobalTransform aimed at real-time applications. The optimization is achieved by providing an
	  * integer storage address that can be re-used for much faster access to the evaluation state cache result slot.
      * \param pNode The transform node to evaluate.
      * \param pDirectIndex The index used to retrieve the information in the evaluation state. The first time you evaluate a specific node,
	  * it must be set to -1. Then the returned index can be used for a quicker access to the evaluation state array by eliminating the search.
      * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	  * \param pPivotSet The pivot set to take into account.
	  * \param pApplyTarget Applies the necessary transform to align into the target node.
	  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
	  * \return The resulting global transform of the specified node at the specified time.
      * \remarks If the scene change in the application, all direct indexes must be updated, and the evaluation state cache must be invalidated.
	  *  The translation, rotation and scaling limits are taken into consideration.
      */
	KFbxXMatrix& GetNodeGlobalTransformFast(KFbxNode* pNode, int& pDirectIndex, KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);

    /** A faster version of GetNodeLocalTransform aimed at real-time applications. The optimization is achieved by providing an
	  * integer storage address that can be re-used for much faster access to the evaluation state cache result slot.
      * \param pNode The transform node to evaluate.
      * \param pDirectIndex The index used to retrieve the information in the evaluation state. The first time you evaluate a specific node,
	  * it must be set to -1. Then the returned index can be used for a quicker access to the evaluation state array by eliminating the search.
      * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	  * \param pPivotSet The pivot set to take into account.
	  * \param pApplyTarget Applies the necessary transform to align into the target node.
	  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
	  * \return The resulting local transform of the specified node for the specified time.
      * \remarks If the scene change in the application, all direct indexes must be updated, and the evaluation state cache must be invalidated.
	  * \The translation, rotation and scaling limits are taken into consideration.
	  * The local transform matrix is calculated in this way: ParentGlobal.Inverse * Global, all transforms such as pre/post rotation are taken into consideration.
	  * So to get TRS from returned matrix, it may different with values of LclTranslation, LclRotaion and LclScaling at specified time.
	  * To get values of properties LclTranslation, LclRotaion and LclScaling at the specified time, please use 
	  * GetNodeLocalTranslationFast(), GetNodeLocalRotationFast() and GetNodeLocalScalingFast().
      */
	KFbxXMatrix& GetNodeLocalTransformFast(KFbxNode* pNode, int& pDirectIndex, KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);


	/** A faster version of GetNodeLocalTranslation aimed at real-time applications. The optimization is achieved by providing an
	* integer storage address that can be re-used for much faster access to the evaluation state cache result slot.
	* \param pNode The transform node to evaluate.
	* \param pDirectIndex The index used to retrieve the information in the evaluation state. The first time you evaluate a specific node,
	* it must be set to -1. Then the returned index can be used for a quicker access to the evaluation state array by eliminating the search.
	* \param pTime The time used for evaluate.If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	* \param pPivotSet The pivot set to take into account.
	* \param pApplyTarget Applies the necessary transform to align into the target node
	* \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
	* \return The resulting value of LclTranslation property of the specified node at the specified time.
	* \remarks If the scene change in the application, all direct indexes must be updated, and the evaluation state cache must be invalidated.
	* Also, the translation limit is taken into consideration.
	*/
	KFbxVector4& GetNodeLocalTranslationFast(KFbxNode* pNode, int& pDirectIndex, KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);
  
	/** A faster version of GetNodeLocalRotation aimed at real-time applications. The optimization is achieved by providing an
	* integer storage address that can be re-used for much faster access to the evaluation state cache result slot.
	* \param pNode The transform node to evaluate.
	* \param pDirectIndex The index used to retrieve the information in the evaluation state. The first time you evaluate a specific node,
	* it must be set to -1. Then the returned index can be used for a quicker access to the evaluation state array by eliminating the search.
	* \param pTime The time used for evaluate.If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	* \param pPivotSet The pivot set to take into account.
	* \param pApplyTarget Applies the necessary transform to align into the target node.
	* \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
	* \return The resulting local rotation of the specified node at the specified time.
	* \remarks Only calculate value of LclRotation property, no pre/post rotation, rotation pivot, rotation offset or any other transform are taken into account.
	* If the scene change in the application, all direct indexes must be updated, and the evaluation state cache must be invalidated.
	* Also, the rotation limit is taken into consideration.
	*/
	KFbxVector4& GetNodeLocalRotationFast(KFbxNode* pNode, int& pDirectIndex, KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);
  
	/** A faster version of GetNodeLocalScaling aimed at real-time applications. The optimization is achieved by providing an
	* integer storage address that can be re-used for much faster access to the evaluation state cache result slot.
	* \param pNode The transform node to evaluate.
	* \param pDirectIndex The index used to retrieve the information in the evaluation state. The first time you evaluate a specific node,
	* it must be set to -1. Then the returned index can be used for a quicker access to the evaluation state array by eliminating the search.
	* \param pTime The time used for evaluate.If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	* \param pPivotSet The pivot set to take into account.
	* \param pApplyTarget Applies the necessary transform to align into the target node.
    * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
	* \return The resulting local scaling of the specified node at the specified time.
	* \remarks Only calculate value of LclScaling property, no scaling pivot, scaling offset or any other transform are taken into account.
	* If the scene change in the application, all direct indexes must be updated, and the evaluation state cache must be invalidated.
	* Also, the scaling limit is taken into consideration.
	*/
	KFbxVector4& GetNodeLocalScalingFast(KFbxNode* pNode, int& pDirectIndex, KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);


    /** A faster version of GetPropertyValue aimed at real-time applications. The optimization is achieved by providing an
	  * integer storage address that can be re-used for much faster access to the evaluation state cache result slot.
      * \param pProperty The property to evaluate.
      * \param pDirectIndex The index used to retrieve the information in the evaluation state. The first time you evaluate a specific property,
	  *                     it must be set to -1. Then the returned index can be used for a quicker access to the evaluation state array.
	  * \param pTime The time used for evaluate.If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
      * \return The property value at the specified time stored in a curve node structure for easier access of complex types.
      * \remarks If the scene change in the application, all direct indexes must be updated, and the evaluation state cache must be invalidated.
      */
    KFbxAnimCurveNode& GetPropertyValueFast(KFbxProperty& pProperty, int& pDirectIndex, KTime pTime=KTIME_INFINITE, bool pForceEval=false);

	/** Completely reset the evaluation state cache by deleting all entries. This reset also happens when changing the current context. This
	  * function is very useful in case the scene structure change, as noted in the remarks of fast functions.
	  */
	void ResetEvaluationState();

    /** Clears the specified node evaluation state cache, so the next time the evaluation is called for this node it get refreshed.
      * \param pNode The node that needs to be re-evaluated in next evaluation.
      */
	void InvalidateNode(KFbxNode* pNode);

    /** Clears the specified property evaluation state cache, so the next time the evaluation is called for this property it get refreshed.
      * \param pProperty The property that needs to be re-evaluated in next evaluation.
      */
	void InvalidateProperty(KFbxProperty& pProperty);

	/** Compute node local TRS from global transform. Doesn't change cached state for current time.
	* \retval pRetLT Computed local translation.
	* \retval pRetLR Computed local rotation.
	* \retval pRetLS Computed local scaling.
	* \param pNode The transform node to evaluate.
	* \param pDirectIndex The index used to retrieve the information in the evaluation state. The first time you evaluate a specific node,
	* it must be set to -1. Then the returned index can be used for a quicker access to the evaluation state array by eliminating the search.
	* \param pGX Global transformation state .
	* \param pTime The time used for evaluate.If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	* \param pPivotSet The pivot set to take into account.
	* \param pApplyTarget Applies the necessary transform to align into the target node.
	* \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
	* \remarks If the scene change in the application, all direct indexes must be updated, and the evaluation state cache must be invalidated.
	*/
	virtual void ComputeLocalTRSFromGlobal(	KFbxVector4 &pRetLT, KFbxVector4 &pRetLR, KFbxVector4 &pRetLS, KFbxNode* pNode, int& pDirectIndex, KFbxXMatrix &pGX,
											KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false) = 0;


///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//    Anything beyond these lines may not be documented accurately and is 
//     subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    virtual void EvaluateNodeTransform(KFbxNodeEvalState* pResult, KFbxNode* pNode, KTime pTime, KFbxAnimStack* pStack, KFbxNode::EPivotSet pPivotSet, bool pApplyTarget) = 0;
    virtual void EvaluatePropertyValue(KFbxAnimCurveNode* pResult, KFbxProperty& pProperty, KTime pTime, KFbxAnimStack* pStack) = 0;

    KFbxAnimEvaluator(KFbxSdkManager& pManager, char const* pName);
    virtual void Destruct(bool pRecursive, bool pDependents);

	KFbxAnimEvalState*	GetEvalState(KTime pTime);
	KFbxNodeEvalState*	GetNodeEvalState(KFbxNode* pNode, int& pDirectIndex, KTime pTime, KFbxNode::EPivotSet pPivotSet, bool pApplyTarget, bool pForceEval);

private:
    KFbxAnimEvaluator_prv* mData;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif /* FBXFILESDK_KFBXPLUGINS_KFBXANIMEVALUATOR_H */
