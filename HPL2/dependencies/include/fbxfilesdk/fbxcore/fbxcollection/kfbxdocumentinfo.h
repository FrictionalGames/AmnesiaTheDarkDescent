/*!  \file kfbxDocumentInfo.h
 */
#ifndef FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXDOCUMENTINFO_H
#define FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXDOCUMENTINFO_H

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

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/kfbxplugins/kfbxobject.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxThumbnail;
class KFbxSdkManager;

/** This class contains scene thumbnails and user-defined summary data.
  */
class KFBX_DLL KFbxDocumentInfo : public KFbxObject
{
	KFBXOBJECT_DECLARE(KFbxDocumentInfo,KFbxObject);

public:
	/**
	* \name Public properties
	*/
	//@{
		/** This property contains the last saved URL.
		  *
		  * To retrieve the value of this property, use LastSavedUrl.Get().
		  * To set the value of this property, use LastSavedUrl.Set(fbxString).
		  *
		  * The default value is empty.
		  */
		KFbxTypedProperty<fbxString> LastSavedUrl;

		/** This property contains the URL.
		  *
		  * To retrieve the value of this property, use Url.Get().
		  * To set the value of this property, use Url.Set(fbxString).
		  *
		  * The default value is empty.
		  */
		KFbxTypedProperty<fbxString> Url;

		/** Parent property for all properties related to creation. These properties
		  * should be set once when the file is created, and you should not change them
		  * during subsequent save or reload operations.            
		  * The default properties are listed below, but application vendors can add new
		  * properties under this parent property.
		  */
		KFbxProperty Original;

		/** This property contains the name of the original application vendor.
		  *
		  * To retrieve the value of this property, use Original_ApplicationVendor.Get().
		  * To set the value of this property, use Original_ApplicationVendor.Set(fbxString).
		  *
		  * The default value is empty.
		  */
		KFbxTypedProperty<fbxString> Original_ApplicationVendor;

		/** This property contains the original application name.
		  *
		  * To retrieve the value of this property, use Original_ApplicationName.Get().
		  * To set the value of this property, use Original_ApplicationName.Set(fbxString).
		  *
		  * The default value is empty.
		  */
		KFbxTypedProperty<fbxString> Original_ApplicationName;

		/** This property contains the version of the original application.
		  *
		  * To retrieve the value of this property, use Original_ApplicationVersion.Get().
		  * To set the value of this property, use Original_ApplicationVersion.Set(fbxString).
		  *
		  * The default value is empty.
		  */
		KFbxTypedProperty<fbxString> Original_ApplicationVersion;

		/** This property contains the original file name.
		  *
		  * To retrieve the value of this property, use Original_FileName.Get().
		  * To set the value of this property, use Original_FileName.Set(fbxString).
		  *
		  * The default value is empty.
		  */
		KFbxTypedProperty<fbxString> Original_FileName;

		/** This property contains the original date and time.
		  *
		  * To retrieve the value of this property, use Original_DateTime_GMT.Get().
		  * To set the value of this property, use Original_DateTime_GMT.Set(fbxString).
		  *
		  * The default value is 0.
		  * \remarks                The date/time should use GMT time format. 
		  */
		KFbxTypedProperty<fbxDateTime> Original_DateTime_GMT;

		/** The parent property for all last saved-related properties.
		  * These properties update every time a file is saved.               
		  * The default properties are below, but application vendors can add new
		  * properties under this parent property.             
		  * The file creator must set both the original and last saved properties.
		  */
		KFbxProperty LastSaved;

		/** This property contains the last saved application vendor.
		  *
		  * To retrieve the value of this property, use LastSaved_ApplicationVendor.Get().
		  * To set the value of this property, use LastSaved_ApplicationVendor.Set(fbxString).
		  *
		  * The default value is empty.
		  */
		KFbxTypedProperty<fbxString> LastSaved_ApplicationVendor;

		/** This property contains the last saved application name.
		  *
		  * To retrieve the value of this property, use LastSaved_ApplicationName.Get().
		  * To set the value of this property, use LastSaved_ApplicationName.Set(fbxString).
		  *
		  * The default value is empty.
		  */
		KFbxTypedProperty<fbxString> LastSaved_ApplicationName;

		/** This property contains the last saved application version.
		  *
		  * To retrieve the value of this property, use LastSaved_ApplicationVersion.Get().
		  * To set the value of this property, use LastSaved_ApplicationVersion.Set(fbxString).
		  *
		  * The default value is empty.
		  */
		KFbxTypedProperty<fbxString> LastSaved_ApplicationVersion;

		/** This property contains the last saved date and time.
		  *
		  * To retrieve the value of this property, use LastSaved_DateTime_GMT.Get().
		  * To set the value of this property, use LastSaved_DateTime_GMT.Set(fbxString).
		  *
		  * The default value is 0.
		  *
		  * \remarks The date/time should use GMT time format.
		  */
		KFbxTypedProperty<fbxDateTime> LastSaved_DateTime_GMT;

		/** This property points at the ".fbm" folder that is created when 
		  * reading a FBX file that has embedded data. The embedded data 
		  * is not saved in the FBX file. 
		  *
		  * The default value is empty.
		  */
		KFbxTypedProperty<fbxString> EmbeddedUrl;
	//@}

	/** \name User-defined summary data.
	  * These are user-completed fields that identify or classify the files.
	  */
	//@{
		KString mTitle;		//! Title.
		KString mSubject;	//! Subject.
		KString mAuthor;	//! Author
		KString mKeywords;	//! Keywords.
		KString mRevision;	//! Revision.
		KString mComment;	//! Comment.
	//@}

	/**
	  * \name Scene Thumbnail.
	  */
	//@{
		/** Returns the thumbnail for the scene.
		  * \return                 Pointer to the thumbnail.
		  */
		KFbxThumbnail* GetSceneThumbnail();

		/** Sets the thumbnail for the scene.
		  * \param pSceneThumbnail  Pointer to the thumbnail.
		  */
		void SetSceneThumbnail(KFbxThumbnail* pSceneThumbnail);
	//@}

	/** Clears the content.
	  * Resets all the strings to an empty string and clears 
	  * the pointer to the thumbnail.
	  */
	void Clear();

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

protected:
	//! Object Constructors and Destructor.
	KFbxDocumentInfo(KFbxSdkManager& pManager,char const *pName);

	bool			ConstructProperties(bool pForceSet);
	KFbxThumbnail*	mSceneThumbnail;

	friend class KStreamFbx;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 
};

typedef KFbxDocumentInfo* HKFbxDocumentInfo;

// Backward compatibility
// --------------------------------------------------------------

//	typedef KFbxDocumentInfo* HKFbxDocumentInfo;
//	typedef KFbxDocumentInfo* HKFbxDocumentInfo;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXDOCUMENTINFO_H

