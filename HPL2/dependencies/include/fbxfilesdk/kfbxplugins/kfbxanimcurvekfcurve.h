/****************************************************************************************

Copyright (C) 2010 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxanimcurvekfcurve.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVEKFCURVE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVEKFCURVE_H

#include <fbxfilesdk/components/kfcurve/kfcurve.h>
#include <fbxfilesdk/kfbxplugins/kfbxanimcurve.h>
#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KSet;

/** This class implements the interface KFbxAnimCurveKeyImpl for FBX animation curve keys.
  * \nosubgrouping
  *
  * \remarks Users should not use this class directly, but always use KFbxAnimCurveKey. 
  * This class encapsulates the legacy KFCurveKey structure for backward compatibility.
  * 
  */
class KFBX_DLL KFbxAnimCurveKFCurveKey : public KFbxAnimCurveKeyImpl
{
private:
    KFCurveKey mKey;

public:
	/**
	 * \name Constructors, Destructors, Copy Constructors, and other base functions.
	**/

	//@{

	/** Get a new instance of KFbxAnimCurveKFCurveKey.
	 * \return Pointer to new instance of KFbxAnimCurveKFCurveKey.
	 */
	static KFbxAnimCurveKeyImpl* AllocatorFct()
	{
		return FbxSdkNew<KFbxAnimCurveKFCurveKey>();
	}

	/** Get a new instance of KFbxAnimCurveKFCurveKey, copied from the given instance.
	 * \param pImpl Key to copy.
	 * \return Pointer to new instance of KFbxAnimCurveKFCurveKey.
	 */
	static KFbxAnimCurveKeyImpl* CopyAllocatorFct(KFbxAnimCurveKeyImpl* pImpl)
	{
		return FbxSdkNew<KFbxAnimCurveKFCurveKey>(*((KFbxAnimCurveKFCurveKey*)pImpl));
	}

	/** Delete a KFbxAnimCurveKFCurveKey.
	 * \param pImpl Key to delete.
	 */
	static void DeallocatorFct(KFbxAnimCurveKeyImpl* pImpl)
	{
		FbxSdkDelete<KFbxAnimCurveKFCurveKey>((KFbxAnimCurveKFCurveKey*)pImpl);
	}

    /** Default Constructor.
    */
	KFbxAnimCurveKFCurveKey() : KFbxAnimCurveKeyImpl()
    {
		KTime lTime = GetTime();
		SetTime(lTime);
        mKey.Set(lTime, 0.0);
    };

    /** Destructor.
    */
    virtual ~KFbxAnimCurveKFCurveKey()
    {
        // nothing specific to do!
    }

	/** Constructor with time. Value will be set to 0.0.
	 * \param pTime Time of the key.
	 */
	KFbxAnimCurveKFCurveKey(KTime pTime) : KFbxAnimCurveKeyImpl()
	{
		SetTime(pTime);
		mKey.Set(pTime, 0.0);
	}

	/** Constructor with time and value.
	* \param pTime Time of the key.
	* \param pVal Value of the key.
	*/
	KFbxAnimCurveKFCurveKey(KTime pTime, float pVal) : KFbxAnimCurveKeyImpl()
	{
		SetTime(pTime);
		mKey.Set(pTime, pVal);

	}

	/** Copy constructor.
      * \param pFKey Key instance (of type KFbxAnimCurveKFCurveKey) to be copied.
      */
    KFbxAnimCurveKFCurveKey(KFbxAnimCurveKFCurveKey const& pFKey) : KFbxAnimCurveKeyImpl()
    {
		mKey = pFKey.mKey;
        SetTime(mKey.GetTime());
    };

    /** Constructor from KFCurveKey.
      * \param pFKey Key instance (of type KFCurveKey) to be copied.
      */
    KFbxAnimCurveKFCurveKey(KFCurveKey const& pFKey) : KFbxAnimCurveKeyImpl()
    {
        mKey = pFKey;
        SetTime(mKey.GetTime());
    };

	/** Assignment operator.
	  * \param pFKey Key (of type KFbxAnimCurveKeyImpl) to be copied.
	  * \return Modified key.
	  */
    KFbxAnimCurveKeyImpl& operator=(const KFbxAnimCurveKeyImpl& pFKey)
	{
		*this = *((KFbxAnimCurveKFCurveKey*)&pFKey);
		return *this;
	}

	/** Assignment operator.
	* \param pFKey Key (of type KFbxAnimCurveKFCurveKey) to be copied.
	* \return Modified key.
	  */
	KFbxAnimCurveKFCurveKey& operator=(const KFbxAnimCurveKFCurveKey& pFKey)
	{
		SetTime(pFKey.GetTime());
		mKey = pFKey.mKey;
		return *this;
	}

    /** Cast operator to convert this to its base class instance.
      */
    operator KFCurveKey&()
    { 
        return mKey; 
    };

    /** Get base class pointer.
      * \return Key of base type KFCurveKey.
      */
    KFCurveKey* GetKFCurveKey()
    {
        return &mKey;
    };
	//@}

	/* \name Key time, values, data.
	*/
	//@{
    /** Get time value of the key.
    * \return Time at which the key is occurring.
    */
    virtual KTime GetTime() const
    {
        return mKey.GetTime();
    };

    /** Set time of key.
      * \param pTime New time of this key (time at which this key is occurring).
      */
    virtual void  SetTime(const KTime& pTime)
    {
        mKey.SetTime(pTime);
    };

