/*!  \file kfbxtexture.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXTEXTURE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXTEXTURE_H

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

// FBX includes
#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxmath/kfbxvector2.h>

// FBX namespace
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

// Forward declaration
class KFbxVector4;
class KFbxLayerContainer;


/** This class is the base class for textures, ie classes KFbxFileTexture, KFbxLayeredTexture and KFbxProceduralTexture. 
  * It describes image mapping on top of a geometry.
  * \nosubgrouping
  */
class KFBX_DLL KFbxTexture : public KFbxObject
{
	KFBXOBJECT_DECLARE(KFbxTexture,KFbxObject);

	public:
	/**
	  * \name Texture Properties
	  */
	//@{

	  /** \enum EUnifiedMappingType      Internal enum for texture mapping types.
	    * Includes mapping types and planar mapping normal orientations.
		* Use SetMappingType(), GetMappingType(), SetPlanarMappingNormal() 
		* and GetPlanarMappingNormal() to access these values.
	    */
		typedef enum
		{ 
			//! Maps to EMappingType::eUV.
			eUMT_UV, 
			//! Maps to EMappingType::ePLANAR and EPlanarMappingNormal::ePLANAR_NORMAL_Z.
			eUMT_XY, 
			//! Maps to EMappingType::ePLANAR and EPlanarMappingNormal::ePLANAR_NORMAL_X.
			eUMT_YZ, 
			//! Maps to EMappingType::ePLANAR and EPlanarMappingNormal::ePLANAR_NORMAL_Y.
			eUMT_XZ, 
			//! Maps to EMappingType::eSPHERICAL.
			eUMT_SPHERICAL,
			//! Maps to EMappingType::eCYLINDRICAL.
			eUMT_CYLINDRICAL,
			//! Maps to EMappingType::eENVIRONMENT.
			eUMT_ENVIRONMENT,
			//! Unused.
			eUMT_PROJECTION,
			//! Maps to EMappingType::eBOX. Deprecated.
			eUMT_BOX, // deprecated
			//! Maps to EMappingType::eFACE. Deprecated.
			eUMT_FACE, // deprecated
			//! Maps to EMappingType::eNULL.
			eUMT_NO_MAPPING,
		} EUnifiedMappingType;


	  /** \enum ETextureUse6         Internal enum for texture usage.
		* For example, the texture might be used as a standard texture, as a shadow map, as a bump map, etc.
	    * Use SetTextureUse() and GetTextureUse() to access these values.	
	    */
		typedef enum 
		{
			//! Maps to ETextureUse::eSTANDARD.
			eTEXTURE_USE_6_STANDARD,
			//! Maps to ETextureUse::eSPHERICAL_REFLEXION_MAP.
			eTEXTURE_USE_6_SPHERICAL_REFLEXION_MAP,
			//! Maps to ETextureUse::eSPHERE_REFLEXION_MAP.
			eTEXTURE_USE_6_SPHERE_REFLEXION_MAP,
			//! Maps to ETextureUse::eSHADOW_MAP.
			eTEXTURE_USE_6_SHADOW_MAP,
			//! Maps to ETextureUse::eLIGHT_MAP.
			eTEXTURE_USE_6_LIGHT_MAP,
			//! Maps to ETextureUse::eBUMP_NORMAL_MAP.
			eTEXTURE_USE_6_BUMP_NORMAL_MAP
		} ETextureUse6;

		/** \enum EWrapMode Wrap modes.
		  * Use SetWrapMode(), GetWrapModeU() and GetWrapModeV() to access these values.
		  */
		typedef enum 
		{
			//! Apply the texture over and over on the model's surface until the model is covered. This is the default setting.
			eREPEAT,
			//! Apply the texture to a model only once, using the color at the ends of the texture as the "filter".
			eCLAMP
		} EWrapMode;

		/** \enum EBlendMode Blend modes.
		  */
		typedef enum 
		{
			//! The texture is transparent, depending on the Alpha settings.
			eTRANSLUCENT,
			//! The color of the texture is added to the previous texture.
			eADDITIVE,
			//! The color value of the texture is multiplied by the color values of all previous layers of texture.
			eMODULATE,
			//! The color value of the texture is multiplied by two and then multiplied by the color values of all previous layers of texture.
			eMODULATE2,
			//! The texture is opaque.
			eOVER
        } EBlendMode;

        /** \enum EAlignMode Align indices for cropping.
          */
        typedef enum  
        {
			//! Left cropping.
            eLEFT = 0,
			//! Right cropping.
            eRIGHT,
			//! Top cropping.
            eTOP,
			//! Bottom cropping.
            eBOTTOM
        } EAlignMode;

