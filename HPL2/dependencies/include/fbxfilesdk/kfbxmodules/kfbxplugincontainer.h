/*!  \file kfbxplugincontainer.h
 */

#ifndef FBXFILESDK_KFBXMODULES_KFBXPLUGINCONTAINER_H
#define FBXFILESDK_KFBXMODULES_KFBXPLUGINCONTAINER_H

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

#include <fbxfilesdk/kfbxmodules/kfbxplugin.h>
#include <fbxfilesdk/kfbxevents/kfbxemitter.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** Manages registration and ownership of FBX SDK plug-ins (FbxPlugin). 
  * The FBX SDK will provide a pointer to KFbxPluginContainer as an argument
  * to the FBXPluginRegistration() function exported from a plug-in's DLL.
  * A plug-in must register itself explicitly with the KFbxPluginContainer
  * by calling KFbxPluginContainer::Register() after it is constructed. 
  * For an example of this process see the code example in the KFbxPlugin 
  * class documentation.
  * \see KFbxPlugin
  */
class KFBX_DLL KFbxPluginContainer : public kfbxevents::KFbxEmitter
{
public:
	/** The registration function that must be called when the module containing the plug-in is loaded.
	  * \param pPlugin The plug-in to register.
	  */
	void Register(KFbxPlugin& pPlugin);

	/** The unregistration function that must be called when the module containing the plug-in is unloaded.
	  * \param pPlugin The plug-in to unregister.
	  */
	void Unregister(KFbxPlugin& pPlugin);

	//! Definition of a plug-in list.
	typedef KIntrusiveList<KFbxPlugin> PluginList;

	/** Const accessor to the list of plug-ins owned by the container.
	  * \return A list of plug-in registered to this container.
	  */
	const PluginList& GetPlugins() const;

	/** Accessor to the list of plug-ins owned by the container.
	  * \return A list of plug-in registered to this container.
	  */
	PluginList& GetPlugins();

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//    Anything beyond these lines may not be documented accurately and is 
//     subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
	virtual ~KFbxPluginContainer();
	PluginList mPlugins;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMODULES_KFBXPLUGINCONTAINER_H
