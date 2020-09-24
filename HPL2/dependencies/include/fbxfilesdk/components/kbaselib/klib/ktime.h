/*!  \file ktime.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KTIME_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KTIME_H

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

#ifdef K_QUICKTIME_PLUGIN
	#ifdef KFBX_DLL
	#undef KFBX_DLL
	#endif
	#define KFBX_DLL
#endif

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

// Basic constants.
#define KTIME_INFINITE			KTime (K_LONGLONG( 0x7fffffffffffffff))
#define KTIME_MINUS_INFINITE	KTime (K_LONGLONG(-0x7fffffffffffffff))
#define KTIME_ZERO				KTime (0)
#define KTIME_EPSILON			KTime (1)
#define KTIME_ONE_SECOND		KTime (K_LONGLONG(46186158000))

#define KTIME_ASSERT_EPSILON	0.5

KFBX_FORWARD(KTimeModeObject);

/** Class to encapsulate time units.
* \nosubgrouping
* KTime can measure time in hour, minute, second, frame, field, residual and also combination of these units.
* It is recommended to use KTime for all time related operations. For example, currently it is used in KFbxGlobalSettings,
* KFbxGlobalTimeSettings, KFbxCache, all curve filters and all animation-related classes, etc.
* KTime is just used to represent a moment, to represent a period of time, KTimeSpan should be used.
* \see KTimeSpan
*/
class KFBX_DLL KTime 
{
public:
    /** Constructor.
    * \param pTime Initial value.
    */
    KTime(kLongLong pTime=0) {mTime=pTime;}

    /**
    * \name Time Modes and Protocols
    */
    //@{

    /** Time modes.
    * \remarks
    * Mode \c eNTSC_DROP_FRAME is used for broadcasting operations where 
    * clock time must be (almost) in sync with time code. To bring back color 
    * NTSC time code with clock time, this mode drops 2 frames per minute
    * except for every 10 minutes (00, 10, 20, 30, 40, 50). 108 frames are 
    * dropped per hour. Over 24 hours the error is 2 frames and 1/4 of a 
    * frame.
    * 
    * \par
    * Mode \c eNTSC_FULL_FRAME represents a time address and therefore is NOT 
    * IN SYNC with clock time. A time code of 01:00:00:00 equals a clock time 
    * of 01:00:03:18.
    * 
    * \par
    * Mode \c eFRAMES30_DROP drops 2 frames every minutes except for every 10 
    * minutes (00, 10, 20, 30, 40, 50). This time code represents a time 
    * address and is therefore NOT IN SYNC with clock time. A time code of
    * 01:00:03:18 equals a clock time of 01:00:00:00. It is the close 
    * counterpart of mode \c eNTSC_FULL_FRAME.
    */
    //
    // *** Affected files when adding new enum values ***
    // (ktimeinline.h kfcurveview.cxx, kaudioview.cxx, kvideoview.cxx, kttimespanviewoptical.cxx,
    //  kttimespanview.cxx, ktcameraswitchertimelinecontrol.cxx, fbxsdk(fpproperties.cxx) )
    //
    /** \enum ETimeMode Time modes. 
    * - \e eDEFAULT_MODE	
    * - \e eFRAMES120		  120 frames/s
    * - \e eFRAMES100	      100 frames/s
    * - \e eFRAMES60          60 frames/s
    * - \e eFRAMES50          50 frames/s
    * - \e eFRAMES48          48 frame/s
    * - \e eFRAMES30          30 frames/s	 BLACK & WHITE NTSC
    * - \e eFRAMES30_DROP     30 frames/s use when display in frame is selected(equivalent to NTSC_DROP)
    * - \e eNTSC_DROP_FRAME   29.97002617 frames/s drop COLOR NTSC
    * - \e eNTSC_FULL_FRAME   29.97002617 frames/s COLOR NTSC
    * - \e ePAL               25 frames/s	 PAL/SECAM
    * - \e eCINEMA            24 frames/s
    * - \e eFRAMES1000        1000 milli/s (use for date time)
  	* - \e eCINEMA_ND	      23.976 frames/s
  	* - \e eCUSTOM            Custom Framerate value
	* - \e eTIME_MODE_COUNT   Number of time modes.
    */
    enum ETimeMode
    {
        eDEFAULT_MODE       = 0,
        eFRAMES120          = 1,
        eFRAMES100          = 2,
        eFRAMES60           = 3,
        eFRAMES50           = 4,
        eFRAMES48           = 5,
        eFRAMES30           = 6,
        eFRAMES30_DROP      = 7,
        eNTSC_DROP_FRAME    = 8,
        eNTSC_FULL_FRAME	= 9,
        ePAL                = 10,
        eCINEMA             = 11,
        eFRAMES1000         = 12,
		eCINEMA_ND		= 13,
    	eCUSTOM             = 14,
    	eTIME_MODE_COUNT    = 15
    };

