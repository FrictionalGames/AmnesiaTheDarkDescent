/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxloadingstrategy.h
 */

#ifndef FBXFILESDK_KFBXMODULES_KFBXLOADINGSTRATEGY_H
#define FBXFILESDK_KFBXMODULES_KFBXLOADINGSTRATEGY_H


#include <fbxfilesdk/fbxfilesdk_def.h>

// Local includes 
#include <fbxfilesdk/kfbxmodules/kfbxplugin.h>
#include <fbxfilesdk/kfbxmodules/kfbxplugincontainer.h>

// FBX begin namespace
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

    /** 
     * Abstract class used to implemented some plug-in loading strategy.
     * A loading strategy dictate how some plug-ins will be loaded for instance.
     * We could have a simple strategy that loads only a single dll on PC. 
     * We could also implement a strategy that load multiple dlls from a directory.
     */
    class KFBX_DLL KFbxLoadingStrategy : public KFbxPluginContainer
    {

    public:
        /** Result state of loading plug-in.
         */
        enum kPluginsLoadedState {
            kAllLoaded,     //!< All plug-in are loaded.
            kNoneLoaded,    //!< No plug-in is loaded, i.e., there is not plug-in to load. 
            kAllFailed,     //!< All plug-in are failed to load.
            kSomeFailed     //!< Some plug-ins are loaded but some are failed.
        };

        /**
        *\name Public interface
        */
        //@{

        /** Execute the operation of loading the plug-in(s). The way it is executed is determined by the specific implementations.
        * \param pData  Plug in data that can be access inside the plug-ins.
        * \return If the plugin loading is successful return \c true, otherwise return \c false.
        */
        kPluginsLoadedState Load(KFbxPluginData& pData);

        /** Execute the operation of unloading the plug-in(s). The way it is executed is determined by the specific implementations.
        */
        void Unload();
        //@}

    protected:
        /**
        *\name User implementation
        */
        //@{
        /** Called by the Load method, it contains the specific user implementation strategy to load the desired plug-in(s).
		* \param pData  Plug in data that can be access inside the plug-ins.
        * \return If the plugin loading is successful return \c true, otherwise return \c false
        */
        virtual bool SpecificLoad(KFbxPluginData& pData) = 0;

        /** Called by the Unload method, it contains the specific user implementation strategy to unload the desired plug-in(s).
        */
        virtual void SpecificUnload() = 0;
        //@}

        //! Whether the plugin is loaded or not.
        kPluginsLoadedState mPluginsLoadedState;
    private:
        KFbxPluginData mData;
    };

// FBX end namespace
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMODULES_KFBXLOADINGSTRATEGY_H

