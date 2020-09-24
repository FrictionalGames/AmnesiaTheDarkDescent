/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxscopedloadingdirectory.h
 */

#ifndef FBXFILESDK_KFBXMODULES_KFBXSCOPEDLOADINGDIRECTORY_H
#define FBXFILESDK_KFBXMODULES_KFBXSCOPEDLOADINGDIRECTORY_H


#include <fbxfilesdk/fbxfilesdk_def.h>

// Local includes
#include <fbxfilesdk/kfbxmodules/kfbxloadingstrategy.h>

// FBX includes
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/object/klibrary.h>

// FBX begin namespace
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxPluginHandle;


    /** 
     * A plug-in loading strategy that loads all DLLs with a specific extension from a specific directory.
     * When this class is destroyed all of the plug-ins are unloaded.
     */
    class KFBX_DLL KFbxScopedLoadingDirectory : public KFbxLoadingStrategy
    {
    public:
        /**
         *\name Public interface
         */
        //@{

        /** Constructor. Load plug-ins.
		 * \param pDirectoryPath The directory path.
		 * \param pPluginExtension The plug-in extension.
         */
        KFbxScopedLoadingDirectory(const char* pDirectoryPath, const char* pPluginExtension);

        /** Destructor. Unload plug-ins.
        */
        virtual ~KFbxScopedLoadingDirectory();

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
        KString mDirectoryPath;
        KString mExtension;

        // From KFbxLoadingStrategy
        virtual bool SpecificLoad(KFbxPluginData& pData);
        virtual void SpecificUnload();

        // Plug-in management
        typedef KIntrusiveList<KFbxPluginHandle> PluginHandleList;
        PluginHandleList mPluginHandles;

	#endif
    };


#ifndef DOXYGEN_SHOULD_SKIP_THIS
   class KFbxPluginHandle
    {
        KFBX_LISTNODE(KFbxPluginHandle, 1);

    public:
        KFbxPluginHandle(kLibHandle pInstance=NULL) : mInstance(pInstance){}
        kLibHandle mInstance;
    };
#endif //DOXYGEN_SHOULD_SKIP_THIS


// FBX end namespace
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMODULES_KFBXSCOPEDLOADINGDIRECTORY_H