    /** Set time and value of key.
      * \param pTime New time for this key.
      * \param pValue New value for this key.
      */
    void Set(KTime pTime, float pValue)
    {
        mKey.Set(pTime, pValue);
    }

    /**	Set a key with cubic interpolation, TCB tangent mode. 	
	* The key is modified according to the other parameters. 
	* The TCB mode controls the tension, continuity,
	* and bias of the curve.
    *	\param pTime	Key time.
    *	\param pValue	Key value.
    *	\param pData0	Tension. Controls the amount of curvature in the animation curve. The higher the tension is, the more linear
	* the curve looks. When the tension is low, the curve looks rounder or wider. 
    *	\param pData1	Continuity. Controls the smoothness or singularity of the curve on the key. 
    *	\param pData2	Bias. Controls if the effect of tension and continuity affect the curve before or after the key.
    */
	void SetTCB(KTime pTime, float pValue, float pData0 = 0.0f, float pData1 = 0.0f, float pData2 = 0.0f)
	{
		mKey.SetTCB(pTime, pValue, pData0, pData1, pData2);
	}

    /** Get key value.
	* \return Key value.
    */
    float GetValue() const
	{
		return mKey.GetValue();
	}

    /** Set key value.
    * \param pValue The value to set.
    */ 
	void SetValue(float pValue)
	{
		mKey.SetValue(pValue);
	}

    /** Get key's interpolation type.
    *   \return                  Interpolation type of the queried key.
    */
    KFbxAnimCurveDef::EInterpolationType GetInterpolation() const
	{
		return KFbxAnimCurveDef::EInterpolationType(mKey.GetInterpolation());
	}

    /** Set key's interpolation type.
    *	\param pInterpolation Key interpolation type.
    */
	void SetInterpolation (KFbxAnimCurveDef::EInterpolationType pInterpolation)
	{
		mKey.SetInterpolation(pInterpolation);
	}

    /** Get key's tangent mode.
    *   \param pIncludeOverrides Include override flags: Break, Clamp, Time-Independent.
    *	This method is meaningful for cubic interpolation only.
    *	Using this method for non cubic interpolated key will return unpredictable value.
    *	\return Key tangent mode.
    */
    KFbxAnimCurveDef::ETangentMode GetTangentMode(bool pIncludeOverrides = false) const
	{
		return KFbxAnimCurveDef::ETangentMode(mKey.GetTangeantMode(pIncludeOverrides));
	}

    /** Set key's tangent mode.
	*	This method is meaningful for cubic interpolation only.
	*	\param pTangentMode Key tangent mode.
    */
	void SetTangentMode (KFbxAnimCurveDef::ETangentMode pTangentMode)
	{
        bool lTI = (pTangentMode&KFbxAnimCurveDef::eTANGENT_GENERIC_TIME_INDEPENDENT)==KFbxAnimCurveDef::eTANGENT_GENERIC_TIME_INDEPENDENT;
		mKey.SetTangeantMode(pTangentMode, lTI==false);
	}

	/** Get key's tangent weight mode.
	*	This method is meaningful for cubic interpolation only.
	*/
	KFbxAnimCurveDef::EWeightedMode GetTangentWeightMode() const
	{
		return KFbxAnimCurveDef::EWeightedMode(mKey.GetTangeantWeightMode());
	}

	/** Set key's tangent weight mode as double value (cubic interpolation, non TCB tangent mode).
    *	This method is meaningful for cubic interpolation only.
	*	\param pTangentWeightMode	Weight mode.
	*	\param pMask				Used to select the affected tangents.
	*/
	void SetTangentWeightMode(KFbxAnimCurveDef::EWeightedMode pTangentWeightMode, KFbxAnimCurveDef::EWeightedMode pMask = KFbxAnimCurveDef::eWEIGHTED_ALL )
	{
		mKey.SetTangeantWeightMode(pTangentWeightMode, pMask);
	}


	/** Get key's tangent velocity mode.
    *	This method is meaningful for cubic interpolation only.
	*/
	KFbxAnimCurveDef::EVelocityMode GetTangentVelocityMode() const
	{
		return KFbxAnimCurveDef::EVelocityMode(mKey.GetTangeantVelocityMode());
	}

	/** Set key's tangent velocity mode as double value (cubic interpolation, non TCB tangent mode).
    *	This method is meaningful for cubic interpolation only.
    *	\param pTangentVelocityMode	Velocity mode. 
	*	\param pMask				Used to select the affected tangents
	*/
	void SetTangentVelocityMode(KFbxAnimCurveDef::EVelocityMode pTangentVelocityMode, KFbxAnimCurveDef::EVelocityMode pMask = KFbxAnimCurveDef::eVELOCITY_ALL )
	{
		mKey.SetTangeantVelocityMode(pTangentVelocityMode, pMask);
	}

	/** Get key's constant mode.
    *	\note This method is only relevant if the key's interpolation type is constant (eINTERPOLATION_CONSTANT).
    *	Using this method on a key with an other interpolation type will return unpredictable value.
	*	\return Key constant mode.
	*/
	KFbxAnimCurveDef::EConstantMode GetConstantMode() const
	{
		return KFbxAnimCurveDef::EConstantMode(mKey.GetConstantMode());
	}

	/** Set key's constant mode.
    *	This method is meaningful for constant interpolation only.
	  * \param pMode Constant mode to set.
	  */
	void SetConstantMode(KFbxAnimCurveDef::EConstantMode pMode)
	{
		mKey.SetConstantMode(pMode);
	}

	/** Get key data.
	  * If this is a TCB key, the data can be Tension, Continuity or Bias.
	  * If the key is weighted, the data is the weights values.
	  * If the key has velocity, the data is the velocity values.
	  * \param pIndex Data index, dependent on the key tangent mode.
	  * \return Data value.
	  */
	float GetDataFloat(KFbxAnimCurveDef::EKFbxAnimCurveDataIndex pIndex) const
	{
		// VR: The way KFCurveKey::GetDataFloat() and GetDataDouble() are implemented are different.
		// GetDataDouble() works for cases USER, AUTO, BREAK, etc, which have 2 or 4 float values to store.
		// GetDataFloat() works for the TCB case which has 3 float values (Tension, Continuity and Bias).
		// It is not my purpose to modify KFCurveKey at this point, so we use GetDataDouble in general,
		// and GetDataFloat in the case of a TCB.

		if (GetTangentMode() == KFbxAnimCurveDef::eTANGENT_TCB)
		{
			return mKey.GetDataFloat(EKFCurveDataIndex(pIndex));
		}

		return (float)mKey.GetDataDouble(EKFCurveDataIndex(pIndex));
	}

	/** Set key data.
	  * If this is a TCB key, the data can be Tension, Continuity or Bias.
	  * If the key is weighted, the data is the weights values.
	  * If the key has velocity, the data is the velocity values.
	  * \param pIndex Data index, dependent on the key tangent mode.
	  * \param pValue Data value to set.
	  */
	void SetDataFloat(KFbxAnimCurveDef::EKFbxAnimCurveDataIndex pIndex, float pValue)
	{
		// VR: The way KFCurveKey::SetDataFloat() and SetDataDouble() are implemented are different.
		// SetDataDouble() works for cases USER, AUTO, BREAK, etc, which have 2 or 4 float values to store.
		// SetDataFloat() works for the TCB case which has 3 float values (Tension, Continuity and Bias).
		// It is not my purpose to modivy KFCurveKey at this point, so we use SetDataDouble in general,
		// and SetDataFloat in the case of a TCB.

		if (GetTangentMode() == KFbxAnimCurveDef::eTANGENT_TCB)
		{
			mKey.SetDataFloat(EKFCurveDataIndex(pIndex), pValue);
		}

		mKey.SetDataDouble(EKFCurveDataIndex(pIndex), (kFCurveDouble)pValue);
	}

	/** Set tangent visibility mode. This would indicate what part of the tangent is visible in a graphical interface.
    *	This method is relevant for cubic interpolation only.
    *	\param pVisibility	Tangent visibility mode.
	*/
	void	SetTangentVisibility (KFbxAnimCurveDef::ETangentVisibility pVisibility)
	{
		mKey.SetTangeantVisibility(pVisibility);
	}

	/** Return tangent visibility mode.
    *	This method is relevant for cubic interpolation only.
    *	\return Tangent visibility mode.
	*/
	KFbxAnimCurveDef::ETangentVisibility GetTangentVisibility () const
	{
		return KFbxAnimCurveDef::ETangentVisibility(mKey.GetTangeantVisibility());
	}

	/** Set or unset the tangent break. When this flag is set (KFbxAnimCurveDef::eTANGENT_BREAK), the key's left and right slopes are independent.
	* When this flag is off, the key's left and right slope are equal.
    * This method is relevant for User (KFbxAnimCurveDef::eTANGENT_USER) and Auto (KFbxAnimCurveDef::eTANGENT_AUTO) tangent modes only.
    * \param pVal Break flag (\c true or \c false).
	*/
	void SetBreak(bool pVal)
	{
		mKey.SetBreak(pVal);
	}

	/** Get if the tangent has a break. When this flag is set (KFbxAnimCurveDef::eTANGENT_BREAK), the key's left and right slopes are independent.
	* When this flag is off, the key's left and right slope are equal.
    * This method is relevant for User (KFbxAnimCurveDef::eTANGENT_USER) and Auto (KFbxAnimCurveDef::eTANGENT_AUTO) tangent modes only.
	* \return Break flag (\c true or \c false).
	*/
	bool GetBreak() const
	{
		return mKey.GetBreak();
	}
	//@}
};


/** This class is an implementation for the FBX animation curves interface KFbxAnimCurve.
  * \nosubgrouping
  *
  * \remarks Users should not use this class directly, but always use KFbxAnimCurve. 
  * This class encapsulates the legacy KFCurve structure for backward compatibility.
  */
class KFBX_DLL KFbxAnimCurveKFCurve : public KFbxAnimCurve
{
    KFBXOBJECT_DECLARE(KFbxAnimCurveKFCurve, KFbxAnimCurve);

public:
		/**
		  * \name Key management.
		  *
		  */
		//@{

        /** Resize animation curve buffer to hold a certain number of keys.
		* \param pKeyCount Number of keys the animation curve will eventually hold.
        */
        void ResizeKeyBuffer(int pKeyCount);

        /** Call this function prior to adding, removing or editing keys of an animation curve.
		* Call function KbxAnimCurveKFCurve::KeyModifyEnd() after modification.
        */
        void KeyModifyBegin ();
        
        /** Call this function after adding, removing or editing keys of an animation curve.
	    * Function KbxAnimCurveKFCurve::KeyModifyBegin() must have been called prior to modify the keys.
        */
        void KeyModifyEnd ();

        /** Minimize use of buffer memory.
		* Use this method after a lot of changes have been made to keys, on an animation curve with a lot of keys.
		*/
        void KeyShrink();

        //! Remove all the keys of the animation curve and free buffer memory.
        virtual void KeyClear (); 
       
        /** Get the number of keys.
          * \return Key count.
          */
        virtual int KeyGetCount () const;
  
		/** Add a given key at given time. The new key is appended after all the other animation curve's keys.
		* Function KFbxAnimCurveKFCurve::KeyInsert() should be used instead if the key 
		* is to be added in the curve and not at the end. This function does not
		* respect the interpolation type and tangents of the neighboring keys. 
		* If there is already a key at the given time, the key is modified and no 
		* new key is added.
		* \param pTime Time of the new key.
		* \param pKey Key to add.
		* \param pLast Index of the last processed key to speed up search. If this function is called in a loop, 
		*              initialize this value to 0 and let it be updated by each call.
		* \return Index of the key at given time, no matter if it was added 
		*         or already present.
		* \remark Key value, interpolation type and tangent mode must be set 
		*  explicitly afterwards.
		*/
        int KeyAdd (KTime pTime, KFbxAnimCurveKeyBase& pKey, int* pLast = NULL); 

		/** Add a key at given time. The new key is appended after all the other animation curve's keys.
		* Function KFbxAnimCurveKFCurve::KeyInsert() should be used instead if the key 
		* is to be added in the curve and not at the end. This function does not
		* respect of the interpolation type and tangents of the neighboring keys. 
		* If there is already a key a the given time, no key is added.
		* \param pTime Time of the new key.
		* \param pLast Index of the last processed key to speed up search. If this function is called in a loop, 
		*              initialize this value to 0 and let it be updated by each call.
		* \return Index of the key at given time, no matter if it was added 
		*         or already present.
		* \remark Key value, interpolation type and tangent mode must be set 
		*  explicitly afterwards.
		*/
        int KeyAdd (KTime pTime, int* pLast = NULL) 
        {   
            KFbxAnimCurveKFCurveKey key(pTime);
            return KeyAdd(pTime, key, pLast); 
        }

		/** Set (or replace) key at given index with given key. 
		* \param pIndex Index of the key to be set or replaced.
		* \param pKey New key at this index.
		* \return \c true if key time is superior to previous key time
		*         and inferior to next key time, \c false otherwise.
		* \remark Result is undetermined if animation curve has no key or if index 
		* is out of bounds.
		*/
        bool KeySet(int pIndex,  KFbxAnimCurveKeyBase& pKey);
        
		/** Remove key at given index. Other key indices are updated automatically.
		* \param pIndex Index of key to remove.
		* \return \c true on success, \c false otherwise.
		*/
        bool KeyRemove(int pIndex);

        /** Remove all the keys in the given range.
          * \param pStartIndex Index of the first key to remove (inclusive).
          * \param pEndIndex Index of the last key to remove (inclusive).
		  * \return \c true on success, \c false otherwise.
          */
        bool KeyRemove(int pStartIndex, int pEndIndex);

        /** Insert a key at given time.
          * This function should be used instead of KFbxAnimCurveKFCurve::KeyAdd() if the key 
          * is to be added in the curve and not at the end. It inserts the key in 
          * respect to the interpolation type and tangents of the neighboring keys. 
          * If there is already a key a the given time, the key is modified and no 
          * new key is added.
          *
          * \param pTime Time of the new key.
          * \param pLast Index of the last processed key to speed up search. If this function is called in a loop, 
          *              initialize this value to 0 and let it be updated by each call.
          * \return Index of the key at given time, no matter if it was inserted 
          *         or already present.
          * \remark Key value must be set explicitly afterwards. 
		  *         The interpolation type and tangent mode are copied from the previous key.
          */
        int KeyInsert ( KTime pTime, int* pLast = NULL );
        
        /** Find key index for a given time.
          * \param pTime Time of the key looked for.
          * \param pLast Index of the last processed key to speed up search. If this function is called in a loop, 
          *              initialize this value to 0 and let it be updated by each call.
          * \return Key index. The integer part of the key index gives the 
		  * index of the closest key with a smaller time. The decimals give
		  * the relative position of given time compared to previous and next
		  * key times. Returns -1 if animation curve has no key.
		  *
		  * For example (using seconds for clarity), if there is a key at time 10s with index 5, and a key at
		  * time 11s with index 6, KeyFind(10.3s) would return 5.3.
          */
        double KeyFind (KTime pTime, int* pLast = NULL);

        /** Scale value of all keys.
		*	\param pMultValue Scale applied on key values.
		*	\return \c true on success, \c false otherwise.
          */
        bool KeyScaleValue (float pMultValue);

        /** Scale value and tangent of all keys.
		*	\param pMultValue Scale applied on key values and tangents.
		*	\return \c true on success, \c false otherwise.
          */
        bool KeyScaleValueAndTangent (float pMultValue);
    //@}

    /**
    * \name Key Manipulation
    */

     //@{
		/** General function to set key properties.
		* The key at index pKeyIndex is retrieved and modified according to the other parameters. 
		* The key must have been previously created, for example using KeyAdd.
		*	Use KFbxAnimCurveKFCurve::SetTCB() in the specific case of setting a key with cubic interpolation and TCB tangent mode.
		*   \param pKeyIndex        Index of the key.
		*	\param pTime			Key time.
		*	\param pValue			Key value.
		*	\param pInterpolation	Key interpolation type.
		*	\param pTangentMode		Key tangent mode (meaningful for cubic interpolation only).
		*	\param pData0			Value of right slope.
		*	\param pData1			Value of next left slope.
		*	\param pTangentWeightMode	Weight mode, if used.
		*	\param pWeight0				Weight for right slope, if tangent weight mode is eWEIGHTED_RIGHT or eWEIGHTED_ALL.
		*	\param pWeight1				Weight for next left slope, if tangent weight mode is eWEIGHTED_NEXT_LEFT or eWEIGHTED_ALL.
		*	\param pVelocity0			Velocity for right slope, if tangent velocity mode is eVELOCITY_RIGHT or eVELOCITY_ALL.
		*	\param pVelocity1			Velocity for next left slope, if tangent velocity mode is eVELOCITY_NEXT_LEFT or eVELOCITY_ALL.
		*/
		void KeySet 
        (
            int pKeyIndex,
            KTime pTime, 
            float pValue, 
            KFbxAnimCurveDef::EInterpolationType pInterpolation = KFbxAnimCurveDef::eINTERPOLATION_CUBIC, 
            KFbxAnimCurveDef::ETangentMode pTangentMode = KFbxAnimCurveDef::eTANGENT_AUTO, 
            float pData0 = 0.0,
            float pData1 = 0.0,
            KFbxAnimCurveDef::EWeightedMode pTangentWeightMode = KFbxAnimCurveDef::eWEIGHTED_NONE, 
            float pWeight0 = KFbxAnimCurveDef::sDEFAULT_WEIGHT,
            float pWeight1 = KFbxAnimCurveDef::sDEFAULT_WEIGHT,
            float pVelocity0 = KFbxAnimCurveDef::sDEFAULT_VELOCITY,
            float pVelocity1 = KFbxAnimCurveDef::sDEFAULT_VELOCITY
        );

		/**	Set a key with cubic interpolation, TCB tangent mode. 	
		* The key at index pKeyIndex is retrieved and modified according to the other parameters. 
		* The TCB mode controls the tension, continuity,
		* and bias of the curve.
		*   \param pKeyIndex  Index of the key.
		*	\param pTime	Key time.
		*	\param pValue	Key value.
		*	\param pData0	Tension. Controls the amount of curvature in the animation curve. The higher the tension is, the more linear
		*                   the curve looks. When the tension is low, the curve looks rounder or wider. 
		*	\param pData1	Continuity. Controls the smoothness or singularity of the curve on the key. 
		*	\param pData2	Bias. Controls if the effect of tension and continuity affect the curve before or after the key.
		*/
        void KeySetTCB 
        (
            int pKeyIndex,
            KTime pTime, 
            float pValue, 
            float pData0 = 0.0f, 
            float pData1 = 0.0f, 
            float pData2 = 0.0f
        );

		/** Get key's interpolation type.
		* \param pKeyIndex         Index of the queried key.
		* \return                  Interpolation type of the queried key.
		*/
        KFbxAnimCurveDef::EInterpolationType KeyGetInterpolation(int pKeyIndex) const;

		/** Set key's interpolation type.
		* \param pKeyIndex      Index of the key.
		* \param pInterpolation Key interpolation type.
		*/
        void KeySetInterpolation(int pKeyIndex, KFbxAnimCurveDef::EInterpolationType pInterpolation);

        /** Get key's constant mode.
		* This method is only relevant if the key's interpolation type is constant (eINTERPOLATION_CONSTANT).
		* Using this method on a key with an other interpolation type will return unpredictable value.
        * \param pKeyIndex Key index
        * \return Key constant mode.
        */
        KFbxAnimCurveDef::EConstantMode KeyGetConstantMode(int pKeyIndex) const;

		/** Get key's tangent mode.
		* This method is meaningful for cubic interpolation only.
		* Using this method for non cubic interpolated key will return unpredictable value.
		* \param pKeyIndex  Index of the key.
		* \param pIncludeOverrides Include override flags: Break, Clamp, Time-Independent.
		* \return Key tangent mode.
		*/
        KFbxAnimCurveDef::ETangentMode KeyGetTangentMode(int pKeyIndex, bool pIncludeOverrides = false ) const;

        /** Set key's constant mode.
		  *	This method is meaningful for constant interpolation only.
          * \param pKeyIndex Key index
          * \param pMode Key constant mode.
          */
        void KeySetConstantMode(int pKeyIndex, KFbxAnimCurveDef::EConstantMode pMode);

        /** Set key's tangent mode.
		* This method is meaningful for cubic interpolation only.
		* \param pKeyIndex   Index of the key.
		* \param pTangent Key tangent mode.
        */
        void KeySetTangentMode(int pKeyIndex, KFbxAnimCurveDef::ETangentMode pTangent);

        /** Get key at given index.
        * \param  pIndex Index of the key on the animation curve.
        * \return The key at the given index.
		* \remark Result is undetermined if animation curve has no key or if index 
		*         is out of bounds.
        */
        KFbxAnimCurveKey KeyGet(int pIndex) const;

        /** Get key value.
		* \param pKeyIndex   Index of the queried key.
		* \return            Key value.
        */
        float KeyGetValue(int pKeyIndex) const;

		/** Set key value.
		* \param pKeyIndex   Index of the key.
		* \param pValue      The value to set.
		*/ 
        void KeySetValue(int pKeyIndex, float pValue);

