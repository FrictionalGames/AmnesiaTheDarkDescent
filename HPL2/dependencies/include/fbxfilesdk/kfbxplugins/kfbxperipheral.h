/*!  \file kfbxperipheral.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXPERIPHERAL_H
#define FBXFILESDK_KFBXPLUGINS_KFBXPERIPHERAL_H

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

#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/kfbxplugins/kfbxobject.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** KFbxPeripheral is an interface to load/unload content of KFbxObject from memory to
somewhere you defined, for example, to a temporary file on disk .
* \nosubgrouping
* You need to inherited your own peripheral class from this class and overload
* the functions to control what information of a KFbxObject you want to load/unload,
* and where you are going to load/unload these information to.
* For example, you can ask an object to dump itself on disk to free some memory and vice-versa 
* when you want to load/unload this object from your scene flexibly.
*/
class KFBX_DLL KFbxPeripheral 
{
public:
	/**
	  * \name Constructor and Destructor
	  */
	//@{

	//!Constructor.
	KFbxPeripheral();

    //!Destructor.
	virtual ~KFbxPeripheral();
	//@}

	/** Reset the peripheral to its initial state.
	  */
	virtual void Reset() = 0;

	/** Unload the content of pObject.
	  * \param pObject                 Object whose content is to be offloaded into 
	  * the peripheral storage area.
	  * \return                        \c true if the object content has been successfully transferred.
	  * \c false otherwise.
	  */
	virtual bool UnloadContentOf(KFbxObject* pObject) = 0;

	/** Load the content of pObject.
	  * \param pObject                 Object whose content is to be loaded from
	  * the peripheral storage area.
	  * \return                        \c true if the object content has been successfully transferred.
	  * \c false otherwise.
	  */
	virtual bool LoadContentOf(KFbxObject* pObject) = 0;

	/** Check if this peripheral can unload the given object content.
	  * \param pObject                 Object whose content has to be transferred.
	  * \return                        \c true if the peripheral can handle this object content and
	  * has enough space in its storage area.\c false otherwise.
	  */
	virtual bool CanUnloadContentOf(KFbxObject* pObject) = 0;

    /** Check if this peripheral can load the given object content.
    * \param pObject                  Object whose content has to be transferred.
    * \return                         \c true if the peripheral can handle this object content.
	* \c false otherwise.
    */
    virtual bool CanLoadContentOf(KFbxObject* pObject) = 0;

    /** Initialize the connections of an object
    * \param pObject                  Object on which the request for connection is done.
    */
    virtual void InitializeConnectionsOf(KFbxObject* pObject) = 0;

    /** Uninitialize the connections of an object
    * \param pObject                 Object on which the request for disconnection is done.
    */
    virtual void UninitializeConnectionsOf(KFbxObject* pObject) = 0;
};

// predefined offload peripherals
extern KFBX_DLL KFbxPeripheral* TMPFILE_PERIPHERAL;
extern KFBX_DLL KFbxPeripheral* NULL_PERIPHERAL;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXPERIPHERAL_H

