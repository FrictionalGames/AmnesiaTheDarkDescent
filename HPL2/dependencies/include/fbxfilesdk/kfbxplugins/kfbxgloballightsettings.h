/*!  \file kfbxgloballightsettings.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXGLOBALLIGHTSETTINGS_H
#define FBXFILESDK_KFBXPLUGINS_KFBXGLOBALLIGHTSETTINGS_H

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

#include <fbxfilesdk/kfbxplugins/kfbxcolor.h>

#include <fbxfilesdk/kfbxmath/kfbxvector4.h>

#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxGlobalLightSettingsProperties;

/** This class contains functions for accessing global light settings.
  * \nosubgrouping
  * \remarks This class exists for FBX version 6.x and earlier. The new FBX v7.x file format that is 
  * now the default no longer uses it. The relevant data (a subset of this class) has been moved to
  * the KFbxGlobalSettings object and should be used instead.
  */
class KFBX_DLL KFbxGlobalLightSettings
{

public:
    KFBXNEW_DECLARE_FRIEND
    /**
      * \name Ambient Color
      */
    //@{

    /** Sets the ambient color.
      * \param pAmbientColor            The ambient color to set.
      * \remarks                        The ambient color only use RGB channels.
      */
    void SetAmbientColor(KFbxColor pAmbientColor);

    /** Returns the ambient color.
      * \return                         The ambient color.
      */
    KFbxColor GetAmbientColor() const;

    //@}

    /**
      * \name Fog Option
      */
    //@{

    /** Activates or disables the fog.
      * \param pEnable                  Set to \c true to activate the fog option or set to \c false to disable the fog option.
      */
    void SetFogEnable(bool pEnable);

    /** Returns the fog option's current state.
      * \return                         \c True if fog is activated, returns \c false if fog is disabled.
      */
    bool GetFogEnable() const;

    /** Sets the fog color.
      * \param pColor                   The fog color to be set.
      * \remarks                        The fog color only uses RGB channels.
      */
    void SetFogColor(KFbxColor pColor);

    /** Returns the fog color.
      * \return                         The fog color.
      * \remarks                        The fog color only uses RGB channels.
      */
    KFbxColor GetFogColor() const;

    /** \enum EFogMode                  Fog types.
      * - \e eLINEAR
        - \e eEXPONENTIAL
        - \e eSQUAREROOT_EXPONENTIAL
      */
    typedef enum
    {
        eLINEAR,
        eEXPONENTIAL,
        eSQUAREROOT_EXPONENTIAL
    } EFogMode;

    /** Sets the fog mode.
      * \param pMode                    The fog type to be set.
      */
    void SetFogMode(EFogMode pMode);

    /** Returns the fog mode.
      * \return                         The currently set fog mode.
      */
    EFogMode GetFogMode() const;

    /** Sets the fog density.
      * \param pDensity                 The fog density to be set. It can be any double value, however it can
      *                                 happen that other sections of FBX SDK may clamp values to reasonable values.
      * \remarks                        This function is only used when the fog mode is set to exponential or square root exponential.
      */
    void SetFogDensity(double pDensity);

    /** Returns the fog density.
      * \return                         The currently set fog density.
      * \remarks                        This function is only used when the fog mode is set to exponential or square root exponential.
      */
    double GetFogDensity() const;

    /** Sets the distance from the view where the fog begins.
      * \param pStart                   Distance where the fog begins.
      * \remarks                        This function is only used when the fog mode is set to linear. The new value is clamped to fit inside the interval [0, FogEnd()].
      */
    void SetFogStart(double pStart);

    /** Returns the distance from the view where the fog begins.
      * \return                         The distance from the view where the fog begins.
      * \remarks                        This function is only used when the fog mode is set to linear.
      */
    double GetFogStart() const;

    /** Sets the distance from the view where the fog ends.
      * \param pEnd                     Distance where the fog ends.
      * \remarks                        This function is only used when the fog mode is set to linear. The new value is adjusted to fit within the interval [FogStart(), inf).
      */
    void SetFogEnd(double pEnd);

    /** Returns the distance from the view where the fog ends.
      * \return                         The distance from the view where the fog ends.
      * \remarks                        This function is only used when the fog mode is set to linear.
      */
    double GetFogEnd() const;

    //@}

    /**
      * \name Shadow Planes
      * The functions in this section are supported only by FiLMBOX version 2.7 and earlier.
      * FiLMBOX 3.0 supports shadow planes within a specific shader, which is not supported by the FBX SDK.
      */
    //@{

    /** Struct used to define the shadow plane.
      */
    struct KFBX_DLL KFbxShadowPlane
    {
        //! Default constructor.
        KFbxShadowPlane();

        //! Activate flag.
        bool mEnable;

        //! Origin point.
        KFbxVector4 mOrigin;

        //! Normal vector.
        KFbxVector4 mNormal; 
    };

    /** Activates or disables the display of shadow planes.
      * \param pShadowEnable        Set to \c true to display shadow planes in the scene.
      */
    void SetShadowEnable(bool pShadowEnable);

    /** Returns the current state of the shadow enable flag.
      * \return                     \c True if shadow planes are set to be displayed in the scene.
      */
    bool GetShadowEnable() const;

    /** Sets the shadow intensity that is applied to all shadow planes.
      * \param pShadowIntensity     Intensity applied to all the shadow planes.
      * \remarks                    Ranges from 0 to 300.
      */
    void SetShadowIntensity(double pShadowIntensity);

    /** Returns the shadow intensity applied to all shadow planes.
      * \return                     The intensity applied to all shadow planes in the scene.
      * \remarks                    Ranges from 0 to 300.
      */
    double GetShadowIntensity() const;

    /** Returns the number of shadow planes.
      * \return                     Number of shadow planes.
      */
    int GetShadowPlaneCount() const;

    /** Returns a shadow plane at the specified index.
      * \param pIndex               Shadow plane index.
      * \return                     Pointer the shadow plane, or \c NULL if the index is out of range.
      * \remarks                    To identify the error, use KFbxGlobalLightSettings::GetLastErrorID() which returns eINDEX_OUT_OF_RANGE.
      */
    KFbxShadowPlane* GetShadowPlane(int pIndex);

    /** Adds a shadow plane.
      * \param pShadowPlane         The shadow plane to be added.
      */
    void AddShadowPlane(KFbxShadowPlane pShadowPlane);

    //! Removes all shadow planes.
    void RemoveAllShadowPlanes();

    //@}

    /**
      * \name Error Management
      */
    //@{

    /** Retrieves the error object.
     *  \return                     Reference to the error object.
     */
    KError& GetError();

    /** \enum EError                Error identification.
     *  - \e eINDEX_OUT_OF_RANGE
     *  - \e eERROR_COUNT
     */
    typedef enum
    {
        eINDEX_OUT_OF_RANGE,
        eERROR_COUNT
    } EError;

    /** Returns the last error code.
     *  \return                     The last error code.
     */
    EError GetLastErrorID() const;

    /** Returns the last error string.
     *  \return                     Text description of the last error.
     */
    const char* GetLastErrorString() const;

    //@}

    //! Restores default settings.
    void RestoreDefaultSettings();

    /** Assignment operator.
	  * \param pGlobalLightSettings KFbxGlobalLightSettings object assigned to this one.
	  */
    const KFbxGlobalLightSettings& operator=(const KFbxGlobalLightSettings& pGlobalLightSettings);


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

    KFbxGlobalLightSettings();
    ~KFbxGlobalLightSettings();

    KFbxGlobalLightSettingsProperties* mPH;

    friend class KFbxScene;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXGLOBALLIGHTSETTINGS_H

