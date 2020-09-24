/*!  \file kfbxskeleton.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSKELETON_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSKELETON_H

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

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/kfbxplugins/kfbxnodeattribute.h>
#include <fbxfilesdk/kfbxplugins/kfbxcolor.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;

/**	This class specializes a node attribute to represent the elements forming "bone" 
  * chains. The KFbxSkeleton name of the class comes from the analogy with the human body
  * skeletal structure. In fact, an object of this type is nothing more than a transform
  * node with special properties that are useful for its graphical representation and during
  * IK/FK and skin deformation computations. Typically, a scene will contain chains of KFbxSkeleton
  * node attributes that, together, form a skeleton segment. For instance, the representation of the
  * leg of a character can be achieved using one parent node with the attribute eROOT, followed by
  * one child (femur) of type eLIMB, this child having a child also (tibia) of the same type. Finally,
  * terminated with a last node attribute of type eEFFECTOR (ankle).
  * 	
  * \nosubgrouping
  */
class KFBX_DLL KFbxSkeleton : public KFbxNodeAttribute
{
	KFBXOBJECT_DECLARE(KFbxSkeleton,KFbxNodeAttribute);

public:
 	//! Return the type of node attribute (i.e: EAttributeType::eSKELETON).
	virtual EAttributeType GetAttributeType() const;

    //! Reset the skeleton to default values and type to \c eROOT.
	void Reset();

	/**
	  * \name Skeleton Properties
	  */
	//@{

	/** \enum ESkeletonType Skeleton types.
	  * \remarks \e eEFFECTOR is synonymous to \e eROOT.
	  * \remarks The \e eLIMB_NODE type is a bone defined uniquely by a transform and a size value while
	  * \remarks the \e eLIMB type is a bone defined by a transform and a length.
	  */
    typedef enum   
    {
	    eROOT,			/*!< First element of a chain. */
	    eLIMB,			/*!< Chain element. */
	    eLIMB_NODE,		/*!< Chain element. */
	    eEFFECTOR		/*!< Last element of a chain. */
    } ESkeletonType;    

    /** Set the skeleton type.
	  * \param pSkeletonType Skeleton type identifier.
	  */
    void SetSkeletonType(ESkeletonType pSkeletonType);

	/** Get the skeleton type.
	  * \return Skeleton type identifier.
	  */
    ESkeletonType GetSkeletonType() const;

	/** Get a flag to know if the skeleton type was set.
	  * \return \c true if a call to SetSkeletonType() has been made.
	  * \remarks When the attribute is not set, the application can choose to ignore the attribute or use the default value.
	  * \remarks The flag is set back to \c false when Reset() is called.
      */
	bool GetSkeletonTypeIsSet() const;

	/** Get the default value for the skeleton type.
	  * \return \c eROOT
	  */
	ESkeletonType GetSkeletonTypeDefaultValue() const;
		
	/** Get the default value for the limb length.
	  * \return 1.0
	  */
	double GetLimbLengthDefaultValue() const;
	
	/** Get the default value for the limb node size.
	  * \return 100.0
	  */
	double GetLimbNodeSizeDefaultValue() const;

	/** Set limb or limb node color.
	  * \param pColor RGB values for the limb color.
	  * \return \c true if skeleton type is \c eLIMB or \c eLIMB_NODE, \c false otherwise.
	  * \remarks Limb or limb node color is only set if skeleton type is \c eLIMB or \c eLIMB_NODE.
      */
	bool SetLimbNodeColor(const KFbxColor& pColor);
	
	/** Get limb or limb node color.
	  * \return Currently set limb color.
	  * \remarks Limb or limb node color is only valid if skeleton type is \c eLIMB or \c eLIMB_NODE.
      */
	KFbxColor GetLimbNodeColor() const;

	/** Get a flag to know if the limb node color was set.
	  * \return \c true if a call to SetLimbNodeColor() has been made.
	  * \remarks When the attribute is not set, the application can choose to ignore the attribute or use the default value.
	  * \remarks The flag is set back to \c false when Reset() is called.
      */
	bool GetLimbNodeColorIsSet() const;

	/** Get the default value for the limb node color.
	  * \return R=0.8, G=0.8, B=0.8
	  */
	KFbxColor GetLimbNodeColorDefaultValue() const;

    /** To see if this skeleton is Root.
    * \return \c true if this is root of the skeleton, \c false otherwise.
    * \remarks if a skeleton node do not have a parent or its parent is not a skeleton node itself, then this 
	* skeleton is root in the hierarchy.
    */
    bool IsSkeletonRoot() const;

	//@}


	/**
	  * \name Property Names
	  */
	static const char*			sSize;
	static const char*			sLimbLength;

	/**
	  * \name Property Default Values
	  */
	//@{	
	static const fbxDouble1		sDefaultSize;
	static const fbxDouble1		sDefaultLimbLength;


	//////////////////////////////////////////////////////////////////////////
	//
	// Properties
	//
	//////////////////////////////////////////////////////////////////////////
	
	/** This property handles the limb node size.
	  *
      * To access this property do: Size.Get().
      * To set this property do: Size.Set(fbxDouble1).
      *
	  * Default value is 100.0
	  */
	KFbxTypedProperty<fbxDouble1>		Size;

	/** This property handles the skeleton limb length.
	*
	* To access this property do: LimbLength.Get().
	* To set this property do: LimbLength.Set(fbxDouble1).
	*
	* KFbxSkeleton is a node attribute and it will be attached to a KFbxNode which represents the transform.
	* Given a chain of skeleton nodes the parent and child skeletons will be attached to a parent node and a child node.
	* The orientation of the limb is computed from the vector between the parent and child position (from parent to child). 
	* The LimbLength represents the proportion 
	* of the parent node's position to the child node's position which is used to compute the actual limb length.
	* The default value of 1.0 means the LimbLength is equal to the length between the parent and child node's position.
	* So if the value is 0.5, it means the LimbLength will be half of the length between the parent and child node's position.
	*/
	KFbxTypedProperty<fbxDouble1>			LimbLength;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
    KFbxSkeleton(KFbxSdkManager& pManager, char const* pName);

	virtual void Construct(const KFbxSkeleton* pFrom);
	virtual bool ConstructProperties(bool pForceSet);

	void Reset( bool pResetProperties );

	virtual const char* GetTypeName() const;
	virtual KStringList GetTypeFlags() const;

    ESkeletonType mSkeletonType;

	bool mLimbLengthIsSet;
	bool mLimbNodeSizeIsSet;
	bool mLimbNodeColorIsSet;
	bool mSkeletonTypeIsSet;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxSkeleton* HKFbxSkeleton;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSKELETON_H