    /** \enum ETimeProtocol Time protocols.
    * - \e eSMPTE                SMPTE Protocol
    * - \e eFRAME                Frame count
    * - \e eDEFAULT_PROTOCOL     Default protocol (initialized to eFRAMES)
	* - \e eTIME_PROTOCOL_COUNT  Number of time protocols.
    */
    enum ETimeProtocol
    {
	    eSMPTE,
	    eFRAME,
	    eDEFAULT_PROTOCOL,
	    eTIME_PROTOCOL_COUNT
    };

    /** Set default time mode.
    * \param pTimeMode  Time mode identifier.
  	* \param pFrameRate Custom framerate, only have effect in case of pTimeMode = KTime::eCUSTOM
    * \remarks It is meaningless to set default time mode to \c eDEFAULT_MODE.
    */
	static void SetGlobalTimeMode(ETimeMode pTimeMode, double pFrameRate=0.0);

    /** Get default time mode.
    * \return Currently set time mode identifier.
    * \remarks Default time mode initial value is eFRAMES30.
    */
    static ETimeMode GetGlobalTimeMode();
	
    /** Set default time protocol.
    * \param pTimeProtocol Time protocol identifier.
    * \remarks It is meaningless to set default time protocol to \c eDEFAULT_PROTOCOL.
    */
    static void SetGlobalTimeProtocol(ETimeProtocol pTimeProtocol);

    /** Get default time protocol.
    * \return Currently set time protocol identifier.
    * \remarks Default time protocol initial value is eSMPTE.
    */
    static ETimeProtocol GetGlobalTimeProtocol();

    /** Get frame rate associated with time mode, in frames per second.
    * \param pTimeMode Time mode identifier.
    * \return Frame rate value.
    */
    static double GetFrameRate(ETimeMode pTimeMode);

    /** Get time mode associated with frame rate.
    * \param pFrameRate The frame rate value.
    * \param lPrecision The tolerance value.
    * \return The corresponding time mode identifier or \c eDEFAULT_MODE if no time 
    * mode associated to the given frame rate is found.
    */
    static ETimeMode ConvertFrameRateToTimeMode(double pFrameRate, double lPrecision = 0.00000001);

    //@}
	
    /**
    * \name Time Conversion
    */
    //@{
	
    /** Set time in internal format.
    * \param pTime Time value to set.
    */
    inline void Set(kLongLong pTime) {mTime = pTime;}

    /** Get time in internal format.
    * \return Time value.
    */
    inline const kLongLong& Get() const {return mTime;}

    /** Set time in milliseconds.
    * \param pMilliSeconds Time value to set.
    */
    inline void SetMilliSeconds(kLongLong pMilliSeconds) {mTime = pMilliSeconds * K_LONGLONG(46186158);}

    /** Get time in milliseconds.
    * \return Time value.
    */
    inline kLongLong GetMilliSeconds() const {return mTime / K_LONGLONG(46186158);}

    /** Set time in seconds.
    * \param pTime Time value to set.
    */
    void SetSecondDouble(double pTime);

    /** Get time in seconds.
    * \return Time value.
    */
    double GetSecondDouble() const;


    /** Set time in hour/minute/second/frame/field format.
    * \param pHour The hours value.
    * \param pMinute    The minutes value.
    * \param pSecond    The seconds value.
    * \param pFrame     The frames values.
    * \param pField     The field value.
    * \param pTimeMode  Time mode identifier.
  	* \param pFramerate Custom framerate in case of pTimeMode = KTime::eCUSTOM
    * \remarks Parameters pHour, pMinute, pSecond, pFrame and pField are summed together.
    * For example, it is possible to set the time to 83 seconds in the following
    * ways: SetTime(0,1,23) or SetTime(0,0,83).
    */
    void SetTime 
    (
	    int pHour, 
	    int pMinute, 
	    int pSecond, 
	    int pFrame = 0, 
	    int pField = 0, 
		int pTimeMode=eDEFAULT_MODE,
    	double pFramerate = 0.0
    );

