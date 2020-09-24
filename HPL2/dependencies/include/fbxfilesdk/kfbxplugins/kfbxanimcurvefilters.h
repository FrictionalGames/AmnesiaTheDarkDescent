/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxanimcurvefilters.h
*/

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVEFILTERS_H
#define FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVEFILTERS_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxplugins/kfbxanimcurve.h>

#include <fbxfilesdk/components/kbaselib/klib/ktime.h>
#include <fbxfilesdk/components/kbaselib/klib/kerror.h>
#include <fbxfilesdk/kfbxmath/kfbxxmatrix.h>

#include <fbxfilesdk/kfbxio/kfbxiosettings.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxAnimStack;
class KFbxAnimCurve;
class KFbxAnimCurveNode;
class KMBRotationOrder;

/** Base class for animation curve filters.
* Animation curves can be modified through filters. The filters act on 
* the curve keys and values. They can move, add or remove keys,
* modify key values and key tangents, depending on the desired action
* of the filter.
* Some simple examples are:
* \li A scale filter, that would multiply all key
* values of a curve, and the curve default value, by a given scale.
* \li A constant key reducer filter, that would clean a curve by removing
* redundant keys that all have the same value.
*
* Filters can act on a single animation curve (KFbxAnimCurve), but some 
* filters need to work on many animation curves at the same time. For
* this reason, the input to a filter can be an animation stack (KFbxAnimStack), an object (KFbxObject)
* with animated properties, an animation curve node (KFbxAnimCurveNode), or an array of animation
* curves (KFbxAnimCurve).
* For example, an unroll filter acts on 3 Euler rotation curves (X, Y and Z) at the same time.
*
* A filter has a start time (that can be as low as TC_MINFINITY) and a stop time (that can be as high as TC_INFINITY).
* The filter is only applied to the parts of the animation curves that are between the start and stop time.
* 
* The following are two code samples about how to use filter.
* Code sample to use sync filter:
* \code
*    KFbxAnimCurve* lWorkCurves[3]; //Put some keys in the lWorkCurves and they sync them up.
*    KFbxAnimCurveFilterKeySync lSyncFilter;
*    KTime pStart, pStop; //Given start and stop time.
*    lSyncFilter.SetStartTime( pStart );
*    lSyncFilter.SetStopTime ( pStop  );
*    if( lSyncFilter.NeedApply( lWorkCurves, 3 ) )
*    {
*        lSyncFilter.Apply( lWorkCurves, 3 );
*    }
* \endcode
*
* Code sample to use unroll filter:
* \code
*    KFbxAnimCurveNode* pCurveNode; //An Euler rotation animation curve node.
*    KFbxAnimCurveFilterUnroll lUnrollFilter;
*    lUnrollFilter.SetForceAutoTangents(true);
*    lUnrollFilter.Apply(*pCurveNode);
* \endcode
*
* \nosubgrouping
*/
class KFBX_DLL KFbxAnimCurveFilter
{
public:
    //! Constructor.
    KFbxAnimCurveFilter();

    //! Destructor.
    virtual ~KFbxAnimCurveFilter() {};

	/**
	 * \name Member functions
	 */
	//@{
    /** Get the name of the filter.
      * \return     Pointer to the name.
      */
    virtual const char* GetName() const {return NULL;}

    /** Get the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \return     The time expressed as KTime.
      */
    KTime& GetStartTime() {return mStart;}

    /** Set the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \param pTime     The time to be set.
      */
    void SetStartTime(KTime& pTime) { mStart = pTime; }

    /** Get the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \return     The time expressed as KTime.
      */
    KTime& GetStopTime() {return mStop;}

    /** Set the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \param pTime     The time to be set.
      */
    void SetStopTime(KTime& pTime) { mStop = pTime; }

    /** Get the index of start key on the given curve. This is the index of the first key
	  * after (or on) the filter's start time.
      * \param pCurve     Curve on which we want to retrieve the start key.
      * \return           Index of the start key.
      */
    int GetStartKey(KFbxAnimCurve& pCurve) const;

    /** Get the index of stop key on the given curve. This is the index of the last key
	  * before (or on) the filter's stop time.
      * \param pCurve     Curve on which we want to retrieve the stop key.
      * \return           Index of the stop key.
      */
    int GetStopKey(KFbxAnimCurve& pCurve) const;

    /** Check if any curve on the animation stack needs an application of the filter.
      * \param pAnimStack     Animation stack where to retrieve the animation curves
      * \return               \c true if at least one animated property needs an application of the filter.
      */
    virtual bool NeedApply(KFbxAnimStack* pAnimStack);

    /** Check if all the animated properties of the object need an application of the filter.
      * \param pObj           Object containing the properties to test.
      * \param pAnimStack     Animation stack where to retrieve the animation curves
      * \return               \c true if at least one animated property needs an application of the filter.
      */
    virtual bool NeedApply(KFbxObject* pObj, KFbxAnimStack* pAnimStack);

