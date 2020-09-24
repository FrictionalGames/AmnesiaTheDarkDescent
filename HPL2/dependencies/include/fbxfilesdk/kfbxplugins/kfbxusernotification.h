/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxusernotification.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXUSERNOTIFICATION_H
#define FBXFILESDK_KFBXPLUGINS_KFBXUSERNOTIFICATION_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/kset.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxMessageEmitter;
class KFbxNode;
class KFbxUserNotificationFilteredIterator;
class KFbxSdkManager;
class KFbxLogFile;

/** This class defines one entry object held by the KFbxUserNotification class.
  * \nosubgrouping
  * An entry object is a message to show error, warning or information.
  * Direct manipulation of this object should not be required. At most, access to
  * its members can be granted for querying purposes.
  */
class KFBX_DLL AccumulatorEntry
{
public:
    /** Category of the accumulator entry.   
     */
    enum AEClass {
        eAE_ERROR=1,    //!< Error message entry.
        eAE_WARNING=2,  //!< Warning message entry.
        eAE_INFO=4,     //!< Information message entry.
        eAE_ANY=7       //!< Entry that does not belong to above class. Cannot be used as a class ID
    };

    /** Constructor.
      * \param pAEClass     Specify the category for this entry.
      * \param pName        Identifies this entry (more than one object can have the same name).
      * \param pDescr       The description of the entry. This is the common message. The details
      *                        are added separately by the KFbxUserNotification classes.
      * \param pDetail      A list of detail string that will be copied into the local array.
      * \param pMuteState   Whether this entry is muted.
      * \remarks            By default the object is muted so it does not get processed by the low level
      *                     output routines of the UserNotification accumulator. The entry gets activated 
      *                     (unmuted) by the calls to AddDetail() in the accumulator.                
      */
    AccumulatorEntry(AEClass pAEClass, const KString& pName, const KString& pDescr, 
                     KString pDetail="", bool pMuteState=true);

    /** Copy Constructor.
      * \param pAE Another AccumulatorEntry object to be copied.
      * \param pSkipDetails Flag to skip details.
      */
    AccumulatorEntry(const AccumulatorEntry& pAE, bool pSkipDetails);

    //! Destructor.
    ~AccumulatorEntry();

    //! Returns the category class of this entry.
    AEClass GetClass() const;

    //! Returns the name of this entry.
    KString GetName() const;

    //! Returns the description of this entry.
    KString    GetDescription() const;

    //! Returns the number of details stored.
    int GetDetailsCount() const;

    /** Returns a pointer to one specific detail string (or NULL if the id is invalid).
      * Detail string is dynamic. One entry can have multiple detail strings to hold extra information.
      * For example, if one entry message is related to many FBX nodes, user can add these nodes' name as details.
      * \param id  The detail id.
      * \return Pointer to the specific detail.
      */
    const KString* GetDetail(int id) const;

    //! Returns True if this entry is muted.
    bool IsMuted() const;

private:
    friend class KFbxUserNotification;
    KArrayTemplate<KString*>& GetDetails();
    void Mute(bool pState);

    bool    mMute;
    AEClass mAEClass;
    KString mName;
    KString mDescr;
    KArrayTemplate<KString*> mDetails;
};


/** This class accumulates user notifications and sends them to any device opened by the derived classes.
  * If this class is not derived, the data can only be sent to a log file. To send data to a log file, 
  * it must be opened before attempting to send data, otherwise, the messages will be lost.
  */
class KFBX_DLL KFbxUserNotification
{
public:
    /**
     * Create and initialize user notification object for the SDK manager.
     * One SDK manager has one global user notification object.
     * If the SDK manager already has global user notification object, the function will do nothing.
     *
     * \param pManager
     * \param pLogFileName            Name of the log file that will be open in the directory 
     *                                defined by the GetLogFilePath method.
     * \param pSessionDescription     This string is used to separate session logs in the file.
     * \return    the global user notification object owned by the SDK manager. 
     */
    static KFbxUserNotification* Create(KFbxSdkManager* pManager, 
                                        const KString& pLogFileName, 
                                        const KString& pSessionDescription);

    /**
     * Destroy the global user notification object owned by the SDK manager.
     */
    static void Destroy(KFbxSdkManager* pManager);

