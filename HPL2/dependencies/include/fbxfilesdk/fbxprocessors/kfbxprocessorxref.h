/*!  \file kfbxprocessorxref.h
 */

#ifndef FBXFILESDK_FBXPROCESSORS_KFBXPROCESSORXREF_H
#define FBXFILESDK_FBXPROCESSORS_KFBXPROCESSORXREF_H

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

#include <fbxfilesdk/fbxprocessors/kfbxprocessor.h>
#include <fbxfilesdk/components/kbaselib/klib/kmap.h>

namespace FBXPROCESSORS_NAMESPACE {

    /** This class contains objects
      * This class also provides access to global settings and take information.
      */
    class KFBX_DLL KFbxProcessorXRefCopy : public KFbxProcessor
    {
        KFBXOBJECT_DECLARE(KFbxProcessorXRefCopy, KFbxProcessor);

	public:
        class KFBX_DLL KFbxMissingUrlHandler
        {
        public:
            virtual ~KFbxMissingUrlHandler();
            virtual void MissingUrl(const KString& pUrl, const KFbxProperty&) = 0;
        };

        /**
        * \name Properties
        */
        //@{
            KFbxTypedProperty<fbxString>    OutputDirectory;

            /** As we resolve xref and copy assets, do we update properties to
              * now use this relative path?  Defaults to TRUE.
              */
            KFbxTypedProperty<fbxBool1>     UpdateProperties;

            /** Default to FALSE -- when set, this informs the processor to track
              * every properties that were modified during the scene processing.
              */
            KFbxTypedProperty<fbxBool1>     TrackUpdatedProperties;

            /** Default to TRUE -- when not set, files are only copied if one of
              * the following conditions is met:
              * 
              * 1) Target does not exist
              * 2) Target has a different time
              * 3) Target has a different size
              */
            KFbxTypedProperty<fbxBool1>     ForceCopy;

            /** Default to TRUE -- when copying a file, also copy its modification
              * time.  A bit of a requirement if you're not going to use ForceCopy.
              */
            KFbxTypedProperty<fbxBool1>     CopyFileTimes;
        //@}

            /** Optional callback; when set, this will be called when an Url cannot be
              * be copied because the source is not found.
              * Memory is owned by the client code, and will not be freed by us.
              */
            KFbxMissingUrlHandler*          MissingUrlHandler;

        /** Since KFbxProperty is an opaque type, we can't do an efficient operator <
          * on it, and must keep the data on the object, which can be compared through
          * pointers, and then we can further compare against the property name.
          */
        struct KFbxPropertyUpdate
        {
            KFbxProperty mProperty;
            fbxString    mOriginalValue;

            inline KFbxPropertyUpdate() {}
            inline KFbxPropertyUpdate(const KFbxProperty& pProp, const fbxString& pVal) :
                mProperty(pProp), mOriginalValue(pVal) {}

            inline bool operator <(const KFbxPropertyUpdate& pOther) const
            {
                return strcmp(mProperty.GetName(), pOther.mProperty.GetName()) < 0;
            }
        };
        typedef KSet2<KFbxPropertyUpdate>           KFbxUpdateSet;
        typedef KMap<KFbxObject*, KFbxUpdateSet>    KFbxPropertyUpdateMap;

        /** All properties that were updated, with their original value.
          * Will always be empty if TrackUpdatedProperties
          * was not set before calling ProcessCollection/ProcessObject.
          * NOT cleared before each processing run.
          */
        KFbxPropertyUpdateMap& GetUpdatedProperties();

        /** If property tracking was enabled, goes through and reverts all changes
          * to the properties.  Does not un-copy the files, naturally.
          */
        void RevertPropertyChanges();

        /** This is just a safety net to make sure RevertPropertyChanges is called when
          * this goes out of scope.
          */
        struct KFBX_DLL KFbxAutoRevertPropertyChanges
        {
            KFbxAutoRevertPropertyChanges(KFbxProcessorXRefCopy* pCopy) : mXRefCopy(pCopy) {}
            ~KFbxAutoRevertPropertyChanges()
            {
                if( mXRefCopy )
                    mXRefCopy->RevertPropertyChanges();
            }

            KFbxProcessorXRefCopy* mXRefCopy;
        };

        ///////////////////////////////////////////////////////////////////////////////
        //  WARNING!
        //  Anything beyond these lines may not be documented accurately and is
        //  subject to change without notice.
        ///////////////////////////////////////////////////////////////////////////////
        #ifndef DOXYGEN_SHOULD_SKIP_THIS

            protected:
                KFbxPropertyUpdateMap       mUpdatedProperties;

            // Constructor / Destructor
            protected:
                KFbxProcessorXRefCopy(KFbxSdkManager& pManager, char const* pName);

                bool ConstructProperties(bool pForceSet);

                // Implements the rules specified for the ForceCopy property.
                // Also checks the ForceCopy property.
                bool ShouldCopyFile(const KString& pTarget, const KString& pSource) const;

            /**
            * \name Overridable internal function */
            //@{
            protected:
                virtual bool                internal_ProcessCollectionBegin (KFbxCollection*     pObject);
                virtual bool                internal_ProcessCollectionEnd   (KFbxCollection*     pObject);
                virtual bool                internal_ProcessObject          (KFbxObject*     pObject);
                bool                        ProcessPathProperty(KFbxProperty &pProperty);
                virtual bool                ValidPropertyForXRefCopy(KFbxObject* pObject, KFbxProperty& lProperty) const;
            //@}

        #endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

    };
}

#endif // FBXFILESDK_FBXPROCESSORS_KFBXPROCESSORXREF_H

