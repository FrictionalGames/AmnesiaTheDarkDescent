/*!  \file kfbxgeometry.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXGEOMETRY_H
#define FBXFILESDK_KFBXPLUGINS_KFBXGEOMETRY_H

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
#include <fbxfilesdk/kfbxplugins/kfbxdeformer.h>
#include <fbxfilesdk/kfbxplugins/kfbxshape.h>
#include <fbxfilesdk/kfbxplugins/kfbxblendshape.h>
#include <fbxfilesdk/kfbxplugins/kfbxblendshapechannel.h>

#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/kfbxmath/kfbxxmatrix.h>
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxGeometryWeightedMap;
class KFbxNode;
class KFbxShape;
class KFbxTexture;
class KFbxSdkManager;
class KFbxVector4;
class KFbxAnimCurve;
class KFbxAnimLayer;
class KFbxScene;
class KFbxCluster;
typedef class KFbxCluster KFbxLink;

/** The base class of geometric objects that support control point deformations (e.g. KFbxMesh, KFbxNurb, 
* and KFbxPatch). The KFbxGeometry provides support for the following kinds of deformations.
*
* \li Skin deformation deformers
* \li Vertex cache deformers 
* \li Geometry weighted maps
* \li Shapes
* 
* Most of the methods of KFbxGeometry are wrappers to simplify the access/manipulation of the connections 
* to the deformers. For example, calling the GetDeformerCount() method is the same 
* thing as calling: 
*
* \code
* geometry.GetSrcObjectCount(KFbxDeformer::ClassId)
* \endcode
*/
class KFBX_DLL KFbxGeometry : public KFbxGeometryBase
{
    KFBXOBJECT_DECLARE(KFbxGeometry,KFbxGeometryBase);

public:
    /** Returns the node attribute type.
      * This method is derived in the more high level classes (KFbxMesh, KFbxNurbs, etc...) and returns the
      * actual type of the geometry object. 
      *
      * \return \e eUNIDENTIFIED
      */
    virtual EAttributeType GetAttributeType() const;

    /**
      * \name Deformer Management
      */
    //@{

    /** Adds a deformer to this geometry (as mentioned in the description of this class, adding a deformer is a synonym
      * for "connect a deformer").
      * \param pDeformer     Pointer to the deformer to be added.
      * \return              Index of the added deformer.
      */
    int AddDeformer(KFbxDeformer* pDeformer);

	/** Remove a deformer.
	* \param pIndex Index of deformer to remove.
	* \return Pointer to the removed deformer (or \c NULL if pIndex is out of range). If pIndex is out of range
	*         KFbxGeometry::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
	*/
	KFbxDeformer* RemoveDeformer(int pIndex);

    /** Returns the number of deformers.
      * \return     The number of deformers that are connected to this geometry.
      */
    int GetDeformerCount() const;

    /** Returns the deformer at the specified index.
      * \param pIndex     The specified deformer index.
      * \return           Pointer to the deformer (or \c NULL if pIndex is out of range). If pIndex is out of range,
      *                   KFbxGeometry::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
      */
    KFbxDeformer* GetDeformer(int pIndex) const;

    /** Returns the number of deformers of a specified type.
      * \param pType     The specified deformer type.
      * \return          The number of deformers of the specified type.
      */
    int GetDeformerCount(KFbxDeformer::EDeformerType pType) const;

    /** Returns the deformer of a specified type at the specified index.
      * \param pIndex     The specified deformer index.
      * \param pType      The specified deformer type.
      * \return           Pointer to the deformer (or \c NULL if pIndex is out of range). If pIndex is out of range,
      *                   KFbxGeometry::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
      */
    KFbxDeformer* GetDeformer(int pIndex, KFbxDeformer::EDeformerType pType) const;

    //@}

    /**
      * \name Geometry Weighted Maps Management
      */
    //@{

    /** Returns the source geometry weighted map that is connected to this geometry.
      * \return     Pointer to the source geometry weighted map that is connected to this geometry if any.
      */
    KFbxGeometryWeightedMap* GetSourceGeometryWeightedMap();

    /** Returns the number of destination geometry weighted map(s) that are connected to this geometry.
      * \return     The number of destination geometry weighted map(s) that are connected to this geometry.
      */
    int GetDestinationGeometryWeightedMapCount() const;

    /** Returns the destination geometry weighted map at a specified index.
      * \param pIndex     The specified index.
      * \return           Pointer to the destination geometry weighted map at the specified index (if any).
      */
    KFbxGeometryWeightedMap* GetDestinationGeometryWeightedMap(int pIndex);

    //@}

    /**
      * \name Shape Management - Deprecated functions.
	  * All the methods in this section have been deprecated and will be removed in the next release. 
	  * \see KFbxBlendShape, KFbxBlendShapeChannel, KFbxAnimCurve, KFbxShape.
      */
	//@{
    /** Deprecated. Use this code instead:
	  * \code
	  * KFbxBlendShape* lBlendShape = KFbxBlendShape::Create(pScene,"MyBlendShape");
	  * KFbxBlendShapeChannel* lBlendShapeChannel = KFbxBlendShapeChannel::Create(pScene,"MyBlendShapeChannel");
	  * KFbxShape* lShape = KFbxShape::Create(pScene,"StretchedShape");
	  * lBlendShapeChannel->AddTargetShape(lShape);
	  * lBlendShape->AddBlendShapeChannel(lBlendShapeChannel);
	  * lGeometry->AddDeformer(lBlendShape);
	  * \endcode
	  * Add a shape and the shape's associated name to this geometry.
      * \param pShape         Pointer to the shape object to be added.
      * \param pShapeName     Name of the shape.
      * \return               Index of the added shape, -1 if operation fails.
      *                       If the operation fails, KFbxGeometry::GetLastErrorID() can return one of the following:
      *                            - eNULL_PARAMETER: Pointer to shape is \c NULL.
      *                            - eSHAPE_ALREADY_ADDED: Shape has already been added.
      *                            - eSHAPE_INVALID_NAME: The name provided is empty.
      *                            - eSHAPE_NAME_CLASH: The name provided is already used by another shape.
      * \remarks              The name provided is stripped from the surrounding white space before it is
      *                       compared to the other shape names. It is recommended not to prefix the shape name with its
      *                       enclosing node name because MotionBuilder is known to strip this prefix and not restore it.
      */
    K_DEPRECATED virtual int AddShape(KFbxShape* pShape, char const* pShapeName);

    /** Deprecated.
	  * Use the following code snippet to:
	  * \li Get Shapes and shape influences.
	  * \li Find out which shape of the In-between blend-shapes is working at a given time pTime.
	  * \code
	  * 	int lBlendShapeDeformerCount = pMesh->GetDeformerCount(KFbxDeformer::eBLENDSHAPE);
	  *	for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
	  *	{
	  *		KFbxBlendShape* lBlendShape = (KFbxBlendShape*)pMesh->GetDeformer(lBlendShapeIndex, KFbxDeformer::eBLENDSHAPE);
	  *
	  *		int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
	  *		for(int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex)
	  *		{
	  *			KFbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
	  *
	  *			if(lChannel)
	  *			{
	  *				// Get the percentage of influence of the shape.
	  *				KFbxAnimCurve* lFCurve = pMesh->GetShapeChannel(lBlendShapeIndex, lChannelIndex, gCurrentAnimationLayer);
	  *				if (!lFCurve) continue;
	  *				double lWeight = lFCurve->Evaluate(pTime);
	  *
	  *				//Find which shape should we use according to the weight.
	  *				int lShapeCount = lChannel->GetTargetShapeCount();
	  *				double* lFullWeights = lChannel->GetTargetShapeFullWeights();
	  *				for(int lShapeIndex = 0; lShapeIndex<lShapeCount; ++lShapeIndex)
	  *				{
	  *					KFbxShape* lShape = NULL;
	  *					if(lWeight > 0 && lWeight < lFullWeights[0])
	  *					{
	  *						lShape = lChannel->GetTargetShape(0);
	  *					}
	  *					if(lWeight > lFullWeights[lShapeIndex] && lWeight < lFullWeights[lShapeIndex+1])
	  *					{
	  *						lShape = lChannel->GetTargetShape(lShapeIndex+1);
	  *					}
	  *
	  *					if(lShape)
	  *					{				
	  *						for (int j = 0; j < lVertexCount; j++)
	  *						{
	  *							// Add the influence of the shape vertex to the mesh vertex.
	  *							KFbxVector4 lInfluence = (lShape->GetControlPoints()[j] - lSrcVertexArray[j]) * lWeight * 0.01;
	  *							lDstVertexArray[j] += lInfluence;
	  *						}						
	  *					}
	  *				}//For each target shape
	  *			}//If lChannel is valid
	  *		}//For each blend shape channel
	  *	}//For each blend shape deformer
	  * \endcode
	  * Returns the shape found at the specified index.
      * \param pIndex     The specified index.
      * \return           Pointer to the shape (or \c NULL if pIndex is out of range). If pIndex is out of range,
      *                   KFbxGeometry::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
      */
    K_DEPRECATED virtual KFbxShape* GetShape(int pIndex);

    /** Deprecated: See code snippet in KFbxGeometry::GetShape().
	  * Returns the shape found at the specified index.
      * \param pIndex     The specified index.
      * \return           Pointer to the shape (or \c NULL if pIndex is out of range). If pIndex is out of range,
      *                   KFbxGeometry::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
      */
    K_DEPRECATED virtual KFbxShape const* GetShape(int pIndex) const;

    /** Deprecated: See code snippet in KFbxGeometry::GetShape() and use KFbxObject::GetName() on the desired shape.
	  * Returns the shape name found at the specified index.
      * \param pIndex     The specified index.
      * \return           Shape name (or \c NULL if pIndex is out of range). If pIndex is out of range,
      *                   KFbxGeometry::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
      */
    K_DEPRECATED virtual char const* GetShapeName(int pIndex) const;

    /** Deprecated: See code snippet in KFbxGeometry::GetShape().
	  * Get the shape animation curve.
      * The shape channel is an animatable property with a value range from 0 to 100 (with 100 being full shape deformation).
      * The default value is 0.
      * \param pShapeName      The shape name.
      * \param pCreateAsNeeded If \c true, creates the animation curve if it is not already present.
      * \param pLayer          The animation layer from which we want to get the requested animation curve.
      * \return                Animation curve (or NULL if an error occurred). If an error occurs,
      *                        KFbxGeometry::GetLastErrorID() returns one of the following:
      *                             - eINDEX_OUT_OF_RANGE: Shape index is out of range.
      *                             - eSHAPE_NO_CURVE_FOUND: Shape curve could not be found.
      * \remarks If pLayer is left at NULL, the method will use the first layer of the Animation stack.
      */
    K_DEPRECATED virtual KFbxAnimCurve* GetShapeChannel(char const* pShapeName, KFbxAnimLayer* pLayer, bool pCreateAsNeeded = false);

	/** Deprecated: See code snippet in KFbxGeometry::GetShape().
	* Get the shape animation curve.
	* This method is deprecated and should not be used anymore. Instead call the equivalent using the KFbxAnimLayer.
	* \param pIndex          
	* \param pCreateAsNeeded 
	* \param pTakeName       
	*/
	K_DEPRECATED virtual KFCurve* GetShapeChannel(int pIndex, bool pCreateAsNeeded=false, char const* pTakeName = NULL);

    /** Deprecated: See code snippet in KFbxGeometry::GetShape().
	  * Get the shape animation curve.
      * The shape channel is an animatable property with a value range from 0 to 100 (with 100 being full shape deformation).
      * The default value is 0.
      * \param pIndex          The shape index.
      * \param pCreateAsNeeded If \c true, creates the animation curve if it is not already present.
      * \param pLayer          Animation layer from which we want to get the requested animation curve.
      * \return                Animation curve (or NULL if an error occurred). If an error occurs,
      *                        KFbxGeometry::GetLastErrorID() returns one of the following:
      *                             - eINDEX_OUT_OF_RANGE: Shape index is out of range.
      *                             - eSHAPE_NO_CURVE_FOUND: Shape curve could not be found.
      * \remarks If pLayer is left NULL, use the first layer of the Animation stack.
      */
    K_DEPRECATED virtual KFbxAnimCurve* GetShapeChannel(int pIndex, KFbxAnimLayer* pLayer, bool pCreateAsNeeded = false);

    //@}

	/**
	* \name Shape Management
	*/
	//@{

	/** Add a shape to the specified blend shape deformer and blend shape channel of this geometry.
	* \param pBlendShapeIndex              The blend shape deformer index.
	* \param pBlendShapeChannelIndex       The blend shape channel index.
	* \param pShape         Pointer to the shape object to be added.
	* \param pPercent       The full deform percentage of this shape.
	* \return               \c true if success, \c false otherwise.
	*/
	bool AddShape(int pBlendShapeIndex, int pBlendShapeChannelIndex, KFbxShape* pShape, double pPercent = 100);

	/** Removes all the shapes without destroying them.
	* If shapes aren't explicitly destroyed before calling this function, they will be
	* destroyed along with the SDK manager that created them.
	*/
	void ClearShape();

	/** Returns the number of shapes.
	* \return     The number of shapes that have been added to this geometry.
	*/
	int GetShapeCount() const;

	/** Returns the number of shapes.
	* \param pBlendShapeIndex              The blend shape deformer index.
	* \param pBlendShapeChannelIndex       The blend shape channel index.
	* \return     The number of shapes that have been added to this blend shape channel of this blend shape deformer.
	*/
	int GetShapeCount(int pBlendShapeIndex, int pBlendShapeChannelIndex) const;

	/** Returns the shape found at the specified index on a blend shape channel of a blend shape deformer.
	* \param pBlendShapeIndex              The blend shape deformer index.
	* \param pBlendShapeChannelIndex       The blend shape channel index.
	* \param pShapeIndex                   The specified shape index.
	* \return                              Pointer to the shape (or \c NULL if pIndex is out of range). If pIndex is out of range,
	*                                      KFbxGeometry::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
	*/
	KFbxShape* GetShape(int pBlendShapeIndex, int pBlendShapeChannelIndex, int pShapeIndex);

	/** Returns the shape found at the specified index on a blend shape channel of a blend shape deformer.
	* \param pBlendShapeIndex              The blend shape deformer index.
	* \param pBlendShapeChannelIndex       The blend shape channel index.
	* \param pShapeIndex                   The specified shape index.
	* \return                              Pointer to the shape (or \c NULL if pIndex is out of range). If pIndex is out of range,
	*                                      KFbxGeometry::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
	*/
	KFbxShape const* GetShape(int pBlendShapeIndex, int pBlendShapeChannelIndex, int pShapeIndex) const;

	/** Get the shape animation curve.
	* The shape channel is an animatable property with a value range from 0 to 100 (with 100 being full shape deformation).
	* The default value is 0.
	* \param pBlendShapeIndex              The blend shape deformer index.
	* \param pBlendShapeChannelIndex       The blend shape channel index.
	* \param pCreateAsNeeded If \c true, creates the animation curve if it is not already present.
	* \param pLayer          The animation layer from which we want to get the requested animation curve.
	* \return                Animation curve (or NULL if an error occurred). If an error occurs,
	*                        KFbxGeometry::GetLastErrorID() returns one of the following:
	*                             - eINDEX_OUT_OF_RANGE: Shape index is out of range.
	*                             - eSHAPE_NO_CURVE_FOUND: Shape curve could not be found.
	* \remarks If pLayer is left at NULL, the method will use the first layer of the Animation stack.
	*/
	KFbxAnimCurve* GetShapeChannel(int pBlendShapeIndex, int pBlendShapeChannelIndex, KFbxAnimLayer* pLayer, bool pCreateAsNeeded = false);
	//@}

    /** NURBS and Patches surface modes.
      * This information is never directly used inside the FBX SDK. Applications can use these values if they wish to 
	  * carry the "rendering" details of the NURBS and Patches. The FBX SDK guarantee that the value (member of the KFbxNurb,
	  * KFbxNurbSurface and KFbxPatch classes) is stored to FBX files and retrieved from them.
	  * \remarks The enum has been defined in this class to avoid symbols duplication.
      */
    typedef enum
    {
        eRAW,
        eLOW_NO_NORMALS,
        eLOW,
        eHIGH_NO_NORMALS,
        eHIGH
    } ESurfaceMode;

    /**
      * \name Pivot Management
      * The geometry pivot is used to specify additional translation, rotation,
      * and scaling information applied to all control points when the model is
      * exported.
      */
    //@{

    /** Returns the pivot matrix.
      * \param pXMatrix     Placeholder for the returned matrix.
      * \return             Reference to the passed argument.
      */
    KFbxXMatrix& GetPivot(KFbxXMatrix& pXMatrix) const;

    /** Sets the pivot matrix.
      * \param pXMatrix     The transformation matrix that is assigned to the pivot matrix.
      */
    void SetPivot(KFbxXMatrix& pXMatrix);

    /** Applies the pivot matrix to all vertices/normals of the geometry.
      */
    void ApplyPivot();

    //@}

    /**
      * \name Default Animation Values - Deprecated functions.
	  * All the methods in this section have been deprecated and will be removed in the next release. 
      * These functions provides direct access to default animation values that are specific to a geometry.
      * These functions only work if the geometry has been associated with a node.
      */
    //@{

    /** Sets the default deformation for a specified shape.
      * The default shape property has a value range from 0 to 100 (with 100 being full shape deformation).
      * The default value is 0.
      * \param pIndex       The shape index.
      * \param pPercent     Deformation percentage (on a scale ranging from 0 to 100).
      * \remarks            This function has no effect if pIndex is out of range.
      */
    K_DEPRECATED void SetDefaultShape(int pIndex, double pPercent);

    /** Sets the default deformation for a specified shape.
      * The default shape property has a value range from 0 to 100 (with 100 being full shape deformation).
      * The default value is 0.
      * \param pShapeName   The shape name.
      * \param pPercent     Deformation percentage (on a scale ranging from 0 to 100).
      * \remarks            This function has no effect if pShapeName is invalid.
      */
    K_DEPRECATED void SetDefaultShape(char const* pShapeName, double pPercent);

    /** Returns the default deformation value for the specified shape.
      * The default shape property has a value range from 0 to 100 (with 100 being full shape deformation).
      * The default value is 0.
      * \param pIndex     The shape index.
      * \return           The deformation value for the specified shape, or 0 if pIndex is out of range.
      */
    K_DEPRECATED const double GetDefaultShape(int pIndex) const;

    /** Returns the default deformation value for the specified shape.
      * The default shape property has a value range from 0 to 100 (with 100 being full shape deformation).
      * The default value is 0.
      * \param pShapeName     The shape name.
      * \return               The deformation value for the specified shape, or 0 if pShapeName is invalid.
      */
    K_DEPRECATED const double GetDefaultShape(char const* pShapeName) const;

    //@}

	/**
	* \name Default Animation Values
	* These functions provides direct access to default animation values that are specific to a geometry.
	* These functions only work if the geometry has been associated with a node.
	*/
	//@{

	/** Sets the default deformation for a specified shape.
	* The default shape property has a value range from 0 to 100 (with 100 being full shape deformation).
	* The default value is 0.
	* \param pBlendShapeIndex              The blend shape deformer index.
	* \param pBlendShapeChannelIndex       The blend shape channel index.
	* \param pPercent                      Deformation percentage (on a scale ranging from 0 to 100).
	* \remarks                             This function has no effect if pIndex is out of range.
	*/
	void SetDefaultShape(int pBlendShapeIndex, int pBlendShapeChannelIndex, double pPercent);

	/** Sets the default deformation for a specified shape.
	* The default shape property has a value range from 0 to 100 (with 100 being full shape deformation).
	* The default value is 0.
	* \param pBlendShape              The blend shape deformer.
	* \param pBlendShapeChannel       The blend shape channel.
	* \param pPercent                 Deformation percentage (on a scale ranging from 0 to 100).
	* \remarks                        This function has no effect if pShapeName is invalid.
	*/
	void SetDefaultShape(KFbxBlendShapeChannel* pBlendShapeChannel,  double pPercent);