    /** Instantiate a KFbxUserNotification but leave it uninitialized. The caller must
      * explicitly call InitAccumulator to initialize it and ClearAccumulator when finished
      * using it.
      * \param pManager
      * \param pLogFileName            Name of the log file that will be open in the directory 
      *                                defined by the GetLogFilePath method.
      * \remarks                       If pLogFileName is an empty string the log file does not get created and any
      *                                output sent to it is lost.
      * \param pSessionDescription     This string is used to separate session logs in the file.
      * \remarks                       If the specified log file already exists, messages are appended to it. This
      *                                class never deletes the log file. Derived classes may delete the log file 
      *                                before opening (it must be done in the constructor because the log file is 
      *                                opened in the InitAccumulator) or at the end of the processing in the 
      *                                PostTerminate method.
      */
    KFbxUserNotification(KFbxSdkManager* pManager,
                         KString const& pLogFileName, 
                         KString const& pSessionDescription);

    //! Destructor.
    virtual ~KFbxUserNotification();

    /** 
      * Accumulator is to hold the notification entries. User can add entries to it.
      * This method must be called before using the Accumulator. It opens the log file and
      * calls AccumulatorInit followed by OpenExtraDevices. Failing to call this method
      * will prevent other actions except ClearAccumulator, GetLogFileName and GetLogFilePath.
      */
    void InitAccumulator();

    /** This method must be called when the Accumulator is no longer needed. It calls 
      * CloseExtraDevices, followed by the AccumulatorClear, and then closes the log file.
      */
    void ClearAccumulator();

    /** IDs for pre-defined message entries.
     */
    enum AEid {
        eBINDPOSE_INVALIDOBJECT = 0x0000,
        eBINDPOSE_INVALIDROOT,
        eBINDPOSE_NOTALLANCESTORS_NODES,
        eBINDPOSE_NOTALLDEFORMING_NODES,
        eBINDPOSE_NOTALLANCESTORS_DEFNODES,
        eBINDPOSE_RELATIVEMATRIX,
        eEMBED_MADIA_NOTIFICATION,
        eFILE_IO_NOTIFICATION,       //!< this is generic for reader and writer to log notifications.
        eFILE_IO_NOTIFICATION_MATERIAL,
        eFILE_IO_NOTIFICATION_DXF_NOT_SUPPORT_NURBS,
        eAE_START_ID                 //!< Starting ID for any Accumulator entry added by derived classes.
    };

    /**
      * \name Accumulator Management
      */
    //@{
    /** Adds one entry into the accumulator.
      * \param pID          This entry unique ID.
      * \param pName        This entry name.
      * \param pDescr       The description of this entry.
      * \param pClass       The category of this entry.
      * \return             The ID of the newly allocated entry. This ID is pEntryId.
      */
    int AddEntry(const int pID, const KString& pName, const KString& pDescr, AccumulatorEntry::AEClass pClass=AccumulatorEntry::eAE_WARNING);

    /** Completes the accumulator entry (there can be more that one detail for each entry) and implicitly defines
      * the sequence of events. Each call to this method is internally recorded, making it possible to output each
      * notification in the order they have been defined. Also, when a detail is added to an entry, it is automatically unmuted
      * so it can be sent to the devices (muted AccumulatorEntry objects are not processed).
      * \param pEntryId     The entry index (as returned by AddEntry).
      * \return             The id of the detail in the recorded sequence of events. This Id should be used when the call to
      *                     Output has the eSEQUENCED_DETAILS set as a source. If an error occurs, the returned value is -1
      */
    int AddDetail(int pEntryId);

    /** Completes the accumulator entry (there can be more that one detail for each entry) and implicitly defines
      * the sequence of events. Each call to this method is internally recorded, making it possible to output each
      * notification in the order they have been defined. Also, when a detail is added to an entry, it is automatically unmuted
      * so it can be sent to the devices (muted AccumulatorEntry objects are not processed).
      * \param pEntryId     The entry index (as returned by AddEntry).
      * \param pString      The detail string to add to the entry.
      * \return             The id of the detail in the recorded sequence of events. This Id should be used when the call to
      *                     Output has the eSEQUENCED_DETAILS set as a source. If an error occurs, the returned value is -1
      */
    int AddDetail(int pEntryId, KString pString);

    /** Completes the accumulator entry (there can be more that one detail for each entry) and implicitly defines
      * the sequence of events. Each call to this method is internally recorded, making it possible to output each
      * notification in the order they have been defined. Also, when a detail is added to an entry, it is automatically unmuted
      * so it can be sent to the devices (muted AccumulatorEntry objects are not processed).
      * \param pEntryId     The entry index (as returned by AddEntry).
      * \param pNode        The node to add to the entry.
      * \return             The id of the detail in the recorded sequence of events. This Id should be used when the call to
      *                     Output has the eSEQUENCED_DETAILS set as a source. If an error occurs, the returned value is -1
      */
    int AddDetail(int pEntryId, KFbxNode* pNode);

    //! Returns the number of AccumulatorEntries currently stored in this accumulator.
    int  GetNbEntries() const;

    /** Get the specified AccumulatorEntry.
      * \param pEntryId     ID of the entry to retrieve.
      * \return             Pointer to the specified entry, otherwise \c NULL if either the id is invalid or the Accumulator
      *                     is not properly initialized.
      */
    const AccumulatorEntry* GetEntry(int pEntryId) const;

    /** Get the AccumulatorEntry at the specified index.
      * \param pEntryIndex     index of the entry to retrieve.
      * \return                Pointer to the specified entry, otherwise \c NULL if either the index is invalid or the Accumulator
      *                        is not properly initialized..
      */
    const AccumulatorEntry* GetEntryAt(int pEntryIndex) const;

    //! Returns the number of Details recorded so far in this accumulator.
    int GetNbDetails() const;

    /** Get the specified detail.
      * \param pDetailId     Index of the detail. This is the id-th detail of type pClass as inserted
      *                      when the AddDetail 
      * \param pAE           Pointer to the AccumulatorEntry object that contains the requested detail.
      *                      The returned valued can be NULL if an error occurred.
      * \return              The index of the detail to be used when calling the GetDetail of the AccumulatorEntry.
      * \remarks             A value of -1 is acceptable and means that the AccumulatorEntry has no details. However,
      *                      if pAE is NULL, the return value is meaningless.
      */
    int GetDetail(int pDetailId, const AccumulatorEntry*& pAE) const;

    //@}

    /**
      * \name Accumulator Output
      */
    //@{
    /** Specify send what kind of data to output device.
     */
    enum OutputSource {
        eACCUMULATOR_ENTRY,     //!< Entry with its details.  
        eSEQUENCED_DETAILS      //!< Details in the recorded order.
    };

    /** Send the accumulator entries to the output devices.
      * This method needs to be explicitly called by the program that uses this
      * class. 
      * \param pOutSrc               Specify which data has to be sent to the output devices. Set to SEQUENCED_DETAILS
      *                              to send the Details in the recorded order. Set to ACCUMULATOR_ENTRY to send
      *                              each entry with its details regardless of the order in which the events occurred.
      * \param pIndex                If this parameter >= 0, only send the specified entry/detail index to the output devices.
      *                              Otherwise send all of them.
      * \param pExtraDevicesOnly     If this parameter is True, the output is not sent to the log file.
      * \remarks                     The pExtraDevicesOnly parameter is ignored if the log file has been disabled.
      */
    bool Output(OutputSource pOutSrc=eACCUMULATOR_ENTRY, int pIndex = -1, bool pExtraDevicesOnly = false);

    /** Send the accumulator entry to the output devices.
      * \param pId                     Send the entry/detail that matching pIdx to the output devices,
      *                              otherwise send all of them.
      * \param pOutSrc               Specify which data has to be sent to the output devices. Set to SEQUENCED_DETAILS
      *                              to send the Details in the recorded order. Set to ACCUMULATOR_ENTRY to send
      *                              each entry with its details regardless of the order in which the events occurred..
      * \param pExtraDevicesOnly     If this parameter is True, the output is not sent to the log file.
      */      
    bool OutputById(AEid pId, OutputSource pOutSrc=eACCUMULATOR_ENTRY, bool pExtraDevicesOnly = false);

    /** Send an immediate entry to the output devices.
      * This method bypasses the accumulator by sending the entry directly to the output devices
      * and discarding it right after. The internal accumulator lists are left unchanged by this call.
      *    \param pName                 This entry name.
      *    \param pDescr                The description of this entry.
      * \param pClass                The category of this entry.
      * \param pExtraDevicesOnly     If this parameter is True, the output is not sent to the log file.
      * \remarks                     The pExtraDevicesOnly parameter is ignored if the log file has been disabled.
      */
    bool Output(const KString& pName, const KString& pDescr, AccumulatorEntry::AEClass pClass, bool pExtraDevicesOnly = false);

    /** Sends the content of the iterator to the output devices.
      * This method bypasses the accumulator by sending each entry in the iterator directly to 
      * the output devices. The internal accumulator lists are left unchanged by this call.
      *    \param pAEFIter              The Filtered AccumulatorEntry iterator object.
      * \param pExtraDevicesOnly     If this parameter is True, the output is not sent to the log file.
      * \remarks                     The pExtraDevicesOnly parameter is ignored if the log file has been disabled.
      */
    bool Output(KFbxUserNotificationFilteredIterator& pAEFIter, bool pExtraDevicesOnly = false);

