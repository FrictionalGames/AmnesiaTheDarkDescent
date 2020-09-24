/*!  \file kfbxanimevalclassic.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXEVALUATIONCLASSIC_H
#define FBXFILESDK_KFBXPLUGINS_KFBXEVALUATIONCLASSIC_H

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
#include <fbxfilesdk/kfbxplugins/kfbxanimevaluator.h>
#include <fbxfilesdk/kfbxplugins/kfbxanimlayer.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** An evaluator implementation that behaves like the original FBX SDK (2010 and previous) evaluation system.
  *
  * It works by implementing the abstract class KFbxAnimEvaluator, which is used as the main interface for evaluators.
  * \note While this class can be instanced at any time, it is preferable to access the evaluator via the function
  * KFbxScene::GetEvaluator(), which will automatically return the default evaluator used in the current FBX SDK.
  * This is very useful because it will allow the user to use the very same evaluator used by the FBX SDK internally.
  * \see KFbxAnimEvaluator, KFbxScene
  */
class KFBX_DLL KFbxAnimEvalClassic : public KFbxAnimEvaluator
{
    KFBXOBJECT_DECLARE(KFbxAnimEvalClassic, KFbxAnimEvaluator);
	
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
											KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);

public:
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
	virtual void EvaluateNodeTransform(KFbxNodeEvalState* pResult, KFbxNode* pNode, KTime pTime, KFbxAnimStack* pStack, KFbxNode::EPivotSet pPivotSet, bool pApplyTarget);
    virtual void EvaluatePropertyValue(KFbxAnimCurveNode* pResult, KFbxProperty& pProperty, KTime pTime, KFbxAnimStack* pStack);

