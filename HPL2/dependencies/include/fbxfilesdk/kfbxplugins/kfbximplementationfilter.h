/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbximplementationfilter.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXIMPLEMENTATIONFILTER_H
#define FBXFILESDK_KFBXPLUGINS_KFBXIMPLEMENTATIONFILTER_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxobjectfilter.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxCriteria;

/** \brief This object represents a shading node filter criteria 
  * based on the shading node implementation.
  * \nosubgrouping
  */
class KFBX_DLL KFbxImplementationFilter : public KFbxObjectFilter
{

public:

	/** The default shading API.
      */
	static const char * sCHR_ANY_SHADING_API;

	/** The default shading API version.
      */
	static const char * sCHR_ANY_SHADING_API_VERSION;

	/** The default shading language.
      */
	static const char * sCHR_ANY_SHADING_LANGUAGE;
	
	/** The default shading language version.
      */
	static const char * sCHR_ANY_SHADING_LANGUAGE_VERSION;


    /** Constructor
	  * \param pShadingAPI a string containing the implementation API name:
	  *		MentalRay
	  *		OpenGL
	  *		DirectX
	  *
	  * \param pShadingAPIVersion a string containing the implementation API version:
	  *		eg. 1.0
	  *
	  * \param pShadingLanguage a string identifying the implementation language name:
	  *		GLSL	= GL Shading Language
	  *		HLSL	= High Level Shading Language
	  *		CGFX	= CG effect(NVidia)
	  *		RIB		= RenderMan (RIB)
	  *		etc...
	  *
	  * \param pShadingLanguageVersion a string identifying the implementation language version:
	  *		eg. 1.0
	  *
	  * \remarks by default the created criteria correspond to any shader
	  */
	KFbxImplementationFilter(
		const char * pShadingAPI				= sCHR_ANY_SHADING_API,
		const char * pShadingAPIVersion			= sCHR_ANY_SHADING_API_VERSION,
		const char * pShadingLanguage			= sCHR_ANY_SHADING_LANGUAGE,
		const char * pShadingLanguageVersion	= sCHR_ANY_SHADING_LANGUAGE_VERSION
	);

    //! Destructor.
    virtual ~KFbxImplementationFilter();

	/** Tells if this filter matches the given shading node implementation
	  * \param pObjectPtr The given shading node implementation
	  */
	virtual bool Match(const KFbxObject * pObjectPtr) const;

	//! Stores the shading API
	KString mShadingAPI;

	//! Stores the shading API Version
	KString mShadingAPIVersion;

	//! Stores the shading language
	KString mShadingLanguage;

	//! Stores the shading language version
	KString mShadingLanguageVersion;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

	/** Utility method to determine if the given object is a shading node
	  * that we recognize.
	  */
	static bool IsShadingObject( const KFbxObject* pObject );

	/** Returns a criteria suitable for use with querying connections 
	  * to shading nodes that we recognize, on KFbxObjects.
	  */
	static KFbxCriteria Criteria();
private:

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXIMPLEMENTATIONFILTER_H

