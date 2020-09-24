/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxclonemanager.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXCLONEMANAGER_H
#define FBXFILESDK_KFBXPLUGINS_KFBXCLONEMANAGER_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/fbxcore/kfbxquery.h>
#include <fbxfilesdk/fbxcore/fbxcollection/kfbxpropertymap.h>
#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** The clone manager is a utility for cloning entire networks of KFbxObjects.
  * Options are available for specifying how the clones inherit the connections
  * of the original.
  *
  * Networks of KFbxObjects (inter-connected objects by OO, OP, PO or PP connections)
  * can be cloned. How the connections of clones are handled depends on mSrcPolicy and mExternalDstPolicy.
  *
  * To clone KFbxObject instances and their dependents, put them into a CloneSet
  * and pass the CloneSet to this class:
  * \code
  * KFbxCloneManager                  cloneManager;
  * KFbxCloneManager::CloneSet        cloneSet;
  * KFbxCloneManager::CloneSetElement defaultCloneOptions(KFbxCloneManager::sConnectToClone,
  *                                       KFbxCloneManager::sConnectToOriginal, KFbxObject::eDEEP_CLONE);
  * cloneSet.Insert(someObject, defaultCloneOptions);
  * cloneManager.AddDependents(cloneSet, someObject, defaultCloneOptions);
  * cloneManager.Clone(cloneSet, scene)
  * \endcode
  *
  * \see KFbxCloneManager::CloneSetElement
  * \see KFbxCloneManager::CloneSet
  * \nosubgrouping
  */
class KFBX_DLL KFbxCloneManager
{
public:

    //! Maximum depth to clone dependents.
    static const int sMaximumCloneDepth;

    /** Connect to objects that are connected to original object.
      * This is a flag to mSrcPolicy or mExternalDstPolicy.
      */
    static const int sConnectToOriginal;

    /** Connect to clones of objects that are connected to original object.
      * (only if those original objects are also in the clone set)
      * This is a flag to mSrcPolicy.
      */
    static const int sConnectToClone;

    /** This represents an element in KFbxCloneManager::CloneSet to be cloned.
      * This class contains the option for specifying how connections are cloned and the
      * cloned object.
      * \see KFbxCloneManager
      * \see KFbxCloneManager::CloneSet
      */
    struct KFBX_DLL CloneSetElement
    {
    public:
        /** Constructor.
          * \param pSrcPolicy Specify how to handle source connections. Valid values are 0, sConnectToOriginal,
          *                   sConnectToClone or sConnectToOriginal|sConnectToClone.
          * \param pExternalDstPolicy Specify how to handle destination connections to objects NOT in
          *                           the clone set. Valid values are 0 or sConnectToOriginal.
          * \param pCloneType Specify the type of cloning. KFbxObject::Clone uses the same parameter.
          */
        CloneSetElement( int pSrcPolicy = 0,
                         int pExternalDstPolicy = 0,
                         KFbxObject::ECloneType pCloneType = KFbxObject::eREFERENCE_CLONE );

        //! the type of cloning to perform
        KFbxObject::ECloneType mType;

        /** Policy on how to handle source connections on the original object. Valid values are 0
          * or any bitwise OR'd combination of sConnectToOriginal, and sConnectToClone.
          */
        int mSrcPolicy;

        /** policy on how to handle destination connections on the original object to
          * objects NOT in the clone set. (Destination connections to objects in the set
          * are handled by that object's source policy) Valid values are 0 or sConnectToOriginal.
          */
        int mExternalDstPolicy;

        /** This is a pointer to the newly created clone.
          * It is set after the call to KFbxCloneManager::Clone()
          */
        KFbxObject* mObjectClone;
    };

    /** \brief Functor to compare object pointers. This class is used internally in KFbxCloneManager.
      */
    class KFBX_DLL KFbxObjectCompare
    {
    public:
        /** Operator converting instance to int type.
          * \param pKeyA Left key to be compared.
          * \param pKeyB Right key to be compared.
          */
        inline int operator()(KFbxObject* const& pKeyA, KFbxObject* const& pKeyB) const
        {
            return (pKeyA < pKeyB) ? -1 : ((pKeyB < pKeyA) ? 1 : 0);
        }
    };

    /** The CloneSet is a collection of pointers to objects that will be cloned in Clone()
      * Attached to each object is a CloneSetElement. Its member variables dictate how
      * the corresponding object will be cloned, and how it will inherit connections
      * on the original object.
      */
    typedef KMap<KFbxObject*,CloneSetElement,KFbxObjectCompare> CloneSet;

    /** Constructor
      */
    KFbxCloneManager();

    /** Destructor
      */
    virtual ~KFbxCloneManager();

    /** Clone all objects in the set using the given policies for duplication
      * of connections. Each CloneSetElement in the set will have its mObjectClone
      * pointer set to the newly created clone.
      * \param pSet Set of objects to clone
      * \param pContainer This object (typically a scene or document) will contain the new clones
      * \return true if all objects were cloned, false otherwise.
      */
    virtual bool Clone( CloneSet& pSet, KFbxObject* pContainer = NULL ) const;

    /** Add all dependents of the given object to the CloneSet.
      * Dependents of items already in the set are ignored to prevent
      * infinite recursion on cyclic dependencies.
      * \param pSet The set to add items.
      * \param pObject Object to add dependents to
	  * \param pCloneOptions  
      * \param pTypes Types of dependent objects to consider
      * \param pDepth Maximum recursive depth. Valid range is [0,sMaximumCloneDepth]
        */
    virtual void AddDependents( CloneSet& pSet,
                        const KFbxObject* pObject,
                        const CloneSetElement& pCloneOptions = CloneSetElement(),
                        KFbxCriteria pTypes = KFbxCriteria::ObjectType(KFbxObject::ClassId),
                        int pDepth = sMaximumCloneDepth ) const;


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
        bool CloneConnections( CloneSet::RecordType* pIterator, const CloneSet& pSet ) const;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXCLONEMANAGER_H

