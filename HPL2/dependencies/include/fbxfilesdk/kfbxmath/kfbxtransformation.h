/*!  \file kfbxtransformation.h
 */
 
#ifndef FBXFILESDK_KFBXMATH_KFBXTRANSFORMATION_H
#define FBXFILESDK_KFBXMATH_KFBXTRANSFORMATION_H

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

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** \enum ERotationOrder Rotation order flags.
  * - \e eEULER_XYZ
  * - \e eEULER_XZY
  * - \e eEULER_YZX 
  * - \e eEULER_YXZ 
  * - \e eEULER_ZXY 
  * - \e eEULER_ZYX
  * - \e eSPHERIC_XYZ
  */
typedef enum 
{ 
	eEULER_XYZ = 0, 
	eEULER_XZY, 
	eEULER_YZX, 
	eEULER_YXZ, 
	eEULER_ZXY, 
	eEULER_ZYX,
	eSPHERIC_XYZ
} ERotationOrder;

/** \enum ETransformInheritType Transformation inheritance flags.
  * - \e eINHERIT_RrSs
  * - \e eINHERIT_RSrs
  * - \e eINHERIT_Rrs 
  */
typedef enum 
{
	eINHERIT_RrSs = 0, 
	eINHERIT_RSrs, 
	eINHERIT_Rrs 
} ETransformInheritType;

/** \enum EQuaternionInterpolation  Quaternion interpolation modes.
  */
typedef enum
{
    eQUATINTERP_OFF = 0,            /*!<    Do not evaluate using quaternion interpolation. */
    eQUATINTERP_CLASSIC = 1,        /*!<    Legacy quaternion interpolation mode. */
    eQUATINTERP_SLERP,			    /*!<	Spherical linear interpolation. */
    eQUATINTERP_CUBIC,	            /*!<	Cubic interpolation . */
    eQUATINTERP_TANGENTDEPENDENT,   /*!<    Mix between Slerp and cubic interpolation 
                                            depending on the specified tangents for each key. */
    eQUATINTERP_NBMODES
} EQuaternionInterpolation;


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMATH_KFBXTRANSFORMATION_H

