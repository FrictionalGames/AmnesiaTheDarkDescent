/*!  \file kfbxsubdeformer.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSUBDEFORMER_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSUBDEFORMER_H

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

    /** FBX SDK sub-deformer class
	  * \nosubgrouping
	  */

class KFBX_DLL KFbxSubDeformer : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxSubDeformer,KFbxObject);
    public:

        /** Set multi layer state flag.
          * \param pMultiLayer               If \c true, multi-layering is enabled.
          */
        void SetMultiLayer(bool pMultiLayer);

        /** Get multilayer state.
          * \return                         The state of the multi-layer flag.
          */
        bool GetMultiLayer() const;

        /** \enum ESubDeformerType SubDeformer types.
          * - \e eCLUSTER
          */
        typedef enum
        {
            eUNIDENTIFIED,
            eCLUSTER,
			eBLENDSHAPECHANNEL,
            eSUBDEFORMER_COUNT
        } ESubDeformerType;

        /** Get the type of the sub deformer.
          * \return                         SubDeformer type identifier.
          */
        virtual ESubDeformerType GetSubDeformerType() const { return eUNIDENTIFIED; }

        /**
          * \name Error Management
          */
        //@{

        /** Retrieve error object.
          * \return                        Reference to error object.
          */
        KError& GetError();

        /** \enum EError Error identifiers.
          * - \e eERROR
          */
        typedef enum
        {
            eERROR,
            eERROR_COUNT
        } EError;

        /** Get last error code.
          * \return                        Last error code.
          */
        EError GetLastErrorID() const;

        /** Get last error string.
          * \return                        Textual description of the last error.
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

    protected:
        KFbxSubDeformer(KFbxSdkManager& pManager, char const* pName);

        virtual KStringList GetTypeFlags() const { return KStringList(); }

        // Local
        KError mError;
        bool   mMultiLayer;

        friend class KFbxGeometry;
        friend class KFbxScene;
};

typedef KFbxSubDeformer* HKFbxSubDeformer;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSUBDEFORMER_H