    /** Set time in hour/minute/second/frame/field/residual format.
    * \param pHour The hours value.
    * \param pMinute       The minutes value.
    * \param pSecond       The seconds value.
    * \param pFrame        The frames values.
    * \param pField        The field value.
    * \param pResidual     The hundredths of frame value.
    * \param pTimeMode     Time mode identifier.
  	* \param pFramerate    Custom framerate in case of pTimeMode = KTime::eCUSTOM
    * \remarks Parameters pHour, pMinute, pSecond, pFrame, pField and pResidual 
    * are summed together, just like above.
    * pResidual represents hundredths of frame, and won't necessarily
    * correspond to an exact internal value.
    *
    * \remarks The time mode can't have a default value, because
    *         otherwise SetTime(int, int, int, int, int, int)
    *         would be ambiguous. Please specify DEFAULT_MODE.
    */
    void SetTime 
    (
	    int pHour, 
	    int pMinute, 
	    int pSecond, 
	    int pFrame, 
	    int pField, 
	    int pResidual, 
		int pTimeMode,
    	double pFramerate = 0.0
    );

    /** Get time in hour/minute/second/frame/field/residual format.
    * \param pHour       The returned hours value.
    * \param pMinute     The returned minutes value.
    * \param pSecond     The returned seconds value.
    * \param pFrame      The returned frames values.
    * \param pField      The returned field value.
    * \param pResidual   The returned hundredths of frame value.
    * \param pTimeMode   The time mode identifier which will dictate the extraction algorithm.
  	* \param pFramerate  Custom framerate in case of pTimeMode = KTime::eCUSTOM
    * \return \c true if the pTimeMode parameter is a valid identifier and thus the extraction
    * succeeded. If the function returns \c false, all the values are set to 0.
    */
    bool GetTime 
    (
	    kLongLong& pHour, 
	    kLongLong& pMinute, 
	    kLongLong& pSecond, 
	    kLongLong& pFrame, 
	    kLongLong& pField, 
	    kLongLong& pResidual, 
		int pTimeMode=eDEFAULT_MODE,
    	double pFramerate = 0.0
    ) const;

	/** Snaps a time value to the time value associated with the nearest frame.
	* \param pRound  If \c true the return value is rounded to the nearest integer.
	* \return        The snapped time value.
	*/
    KTime	GetFramedTime(bool pRound = true);

    /** Get number of hours in time.
    * \param pCummul This parameter has no effect.
    * \param pTimeMode Time mode identifier.
    * \param pFramerate Custom framerate in case of pTimeMode = KTime::eCUSTOM
    * \return Hours value.
    */
	kLongLong GetHour(bool pCummul=false, int pTimeMode=eDEFAULT_MODE, double pFramerate=0.0) const;
	
    /** Get number of minutes in time.
    * \param pCummul If \c true, get total number of minutes. If \c false, get number of 
    * minutes exceeding last full hour.
    * \param pTimeMode Time mode identifier.
    * \param pFramerate Custom framerate in case of pTimeMode = KTime::eCUSTOM
    * \return Minutes value.
    */
	kLongLong GetMinute(bool pCummul=false, int pTimeMode=eDEFAULT_MODE, double pFramerate=0.0) const;
	
    /** Get number of seconds in time.
    * \param pCummul If \c true, get total number of seconds. If \c false, get number of 
    * seconds exceeding last full minute.
    * \param pTimeMode Time mode identifier.
    * \param pFramerate Custom framerate in case of pTimeMode = KTime::eCUSTOM
    * \return Seconds value.
    */
	kLongLong GetSecond(bool pCummul=false, int pTimeMode=eDEFAULT_MODE, double pFramerate=0.0) const;
	
    /** Get number of frames in time.
    * \param pCummul If \c true, get total number of frames. If \c false, get number of 
    * frames exceeding last full second.
    * \param pTimeMode Time mode identifier.
    * \param pFramerate  Custom framerate in case of pTimeMode = KTime::eCUSTOM
    * \return Frames values.
    */
	kLongLong GetFrame(bool pCummul=false, int pTimeMode=eDEFAULT_MODE, double pFramerate=0.0) const;
	
