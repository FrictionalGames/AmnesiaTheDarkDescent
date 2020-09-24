/*!  \file kfbxplug.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXPLUG_H
#define FBXFILESDK_KFBXPLUGINS_KFBXPLUG_H

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

#include <fbxfilesdk/kfbxevents/kfbxemitter.h>
#include <fbxfilesdk/fbxfilesdk_new.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxPlug;
class KFbxSdkManager;
class KFbxClassIdInfo;
class KFbxObject;
class KFbxPropertyHandle;

//! The function pointer type for object constructor functions.
typedef KFbxPlug* (*kFbxPlugConstructor)(KFbxSdkManager& pManager, const char* pName, const KFbxPlug* pFrom, const char* pFBXType, const char* pFBXSubType);

/** Internal class used to differentiate objects during run-time. Essentially, each class has an unique ClassId,
  * that the system can request in order to test if the class match the description. This class implement
  * the necessary tools to be able to perform hierarchic class testing. This means that a class B that inherits
  * from the class A will answer yes to a "Is A" query of type A or B, but will answer no to a class C that can
  * still inherit from A. All class must inherit from KFbxPlug before they can have their own ClassId. Generally,
  * classes inherit from KFbxObject rather than KFbxPlug, since it offer much more functionality and support
  * throughout the FBX SDK API. When using the standard macros to create new types of objects in the FBX SDK,
  * a static ClassId will automatically be generated for that new class.
  *
  * When objects are exported to an FBX file, their class type is maintained using 3 sort of strings. They are
  * the Object Type string, the Object Sub Type string and the Object Type Prefix. There is no good or bad way
  * to choose the value of these identifiers, but it is preferable to use meaningful values to keep the
  * ASCII version of FBX readable and easy to understand.
  * \see KFbxPlug, KFbxObject
  */
class KFBX_DLL kFbxClassId
{
public:
	//! Constructor.
	kFbxClassId();

	/** Advanced constructor were we can specify the general parameters for this ClassId.
	  * \param pClassName		The name of the class represented.
	  * \param pParentClassId	The parent ClassId of this class.
	  * \param pConstructor		A function pointer to a construction method for this ClassId.
	  * \param pFBXType			The FBX file Object Type string associated to this class.
	  * \param pFBXSubType		The FBX file Object Sub Type string associated to this class.
	  */
	kFbxClassId(const char* pClassName, const kFbxClassId &pParentClassId, kFbxPlugConstructor pConstructor=0, const char* pFBXType=NULL, const char* pFBXSubType=NULL);

	//! Destructor.
	void Destroy();

	/** Retrieve the class name.
	  * \return The class identification string name.
	  */
	const char* GetName() const;

    /** Retrieve the parent ClassId.
	  * \return The parent ClassId.
	  */
	kFbxClassId GetParent() const;

    /** Create an instance of this class.
	  * \param pManager	The FBX SDK Manager to be used to instantiate this object. This allow the object to use the same memory manager as the provided manager.
	  * \param pName	The name to assign to this new object instance.
	  * \param pFrom	An object to clone if it matches the same ClassId. This is an optional parameter.
	  * \return			The newly created instance of this class.
	  */
	KFbxPlug* Create(KFbxSdkManager& pManager, const char* pName, const KFbxPlug* pFrom);

	/** Override the function pointer method to construct this object.
	  * \param pConstructor	A newly defined function pointer to a construction method to replace the existing one.
	  * \return				True if the operation was successful.
	  */
	bool Override(kFbxPlugConstructor pConstructor);

	/** Test if this class is a hierarchical children of the specified class type. This is the standard method to differentiate object classes.
	  * \param pId	The class type to test against self.
	  * \return		True if the object is a hierarchical children of the type specified.
	  * \remarks	This function will perform a complete search until it reaches the top level class, but it will stop as soon as one ClassId matches the test.
	  */
	bool Is(kFbxClassId const pId) const;

	/** Equivalence operator.
	  * \param pClassId	The class type to test against self.
	  * \return			True if the ClassId is exactly the same.
	  * \remarks		This function only perform direct equality test, and doesn't test hierarchic children.
	  */
	bool operator==(kFbxClassId const& pClassId) const;

	/** Inequivalence operator.
	  * \param pClassId	The class type to test against self.
	  * \return			True if the ClassId is not the same.
	  * \remarks		This function only perform direct inequality test, and doesn't test hierarchic children.
	  */
	bool operator!=(kFbxClassId const& pClassId) const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
	/** Retrieve the information associated to this ClassId.
	  * \return Various class information such as the constructor function pointer or the parent ClassId.
	  */
	inline KFbxClassIdInfo* GetClassIdInfo() { return mClassInfo; }

	/** Retrieve the information associated to this ClassId.
	  * \return Various class information such as the constructor function pointer or the parent ClassId.
	  */
    inline const KFbxClassIdInfo* GetClassIdInfo() const { return mClassInfo; }
#endif 
    
	/** Retrieve the FBX file Object Type string associated to this class.
	  * \param pAskParent	If True, retrieve the parent ClassId, but only if self ClassId is not valid.
	  * \return				The FBX file Object Type string associated to this class.
	  */
	const char* GetFbxFileTypeName(bool pAskParent=false) const;

	/** Retrieve the FBX file Object Sub Type string associated to this class.
	  * \param pAskParent	If True, retrieve the parent ClassId, but only if self ClassId is not valid.
	  * \return				The FBX file Object Sub Type string associated to this class.
	  */
	const char* GetFbxFileSubTypeName() const;

	/** Find out if self ClassId is valid or not.
	  * \return True if self ClassId is valid.
	  */
	inline bool IsValid() const { return mClassInfo ? true : false; }
    
	/** Set the Object Type Prefix string associated to this class. This will change the "ObjectTypePrefix::" found in
	  * front of object name in the FBX file. This is useful to differentiate objects by their name without using the
	  * Object Type or Sub Type strings in the file.
	  * \param pObjectTypePrefix The Object Type Prefix string.
	  */
	void SetObjectTypePrefix(const char* pObjectTypePrefix);

	/** Retrieve the Object Type Prefix string associated to this class.
	  * \return The Object Type Prefix string.
	  */
	const char* GetObjectTypePrefix();
   
	/** Retrieve the root property handle of this class. This is useful to access the default property hierarchy for
	  * this class. This allow users to retrieve information such as the default value for all properties of this class.
	  * \return The root property handle for this class.
	  */
	KFbxPropertyHandle* GetRootClassDefaultPropertyHandle();

	/** Increase the instance reference count for this class type.
	  * \return The new count of reference to this class after increment.
	  */
	int ClassInstanceIncRef();

    /** Decrease the instance reference count for this class type.
	  * \return The new count of reference to this class after decrement.
	  */
	int ClassInstanceDecRef();
	 
	/** Retrieve the instance reference count for this class type.
	  * \return The reference count of this class type.
	  */
	int GetInstanceRef();

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
	kFbxClassId(KFbxClassIdInfo* mClassInfo);

	bool SetFbxFileTypeName(const char* pName);
	bool SetFbxFileSubTypeName(const char* pName);

	KFbxClassIdInfo* mClassInfo;

	friend class KFbxSdkManager;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
};

//! Macro used to declare a new plug inherited class.
#define KFBXPLUG_DECLARE(Class)																	\
public:																							\
    KFBXNEW_DECLARE_FRIEND																		\
	static kFbxClassId ClassId;																	\
	static Class* Create(KFbxSdkManager *pManager, const char *pName);							\
	static Class* SdkManagerCreate(KFbxSdkManager *pManager, const char *pName, Class* pFrom)	\
	{																							\
		Class* lClass = FbxSdkNew< Class >(*pManager, pName);									\
		lClass->Construct(pFrom);																\
		return lClass;																			\
	}																							\
	virtual kFbxClassId	GetClassId() const { return ClassId; }									\
	friend class FBXFILESDK_NAMESPACE::KFbxSdkManager;											\

