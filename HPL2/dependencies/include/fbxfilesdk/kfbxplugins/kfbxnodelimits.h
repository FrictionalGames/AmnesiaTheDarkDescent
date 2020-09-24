/*!  \file kfbxnodelimits.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXNODELIMITS_H
#define FBXFILESDK_KFBXPLUGINS_KFBXNODELIMITS_H

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

#include <fbxfilesdk/kfbxmath/kfbxvector4.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxNode;
class KMBLimit;

 /** \enum ELimitedProperty Defines which transform component (translation, 
   *       rotation, or scaling) is limited by a KFbxLimits object.
   */
typedef enum 
	{
		eTRANSLATION,	/*!< The KFbxLimits object defines the limit for translation. */  
		eROTATION,		/*!< The KFbxLimits object defines the limit for rotation. */           
		eSCALE,			/*!< The KFbxLimits object defines the limit for scale. */
	} ELimitedProperty;

/** KFbxLimits defines a limit range for one transform component (either translation, 
  * rotation or scaling).
  * One transform component limit contains two part: a min value and a max value limit, which means
  * that each value of the corresponding transform component cannot go beyond the range set by the
  * min and max values. KFbxLimits uses KFbxVector4 objects to store the range values. Although the 
  * members are identified as X, Y and Z (the W component is ignored) at this level, they are 
  * unitless values and will only have meaning within the context they are queried.
  *
  * For each limit, there is one flag to indicate if the limit is active or not. Before accessing the 
  * limit info, the caller need to query the flag first to make sure that the retrieved values will be
  * meaningful.
  * \nosubgrouping
  */
class KFBX_DLL KFbxLimits
{
public:
	/** Constructor.
	  */
	KFbxLimits();

	//! Destructor
	virtual ~KFbxLimits();

    KFbxLimits(const KFbxLimits & other);

	/** Set the active state of min limit.
	  * \param pXActive     Set to \c true, to activate the X component min limit.
	  * \param pYActive     Set to \c true, to activate the Y component min limit.
	  * \param pZActive     Set to \c true, to activate the Z component min limit.
	  */
	void SetLimitMinActive(const bool pXActive, const bool pYActive, const bool pZActive);

	/** Get the active states of the three components of the min limit.
	  * \param pXActive     \c true if the X component of the min limit is active.
	  * \param pYActive     \c true if the Y component of the min limit is active.
	  * \param pZActive     \c true if the Z component of the min limit is active.
	  */
	void GetLimitMinActive(bool& pXActive, bool& pYActive, bool& pZActive) const;

	/** Set the active state of max limit.
	  * \param pXActive     Set to \c true, to activate the X component max limit.
	  * \param pYActive     Set to \c true, to activate the Y component max limit.
	  * \param pZActive     Set to \c true, to activate the Z component max limit.
	  */
	void SetLimitMaxActive(const bool pXActive, const bool pYActive, const bool pZActive);

	/** Get the active states of the three components of the max limit.
	  * \param pXActive     \c true if the X component of the max limit is active.
	  * \param pYActive     \c true if the Y component of the max limit is active.
	  * \param pZActive     \c true if the Z component of the max limit is active.
	  */
	void GetLimitMaxActive(bool& pXActive, bool& pYActive, bool& pZActive) const;

	/** Check if at least one of the active flags is set.
	  * \return     \c true if one of the six active flags is set.
	  */
	bool GetLimitSomethingActive() const;
	
	/** Set the min limit.
	  * \param pMin     The X, Y and Z values to be set for the min limit.
	  */
	void        SetLimitMin(const KFbxVector4& pMin);

	/** Get the min limit.
	  * \return     The current X, Y and Z values for the min limit.
	  */
	KFbxVector4 GetLimitMin() const;

	/** Set the max limit.
	  * \param pMax    The X, Y and Z values to be set for the max limit.
	  */
	void        SetLimitMax(const KFbxVector4& pMax);

	/** Get the max limit.
	  * \return     The current X, Y and Z values for the max limit.
	  */
	KFbxVector4 GetLimitMax() const;
	
	/** Assignment operation
	  * \param pFbxLimit Another KFbxLimits object to be assigned to this one.
	  */
	KFbxLimits& operator=(const KFbxLimits& pFbxLimit);

	void Apply(KFbxVector4& pOut, KFbxVector4& pIn);

private:	
	KMBLimit* mLimit;

	friend class KFbxNodeLimits;
};

/** 
* Manages a separate KFbxLimits object for the translation, rotation, and scaling 
* transform components of a node.
* Each transform component limit can be activated or deactivated. 
*
* The following example does not show good practice, but does demonstrates how 
* the members of this class are affecting each other :
*
* \code
* // assuming 'this' is a valid KFbxNodeLimits object.
*
* // Set the translation limits active
* this->SetTranslationLimitActive(false); 
*
* // overwrite the translation limits (from active to inactive). 
* // rotation limits correctly set to inactive and scaling limits set to active too! 
* this->mRotationLimits.SetLimitMinActive(false, false, true); 
*
* // The following now returns true even though we never explicitly set 
* // the flags from the mScalingLimit object. 
* this->GetScalingLimitActive() 
* \endcode
*/
class KFBX_DLL KFbxNodeLimits
{
public:
	/** Constructor.
	  * \param pLimitedNode     Pointer to the node to which these limits apply.
	  */
	KFbxNodeLimits(KFbxNode *pLimitedNode);

	/** Get the limited node.
	  * \return     Pointer to the node to which these limits apply. This node is the same pointer as 
	  *             the one passed to the constructor.
	  */
	KFbxNode* GetLimitedNode();

	/**
	  * \name Node Translation Limits
	  */
	//@{
		/** Change the translation limit active flag.
		  * \param pActive     State of the translation limits active flag.
		  * \remarks           If this flag is set to \c false, the values in the mTranslationLimits are ignored.
		  */
		void SetTranslationLimitActive(bool pActive);

		/** Get the translation limit active flag.
		  * \return     Translation limit active flag state.
		  * \remarks    If this flag is \c false, the values in the mTranslationLimits are ignored.
		  */
		bool GetTranslationLimitActive() const;

		//! The translation limits.
		KFbxLimits mTranslationLimits;
	//@}

	/**
	  * \name Node Rotation Limits
	  */
	//@{
		/** Change the rotation limit active flag.
		  * \param pActive     State of the rotation limits active flag.
		  * \remarks           If this flag is set to \c false, the values in the mRotationLimits are ignored.
		  */
		void SetRotationLimitActive(bool pActive);

		/** Get the rotation limit active flag.
		  * \return     Rotation limit active flag state. 
		  * \remarks    If this flag is \c false, the values in the mRotationLimits are ignored.
		  */
		bool GetRotationLimitActive()const;

		//! The rotation limits.
		KFbxLimits mRotationLimits;
	//@}

	/**
	  * \name Node Scale Limits
	  */
	//@{
		/** Change the scaling limit active flag.
		  * \param pActive     State of the scaling limits active flag.
		  * \remarks           If this flag is set to \c false, the values in the mScalingLimits are ignored.
		  */
		void SetScalingLimitActive(bool pActive);

		/** Get the scaling limit active flag.
		  * \return      Scaling limit active flag state.
		  * \remarks     If this flag is \c false, the values in the mScalingLimits are ignored.
		  */
		bool GetScalingLimitActive() const;

		//! The scaling limits.
		KFbxLimits mScalingLimits;
	//@}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
	KFbxNodeLimits& operator=(KFbxNodeLimits const& pNodeLimits);
	inline KMBLimit* GetRLimit() { return mRotationLimits.mLimit; }

private:
	//! Node on which the limits are applied
	KFbxNode* mLimitedNode;
	friend class KFbxNode;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxLimits* HKFbxLimits;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXNODELIMITS_H

