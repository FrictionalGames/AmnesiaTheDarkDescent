/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxlibrary.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXLIBRARY_H
#define FBXFILESDK_KFBXPLUGINS_KFBXLIBRARY_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/fbxcore/fbxcollection/kfbxdocument.h>
#include <fbxfilesdk/kfbxplugins/kfbxobjectfilter.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxLocalizationManager;
class KFbxCriteria;

/** This library class represents libraries that store sub-libraries and shading objects.
  * Shading objects are objects of class KFbxTexture, KFbxSurfaceMaterial, and KFbxLight.
  * \nosubgrouping
  */
class KFBX_DLL KFbxLibrary : public KFbxDocument
{
    KFBXOBJECT_DECLARE(KFbxLibrary,KFbxDocument);

public:
    //! Returns a pointer to the parent library if one exists.
    KFbxLibrary* GetParentLibrary(void) const;

    /** Sets whether not this library is a system library.
      * \param pSystemLibrary       A flag which indicates whether or not this library is a system library.
      */
    void SystemLibrary(bool pSystemLibrary);

    //! Returns whether or not this library is a system library.
    bool IsSystemLibrary() const;

    /** The prefix must not include the dash and language code, nor
      * can it contain the extension.  But if you want, it can contain 
      * a folder or sub-folder, such as: locales/mydocloc.
      * This is resolved using the XRef Manager, with priority
      * given to the library's ".fbm" folder, if one exists.
	  * \param pPrefix              New prefix to be set.
      */
    void LocalizationBaseNamePrefix(const char* pPrefix);

	//! Retrieves the localization prefix.
    KString LocalizationBaseNamePrefix() const;

    // =======================================================================
    //
    // sub-library
    //
    // =======================================================================

    /** Adds a sub-library
	  * \param pSubLibrary          The sub-library to be added.
      * \return                     \c True if adding the sub-library is successful, returns \c false if not.
	  */
    bool AddSubLibrary(KFbxLibrary* pSubLibrary);

    /** Removes a sub-library
	  * \param pSubLibrary          The sub-library to be removed.
	  * \return                     \c True if the sub-library is removed, \c false if not.
	  */
    bool RemoveSubLibrary(KFbxLibrary* pSubLibrary);

    //! Returns the total number of sub-libraries
    int GetSubLibraryCount(void) const;

    /** Returns the sub-library at the specified index. 
	  * \param pIndex               The sub-library index.
	  */
    KFbxLibrary* GetSubLibrary(int pIndex) const;

    /** Clones the specified asset. 
      * \param pToClone             The asset to be cloned.
      * \param pOptionalDestinationContainer    The container for the asset copy.
      * \remarks                    The asset and all its dependents are cloned.               
      */
    KFbxObject* CloneAsset( KFbxObject* pToClone, KFbxObject* pOptionalDestinationContainer = NULL) const;

    
    /** Returns a criteria filter which you can use to filter objects
      * when iterating items in the library. Only real 'assets' are returned,
      * rather than FBX support objects.  This currently includes
      * lights, environments, materials and textures (maps).
      * This is typically used to IMPORT from a library.
      */
    static KFbxCriteria GetAssetCriteriaFilter();

    /** Returns a filter which you should use when you clone / export objects. 
      * This filters out objects that should remain in the asset library.
      *
      * Use this to EXPORT from a library (or CLONE from a library).
      */
    static KFbxCriteria GetAssetDependentsFilter();

    /** Transfers ownership from the source library to this library for any assets passing through the filter.
      * It is assumed that name conflicts and other details have been resolved beforehand.
      *
      * External asset files required by the assets are copied (not moved). It's
      * up to the owner of the source library to clean up the files if the files are
      * not on a read-only transport. If this document hasn't been committed yet,
      * the assets will not be copied.
      *
      * Returns true if no filtered assets were skipped.  If no assets pass through 
      * the filter, it returns true, because nothing has been skipped.
      *
      * This may leave the source library in an invalid state. For example, the source library 
      * would be in an invalid state if you had decided to transfer texture objects to the library,
      * but materials were kept in the source library.
      *
      * To safeguard against this, the transfer disconnects objects, and thus materials
      * are left without textures.
      *
      * When you transfer an object, all its dependents come with it.  If you move
      * a material, it takes the associated textures as well. Although if you moved a texture, 
      * the material would not be transferred with it.
	  * \param pSrcLibrary          The source library to be imported.
      * \return                     \c True if no filtered assets are skipped.
      **/
    bool ImportAssets(KFbxLibrary* pSrcLibrary);

    /** Transfers ownership from the source library to this library for any assets passing through the filter.
      * It is assumed that name conflicts and other details have been resolved beforehand.
      *
      * External asset files required by the assets are copied (not moved). It's
      * up to the owner of the source library to clean up the files if the files are
      * not on a read-only transport. If this document hasn't been committed yet,
      * the assets will not be copied.
      *
      * Returns true if no filtered assets were skipped.  If no assets pass through 
      * the filter, it returns true, because nothing has been skipped.
      *
      * This may leave the source library in an invalid state. For example, the source library 
      * would be in an invalid state if you had decided to transfer texture objects to the library,
      * but materials were kept in the source library.
      *
      * To safeguard against this, the transfer disconnects objects, and thus materials
      * are left without textures.
      *
      * When you transfer an object, all its dependents come with it.  If you move
      * a material, it takes the associated textures as well. Although if you moved a texture, 
      * the material would not be transferred with it.
	  * \param pSrcLibrary          The source library to be imported.
	  * \param pAssetFilter         The asset filter.
      * \return                     \c True if no filtered assets are skipped.
      **/
    bool ImportAssets(KFbxLibrary* pSrcLibrary, const KFbxCriteria& pAssetFilter);


    /** Returns a new instance of a library member.
      * This instantiates the first object found that matches the filter.
      * \param pFBX_TYPE            The type of member
      * \param pFilter              A user specified filter
      * \param pRecurse             A flag that indicates whether to check sub-libraries
      * \param pOptContainer        Optional container for the cloned asset
      * \return                     A new instance of the member. Note that the new member is not included with this library.
      */
    template < class T > T* InstantiateMember( T const* pFBX_TYPE, const KFbxObjectFilter& pFilter, bool pRecurse = true, KFbxObject* pOptContainer = NULL);


	// =======================================================================
	//
	// Localization
	//
	// =======================================================================
    /** Returns the localization manager for the library.
      */

    KFbxLocalizationManager & GetLocalizationManager() const;

    /** Localization helper function. Calls the FBX SDK manager implementation.
      * Sub-classes that manage their own localization can over-ride this function.
      * \param pID                  The identifier for the text to localize.
      * \param pDefault             The default text. Uses pID if NULL.
      * \return                     The potentially localized text. May return the parameter passed in.
    */
    virtual const char * Localize( const char * pID, const char * pDefault = NULL ) const;

    // =======================================================================
    //
    // Shading Object
    //
    // =======================================================================

    /** Adds a shading object.
	  * \param pShadingObject       The shading object to be added.
	  */
    bool AddShadingObject(KFbxObject* pShadingObject);

    /** Removes a shading object.
	  * \param pShadingObject       The shading object to be removed.
	  */
    bool RemoveShadingObject(KFbxObject* pShadingObject);

    //! Returns the total number of shading objects
    int GetShadingObjectCount(void) const;

    /** Returns the shading object at the specified index.
	  * \param pIndex               Shading object index. 
	  * \return                     The shading object located at the specified index.
	  */
    KFbxObject* GetShadingObject(int pIndex) const;

    /** Returns the number of shading objects according to their implementations.
      * \param pCriteria            Filtering criteria that identifies what kind of
      *                             implementations to consider.
      * \returns                    The number of shading objects corresponding to the filtering parameters
      */
    int GetShadingObjectCount(
        const KFbxImplementationFilter & pCriteria
    ) const;

    /** Returns a handle on the shading object at the specified index that corresponds to the given filtering parameters.
      * \param pIndex               Shading object index.
      * \param pCriteria            Filtering criteria that identifies what kind of
      *                             implementations to consider.
      * \returns                    A handle on the shading object at the specified index that corresponds to the given filtering parameters.
      */
    KFbxObject* GetShadingObject(
        int pIndex,
        const KFbxImplementationFilter & pCriteria
    ) const;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
	// Constructor / Destructor
	KFbxLibrary(KFbxSdkManager& pManager, char const* pName);

	virtual void Destruct(bool pRecursive, bool pDependents);
	virtual void Construct(const KFbxLibrary* pFrom);

	mutable KFbxLocalizationManager * mLocalizationManager;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};


    template < class T > T* KFbxLibrary::InstantiateMember( T const* pFBX_TYPE, const KFbxObjectFilter& pFilter, bool pRecurse, KFbxObject* pOptContainer )
    {
        // first check all materials in the library.
        int i = 0;
        for( i = 0; i < GetMemberCount( FBX_TYPE(T) ); ++i )
        {
            T* lObject = GetMember( FBX_TYPE(T), i );
            if( pFilter.Match(lObject) )
                return KFbxCast<T>(CloneAsset(lObject,pOptContainer));
        }

        if( pRecurse )
        {
            // then check all materials in each sub-library.
            for( i = 0; i < GetMemberCount( FBX_TYPE(KFbxLibrary) ); ++i )
            {
                KFbxLibrary* lLibrary = GetMember( FBX_TYPE(KFbxLibrary), i );
                T* lClonedObject = lLibrary->InstantiateMember( pFBX_TYPE, pFilter, pRecurse, pOptContainer );
                if( lClonedObject )
                    return lClonedObject;
            }
        }

        return NULL;
    }


  /** \class KFbxEventPopulateSystemLibrary
    * \nosubgrouping
    * \brief Library events are triggered when an application requires specific library content
    */
    class KFBX_DLL KFbxEventPopulateSystemLibrary : public KFbxEvent<KFbxEventPopulateSystemLibrary>
    {
      KFBXEVENT_DECLARE(KFbxEventPopulateSystemLibrary)

        public:			
        /**
          *\name Constructor / Destructor
          */
        //@{

			/** Constructor.
			  * \param pLibrary 
			  */
            KFbxEventPopulateSystemLibrary(KFbxLibrary *pLibrary) { mLibrary = pLibrary; }

        //@}
       
            public:
		/**
          *\name Member access
          */
        //@{
				//!Retrieve the library.
                inline KFbxLibrary* GetLibrary() const { return mLibrary; }
            private:
                KFbxLibrary*    mLibrary;
        //@}
    };

    /** \class KFbxEventUpdateSystemLibrary
	  * \nosubgrouping
      * \brief This library event is triggered when an application requires an update to a specific library
      */
    class KFBX_DLL KFbxEventUpdateSystemLibrary : public KFbxEvent<KFbxEventUpdateSystemLibrary>
    {
        KFBXEVENT_DECLARE(KFbxEventUpdateSystemLibrary)
        public:
	  /**
        *\name Constructor and Destructor
        */
        //@{

			/**Constructor.
			  * \param pLibrary
			  */
            KFbxEventUpdateSystemLibrary(KFbxLibrary *pLibrary) { mLibrary = pLibrary; }

		//@}

	  /**
        *\name Member access
        */
        //@{

			//! Retrieve the library.
            inline KFbxLibrary* GetLibrary() const { return mLibrary; }

		//@}
        private:
                KFbxLibrary*    mLibrary;
    };

    /**
     * This library event is used by the asset building system to request that a
     * library provides its localization information.  This is used by the asset
     * builder.
     *
     * This is used by libraries that do not want to include their
     * data in the main asset library's xlf file, but rather as a
     * separate file.  These libraries are then responsible for
     * loading their files back.
    */
    class KFBX_DLL KFbxEventWriteLocalization : public KFbxEvent<KFbxEventWriteLocalization>
    {
        KFBXEVENT_DECLARE(KFbxEventWriteLocalization)
        public:
            KFbxEventWriteLocalization(KFbxLibrary *pAssetLibrary) { mAssetLibrary = pAssetLibrary; }

            //! Retrieve the library.
            inline KFbxLibrary* GetLibrary() const { return mAssetLibrary; }

        private:
            KFbxLibrary*    mAssetLibrary;
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  WARNING!
    //
    //  Anything beyond these lines may not be documented accurately and is
    //  subject to change without notice.
    //
    ///////////////////////////////////////////////////////////////////////////////
    #ifndef DOXYGEN_SHOULD_SKIP_THIS

    // Some assets correspond 1:1 with files containing their actual data.
    class KFBX_DLL KFbxEventMapAssetFileToAssetObject : public KFbxEvent<KFbxEventMapAssetFileToAssetObject>
    {
        KFBXEVENT_DECLARE(KFbxEventMapAssetFileToAssetObject)
        public:

            // pFile - The asset file to query
            KFbxEventMapAssetFileToAssetObject(const char* pFile) :
                mAsset(NULL),
                mFilePath( pFile )
                {}

            // listeners query this
            inline const char* GetFilePath() const { return mFilePath; }

            // listeners set this
            mutable KFbxObject* mAsset;

        private:
            KString mFilePath;
    };

    #endif //DOXYGEN_SHOULD_SKIP_THIS

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXLIBRARY_H