	/** Returns the default deformation value for the specified shape.
	* The default shape property has a value range from 0 to 100 (with 100 being full shape deformation).
	* The default value is 0.
	* \param pBlendShapeIndex              The blend shape deformer index.
	* \param pBlendShapeChannelIndex       The blend shape channel index.
	* \return           The deformation value for the specified shape, or 0 if pIndex is out of range.
	*/
	double GetDefaultShape(int pBlendShapeIndex, int pBlendShapeChannelIndex) const;

	/** Returns the default deformation value for the specified shape.
	* The default shape property has a value range from 0 to 100 (with 100 being full shape deformation).
	* The default value is 0.
	* \param pBlendShape              The blend shape deformer.
	* \param pBlendShapeChannel       The blend shape channel.
	* \return                         The deformation value for the specified shape, or 0 if pShapeName is invalid.
	*/
	double GetDefaultShape(KFbxBlendShapeChannel* pBlendShapeChannel) const;

	//@}

    /**
      * \name Error Management
      */
    //@{

    /** Retrieves the error object.
     *  \return Reference to the error object.
     */
    KError& GetError ();

    /** \enum EError Error identifiers.
      */
    typedef enum
    {
        eINDEX_OUT_OF_RANGE,			/*!< The index used to access an item is out of range. */
        eNULL_PARAMETER,				/*!< Requested pointer to shape object is \c NULL. */
        eSHAPE_ALREADY_ADDED,			/*!< The specified shape has already been added to this object. */
        eSHAPE_INVALID_NAME,			/*!< The provided name argument is empty. */
        eSHAPE_NAME_CLASH,				/*!< The provided name is already used by another shape. */
        eSHAPE_NO_CURVE_FOUND,			/*!< The shape's animation curve could not be found. */
        eUNKNOWN_ERROR,					/*!< Generic error message. */
        eERROR_COUNT
    } EError;

    /** Returns the last error code.
     *  \return     The last error code.
     */
    EError GetLastErrorID() const;

    /** Returns the last error string.
     *  \return     Text description of the last error.
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

    virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
    KFbxGeometry(KFbxSdkManager& pManager, char const* pName);
    virtual void Destruct(bool pRecursive, bool pDependents);

    virtual void SetDocument(KFbxDocument* pDocument);

public:
	//! Rename the shapes from the old MotionBuilder 4.01 format to the new one.
    void CleanShapeChannels(KFbxAnimLayer* pAnimLayer);
	//! Rename the shapes from the old MotionBuilder 4.01 format to the new one.
    void CleanShapeChannel(KFbxAnimLayer* pAnimLayer, int pShapeIndex);

	void CreateShapeChannelProperties(KString& pShapeName);

protected:
    // Shape channel name creation for nurb and patch.
    KString CreateShapeChannelName(int pShapeIndex);

    // Shape channel name creation for mesh.
    KString CreateShapeChannelName(KString pShapeName);

public:
    void ConvertShapeNamesToV5Format(KString pTakeNodeName);
    void ConvertShapeNamesToV5Format(KString pTakeNodeName, int pShapeIndex);
    void RevertShapeNamesToV6Format(KString pTakeNodeName);
    void RevertShapeNamesToV6Format(KString pTakeNodeName, int pShapeIndex);
    void ClearTemporaryShapeNames();

protected:
    void CopyDeformers(KFbxGeometry const* pGeometry);
    void CopyShapes(KFbxGeometry const* pGeometry);

    void CopyPivot(KFbxGeometry const* pSource);

    // Used during FBX v5 file store
    KArrayTemplate<KString*> mShapeNameArrayV6;
    KArrayTemplate<KString*> mShapeNameArrayV5;
    KArrayTemplate<KString*> mShapeChannelNameArrayV5;

    KFbxXMatrix* mPivot;


    mutable KError mError;

    friend class KFbxScene;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXGEOMETRY_H

