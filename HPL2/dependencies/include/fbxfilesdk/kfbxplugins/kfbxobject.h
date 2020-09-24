/*!  \file kfbxobject.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXOBJECT_H
#define FBXFILESDK_KFBXPLUGINS_KFBXOBJECT_H

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

#include <fbxfilesdk/kfbxplugins/kfbxplug.h>
#include <fbxfilesdk/kfbxplugins/kfbxproperty.h>

#include <fbxfilesdk/components/kbaselib/klib/kstringlist.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxDocument;
class KFbxImplementation;
class KFbxImplementationFilter;
class KFbxLibrary;
class KFbxMessage;
class KFbxPeripheral;
class KFbxStream;
class UserDataRecord;

enum eFbxCompare {
    eFbxCompareProperties
};

//! Macro used to declare a new class derived from KFbxObject.
#define KFBXOBJECT_DECLARE(Class,Parent)																		\
	KFBXPLUG_DECLARE(Class)		/* this also set public */														\
    typedef Parent ParentClass;																					\
    static Class* Create(KFbxObject* pContainer,  char const* pName);											\
	virtual ~Class(){};																							\
private:																										\
	Class(const Class&);																						\
	Class& operator=(const Class&);																				\

//! Macro used to declare a new abstract class derived from KFbxObject.
#define KFBXOBJECT_DECLARE_ABSTRACT(Class,Parent)																\
	KFBXPLUG_DECLARE_ABSTRACT(Class)	/* this also set public */												\
	typedef Parent ParentClass;																					\
private:																										\
	Class(const Class&);																						\
	Class& operator=(const Class&);																				\

//! Macro used to implement a new class derived from KFbxObject.
#define KFBXOBJECT_IMPLEMENT(Class)																				\
    KFBXPLUG_IMPLEMENT(Class)																					\
	Class* Class::Create(KFbxObject* pContainer, char const* pName)												\
	{																											\
		if( !pContainer ) return NULL;																			\
		Class* ClassPtr = Class::Create(pContainer->GetFbxSdkManager(), pName);									\
		pContainer->ConnectSrcObject(ClassPtr);																	\
		return ClassPtr;																						\
	}																											\

//! Macro used to implement a new abstract class derived from KFbxObject.
#define KFBXOBJECT_IMPLEMENT_ABSTRACT(Class)																	\
    KFBXPLUG_IMPLEMENT_ABSTRACT(Class)																			\

typedef size_t KFbxObjectID;
typedef int kFbxUpdateId;

class _KFbxObjectData;

/** The base class of most FBX objects. Provides the benefits of connectivity, identity, run-time typing,
  * properties, naming, copying, cloning, selection, and automated file IO. Most of
  * the FBX SDK API deals with KFbxObject pointers when it comes to manipulate objects in its simplest form.
  *
  * The KFbxObject class inherits from KFbxPlug, which allows objects to be differentiated via the ClassId mechanism. 
  * The ClassID mechanism replaces the dynamic_cast mechanism for efficient run-time type information.
  *
  * The KFbxObject provides methods for managing the connections between objects.  
  * Using connections, objects can be related to each other to form hierarchies or structures. All of
  * the FBX scene's object relations are expressed as connections between objects. Those connections can
  * be altered as needed to reflect most kind of setups encountered in this world. For example,
  * connections can be used to express parenting between transform nodes. Connections are not strict in
  * the sense that we allow any type of objects to connect to any other type of objects. The meaning of
  * the connection is purely semantic. As of yet, we do not provide the functionality to validate if
  * the connections made by the users are allowed or not.
  *
  * KFbxObject provide a property (KFbxProperty) mechanism to describe characteristics of 
  * objects in a scene. Properties may be either static or dynamic. Static properties are defined in the class direction 
  * and can be accessed directly by their name on the object exposing them without
  * the need for a search in the property list of the object. Dynamic properties can be added during run-time,
  * while the program is running. Objects can have an unlimited amount of properties. 
  * Properties can be listed at run-time, allowing for a flexible support of custom data
  * on objects, since they might be considered by the FBX file readers/writers depending on the flags set.
  *
  * Here is an example of a new empty minimal class template for FBX objects:
  * \code
  * //Declaration
  * class MyClass : public KFbxObject
  * {
  *     KFBXOBJECT_DECLARE(MyClass, KFbxObject);	//Be careful! The second parameter to this macro must be the parent class name!
  *
  * public:
  *     //Declare methods and properties here...
  *
  * private:
  *     //A default constructor must be declared as either protected or private since objects must be created using the static function MyClass::Create.
  *     MyClass(KFbxSdkManager& pManager, char const* pName);
  * };
  * \endcode
  * \code
  * //Implementation
  * KFBXOBJECT_IMPLEMENT(MyClass);
  * 
  * MyClass::MyClass(KFbxSdkManager& pManager, char const* pName) : KFbxObject(pManager, pName)
  * {
  * }
  * \endcode
  * Before the new class can be used, it needs to be registered to the manager with the following method:
  * \code
  * FbxSdkManager->RegisterFbxClass("MyClassName", FBX_TYPE(MyClass), FBX_TYPE(KFbxObject));	//Be careful! The 3rd parameter must be the parent class! If the parent class change, it must be updated here too!
  * \endcode
  * Then to create or delete instances of your new class, the following methods must be used:
  * \code
  * //Creating a new instance
  * MyClass* MyObject = MyClass::Create(FbxSdkManager, "Object Name");
  *
  * //Deleting this instance
  * MyObject->Destroy();
  * MyObject = NULL;
  * \endcode
  * \see KFbxPlug, KFbxProperty
  */
class KFBX_DLL KFbxObject : public KFbxPlug
{
	KFBXOBJECT_DECLARE(KFbxObject, KFbxPlug);

public:
	/**
	  * \name Cloning and references
	  */
	//@{
		/** Types of clones that can be created for KFbxObject.
		*/
		typedef enum
		{
			eDEEP_CLONE,		//!< A deep copy of the object. Changes to either the original or clone do not propagate to each other.
			eREFERENCE_CLONE	//!< Changes to original object propagate to clone. Changes to clone do not propagate to original.
		} ECloneType;

		/** Creates a clone of this object.
		  * \param pCloneType	The type of clone to be created. By default, the clone type is eDEEP_CLONE.
		  * \param pContainer	An optional parameter to specify which object will "contain" the new object. By contain, we mean
		  *						the new object will become a source to the container, connection-wise.
		  * \return The new clone, or NULL (if the specified clone type is not supported).
		  * \remarks			When doing either a "deep" or "reference" clone type, the clone will always get its properties values
		  *						set from the source object properties values.
		  */
		virtual KFbxObject* Clone(KFbxObject::ECloneType pCloneType=eDEEP_CLONE, KFbxObject* pContainer=NULL) const;

		/** Checks if this object is a reference clone of another object.
		  * \return \c True if this object is a clone of another object, \c false otherwise
		  */
		bool IsAReferenceTo() const;

		/** If this object is a reference clone, returns the original object (from which the clone originates).
		  * \return The original object, or NULL (if this object is not a reference clone).
		  */
		KFbxObject* GetReferenceTo() const;

		/** Checks if any objects are reference cloned from this object.
		  * \return \c True if there are objects reference cloned from this object, \c false otherwise.
		  */
		bool IsReferencedBy() const;

		/** Returns the number of objects that are reference clones of this object.
		  * \return The number of objects that are reference clones of this object.
		  */
		int GetReferencedByCount() const;

		/** Returns a reference clone of this object at the specified index.
		  * \param pIndex The specified index, valid values are [0, GetReferencedByCount())
		  * \return The reference clone, or NULL (if pIndex is out of range).
		  */
		KFbxObject* GetReferencedBy(int pIndex) const;
	//@}

	/**
	  * \name Object Name Management
	  */
	//@{
		/** Sets the name of this object.
		  * \param pName The object name as a \c NULL terminated string.
		  */
		void SetName(char const* pName);

		/** Returns the full name of this object.
		  * \return The full name as a \c NULL terminated string.
		  */
		char const* GetName() const;

		/** Returns the name of the object without the namespace qualifier.
		  * \return The object name without the namespace qualifier.
		  */
		KString GetNameWithoutNameSpacePrefix() const;

		/** Returns the name of the object with the namespace qualifier.
		  * \return The object name with the namespace qualifier.
		  */
		KString GetNameWithNameSpacePrefix() const;

		/** Sets the initial name of the object.
		  * \param pName The object's initial name as a \c NULL terminated string.
		  */
		void SetInitialName(char const* pName);

		/** Returns the initial name of the object.
		  * \return The object's initial name as a \c NULL terminated string.
		  */
		char const* GetInitialName() const;

		/** Returns the namespace of the object.
		  * \return The object's namespace as a \c NULL terminated string.
		  */
		KString GetNameSpaceOnly( );

		/** Sets the namespace of the object.
		  * \param pNameSpace The object's namespace as a \c NULL terminated string.
		  */
		void SetNameSpace(KString pNameSpace);

		/** Returns an array of all the namespaces for this object
		  * \param identifier The identifier of the namespaces.
		  * \return The array of all namespaces.
		  */
		KArrayTemplate<KString*> GetNameSpaceArray( char identifier );

		/** Returns only the name (no namespace or prefix) of the object.
		  * \return The name only as a \c NULL terminated string.
		  */
		KString GetNameOnly() const;

		/** Returns the namespace qualifier.
		  * \return The namespace qualifier.
		  */
		KString GetNameSpacePrefix() const;

		/** Removes the prefix of pName
		  * \param pName Whose prefix is removed.
		  * \return A temporary string without prefix.
		  */
		static KString RemovePrefix(char* pName);

		/** Strips the prefix of pName
		  * \param lName Whose prefix is stripped.
		  * \return lName stripped of its prefix.
		  */
		static KString StripPrefix(KString& lName);

		/** Strips the prefix of pName
		  * \param pName Whose prefix is stripped.
		  * \return A temporary string stripped of its prefix.
		  */
		static KString StripPrefix(const char* pName);

		//!Returns the unique ID of this object.
		KFbxObjectID const& GetUniqueID() const;
	//@}

	/**
	  * \name UpdateId Management
	  */
	//@{
		/** \enum eFbxUpdateIdType Update ID type.
		  * - \e eUpdateId_Object 
		  * - \e eUpdateId_Dependency
		  */
		typedef enum {
			eUpdateId_Object,
			eUpdateId_Dependency
		} eFbxUpdateIdType;

		/** Returns the update ID of this object. 
		  * \param pUpdateId The update ID type.
		  * \return The update ID.
		  */
		virtual kFbxUpdateId GetUpdateId(eFbxUpdateIdType pUpdateId=eUpdateId_Object) const;
	//@}

	/**
	  * \name Off-loading Management
	  * \remarks You can modify the unloaded state flag using the SetObjectFlags()
	  *         method. The ContentIsUnloaded() method below (implemented in this class)
	  *         is simply a synonym of GetObjectFlags(eCONTENT_UNLOADED_FLAG)
	  */
	//@{
		/** Unloads this object's content using the offload peripheral that is currently set in the document
		  * then flushes it from memory.
		  * \return 2 if the object's content is already unloaded or 1 if
		  *         this object's content has been successfully unloaded to the current
		  *         peripheral.
		  *
		  * \remarks If the content is locked more than once, or the peripheral cannot handle
		  *         this object's unloading, or if an error occurs, this method returns 0 and does not flush the content.         
		  */
		int ContentUnload();

		/** Loads this object's content using the offload peripheral that is currently set in the document.
		  * \return 1 if this object's content has been successfully loaded from the current
		  *         peripheral, 2 if the content is already loaded, and 0 if an error occurs or
		  *         the object's content is locked.
		  * \remarks On a successful Load attempt, the object content is locked.
		  */
		int ContentLoad();

		/** Judges if this object's content is loaded.
		  * \return \c True if this object's content is loaded, \c false otherwise.
		  * \remarks An object that has not been filled yet must be considered
		  * unloaded.
		  */
		bool ContentIsLoaded() const;

		/** Decreases the content lock count of an object. If the content lock count of an object
		  * is greater than 0, the content of the object is considered locked.
		  */
		void ContentDecrementLockCount();

		/** Increases the content lock count of an object. If the content lock count of an object
		  * is greater than 0, the content of the object is considered locked.
		  */
		void ContentIncrementLockCount();

		/** Judges if this object's content is locked. The content is considered locked if the content lock count
		  * is greater than 0
		  * \return \c True if this object's content is locked, \c false otherwise.
		  * \remarks A locked state prevents the object content from being unloaded from memory but
		  * does not block the loading.
		  */
		bool ContentIsLocked() const;
	//@}

	/**
	  * \name Off-loading Serialization section.
	  * The methods in this section are usually called by
	  * a peripheral.
	  */
	//@{
		/** Writes the content of the object to the given stream.
		  * \param pStream The destination stream.
		  * \return \c True if the content is successfully processed
		  * by the receiving stream, \c false otherwise.
		  */
		virtual bool ContentWriteTo(KFbxStream& pStream) const;

		/** Reads the content of the object from the given stream.
		  * \param pStream The source stream.
		  * \return \c True if the object fills itself with the received data
		  * from the stream successfully, \c false otherwise.
		  */
		virtual bool ContentReadFrom(const KFbxStream& pStream);
	//@}

	/**
	  * \name Selection management
	  */
	//@{
		/** Returns if this object is currently in a selected state.
		  * \return \c True if this object is selected, \c false otherwise.
		  */
		virtual bool GetSelected();

		/** Sets whether this object is currently selected.
		  * \param pSelected The selection flag.
		  */
		virtual void SetSelected(bool pSelected);
	//@}

	/**
	  * \name Properties access
	  */
	//@{
		/** Returns the first property of this object.
		  * \return The first property of this object.
		  */
		inline KFbxProperty GetFirstProperty() const
		{
			return RootProperty.GetFirstDescendent();
		}

		/** Returns the next property of this object that follows the specified property.
		  * \param pProperty The specified property.
		  * \return The next property of this object that follows pProperty.
		  */
		inline KFbxProperty GetNextProperty(KFbxProperty const &pProperty) const
		{
			return RootProperty.GetNextDescendent(pProperty);
		}

		/** Searches a property by name.
		  * \param pName The property name.
		  * \param pCaseSensitive Whether the name is case-sensitive.
		  * \return A valid KFbxProperty if found, else an invalid KFbxProperty. See KFbxProperty::IsValid()
		  */
		inline KFbxProperty FindProperty(const char* pName, bool pCaseSensitive = true)const
		{
			return RootProperty.Find(pName, pCaseSensitive );
		}

		/** Searches a property by name and data type.
		  * \param pName The property name.
		  * \param pDataType The data type of the property.
		  * \param pCaseSensitive Whether the name is case-sensitive.
		  * \return A valid KFbxProperty if the property is found, else an invalid KFbxProperty. See KFbxProperty::IsValid()
		  */
		inline KFbxProperty FindProperty(const char* pName, KFbxDataType const &pDataType, bool pCaseSensitive = true) const
		{
			return RootProperty.Find(pName, pDataType, pCaseSensitive );
		}

		/** Searches a property by full name.
		  * \param pName The full name of the property as a \c NULL terminated string.
		  * \param pCaseSensitive whether or not the name is case-sensitive.
		  * \return A valid KFbxProperty if the property is found, else
		  *         an invalid KFbxProperty. See KFbxProperty::IsValid()
		  */
		inline KFbxProperty FindPropertyHierarchical(const char* pName, bool pCaseSensitive = true)const
		{
			return RootProperty.FindHierarchical(pName, pCaseSensitive );
		}

		/** Searches a property by full name and data type.
		  * \param pName The full name of the property as a \c NULL terminated string.
		  * \param pDataType The data type of the property.
		  * \param pCaseSensitive whether or not the name is case-sensitive.
		  * \return A valid KFbxProperty if the property is found, else
		  *         an invalid KFbxProperty. See KFbxProperty::IsValid()
		  */
		inline KFbxProperty FindPropertyHierarchical(const char* pName, KFbxDataType const &pDataType, bool pCaseSensitive = true) const
		{
			return RootProperty.FindHierarchical(pName, pDataType, pCaseSensitive );
		}

		/** Returns the root property of this object.
		  * \return The root property.
		  */
		inline KFbxProperty& GetRootProperty() { return RootProperty; }

		/** Returns the root property of this object.
		  * \return The root property.
		  */
		inline const KFbxProperty& GetRootProperty()const{ return RootProperty; }

		/** Returns the class root property.
		  * \return The class root property if it exists, else an invalid KFbxProperty. See KFbxProperty::IsValid().
		  * \remarks Class KFbxObject and its sub-classes all have a class root property. This class root property contains basic information about the class type, such as the class name.  
		  */
		KFbxProperty GetClassRootProperty();
	//@}

	/**
	  * \name General Object Connection and Relationship Management
	  */
	//@{
		/** Connects this object to a source object.
		  * \param pObject The source object to which this object connects.
		  * \param pType The connection type between this object and the source object.
		  * \return \c True on success, \c false otherwise.
		  */
		inline bool ConnectSrcObject(KFbxObject* pObject,kFbxConnectionType pType=eFbxConnectionNone){ return RootProperty.ConnectSrcObject(pObject,pType); }

		/** Judges whether this object connects with the source object.
		  * \param pObject The source object.
		  * \return \c True if this object connects with the source object, \c false otherwise.
		  */
		inline bool IsConnectedSrcObject(const KFbxObject* pObject) const { return RootProperty.IsConnectedSrcObject(pObject); }

		/** Disconnects this object from a source object.
		  * \param pObject The source object from which this object will be disconnected.
		  * \return \c True on success, \c false otherwise.
		  */
		inline bool DisconnectSrcObject(KFbxObject* pObject){ return RootProperty.DisconnectSrcObject(pObject); }

		/** Disconnects this object from all source objects.
		  * \return \c True if it disconnects all source objects successfully, \c false otherwise.
		  */
		inline bool DisconnectAllSrcObject() { return RootProperty.DisconnectAllSrcObject(); }

		/** Disconnects this object from all source objects that satisfy a given criteria. 
		  * \param pCriteria The given criteria.
		  * \return \c True if it disconnects all the source objects successfully, \c false otherwise.
		  */
		inline bool DisconnectAllSrcObject(KFbxCriteria const &pCriteria) { return RootProperty.DisconnectAllSrcObject(pCriteria); }

		/** Disconnects this object from all source objects of a specific class type.
		  * \param pClassId The specific class type.
		  * \return \c True if it disconnects all source objects successfully, \c false otherwise.
		  */
		inline bool DisconnectAllSrcObject(kFbxClassId pClassId) { return RootProperty.DisconnectAllSrcObject(pClassId); }

		/** Disconnects this object from all source objects that satisfy a given criteria and that are a specific class type.
		  * \param pClassId The specific class type.
		  * \param pCriteria The given criteria.
		  * \return \c True if it disconnects from all source objects successfully, \c false otherwise.
		  */
		inline bool DisconnectAllSrcObject(kFbxClassId pClassId,KFbxCriteria const &pCriteria) { return RootProperty.DisconnectAllSrcObject(pClassId,pCriteria); }

		/** Returns the number of source objects with which this object connects.
		  * \return The number of source objects with which this object connects. 
		  */
		inline int GetSrcObjectCount() const { return RootProperty.GetSrcObjectCount(); }

		/** Returns the number of source objects that satisfy the given criteria with which this object connects.
		  * \param pCriteria The given criteria.
		  * \return The number of source objects that satisfy the given criteria with which this object connects.
		  */
		inline int GetSrcObjectCount(KFbxCriteria const &pCriteria) const { return RootProperty.GetSrcObjectCount(pCriteria); }

		/** Returns the number of source objects of the specific class type with which this object connects. 
		  * \param pClassId The specific class type.
		  * \return The number of source objects of the specific class type with which this object connects.
		  */
		inline int GetSrcObjectCount(kFbxClassId pClassId) const { return RootProperty.GetSrcObjectCount(pClassId); }

		/** Returns the number of source objects that satisfy the given criteria and that are the specified class type.
		  * \param pClassId The specified class type.
		  * \param pCriteria The given criteria.
		  * \return The number of source objects.
		  */
		inline int GetSrcObjectCount(kFbxClassId pClassId,KFbxCriteria const &pCriteria) const { return RootProperty.GetSrcObjectCount(pClassId,pCriteria); }

		/** Returns the source object with which this object connects at the specified index.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The source object at the specified index, NULL if not found.
		  */
		inline KFbxObject* GetSrcObject(int pIndex=0) const { return RootProperty.GetSrcObject(pIndex); }

		/** Returns the source object that satisfies the criteria at the specified index with which this object connects.
		  * \param pCriteria The given criteria.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The source object that satisfies the given criteria at the specified index, NULL if not found.
		  */
		inline KFbxObject* GetSrcObject(KFbxCriteria const &pCriteria,int pIndex=0) const { return RootProperty.GetSrcObject(pCriteria,pIndex); }

		/** Returns the source object of the specified class type at the specified index with which this object connects.
		  * \param pClassId The specified class type.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The source object of the specified class type at the specified index, NULL if not found.
		  */
		inline KFbxObject* GetSrcObject(kFbxClassId pClassId,int pIndex=0) const { return RootProperty.GetSrcObject(pClassId,pIndex); }

		/** Returns the source object of the specified class type that satisfies the given criteria at the specified index.
		  * \param pClassId The specified class.
		  * \param pCriteria The given criteria.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The source object of the specified class type that satisfies the given criteria at the specified index, NULL if not found.
		  */
		inline KFbxObject* GetSrcObject(kFbxClassId pClassId,KFbxCriteria const &pCriteria,int pIndex=0) const { return RootProperty.GetSrcObject(pClassId,pCriteria,pIndex); }

		/** Searches the source object with the specified name, starting at the specified index.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The source object with the name, NULL if not found.
		  */
		inline KFbxObject* FindSrcObject(const char *pName,int pStartIndex=0) const { return RootProperty.FindSrcObject(pName,pStartIndex); }

		/** Searches the source object with the specified name which satisfies the given criteria, starting at the specified index.
		  * \param pCriteria The given criteria.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The source object with the name, NULL if not found.
		  */
		inline KFbxObject* FindSrcObject(KFbxCriteria const &pCriteria,const char *pName,int pStartIndex=0) const { return RootProperty.FindSrcObject(pCriteria,pName,pStartIndex); }

		/** Searches the source object with the specified name which is also the specified class type, starting at the specified index.
		  * \param pClassId The specified class type.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The source object with the name, NULL if not found.
		  */
		inline KFbxObject* FindSrcObject(kFbxClassId pClassId,const char *pName,int pStartIndex=0) const { return RootProperty.FindSrcObject(pClassId,pName,pStartIndex); }

		/** Searches the source object with the specified name which is the specified class type and satisfies the given criteria, starting at the specified index.
		  * \param pClassId The specified class type.
		  * \param pCriteria The given criteria.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The source object with the name, NULL if not found.
		  */
		inline KFbxObject* FindSrcObject(kFbxClassId pClassId,KFbxCriteria const &pCriteria,const char *pName,int pStartIndex=0) const { return RootProperty.FindSrcObject(pClassId,pCriteria,pName,pStartIndex); }

		/** Disconnects this object from all source objects of the specified class type.
		  * \param pFBX_TYPE The specified class type.
		  * \return \c True if it disconnects all source objects successfully, \c false otherwise.
		  */
		template <class T> inline bool DisconnectAllSrcObject(T const *pFBX_TYPE) { return RootProperty.DisconnectAllSrcObject(pFBX_TYPE); }

		/** Disconnects this object from all source objects that are of the specified class type and that satisfy the given criteria.
		  * \param pFBX_TYPE The specified class type.
		  * \param pCriteria The given criteria.
		  * \return \c True if it disconnects all source objects successfully, \c false otherwise.
		  */
		template <class T> inline bool DisconnectAllSrcObject(T const *pFBX_TYPE,KFbxCriteria const &pCriteria) { return RootProperty.DisconnectAllSrcObject(pFBX_TYPE,pCriteria); }

		/** Returns the number of source objects of a specific class type with which this object connects. 
		  * \param pFBX_TYPE The specified class type.
		  * \return The number of source objects of the specified class type with which this object connects. 
		  */
		template <class T> inline int GetSrcObjectCount(T const *pFBX_TYPE) const { return RootProperty.GetSrcObjectCount(pFBX_TYPE); }

		/** Returns the number of source objects with which this object connects that are the specified class type and that satisfy the given criteria. 
		* \param pFBX_TYPE The specified class type.
		* \param pCriteria The given criteria.
		* \return The number of source objects that are the specified class type and that satisfy the given criteria.
		*/
		template <class T> inline int GetSrcObjectCount(T const *pFBX_TYPE,KFbxCriteria const &pCriteria) const { return RootProperty.GetSrcObjectCount(pFBX_TYPE,pCriteria); }

		/** Returns the source object of the specified class type at the specified index.
		  * \param pFBX_TYPE The specified class type.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The source object of a specified class type at the specified index, NULL if not found.
		  */
		template <class T> inline T* GetSrcObject(T const *pFBX_TYPE,int pIndex=0) const { return RootProperty.GetSrcObject(pFBX_TYPE,pIndex); }

		/** Returns the source object that is the specified class type and that satisfies the given criteria at the specified index.
		  * \param pFBX_TYPE The specified class type.
		  * \param pCriteria The given criteria.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The source object that is of the specified class type and that satisfies the given criteria at the specified index, NULL if not found.
		  */
		template <class T> inline T* GetSrcObject(T const *pFBX_TYPE,KFbxCriteria const &pCriteria,int pIndex=0) const { return RootProperty.GetSrcObject(pFBX_TYPE,pCriteria,pIndex); }

		/** Searches the source object with the specified name that is the specified class type, starting at the specified index.
		  * \param pFBX_TYPE The specified class type.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The source object with the name, NULL if not found.
		  */
		template <class T> inline T* FindSrcObject(T const *pFBX_TYPE,const char *pName,int pStartIndex=0) const { return RootProperty.FindSrcObject(pFBX_TYPE,pName,pStartIndex); }

		/** Searches the source object with the specified name that is the specified class type and that satisfies the given criteria, starting at the specified index.
		  * \param pFBX_TYPE The specified class type.
		  * \param pCriteria The given criteria.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The source object with the name, NULL if not found.
		  */
		template <class T> inline T* FindSrcObject(T const *pFBX_TYPE,KFbxCriteria const &pCriteria,const char *pName,int pStartIndex=0) const { return RootProperty.FindSrcObject(pFBX_TYPE,pCriteria,pName,pStartIndex); }

		/** Connects this object to one destination object.
		  * \param pObject The destination object with which this object connects.
		  * \param pType The connection type between this object and the destination object.
		  * \return \c True on success, \c false otherwise.
		  */
		inline bool ConnectDstObject(KFbxObject* pObject,kFbxConnectionType pType=eFbxConnectionNone) { return RootProperty.ConnectDstObject(pObject,pType); }

		/** Judges whether this object connects with the destination object.
		  * \param pObject The destination object.
		  * \return \c True if this object connects with the destination object, \c false otherwise.
		  */
		inline bool IsConnectedDstObject(const KFbxObject* pObject) const { return RootProperty.IsConnectedDstObject(pObject); }

		/** Disconnects this object from the destination object.
		  * \param pObject The destination object from which this object disconnects.
		  * \return \c True on success, \c false otherwise.
		  */
		inline bool DisconnectDstObject(KFbxObject* pObject) { return RootProperty.DisconnectDstObject(pObject); }

		/** Disconnects this object from all destination objects.
		  * \return \c True if it disconnects all destination objects successfully, \c false otherwise.
		  */
		inline bool DisconnectAllDstObject() { return RootProperty.DisconnectAllDstObject(); }

		/** Disconnects this object from all destination objects that satisfy given criteria.
		  * \param pCriteria The given criteria.
		  * \return \c True if it disconnects all destination objects successfully, \c false otherwise.
		  */
		inline bool DisconnectAllDstObject(KFbxCriteria const &pCriteria) { return RootProperty.DisconnectAllDstObject(pCriteria); }

		/** Disconnects this object from all destination objects of the specified class type.
		  * \param pClassId The specified class type.
		  * \return \c True if it disconnects all destination objects of the specified class type successfully, \c false otherwise.
		  */
		inline bool DisconnectAllDstObject(kFbxClassId pClassId) { return RootProperty.DisconnectAllDstObject(pClassId); }

		/** Disconnects this object from all the destination objects that are the specified class type and that satisfy the given criteria.
		  * \param pClassId The specified class type.
		  * \param pCriteria The given criteria.
		  * \return \c True if it disconnects all the destination objects successfully, \c false otherwise.
		  */
		inline bool DisconnectAllDstObject(kFbxClassId pClassId,KFbxCriteria const &pCriteria) { return RootProperty.DisconnectAllDstObject(pClassId,pCriteria); }

		/** Returns the number of destination objects with which this object connects. 
		  * \return The number of destination objects with which this object connects. 
		  */
		inline int GetDstObjectCount() const { return RootProperty.GetDstObjectCount(); }

		/** Returns the number of destination objects with which this object connects that satisfy the given criteria. 
		  * \param pCriteria The given criteria.
		  * \return The number of destination objects with which this object connects that satisfy the given criteria.
		  */
		inline int GetDstObjectCount(KFbxCriteria const &pCriteria) const { return RootProperty.GetDstObjectCount(pCriteria); }

		/** Returns the number of destination objects of the specified class type with which this object connects. 
		  * \param pClassId The specified class type.
		  * \return The number of destination objects of the specified class type with which this object connects. 
		  */
		inline int GetDstObjectCount(kFbxClassId pClassId) const { return RootProperty.GetDstObjectCount(pClassId); }

		/** Returns the number of destination objects of the specified class type with which this object connects that also satisfy the given criteria . 
		  * \param pClassId The specified class type.
		  * \param pCriteria The given criteria.
		  * \return The number of destination objects that are the specified class type and that satisfy the given criteria.
		  */
		inline int GetDstObjectCount(kFbxClassId pClassId,KFbxCriteria const &pCriteria) const { return RootProperty.GetDstObjectCount(pClassId,pCriteria); }

		/** Returns the destination object at the specified index with which this object connects.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The destination object at the specified index, NULL if not found.
		  */
		inline KFbxObject* GetDstObject(int pIndex=0) const { return RootProperty.GetDstObject(pIndex); }

		/** Returns the destination object with which this object connects that satisfies the given criteria at the specified index.
		  * \param pCriteria The given criteria.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The destination object that satisfies the given criteria at the specified index, NULL if not found.
		  */
		inline KFbxObject* GetDstObject(KFbxCriteria const &pCriteria,int pIndex=0) const { return RootProperty.GetDstObject(pCriteria,pIndex); }

		/** Returns the destination object of the specified class type with which this object connects at the specified index.
		  * \param pClassId The specified class type.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The destination object of the specified class type at the specified index, NULL if not found.
		  */
		inline KFbxObject* GetDstObject(kFbxClassId pClassId,int pIndex=0) const { return RootProperty.GetDstObject(pClassId,pIndex); }

		/** Returns the destination object with which this object connects that is of the specified class type and that satisfies the given criteria at the specified index.
		  * \param pClassId The specified class type.
		  * \param pCriteria The given criteria.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The destination object that is of the specified class type and that satisfies the given criteria at the specified index, NULL if not found.
		  */
		inline KFbxObject* GetDstObject(kFbxClassId pClassId,KFbxCriteria const &pCriteria,int pIndex=0) const { return RootProperty.GetDstObject(pClassId,pCriteria,pIndex); }

		/** Searches the destination object with the specified name, starting at the specified index.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The destination object with the name, NULL if not found.
		  */
		inline KFbxObject* FindDstObject(const char *pName,int pStartIndex=0) const { return RootProperty.FindDstObject(pName,pStartIndex); }

		/** Searches the destination object with the specified name which satisfies the given criteria, starting at the specified index.
		  * \param pCriteria The given criteria.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The destination object with the name, NULL if not found.
		  */
		inline KFbxObject* FindDstObject(KFbxCriteria const &pCriteria,const char *pName,int pStartIndex=0) const { return RootProperty.FindDstObject(pCriteria,pName,pStartIndex); }

		/** Searches the destination object with the specified name which is the specified class type, starting at the specified index.
		  * \param pClassId The specified class type.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The destination object with the name, NULL if not found.
		  */
		inline KFbxObject* FindDstObject(kFbxClassId pClassId,const char *pName,int pStartIndex=0) const { return RootProperty.FindDstObject(pClassId,pName,pStartIndex); }

		/** Searches the destination object with the specified name that is the specified class type and that satisfies the given criteria, starting at the specified index.
		  * \param pClassId The specified class type.
		  * \param pCriteria The given criteria.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The destination object with the name, NULL if not found.
		  */
		inline KFbxObject* FindDstObject(kFbxClassId pClassId,KFbxCriteria const &pCriteria,const char *pName,int pStartIndex=0) const { return RootProperty.FindDstObject(pClassId,pCriteria,pName,pStartIndex); }

		/** Disconnects this object from all destination objects of the specified class type.
		  * \param pFBX_TYPE The specified class type.
		  * \return \c True if it disconnects all destination objects of the specified class type successfully, \c false otherwise.
		  */
		template <class T> inline bool DisconnectAllDstObject(T const *pFBX_TYPE) { return RootProperty.DisconnectAllDstObject(pFBX_TYPE); }

		/** Disconnects this object from all destination objects that are the specified class type and that satisfy the given criteria.
		  * \param pFBX_TYPE The specified class type.
		  * \param pCriteria The given criteria.
		  * \return \c True if it disconnects all destination objects successfully, \c false otherwise.
		  */
		template <class T> inline bool DisconnectAllDstObject(T const *pFBX_TYPE,KFbxCriteria const &pCriteria) { return RootProperty.DisconnectAllDstObject(pFBX_TYPE,pCriteria); }

		/** Returns the number of destination objects of the specified class type with which this object connects. 
		  * \param pFBX_TYPE The specified class type.
		  * \return The number of destination objects of the specified class type with which this object connects. 
		  */
		template <class T> inline int GetDstObjectCount(T const *pFBX_TYPE) const { return RootProperty.GetDstObjectCount(pFBX_TYPE); }

		/** Returns the number of destination objects with which this object connects that are the specified class type and that satisfy the given criteria. 
		  * \param pFBX_TYPE The specified class type.
		  * \param pCriteria The given criteria.
		  * \return The number of destination objects that are the specified class type and that satisfy the given criteria. 
		  */
		template <class T> inline int GetDstObjectCount(T const *pFBX_TYPE,KFbxCriteria const &pCriteria) const { return RootProperty.GetDstObjectCount(pFBX_TYPE,pCriteria); }

		/** Returns the destination object with which this object connects that is the specified class type at the specified index.
		  * \param pFBX_TYPE The specified class type.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The destination object of the specified class type at the specified index, NULL if not found.
		  */
		template <class T> inline T* GetDstObject(T const *pFBX_TYPE,int pIndex=0) const { return RootProperty.GetDstObject(pFBX_TYPE,pIndex); }

		/** Returns the destination object with which this object connects that is the specified class type and that satisfies the given criteria at the specified index.
		  * \param pFBX_TYPE The specified class type.
		  * \param pCriteria The given criteria.
		  * \param pIndex The specified index whose default value is 0.
		  * \return The destination object that is the specified class type and that satisfies the given criteria at the specified index, NULL if not found.
		  */
		template <class T> inline T* GetDstObject(T const *pFBX_TYPE,KFbxCriteria const &pCriteria,int pIndex=0) const { return RootProperty.GetDstObject(pFBX_TYPE,pCriteria,pIndex); }

		/** Searches the destination object with the specified name which is of the specified class type, starting at the specified index.
		  * \param pFBX_TYPE The specified class type.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The source object with the name, NULL if not found.
		  */
		template <class T> inline T* FindDstObject(T const *pFBX_TYPE,const char *pName,int pStartIndex=0) const { return RootProperty.FindDstObject(pFBX_TYPE,pName,pStartIndex); }

		/** Searches the destination object with the specified name that is the specified class type and that satisfies the given criteria, starting at the specified index.
		  * \param pFBX_TYPE The specified class type.
		  * \param pCriteria The given criteria.
		  * \param pName The object name.
		  * \param pStartIndex The start index.
		  * \return The source object with the name, NULL if not found.
		  */
		template <class T> inline T* FindDstObject(T const *pFBX_TYPE,KFbxCriteria const &pCriteria,const char *pName,int pStartIndex=0) const { return RootProperty.FindDstObject(pFBX_TYPE,pCriteria,pName,pStartIndex); }
	//@}

	/**
	  * \name General Property Connection and Relationship Management
	  */
	//@{
		/** Connects this object to a source property.
		  * \param pProperty The source property with which this object connects.
		  * \return \c True on success, \c false otherwise.
		  */
		inline bool ConnectSrcProperty(KFbxProperty const & pProperty) { return RootProperty.ConnectSrcProperty(pProperty); }

		/** Determines whether this object connects with the specified source property.
		  * \param pProperty The specified source property.
		  * \return \c True if this object connects with the specified source property, \c false otherwise.
		  */
		inline bool IsConnectedSrcProperty(KFbxProperty const & pProperty) { return RootProperty.IsConnectedSrcProperty(pProperty); }

		/** Disconnects this object from the specified source property.
		  * \param pProperty The specified source property.
		  * \return \c True on success, \c false otherwise.
		  */
		inline bool DisconnectSrcProperty(KFbxProperty const & pProperty) { return RootProperty.DisconnectSrcProperty(pProperty); }

		/** Returns the number of source properties with which this object connects. 
		  * \return The number of source properties with which this object connects. 
		  */
		inline int GetSrcPropertyCount() const { return RootProperty.GetSrcPropertyCount(); }

		/** Returns the source property at the specified index with which this object connects. 
		  * \param pIndex The specified index.
		  * \return The source property at the specified index. 
		  */
		inline KFbxProperty GetSrcProperty(int pIndex=0) const { return RootProperty.GetSrcProperty(pIndex); }

		/** Searches a source property with which this object connects that has a specific name, starting at the specified index. 
		  * \param pName The specified property name.
		  * \param pStartIndex The start index.
		  * \return The source property with the specified name. 
		  */
		inline KFbxProperty FindSrcProperty(const char *pName,int pStartIndex=0) const { return RootProperty.FindSrcProperty(pName,pStartIndex); }

		/** Connects this object to a destination property.
		  * \param pProperty The destination property with which this object connects.
		  * \return \c True on success, \c false otherwise.
		  */
		inline bool ConnectDstProperty(KFbxProperty const & pProperty) { return RootProperty.ConnectDstProperty(pProperty); }

		/** Determines if this object connects with the specified destination property.
		  * \param pProperty The specified destination property.
		  * \return \c True if this object connects with the specified destination property, \c false otherwise.
		  */
		inline bool IsConnectedDstProperty(KFbxProperty const & pProperty) { return RootProperty.IsConnectedDstProperty(pProperty); }

		/** Disconnects this object from the specified destination property.
		  * \param pProperty The specified destination property.
		  * \return \c True on success, \c false otherwise.
		  */
		inline bool DisconnectDstProperty(KFbxProperty const & pProperty) { return RootProperty.DisconnectDstProperty(pProperty); }

		/** Returns the number of destination properties with which this object connects. 
		  * \return The number of destination properties with which this object connects. 
		  */
		inline int GetDstPropertyCount() const { return RootProperty.GetDstPropertyCount(); }

		/** Returns the destination property at the specified index with which this object connects. 
		  * \param pIndex The specified index.
		  * \return The destination property at the specified index. 
		  */
		inline KFbxProperty GetDstProperty(int pIndex=0) const { return RootProperty.GetDstProperty(pIndex); }

		/** Searches a destination property with which this object connects that has a specific name, starting at the specified index. 
		  * \param pName The specified property name.
		  * \param pStartIndex The start index.
		  * \return The destination property with the specified name. 
		  */
		inline KFbxProperty FindDstProperty(const char *pName,int pStartIndex=0) const { return RootProperty.FindDstProperty(pName,pStartIndex); }
	//@}

	/**
	  * \name User data
	  */
	//@{
		/** Sets the data pointer for an user data record whose ID is pUserID.
		  * \param pUserID The ID of the user data record. 
		  * \param pUserData The data pointer of the user data record. 
		  * \remarks An user data record is composed of an ID and a data pointer.
		  * If the user data record identified by pUserID does not exist, a new user data record is created and its data pointer is set as pUserData. 
		  */
		void SetUserDataPtr(KFbxObjectID const& pUserID, void* pUserData);

		/** Returns the data pointer of an user data record whose ID is pUserID.
		  * \param pUserID The ID of the user data record.
		  * \return The data pointer of the user data record, \c NULL if the user data record is not found. 
		  */
		void* GetUserDataPtr(KFbxObjectID const& pUserID) const;

		/** Sets the data pointer for the user data record whose ID is the object ID.
		  * \param pUserData The data pointer of the user data record. 
		  * \remarks An user data record is composed of an ID and a data pointer.
		  * If the user data record identified by pUserID does not exist, a new user data record is created and its data pointer is set as pUserData.
		  */
		inline void SetUserDataPtr(void* pUserData){ SetUserDataPtr(GetUniqueID(), pUserData); }

		/** Returns the data pointer of the user data record whose ID is the object ID.
		  * \return The data pointer of the user data record, \c NULL if the user data record is not found. 
		  */
		inline void* GetUserDataPtr() const { return GetUserDataPtr(GetUniqueID()); }
	//@}

	/**
	  * \name Document Management
	  */
	//@{
		/** Returns a const pointer to the document that contains this object.
		  * \return A const pointer to the document that contains this object or \c NULL if the
		  *         object does not belong to any document.
		  */
		KFbxDocument* GetDocument() const;

		/** Returns a const pointer to the root document that contains this object.
		  * \return A const pointer to the root document that contains this object or \c NULL if the
		  * object does not belong to any document. 
		  * \remarks It returns this pointer if this object is a document object and does not belong to any document.
		  * That means this object is the root document.
		  */
		KFbxDocument* GetRootDocument() const;

		/** Returns a const pointer to the scene that contains this object.
		  * \return A pointer to the scene that contains this object or \c NULL if the
		  * object does not belong to any scene.
		  */
		KFbxScene* GetScene() const;
	//@}

	/**
	  * \name Logging.
	  */
	//@{
		/** Emits a message in all available message emitters in the document or SDK manager.
		  * \param pMessage The message to emit. 
		  * \remarks The ownership of the message is transferred, don't delete it.
		  */
		void EmitMessage(KFbxMessage * pMessage) const;
	//@}

	/**
	  * \name Localization helper.
	  */
	//@{
		/** Localization helper function, it calls the implementation of FBX SDK manager.
		  * Sub-classes that manage their own localization could over-ride this function.
		  * \param pID The identifier of the text to be localized.
		  * \param pDefault The default text. Uses pID as the default text if pDefault is NULL.
		  * \return The localized text or the default text if the text can't be localized, .
		  */
		virtual const char* Localize(const char* pID, const char* pDefault=NULL) const;
	//@}

	/**
	  * \name Application Implementation Management
	  */
	//@{
		/** Returns a handle on the parent library of this object.
		  * \return The parent library of this object, or \c NULL if the parent library doesn't exist.
		  */
		KFbxLibrary* GetParentLibrary() const;

		/** Adds an implementation.
		  * \param pImplementation The implementation to be added.
		  * \return \c True on success, \c false otherwise.
		  * \remarks To succeed this function must be called with an implementation that has not already been added to this node.
		  */
		bool AddImplementation(KFbxImplementation* pImplementation);

		/** Removes an implementation.
		  * \param pImplementation The implementation to be removed.
		  * \return \c True on success, \c false otherwise.
		  * \remarks To succeed this function must be called with an implementation that has already been added to this node.
		  */
		bool RemoveImplementation(KFbxImplementation* pImplementation);

		/** Determines if this shading node has a default implementation.
		  * \return \c True if this shading node has a default implementation, \c false otherwise.
		  */
		bool HasDefaultImplementation(void) const;

		/** Returns the default implementation of this shading node.
		  * \return The default implementation of this shading node.
		  */
		KFbxImplementation* GetDefaultImplementation(void) const;

		/** Sets the default implementation of this shading node.
		  * \param pImplementation The implementation to be set.
		  * \return \c True on success, \c false otherwise.
		  * \remarks To succeed this function must be called with an implementation that has already been added to this node.
		  *	Only the implementation which has already been added can be set as the default implementation.
		  */
		bool SetDefaultImplementation(KFbxImplementation* pImplementation);

		/** Returns the number of implementations that satisfy a given criteria.
		  * \param pCriteria The given criteria.
		  * \returns The number of implementations.
		  */
		int GetImplementationCount(const KFbxImplementationFilter* pCriteria=NULL) const;

		/** Returns the implementation at the specified index that satisfies the given criteria.
		  * \param pIndex The specified index.
		  * \param pCriteria The given criteria.
		  * \return The implementation at the specified index, NULL if not found.
		  */
		KFbxImplementation* GetImplementation(int pIndex, const KFbxImplementationFilter* pCriteria=NULL) const;
	//@}

	/**
	* \name Object Storage && Retrieval
	*/
	//@{
		/** Returns the URL of this object.
		  * \return The URL of this object.
		  * \remarks The URL indicates where the object is stored.
		  */
		virtual KString GetUrl() const;

		/** Sets the URL of this object.
		  * \param pUrl The URL to be set.
		  * \return \c True on success, \c false otherwise.
		  * \remarks The URL indicates where the object is stored.
		  */
		virtual bool SetUrl(char* pUrl);
	//@}

	/** Copy an object content into this object.
	  * \param pObject The source object to copy data from.
	  * \return Returns the destination object being modified by the source.
	  * \remarks This function replace the assignment operator (operator=). It will copy all property values and the name. Connections are NOT copied.
	  */
	virtual KFbxObject& Copy(const KFbxObject& pObject);

	//! The root property that holds all children property for this object
	KFbxProperty RootProperty;

protected:
	/** Clears this object's content from memory.
	  * This method must be overridden in the derived classes.
	  * \remarks This method is called by ContentUnload() if the object content's unloading is successful .
	  */
	virtual void ContentClear();

	/** Retrieves the peripheral of that object.
	* \return The current peripheral for that object
	* \remarks A peripheral manipulates the content of an object. For instance, a peripheral can load the connections of an object on demand.
	*/
	virtual KFbxPeripheral* GetPeripheral();

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
public:
    virtual bool			Compare(KFbxObject *pOtherObject,eFbxCompare pCompareMethod=eFbxCompareProperties);

    virtual KFbxSdkManager*	GetFbxSdkManager() const;
    virtual kFbxClassId		GetRuntimeClassId() const;

    typedef enum
    {
        eNONE                   = 0x00000000,
        eSYSTEM_FLAG            = 0x00000001,
        eSAVABLE_FLAG           = 0x00000002,
        eHIDDEN_FLAG            = 0x00000008,

        eSYSTEM_FLAGS           = 0x0000ffff,

        eUSER_FLAGS             = 0x000f0000,

        // These flags are not saved to the fbx file
        eSYSTEM_RUNTIME_FLAGS   = 0x0ff00000,

        eCONTENT_LOADED_FLAG    = 0x00100000,

        eUSER_RUNTIME_FIRST_FLAG= 0x10000000,
        eUSER_RUNTIME_FLAGS     = 0xf0000000,

        eRUNTIME_FLAGS          = 0xfff00000
    } EObjectFlag;

    void	SetObjectFlags(EObjectFlag pFlags, bool pValue);
    bool	GetObjectFlags(EObjectFlag pFlags) const;

    // All flags replaced at once. This includes overriding the runtime flags, so
    // most likely you'd want to do something like this:
    //
    // SetObjectFlags(pFlags | (GetObjectFlags() & KFbxObject::eRUNTIME_FLAGS));
    void	SetObjectFlags(kUInt pFlags);
    kUInt	GetObjectFlags() const; // All flags at once, as a bitmask

	//Basic comparison operator implementation. It simply compare property values between source and target.
	//NOTE: If a property cannot be found on one of the object, the comparison fails (return false).
	//Different classid will fail comparison as well as different property count. Reference properties are not compared.
	bool	operator==(KFbxObject const& pObject);
	bool	operator!=(KFbxObject const& pObject);

	//Important note: If this function is not implemented, the pFileSubTypeName string used when registering your
	//class via KFbxSdkManager::RegisterFbxClass will be used instead. This makes it useless to re-implement this
	//function if you do not intend to return a different string for the same class.
    virtual const char*	GetTypeName() const;

    virtual KStringList	GetTypeFlags() const;

    virtual void				SetDocument(KFbxDocument* pDocument);
	inline KFbxObjectHandle&	GetPropertyHandle() { return RootProperty.mPropertyHandle; }

	// This function will go as deep as possible to clear the Connection list without sending 
	// notifications to the connections to give them the chance to Disconnect themselves. 
	// This is a bypass of the intended workflow and should be used with care.
	void WipeAllConnections();

protected:
    KFbxObject(KFbxSdkManager& pManager, char const* pName);

    // Constructs the object. Each subclass should call the base
    // KFbxObject::Construct first. Note that property initialization
    // should be done in the ConstructProperties() method, not this one.
    // pFrom - The object this object should be cloned from. NULL to
    // construct an independent object.
    virtual void			Construct(const KFbxObject* pFrom);

    // Initialize the KFbxProperty member variables of this class.
    // Each subclass should call its parent, and initialize its properties
    // in this method.
    // pForceSet - Forces the property values to be set to default values.
    virtual bool			ConstructProperties(bool pForceSet);
    virtual void			Destruct(bool pRecursive, bool pDependents);

	bool					Copyable(const KFbxObject& pObject);

	virtual bool			SetRuntimeClassId(kFbxClassId pClassId);
	virtual bool			ConnecNotify(KFbxConnectEvent const& pEvent);
	virtual kFbxUpdateId	IncUpdateId(eFbxUpdateIdType pUpdateId=eUpdateId_Object);

	// property callbacks
	typedef enum {
		eFbxProperty_SetRequest,
		eFbxProperty_Set,
		eFbxProperty_Get
	} eFbxPropertyNotify;
	virtual bool PropertyNotify(eFbxPropertyNotify pType, KFbxProperty* pProperty);

private:
    void	CopyPropertiesFrom(const KFbxObject& pFrom);
	void	SetClassRootProperty(KFbxProperty &lProperty);
	int		GetFlatPropertyCount() const;

	_KFbxObjectData* mData;

    // friend classes for SDK access
    friend class KFbxScene;
    friend class KFbxProperty;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

inline bool KFbxConnectSrc(KFbxObject *pDstObject,KFbxObject *pSrcObject)   { return (pSrcObject && pDstObject) ? pDstObject->ConnectSrcObject(pSrcObject) : 0; }
inline bool KFbxConnectDst(KFbxObject *pSrcObject,KFbxObject *pDstObject)   { return (pSrcObject && pDstObject) ? pSrcObject->ConnectDstObject(pDstObject) : 0; }

typedef KFbxObject* HKFbxObject;

// template access functions SRC
template < class T > inline int KFbxGetSrcCount(KFbxObject const *pObject)                                                      { return pObject ? pObject->GetSrcObjectCount(T::ClassId) : 0; }
template < class T > inline int KFbxGetSrcCount(KFbxObject const *pObject,kFbxClassId pClassId)                                 { return pObject ? pObject->GetSrcObjectCount(pClassId) : 0;     }
template < class T > inline T* KFbxGetSrc(KFbxObject const *pObject,int pIndex=0)                                               { return pObject ? (T *) pObject->GetSrcObject(T::ClassId,pIndex) : 0; }
template < class T > inline T* KFbxGetSrc(KFbxObject const *pObject,int pIndex,kFbxClassId pClassId)                            { return pObject ? (T *) pObject->GetSrcObject(pClassId,pIndex) : 0;    }
template < class T > inline T* KFbxFindSrc(KFbxObject const *pObject,char const *pName,int pIndex=0)                            { return pObject ? (T *) pObject->FindSrcObject(T::ClassId,pName,pIndex) : 0;   }
template < class T > inline T* KFbxFindSrc(KFbxObject const *pObject,char const *pName,kFbxClassId pClassId,int pIndex=0)       { return pObject ? (T *) pObject->FindSrcObject(pClassId,pName,pIndex) : 0; }

template < class T > inline bool KFbxDisconnectAllSrc(KFbxObject *pObject)                                                      { return pObject->DisconnectAllSrcObject(T::ClassId);   }

// template access functions DST
template < class T > inline int KFbxGetDstCount(KFbxObject const *pObject)                                                      { return pObject ? pObject->GetDstObjectCount(T::ClassId) : 0; }
template < class T > inline int KFbxGetDstCount(KFbxObject const *pObject,kFbxClassId pClassId)                                 { return pObject ? pObject->GetDstObjectCount(pClassId) : 0;     }
template < class T > inline T* KFbxGetDst(KFbxObject const *pObject,int pIndex=0)                                               { return pObject ? (T *) pObject->GetDstObject(T::ClassId,pIndex) : 0; }
template < class T > inline T* KFbxGetDst(KFbxObject const *pObject,int pIndex,kFbxClassId pClassId)                            { return pObject ? (T *) pObject->GetDstObject(pClassId,pIndex) : 0;    }
template < class T > inline T* KFbxFindDst(KFbxObject const *pObject,char const *pName,int pIndex=0)                            { return pObject ? (T *) pObject->FindDstObject(T::ClassId,pName,pIndex) : 0;   }
template < class T > inline T* KFbxFindDst(KFbxObject const *pObject,char const *pName,kFbxClassId pClassId,int pIndex=0)       { return pObject ? (T *) pObject->FindDstObject(pClassId,pName,pIndex) : 0; }
template < class T > inline bool KFbxDisconnectAllDst(KFbxObject *pObject)                                                      { return pObject->DisconnectAllDstObject(T::ClassId);   }


/**********************************************************************
* Object Iterator
**********************************************************************/

/** A utility class for iterating over the properties (KFbxProperty) of any KFbxObject.
* \nosubgrouping
*/
template<typename KFbxProperty> class KFbxIterator
{
    public:
		/** Constructor.
		*\param pObject  The object whose properties are going to be iterated.
		*/
        KFbxIterator(KFbxObject const *pObject) : mObject(pObject) {}

		/**  Get the first property of the object. 
		*    \return The first property of the object.
		*/
        inline KFbxProperty const &GetFirst() { mProperty = mObject->GetFirstProperty(); return mProperty; }

		/**  Get next property of the object. 
		*    \return The next property of the object.
		*/
        inline KFbxProperty const &GetNext() { mProperty = mObject->GetNextProperty(mProperty); return mProperty; }

    private:
        KFbxProperty        mProperty;
        KFbxObject const*   mObject;
};

class KFbxIteratorSrcBase
{
protected:
    KFbxProperty    mProperty;
    kFbxClassId     mClassId;
    int             mSize;
    int             mIndex;
public:
    inline KFbxIteratorSrcBase(KFbxProperty &pProperty,kFbxClassId pClassId) :
        mProperty(pProperty),
        mClassId(pClassId),
        mSize(0),
        mIndex(-1)
    {
        ResetToBegin();
    }
    inline KFbxIteratorSrcBase(KFbxObject* pObject,kFbxClassId pClassId) :
        mProperty(pObject->RootProperty),
        mClassId(pClassId),
        mSize(0),
        mIndex(-1)
    {
        ResetToBegin();
    }
    inline KFbxObject* GetFirst()
    {
        ResetToBegin();
        return GetNext();
    }
    inline KFbxObject* GetNext()
    {
        mIndex++;
        return ((mIndex>=0) && (mIndex<mSize)) ? mProperty.GetSrcObject(mClassId,mIndex) : NULL;
    }
    inline KFbxObject* GetSafeNext()
    {
        mSize = mProperty.GetSrcObjectCount(mClassId);
        return GetNext();
    }
    inline KFbxObject* GetLast()
    {
        ResetToEnd();
        return GetPrevious();
    }
    inline KFbxObject* GetPrevious()
    {
        mIndex--;
        return ((mIndex>=0) && (mIndex<mSize)) ? mProperty.GetSrcObject(mClassId,mIndex) : NULL;
    }
    inline KFbxObject* GetSafePrevious()
    {
        mSize = mProperty.GetSrcObjectCount(mClassId);
        while (mIndex>mSize) mIndex--;
        return GetPrevious();
    }


// Internal Access Function
protected:
    inline void ResetToBegin()
    {
        mSize = mProperty.GetSrcObjectCount(mClassId);
        mIndex = -1;
    }
    inline void ResetToEnd()
    {
        mSize = mProperty.GetSrcObjectCount(mClassId);
        mIndex = mSize;
    }
};

template<class Type> class KFbxIteratorSrc : protected KFbxIteratorSrcBase
{
public:
    inline KFbxIteratorSrc(KFbxObject* pObject) : KFbxIteratorSrcBase(pObject,Type::ClassId) {}
    inline KFbxIteratorSrc(KFbxProperty& pProperty) : KFbxIteratorSrcBase(pProperty,Type::ClassId) {}
    inline Type *GetFirst()         { return (Type *)KFbxIteratorSrcBase::GetFirst(); }
    inline Type *GetNext()          { return (Type *)KFbxIteratorSrcBase::GetNext(); }
    inline Type *GetSafeNext()      { return (Type *)KFbxIteratorSrcBase::GetSafeNext(); }
    inline Type *GetLast()          { return (Type *)KFbxIteratorSrcBase::GetLast(); }
    inline Type *GetPrevious()      { return (Type *)KFbxIteratorSrcBase::GetPrevious(); }
    inline Type *GetSafePrevious()  { return (Type *)KFbxIteratorSrcBase::GetSafePrevious(); }

// Internal Access Function
protected:
};

class KFbxIteratorDstBase
{
protected:
    KFbxProperty    mProperty;
    kFbxClassId     mClassId;
    int             mSize;
    int             mIndex;
public:
    inline KFbxIteratorDstBase(KFbxProperty &pProperty,kFbxClassId pClassId) :
        mProperty(pProperty),
        mClassId(pClassId),
        mSize(0),
        mIndex(-1)
    {
        ResetToBegin();
    }
    inline KFbxIteratorDstBase(KFbxObject* pObject,kFbxClassId pClassId) :
        mProperty(pObject->RootProperty),
        mClassId(pClassId),
        mSize(0),
        mIndex(-1)
    {
        ResetToBegin();
    }
    inline KFbxObject* GetFirst()
    {
        ResetToBegin();
        return GetNext();
    }
    inline KFbxObject* GetNext()
    {
        mIndex++;
        return ((mIndex>=0) && (mIndex<mSize)) ? mProperty.GetDstObject(mClassId,mIndex) : NULL;
    }
    inline KFbxObject* GetSafeNext()
    {
        mSize = mProperty.GetDstObjectCount(mClassId);
        return GetNext();
    }
    inline KFbxObject* GetLast()
    {
        ResetToEnd();
        return GetPrevious();
    }
    inline KFbxObject* GetPrevious()
    {
        mIndex--;
        return ((mIndex>=0) && (mIndex<mSize)) ? mProperty.GetDstObject(mClassId,mIndex) : NULL;
    }
    inline KFbxObject* GetSafePrevious()
    {
        mSize = mProperty.GetDstObjectCount(mClassId);
        while (mIndex>mSize) mIndex--;
        return GetPrevious();
    }


// Internal Access Function
protected:
    inline void ResetToBegin()
    {
        mSize = mProperty.GetDstObjectCount(mClassId);
        mIndex = -1;
    }
    inline void ResetToEnd()
    {
        mSize = mProperty.GetDstObjectCount(mClassId);
        mIndex = mSize;
    }
};

template<class Type> class KFbxIteratorDst : protected KFbxIteratorDstBase
{
public:
    inline KFbxIteratorDst(KFbxObject* pObject) : KFbxIteratorDstBase(pObject,Type::ClassId) {}
    inline KFbxIteratorDst(KFbxProperty& pProperty) : KFbxIteratorDstBase(pProperty,Type::ClassId) {}
    inline Type *GetFirst()         { return (Type *)KFbxIteratorDstBase::GetFirst(); }
    inline Type *GetNext()          { return (Type *)KFbxIteratorDstBase::GetNext(); }
    inline Type *GetSafeNext()      { return (Type *)KFbxIteratorDstBase::GetSafeNext(); }
    inline Type *GetLast()          { return (Type *)KFbxIteratorDstBase::GetLast(); }
    inline Type *GetPrevious()      { return (Type *)KFbxIteratorDstBase::GetPrevious(); }
    inline Type *GetSafePrevious()  { return (Type *)KFbxIteratorDstBase::GetSafePrevious(); }
};

#define KFbxForEach(Iterator,Object) for ( (Object)=(Iterator).GetFirst(); (Object)!=0; (Object)=(Iterator).GetNext() )
#define KFbxReverseForEach(Iterator,Object) for ( Object=(Iterator).GetLast(); (Object)!=0;  Object=(Iterator).GetPrevious() )
#define KFbxForEach_Safe(Iterator,Object) for ( Object=(Iterator).GetFirst(); (Object)!=0; Object=(Iterator).GetSafeNext() )
#define KFbxReverseForEach_Safe(Iterator,Object) for ( Object=(Iterator).GetLast(); (Object)!=0;  Object=(Iterator).GetSafePrevious() )

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
class KFbxObjectPropertyChanged : public kfbxevents::KFbxEvent<KFbxObjectPropertyChanged>
{
	KFBXEVENT_DECLARE(KFbxObjectPropertyChanged);

public:
	KFbxObjectPropertyChanged(KFbxProperty pProp) : mProp(pProp) {}
	KFbxProperty mProp;
};
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXOBJECT_H