        /** \enum ECoordinates Texture coordinates.
          */
        typedef enum 
        {
			//! U axis.
            eU = 0,
			//! V axis.
            eV,
			//! W axis.
            eW
        } ECoordinates;

		// Type description

        /** This property handles the use of textures.
          * Default value is eTEXTURE_USE_6_STANDARD.
          */
		KFbxTypedProperty<ETextureUse6>			TextureTypeUse;

        /** This property handles the default alpha value for textures.
          * Default value is 1.0
          */
		KFbxTypedProperty<fbxDouble1>			Alpha;


		// Mapping information

        /** This property handles the texture mapping types.
          * Default value is eUMT_UV.
          */
		KFbxTypedProperty<EUnifiedMappingType>	CurrentMappingType;

        /** This property handles the texture wrap modes in U.
          * Default value is eREPEAT.
          */
		KFbxTypedProperty<EWrapMode>			WrapModeU;

        /** This property handles the texture wrap modes in V.
          * Default value is eREPEAT.
          */
		KFbxTypedProperty<EWrapMode>			WrapModeV;

        /** This property handles the swap UV flag.
          * If swap UV flag is enabled, the texture's width and height are swapped.
          * Default value is false.
          */
		KFbxTypedProperty<fbxBool1>				UVSwap;

        /** This property handles the PremultiplyAlpha flag.
          * If PremultiplyAlpha flag is true, the R, G, and B components you store have already been multiplied in with the alpha.
          * Default value is true.
          */
		KFbxTypedProperty<fbxBool1>				PremultiplyAlpha;

		// Texture positioning

        /** This property handles the default translation vector.
          * Default value is fbxDouble3(0.0,0.0,0.0).
          */
		KFbxTypedProperty<fbxDouble3>			Translation;

        /** This property handles the default rotation vector.
          * Default value is fbxDouble3(0.0,0.0,0.0).
          */
		KFbxTypedProperty<fbxDouble3>			Rotation;

        /** This property handles the default scale vector.
          * Default value is fbxDouble3(1.0,1.0,1.0).
          */
		KFbxTypedProperty<fbxDouble3>			Scaling;

        /** This property handles the rotation pivot vector.
          * Default value is fbxDouble3(0.0,0.0,0.0).
          */
		KFbxTypedProperty<fbxDouble3>			RotationPivot;

        /** This property handles the scaling pivot vector.
          * Default value is fbxDouble3(0.0,0.0,0.0).
          */
		KFbxTypedProperty<fbxDouble3>			ScalingPivot;

		// Blend mode
        /** This property handles the texture blend mode.
          * Default value is eADDITIVE.
          */
		KFbxTypedProperty<EBlendMode>	CurrentTextureBlendMode;

		// UV set to use.
        /** This property handles the use of UV sets.
          * Default value is default.
          */
		KFbxTypedProperty<fbxString>			UVSet;

        /** This property only used by Vector Displacement Texture so it is not added to KFbxTexture.
          * It is a dynamic enum property which has values : "World", "Object" and "Tangent"
          * Default value is "Object".
          */
        static const char* sVectorSpace        ;
        static const char* sVectorSpaceWorld   ;
        static const char* sVectorSpaceObject  ;
        static const char* sVectorSpaceTangent ;

        /** This property only used by Vector Displacement Texture so it is not added to KFbxTexture.
          * It is a dynamic enum property which has values : "Floating-point Absolute" and "Signed Encoding"
          * Default value is "Floating-point Absolute".
          */
        static const char* sVectorEncoding     ;
        static const char* sVectorEncodingFP   ;
        static const char* sVectorEncodingSE   ;


	/** Resets the default texture values.
	  */
	virtual void Reset();

    /** Sets the swap UV flag.
	  * \param pSwapUV      Set to \c true if the swap UV flag is enabled.
	  * \remarks            If the swap UV flag is enabled, the texture's width and height are swapped.
	  */
    void SetSwapUV(bool pSwapUV);

    /** Returns the swap UV flag.
	  * \return             \c True if the swap UV flag is enabled.
	  * \remarks            If the swap UV flag is enabled, the texture's width and height are swapped.
	  */
    bool GetSwapUV() const;

    /** Sets the PremultiplyAlpha flag.
	  * \param pPremultiplyAlpha       Set to \c true if the method of storing alpha is PremultiplyAlpha.
	  * \remarks                        If PremultiplyAlpha flag is true, the R, G, and B components you store have already been multiplied in with the alpha.
	  */
    void SetPremultiplyAlpha(bool pPremultiplyAlpha);