private:
    KFbxAnimEvalClassic(KFbxSdkManager& pManager, char const* pName);
	virtual void Destruct(bool pRecursive, bool pDependents);

	enum eFbxBlendType {eFbxBlendSimple, eFbxBlendRotation, eFbxBlendScaling, eFbxBlendTypeCount};

	/** Calculate values of properties LclTranslation, LclRotation, LclScaling of a node at the specified time 
	*   and update the mLT, mLR, mLT fields of the node's NodeEvalState.
	* \param pResult The NodeEvalState to update.
	* \param pNode The node to evaluate.
	* \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	* \param pStack The current animation stack used by the evaluator.
	* \remarks Values of properties LclTranslation, LclRotation, LclScaling will be updated to pResult->mLT, pResult->mLR, pResult->mLS. 
	*  The translation, rotation and scaling limits are taken into consideration.
	*  Only LclTranslation, LclRotation and LclScaling are taken into accounts, no other transform, such as pivot, offset are calculated here.
	*/
	void ComputeTRSLocal(KFbxNodeEvalState* pResult, KFbxNode* pNode, KTime pTime, KFbxAnimStack* pStack);
	
	/** Calculate global transform of a node at the specified time and update the mGX field of the node's NodeEvalState.
	* \param pResult The NodeEvalState to update.
	* \param pNode The node to evaluate.
	* \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	* \param pStack The current animation stack used by the evaluator.
	* \param pPivotSet The pivot set to take into account.
	* \param pApplyTarget Applies the necessary transform to align into the target node
	* \remarks 	Calculated global transform will be updated to pResult->mGX.
	* ComputeGlobalTransform must be called after the call to ComputeTRSLocal, there is a dependency.
	* All transforms are taken into account, including:
	* Transform = Translation * RotationOffset* RotationPivot* PreRotation * LocalRotation* PostRotation * RotationPivotInverse* ScalingOffset* ScalingPivot* LocalScaling* ScalingPivotInverse
	* Also,the translation, rotation and scaling limits are taken into consideration.
	*/
	void ComputeGlobalTransform(KFbxNodeEvalState* pResult, KFbxNode* pNode, KTime pTime, KFbxAnimStack* pStack, KFbxNode::EPivotSet pPivotSet, bool pApplyTarget);
	
	/** Calculate local transform of a node at the specified time and update the mLX field of the node's NodeEvalState.
	* \param pResult The NodeEvalState to update.
	* \param pNode The node to evaluate.
	* \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
	* \param pStack The current animation stack used by the evaluator.
	* \param pPivotSet The pivot set to take into account.
	* \param pApplyTarget Applies the necessary transform to align into the target node
	* \remarks 	Calculated local transform will be updated to pResult->mLX.
	* ComputeLocalTransform must be called after the call to ComputeGlobalTransform, there is a dependency.
	* The local transform matrix is calculated in this way: ParentGlobal.Inverse() * Global, all transforms such as pre/post rotation are taken into consideration.
	* To get values of properties LclTranslation, LclRotaion and LclScaling at the specified time, please use ComputeTRSLocal.
	* Also,the translation, rotation and scaling limits are taken into consideration.
	*/
	void ComputeLocalTransform(KFbxNodeEvalState* pResult, KFbxNode* pNode, KTime pTime, KFbxAnimStack* pStack, KFbxNode::EPivotSet pPivotSet, bool pApplyTarget);
	
	/** Calculate values of properties LclTranslation, LclRotation, LclScaling of a node at the specified time on certain animation layer.
	* \param pResult The NodeEvalState to update.
	* \param pNode The node to evaluate.
	* \param pLT To take the calculated value of LclTranslation.
	* \param pLR To take the calculated value of LclRotation.
	* \param pLS To take the calculated value of LclScaling.
	* \param pTime The time used for evaluate. 
	* \param pLayer The current animation layer used to do the calculation.
	* \param pBlend if \c false, only animation on current layer will be taken into account, and pResult->mCurveNode will be updated accordingly.
	                if \c true, the value on this animation layer will be blended with current value of pLT, pLR and pLS.
	* \remarks The usual usage of this function is to call it on the first animation layer with out blending, then call it repeatedly on other 
	*  animation layers with blending to get the blended value of pLT, pLR and pLS of all animation layers.
	*/
	void ComputeTRSAnimationLayer(KFbxNodeEvalState* pResult, KFbxNode* pNode, KFbxVector4& pLT, KFbxVector4& pLR, KFbxVector4& pLS, KTime pTime, KFbxAnimLayer* pLayer, bool pBlend);
	
	/** Blend value of a property on certain animation layer to pResult.
	* \param pResult The blended value of the property.
	* \param pResultSize The elements number of the property value.
	* \param pProperty  The property to be blended.
	* \param pEvalState An auxiliary parameter, the NodeEvalState to get rotation order for eFbxBlendRotation type blending.
	* \param pTime The time used for evaluate. 
	* \param pLayer The current animation layer used to do the calculation.
	* \param pType There are three blend types, eFbxBlendSimple, eFbxBlendRotation, eFbxBlendScaling
	* \param pResetPResultBeforeUse This flag instructs the routine to ignore the content of pResult in the computation. It 
	*        must be true ONLY when processing the first animation layer so we can avoid adding the property default
	*        value to the KFbxAnimCurveNode channel's default value.
	* \remarks The blended value will be kept in pResult.
	*/
	void BlendPropertyEvalWithLayer(double* pResult, int pResultSize, KFbxProperty& pProperty, KFbxNodeEvalState* pEvalState, KTime pTime, KFbxAnimLayer* pLayer, eFbxBlendType pType, bool pResetPResultBeforeUse = false);
	
	/** Blends two arrays of values in a simple weighted linear blending way.
	* \param pResult The first array of values to be blended.
	* \param pResultSize The number of elements of the first value to be blended.
	* \param pApply  The second array of values to be blended.
	* \param pApplySize The number of elements of the second value to be blended.
	* \param pWeight The weight used to blend. 
	* \param pBlendMode The blend mode to use.
	* \see EBlendMode
	* \remarks The blended value will be kept in pResult.
	*/
	void BlendSimple(double* pResult, int pResultSize, double* pApply, int pApplySize, double pWeight, KFbxAnimLayer::EBlendMode pBlendMode);
	
	/** Blends two arrays of values representing rotations.
	* \param pResult The first array of values to be blended.
	* \param pResultSize The number of elements of the first value to be blended.
	* \param pApply  The second array of values to be blended.
	* \param pApplySize The number of elements of the second value to be blended.
	* \param pWeight The weight used to blend. 
	* \param pBlendMode The blend mode to use.
	* \param pRotAccuMode The rotation accumulation mode.
	* \param pRotationOrder The rotation order to be used for blending.
	* \remarks The blended value will be kept in pResult. And this blend should not be used with anything other than rotations.
	* \see EBlendMode, ERotationAccumulationMode
	*/
	void BlendRotation(double* pResult, int pResultSize, double* pApply, int pApplySize, double pWeight, KFbxAnimLayer::EBlendMode pBlendMode, KFbxAnimLayer::ERotationAccumulationMode pRotAccuMode, int pRotationOrder);
	
	/** Blends two arrays of values representing scaling transforms.
	* \param pResult The first array of values to be blended.
	* \param pResultSize The number of elements of the first value to be blended.
	* \param pApply  The second array of values to be blended.
	* \param pApplySize The number of elements of the second value to be blended.
	* \param pWeight The weight used to blend. 
	* \param pBlendMode The blend mode to use.
	* \param pScaleAccuMode The scaling accumulation mode.
	* \remarks The blended value will be kept in pResult.And this blend should not be used with anything other than scaling transform.
	* \see EBlendMode, EScaleAccumulationMode.
	*/
	void BlendScaling(double* pResult, int pResultSize, double* pApply, int pApplySize, double pWeight, KFbxAnimLayer::EBlendMode pBlendMode, KFbxAnimLayer::EScaleAccumulationMode pScaleAccuMode);

	double* mPropertyValues;
	int		mPropertySize;

	double*	mCurveNodeEvalValues;
	int		mCurveNodeEvalSize;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif /* FBXFILESDK_KFBXPLUGINS_KFBXEVALUATIONCLASSIC_H */
