/*!  \file kfbxshape.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSHAPE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSHAPE_H

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

#include <fbxfilesdk/kfbxplugins/kfbxgeometrybase.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;
class KFbxBlendShapeChannel;
class KFbxGeometry;

/** A shape describes the deformation on a set of control points, which is similar to the cluster deformer in Maya.
  * For example, we can add a shape to a created geometry. And the shape and the geometry have the same
  * topological information but different position of the control points.
  * With varying amounts of influence, the geometry performs a deformation effect.
  * \nosubgrouping
  * \see KFbxGeometry
  */
class KFBX_DLL KFbxShape : public KFbxGeometryBase
{
    KFBXOBJECT_DECLARE(KFbxShape,KFbxGeometryBase);

public:
	/** Set the blend shape channel that contains this target shape.
	* \param pBlendShapeChannel      Pointer to the blend shape channel to set.
	* \return                        \c true on success, \c false otherwise.
	*/
	bool SetBlendShapeChannel(KFbxBlendShapeChannel* pBlendShapeChannel);

	/** Get the blend shape channel that contains this target shape.
	* \return                        a pointer to the blend shape channel if set or NULL.
	*/
	KFbxBlendShapeChannel* GetBlendShapeChannel() const;

	/** Get the base geometry of this target shape.
	* \return                        a pointer to the base geometry if set or NULL.
	* \remarks Since target shape can only connected to its base geometry through
	*          blend shape channel and blend shape deformer.
	*          So only when this target shape is connected to a blend shape channel,
	*          and the blend shape channel is connected to a blend shape deformer,
	*          and the blend shape deformer is used on a base geometry, then to get 
    *          base geometry will success.
	*/
	KFbxGeometry* GetBaseGeometry();

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


	/** Set the array size for the control point indices
	* \param pCount The new count.
	*/
	void SetControlPointIndicesCount(int pCount);

	/** Add a control point index to the control point indices array
	* \param pIndex The control point index to add.
	*/	
	void AddControlPointIndex(int pIndex);

	/** Restore the shape to its initial state.
	* Calling this function will clear the following:
	* \li Pointer to blend shape channel.
	* \li Control point indices.
	*/
	void Reset();

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
	virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
    /** Return the type of node attribute which is EAttributeType::eUNIDENTIFIED.
      * EAttributeType::eUNIDENTIFIED is returned because this class derives
      * accidentally from KFbxNodeAttribute but its instances are not meant to 
      * be assigned to nodes.
      */
    virtual EAttributeType GetAttributeType() const;
    
	virtual KStringList GetTypeFlags() const;

    KFbxShape(KFbxSdkManager& pManager, char const* pName);
	virtual bool ConstructProperties( bool pForceSet );

	KArrayTemplate<int>    mControlPointIndices;

    friend class KFbxGeometry;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxShape* HKFbxShape;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSHAPE_H

