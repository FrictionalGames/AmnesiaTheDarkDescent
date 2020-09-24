/*!  \file kfbxkfcurvefilters.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXKFCURVEFILTERS_H
#define FBXFILESDK_KFBXPLUGINS_KFBXKFCURVEFILTERS_H

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

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/components/kbaselib/klib/ktime.h>
#include <fbxfilesdk/components/kbaselib/klib/kerror.h>
#include <fbxfilesdk/kfbxmath/kfbxxmatrix.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxAnimStack;
class KFbxAnimCurve;
class KFbxAnimCurveNode;
class KFCurve;
class KFCurveNode;
class KFCurveFilterKeyReducer;
class KFCurveFilterConstantKeyReducer;
class KFCurveFilterMatrixConverter;
class KFCurveFilterResample;
class KFCurveFilterUnroll;
class KFCurveFilterGimbleKiller;
class KFCurveFilterTimeShiftAndScale;
class KFCurveFilterKeySync;
class KFCurveFilterScale;


/** Base class for animation curve filters.
* Animation curves can be modified through filters. The filters act on 
* the curve keys and values. They can move keys, add or remove keys,
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
* For example, an unroll filter acts on 3 rotation curves (X, Y and Z) at the same time.
*
* A filter has a start time (that can be as low as TC_MINFINITY) and a stop time (that can be as high as TC_INFINITY).
* The filter is only applied to the parts of the animation curves that are between the
* start and stop time.
* 
* \nosubgrouping
* \remarks This is now a deprecated class and should be replaced with its KFbxAnimCurveFilter equivalent.
*/
class KFBX_DLL KFbxKFCurveFilter : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxKFCurveFilter,KFbxObject);

public:
	/**
	 * \name Member functions
	 */
	//@{
    /** Get the name of the filter.
    * \return     Pointer to name.
    */
    virtual const char* GetName() const {return NULL;}

    /** Get the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
    * \return     The time expressed as KTime.
    */
    virtual KTime& GetStartTime() {return mTime;}

    /** Set the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
    * \param pTime     The time to be set.
    */
    virtual void SetStartTime(KTime& pTime){return;}

    /** Get the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
    * \return     The time expressed as KTime.
    */
    virtual KTime& GetStopTime(){return mTime;}

    /** Set the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
    * \param pTime     The time to be set.
    */
    virtual void SetStopTime(KTime& pTime){return ;}

    /** Get the index of start key on the given curve. This is the index of the first key
	* after (or on) the filter's start time.
    * \param pCurve     Curve on which we want to retrieve the start key.
    * \return           Index of the start key.
    */
    virtual int GetStartKey(KFbxAnimCurve& pCurve) const {return 0;}

    /** Get the index of stop key on the given curve. This is the index of the last key
	* before (or on) the filter's stop time.
    * \param pCurve     Curve on which we want to retrieve the stop key.
    * \return           The position of the stop key.
    */
    virtual int GetStopKey(KFbxAnimCurve& pCurve) const {return 0;}

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
    */
    virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode) {return false;}

    /** Check if the given animation curve need an application of the filter.
    * \param pCurve     Array of curves to test if they need the and application of the filter.
    * \param pCount     Number of curves in array.
    * \return           \c true if at least one animation curve in the array needs an application of the filter.
    */
    virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount){return false;}

    /** Check if an animation curve need an application of the filter.
    * \param pCurve     Curve to test if it needs application of filter.
    * \return           \c true if the animation curve needs an application of the filter.
    */
    virtual bool NeedApply(KFbxAnimCurve& pCurve){return false;}

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
    */
    virtual bool Apply(KFbxAnimCurveNode& pCurveNode){return false;}

    /** Apply filter on an array of animation curves.
    * \param pCurve     Array of curves to which the filter is applied.
    * \param pCount     Number of curves in the array.
    * \return           \c true if the curve filtering operation was successful, \c false otherwise.
    */
    virtual bool Apply(KFbxAnimCurve** pCurve, int pCount){return false;}

    /** Apply filter on an animation curve.
    * \param pCurve         Curve to which the filter is applied.
    * \return               \c true if the curve filtering operation was successful, \c false otherwise.
    */
    virtual bool Apply(KFbxAnimCurve& pCurve){return false;}

    /** Reset the filter to its default parameters.
    */
    virtual void Reset(){return ;}

    /** Retrieve error object.
    * \return     Error object.
    */
    virtual KError* GetError() {return NULL;}

    /** Get last error ID.
    * \return     Last error ID.
    */
    virtual int GetLastErrorID(){return -1;}

    /** Get last error string.
    * \return     Last error string.
    */
    virtual char* GetLastErrorString(){return NULL;}
	//@}
	/**
	 * \name Deprecated functions
	 */
	//@{

    /** \deprecated. Use GetStartKey(KFbxAnimCurve&) instead.
    * \param pCurve Curve on which we want to retrieve the start key.
    */
    virtual int GetStartKey(KFCurve& pCurve) const {return 0;}

    /** \deprecated. Use GetStopKey(KFbxAnimCurve&) instead.
    * \param pCurve Curve on which we want to retrieve the stop key.
    */
    virtual int GetStopKey(KFCurve& pCurve) const {return 0;}

    /** \deprecated. Use NeedApply(KFbxAnimCurveNode&) instead.
    * \param pCurveNode Curve node to test if it needs application of filter.
    * \param pRecursive Apply recursively to all curve node children.
    */
    virtual bool NeedApply(KFCurveNode& pCurveNode, bool pRecursive = true){return false;}

    /** \deprecated. Use NeedApply(KFbxAnimCurve**)
    * \param pCurve Array of curves to test if they need the and application of the filter.
    * \param pCount Number of curves in array.
    */
    virtual bool NeedApply(KFCurve** pCurve, int pCount){return false;}

    /** \deprecated. Use NeedApply(KFbxAnimCurve&)
    * \param pCurve Curve to test if it needs application of filter.
    */
    virtual bool NeedApply(KFCurve& pCurve){return false;}

    /** \deprecated. Use Apply(KFbxAnimCurveNode&)
    * \param pCurveNode Curve node to which the filter is applied.
    * \param pRecursive Apply recursively to all curve node children.
    */
    virtual bool Apply(KFCurveNode& pCurveNode, bool pRecursive = true){return false;}

    /** \deprecated. Use Apply(KFbxAnimCurve**)
    * \param pCurve  Array of curves to which the filter is applied.
    * \param pCount Number of curves in the array.
    */
    virtual bool Apply(KFCurve** pCurve, int pCount){return false;}

    /** \deprecated. Use Apply(KFbxAnimCurve&)
    * \param pCurve Curve to which the filter is applied.
    */
    virtual bool Apply(KFCurve& pCurve){return false;}
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
    KFbxKFCurveFilter(KFbxSdkManager& pManager, char const* pName);
    void GetKFCurvesFromAnimCurve(KFbxAnimCurve** pSrc, int pSrcCount, KFCurve** pDst, int& pDstCount);

    KTime mTime;
#endif
};


/** Key reducing filter.
  * \nosubgrouping
  * Filter to test if each key is really necessary to define the curve
  * at a definite degree of precision. It filters recursively from the
  * strongest difference first. All useless keys are eliminated.
  * \remarks This is now a deprecated class and should be replaced with its KFbxAnimCurveFilter equivalent.
  */
class KFBX_DLL KFbxKFCurveFilterKeyReducer : public KFbxKFCurveFilter
{
    KFBXOBJECT_DECLARE(KFbxKFCurveFilterKeyReducer,KFbxKFCurveFilter);

public:

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    K_DEPRECATED virtual const char* GetName() const;

    /** Get the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStartTime();
    /** Set the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStartTime(KTime& pTime);
    /** Get the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStopTime();

    /** Set the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStopTime(KTime& pTime);

    /** Get the index of start key on the given curve. This is the index of the first key
	* after the filter's start time.
      * \param pCurve     Curve on which we want to retrieve the start key.
      * \return           Index of the start key.
      */
    K_DEPRECATED virtual int GetStartKey(KFbxAnimCurve& pCurve) const;

    /** Get the index of stop key on the given curve. This is the index of the last key
	* before the filter's stop time.
      * \param pCurve     Curve on which we want to retrieve the stop key
      * \return           The position of the stop key
      */
    K_DEPRECATED virtual int GetStopKey(KFbxAnimCurve& pCurve) const;

    /** Check if any curve on the animation stack needs an application of the filter.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if at least one animated property needs an application of the filter.
    */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return ParentClass::NeedApply(pAnimStack); }

    /** Check if the animation curve node needs an application of the filter.
      * \param pCurveNode     Curves to test if they needs application of filter
      * \return               \c true if the animation curve node needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode);

    /** Check if the given animation curve need an application of the filter.
      * \param pCurve         Array of curves to test if they need the and application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

    /** Check if an animation curve need an application of the filter.
      * \param pCurve         Curve to test if it needs application of filter.
      * \return               \c true if the animation curve needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve& pCurve);

    /** Retrieve error object.
      * \return     Error object.
      */
    K_DEPRECATED virtual KError* GetError();
    /** Get last error ID.
      * \return     Last error ID.
      */
    K_DEPRECATED virtual int GetLastErrorID() const;
    /** Get last error string.
      * \return     Last error string.
      */
    K_DEPRECATED virtual const char* GetLastErrorString() const;

    /** Apply filter to all the curves stored in the animation stack.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if the curve filtering operation was successful, \c false otherwise.
    */
    K_DEPRECATED virtual bool Apply(KFbxAnimStack* pAnimStack) { return ParentClass::Apply(pAnimStack); }

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    K_DEPRECATED virtual void Reset();

	//!	Get precision.
	K_DEPRECATED double GetPrecision() const;

	/**	Set precision.
	  * \param pPrecision The precision to set.
	  */
	K_DEPRECATED void SetPrecision(double pPrecision);

	//!	Return \c true key sync is applied at the end.
	K_DEPRECATED bool GetKeySync() const;

	/**	Set to \c true to apply key sync at the end.
	  * \param pKeySync Key sync flag.
	  */
	K_DEPRECATED void SetKeySync(bool pKeySync);

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
    //! Constructor.
    KFbxKFCurveFilterKeyReducer(KFbxSdkManager& pManager, char const* pName);
    KFCurveFilterKeyReducer *mDataCurveFilter;
    virtual void Destruct(bool pRecursive, bool pDependents);

#endif
};



/** Constant Key reducing filter.
  * \nosubgrouping
  * Filter to test if each key is really necessary to define the curve
  * at a definite degree of precision. It filters recursively from the
  * strongest difference first. All useless keys are eliminated.
  * \remarks This is now a deprecated class and should be replaced with its KFbxAnimCurveFilter equivalent.
  */
class KFBX_DLL KFbxKFCurveFilterConstantKeyReducer : public KFbxKFCurveFilter
{
    KFBXOBJECT_DECLARE(KFbxKFCurveFilterConstantKeyReducer,KFbxKFCurveFilter);

public:

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    K_DEPRECATED virtual const char* GetName() const;

    /** Get the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStartTime();
    /** Set the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStartTime(KTime& pTime);
    /** Get the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStopTime();

    /** Set the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStopTime(KTime& pTime);

    /** Get the index of start key on the given curve. This is the index of the first key
	* after the filter's start time.
      * \param pCurve     Curve on which we want to retrieve the start key.
      * \return           Index of the start key.
      */
    K_DEPRECATED virtual int GetStartKey(KFbxAnimCurve& pCurve) const;

    /** Get the index of stop key on the given curve. This is the index of the last key
	* before the filter's stop time.
      * \param pCurve     Curve on which we want to retrieve the stop key
      * \return           The position of the stop key
      */
    K_DEPRECATED virtual int GetStopKey(KFbxAnimCurve& pCurve) const;

    /** Check if any curve on the animation stack needs an application of the filter.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if at least one animated property needs an application of the filter.
    */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return ParentClass::NeedApply(pAnimStack); }

    /** Check if the animation curve node needs an application of the filter.
      * \param pCurveNode     Curves to test if they needs application of filter
      * \return               \c true if the animation curve node needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode);

    /** Check if the given animation curve need an application of the filter.
      * \param pCurve         Array of curves to test if they need the and application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

    /** Check if an animation curve need an application of the filter.
      * \param pCurve         Curve to test if it needs application of filter.
      * \return               \c true if the animation curve needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve& pCurve);

    /** Retrieve error object.
      * \return     Error object.
      */
    K_DEPRECATED virtual KError* GetError();
    /** Get last error ID.
      * \return     Last error ID.
      */
    K_DEPRECATED virtual int GetLastErrorID() const;
    /** Get last error string.
      * \return     Last error string.
      */
    K_DEPRECATED virtual const char* GetLastErrorString() const;

    /** Apply filter to all the curves stored in the animation stack.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if the curve filtering operation was successful, \c false otherwise.
    */
    K_DEPRECATED virtual bool Apply(KFbxAnimStack* pAnimStack) { return ParentClass::Apply(pAnimStack); }

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    K_DEPRECATED virtual void Reset();

    /** Get the derivative tolerance.
      * \return     The value of the derivative tolerance.
      */
    K_DEPRECATED double GetDerivativeTolerance() const;

    /** Set the derivative tolerance.
      * \param pValue     Value derivative tolerance.
      */
    K_DEPRECATED void SetDerivativeTolerance(double pValue);

    /** Get the tolerance value.
      * \return     The tolerance value.
      */
    K_DEPRECATED double GetValueTolerance() const;

    /** Set the tolerance value.
      * \param pValue     Tolerance value.
      */
    K_DEPRECATED void SetValueTolerance(double pValue);

    /** Get the state of the KeepFirstAndLastKeys flag.
      * \return      \c true if the filter keeps the first and last keys.
      */
    K_DEPRECATED bool GetKeepFirstAndLastKeys() const;

    /** Set the state of the KeepFirstAndLastKeys flag.
      * \param pKeepFirstAndLastKeys     Set to \c true if you want the filter to keep the first and last keys.
      */
    K_DEPRECATED void SetKeepFirstAndLastKeys( bool pKeepFirstAndLastKeys );

    /** Get the state of the KeepOneKey flag.
      * \return     \c true if the filter keeps one keys.
      */
    K_DEPRECATED bool GetKeepOneKey() const;

    /** Set the state of the KeepOneKey flag.
      * \param pKeepOneKey     Set to \c true if you want the filter to keep one key.
      */
    K_DEPRECATED void SetKeepOneKey( bool pKeepOneKey );

    /** Tell the filter keep cubic curve keys which are not pure auto
	  * \param pKeep KeepNotPureAutoKeys flag.
	  */
	K_DEPRECATED void SetKeepNotPureAutoKeys(bool pKeep);

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
    K_DEPRECATED void SetTranslationThreshold    ( double pTranslationThreshold );
    K_DEPRECATED void SetRotationThreshold       ( double pRotationThreshold );
    K_DEPRECATED void SetScalingThreshold        ( double pScalingThreshold );
    K_DEPRECATED void SetDefaultThreshold        ( double pDefaultThreshold );
protected:
    //! Constructor.
    KFbxKFCurveFilterConstantKeyReducer(KFbxSdkManager& pManager, char const* pName);
    KFCurveFilterConstantKeyReducer *mDataCurveFilter;
    virtual void Destruct(bool pRecursive, bool pDependents);

#endif
};




/** Matrix conversion filter.
  * \nosubgrouping
  * \remarks The current implementation of this filter expects to process 9 curves.
  * \remarks This is now a deprecated class and should be replaced with its KFbxAnimCurveFilter equivalent.
  */
class KFBX_DLL KFbxKFCurveFilterMatrixConverter : public KFbxKFCurveFilter
{
    KFBXOBJECT_DECLARE(KFbxKFCurveFilterMatrixConverter,KFbxKFCurveFilter);

public:

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    K_DEPRECATED virtual const char* GetName() const;

    /** Get the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStartTime();
    /** Set the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStartTime(KTime& pTime);
    /** Get the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStopTime();

    /** Set the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStopTime(KTime& pTime);

    /** Get the index of start key on the given curve. This is the index of the first key
	* after the filter's start time.
      * \param pCurve     Curve on which we want to retrieve the start key.
      * \return           Index of the start key.
      */
    K_DEPRECATED virtual int GetStartKey(KFbxAnimCurve& pCurve) const;

    /** Get the index of stop key on the given curve. This is the index of the last key
	* before the filter's stop time.
      * \param pCurve     Curve on which we want to retrieve the stop key
      * \return           The position of the stop key
      */
    K_DEPRECATED virtual int GetStopKey(KFbxAnimCurve& pCurve) const;

    /** This filter expects to works with interdependent curves. Passing the animation stack makes no sense
      * since this object would not know which curves to handle.
    * \param pAnimStack     Animation stack. 
    * \return               \c false 
    */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return false; }

    /** Check if the animation curve nodes need an application of the filter.
      * \param pCurveNode     Curves to test if they needs application of filter
      * \return               \c true if the animation curve node needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurveNode* pCurveNode[3]);

    /** Check if the given animation curves need an application of the filter.
      * \param pCurve         Array of curves to test if they need the and application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

    /** This filter expects to works with interdependent curves. Receiving one single curve is useless.
      * \return               \c false
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve& pCurve);

    /** Retrieve error object.
      * \return     Error object.
      */
    K_DEPRECATED virtual KError* GetError();
    /** Get last error ID.
      * \return     Last error ID.
      */
    K_DEPRECATED virtual int GetLastErrorID() const;
    /** Get last error string.
      * \return     Last error string.
      */
    K_DEPRECATED virtual const char* GetLastErrorString() const;

    /** This filter expects to works with interdependent curves. Passing the animation stack makes no sense
      * since this object would not know which curves to handle.
      * \param pAnimStack     Animation stack
      * \return               \c false.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimStack* pAnimStack) { return false; }

    /** Apply filter on all the curves of the animation curve nodes.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurveNode* pCurveNode[3]);

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** This filter expects to works with interdependent curves. Receiving one single curve is useless.
      * \return               \c false
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    K_DEPRECATED virtual void Reset();

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
    K_DEPRECATED void GetSourceMatrix(EMatrixID pIndex, KFbxXMatrix& pMatrix) const;

    /** Set the Translation Rotation Scaling source matrix.
      * \param pIndex      The matrix ID.
      * \param pMatrix     The matrix used to set the source matrix.
      */
    K_DEPRECATED void SetSourceMatrix(EMatrixID pIndex, KFbxXMatrix& pMatrix);

    /** Get the Translation Rotation Scaling destination matrix.
      * \param pIndex      The matrix ID.
      * \param pMatrix     The matrix used to receive the destination matrix.
      */
    K_DEPRECATED void GetDestMatrix(EMatrixID pIndex, KFbxXMatrix& pMatrix) const;

    /** Set the Translation Rotation Scaling destination matrix.
      * \param pIndex      The matrix ID.
      * \param pMatrix     The matrix used to set the destination matrix.
      */
    K_DEPRECATED void SetDestMatrix(EMatrixID pIndex, KFbxXMatrix& pMatrix);

    /** Get the resampling period.
      * \return     the resampling period.
      */
    K_DEPRECATED KTime GetResamplingPeriod () const;

    /** Set the resampling period.
      * \param pResamplingPeriod     The resampling period to be set.
      */
    K_DEPRECATED void SetResamplingPeriod (KTime& pResamplingPeriod);

    /** Check if the last key is exactly at the end time.
      * \return     \c true if last key is set exactly at end time.
      */
    K_DEPRECATED bool GetGenerateLastKeyExactlyAtEndTime() const;

    /** Set the last key to be is exactly at end time or not
      * \param pFlag     value to set if last key is set exactly at end time.
      */
    K_DEPRECATED void SetGenerateLastKeyExactlyAtEndTime(bool pFlag);

    /** Check if resampling is on frame rate multiple
      * \return     \c true if resampling is on a frame rate multiple.
      */
    K_DEPRECATED bool GetResamplingOnFrameRateMultiple() const;

    /** Set the resample on a frame rate multiple.
      * \param pFlag     The value to be set
      * \remarks         It might be necessary that the starting time of the converted
      *                  animation starts at an multiple of frame period starting from time 0.
      *                  Most softwares play their animation at a definite frame rate, starting
      *                  from time 0.  As resampling occurs when we can't guarantee interpolation,
      *                  keys must match with the moment when the curve is evaluated.
      */
    K_DEPRECATED void SetResamplingOnFrameRateMultiple(bool pFlag);

    /** Get if Apply Unroll is used
      * \return     \c true if unroll is applied.
      */
    K_DEPRECATED bool GetApplyUnroll() const;

    /** Set if Apply Unroll is used
      * \param pFlag     Value to set
      */
    K_DEPRECATED void SetApplyUnroll(bool pFlag);

    /** Get if constant key reducer is used
      * \return     \c true if constant key reducer is applied.
      */
    K_DEPRECATED bool GetApplyConstantKeyReducer() const;

    /** Set if constant key reducer is used
      * \param pFlag     value to set
      */
    K_DEPRECATED void SetApplyConstantKeyReducer(bool pFlag);

    /** Get if the resample translation is used
      * \return      \c true if translation data is resampled upon conversion.
      * \remarks     If this flag isn't set, translation data must be calculated
      *              after the conversion process, overriding the resampling process.
      */
    K_DEPRECATED bool GetResampleTranslation() const;

    /** Set the resample translation data.
      * \param pFlag     Value to be set.
      * \remarks         If this flag isn't set, translation data must be calculated
      *                  after the conversion process, overriding the resampling process.
      */
    K_DEPRECATED void SetResampleTranslation(bool pFlag);

    /** Set the rotation order of the source.
      * \param pOrder     The order to be set.
      */
    K_DEPRECATED void SetSrcRotateOrder(int pOrder);

    /** Set the rotation order of the destination.
      * \param pOrder     The order to be set.
      */
    K_DEPRECATED void SetDestRotateOrder(int pOrder);

    /** Set to force apply even if source and destination matrices are equivalent
      * \param pVal     If the forces apply is to be used
      */
    K_DEPRECATED void SetForceApply(bool pVal);

    /** Get if the force apply is used
      * \return     \c true if the force apply is used
      */
    K_DEPRECATED bool GetForceApply() const;
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
    //! Constructor.
    KFbxKFCurveFilterMatrixConverter(KFbxSdkManager& pManager, char const* pName);
    KFCurveFilterMatrixConverter *mDataCurveFilter;
    virtual void Destruct(bool pRecursive, bool pDependents);
#endif
};




/** Resampling filter.
* \nosubgrouping
* \remarks This is now a deprecated class and should be replaced with its KFbxAnimCurveFilter equivalent.
*/
class KFBX_DLL KFbxKFCurveFilterResample : public KFbxKFCurveFilter
{
    KFBXOBJECT_DECLARE(KFbxKFCurveFilterResample,KFbxKFCurveFilter);

public:
    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    K_DEPRECATED virtual const char* GetName() const;

    /** Get the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStartTime();
    /** Set the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStartTime(KTime& pTime);
    /** Get the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStopTime();

    /** Set the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStopTime(KTime& pTime);

    /** Get the index of start key on the given curve. This is the index of the first key
	* after the filter's start time.
      * \param pCurve     Curve on which we want to retrieve the start key.
      * \return           Index of the start key.
      */
    K_DEPRECATED virtual int GetStartKey(KFbxAnimCurve& pCurve) const;

    /** Get the index of stop key on the given curve. This is the index of the last key
	* before the filter's stop time.
      * \param pCurve     Curve on which we want to retrieve the stop key
      * \return           The position of the stop key
      */
    K_DEPRECATED virtual int GetStopKey(KFbxAnimCurve& pCurve) const;

    /** Check if any curve on the animation stack needs an application of the filter.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if at least one animated property needs an application of the filter.
    */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return ParentClass::NeedApply(pAnimStack); }

    /** Check if the animation curve node needs an application of the filter.
      * \param pCurveNode     Curves to test if they needs application of filter
      * \return               \c true if the animation curve node needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode);

    /** Check if the given animation curve needs an application of the filter.
      * \param pCurve         Array of curves to test if they need the and application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

    /** Check if an animation curve need an application of the filter.
      * \param pCurve         Curve to test if it needs application of filter.
      * \return               \c true if the animation curve needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve& pCurve);
    
    /** Retrieve error object.
      * \return     Error object.
      */
    K_DEPRECATED virtual KError* GetError();
    /** Get last error ID.
      * \return     Last error ID.
      */
    K_DEPRECATED virtual int GetLastErrorID() const;
    /** Get last error string.
      * \return     Last error string.
      */
    K_DEPRECATED virtual const char* GetLastErrorString() const;

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter to all the curves stored in the animation stack.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if the curve filtering operation was successful, \c false otherwise.
    */
    K_DEPRECATED virtual bool Apply(KFbxAnimStack* pAnimStack) { return ParentClass::Apply(pAnimStack); }

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    K_DEPRECATED virtual void Reset();

    /** Set if the keys are on frame
      * \param pKeysOnFrame     value if keys are set on frame multiples.
      */
    K_DEPRECATED void SetKeysOnFrame(bool pKeysOnFrame);

    /** Get if the keys are on frame
      * \return     Value if keys are on frame multiples.
      */
    K_DEPRECATED bool GetKeysOnFrame() const;

    /** Get the Resampling period
      * \return     The Resampling period
      */
    K_DEPRECATED KTime GetPeriodTime() const;

    /** Set the Resampling Period
      * \param pPeriod     The Resampling Period to be set
      */
    K_DEPRECATED void SetPeriodTime(KTime &pPeriod);


    /** Get the Intelligent Mode
      * \return     the Intelligent Mode
      */
    K_DEPRECATED bool  GetIntelligentMode() const;

    /** Set the Intelligent Mode
      * \param pIntelligent     the Intelligent Mode to be set
      */
    K_DEPRECATED void  SetIntelligentMode( bool pIntelligent );

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
    //! Constructor.
    KFbxKFCurveFilterResample(KFbxSdkManager& pManager, char const* pName);
    KFCurveFilterResample *mDataCurveFilter;
    virtual void Destruct(bool pRecursive, bool pDependents);
#endif

};

/**Unroll filter
  *\nosubgrouping
  * \remarks This is now a deprecated class and should be replaced with its KFbxAnimCurveFilter equivalent.
  * \remarks The current implementation of this filter expects to process 3 curves.
  */
class KFBX_DLL KFbxKFCurveFilterUnroll : public KFbxKFCurveFilter
{
    KFBXOBJECT_DECLARE(KFbxKFCurveFilterUnroll,KFbxKFCurveFilter);

public:

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    K_DEPRECATED virtual const char* GetName() const;

    /** Get the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStartTime();
    /** Set the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED void SetStartTime(KTime& pTime);
    /** Get the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStopTime();

    /** Set the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStopTime(KTime& pTime);

    /** Get the index of start key on the given curve. This is the index of the first key
	* after the filter's start time.
      * \param pCurve     Curve on which we want to retrieve the start key.
      * \return           Index of the start key.
      */
    K_DEPRECATED virtual int GetStartKey(KFbxAnimCurve& pCurve) const;
    
    /** Get the index of stop key on the given curve. This is the index of the last key
	* before the filter's stop time.
      * \param pCurve     Curve on which we want to retrieve the stop key
      * \return           The position of the stop key
      */
    K_DEPRECATED virtual int GetStopKey(KFbxAnimCurve& pCurve) const;
    
    /** This filter expects to works with 3 interdependent curves. Passing the animation stack makes no sense
      * since this object would not know which curves to handle.
      * \param pAnimStack     Animation stack
      * \return               \c false.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return false; };

    /** Check if the animation curve node needs an application of the filter.
      * \param pCurveNode     Curves to test if they needs application of filter
      * \return               \c true if the animation curve node needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode);

    /** Check if the given animation curve need an application of the filter.
      * \param pCurve         Array of curves to test if they need the and application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

    /** This filter expects to works with interdependent curves. Receiving one single curve is useless.
      * \return               \c false
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve& pCurve);

    /** Retrieve error object.
      * \return     Error object.
      */
    K_DEPRECATED virtual KError* GetError();
    /** Get last error ID.
      * \return     Last error ID.
      */
    K_DEPRECATED virtual int GetLastErrorID() const;
    /** Get last error string.
      * \return     Last error string.
      */
    K_DEPRECATED virtual const char* GetLastErrorString() const;

    /** This filter expects to works with 3 interdependent curves. Passing the animation stack makes no sense
      * since this object would not know which curves to handle.
      * \param pAnimStack     Animation stack
      * \return               \c false.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimStack* pAnimStack) { return false; };

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** This filter expects to works with interdependent curves. Receiving one single curve is useless.
      * \return               \c false
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    K_DEPRECATED virtual void Reset();

    /** Get quality tolerance.
    * \return     The Quality Tolerance
    */
    K_DEPRECATED double GetQualityTolerance() const;

    /** Set quality tolerance.
      * \param pQualityTolerance     Value to be set.
      */
    K_DEPRECATED void SetQualityTolerance(double pQualityTolerance);

    /** Get if the test path is enabled
      * \return     \c true if test for path is enabled.
      */
    K_DEPRECATED bool GetTestForPath() const;

    /** Set if the test path is enabled
      * \param pTestForPath     Value to set if test for path is to be enabled.
      */
    K_DEPRECATED void SetTestForPath(bool pTestForPath);

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
    void SetRotationOrder(int pOrder);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    //! Constructor.
    KFbxKFCurveFilterUnroll(KFbxSdkManager& pManager, char const* pName);
    KFCurveFilterUnroll *mDataCurveFilter;
    virtual void Destruct(bool pRecursive, bool pDependents);
#endif
};


/**GimbleKiller filter
  *\nosubgrouping
  * \remarks This is now a deprecated class and should be replaced with its KFbxAnimCurveFilter equivalent.
  */
class KFBX_DLL KFbxKFCurveFilterGimbleKiller : public KFbxKFCurveFilter
{
    KFBXOBJECT_DECLARE(KFbxKFCurveFilterGimbleKiller,KFbxKFCurveFilter);

public:

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    K_DEPRECATED virtual const char* GetName() const;

    /** Get the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStartTime();
    /** Set the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStartTime(KTime& pTime);
    /** Get the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStopTime();

    /** Set the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStopTime(KTime& pTime);

    /** Get the index of start key on the given curve. This is the index of the first key
	* after the filter's start time.
      * \param pCurve     Curve on which we want to retrieve the start key.
      * \return           Index of the start key.
      */
    K_DEPRECATED virtual int GetStartKey(KFbxAnimCurve& pCurve) const;

    /** Get the index of stop key on the given curve. This is the index of the last key
	* before the filter's stop time.
      * \param pCurve     Curve on which we want to retrieve the stop key
      * \return           The position of the stop key
      */
    K_DEPRECATED virtual int GetStopKey(KFbxAnimCurve& pCurve) const;

    /** This filter expects to works with 3 interdependent curves. Passing the animation stack makes no sense
      * since this object would not know which curves to handle.
      * \param pAnimStack     Animation stack
      * \return               \c false.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return false; }

    /** Check if the animation curve node needs an application of the filter.
      * \param pCurveNode     Curves to test if they needs application of filter
      * \return               \c true if the animation curve node needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode);

    /** Check if the given animation curve need an application of the filter.
      * \param pCurve         Array of curves to test if they need the and application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

     /** This filter expects to works with interdependent curves. Receiving one single curve is useless.
      * \return               \c false
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve& pCurve);
    
    /** Retrieve error object.
      * \return     Error object.
      */
    K_DEPRECATED virtual KError* GetError();
    /** Get last error ID.
      * \return     Last error ID.
      */
    K_DEPRECATED virtual int GetLastErrorID() const;
    /** Get last error string.
      * \return     Last error string.
      */
    K_DEPRECATED virtual const char* GetLastErrorString() const;

    /** This filter expects to works with 3 interdependent curves. Passing the animation stack makes no sense
      * since this object would not know which curves to handle.
      * \param pAnimStack     Animation stack
      * \return               \c false.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimStack* pAnimStack) { return false; }

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** This filter expects to works with interdependent curves. Receiving one single curve is useless.
      * \return               \c false
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    K_DEPRECATED virtual void Reset();

	//! Return \c true if key sync filter is enabled.
	K_DEPRECATED bool GetApplyKeySyncFilter() const;

	/** Set to \c true to enable key sync filter.
	  * \param pFlag Key sync filter flag.
	  */
	K_DEPRECATED void SetApplyKeySyncFilter(bool pFlag);

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
    //! Constructor.
    KFbxKFCurveFilterGimbleKiller(KFbxSdkManager& pManager, char const* pName);
    KFCurveFilterGimbleKiller *mDataCurveFilter;
    virtual void Destruct(bool pRecursive, bool pDependents);
#endif
};


/**Time shift and scale filter
  *\nosubgrouping
  * \remarks This is now a deprecated class and should be replaced with its KFbxAnimCurveFilter equivalent.
  */
class KFBX_DLL KFbxKFCurveFilterTSS : public KFbxKFCurveFilter
{
    KFBXOBJECT_DECLARE(KFbxKFCurveFilterTSS,KFbxKFCurveFilter);

public:

    /** Get the name of the filter.
      * \return     Pointer to name.
      */
    K_DEPRECATED virtual const char* GetName() const;

    /** Get the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStartTime();
    /** Set the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStartTime(KTime& pTime);
    /** Get the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStopTime();

    /** Set the stopping time.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStopTime(KTime& pTime);

    /** Get the index of start key on the given curve. This is the index of the first key
	* after the filter's start time.
      * \param pCurve     Curve on which we want to retrieve the start key.
      * \return           Index of the start key.
      */
    K_DEPRECATED virtual int GetStartKey(KFbxAnimCurve& pCurve) const;

    /** Get the index of stop key on the given curve. This is the index of the last key
	* before the filter's stop time.
      * \param pCurve     Curve on which we want to retrieve the stop key
      * \return           The position of the stop key
      */
    K_DEPRECATED virtual int GetStopKey(KFbxAnimCurve& pCurve) const;

    /** Check if any curve on the animation stack needs an application of the filter.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if at least one animated property needs an application of the filter.
    */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return ParentClass::NeedApply(pAnimStack); }

    /** Check if the animation curve node needs an application of the filter.
      * \param pCurveNode     Curves to test if they needs application of filter
      * \return               \c true if the animation curve node needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode);

    /** Check if the given animation curve need an application of the filter.
      * \param pCurve         Array of curves to test if they need the and application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

    /** Check if an animation curve need an application of the filter.
      * \param pCurve         Curve to test if it needs application of filter.
      * \return               \c true if the animation curve needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve& pCurve);

    /** Retrieve error object.
      * \return     Error object.
      */
    K_DEPRECATED virtual KError* GetError();
    /** Get last error ID.
      * \return     Last error ID.
      */
    K_DEPRECATED virtual int GetLastErrorID() const;
    /** Get last error string.
      * \return     Last error string.
      */
    K_DEPRECATED virtual const char* GetLastErrorString() const;

    /** Apply filter to all the curves stored in the animation stack.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if the curve filtering operation was successful, \c false otherwise.
    */
    K_DEPRECATED virtual bool Apply(KFbxAnimStack* pAnimStack) { return ParentClass::Apply(pAnimStack); }

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    K_DEPRECATED virtual void Reset();

    /** Get time shift value.
    * \return     The time value used for the shift.
    */
	K_DEPRECATED KTime& GetShift() const;

    /** Set shift.
      * \param pShift     The time value used for the shift.
      */
	K_DEPRECATED void SetShift(KTime& pShift);

    /** Get scale.
    * \return     The Scale Value.
    */
	K_DEPRECATED double GetScale() const;

    /** Set scale.
      * \param pScale     Value to be set.
      */
	K_DEPRECATED void SetScale(double pScale);

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
    //! Constructor.
    KFbxKFCurveFilterTSS(KFbxSdkManager& pManager, char const* pName);
    KFCurveFilterTimeShiftAndScale *mDataCurveFilter;
    virtual void Destruct(bool pRecursive, bool pDependents);
#endif
};

/**	Key sync filter.
  * \nosubgrouping
  * Filter to synchronize the keys of a set of function curves.
  * \remarks This is now a deprecated class and should be replaced with its KFbxAnimCurveFilter equivalent.
*/
class KFBX_DLL KFbxKFCurveFilterKeySync : public KFbxKFCurveFilter
{
    KFBXOBJECT_DECLARE(KFbxKFCurveFilterKeySync,KFbxKFCurveFilter);

public:
   /** Get the name of the filter.
      * \return     Pointer to name.
      */
    K_DEPRECATED virtual const char* GetName() const;

    /** Get the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStartTime();
    /** Set the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStartTime(KTime& pTime);
    /** Get the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStopTime();

    /** Set the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStopTime(KTime& pTime);

    /** Get the index of start key on the given curve. This is the index of the first key
	* after the filter's start time.
      * \param pCurve     Curve on which we want to retrieve the start key.
      * \return           Index of the start key.
      */
    K_DEPRECATED virtual int GetStartKey(KFbxAnimCurve& pCurve) const;

    /** Get the index of stop key on the given curve. This is the index of the last key
	* before the filter's stop time.
      * \param pCurve     Curve on which we want to retrieve the stop key
      * \return           The position of the stop key
      */
    K_DEPRECATED virtual int GetStopKey(KFbxAnimCurve& pCurve) const;

    /** Check if any curve on the animation stack needs an application of the filter.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if at least one animated property needs an application of the filter.
    */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return ParentClass::NeedApply(pAnimStack); }

    /** Check if the animation curve node needs an application of the filter.
      * \param pCurveNode     Curves to test if they needs application of filter
      * \return               \c true if the animation curve node needs an application of the filter.
      * \remarks              The current implementation of this filter limits the number of animation curves on
      *                       the animation curve node to 20.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode);

    /** Check if the given animation curve need an application of the filter.
      * \param pCurve         Array of curves to test if they need the and application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

    /** Check if an animation curve need an application of the filter.
      * \param pCurve         Curve to test if it needs application of filter.
      * \return               \c true if the animation curve needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve& pCurve);

    /** Retrieve error object.
      * \return     Error object.
      */
    K_DEPRECATED virtual KError* GetError();
    /** Get last error ID.
      * \return     Last error ID.
      */
    K_DEPRECATED virtual int GetLastErrorID() const;
    /** Get last error string.
      * \return     Last error string.
      */
    K_DEPRECATED virtual const char* GetLastErrorString() const;

    /** Apply filter to all the curves stored in the animation stack.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if the curve filtering operation was successful, \c false otherwise.
    */
    K_DEPRECATED virtual bool Apply(KFbxAnimStack* pAnimStack) { return ParentClass::Apply(pAnimStack); }

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      * \remarks              The current implementation of this filter limits the number of animation curves on
      *                       the animation curve node to 20.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    K_DEPRECATED virtual void Reset();

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
    //! Constructor.
    KFbxKFCurveFilterKeySync(KFbxSdkManager& pManager, char const* pName);
    KFCurveFilterKeySync *mDataCurveFilter;
    virtual void Destruct(bool pRecursive, bool pDependents);

#endif
};

/**	Key scale filter.
  * \nosubgrouping
  * Filter to scale the keys of a set of function curves.
  * \remarks This is now a deprecated class and should be replaced with its KFbxAnimCurveFilter equivalent.
*/
class KFBX_DLL KFbxKFCurveFilterScale : public KFbxKFCurveFilter
{
    KFBXOBJECT_DECLARE(KFbxKFCurveFilterScale,KFbxKFCurveFilter);

public:
   /** Get the name of the filter.
      * \return     Pointer to name.
      */
    K_DEPRECATED virtual const char* GetName() const;

    /** Get the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStartTime();
    /** Set the start time for the application of the filter.
	  * The part of the animation curves before the start time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStartTime(KTime& pTime);
    /** Get the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \return     The time expressed as KTime.
      */
    K_DEPRECATED virtual KTime& GetStopTime();

    /** Set the stop time for the application of the filter.
	  * The part of the animation curves after the stop time will remain untouched.
      * \param pTime     The time to be set.
      */
    K_DEPRECATED virtual void SetStopTime(KTime& pTime);

    /** Get the index of start key on the given curve. This is the index of the first key
	* after the filter's start time.
      * \param pCurve     Curve on which we want to retrieve the start key.
      * \return           Index of the start key.
      */
    K_DEPRECATED virtual int GetStartKey(KFbxAnimCurve& pCurve) const;

    /** Get the index of stop key on the given curve. This is the index of the last key
	* before the filter's stop time.
      * \param pCurve     Curve on which we want to retrieve the stop key
      * \return           The position of the stop key
      */
    K_DEPRECATED virtual int GetStopKey(KFbxAnimCurve& pCurve) const;

    /** Check if any curve on the animation stack needs an application of the filter.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if at least one animated property needs an application of the filter.
    */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimStack* pAnimStack) { return ParentClass::NeedApply(pAnimStack); }

    /** Check if the animation curve node needs an application of the filter.
      * \param pCurveNode     Curves to test if they needs application of filter
      * \return               \c true if the animation curve node needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurveNode& pCurveNode);

    /** Check if the given animation curve need an application of the filter.
      * \param pCurve         Array of curves to test if they need the and application of the filter.
      * \param pCount         Number of curves in array.
      * \return               \c true if at least one animation curve in the array needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve** pCurve, int pCount);

    /** Check if an animation curve need an application of the filter.
      * \param pCurve         Curve to test if it needs application of filter.
      * \return               \c true if the animation curve needs an application of the filter.
      */
    K_DEPRECATED virtual bool NeedApply(KFbxAnimCurve& pCurve);

    /** Retrieve error object.
      * \return     Error object.
      */
    K_DEPRECATED virtual KError* GetError();
    /** Get last error ID.
      * \return     Last error ID.
      */
    K_DEPRECATED virtual int GetLastErrorID() const;
    /** Get last error string.
      * \return     Last error string.
      */
    K_DEPRECATED virtual const char* GetLastErrorString() const;

    /** Apply filter to all the curves stored in the animation stack.
    * \param pAnimStack     Animation stack where to retrieve the animation curves
    * \return               \c true if the curve filtering operation was successful, \c false otherwise.
    */
    K_DEPRECATED virtual bool Apply(KFbxAnimStack* pAnimStack) { return ParentClass::Apply(pAnimStack); }

    /** Apply filter on all the curves of an animation curve node.
      * \param pCurveNode     Curve node to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurveNode& pCurveNode);

    /** Apply filter on the given animation curve.
      * \param pCurve         Array of curve to which the filter is applied.
      * \param pCount         Number of curves in array.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve** pCurve, int pCount);

    /** Apply filter on an animation curve.
      * \param pCurve         Curve to which the filter is applied.
      * \return               \c true if the curve filtering operation was successful, \c false otherwise.
      */
    K_DEPRECATED virtual bool Apply(KFbxAnimCurve& pCurve);

    /** Reset the filter to its default parameters.
      */
    K_DEPRECATED virtual void Reset();

    /** GetScale.
	  *	\return the current scale factor.
	  */
	K_DEPRECATED float GetScale() const;

	/** SetScale.
      * \param pScale The new scale factor.
	  *	\return
	  */
	K_DEPRECATED void SetScale(float pScale);

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
    //! Constructor.
    KFbxKFCurveFilterScale(KFbxSdkManager& pManager, char const* pName);
    KFCurveFilterScale *mDataCurveFilter;
    virtual void Destruct(bool pRecursive, bool pDependents);

#endif
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXKFCURVEFILTERS_H