    /** Check if the animation curve node needs an application of the filter.
      * \param pCurveNode     Curve node to test.
      * \return               \c true if the animation curve node needs an application of the filter.
      * \remarks              This method collects all the KFbxAnimCurve objects connected to the curve node
      *                       and calls NeedApply(KFbxAnimCurve**, int)
      */
    virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode);

    /** Check if the given animation curve need an application of the filter.
      * \param pCurve     Array of curves to test if they need the and application of the filter.
      * \param pCount     Number of curves in array.
      * \return           \c true if at least one animation curve in the array needs an application of the filter.
      */
    virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

    /** Check if an animation curve need an application of the filter.
      * \param pCurve     Curve to test if it needs application of filter.
      * \return           \c true if the animation curve needs an application of the filter.
      */
    virtual bool NeedApply(KFbxAnimCurve& pCurve);

    /** Apply filter to all the curves stored in the animation stack.
      * \param pAnimStack     Animation stack where to retrieve the animation curves
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    virtual bool Apply(KFbxAnimStack* pAnimStack);

    /** Apply filter to all the animated properties of the object.
      * \param pObj           Object containing the animated properties to which the filter is applied.
      * \param pAnimStack     Animation stack where to retrieve the animation curves
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    virtual bool Apply(KFbxObject* pObj, KFbxAnimStack* pAnimStack);

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      * \remarks              This method collects all the KFbxAnimCurve objects connected to the curve node
      *                       and calls Apply(KFbxAnimCurve**, int)
      */
    virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on an array of animation curves.
      * \param pCurve     Array of curves to which the filter is applied.
      * \param pCount     Number of curves in the array.
      * \return           \c true if the curve filtering operation was successful, \c false otherwise.
      */
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    virtual bool Apply(KFbxAnimCurve& pCurve) = 0;

    /** Reset the filter to its default parameters.
    */
    virtual void Reset() 
    { 
        mStart= KTIME_MINUS_INFINITE;
        mStop = KTIME_INFINITE;    
    }

    /** Retrieve error object.
      * \return     Error object.
      */
    KError* GetError() { return &mError; }

    /** Get last error ID.
      * \return     Last error ID.
      */
    int GetLastErrorID() { return mError.GetLastErrorID(); }

    /** Get last error string.
      * \return     Last error string.
      */
    const char* GetLastErrorString() const { return mError.GetLastErrorString(); }
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

	static bool GetContinuousOffset(KMBRotationOrder& pOrder, double pOffset[3], double pNew[3], double pOld[3]);

protected:
    void GetKFCurvesFromAnimCurve(KFbxAnimCurve** pSrc, int pSrcCount, KFCurve** pDst, int& pDstCount);

    // Called for progress bar update, indicating what portion of work is done.
    virtual void UpdateProgressInformation(KTime pStart, KTime pStop) {};

    void FillError(const char* pErrorStringArray [], int pErrorCount)
    {
        mError.Reset(pErrorStringArray, pErrorCount);
    };

    // Time span for applying the filter.
    KTime mStart, mStop;

    // Error handling.
    KError mError;
#endif
};


/** Constant key reducing filter.
  * \nosubgrouping
  * Filter to test if each key is really necessary to define the curve
  * at a definite degree of precision. It filters recursively from the
  * strongest difference first. All useless keys are eliminated.
  */
class KFBX_DLL KFbxAnimCurveFilterConstantKeyReducer : public KFbxAnimCurveFilter
{
public:
    //! Constructor.
    KFbxAnimCurveFilterConstantKeyReducer();

    //! Destructor.
    virtual ~KFbxAnimCurveFilterConstantKeyReducer() {};

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    virtual const char* GetName() const;

    /**
	 * \name Exposed parent class methods.
	 */
	//@{
    virtual bool Apply(KFbxAnimStack* pAnimStack)                   { return KFbxAnimCurveFilter::Apply(pAnimStack); }
    virtual bool Apply(KFbxObject* pObj, KFbxAnimStack* pAnimStack) { return KFbxAnimCurveFilter::Apply(pObj, pAnimStack); }
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount)          { return KFbxAnimCurveFilter::Apply(pCurve, pCount); }
    //@}

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      * \remarks              This method collects all the KFbxAnimCurve objects connected to the curve node
      *                       and calls Apply(KFbxAnimCurve**, int)
      */
    virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    virtual void Reset();

    /** Get the current derivative tolerance.
      * \return     The value of the current derivative tolerance.
      */
    double GetDerivativeTolerance() const;

    /** Set the derivative tolerance.
      * \param pValue     Value derivative tolerance.
      */
    void SetDerivativeTolerance(double pValue);

    /** Get the tolerance value.
      * \return     The tolerance value.
      */
    double GetValueTolerance() const;

    /** Set the tolerance value.
      * \param pValue     Tolerance value.
      */
    void SetValueTolerance(double pValue);

    /** Get the state of the KeepFirstAndLastKeys flag.
      * \return      \c true if the filter keeps the first and last keys.
      */
    bool GetKeepFirstAndLastKeys() const;

    /** Set the state of the KeepFirstAndLastKeys flag.
      * \param pKeepFirstAndLastKeys     Set to \c true if you want the filter to keep the first and last keys.
      */
    void SetKeepFirstAndLastKeys( bool pKeepFirstAndLastKeys );

    /** Get the state of the KeepOneKey flag. 
	  * If all the keys are constant and this flag is c\ true, the filter will keep the first key.  
	  * If all the keys are constant and this flag is c\ false, the filter will delete all the keys. 
      * \return     \c true if the filter keeps the first key when all keys are constant.
      */
    bool GetKeepOneKey() const;

    /** Set the state of the KeepOneKey flag.
	  * If all the keys are constant and this flag is c\ true, the filter will keep the first key.  
	  * If all the keys are constant and this flag is c\ false, the filter will delete all the keys. 
      * \param pKeepOneKey     Set to \c true if you want the filter to keep the first key when all keys are constant.
      */
    void SetKeepOneKey( bool pKeepOneKey );

    /** Tell the filter to keep CUBIC curve keys which are not pure AUTO.
	  * \param pKeep KeepNotPureAutoKeys flag.
	  */
	void SetKeepNotPureAutoKeys(bool pKeep);

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    //
    //  If ValueTolerance is default, we use the thresholds here, otherwise
    //  it is the ValueTolerance that is used. (Mainly for backward compatibility)
    //
    void SetTranslationThreshold    ( double pTranslationThreshold );
    void SetRotationThreshold       ( double pRotationThreshold );
    void SetScalingThreshold        ( double pScalingThreshold );
    void SetDefaultThreshold        ( double pDefaultThreshold );

    void SetModes(bool pExporting, KFbxIOSettings& pIOS);

private:
    double  mDerTol;
    double  mValTol;

    double  mTranslationThreshold;
    double  mRotationThreshold;
    double  mScalingThreshold;
    double  mDefaultThreshold;

    bool   mKeepFirstAndLastKeys;
    bool   mKeepOneKey;
    bool   mKeepNotPureAutoKeys;

    bool IsKeyConstant(KFbxAnimCurve& pCurve, int pIndex, int pFirstIndex, int pLastIndex, 
                       double pMinValue, double pMaxValue, bool pOnlyCheckAutoKeys);
#endif
};

/**This filter tries to compensate parent's scale to children's scale.
 * This filter is used to convert scale animation curves of nodes whose transform inherit type are eINHERIT_Rrs.
 * In the eINHERIT_Rrs mode, child objects do not inherit scaling from parent objects at all.
 * When a parent object is scaled, the child does not scale, but translates in order to keep proportional distance between models.
 * If you want to change the inherit type of certain nodes from eINHERIT_Rrs to eINHERIT_RrSs, 
 * you may call this filter to compensate scale.
 */
class KFBX_DLL KFbxAnimCurveFilterScaleCompensate : public KFbxAnimCurveFilter
{
public:
    //! Error codes.
    enum EError
    {
        eNo2Curves, //!< The filter needs 2 curves.
        eErrorCount //!< End flag.
    };

    //! Constructor.
    KFbxAnimCurveFilterScaleCompensate();
    //! Return name of the filter.
    virtual const char* GetName() const;

    /**Compensate parent's scale to children's scale. 
     * \param pCurve In pCurve, index 0 is the curve to be filtered. index 1 is the parent curve.
     * \param pCount Need to be 2.
     * \param pIOS   IO setting object.   
     * \return       \c true if the curve filtering operation was successful, \c false otherwise.
     * \remarks      This filter will re-sample the animation curves.
     */
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount, KFbxIOSettings& pIOS);
    /** Always fail because this filter needs 2 curves. */
    virtual bool Apply(KFbxAnimCurve& pCurve);
};

/**GimbleKiller filter.
  *\nosubgrouping
  * This filter try to minimize gimble locks on rotation curves.
  * \remarks The current implementation of this filter expects to process 3 curves at the same time.
  * \remarks This filter has been superseded by the Unroll filter. It is strongly advised to use
  *          the latter.
  */
class KFBX_DLL KFbxAnimCurveFilterGimbleKiller : public KFbxAnimCurveFilter
{
public:
    //! Constructor.
    KFbxAnimCurveFilterGimbleKiller();

    //! Destructor.
    virtual ~KFbxAnimCurveFilterGimbleKiller();

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    virtual const char* GetName() const;

    /** This filter expects to work with 3 interdependent curves. Passing the animation stack makes no sense.
      * since this object would not know which curves to handle.
      * \param pAnimStack     Animation stack
      * \return               \c false.
      */
    virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return false; }

    /** This filter expects to work with 3 interdependent curves. Collecting all the animation curves from
      * the properties defined in \e pObj could not guarantee that we are manipulating 3 interdependent curves.
      * \param pObj           Object containing the properties to test.
      * \param pAnimStack     Animation stack where to retrieve the animation curves
      * \return               \c false
      */
    virtual bool NeedApply(KFbxObject* pObj, KFbxAnimStack* pAnimStack) { return false; }

	/** Check if the animation curve node needs an application of the filter.
	 * \param pCurveNode     Curve node to test.
	 * \return               \c true if the animation curve node needs an application of the filter, \c false otherwise.
	 * \remarks              This method checks that the \e pCurveNode is representing an Euler rotation. 
	 *                       It will validate that 3 animation curves are defined. 
	 *                       If the condition is not met, the method will return \c false.
	 */
    virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode);

    /** Check if the given animation curve need an application of the filter.
      * \param pCurve         Array of curves to test if they need the and application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter.
      * \remarks              Because this method only receives an array of interdependent curves, this filter assumes 
      *                       that they are all coming from an Euler rotation anim curve node. Therefore, it expects 
      *                       \e pCount to be equal to 3.
      */
    virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

     /** This filter expects to work with interdependent curves. Receiving one single curve is useless.
       * \return              \c false
       */
    virtual bool NeedApply(KFbxAnimCurve& pCurve) { return false; }
    
    /** This filter expects to work with 3 interdependent curves. Passing the animation stack makes no sense
      * since this object would not know which curves to handle.
      * \param pAnimStack     Animation stack
      * \return               \c false.
      */
    virtual bool Apply(KFbxAnimStack* pAnimStack) { return false; }

    /** This filter expects to work with 3 interdependent curves. Collecting all the animation curves from
      * the properties defined in \e pObj could not guarantee that we are manipulating 3 interdependent curves.
      * \param pObj           Object containing the properties to test.
      * \param pAnimStack     Animation stack where to retrieve the animation curves
      * \return               \c false
      */
    virtual bool Apply(KFbxObject* pObj, KFbxAnimStack* pAnimStack) { return false; }

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      * \remarks              This method collects all the KFbxAnimCurve objects connected to the curve node
      *                       and calls Apply(KFbxAnimCurve**, int)
      */
    virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      * \remarks              Because this method only receives an array of interdependent curves, this filter assumes 
      *                       that they are all coming from an Euler rotation anim curve node. Therefore, it expects 
      *                       \e pCount to be equal to 3.
      */
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** This filter expects to work with interdependent curves. Receiving one single curve is useless.
      * \return               \c false
      */
    virtual bool Apply(KFbxAnimCurve& pCurve) { return false; }

    /** Reset the filter to its default parameters.
      */
    virtual void Reset();

	//! Return \c true if key sync filter is enabled.
	bool GetApplyKeySyncFilter() const;

	/** Set to \c true to enable key sync filter.
	  * \param pFlag Key sync filter flag.
	  */
	void SetApplyKeySyncFilter(bool pFlag);

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
    KMBRotationOrder*   mRotationOrder;
    bool                mApplyKeySyncFilter;
    int                 mRotationLayerType;
#endif
};

/** Key reducing filter.
  * \nosubgrouping
  * Filter to test if each key is really necessary to define the curve
  * at a definite degree of precision. It filters recursively from the
  * strongest difference first. All useless keys are eliminated.
  */
class KFBX_DLL KFbxAnimCurveFilterKeyReducer : public KFbxAnimCurveFilter
{
public:
    //! Constructor.
    KFbxAnimCurveFilterKeyReducer();

    //! Destructor.
    virtual ~KFbxAnimCurveFilterKeyReducer() {};

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    virtual const char* GetName() const;

    /**
	 * \name Exposed parent class methods.
	 */
	//@{
    virtual bool Apply(KFbxAnimStack* pAnimStack)                   { return KFbxAnimCurveFilter::Apply(pAnimStack); }
    virtual bool Apply(KFbxObject* pObj, KFbxAnimStack* pAnimStack) { return KFbxAnimCurveFilter::Apply(pObj, pAnimStack); }
    virtual bool Apply(KFbxAnimCurveNode& pCurveNode)               { return KFbxAnimCurveFilter::Apply(pCurveNode); }
    //@}

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    virtual void Reset();

	//!	Get precision.
	double GetPrecision() const;

	/**	Set precision.
	  * \param pPrecision The precision to set.
	  */
	void SetPrecision(double pPrecision);

	//!	Return \c true key sync is applied at the end.
	bool GetKeySync() const;

	/**	Set to \c true to apply key sync at the end.
	  * \param pKeySync Key sync flag.
	  */
	void SetKeySync(bool pKeySync);

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
    bool  KeyReducer(KFbxAnimCurve& pSCurve, KFbxAnimCurve& pTCurve, KTime pStart, KTime pStop);
    bool  Subdivise(KFbxAnimCurve& pSCurve, KFbxAnimCurve& pTCurve, int pLeft, int pRight);
    double FindMaxError(KFbxAnimCurve& pSCurve, KFbxAnimCurve& pTCurve, int pLeft, int pRight, int& pSplit);
    	
    // User parameters.
    double  mPrecision;
    int    mProgressCurrentRecurseLevel;
    bool   mKeySync;
#endif
};


/**	Key sync filter.
  * \nosubgrouping
  * Filter to synchronize the keys of a set of animation curves.
*/
class KFBX_DLL KFbxAnimCurveFilterKeySync : public KFbxAnimCurveFilter
{
public:
    //! Constructor.
    KFbxAnimCurveFilterKeySync();

    //! Destructor.
    virtual ~KFbxAnimCurveFilterKeySync() {};

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    virtual const char* GetName() const;

    /**
	 * \name Exposed parent class methods.
	 */
	//@{
    virtual bool NeedApply(KFbxAnimStack* pAnimStack)                   { return KFbxAnimCurveFilter::NeedApply(pAnimStack); }
    virtual bool NeedApply(KFbxObject* pObj, KFbxAnimStack* pAnimStack) { return KFbxAnimCurveFilter::NeedApply(pObj, pAnimStack); }
    virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode)               { return KFbxAnimCurveFilter::NeedApply(pCurveNode); }
    virtual bool Apply(KFbxAnimStack* pAnimStack)                       { return KFbxAnimCurveFilter::Apply(pAnimStack); }
    virtual bool Apply(KFbxObject* pObj, KFbxAnimStack* pAnimStack)     { return KFbxAnimCurveFilter::Apply(pObj, pAnimStack); }
    virtual bool Apply(KFbxAnimCurveNode& pCurveNode)                   { return KFbxAnimCurveFilter::Apply(pCurveNode); }
    //@}

    /** Check if the given animation curve need an application of the filter.
      * \param pCurve         Array of curves to test if they need the and application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter.
      */
    virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

    /** One single curve cannot be sync'ed.
      * \param pCurve     Curve to test if it needs application of filter.
      * \return           \c false
      */
    virtual bool NeedApply(KFbxAnimCurve& pCurve) { return false; }

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true.
      * \remarks              Has no effect since there is only one curve.
      */
    virtual bool Apply(KFbxAnimCurve& pCurve) { return true; }

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#endif
};


/** Re-sampling filter.
  * \nosubgrouping
  * Filter to re-sample animation curves.
  */
class KFBX_DLL KFbxAnimCurveFilterResample : public KFbxAnimCurveFilter
{
public:
    //! Constructor.
    KFbxAnimCurveFilterResample();

    //! Destructor.
    virtual ~KFbxAnimCurveFilterResample() {};

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    virtual const char* GetName() const;

    /**
	 * \name Exposed parent class methods.
	 */
	//@{
    virtual bool Apply(KFbxAnimStack* pAnimStack)                       { return KFbxAnimCurveFilter::Apply(pAnimStack); }
    virtual bool Apply(KFbxObject* pObj, KFbxAnimStack* pAnimStack)     { return KFbxAnimCurveFilter::Apply(pObj, pAnimStack); }
    virtual bool Apply(KFbxAnimCurveNode& pCurveNode)                   { return KFbxAnimCurveFilter::Apply(pCurveNode); }
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount)              { return KFbxAnimCurveFilter::Apply(pCurve, pCount); }
    //@}

    /** Apply the filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    virtual void Reset();

    /** Set if the keys are on frame.
      * \param pKeysOnFrame     value if keys are set on frame multiples.
      */
    void SetKeysOnFrame(bool pKeysOnFrame);

    /** Get if the keys are on frame.
      * \return     Value if keys are on frame multiples.
      */
    bool GetKeysOnFrame() const;

    /** Get the re-sampling period
      * \return     The re-sampling period.
      */
    KTime GetPeriodTime() const;

    /** Set the re-sampling period
      * \param pPeriod     The re-sampling period to be set.
      */
    void SetPeriodTime(KTime &pPeriod);


    /**Get the mode that determines how the re-sample filter will set the interpolation and tangent of each key.
      * \return     \c true if the intelligent mode is on, \c false otherwise.
	  * \remarks If intelligent mode is on, interpolation type and tangent mode of each created curve key 
	  *          are set equal to the interpolation type and tangent mode of the closest curve key encountered.
	  *          If intelligent mode is off, the interpolation type of each created curve key 
	  *          will always be set to CUBIC, and tangent mode will always be set to AUTO.
      */
    bool  GetIntelligentMode() const;

    /** Set the mode that determines how the re-sample filter will set the interpolation and tangent of each key.
	  * \param pIntelligent     \c true, set interpolation type and tangent mode of each created curve key equal to
	  *                         the interpolation type and tangent mode of the closest curve key encountered.
	  *                         \c false, always set the interpolation type of each created curve key to CUBIC,
	  *                         and always set the tangent mode to AUTO.
      */
    void  SetIntelligentMode( bool pIntelligent );

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
    bool    mKeysOnFrame;
    KTime   mPeriod;
	bool    mIntelligent;
#endif

};


/**	Key scale filter.
  * \nosubgrouping
  * Filter to scale the keys of a set of animation curves.
*/
class KFBX_DLL KFbxAnimCurveFilterScale : public KFbxAnimCurveFilter
{
public:
    //! Constructor.
    KFbxAnimCurveFilterScale();

    //! Destructor.
    virtual ~KFbxAnimCurveFilterScale() {};

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    virtual const char* GetName() const;

    /**
	 * \name Exposed parent class methods.
	 */
	//@{
    virtual bool Apply(KFbxAnimStack* pAnimStack)                       { return KFbxAnimCurveFilter::Apply(pAnimStack); }
    virtual bool Apply(KFbxObject* pObj, KFbxAnimStack* pAnimStack)     { return KFbxAnimCurveFilter::Apply(pObj, pAnimStack); }
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount)              { return KFbxAnimCurveFilter::Apply(pCurve, pCount); }
    //@}

     /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      * \remarks              This method collects all the KFbxAnimCurve objects connected to the curve node
      *                       and calls Apply(KFbxAnimCurve**, int)
      */
    virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    virtual void Reset();

    /** Get the scale factor.
	  *	\return The current scale factor.
	  */
	double GetScale() const;

	/** Set the scale factor.
      * \param pScale The new scale factor to set.
	  */
	void SetScale(double pScale);

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
    double mScale;
#endif
};


/**Time shift and scale filter.
  *\nosubgrouping
  * Filter to shift key times and scale key values on animation curves.
  */
class KFBX_DLL KFbxAnimCurveFilterTSS : public KFbxAnimCurveFilter
{
public:
    //! Constructor. 
    KFbxAnimCurveFilterTSS();

    //! Destructor.
    virtual ~KFbxAnimCurveFilterTSS() {};

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    virtual const char* GetName() const;

    /**
	 * \name Exposed parent class methods.
	 */
	//@{
    virtual bool Apply(KFbxAnimStack* pAnimStack)                       { return KFbxAnimCurveFilter::Apply(pAnimStack); }
    virtual bool Apply(KFbxObject* pObj, KFbxAnimStack* pAnimStack)     { return KFbxAnimCurveFilter::Apply(pObj, pAnimStack); }
    virtual bool Apply(KFbxAnimCurveNode& pCurveNode)                   { return KFbxAnimCurveFilter::Apply(pCurveNode); }
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount)              { return KFbxAnimCurveFilter::Apply(pCurve, pCount); }
    //@}

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    virtual void Reset();

    /** Get the time shift value.
    * \return     The time value used for the shift.
    */
	KTime GetShift() const;

    /** Set the time shift value.
      * \param pShift     The time value used for the shift.
      */
	void SetShift(KTime& pShift);

    /** Get the scale factor.
    * \return     The current scale factor.
    */
	double GetScale() const;

    /** Set the scale factor.
      * \param pScale     The new scale factor to set.
      */
	void SetScale(double pScale);

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
    KTime  mShift;
    double  mScale;
#endif
};

/** Unroll filter.
  *\nosubgrouping
  * Filter to apply continuous rotation values to animation curves. Due to Euler rotation
  * properties, when a rotation angle cross over the 180 degree value, it becomes -179. This
  * filter tries to keep a continuous rotation effectively by producing increasing values, to 
  * actually become 181 degrees, etc...
  * \remarks The current implementation of this filter expects to process 3 curves at the same time.
  * \remarks By default, this filter does not affect the tangent values of the modified keys.
  *          This means that, for CUBIC interpolation curves containing keys with USER or BREAK
  *          tangents, the unrolled curves will correctly match the original rotation exactly on 
  *          the curve keys but not in-between them. The filter can be configured to automatically 
  *          convert the USER and BREAK tangents to AUTO tangents by setting the ForceAutoTangents flag.
  *          Using the AUTO tangents mode can result in a more consistent interpolation between
  *          the curve keys.
  */
class KFBX_DLL KFbxAnimCurveFilterUnroll : public KFbxAnimCurveFilter
{
public:
    //! Constructor.
    KFbxAnimCurveFilterUnroll();

    //! Destructor.
    virtual ~KFbxAnimCurveFilterUnroll() {};
   
    /** Get the name of the filter.
      * \return     Pointer to the name.
      */
    virtual const char* GetName() const;

    /** This filter expects to work with 3 interdependent curves. Passing the animation stack makes no sense
      * since this object would not know which curves to handle.
      * \param pAnimStack     Animation stack
      * \return               \c false.
      */
    virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return false; };

    /** This filter expects to work with 3 interdependent curves. Collecting all the animation curves from
      * the properties defined in \e pObj could not guarantee that we are manipulating 3 interdependent curves.
      * \param pObj           Object containing the properties to test.
      * \param pAnimStack     Animation stack where to retrieve the animation curves
      * \return               \c false.
      */
    virtual bool NeedApply(KFbxObject* pObj, KFbxAnimStack* pAnimStack) { return false; }

    /** Check if the animation curve node needs an application of the filter.
      * \param pCurveNode     Curve node to test.
      * \return               \c true if the animation curve node needs an application of the filter, \c false otherwise.
      * \remarks              This method checks that the \e pCurveNode is representing an Euler rotation. 
	  *                       It will validate that 3 animation curves are defined. 
	  *                       If the condition is not met, the method will return \c false.
      */
    virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode);

    /** Check if the given animation curve needs an application of the filter.
      * \param pCurve         Array of curves to test if they need an application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter, 
	  *                       \c false otherwise.
      * \remarks              Because this method only receives an array of interdependent curves, this filter assumes 
      *                       that they are all coming from an Euler rotation anim curve node. Therefore, it expects 
      *                       \e pCount to be equal to 3.
      */
    virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

    /** This filter expects to work with interdependent curves. Receiving one single curve is useless.
      * \return               \c false.
      */
    virtual bool NeedApply(KFbxAnimCurve& pCurve) { return false; };

    /** This filter expects to work with 3 interdependent curves. Passing the animation stack makes no sense
      * since this object would not know which curves to handle.
      * \param pAnimStack     Animation stack where to retrieve the animation curves.
      * \return               \c false.
      */
    virtual bool Apply(KFbxAnimStack* pAnimStack) { return false; };

    /** This filter expects to work with 3 interdependent curves. Collecting all the animation curves from
      * the properties defined in \e pObj could not guarantee that we are manipulating 3 interdependent curves.
      * \param pObj           Object containing the properties to test.
      * \param pAnimStack     Animation stack where to retrieve the animation curves.
      * \return               \c false.
      */
    virtual bool Apply(KFbxObject* pObj, KFbxAnimStack* pAnimStack) { return false; }

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      * \remarks              This filter expects a Euler rotation curve node with three curves.
      */
    virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      * \remarks              Because this method only receives an array of interdependent curves, this filter assumes 
      *                       that they are all coming from an Euler rotation anim curve node. Therefore, it expects 
      *                       \e pCount to be equal to 3.
      */
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** This filter expects to work with 3 interdependent curves. Receiving one single curve is useless.
      * \return               \c false.
      */
    virtual bool Apply(KFbxAnimCurve& pCurve) { return false; }

    /** Reset the filter to its default parameters.
      */
    virtual void Reset();

    /** Get the unroll quality tolerance.
    * \return     The current unroll quality tolerance.
	* \remarks    This value is only used when SetTestForPath() is set to true. 
    */
    double GetQualityTolerance() const;

    /** Set the unroll quality tolerance.
      * \param pQualityTolerance     The unroll quality tolerance to set.
	  * \remarks                     This value is only used when SetTestForPath() is set to true. 
      */
    void SetQualityTolerance(double pQualityTolerance);

    /** Get if the test path is enabled.
      * \return  \c true if test for path is enabled.
	  * \remarks The unroll filter takes a key as a reference key and updates the following keys accordingly to try to keep
	  *          the continuity between this reference key and its following keys.
	  *          If the test path is enabled, the filter can use the same key as reference key to update the following keys
	  *          until the difference of continuity between the newly updated key and the reference key exceeds the 
	  *          quality tolerance, then the reference key will be updated as the newly updated key.
	  *          If the test path is not enabled, the filter will always use the newly updated key as reference to update the next key.
	  *          The quality tolerance can be set and queried by SetQualityTolerance() and GetQualityTolerance().
      */
    bool GetTestForPath() const;

    /** Set if the test path is enabled.
      * \param pTestForPath     Value to set if test for path is to be enabled.
	  * \remarks The unroll filter takes a key as a reference key and updates the following keys accordingly to try to keep
	  *          the continuity between this reference key and its following keys.
	  *          If the test path is enabled, the filter can use the same key as reference key to update the following keys
	  *          until the difference of continuity between the newly updated key and the reference key exceeds the 
	  *          quality tolerance, then the reference key will be updated as the newly updated key.
	  *          If the test path is not enabled, the filter will always use the newly updated key as reference to update the next key.
	  *          The quality tolerance can be set and queried by SetQualityTolerance() and GetQualityTolerance().
	  */
    void SetTestForPath(bool pTestForPath);

    /** Get the current state of the ForceAutoTangents flag.
      * \return     \c true if forcing AUTO tangents is enabled.
      * \remarks This flag is considered only on curves using the CUBIC interpolation and
      *          keys with the USER or BREAK tangents. For any other type of interpolations
      *          or tangents, this flag is ignored.
      */
    bool GetForceAutoTangents() const;

    /** Set the new state of the ForceAutoTangents flag.
      * \param pForceAutoTangents     New value of the flag.
      * \remarks This flag is considered only on curves using the CUBIC interpolation and
      *          keys with the USER or BREAK tangents. For any other type of interpolations
      *          or tangents, this flag is ignored.
      */
    void SetForceAutoTangents(bool pForceAutoTangents);

	///////////////////////////////////////////////////////////////////////////////
	//
	//  WARNING!
	//
	//  Anything beyond these lines may not be documented accurately and is
	//  subject to change without notice.
	//
	///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

		void SetRotationOrder(int pOrder);

private:
    double  InterpolationQualityFactor(KFbxVector4& lV1, KFbxVector4& lV2);

    double          mQualityTolerance;
    bool            mTestForPath;
    bool            mForceAutoTangents;
    int	            mRotationOrder;
    int             mRotationLayerType;
#endif
};

/** Matrix conversion filter.
  * \nosubgrouping
  * \remarks The current implementation of this filter expects to process 9 curves. If the 
  *          ApplyUnroll flag is enabled, set with a call to SetApplyUnroll(), the 
  *          internal unroll filter will automatically be configured to convert USER and 
  *          BREAK tangents to AUTO (refer to the KFbxAnimCurveFilterUnroll documentation).
  */
class KFBX_DLL KFbxAnimCurveFilterMatrixConverter : public KFbxAnimCurveFilter
{
public:
    //! Constructor.
    KFbxAnimCurveFilterMatrixConverter();

    //! Destructor.
    virtual ~KFbxAnimCurveFilterMatrixConverter();

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    virtual const char* GetName() const;

    /**
	 * \name Exposed parent class methods.
	 */
	//@{
    virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount)          { return KFbxAnimCurveFilter::NeedApply(pCurve, pCount); }
    //@}

    /** This filter expects to work with interdependent curves. Passing the animation stack makes no sense
      * since this object would not know which curves to handle.
      * \param pAnimStack     Animation stack. 
      * \return               \c false 
      */
    virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return false; }

    /** This filter expects to work with 9 interdependent curves. Collecting all the animation curves from
      * the properties defined in \e pObj could not guarantee that we are manipulating 9 interdependent curves.
      * \param pObj           Object containing the properties to test.
      * \param pAnimStack     Animation stack where to retrieve the animation curves
      * \return               \c false
      */
    virtual bool NeedApply(KFbxObject* pObj, KFbxAnimStack* pAnimStack) { return false; }

    /** Check if the animation curve nodes need an application of the filter.
      * \param pCurveNode     Curves to test if they need an application of the filter.
      * \return               \c true if the animation curve nodes need an application of the filter and 
      *                       \c false if they don't or an incompatible configuration is detected.
      * \remarks              This method assumes that \e pCurveNode[0] holds the translation curve,
      *                       \e pCurveNode[1] holds the rotation curves and \e pCurveNode[2] holds the
      *                       scaling curves.
      */
    virtual bool NeedApply(KFbxAnimCurveNode* pCurveNode[3]);

    /** This filter expects to work with interdependent curves. Receiving one single curve is useless.
      * \return               \c false.
      */
    virtual bool NeedApply(KFbxAnimCurve& pCurve) { return false; }

    /** This filter expects to work with interdependent curves. Passing the animation stack makes no sense
      * since this object would not know which curves to handle.
      * \param pAnimStack     Animation stack where to retrieve the animation curves.
      * \return               \c false.
      */
    virtual bool Apply(KFbxAnimStack* pAnimStack) { return false; }

    /** This filter expects to work with 9 interdependent curves. Collecting all the animation curves from
      * the properties defined in \e pObj could not guarantee that we are manipulating 9 interdependent curves.
      * \param pObj           Object containing the properties to test.
      * \param pAnimStack     Animation stack where to retrieve the animation curves.
      * \return               \c false.
      */
    virtual bool Apply(KFbxObject* pObj, KFbxAnimStack* pAnimStack) { return false; }

    /** Apply filter on all the curves of the animation curve nodes.
      * \param pCurveNode     Curve nodes to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      * \remarks              This method assumes that \e pCurveNode[0] holds the translation curve,
      *                       \e pCurveNode[1] holds the rotation curves and \e pCurveNode[2] holds the
      *                       scaling curves.
      */
    virtual bool Apply(KFbxAnimCurveNode* pCurveNode[3]);

    /** Apply filter on the given animation curves.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      * \remarks              \e pCount must be equal to 9
      * \remarks              Because this method only manipulates KFbxAnimCurve objects, it cannot set/get
      *                       the channels value. If the calling application wishes to use this flavor of the
      *                       Apply() method, it is strongly suggested to use the method: 
      *                       KFbxAnimCurveFilterMatrixConverter::Apply(KFbxAnimCurve** pCurve, double& pVals[9]);
      *                       The Apply(KFbxAnimCurveNode*) method is not affected by this limitation since
      *                       the channel values can be accessed via the animation curve node. 
      */
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** Apply filter on the given animation curves.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pVals          Array of channel values (same size as \e pCurve).
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      * \remarks              This method assumes that \e pCurve contains exactly 9 curves.
      * \remarks              \e pVals must be correctly initialized with the channels values and, if the
      *                       method calculates new values, they will be returned in this array.
      * \remarks              The curves are assumed to represent: Translation X,Y and Z, Rotation X,Y and Z and
      *                       Scaling X,Y and Z in this order.
      */
    bool Apply(KFbxAnimCurve** pCurve, double* pVals);

    /** This filter expects to work with interdependent curves. Receiving one single curve is useless.
      * \return               \c false.
      */
    virtual bool Apply(KFbxAnimCurve& pCurve) { return false; };

    /** Reset the filter to its default parameters.
      */
    virtual void Reset();

    /** \enum EMatrixID Matrix ID
      * - \e ePreGlobal
      * - \e ePreTranslate
      * - \e ePostTranslate
      * - \e ePreRotate
      * - \e ePreScale
      * - \e ePostGlobal
      * - \e eScaleOffset
      * - \e eInactivePre
      * - \e eInactivePost
      * - \e eRotationPivot
      * - \e eScalingPivot
      * - \e eMatrixCount
      */
    enum EMatrixID
    {
        ePreGlobal,
        ePreTranslate,
        ePostTranslate,
        ePreRotate,
        ePostRotate,
        ePreScale,
        ePostScale,
        ePostGlobal,
        eScaleOffset,
        eInactivePre,
        eInactivePost,
        eRotationPivot,
        eScalingPivot,
        eMatrixCount
    };

    /** Get the Translation Rotation Scaling source matrix
      * \param pIndex      The matrix ID.
      * \param pMatrix     The matrix used to receive the source matrix.
      */
    void GetSourceMatrix(EMatrixID pIndex, KFbxXMatrix& pMatrix) const;

    /** Set the Translation Rotation Scaling source matrix.
      * \param pIndex      The matrix ID.
      * \param pMatrix     The matrix used to set the source matrix.
      */
    void SetSourceMatrix(EMatrixID pIndex, KFbxXMatrix& pMatrix);
    void SetSourceMatrix(EMatrixID pIndex, KgeAMatrix& pMatrix);

    /** Get the Translation Rotation Scaling destination matrix.
      * \param pIndex      The matrix ID.
      * \param pMatrix     The matrix used to receive the destination matrix.
      */
    void GetDestMatrix(EMatrixID pIndex, KFbxXMatrix& pMatrix) const;

    /** Set the Translation Rotation Scaling destination matrix.
      * \param pIndex      The matrix ID.
      * \param pMatrix     The matrix used to set the destination matrix.
      */
    void SetDestMatrix(EMatrixID pIndex, KFbxXMatrix& pMatrix);
    void SetDestMatrix(EMatrixID pIndex, KgeAMatrix& pMatrix);

    /** Get the re-sampling period.
      * \return     the re-sampling period.
      */
    KTime GetResamplingPeriod () const;

    /** Set the re-sampling period.
      * \param pResamplingPeriod     The re-sampling period to be set.
      */
    void SetResamplingPeriod (KTime& pResamplingPeriod);

    /** Get the current state of the flag which determines if the last key should be generated exactly at the end time or not.
	  * This filter handles 9 animation curves, each of them has a stop time, the latest one is defined as the end time.
      * \return     \c true if last key is set exactly at end time, \c false otherwise.
      */
    bool GetGenerateLastKeyExactlyAtEndTime() const;

    /** Set the flag to determine if the last key will be generated exactly at the end time or not.
	  * This filter handles 9 animation curves, each of them has a stop time, the latest one is defined as the end time.
      * \param pFlag    Set to \c true to generate the last key exactly at the end time, \c false otherwise.
      */
    void SetGenerateLastKeyExactlyAtEndTime(bool pFlag);

    /** Check if re-sampling is on frame rate multiple.
      * \return     \c true if re-sampling is on a frame rate multiple.
      */
    bool GetResamplingOnFrameRateMultiple() const;

    /** Set the re-sample on a frame rate multiple.
      * \param pFlag     The value to be set.
      * \remarks         It might be necessary that the starting time of the converted
      *                  animation starts at an multiple of frame period starting from time 0.
      *                  Most softwares play their animation at a definite frame rate, starting
      *                  from time 0.  As re-sampling occurs when we can't guarantee interpolation,
      *                  keys must match with the moment when the curve is evaluated.
      */
    void SetResamplingOnFrameRateMultiple(bool pFlag);

    /** Get the current state of the ApplyUnroll flag.
      * \return     \c true if the internal unroll filter is applied, \c false otherwise.
	  * \remarks    Enable the internal unroll filter to get continuous rotation animation curves.
	  * \see        KFbxAnimCurveFilterUnroll.
      */
    bool GetApplyUnroll() const;

    /** Set the state of the ApplyUnroll flag.
      * \param pFlag     Set to \c true to apply an unroll filter to the rotation curves internally, 
	  * \                set to \c false otherwise.
      */
    void SetApplyUnroll(bool pFlag);

    /** Get the current state of the flag that determines if constant key reducer is used or not.
      * \return     \c true if constant key reducer is applied, \c false otherwise.
      */
    bool GetApplyConstantKeyReducer() const;

    /** Set the state of the flag that determines if constant key reducer is used or not.
      * \param pFlag     Set to \c true to apply the constant key reducer,
      * \                Set to \c false otherwise.
      */
    void SetApplyConstantKeyReducer(bool pFlag);

    /** Get the current state of the flag that determines if the translation data should be re-sampled or not.
      * \return      \c true if translation data is re-sampled upon conversion, \c false otherwise.
      * \remarks     If this flag is \c false, translation data must be calculated
      *              after the conversion process, overriding the re-sampling process.
      */
    bool GetResampleTranslation() const;

    /** Set the state of the flag that determines if the translation data should be re-sampled or not.
      * \param pFlag     Set to \c true to re-sample the translation data, set to \c false otherwise.
      * \remarks         If this flag is set to \c false, translation data must be calculated
      *                  after the conversion process, overriding the re-sampling process.
      */
    void SetResampleTranslation(bool pFlag);

    /** Set the rotation order of the source matrix.
      * \param pOrder     The rotation order to be set.
      */
    void SetSrcRotateOrder(int pOrder);

    /** Set the rotation order of the destination matrix.
      * \param pOrder     The rotation order to be set.
      */
    void SetDestRotateOrder(int pOrder);

    /** Set the state of the flag to force usage of the filter even if source and destination matrices are equivalent.
      * \param pVal     Set to \c true to force usage of the filter, set to \c false otherwise.
      */
    void SetForceApply(bool pVal);

    /** Get the current state of the flag to force usage of the filter even if source and destination matrices are equivalent.
      * \return     \c true to force usage of the filter, \c false otherwise.
      */
    bool GetForceApply() const;
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
    // Nicer than referring to 0, 1, 2...
    enum
    {
        eX,
        eY,
        eZ,
        eDimCount
    };
		
	// Convert parameter cell.
    class Cell;
    
    bool MatricesEquivalence(KFbxXMatrix pMatArrayA [eMatrixCount], 
                             KFbxXMatrix pMatArrayB [eMatrixCount]) const;

    bool DoConvert(KFbxAnimCurve** pCurve, 
                    double pT[eDimCount], 
                    double pR[eDimCount], 
                    double pS[eDimCount]);

    void FindTimeInterval
    (
        KTime& pStart, 
        KTime& pEnd,
        KFbxAnimCurve* pTFCurve [eDimCount], 
        KFbxAnimCurve* pRFCurve [eDimCount], 
        KFbxAnimCurve* pSFCurve [eDimCount]
    );

    void ComputeTotalMatrix
    (
        KFbxXMatrix& pGlobal, 
        Cell& pCell,
        KFbxXMatrix& pTranslate,
        KFbxXMatrix& pRotate,
        KFbxXMatrix& pScale
	);

    void ExtractTransforms
    (
        KFbxVector4& pScaleVector,
        KFbxVector4& pRotateVector,
        KFbxVector4& pTranslateVector,
        KFbxXMatrix& pGlobal,
        Cell& pDest
    );

    void SetDestFCurve(KFbxAnimCurve* pCurve [eDimCount], 
                       int pIndex, 
                       KTime pTime, 
                       KFbxVector4 pVector,
                       KFbxAnimCurveDef::EInterpolationType pInterpMode[eDimCount], 
                       KFbxAnimCurveDef::ETangentMode pTangentMode[eDimCount]);

    void FillInterpAndTangeant(KTime& pTime, 
                               KFbxAnimCurve* pSourceCurve[eDimCount], 
                               KFbxAnimCurveDef::EInterpolationType* pInterp, 
                               KFbxAnimCurveDef::ETangentMode* pTangeant);

    void SetDestFCurveTangeant(KFbxAnimCurve* pCurve [eDimCount], 
                               int pIndex, 
                               KFbxAnimCurveDef::ETangentMode pTangentMode[eDimCount], 
                               KFbxVector4 pKeyValue, 
                               KFbxVector4 pNextKeyValue);

    Cell* mSource;
    Cell* mDest;

    KTime mResamplingPeriod;
    bool mResamplingOnFrameRateMultiple;

    bool mApplyUnroll;
    bool mApplyConstantKeyReducer;

    // PP : So that the concatenation of matrices takes into account the rotation order
    KMBRotationOrder* mSrcRotationOrder;
    KMBRotationOrder* mDestRotationOrder;

    // Set last key exactly at end time or a frame period later.	
    bool mGenerateLastKeyExactlyAtEndTime;

    // Translation re-sampling flag.
    bool mResampleTranslation;

    // Force Apply
    bool mForceApply;

    // internal usage
    KFbxAnimCurveNode* mRotationCurveNode;
#endif
};


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVEFILTER_H



