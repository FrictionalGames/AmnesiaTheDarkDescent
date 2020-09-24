/*!  \file kfbxnurb.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXNURB_H
#define FBXFILESDK_KFBXPLUGINS_KFBXNURB_H

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

#include <fbxfilesdk/kfbxplugins/kfbxgeometry.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxPatch;
class KFbxSdkManager;


/** A NURBS surface is a type of parametric geometry. A NURBS surface is defined by the
    degree, form, knot vector and control points in the U and V coordinates.

    For more information on the meaning of form, knot vectors and control points,
    see the KFbxNurbsCurve documentation. The same rules apply for the NURBS curves
    and NURBS surfaces, but NURBS surfaces have two dimensions (U and V).

  * \nosubgrouping
  */
class KFBX_DLL KFbxNurb : public KFbxGeometry
{
    KFBXOBJECT_DECLARE(KFbxNurb,KFbxGeometry);
public:
    //! Returns the EAttributeType::eNURB node attribute type.
    virtual EAttributeType GetAttributeType() const;

    //! Resets the NURBS surface its default values.
    void Reset();

    /**
      * \name NURBS surface Properties
      */
    //@{

    /** Sets the surface mode.
      * \param pMode        Surface mode identifier (see class KfbxGeometry)
      */
    void SetSurfaceMode(KFbxGeometry::ESurfaceMode pMode);

    /** Returns the surface mode.
      * \return             The surface mode identifier that is currently set.
      */
    inline ESurfaceMode GetSurfaceMode() const {return mSurfaceMode;}

    /** \enum ENurbType NURBS types.
      * - \e ePERIODIC
      * - \e eCLOSED
      * - \e eOPEN
      */
    typedef enum
    {
        ePERIODIC,
        eCLOSED,
        eOPEN
    } ENurbType;

    /** Allocates memory space for an array of control points as well as knot
      * and multiplicity vectors.
      * \param pUCount      Number of U-dimension control points.
      * \param pUType       U-dimension NURBS type.
      * \param pVCount      Number of V-dimension control points.
      * \param pVType       V-dimension NURBS type.
      * \remarks            Always call this function after KFbxNurb::SetOrder().
      */
    void InitControlPoints(int pUCount, ENurbType pUType, int pVCount, ENurbType pVType);

    /** Returns the number of U-dimension control points.
      * \return             Number of U-dimension control points.
      */
    inline int GetUCount() const {return mUCount;}

    /** Returns the number of V-dimension control points.
      * \return             Number of V-dimension control points.
      */
    inline int GetVCount() const {return mVCount;}

    /** Returns the U-dimension NURBS type.
      * \return             NURBS type identifier.
      */
    inline ENurbType GetNurbUType() const {return mUType;}

    /** Returns the V-dimension NURBS type.
      * \return             NURBS type identifier.
      */
    inline ENurbType GetNurbVType() const {return mVType;}

    /** Returns the number of elements in the U-dimension knot vector. See KFbxNurbsCurve for more information.
      * \return             The number of elements in the U-dimension knot vector.
      */
    int GetUKnotCount() const;

    /** Returns the U-dimension knot vector.
      * \return             Pointer to the U-dimension knot vector.
      */
    double* GetUKnotVector() const;

    /** Returns the number of elements in the V-dimension knot vector. See KFbxNurbsCurve for more information.
      * \return             The number of elements in the V-dimension knot vector.
      */
    int GetVKnotCount() const;

    /** Returns the V-dimension knot vector.
      * \return             Pointer to the V-dimension knot vector.
      */
    double* GetVKnotVector() const;

    /** Returns multiplicity of U-dimension control points.
      * \return             Pointer to the array of multiplicity values.
      * \remarks            The length of this vector is equal to the U count.
      *                     Its elements are set to 1 by default.
      */
    int* GetUMultiplicityVector() const;

