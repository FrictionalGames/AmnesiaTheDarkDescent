/*!  \file kfbxdocument.h
 */

#ifndef FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXDOCUMENT_H
#define FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXDOCUMENT_H

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

#include <fbxfilesdk/fbxcore/fbxcollection/kfbxcollection.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

    // Forward declaration
    class KFbxTakeInfo;
    class KFbxPeripheral;
    class KFbxDocumentInfo;

    /** KFbxDocument is a base class for KFbxScene and KFbxLibrary classes.
	  * A document is a collection (KFbxCollection) of objects (KFbxObject), called the root member objects. 
	  * This is because these objects each form the root of an object graph. The manager (KFbxSdkManager) has access to all
	  * documents, scenes and libraries.
	  *
	  * A document can be contained in another document, thus, a hierarchy of documents
	  * can be built. The root of all documents is simply called the root document.
	  *
      * A document manages animation stacks (KFbxAnimStack). It also provides access to animation stack information (KFbxTakeInfo).
	  *
	  * A document carries information in its KFbxDocumentInfo.
	  * 
	  * Documents manage peripherals to load and unload objects (see class KFbxPeripheral),
	  * as well as references to other objects or documents.
	  *
	  * Error management is also available.
	  * 
      * \nosubgrouping
      */
    class KFBX_DLL KFbxDocument : public KFbxCollection
    {
        KFBXOBJECT_DECLARE(KFbxDocument,KFbxCollection);

	public:
        /**
          * \name Properties
          */
        //@{
            KFbxTypedProperty<fbxReference*>                    Roots;
        //@}

        /**
        * \name Document Member Manager
        */
        //@{
            //! Remove document members and restore default settings.
            virtual void    Clear();
            /** Add a member object and connect it to Roots.
			  * \param pMember Object to add to the document.
			  */
            inline  void    AddRootMember   (KFbxObject *pMember)   { AddMember(pMember); Roots.ConnectSrcObject(pMember); }
            /** Remove a member object from the document.
			  * \param pMember Object to remove from the document.
			  */
            inline  void    RootRootRemoveMember(KFbxObject *pMember)   { RemoveMember(pMember); Roots.DisconnectSrcObject(pMember); }
            /** Find a member object in the document, that has the given type and name.
			  * \param pfbxType Type information.
			  * \param pName Member name.  
			  */
            template <class T> inline T *       FindRootMember(T const *pfbxType, char *pName) { return Roots.FindSrcObject(pfbxType, pName); }

            //! Return the number of objects in the document.
            inline int                          GetRootMemberCount () const { return Roots.GetSrcObjectCount(); }
            /** Return the number of objects of class T in the document.
              * \param pFBX_TYPE Type information.
			  * \return The number of objects of class T in the document.
			  */
            template < class T > inline int     GetRootMemberCount (T const *pFBX_TYPE) const { return Roots.GetSrcObjectCount(T::ClassId); }
			/** Return the number of objects of the document that satisfy the given criteria.
			  * \param pCriteria Criteria for selecting objects.
			  * \return The number of objects satisfying the given criteria.
			  */
            int                                 GetRootMemberCount( KFbxCriteria pCriteria ) const;

            /** Return the member of the document at given index.
			  * \param pIndex Selection index.
			  */
            inline KFbxObject*                  GetRootMember (int pIndex=0) const                  { return Roots.GetSrcObject(pIndex); }
            /** Return the member of class T of the document at given index.
			  * \param pFBX_TYPE Type information.
			  * \param pIndex Selection index.
			  */
            template < class T > inline T*      GetRootMember (T const *pFBX_TYPE, int pIndex=0) const  { return (T *)Roots.GetSrcObject(T::ClassId,pIndex); }
			/** Return the document member which satisfies given criteria, for given index.
              * \param pCriteria Criteria for selecting objects.
			  * \param pIndex Selection index.
			  */
            KFbxObject*                         GetRootMember (KFbxCriteria pCriteria, int pIndex=0) const;
 
			/** Is an object part of the document.
			  * \param pMember Queried object.
			  * \return \c true if pMember is an object part of the document, \c false otherwise.
			  */
            virtual bool    IsRootMember(KFbxObject *pMember) const;
        //@}


        /**
          * \name Document information
          */
        //@{
            /** Get the document information.
              * \return Pointer to the document information object.
              */
            KFbxDocumentInfo* GetDocumentInfo() const;

            /** Set the document information.
              * \param pSceneInfo Pointer to the document information object.
              */
            void SetDocumentInfo(KFbxDocumentInfo* pSceneInfo);
        //@}

        /**
          * \name Offloading management
          *
		  * Documents manage peripherals to load and unload objects (see 
		  * class KFbxPeripheral). A peripheral manipulates the content 
		  * of an object. For instance, a peripheral can load the connections 
		  * of an object on demand.
		  *
          * The document does not own the peripheral therefore
          * it will not attempt to delete it at destruction time. Cloning
          * the document will share the pointer to the peripheral across
          * the cloned objects. The assignment operator has a similar behavior.
          */
        //@{
        public:
            /** Set the current peripheral to be used to load or unload objects from this document.
			  * \param pPeripheral The peripheral to be set.
              */
            void SetPeripheral(KFbxPeripheral* pPeripheral);

            /** Retrieve the current peripheral of the document.
            * \return Current peripheral.
            */
            virtual KFbxPeripheral* GetPeripheral();

            /** Unload all the unloadable objects contained in the document using the
              * currently set peripheral.
			  * Errors that occurred during the operation can be inspected using the
              * GetError() method.
              * \return The number of objects that the document has been able to unload.
              */
            int UnloadContent();

            /** Load all the objects contained in the document with the data from the
              * currently set peripheral.
			  * Errors that occurred during the operation can be inspected using the
              * GetError() method.
              * \return The number of loaded objects.
              */
            int LoadContent();

        //@}

        /**
          * \name Referencing management
          */
        //@{

            /**
              * Fills an array of pointers to documents that reference objects in this document.
              *
              * \param pReferencingDocuments Array of pointers to documents.
              * \returns Number of documents that reference objects in this document.
              */
            int GetReferencingDocuments(KArrayTemplate<KFbxDocument*> & pReferencingDocuments) const;

            /**
              * Fills an array of pointers to objects in a given document (pFromDoc)
              * that reference objects in this document.
              *
              * \param pFromDoc Pointer to the document containing referencing objects.
              * \param pReferencingObjects Array of pointers to referencing objects.
              * \returns Number of objects that reference objects in this document.
              */
            int GetReferencingObjects(KFbxDocument const * pFromDoc, KArrayTemplate<KFbxObject*> & pReferencingObjects) const;

            /**
              * Fills an array of pointers to documents that are referenced by objects in this document.
              *
              * \param pReferencedDocuments Array of pointers to documents.
              * \returns Number of documents that are referenced by objects in this document.
              */
            int GetReferencedDocuments(KArrayTemplate<KFbxDocument*> & pReferencedDocuments) const;

            /**
              * Fills an array of pointers to objects in a given document (pToDoc)
              * that are referenced by objects in this document.
              *
              * \param pToDoc Pointer to the document containing referenced objects.
              * \param pReferencedObjects Array of pointers to referenced objects.
              * \returns Number of objects that are referenced by objects in this document.
              */
            int GetReferencedObjects(KFbxDocument const * pToDoc, KArrayTemplate<KFbxObject*> & pReferencedObjects) const;

            /**
              * Gets the path string to the root document, if the current document is contained in another document.
			  \returns Path to the root document.
              */
            KString GetPathToRootDocument(void) const;
            /**
              * Gets the document path to the root document as an array of documents, if the current document is contained in another document.
			  * \param pDocumentPath Array of KFbxDocument to store the document path.
			  * \param pFirstCall Recursive flag: always use pFirstCall = \c true.
              */
            void GetDocumentPathToRootDocument(KArrayTemplate<KFbxDocument*> & pDocumentPath, bool pFirstCall = true) const;

            /**
              * Tells if this document is a root document.
			  \return \c false if the current document is contained in another document, \c true otherwise.
              */
            bool IsARootDocument(void) { return (NULL == GetDocument()); }
        //@}

        /**
          * \name Animation Stack Management
		  * \remarks Animation stacks replaces the deprecated takes.
          */
        //@{
            /** Holds the name of the KFbxAnimStack that the application uses for animation in this document.
              */
            KFbxTypedProperty<fbxString>    ActiveAnimStackName;

            /** Adds a new animation stack object to this document.
              * In case of error, KFbxDocument::GetLastErrorID() will return
              * \c eTAKE_ERROR.
              * \param pName Animation stack name.
              * \return \c true if a new KFbxAnimStack has been successfully created,
              * \c false if an error occurred or if the specified name defines
              * a KFbxAnimStack that already exists in the document.
              */
            bool CreateAnimStack(const char* pName);

            /** Destroy the animation stack object identified by pName from this document.
              * \param pName Name of the animation stack to be deleted.
              * \return \c true if the KFbxAnimStack has been destroyed and \c false otherwise.
              */
            bool RemoveAnimStack(const char* pName);

            /** Fill a string array with all existing animation stack names.
			  * The array of string is cleared before it is used
              * \param pNameArray An array of string objects.
              */
            void FillAnimStackNameArray(KArrayTemplate<KString*>& pNameArray);

        //@}

        /**
          * \name Animation Stack Information Management
		  * \remark Although takes are deprecated, class KFbxTakeInfo is not deprecated and
		  * now contains animation stack information.
          */
        //@{

            /** Set information about an animation stack.
              * \param pTakeInfo Animation stack information. Field KFbxTakeInfo::mName specifies
              * the targeted animation stack.
              * \return \c true if animation stack is found with this name, and if information is set.
              */
            bool SetTakeInfo(const KFbxTakeInfo& pTakeInfo);

            /** Get information about an animation stack.
              * \param pTakeName Name of the targeted animation stack.
              * \return Animation stack information, or \c NULL if animation stack isn't found or
              * has no information set for this document.
              */
            KFbxTakeInfo* GetTakeInfo(const KString& pTakeName) const;

        //@}

        /**
          * \name Error Management
          * The same error object is shared among instances of this class.
          */
        //@{

            /** Retrieve error object for error inspection.
              * \return Reference to error object.
              */
            KError& GetError();

            /** Error identifiers.
              * Most of these are only used internally.
              */
            typedef enum
            {
				//! Could not create animation stack
                eTAKE_ERROR,
				 //! Null object
                eKFBX_OBJECT_IS_NULL,
				//! Object is already owned by the document
                eKFBX_OBJECT_ALREADY_OWNED,
				//! Object does not belong to the document
                eKFBX_OBJECT_UNKNOWN,
				//! Could not find offload peripheral associated to this document
                eKFBX_MISSING_PERIPHERAL,
				//! Peripheral failed to offload one or more objects
                eKFBX_OBJECT_PERIPHERAL_FAILURE,
				//! Number of errors
                eERROR_COUNT
            } EError;

            /** Get last error code.
              * \return Last error code.
              */
            EError GetLastErrorID() const;

            /** Get last error string.
              * \return Textual description of the last error.
              */
            const char* GetLastErrorString() const;

        //@}

        ///////////////////////////////////////////////////////////////////////////////
        //  WARNING!
        //  Anything beyond these lines may not be Documented accurately and is
        //  subject to change without notice.
        ///////////////////////////////////////////////////////////////////////////////
        #ifndef DOXYGEN_SHOULD_SKIP_THIS

				virtual KFbxObject& Copy(const KFbxObject& pObject);

            // Constructor / Destructor
            protected:

                KFbxDocument(KFbxSdkManager& pManager, char const* pName);
                virtual void Construct  (const KFbxDocument* pFrom);
                virtual void Destruct   (bool pRecursive, bool pDependents);
                bool    ConstructProperties(bool pForceSet);

            // Notification and connection management
            protected:
                virtual bool    ConnecNotify (KFbxConnectEvent const &pEvent);
                virtual void    SetDocument(KFbxDocument* pDocument);

            // Helper functions
            public:
                void ConnectVideos();

            // Take management
            protected:
                bool FindTakeName(const KString& pTakeName);
              
            //
            protected:
                KFbxSdkManager*                     mSdkManager;
                KFbxPeripheral*                     mPeripheral;
                KArrayTemplate<KFbxTakeInfo *>      mTakeInfoArray;
                KError                              mError;
                KFbxDocumentInfo*                   mDocumentInfo;



            friend class KFbxLayerContainer;
            friend class KFbxNodeFinderDuplicateName;
        #endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
    };

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXDOCUMENT_H

