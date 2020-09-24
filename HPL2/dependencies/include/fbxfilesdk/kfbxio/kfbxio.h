/*!  \file kfbxio.h
 */

#ifndef FBXFILESDK_KFBXIO_KFBXIO_H
#define FBXFILESDK_KFBXIO_KFBXIO_H

/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
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

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;

#define KFBXIO_END_NODE_STR ("_End")

/** \brief Base class for FBX file importer and exporter.
  * \nosubgrouping
  */
class KFBX_DLL KFbxIO : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxIO, KFbxObject);

public:
    /** Initializes the object.
      * \param pFileName     The name of the file to access.
      * \return              \c True if successful, returns \c false otherwise.
      * \remarks             To identify the error, call KFbxIO::GetLastErrorID().
      */
    virtual bool Initialize(const char *pFileName);

    /** Returns the file name.
       * \return     The file name or an empty string if no filename has been set.
       */
    virtual KString GetFileName();

    /**
      * \name Error Management
      */
    //@{

    /** Retrieves the error object.
      * \return     A reference to the error object.
      */
    KError& GetError();

    /** \enum EError Error identifiers.
      * - \e eFILE_CORRUPTED
      * - \e eFILE_VERSION_NOT_SUPPORTED_YET
      * - \e eFILE_VERSION_NOT_SUPPORTED_ANYMORE
      * - \e eFILE_NOT_OPENED
      * - \e eFILE_NOT_CREATED
      * - \e eOUT_OF_DISK_SPACE
      * - \e eUNINITIALIZED_FILENAME
      * - \e eUNIDENTIFIED_ERROR
      * - \e eINDEX_OUT_OF_RANGE
      * - \e ePASSWORD_ERROR
      * - \e eEMBEDDED_OUT_OF_SPACE
      */
    typedef enum
    {
        eFILE_CORRUPTED,
        eFILE_VERSION_NOT_SUPPORTED_YET,
        eFILE_VERSION_NOT_SUPPORTED_ANYMORE,
        eFILE_NOT_OPENED,
        eFILE_NOT_CREATED,
        eOUT_OF_DISK_SPACE,
        eUNINITIALIZED_FILENAME,
        eUNIDENTIFIED_ERROR,
        eINDEX_OUT_OF_RANGE,
        ePASSWORD_ERROR,
        eEMBEDDED_OUT_OF_SPACE,
        eERROR_COUNT
    } EError;

    /** Returns the last error code.
      * \return     The last error code.
      */
    EError GetLastErrorID() const;

    /** Returns the last error string.
      * \return     A textual description of the last error.
      */
    const char* GetLastErrorString() const;

    /** Returns the warning message from the file Reader and Writer.
      * \param pMessage     The warning message.
      */
    void GetMessage(KString& pMessage) const;

    //@}

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

    KFbxIO(KFbxSdkManager& pManager,char const* pName);

    int DetectReaderFileFormat(const char *pFileName);
    int DetectWriterFileFormat(const char *pFileName);

    KError mError;
    KString mFilename;
    KFbxSdkManager* mManager;
    KString mMessage;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXIO_KFBXIO_H