    /** Get number of fields in time.
    * \param pCummul If \c true, get total number of fields. If \c false, get number of 
    * fields exceeding last full frame.
    * \param pTimeMode Time mode identifier.
    * \param pFramerate  Custom framerate in case of pTimeMode = KTime::eCUSTOM
    * \return Fields value.
    */
	kLongLong GetField(bool pCummul=false, int pTimeMode=eDEFAULT_MODE, double pFramerate=0.0) const;

    /** Get residual time exceeding last full field.
    * \param pTimeMode Time mode identifier.
    * \param pFramerate Custom framerate in case of pTimeMode = KTime::eCUSTOM
    * \return Residual value.
    */
	kLongLong GetResidual(int pTimeMode=eDEFAULT_MODE, double pFramerate=0.0) const;

    /** Get time in a human readable format.
    * \param pTimeString An array large enough to contain a minimum of 19 characters.
    * \param pInfo The amount of information if time protocol is \c eSMPTE:
    * <ul><li>1 means hours only
    *     <li>2 means hours and minutes
    *     <li>3 means hours, minutes and seconds
    *     <li>4 means hours, minutes, seconds and frames
    *     <li>5 means hours, minutes, seconds, frames and field
    *     <li>6 means hours, minutes, seconds, frames, field and residual value</ul>
    * \param pTimeMode Requested time mode.
    * \param pTimeFormat Requested time protocol.
    * \param pFramerate Custom framerate in case of pTimeMode = KTime::eCUSTOM
    * \return pTimeString parameter filled with a time value or set to a empty string
    * if parameter pInfo is not valid.
    */
    char* GetTimeString 
    (
	    char* pTimeString, 	
	    int pInfo=5, 
	    int pTimeMode=eDEFAULT_MODE, 
	    int pTimeFormat=eDEFAULT_PROTOCOL,
        double pFramerate = 0.0
    ) const;

    /** Set time in a human readable format.
    * \param pTime An array of a maximum of 18 characters.
    * If time protocol is \c eSMPTE, pTimeString must be formatted this way:
    * "[hours:]minutes[:seconds[.frames[.fields]]]". Hours, minutes, seconds, 
    * frames and fields are parsed as integers and brackets indicate optional 
    * parts. 
    * If time protocol is \c eFRAME, pTimeString must be formatted this way:
    * "frames". Frames is parsed as a 64 bits integer.
    * \param pTimeMode   Given time mode.
    * \param pTimeFormat Given time protocol.
    * \param pFramerate  Custom framerate in case of pTimeMode = KTime::eCUSTOM
    */
	void SetTimeString(char* pTime, int pTimeMode=eDEFAULT_MODE, int pTimeFormat=eDEFAULT_PROTOCOL, double pFramerate = 0.0);

    //@}

    /**
    * \name Time Operators
    */
    //@{
 
    /** Equality operator.
	  * \param pTime The KTime to be compared.
	  * \return \c true if equal, \c false otherwise.
	  */
    inline bool operator==(const KTime& pTime) const {return mTime == pTime.mTime;}

    /** Inequality operator.
	  * \param pTime The KTime to be compared.
	  * \return \c true if unequal, \c false otherwise.
	  */
    inline bool operator!=(const KTime& pTime) const {return mTime != pTime.mTime;}

    /** Superior or equal to operator.
	  * \param pTime The KTime to be compared.
	  * \return \c true if this KTime is superior or equal to the passed KTime, \c false otherwise.
	  */
    inline bool operator>=(const KTime& pTime) const {return mTime >= pTime.mTime;}

    /** Inferior or equal to operator.
	  * \param pTime The KTime to be compared.
	  * \return \c true if this KTime is inferior or equal to the passed KTime, \c false otherwise.
	  */
    inline bool operator<=(const KTime& pTime) const {return mTime <= pTime.mTime;}

    /** Superior to operator.
	  * \param pTime The KTime to be compared.
	  * \return \c true if this KTime is superior to the passed KTime, \c false otherwise.
	  */
    inline bool operator>(const KTime& pTime) const {return mTime > pTime.mTime;}
 
    /** Inferior to operator.
	  * \param pTime The KTime to be compared.
	  * \return \c true if this KTime is inferior to the passed KTime, \c false otherwise.
	  */
    inline bool operator<(const KTime& pTime) const {return mTime < pTime.mTime;} 

    /** Assignment operator.
	  * \param pTime The KTime to be assigned.
	  */
    inline KTime& operator=(const KTime& pTime) {mTime = pTime.mTime; return *this;}
	
