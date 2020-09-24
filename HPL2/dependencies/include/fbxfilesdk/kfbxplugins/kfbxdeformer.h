/*!  \file kfbxdeformer.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXDEFORMER_H
#define FBXFILESDK_KFBXPLUGINS_KFBXDEFORMER_H

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
#include <fbxfilesdk/kfbxmath/kfbxmatrix.h>

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxGeometry;
class KFbxSdkManager;

/** Base class for skin deformer (KFbxSkin) and vertex cache deformer (KFbxVertexCacheDeformer).
  * The corresponding deformer types are KFbxDeformer::eSKIN and KFbxDeformer::eVERTEX_CACHE.
  * A deformer can be binded to a geometry (KFbxGeometry) to act on its shape. Typically,
  * some objects under the deformer are animated, and via the deformer, the geometry
  * is animated too.
  *
  * A skin deformer contains clusters (KFbxCluster). Each cluster acts on a subset of the geometry's
  * control points, with different weights. For example, a mesh of humanoid shape
  * can have a skin attached, that describes the way the humanoid mesh is deformed
  * by bones. When the bones are animated, the clusters act on the geometry to
  * animate it too.
  *
  * A vertex cache deformer contains a cache (KFbxCache). The cache contains animation
  * information for every control point of the geometry.
  *
  *\nosubgrouping
  */
class KFBX_DLL KFbxDeformer : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxDeformer,KFbxObject);

public:
        /**
          * \name Multi-Layer Flag
		  * This flag is available for backward compatibility with older 
		  * version of FBX files and should not be used anymore. All the animation layering
		  * system has been moved to the KFbxAnimLayer and KFbxAnimStack classes.
          */
        //@{
        /** Set multi-layer state flag.
          * \param pMultiLayer     Set to \c true to enable multi-layering.
          */
        void SetMultiLayer(bool pMultiLayer);

        /** Get multi-layer state.
          * \return     The current state of the multi-layer flag.
          */
        bool GetMultiLayer() const;
        //@}

        /**
          * \name Deformer types
          */
        //@{
        /** \enum EDeformerType Deformer types.
          */
        typedef enum
        {
			//! Untyped deformer
            eUNIDENTIFIED,
			//! Type KFbxSkin
            eSKIN,
			//! Type KFbxBlendShape
			eBLENDSHAPE,
			//! Type KFbxVertexCacheDeformer
            eVERTEX_CACHE,
			//! Number of deformer types
            eDEFORMER_COUNT
        } EDeformerType;

        /** Get the deformer type.
          * \return     Deformer type identifier. Default value is eUNIDENTIFIED.
          */
        virtual EDeformerType GetDeformerType() const { return eUNIDENTIFIED; }
        //@}

        /**
          * \name Error Management
          */
        //@{

        /** Retrieve error object.
          * \return     Reference to error object.
          */
        KError& GetError();

        /** \enum EError Error identifiers.
          */
        typedef enum
        {
			//! An error occurred
            eERROR,
			//! Number of error identifiers
            eERROR_COUNT
        } EError;

        /** Get last error code.
          * \return     Last error code.
          */
        EError GetLastErrorID() const;

        /** Get last error string.
          * \return     Textual description of the last error.
          */
        const char* GetLastErrorString() const;

        //@}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

    protected:
        KFbxDeformer(KFbxSdkManager& pManager, char const* pName);

        virtual KStringList GetTypeFlags() const { return KStringList(); }

        // Local
        KError mError;
        bool   mMultiLayer;

        friend class KFbxGeometry;
        friend class KFbxScene;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxDeformer* HKFbxDeformer;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXDEFORMER_H

