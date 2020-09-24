/*!  \file kfbxplugin.h
 */

#ifndef FBXFILESDK_KFBXMODULES_KFBXPLUGIN_H
#define FBXFILESDK_KFBXMODULES_KFBXPLUGIN_H

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

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/kintrusivelist.h>
#include <fbxfilesdk/components/kbaselib/object/klibrary.h>
#include <fbxfilesdk/components/kbaselib/klib/kscopedptr.h>

#include <fbxfilesdk/kfbxevents/kfbxemitter.h>
#include <fbxfilesdk/kfbxevents/kfbxlistener.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;
class KFbxLoadingStrategy;
class KFbxPluginContainer;

//! Plug-in declaration macro that must to be used when defining new KFbxPlugin objects.
#define KFBXPLUGIN_DECLARE(Plugin)\
KFBXNEW_DECLARE_FRIEND \
public:\
	static Plugin * Create(const KFbxPluginDefinition& pDefinition, kLibHandle pLibHandle);\
	void Destroy();

//! Plug-in implementation macro that must be used when implementing new KFbxPlugin objects.
#define KFBXPLUGIN_IMPLEMENT(Plugin)\
	Plugin* Plugin::Create(const KFbxPluginDefinition& pDefinition, kLibHandle pLibHandle){ return FbxSdkNew<Plugin>(pDefinition, pLibHandle); }\
	void Plugin::Destroy(){ FbxSdkDelete(this); }

/** Structure used by plug-ins for identification purposes.
  * \note To avoid confusions in the system, it is recommended to choose an appropriate unique identifier string name when
  * defining your plug-in, as well as incrementing the version string to a correct value whenever something changes in the
  * implementation of the plug-in. Both of these string are used when comparing plug-ins for searches, as well as
  * identification in FBX files.
  */
struct KFBX_DLL KFbxPluginDefinition
{
	//! Constructor
	KFbxPluginDefinition() :
		mName("Unknown Name"),
		mVersion("Unknown Version")
	{
	}

	KString mName;		//!< The identifier name string of the plug-in. If the name is already used by another plug-in, the plug-in will still register.
	KString mVersion;	//!< The version string of the plug-in.
};

/** Data used to communicate information between an application and the plug-in.
  */
struct KFBX_DLL KFbxPluginData
{
	//! Constructor
	KFbxPluginData() :
		mQueryEmitter(NULL),
		mSDKManager(NULL),
		mPluginContainer(NULL)
	{
	}

	//! Copy Constructor
	explicit KFbxPluginData(const KFbxPluginData& pOther) :
		mQueryEmitter(pOther.mQueryEmitter),
		mSDKManager(pOther.mSDKManager),
		mPluginContainer(pOther.mPluginContainer)
	{
	}

	kfbxevents::KFbxEmitter*	mQueryEmitter;		//!< The emitter on which the plug-in can listen to receive events.
	KFbxSdkManager*				mSDKManager;		//!< The FBX SDK Manager on which the plug-in was instanced.
	KFbxPluginContainer*		mPluginContainer;   //!< The container which will have the ownership of the plug-in.
};

/** The base class to inherit from when creating new plug-ins for the FBX SDK. Plug-ins for the FBX SDK are extremely flexible
  * allowing a wide-range of possibilities. For example, one can write his own plug-in to add new readers/writers to the current list
  * of supported I/O formats, or add new dynamic classes to instantiate custom objects that can later be stored in FBX files. We also use the same
  * interface for plug-ins written using the FBX Extension SDK, which allow additional callbacks for other various Autodesk products
  * enabling greater interoperability with multiple various SDKs.
  *
  * Here is typical implementation of an FBX SDK plug-in that doesn't do anything else than just registering itself:
  * \code
  * class MyPlugin : public KFbxPlugin
  * {
  *     KFBXPLUGIN_DECLARE(MyPlugin); //This macro is mandatory for any plug-in definition
  *
  * protected:
  *     explicit MyPlugin(const KFbxPluginDefinition& pDefinition, kLibHandle pLibHandle) : KFbxPlugin(pDefinition, pLibHandle)
  *     {
  *     }
  *
  *     //Abstract functions that *must* be implemented
  *     virtual bool SpecificInitialize()
  *     {
  *         //For example, here we could register as many new I/O readers/writers as we would like, or classes, etc.
  *         return true;
  *     }
  *
  *     virtual bool SpecificTerminate()
  *     {
  *         //Here we would have to unregister whatever we registered to the FBX SDK
  *         return true;
  *     }
  * };
  *
  * KFBXPLUGIN_IMPLEMENT(MyPlugin); //This macro is mandatory for any plug-in implementation
  *
  * //Standard C export needed for any new FBX SDK plug-in
  * extern "C"
  * {
  *     static MyPlugin* sMyPluginInstance = NULL; //The module is owner of the plug-in
  *
  *     //This function will be called when an application will request the plug-in
  * #ifdef KARCH_ENV_WIN
  *     __declspec(dllexport) void FBXPluginRegistration(KFbxPluginContainer& pContainer, kLibHandle pLibHandle)
  * #else
  *     void FBXPluginRegistration(KFbxPluginContainer& pContainer, kLibHandle pLibHandle)
  * #endif
  *     {
  *         if( sPlugin == NULL )
  *         {
  *             //Create the plug-in definition which contains the information about the plug-in
  *             KFbxPluginDefinition sPluginDef;
  *             sPluginDef.mName = "My Plugin";
  *             sPluginDef.mVersion = "1.0";
  *
  *             //Create an instance of the plug-in
  *             sMyPluginInstance = MyPlugin::Create(sPluginDef, pLibHandle);
  *
  *             //Register the plug-in with the FBX SDK
  *             pContainer.Register(*sPlugin);
  *         }
  *     }
  * }
  * \endcode
  * \see KFbxPluginDefinition, KFbxPluginData
  */
class KFBX_DLL KFbxPlugin : public kfbxevents::KFbxListener
{
	KFBX_LISTNODE(KFbxPlugin, 1);

public:
	/** Accessor to the plug-in definition structure that contains basic information on the plug-in like its name or version. This is
	  * the only method available to differentiate plug-ins.
	  * \return The definition structure for this plug-in.
	  */
	const KFbxPluginDefinition& GetDefinition() const;

	/** Retrieve the library address pointer for this plug-in. With this module instance handle, for example someone can query procedures addresses,
	  * allowing more complex interactions, as well as other operating system module specific functions.
	  */
	kLibHandle GetLibraryHandle();

	/** Abstract function called once at the end of the plug-in construction. At that moment, plug-in data have been properly initialized.
	  * This function must be implemented by anyone who writes a new plug-in for the FBX SDK.
	  */
	virtual bool SpecificInitialize() = 0;

	/** Abstract function called once at the beginning of the plug-in destruction. At that moment, plug-in data is fully available.
	  * This function must be implemented by anyone who writes a new plug-in for the FBX SDK.
	  */
	virtual bool SpecificTerminate() = 0;

	/** Virtual function called once when the FBX SDK is about to write an FBX file. Users can re-implement it in their plug-in if they need
	  * to perform tasks at that moment. The scene provided in parameter can be altered. If not re-implemented, this function does nothing.
	  * \param pScene The scene that is about to be written in the FBX file.
	  */
	virtual void WriteBegin(KFbxScene& pScene);

	/** Virtual function called once when the FBX SDK is about to write plug-in's parameters. Users can re-implement it in their plug-in if they need
	  * to store properties in the FBX file for their own usage. The object in parameter is used to store those properties.
	  * If not re-implemented, this function does nothing.
	  * \param pParams An abstract object that can be used as a property container, to allow the plug-in to store properties about the plug-in.
	  */
	virtual void WriteParameters(KFbxObject& pParams);

	/** Virtual function called once after the FBX SDK wrote an FBX file. Users can re-implement it in their plug-in if they need
	  * to perform tasks at that moment. The scene provided in parameter can be altered, but the changes will not appear in the FBX file.
	  * If not re-implemented, this function does nothing.
	  * \param pScene The scene that was written in the FBX file.
	  */
	virtual void WriteEnd(KFbxScene& pScene);

	/** Virtual function called once when the FBX SDK is about to read an FBX file. Users can re-implement it in their plug-in if they need
	  * to perform tasks at that moment. The scene provided in parameter can be altered. If not re-implemented, this function does nothing.
	  * \param pScene The scene that is about to be read in the FBX file.
	  */
	virtual void ReadBegin(KFbxScene& pScene);

	/** Virtual function called once after the FBX SDK reads the plug-in's parameters. Users can re-implement it in their plug-in if they need
	  * to retrieve properties for their own usage. The object in parameter is used to retrieve those properties.
	  * If not re-implemented, this function does nothing.
	  * \param pParams An abstract object that can be used as a property container, to allow the plug-in to read properties about the plug-in.
	  */
	virtual void ReadParameters(KFbxObject& pParams);

	/** Virtual function called once after the FBX SDK read an FBX file. Users can re-implement it in their plug-in if they need
	  * to perform tasks at that moment. The scene provided in parameter can be altered. If not re-implemented, this function does nothing.
	  * \param pScene The scene that was read in the FBX file.
	  */
	virtual void ReadEnd(KFbxScene& pScene);

protected:
	/** Use the Create() and Destroy() methods declared and implemented in the KFBXPLUGIN_DECLARE and KFBXPLUGIN_IMPLEMENT macros to construct and destroy KFbxPlugin objects.
	  * \param pDefinition The definition associated with this plug-in. Each plug-in must have its own definition to differentiate it with other plug-ins.
	  * \param pLibHandle A pointer to the plug-in library address.
	  */
	explicit KFbxPlugin(const KFbxPluginDefinition& pDefinition, kLibHandle pLibHandle);

	//! Destructor
	virtual ~KFbxPlugin();

	/** Accessor to the plug-in private data.
	  * \return The data for the current plug-in.
	  */
	KFbxPluginData& GetData();

	/** Const accessor to the plug-in private data.
	  * \return The const data for the current plug-in.
	  */
	const KFbxPluginData& GetData() const;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
public:
	inline KFbxObject& GetPluginSettings() { return *(mPluginSettings.Get()); }
	inline const KFbxObject& GetPluginSettings() const { return *(mPluginSettings.Get()); }
	template <typename EventType, typename ListernerType> inline KFbxEventHandler* Bind(void (ListernerType::*pFunc)(const EventType*))
	{
		return KFbxListener::Bind<EventType,ListernerType>(*(GetData().mQueryEmitter), pFunc );
	}
	virtual void Destroy() = 0;

private:
	friend class					KFbxLoadingStrategy;
	bool							Initialize(const KFbxPluginData& pData);
	bool							Terminate();

	bool							mInitialized;
	KFbxPluginData					mData;
	KFbxPluginDefinition			mDefinition;
	kLibHandle						mLibraryHandle;
	KFBXObjectScopedPtr<KFbxObject>	mPluginSettings;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMODULES_KFBXPLUGIN_H
