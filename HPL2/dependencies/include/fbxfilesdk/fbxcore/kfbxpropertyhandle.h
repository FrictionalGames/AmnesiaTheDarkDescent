/*!  \file kfbxpropertyhandle.h
 */

#ifndef FBXFILESDK_FBXCORE_KFBXPROPERTYHANDLE_H
#define FBXFILESDK_FBXCORE_KFBXPROPERTYHANDLE_H

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

#include <fbxfilesdk/kfbxplugins/kfbxtypes.h>
#include <fbxfilesdk/fbxcore/kfbxpropertydef.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

	// Forwards
	class	KMemoryAllocator;
	class	KFbxPropertyPage;
	class   KFbxPropertyHandle;
	class   KFbxConnectionPointFilter;

	/***********************************************
	  KFbxPropertyHandle
	************************************************/
	typedef KFbxPropertyHandle	KFbxTypeInfoHandle;
	typedef KFbxPropertyHandle	KFbxObjectHandle;
	typedef int					kFbxPropertyId;

	/**	\brief Class to manage property handle.
	* \nosubgrouping
	*/
	class KFBX_DLL KFbxPropertyHandle
	{
		/**
		* \name Constructor and Destructor
		*/
		//@{
	public:

		//! Create an instance
		static KFbxObjectHandle		Create();

		/** Create an instance with given instance.
		  * \param pInstanceOf The given instance.
		  */
		static KFbxObjectHandle		Create(KFbxObjectHandle	const &pInstanceOf);

		/** Create an instance with given name and type.
		  * \param pName
		  * \param pType
		  */
		static KFbxTypeInfoHandle	Create(char const *pName,EFbxType pType=eUNIDENTIFIED);

		/** Create an instance with given name and type info.
		  * \param pName
		  * \param pTypeInfo
		  */
		static KFbxTypeInfoHandle	Create(char const *pName,KFbxTypeInfoHandle	pTypeInfo);

		/** If this property is root property, delete the property page, otherwise delete the property.
        * \return  If succeed, return true.
		*/
		bool						Destroy();
	public:
		//! Default constructor. 
		KFbxPropertyHandle();
		/** Copy constructor.
		  * \param pAddress KFbxPropertyHandle copied to this one.
		  */
		KFbxPropertyHandle(KFbxPropertyHandle const &pAddress);

		//! Destructor
		~KFbxPropertyHandle();

	public:
		/**  Character constructor.
		  * \param pPage
		  * \param pId
		  */
		KFbxPropertyHandle(KFbxPropertyPage* pPage,kFbxPropertyId pId=kFbxProperyIdRoot);


		//@}

		/**
		* \name Assignment and basic info
		*/
		//@{
	public:
		// Assignment
		/** KFbxPropertyHandle assignment operator.
		  * \param pHandle KFbxPropertyHandle assigned to this one.
		  */
		KFbxPropertyHandle &operator=(KFbxPropertyHandle const &pHandle);

		/** Equality operator.
		  * \param pHandle KFbxPropertyHandle compared with this one.
		  * \return \c True if equal, \c false otherwise.
		  */
		bool				operator==(KFbxPropertyHandle const &pHandle) const;

		/** Inequality operator.
		  * \param pHandle KFbxPropertyHandle compared with this one.
		  * \return \c True if unequal, \c false otherwise.
		  */
		bool				operator!=(KFbxPropertyHandle const &pHandle) const;

		/** Compare type info together
		  * \param pHandle KFbxPropertyHandle compared with this one.   
		  * \return \c True if equal, \c false otherwise.
		  */
		bool				Is			(KFbxPropertyHandle const &pHandle) const;

		//! Judge validity
		bool				Valid() const;

		//! Get the property name
		const char *			GetName() const;
		//! Get the property type
		EFbxType				GetType() const;
		//! Get the property type info
		KFbxTypeInfoHandle		GetTypeInfo() const;
		//! Get the property label
		const char *			GetLabel() const;

		/** Set a label to the property
		* \param pLabel    The given label string
		* \return  If succeed, return true.
		*/
		bool					SetLabel(char const *pLabel);

		// Flag management
		//! Get the property attribute state
		FbxPropertyFlags::eFbxPropertyFlags		GetFlags() const;

		/**	According the given parameter Change the attributes of the property.
		* \param pFlags The given flags used as mask.
		* \param pValue If pValue is true, set mask with given flags, otherwise unset mask with given flags.
		* \return  If succeed, return true.
		*/
		bool									ModifyFlags(FbxPropertyFlags::eFbxPropertyFlags pFlags, bool pValue);

		/**	Gets the inheritance type for the given flag. 
		* \param pFlags The flag to query
		* \param pCheckReferences Decide whether check instance. If it is true, check instance.
		* \return The inheritance type
		*/
		KFbxInheritType							GetFlagsInheritType( FbxPropertyFlags::eFbxPropertyFlags pFlags, bool pCheckReferences ) const;

		/**Sets the inheritance type for the given flag
		* \param pFlags The flag to set 
		* \param pType The inheritance type to set 
		* \return  If succeed, return true.
		*/
		bool									SetFlagsInheritType( FbxPropertyFlags::eFbxPropertyFlags pFlags, KFbxInheritType pType );

		//! Get the property user data.
		void *					GetUserData() const;

		/** Set user data to the property
		* \param pUserData The given user data
		* \return  If succeed, return true.
		*/
		bool					SetUserData(void const *pUserData);

		//! Get the property user tag
		int						GetUserTag() const;

		/** Set user tag to the property
		* \param pUserData The given user tag
		* \return  If succeed, return true.
		*/
		bool					SetUserTag(int pUserData);
		//@}

		/**
		* \name Enum management
		*/
		//@{
	public:

		/** Add new value at the end of the enum list in the property.
		* \param pStringValue The given new value
		* \return  The index of the value.
		*/
		int		AddEnumValue(char const *pStringValue);

		/** Insert new value at the given index of the enum list in property.
		* \param pIndex The given index
		* \param pStringValue The given new value
		*/
		void	InsertEnumValue(int pIndex, char const *pStringValue);

		/** Get the enum count of enum list in property
		* \return The enum count of enum list in property
		*/
		int		GetEnumCount();

		/** Set value at the given index of the enum list in the property.
		* \param pIndex  The given index
		* \param pStringValue The given new value used to instead the old value.
		*/
		void	SetEnumValue(int pIndex,  char const *pStringValue);

		/** Remove the value at the index of the enum list in the property.
		* \param pIndex    The given index
		*/
		void	RemoveEnumValue(int pIndex);

		/** Get the value at the index of enum list in the property.
		* \param pIndex    The given index
		* \return The value at the given index
		*/
		char *	GetEnumValue(int pIndex);
		//@}


		/**
		* \name Child and Struct management
		*/
		//@{
	public:
		/** Add a property to the property page.
		* \param pName The name of property.
		* \param pTypeInfo The added property's type info.
		* \return The handle of the new added property
		*/
		KFbxPropertyHandle		Add(char const * pName,KFbxTypeInfoHandle const &pTypeInfo);

		//! Create the map for find property in the property page
		void					BeginCreateOrFindProperty();
		//! Clear the map which created for find property.
		void					EndCreateOrFindProperty();

		/** Judge if the property is the root property.
		* \return Return true if this property is root property.
		*/
		inline bool				IsRoot() const				{ return (mPage && (mId==0)) ? true : false; }

		/** Judge if the property is the child property of the given parent property.
		* \param pParent The given parent property handle
		* \return Return true if this property is child of given property.
		*/
		bool					IsChildOf(KFbxPropertyHandle	const & pParent) const;

		/** Judge if the property is descendent property of the given property.
		* \param pParent The given parent property handle
		* \return Return true if this property is descendant of given property.
		*/
		bool					IsDescendentOf(KFbxPropertyHandle	const & pParent) const;

		/** Get parent property
		* \return If the parent property exists, return the property handle,otherwise return -1.
		*/
		KFbxPropertyHandle		GetParent() const;

		/** Set parent property handle.No matter what enters,the result is always false.
		* \param pOther
		* \return False
		*/
		bool					SetParent( KFbxPropertyHandle const& pOther );

		/**  Get child property 
		* \return  If the child property is exist, return the property handle,otherwise return -1.
		*/
		KFbxPropertyHandle		GetChild()  const;

		/**  Get sibling property  
		* \return If the sibling property is exist, return the property handle,otherwise return -1.
		*/
		KFbxPropertyHandle		GetSibling() const;

		/**  Get first descendent property 
		* \return If the descendent property is exist, return the first descendent property handle,otherwise return -1.
		*/
		KFbxPropertyHandle		GetFirstDescendent()  const;

		/**  Get first descendent property which after the given property 
		* \param pHandle The given property handle
		* \return If the descendent property can be found after the given property, 
		* return the first found property handle,otherwise return -1.
		*/
		KFbxPropertyHandle		GetNextDescendent(KFbxPropertyHandle const &pHandle) const;

		/** Find the property with given name 
		* \param pName The given property name
		* \param pCaseSensitive Decide if the given property name is case sensitive
		* \return  Return a property handle which be created with the found property   
		*/
		KFbxPropertyHandle		Find (char const *pName,bool pCaseSensitive) const;

		/** Find the property with given name and type info.
		* \param pName The given property name
		* \param pTypeInfo The given property type info
		* \param pCaseSensitive Decide if the given property name is case sensitive
		* \return  Return a property handle which be created with the found property   
		*/
		KFbxPropertyHandle		Find (char const *pName,KFbxTypeInfoHandle const &pTypeInfo,bool pCaseSensitive) const;

		/** Separate the given name by  children separator string and then find the property.The step is  
		*  strip the first part of the name and search, if the property can be found, strip the second part  
		*  of the name and continue search, until no property be found,then return the last found property.
		* \param pName The given property name
		* \param pChildrenSeparator The given children separator string 
		* \param pCaseSensitive Decide if the given property name is case sensitive
		* \return  Return a property handle which be created with the found property   
		*/
		KFbxPropertyHandle		Find (char const *pName, const char* pChildrenSeparator,bool pCaseSensitive) const;

		/** Separate the given name by  children separator string and then find the property.The step is  
		*  strip the first part of the name and search, if the property can be found, strip the second part  
		*  of the name and continue search, until no property be found,then return the last found property.
		* \param pName The given property name
		* \param pChildrenSeparator The given children separator string 
		* \param pTypeInfo The given property type info
		* \param pCaseSensitive Decide if the given property name is case sensitive
		* \return  Return a property handle which be created with the found property   
		*/
		KFbxPropertyHandle		Find (char const *pName, const char* pChildrenSeparator,KFbxTypeInfoHandle const &pTypeInfo,bool pCaseSensitive) const;
		//@}

		/**
		* \name Connection management
		*/
		//@{
	public:

		/** Connect source property.
		* \param pSrc    The given source property
		* \param pType    The given property type
		* \return If connect successfully, return true,otherwise, return false.
		*/
		bool				ConnectSrc		(KFbxPropertyHandle const &pSrc,kFbxConnectionType const pType=eFbxConnectionDefault); 

		/** Get source properties' count.
		* \param pFilter    The filter used to get sub connection point. If it is not zero, return the source count of the sub connection point.
		* Otherwise, return the src count of this property.
		* \return The count of source properties
		*/
		int					GetSrcCount		(KFbxConnectionPointFilter* pFilter=0) const; 

		/** Get source property with the given index.
		* \param pFilter    The filter used to get sub connection point. If it is not zero, return the source property of the sub connection point.
		* Otherwise, return the source property of this property.
		* \param pIndex    The given index
		* \return The source property handle.
		*/
		KFbxPropertyHandle	GetSrc			(KFbxConnectionPointFilter* pFilter=0,int pIndex=0) const; 

		/** Disconnect source property.
		* \param pSrc    The given source property
		* \return If disconnect successfully, return true,otherwise, return false.
		*/
		bool				DisconnectSrc	(KFbxPropertyHandle const &pSrc);

		/** Judge if it is connected with the given source property.
		* \param pSrc    The given source property
		* \return If it is connected, return true,otherwise, return false.
		*/
		bool				IsConnectedSrc	(KFbxPropertyHandle const &pSrc);

		/** Connect destination property.
		* \param pDst    The given destination property
		* \param pType    The given property type
		* \return If connect successfully, return true,otherwise, return false.
		*/
		bool				ConnectDst		(KFbxPropertyHandle const &pDst,kFbxConnectionType const pType=eFbxConnectionDefault); 

		/** Get destination properties' count.
		* \param pFilter    The filter used to get sub connection point.If it is not zero,return the destination count of the sub connection point.
		* Otherwise, return the destination count of this property.
		* \return The count of destination properties
		*/
		int					GetDstCount		(KFbxConnectionPointFilter* pFilter=0) const; 

		/** Get destination property with the given index.
		* \param pFilter    The filter used to get sub connection point.If it is not zero,return the destination property of the sub connection point.
		* Otherwise, return the destination property of this property.
		* \param pIndex    The given index
		* \return The destination property handle.
		*/
		KFbxPropertyHandle	GetDst			(KFbxConnectionPointFilter* pFilter=0,int pIndex=0) const; 

		/** Disconnect destination property.
		* \param pDst    The given destination property
		* \return If disconnect successfully, return true,otherwise, return false.
		*/
		bool				DisconnectDst	(KFbxPropertyHandle const &pDst);

		/** Judge if it is connected with the given destination property.
		* \param pDst    The given destination property
		* \return If it is connected, return true,otherwise, return false.
		*/
		bool				IsConnectedDst	(KFbxPropertyHandle const &pDst);

		//! Clear connect cache
		void				ClearConnectCache();

		//! Clear all connect without sending any notification (Internal use ONLY)
		void				WipeAllConnections();
		//@}

		/**
		* \name Limits Functions
		  * Minimum and maximum value limits can be associated with properties, but FBX 
		  * will not verify that these limits are respected. FBX however will store and 
		  * retrieve limits from files, and will assure that they are persistent in memory 
		  * while the property handle object exists. 
		  *
		  * Soft minimums and maximums values are specifying a second set of limits that can be 
		  * used for UI objects such as sliders. FBX will handle them the same way it does
		  * with the normal limits.
		  * 
		*/
		//@{
	public:

		/** Judge if this property has a minimum value.
		* \return If the minimum value exist, return true,otherwise, return false.
		*/
		bool                    HasMin() const;

		/** Get the minimum value and value type of this property.
		* \param pValue    The minimum value of this property.
		* \param pValueType The value type of this property.
		* \return If the minimum value exist, return true,otherwise, return false.
		*/
		bool							GetMin		(void *pValue,EFbxType pValueType) const;

		/** Set the minimum value and value type for this property.
		* \param pValue    The given minimum value .
		* \param pValueType The given value type .
		* \return If it be set successfully, return true,otherwise, return false.
		*/
		bool							SetMin		(void const *pValue,EFbxType pValueType);

		/** According the given value and its value type, set the minimum value and value type for this property.
		* \param pValue    The given value .
		* \return If it be set successfully, return true,otherwise, return false.
		*/
		template <class T> inline bool  SetMin		( T const &pValue )			{ return SetMin( &pValue,FbxTypeOf(pValue) ); }

		/** Get the minimum value of this property.
		* \param pFBX_TYPE    Not used in this function. This is a dummy argument for 
		*                     the correct instantiation of the templated function.
		* \return The minimum value of this property
		*/
		template <class T> inline T		GetMin		( T const *pFBX_TYPE) const	{ T lValue; GetMin( &lValue,FbxTypeOf(lValue) ); return lValue; }

		/** Judge if this property has soft minimum value.
		* \return If the soft minimum value exist, return true,otherwise, return false.
		*/
		bool                    HasSoftMin() const;

		/** Get the soft minimum value and value type of this property.
		* \param pValue    The soft minimum value of this property.
		* \param pValueType The value type of this property.
		* \return If the soft minimum value exist, return true,otherwise, return false.
		*/
		bool							GetSoftMin	(void *pValue,EFbxType pValueType) const;

		/** Set the soft minimum value and value type for this property.
		* \param pValue    The given soft minimum value .
		* \param pValueType The given value type .
		* \return If it be set successfully, return true,otherwise, return false.
		*/
		bool							SetSoftMin	(void const *pValue,EFbxType pValueType);

		/** According the given value and its value type, set the soft minimum value and value type for this property.
		* \param pValue    The given value .
		* \return If it be set successfully, return true,otherwise, return false.
		*/
		template <class T> inline bool  SetSoftMin	( T const &pValue )			{ return SetSoftMin( &pValue,FbxTypeOf(pValue) ); }

		/** Get the soft minimum value of this property.
		* \param pFBX_TYPE    Not used in this function. This is a dummy argument for 
		*                     the correct instantiation of the templated function.
		* \return The soft minimum value of this property
		*/
		template <class T> inline T		GetSoftMin	( T const *pFBX_TYPE) const	{ T lValue; GetSoftMin( &lValue,FbxTypeOf(lValue) ); return lValue; }

		/** Judge if this property has maximum value.
		* \return If the maximum value exist, return true,otherwise, return false.
		*/
		bool                    HasMax() const;

		/** Get the maximum value and value type of this property.
		* \param pValue    The maximum value of this property.
		* \param pValueType The value type of this property.
		* \return If the maximum value exist, return true,otherwise, return false.
		*/
		bool							GetMax		(void *pValue,EFbxType pValueType) const;

		/** Set the maximum value and value type for this property.
		* \param pValue    The given maximum value .
		* \param pValueType The given value type .
		* \return If it be set successfully, return true,otherwise, return false.
		*/
		bool							SetMax		(void const *pValue,EFbxType pValueType);

		/** According the given value and its value type, set the maximum value and value type for this property.
		* \param pValue    The given value .
		* \return If it be set successfully, return true,otherwise, return false.
		*/
		template <class T> inline bool  SetMax		( T const &pValue )			{ return SetMax( &pValue,FbxTypeOf(pValue) ); }

		/** Get the maximum value of this property.
		* \param pFBX_TYPE    Not used in this function. This is a dummy argument for 
		*                     the correct instantiation of the templated function.
		* \return The maximum value of this property
		*/
		template <class T> inline T		GetMax		( T const *pFBX_TYPE) const	{ T lValue; GetMax( &lValue,FbxTypeOf(lValue) ); return lValue; }

		/** Judge if this property has soft maximum value.
		* \return If the soft maximum value exist, return true,otherwise, return false.
		*/
		bool                    HasSoftMax() const;

		/** Get the soft maximum value and value type of this property.
		* \param pValue    The soft maximum value of this property.
		* \param pValueType The value type of this property.
		* \return If the soft maximum value exist, return true,otherwise, return false.
		*/
		bool							GetSoftMax	(void *pValue,EFbxType pValueType) const;

		/** Set the soft maximum value and value type for this property.
		* \param pValue    The given soft maximum value .
		* \param pValueType The given value type .
		* \return If it be set successfully, return true,otherwise, return false.
		*/
		bool							SetSoftMax	(void const *pValue,EFbxType pValueType);

		/** According the given value and its value type, set the soft maximum value and value type for this property.
		* \param pValue    The given value .
		* \return If it be set successfully, return true,otherwise, return false.
		*/
		template <class T> inline bool  SetSoftMax	( T const &pValue )			{ return SetSoftMax( &pValue,FbxTypeOf(pValue) ); }

		/** Get the soft maximum value of this property.
		* \param pFBX_TYPE    Not used in this function. This is a dummy argument for 
		*                     the correct instantiation of the templated function.
		* \return The soft maximum value of this property
		*/
		template <class T> inline T		GetSoftMax	( T const *pFBX_TYPE) const	{ T lValue; GetSoftMax( &lValue,FbxTypeOf(lValue) ); return lValue; }


		/**
		* \name Value 
		*/
		//@{
	public:

		/** Get value inherit type of this property.
		* \param pCheckReferences   If it is true,check instance of this property page,otherwise,only check this page.
		* \return The value inherit type of this property
		*/
		KFbxInheritType	GetValueInheritType(bool pCheckReferences) const;

		/** Set value inherit type for this property .
		* \param pType  The given value inherit type.
		* \return If set successfully, return true,otherwise, return false.
		*/
		bool			SetValueInheritType(KFbxInheritType pType);

		/** Get default value and value type of this property .
		* \param pValue  The gotten default value of this property.
		* \param pValueType The gotten default value type of this property.
		* \return If default value be gotten successfully, return true,otherwise, return false.
		*/
		bool			GetDefaultValue(void *pValue,EFbxType pValueType) const;

		/** Get value and value type of this property .
		* \param pValue  The gotten value of this property.
		* \param pValueType The gotten value type of this property.
		* \return If value be gotten successfully, return true,otherwise, return false.
		*/
		bool			Get(void *pValue,EFbxType pValueType) const;

		/** Set property value and value type for this property.
		* \param pValue    The given property value .
		* \param pValueType The given property value type 
		* \param pCheckValueEquality If it is true, when the given value is equal with
		* the property value, the property value will not be set.
		* \return If the property value be set successfully, return true,otherwise, return false.
		*/
		bool			Set(void const *pValue,EFbxType pValueType,bool pCheckValueEquality);

		/** Set property value with the given value .
		* \param pValue  The given value .
		* \return If set successfully, return true,otherwise, return false.
		*/
		template <class T> inline bool  Set( T const &pValue )			{ return Set( &pValue,FbxTypeOf(pValue) ); }

		/** get property value.
		* \param pFBX_TYPE  Not be used.
		* \return The gotten property value.
		*/
		template <class T> inline T		Get( T const *pFBX_TYPE) const	{ T lValue; Get( &lValue,FbxTypeOf(lValue) ); return lValue; }
		//@}

		/**
		* \name Page settings
		*/
		//@{
	public:

		/** Set the property page data pointer.
		* \param pData  The given page data pointer.
		*/
		void	SetPageDataPtr(void *pData);

		/** Get property page data pointer.
		* \return The gotten property page data pointer.
		*/
		void*	GetPageDataPtr() const;

		//@}

		/**
		* \name Page Internal Entry Management
		*/
		//@{

		/** Push properties to parent instance.
		* \return If push successful return true,otherwise,return false.
		*/
		bool PushPropertiesToParentInstance();


		//@}

		/**
		* \name Reference Management
		*/
		//@{
	public:
		/** Judge if this property page is a instance of other page.
		* \return If this property page is a instance of other page, return true,otherwise,return false.
		*/
		bool	IsAReferenceTo(void) const;

		/** Get the property page which this property page make reference to
		* \return The property page which this property page make reference to
		*/
		void*	GetReferenceTo(void) const;

		/** Judge if this property page is referenced by other pages.
		* \return If this property page is referenced by other pages, return true,otherwise,return false.
		*/
		bool	IsReferencedBy(void) const;

		/** Get the count of property pages which make reference to this property page.
		* \return The count of property pages which make reference to this property page.
		*/
		int		GetReferencedByCount(void) const;

		/** According the given index,get the property page which make reference to this property page.
		* \param pIndex The given index
		* \return The pointer to the property page which reference to this property page and be found by index.
		*/
		void*	GetReferencedBy(int pIndex) const; 
		//@}


	private:
		KFbxPropertyPage*		mPage;
		kFbxPropertyId			mId;

	};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_FBXCORE_KFBXPROPERTYHANDLE_H

