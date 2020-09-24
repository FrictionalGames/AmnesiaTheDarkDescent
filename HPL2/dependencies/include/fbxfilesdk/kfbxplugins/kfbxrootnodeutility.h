/*!  \file kfbxrootnodeutility.h
 */
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXROOTNODEUTILITY_H 
#define FBXFILESDK_KFBXPLUGINS_KFBXROOTNODEUTILITY_H

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

#include <fbxfilesdk/kfbxplugins/kfbxsystemunit.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxAxisSystem;
class KFbxScene;
class KFbxNode;

/** \brief This class collects static functions for manipulating Fbx_Root nodes. 
  * Fbx_Root nodes were used to orient and scale scenes from other graphics applications. They have been replaced by the 
  * conversion routines in KFbxAxisSystem and KFbxSystemUnit. These methods are provided for backward compatibility only 
  * and will eventually be removed. Use the conversion routines in KFbxSystemUnit and KFbxAxisSystem when possible.
  */
class KFBX_DLL KFbxRootNodeUtility 
{
public:
	
	static const char* sFbxRootNodePrefix;

	/** This method strips the scene of all Fbx_Root nodes.
	  * \param pScene     The scene to convert
	  * \return           \c true if successful, \c false otherwise.
	  * \remarks          Converts the children of any Fbx_Roots to the orientation
	  *                   and units that the Fbx_Root transformation represented.
	  *                   The scene should look unchanged.
	  */
	static bool RemoveAllFbxRoots( KFbxScene* pScene );

	/** Inserts an Fbx_Root node into the scene to orient the 
	  * scene from its axis and unit systems to the specified ones.
	  * \param pScene            The scene to convert
	  * \param pDstAxis          Destination axis.
	  * \param pDstUnit          Destination unit
	  * \param pUnitOptions      Unit conversion options
	  * 
	  */
	static bool InsertFbxRoot(  KFbxScene* pScene, 
								const KFbxAxisSystem& pDstAxis, 
								const KFbxSystemUnit& pDstUnit,
								const KFbxSystemUnit::KFbxUnitConversionOptions& pUnitOptions = KFbxSystemUnit::DefaultConversionOptions );

	/** Check if a node is an Fbx_Root node
	  * \param pNode     The node to query
	  * \return          \c true if pNode is a Fbx_Root node, false otherwise
	  */
	static bool IsFbxRootNode(KFbxNode* pNode);

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

protected:
	
	/* This class is not meant to be instantiated by anything.
	 */
	KFbxRootNodeUtility(); 
	KFbxRootNodeUtility(const KFbxRootNodeUtility& pOther); 
	~KFbxRootNodeUtility();


#ifndef DOXYGEN_SHOULD_SKIP_THIS


#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXROOTNODEUTILITY_H

