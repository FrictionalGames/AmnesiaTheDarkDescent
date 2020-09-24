/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxmarker.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXMARKER_H
#define FBXFILESDK_KFBXPLUGINS_KFBXMARKER_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxnodeattribute.h>
#include <fbxfilesdk/kfbxplugins/kfbxcolor.h>
#include <fbxfilesdk/kfbxmath/kfbxvector4.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;

/**	This node attribute contains the properties of a marker.
  * A KFbxMarker can represent a motion capture marker or a HIK IK/FK effector.
  * \nosubgrouping
  */
class KFBX_DLL KFbxMarker : public KFbxNodeAttribute
{
	KFBXOBJECT_DECLARE(KFbxMarker,KFbxNodeAttribute);

public:
	//! Return the type of node attribute which is EAttributeType::eMARKER.
	virtual EAttributeType GetAttributeType() const;

	//! Reset the marker to default values.
	void Reset();

	/** \enum EType Marker types.
	  * - \e eSTANDARD
	  * - \e eOPTICAL
	  * - \e eFK_EFFECTOR
	  * - \e eIK_EFFECTOR
	  */
	typedef enum { 
		eSTANDARD, 
		eOPTICAL, 
		eFK_EFFECTOR,
		eIK_EFFECTOR
	} EType;

	/** Set marker type.
	  * \param pType The type of marker.
	  */
	void SetType(EType pType);

	/** Get marker type.
	  * \return The type of the marker.
	  */
	EType GetType() const;

	/** \enum ELook Marker look.
	  * - \e eCUBE
	  * - \e eHARD_CROSS
	  * - \e eLIGHT_CROSS
	  * - \e eSPHERE
      * - \e eCAPSULE
      * - \e eBOX
      * - \e eBONE
      * - \e eCIRCLE
      * - \e eSQUARE
      * - \e eSTICK
	  * - \e eNONE
	  */
	typedef enum { 
		eCUBE, 
		eHARD_CROSS, 
		eLIGHT_CROSS, 
        eSPHERE,
        eCAPSULE,
        eBOX,
        eBONE,
        eCIRCLE,
        eSQUARE,
        eSTICK,
		eNONE
	} ELook;
	
	/**
	  * \name Default Animation Values
	  * This set of functions provides direct access to default
	  * animation values specific to a marker. The default animation 
	  * values are found in the default take node of the associated node.
	  * Hence, these functions only work if the marker has been associated
	  * with a node.
	  */
	//@{

	/** Get default occlusion.
	  * \return 0.0 if optical marker animation is valid by default, 1.0 if it is occluded by default.
	  * \remarks This function only works if marker type is set to KFbxMarker::eOPTICAL.
	  */
	double GetDefaultOcclusion() const;

	/** Set default occlusion.
	  * \param pOcclusion 0.0 if optical marker animation is valid by default, 1.0 if it is occluded by default.
	  * \remarks This function only works if marker type is set to KFbxMarker::eOPTICAL.
	  */
	void SetDefaultOcclusion(double pOcclusion);

	/** Get default IK reach translation.
	  * \return A value between 0.0 and 100.0, 100.0 means complete IK reach.
	  * \remarks This function only works if marker type is set to KFbxMarker::eIK_EFFECTOR.
	  */
	double GetDefaultIKReachTranslation() const;

	/** Set default IK reach translation.
	  * \param pIKReachTranslation A value between 0.0 and 100.0, 100.0 means complete IK reach.
	  * \remarks This function only works if marker type is set to KFbxMarker::eIK_EFFECTOR.
	  */
	void SetDefaultIKReachTranslation(double pIKReachTranslation);

	/** Get default IK reach rotation.
	  * \return A value between 0.0 and 100.0, 100.0 means complete IK reach.
	  * \remarks This function only works if marker type is set to KFbxMarker::eIK_EFFECTOR.
	  */
	double GetDefaultIKReachRotation() const;

	/** Set default IK reach rotation.
	  * \param pIKReachRotation A value between 0.0 and 100.0, 100.0 means complete IK reach.
	  * \remarks This function only works if marker type is set to KFbxMarker::eIK_EFFECTOR.
	  */
	void SetDefaultIKReachRotation(double pIKReachRotation);

	//@}

	/**
	  * \name Obsolete functions
	  */
	//@{

	/** Get default color.
	  * \param pColor Filled with appropriate data
	  * \return Input parameter filled with appropriate data.
	  * \remarks Marker color can not be animated anymore.
	  */
	KFbxColor& GetDefaultColor(KFbxColor& pColor) const;

	/** Set default color.
	  * \param pColor The marker color to be set.
	  * \remarks Marker color can not be animated anymore.
	  */
	void SetDefaultColor(KFbxColor& pColor);

	//@}

	/**
	  * \name Property Names
	  */
	static const char*			sLook;
    static const char*            sDrawLink;
	static const char*			sSize;
	static const char*			sShowLabel;
	static const char*			sIKPivot;

	/**
	  * \name Property Default Values
	  */
	static const ELook			sDefaultLook;
    static const fbxBool1        sDefaultDrawLink;
	static const fbxDouble1		sDefaultSize;
	static const fbxBool1		sDefaultShowLabel;
	static const fbxDouble3		sDefaultIKPivot;

	//////////////////////////////////////////////////////////////////////////
	//
	// Properties
	//
	//////////////////////////////////////////////////////////////////////////
	
	/** This property handles the marker's look.
	  *
      * To access this property do: Look.Get().
      * To set this property do: Look.Set(ELook).
      *
	  * Default value is eCUBE
	  */
	KFbxTypedProperty<ELook> Look;
	
    /** This property handles the marker's link visibility.
    *
    * To access this property do: DrawLink.Get().
    * To set this property do: DrawLink.Set(fbxBool1).
    *
    * Default value is false
    */
    KFbxTypedProperty<fbxBool1> DrawLink;
    
	/** This property handles the marker's size.
	  *
      * To access this property do: Size.Get().
      * To set this property do: Size.Set(fbxDouble1).
      *
	  * Default value is 100
	  */
	KFbxTypedProperty<fbxDouble1> Size;
	
	/** This property handles the marker's label visibility.
	  *
      * To access this property do: ShowLabel.Get().
      * To set this property do: ShowLabel.Set(fbxBool1).
      *
	  * Default value is false
	  */
	KFbxTypedProperty<fbxBool1> ShowLabel;
	
	/** This property handles the marker's pivot position.
	  *
      * To access this property do: IKPivot.Get().
      * To set this property do: IKPivot.Set(fbxDouble3).
      *
	  * Default value is (0., 0., 0.)
	  */
	KFbxTypedProperty<fbxDouble3> IKPivot;

	// Dynamic properties

	/** This method grants access to the occlusion property.
	  * \remarks If the marker is not of type Optical or the property
	  * is invalid, return NULL
	  */
	KFbxProperty* GetOcclusion();

	/** This method grants access to the IKReachTranslation property.
	  * \remarks If the marker is not of type IK Effector or the property
	  * is invalid, return NULL
	  */
	KFbxProperty* GetIKReachTranslation();
	/** This method grants access to the IKReachRotation property.
	  * \remarks If the marker is not of type IK Effector or the property
	  * is invalid, return NULL
	  */
	KFbxProperty* GetIKReachRotation();

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
	KFbxMarker(KFbxSdkManager& pManager, char const* pName);

	virtual void Construct(const KFbxMarker* pFrom);
	virtual bool ConstructProperties(bool pForceSet);

	void Reset( bool pResetProperties );

	/**
	  *	Used to retrieve the KProperty list from an attribute.
	  */

	virtual const char* GetTypeName() const;
	virtual KStringList GetTypeFlags() const;

	EType mType;

	KFbxProperty dynProp; // temporary placeholder for either
	// the Occlusion, IKReachTranslation or IKReachRotation 
	// properties. Its address is returned in the GetOcclusion(),
	// GetIKReachTranslation() and GetIKReachRotation() if the property
	// is valid
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxMarker* HKFbxMarker;

inline EFbxType FbxTypeOf( KFbxMarker::ELook const &pItem )			{ return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXMARKER_H

