/****************************************************************************************

Copyright (C) 2010 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxanimcurvebase.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVEBASE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVEBASE_H

#include <fbxfilesdk/fbxfilesdk_def.h>
#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** This is the base class interface for the FBX animation curve keys.
  * \nosubgrouping
  *
  * \remarks For an example of implemented class, please see KFbxAnimCurveKey.
  */
class KFBX_DLL KFbxAnimCurveKeyBase
{
public:
    /** Data member representing time value.
    */
    KTime mTime;

    /** Constructor.
    */
    KFbxAnimCurveKeyBase()
    {
        mTime = KTIME_ZERO;
    }

    /** Destructor.
    */
    virtual ~KFbxAnimCurveKeyBase() {};

    /** Get time value.
    * \return Time value.
    */
    virtual KTime GetTime() const
	{
		return mTime;
	}

    /** Set time value.
    * \param pTime Time value to set.
    */
    virtual void  SetTime(const KTime& pTime) {
		mTime = pTime;
	}
};

/** This is the base class for implementing animation curves.
  * \nosubgrouping
  * It is a pure virtual class that defines the general interface to animation
  * key management and manipulation.
  *
  * \see KFbxAnimCurve and KFbxAnimCurveKFCurve for fully implemented class.
  */
class KFBX_DLL KFbxAnimCurveBase : public KFbxObject
{
    KFBXOBJECT_DECLARE_ABSTRACT(KFbxAnimCurveBase, KFbxObject);

public:
    /**
      * \name Key management.
      *
      */
    //@{
        //! Remove all the keys and free buffer memory.
        virtual void KeyClear () = 0;
        
        //! Get the number of keys.
        virtual int KeyGetCount () const = 0;

        /** Add a key at given time.
          * \param pTime Time to add the key.
          * \param pKey Key to add.
          * \param pLast Index of the last processed key to speed up search. If this 
          *              function is called in a loop, initialize this value to 0 and let it 
          *              be updated by each call.
          * \return Index of the key at given time, no matter if it was added 
          *         or already present.
          */
        virtual int KeyAdd (KTime pTime, KFbxAnimCurveKeyBase& pKey, int* pLast = NULL) = 0;

        /** Set key at given index.
          * \param pIndex Index of where the key should be set.
          * \param pKey The key to set.
          * \return \c true if key time is superior to previous key and inferior 
          *         to next key, \c false otherwise.
          * \remarks Result is undetermined if function curve has no key or index 
          *          is out of bounds.
          */
        virtual bool KeySet(int pIndex, KFbxAnimCurveKeyBase& pKey) = 0;

        /** Remove key at given index.
          * \param pIndex Index of key to remove.
		  *	\return \c true on success, \c false otherwise.
          */
        virtual bool KeyRemove(int pIndex) = 0;

        /** Remove all the keys in the given range.
          * \param pStartIndex Index of the first key to remove (inclusive).
          * \param pEndIndex Index of the last key to remove (inclusive).
		  *	\return \c true on success, \c false otherwise.
          */
        virtual bool KeyRemove(int pStartIndex, int pEndIndex) = 0;

    //@}

    /**
    * \name Key Time Manipulation
    */
    //@{
        /** Get key time.
          * \param pKeyIndex Key index.
          * \return Key time (time at which this key is occurring).
          */
        virtual KTime KeyGetTime(int pKeyIndex) const { return KTIME_INFINITE; }

        /** Set key time.
          * \param pKeyIndex Key index.
          * \param pTime Key time (time at which this key is occurring).
          */
        virtual void KeySetTime(int pKeyIndex, KTime pTime) = 0;

    //@}

    /**
    * \name Extrapolation 
    * Extrapolation defines the function curve value before and after the keys.
    * Pre-extrapolation defines the function curve value before first key.
    * Post-extrapolation defines the function curve value after last key.
    * <ul><li>CONSTANT means a constant value matching the first/last key.
    *     <li>REPETITION means the entire function curve is looped.
    *     <li>MIRROR_REPETITION means the entire function curve is looped once backward, once forward and so on. 
    *     <li>KEEP_SLOPE means a linear function with a slope matching the first/last key.</ul>
    */
    //@{
        typedef enum {
            eCONSTANT          = 1,
            eREPETITION        = 2,
            eMIRROR_REPETITION = 3,
            eKEEP_SLOPE        = 4
        } EExtrapolationType;

        /** Set pre-extrapolation mode.
          * \param pExtrapolation The pre-extrapolation mode to set.
          */
        void SetPreExtrapolation(EExtrapolationType pExtrapolation) { mPreExtrapolation = pExtrapolation; }
            
        /** Get pre-extrapolation mode.
          * \return The current pre-extrapolation mode.
          */
        EExtrapolationType GetPreExtrapolation() const { return mPreExtrapolation; }
        
        /** Set pre-extrapolation count.
          * \param pCount Number of repetitions if pre-extrapolation mode is
          *               REPETITION or MIRROR_REPETITION.
          */
        void SetPreExtrapolationCount(unsigned long pCount) { mPreExtrapolationCount = pCount; }
        
        /** Get pre-extrapolation count.
          * \return Number of repetitions if pre-extrapolation mode is
          *         REPETITION or MIRROR_REPETITION.
          */
        unsigned long GetPreExtrapolationCount() const { return mPreExtrapolationCount; }
        
        /** Set post-extrapolation mode.
          * \param pExtrapolation The post-extrapolation mode to set.
          */
        void SetPostExtrapolation(EExtrapolationType pExtrapolation) { mPostExtrapolation = pExtrapolation; }
        
        /** Get post-extrapolation mode.
          * \return The current post-extrapolation mode.
          */
        EExtrapolationType GetPostExtrapolation() const { return mPostExtrapolation; }
        
        /** Set post-extrapolation count.
          * \param pCount Number of repetitions if post-extrapolation mode is
          *               REPETITION or MIRROR_REPETITION.
          */
        void SetPostExtrapolationCount(unsigned long pCount) { mPostExtrapolationCount = pCount; }
            
        /** Get post-extrapolation count.
          * \return Number of repetitions if post-extrapolation mode is
          *         REPETITION or MIRROR_REPETITION.
          */
        unsigned long GetPostExtrapolationCount() const { return mPostExtrapolationCount; }
    //@}

    /**
      * \name Evaluation and Analysis
      */
    //@{
        /** Evaluate curve value at a given time.
          * \param pTime Time of evaluation.
          * \param pLast Index of the last processed key to speed up search. If this 
          *              function is called in a loop, initialize this value to 0 and let it 
          *              be updated by each call.
          * \return Evaluated curve value.
          * \remarks This function take extrapolation into account.
          */
          virtual float Evaluate (KTime pTime, int* pLast = NULL) = 0;

        /** Evaluate curve value at the given key index.
          * \param pIndex Any value from 0 to KFCurve::KeyGetCount() - 1.
          *               If this index falls between keys, the curve value will
          *               be interpolated based on the surrounding keys.
          * \return Evaluated curve value.
          */
        virtual float EvaluateIndex( double pIndex) = 0;
    //@}

    /**
      * \name Utility functions.
      *
      */
    //@{
        /** Find out start and end time of the animation curve.
          * This function retrieves the Curve's time span.
          * \param pStart Reference to receive start time.
          * \param pStop Reference to receive end time.
          * \return \c true on success, \c false otherwise.
          * \remarks This function is deprecated, please use GetTimeInterval(KTimeSpan&) instead.
          */
        virtual bool GetTimeInterval(KTime& pStart, KTime& pStop);

        /** Find out start and end time of the animation curve.
          * This function retrieves the Curve's time span.
          * \param pTimeInterval Reference to receive start time and end time.
          * \return \c true on success, \c false otherwise.
          */
        virtual bool GetTimeInterval(KTimeSpan& pTimeInterval);
    //@}

protected:
    KFbxAnimCurveBase(KFbxSdkManager& pManager, char const* pName);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    ///////////////////////////////////////////////////////////////////////////////
    //  WARNING!
    //    Anything beyond these lines may not be documented accurately and is 
    //     subject to change without notice.
    ///////////////////////////////////////////////////////////////////////////////
public:
    virtual bool Store(KFbx* pFileObject) = 0;
    virtual bool Retrieve(KFbx* pFileObject) = 0;

private:
    EExtrapolationType mPreExtrapolation;
    unsigned long      mPreExtrapolationCount;
    EExtrapolationType mPostExtrapolation;
    unsigned long      mPostExtrapolationCount;

    friend class KFbxObject;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVEBASE_H
