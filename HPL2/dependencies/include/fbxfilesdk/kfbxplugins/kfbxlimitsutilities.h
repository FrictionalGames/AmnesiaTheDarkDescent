/*!  \file kfbxlimitsutilities.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXLIMITSUTILITIES_H
#define FBXFILESDK_KFBXPLUGINS_KFBXLIMITSUTILITIES_H

/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
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

#include <fbxfilesdk/kfbxmath/kfbxvector4.h>
#include <fbxfilesdk/kfbxplugins/kfbxnodelimits.h>
#include <fbxfilesdk/kfbxplugins/kfbxnode.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** \brief This class represent a utility of limits for transforms.
* \nosubgrouping
*/
class KFBX_DLL KFbxLimitsUtilities
{
public:
    /** \enum ELimitType    Limit Type.
    * - \e eT               Translation type
    * - \e eR               Rotation type
    * - \e eS               Scale type
    */
	typedef enum 
	{
		eT,
		eR,
		eS,
	} ELimitType;

    /** \enum ERotationType            Rotation type.
    * - \e eROTATION_TYPE_QUATERNION   Quaternion rotation type
    * - \e eROTATION_TYPE_EULER        Euler rotation type
    */
	typedef enum 
	{ 
		eROTATION_TYPE_QUATERNION, 
		eROTATION_TYPE_EULER, 
	} ERotationType ;

    /** \enum ERotationClampType              Rotation clamp type.
    * - \e eROTATION_CLAMP_TYPE_RECTANGULAR   Rectangular clamp type
    * - \e eROTATION_CLAMP_TYPE_ELIPSOID      Ellipsoid clamp type
    */
	typedef enum 
	{ 
		eROTATION_CLAMP_TYPE_RECTANGULAR, 
		eROTATION_CLAMP_TYPE_ELIPSOID, 
	} ERotationClampType ;


	KFbxLimitsUtilities(KFbxNodeLimits* pLimits);
	KFbxNodeLimits* mLimits;

    /**
    * \name Getter/Setter functions
    */
    //@{

	void SetAuto(ELimitType pType, bool pAuto);
	bool GetAuto(ELimitType pType) const;

	void SetEnable(ELimitType pType, bool pEnable);
	bool GetEnable(ELimitType pType) const;

	void SetDefault(ELimitType pType, KFbxVector4 pDefault);
	KFbxVector4 GetDefault(ELimitType pType) const;

	void SetMin(ELimitType pType, KFbxVector4 pMin);
	KFbxVector4 GetMin(ELimitType pType) const;

	void SetMax(ELimitType pType, KFbxVector4 pMax);
	KFbxVector4 GetMax(ELimitType pType) const;

	void SetRotationType(ERotationType pType);
	ERotationType GetRotationType() const;

	ERotationClampType GetRotationClampType() const;

	void SetRotationAxis(KFbxVector4 pRotationAxis);
	KFbxVector4 GetRotationAxis() const;

	void SetAxisLength(double pLength);
	double GetAxisLength() const;

	void UpdateAutomatic(KFbxNode* pNode);
	KFbxVector4 GetEndPointTranslation(KFbxNode* pNode) const;
	KFbxVector4 GetEndSite(KFbxNode* pNode) const;

    //@}

	double mAxisLength; 
};


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXLIMITSUTILITIES_H

