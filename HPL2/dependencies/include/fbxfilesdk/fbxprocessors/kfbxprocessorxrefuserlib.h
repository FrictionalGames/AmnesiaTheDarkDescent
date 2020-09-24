/*!  \file kfbxprocessorxrefuserlib.h
 */

#ifndef FBXFILESDK_FBXPROCESSORS_KFBXPROCESSORXREFUSERLIB_H
#define FBXFILESDK_FBXPROCESSORS_KFBXPROCESSORXREFUSERLIB_H

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

#include <fbxfilesdk/fbxprocessors/fbxprocessors_def.h>

#include <fbxfilesdk/fbxprocessors/kfbxprocessorxref.h>

namespace FBXPROCESSORS_NAMESPACE {

    /**
      * Specialized xref copy processor
      */
    class KFBX_DLL KFbxProcessorXRefCopyUserLibrary : public KFbxProcessorXRefCopy
    {
        KFBXOBJECT_DECLARE(KFbxProcessorXRefCopyUserLibrary, KFbxProcessorXRefCopy);

        /**
        * \name Properties
        */
        //@{
        public:
            // Do we copy files even if they are in the system library?
            // Defaults to FALSE.
            KFbxTypedProperty<fbxBool1>     CopyAllAssets;

            // Do we copy files even if they are not within the scene?  This is
            // the typical use case when creating a new library, and defaults to
            // TRUE.  If you want to extract assets from a specific library you
            // you would set this to FALSE to ignore assets from external (user,
            // system) libraries.
            KFbxTypedProperty<fbxBool1>     CopyExternalAssets;

            // Do we copy assets that use absolute paths?  If true, then after
            // the scene processor has run through the URL will be relative to
            // the scene document.
            // Defaults to TRUE.
            KFbxTypedProperty<fbxBool1>     CopyAbsoluteUrlAssets;
        //@}


        ///////////////////////////////////////////////////////////////////////////////
        //  WARNING!
        //  Anything beyond these lines may not be documented accurately and is
        //  subject to change without notice.
        ///////////////////////////////////////////////////////////////////////////////
        #ifndef DOXYGEN_SHOULD_SKIP_THIS

            // Constructor / Destructor
            protected:
                KFbxProcessorXRefCopyUserLibrary(KFbxSdkManager& pManager, char const* pName);
                bool ConstructProperties(bool pForceSet);

            /**
            * \name Overridable internal function */
            //@{
            protected:
                virtual bool                ValidPropertyForXRefCopy(KFbxObject* pObject, KFbxProperty& lProperty) const;
            //@}

        #endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

    };
}

#endif // FBXFILESDK_FBXPROCESSORS_KFBXPROCESSORXREFUSERLIB_H