    /** Returns multiplicity of V-dimension control points.
      * \return             Pointer to the array of multiplicity values.
      * \remarks            The length of this vector is equal to the V count.
      *                     Its elements are set to 1 by default.
      */
    int* GetVMultiplicityVector() const;

    /** Sets the order of the NURBS surface.
      * \param pUOrder      NURBS order in U dimension.
      * \param pVOrder      NURBS order in V dimension.
      */
    void SetOrder(kUInt pUOrder, kUInt pVOrder);

    /** Returns the NURBS order in U dimension.
      * \return             NURBS order in U dimension.
      */
    inline int GetUOrder() const {return mUOrder;}

    /** Returns the NURBS order in V dimension.
      * \return             NURBS order in V dimension.
      */
    inline int GetVOrder() const {return mVOrder;}

    /** Sets the NURBS step.
      * The step value is the number of divisions between adjacent control points.
      * \param pUStep       Steps in U dimension.
      * \param pVStep       Steps in V dimension.
      */
    void SetStep(int pUStep, int pVStep);

    /** Returns the number of divisions between adjacent control points in U dimension.
      * \return             Steps in U dimension.
      */
    inline int GetUStep() const {return mUStep;}

    /** Returns the number of divisions between adjacent control points in V dimension.
      * \return             Steps in V dimension.
      */
    inline int GetVStep() const {return mVStep;}

    /** Calculates the number of surface spans in the U dimension.
      * See KFbxNurbsCurve::GetSpanCount() for more information.
      * \return             The number of spans in the U dimension if the surface has been initialized.
      *                     If the spans have not been initialized, returns -1.
      */
    int GetUSpanCount() const;

    /** Calculates the number of surface spans in the V dimension.
      * See KFbxNurbsCurve::GetSpanCount() for more information.
      * \return             The number of spans in the V dimension if the surface has been initialized.
      *                     If the spans have not been initialized, returns -1.
      */
    int GetVSpanCount() const;

    //@}

    /**
      * \name NURBS Export Flags
      */
    //@{

    /** Sets the flag that induces UV flipping at export.
      * \param pFlag        If \c true, UV flipping occurs.
      */
    void SetApplyFlipUV(bool pFlag);

    /** Returns the flag that induces UV flipping at export.
      * \return             The current state of the UV flip flag.
      */
    bool GetApplyFlipUV() const;

    /** Sets the flag that induces link flipping at export.
      * \param pFlag        If \c true, the links control points indices are flipped.
      */
    void SetApplyFlipLinks(bool pFlag);

    /** Returns the flag that induces link flipping at export.
      * \return             The current state of the link flip flag.
      */
    bool GetApplyFlipLinks() const;

    /** Returns flip flags state.
      * \return             \c True if we need to flip either the UV or the links.
      */
    bool GetApplyFlip() const { return GetApplyFlipUV() || GetApplyFlipLinks(); }

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

    KFbxNurb(KFbxSdkManager& pManager, char const* pName);

    virtual void Destruct(bool pRecursive, bool pDependents);

    kUInt mUOrder, mVOrder;
    int mUCount, mVCount;
    int mUStep, mVStep;
    ENurbType mUType, mVType;

    double* mUKnotVector;
    double* mVKnotVector;

    int* mUMultiplicityVector;
    int* mVMultiplicityVector;

    ESurfaceMode mSurfaceMode;

    // Export flags.
    bool mApplyFlipUV;
    bool mApplyFlipLinks;

public:
    // Error identifiers, these are only used internally.
    typedef enum
    {
        eNurbTypeUnknown,
        eWrongNumberOfControlPoint,
        eWeightTooSmall,
        eUMultiplicityVectorError,
        eVMultiplicityVectorError,
        eUKnotVectorError,
        eVKnotVectorError,
        eErrorCount
    } EError;

protected:
    friend class KFbxGeometryConverter;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

typedef KFbxNurb* HKFbxNurb;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXNURB_H

