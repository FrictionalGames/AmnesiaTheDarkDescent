/*!  \file kfbxembeddedfilesaccumulator.h
 */

#ifndef FBXFILESDK_FBXPROCESSORS_KFBXEMBEDDEDFILESACCUMULATOR_H
#define FBXFILESDK_FBXPROCESSORS_KFBXEMBEDDEDFILESACCUMULATOR_H

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

    /** This processor is used to accumulate the list of file dependencies (embedded files) in a hierarchy of objects.
      * It retrieves information of embedded files from objects and accumulates them to its class member mEmbeddedFiles.
      * \see KFbxProcessor::ProcessCollection(KFbxCollection *)
      * \nosubgrouping
      */
    class KFBX_DLL KFbxEmbeddedFilesAccumulator : public KFbxProcessor
    {
    public:

        /**
        * Map the object to the property's hierarchical name.
        * An object may use the same file on multiple properties, hence the
        * set.
        * Each property may have multiple URLs, separate by |.
        * We thus need to store the index along with the property.
        */
        //@{
        struct KFbxPropertyUrlIndex
        {
            KString mPropName;
            int     mIndex;

            KFbxPropertyUrlIndex() : mIndex(0)
            {
            }

            KFbxPropertyUrlIndex(const KString& pUrl, int pIndex)
                : mPropName(pUrl)
                , mIndex(pIndex)
            {
            }
        };

        //! Comparer for KFbxPropertyUrlIndexSet, which outputs consistent partial orders for KFbxPropertyUrlIndex pairs
        struct KFbxPropertyUrlIndexCompare
        {
            inline int operator()(const KFbxPropertyUrlIndex& pUrl1, const KFbxPropertyUrlIndex& pUrl2) const
            {
                if( pUrl1.mPropName < pUrl2.mPropName )
                {
                    return -1;
                }
                if( pUrl2.mPropName < pUrl1.mPropName )
                {
                    return 1;
                }

                if(pUrl1.mIndex < pUrl2.mIndex)
                {
                    return -1;
                }
                if(pUrl2.mIndex < pUrl1.mIndex)
                {
                    return 1;
                }

                return 0;                
            }
        };

        typedef KSet2<KFbxPropertyUrlIndex, KFbxPropertyUrlIndexCompare> KFbxPropertyUrlIndexSet;

        typedef KMap<KFbxObject*, KFbxPropertyUrlIndexSet> KFbxObjectPropertyMap;

        struct KFbxEmbeddedFileInfo
        {
            KString                 mOriginalPropertyUrl;
            KFbxObjectPropertyMap   mConsumers;
        };
        //@}

        /**
        * Map the (absolute filename) to which object/properties use this file.
        * To simply get the list of file dependencies, iterate through this map and query
        * all the keys.
        */
        //@{
        typedef KMap<KString, KFbxEmbeddedFileInfo>     KFbxEmbeddedFilesMap;

        KFbxEmbeddedFilesMap   mEmbeddedFiles;
        //@}

    public:
        
        /** Constructor.
        * The name is not important.
        * The property filter is a list of strings, property names, which are automatically ignored when
        * encountered. Property names must be the full hierarchical property name (ie: parent|child|child).
        *
        * \param pManager           Reference to the SDK manager.
        * \param pName              Name of this object.
        * \param pPropertyFilter    Reference to the property filter.
        */
        KFbxEmbeddedFilesAccumulator(KFbxSdkManager& pManager, const char* pName, KSet2<KString>& pPropertyFilter);
        virtual ~KFbxEmbeddedFilesAccumulator();

        ///////////////////////////////////////////////////////////////////////////////
        //
        //  WARNING!
        //
        //  Anything beyond these lines may not be documented accurately and is
        //  subject to change without notice.
        //
        ///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    private:
        virtual bool internal_ProcessObject(KFbxObject* pObject);
        KSet2<KString>   mPropertyFilter;

#endif

    };

}

#endif // FBXFILESDK_FBXPROCESSORS_KFBXEMBEDDEDFILESACCUMULATOR_H

