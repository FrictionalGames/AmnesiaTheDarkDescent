/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxglobalsettings.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXGLOBALSETTINGS_H
#define FBXFILESDK_KFBXPLUGINS_KFBXGLOBALSETTINGS_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/ktime.h>
#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/kfbxplugins/kfbxaxissystem.h>
#include <fbxfilesdk/kfbxplugins/kfbxsystemunit.h>
#include <fbxfilesdk/kfbxplugins/kfbxobject.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** \brief This class contains functions for accessing global settings.
  * \nosubgrouping
  */
class KFBX_DLL KFbxGlobalSettings : public KFbxObject
{
	KFBXOBJECT_DECLARE(KFbxGlobalSettings,KFbxObject);

public:
    /** 
	  * \name Axis system
	  */
	//@{
    
	/** Sets the scene's coordinate system.
	  * \param pAxisSystem              The coordinate system to set.
	  */
    void SetAxisSystem(const KFbxAxisSystem& pAxisSystem);
    
	/** Returns the scene's current coordinate system.
	  * \return                         The scene's current coordinate system.
	  */
    KFbxAxisSystem GetAxisSystem();
    //@}

    /** Sets the coordinate system's original Up Axis when the scene is created.
      * \param pAxisSystem              The coordinate system whose Up Axis is copied.
      */
    void SetOriginalUpAxis(const KFbxAxisSystem& pAxisSystem);

    /** Returns the coordinate system's original Up Axis.
      * \return                         The coordinate system's original Up Axis when the scene is created. 0 is X, 1 is Y, 2 is Z axis.
      */
    int GetOriginalUpAxis() const;
    //@}

    /** 
	  * \name System Units
	  */
	//@{

	/** Sets the unit of measurement used by the system.
	  * \param pOther                   The system unit to set. 
	  */
    void SetSystemUnit(const KFbxSystemUnit& pOther);
    
	/** Returns the unit of measurement used by the system.
	  * \return                         The unit of measurement used by the system.     
	  */
    KFbxSystemUnit GetSystemUnit() const;

    /** Sets the original unit of measurement used by the system.
      * \param pOther                   The original system unit to set. 
      */
    void SetOriginalSystemUnit(const KFbxSystemUnit& pOther);

    /** Returns the original unit of measurement used by the system.
      * \return                         The original unit of measurement used by the system.
      */
    KFbxSystemUnit GetOriginalSystemUnit() const;
    //@}


    /** 
	  * \name Light Settings
	  */
	//@{

    /** Sets the ambient color.
      * \param pAmbientColor            The ambient color to set.
      * \remarks                        The ambient color only uses the RGB channels.
      */
    void SetAmbientColor(KFbxColor pAmbientColor);

    /** Returns the ambient color.
      * \return                         The ambient color.
      */
    KFbxColor GetAmbientColor() const;

    //@}

    /** 
	  * \name Camera Settings
	  */
	//@{

    //! Defined camera name: PRODUCER_PERSPECTIVE
    static const char* ePRODUCER_PERSPECTIVE;

    //! Defined camera name: PRODUCER_TOP
    static const char* ePRODUCER_TOP;

    //! Defined camera name: PRODUCER_FRONT
    static const char* ePRODUCER_FRONT;

    //! Defined camera name: PRODUCER_BACK
    static const char* ePRODUCER_BACK;

    //! Defined camera name: PRODUCER_RIGHT
    static const char* ePRODUCER_RIGHT;

    //! Defined camera name: PRODUCER_LEFT
    static const char* ePRODUCER_LEFT;

    //! Defined camera name: PRODUCER_BOTTOM
    static const char* ePRODUCER_BOTTOM;

    /** Sets the default camera.
      * \param pCameraName              Name of the default camera.
      * \return                         \c true if camera name is valid, returns \c false if the camera does not have a valid name.
      * \remarks                        A valid camera name can be either one of the defined tokens (PRODUCER_PERSPECTIVE,
      *                                 PRODUCER_TOP, PRODUCER_FRONT, PRODUCER_BACK, PRODUCER_RIGHT, PRODUCER_LEFT and PRODUCER_BOTTOM) or the name
      *                                 of a camera inserted in the node tree under the scene's root node.
      */
    bool SetDefaultCamera(const char* pCameraName);

    /** Returns the default camera name.
      * \return                         The default camera name, or an empty string if no camera name has been set.
      */
    KString GetDefaultCamera() const;
    //@}

    /** 
	  * \name Time Settings
	  */
	//@{
    /** Sets the time mode.
    * \param pTimeMode                  One of the defined modes in class KTime.
    */
    void SetTimeMode(KTime::ETimeMode pTimeMode);

    /** Returns the time mode.
    * \return                           The currently set TimeMode.
    */
    KTime::ETimeMode GetTimeMode() const;

    /** Sets the default time span of the time line.
    * \param pTimeSpan                  The default time span of the time line.
    */
    void SetTimelineDefaultTimeSpan(const KTimeSpan& pTimeSpan);

    /** Returns the default time span of the time line.
    * \param pTimeSpan                  The default time span of the time line.
    */
    void GetTimelineDefaultTimeSpan(KTimeSpan& pTimeSpan) const;

    /** Set custom frame rate.
     *  This is meaningless if the time mode is not KTime::eCUSTOM.
     */
    void SetCustomFrameRate(double pCustomFrameRate);

    /** Return frame rate if the time mode is KTime::eCUSTOM.
     *  If the time mode is not KTime::eCUSTOM, return -1.
     */
    double GetCustomFrameRate() const; 
    //@}

protected:

	/**
	  * \name Properties
	  */
	//@{
		KFbxTypedProperty<fbxInteger1>	UpAxis;
		KFbxTypedProperty<fbxInteger1>	UpAxisSign;

		KFbxTypedProperty<fbxInteger1>	FrontAxis;
		KFbxTypedProperty<fbxInteger1>	FrontAxisSign;

		KFbxTypedProperty<fbxInteger1>	CoordAxis;
		KFbxTypedProperty<fbxInteger1>	CoordAxisSign;

        KFbxTypedProperty<fbxInteger1>	OriginalUpAxis;
        KFbxTypedProperty<fbxInteger1>	OriginalUpAxisSign;

		KFbxTypedProperty<fbxDouble1>	UnitScaleFactor;
        KFbxTypedProperty<fbxDouble1>	OriginalUnitScaleFactor;

        KFbxTypedProperty<fbxDouble3>   AmbientColor;
        KFbxTypedProperty<fbxString>    DefaultCamera;
        KFbxTypedProperty<fbxEnum>      TimeMode;
        KFbxTypedProperty<fbxTime>      TimeSpanStart;
        KFbxTypedProperty<fbxTime>      TimeSpanStop;
        KFbxTypedProperty<fbxDouble1>   CustomFrameRate;
	//@}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
public:
	virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
	virtual void Construct(const KFbxGlobalSettings* pFrom);
	virtual bool ConstructProperties(bool pForceSet);
	
private:
	KFbxGlobalSettings(KFbxSdkManager& pManager, char const* pName);

    void AxisSystemToProperties();
    void PropertiesToAxisSystem();

    void Init();

    KFbxAxisSystem mAxisSystem;
    friend class KFbxScene;    

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 
};

inline EFbxType FbxTypeOf( KTime::ETimeMode const &pItem )             { return eENUM; }


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXGLOBALSETTINGS_H