    /** Returns the PremultiplyAlpha flag.
	  * \return             \c True if the method of storing alpha is PremultiplyAlpha.
	  * \remarks            If PremultiplyAlpha flag is true, the R, G, and B components you store have already been multiplied in with the alpha.
	  */
    bool GetPremultiplyAlpha() const;

	/** \enum EAlphaSource Controls if the Alpha computation of the current texture comes from the Alpha channel, RGB Intensity channel, or if there is No Alpha.
	  */
    typedef enum    
    { 
		//! No Alpha.
        eNONE, 
		//! RGB Intensity (computed).
        eRGB_INTENSITY, 
		//! Alpha channel. Black is 100% transparency, white is opaque.
        eBLACK 
    } EAlphaSource;

    /** Sets the alpha source.
	  * \param pAlphaSource The alpha source identifier.
	  */
    void SetAlphaSource(EAlphaSource pAlphaSource);

    /** Returns the alpha source.
      * \return             The alpha source identifier for this texture.
	  */
	EAlphaSource GetAlphaSource() const;

    /** Sets cropping.
	  * \param pLeft        Left cropping value.
	  * \param pTop         Top cropping value.
	  * \param pRight       Right cropping value.
	  * \param pBottom      Bottom cropping value.
	  * \remarks            The defined rectangle is not checked for invalid values.
	  *                     The caller must verify that the rectangle
	  *                     is meaningful for this texture.
	  */
    void SetCropping(int pLeft, int pTop, int pRight, int pBottom);

    /** Returns left cropping.
	  * \return             The left side of the cropping rectangle.
	  */
    int GetCroppingLeft() const;

    /** Returns top cropping.
	  * \return             The top side of the cropping rectangle.
	  */
    int GetCroppingTop() const;

    /** Returns right cropping.
	  * \return             The right side of the cropping rectangle.
	  */
    int GetCroppingRight() const;

    /** Returns bottom cropping.
	  * \return             The bottom side of the cropping rectangle.
	  */
    int GetCroppingBottom() const;
	
	/** \enum EMappingType Texture mapping types.
	  */
    typedef enum    
    { 
		//! No texture mapping defined.
        eNULL, 
		//! Apply texture to the model viewed as a plane.
        ePLANAR, 
		//! Wrap texture around the model as if it was a sphere.
        eSPHERICAL, 
		//! Wrap texture around the model as if it was a cylinder.
        eCYLINDRICAL, 
		//! Wrap texture around the model as if it was a box.
        eBOX, 
		//! Apply texture to the model viewed as a face.
        eFACE,
		//! Apply texture to the model according to UVs.
        eUV,
		//! Texture is an environment map.
		eENVIRONMENT
    } EMappingType;

    /** Sets the mapping type.
	  * \param pMappingType The mapping type identifier.
	  */
    void SetMappingType(EMappingType pMappingType);

    /** Returns the mapping type.
	  * \return             The mapping type identifier.
	  */
    EMappingType GetMappingType() const;

	/** \enum EPlanarMappingNormal Planar mapping normal orientations.
	  */
    typedef enum   
    { 
		//! Normals are in the direction of the X axis, mapping plan is in the YZ axis.
        ePLANAR_NORMAL_X, 
		//! Normals are in the direction of the Y axis, mapping plan is in the XZ axis.
        ePLANAR_NORMAL_Y, 
		//! Normals are in the direction of the Z axis, mapping plan is in the XY axis.
        ePLANAR_NORMAL_Z 
    } EPlanarMappingNormal;

    /** Sets the normal orientations for planar mapping.
	  * \param pPlanarMappingNormal The identifier for planar mapping normal orientation.
	  */
    void SetPlanarMappingNormal(EPlanarMappingNormal pPlanarMappingNormal);

    /** Returns the normal orientations for planar mapping.
	  * \return                     The identifier for planar mapping normal orientation.
	  */
    EPlanarMappingNormal GetPlanarMappingNormal() const;

	/** \enum ETextureUse           Texture uses.
	  */
	typedef enum 
	{
		//! Standard texture use (ex. image)
		eSTANDARD,
		//! Shadow map
		eSHADOW_MAP,
		//! Light map
		eLIGHT_MAP,
		//! Spherical reflection map: Object reflects the contents of the scene
		eSPHERICAL_REFLEXION_MAP,
		//! Sphere reflection map: Object reflects the contents of the scene from only one point of view
		eSPHERE_REFLEXION_MAP,
		//! Bump map: Texture contains two direction vectors, that are used to convey relief in a texture.
		eBUMP_NORMAL_MAP
	} ETextureUse;

