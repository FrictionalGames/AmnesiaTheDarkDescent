/*!  \file kfbxcollection.h
 */

#ifndef FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXCOLLECTION_H
#define FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXCOLLECTION_H

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

// FBX includes
#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxplugins/kfbxsdkmanager.h>

#include <fbxfilesdk/kfbxevents/kfbxevents.h>

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxCriteria;

/** A KFbxObject derived container for KFbxObjects.
  * \nosubgrouping
  *
  */
class KFBX_DLL KFbxCollection : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxCollection, KFbxObject);

public:
	/**
	  * \name Collection member management
	  */
	//@{
        //! Deletes all objects in the container.
        virtual void    Clear();

        /** Adds a member.
		  * \param pMember          Object to be added.
		  */
        virtual bool    AddMember(KFbxObject *pMember)      { return ConnectSrcObject(pMember); }

        /** Removes a member.
		  * \param pMember          Object to be removed.
		  */
        virtual bool    RemoveMember(KFbxObject *pMember)   { return DisconnectSrcObject(pMember); }

        /** Searches for a member of class T.
		  * \param pfbxType         Member type.
		  * \param pName            Member name.
		  */
        template <class T> inline T *       FindMember(T const *pfbxType, const char *pName) { return FindSrcObject(pfbxType, pName); }

        /** Returns the number of objects contained within the collection.
		  * \return                 The number of objects the collection contains.
		  */
        inline int                          GetMemberCount () const { return GetSrcObjectCount(); }

        /** Returns the number of class T objects contained within the collection.
		  * \param pFBX_TYPE        Member type.
		  * \return                 The number of objects of class T the collection contains.
		  */
        template < class T > inline int     GetMemberCount (T const *pFBX_TYPE) const { return GetSrcObjectCount(T::ClassId); }

        /** Returns the number of objects in the collection that satisfy the given criteria.
		  * \param pCriteria        The given criteria
		  * \return                 The number of objects in the collection that satisfy the given criteria.
		  */
        int                                 GetMemberCount( KFbxCriteria pCriteria ) const;

        /** Returns the member of the collection at the given index.
		  * \param pIndex           The given index.
		  * \return                 The member of the collection at the given index.
		  */
        inline KFbxObject*                  GetMember (int pIndex=0) const                  { return GetSrcObject(pIndex); }

        /** Returns the member of class T at the given index in the collection.
		  * \param pFBX_TYPE        Member type.
		  * \param pIndex           The given index.
		  * \return                 The member of class T at the given index.
		  */
        template < class T > inline T*      GetMember (T const *pFBX_TYPE, int pIndex=0) const  { return (T *)GetSrcObject(T::ClassId,pIndex); }
        
		/** Returns the member that satisfies the given criteria at the given index in the collection.
		  * \param pCriteria        The given criteria.
		  * \param pIndex           The given index.
		  * \return                 The member that satisfies the given criteria at the given index.
		  */
        KFbxObject*                         GetMember (KFbxCriteria pCriteria, int pIndex=0) const;

        /** Judges whether an object is a part of the collection.
		  * \param pMember          The member to be judged.
		  * \return                 \c True if it is a member of the collection, returns \c false if it is not a member.
          */
        virtual bool    IsMember(const KFbxObject *pMember) const;
    //@}

	/**
	  * \name Selection management
	  */
	//@{
        /** Selects/Deselects all the contained objects.
		  * \param pSelection       If \c true, all objects are selected, if \c false, all objects are deselected.
		  */
        virtual void    SetSelectedAll(bool pSelection);

        /** Selects/Deselects an object.
		  * \param pObject          The object to be selected or deselected.
		  * \param pSelection       If \c true, pObject is selected, if \c false, pObject is deselected.
		  */
        virtual void    SetSelected(KFbxObject* pObject,bool pSelection);

        /** Returns whether the specified object is selected or not.
		  * \param pObject          The specified object.
          * \return                 Whether pObject is selected or not. 
		  */
        virtual bool    GetSelected(KFbxObject* pObject);
	//@}

///////////////////////////////////////////////////////////////////////////////
//  WARNING!
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
	KFbxCollection(KFbxSdkManager& pManager, char const* pName);
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXCOLLECTION_H