//! Macro used to declare a new abstract plug inherited class.
#define KFBXPLUG_DECLARE_ABSTRACT(Class)														\
public:																							\
	static kFbxClassId ClassId;																	\
	static Class* Create(KFbxSdkManager *pManager, const char *pName);							\
	static kFbxPlugConstructor SdkManagerCreate;												\
	virtual kFbxClassId	GetClassId() const { return ClassId; }									\
	friend class FBXFILESDK_NAMESPACE::KFbxSdkManager;											\

//! Macro used to implement a new plug inherited class.
#define KFBXPLUG_IMPLEMENT(Class)																\
	kFbxClassId	Class::ClassId;																	\
	Class* Class::Create(KFbxSdkManager* pManager, const char* pName)							\
	{																							\
		Class* ClassPtr;    																	\
		ClassPtr = 0;																			\
		return (Class*)pManager->CreateNewObjectFromClassId(Class::ClassId, pName);				\
	}																							\
	
//! Macro used to implement a new abstract plug inherited class.
#define KFBXPLUG_IMPLEMENT_ABSTRACT(Class)														\
	kFbxClassId	Class::ClassId;																	\
	kFbxPlugConstructor Class::SdkManagerCreate = 0;											\
	Class* Class::Create(KFbxSdkManager *pManager, const char *pName)							\
	{																							\
		Class* ClassPtr;    																	\
		ClassPtr = 0;																			\
		return (Class*)pManager->CreateNewObjectFromClassId(Class::ClassId, pName);				\
	}																							\
	
/** Top level class for any FBX object that requires ClassId functionality. For more information
  * about ClassId, please refer to the kFbxClassId documentation.
  * \see kFbxClassId, KFbxObject
  */
class KFBX_DLL KFbxPlug : public kfbxevents::KFbxEmitter
{
	KFBXPLUG_DECLARE(KFbxPlug);

public:
	/** Unregister and delete this object. This will also breaks all connections to this object as well
	  * as removing all the instance of all the properties of this object with the object's class.
	  * \param pRecursive	Unused parameter, deprecated.
	  * \param pDependents	Unused parameter, deprecated.
	  */
	virtual void Destroy(bool pRecursive=false, bool pDependents=false);

	/** Retrieve the FBX SDK Manager associated to this object. There is no implementation at the level
	  * of KFbxPlug. A basic implementation is available at the KFbxObject level.
	  * \return A pointer to the FBX SDK Manager of this object.
	  */
	virtual KFbxSdkManager*	GetFbxSdkManager() const { return 0; }

	/** Test if this class is a hierarchical children of the specified class type. This is the standard method to differentiate object classes.
	  * \param pClassId	The class type to test against self.
	  * \return			True if the object is a hierarchical children of the type specified.
	  * \remarks		This function will perform a complete search until it reaches the top level class, but it will stop as soon as one ClassId matches the test.
	  */
	virtual bool Is(kFbxClassId pClassId) const { return GetClassId().Is(pClassId); }

	/** Templated test if this class is a hierarchical children of the specified class type.
	  * \param pFBX_TYPE	A direct pointer to the C++ type of the object to test against.
	  * \return				True if the object is a hierarchical children of the type specified.
	  * \remarks			This function will perform a complete search until it reaches the top level class, but it will stop as soon as one ClassId matches the test.
	  */
	template < class T >inline bool	Is(T *pFBX_TYPE) const					{ return Is(T::ClassId); }

	/** Test if this class is a hierarchical children of the specified class type. This test will be performed on the run-time
	  * class registered with the FBX SDK Manager rather than the static ClassId generated at compile time.
	  * \param pClassId	The class type to test against self.
	  * \return			True if the object is a hierarchical children of the type specified.
	  * \remarks		This function will perform a complete search until it reaches the top level class, but it will stop as soon as one ClassId matches the test.
	  */
	virtual bool IsRuntime(kFbxClassId pClassId) const { return GetRuntimeClassId().Is(pClassId); }

	/** Set the run-time ClassId for this class. In most contexts, users do not have to change the run-time ClassId,
	  * they are automatically generated when registered a new class during run-time.
	  * \param pClassId	The ClassId to set as the run-time ClassId for this object.
	  * \return			Unused return value, deprecated.
	  * \remarks		This function is not implemented by KFbxPlug, hence it will always return false. KFbxObject implements
	  *					this function and always return true.
	  */
	virtual bool SetRuntimeClassId(kFbxClassId pClassId);

	/** Retrieve the run-time ClassId for this object.
	  * \return The run-time ClassId for this object.
	  */
	virtual kFbxClassId GetRuntimeClassId() const;

    /** Find out if the ClassId was registered during run-time rather than at compile time.
	  * \return True if the run-time ClassId is inequal to the ClassId.
	  */
    virtual bool IsRuntimePlug() const { return !( GetRuntimeClassId() == GetClassId() ); }

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
	inline static void SetWipeMode(bool pState) { KFbxPlug::sgWipeMode = pState; }
	inline static bool GetWipeMode() { return KFbxPlug::sgWipeMode; }

private:
    // used as global flag to modify the behavior of KFbxObject::Destruct() during a ForceKill() on the scene.
	// This is for internal use
	static bool sgWipeMode;

protected:
	inline KFbxPlug() {}
	inline KFbxPlug(KFbxSdkManager& pManager, const char* pName) {}
	virtual ~KFbxPlug() {}

	virtual void Construct(const KFbxPlug* pFrom);
	virtual void Destruct(bool pRecursive, bool pDependents);
	friend class KFbxProperty;
	friend class KFbxObject;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

/** Convert the class type parameter into a C class parameter for other function inputs.
  * Usage example:
  * \code
  * //Assuming MyCamera is a valid KFbxCamera object
  * bool AreCamerasObject = MyCamera->Is(FBX_TYPE(KFbxObject)); //Should return true :)
  * \endcode
  */
#define FBX_TYPE(class) ((class const*)0)

/** Retrieve the ClassId of the class type parameter.
  * Usage example:
  * \code
  * //Assuming MyLight is a valid KFbxLight object
  * bool AreLightsObject = MyLight->Is(FBX_CLASSID(KFbxObject)); //Should return true :)
  * 
  * //However, most people prefer this valid syntax...
  * bool AreLightsObject = MyLight->Is(KFbxObject::ClassId); //Should return true :)
  * \endcode
  */
#define FBX_CLASSID(class) (class::ClassId)

/** Safe casting of FBX SDK objects into other FBX SDK class types. This cast will perform
  * the complete test to make sure the object inherits from the requested class type. This is
  * the equivalent of a dynamic_cast but much faster.
  * \param pPlug	The object to try to cast into T type.
  * \return			A non-null pointer if the cast was successful.
  */
template < class T > inline T* KFbxCast(KFbxPlug *pPlug)
{
	return pPlug && pPlug->Is(FBX_CLASSID(T)) ? (T *)pPlug : 0;
}

/** Safe const casting of FBX SDK objects into other FBX SDK class types. This cast will perform
  * the complete test to make sure the object inherits from the requested class type. This is
  * the equivalent of a dynamic_cast but much faster.
  * \param pPlug	The object to try to cast into T type.
  * \return			A non-null pointer if the cast was successful.
  */
template < class T > inline T const* KFbxCast(KFbxPlug const*pPlug)
{
	return pPlug && pPlug->Is(FBX_CLASSID(T)) ? (T const*)pPlug : 0;
}

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXPLUG_H

