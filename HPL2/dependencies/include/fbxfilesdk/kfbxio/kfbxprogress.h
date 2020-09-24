/*!  \file kfbxprogress.h
 */

#ifndef FBXFILESDK_KFBXIO_KFBXPROGRESS_H
#define FBXFILESDK_KFBXIO_KFBXPROGRESS_H

/**************************************************************************************

 Copyright (C) 2009 Autodesk, Inc. and/or its licensors.
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
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>

// FBX namespace
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

typedef bool (*KFbxProgressCallback)(float pPercentage, KString pStatus);
#define KFBX_PROGRESS_UPDATE_THRESHOLD 0.5f

class KMpFastLock;

/** Class for progress reporting
* \nosubgrouping
*/
class KFBX_DLL KFbxProgress
{
public:
    KFbxProgress();
    ~KFbxProgress();
    
    /** Set the total amount of workload.
    * \param pTotal         Total amount of workload
    */
    void SetTotal(float pTotal);

    /** Report recent finished workload.
    * \param pDetaWorkload  Amount of workload
    * \param pStatus        Current progress status
    */
    void Add(float pDetaWorkload, const KString & pStatus);

    //! Reset the progress status.
    void Reset();

    /** Set the progress status to completed.
    * \param pStatus        Current progress status
    */
    void Complete(const KString & pStatus);

    /** Retrieve the progress status.
    * \param pStatus        Current progress status
    * \return               Percentage
    */
    float GetProgress(KString & pStatus) const;

    /** Register a callback function for progress reporting in single thread mode.
    * \param pCallback       Pointer of the callback function
    */
    void SetProgressCallback(KFbxProgressCallback pCallback);

    /** Set whether this progress was canceled by users.
    * \param pIsCanceled       Set to \c true if canceled, \c false otherwise.
    */
    void Cancel(bool pIsCanceled)
    {
        mCanceled = pIsCanceled;
    }

    //! Query whether user canceled this progress.
    bool IsCanceled() const
    {
        return mCanceled;
    }

#ifndef DOXYGEN_SHOULD_SKIP_THIS

private:
    float GetPercentage() const;
    KMpFastLock *mLock;
    float mTotal;
    float mCurrent;
    KString mStatus;
    float mUpdateThreshold;
    float mLastCallbackProgress;
    KFbxProgressCallback mCallback;
    bool mCanceled;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXIO_KFBXPROGRESS_H
