/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   This computer source code and related instructions and comments are the unpublished
   confidential and proprietary information of Autodesk, Inc. and are protected under
   applicable copyright and trade secret law.  They may not be disclosed to, copied or
   used by any third party without the prior written consent of Autodesk, Inc.

****************************************************************************************/

/*!  \file kfbxgobo.h
 */

#ifndef FBXFILESDK_KFBXIO_KFBXGOBO_H
#define FBXFILESDK_KFBXIO_KFBXGOBO_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/**
 * \brief A gobo is a filter placed over a spot light to project light patterns through fog on a surface.
 * You can also use an image file as a gobo, which cause the light to project an image, much like a projector. 
 */
class KFbxGobo
{
public:
	KFbxGobo(char* pName) :
	  mName(pName)
	  {
	  }

	//! Gobo name.
    KString mName;
    //! path and file name of the image file.
    KString mFileName;
    //! Flag that if shows the light projected on the ground.
    bool mDrawGroundProjection;
    //! Flag that lets you create a volumetric lighting effect by making the light stream visible.
    bool mVolumetricLightProjection;
    //! Flag that front facing light occurs when the camera view is looking down or up the light stream of a Spot light, which makes the light stream look three-dimensional.
    bool mFrontVolumetricLightProjection;
};


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXIO_KFBXGOBO_H

