/*!  \file kfbxtakeinfo.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXTAKEINFO_H
#define FBXFILESDK_KFBXPLUGINS_KFBXTAKEINFO_H

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
#include <fbxfilesdk/components/kbaselib/klib/ktime.h>
#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxThumbnail;

/** This KLayerInfo structure is used to identify a layer by name and id number.
  */ 
struct KLayerInfo
{
	KString	mName;
	int		mId;
};

/** This class contains take information from an imported file
  * or exported to an output file.
  *
  * A "take" is in fact a group of animation data grouped by name, so
  * the FBX file format can support many "animation takes" in an FBX file to mimic
  * how a movie is produced by making many takes of the same scene.
  *
  * The most used data is the "take name", other data are rarely used.
  * Example of use: to get the list of all 
  * animation take names of FBX file without loading all the scene content.
  * When a KFbxImporter is initialized, the take information can be read and can be available
  * before the long Import() step, this way, we can get the take info data very fast 
  * since we don't need to load all the animation scene data.
  * \code
  * // Ex: to get all take names in a FBX file
  * for(int lAnimStackCount=0; lAnimStackCount < lImporter->GetAnimStackCount(); lAnimStackCount++) 
  * {
  *   KFbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(lAnimStackCount);
  *   KString lTakeName = lTakeInfo->mName;
  * }
  * \endcode 
  */
class KFBX_DLL KFbxTakeInfo
{
public:

	/** Default constructor.
	  */
	KFbxTakeInfo();

    /** Destructor.
	  */
	virtual ~KFbxTakeInfo();

    /** Copy Constructor.
      * \param pTakeInfo        The take information to be copied. 
      */
	KFbxTakeInfo(const KFbxTakeInfo& pTakeInfo);

    /** Assignment operator.
      * \param pTakeInfo        The take information to be assigned. . 
      */
	KFbxTakeInfo& operator=(const KFbxTakeInfo& pTakeInfo);

	//! Take name.
	KString mName;

	/** The take name once it is imported in a scene.
	  * You can modify it if it must be different from the take name in the imported file.
	  * \remarks                This field is only used when importing a scene.
	  */
	KString mImportName;

	//! Take description.
	KString mDescription;

	/** Import/export flag.
	  * Set to \c true by default, set to \c false if the take must not be imported or exported.
	  */
	bool mSelect;

	//! Local time span, set to animation interval if it is left at the default value.
	KTimeSpan mLocalTimeSpan;

	//! Reference time span, set to animation interval if it is left at the default value.
	KTimeSpan mReferenceTimeSpan;

	/** Time value for offsetting the animation keys once they are imported in a scene.
	  * You can modify it if you need the animation of a take to be offset.
	  * The effect depends on the state of \c mImportOffsetType.
	  * \remarks                This field is only used when importing a scene.
	  */
	KTime mImportOffset;

	/** \enum  EImportOffsetType       Import offset types.
	  * - \e eABSOLUTE
	  * - \e eRELATIVE
	  */
	typedef enum  
	{
		eABSOLUTE,
		eRELATIVE
	} EImportOffsetType;

	/** Import offset type.
	  * If set to \c eABSOLUTE, \c mImportOffset gives the absolute time of 
	  * the first animation key and the appropriate time shift is applied 
	  * to all of the other animation keys.
	  * If set to \c eRELATIVE, \c mImportOffset gives the relative time 
	  * shift applied to all animation keys.
	  */
	EImportOffsetType mImportOffsetType;

	/**	Returns the thumbnail of the take.
	  * \return                 Pointer to the thumbnail. Maybe NULL if not found.
	  */
	KFbxThumbnail* GetTakeThumbnail();

	/** Sets the take thumbnail.
	  * \param pTakeThumbnail   The referenced thumbnail object.
	  */
	void SetTakeThumbnail(KFbxThumbnail* pTakeThumbnail);

    /** Copies the layer information from the take information.
      * \param pTakeInfo          The take information to be copied.
      */
	void CopyLayers(const KFbxTakeInfo& pTakeInfo);

    //! List of each layer's information.
	KArrayTemplate<KLayerInfo*>	mLayerInfoList;

    //! Current Layer.
	int							mCurrentLayer;

protected:
    //! The take thumbnail.
	KFbxThumbnail* mTakeThumbnail;
};

typedef KFbxTakeInfo* HKFbxTakeInfo;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXTAKEINFO_H

