/*!  \file kfbxblendshapechannel.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXBLENDSHAPECHANNEL_H
#define FBXFILESDK_KFBXPLUGINS_KFBXBLENDSHAPECHANNEL_H

/**************************************************************************************

 Copyright (C) 2010 Autodesk, Inc. and/or its licensors.
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
class KFbxBlendShape;
class KFbxShape;

/** Class for blend shape channels. 
  * A blend shape channel is a sub-deformer to help blend shape deformer to organize the target shapes.
  * One blend shape deformer can have multiple blend shape channels in parallel, and each of them can 
  * control one or multiple target shapes. If there are multiple target shapes connected to one channel,
  * and each target shape could have its own full deformation percentage, for example, one channel could have 3 target shapes,
  * whose full deform percentage are 30, to 80 to 100, then when the percent change from 0 to 100, the base geometry will
  * deform from the first target shape to the last one, this is called In-Between blend-shapes.
  * The blend shape channel also control the deform percent of each target shape or In-Between blend shape on it.
  *\nosubgrouping
  */
class KFBX_DLL KFbxBlendShapeChannel : public KFbxSubDeformer
{
    KFBXOBJECT_DECLARE(KFbxBlendShapeChannel,KFbxSubDeformer);

public:
	/** This property stores deform percent on this channel.
	* The value range is from 0.0 to 100.0, default value is 0.0. 
	*/
	KFbxTypedProperty<double>        DeformPercent;

	/** Set the blend shape deformer that contains this blend shape channel.
	* \param pBlendShape             Pointer to the blend shape deformer to set.
	* \return                        \c true on success, \c false otherwise.
	*/
	bool SetBlendShapeDeformer(KFbxBlendShape* pBlendShape);

	/** Get the blend shape deformer that contains this blend shape channel.
	* \return                        a pointer to the blend shape deformer if set or NULL.
	*/
	KFbxBlendShape* GetBlendShapeDeformer();

	/** Add a target shape.
	* \param pShape                  Pointer to the target shape to add.
	* \param pFullDeformPercent      The full deform percentage for the target shape.
	* \return                        \c true on success, \c false otherwise.
	*/
	bool AddTargetShape(KFbxShape* pShape, double pFullDeformPercent = 100);

	/** Remove the given target shape.
	* \param pShape                  Pointer to the target shape to remove from this blend shape channel.
	* \return                        Pointer to the target shape or \c NULL if pShape is not owned by this blend shape channel.
	*/
	KFbxShape* RemoveTargetShape(KFbxShape* pShape);

	/** Get the number of target shapes.
	* \return                        Number of target shapes that have been added to this blend shape channel.
	*/
	int GetTargetShapeCount() const;

	/** Get the target shape at given index.
	* \param pIndex                  Index of the target shape.
	* \return                        Pointer to the target shape or \c NULL if index is out of range.
	*/
	KFbxShape* GetTargetShape(int pIndex);

	/** Get the target shape at given index.
	* \param pIndex                  Index of the target shape.
	* \return                        Pointer to the target shape or \c NULL if index is out of range.
	*/
	KFbxShape const* GetTargetShape(int pIndex) const;

	/** Get the index of the given target shape.
	* \param pShape                  The given target shape to find index.
	* \return                        The index of the target shape.
	*/
	int GetTargetShapeIndex( KFbxShape* pShape);

	/** Get the full weight value of target shape at given index.
	* \param pIndex                  Index of the target shape.
	* \return                        The full weight value of target shape at given index.
	*/
	double* GetTargetShapeFullWeights();

	/** Set the array size for the fully deform weights
	* \param pCount The new count.
	*/
	void SetFullWeightsCount(int pCount);

    /**
      * \name General Functions
      */
    //@{
    /** Get the type of the sub deformer.
      * \return SubDeformer type identifier: eBLENDSHAPECHANNEL.
      */
    ESubDeformerType GetSubDeformerType() const {return eBLENDSHAPECHANNEL; };

    /** Restore the blend shape channel to its initial state.
      * Calling this function will do the following:
      * \li Set the DeformPercent to 0.
      * \li Remove all target shapes.
      * \li Clear the array for fully deform weights.
      */
    void Reset();


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
    KFbxBlendShapeChannel(KFbxSdkManager& pManager, char const* pName);

    virtual void Construct(const KFbxBlendShapeChannel* pFrom);
    virtual bool ConstructProperties( bool pForceSet );

    virtual KStringList GetTypeFlags() const;


protected:
    // The full weights array of each shapes on this blend shape channel
	KArrayTemplate<double> mShapeFullWeightArray;

    // Properties

    friend class KFbxScene;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXBLENDSHAPECHANNEL_H

