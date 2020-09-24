/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxcamera.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXCAMERA_H
#define FBXFILESDK_KFBXPLUGINS_KFBXCAMERA_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxnodeattribute.h>
#include <fbxfilesdk/kfbxplugins/kfbxcolor.h>

#include <fbxfilesdk/kfbxmath/kfbxvector4.h>
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxTexture;
class KFbxSdkManager;
class KFbxMatrix;
class KFbxXMatrix;
class KTime;

/** This node attribute contains methods for accessing the properties of a camera.
  * \nosubgrouping
  * A camera can be set to automatically point at and follow
  * another node in the hierarchy. To do this, the focus source
  * must be set to ECameraFocusDistanceSource::eCAMERA_INTEREST and the
  * followed node associated with function KFbxNode::SetTarget().
  * \see KFbxCameraStereo and KFbxCameraSwitcher.
  */
class KFBX_DLL KFbxCamera : public KFbxNodeAttribute
{
    KFBXOBJECT_DECLARE(KFbxCamera,KFbxNodeAttribute);

public:
    //! Return the type of node attribute which is EAttributeType::eCAMERA.
    virtual EAttributeType GetAttributeType() const;

    //! Reset the camera to default values.
    void Reset();

    /** Camera projection types.
      * \remarks     By default, the camera projection type is set to ePERSPECTIVE.
      *              If the camera projection type is set to eORTHOGONAL, the following options
      *              are not relevant:
      *                   - aperture format
      *                   - aperture mode
      *                   - aperture width and height
      *                   - angle of view/focal length
      *                   - squeeze ratio
      */
    typedef enum
    {
        ePERSPECTIVE,  //!< Perspective projection.
        eORTHOGONAL    //!< Orthogonal projection.
    } ECameraProjectionType;


	/**
      * \name Functions to handle the viewing area.
      */
    //@{

        /** Camera formats identifiers.
		  * \remarks This is designed as the same as in MotionBuilder.
		  * \see SetFormat, GetFormat and CameraFormat.
          */
        typedef enum
        {
            eCUSTOM_FORMAT,   /*!< The format's width, height, or pixel ratio has been user-specified, 
			                       and matches none of the other picture formats.*/
            eD1_NTSC,         //!< Standard format for D1 NTSC (720 by 486).
            eNTSC,            //!< NTSC standard for North American television broadcast (640 by 480).
            ePAL,             //!< PAL standard for European television broadcast (570 by 486).
            eD1_PAL,          //!< Standard format for D1 PAL (720 by 576).
            eHD,              //!< HD format(1920 by 1080).
            e640x480,         //!< Recommended computer screen format (640 by 480).
            e320x200,         //!< Recommended format for World Wide Web production(320 by 200).
            e320x240,         //!< Alternate World Wide Web format(320 by 240).
            e128x128,         //!< Format(128 by 128)
            eFULL_SCREEN      //!< Full computer screen format (1280 by 1024 pixels).
        } ECameraFormat;

        /** Set the camera format.
          * \param pFormat     The camera format identifier.
          * \remarks           Changing the camera format sets the camera aspect
          *                    ratio mode to eFIXED_RESOLUTION and modifies the aspect width
          *                    size, height size, and pixel ratio accordingly.
          */
        void SetFormat(ECameraFormat pFormat);

        /** Get the camera format.
          * \return     The current camera format identifier.
          */
        ECameraFormat GetFormat() const;

        /** Camera's aspect ratio modes.
		  * \see SetAspect, GetAspectRatioMode, AspectWidth, AspectHeight and AspectRatioMode.
          */
        typedef enum
        {
            eWINDOW_SIZE,        //!< Both width and height values aren't relevant.
            eFIXED_RATIO,        //!< The height value is set to 1.0 and the width value is relative to the height value.
            eFIXED_RESOLUTION,   //!< Both width and height values are in pixels.
            eFIXED_WIDTH,        //!< The width value is in pixels and the height value is relative to the width value.
            eFIXED_HEIGHT        //!< The height value is in pixels and the width value is relative to the height value.
        } ECameraAspectRatioMode;

        /** Set the camera's aspect ratio mode.
          * \param pRatioMode     Camera's aspect ratio mode.
          * \param pWidth         Camera's aspect width, must be a positive value.
          * \param pHeight        Camera's aspect height, must be a positive value.
          * \remarks              Changing the camera aspect sets the camera format to eCustom.
		  * \see ECameraAspectRatioMode.
          */
        void SetAspect(ECameraAspectRatioMode pRatioMode, double pWidth, double pHeight);

        /** Get the camera aspect ratio mode.
          * \return     The current aspect ratio mode.
          */
        ECameraAspectRatioMode GetAspectRatioMode() const;

        /** Set the pixel ratio.
          * \param pRatio     The pixel ratio value.
          * \remarks          The value must be a positive number. Comprised between 0.05 and 20.0. Values
          *                   outside these limits will be clamped. Changing the pixel ratio sets the camera format to eCUSTOM_FORMAT.
          */
        void SetPixelRatio(double pRatio);

        /** Get the pixel ratio.
          * \return     The current camera's pixel ratio value.
          */
        double GetPixelRatio() const;

        /** Set the near plane distance from the camera.
          * The near plane is the minimum distance to render a scene on the camera display.
	      * A synonym for the near plane is "front clipping plane".
          * \param pDistance     The near plane distance value.
          * \remarks             The near plane value is limited to the range [0.001, 600000.0] and
          *                      must be inferior to the far plane value.
          */
        void SetNearPlane(double pDistance);

        /** Get the near plane distance from the camera.
          * The near plane is the minimum distance to render a scene on the camera display.
	      * A synonym for the near plane is "front clipping plane".
          * \return     The near plane value.
          */
        double GetNearPlane() const;

        /** Set the far plane distance from camera.
          * The far plane is the maximum distance to render a scene on the camera display.
	      * A synonym for the far plane is "back clipping plane".
          * \param pDistance     The far plane distance value.
          * \remarks             The far plane value is limited to the range [0.001, 600000.0] and
          *                      must be superior to the near plane value.
          */
        void SetFarPlane(double pDistance);

        /** Get the far plane distance from camera.
          * The far plane is the maximum distance to render a scene on the camera display.
	      * A synonym for the far plane is "back clipping plane".
          * \return     The far plane value.
          */
        double GetFarPlane() const;

    //@}

    /**
      * \name Aperture and Film Functions.
	  * In photography, the aperture is the size of hole allowing light from the lens to get through to the film. 
      * The aperture mode determines which values drive the camera aperture. When the aperture mode is \e eHORIZONTAL_AND_VERTICAL,
      * \e eHORIZONTAL or \e eVERTICAL, the field of view is used. When the aperture mode is \e eFOCAL_LENGTH, the focal length is used.
      *
      * It is possible to convert the aperture mode into field of view or vice versa using functions ComputeFieldOfView and
      * ComputeFocalLength. These functions use the camera aperture width and height for their computation.
      */
    //@{

    /** Camera's aperture formats.
   	  * \remarks This is designed as the same as in MotionBuilder.
	  * \see SetApertureFormat, GetApertureFormat, FilmFormat, FilmWidth, FilmHeight, FilmSqueezeRatio and FilmAspectRatio.
      */
    typedef enum
    {
		eCUSTOM_APERTURE_FORMAT = 0,   /*!< The film size, squeeze ratio and aspect ratio has been user-specified, 
									        and matches none of the other aperture formats.*/ 
        e16MM_THEATRICAL,              //!< Film Size: 0.404, 0.295 inches. Film Squeeze Ratio: 1.0. Film Aspect Ratio: 1.369.
        eSUPER_16MM,                   //!< Film Size: 0.493, 0.292 inches. Film Squeeze Ratio: 1.0. Film Aspect Ratio: 1.688.
        e35MM_ACADEMY,                 //!< Film Size: 0.864, 0.630 inches. Film Squeeze Ratio: 1.0. Film Aspect Ratio: 1.371. 
        e35MM_TV_PROJECTION,           //!< Film Size: 0.816, 0.612 inches. Film Squeeze Ratio: 1.0. Film Aspect Ratio: 1.333.
        e35MM_FULL_APERTURE,           //!< Film Size: 0.980, 0.735 inches. Film Squeeze Ratio: 1.0. Film Aspect Ratio: 1.333.
        e35MM_185_PROJECTION,          //!< Film Size: 0.825, 0.446 inches. Film Squeeze Ratio: 1.0. Film Aspect Ratio: 1.850.
        e35MM_ANAMORPHIC,              //!< Film Size: 0.864, 0.732 inches. Film Squeeze Ratio: 2.0. Film Aspect Ratio:1.180. 
        e70MM_PROJECTION,              //!< Film Size: 2.066, 0.906 inches. Film Squeeze Ratio: 1.0. Film Aspect Ratio: 2.280.
        eVISTAVISION,                  //!< Film Size: 1.485, 0.991 inches. Film Squeeze Ratio: 1.0. Film Aspect Ratio: 1.498.
        eDYNAVISION,                   //!< Film Size: 2.080, 1.480 inches. Film Squeeze Ratio: 1.0. Film Aspect Ratio: 1.405.
        eIMAX                          //!< Film Size: 2.772, 2.072 inches. Film Squeeze Ratio: 1.0. Film Aspect Ratio: 1.338.
    } ECameraApertureFormat;

    /** Set the camera aperture format.
      * \param pFormat     The camera aperture format identifier.
      * \remarks           Changing the aperture format modifies the aperture width, height, and squeeze ratio accordingly.
      */
    void SetApertureFormat(ECameraApertureFormat pFormat);

    /** Get the camera aperture format.
      * \return     The camera's current aperture format identifier.
      */
    ECameraApertureFormat GetApertureFormat() const;

    /** Camera aperture modes. 
      * The aperture mode determines which values drive the camera aperture. 
	  * If the aperture mode is \e eHORIZONTAL_AND_VERTICAL, \e eHORIZONTAL, or \e eVERTICAL, then the field of view is used. 
	  * If the aperture mode is \e eFOCAL_LENGTH, then the focal length is used.
      */
    typedef enum
    {
		eHORIZONTAL_AND_VERTICAL,  //!< Set the angle values for both the horizontal and vertical settings. 
        eHORIZONTAL,               //!< Set only the horizontal angle. 
        eVERTICAL,                 //!< Set only the vertical angle. 
        eFOCAL_LENGTH              //!< Use focal length directly. 
    } ECameraApertureMode;

    /** Set the camera aperture mode.
      * \param pMode     The camera aperture mode identifier.
      */
    void SetApertureMode(ECameraApertureMode pMode);

    /** Get the camera aperture mode.
      * \return     The camera's current aperture mode identifier.
      */
    ECameraApertureMode GetApertureMode() const;

    /** Set the camera aperture width in inches.
      * \param pWidth     The aperture width value.
      * \remarks          Must be a positive value. The minimum accepted value is 0.0001.
      *                   Changing the aperture width sets the camera aperture format to eCUSTOM_FORMAT.
      */
    void SetApertureWidth(double pWidth);

    /** Get the camera aperture width in inches.
      * \return     The camera's current aperture width value in inches.
      */
    double GetApertureWidth() const;

    /** Set the camera aperture height in inches.
      * \param pHeight     The aperture height value.
      * \remarks           Must be a positive value. The minimum accepted value is 0.0001.
      *                    Changing the aperture height sets the camera aperture format to eCUSTOM_FORMAT.
      */
    void SetApertureHeight(double pHeight);

    /** Get the camera aperture height in inches.
      * \return     The camera's current aperture height value in inches.
      */
    double GetApertureHeight() const;

    /** Set the squeeze ratio.
      * \param pRatio      The squeeze ratio value.
      * \remarks           Must be a positive value. The minimum accepted value is 0.0001.
      *                    Changing the squeeze ratio sets the camera aperture format to eCUSTOM_FORMAT.
      */
    void SetSqueezeRatio(double pRatio);

    /** Get the camera squeeze ratio.
      * \return     The camera's current squeeze ratio value.
      */
    double GetSqueezeRatio() const;

    /** Camera's gate fit modes. 
	  * There are two gates for a camera, film gate and resolution gate.
	  * Film gate is a border indicating the area of the camera's view as a real-world camera records on film. 
	  * The dimensions of the film gate represent the dimensions of the camera aperture. 
	  * But the film gate does not represent the render region.
	  * It is the resolution gate that represents the rendering resolution. 
	  * The gate fit mode controls the size of the resolution gate relative to the film gate. 
      */
    typedef enum
    {
        eNO_FIT,           //!< No resolution gate fit.
        eVERTICAL_FIT,     //!< Fit the resolution gate vertically within the film gate.
        eHORIZONTAL_FIT,   //!< Fit the resolution gate horizontally within the film gate.
        eFILL_FIT,         //!< Fit the resolution gate within the film gate.
        eOVERSCAN_FIT,     //!< Fit the film gate within the resolution gate.
        eSTRETCH_FIT       //!< Fit the resolution gate to the film gate.
    } ECameraGateFit;

    /** Compute the angle of view based on the given focal length, the aperture width, and aperture height.
      * \param pFocalLength     The focal length in millimeters.
      * \return                 The computed angle of view in degrees.
      */
    double ComputeFieldOfView(double pFocalLength) const;

    /** Compute the focal length based on the given angle of view, the aperture width, and aperture height.
      * \param pAngleOfView     The angle of view in degrees.
      * \return                 The computed focal length in millimeters.
      */
    double ComputeFocalLength(double pAngleOfView) const;

    /** Specifies how the roll is applied with respect to the pivot value.
      */
    typedef enum
    {
        eROTATE_TRANSLATE,   //!< The film back is first rotated then translated by the pivot point value.
        eTRANSLATE_ROTATE    //!< The film back is first translated then rotated by the film roll value.
    } ECameraFilmRollOrder;

    //@}

    /**
      * \name Functions to handle BackPlane/FrontPlane and Plate.
	  * 
	  * In the FbxSdk terminology, the Back/Front plane is the support of the plate. And the plate is
	  * the support of the texture used for backgrounds/foregrounds. Functions and properties 
	  * identified by the "Plate" name are affecting the display of the texture on the plate. 
	  * The functions and properties identified with the "Back/FrontPlane" are affecting the plate.
	  *
	  * Typically a client application would place the BackPlate a small distance in front of the 
	  * FarPlane and the FrontPlate just behind the NearPlane to avoid them to be hidden by the clipping.
	  * Unless otherwise noted, there are no restrictions on the values stored by the camera object
	  * therefore it is the responsibility of the client application to process the information in a 
	  * meaningful way and to maintain consistency between the different properties relationships.
      */
    //@{

    /** Set the associated background image file.
      * \param pFileName     The path of the background image file.
      * \remarks             The background image file name must be valid.
	  * \remarks             This method is still provided just for legacy files (Fbx version 5.0 and earlier)
	  *                      and must not be used in any other cases.
      */
    void SetBackgroundFileName(const char* pFileName);

    /** Get the background image file name.
      * \return     Pointer to the background filename string or \c NULL if not set.
	  * \remarks             This method is still provided just for legacy files (Fbx version 5.0 and earlier)
	  *                      and must not be used in any other cases.
      */
    char const* GetBackgroundFileName() const;

    /** Set the media name associated to the background image file.
      * \param pFileName     The media name of the background image file.
      * \remarks             The media name is a unique name used to identify the background image file.
	  * \remarks             This method is still provided just for legacy files (Fbx version 5.0 and earlier)
	  *                      and must not be used in any other cases.
      */
    void SetBackgroundMediaName(const char* pFileName);

    /** Get the media name associated to the background image file.
      * \return     Pointer to the media name string or \c NULL if not set.
	  * \remarks             This method is still provided just for legacy files (Fbx version 5.0 and earlier)
	  *                      and must not be used in any other cases.
      */
    char const* GetBackgroundMediaName() const;

    /** Set the associated foreground image file.
    * \param pFileName     The path of the foreground image file.
    * \remarks             The foreground image file name must be valid.
    * \remarks             This method is still provided just for legacy files (Fbx version 5.0 and earlier)
    *                      and must not be used in any other cases.
    */
    void SetForegroundFileName(const char* pFileName);

    /** Get the foreground image file name.
    * \return     Pointer to the foreground filename string or \c NULL if not set.
    * \remarks             This method is still provided just for legacy files (Fbx version 5.0 and earlier)
    *                      and must not be used in any other cases.
    */
    char const* GetForegroundFileName() const;

    /** Set the media name associated to the foreground image file.
    * \param pFileName     The media name of the foreground image file.
    * \remarks             The media name is a unique name used to identify the foreground image file.
    * \remarks             This method is still provided just for legacy files (Fbx version 5.0 and earlier)
    *                      and must not be used in any other cases.
    */
    void SetForegroundMediaName(const char* pFileName);

    /** Get the media name associated to the foreground image file.
    * \return     Pointer to the media name string or \c NULL if not set.
    * \remarks             This method is still provided just for legacy files (Fbx version 5.0 and earlier)
    *                      and must not be used in any other cases.
    */
    char const* GetForegroundMediaName() const;

	
	/** Image plate drawing modes.
      */
    typedef enum
    {
        eBACKGROUND,                  //!< Image is drawn behind models.
        eFOREGROUND,                  //!< Image is drawn in front of models based on alpha channel.
        eBACKGROUND_AND_FOREGROUND    //!< Image is drawn behind and in front of models depending on alpha channel.
    } ECameraPlateDrawingMode;

	/** Set front plate matte threshold.
      * \param pThreshold     Threshold value on a range from 0.0 to 1.0.
      * \remarks              This option is only relevant if the image plate drawing mode is set to eFOREGROUND or eBACKGROUND_AND_FOREGROUND.
      */
    void SetBackgroundAlphaTreshold(double pThreshold);

    /** Get front plate matte threshold.
      * \return      Threshold value on a range from 0.0 to 1.0.
      * \remarks     This option is only relevant if the image plate drawing mode is set to eFOREGROUND or eBACKGROUND_AND_FOREGROUND.
      */
    double GetBackgroundAlphaTreshold() const;

	/** Change the back plate fit image flag.
	  * If this flag is on, scale the back plate image to fit on the back plane. 
      * \param pFitImage    New value for the FitImage property.
      */
    void SetFitImage(bool pFitImage);

    /** Get the current back plate image flag.
   	  * If this flag is on, scale the back plate image to fit on the back plane. 
      * \return             The value of the FitImage property.
      */
    bool GetFitImage() const;

    /** Change the back plate crop flag.
	  * If this flag is on, crop the back plate image to fit on the back plane. 
	  * If the image is smaller than the plane, this flag has no effect.
      * \param pCrop          New value for the Crop property.
      */
    void SetCrop(bool pCrop);

    /** Get the current back plate crop flag.
	  * If this flag is on, crop the back plate image to fit on the back plane. 
	  * If the image is smaller than the plane, this flag has no effect.
      * \return               The value of the Crop property.
      */
    bool GetCrop() const;

    /** Change the back plate center flag.
	  * If this flag is on, center the back plate image on the back plane.
      * \param pCenter        New value for the BackPlateCenter property.
      */
    void SetCenter(bool pCenter);

    /** Get the current back plate center flag.
	  * If this flag is on, center the back plate image on the back plane.
      * \return               The value of the BackPlateCenter property.
      */
    bool GetCenter() const;

    /** Change the back plate keep ratio flag.
	  * If this flag is on, keep the aspect ratio of the back plate image. 
	  * Turn on both the keep ration flag and the fit image flag to scale the back plate image proportionately.
      * \param pKeepRatio     New value for the KeepRatio property.
      */
    void SetKeepRatio(bool pKeepRatio);

    /** Get the current back plate keep ratio flag.
	  * If this flag is on, keep the aspect ratio of the back plate image. 
	  * Turn on both the keep ration flag and the fit image flag to scale the back plate image proportionately.
      * \return               The value of the KeepRatio property.
      */
    bool GetKeepRatio() const;

	/** Enable or disable the display of the texture without the need to disconnect it from its plate.
      * \param pEnable     If \c true the texture is displayed, \c false otherwise.
      * \remarks           It is the responsibility of the client application to perform the required tasks according to the state
      *                    of this flag.
      */
    void SetShowFrontPlate(bool pEnable);

	/** Get the current state of the flag to display the front plate or not.
      * \return            \c true if show front plate is enabled, otherwise \c false.
      * \remarks           It is the responsibility of the client application to perform the required tasks according to the state
      *                    of this flag.
      */
    bool GetShowFrontPlate() const;
 
	/** Change the front plate fit image flag.
	  * If this flag is on, scale the front plate image to fit on the front plane. 
      * \param pFrontPlateFitImage	  New value for the FrontPlateFitImage property.
      */
    void SetFrontPlateFitImage(bool pFrontPlateFitImage);

    /** Get the current front plate fit image flag.
	  * If this flag is on, scale the front plate image to fit on the front plane. 
      * \return               The value of the BackPlateFitImage property.
      */
    bool GetFrontPlateFitImage() const;

    /** Change the front plate crop flag.
	  * If this flag is on, crop the front plate image to fit on the front plane. 
	  * If the image is smaller than the plane, this flag has no effect.
      * \param pFrontPlateCrop          New value for the FrontPlateCrop property.
      */
    void SetFrontPlateCrop(bool pFrontPlateCrop);

    /** Get the current front plate crop flag.
	  * If this flag is on, crop the front plate image to fit on the front plane. 
	  * If the image is smaller than the plane, this flag has no effect.
      * \return               The value of the FrontPlateCrop property.
      */
    bool GetFrontPlateCrop() const;

    /** Change the front plate center flag.
	  * If this flag is on, center the front plate image on the front plane.
      * \param pFrontPlateCenter		  New value for the FrontPlateCenter property.
      */
    void SetFrontPlateCenter(bool pFrontPlateCenter);

    /** Get the current front plate center flag.
	  * If this flag is on, center the front plate image on the front plane.
      * \return               The value of the FrontPlateCenter property.
      */
    bool GetFrontPlateCenter() const;

    /** Change the front plate keep ratio flag.
	  * If this flag is on, keep the aspect ratio of the front plate image. 
	  * Turn on both the keep ration flag and the fit image flag to scale the front plate image proportionately.
      * \param pFrontPlateKeepRatio     New value for the FrontPlateKeepRatio property.
      */
    void SetFrontPlateKeepRatio(bool pFrontPlateKeepRatio);

    /** Get the current front plate keep ratio flag.
	  * If this flag is on, keep the aspect ratio of the front plate image. 
	  * Turn on both the keep ration flag and the fit image flag to scale the front plate image proportionately.
      * \return               The value of the FrontPlateKeepRatio property.
      */
    bool GetFrontPlateKeepRatio() const;

    /** Set the front plate opacity value.
      * \param pOpacity       New value for the ForegroundOpacity property.
      */
    void SetForegroundOpacity(double pOpacity);

    /** Get the front plate opacity value.
      * \return               The value of the ForegroundOpacity property.
      */
    double GetForegroundOpacity() const;

    /** Attach the texture to the front plate.
      * \param pTexture       The pointer to the texture to attach.
      */
    void SetForegroundTexture(KFbxTexture* pTexture);

    /** Get the texture connected to the front plate.
      * \return     A pointer to the texture attached to front plate.
      */
    KFbxTexture* GetForegroundTexture() const;

	/** Front and BackPlane distance modes.
	  * \see SetBackPlaneDistanceMode and GetBackPlaneDistanceMode.
      */
    typedef enum
    {
        eRELATIVE_TO_INTEREST, //!< The back plane distance is measured in relation to the camera interest.
        eRELATIVE_TO_CAMERA    //!< The back plane distance is measured in relation to the camera.
    } ECameraFrontBackPlaneDistanceMode;

	/** Set the back plane distance mode.
      * \param pMode    The back plane distance mode to set.
      */
    void SetBackPlaneDistanceMode(ECameraFrontBackPlaneDistanceMode pMode);

    /** Get the back plane distance mode.
      * \return     Return the back plane distance mode.
      */
    ECameraFrontBackPlaneDistanceMode GetBackPlaneDistanceMode() const;

	/** Set the front plane distance from the camera. The the absolute position of the plane must be calculated
	  * by taking into consideration of the FrontPlaneDistanceMode.
      * \param pDistance    The front plane distance value.
	  * \remarks			It is the responsibility of the client application to ensure that this plane position is 
	  *                     within the frustum boundaries.
      */
    void SetFrontPlaneDistance(double pDistance);

    /** Get the front plane distance value.
      * \return double      The front plane distance value.
      */
    double GetFrontPlaneDistance() const;

    /** Set the front plane distance mode.
      * \param pMode        The front plane distance mode to set.
      */
    void SetFrontPlaneDistanceMode(ECameraFrontBackPlaneDistanceMode pMode);

    /** Get the front plane distance mode flag.
      * \return     The front plane distance mode.
      */
    ECameraFrontBackPlaneDistanceMode GetFrontPlaneDistanceMode() const;

    /** Front/back plane display modes.
      */
    typedef enum
    {
        eDISABLED,    //!< Disables the front/back plane whether a texture is being projected or not.
        eALWAYS,      //!< Always shows the front/back plane, even if no texture has been added.
        eWHEN_MEDIA   //!< Shows the front/back plane only if a texture has been added.
    } ECameraFrontBackPlaneDisplayMode;
	
    /** Set the front plane display mode. This mode can be used by the client application to
	  * decide under which circumstance the front plane should be drawn in the viewport.
      * \param pMode        The front/back plane display mode.
      */
    void SetViewFrustumFrontPlaneMode(ECameraFrontBackPlaneDisplayMode pMode);

    /** Get the front plane display mode.
      * \return     The front/back plane display mode.
      */
    ECameraFrontBackPlaneDisplayMode GetViewFrustumFrontPlaneMode() const;

    /** Set the back plane display mode. This mode can be used by the client application to
	  * decide under which circumstance the back plane should be drawn in the viewport.
      * \param pMode        The front/back plane display mode.
      */
    void SetViewFrustumBackPlaneMode(ECameraFrontBackPlaneDisplayMode pMode);

    /** Get the back plane display mode.
      * \return     The front/back plane display mode.
      */
    ECameraFrontBackPlaneDisplayMode GetViewFrustumBackPlaneMode() const;
    
    //@}

    /**
      * \name Camera View Functions
      * It is the responsibility of the client application to perform the required tasks according to the state
      * of the options that are either set or returned by these methods.
      */
    //@{

    /** Change the camera interest visibility flag.
      * \param pEnable     Set to \c true if the camera interest is shown, \c false otherwise.
      */
    void SetViewCameraInterest(bool pEnable);

    /** Get current visibility state of the camera interest.
      * \return     \c true if the camera interest is shown, or \c false if hidden.
      */
    bool GetViewCameraInterest() const;

	/** Change the camera near and far planes visibility flag.
      * \param pEnable      Set to \c true if the near and far planes are shown, \c false otherwise.
      */
    void SetViewNearFarPlanes(bool pEnable);

    /** Get current visibility state of the camera near and far planes.
      * \return     \c true if the near and far planes are shown, \c false otherwise.
      */
    bool GetViewNearFarPlanes() const;

    /** Camera safe area display styles.
      */
    typedef enum
    {
		eROUND = 0,   //!< Rounded safe area.
        eSQUARE = 1   //!< Square safe area.
    } ECameraSafeAreaStyle;

    //@}

    /**
      * \name Render Functions
      * It is the responsibility of the client application to perform the required tasks according to the state
      * of the options that are either set or returned by these methods.
      */
    //@{

    /** Render options usage time.
      */
    typedef enum
    {
        eINTERACTIVE,   //!< To render in real time.
        eAT_RENDER      //!< Only render when it is asked.
    } ECameraRenderOptionsUsageTime;

    /** Anti-aliasing methods.
      */
    typedef enum
    {
        eOVERSAMPLING_ANTIALIASING,  //!< To do anti-aliasing by oversampling.
        eHARDWARE_ANTIALIASING       //!< To do anti-aliasing by hardware.
    } ECameraAntialiasingMethod;

    /** Oversampling types for anti-aliasing.
      */
    typedef enum
    {
		eUNIFORM,    /*!< The Uniform method samples each pixel at the same location. 
					      The pixel is divided into equal parts, and each part is sampled. 
					      The number of samples determines the number of times the pixel is divided.*/
		eSTOCHASTIC  /*!< The Stochastic method randomly samples each pixel. 
					      This produces an accurate color using a small number of samples.*/
    } ECameraSamplingType;

    /** Camera focus sources, that is the focal point for the depth of field.
	  * \see FocusDistance.
      */
    typedef enum
    {
        eCAMERA_INTEREST,     /*!< Base the depth of field on the camera interest. Models at the camera interest are in focus. 
							       As you move toward or away from the camera interest, models become increasingly blurred.*/
        eSPECIFIC_DISTANCE    //!< Base the depth of field on a point defined by a specific distance from the camera interest.
    } ECameraFocusDistanceSource;

    //@}

    /**
      * \name Utility Functions.
      */
    //@{

    /** Determine if the given bounding box is in the camera's view. 
      * The input points do not need to be ordered in any particular way.
      * \param pWorldToScreen The world to screen transformation. Please refer to KFbxCamera::ComputeWorldToScreen.
      * \param pWorldToCamera The world to camera transformation. 
               Inverse of the matrix returned from KFbxAnimEvaluator::GetNodeGlobalTransform or KFbxAnimEvaluator::GetNodeGlobalTransformFast is suitable.
               Please refer to KFbxScene::GetEvaluator and KFbxAnimEvaluator::GetNodeGlobalTransform or KFbxAnimEvaluator::GetNodeGlobalTransformFast.
      * \param pPoints 8 corners of the bounding box.
      * \return \c true if any of the given points are in the camera's view, \c false otherwise.
      */
    bool IsBoundingBoxInView( const KFbxMatrix& pWorldToScreen, 
                             const KFbxMatrix& pWorldToCamera, 
                             const KFbxVector4 pPoints[8] ) const;

    /** Determine if the given 3d point is in the camera's view. 
      * \param pWorldToScreen The world to screen transformation. Please refer to KFbxCamera::ComputeWorldToScreen.
      * \param pWorldToCamera The world to camera transformation. 
               Inverse of the matrix returned from KFbxAnimEvaluator::GetNodeGlobalTransform or KFbxAnimEvaluator::GetNodeGlobalTransformFast is suitable.
               Please refer to KFbxScene::GetEvaluator and KFbxAnimEvaluator::GetNodeGlobalTransform or KFbxAnimEvaluator::GetNodeGlobalTransformFast.
      * \param pPoint World-space point to test.
      * \return \c true if the given point is in the camera's view, \c false otherwise.
      */
    bool IsPointInView( const KFbxMatrix& pWorldToScreen, const KFbxMatrix& pWorldToCamera, const KFbxVector4& pPoint ) const;

    /** Compute world space to screen space transformation matrix.
      * \param pPixelHeight   The pixel height of the output image.
      * \param pPixelWidth    The pixel height of the output image.
      * \param pWorldToCamera The world to camera affine transformation matrix.
      * \return The world to screen space matrix, or the identity matrix on error. 
      */
    KFbxMatrix ComputeWorldToScreen(int pPixelWidth, int pPixelHeight, const KFbxXMatrix& pWorldToCamera) const;

    /** Compute the perspective matrix for this camera. 
      * Suitable for transforming camera space to normalized device coordinate space.
      * Also suitable for use as an OpenGL projection matrix. Note this fails if the
      * ProjectionType is not ePERSPECTIVE. 
      * \param pPixelHeight            The pixel height of the output image.
      * \param pPixelWidth             The pixel width of the output image.
      * \param pIncludePostPerspective Indicate that post-projection transformations (offset, roll) 
      *        are included in the output matrix.
      * \return A perspective matrix, or the identity matrix on error.
      */
    KFbxMatrix ComputePerspective( int pPixelWidth, int pPixelHeight, bool pIncludePostPerspective ) const;

    //@}

    //////////////////////////////////////////////////////////////////////////
    //
    // Properties
    //
    //////////////////////////////////////////////////////////////////////////

    // -----------------------------------------------------------------------
    // Geometrical
    // -----------------------------------------------------------------------

    /** This property handles the camera's position (XYZ coordinates).
      *
      * To access This property do: Position.Get().
      * To set This property do: Position.Set(fbxDouble3).
      *
      * \remarks Default Value is (0.0, 0.0, 0.0).
      */
    KFbxTypedProperty<fbxDouble3>                       Position;

    /** This property handles the camera's Up Vector (XYZ coordinates).
      *
      * To access This property do: UpVector.Get().
      * To set This property do: UpVector.Set(fbxDouble3).
      *
      * \remarks Default Value is (0.0, 1.0, 0.0).
      */
    KFbxTypedProperty<fbxDouble3>                       UpVector;

    /** This property handles the default point (XYZ coordinates) the camera is looking at.
      *
      * To access This property do: InterestPosition.Get().
      * To set This property do: InterestPosition.Set(fbxDouble3).
      *
      * \remarks During the computations of the camera position
      * and orientation, this property is overridden by the
      * position of a valid target in the parent node.
      *
      * \remarks Default Value is (0.0, 0.0, 0.0).
      */
    KFbxTypedProperty<fbxDouble3>                       InterestPosition;

    /** This property handles the camera roll angle in degrees.
      *
      * To access This property do: Roll.Get().
      * To set This property do: Roll.Set(fbxDouble1).
      *
      * Default value is 0.0.
      */
    KFbxTypedProperty<fbxDouble1>                       Roll;

    /** This property handles the camera optical center X, in pixels.
      * It sets horizontal offset of the optical center.
	  * When the camera's aperture mode is set to \e eVERTICAL, this property has no effect.
      *
      * To access This property do: OpticalCenterX.Get().
      * To set This property do: OpticalCenterX.Set(fbxDouble1).
      *
      * Default value is 0.0.
      */
    KFbxTypedProperty<fbxDouble1>                       OpticalCenterX;

    /** This property handles the camera optical center Y, in pixels.
      * It sets the vertical offset of the optical center. 
	  * When the camera's aperture mode is set to \e eHORIZONTAL, this property has no effect.
      *
      * To access This property do: OpticalCenterY.Get().
      * To set This property do: OpticalCenterY.Set(fbxDouble1).
      *
      * Default value is 0.0.
      */
    KFbxTypedProperty<fbxDouble1>                       OpticalCenterY;

    /** This property handles the RGB values of the camera's background color.
      *
      * To access This property do: BackgroundColor.Get().
      * To set This property do: BackgroundColor.Set(fbxDouble3).
      *
      * Default value is black (0, 0, 0)
      */
    KFbxTypedProperty<fbxDouble3>                       BackgroundColor;

    /** When modeling 3D objects, you often need to review or evaluate your models during the creation process. 
	  * You may create a camera with turn table animation to view your models in 360 or certain degrees.
	  * This property handles the camera's turn table angle in degrees.
	  *
      * To access This property do: TurnTable.Get().
      * To set This property do: TurnTable.Set(fbxDouble1).
      *
      * Default value is 0.
      */
    KFbxTypedProperty<fbxDouble1>                       TurnTable;

    /** This property handles a flag that indicates if the camera displays the
      * Turn Table icon or not.
      *
      * To access This property do: DisplayTurnTableIcon.Get().
      * To set This property do: DisplayTurnTableIcon.Set(fbxBool1).
      *
      * Default value is false (no display).
      */
    KFbxTypedProperty<fbxBool1>                         DisplayTurnTableIcon;

    // -----------------------------------------------------------------------
    // Motion Blur
    // -----------------------------------------------------------------------

    /** This property handles a flag that indicates if the camera uses
      * motion blur or not.
      *
      * To access This property do: UseMotionBlur.Get().
      * To set This property do: UseMotionBlur.Set(fbxBool1).
      *
      * Default value is false (do not use motion blur).
      */
    KFbxTypedProperty<fbxBool1>                         UseMotionBlur;

    /** This property handles a flag that indicates if the camera uses
      * real time motion blur or not.
      *
      * To access This property do: UseRealTimeMotionBlur.Get().
      * To set This property do: UseRealTimeMotionBlur.Set(fbxBool1).
      *
      * Default value is false (use real time motion blur).
      */
    KFbxTypedProperty<fbxBool1>                         UseRealTimeMotionBlur;

    /** This property handles the camera's motion blur intensity (in pixels).
      *
      * To access This property do: MotionBlurIntensity.Get().
      * To set This property do: MotionBlurIntensity.Set(fbxDouble1).
      *
      * Default value is 1.0.
      */
    KFbxTypedProperty<fbxDouble1>                       MotionBlurIntensity;

    // -----------------------------------------------------------------------
    // Optical
    // -----------------------------------------------------------------------

    /** This property handles the camera's aspect ratio mode.
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetAspect() if you want to change its value.
      *
      * Default value is eWINDOW_SIZE.
      *
      */
    KFbxTypedProperty<ECameraAspectRatioMode>           AspectRatioMode;

    /** This property handles the camera's aspect width.
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetAspect() if you want to change its value.
      *
      * Default value is 320.
      */
    KFbxTypedProperty<fbxDouble1>                       AspectWidth;

    /** This property handles the camera's aspect height.
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetAspect() if you want to change its value.
      *
      * Default value is 200.
      */
    KFbxTypedProperty<fbxDouble1>                       AspectHeight;

    /** This property handles the pixel aspect ratio.
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetPixelRatio() if you want to change its value.
	  *
      * Default value is 1.
      * \remarks Value range is [0.050, 20.0].
      */
    KFbxTypedProperty<fbxDouble1>                       PixelAspectRatio;

    /** This property handles the aperture mode.
      *
	  * To access This property do: ApertureMode.Get().
	  * To set This property do: ApertureMode.Set(ECameraApertureMode).
	  *
      * Default value is eVERTICAL.
      */
    KFbxTypedProperty<ECameraApertureMode>              ApertureMode;

    /** This property handles the gate fit mode.
      * To control the size of the resolution gate relative to the film gate. 
	  * If the resolution gate and the film gate have the same aspect ratio, then the property has no effect. 
	  *
      * To access This property do: GateFit.Get().
      * To set This property do: GateFit.Set(ECameraGateFit).
      *
      * Default value is eNO_FIT.
      */
    KFbxTypedProperty<ECameraGateFit>                   GateFit;

    /** This property handles the field of view in degrees.
      *
      * To access This property do: FieldOfView.Get().
      * To set This property do: FieldOfView.Set(fbxDouble1).
      *
      * \remarks This property has meaning only when
      * property ApertureMode equals eHORIZONTAL or eVERTICAL.
      *
      * \remarks Default value is 40.
      * \remarks Value range is [1.0, 179.0].
      */
    KFbxTypedProperty<fbxDouble1>                       FieldOfView;

    /** This property handles the X (horizontal) field of view in degrees.
      *
      * To access This property do: FieldOfViewX.Get().
      * To set This property do: FieldOfViewX.Set(fbxDouble1).
      *
      * \remarks This property has meaning only when
      * property ApertureMode equals eHORIZONTAL_AND_VERTICAL.
      *
      * Default value is 1.
      * \remarks Value range is [1.0, 179.0].
      */
    KFbxTypedProperty<fbxDouble1>                       FieldOfViewX;

    /** This property handles the Y (vertical) field of view in degrees.
      *
      * To access This property do: FieldOfViewY.Get().
      * To set This property do: FieldOfViewY.Set(fbxDouble1).
      *
      * \remarks This property has meaning only when
      * property ApertureMode equals eHORIZONTAL_AND_VERTICAL.
      *
      * \remarks Default value is 1.
      * \remarks Value range is [1.0, 179.0].
      */
    KFbxTypedProperty<fbxDouble1>                       FieldOfViewY;

    /** This property handles the focal length (in millimeters).
      *
      * To access This property do: FocalLength.Get().
      * To set This property do: FocalLength.Set(fbxDouble1).
      *
      * Default value is the result of ComputeFocalLength(40.0).
      */
    KFbxTypedProperty<fbxDouble1>                       FocalLength;

    /** This property handles the camera's format.
      *
      * To access This property do: CameraFormat.Get().
      * To set This property do: CameraFormat.Set(ECameraFormat).
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetFormat() if you want to change its value.
	  *
      * Default value is eCUSTOM_FORMAT.
      */
    KFbxTypedProperty<ECameraFormat>                    CameraFormat;

    // -----------------------------------------------------------------------
    // Frame
    // -----------------------------------------------------------------------

    /** This property stores a flag that indicates to draw a border with color around the camera's viewable area or not.
      * To access This property do: UseFrameColor.Get().
      * To set This property do: UseFrameColor.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         UseFrameColor;

	/** This property is used to define the color of the border around the camera view.
      *
      * To access This property do: FrameColor.Get().
      * To set This property do: FrameColor.Set(fbxDouble3).
      *
      * Default value is (0.3, 0.3, 0.3).
      */
    KFbxTypedProperty<fbxDouble3>                       FrameColor;

    // -----------------------------------------------------------------------
    // On Screen Display
    // -----------------------------------------------------------------------

    /** This property handles the flag to show the camera's name or not.
      *
      * To access This property do: ShowName.Get().
      * To set This property do: ShowName.Set(fbxBool1).
      *
      * Default value is true.
      */
    KFbxTypedProperty<fbxBool1>                         ShowName;

    /** This property handles the flag to show info on moving or not.
      *
      * To access This property do: ShowInfoOnMoving.Get().
      * To set This property do: ShowInfoOnMoving.Set(fbxBool1).
      *
      * Default value is true.
      */
    KFbxTypedProperty<fbxBool1>                         ShowInfoOnMoving;

    /** This property handles the flag to draw floor grid or not.
      *
      * To access This property do: ShowGrid.Get().
      * To set This property do: ShowGrid.Set(fbxBool1).
      *
      * Default value is true.
      */
    KFbxTypedProperty<fbxBool1>                         ShowGrid;

    /** This property handles the flag to show optical center or not.
      *
      * To access This property do: ShowOpticalCenter.Get().
      * To set This property do: ShowOpticalCenter.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         ShowOpticalCenter;

    /** This property handles the flag to show the camera's sight line or not.
	  * When the camera is revolved about the center of interest in the perspective view,
	  * the angle of a camera's sight line relative to a plane perpendicular to the ground plane is referred to as its azimuth;
	  * and the angle of a camera's sight line relative to the ground plane is referred to as its elevation; 
      *
      * To access This property do: ShowAzimut.Get().
      * To set This property do: ShowAzimut.Set(fbxBool1).
      *
      * Default value is true.
      */
    KFbxTypedProperty<fbxBool1>                         ShowAzimut;

    /** This property handles the flag to show time code or not.
      *
      * To access This property do: ShowTimeCode.Get().
      * To set This property do: ShowTimeCode.Set(fbxBool1).
      *
      * Default value is true.
      */
    KFbxTypedProperty<fbxBool1>                         ShowTimeCode;

    /** This property handles the flag to show audio or not.
      *
      * To access This property do: ShowAudio.Get().
      * To set This property do: ShowAudio.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         ShowAudio;

    /** This property handles audio color.
      *
      * To access This property do: AudioColor.Get().
      * To set This property do: AudioColor.Set(fbxDouble3).
      *
      * Default value is (0.0, 1.0, 0.0).
      */
    KFbxTypedProperty<fbxDouble3>                       AudioColor;

    // -----------------------------------------------------------------------
    // Clipping Planes
    // -----------------------------------------------------------------------

    /** This property handles the near plane distance.
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetNearPlane() if you want to change its value.
	  *
      * Default value is 10.
      * \remarks Value range is [0.001, 600000.0].
      */
    KFbxTypedProperty<fbxDouble1>                       NearPlane;

    /** This property handles the far plane distance.
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetFarPlane() if you want to change its value.
	  *
      * Default value is 4000.
      * \remarks Value range is [0.001, 600000.0].
      */
    KFbxTypedProperty<fbxDouble1>                       FarPlane;

    /** This property indicates that the clip planes should be automatically computed or not.
      *
      * To access This property do: AutoComputeClipPlanes.Get().
      * To set This property do: AutoComputeClipPlanes.Set(fbxBool1).
      *
      * When This property is set to true, the NearPlane and FarPlane values are
      * ignored. Note that not all applications support this flag.
      */
    KFbxTypedProperty<fbxBool1>                         AutoComputeClipPlanes;


    // -----------------------------------------------------------------------
    // Camera Film Setting
    // -----------------------------------------------------------------------

    /** This property handles the film aperture width (in inches).
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetApertureWidth()
      * or SetApertureFormat() if you want to change its value.
	  *
      * Default value is 0.8160.
      * \remarks Value range is [0.0001, +inf].
      */
    KFbxTypedProperty<fbxDouble1>                       FilmWidth;

    /** This property handles the film aperture height (in inches).
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetApertureHeight()
      * or SetApertureFormat() if you want to change its value.
	  *
      * Default value is 0.6120.
      * \remarks Value range is [0.0001, +inf].
      */
    KFbxTypedProperty<fbxDouble1>                       FilmHeight;

    /** This property handles the film aperture aspect ratio.
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetApertureFormat() if you want to change its value.
	  *
      * Default value is (FilmWidth / FilmHeight).
      * \remarks Value range is [0.0001, +inf].
      */
    KFbxTypedProperty<fbxDouble1>                       FilmAspectRatio;

    /** This property handles the film aperture squeeze ratio.
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetSqueezeRatio()
      * or SetApertureFormat() if you want to change its value.
	  *
      * Default value is 1.0.
      * \remarks Value range is [0.0001, +inf].
      */
    KFbxTypedProperty<fbxDouble1>                       FilmSqueezeRatio;

    /** This property handles the film aperture format.
      *
      * \remarks This property is read-only.
      * \remarks Please use function SetApertureFormat()
      * if you want to change its value.
	  *
      * Default value is eCUSTOM_APERTURE_FORMAT.
      */
    KFbxTypedProperty<ECameraApertureFormat>            FilmFormat;

    /** This property handles the horizontal offset from the center of the film aperture,
    * defined by the film height and film width. The offset is measured in inches.
    *
    * To access This property do: FilmOffsetX.Get().
    * To set This property do: FilmOffsetX.Set(fbxDouble1).
    *
	* Default value is 0.0.
    */
    KFbxTypedProperty<fbxDouble1>                       FilmOffsetX;

    /** This property handles the vertical offset from the center of the film aperture,
    * defined by the film height and film width. The offset is measured
    * in inches.
    *
    * To access This property do: FilmOffsetY.Get().
    * To set This property do: FilmOffsetY.Set(fbxDouble1).
    *
	* Default value is 0.0.
    */
    KFbxTypedProperty<fbxDouble1>                       FilmOffsetY;

    /** This property handles the pre-scale value. 
      * The value is multiplied against the computed projection matrix. 
      * It is applied before the film roll. 
	  *
      * To access This property do: PreScale.Get().
      * To set This property do: PreScale.Set(fbxDouble1).
	  *
      * Default value is 1.0.
      */
    KFbxTypedProperty<fbxDouble1>                       PreScale;

    /** This property handles the horizontal film horizontal translation.
      * To access This property do: FilmTranslateX.Get().
      * To set This property do: FilmTranslateX.Set(fbxDouble1).
      * Default value is 0.0
      */
    KFbxTypedProperty<fbxDouble1>                       FilmTranslateX;

    /** This property handles the vertical film translation.
	  *
      * To access This property do: FilmTranslateY.Get().
      * To set This property do: FilmTranslateY.Set(fbxDouble1).
	  *
      * Default value is 0.0.
      */
    KFbxTypedProperty<fbxDouble1>                       FilmTranslateY;

    /** This property handles the horizontal pivot point used for rotating the film back.
	  *
      * To access This property do: FilmRollPivotX.Get().
      * To set This property do: FilmRollPivotX.Set(fbxDouble1).
	  *
      * Default value is 0.0.
      * \remarks FilmRollPivot value is used to compute the film roll matrix, which is a component of the post projection matrix.
      */
    KFbxTypedProperty<fbxDouble1>                       FilmRollPivotX;

    /** This property handles the vertical pivot point used for rotating the film back.
	  *
      * To access This property do: FilmRollPivotY.Get().
      * To set This property do: FilmRollPivotY.Set(fbxDouble1).
      *
	  * Default value is 0.0.
      * \remarks FilmRollPivot value is used to compute the film roll matrix, which is a component of the post projection matrix.
      */
    KFbxTypedProperty<fbxDouble1>                       FilmRollPivotY;

    /** This property handles the amount of rotation around the film back. 
      * The roll value is specified in degrees.
	  *
      * To access This property do: FilmRollValue.Get().
      * To set This property do: FilmRollValue.Set(fbxDouble1).
	  *
      * Default value is 0.0.
      * \remarks The rotation occurs around the specified pivot point, 
      * this value is used to compute a film roll matrix, which is a component of the post-projection matrix. 
      */
    KFbxTypedProperty<fbxDouble1>                       FilmRollValue;

    /** This property handles how the roll is applied with respect to the pivot value.
      * eROTATE_TRANSLATE    The film back is first rotated then translated by the pivot point value.
      * eTRANSLATE_ROTATE    The film back is first translated then rotated by the film roll value.
	  *
      * To access This property do: FilmRollOrder.Get().
      * To set This property do: FilmRollOrder.Set(ECameraFilmRollOrder).
	  *
      * Default value is eROTATE_TRANSLATE.
      */
    KFbxTypedProperty<ECameraFilmRollOrder>             FilmRollOrder ;

    // -----------------------------------------------------------------------
    // Camera View Widget Option
    // -----------------------------------------------------------------------

    /** This property handles the camera's look-at flag.
      * If this flag is on, the camera will look at the camera interest.  
	  *
      * To access This property do: ViewCameraToLookAt.Get().
      * To set This property do: ViewCameraToLookAt.Set(fbxBool1).
      *
      * Default value is true.
      */
    KFbxTypedProperty<fbxBool1>                         ViewCameraToLookAt;

	/** This property handles to display the near and far plane or not.
      *
      * To access This property do: ViewFrustumNearFarPlane.Get().
      * To set This property do: ViewFrustumNearFarPlane.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         ViewFrustumNearFarPlane;

    /** This property handles the back plane display mode.
      *
      * To access This property do: ViewFrustumBackPlaneMode.Get().
      * To set This property do: ViewFrustumBackPlaneMode.Set(ECameraFrontBackPlaneDisplayMode).
      *
      * Default value is eWHEN_MEDIA.
      */
    KFbxTypedProperty<ECameraFrontBackPlaneDisplayMode>	ViewFrustumBackPlaneMode;

    /** This property handles the back plane distance.
      *
      * To access This property do: BackPlaneDistance.Get().
      * To set This property do: BackPlaneDistance.Set(fbxDouble1).
      *
      * Default value is 100.0.
      */
    KFbxTypedProperty<fbxDouble1>                       BackPlaneDistance;

    /** This property handles the back plane distance mode.
      *
      * To access This property do: BackPlaneDistanceMode.Get().
      * To set This property do: BackPlaneDistanceMode.Set(ECameraFrontBackPlaneDistanceMode).
      *
      * Default value is eRELATIVE_TO_INTEREST.
      */
    KFbxTypedProperty<ECameraFrontBackPlaneDistanceMode>	BackPlaneDistanceMode;

    /** This property handles the front plane mode.
      *
      * To access This property do: ViewFrustumFrontPlaneMode.Get().
      * To set This property do: ViewFrustumFrontPlaneMode.Set(ECameraFrontBackPlaneDisplayMode).
      *
      * Default value is eWHEN_MEDIA.
      */
    KFbxTypedProperty<ECameraFrontBackPlaneDisplayMode>	ViewFrustumFrontPlaneMode;

    /** This property handles the front plane distance.
      *
      * To access This property do: FrontPlaneDistance.Get().
      * To set This property do: FrontPlaneDistance.Set(fbxDouble1).
      *
      * Default value is 100.0.
      */
    KFbxTypedProperty<fbxDouble1>                       FrontPlaneDistance;

    /** This property handles the front plane distance mode.
      *
      * To access This property do: FrontPlaneDistanceMode.Get().
      * To set This property do: FrontPlaneDistanceMode.Set(ECameraFrontBackPlaneDistanceMode).
      *
      * Default value is eRELATIVE_TO_INTEREST.
      */
    KFbxTypedProperty<ECameraFrontBackPlaneDistanceMode>	FrontPlaneDistanceMode;

    // -----------------------------------------------------------------------
    // Camera Lock Mode
    // -----------------------------------------------------------------------

    /** This property handles the flag to lock the camera's navigation.
	  * When this flag is on, the camera's view can not be changed anymore.
      * To access This property do: LockMode.Get().
      * To set This property do: LockMode.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         LockMode;

    /** This property handles the flag to lock the camera interest's navigation.
      * When this flag is one, the position of the camera interest is locked.
      * To access This property do: LockInterestNavigation.Get().
      * To set This property do: LockInterestNavigation.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         LockInterestNavigation;

    // -----------------------------------------------------------------------
    // Background Image Display Options
    // -----------------------------------------------------------------------

    /** This property handles the fit image flag of back plane.
      *
      * To access This property do: FitImage.Get().
      * To set This property do: FitImage.Set(fbxBool1).
      *
      * Default value is false.
	  * \see SetFitImage and GetFitImage.
      */
    KFbxTypedProperty<fbxBool1>                         FitImage;

    /** This property handles the crop flag of back plane.
      *
      * To access This property do: Crop.Get().
      * To set This property do: Crop.Set(fbxBool1).
      *
      * Default value is false.
	  * \see SetCrop and GetCrop.
      */
    KFbxTypedProperty<fbxBool1>                         Crop;

    /** This property handles the center flag of back plane.
      *
      * To access This property do: Center.Get().
      * To set This property do: Center.Set(fbxBool1).
      *
      * Default value is true.
	  * see SetCenter and GetCenter.
      */
    KFbxTypedProperty<fbxBool1>                         Center;

    /** This property handles the keep ratio flag of back plane.
      *
      * To access This property do: KeepRatio.Get().
      * To set This property do: KeepRatio.Set(fbxBool1).
      *
      * Default value is true.
	  * \see SetKeepRatio and GetKeepRatio.
      */
    KFbxTypedProperty<fbxBool1>                         KeepRatio;

    /** This property handles the background alpha threshold value.
      *
      * To access This property do: BackgroundAlphaTreshold.Get().
      * To set This property do: BackgroundAlphaTreshold.Set(fbxDouble1).
      *
      * Default value is 0.5.
      */
    KFbxTypedProperty<fbxDouble1>                       BackgroundAlphaTreshold;

    /** This property handles the back plane offset X.
    *
    * To access This property do: BackPlaneOffsetX.Get().
    * To set This property do: BackPlaneOffsetX.Set(fbxDouble1).
    *
    * Default value is 0.0.
    */
    KFbxTypedProperty<fbxDouble1>                       BackPlaneOffsetX;

    /** This property handles the back plane offset Y.
    *
    * To access This property do: BackPlaneOffsetY.Get().
    * To set This property do: BackPlaneOffsetY.Set(fbxDouble1).
    *
    * Default value is 0.0.
    */
    KFbxTypedProperty<fbxDouble1>                       BackPlaneOffsetY;

    /** This property handles the back plane rotation.
      *
      * To access This property do: BackPlaneRotation.Get().
      * To set This property do: BackPlaneRotation.Set(fbxDouble1).
      *
      * Default value is 0.0.
      */
    KFbxTypedProperty<fbxDouble1>                       BackPlaneRotation;

    /** This property handles the back plane scaling X.
    *
    * To access This property do: BackPlaneScaleX.Get().
    * To set This property do: BackPlaneScaleX.Set(fbxDouble1).
    *
    * Default value is 1.0.
    * \remarks The application manipulating the camera has to take into consideration of
    * the KeepRatio value too.
    */
    KFbxTypedProperty<fbxDouble1>                       BackPlaneScaleX;

    /** This property handles the back plane scaling Y.
    *
    * To access This property do: BackPlaneScaleY.Get().
    * To set This property do: BackPlaneScaleY.Set(fbxDouble1).
    *
    * Default value is 1.0.
    * \remarks The application manipulating the camera has to take into consideration of
    * the KeepRatio value too.
    */
    KFbxTypedProperty<fbxDouble1>                       BackPlaneScaleY;

    /** This property handles the flag to show back plane or not.
    *
    * To access This property do: ShowBackPlate.Get().
    * To set This property do: ShowBackPlate.Set(fbxBool1).
    *
    * Default value is false.
    * \remarks This replaces ForegroundTransparent. 
    */
    KFbxTypedProperty<fbxBool1>                         ShowBackplate;

    /** This property has the background texture connected to it.
      *
      * To access This property do: BackgroundTexture.GetSrcObject().
      * To set This property do: BackgroundTexture.ConnectSrcObject(KFbxObject*).
      *
      * \remarks The background texture is connected as source object.
      */
    KFbxTypedProperty<fbxReference> BackgroundTexture;


    // -----------------------------------------------------------------------
    // Foreground Image Display Options
    // -----------------------------------------------------------------------

    /** This property handles the fit image flag of front plate.
      *
      * To access This property do: FrontPlateFitImage.Get().
      * To set This property do: FrontPlateFitImage.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1> FrontPlateFitImage;

    /** This property handles the crop flag of front plane.
      *
      * To access This property do: FrontPlateCrop.Get().
      * To set This property do: FrontPlateCrop.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1> FrontPlateCrop;

    /** This property handles the center flag of front plane.
      *
      * To access This property do: FrontPlateCenter.Get().
      * To set This property do: FrontPlateCenter.Set(fbxBool1).
      *
      * Default value is true.
      */
    KFbxTypedProperty<fbxBool1> FrontPlateCenter;

    /** This property handles the keep ratio flag of front plane.
      *
      * To access This property do: FrontPlateKeepRatio.Get().
      * To set This property do: FrontPlateKeepRatio.Set(fbxBool1).
      *
      * Default value is true.
      */
    KFbxTypedProperty<fbxBool1> FrontPlateKeepRatio;


    /** This property handles the flag to show front plane or not.
      *
      * To access This property do: ShowFrontplate.Get().
      * To set This property do: ShowFrontplate.Set(fbxBool1).
      *
      * Default value is false.
      * \remarks This replaces ForegroundTransparent.
      */
    KFbxTypedProperty<fbxBool1> ShowFrontplate;

    /** This property handles the front plane offset X.
    *
    * To access This property do: FrontPlaneOffsetX.Get().
    * To set This property do: FrontPlaneOffsetX.Set(fbxDouble1).
    *
    * Default value is 0.0.
    */
    KFbxTypedProperty<fbxDouble1>                       FrontPlaneOffsetX;

    /** This property handles the front plane offset Y.
    *
    * To access This property do: FrontPlaneOffsetY.Get().
    * To set This property do: FrontPlaneOffsetY.Set(fbxDouble1).
    *
    * Default value is 0.0.
    */
    KFbxTypedProperty<fbxDouble1>                       FrontPlaneOffsetY;

    /** This property handles the front plane rotation.
      *
      * To access This property do: FrontPlaneRotation.Get().
      * To set This property do: FrontPlaneRotation.Set(fbxDouble1).
      *
      * Default value is 0.0.
      */
    KFbxTypedProperty<fbxDouble1>                       FrontPlaneRotation;

    /** This property handles the front plane scaling X.
    *
    * To access This property do: FrontPlaneScaleX.Get().
    * To set This property do: FrontPlaneScaleX.Set(fbxDouble1).
    *
    * Default value is 1.0.
    */
    KFbxTypedProperty<fbxDouble1>                       FrontPlaneScaleX;

    /** This property handles the front plane scaling Y.
    *
    * To access This property do: FrontPlaneScaleY.Get().
    * To set This property do: FrontPlaneScaleY.Set(fbxDouble1).
    *
    * Default value is 1.0.
    */
    KFbxTypedProperty<fbxDouble1>                       FrontPlaneScaleY;
	
	/** This property has the foreground texture connected to it.
      *
      * To access This property do: ForegroundTexture.GetSrcObject().
      * To set This property do: ForegroundTexture.ConnectSrcObject(KFbxObject*).
      *
      * \remarks The foreground texture is connected as source object.
      */
    KFbxTypedProperty<fbxReference>						ForegroundTexture;

    /** This property handles the foreground image opacity value.
      *
      * To access This property do: ForegroundOpacity.Get().
      * To set This property do: ForegroundOpacity.Set(fbxDouble1).
      *
      * Default value is 1.0.
      */
    KFbxTypedProperty<fbxDouble1>						ForegroundOpacity;

    // -----------------------------------------------------------------------
    // Safe Area
    // -----------------------------------------------------------------------

    /** This property handles the flag to display safe area or not.
      *
      * To access This property do: DisplaySafeArea.Get().
      * To set This property do: DisplaySafeArea.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         DisplaySafeArea;

    /** This property handles the flag display safe area on render or not.
      *
      * To access This property do: DisplaySafeAreaOnRender.Get().
      * To set This property do: DisplaySafeAreaOnRender.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         DisplaySafeAreaOnRender;

    /** This property handles the style to display safe area.
      *
      * To access This property do: SafeAreaDisplayStyle.Get().
      * To set This property do: SafeAreaDisplayStyle.Set(ECameraSafeAreaStyle).
      *
      * Default value is eSQUARE.
      */
    KFbxTypedProperty<ECameraSafeAreaStyle>             SafeAreaDisplayStyle;

    /** This property handles the display aspect ratio of safe area.
      *
      * To access This property do: SafeAreaDisplayStyle.Get().
      * To set This property do: SafeAreaAspectRatio.Set(fbxDouble1).
      *
      * Default value is 1.33333333333333.
      */
    KFbxTypedProperty<fbxDouble1>                       SafeAreaAspectRatio;

    // -----------------------------------------------------------------------
    // 2D Magnifier
    // -----------------------------------------------------------------------

    /** This property handles the flag to use 2d magnifier zoom or not.
	  * The 2D Magnifier lets you perform a 2D enlargement of the scene using the 
	  * current camera without changing any camera settings. 
	  *
      * To access This property do: Use2DMagnifierZoom.Get().
      * To set This property do: Use2DMagnifierZoom.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         Use2DMagnifierZoom;

    /** This property handles the 2d magnifier zoom value.
      *
      * To access This property do: _2DMagnifierZoom.Get().
      * To set This property do: _2DMagnifierZoom.Set(fbxDouble1).
      *
      * Default value is 100.0.
      */
    KFbxTypedProperty<fbxDouble1>                       _2DMagnifierZoom;

    /** This property handles the 2d magnifier X value.
      *
      * To access This property do: _2DMagnifierX.Get().
      * To set This property do: _2DMagnifierX.Set(fbxDouble1).
      *
      * Default value is 50.0.
      */
    KFbxTypedProperty<fbxDouble1>                       _2DMagnifierX;

    /** This property handles the 2d magnifier Y value.
      *
      * To access This property do: _2DMagnifierY.Get().
      * To set This property do: _2DMagnifierY.Set(fbxDouble1).
      *
      * Default value is 50.0.
      */
    KFbxTypedProperty<fbxDouble1>                       _2DMagnifierY;

    // -----------------------------------------------------------------------
    // Projection Type: Ortho, Perspective
    // -----------------------------------------------------------------------

    /** This property handles the projection type.
      *
      * To access This property do: ProjectionType.Get().
      * To set This property do: ProjectionType.Set(ECameraProjectionType).
      *
      * Default value is ePERSPECTIVE.
      */
    KFbxTypedProperty<ECameraProjectionType>            ProjectionType;

    /** This property handles the orthographic zoom value.
      *
      * To access This property do: OrthoZoom.Get().
      * To set This property do: OrthoZoom.Set(fbxDouble1).
      *
      * Default value is 1.0.
      */
    KFbxTypedProperty<fbxDouble1>                       OrthoZoom;

    // -----------------------------------------------------------------------
    // Depth Of Field & Anti Aliasing
    // -----------------------------------------------------------------------

    /** This property handles the flag to use real time Depth of Field and Anti-Aliasing or not.
      *
      * To access This property do: UseRealTimeDOFAndAA.Get().
      * To set This property do: UseRealTimeDOFAndAA.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         UseRealTimeDOFAndAA;

    /** This property handles the flag to use depth of field or not.
      *
      * To access This property do: UseDepthOfField.Get().
      * To set This property do: UseDepthOfField.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         UseDepthOfField;

    /** This property handles the focus source.
      *
      * To access This property do: FocusSource.Get().
      * To set This property do: FocusSource.Set(ECameraFocusDistanceSource).
      *
      * Default value is eCAMERA_INTEREST.
	  * \see FocusDistance.
      */
    KFbxTypedProperty<ECameraFocusDistanceSource>       FocusSource;

    /** This property handles the focus angle (in degrees).
      *
      * To access This property do: FocusAngle.Get().
      * To set This property do: FocusAngle.Set(fbxDouble1).
      *
      * Default value is 3.5.
      */
    KFbxTypedProperty<fbxDouble1>                       FocusAngle;

    /** This property handles the focus distance.
	  * Focus distance is the distance between the camera and the object on which the camera is focused. 
	  * There are two possible sources for this distance.
	  * \see ECameraFocusDistanceSource
	  *
	  * To access This property do: FocusDistance.Get().
      * To set This property do: FocusDistance.Set(fbxDouble1).
      *
      * Default value is 200.0.
      */
    KFbxTypedProperty<fbxDouble1>                       FocusDistance;

    /** This property handles the flag to use anti aliasing or not.
      *
      * To access This property do: UseAntialiasing.Get().
      * To set This property do: UseAntialiasing.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         UseAntialiasing;

    /** This property handles the anti aliasing intensity.
      *
      * To access This property do: AntialiasingIntensity.Get().
      * To set This property do: AntialiasingIntensity.Set(fbxDouble1).
      *
      * Default value is 0.77777.
      */
    KFbxTypedProperty<fbxDouble1>                       AntialiasingIntensity;

    /** This property handles the anti aliasing method.
      *
      * To access This property do: AntialiasingMethod.Get().
      * To set This property do: AntialiasingMethod.Set(ECameraAntialiasingMethod).
      *
      * Default value is eOVERSAMPLING_ANTIALIASING.
      */
    KFbxTypedProperty<ECameraAntialiasingMethod>        AntialiasingMethod;

    // -----------------------------------------------------------------------
    // Accumulation Buffer
    // -----------------------------------------------------------------------

    /** This property handles the flag to use accumulation buffer or not.
      *
      * To access This property do: UseAccumulationBuffer.Get().
      * To set This property do: UseAccumulationBuffer.Set(fbxBool1).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>                         UseAccumulationBuffer;

    /** This property handles the frame sampling count.
      *
      * To access This property do: FrameSamplingCount.Get().
      * To set This property do: FrameSamplingCount.Set(fbxInteger1).
      *
      * Default value is 7.
      */
    KFbxTypedProperty<fbxInteger1>                      FrameSamplingCount;

    /** This property handles the frame sampling type.
      *
      * To access This property do: FrameSamplingType.Get().
      * To set This property do: FrameSamplingType.Set(ECameraSamplingType).
      *
      * Default value is eSTOCHASTIC.
      */
    KFbxTypedProperty<ECameraSamplingType>              FrameSamplingType;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    friend class KFbxGlobalCameraSettings;

    virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
    KFbxCamera(KFbxSdkManager& pManager, char const* pName);

    virtual bool ConstructProperties(bool pForceSet);

    /**
      * Used to retrieve the KProperty list from an attribute
      */
    virtual KStringList GetTypeFlags() const;

private:
    double ComputePixelRatio(kUInt pWidth, kUInt pHeight, double pScreenRatio = 1.3333333333);

    // Background Properties
    KString mBackgroundMediaName;
    KString mBackgroundFileName;

    // Foreground Properties
    KString mForegroundMediaName;
    KString mForegroundFileName;

    friend class KFbxNode;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#define ImageFit	

typedef KFbxCamera* HKFbxCamera;

inline EFbxType FbxTypeOf( KFbxCamera::ECameraAntialiasingMethod const &pItem )         { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraApertureFormat const &pItem )             { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraApertureMode const &pItem )               { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraAspectRatioMode const &pItem )            { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraFrontBackPlaneDisplayMode const &pItem )  { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraFrontBackPlaneDistanceMode const &pItem )	{ return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraPlateDrawingMode const &pItem )			{ return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraFocusDistanceSource const &pItem )        { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraFormat const &pItem )                     { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraGateFit const &pItem )                    { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraProjectionType const &pItem )             { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraRenderOptionsUsageTime const &pItem )     { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraSafeAreaStyle const &pItem )              { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraSamplingType const &pItem )               { return eENUM; }
inline EFbxType FbxTypeOf( KFbxCamera::ECameraFilmRollOrder const &pItem )               { return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXCAMERA_H

