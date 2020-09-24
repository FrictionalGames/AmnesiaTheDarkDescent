/*!  \file kfbxglobaltimesettings.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXGLOBALTIMESETTINGS_H
#define FBXFILESDK_KFBXPLUGINS_KFBXGLOBALTIMESETTINGS_H

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

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/ktime.h>
#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxGlobalTimeSettingsProperties;

/** This class contains functions for accessing global time settings.
  * \nosubgrouping
  * \remarks This class exists for FBX version 6.x and earlier. The new FBX v7.x file format that is 
  * now the default no longer uses it. The relevant data (a subset of this class) has been moved to
  * the KFbxGlobalSettings object and should be used instead.
  */
class KFBX_DLL KFbxGlobalTimeSettings
{

public:
    KFBXNEW_DECLARE_FRIEND

    //! Restores the default settings.
    void RestoreDefaultSettings();

    /** Sets the time mode.
      * \param pTimeMode            One of the defined modes in KTime class.
      */
    void SetTimeMode(KTime::ETimeMode pTimeMode);

    /** Returns the time mode.
      * \return                     The currently set time mode.
      */
    KTime::ETimeMode GetTimeMode() const;

    /** Sets the time protocol.
      * \param pTimeProtocol        One of the defined protocols in KTime class.
      */
    void SetTimeProtocol(KTime::ETimeProtocol pTimeProtocol);

    /** Returns the time protocol.
      * \return                     The currently set time protocol.
      */
    KTime::ETimeProtocol GetTimeProtocol() const;

    /** \enum ESnapOnFrameMode      Snap on frame mode.
      * - \e eNO_SNAP
      * - \e eSNAP_ON_FRAME
      * - \e ePLAY_ON_FRAME
      * - \e eSNAP_PLAY_ON_FRAME
      */
    typedef enum
    {
        eNO_SNAP,
        eSNAP_ON_FRAME,
        ePLAY_ON_FRAME,
        eSNAP_PLAY_ON_FRAME
    } ESnapOnFrameMode;

    /** Sets the snap on frame mode.
      * \param pSnapOnFrameMode     One of the following values: eNO_SNAP, eSNAP_ON_FRAME, ePLAY_ON_FRAME, or eSNAP_PLAY_ON_FRAME.
      */
    void SetSnapOnFrameMode(ESnapOnFrameMode pSnapOnFrameMode);

    /** Returns the snap on frame mode.
      * \return                     The currently set snap on frame mode.
      */
    ESnapOnFrameMode GetSnapOnFrameMode() const;

    /**
      * \name Time span of time line 
      */
    //@{

    /** Sets the default time span of time line.
      * \param pTimeSpan            The time span of time line.
      */
    void SetTimelineDefautTimeSpan(const KTimeSpan& pTimeSpan);

    /** Returns the default time span of time line.
      * \param pTimeSpan            Holds the default time span of time line.
      */
    void GetTimelineDefautTimeSpan(KTimeSpan& pTimeSpan) const;

    //@}

    /**
      * \name Time Markers
      */
    //@{

    /** This is a struct to define time markers.
      */
    struct KFBX_DLL KFbxTimeMarker
    {
        //! Default constructor.
        KFbxTimeMarker();

        /** Copy constructor.
          * \param pTimeMarker      Another time marker copied to this time marker.
          */
        KFbxTimeMarker(const KFbxTimeMarker& pTimeMarker);

        /** Assignment operator.
          * \param pTimeMarker      Another time marker assigned to this time marker.
          */
        KFbxTimeMarker& operator=(const KFbxTimeMarker& pTimeMarker);

        //! Marker name.
        KString mName; 

        //! Marker time.
        KTime mTime; 

        //! Loop flag.
        bool mLoop; 
    };

    /** Returns the number of time markers.
      * \return                     The number of time markers.
      */
    int GetTimeMarkerCount() const;

    /** Sets the index of the current time marker.
      * \param pIndex               The current time marker index.
      * \return                     \c True if successful, or returns \c false if the index is not valid.
      * \remarks                    If the index is not valid, KFbxGlobalTimeSettings::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
      */
    bool SetCurrentTimeMarker(int pIndex);

    /** Returns the current time marker index.
      * \return                     The current time marker index, or -1 if no current time marker has been set.
      */
    int GetCurrentTimeMarker() const;

    /** Returns the time marker at the given index.
      * \param pIndex               The time marker index.
      * \return                     A pointer to the time marker at the given index, or \c NULL if the index is out of range.
      * \remarks                    If the index is out of range, KFbxGlobalTimeSettings::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
      */
    KFbxTimeMarker* GetTimeMarker(int pIndex) const;

    /** Adds a time marker.
      * \param pTimeMarker          The new time marker to be added.
      */
    void AddTimeMarker(KFbxTimeMarker pTimeMarker);

    //! Removes all time markers and sets the current time marker index to -1.
    void RemoveAllTimeMarkers();

    //@}

    /** Assignment operator.
	  * \param pGlobalTimeSettings  KFbxGlobalTimeSettings object assigned to this one.
	  */
    const KFbxGlobalTimeSettings& operator=(const KFbxGlobalTimeSettings& pGlobalTimeSettings);

    /**
      * \name Error Management
      */
    //@{

    /** Retrieves the error object.
     *  \return                     Reference to the error object.
     */
    KError& GetError();

    /** \enum EError                Error identifiers, most of these are only used internally.
      * - \e eINDEX_OUT_OF_RANGE
      * - \e eERROR_COUNT
      */
    typedef enum
    {
        eINDEX_OUT_OF_RANGE,
        eERROR_COUNT
    } EError;

    /** Returns the last error code.
     *  \return                     The last error code.
     */
    EError GetLastErrorID() const;

    /** Returns the last error string.
     *  \return                     Text description of the last error.
     */
    const char* GetLastErrorString() const;

    //@}

    /**
      * \name Obsolete Functions
      * These functions still work but are no longer relevant.
      */
    //@{

    /** Sets the snap on frame mode flag.
      * \param pSnapOnFrame         If \c true, snap on frame mode is set to eSNAP_ON_FRAME. If \c false, snap on frame mode is set to \c eNO_SNAP.
      * \remarks                    This function is replaced by KFbxGlobalTimeSettings::SetSnapOnFrameMode().
      */
    void SetSnapOnFrame(bool pSnapOnFrame);

    /** Returns the snap on frame mode flag.
      * \return                     \c True if the snap on frame mode is set to either eSNAP_ON_FRAME or ePLAY_ON_FRAME, returns \c false if the snap on frame mode is set to \c eNO_SNAP.
      * \remarks                    This function is replaced by KFbxGlobalTimeSettings::GetSnapOnFrameMode().
      */
    bool GetSnapOnFrame() const;

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

private:

    KFbxGlobalTimeSettings();
    ~KFbxGlobalTimeSettings();

    KFbxGlobalTimeSettingsProperties* mPH;

    friend class KFbxScene;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXGLOBALTIMESETTINGS_H

