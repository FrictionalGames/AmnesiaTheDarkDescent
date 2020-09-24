/*!  \file kerror.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KERROR_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KERROR_H

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

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

    /** This class provides access to the last error integer ID and is used for translating
      * from integer error ID to strings. Internally a KError instance maintains a translation
      * table from integer IDs to strings. Note that different classes and systems in FBX
      * (e.g. KFbxSdkManager) have different sets of error codes, and maintain different
      * instances of the KError class. The translation table is an array of strings (char*)
      * and the index into the string table is assumed to be the error number.
      * \see KFbxSdkManager::EError
      * \see KFbxSdkManager::GetError()
      * \nosubgrouping
      */
    class KFBX_DLL KError
    {
    public:

        //! Default constructor.
        KError();

        /** Creates a KError from an array of strings. An error code is treated as index into the
        * array of strings when looking up the error code.
        *   \param pStringArray The error string table in use.
        *   \param pErrorCount Number of elements in the error string table.
        */
        KError(const char* pStringArray [], int pErrorCount);

        //! Destructor.
        ~KError();

        /** Reset the object (clears the last error) and use the received error string table.
        *   \param pStringArray The error string table in use.
        *   \param pErrorCount Number of elements in the error string table.
        */
        void Reset(const char* pStringArray [], int pErrorCount);

        /** Get the size of the error string table.
        *   \return Number of elements in the error string table.
        */
        int GetErrorCount() const;

        /** Get the error message string.
        *   \param pIndex Error index.
        *   \return Error string.
        */
        const char* GetErrorString(int pIndex) const;

        /** Set the last error ID and the last error string.
        *   \param pIndex Error index.
        *   \param pString Error string.
        *   \remarks This method will also set the last error string to the default
        *   string value contained in the error string table for this error ID.
        */
        void SetLastError(int pIndex, const char* pString);

        /** Set the last error index.
        *   \param pIndex Error index.
        *   \remarks This method will also set the last error string to the default
        *   string value contained in the error string table for this error index.
        */
        void SetLastErrorID(int pIndex);

        /** Return the last error index.
        *   \return The last error index or -1 if none is set.
        */
        int GetLastErrorID() const;

        /** Get the message string associated with the last error.
        *   \return Error string or empty string if none is set.
        */
        const char* GetLastErrorString() const;

        /** Set the message string associated with the last error.
        *   \param pString Error string.
        *   This method should be called after KError::SetLastErrorID()
        *   in order to customize the error string.
        */
        void SetLastErrorString(const char * pString);

        //! Reset the last error.
        void ClearLastError();

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  WARNING!
    //
    //  Anything beyond these lines may not be documented accurately and is
    //  subject to change without notice.
    //
    ///////////////////////////////////////////////////////////////////////////////

    #ifndef DOXYGEN_SHOULD_SKIP_THIS

    private:

        int mLastErrorID;
        int mErrorCount;

        KString mLastErrorString;
        const char** mStringArray;

    #endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

    };
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KERROR_H