    /** Set log message emitter.
     *  \param pLogMessageEmitter   The new log message emitter.
     */
    void SetLogMessageEmitter(KFbxMessageEmitter * pLogMessageEmitter);

    /**
      * \name Utilities
      */
    //@{
    /** Returns the absolute path to the log file. If this method is not overridden in a derived class, it
      *  returns the TEMP directory.
      * \param pPath     The returned path.
      */
    virtual void GetLogFilePath(KString& pPath);
    
    /** Returns the log file name.      */    
    inline KString GetLogFileName() { return mLogFileName; }
    //@}

protected:
    /**
     * Identify one detail in all accumulator entries by record the entry object and its detail id.
     */
    class AESequence
    {
    public:
        AESequence(AccumulatorEntry* pAE, int pDetailId) :
            mAE(pAE),
            mDetailId(pDetailId)
            {
            };

        //! Return the entry object the detail belongs to.
        AccumulatorEntry* AE() { return mAE; }
        //! Return the detail id in the entry object
        int DetailId() { return mDetailId; }

    private:
        AccumulatorEntry* mAE;
        int mDetailId;
    };

    friend class KFbxUserNotificationFilteredIterator;

    /** Allow a derived class to finalize processing AFTER the log file handle has been
      * deleted. This may be required if the log file needs to be moved or shown.
      * \returns     True if the object is properly cleaned.
      */
    virtual bool PostTerminate();

    /** Allow the implementation class to perform accumulator initializations before 
      * the Extra devices are opened. By default this method does nothing.
      */
    virtual void AccumulatorInit();

    /** Allow the implementation class to perform accumulator clear after the Extra devices are
      * closed. By default this method does nothing.
      */
    virtual void AccumulatorClear();

    /** Allow the implementation class to opens its output devices (called by InitAccumulator).
      * By default this method does nothing.
      */
    virtual void OpenExtraDevices();

    /** Allow the implementation class to send all the accumulator entries to the devices.
      * By default this method loop trough all the elements of the received array and
      * call the SendToExtraDevices method with the appropriate AccumulatorEntry element and id.
      * \param pOutputNow Flag indicates whether to output now.
      * \param pEntries Accumulator entries to output.
      * \return     \c true if successful, \c false otherwise.
      */
    virtual bool SendToExtraDevices(bool pOutputNow, KArrayTemplate<AccumulatorEntry*>& pEntries);

    /** Allow the implementation class to send all the accumulator entries to the devices.
      * By default this method loop trough all the elements of the received array and
      * call the SendToExtraDevices method with the appropriate AccumulatorEntry element and id.
      * \param pOutputNow Flag indicates whether to output now.
      * \param pAESequence Accumulator entries to output.
      * \return     \c true if successful, \c false otherwise.
      */
    virtual bool SendToExtraDevices(bool pOutputNow, KArrayTemplate<AESequence*>& pAESequence);

    /** Allow the implementation class to send one accumulator entry to the devices.
      * By default this method does nothing. 
      * \param pOutputNow Flag indicates whether to output now.
      * \param pAccEntry Accumulator entry to output.
      * \param pDetailId Detail id. 
      * \return      \c true if successful, \c false otherwise.
      * \remarks     Derived methods should check for the IsMuted() state to decide if the accumulator
      *              entry should get through or get discarded. See AddDetail for more details.
      */
    virtual bool SendToExtraDevices(bool pOutputNow, const AccumulatorEntry* pAccEntry, int pDetailId = -1);

    
    /** Allow the implementation class to close it's output devices (called in the ClearAccumulator)
      * By default this method does nothing.
      */
    virtual void CloseExtraDevices();

    //! Clears the Accumulator list, remove all user notification entries..
    void ResetAccumulator();

    //! Clears the Sequence list.
    void ResetSequence();

    /** Send the pIdth element of the accumulator or sequence list to the log file.
      * \param pOutSrc The output source, accumulator or sequence list.
      * \param pId Element id.
      */
    void SendToLog(OutputSource pOutSrc, int pId);

    /** Send the accumulator entry to the log file.
      * \param pAccEntry The accumulator entry.
      * \param pDetailId Detail id.
      */
    void SendToLog(const AccumulatorEntry* pAccEntry, int pDetailId = -1);

private:
    KString mLogFileName;
    KString* mLog;
    KFbxLogFile* mLogFile;
    KFbxMessageEmitter* mLogMessageEmitter;

    bool mProperlyInitialized;
    KString mSessionDescription;
    bool mProperlyCleaned;

    KSet mAccuHT;                             // The set establish a relationship between an AccumulatorEntry and it's ID
    KArrayTemplate<AccumulatorEntry*> mAccu;  // The array defines the order the AccumulatorEntry objects have been 
                                              // added to the accumulator (calls to AddEntry)
                                              // Both structures share the same pointers.
    KArrayTemplate<AESequence*> mAESequence;
    KFbxSdkManager*             mSdkManager;
};

#if 0
/** \brief This class sends accumulated messages to a file handler specified by a string.
  * If the string argument is "StdOut" or "StdErr" (case insensitive), the standard C stdout/stderr devices are used. 
  * Similarly, "cout" and "cerr" can be used for standard out/error. Otherwise, the string argument is assumed to be 
  * a full filename and is used to open a text file for write. This class does not creates a log file by default.
  */
class KFBX_DLL KFbxUserNotificationFILE : public KFbxUserNotification
{
public:

    KFbxUserNotificationFILE(KString pFileDevice, KString pLogFileName="", KString pSessionDescription="");
    virtual ~KFbxUserNotificationFILE();

    virtual void OpenExtraDevices();
    virtual bool SendToExtraDevices(bool pOutputNow, KArrayTemplate<AccumulatorEntry*>& pEntries);
    virtual bool SendToExtraDevices(bool pOutputNow, KArrayTemplate<AESequence*>& pAESequence);
    virtual bool SendToExtraDevices(bool pOutputNow, const AccumulatorEntry* pAccEntry, int pDetailId = -1);
    virtual void CloseExtraDevices();

private:
    KString mFileDevice;
    FILE* mFP;
    int   mUseStream;
};
#endif
/** This class iterates through the accumulated messages depending on the configuration
  * flags (filter). The iterator keeps a local copy of the data extracted from the
  * accumulator.
  */
class KFBX_DLL KFbxUserNotificationFilteredIterator
{
public:
    /** Constructor. 
      * \param pAccumulator     This reference is only used during construction for retrieving
      *                         the data required to fill the iterator.
      * \param pFilterClass     The bitwise combination of the AEClass identifiers. An AccumulatorEntry
      *                         element is copied from the accumulator if its Class matches one of the
      *                            bits of this flag.
      * \param pSrc                Specify which data format is extracted from the accumulator.
      * \param pNoDetail        This parameter is used ONLY if pSrc == eACCUMULATOR_ENTRY and, if set to
      *                         false, the details of the AccumulatorEntry are also sent to the output
      *                            devices. If left to its default value, only the description of the
      *                            AccumulatorEntry is sent.
      */
    KFbxUserNotificationFilteredIterator(KFbxUserNotification& pAccumulator, 
            int pFilterClass,
            KFbxUserNotification::OutputSource pSrc = KFbxUserNotification::eSEQUENCED_DETAILS,
            bool pNoDetail = true);

    virtual ~KFbxUserNotificationFilteredIterator();

    //! Returns the number of elements contained in this iterator.
    int  GetNbItems() const;

    //! Put the iterator in its reset state.
    void Reset();

    /** Get this iterator's first item. 
      * \return     NULL if the iterator is empty.
      */
    AccumulatorEntry* const First();

    /** Get this iterator's previous item.
      * \return     NULL if the iterator reached the beginning (or is empty).
      * \remarks    This method will also return NULL if it is called before
      *             or immediately after a call to First() and reset the iterator to
      *             its reset state (meaning that a call to First() is mandatory
      *             to be able to iterate again).
      */
    AccumulatorEntry* const Previous();

    /** Get this iterator's next item.
      * \return     NULL if the iterator reached the end (or is empty).
      * \remarks    This method will also return NULL if it is called while 
      *             the iterator is in its reset state (called before
      *             First() or after a preceding call to Previous() reached 
      *             beyond the beginning).
      */
    AccumulatorEntry* const Next();

protected:
    // Called in the constructor.
    virtual void BuildFilteredList(KFbxUserNotification& pAccumulator);

    int                                    mIterator;
    int                                    mFilterClass;
    bool                                mNoDetail;
    KFbxUserNotification::OutputSource    mAccuSrcData;
    KArrayTemplate<AccumulatorEntry*>    mFilteredAE;
};


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXUSERNOTIFICATION_H

