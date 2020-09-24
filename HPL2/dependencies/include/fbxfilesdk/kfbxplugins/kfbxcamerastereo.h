/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxcamerastereo.h
*/

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXCAMERASTEREO_H
#define FBXFILESDK_KFBXPLUGINS_KFBXCAMERASTEREO_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxcamera.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** This node attribute contains methods for accessing the properties of a stereo camera.
  * \nosubgrouping
  * Generally, a set of stereoRig contains the center camera, the left camera and the right camera.
  * KFbxCameraStereo is used to represent the center camera. The left and right camera could be KFbxCamera.
  * KFbxCameraStereo contains stereo properties.
  * The left and right camera can also be get and set via related methods in KFbxCameraStereo class.
  * \see KFbxCamera and KFbxCameraSwitcher.
  */
class KFBX_DLL KFbxCameraStereo : public KFbxCamera
{
    KFBXOBJECT_DECLARE(KFbxCameraStereo, KFbxCamera);

public:
    //! Return the type of node attribute which is EAttributeType::eCAMERA_STEREO.
    virtual EAttributeType GetAttributeType() const;

    //! Reset the stereo camera to default values.
    void Reset();

    /** Types of Stereo camera.
      */
    typedef enum
    {
        eNONE,          //!< Disable the stereo effect.(Default value)
        eCONVERGED,     //!< Computes the zero parallax plane by toeing in the cameras.
        eOFF_AXIS,      //!< Computes the convergence plane by shifting the frustum using camera film back.
        ePARALLEL       //!< A parallel camera setup where there is effectively no convergence plane.
    } ECameraStereo;

    /** Get the left camera which connect to property LeftCamera.
      * \return   A pointer to KFbxCamera.
      * \remarks  Current KFbxCameraStereo should work with two KFbxCamera, left camera and right camera.
      * Use this method to get the left camera.
      */
    KFbxCamera* GetLeftCamera() const;

    /** Get the right camera which connect to property RightCamera.
      * \return   A pointer to KFbxCamera.
      * \remarks  Current KFbxCameraStereo should work with two KFbxCamera, left camera and right camera.
      * Use this method to get the right camera.
      */
    KFbxCamera* GetRightCamera() const;

    /** Set the left camera, connect property LeftCamera to pCamera.
	  * \param pCamera The camera to set.
      * \return        \c true if it's successful, \c false otherwise.
      * \remarks Current KFbxCameraStereo should work with two KFbxCamera, left camera and right camera.
      * Use this method to set the left camera.
      */
    bool SetLeftCamera(KFbxCamera* pCamera);

    /** Set the right camera, connect property RightCamera to pCamera.
	  * \param pCamera The camera to set.
      * \return        \c true if it's successful, \c false otherwise.
      * \remarks Current KFbxCameraStereo should work with two KFbxCamera, left camera and right camera.
      * Use this method to set the right camera.
      */
    bool SetRightCamera(KFbxCamera* pCamera);

    /** Get the local transformation matrix of left camera.
      * \return The local transformation matrix of left camera.
      * \remarks Use this method to reevaluate the local transformation of left camera.
      */
    KFbxXMatrix GetLeftCameraLocalMatrix() const;

    /** Get the global matrix of left camera.
      * \return The global transformation matrix of left camera.
      * \remarks Use this method to reevaluate the global transformation of left camera.
      */
    KFbxXMatrix GetLeftCameraGlobalMatrix() const;

    /** Get the local transformation matrix of right camera.
      * \return The local transformation matrix of right camera..
      * \remarks Use this method to reevaluate the local transformation of right camera.
      */
    KFbxXMatrix GetRightCameraLocalMatrix() const;

    /** Get the global transformation matrix of right camera.
      * \return The global transformation matrix of right camera.
      * \remarks Use this method to reevaluate the global transformation of right camera.
      */
    KFbxXMatrix GetRightCameraGlobalMatrix() const;

    /** Reevaluate the FilmOffsetX of left camera.
      * It's computed through stereo camera properties.
      * \return Current FilmOffsetX value.
      * \remarks This method does not set the FilmOffsetX of left camera.
      */
    double ReevaluateLeftCameraFilmOffsetX() const;

    /** Reevaluate the FilmOffsetX of right camera.
      * It's computed through stereo camera properties.
      * \return Current FilmOffsetX value.
      * \remarks this method does not set the FilmOffsetX of right camera
      */
    double ReevaluateRightCameraFilmOffsetX() const;

    //////////////////////////////////////////////////////////////////////////
    //
    // Properties
    //
    //////////////////////////////////////////////////////////////////////////

    // -----------------------------------------------------------------------
    // Stereo and Stereo Adjustments
    // -----------------------------------------------------------------------

    /** This property handles the types of Stereo camera.
      *
      * To access this property do: Stereo.Get().
      * To set this property do: Stereo.Set(ECameraStereo).
      *
      * \remarks Default Value is eNONE.
      */
    KFbxTypedProperty<ECameraStereo>                    Stereo;

    /** This property handles the distance between left and right cameras.
      *
      * To access this property do: InteraxialSeparation.Get().
      * To set this property do: InteraxialSeparation.Set(fbxDouble1).
      *
      * \remarks Default Value is 0.0.
      */
    KFbxTypedProperty<fbxDouble1>                       InteraxialSeparation;

    /** This property handles the distance on the camera view axis where the zero parallax plane occurs.
      *
      * To access this property do: ZeroParallax.Get().
      * To set this property do: ZeroParallax.Set(fbxDouble1).
      *
      * \remarks Default Value is 0.0.
      */
    KFbxTypedProperty<fbxDouble1>                       ZeroParallax;

    /** This property is to offset the computed toe-in effect when it's in Converged mode.
      *
      * To access this property do: ToeInAdjust.Get().
      * To set this property do: ToeInAdjust.Set(fbxDouble1).
      *
      * \remarks Default Value is 0.0. 
      * This value is specified in degrees and acts as an offset to the computed toe-in.
	  * \see ECameraStereo.
      */
    KFbxTypedProperty<fbxDouble1>                       ToeInAdjust;

    /** This property handles the film offset for the right camera.
      *
      * To access this property do: FilmOffsetRightCam.Get().
      * To set this property do: FilmOffsetRightCam.Set(fbxDouble1).
      *
      * \remarks Default Value is 0.0.
      */
    KFbxTypedProperty<fbxDouble1>                       FilmOffsetRightCam;

    /** This property handles the film offset for the left camera.
      *
      * To access this property do: FilmOffsetLeftCam.Get().
      * To set this property do: FilmOffsetLeftCam.Set(fbxDouble1).
      *
      * \remarks Default Value is 0.0.
      */
    KFbxTypedProperty<fbxDouble1>                       FilmOffsetLeftCam;

    /** This property has the right camera connected to it.
      *
      * To access this property do: GetRightCamera().
      * To set this property do: SetRightCamera(KFbxCamera* pCamera).
      *
      * \remarks The right camera is connected as source object.
      */
    KFbxTypedProperty<fbxReference>                     RightCamera;

    /** This property has the left camera connected to it.
      *
      * To access this property do: GetLeftCamera().
      * To set this property do: SetLeftCamera(KFbxCamera* pCamera).
      *
      * \remarks The left camera is connected as source object.
      */
    KFbxTypedProperty<fbxReference>                     LeftCamera;


    ///////////////////////////////////////////////////////////////////////////////
    //
    //  WARNING!
    //
    //  Anything beyond these lines may not be documented accurately and is
    //  subject to change without notice.
    //
    ///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    /** This property handles the precomp file name
    *
    * To access this property do: PrecompFileName.Get().
    * To set this property do: PrecompFileName.Set(fbxString).
    *
    * Default value is ""
    */
    KFbxTypedProperty<fbxString>                        PrecompFileName;

    /** This property handles the relative precomp file name
    *
    * To access this property do: RelativePrecompFileName.Get().
    * To set this property do: RelativePrecompFileName.Set(fbxString).
    *
    * Default value is ""
    */
    KFbxTypedProperty<fbxString>                        RelativePrecompFileName;

    friend class KFbxGlobalCameraSettings;

public:
    /** connect left and right camera property to stereo camera.
    * \return true if it's successful, otherwise return false.
    * \remarks It's used to connect the left/right camera property [FocalLength, FarPlane, NearPlane, FilmWidth,
    * FilmHeight, FilmSqueezeRatio] to stereo camera.
    * During FBX SDK reevaluating, if ConnectProperties is called, 
    * to get the newest FocalLength property of left camera, please use lLeft_Camera->FocalLength.GetSrcProperty();
    */
    bool ConnectProperties();

protected:
    KFbxCameraStereo(KFbxSdkManager& pManager, char const* pName);
    virtual bool ConstructProperties(bool pForceSet);

    /**
    * Used to retrieve the KProperty list from an attribute
    */
    virtual KStringList GetTypeFlags() const;

private:
    // no private members

    friend class KFbxNode;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxCameraStereo* HKFbxCameraStereo;

inline EFbxType FbxTypeOf( KFbxCameraStereo::ECameraStereo const &pItem )         { return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXCAMERASTEREO_H