		/** Increment key value.
		* \param pKeyIndex   Index of the key.
		* \param pValue      Term added to the key value.
		*/
        void KeyIncValue(int pKeyIndex, float pValue);

		/** Multiply key value.
		* \param pKeyIndex   Index of the key.
		* \param pValue      Factor multiplying the key value.
		* \see KFbxAnimCurve::KeyMultTangent.
		*/
        void KeyMultValue(int pKeyIndex, float pValue);

		/** Multiply key tangents.
		* \param pKeyIndex   Index of the key.
		* \param pValue Factor multiplying the key tangents.
		* \remark When multiplying a key value, tangents must be
		*         multiplied to conserve the same topology.
		*/
        void KeyMultTangent(int pKeyIndex, float pValue);

		/** Get key time
		*   \param pKeyIndex   Index of the queried key.
		*	\return Key time (time at which this key is occurring).
		*/
        virtual KTime KeyGetTime(int pKeyIndex) const;

		/** Set key time.
		* \param pKeyIndex   Index of the key.
		* \param pTime       Key time (time at which this key is occurring).
		* \remark The new key time might modify the key index.
		*/
        virtual void KeySetTime(int pKeyIndex, KTime pTime);

        /** Set or unset the tangent break. When this flag is set (KFbxAnimCurveDef::eTANGENT_BREAK), the key's left and right slopes are independent.
		* When this flag is off, the key's left and right slope are equal.
		* This method is relevant for User (KFbxAnimCurveDef::eTANGENT_USER) and Auto (KFbxAnimCurveDef::eTANGENT_AUTO) tangent modes only.
		* \param pKeyIndex Index of the key.
		* \param pVal Break flag (\c true or \c false).
        */
        void KeySetBreak(int pKeyIndex, bool pVal); 

        /** Get if the tangent has a break. When this flag is set (KFbxAnimCurveDef::eTANGENT_BREAK), the key's left and right slopes are independent.
		* When this flag is off, the key's left and right slope are equal.
		* This method is relevant for User (KFbxAnimCurveDef::eTANGENT_USER) and Auto (KFbxAnimCurveDef::eTANGENT_AUTO) tangent modes only.
		* \param pKeyIndex Index of the queried key.
		* \return Break flag (\c true or \c false).
        */
        bool KeyGetBreak(int pKeyIndex) const; 
    //@}

    /**
      * \name Key Tangent Management
      */
    //@{
        /** Get the left derivative of a key.
        * \param pIndex Index of key.
		* \return Left derivative (Value over time (s)).
		* \remark Result is undetermined if animation curve has no key or if index 
		* is out of bounds.
        */
        float KeyGetLeftDerivative(int pIndex);

        /** Set the left derivative of a key.
        * \param pIndex Index of key.
        * \param pValue Left derivative.
		* \remark Result is undetermined if animation curve has no key or if index 
		* is out of bounds.
		* This function is only relevant if previous key interpolation
		* type is eINTERPOLATION_CUBIC and tangent mode is
		* KFbxAnimCurveDef::eTANGENT_USER, KFbxAnimCurveDef::eTANGENT_BREAK or KFbxAnimCurveDef::eTANGENT_AUTO. 
        */
        void KeySetLeftDerivative(int pIndex, float pValue);

		/** Get the left auto parametric of a key. This is used to compute the slope of Auto and User keys.
		* \param pIndex Index of the key.
		* \param pApplyOvershootProtection Clamp flag (eGENERIC_CLAMP) is taken into account.
		* \return Left auto parametric.
		* \remark Result is undetermined if animation curve has no key or if index 
		* is out of bounds.
		*/
        float KeyGetLeftAuto(int pIndex, bool pApplyOvershootProtection = false);

		/** Get the left derivative info (of type KFCurveTangeantInfo) of a key.
		* \param pIndex Index of the queried key.
		* \return Left derivative info.
		* \remark Result is undetermined if animation curve has no key or if index 
		* is out of bounds.
		*/
		KFCurveTangeantInfo KeyGetLeftDerivativeInfo(kFCurveIndex pIndex);

		/** Set the left derivative info (of type KFCurveTangeantInfo) of a key.
		* \param pIndex Index of the key.
		* \param pValue Left derivative info.
		* \param pForceDerivative If \c true, assign the tangent info's derivative value to the key derivative.
		* If \c false, use the tangent info's auto parametric value to recompute the key derivative.
		* \remark Result is undetermined if animation curve has no key or if index 
		* is out of bounds.
		* This function is only relevant if previous key interpolation
		* type is eINTERPOLATION_CUBIC and tangent mode is
		* KFbxAnimCurveDef::eTANGENT_USER or KFbxAnimCurveDef::eTANGENT_BREAK.
		*/
		void KeySetLeftDerivativeInfo(kFCurveIndex pIndex, KFCurveTangeantInfo pValue, bool pForceDerivative = false);

		/** Get the right derivative of a key.
		* \param pIndex Index of the key.
		* \return Right derivative (Value over time (s)).
		* \remark Result is undetermined if animation curve has no key or if index 
		* is out of bounds.
		*/
        float KeyGetRightDerivative(int pIndex);

		/** Set the right derivative of a key.
		* \param pIndex Index of the key.
		* \param pValue Right derivative.
		* \remark Result is undetermined if animation curve has no key or if index 
		* is out of bounds.
		* This function is only relevant if previous key interpolation
		* type is eINTERPOLATION_CUBIC and tangent mode is
		* KFbxAnimCurveDef::eTANGENT_USER, KFbxAnimCurveDef::eTANGENT_BREAK or KFbxAnimCurveDef::eTANGENT_AUTO.
		*/
        void KeySetRightDerivative(int pIndex, float pValue);

		/** Get the right auto parametric of a key. This is used to compute the slope of Auto and User keys.
		* \param pIndex Index of the key.
		* \param pApplyOvershootProtection Clamp flag (eGENERIC_CLAMP) is taken into account.
		* \return Right auto parametric.
		* \remark Result is undetermined if animation curve has no key or if index 
		* is out of bounds.
		*/
        float KeyGetRightAuto(int pIndex, bool pApplyOvershootProtection = false);

		/** Get the right derivative info (of type KFCurveTangeantInfo) of a key.
		* \param pIndex Index of the queried key.
		* \return Right derivative info.
		* \remark Result is undetermined if animation curve has no key or if index 
		* is out of bounds.
		*/
		KFCurveTangeantInfo KeyGetRightDerivativeInfo(kFCurveIndex pIndex);

		/** Set the right derivative info (of type KFCurveTangeantInfo) of a key.
		* \param pIndex Index of the key.
		* \param pValue Right derivative info.
		* \param pForceDerivative If \c true, assign the tangent info's derivative value to the key derivative.
		* If \c false, use the tangent info's auto parametric value to recompute the key derivative.
		* \remark Result is undetermined if animation curve has no key or if index 
		* is out of bounds.
		* This function is only relevant if previous key interpolation
		* type is eINTERPOLATION_CUBIC and tangent mode is
		* KFbxAnimCurveDef::eTANGENT_USER or KFbxAnimCurveDef::eTANGENT_BREAK.
		*/
        void KeySetRightDerivativeInfo(kFCurveIndex pIndex, KFCurveTangeantInfo pValue, bool pForceDerivative = false);

		/** Get the left tangent weight mode of a key.
		* \param pIndex Index of queried key.
		* \return \c true if the key is left weighted (Weight mode is eWEIGHT_WEIGHTED_RIGHT or eWEIGHTED_ALL). 
		*         \c false otherwise.
		* \remark Result is undetermined if animation curve has no key or if index 
		*  is out of bounds.
		*/
        bool KeyIsLeftTangentWeighted(int pIndex) const;

		/** Get the right tangent weight mode of a key.
		* \param pIndex Index of queried key.
		* \return \c true if the key is right weighted (Weight mode is eWEIGHTED_RIGHT or eWEIGHTED_ALL). 
		*         \c false otherwise.
		* \remark Result is undetermined if animation curve has no key or if index 
		*  is out of bounds.
		*/
        bool KeyIsRightTangentWeighted(int pIndex) const;

		/** Get the weight value component of the left tangent of a key.
		* \param pIndex Index of the key.
		* \return Left tangent weight, or eDEFAULT_WEIGHT (0.333...) if left tangent is not weighted.
		* \remark This function is only relevant if key interpolation type is eINTERPOLATION_CUBIC.
		*/
        float KeyGetLeftTangentWeight(int pIndex) const;

		/** Get the weight value component of the right tangent of a key.
		* \param pIndex Index of the key.
		* \return Right tangent weight, or eDEFAULT_WEIGHT (0.333...) if right tangent is not weighted.
		* \remark This function is only relevant if key interpolation type is eINTERPOLATION_CUBIC.
		*/	
        float KeyGetRightTangentWeight(int pIndex) const;

		/** Set the left tangent weight of a key
		* \param pIndex Index of the key.
		* \param pWeight Weight to set on the left tangent.
		* \param pAdjustTan If true, recompute the tangent height to compensate for very small weights.
		* \remarks This function is only relevant if previous key interpolation
		* type is eINTERPOLATION_CUBIC and tangent mode is
		* KFbxAnimCurveDef::eTANGENT_USER or KFbxAnimCurveDef::eTANGENT_BREAK. The tangent is 
		* automatically set in weighted mode.
		* The pAdjustTan option will only produce correct results provided that the tangent has already been
		* set before calling this function.
		*/
        void   KeySetLeftTangentWeight( int pIndex, float pWeight, bool pAdjustTan = false );

		/** Set the right tangent weight of a key
		* \param pIndex Index of the key.
		* \param pWeight Weight to set on the right tangent.
		* \param pAdjustTan If true, recompute the tangent height to compensate for very small weights.
		* \remarks This function is only relevant if key interpolation
		* type is eINTERPOLATION_CUBIC and tangent mode is
		* KFbxAnimCurveDef::eTANGENT_USER or KFbxAnimCurveDef::eTANGENT_BREAK. The tangent is 
		* automatically set in weighted mode.
		* The pAdjustTan option will only produce correct results provided that the tangent has already been
		* set before calling this function.
		*/
        void   KeySetRightTangentWeight( int pIndex, float pWeight, bool pAdjustTan = false  );

		/** Get the velocity value component of the left tangent of a key
		* \param pIndex Index of the key.
		* \return Tangent velocity of the left tangent.
		* This function is only relevant if key interpolation type is eINTERPOLATION_CUBIC.
		*/
        float KeyGetLeftTangentVelocity( int pIndex) const;

		/** Get the velocity value component of the right tangent of a key
		* \param pIndex Index of the key.
		* \return Tangent velocity of the right tangent.
		* This function is only relevant if key interpolation type is eINTERPOLATION_CUBIC.
		*/			
        float KeyGetRightTangentVelocity( int pIndex) const;

    //@}

    /**
      * \name Evaluation and Analysis
      */
    //@{
		/**	Evaluate animation curve value at a given time.
		* \param pTime Time of evaluation.
		* If time falls between two keys, animation curve value is 
		* interpolated according to previous key interpolation type and
		* tangent mode if relevant.
		* \param pLast Index of the last processed key to speed up search. If this 
		* function is called in a loop, initialize this value to 0 and let it 
		* be updated by each call.
		* \return Animation curve value on given time, or animation curve's default value if animation curve
		* has no key.
		* \remark This function takes extrapolation into account.
		*/
		virtual float Evaluate (KTime pTime, int* pLast = NULL);

		/**	Evaluate animation curve value at a given key index.
		* \param pIndex Any value between 0 and KFbxAnimCurve::KeyGetCount() - 1.
		* If key index is not an integer value, animation curve value is 
		* interpolated according to previous key interpolation type and
		* tangent mode, if relevant.
		* This function does not take extrapolation into account.
		* \return Animation curve value, or default value if animation curve has no key.
		* \remark Result is undetermined if index is out of bounds.
		*/
        virtual float EvaluateIndex( double pIndex);
        
		/**	Evaluate function left derivative at given time.
		* \param pTime Time of evaluation.
		* \param pLast Index of the last processed key to speed up search. If this function is called in a loop, 
		*              initialize this value to 0 and let it be updated by each call.
		* \return Left derivative at given time.
		* \remark This function does not take extrapolation into account. Result is undetermined if index is out of bounds.
		*/
		float EvaluateLeftDerivative (KTime pTime, int* pLast = NULL);
        
		/**	Evaluate function right derivative at given time.
		* \param pTime Time of evaluation.
		* \param pLast Index of the last processed key to speed up search. If this function is called in a loop, 
		*              initialize this value to 0 and let it be updated by each call.
		* \return Right derivative at given time.
		* \remark This function does not take extrapolation into account. Result is undetermined if index is out of bounds.
		*/
		float EvaluateRightDerivative (KTime pTime, int* pLast = NULL);
    //@}

    /**
      * \name Utility functions.
      *
      */
    //@{
        /** Get the animation curve's default value.
		* Default value is the value that will be used when there is no key in the animation curve.
		* \return Default value of the animation curve.
        */
        float GetValue() const;

		/** Find out start and end time of the animation animation curve.
		* This function retrieves the animation curve's time span.
		* \param pStart Reference to receive start time.
		* \param pStop Reference to receive end time.
		* \return \c true on success, \c false otherwise.
        * \remarks This function is deprecated, please use GetTimeInterval(KTimeSpan&) instead.
		*/
		K_DEPRECATED virtual bool GetTimeInterval(KTime& pStart, KTime& pStop);

		/** Find out start and end time of the animation animation curve.
		* This function retrieves the animation curve's time span.
		* \param pTimeInterval Reference to receive start time and end time.
		* \return \c true on success, \c false otherwise.
		*/
		virtual bool GetTimeInterval(KTimeSpan& pTimeInterval);

        /** Set the display color of the animation curve.
        * \param pColor The desired color (3 values 0 to 1).
        */
        void SetColor(float *pColor);

        /** Get the display color of the animation curve.
        * \return Array of 3 floats (color) or NULL.
        */
        float* GetColor();

        /** Returns the pointer to internal KFCurve.
        * \return Pointer to KFCurve.
        */
        KFCurve* GetKFCurve();

		/** Copy animation curve content into current animation curve.
        * \param pSource Animation curve to be copied (which will not be modified).
        * \param pWithKeys If \c true, clear keys in current animation curve and copy
		* keys from source animation curve. If \c false, keys in current animation curve
		* are left as is.
        */
		void CopyFrom(KFbxAnimCurve& pSource, bool pWithKeys = true);
    //@}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    ///////////////////////////////////////////////////////////////////////////////
    //  WARNING!
    //    Anything beyond these lines may not be documented accurately and is 
    //     subject to change without notice.
    ///////////////////////////////////////////////////////////////////////////////
    virtual int KeyAdd (KTime pTime, KFbxAnimCurveKFCurveKey& pKey, int* pLast = 0);
    virtual bool KeySet(int pIndex, KFbxAnimCurveKFCurveKey& pKey);

protected:
    KFbxAnimCurveKFCurve(KFbxSdkManager& pManager, char const* pName);

    virtual void Construct(const KFbxAnimCurve* pFrom);
    virtual void Destruct(bool pRecursive, bool pDependents);
 
	friend void CollectAnimFromCurveNode(KFCurve **lSrc, KFCurveNode *fcn, unsigned int nbCrvs, KFbxAnimCurveNode *cn, KSet* pNickToAnimCurveTimeWarpsSet, KSet& pNickToKFCurveNodeWarpSet);
    friend void TransferTimeWarp(int lNickNumber, KFCurveNode* lTimeWarp, KSet& mNickToKFCurveNodeTimeWarpsSet, KSet& pTimeWarpSet, KFbxScene& pScene);
    
	void SetKFCurve(KFCurve *lKFCurve);
    KFCurve* mFCurve;

public:
    virtual bool Store(KFbx* pFileObject);
            bool LegacyStore(KFbx* pFileObject); // this one can only exist in this class and should not be inherited!
    virtual bool Retrieve(KFbx* pFileObject);
 
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVEKFCURVE_H
