/*!  \file kfbxsdkmanager.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSDKMANAGER_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSDKMANAGER_H

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
#include <fbxfilesdk/kfbxplugins/kfbxmemoryallocator.h>
#include <fbxfilesdk/kfbxplugins/kfbxgeometryconverter.h>
#include <fbxfilesdk/kfbxplugins/kfbxproperty.h>
#include <fbxfilesdk/kfbxplugins/kfbxtypes.h>
#include <fbxfilesdk/kfbxplugins/kfbxlayer.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

// Forward declarations
class KFbxIOPluginRegistry;
class KFbxUserNotification;
class KFbxMessageEmitter;
class KFbxIOSettings;
class KFbxSdkManager_internal;
class KFbxPreviewManager;
class KFbxXRefManager;
class KFbxLocalizationManager;
class KFbxPlugin;
class KFbxAnimStack;
class KFbxReference;

    /** 
	  * SDK object manager.
	  *   The SDK manager is in charge of:
      *     \li scene element allocation, for example, KFbxScene::Create(pSdkManager, "").
      *     \li scene element deallocation, call KFbxSdkManager::Destroy() to deallocates all object created by the SDK manager.
      *     \li scene element search and access, please see \ref GlobalObjectManagement section.
      *
      * Upon destruction, all objects allocated by the SDK manager and not explicitly destroyed are destroyed as well. 
      * A derived class can be defined to allocate and deallocate specialized scene elements.
      * SDK manager can also specify custom memory allocators. \see SetMemoryAllocator
      * \remarks You could create more than one SDK manager. However, it's better to NOT share the same object among different managers.
      * \nosubgrouping
      */
    class KFBX_DLL KFbxSdkManager
    {
    public:
        /**
          * \name Memory Management
          */
        //@{

            /** SDK Memory management.
			  * \nosubgrouping
              * Use this method to specify custom memory management routines.
              * The FBX SDK will use the provided routines to allocate and
              * deallocate memory.
			  * \param pMemoryAllocator The memory allocator to set. 
              * \remarks Important: If you plan to specify custom memory management
              * routines, you must do so BEFORE creating the first SDK manager. Those
              * routines are global and thus shared between different instances of SDK managers.
              */
            static bool SetMemoryAllocator(KFbxMemoryAllocator* pMemoryAllocator);
        //@}


        /**
          * \name FBX SDK Manager Creation/Destruction/retrieval
          */
        //@{
            /** SDK manager allocation method.
              * \return A pointer to the SDK manager or \c NULL if this is an
              * evaluation copy of the FBX SDK and it is expired.
              */
            static KFbxSdkManager* Create();

            /** Destructor.
              * Deallocates all object previously created by the SDK manager.
              */
            virtual void Destroy();

			/** Get the current default FBX file format version number for this version of the FBX SDK.
			  * \param pMajor        Version major number.
			  * \param pMinor        Version minor number.
			  * \param pRevision     Version revision number.
			  */
			static void GetFileFormatVersion(int& pMajor, int& pMinor, int& pRevision);
        //@}


        /**
          * \name Plug and Object Definition and Management
          */
        //@{
        public:
            /** Class registration.
              * \param pName    The class name. For example, "KFbxMesh" for KFbxMesh class.
              * \param pFBX_TYPE_Class  FBX type of the specified class.
              * \param pFBX_TYPE_ParentClass     FBX type of parent class.
              * \param pFbxFileTypeName     The type name of the class in FBX file.
              * \param pFbxFileSubTypeName  The sub type name of the class in FBX file.
              * \return The class Id of the newly register class.
              * Such as:
              * \code RegisterFbxClass("KFbxCamera", FBX_TYPE(KFbxCamera), FBX_TYPE(KFbxNodeAttribute)); \endcode
              */
            template <typename T1,typename T2> inline kFbxClassId RegisterFbxClass(char const *pName,T1 const *pFBX_TYPE_Class,T2 const *pFBX_TYPE_ParentClass,const char *pFbxFileTypeName=0,const char *pFbxFileSubTypeName=0) {
                T1::ClassId  = Internal_RegisterFbxClass(pName,T2::ClassId,(kFbxPlugConstructor)T1::SdkManagerCreate,pFbxFileTypeName,pFbxFileSubTypeName );
                return T1::ClassId;
            }
         
            /** Runtime class registration.
              * \param pName                    The class name. For example, "KFbxUIWidgetBoolean".
              * \param pFBX_TYPE_ParentClass    FBX type of parent class.
              * \param pFbxFileTypeName         The type name of the class in FBX file.
              * \param pFbxFileSubTypeName      The sub type name of the class in FBX file.
              * \return The class Id of the newly register class.
              * Such as:
              * \code RegisterRuntimeFbxClass( "KFbxUIWidgetBoolean", FBX_TYPE(KFbxUIWidgetDefinition), NULL, "KFbxUIWidgetBoolean"); \endcode
              */
            template <typename T> inline kFbxClassId    RegisterRuntimeFbxClass(char const *pName,T const *pFBX_TYPE_ParentClass,const char *pFbxFileTypeName=0,const char *pFbxFileSubTypeName=0) {
                return Internal_RegisterFbxClass(pName,T::ClassId,(kFbxPlugConstructor)T::SdkManagerCreate,pFbxFileTypeName,pFbxFileSubTypeName );
            }
            
			/** Runtime class unregistration.
			  * \param pName The class name.
			  */
            inline void UnregisterRuntimeFbxClass(char const* pName)
            {
                kFbxClassId lClassId = FindClass(pName);

                if( !(lClassId == kFbxClassId()) )
                {
                    Internal_UnregisterFbxClass(lClassId);
                }
            }
            
			/** Override class.
			  * \param pFBX_TYPE_Class      FBX type of class.
			  * \param pFBX_TYPE_OverridenClass FBX type of overridden class.
			  * \return The class Id
			  */
            template <typename T1,typename T2> inline kFbxClassId OverrideFbxClass(T1 const *pFBX_TYPE_Class,T2 const *pFBX_TYPE_OverridenClass) {
                T1::ClassId  = Internal_OverrideFbxClass(T2::ClassId,(kFbxPlugConstructor)T1::SdkManagerCreate );
                return T1::ClassId;
            }

            /** Create class
			  * \param pClassId		Class ID for the created class.
			  * \param pName		Specify the name for the created class.
			  * \param pFBXType		The type name of the created class.
			  * \param pFBXSubType	The sub type name of the created class.
			  * \remark				This function is deprecated, please use CreateNewObjectFromClassId() instead.
			  */
            K_DEPRECATED KFbxPlug* CreateClass(kFbxClassId pClassId, char const *pName, const char* pFBXType=0, const char* pFBXSubType=0);

            /** Create a new object of the specified ClassId.
			  * \param pClassId		The ClassId of the object to be created.
			  * \param pName		The name given to the newly created object.
			  * \param pContainer	An optional parameter to specify which object will "contain" the new object. By contain, we mean
			  *						the new object will become a source to the container, connection-wise.
			  * \param pCloneFrom	A valid object pointer to use as the reference for cloning the object upon construction.
			  * \return				If not null, a new instance of the specified class.
			  * \remark				This function will return NULL if the ClassId used is invalid. New ClassId can be registered using
			  *						the function RegisterFbxClass().
			  */
            KFbxObject* CreateNewObjectFromClassId(kFbxClassId pClassId, char const* pName, KFbxObject* pContainer=NULL, const KFbxObject* pCloneFrom=NULL);

            /** Create class
			  * \param pContainer	New class will be connected to pContainer.
			  * \param pClassId		Class ID for the created class.
			  * \param pName		Specify the name for the created class
			  * \param pFBXType		The type name of the created class.
			  * \param pFBXSubType	The sub type name of the created class.
			  * \remark				This function is deprecated, please use CreateNewObjectFromClassId() instead.
			  */
            K_DEPRECATED KFbxPlug* CreateClass(KFbxObject* pContainer, kFbxClassId pClassId, const char* pName, const char* pFBXType=0, const char* pFBXSubType=0);

            /** Find class by the specified name.
			  * \param pClassName Class Name to find.
			  */
            kFbxClassId     FindClass(const char* pClassName) const;

			/** Find file class.
			  * \param pFbxFileTypeName     Specify the type name in FBX file to find.
			  * \param pFbxFileSubTypeName  Specify by The sub type name in FBX file to find.
			  */
            kFbxClassId     FindFbxFileClass(const char* pFbxFileTypeName, const char* pFbxFileSubTypeName) const;

			/** Class unregistration.
			  * \param pFBX_TYPE_Class  FBX type of unregistered class.
			  */
            template <typename T> inline void UnregisterFbxClass( T const* pFBX_TYPE_Class )
            {
                Internal_UnregisterFbxClass( T::ClassId );
                T::ClassId = kFbxClassId();
            }

        //@}

        /**
          * \name Error Management
          */
        //@{
        public:
            /** Retrieve error object.
             *  \return Reference to the Manager's error object.
             */
            KError& GetError();

            /** \enum EError Error codes
              */
            typedef enum
            {
                eOBJECT_NOT_FOUND,    /**< The requested object was not found in the Manager's database. */
                eNAME_ALREADY_IN_USE, /**< A name clash occurred.                                        */
                eERROR_COUNT          /**< Mark the end of the error enum.                               */
            } EError;

            /** Get last error code.
             *  \return Last error code.
             */
            EError GetLastErrorID() const;

            /** Get last error string.
             *  \return Textual description of the last error.
             */
            const char* GetLastErrorString() const;

        //@}

        /**
          * \name Data Type Management
          */
        //@{
            /** Register a new data type to the manager
			 *  \param pName The type name.
			 *  \param pType The data type.
             *  \return The newly created KFbxDataType
             */
            KFbxDataType CreateFbxDataType(const char *pName,EFbxType pType);

            /** Find a data type from the type name.
			 *  \param pDataType The type name.
             *  \return the found datatype. return null if not found
             */
            KFbxDataType const &GetFbxDataTypeFromName(const char *pDataType);

            /** List the data types
             *  \return the number of registered datatypes
             */
            int GetFbxDataTypeCount() const;

            /** Find a data types at pIndex.
			 *  \param pIndex The data type index.
             *  \return the found datatype. return null if not found
             */
            KFbxDataType &GetFbxDataType(int pIndex);
        //@}


        /**
          * \name User Notification Object
          */
        //@{
            /** Access to the unique UserNotification object.
              * \return The pointer to the user notification or \c NULL \c if the object
              * has not been allocated.
            */
            KFbxUserNotification* GetUserNotification() const;

			/** Set the user notification
			  * \param pUN  
			  */
            void SetUserNotification(KFbxUserNotification* pUN);
        //@}

        /**
          * \name IOSettings Object
          */
        //@{
            /** Access to a IOSettings object.
              * \return The pointer to IOSettings or \c NULL \c if the object
              * has not been allocated.
            */
			virtual KFbxIOSettings * GetIOSettings() const;

			/** Set the IOSettings pointer
			  * \param pIOSettings  
			  */
			virtual void SetIOSettings(KFbxIOSettings * pIOSettings);
        //@}


        /**
          * \name Message Emitter (for Message Logging)
          */
        //@{
            /** Access to the unique KFbxMessageEmitter object.
              * \return The pointer to the message emitter.
            */
            KFbxMessageEmitter & GetMessageEmitter();
            /** Sets to the unique KFbxMessageEmitter object.
              * \param pMessageEmitter the emitter to use, passing NULL will reset to the default emitter.
              * The object will be deleted when the SDK manager is destroyed, thus ownership is transfered.
            */
            bool SetMessageEmitter(KFbxMessageEmitter * pMessageEmitter);
        //@}

        private:
            KArrayTemplate<KFbxLocalizationManager *>   mKFbxLocalizations;
        public:
        /**
          * \name Localization Hierarchy
          */
        //@{
            /** Add a localization object to the known localization providers.
              * \param pLocManager the localization object to register.
            */
            void AddLocalization( KFbxLocalizationManager * pLocManager );
            /** Remove a localization object from the known localization providers.
              * \param pLocManager the localization object to remove.
            */
            void RemoveLocalization( KFbxLocalizationManager * pLocManager );
            /** Select the current locale for localization.
              * \param pLocale the locale name, for example "fr" or "en-US".
            */
            bool SetLocale( const char * pLocale );
            /** Localization helper function. Calls each registered localization manager
              * until one can localizes the text.
              * \param pID the identifier for the text to localize.
              * \param pDefault the default text. Uses pID if NULL.
              * \return the potentially localized text. May return the parameter passed in.
            */
            const char * Localize( const char * pID, const char * pDefault = NULL) const;
        //@}

        /**
          * \name Sub-Manager Management
          */
        //@{

            /** Retrieve the manager responsible for managing object previews.
              * \return The Preview manager for this SDK manager.
              */
            KFbxPreviewManager& GetPreviewManager();

        //@}

        /**
          * \name XRef Manager
          */
        //@{
            /** Retrieve the manager responsible for managing object XRef resolution.
              * \return The XRef manager for this SDK manager.
              */
            KFbxXRefManager& GetXRefManager();
        //@}

        /**
          * \name Library Management
          */
        //@{
            /** Retrieve the main object Libraries
              * \return The Root library
              */
            KFbxLibrary*    GetRootLibrary() const;
            KFbxLibrary*    GetSystemLibraries() const;
            KFbxLibrary*    GetUserLibraries() const;
        //@}

        /**
          * \name Plug-in Registry Object
          */
        //@{
            /** Access to the unique KFbxIOPluginRegistry object.
              * \return The pointer to the user KFbxIOPluginRegistry
            */
            KFbxIOPluginRegistry* GetIOPluginRegistry() const;
        //@}

        /**
          * \name Fbx Generic Plugins Management
          */
        //@{
			/** Load plug-ins directory
              * \param pFilename The directory path.
			  * \param pExtensions The plug in extension.
			  * \return \c True
			  */
            bool LoadPluginsDirectory (char *pFilename,char *pExtensions);

			/** Load plug-in
              * \param pFilename The file name
			  * \return \c True
 			  */
            bool LoadPlugin (char *pFilename);

			/** Unload all plug-ins
			*/
            bool UnloadPlugins();

            /** Emit plugins event.
			  * \param pEvent The event to be emitted.
			  */
            bool EmitPluginsEvent(KFbxEventBase const &pEvent);
           
			//!Get plugins.
            KArrayTemplate<KFbxPlugin const*> GetPlugins() const;

			/** get plugins count
			  * \return The number of plugins.
			  */
			int GetPluginCount() const;

			/** Find plug in.
			  * \param pName The plug in name.
			  * \param pVersion The plug in version.
			  * \return The plugin, \c null if not found.
			  */
			KFbxPlugin*	FindPlugin(const char* pName, const char* pVersion) const;
        //@}

		
        /**
          * \name IO Settings
          */
		//@{
        // Add IOSettings in hierarchy from different modules
        
	    /** Fill IO Settings for registered readers. 
		  * \param pIOS The properties hierarchies to fill.
		  */
        void FillIOSettingsForReadersRegistered(KFbxIOSettings & pIOS);

	    /** Fill IO Settings for registered writers. 
		  * \param pIOS The properties hierarchies to fill.
		  */
        void FillIOSettingsForWritersRegistered(KFbxIOSettings & pIOS);

        /** Fill common IO Settings 
		  * \param pIOS The properties hierarchies to fill.
		  * \param pImport If \c true, import properties are set, otherwise export properties are set.
		  */
        void FillCommonIOSettings(KFbxIOSettings & pIOS, bool pImport);

		private:

		/** Build import groups hierarchies.
		  * \param pIOS  The properties hierarchies to fill.
		  */
        void Create_Common_Import_IOSettings_Groups(KFbxIOSettings & pIOS);

		/** Build export groups hierarchies.
		  * \param pIOS  The properties hierarchies to fill.
		  */       
        void Create_Common_Export_IOSettings_Groups(KFbxIOSettings & pIOS);

        /** Add common import properties.
		  * \param pIOS  The properties hierarchies to fill.
		  */
        void Add_Common_Import_IOSettings(KFbxIOSettings & pIOS);

        /** Add common export properties.
		  * \param pIOS  The properties hierarchies to fill.
		  */
        void Add_Common_Export_IOSettings(KFbxIOSettings & pIOS);

        /** Add common Read/Write Import properties.
		  * \param pIOS  The properties hierarchies to fill.
		  */
        void Add_Common_RW_Import_IOSettings(KFbxIOSettings & pIOS);

        /** Add common Read/Write export properties.
		  * \param pIOS  The properties hierarchies to fill.
		  */
        void Add_Common_RW_Export_IOSettings(KFbxIOSettings & pIOS);

        //@}

        // Temporary for Managing global objects
        /**
          * \name Global Object Management 
          */
        //@{

        protected:
            KArrayTemplate<KFbxObject*>     mObjectArray;
            KArrayTemplate<KFbxObject*>     mDestroyingObjects;
            KArrayTemplate<KFbxNode*>       mDestroyingKFbxNode;
            KArrayTemplate<KFbxTexture*>    mDestroyingKFbxTexture;
            KArrayTemplate<KFbxCluster*>    mDestroyingKFbxCluster;
            KArrayTemplate<KFbxDocument*>   mDestroyingKFbxDocument;
            KArrayTemplate<KFbxScene*>      mDestroyingKFbxScene;
            KArrayTemplate<KFbxSurfaceMaterial*> mDestroyingKFbxSurfaceMaterial;

            int                             mIsDestroyingScene;

        public:
            /** Register object with the manager.
			  * \internal
			  * \param pPlug The object to be registered.
              * \anchor GlobalObjectManagement
              */
            void RegisterObject(KFbxPlug const* pPlug);

            /** Unregister object with the manager.
			  * \internal
              * \param pPlug The object to be unregistered.
			  */
            void UnregisterObject(KFbxPlug const* pPlug);

            /** Register a list of objects with the manager.
			  * \internal
			  * \param pArray The list of object to be registered.
              */
            void RegisterObjects(const KArrayTemplate<KFbxObject*>& pArray);

            /** Unregister a list of objects with the manager.
			  * \internal
              * \param pArray The list of object to be unregistered.
			  */
            void UnregisterObjects(const KArrayTemplate<KFbxObject*>& pArray);

            void IncreaseDestroyingSceneFlag();
            void DecreaseDestroyingSceneFlag();

        private:
            KArrayTemplate<KFbxNode *>  mKFbxNodeArray;

        public:
			/** Get the count of KFbxNode.
			 */
            inline int  GetSrcObjectCount(KFbxNode const *) const                 { return mKFbxNodeArray.GetCount();  }
			/** Get the (pIndex)th KFbxNode.
			  * \param pIndex the index.
			  */
            inline KFbxNode*        GetSrcObject(KFbxNode const *,int pIndex=0)  const    { return mKFbxNodeArray.GetAt(pIndex); }
        private:
            KArrayTemplate<KFbxTexture *>   mKFbxTextureArray;
        public:
			/** Get the count of KFbxTexture.
			  */
            inline int  GetSrcObjectCount(KFbxTexture const *) const              { return mKFbxTextureArray.GetCount();  }
			/** Get the (pIndex)th KFbxTexture.
			  * \param pIndex the index.
			  */
            inline KFbxTexture*     GetSrcObject(KFbxTexture const *,int pIndex=0)  const   { return mKFbxTextureArray.GetAt(pIndex); }
        private:
            KArrayTemplate<KFbxCluster *>   mKFbxClusterArray;
        public:
			/** Get the count of KFbxCluster.
			  */
            inline int  GetSrcObjectCount(KFbxCluster const *) const                     { return mKFbxClusterArray.GetCount();  }
			/** Get the (pIndex)th KFbxCluster.
			  * \param pIndex the index.
			  */
            inline KFbxCluster*     GetSrcObject(KFbxCluster const *,int pIndex=0) const   { return mKFbxClusterArray.GetAt(pIndex); }
        private:
            KArrayTemplate<KFbxScene *> mKFbxSceneArray;
        public:
			/** Get the count of KFbxScene.
			  */
            inline int  GetSrcObjectCount(KFbxScene const *) const                 { return mKFbxSceneArray.GetCount();  }
			/** Get the (pIndex)th KFbxScene.
			  * \param pIndex the index.
			  */
            inline KFbxScene*       GetSrcObject(KFbxScene const *,int pIndex=0) const  { return mKFbxSceneArray.GetAt(pIndex); }
        private:
            KArrayTemplate<KFbxDocument *>  mKFbxDocumentArray;
        public:
			/** Get the count of KFbxDocument.
			  */
            inline int  GetSrcObjectCount(KFbxDocument const *) const            { return mKFbxDocumentArray.GetCount();  }
			/** Get the (pIndex)th KFbxDocument.
			  * \param pIndex the index.
			  */
            inline KFbxDocument*        GetSrcObject(KFbxDocument const *,int pIndex=0) const     { return mKFbxDocumentArray.GetAt(pIndex); }
        private:
            KArrayTemplate<KFbxSurfaceMaterial *>   mKFbxSurfaceMaterialArray;
        public:
			/** Get the count of KFbxSurfaceMaterial.
			  */
            inline int  GetSrcObjectCount(KFbxSurfaceMaterial const *)  const               { return mKFbxSurfaceMaterialArray.GetCount();  }
			/** Get the (pIndex)th KFbxSurfaceMaterial.
			  * \param pIndex the index.
			  */
            inline KFbxSurfaceMaterial*     GetSrcObject(KFbxSurfaceMaterial const *,int pIndex=0) const      { return mKFbxSurfaceMaterialArray.GetAt(pIndex); }
       

        private:
             KArrayTemplate<KFbxReference *>   mKFbxReferenceArray;
        public:
            /**
            * \name Reference Management
            */
            //@{

            /** Get number of references.
            * \return Number of references.
            */
            int GetReferenceCount() const;

            /** Get reference at given index.
            * \param pIndex Position in the list of references.
            * \return Pointer to the reference or \c NULL if index is out of bounds.
            */
            KFbxReference* GetReference(int pIndex) const;

            /** Add a reference.
            * \param pReference The reference to be added.
            * \return If the reference is correctly added to the scene, return \c true otherwise, if the reference is
            *  already there, returns \c false.
            */
            int AddReference(KFbxReference* pReference);

            /** Remove the specified reference from reference list.
            * \param pReference The reference to be removed.
            * \return If the reference was successfully removed, return \c true otherwise, if the
            *  reference could not be found returns \c false.
            */
            bool RemoveReference(KFbxReference* pReference);

            /** Clear the specified reference from the SDK manager.
            * \param pReference The reference to be removed.
            * \return If the reference was successfully cleared from the SDK manager, return \c true otherwise, if the
            *  reference could not be found returns \c false.
            */
            bool ClearReference(KFbxReference* pReference);

            //@}

        public:
            /** Add a prefix to a name.
              * \param pPrefix The prefix to be added to the \c pName. This
              * string must contain the "::" characters in order to be considered
              * as a prefix.
              * \param pName The name to be prefix.
              * \return The prefixed string
              * \remarks If a prefix already exists, it is removed before
              * adding \c pPrefix.
              */
            static KString PrefixName(char const* pPrefix, char const* pName);


    ///////////////////////////////////////////////////////////////////////////////
    //
    //  WARNING!
    //
    //  Anything beyond these lines may not be documented accurately and is
    //  subject to change without notice.
    //
    ///////////////////////////////////////////////////////////////////////////////

    #ifndef DOXYGEN_SHOULD_SKIP_THIS

    public:
    
        static KFbxSdkManager* GetDefaultKFbxManager();
    
        bool CanDestroyFbxSrcObject(KFbxObject* pObject, KFbxObject* pSrcObject, bool pRecursive, bool pDependents) const;

        void CreateMissingBindPoses(KFbxScene *pScene);
        int  GetBindPoseCount(KFbxScene *pScene) const;

    private:
		kFbxClassId     Internal_RegisterFbxClass(const char* pClassName, kFbxClassId pParentClassId, kFbxPlugConstructor=0, char const* pFbxFileTypeName=0, char const* pFbxFileSubTypeName=0);
        bool            Internal_RegisterFbxClass(kFbxClassId pClassId);
        kFbxClassId     Internal_OverrideFbxClass(kFbxClassId pClassId, kFbxPlugConstructor=0);

        void            Internal_UnregisterFbxClass (kFbxClassId    pClassId);

        template< class T > void RemoveObjectsOfType( KArrayTemplate<T>& pArray, kFbxClassId pClassId );
        KArrayTemplate<KFbxObject*> mSystemLockedObjects; // objects that can only be destroyed when the manager is destroyed

        #ifdef K_FBXNEWRENAMING

            enum ERenamingMode
            {
                eMAYA_TO_FBX,
                eFBX_TO_MAYA,
                eLW_TO_FBX,
                eFBX_TO_LW,
                eXSI_TO_FBX,
                eFBX_TO_XSI,
                eMAX_TO_FBX,
                eFBX_TO_MAX,
                eMB_TO_FBX,
                eFBX_TO_MB
            };

            void RenameFor(ERenamingMode pMode);

            /** Rename all object to remove name clashing.
             * \param pFromFbx Index of the requested KFbxConstraint.
             * \param pIgnoreNS Index of the requested KFbxConstraint.
             * \param pIsCaseSensitive .
             * \param pComvertToWideChar Needs to be converted to wide char.
             * \param pReplaceNonAlphaNum replace non alpha character.
             * \param pNameSpaceSymbol identifier of a namespace.
             *  \return void.
             */
            void ResolveNameClashing(bool pFromFbx, bool pIgnoreNS, bool pIsCaseSensitive, bool pComvertToWideChar, bool pReplaceNonAlphaNum, KString pNameSpaceSymbol);

        #endif


    protected:
        KFBXNEW_DECLARE_FRIEND
        KFbxSdkManager();
        virtual ~KFbxSdkManager();

        /** Clear the manager of all the objects it has created.
          * \param pExcludeList List of classId that should not be cleared.
          * \remarks This exclusion list will only affect the mObjectArray. All the
          * scenes, nodes and documents will be destroyed. Also, because of the 
          * high risk of instability that this behavior can introduce, the caller
          * of this function with a non empty argument is responsible to ensure that
          * the system stay in a stable state.
          * \param pDestroyUserNotification If false, the mUserNotification member is not
          * destroyed by this call.
          * \note Some objects called SystemLockedObjects will NOT be cleared, either. They are KFbxDocumentInfo, KFbxLibrary(include FbxRootLibrary, mFbxSystemLibraries,mFbxUserLibraries).
          * They will be delete after calling KFbxSdkManager::Destroy().
        */
        virtual void Clear(KArrayTemplate<kFbxClassId>* pExcludeList = NULL, bool pDestroyUserNotification=true);

        // this object is destroyed when the manager is destroyed.
        KFbxUserNotification* mUserNotification;

		// this object is destroyed when the manager is destroyed.
		KFbxIOSettings * mIOSettings;

        // this object is destroyed when the manager is destroyed.
        KFbxMessageEmitter * mMessageEmitter;

        // this object is destroyed when the manager is destroyed.
        KFbxIOPluginRegistry* mRegistry;

        mutable KError mError;

		friend class KFbxPose;

        // Class Management
        void ClassInit();
        void ClassRelease();

	public:
		int GetFbxClassCount() const;
		kFbxClassId GetNextFbxClass(kFbxClassId pClassId /* invalid id: first one */) const;

	private:
		KFbxSdkManager_internal* mInternal;
        static KFbxSdkManager* smDefaultManager;

    #endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

    };

    typedef KFbxSdkManager KFbxSdk;

    typedef KFbxSdkManager* HKFbxSdkManager;

    // Connection management
    template < class T > inline int KFbxGetSrcCount(KFbxSdkManager *pObject)                    { T const* FBXTYPE = 0; return pObject ? pObject->GetSrcObjectCount(FBXTYPE) : 0; }
    template < class T > inline int KFbxGetSrcCount(KFbxSdkManager *pObject, T const* FBXTYPE)   { return pObject ? pObject->GetSrcObjectCount(FBXTYPE) : 0; }
    template < class T > inline T*  KFbxGetSrc(KFbxSdkManager *pObject,int pIndex=0)  { T const* FBXTYPE = 0; return pObject ? (T *)pObject->GetSrcObject(FBXTYPE,pIndex) : 0; }
    template < class T > inline T*  KFbxFindSrc(KFbxSdkManager *pObject,char const *pName) 
    {
        for (int i=0; i<KFbxGetSrcCount<T>(pObject); i++) {
          T* lObject = KFbxGetSrc<T>(pObject,i);
            if (strcmp(lObject->GetName(),pName)==0 ){
                return lObject;
            }
        }
        return 0;
    }

    // Take index
    #define TAKE_NO_ANIMATION   0
    #define TAKE_DEFAULT        -1
    #define TAKE_INVALID        -3

    // Class Registration management

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSDKMANAGER_H