    /** Addition operator.
	  * \param  pTime The KTime to be added.
	  * \return This KTime after addition.
	  */
    inline KTime& operator+=(const KTime& pTime) {mTime += pTime.mTime; return *this;}
	
    /** Subtraction operator.
	  * \param pTime The KTime to be subtracted.
	  * \return This KTime after subtraction.
	  */
    inline KTime& operator-=(const KTime& pTime) {mTime -= pTime.mTime; return *this;}
	
    /** Addition operator.
	  * \param pTime The KTime to be added.
	  * \return A temporary KTime after addition. 
	  */
    KTime operator+(const KTime& pTime) const;

    /** Subtraction operator.
	  * \param pTime The KTime to be subtracted.
	  * \return A temporary KTime after subtraction. 
	  */
    KTime operator-(const KTime& pTime) const;

    /** Multiplication operator.
	  * \param Mult Multiply this KTime by int Mult.
	  * \return A temporary KTime after multiplication. 
	  */
    KTime operator*(const int Mult) const;

    /** Division operator.
	  * \param pTime Divide this KTime by pTime.
	  * \return A temporary KTime after division. 
	  */
    KTime operator/(const KTime &pTime) const;

    /** Multiplication operator.
	  * \param pTime Multiply this KTime by pTime.
	  * \return A temporary KTime after multiplication. 
	  */
    KTime operator*(const KTime &pTime) const;

    //! Increment time of one unit of the internal format (prefix form).
    inline KTime &operator++() {mTime += 1; return (*this);}

    //! Increment time of one unit of the internal format (postfix form).
    inline const KTime operator++(int) {KTime lOld = *this; ++(*this); return lOld;}

    //! Decrement time of one unit of the internal format (prefix form).
    inline KTime &operator--() {mTime -= 1; return (*this);}

    //! Decrement time of one unit of the internal format (postfix form).
    inline const KTime operator--(int) {KTime lOld = *this; --(*this); return lOld;}

    //@}
	static KTime GetSystemTimer();
	///////////////////////////////////////////////////////////////////////////////
	//
	//  WARNING!
	//
	//	Anything beyond these lines may not be documented accurately and is 
	// 	subject to change without notice.
	//
	///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    kLongLong mTime; // In 1/46, 186, 158, 000 Seconds

    static ETimeMode gsGlobalTimeMode;
    static ETimeProtocol gsGlobalTimeProtocol;
	static KTimeModeObject* gsTimeObject;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
	
    // Global Friend Function
    friend KFBX_DLL KTime::ETimeMode		KTime_GetGlobalTimeMode();
	friend KFBX_DLL HKTimeModeObject		KTime_GetGlobalTimeModeObject();
    friend KFBX_DLL KTime::ETimeProtocol	KTime_GetGlobalTimeFormat();
	friend KFBX_DLL void					KTime_SetGlobalTimeMode( KTime::ETimeMode pTimeMode, double pFrameRate );
    friend KFBX_DLL void					KTime_SetGlobalTimeFormat( KTime::ETimeProtocol pTimeFormat );

	inline kLongLong GetOneFrameValue ( int pTimeMode, double pFramerate ) const;
};

#define KTS_FORWARD 1
#define KTS_BACKWARD -1

/** Class to encapsulate time intervals.
* \nosubgrouping
* \see KTime 
*/
class KFBX_DLL KTimeSpan
{
public:
    //! Constructor.
    KTimeSpan() {}

    /** Constructor.
    * \param pStart Beginning of the time interval.
    * \param pStop  Ending of the time interval.
    */
    KTimeSpan(KTime pStart, KTime pStop) {mStart=pStart; mStop=pStop;}

    /** Set start and stop time.
    * \param pStart Beginning of the time interval.
    * \param pStop  Ending of the time interval.
    */
    inline void Set(KTime pStart, KTime pStop) {mStart=pStart; mStop=pStop;}

    /** Set start time.
    * \param pStart Beginning of the time interval.
    */
    inline void SetStart(KTime pStart) {mStart=pStart;}
	
    /** Set stop time.
    * \param pStop  Ending of the time interval.
    */
    inline void SetStop(KTime pStop) {mStop=pStop;}
	
    /** Get start time.
    * \return Beginning of time interval.
    */
    inline KTime GetStart() const {return mStart;}
	
    /** Get stop time.
    * \return Ending of time interval.
    */
    inline KTime GetStop() const {return mStop;}
	
    /** Get time interval in absolute value.
    * \return Time interval.
    */
    inline KTime GetDuration() const {if (mStop>mStart)return mStop-mStart; else return mStart-mStop;}
	
    /** Get time interval.
    * \return Signed time interval.
    */
    inline KTime GetSignedDuration() const {return mStop-mStart;}
	
    /** Get direction of the time interval.
    * \return \c KTS_FORWARD if time interval is forward, \c KTS_BACKWARD if backward.
    */
    inline int GetDirection() const {if (mStop>=mStart)return KTS_FORWARD; else return KTS_BACKWARD;}

    /** Return \c true if the time is inside the timespan.
	  * \param pTime Judge whether pTime is inside the timespan.
	  * \return \c True if is, \c false otherwise.
	  */
    bool IsInside(KTime pTime) const;
	
    /** Return the intersection of the two time spans.
	  * \param pTime 
	  * \return The intersection of pTime and this KTimeSpan.
	  */
    KTimeSpan Intersect(const KTimeSpan &pTime) const;

    /** Inequality operator.
	  * \param pTime KTimeSpan compared with this one.
	  * \return \c True if unequal, \c false otherwise.
	  */
    bool operator!=(const KTimeSpan &pTime) const;

    /** Equality operator.
	  * \param pTime KTimeSpan compared with this one.
	  * \return \c True if equal, \c false otherwise.
	  */
    bool operator==(const KTimeSpan &pTime) const;

    /** Unite with another KTimeSpan
     * \param pSpan The KTimeSpan
     * \remarks This function assumes both of the KTimeSpan objects are in the forward direction.
     */
    void UnionAssignment(const KTimeSpan& pSpan);

	
	///////////////////////////////////////////////////////////////////////////////
	//
	//  WARNING!
	//
	//	Anything beyond these lines may not be documented accurately and is 
	// 	subject to change without notice.
	//
	///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    KTime mStart;
    KTime mStop;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

/** \enum EOldTimeMode  Keep compatibility with old fbx format
  * - \e eOLD_DEFAULT_MODE                  Default mode set using KTime::SetGlobalTimeMode (ETimeMode pTimeMode)
  * - \e eOLD_CINEMA                        24 frameOLD_s/s
  * - \e eOLD_PAL                           25 frameOLD_s/s	 PAL/SECAM
  * - \e eOLD_FRAMES30                      30 frameOLD_s/s	 BLACK & WHITE NTSC                 
  * - \e eOLD_NTSC_DROP_FRAME               29.97002617 frameOLD_s/s COLOR NTSC        
  * - \e eOLD_FRAMES50                      50 frameOLD_s/s   
  * - \e eOLD_FRAMES60                      60 frameOLD_s/s             
  * - \e eOLD_FRAMES100                     100 frameOLD_s/s
  * - \e eOLD_FRAMES120                     120 frameOLD_s/s
  * - \e eOLD_NTSC_FULL_FRAME               29.97002617 frameOLD_s/s COLOR NTSC
  * - \e eOLD_FRAMES30_DROP                 30 frameOLD_s/s
  * - \e eOLD_FRAMES1000                    1000 frameOLD_s/s
  * - \e eOLD_TIME_MODE_COUNT
  */
enum EOldTimeMode
{
	eOLD_DEFAULT_MODE,		//!< Default mode set using KTime::SetGlobalTimeMode (ETimeMode pTimeMode)
	eOLD_CINEMA,			//!< 24 frameOLD_s/s
	eOLD_PAL,				//!< 25 frameOLD_s/s	 PAL/SECAM
	eOLD_FRAMES30,			//!< 30 frameOLD_s/s	 BLACK & WHITE NTSC
	eOLD_NTSC_DROP_FRAME,   //!< 29.97002617 frameOLD_s/s COLOR NTSC
	eOLD_FRAMES50,			//!< 50 frameOLD_s/s
	eOLD_FRAMES60,			//!< 60 frameOLD_s/s
	eOLD_FRAMES100,			//!< 100 frameOLD_s/s
	eOLD_FRAMES120,			//!< 120 frameOLD_s/s
	eOLD_NTSC_FULL_FRAME,	//!< 29.97002617 frameOLD_s/s COLOR NTSC
	eOLD_FRAMES30_DROP,		//!< 30 frameOLD_s/s
	eOLD_FRAMES1000,		//!< 1000 frameOLD_s/s
	eOLD_TIME_MODE_COUNT
};

class KFBX_DLL KTimeModeObject
{
public:
    double				mFrameRateValue;
    char*				mFrameRateString;//!< *** use to store/retrieve the framerate in files, do not change existing value
    KTime::ETimeMode    mTimeMode;
    EOldTimeMode        mOldTimeMode;
    char*				mTimeModeString;//!< Use to Display in the UI
    int					mShowValue;     //!< UI in TC view -> bit 0 set to 1 ..... UI in FR view -> bit 1 set to 1
                                        //!< 0 = don't show this mode in TC or FR view, 1 = show in TC view only, 2 = show in FR only, 3 = show in FR and TC
    bool                mSystemRate;    //!< Indicate that this is a built-in value, false indicate it is a custom value
    bool                mExistInOldValue;//!< Indicate if this value exist in the old value enum
    kLongLong           mOneFrameValue;  //!< indicate internal value for 1 frame
};


KFBX_DLL KTime::ETimeMode		KTime_GetGlobalTimeMode();
KFBX_DLL HKTimeModeObject       KTime_GetGlobalTimeModeObject();
KFBX_DLL KTime::ETimeProtocol	KTime_GetGlobalTimeFormat();
KFBX_DLL void					KTime_SetGlobalTimeMode(KTime::ETimeMode pTimeMode, double pFrameRate);
KFBX_DLL void					KTime_SetGlobalTimeFormat(KTime::ETimeProtocol pTimeFormat);
//
// Use those functions to keep the compatibility with old time mode since we added new time mode.
//
KFBX_DLL int					KTime_GetOldTimeModeCorrespondance(KTime::ETimeMode pNewTimeMode);
KFBX_DLL int					KTime_GetTimeModeFromOldValue(int pOldTimeMode);
//
// We now store the framerate instead of the time mode.
//
KFBX_DLL int					KTime_GetTimeModeFromFrameRate(char* pFrameRate);
KFBX_DLL void					KTime_GetControlStringList(char* pControlString, KTime::ETimeProtocol pTimeFormat);
KFBX_DLL char*					KTime_GetGlobalFrameRateString(KTime::ETimeMode pTimeMode);
KFBX_DLL char*					KTime_GetGlobalTimeModeString(KTime::ETimeMode pTimeMode);
KFBX_DLL double					KTime_GetFrameRate(KTime::ETimeMode pTimeMode);
//
// Time format
//
KFBX_DLL int					KTime_SelectionToTimeFormat(int pSelection);
KFBX_DLL int					KTime_SelectionToTimeMode(int pSelection);
KFBX_DLL int					KTime_TimeToSelection(int pTimeMode=KTime::eDEFAULT_MODE, int pTimeFormat=KTime::eDEFAULT_PROTOCOL);
KFBX_DLL char*					KTime_GetTimeModeName(int pTimeMode);
KFBX_DLL int					KTime_GetFrameRateStringListIndex(KTime::ETimeMode pTimeMode);
KFBX_DLL bool					KTime_IsValidCustomFramerate(double pFramerate);
KFBX_DLL bool					KTime_GetNearestCustomFramerate(double pFramerate, double& pNearestRate);

// Compatibility Only
// Ideally, we want to make this go away :)
#define	DEFAULT_MODE		KTime::eDEFAULT_MODE
#define	CINEMA				KTime::eCINEMA
#define	PAL					KTime::ePAL
#define	FRAMES30			KTime::eFRAMES30
#define	NTSC_DROP_FRAME		KTime::eNTSC_DROP_FRAME
#define	FRAMES50			KTime::eFRAMES50	
#define	FRAMES60			KTime::eFRAMES60
#define	FRAMES100			KTime::eFRAMES100
#define	FRAMES120			KTime::eFRAMES120
#define	NTSC_FULL_FRAME		KTime::eNTSC_FULL_FRAME
#define	FRAMES30_DROP		KTime::eFRAMES30_DROP
#define	FRAMES1000			KTime::eFRAMES1000
#define	TIMEFORMAT_SMPTE	KTime::eSMPTE
#define	TIMEFORMAT_FRAME	KTime::eFRAME
#define	DEFAULT_FORMAT		KTime::eDEFAULT_PROTOCOL

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KTIME_H

