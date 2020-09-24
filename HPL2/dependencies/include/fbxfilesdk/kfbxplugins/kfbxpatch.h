/*!  \file kfbxpatch.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXPATCH_H
#define FBXFILESDK_KFBXPLUGINS_KFBXPATCH_H

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

class KFbxSdkManager;

/** A patch is a type of node attribute with parametric surface.
  * A patch object is useful for creating gently curved surfaces, and provides very detailed control for manipulating complex geometry.
  * \nosubgrouping
  */
class KFBX_DLL KFbxPatch : public KFbxGeometry
{
    KFBXOBJECT_DECLARE(KFbxPatch,KFbxGeometry);

public:
    //! Returns the EAttributeType::ePATCH node attribute type.
    virtual EAttributeType GetAttributeType() const;

    //! Resets the patch to its default values.
    void Reset();

    /**
      * \name Patch Properties
      */
    //@{

        /** Sets the surface mode.
          * \param pMode Surface mode identifier (see Class KFbxGeometry).
          */
        void SetSurfaceMode(KFbxGeometry::ESurfaceMode pMode);

        /** Returns the surface mode.
          * \return The current surface mode identifier.
          */
        inline KFbxGeometry::ESurfaceMode GetSurfaceMode() const {return mSurfaceMode;}

        /** \enum EPatchType Patch types.
          * - \e eBEZIER
          * - \e eBEZIER_QUADRIC
          * - \e eCARDINAL
          * - \e eBSPLINE
          * - \e eLINEAR
          */
        typedef enum
        {
            eBEZIER         = 0,
            eBEZIER_QUADRIC = 1,
            eCARDINAL       = 2,
            eBSPLINE        = 3,
            eLINEAR         = 4
        } EPatchType;

        /** Allocates memory space for the control points array.
          * \param pUCount      Number of U-dimension control points.
          * \param pUType       U-dimension patch type. 
          * \param pVCount      Number of V-dimension control points.
          * \param pVType       V-dimension patch type. 
          */
        void InitControlPoints(int pUCount, EPatchType pUType, int pVCount, EPatchType pVType);

        /** Returns the number of control points in the U-dimension.
          * \return             The number of control points in the U-dimension.
          */
        inline int GetUCount() const {return mUCount;}

        /** Returns the number of control points in the V-dimension.
          * \return             The number of control points in the V-dimension.
          */
        inline int GetVCount() const {return mVCount;}

        /** Returns the U-dimension patch type.
          * \return             Patch type identifier in the U-dimension.
          */
        inline EPatchType GetPatchUType() const {return mUType;}

        /** Returns the V-dimension patch type.
          * \return             Patch type identifier in the V-dimension.
          */
        inline EPatchType GetPatchVType () const {return mVType;}

        /** Sets the patch step.
          * The step is the number of divisions between adjacent control points.
          * \param pUStep       Steps in U-dimension.
          * \param pVStep       Steps in V-dimension.
          */
        void SetStep(int pUStep, int pVStep);

        /** Returns the number of divisions between adjacent control points in the U-dimension.
          * \return             Step value in the U-dimension.
          */
        inline int GetUStep() const {return mUStep;}

        /** Returns the number of divisions between adjacent control points in the V-dimension.
          * \return             Step value in the V-dimension.
          */
        inline int GetVStep() const {return mVStep;}

        /** Sets closed flags.
          * \param pU           Set to \c true if the patch is closed in U dimension.
          * \param pV           Set to \c true if the patch is closed in V dimension.
          */
        void SetClosed(bool pU, bool pV);

        /** Returns state of the U closed flag.
          * \return             \c True if the patch is closed in U dimension.
          */
        inline bool GetUClosed() const {return mUClosed;}

        /** Returns state of the V closed flag.
          * \return             \c True if the patch is closed in V dimension.
          */
        inline bool GetVClosed() const {return mVClosed;}

        /** Sets U-capped flags.
          * \param pUBottom     Set to \c true if the patch is capped at the bottom in the U-dimension.
          * \param pUTop \c     Set to \c true if the patch is capped on the top in the U-dimension.
          * \remarks            Capping options are saved but not loaded by Motionbuilder because they
          *                     are computed from the patch topography.
          */
        void SetUCapped(bool pUBottom, bool pUTop);

        /** Returns state of the bottom U-capped flag.
          * \return             \c True if the patch is capped at the bottom in the U-dimension.
          */
        inline bool GetUCappedBottom() const {return mUCappedBottom;}

        /** Returns state of the top U-capped flag.
          * \return             \c True if the patch is capped on the top in the U-dimension.
          */
        inline bool GetUCappedTop() const {return mUCappedTop;}

        /** Sets V-capped flags.
          * \param pVBottom     Sets to \c true if the patch is capped at the bottom in the V-dimension.
          * \param pVTop        Sets to \c true if the patch is capped on the top in the V-dimension.
          * \remarks            Capping options are saved but not loaded by Motionbuilder because they
          *                     are computed from the patch topography.
          */
        void SetVCapped(bool pVBottom, bool pVTop);

        /** Returns state of the bottom V-capped flag.
          * \return     \c True if the patch is capped at the bottom.
          */
        inline bool GetVCappedBottom() const {return mVCappedBottom;}

        /** Returns state of the top V-capped flag.
          * \return     \c True if the patch is capped on the top.
          */
        inline bool GetVCappedTop() const {return mVCappedTop;}

    //@}

    /**
      * \name Off-loading Serialization section
      */
    //@{
        /** Writes the content of the patch to the given stream.
          * \param pStream  The destination stream.
          * \return         \c True if the content is successfully processed by the receiving stream.
          *                 If it is not successful, returns \c false.
          */
        virtual bool ContentWriteTo(KFbxStream& pStream) const;

        /** Reads the content of the patch from the given stream.
          * \param pStream  The source stream.
          * \return         \c True if the patch completes with the data received from the stream successfully. 
          *                 If it is not successful, returns \c false.
          */
        virtual bool ContentReadFrom(const KFbxStream& pStream);
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

#ifdef _DEBUG
    virtual int MemoryUsage() const;
#endif

    virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
    KFbxPatch(KFbxSdkManager& pManager, char const* pName);

    virtual void Destruct(bool pRecursive, bool pDependents);

    EPatchType mUType, mVType;
    int mUCount, mVCount;
    int mUStep, mVStep;
    bool mUClosed, mVClosed;
    bool mUCappedBottom, mUCappedTop;
    bool mVCappedBottom, mVCappedTop;

    KFbxGeometry::ESurfaceMode mSurfaceMode;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

typedef KFbxPatch* HKFbxPatch;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXPATCH_H