	/** Sets the texture use.
	  * \param pTextureUse          The texture use identifier.
	  */
    void SetTextureUse(ETextureUse pTextureUse);

    /** Returns the texture use.
	  * \return                     The texture use identifier.
	  */
    ETextureUse GetTextureUse() const;


	/** Sets the U and V wrap mode.
	  * \param pWrapU               Wrap mode identifier.
	  * \param pWrapV               Wrap mode identifier.
	  */
    void SetWrapMode(EWrapMode pWrapU, EWrapMode pWrapV);

    /** Returns the U wrap mode.
	  * \return                     U wrap mode identifier.
	  */
    EWrapMode GetWrapModeU() const;

	/** Returns the V wrap mode.
	  * \return                     V wrap mode identifier.
	  */
	EWrapMode GetWrapModeV() const;


	/** Sets the blend mode.
	  * \param pBlendMode           Blend mode identifier.
	  */
	void SetBlendMode(EBlendMode pBlendMode);

	/** Returns the blend mode.
	  * \return                     Blend mode identifier.
	  */
	EBlendMode GetBlendMode() const;

	//@}

	/**
	  * \name Default Values Management By Vectors
	  * This set of functions provides direct access to the default values in vector base. 
	  */
	//@{

	/** Sets the default translation vector. 
	  * \param pT           The first element is the U translation applied to 
	  *                     the texture. A displacement of one unit is equal to the texture
	  *                     width after the U scaling is applied. The second element is the
	  *                     V translation applied to the texture. A displacement of one unit is 
	  *                     equal to the texture height after the V scaling is applied.
	  *                     The third and fourth elements have no effect on texture 
	  *                     translation.
	  */
	inline void SetDefaultT(const KFbxVector4& pT) { Translation.Set( pT ); }

	/** Returns the default translation vector. 
	  * \param pT           The first element is the U translation applied to 
	  *                     the texture. A displacement of one unit is equal to the texture
	  *                     width after the U scaling is applied. The second element is the
	  *                     V translation applied to the texture. A displacement of one unit is 
	  *                     equal to the texture height after the V scaling is applied.
	  *                     The third and fourth elements have no effect on texture 
	  *                     translation.
	  * \return             The input parameter completed with appropriate data.
	  */
	KFbxVector4& GetDefaultT(KFbxVector4& pT) const;

	/** Sets the default rotation vector. 
	  * \param pR           The first element is the texture rotation around the 
	  *                     U axis in degrees. The second element is the texture rotation 
	  *                     around the V axis in degrees. The third element is the texture 
	  *                     rotation around the W axis in degrees.
	  * \remarks            The W axis is oriented toward the result of the 
	  *                     vector product of the U and V axes that is W = U x V.
      */
	inline void SetDefaultR(const KFbxVector4& pR) { Rotation.Set( fbxDouble3(pR[0],pR[1],pR[2]) ); }

	/** Returns the default rotation vector. 
	  * \param pR           First element is the texture rotation around the 
	  *                     U axis in degrees. Second element is the texture rotation 
	  *                     around the V axis in degrees. Third element is the texture 
	  *                     rotation around the W axis in degrees.
	  * \return             Input parameter filled with appropriate data.
	  * \remarks            The W axis is oriented towards the result of the 
	  *                     vector product of the U axis and V axis i.e. W = U x V.
	  */
	KFbxVector4& GetDefaultR(KFbxVector4& pR) const;

	/** Sets the default scale vector. 
	  * \param pS           The first element is scale applied to the texture width. 
	  *                     The second element is scale applied to the texture height. The third 
	  *                     and fourth elements have no effect on the texture. 
	  * \remarks            A scale value less than 1 stretches the texture.
	  *                     A scale value greater than 1 compresses the texture.
	  */
	inline void SetDefaultS(const KFbxVector4& pS) { Scaling.Set( fbxDouble3(pS[0],pS[1],pS[2]) ); }

	/** Returns the default scale vector. 
	  * \param pS           The first element is scale applied to the texture width. 
	  *                     The second element is scale applied to the texture height. The third 
	  *                     and fourth elements have no effect on the texture. 
	  * \remarks            A scale value less than 1 stretches the texture.
	  *                     A scale value greater than 1 compresses the texture.
	  */
	KFbxVector4& GetDefaultS(KFbxVector4& pS) const;

	//@}

	/**
	  * \name Default Alpha Value
	  */
	//@{

