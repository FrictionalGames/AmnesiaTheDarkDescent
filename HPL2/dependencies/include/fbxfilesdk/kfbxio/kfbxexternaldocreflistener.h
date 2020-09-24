/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/


/*!  \file kfbxexternaldocreflistener.h
 */

#ifndef FBXFILESDK_KFBXIO_KFBXEXTERNALDOCREFLISTENER_H
#define FBXFILESDK_KFBXIO_KFBXEXTERNALDOCREFLISTENER_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxevents/kfbxevents.h>
#include <fbxfilesdk/kfbxevents/kfbxlistener.h>
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxObject;
class KFbxDocument;
class KFbxSdkManager;

/** Contains data about an external document.
  * The document is a KFbxDocument object.
  */
struct KFBX_DLL KFbxExternalDocumentInfo
{
    KString mDocumentName;      //!< Bare name of external document in document hierarchy.
    KString mClassName;         //!< Class name of the document (KFbxDocument, KFbxLibrary...).
    KString mParentFullName;    //!< Full name of the parent document in document hierarchy.
    KString mFilePathUrl;       //!< File path of the external document.
};

/** Event that is emitted on loading document when a referenced document
  * is encountered while loading external references.
  */
class KFBX_DLL KFbxEventReferencedDocument : public KFbxEvent<KFbxEventReferencedDocument>, public KFbxExternalDocumentInfo
{
    KFBXEVENT_DECLARE(KFbxEventReferencedDocument);
public:
    KFbxEventReferencedDocument() {}
};


class KFBX_DLL KFbxExternalDocRefListenerData;

/** Typical handler for the referenced document events.
* 
* Register it like so:
* KFbxExternalDocRefListener lRefDocListener( sdkManager, fileName );
* kfbxevents::KFbxEventHandler * lHandler = lRefDocListener.Bind(scene,
*                                                &KFbxExternalDocRefListener::HandleEvent);  
* 
* And later unregister it like so:
*     lRefDocListener.Unbind(lHandler);
*/
class KFBX_DLL KFbxExternalDocRefListener : public kfbxevents::KFbxListener
{
public:
    /** Constructor. 
      * \param pManager
      * \param pDocFilePath
      * \remarks Keep a reference to the SDK and the path of the document
      * to be able to resolve relative paths. 
      */
    KFbxExternalDocRefListener( KFbxSdkManager & pManager, const KString & pDocFilePath );
    virtual ~KFbxExternalDocRefListener();

    /** Set the document file path used to resolve documents. 
      * \param pDocFilePath
      * \remarks Allows re-using the same instance for multiple document loadings.
      */
    virtual void SetDocumentFilePath( const KString & pDocFilePath );

    /** Verify that all documents that were previously loaded in a previous
      * load session are still valid.
      * \return \c true if all documents are still valid, \c false otherwise.
      */
    virtual bool AreAllExternalDocumentsStillValid() const;

    // 
    /** Verify that all documents that were referred to didn't change. 
      * \return \c true if all documents didn't change, \c false otherwise.
      * \remarks This function should be called if at posteriori check is desired.
      */
    virtual bool WereAllExternalDocumentsValid() const;

    /** Unload all documents that were loaded through this event handler. 
      */
    virtual void UnloadExternalDocuments();

    // External document reference event handler.
    //
    // Operation: calls FindDocument() to find the specified external document
    //            and if not found calls LoadDocument() either directly,
    //            if it has not parent, or via ConnectToParentLibrary().
    //            If its parent cannot be found, it's added to the dangling
    //            document list (and it is not loaded until it's parent is found).
    //            After, it tries to resolve dangling documents by calling
    //            TryConnectingDanglingLibraries().
    /** External document reference event handler. 
      * \param pEvent
      * \remarks    Operation: calls FindDocument() to find the specified external document
      *             and if not found calls LoadDocument() either directly,
      *             if it has not parent, or via ConnectToParentLibrary().
      *             If its parent cannot be found, it's added to the dangling
      *             document list (and it is not loaded until it's parent is found).
      *             After, it tries to resolve dangling documents by calling
      *             TryConnectingDanglingLibraries().
      */
    virtual void HandleEvent(const KFbxEventReferencedDocument * pEvent);

protected:
    /**
     * Turn a relative path to an absolute path using the file path of the original document being loaded.
     * If the path is already is absolute, it is returned as is.
     */
    virtual KString MakeFilenameAbsolute(const KString & pFilename) const;
    //! Locate a document by its document path.
    virtual KFbxDocument * FindDocument( const KString & pPathToDoc );
    //! Load a library, potentially under another library.
    virtual KFbxDocument * LoadDocument(KFbxObject * pParent, const KString & pClassName, const KString & pFilename);
    //! Try to connect a library to its parent given its document path.
    virtual bool ConnectToParentLibrary(const KFbxExternalDocumentInfo &);
    //! Try to reconnect dangling libraries that didn't find their parent.
    virtual void TryConnectingDanglingLibraries();

private:
    KFbxExternalDocRefListenerData * mData;
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXIO_KFBXEXTERNALDOCREFLISTENER_H

