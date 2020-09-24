/*!  \file kfbxlight.h
 */
 
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXLIGHT_H
#define FBXFILESDK_KFBXPLUGINS_KFBXLIGHT_H

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

#include <fbxfilesdk/kfbxplugins/kfbxnodeattribute.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxColor;
class KFbxSdkManager;
class KFbxTexture;

/** \brief This node attribute contains methods for accessing the properties of a light.
  * \nosubgrouping
  */
class KFBX_DLL KFbxLight : public KFbxNodeAttribute
{
	KFBXOBJECT_DECLARE(KFbxLight,KFbxNodeAttribute);

public:

	//! Return the type of node attribute which is EAttributeType::eLIGHT.
	virtual EAttributeType GetAttributeType() const;

	/**
	  * \name Light Properties
	  */
	//@{

    /** \enum ELightType Light types.
	  * - \e ePOINT
	  * - \e eDIRECTIONAL
	  * - \e eSPOT
	  * - \e eAREA
	  */
    typedef enum  
    {
	    ePOINT = 0, 
	    eDIRECTIONAL, 
	    eSPOT,
		eAREA
    } ELightType;

    /** \enum EDecayType     Decay types. Used for setting the attenuation of the light.
	  * - \e eNONE          No decay. The light's intensity will not diminish with distance.		
	  * - \e eLINEAR        Linear decay. The light's intensity will diminish linearly with the distance from the light.
	  * - \e eQUADRATIC     Quadratic decay. The light's intensity will diminish with the squared distance from the light.
	  *                     This is the most physically accurate decay rate.
	  * - \e eCUBIC         Cubic decay. The light's intensity will diminish with the cubed distance from the light.
	  */
    typedef enum  
    {
	    eNONE = 0,
	    eLINEAR,
	    eQUADRATIC,
		eCUBIC
    } EDecayType;

	/** \enum EAreaLightShape	Supported area light types.
	  * - \e eRECTANGLE			Rectangle (or often called a plane) area light type.
	  * - \e eSPHERE			Area light that illuminate all directions.
	  */
	typedef enum
	{
		eRECTANGLE = 0,
		eSPHERE
	} EAreaLightShape;

	/** Set the shadow texture for the light.
	  * \param pTexture     The texture cast by the light shadow.
	  */
	void SetShadowTexture( KFbxTexture* pTexture );

	/** Get the light state.
	  * \return     Pointer to the texture cast by the light shadow, or \c NULL if the shadow texture has not been set.
	  */
	KFbxTexture* GetShadowTexture() const;

	//@}

	/**
	  * \name Light Property Names
	  */
	//@{	
	static const char*			sLightType;
	static const char*			sCastLight;
	static const char*			sDrawVolumetricLight;
	static const char*			sDrawGroundProjection;
	static const char*			sDrawFrontFacingVolumetricLight;
	static const char*			sColor;
	static const char*			sIntensity;
	static const char*          sHotSpot; // inner cone
	static const char*			sConeAngle; // outer cone
	static const char*			sFog;
	static const char*			sDecayType;
	static const char*			sDecayStart;
	static const char*			sFileName;
	static const char*			sEnableNearAttenuation;
	static const char*			sNearAttenuationStart;
	static const char*			sNearAttenuationEnd;
	static const char*			sEnableFarAttenuation;
	static const char*			sFarAttenuationStart;
	static const char*			sFarAttenuationEnd;
	static const char*			sCastShadows;
	static const char*			sShadowColor;
	static const char*			sAreaLightShape;
	static const char*			sLeftBarnDoor;
	static const char*			sRightBarnDoor;
	static const char*			sTopBarnDoor;
	static const char*			sBottomBarnDoor;
	static const char*			sEnableBarnDoor;

	//@}

	/**
	  * \name Light Property Default Values
	  */
	//@{	
	static const ELightType		sDefaultLightType;
	static const fbxBool1		sDefaultCastLight;
	static const fbxBool1		sDefaultDrawVolumetricLight;
	static const fbxBool1		sDefaultDrawGroundProjection;
	static const fbxBool1		sDefaultDrawFrontFacingVolumetricLight;
	static const fbxDouble3		sDefaultColor;
	static const fbxDouble1		sDefaultIntensity;
	static const fbxDouble1		sDefaultHotSpot;
	static const fbxDouble1		sDefaultConeAngle;
	static const fbxDouble1		sDefaultFog;
	static const EDecayType		sDefaultDecayType;
	static const fbxDouble1		sDefaultDecayStart;
	static const fbxString		sDefaultFileName;
	static const fbxBool1		sDefaultEnableNearAttenuation;
	static const fbxDouble1		sDefaultNearAttenuationStart;
	static const fbxDouble1		sDefaultNearAttenuationEnd;
	static const fbxBool1		sDefaultEnableFarAttenuation;
	static const fbxDouble1		sDefaultFarAttenuationStart;
	static const fbxDouble1		sDefaultFarAttenuationEnd;
	static const fbxBool1		sDefaultCastShadows;
	static const fbxDouble3		sDefaultShadowColor;
	static const EAreaLightShape	sDefaultAreaLightShape;
	static const fbxFloat1			sDefaultLeftBarnDoor;
	static const fbxFloat1			sDefaultRightBarnDoor;
	static const fbxFloat1			sDefaultTopBarnDoor;
	static const fbxFloat1			sDefaultBottomBarnDoor;
	static const fbxBool1			sDefaultEnableBarnDoor;

	//@}

	//////////////////////////////////////////////////////////////////////////
	//
	// Properties
	//
	//////////////////////////////////////////////////////////////////////////

	/**
	* \name Properties
	*/
	//@{	
	
	/** This property handles the light type.
	  *
      * To access this property do: LightType.Get().
      * To set this property do: LightType.Set(ELightType).
      *
	  * Default value is ePOINT
	  */
	KFbxTypedProperty<ELightType>		LightType;

	/** This property handles the cast light on object flag.
	  *
      * To access this property do: CastLight.Get().
      * To set this property do: CastLight.Set(fbxBool1).
      *
	  * Default value is true
	  */
	KFbxTypedProperty<fbxBool1>			CastLight;

	/** This property handles the draw volumetric light flag.
	  *
      * To access this property do: DrawVolumetricLight.Get().
      * To set this property do: DrawVolumetricLight.Set(fbxBool1).
      *
	  * Default value is true
	  */
	KFbxTypedProperty<fbxBool1>			DrawVolumetricLight;

	/** This property handles the draw ground projection flag.
	  *
      * To access this property do: DrawGroundProjection.Get().
      * To set this property do: DrawGroundProjection.Set(fbxBool1).
      *
	  * Default value is true
	  */
	KFbxTypedProperty<fbxBool1>			DrawGroundProjection;

	/** This property handles the draw facing volumetric projection flag.
	  *
      * To access this property do: DrawFrontFacingVolumetricLight.Get().
      * To set this property do: DrawFrontFacingVolumetricLight.Set(fbxBool1).
      *
	  * Default value is false
	  */
	KFbxTypedProperty<fbxBool1>			DrawFrontFacingVolumetricLight;

	/** This property handles the light color.
	  *
      * To access this property do: Color.Get().
      * To set this property do: Color.Set(fbxDouble3).
      *
	  * Default value is (1.0, 1.0, 1.0)
	  */
	KFbxTypedProperty<fbxDouble3>		Color;

	/** This property handles the light intensity.
	  *
      * To access this property do: Intensity.Get().
      * To set this property do: Intensity.Set(fbxDouble1).
      *
	  * Default value is 100.0
	  */
	KFbxTypedProperty<fbxDouble1>		Intensity;

	/** This property handles the light inner cone angle (in degrees). Also know as the HotSpot
	  *
      * To access this property do: HotSpot.Get().
      * To set this property do: HotSpot.Set(fbxDouble1).
      *
	  * Default value is 45.0
	  */
	KFbxTypedProperty<fbxDouble1>		HotSpot;

	/** This property handles the light outer cone angle (in degrees). Also known as the Falloff
	  *
      * To access this property do: ConeAngle.Get().
      * To set this property do: ConeAngle.Set(fbxDouble1).
      *
	  * Default value is 45.0
	  */
	KFbxTypedProperty<fbxDouble1>		ConeAngle;

	/** This property handles the light fog intensity
	  *
      * To access this property do: Fog.Get().
      * To set this property do: Fog.Set(fbxDouble1).
      *
	  * Default value is 50.0
	  */
	KFbxTypedProperty<fbxDouble1>		Fog;

	/** This property handles the decay type 
	  *
      * To access this property do: DecayType.Get().
      * To set this property do: DecayType.Set(EDecayType).
      *
	  * Default value is eNONE
	  */
	KFbxTypedProperty<EDecayType>		DecayType;

	/** This property handles the decay start distance
	  *
      * To access this property do: DecayStart.Get().
      * To set this property do: DecayStart.Set(fbxDouble1).
      *
	  * Default value is 0.0
	  */
	KFbxTypedProperty<fbxDouble1>		DecayStart;

	/** This property handles the gobo file name
	  *
      * To access this property do: FileName.Get().
      * To set this property do: FileName.Set(fbxString).
      *
	  * Default value is ""
	  */
	KFbxTypedProperty<fbxString>		FileName;

	/** This property handles the enable near attenuation flag
	  *
      * To access this property do: EnableNearAttenuation.Get().
      * To set this property do: EnableNearAttenuation.Set(fbxBool1).
      *
	  * Default value is false
	  */
	KFbxTypedProperty<fbxBool1>			EnableNearAttenuation;

	/** This property handles the near attenuation start distance
	  *
      * To access this property do: NearAttenuationStart.Get().
      * To set this property do: NearAttenuationStart.Set(fbxDouble1).
      *
	  * Default value is 0.0
	  */
	KFbxTypedProperty<fbxDouble1>		NearAttenuationStart;

	/** This property handles the near end attenuation 
	  *
      * To access this property do: NearAttenuationEnd.Get().
      * To set this property do: NearAttenuationEnd.Set(fbxDouble1).
      *
	  * Default value is 0.0
	  */
	KFbxTypedProperty<fbxDouble1>		NearAttenuationEnd;

	/** This property handles the enable far attenuation flag
	  *
      * To access this property do: EnableFarAttenuation.Get().
      * To set this property do: EnableFarAttenuation.Set(fbxBool1).
      *
	  * Default value is false
	  */
	KFbxTypedProperty<fbxBool1>			EnableFarAttenuation;

	/** This property handles the far attenuation start distance
	  *
      * To access this property do: FarAttenuationStart.Get().
      * To set this property do: FarAttenuationStart.Set(fbxDouble1).
      *
	  * Default value is 0.0
	  */
	KFbxTypedProperty<fbxDouble1>		FarAttenuationStart;

	/** This property handles the attenuation end distance
	  *
      * To access this property do: FarAttenuationEnd.Get().
      * To set this property do: FarAttenuationEnd.Set(fbxDouble1).
      *
	  * Default value is 0.0
	  */
	KFbxTypedProperty<fbxDouble1>		FarAttenuationEnd;

	/** This property handles the cast shadow flag
	  *
      * To access this property do: CastShadows.Get().
      * To set this property do: CastShadows.Set(fbxBool1).
      *
	  * Default value is false
	  */
	KFbxTypedProperty<fbxBool1>			CastShadows;

	/** This property handles the shadow color
	  *
      * To access this property do: ShadowColor.Get().
      * To set this property do: ShadowColor.Set(fbxDouble3).
      *
	  * Default value is (0.0, 0.0, 0.0)
	  */
	KFbxTypedProperty<fbxDouble3>		ShadowColor;

	/** This property handles type when LightType is eAREA
	  *
      * To access this property do: AreaLightShape.Get().
      * To set this property do: AreaLightShape.Set(EAreaLightShape).
      *
	  * Default value is eRECTANGLE
	  */
	KFbxTypedProperty<EAreaLightShape>	AreaLightShape;

	KFbxTypedProperty<fbxFloat1>		LeftBarnDoor;
	KFbxTypedProperty<fbxFloat1>		RightBarnDoor;
	KFbxTypedProperty<fbxFloat1>		TopBarnDoor;
	KFbxTypedProperty<fbxFloat1>		BottomBarnDoor;
	KFbxTypedProperty<fbxBool1>			EnableBarnDoor;

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
	KFbxTypedProperty<fbxDouble1> InnerAngle;
	KFbxTypedProperty<fbxDouble1> OuterAngle;

protected:
	KFbxLight(KFbxSdkManager& pManager, char const* pName);
	virtual bool ConstructProperties(bool pForceSet);
	virtual void Init();
	virtual KStringList	GetTypeFlags() const;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxLight* HKFbxLight;

inline EFbxType FbxTypeOf( KFbxLight::ELightType const &pItem )			{ return eENUM; }
inline EFbxType FbxTypeOf( KFbxLight::EDecayType const &pItem )			{ return eENUM; }
inline EFbxType FbxTypeOf( KFbxLight::EAreaLightShape const &pItem )			{ return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXLIGHT_H

