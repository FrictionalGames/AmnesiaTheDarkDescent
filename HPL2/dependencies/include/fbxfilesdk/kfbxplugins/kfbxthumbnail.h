/*!  \file kfbxthumbnail.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXTHUMBNAIL_H
#define FBXFILESDK_KFBXPLUGINS_KFBXTHUMBNAIL_H

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

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxThumbnailMembers;
class KFbxSdkManager;

/** Simple class to hold RGBA values of a thumbnail image.
  * \nosubgrouping
  */
class KFBX_DLL KFbxThumbnail : public KFbxObject
{
	KFBXOBJECT_DECLARE(KFbxThumbnail,KFbxObject);

public:

	/**
	  * \name Thumbnail properties
	  */
	//@{

	//! Pixel height of the thumbnail image
	KFbxTypedProperty<fbxInteger1> CustomHeight;

	//! Pixel width of the thumbnail image
	KFbxTypedProperty<fbxInteger1> CustomWidth;

	/** \enum EDataFormat Data format.
	  * - \e eRGB_24
	  * - \e eRGBA_32
	  */
	typedef enum 
	{
		eRGB_24, // 3 components
		eRGBA_32 // 4 components
	} EDataFormat;

	/** Set the data format.
	  * \param pDataFormat Data format identifier.
	  */
	void SetDataFormat(EDataFormat pDataFormat);

	/** Get the data format.
	  * \return Data format identifier for the thumbnail.
	  */
	EDataFormat GetDataFormat() const;


	/** \enum EImageSize Image size.
	  * - \e eNOT_SET
	  * - \e e64x64
	  * - \e e128x128
	  * - \e eCUSTOM_SIZE
	  */
	typedef enum 
	{
		eNOT_SET = 0,
		e64x64   = 64,
		e128x128 = 128,
		eCUSTOM_SIZE = -1
	} EImageSize;

	/** Set the thumbnail dimensions.
	  * \param pImageSize Image size identifier.
	  */
	void SetSize(EImageSize pImageSize);	

	/** Get the thumbnail dimensions.
	  * \return Image size identifier.
	  */
	EImageSize GetSize() const;

	/** Get the thumbnail dimensions in bytes.
	  * \return Thumbnail size in bytes.
	  */
	unsigned long GetSizeInBytes() const;


	//@}

	/**
	  * \name Thumbnail data access
	  */
	//@{

	/** Fill the thumbnail image.
	  * \param pImage Pointer to the image data. A copy
	  * of the image data will be made.
	  *	\remarks This pointer must point to a buffer region
	  * that is at least Width * Height * Component count
	  * bytes long. This pointer points to the upper left
	  * corner of the image.
	  * \remarks You must set the data format and the dimensions
	  * before calling this function. If the image size is set to eCUSTOM_SIZE
	  * the CustomHeight and CustomWidth properties must be set before calling
	  * this function.
	  * \return \c true if the thumbnail properties were set
	  * before calling this function. \c false otherwise.
	  */
	bool SetThumbnailImage(const kUByte* pImage);

	/** Get the thumbnail image.
	  * \return Pointer to the image data, or \c NULL if the
	  * thumbnail is empty.
	  */
	kUByte* GetThumbnailImage() const;

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

	virtual KFbxObject&	Copy(const KFbxObject& pObject);

protected:
    //! Constructor.
    KFbxThumbnail(KFbxSdkManager& pManager, char const* pName);

	virtual void Construct(const KFbxThumbnail* pFrom);
	virtual bool ConstructProperties( bool pForceSet );
	virtual void Destruct(bool pRecursive, bool pDependents);

	// Members
	KFbxSdkManager* mManager;
	KFbxThumbnailMembers* mMembers;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXTHUMBNAIL_H

