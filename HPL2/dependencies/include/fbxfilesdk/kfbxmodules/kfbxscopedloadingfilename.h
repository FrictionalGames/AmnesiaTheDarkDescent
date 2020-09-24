/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxscopedloadingfilename.h
 */

#ifndef FBXFILESDK_KFBXMODULES_KFBXSCOPEDLOADINGFILENAME_H
#define FBXFILESDK_KFBXMODULES_KFBXSCOPEDLOADINGFILENAME_H


#include <fbxfilesdk/fbxfilesdk_def.h>

// Local includes
#include <fbxfilesdk/kfbxmodules/kfbxloadingstrategy.h>

// FBX includes
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/object/klibrary.h>

// FBX begin namespace
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>


    /** 
     * A plug-in loading strategy that loads a single DLL by specifying the file name in the constructor, and unloads the DLL in its destructor.
     */
    class KFBX_DLL KFbxScopedLoadingFileName : public KFbxLoadingStrategy
    {
    public:
        /**
         *\name Public interface
         */
        //@{

        /** Constructor.
          * Load plug-in.
		  * \param pPath The file path.
          */
        explicit KFbxScopedLoadingFileName(const char* pPath);

        /** Destructor.
         * Unload plug-in.
         */
        virtual ~KFbxScopedLoadingFileName();

        //@}

    private:
        ///////////////////////////////////////////////////////////////////////////////
        //
        //  WARNING!
        //
        //	Anything beyond these lines may not be documented accurately and is 
        // 	subject to change without notice.
        //
        ///////////////////////////////////////////////////////////////////////////////
        #ifndef DOXYGEN_SHOULD_SKIP_THIS

        kLibHandle mInstance;
        KString mPath;
        
        // From KFbxLoadingStrategy
        virtual bool SpecificLoad(KFbxPluginData& pData);
        virtual void SpecificUnload();

        #endif
    };

// FBX end namespace
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMODULES_KFBXSCOPEDLOADINGFILENAME_H

