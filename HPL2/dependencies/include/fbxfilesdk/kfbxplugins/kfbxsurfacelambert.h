/*!  \file kfbxsurfacelambert.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSURFACELAMBERT_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSURFACELAMBERT_H

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

#include <fbxfilesdk/kfbxplugins/kfbxsurfacematerial.h>
#include <fbxfilesdk/kfbxplugins/kfbxcolor.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** This class contains settings for Lambert Materials.
  * \nosubgrouping
  */
class KFBX_DLL KFbxSurfaceLambert : public KFbxSurfaceMaterial
{
	KFBXOBJECT_DECLARE(KFbxSurfaceLambert,KFbxSurfaceMaterial);

public:
	/**
	 * \name Material properties
	 */
	//@{
	
    //! Emissive color property.
    KFbxPropertyDouble3 Emissive;

	/** Returns the emissive color property.
     *  \return The emissive color.
     * \remark This method is deprecated. The property \e Emissive can be directly accessed. 
	 */
	K_DEPRECATED KFbxPropertyDouble3 GetEmissiveColor() const;
	
    /** Emissive factor property. This factor is used to
	 *  attenuate the emissive color.
     */
    KFbxPropertyDouble1 EmissiveFactor;

	/** Returns the emissive factor property. This factor is used to
	 *  attenuate the emissive color.
     *  \return The emissive factor.
     * \remark This method is deprecated. The property \e EmissiveFactor can be directly accessed. 
	 */
	K_DEPRECATED KFbxPropertyDouble1 GetEmissiveFactor() const;
	
    //! Ambient color property.
    KFbxPropertyDouble3 Ambient;

	/** Returns the ambient color property.
     *  \return The ambient color.
     * \remark This method is deprecated. The property \e Ambient can be directly accessed. 
	 */
	K_DEPRECATED KFbxPropertyDouble3 GetAmbientColor() const;
	
    /** Ambient factor property. This factor is used to
	 * attenuate the ambient color.
     */
    KFbxPropertyDouble1 AmbientFactor;

	/** Returns the ambient factor property. This factor is used to
	 * attenuate the ambient color.
     *  \return The ambient factor.
     * \remark This method is deprecated. The property \e AmbientFactor can be directly accessed. 
	 */
	K_DEPRECATED KFbxPropertyDouble1 GetAmbientFactor() const;
	
    //! Diffuse color property.
    KFbxPropertyDouble3 Diffuse;

	/** Returns the diffuse color property.
     *  \return The diffuse color.
     * \remark This method is deprecated. The property \e Diffuse can be directly accessed. 
	 */
	K_DEPRECATED KFbxPropertyDouble3 GetDiffuseColor() const;
	
    /** Diffuse factor property. This factor is used to
	 * attenuate the diffuse color.
     */
    KFbxPropertyDouble1 DiffuseFactor;

	/** Returns the diffuse factor property. This factor is used to
	 * attenuate the diffuse color.
     *  \return The diffuse factor.
     * \remark This method is deprecated. The property \e DiffuseFactor can be directly accessed. 
	 */
	K_DEPRECATED KFbxPropertyDouble1 GetDiffuseFactor() const;

    /** NormalMap property. This property can be used to specify the distortion of the surface 
     * normals and create the illusion of a bumpy surface.
     */
   	KFbxPropertyDouble3 NormalMap;

    /** Bump property. This property is used to distort the
	 * surface normal and create the illusion of a bumpy surface.
     */
   	KFbxPropertyDouble3 Bump;

	/** Returns the bump property. This property is used to distort the
	 * surface normal and create the illusion of a bumpy surface.
     *  \return The bump property.
     * \remark This method is deprecated. The property \e Bump can be directly accessed. 
	 */
	K_DEPRECATED KFbxPropertyDouble3 GetBump() const;

    /** Bump factor property. This factor is used to
     * make a surface more or less bumpy.
     */
    KFbxPropertyDouble1 BumpFactor;

    /** Returns the bump factor property. This factor is used to
     * make a surface more or less bumpy.
     * \return The bump factor.
     * \remark This method is deprecated. The property \e BumpFactor can be directly accessed. 
    */
    K_DEPRECATED KFbxPropertyDouble1 GetBumpFactor() const;
	
    //! Transparent color property.
    KFbxPropertyDouble3 TransparentColor;

	/** Returns the transparent color property. This property is used to make a
	 * surface more or less transparent.
     *  \return The transparent color.
     * \remark This method is deprecated. The property \e TransparentColor can be directly accessed. 
	 */
	K_DEPRECATED KFbxPropertyDouble3 GetTransparentColor() const;
	
    /** Transparency factor property.  This property is used to make a
	 * surface more or less opaque (0 = opaque, 1 = transparent).
     */
    KFbxPropertyDouble1 TransparencyFactor;

	/** Returns the transparency factor property. This property is used to make a
	 * surface more or less opaque (0 = opaque, 1 = transparent).
     *  \return The transparency factor.
     * \remark This method is deprecated. The property \e TransparencyFactor can be directly accessed. 
	 */
	K_DEPRECATED KFbxPropertyDouble1 GetTransparencyFactor() const;

    //! Displacement color property.
    KFbxPropertyDouble3 DisplacementColor;

	/** Returns the displacement color property. This property is used to make a
	 * surface more or less displaced.
     *  \return The displacement color.
     * \remark This method is deprecated. The property \e DisplacementColor can be directly accessed. 
	 */
	K_DEPRECATED KFbxPropertyDouble3 GetDisplacementColor() const;
	
    //! Displacement factor property.
    KFbxPropertyDouble1 DisplacementFactor;

    /** Returns the displacement factor property. This property is used to make a
	 * surface more or less displaced.
     *  \return The displacement factor
     * \remark This method is deprecated. The property \e DisplacementFactor can be directly accessed. 
	 */
	K_DEPRECATED KFbxPropertyDouble1 GetDisplacementFactor() const;
	
    //! Vector displacement color property.
    KFbxPropertyDouble3 VectorDisplacementColor;

    //! Vector displacement factor property.
    KFbxPropertyDouble1 VectorDisplacementFactor;

	//@}

	//////////////////////////////////////////////////////////////////////////
	// Static values
	//////////////////////////////////////////////////////////////////////////

	/**
	  * \name Default property values
	  */
	//@{

	static const fbxDouble3 sEmissiveDefault;
	static const fbxDouble1 sEmissiveFactorDefault;

	static const fbxDouble3 sAmbientDefault;
	static const fbxDouble1 sAmbientFactorDefault;

	static const fbxDouble3 sDiffuseDefault;
	static const fbxDouble1 sDiffuseFactorDefault;
	
	static const fbxDouble3 sBumpDefault;
    static const fbxDouble3 sNormalMapDefault;
    static const fbxDouble1 sBumpFactorDefault;

	static const fbxDouble3 sTransparentDefault;
	static const fbxDouble1 sTransparencyFactorDefault;

    static const fbxDouble3 sDisplacementDefault;
    static const fbxDouble1 sDisplacementFactorDefault;

    static const fbxDouble3 sVectorDisplacementDefault;
    static const fbxDouble1 sVectorDisplacementFactorDefault;

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
protected:
	KFbxSurfaceLambert(KFbxSdkManager& pManager, char const* pName);

	virtual bool ConstructProperties(bool pForceSet);

	// Local
	void Init();	

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 

};

typedef KFbxSurfaceMaterial* HKFbxSurfaceMaterial;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSURFACELAMBERT_H