	/** Sets the default alpha.
	  *	\param pAlpha       A value on a scale from 0 to 1, with 0 being transparent.
      */
	void SetDefaultAlpha(double pAlpha);

	/** Returns the default alpha.
	  *	\return             A value on a scale from 0 to 1, with 0 being transparent.
	  */
	double GetDefaultAlpha() const;

	//@}

	/**
	  * \name Default Values Management By Numbers
	  * This set of functions provides direct access to the default values in number base.
      * U, V and W coordinates are mapped to the X, Y and Z coordinates of the default vectors
      * found in the "Default Values By Vector" section.
	  */
	//@{

    /** Sets translation.
	  * \param pU       Horizontal translation applied to a texture. A displacement 
	  *                 of one unit is equal to the texture's width after applying U scaling.
	  * \param pV       Vertical translation applied to a texture. A displacement 
	  *                 of one unit is equal to the texture's height after applying V scaling.
	  */
	void SetTranslation(double pU,double pV);

    /** Returns translation applied to the texture width.
      * \remarks        A displacement of one unit is equal to the texture's width 
	  *                 after applying U scaling.
	  */
    double GetTranslationU() const;

    /** Returns translation applied to the texture height.
      * \remarks        A displacement of one unit is equal to the texture's height 
	  *                 after applying V scaling.
	  */
    double GetTranslationV() const;

    /** Sets rotation.
	  * \param pU       Texture rotation around the U axis in degrees.
	  * \param pV       Texture rotation around the V axis in degrees.
	  * \param pW       Texture rotation around the W axis in degrees.
	  * \remarks        The W axis is oriented toward the result of the vector product of 
	  *                 the U and V axes that is W = U x V.
	  */
    void SetRotation(double pU, double pV, double pW = 0.0);

    //! Returns the texture rotation around the U axis in degrees.
    double GetRotationU() const;

    //! Returns the texture rotation around the V axis in degrees.
    double GetRotationV() const;

    //! Returns the texture rotation around the W axis in degrees.
    double GetRotationW() const;

    /** Sets scale.
	  * \param pU       Scale applied to the texture width. 
	  * \param pV       Scale applied to the texture height. 
	  * \remarks        A scale value less than 1 stretches the texture.
	  *                 A scale value greater than 1 compresses the texture.
	  */
	void SetScale(double pU,double pV);

    /** Returns scale applied to the texture width. 
	  * \remarks        A scale value less than 1 stretches the texture.
	  *                 A scale value greater than 1 compresses the texture.
	  */
    double GetScaleU() const;

    /** Returns scale applied to the texture height. 
	  * \remarks        A scale value less than 1 stretches the texture.
	  *                 A scale value greater than 1 compresses the texture.
	  */
    double GetScaleV() const;
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
	virtual KFbxObject& Copy(const KFbxObject& pObject);

	virtual bool operator==(KFbxTexture const& pTexture) const;

	void SetUVTranslation(KFbxVector2& pT);
	KFbxVector2& GetUVTranslation();
	void SetUVScaling(KFbxVector2& pS);
	KFbxVector2& GetUVScaling();

	KString GetTextureType();

protected:
    KFbxTexture(KFbxSdkManager& pManager, char const* pName);  

	virtual void Construct(const KFbxTexture* pFrom);
	virtual bool ConstructProperties(bool pForceSet);

    virtual bool PropertyNotify(eFbxPropertyNotify pType, KFbxProperty* pProperty);

	void Init();

	int mCropping[4]; // not a prop

    EAlphaSource mAlphaSource; // now unused in MB (always set to None); not a prop
	EMappingType mMappingType; // CurrentMappingType
	EPlanarMappingNormal mPlanarMappingNormal; // CurrentMappingType
	static KError smError;

	// Unsupported parameters in the FBX SDK, these are declared but not accessible.
	// They are used to keep imported and exported data identical.

	KFbxVector2 mUVScaling; // not a prop
	KFbxVector2 mUVTranslation; // not a prop

	friend class KFbxLayerContainer;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

inline EFbxType FbxTypeOf( KFbxTexture::EUnifiedMappingType const &pItem )		{ return eENUM; }
inline EFbxType FbxTypeOf( KFbxTexture::ETextureUse6 const &pItem )				{ return eENUM; }
inline EFbxType FbxTypeOf( KFbxTexture::EWrapMode const &pItem )				{ return eENUM; }
inline EFbxType FbxTypeOf( KFbxTexture::EBlendMode const &pItem )				{ return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXTEXTURE_H

