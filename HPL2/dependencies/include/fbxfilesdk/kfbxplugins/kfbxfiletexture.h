/*!  \file kfbxfiletexture.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXFILETEXTURE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXFILETEXTURE_H

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
#include <fbxfilesdk/kfbxplugins/kfbxtexture.h>

// FBX namespace
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** This class describes image mapping on top of geometry.
  * \note To apply a texture to geometry, first connect the 
  * geometry to a KFbxSurfaceMaterial object (e.g. KFbxSurfaceLambert)
  * and then connect one of its properties (e.g. Diffuse) to the 
  * KFbxFileTexture object.
  * \see KFbxSurfaceLambert
  * \see KFbxSurfacePhong
  * \see KFbxSurfaceMaterial
  * \note For some example code, see also the CreateTexture() function
  * in the ExportScene03 of FBX SDK examples.
  * \nosubgrouping
  */
class KFBX_DLL KFbxFileTexture : public KFbxTexture
{
	KFBXOBJECT_DECLARE(KFbxFileTexture,KFbxTexture);

	public:
	/**
	  * \name Texture Properties
	  */
	//@{

		// Material management

        /** This property handles the material use.
          * Default value is false.
          */
		KFbxTypedProperty<fbxBool1>				UseMaterial;

        /** This property handles the Mipmap use.
          * Default value is false.
          */
		KFbxTypedProperty<fbxBool1>				UseMipMap;

	/** Resets the default texture values.
	  * \remarks            The texture file name is not reset.
	  */
	void Reset();

    /** Sets the associated texture file. 
      * \param pName        The absolute path of the texture file.   
      * \return             \c True if successful, returns \c false otherwise.
	  *	\remarks            The texture file name must be valid, you cannot leave the name empty.
      */
    bool SetFileName(char const* pName);

    /** Sets the associated texture file. 
      * \param pName        The relative path of the texture file.   
      * \return             \c True if successful, returns \c false otherwise.
	  *	\remarks            The texture file name must be valid.
      */
    bool SetRelativeFileName(char const* pName);

    /** Returns the absolute texture file path.
	  * \return             The absolute texture file path.
	  * \remarks            An empty string is returned if KFbxFileTexture::SetFileName() has not been called before.
	  */
    char const* GetFileName () const;

    /** Returns the relative texture file path.
	  * \return             The relative texture file path.
	  * \remarks            An empty string is returned if KFbxFileTexture::SetRelativeFileName() has not been called before.
	  */
    char const* GetRelativeFileName() const;

	/** \enum EMaterialUse      Specify if texture uses model material.
	  */
    typedef enum 
    {
		//! Texture uses model material.
        eMODEL_MATERIAL,
		//! Texture does not use model material.
        eDEFAULT_MATERIAL
    } EMaterialUse;

    /** Sets the material use.
	  * \param pMaterialUse         Specify how texture uses model material.
	  */
    void SetMaterialUse(EMaterialUse pMaterialUse);

    /** Returns the material use.
	  * \return                     How the texture uses model material.
	  */
    EMaterialUse GetMaterialUse() const;


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

	bool operator==(KFbxFileTexture const& pTexture) const;

	KString& GetMediaName();
	void SetMediaName(char const* pMediaName);

protected:
    KFbxFileTexture(KFbxSdkManager& pManager, char const* pName);  

	virtual void Construct(const KFbxFileTexture* pFrom);
	virtual bool ConstructProperties(bool pForceSet);

	void Init();
	void SyncVideoFileName(char const* pFileName);
	void SyncVideoRelativeFileName(char const* pFileName);

	KString mFileName;
	KString mRelativeFileName;
	KString mMediaName; // not a prop

	friend class KFbxLayerContainer;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

//typedef KFbxFileTexture* HKFbxFileTexture;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXFILETEXTURE_H

