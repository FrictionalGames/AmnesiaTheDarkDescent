/*!  \file kfbxglobalcamerasettings.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXGLOBALCAMERASETTINGS_H
#define FBXFILESDK_KFBXPLUGINS_KFBXGLOBALCAMERASETTINGS_H

/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
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

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxNode;
class KFbxSdkManager;
class KFbxScene;
class KFbxCamera;
class KFbxCameraSwitcher;
class KFbxGlobalCameraSettingsProperties;

#define PRODUCER_PERSPECTIVE "Producer Perspective"
#define PRODUCER_TOP "Producer Top"
#define PRODUCER_FRONT "Producer Front"
#define PRODUCER_BACK "Producer Back"
#define PRODUCER_RIGHT "Producer Right"
#define PRODUCER_LEFT "Producer Left"
#define PRODUCER_BOTTOM "Producer Bottom"
#define CAMERA_SWITCHER "Camera Switcher"


/** This class contains the global camera settings.
  * \nosubgrouping
  * \remarks This class exists for FBX version 6.x and earlier. The new FBX v7.x file format 
  * that is now the default no longer uses it. The relevant data (a subset of this class) has 
  * been moved to the KFbxGlobalSettings object and should be used instead.
  */
class KFBX_DLL KFbxGlobalCameraSettings
{
    KFBXNEW_DECLARE_FRIEND
    /**
      * \name Default camera settings.
      */
    //@{
    public:
        /** Sets the default camera.
          * \param pCameraName              Name of the default camera.
          * \return                         \c True if camera name is valid, returns \c false if it is not valid.
          * \remarks                        A valid camera name must be either one of the defined tokens (PRODUCER_PERSPECTIVE, PRODUCER_TOP,
          *                                 PRODUCER_BOTTOM, PRODUCER_FRONT, PRODUCER_BACK, PRODUCER_RIGHT, PRODUCER_LEFT and CAMERA_SWITCHER) or the name
          *                                 of a camera that is inserted in the node tree under the scene's root node.
          */
        bool SetDefaultCamera(char* pCameraName);

        /** Returns the default camera name.
          * \return                         The default camera name, or returns an empty string if no camera name has been specified.
          */
        char* GetDefaultCamera() const;

        //! Restores the default settings.
        void RestoreDefaultSettings();

        /** \enum EViewingMode              Viewing modes.
          * - \e eSTANDARD
          * - \e eXRAY
          * - \e eMODELS_ONLY
          */
        typedef enum
        {
            eSTANDARD,
            eXRAY,
            eMODELS_ONLY
        } EViewingMode;

        /** Sets the default viewing mode.
          * \param pViewingMode             Viewing mode to set(eSTANDARD, eXRAY or eMODELS_ONLY).
          */
        void SetDefaultViewingMode(EViewingMode pViewingMode);

        /** Returns the default viewing mode.
          * \return                         The currently set Viewing mode.
          */
        EViewingMode GetDefaultViewingMode() const;

    //@}

    /**
      * \name Producer Cameras
      * Producer cameras are global cameras in MotionBuilder you use to view the scene.
      * You cannot animate Producer cameras but you can specify their default positions.
      */
    //@{

        /** Creates the default Producer cameras.
          */
        void CreateProducerCameras();

        /** Destroys the default Producer cameras.
          */
        void DestroyProducerCameras();

        /** Checks if the camera is a Producer camera.
		  * \param pCamera                  The camera to check.
          * \return                         \c True if it is a producer camera, returns \c false if it is not a producer camera.
          */
        bool IsProducerCamera(KFbxCamera* pCamera) const;

        /** Returns the Camera Switcher.
          * \return                         A pointer to the Camera Switcher.
          * \remarks                        This node has a \c KFbxNodeAttribute::eCAMERA_SWITCHER node attribute type.
          *                                 This node is not saved when there is no camera in the scene.
          *                                 Nodes inserted below are never saved.
          *                                 Camera indices start at 1. Out of range indices are clamped between 1 and the
          *                                 number of cameras in the scene. The index of a camera refers to its order of
          *                                 appearance when searching the node tree depth first.
          *                                 If a camera is added or removed after camera indices have been set, the camera
          *                                 indices must be updated. It is easier to set camera indices once every camera
          *                                 have been set.                               
          *                                 Camera index keys must be set using constant interpolation to ensure that camera
          *                                 switches occur exactly at key time.
          */
        KFbxCameraSwitcher* GetCameraSwitcher() const;

        /** Sets the Camera Switcher.
          * \param pSwitcher                The Camera Switcher to be set.
          */
        void                SetCameraSwitcher(KFbxCameraSwitcher* pSwitcher);

        /** Returns a reference to the Producer perspective camera.
          * \return                         The reference to the internal Perspective camera.
          */
        KFbxCamera* GetCameraProducerPerspective() const;

        /** Returns a reference to the Producer top camera.
          * \return                         The reference to the internal Top camera.
          */
        KFbxCamera* GetCameraProducerTop() const;

        /** Returns a reference to the Producer bottom camera.
          * \return                         The reference to the internal Bottom camera.
          */
        KFbxCamera* GetCameraProducerBottom() const;

        /** Returns a reference to the Producer front camera.
          * \return                         The reference to the internal Front camera.
          */
        KFbxCamera* GetCameraProducerFront() const;

        /** Returns a reference to the Producer back camera.
          * \return                         The reference to the internal Back camera.
          */
        KFbxCamera* GetCameraProducerBack() const;

        /** Returns a reference to the Producer right camera.
          * \return                         The reference to the internal Right camera.
          */
        KFbxCamera* GetCameraProducerRight() const;

        /** Returns a reference to the Producer left camera.
          * \return                         The reference to the internal Left camera.
          */
        KFbxCamera* GetCameraProducerLeft() const;

        //@}

        /** Assignment operator.
		  * \param pGlobalCameraSettings    KFbxGlobalCameraSettings object assigned to this one.
		  */
        const KFbxGlobalCameraSettings& operator=(const KFbxGlobalCameraSettings& pGlobalCameraSettings);

        /**
          * \name Error Management
          * The same error object is shared among instances of this class.
          */
        //@{

        /** Retrieves the error object.
         *  \return                         Reference to the error object.
         */
        KError& GetError();

        /** \enum EError                    Error identifiers, most of these are only used internally.
          * - \e eNULL_PARAMETER
          * - \e eUNKNOWN_CAMERA_NAME
          * - \e eERROR_COUNT
          */
        typedef enum
        {
            eNULL_PARAMETER,
            eUNKNOWN_CAMERA_NAME,
            eERROR_COUNT
        } EError;

        /** Returns last error code.
         *  \return                         Last error code.
         */
        EError GetLastErrorID() const;

        /** Returns the last error string.
         *  \return                         Text description of the last error.
         */
        const char* GetLastErrorString() const;

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
        bool CopyProducerCamera(KString pCameraName, const KFbxCamera* pCamera) const;
        int  GetProducerCamerasCount() const { return 7; }
    private:
        KFbxGlobalCameraSettings(KFbxSdkManager& pManager, KFbxScene& pScene);
        ~KFbxGlobalCameraSettings();

        KFbxGlobalCameraSettingsProperties* mPH;

        friend class KFbxScene;

    #endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXGLOBALCAMERASETTINGS_H

