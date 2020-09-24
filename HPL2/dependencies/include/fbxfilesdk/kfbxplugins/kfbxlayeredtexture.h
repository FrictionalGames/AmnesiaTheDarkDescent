/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxlayeredtexture.h
 */
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXLAYEREDTEXTURE_H 
#define FBXFILESDK_KFBXPLUGINS_KFBXLAYEREDTEXTURE_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>
#include <fbxfilesdk/kfbxplugins/kfbxtexture.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** KFbxLayeredTexture is a combination of multiple textures(KFbxTexture) blended sequentially.
  * For example, you can access individual texture by:
  * \code
  * KFbxTexture* pIndiTexture = lLayeredTexture->GetSrcObject(KFbxTexture::ClassId, pTextureIndex);
  * \endcode
  * Another example to construct a layered texture with two sub textures.
  * \code
  * KFbxFileTexture *background, *file1;
  * KFbxLayeredTexture* layeredTexture;
  *
  * // connect two file textures to a layered texture via OO connections
  * layeredTexture->ConnectSrcObject(background);
  * layeredTexture->ConnectSrcObject(file1);
  *
  * // set the second file texture's blend mode and alpha.
  * layeredTexture->SetTextureBlendMode(1, KFbxLayeredTexture::eOVER);
  * layeredTexture->SetTextureAlpha(1, 0.5);
  * \endcode
  * \nosubgrouping
  * \see KFbxTexture
  */
class KFBX_DLL KFbxLayeredTexture : public KFbxTexture
{
	KFBXOBJECT_DECLARE(KFbxLayeredTexture,KFbxTexture);

public:
	/** \enum EBlendMode Blend modes.
      * - \e eTRANSLUCENT,        The new texture layer is transparent (depending on the Alpha value).
      * - \e eADDITIVE,           Add the color of the new texture to the previous texture.
      * - \e eMODULATE,           Multiples the color value of the new texture by the color values of all previous layers of texture.
      * - \e eMODULATE2,          Multiples the color value of the new texture by two and then by the color values of all previous layers of texture.
      * - \e eOVER,               Blends the new texture over top of the old texture, according to the new texture's alpha channel.
      * - \e eNORMAL,		      The colors of the two layers will not interact in any way, and it will display the full value of the colors in layer 1.
      * - \e eDISSOLVE,           Dissolve makes the lower layer take on the colors of the top layer, and how much depends on the opacity of the upper layer. 
      * - \e eDARKEN,		      Darken compares each pixel value of the upper layer to its counterpart's pixel value of the lower layer and chooses the darker of the two to display.
      * - \e eCOLORBURN,          Color Burn burns in the color of the upper layer with the lower layer. No part of the image will get lighter.
      * - \e eLINEARBURN, 	      Linear Burn works like multiply but the results are more intense.
      * - \e eDARKERCOLOR,        This blend mode simply divides pixel values of one layer with the other.
      * - \e eLIGHTEN,		      Lighten compares the two layers pixel for pixel and uses the lightest pixel value. No part of the image gets darker. 
      * - \e eSCREEN,		      Screen brightens by lightning the lower layer based on the lightness of the upper layer
      * - \e eCOLORDODGE,         Color Dodge dodges the lower layer with the upper layer, resulting in a lighter image. No part of the image will be darkened.
      * - \e eLINEARDODGE,        Linear Dodge works like screen but with more intense results.
      * - \e eLIGHTERCOLOR,       This blend mode has the opposite effect of the Darker Color mode. It compares all the values in both layers, then displays the lightest values.
      * - \e eSOFTLIGHT,		  Soft Light will multiply the dark tones and screen the light tones.
      * - \e eHARDLIGHT,		  Hard Light multiplies the dark colors and screens the light colors.
      * - \e eVIVIDLIGHT,         Vivid Light will dodges or burn the lower layer pixels depending on whether the upper layer pixels are brighter or darker than neutral gray. It works on the contrast of the lower layer.
      * - \e eLINEARLIGHT,        Linear Light is the same as Vivid light but it works on the brightness of the lower layer.
      * - \e ePINLIGHT, 		  Pin Light changes the lower layer pixels depending on how bright the pixels are in the upper layer.
      * - \e eHARDMIX,		      Produces either white or black, depending on similarities between A and B.
      * - \e eDIFFERENCE, 	      Difference reacts to the differences between the upper and lower layer pixels.
      * - \e eEXCLUSION, 	      Exclusion uses the darkness of the lower layer to mask the difference between upper and lower layers.
      * - \e eSUBTRACT,           The result color is the foreground color subtracted from the background color. The result color is then applied over the background color using the foreground alpha to define the opacity of the result.
      * - \e eDIVIDE,             This blend mode simply divides pixel values of one layer with the other.
      * - \e eHUE, 			      Hue changes the hue of the lower layer to the hue of the upper layer but leaves brightness and saturation alone.
      * - \e eSATURATION,	      Saturation changes the saturation of the lower layer to the hue of the upper layer but leaves brightness and hue alone.
      * - \e eCOLOR,		      Color changes the hue and saturation of the lower layer to the hue and saturation of the upper layer but leaves luminosity alone.
      * - \e eLUMINOSITY          Luminosity changes the luminosity of the lower layer to the luminosity of the upper layer while leaving hue and saturation the same.
	  */
	typedef enum 
	{
		eTRANSLUCENT,
		eADDITIVE,
		eMODULATE,
		eMODULATE2,
        eOVER,
        eNORMAL,		
        eDISSOLVE,
        eDARKEN,			
        eCOLORBURN,
        eLINEARBURN, 	
        eDARKERCOLOR,
        eLIGHTEN,			
        eSCREEN,		
        eCOLORDODGE,
        eLINEARDODGE,
        eLIGHTERCOLOR,
        eSOFTLIGHT,		
        eHARDLIGHT,		
        eVIVIDLIGHT,
        eLINEARLIGHT,
        ePINLIGHT, 		
        eHARDMIX,		
        eDIFFERENCE, 		
        eEXCLUSION, 		
        eSUBTRACT,
        eDIVIDE,
        eHUE, 			
        eSATURATION,		
        eCOLOR,		
        eLUMINOSITY
	} EBlendMode;

	/** Equivalence operator.
	  * \param pOther                      The object for comparison.
	  * \return                            \c True if pOther is equivalent to this object, returns \c false otherwise.
	  */
	bool operator==( const KFbxLayeredTexture& pOther ) const;

    /** Sets the blending mode of a specified texture.
      * \param pIndex                      The texture index.
      * \param pMode                       The blend mode to be set.
      * \return                            \c True if successful, returns \c false otherwise.
      */
    bool SetTextureBlendMode( int pIndex, EBlendMode pMode ); 

    /** Returns the blending mode of a specified texture
      * \param pIndex                      The texture index.
      * \param pMode                       The parameter that will hold the returned blend mode.
      * \return                            \c True if successful, returns \c false otherwise.
      */
    bool GetTextureBlendMode( int pIndex, EBlendMode& pMode ) const;

     /** Sets the alpha of a specified texture.
      * \param pIndex                      The texture index.
      * \param pAlpha                      The alpha to be set.
      * \return                            \c True if successful, returns \c false otherwise.
      */
    bool SetTextureAlpha( int pIndex, double pAlpha );

    /** Returns the alpha of a specified texture
      * \param pIndex                      The texture index.
      * \param pAlpha                      The parameter that will hold the returned alpha.
      * \return                            \c True if successful, returns \c false otherwise.
      */
    bool GetTextureAlpha( int pIndex, double& pAlpha ) const;

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

    struct InputData
    {
        EBlendMode mBlendMode;
        double mAlpha;
    };

public:
    KArrayTemplate<InputData> mInputData;

protected:
    KFbxLayeredTexture(KFbxSdkManager& pManager, char const* pName);  

    virtual bool ConnecNotify (KFbxConnectEvent const &pEvent);

    bool RemoveInputData( int pIndex );
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

inline EFbxType FbxTypeOf( KFbxLayeredTexture::EBlendMode const &pItem )				{ return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXLAYEREDTEXTURE_H

