/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxlayer.h
 */
 
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXLAYER_H
#define FBXFILESDK_KFBXPLUGINS_KFBXLAYER_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/kdebug.h>

#include <fbxfilesdk/kfbxmath/kfbxvector2.h>
#include <fbxfilesdk/kfbxmath/kfbxvector4.h>
#include <fbxfilesdk/kfbxplugins/kfbxcolor.h>
#include <fbxfilesdk/kfbxplugins/kfbxdatatypes.h>
#include <fbxfilesdk/kfbxplugins/kfbxstream.h>
#include <fbxfilesdk/kfbxplugins/kfbxsurfacematerial.h>
#include <fbxfilesdk/kfbxplugins/kfbxtexture.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxTexture;
class KFbxSurfaceMaterial;
class KFbxSdkManager;

class KFbxGeometryBase;
class KFbxLayer;
class KFbxLayerContainer;
class KFbxMesh;
class KFbxNode;


/**  Base class for elements of layers (KFbxLayer). 
  *  A layer element type is identified by ELayerElementType. 
  *  A KFbxLayerElement describes how the layer element is mapped to a geometry surface 
  *  and how the mapping information is arranged in memory.
  *  A KFbxLayerElement contains Normals, UVs or other kind of information.
  *
  * \see KFbxLayer
  * \see KFbxLayerElement::EMappingMode
  * \see KFbxLayerElement::EReferenceMode
  */
class KFBX_DLL KFbxLayerElement
{

public:

	/** \enum ELayerElementType     Layer Element type identifier.
	  * - \e eUNDEFINED                        Undefined Layer Element class.
	  * - \e eNORMAL                           Layer Element of type KFbxLayerElementNormal.
      * - \e eBINORMAL                         Layer Element of type KFbxLayerElementBinormal.
      * - \e eTANGENT                          Layer Element of type KFbxLayerElementTangent.
	  * - \e eMATERIAL                         Layer Element of type KFbxLayerElementMaterial.
	  * - \e eDIFFUSE_TEXTURES                 Layer Element of type KFbxLayerElementTexture.
	  * - \e ePOLYGON_GROUP                    Layer Element of type KFbxLayerElementPolygonGroup.
	  * - \e eUV                               Layer Element of type KFbxLayerElementUV.
	  * - \e eVERTEX_COLOR                     Layer Element of type KFbxLayerElementVertexColor.
	  * - \e eSMOOTHING                        Layer Element of type KFbxLayerElementSmoothing.
      * - \e eVERTEX_CREASE                    Layer Element of type KFbxLayerElementCrease.
      * - \e eEDGE_CREASE                      Layer Element of type KFbxLayerElementCrease.
      * - \e eHOLE                             Layer Element of type KFbxLayerElementHole.
	  * - \e eUSER_DATA                        Layer Element of type KFbxLayerElementUserData.
	  * - \e eVISIBILITY                       Layer Element of type KFbxLayerElementVisibility.
	  * - \e eEMISSIVE_TEXTURES                Layer Element of type KFbxLayerElementTexture.
	  * - \e eEMISSIVE_FACTOR_TEXTURES         Layer Element of type KFbxLayerElementTexture.
	  * - \e eAMBIENT_TEXTURES                 Layer Element of type KFbxLayerElementTexture.
	  * - \e eAMBIENT_FACTOR_TEXTURES          Layer Element of type KFbxLayerElementTexture.
	  * - \e eDIFFUSE_FACTOR_TEXTURES          Layer Element of type KFbxLayerElementTexture.
	  * - \e eSPECULAR_TEXTURES                Layer Element of type KFbxLayerElementTexture.
	  * - \e eNORMALMAP_TEXTURES               Layer Element of type KFbxLayerElementTexture.
	  * - \e eSPECULAR_FACTOR_TEXTURES         Layer Element of type KFbxLayerElementTexture.
	  * - \e eSHININESS_TEXTURES               Layer Element of type KFbxLayerElementTexture.
	  * - \e eBUMP_TEXTURES                    Layer Element of type KFbxLayerElementTexture.
	  * - \e eTRANSPARENT_TEXTURES             Layer Element of type KFbxLayerElementTexture.
	  * - \e eTRANSPARENCY_FACTOR_TEXTURES     Layer Element of type KFbxLayerElementTexture.
	  * - \e eREFLECTION_TEXTURES              Layer Element of type KFbxLayerElementTexture.
	  * - \e eREFLECTION_FACTOR_TEXTURES       Layer Element of type KFbxLayerElementTexture.
      * - \e eDISPLACEMENT_TEXTURES            Layer Element of type KFbxLayerElementTexture.
      * - \e eVECTOR_DISPLACEMENT_TEXTURES     Layer Element of type KFbxLayerElementTexture.
	  * - \e eLAST_ELEMENT_TYPE
	  */
	typedef enum 
	{
		eUNDEFINED,

        //NON-TEXTURE LAYER ELEMENT BEGIN
		eNORMAL,
        eBINORMAL,
        eTANGENT,
		eMATERIAL,
		ePOLYGON_GROUP,
		eUV,
		eVERTEX_COLOR,
		eSMOOTHING,
        eVERTEX_CREASE,
        eEDGE_CREASE,
        eHOLE,
		eUSER_DATA,
		eVISIBILITY,
        //NEW NON-TEXTURE LAYER ELEMENT ADD HERE...

        //NON-TEXTURE LAYER ELEMENT END

        //TEXTURE LAYER ELEMENT BEGIN
        eDIFFUSE_TEXTURES,
        eDIFFUSE_FACTOR_TEXTURES,
		eEMISSIVE_TEXTURES,
		eEMISSIVE_FACTOR_TEXTURES,
		eAMBIENT_TEXTURES,
		eAMBIENT_FACTOR_TEXTURES,
		eSPECULAR_TEXTURES,
        eSPECULAR_FACTOR_TEXTURES,
        eSHININESS_TEXTURES,
		eNORMALMAP_TEXTURES,
		eBUMP_TEXTURES,
		eTRANSPARENT_TEXTURES,
		eTRANSPARENCY_FACTOR_TEXTURES,
		eREFLECTION_TEXTURES,
		eREFLECTION_FACTOR_TEXTURES,
        eDISPLACEMENT_TEXTURES,
        //NEW TEXTURE LAYER ELEMENT ADD HERE...
        eVECTOR_DISPLACEMENT_TEXTURES,

        //TEXTURE LAYER ELEMENT END

		eLAST_ELEMENT_TYPE
	} ELayerElementType;

    //common used constant for looping through and access layer element

    //! The start index of texture type layer elements. 
    const static int LAYERELEMENT_TYPE_TEXTURE_START_INDEX = int(eDIFFUSE_TEXTURES);

    //! The end index of texture type layer elements.
    const static int LAYERELEMENT_TYPE_TEXTURE_END_INDEX = int(eLAST_ELEMENT_TYPE) - 1;

    //! The count of texture type layer elements. 
    const static int LAYERELEMENT_TYPE_TEXTURE_COUNT = LAYERELEMENT_TYPE_TEXTURE_END_INDEX - LAYERELEMENT_TYPE_TEXTURE_START_INDEX + 1;

    //! The start index of non-texture type layer elements.
    const static int LAYERELEMENT_TYPE_NON_TEXTURE_START_INDEX = int(eNORMAL);

    //! The end index of non-texture type layer elements.
    const static int LAYERELEMENT_TYPE_NON_TEXTURE_END_INDEX = int(eVISIBILITY);

    //! The count of non-texture type layer elements. 
    const static int LAYERELEMENT_TYPE_NON_TEXTURE_COUNT = LAYERELEMENT_TYPE_NON_TEXTURE_END_INDEX - LAYERELEMENT_TYPE_NON_TEXTURE_START_INDEX + 1;

    //names for different textures and non-textures channels

    //! Array of names of texture type layer elements.
    static const char* const TEXTURE_NAMES[];

    //! Array of names of UV layer elements.
    static const char* const TEXTURE_UV_NAMES[];

    //! Array of names of non-texture type layer elements.
    static const char* const NON_TEXTURE_NAMES[];

    //! Array of texture types.
    static const KFbxDataType TEXTURE_TYPES[];

    //! Array of texture channels.
    static const char* const TEXTURE_CHANNEL_NAMES[];

	/**	\enum EMappingMode     Determines how the element is mapped to a surface.
	  * - \e eNONE                  The mapping is undetermined.
	  * - \e eBY_CONTROL_POINT      There will be one mapping coordinate for each surface control point/vertex.
	  * - \e eBY_POLYGON_VERTEX     There will be one mapping coordinate for each vertex, for every polygon of which it is a part.
	                                This means that a vertex will have as many mapping coordinates as polygons of which it is a part.
	  * - \e eBY_POLYGON            There can be only one mapping coordinate for the whole polygon.
	  * - \e eBY_EDGE               There will be one mapping coordinate for each unique edge in the mesh.
	                                This is meant to be used with smoothing layer elements.
	  * - \e eALL_SAME              There can be only one mapping coordinate for the whole surface.
	  */
	typedef enum 
	{
		eNONE,
		eBY_CONTROL_POINT,
		eBY_POLYGON_VERTEX,
		eBY_POLYGON,
		eBY_EDGE,
		eALL_SAME
	} EMappingMode;

	/** \enum EReferenceMode     Determines how the mapping information is stored in the array of coordinates.
	  * - \e eDIRECT              This indicates that the mapping information for the n'th element is found in the n'th place of 
	                              KFbxLayerElementTemplate::mDirectArray.
	  * - \e eINDEX,              This symbol is kept for backward compatibility with FBX v5.0 files. In FBX v6.0 and higher, 
	                              this symbol is replaced with eINDEX_TO_DIRECT.
	  * - \e eINDEX_TO_DIRECT     This indicates that the KFbxLayerElementTemplate::mIndexArray
	                              contains, for the n'th element, an index in the KFbxLayerElementTemplate::mDirectArray
	                              array of mapping elements. eINDEX_TO_DIRECT is usually useful for storing eBY_POLYGON_VERTEX mapping 
                                  mode elements coordinates. Since the same coordinates are usually
	                              repeated many times, this saves spaces by storing the coordinate only one time
	                              and then referring to them with an index. Materials and Textures are also referenced with this
	                              mode and the actual Material/Texture can be accessed via the KFbxLayerElementTemplate::mDirectArray
	  */
	typedef enum 
	{
		eDIRECT,
		eINDEX,
		eINDEX_TO_DIRECT
	} EReferenceMode;

	
	/** Sets the Mapping Mode.
	  * \param pMappingMode     Specifies the way that layer element is mapped to a surface.
	  */
	void SetMappingMode(EMappingMode pMappingMode) { mMappingMode = pMappingMode; }

	/** Sets the Reference Mode.
	  * \param pReferenceMode     Specifies the reference mode.
	  */
	void SetReferenceMode(EReferenceMode pReferenceMode) { mReferenceMode = pReferenceMode; }

	/** Returns the Mapping Mode.
	  * \return     The current Mapping Mode.
	  */
	EMappingMode GetMappingMode() const { return mMappingMode; }

	/** Returns the Reference Mode.
	  * \return     The current Reference Mode.
	  */
	EReferenceMode GetReferenceMode() const { return mReferenceMode; }

	/** Sets the name of this object.
	  * \param pName     Specifies the name of this LayerElement object.
	  */
	void SetName(const char* pName) { mName = KString(pName); }

	/** Returns the name of this object.
	  * \return     The current name of this LayerElement object.
	  */
	const char* GetName() const { return ((KFbxLayerElement*)this)->mName.Buffer(); }

    /** Equivalence operator
      * \param pOther     Layer element to be compared.
      * \return           \c True if equal, \c false otherwise.
      */
	bool operator==(const KFbxLayerElement& pOther) const
	{
		return (mName == pOther.mName) && 
			   (mMappingMode == pOther.mMappingMode) &&
			   (mReferenceMode == pOther.mReferenceMode);
	}

    /** Assignment operator
      * \param pOther      Layer element assigned to this one.
      * \return            This layer element after assignment.
      */
	KFbxLayerElement& operator=( KFbxLayerElement const& pOther )
	{
		mMappingMode = pOther.mMappingMode;
		mReferenceMode = pOther.mReferenceMode;
		// name, type and owner should not be copied because they are
		// initialized when this object is created
		return *this;
	}

    //! Removes this layer element from its owner and delete it.
	void Destroy();

    //! Clears all the data from this layer element.
    virtual bool Clear() 
    { 
        return true;
    }

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void SetType(const KFbxDataType* pType) { mType = pType; }
	const KFbxLayerContainer* GetOwner() const { return mOwner; }

protected:
	KFbxLayerElement() 
		: mMappingMode(eNONE)
		, mReferenceMode(eDIRECT)
		, mName("")
		, mOwner(NULL)
	{
	}
	
	virtual ~KFbxLayerElement()
	{
	}

	EMappingMode mMappingMode;
	EReferenceMode mReferenceMode;

	KString mName;
	const KFbxDataType* mType;
	KFbxLayerContainer* mOwner;

	void Destruct() { FbxSdkDelete(this); }
	virtual void SetOwner(KFbxLayerContainer* pOwner);

    KFBXNEW_DECLARE_FRIEND
	friend class KFbxLayerContainer;

public:
	/** Queries the amount of memory used by this
	  * object AND its content (does not consider the content pointed)
	  * \return                The amount of memory used.
	  */
	virtual int MemorySize() const { return 0; }

	/**
	  * \name Serialization section
	  */
	//@{

    /** Writes the content of this layer element to the given stream.
      * \param pStream         The destination stream.
      * \return                \c True if the content is successfully processed
      *                        by the receiving stream, \c false otherwise.
      */
	virtual bool ContentWriteTo(KFbxStream& pStream) const;

    /** Reads the content of this layer element from the given stream.
      * \param pStream         The source stream.
      * \return                \c True if the object fills itself with the received data
      *                        from the stream successfully, \c false otherwise.
      */
	virtual bool ContentReadFrom(const KFbxStream& pStream);
	//@}

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

/** \internal
  * Identifies what error occurs when the data arrays are manipulated.
  * \nosubgrouping
  */

class KFBX_DLL LockAccessStatus
{
public:
	/** \internal
      * \enum ELockAccessStatus	        Identifies what error occurs when the data arrays are manipulated.
	  * - \e eSuccess					Operation Successful.
	  * - \e eUnsupportedDTConversion   Attempts to convert to an unsupported DataType.
	  * - \e eCorruptedCopyback         The Release of a converted buffer fails and corrupts the main data.
	  * - \e eBadValue                  Invalid value.
	  * - \e eLockMismatch              Attempts to change to an incompatible lock.
	  * - \e eNoWriteLock               A write operation is attempted but no WriteLock is available.
	  * - \e eNoReadLock                A read operation is attempted but the WriteLock is active.
	  * - \e eNotOwner                  Attempts to release a lock on an invalid data buffer pointer.
	  * - \e eDirectLockExist           A direct access lock is still active.
	  */
	typedef enum {
		eSuccess,
		eUnsupportedDTConversion,
		eCorruptedCopyback,
		eBadValue,
		eLockMismatch,
		eNoWriteLock,
		eNoReadLock,
		eNotOwner,
		eDirectLockExist
	} ELockAccessStatus;
};

/** KFbxLayerElementArray is the base class for KFbxLayerElementArrayTemplate, 
  * it provides lock handling and data array manipulation of the data buffer for KFbxLayerElement.
  * \nosubgrouping
  */

class KFBX_DLL KFbxLayerElementArray
{
public:
	/**
	  * \name Constructor and Destructor
	  */
	//@{

	/** Constructor.
	  * \param pDataType The data type of the items in the data array.
	  */
	KFbxLayerElementArray(EFbxType pDataType);

	//!Destructor.
	virtual ~KFbxLayerElementArray();

	//@}

	/**
	  * \name Status handling
	  */
	//@{

	//!Clears the access state and sets it to eSuccess.
	inline                                void ClearStatus()     { mStatus = LockAccessStatus::eSuccess; }

	//!Retrieves the access state.
	inline LockAccessStatus::ELockAccessStatus GetStatus() const { return mStatus; }
	//@}

	/** 
	  * \name Locks handling
	  */
	//@{

	/** Returns whether write is locked.
	  * \return        \c True if write is locked, \c false otherwise.
	  */
	inline bool IsWriteLocked() const { return mWriteLock; };

	/** Retrieves the read lock count.
	  * \return           The read lock count.
	  */
	inline int  GetReadLockCount() const { return mReadLockCount; }
	//@}

	/** Returns whether this Array is accessed in any way.
      * \return           \c True if it is in use, \c false otherwise.
	  */
	bool	IsInUse() const;

	/** Increments the number of read locks on this array.
	  * \return           The current number of read locks (including the one just grabbed) or 0 if a write lock is active.
	  */
	int     ReadLock() const;

	/** Releases a read lock on this array.
	  * \return           The remaining read locks or -1 if a write lock is active.
	  */
	int     ReadUnlock() const;

	/** Locks this array for writing. The data in the array is wiped out.
	  * \return           \c True if a write lock has been successfully granted, \c false if one or more read locks
	  *                   are active.
	  */
	bool    WriteLock() const;

	/** Releases the write lock on this array.
	  */
	void    WriteUnlock() const;

	/** Locks this array for writing. The data that already exists in the array is kept and is valid.
	  * \return           \c True if a write lock has been successfully granted, \c false if one or more read locks
	  *                   are active.
	  */
	bool    ReadWriteLock() const;

	/** Releases the write lock on this array.
	  */
	void    ReadWriteUnlock() const;


    /** \enum ELockMode	           Identifies the access mode to the data buffer.
    * - \e eREAD_LOCK			   Read mode.
    * - \e eWRITE_LOCK             Write mode.
    * - \e eREADWRITE_LOCK         Read-write mode.
    */
	typedef enum {
		eREAD_LOCK = 1,
		eWRITE_LOCK = 2,
		eREADWRITE_LOCK = 3
	} ELockMode;

	/** Grants a locked access to the data buffer. 
	  * \param pLockMode                 Access mode to the data buffer.
	  * \param pDataType                 If defined, tries to return the data as this type.
	  * \return                          A pointer to the data buffer or NULL if a failure occurs.
	  * \remarks                         In the case of a failure, the Status is updated with the
	  *                                  reason for the failure. Also, when a type conversion occurs, a second buffer 
	  *                                  of the new type is allocated. In this case, the LockMode does not apply to the
	  *	                                 returned buffer since it is a copy but it does apply to the internal data of this
	  *	                                 object. The returned buffer still remains a property of this object and is
	  *	                                 deleted when the pointer is released or the object is destroyed. At the moment of 
      *                                  release or destruction, the values in this buffer are copied back into this object.
	  */
    virtual void*   GetLocked(ELockMode pLockMode, EFbxType pDataType);

	/** Grants a locked access to the data buffer. 
	  * \param pLockMode                 Access mode to the data buffer.
	  * \return                          A pointer to the data buffer or NULL if a failure occurs.
      * \remarks                         In the case of a failure, the Status is updated with the
      *                                  reason for the failure. Also, when a type conversion occurs, a second buffer 
      *                                  of the new type is allocated. In this case, the LockMode does not apply to the
      *	                                 returned buffer since it is a copy but it does apply to the internal data of this
      *	                                 object. The returned buffer still remains a property of this object and is
      *	                                 deleted when the pointer is released or the object is destroyed. At the moment of 
      *                                  release or destruction, the values in this buffer are copied back into this object.
	  */
	void*   GetLocked(ELockMode pLockMode=eREADWRITE_LOCK) { return GetLocked(pLockMode, mDataType); }

	/** Grants a locked access to the data buffer. 
      * \param dummy                     The data type of dummy is used to specialize this function.
	  * \param pLockMode                 Access mode to the data buffer.
	  * \return                          A pointer to the data buffer or NULL if a failure occurs.
      * \remarks                         In the case of a failure, the Status is updated with the
      *                                  reason for the failure. Also, when a type conversion occurs, a second buffer 
      *                                  of the new type is allocated. In this case, the LockMode does not apply to the
      *	                                 returned buffer since it is a copy but it does apply to the internal data of this
      *	                                 object. The returned buffer still remains a property of this object and is
      *	                                 deleted when the pointer is released or the object is destroyed. At the moment of 
      *                                  release or destruction, the values in this buffer are copied back into this object.
	  */
	template <class T> inline T* GetLocked(T* dummy=NULL, ELockMode pLockMode=eREADWRITE_LOCK) {T v; return (T*)GetLocked(pLockMode, _FbxTypeOf(v)); }

	/** Unlock the data buffer.
	  * \param pDataPtr                  The buffer to be released.
	  * \param pDataType                 The data type of the data buffer.
	  * \remarks                         The passed pointer must be the one obtained by the call to GetLocked().
	  *                                  Any other pointer causes this method to fail and the Status is updated with
	  *                                  the reason for the failure. If the passed pointer refers a converted data
	  *	                                 buffer (see comment of GetLocked), this method copies the GetCount() items 
	  *	                                 of the received buffer back into this object. Any other items that may have been added
	  *	                                 using a realloc call are ignored.
	  */    
	virtual void   Release(void** pDataPtr, EFbxType pDataType);

	/** Unlock the data buffer.
	  * \param pDataPtr                  The buffer to be released.
      * \remarks                         The passed pointer must be the one obtained by the call to GetLocked().
      *                                  Any other pointer causes this method to fail and the Status is updated with
      *                                  the reason for the failure. If the passed pointer refers a converted data
      *	                                 buffer (see comment of GetLocked), this method copies the GetCount() items 
      *	                                 of the received buffer back into this object. Any other items that may have been added
      *	                                 using a realloc call are ignored.
	  */    
	void   Release(void** pDataPtr) { Release(pDataPtr, mDataType); }

	/** Unlock the data buffer.
	  * \param pDataPtr                  The buffer to be released.
      * \param dummy                     The data type of dummy is used to specialize this function.
      * \remarks                         The passed pointer must be the one obtained by the call to GetLocked().
      *                                  Any other pointer causes this method to fail and the Status is updated with
      *                                  the reason for the failure. If the passed pointer refers a converted data
      *	                                 buffer (see comment of GetLocked), this method copies the GetCount() items 
      *	                                 of the received buffer back into this object. Any other items that may have been added
      *	                                 using a realloc call are ignored.
	  */    
	template <class T> inline void Release(T** pDataPtr, T* dummy) { Release((void**)pDataPtr, _FbxTypeOf(*dummy)); }

	/** Returns the Stride size which equals the size of the data type of the data buffer.
	  */
	virtual size_t GetStride() const;

    /**
	  * \name Data array manipulation
	  */
	//@{

    //! Returns the count of items in the data buffer.
	int		GetCount() const;

    /** Sets the count of items in the data buffer.
      * \param pCount               The count of items to be set.
      */
	void	SetCount(int pCount);

    //! Clears the data buffer.
	void	Clear();

    /** Resizes the data buffer.
      * \param pItemCount           The new size of the data buffer. 
      */
	void	Resize(int pItemCount);

    /** Appends space to the data buffer.
      * \param pItemCount           The appended space size
      */
	void	AddMultiple(int pItemCount);

    /** Appends a new item to the end of the data buffer.
      * \param pItem                Pointer of the new item to be added
      * \param pValueType           Data type of the new item 
      * \return                     The index of the new item
      */
	int     Add(void const* pItem, EFbxType pValueType);

    /** Inserts a new item at the specified position of the data buffer.
      * \param pIndex               The specified position
      * \param pItem                Pointer of the new item to be inserted
      * \param pValueType           Data type of the new item 
      * \return                     The index of the inserted item
      * \remarks The input index must be within valid range and no error will be thrown if it is invalid.
      */
	int		InsertAt(int pIndex, void const* pItem, EFbxType pValueType);

    /** Sets the value for the specified item.
      * \param pIndex               The index of the item to be updated.
      * \param pItem                Pointer of the item whose value is copied to pIndex'th item
      * \param pValueType           Data type of the item 
      * \remarks The input index must be within valid range and no error will be thrown if it is invalid.
      */
	void	SetAt(int pIndex, void const* pItem, EFbxType pValueType);

    /** Sets the value of the last item.
      * \param pItem                Pointer of the item whose value is copied to the last item
      * \param pValueType           Data type of the item 
      * \remarks The array should contain at least one item and no error will be thrown if it is empty.
      */
	void    SetLast(void const* pItem, EFbxType pValueType);

    /** Removes the specified item from the data buffer.
      * \param pIndex               The index of the item to be removed               
      * \param pItem                Place to hold the value of the removed item.
      * \param pValueType           Data type of the item
      * \remarks The input index must be within valid range and no error will be thrown if it is invalid.
      */
	void    RemoveAt(int pIndex, void** pItem, EFbxType pValueType);

    /** Removes the last item from the data buffer.    
      * \param pItem                Place to hold the value of the removed item.
      * \param pValueType           Data type of the item
      * \remarks The array should contain at least one item and no error will be thrown if it is empty.
      */
	void    RemoveLast(void** pItem, EFbxType pValueType);

    /** Removes one item from the data buffer.    
      * \param pItem                The first item who equals pItem is to be removed
      * \param pValueType           Data type of the item 
      * \return                     \c True if the item is removed successfully, \c false otherwise                     
      */
	bool    RemoveIt(void** pItem, EFbxType pValueType);

    /** Returns the specified item's value.  
      * \param pIndex               Index of the item
      * \param pItem                Place to hold the item's value
      * \param pValueType           Data type of the item 
      * \return                     \c True if the item's value is returned successfully, \c false otherwise 
      * \remarks                    If the index is invalid, pItem is set to zero.
      */
	bool    GetAt(int pIndex, void** pItem, EFbxType pValueType) const;

    /** Returns the first item's value.  
      * \param pItem                Place to hold the item's value
      * \param pValueType           Data type of the item 
      * \return                     \c True if the item's value is returned successfully, \c false otherwise 
      */
	bool    GetFirst(void** pItem, EFbxType pValueType) const;

    /** Returns the last item's value.  
      * \param pItem                Place to hold the item's value
      * \param pValueType           Data type of the item 
      * \return                     \c True if the item's value is returned successfully, \c false otherwise 
      */
	bool    GetLast(void** pItem, EFbxType pValueType) const;

    /** Searches for an item in the data buffer.  
      * \param pItem                The value of the item for which to search.
      * \param pValueType           Data type of the item 
      * \return                     The index of the item found, -1 if not found.
      * \remarks                    The index of the first item whose value equals pItem is returned.
      */
	int     Find(void const* pItem, EFbxType pValueType) const;

    /** Searches for an item after the specified index in the data buffer.  
      * \param pAfterIndex          The specified index after which the searching begins
      * \param pItem                The value of the item for which to search, the searching begins after pAfterIndex.
      * \param pValueType           Data type of the item 
      * \return                     The index of the item found, -1 if not found.
      * \remarks                    The index of the first item whose value equals pItem is returned.
      */
	int     FindAfter(int pAfterIndex, void const* pItem, EFbxType pValueType) const;

    /** Searches for an item before the specified index in the data buffer.  
      * \param pBeforeIndex         The specified index before which the searching begins
      * \param pItem                The value of the item for which to search, the searching begins before pBeforeIndex.
      * \param pValueType           The item's data type. 
      * \return                     The index of the item found, -1 if not found.
      * \remarks                    The index of the first item whose value equals pItem is returned.
      */
	int     FindBefore(int pBeforeIndex, void const* pItem, EFbxType pValueType) const;

    /** Equivalence operator
      * \param pArray               Array compared to this one
      * \return                     \c True if equal. \c false otherwise.
      */
	bool    IsEqual(const KFbxLayerElementArray& pArray) const;

    /** Appends a new item to the end of the data buffer.
      * \param pItem                The new item to be added
      * \return                     The index of the new item
      */   
	template <class T> inline int  Add(T const& pItem)								 { return Add((void const*)&pItem, _FbxTypeOf(pItem)); }

    /** Inserts a new item at the specified position of the data buffer.
      * \param pIndex               The specified position
      * \param pItem                The new item to be inserted
      * \return                     The index of the inserted item
      * \remarks The input index must be within valid range and no error will be thrown if it is invalid.
      */
	template <class T> inline int  InsertAt(int pIndex, T const& pItem)				 { return InsertAt(pIndex, (void const*)&pItem, _FbxTypeOf(pItem)); }

    /** Sets the value of the specified item.
      * \param pIndex               The index of the item to be updated.
      * \param pItem                The item whose value is copied to pIndex'th item
      * \remarks The input index must be within valid range and no error will be thrown if it is invalid.
      */
	template <class T> inline void SetAt(int pIndex, T const& pItem)				 { SetAt(pIndex, (void const*)&pItem, _FbxTypeOf(pItem)); }

    /** Sets the value of the last item.
      * \param pItem                The item whose value is copied to the last item
      * \remarks The array should contain at least one item and no error will be thrown if it is empty.
      */
	template <class T> inline void SetLast(T const& pItem)							 { SetLast((void const*)&pItem, _FbxTypeOf(pItem)); }

    /** Removes the specified item from the data buffer.
      * \param pIndex               The index of the item to be removed               
      * \param pItem                Place to hold the value of the removed item.
      * \remarks The input index must be within valid range and no error will be thrown if it is invalid.
      */
	template <class T> inline void RemoveAt(int pIndex, T* pItem)					 { RemoveAt(pIndex, (void**)&pItem, _FbxTypeOf(*pItem)); }

    /** Removes the last item from the data buffer.    
      * \param pItem                Place to hold the value of the removed item.
      * \remarks The array should contain at least one item and no error will be thrown if it is empty.
      */
	template <class T> inline void RemoveLast(T* pItem)								 { RemoveLast((void**)&pItem, _FbxTypeOf(*pItem)); }

    /** Removes one item from the data buffer.    
      * \param pItem                The first item who equals pItem is to be removed
      * \return                     \c True if the item is removed successfully, \c false otherwise                     
      */
	template <class T> inline bool RemoveIt(T* pItem)								 { return RemoveIt((void**)&pItem, _FbxTypeOf(*pItem)); }

    /** Returns the specified item's value.  
      * \param pIndex               Index of the item
      * \param pItem                Place to hold the item's value
      * \return                     \c True if the item's value is returned successfully, \c false otherwise 
      * \remarks                    If the index is invalid, pItem is set to zero.
      */
	template <class T> inline bool GetAt(int pIndex, T* pItem) const				 { return GetAt(pIndex, (void**)&pItem, _FbxTypeOf(*pItem)); }

    /** Returns the first item's value.  
      * \param pItem                Place to hold the item's value
      * \return                     \c True if the item's value is returned successfully, \c false otherwise 
      */
	template <class T> inline bool GetFirst(T* pItem) const							 { return GetFirst((void**)&pItem, _FbxTypeOf(*pItem)); }

    /** Returns the last item's value.  
      * \param pItem                Place to hold the item's value
      * \return                     \c True if the item's value is returned successfully, \c false otherwise 
      */
	template <class T> inline bool GetLast(T* pItem) const							 { return GetLast((void**)&pItem, _FbxTypeOf(*pItem)); }

    /** Searches for an item in the data buffer.  
      * \param pItem                The value of the item for which to search.
      * \return                     The index of the item found, -1 if not found.
      * \remarks                    The index of the first item whose value equals pItem is returned.
      */
	template <class T> inline int Find(T const& pItem) const						 { return Find((void const*)&pItem, _FbxTypeOf(pItem)); }

    /** Searches for an item after the specified index in the data buffer.  
      * \param pAfterIndex          The specified index after which the searching begins
      * \param pItem                The value of the item for which to search, the searching begins after pAfterIndex.
      * \return                     The index of the item found, -1 if not found.
      * \remarks                    The index of the first item whose value equals pItem is returned.
      */
	template <class T> inline int FindAfter(int pAfterIndex, T const& pItem) const   { return FindAfter(pAfterIndex, (void const*)&pItem, _FbxTypeOf(pItem)); }

    /** Searches for one item before the specified index in the data buffer.  
      * \param pBeforeIndex         The specified index before which the searching begins
      * \param pItem                The value of the item for which to search, the searching begins before pBeforeIndex.
      * \return                     The index of the item found, -1 if not found.
      * \remarks                    The index of the first item whose value equals pItem is returned.
      */
	template <class T> inline int FindBefore(int pBeforeIndex, T const& pItem) const { return FindBefore(pBeforeIndex, (void const*)&pItem, _FbxTypeOf(pItem)); }


    /** Copies the items in the data buffer to an array.
      * \param pDst                 The destination array where the items are to be copied.
      */
	template<typename T> inline void CopyTo(KArrayTemplate<T>& pDst)
	{
		T src;
		T* srcPtr = &src;

		pDst.Clear();
		if (mDataType != _FbxTypeOf(src))
		{
			SetStatus(LockAccessStatus::eUnsupportedDTConversion);
			return;
		}

		pDst.SetCount(GetCount());
		for (int i = 0; i < GetCount(); i++)
		{
			if (GetAt(i, (void**)&srcPtr, mDataType))
			{
				pDst.SetAt(i, src);
			}
		}
		SetStatus(LockAccessStatus::eSuccess);
	}
	//@}

protected:
	void*   GetDataPtr();
	void*   GetReference(int pIndex, EFbxType pValueType);
	void    GetReferenceTo(int pIndex, void** pRef, EFbxType pValueType);

	inline void SetStatus(LockAccessStatus::ELockAccessStatus pVal) const
	{
		const_cast<KFbxLayerElementArray*>(this)->mStatus = pVal;
	}

	        void   SetImplementation(void* pImplementation);
	inline 	void*  GetImplementation() { return mImplementation; }
	virtual void   ConvertDataType(EFbxType pDataType, void** pDataPtr, size_t* pStride);

	EFbxType mDataType;

private:
	LockAccessStatus::ELockAccessStatus	mStatus;

	int			  mReadLockCount;
	bool		  mWriteLock;
	void*		  mImplementation;
	size_t        mStride;
	int           mDirectLockOn;
	bool          mDirectAccessOn;

	KArrayTemplate<void*>	mConvertedData;

};

/** \internal
  * This class provides simple RAII-style read locking of a KFbxLayerElementArray object.
  */
template <typename T>
struct KFbxLayerElementArrayReadLock
{
    /** \internal
      * On construction, this class requires the read lock.
      */
    KFbxLayerElementArrayReadLock(KFbxLayerElementArray& pArray) : mArray(pArray)
    {
        mLockedData = mArray.GetLocked((T*)NULL, KFbxLayerElementArray::eREAD_LOCK);
    }

    /** \internal
      * On destruction, this class releases the read lock.
      */
    ~KFbxLayerElementArrayReadLock()
    {
        if( mLockedData )
        {
            mArray.Release((void **) &mLockedData);
        }
    }

    /** \internal
      * Retrieve the locked array data.
      */
    const T* GetData() const
    {
        return mLockedData;
    }

private:
    KFbxLayerElementArray&  mArray;
    T* mLockedData;
};

class KFbxLayerElementUserData;

/** KFbxLayerElementArrayTemplate provides data array manipulation of the data buffer for KFbxLayerElement.
  * It is the subclass of KFbxLayerElementArray.
  * \nosubgrouping
  */
template <class T> class KFbxLayerElementArrayTemplate : public KFbxLayerElementArray
{
public:

    /** Constructor
      * \param pDataType                The data type of the array items.
      */
	KFbxLayerElementArrayTemplate(EFbxType pDataType) :
		KFbxLayerElementArray(pDataType)
		{
		}

    /** Appends a new item to the end of the data buffer.
      * \param pItem                The new item to be added
      * \return                     The index of the new item
      */   
	inline int  Add( T const &pItem )						{ return KFbxLayerElementArray::Add(pItem); }

    /** Inserts a new item at the specified position of the data buffer.
      * \param pIndex               The specified position
      * \param pItem                The new item to be inserted
      * \return                     The index of the inserted item
      */
	inline int  InsertAt(int pIndex, T const &pItem)		{ return KFbxLayerElementArray::InsertAt(pIndex, pItem); }

    /** Sets the value of the specified item.
      * \param pIndex               The index of the item to be updated.
      * \param pItem                The item whose value is copied to pIndex'th item
      */
	inline void SetAt(int pIndex, T const &pItem)			{ KFbxLayerElementArray::SetAt(pIndex, pItem); }

    /** Sets the value of the last item.
      * \param pItem                The item whose value is copied to the last item
      */
	inline void SetLast( T const &pItem)					{ KFbxLayerElementArray::SetLast(pItem); }

    /** Removes the specified item from the data buffer.
      * \param pIndex               The index of the item to be removed
      * \return                     The value of the item removed
      */
	inline T RemoveAt(int pIndex)			   				{ T lValue; KFbxLayerElementArray::RemoveAt(pIndex, &lValue); return lValue; }

    /** Removes the last item from the data buffer.    
      * \return                     The value of the last item removed
      */
	inline T RemoveLast()									{ T lValue; KFbxLayerElementArray::RemoveLast(&lValue); return lValue; }

    /** Removes one item from the data buffer.    
      * \param pItem                The first item who equals pItem is to be removed
      * \return                     \c True if the item is removed successfully, \c false otherwise                     
      */
	inline bool RemoveIt(T const &pItem)					{ return KFbxLayerElementArray::RemoveIt(&pItem); }

    /** Returns the specified item's value.  
      * \param pIndex               Index of the item
      * \return                     The value of the specified item
      * \remarks                    If the index is invalid, pItem is set to zero.
      */
	inline T  GetAt(int pIndex) const						{ T lValue; KFbxLayerElementArray::GetAt(pIndex, &lValue); return lValue; }

    /** Returns the first item's value.  
      * \return                     The first item's value.                        
      */
	inline T  GetFirst() const								{ T lValue; KFbxLayerElementArray::GetFirst(&lValue); return lValue; }

    /** Returns the last item's value.  
      * \return                     The last item's value.                        
      */
	inline T  GetLast() const								{ T lValue; KFbxLayerElementArray::GetLast(&lValue); return lValue; }

    /** Searches for an item in the data buffer.  
      * \param pItem                The value of the item for which to search
      * \return                     The index of the item found, -1 if not found.
      * \remarks                    The index of the first item whose value equals pItem is returned.
      */
	inline int Find(T const &pItem)							{ return KFbxLayerElementArray::Find(pItem); }

    /** Searches for an item after the specified index in the data buffer.  
      * \param pAfterIndex          The specified index after which the searching begins
      * \param pItem                The value of the item for which to search, the searching begins after pAfterIndex.
      * \return                     The index of the item found, -1 if not found.
      * \remarks                    The index of the first item whose value equals pItem is returned.
      */
	inline int FindAfter(int pAfterIndex, T const &pItem)	{ return KFbxLayerElementArray::FindAfter(pAfterIndex, pItem); }

    /** Searches for one item before the specified index in the data buffer.  
      * \param pBeforeIndex         The specified index before which the searching begins
      * \param pItem                The value of the item for which to search, the searching begins before pBeforeIndex.
      * \return                     The index of the item found, -1 if not found.
      * \remarks                    The index of the first item whose value equals pItem is returned.
      */
	inline int FindBefore(int pBeforeIndex, T const &pItem) { return KFbxLayerElementArray::FindBefore(pBeforeIndex, pItem); }

    /** Returns the specified item's value.  
      * \param pIndex               Index of the item
      * \return                     The value of the item
      * \remarks                    If the index is invalid, pItem is set to zero.
      */
	T  operator[](int pIndex) const							{ T lValue; KFbxLayerElementArray::GetAt(pIndex, &lValue); return lValue; }	

    /** Assignment operator.
      * \param pArrayTemplate       The source array whose items are copied to this array.
      */
    KFbxLayerElementArray& operator=(const KArrayTemplate<T>& pArrayTemplate)
    {
        SetStatus(LockAccessStatus::eNoWriteLock);
        if (WriteLock())
        {
            SetCount(pArrayTemplate.GetCount());
            for (int i = 0; i < pArrayTemplate.GetCount(); i++)
                SetAt(i, pArrayTemplate.GetAt(i));
            WriteUnlock();
            SetStatus(LockAccessStatus::eSuccess);
        }
        return *this;
    }

    /** Assignment operator.
      * \param pArrayTemplate           The source array whose items are copied to this array.
      */
    KFbxLayerElementArrayTemplate<T>& operator=(const KFbxLayerElementArrayTemplate<T>& pArrayTemplate)
    {
        if ( this != &pArrayTemplate )
        {
            SetStatus(LockAccessStatus::eNoWriteLock);
            if (WriteLock())
            {
                SetCount(pArrayTemplate.GetCount());
                for (int i = 0; i < pArrayTemplate.GetCount(); i++)
                    SetAt(i, pArrayTemplate.GetAt(i));
                WriteUnlock();
                SetStatus(LockAccessStatus::eSuccess);
            }
        }
        return *this;
    }

private:
	// This one is not the best thing to do, but at least I don't get deprecated calls inside this file.
	// Note that KFbxLayerElementUserData is kind of a weird class in the first place anyway. So either
	// we clean it up, or we live with this piece of code ;-)
	friend class KFbxLayerElementUserData;
	T& AsReference(int pIndex) 		{ T* v = (T*)KFbxLayerElementArray::GetReference(pIndex, mDataType); return (v)?*v:dummy;}

	T dummy;
};


/** Remap the index array to a new EMappingMode
  * \param pLayerEl     The layer element to remap
  * \param pNewMapping  The new mapping mode
  * \param pIndexArray  The index array to modify
  * \return return -1 if the layer element is KFbxLayerElement::eDIRECT
  *         0 if layer element or index array is \c NULL and 1 if the remap is successful
  */
extern int RemapIndexArrayTo(KFbxLayerElement* pLayerEl, 
							 KFbxLayerElement::EMappingMode pNewMapping, 
							 KFbxLayerElementArrayTemplate<int>* pIndexArray);


/** This class complements the KFbxLayerElement class.
  * It provides interfaces to access the direct array and index array of different layer elements.
  * \nosubgrouping
  */
template <class Type> class KFbxLayerElementTemplate : public KFbxLayerElement
{
public:

	/** Returns the direct array of Layer Elements.
	  * \return      A reference to the Layer Elements direct array.
	  * \remarks     You cannot put elements in the direct array when the mapping mode is set to eINDEX.
	  */
	KFbxLayerElementArrayTemplate<Type>& GetDirectArray() const
	{ 
		K_ASSERT(mReferenceMode == KFbxLayerElement::eDIRECT || mReferenceMode == KFbxLayerElement::eINDEX_TO_DIRECT);
		return *mDirectArray; 
	}

	/** Returns the direct array of Layer Elements.
	  * \return      A reference to the Layer Elements direct array.
	  * \remarks     You cannot put elements in the direct array when the mapping mode is set to eINDEX.
	  */
	KFbxLayerElementArrayTemplate<Type>& GetDirectArray()
	{ 
		K_ASSERT(mReferenceMode == KFbxLayerElement::eDIRECT || mReferenceMode == KFbxLayerElement::eINDEX_TO_DIRECT);
		return *mDirectArray; 
	}

	/** Returns the index array of Layer Elements.
	  * \return      A reference to the index array.
	  * \remarks     You cannot put elements in the index array when the mapping mode is set to eDIRECT.
	  */
	KFbxLayerElementArrayTemplate<int>& GetIndexArray() const
	{ 
		K_ASSERT(mReferenceMode == KFbxLayerElement::eINDEX || mReferenceMode == KFbxLayerElement::eINDEX_TO_DIRECT);
		return *mIndexArray; 
	}

	/** Returns the index array of Layer Elements.
	  * \return      A reference to the index array.
	  * \remarks     You cannot put elements in the index array when the mapping mode is set to eDIRECT.
	  */
	KFbxLayerElementArrayTemplate<int>& GetIndexArray()
	{ 
		K_ASSERT(mReferenceMode == KFbxLayerElement::eINDEX || mReferenceMode == KFbxLayerElement::eINDEX_TO_DIRECT);
		return *mIndexArray; 
	}

	/** Removes all elements from the direct and the index arrays.
	  * \remarks This function fails if there is a lock on the arrays.
	  * \return \c True if successful, \c false if a lock is present.
	  */
	bool Clear()
	{
		bool ret = true;
		mDirectArray->Clear();
		ret = (mDirectArray->GetStatus() == LockAccessStatus::eSuccess);

		mIndexArray->Clear();
		ret |= (mIndexArray->GetStatus() == LockAccessStatus::eSuccess);

		return ret;
	}

public:

    /** Equivalence operator.
      * \param pOther               Another element compared to this object
      * \return                     \c True if equal, \c false if unequal.
      */
	bool operator==(const KFbxLayerElementTemplate& pOther) const
	{
		bool ret = true;

        if (pOther.GetReferenceMode() == KFbxLayerElement::eDIRECT || 
            pOther.GetReferenceMode() == KFbxLayerElement::eINDEX_TO_DIRECT)
        {
            const KFbxLayerElementArrayTemplate<Type>& directArray = pOther.GetDirectArray();
            if( directArray.GetCount() != mDirectArray->GetCount() || 
                !directArray.ReadLock() || !mDirectArray->ReadLock() )
            {
                ret = false;
            }

            if( ret && !mDirectArray->IsEqual(directArray) )
                ret = false;

            directArray.ReadUnlock();
            mDirectArray->ReadUnlock();
        }

        if (ret)
        {
            if (pOther.GetReferenceMode() == KFbxLayerElement::eINDEX || 
                pOther.GetReferenceMode()  == KFbxLayerElement::eINDEX_TO_DIRECT)
            {
                const KFbxLayerElementArrayTemplate<int>& indexArray = pOther.GetIndexArray();
                if( indexArray.GetCount() != mIndexArray->GetCount() ||
                    !indexArray.ReadLock() || !mIndexArray->ReadLock() )
                {
                    ret = false;
                }

                if( ret && !mIndexArray->IsEqual(indexArray) )
                    ret = false;

                indexArray.ReadUnlock();
                mIndexArray->ReadUnlock();
            }
        }

        if (ret == false)
            return false;

        return KFbxLayerElement::operator==(pOther);
	}

    /** Assignment operator.
      * \param pOther               Another element assigned to this one
      */
	KFbxLayerElementTemplate& operator=( KFbxLayerElementTemplate const& pOther )
	{
		K_ASSERT(mDirectArray != NULL);
		K_ASSERT(mIndexArray != NULL);

		if (pOther.GetReferenceMode() == KFbxLayerElement::eDIRECT || 
			pOther.GetReferenceMode() == KFbxLayerElement::eINDEX_TO_DIRECT)
		{
			const KFbxLayerElementArrayTemplate<Type>& directArray = pOther.GetDirectArray();
			*mDirectArray = directArray;
		}

		if (pOther.GetReferenceMode() == KFbxLayerElement::eINDEX || 
			pOther.GetReferenceMode()  == KFbxLayerElement::eINDEX_TO_DIRECT)
		{
			const KFbxLayerElementArrayTemplate<int>& indexArray = pOther.GetIndexArray();
			*mIndexArray = indexArray;
		}
		
		KFbxLayerElement* myself = (KFbxLayerElement*)this;
		KFbxLayerElement* myOther = (KFbxLayerElement*)&pOther;
		*myself = *myOther;
		return *this; 
	}

	/** Changes the Mapping mode to the new one and re-computes the index array.
	  * \param pNewMapping          New mapping mode.
	  * \return                     If the remapping is successful, returns 1.
	  *                             If an error occurs, returns 0. In case the function cannot
	  *                             remap to the desired mode because of incompatible modes or
	  *                             unsupported modes, returns -1.
	  */
	int RemapIndexTo(KFbxLayerElement::EMappingMode pNewMapping)
	{
		return RemapIndexArrayTo(this, pNewMapping, mIndexArray);
	}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
	KFbxLayerElementTemplate() 
	{
		mDirectArray = NULL;
		mIndexArray = NULL;
	}

	~KFbxLayerElementTemplate() 
	{
		FbxSdkDelete(mDirectArray);
		FbxSdkDelete(mIndexArray);
	}

	virtual void AllocateArrays()
	{
		mDirectArray = FbxSdkNew< KFbxLayerElementArrayTemplate<Type> >(mType->GetType());
		mIndexArray = FbxSdkNew< KFbxLayerElementArrayTemplate<int> >(DTInteger.GetType());
	}

public:
	virtual int MemorySize() const
	{
		int size = KFbxLayerElement::MemorySize();
		size += (mDirectArray->GetCount()*sizeof(Type));
		size += (mIndexArray->GetCount()*sizeof(int));
		return size;
	}

	/**
	  * \name Serialization section
	  */
	//@{
	virtual bool ContentWriteTo(KFbxStream& pStream) const
	{
		void* a;
		int s,v;
		int count = 0;

		// direct array
		count = mDirectArray->GetCount();
		s = pStream.Write(&count, sizeof(int)); 
		if (s != sizeof(int)) return false;
		if (count > 0)
		{
			a = mDirectArray->GetLocked();
			K_ASSERT(a != NULL);
			v = count*sizeof(Type);
			s = pStream.Write(a, v); 
			mDirectArray->Release(&a);
			if (s != v) return false;
		}

		// index array
		count = mIndexArray->GetCount();
		s = pStream.Write(&count, sizeof(int)); 
		if (s != sizeof(int)) return false;
		if (count > 0)
		{
			a = mIndexArray->GetLocked();
			K_ASSERT(a != NULL);
			v = count*sizeof(int);
			s = pStream.Write(a, v);
			mIndexArray->Release(&a);
			if (s != v) return false;
		}

		return KFbxLayerElement::ContentWriteTo(pStream);
	}

	virtual bool ContentReadFrom(const KFbxStream& pStream)
	{
		void* a;
		int s,v;
		int count = 0;

		// direct array
		s = pStream.Read(&count, sizeof(int)); 
		if (s != sizeof(int)) return false;
		mDirectArray->Resize(count);
		if (count > 0)
		{
			a = mDirectArray->GetLocked();
			K_ASSERT(a != NULL);
			v = count*sizeof(Type);
			s = pStream.Read(a, v); 
			mDirectArray->Release(&a);
			if (s != v) return false;
		}

		// index array
		s = pStream.Read(&count, sizeof(int)); 
		if (s != sizeof(int)) return false;
		mIndexArray->Resize(count);		
		if (count > 0)
		{
			a = mIndexArray->GetLocked();
			K_ASSERT(a != NULL);
			v = count*sizeof(int);
			s = pStream.Read(a, v);
			mIndexArray->Release(&a);
			if (s != v) return false;
		}
		return KFbxLayerElement::ContentReadFrom(pStream);
	}
	//@}

    typedef Type ArrayElementType;
    typedef KFbxLayerElementArrayTemplate<Type> DirectArrayType;
    typedef KFbxLayerElementArrayTemplate<int> IndexArrayType;

	KFbxLayerElementArrayTemplate<Type>* mDirectArray;
	KFbxLayerElementArrayTemplate<int>*  mIndexArray;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#define CREATE_DECLARE(classDesc) \
    KFBXNEW_DECLARE_FRIEND        \
	static KFbx##classDesc* Create(KFbxLayerContainer* pOwner, char const *pName);

/** \brief Layer element for mapping Normals to a geometry.
  * \nosubgrouping
  */
class KFBX_DLL KFbxLayerElementNormal : public KFbxLayerElementTemplate<KFbxVector4>
{
public:

    /** Allocation method.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	CREATE_DECLARE(LayerElementNormal);
	
protected:
	KFbxLayerElementNormal();
	~KFbxLayerElementNormal();
};

/** \brief Layer element for mapping Binormals to a geometry.
  * \nosubgrouping
  */
class KFBX_DLL KFbxLayerElementBinormal : public KFbxLayerElementTemplate<KFbxVector4>
{
public:

    /** Allocation method.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	CREATE_DECLARE(LayerElementBinormal);
	
protected:
	KFbxLayerElementBinormal();
	~KFbxLayerElementBinormal();
};

/** \brief Layer element for mapping Tangents to a geometry.
  * \nosubgrouping
  */
class KFBX_DLL KFbxLayerElementTangent : public KFbxLayerElementTemplate<KFbxVector4>
{
public:

    /** Allocation method.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	CREATE_DECLARE(LayerElementTangent);
	
protected:
	KFbxLayerElementTangent();
	~KFbxLayerElementTangent();
};

/** Layer element for mapping materials (KFbxSurfaceMaterial) to a geometry.
  *
  * FBX SDK 2011 and later connects materials (KFbxSurfaceMaterial) to nodes (KFbxNode).
  * The direct array of this class is no longer used.
  * The integer "n" in the index array of this class represents the n-th material (zero-based) connected to the node.
  *
  * For example:
  *
  *   Mapping mode eALL_SAME and index array {0} means the whole geometry is assigned with the 0-th material
  *   connected to the node.
  *
  *   Mapping mode eBY_POLYGON and index array {0, 1} means the first polygon is assigned with the 0-th material and
  *   the second polygon is assigned with the 1-th material.
  * 
  * 
  * You can access the materials from a node by using KFbxNode::GetMaterialCount() and KFbxNode::GetMaterial(int pIndex)
  * or the more generic calls to GetSrcObjectCount(FBX_TYPE(KFbxSurfaceMaterial)) and 
  * GetSrcObject(FBX_TYPE(KFbxSurfaceMaterial), index)
  *
  * For example:
  *
  * \code
  * KFbxNode* node;
  * int nbMat = node->GetMaterialCount();
  * int nbMat1= node->GetSrcObjectCount(FBX_TYPE(KFbxSurfaceMaterial));
  *
  * KFbxSurfaceMaterial* material;
  * KFbxLayerElementMaterial* layerElement;
  * if (layerElement->GetMappingMode() == KFbxLayerElement::eALL_SAME)
  * {
  *     int index = layerElement->GetIndexArray()[0];
  *     material = node->GetMaterial(index);
  * }
  * \endcode
  *
  * \remarks
  * The DirectArray() methods still exist for legacy reasons but has been made private and should not be used.
  * \see KFbxSurfaceMaterial
  * \see KFbxNode
  */
class KFBX_DLL KFbxLayerElementMaterial : public KFbxLayerElementTemplate<KFbxSurfaceMaterial*>
{
public:
	typedef KFbxLayerElementTemplate<KFbxSurfaceMaterial*> ParentClass;

    /** Allocation method.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	CREATE_DECLARE(LayerElementMaterial);
	
    /** \internal
      * Internal class to maintain backward compatibility with old FBX code (prior to FBX SDK 2011).
      * This class synchronizes the direct array with KFbxNode connections.
      * Thus, changes on the direct array will reflect on KFbxNode.
      */
	class LayerElementArrayProxy : public KFbxLayerElementArrayTemplate<KFbxSurfaceMaterial*>
	{
	public:
		typedef KFbxLayerElementArrayTemplate<KFbxSurfaceMaterial*> ParentClass;

		LayerElementArrayProxy(EFbxType pType);
		void SetContainer( KFbxLayerContainer* pContainer, int pInstance = 0);
	};

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

	virtual void AllocateArrays();
	virtual void SetOwner( KFbxLayerContainer* pOwner, int pInstance = 0);
	virtual void SetInstance( int pInstance ) { SetOwner( mOwner, pInstance ); }

protected:
	KFbxLayerElementMaterial();
	~KFbxLayerElementMaterial();

private:
    KFbxLayerElementArrayTemplate<KFbxSurfaceMaterial*>& GetDirectArray() const
	{ 
        return ParentClass::GetDirectArray();
	}

    KFbxLayerElementArrayTemplate<KFbxSurfaceMaterial*>& GetDirectArray()
	{ 
        return ParentClass::GetDirectArray();
	}
    
    friend class KFbxLayerContainer;

#endif //DOXYGEN_SHOULD_SKIP_THIS
}; 

/** \brief Layer element for grouping related polygons together.
  * \nosubgrouping
  */
class KFBX_DLL KFbxLayerElementPolygonGroup : public KFbxLayerElementTemplate<int>
{
public:

    /** Allocation method.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	CREATE_DECLARE(LayerElementPolygonGroup);
	
protected:
	KFbxLayerElementPolygonGroup();
	~KFbxLayerElementPolygonGroup();
};

/** \brief Layer element for mapping UVs to a geometry.
  *
  * This class represents a UV set belongs to a geometry. Each UV set in a geometry
  * has a name to identify itself. The string property KFbxTexture.UVSet indicates
  * the UV set to use.
  *
  * \see KFbxTexture
  * \nosubgrouping
  */
class KFBX_DLL KFbxLayerElementUV : public KFbxLayerElementTemplate<KFbxVector2>
{
public:
    /** Allocation method.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	CREATE_DECLARE(LayerElementUV);
	
protected:
	KFbxLayerElementUV();
	~KFbxLayerElementUV();
};

/** \brief Layer element for mapping Vertex Colors to a geometry.
  * \nosubgrouping
  */
class KFBX_DLL KFbxLayerElementVertexColor : public KFbxLayerElementTemplate<KFbxColor>
{
public:

    /** Allocation method.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	CREATE_DECLARE(LayerElementVertexColor);
	
protected:
	KFbxLayerElementVertexColor();
	~KFbxLayerElementVertexColor();
};

/** Flag to indicate no direct array mapping for an index array element
 */
const int kNoMapping = -1;

template <class T> inline KFbxLayerElementArrayTemplate<T>&       KFbxGetDirectArray(KFbxLayerElementUserData       *pLayerElement, int pIndex, bool* pStatus = NULL);
template <class T> inline KFbxLayerElementArrayTemplate<T> const& KFbxGetDirectArray(KFbxLayerElementUserData const *pLayerElement, int pIndex, bool* pStatus = NULL);
template <class T> inline KFbxLayerElementArrayTemplate<T>&       KFbxGetDirectArray(KFbxLayerElementUserData       *pLayerElement, const char* pName, bool* pStatus = NULL );
template <class T> inline KFbxLayerElementArrayTemplate<T> const& KFbxGetDirectArray(KFbxLayerElementUserData const *pLayerElement, const char* pName, bool* pStatus = NULL );

/** \brief Layer element for mapping custom user data to a geometry. 
  * This layer element is different from the other types of layer elements in that it has multiple direct arrays. There is one array for each user data attribute.
  * Each array is indexed by the index array.
  * \nosubgrouping
  */
class KFBX_DLL KFbxLayerElementUserData : public KFbxLayerElementTemplate<void*>
{
public:
    KFBXNEW_DECLARE_FRIEND

    /** Allocation method.
      * \param pOwner       The owner of this layer element.       
      * \param pName        The layer element name.
      * \param pId          The layer element ID.
      * \param pDataTypes   Attribute data types of this layer element, one direct array is allocated for each Attribute data type.
      * \param pDataNames   Attribute names of this layer element.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      * \remarks            Only "bool", "int", "float" and "double" are supported. 
      */
	static KFbxLayerElementUserData* Create(KFbxLayerContainer* pOwner, char const *pName, int pId, KArrayTemplate<KFbxDataType>& pDataTypes, KArrayTemplate<const char*>& pDataNames);

    /** Allocation method.
      * \param pOwner       The owner of this layer element.        
      * \param pOther       Other layer element from which to copy. 
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	static KFbxLayerElementUserData* Create(KFbxLayerContainer* pOwner, KFbxLayerElementUserData const& pOther );

    /** Returns the direct array with the specified attribute index.
	  * \param pIndex                   Specified attribute index.
	  * \param pStatus                  A flag to indicate whether the direct array is returned successfully or not.
	  * \return                         The specified attribute's direct array.
	  */ 
	KFbxLayerElementArrayTemplate<void*>* GetDirectArrayVoid( int pIndex, bool* pStatus = NULL)
	{		
		if( pIndex >= 0 || pIndex < GetDirectArray().GetCount() )
		{
			if (pStatus) *pStatus = true;
			return (KFbxLayerElementArrayTemplate<void*>*)GetDirectArray().AsReference(pIndex);
		}
		else
		{
			if( pStatus ) *pStatus = false;
			K_ASSERT_MSG_NOW("Index out of bounds");
			return (KFbxLayerElementArrayTemplate<void*>*)NULL;
		}
	}

    /** Returns the direct array with the specified attribute index.
	  * \param pIndex                   Specified attribute index.
	  * \param pStatus                  A flag to indicate whether the direct array is returned successfully or not.
	  * \return                         The specified attribute's direct array.
	  */ 
	KFbxLayerElementArrayTemplate<void*> const* GetDirectArrayVoid( int pIndex, bool* pStatus = NULL) const
	{
		if( pIndex >= 0 || pIndex < GetDirectArray().GetCount() )
		{
			if (pStatus) *pStatus = true;
			return (KFbxLayerElementArrayTemplate<void*>*)GetDirectArray().AsReference(pIndex);
		}
		else
		{
			if( pStatus ) *pStatus = false;
			K_ASSERT_MSG_NOW("Index out of bounds");
			return (KFbxLayerElementArrayTemplate<void*> const*)NULL;
		}
	}


    /** Returns the direct array with the specified attribute name.
	  * \param pName                    Specified attribute name.
	  * \param pStatus                  A flag to indicate whether the direct array is returned successfully or not.
	  * \return                         The specified attribute's direct array.
	  */ 
	KFbxLayerElementArrayTemplate<void *>* GetDirectArrayVoid ( const char* pName, bool* pStatus = NULL )
	{
		KString lName( pName );
		for( int i = 0; i < mDataNames.GetCount(); ++i )
		{
			if( *mDataNames[i] == lName )
				return GetDirectArrayVoid(i, pStatus);
		}

		if (pStatus) *pStatus = false;
		return (KFbxLayerElementArrayTemplate<void *>*)NULL;
	}
 
    /** Returns the direct array with the specified attribute name.
	  * \param pName                    Specified attribute name.
	  * \param pStatus                  A flag to indicate whether the direct array is returned successfully or not.
	  * \return                         The specified attribute's direct array.
	  */ 
	KFbxLayerElementArrayTemplate<void *> const* GetDirectArrayVoid ( const char* pName, bool* pStatus = NULL ) const
	{
		KString lName( pName );
		for( int i = 0; i < mDataNames.GetCount(); ++i )
		{
			if( *mDataNames[i] == lName )
				return GetDirectArrayVoid(i, pStatus);
		}

		if (pStatus) *pStatus = false;
		return (KFbxLayerElementArrayTemplate<void *> const*)NULL;
	}

	/** Returns the data type for the specified index
	 * \param pIndex     The index of the attribute being queried
	 * \return           The data type, or DTNone if pIndex is out of range
	 */
	KFbxDataType GetDataType( int pIndex ) const
	{
		if( pIndex < 0 || pIndex >= mDataTypes.GetCount() )
			return DTNone;

		return mDataTypes[pIndex];
	}

	/** Returns the specified attribute data type.
	 * \param pName     The name of the attribute being queried
	 * \return          The data type, or DTNone if no attribute has the given name
	 */
	KFbxDataType GetDataType( const char* pName ) const
	{
		KString lName( pName );

		for( int i = 0; i < mDataNames.GetCount(); ++i )
		{
			if( *mDataNames[i] == lName )
				return mDataTypes[i];
		}

		return DTNone;
	}

	/** Returns the attribute name at the specified index
	 * \param pIndex     Attribute index
	 * \return           The name, or \c NULL if pIndex is out of range.
	 */
	const char* GetDataName( int pIndex ) const
	{
		if( pIndex >= 0 && pIndex < mDataNames.GetCount() )
			return mDataNames[pIndex]->Buffer();

		return NULL;
	}

	/** Resizes all direct arrays to the specified size. 
	 * \param pSize     The new size of the direct arrays.
	 */
	void ResizeAllDirectArrays( int pSize )
	{
		for( int i = 0; i < GetDirectArray().GetCount(); ++i )
		{
			switch( mDataTypes[i].GetType() )
			{
				case eBOOL1:	KFbxGetDirectArray<bool>(this,i).Resize( pSize )  ; break;
				case eINTEGER1:	KFbxGetDirectArray<int>(this,i).Resize( pSize )   ;	break;
				case eFLOAT1:	KFbxGetDirectArray<float>(this,i).Resize( pSize ) ;	break;
				case eDOUBLE1:	KFbxGetDirectArray<double>(this,i).Resize( pSize );	break;
				//case eDOUBLE3:	GetDirectArray< fbxDouble3 >(i).Resize( pSize );	break;
				//case eDOUBLE4:	GetDirectArray< fbxDouble4 >(i).Resize( pSize );	break;
				//case eDOUBLE44:	GetDirectArray< fbxDouble44>(i).Resize( pSize );	break;  
				default:
					K_ASSERT_MSG_NOW("unknown type" ); break;
			}
		}
	}

	/** Removes a single element at pIndex from every direct array.
	 * \param pIndex     The index of the element to be removed.
	 */
	void RemoveFromAllDirectArrays( int pIndex )
	{
		for( int i = 0; i < GetDirectArray().GetCount(); ++i )
		{
			switch( mDataTypes[i].GetType() )
			{
				case eBOOL1:	KFbxGetDirectArray<bool>(this,i).RemoveAt( pIndex )  ; break;
				case eINTEGER1:	KFbxGetDirectArray<int>(this,i).RemoveAt( pIndex )   ; break;
				case eFLOAT1:	KFbxGetDirectArray<float>(this,i).RemoveAt( pIndex ) ; break;
				case eDOUBLE1:	KFbxGetDirectArray<double>(this,i).RemoveAt( pIndex ); break;
				//case eDOUBLE3:	GetDirectArray< fbxDouble3 >(i).RemoveAt( pIndex );	break;
				//case eDOUBLE4:	GetDirectArray< fbxDouble4 >(i).RemoveAt( pIndex );	break;
				//case eDOUBLE44:	GetDirectArray< fbxDouble44>(i).RemoveAt( pIndex );	break;  
				default:
					K_ASSERT_MSG_NOW("unknown type" ); break;
			}
		}
	}

	/** Returns the direct array count for the attribute at pIndex
	 * \param pIndex     The attribute index
	 * \return           The specified attribute's direct array count.
	 */
	int GetArrayCount( int pIndex ) const 
	{
		if( pIndex >= 0 && pIndex < GetDirectArray().GetCount() )
		{
			switch( mDataTypes[pIndex].GetType() )
			{
				case eBOOL1:	return KFbxGetDirectArray<bool>(this,pIndex).GetCount();
				case eINTEGER1:	return KFbxGetDirectArray<int>(this,pIndex).GetCount();
				case eFLOAT1:	return KFbxGetDirectArray<float>(this,pIndex).GetCount();
				case eDOUBLE1:	return KFbxGetDirectArray<double>(this,pIndex).GetCount();
				//case eDOUBLE3:	return GetDirectArray< fbxDouble3 >(pIndex).GetCount();
				//case eDOUBLE4:	return GetDirectArray< fbxDouble4 >(pIndex).GetCount();
				//case eDOUBLE44:	return GetDirectArray< fbxDouble44>(pIndex).GetCount();
				default:
					K_ASSERT_MSG_NOW("Unknown type" ); break;
			}
		}

		return -1;
	}

	/** Queries the this layer element's ID.
	 * \return     The ID expressed as an int
	 */
	int GetId() const { return mId; }

	/** Returns this layer element's direct array count.
	 * \return     The direct array count expressed as an int.
     * \remarks    This count should be equal to the count of user data attributes.
	 */
	int GetDirectArrayCount() const { return GetDirectArray().GetCount(); }

    /** Assignment operator which performs a deep copy. 
      * \param pOther      Other KFbxLayerElementUserData from which to perform a deep copy.
      * \return            This KFbxLayerElementUserData.
      */
    KFbxLayerElementUserData& operator=( KFbxLayerElementUserData const& pOther )
    {
        if (this == &pOther)
            return *this;

        Clear();

        mId = pOther.mId;
        mDataTypes = pOther.mDataTypes;
        mDataNames.Resize(pOther.mDataNames.GetCount());
        for(int i = 0; i < pOther.mDataNames.GetCount(); ++i)
            mDataNames.SetAt(i,  FbxSdkNew< KString >( *pOther.mDataNames[i] ) );

        Init();
        for(int i = 0; i < pOther.GetDirectArrayCount(); ++i)
        {
            switch (mDataTypes[i].GetType())
            {
            case eBOOL1:
                KFbxGetDirectArray<bool>(this, i) = KFbxGetDirectArray<bool>(&pOther, i);
                break;

            case eINTEGER1:
                KFbxGetDirectArray<int>(this, i) = KFbxGetDirectArray<int>(&pOther, i);
                break;

            case eFLOAT1:
                KFbxGetDirectArray<float>(this, i) = KFbxGetDirectArray<float>(&pOther, i);
                break;

            case eDOUBLE1:
                KFbxGetDirectArray<double>(this, i) = KFbxGetDirectArray<double>(&pOther, i);
                break;

            default:
                K_ASSERT_MSG_NOW("Unknown type" );
                break;
            }
        }

        if ( ( mReferenceMode == KFbxLayerElement::eINDEX || 
               mReferenceMode == KFbxLayerElement::eINDEX_TO_DIRECT) &&
             ( pOther.GetReferenceMode() == KFbxLayerElement::eINDEX || 
               pOther.GetReferenceMode()  == KFbxLayerElement::eINDEX_TO_DIRECT))
        {
            GetIndexArray() = pOther.GetIndexArray();
        }

        return *this;
    }

	/** Removes all data from this layer element.
      * \return             \c True always
	  */
	bool Clear()
	{
		int i;
		const int lCount = GetDirectArray().GetCount();
		KFbxLayerElementArray** directArray = NULL;
		directArray = GetDirectArray().GetLocked(directArray);
		for( i = 0; directArray != NULL && i < lCount; ++i )
		{
			if( directArray[i] )
				FbxSdkDelete(directArray[i]);
		}
		GetDirectArray().Release((void**)&directArray);

		for( i = 0; i < mDataNames.GetCount(); ++i )
		{
			KFBX_SAFE_DELETE_PTR(mDataNames[i]);
		}
		mDataNames.Clear();
		mDataTypes.Clear();

		KFbxLayerElementTemplate<void*>::Clear();

        return true;
	}

	/** Queries the amount of memory used by this
	  * object as well as its content. It does not consider the content pointed.
	  * \return                The amount of memory used.
	  */
	virtual int MemorySize() const
	{
		int size = KFbxLayerElementTemplate<void*>::MemorySize();
		size += sizeof(mId);

        for(int i = 0; i < mDataTypes.GetCount(); i++)
        {
            size += sizeof(mDataTypes[i]);
        }
        size += (mDataNames.GetCount() * sizeof(KString*));

		return size;
	}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
	/**
	  * \name Constructor and Destructor.
	  */
	//@{
		/** Constructs a user data layer element. 
	      * \param pId            An identifier for this UserData layer element
	      * \param pDataTypes     Attribute data types for this layer element
	      * \param pDataNames     Attribute names for this layer element
	      */
	KFbxLayerElementUserData( int pId, KArrayTemplate<KFbxDataType>& pDataTypes, KArrayTemplate<const char*>& pDataNames )
		:
		mId( pId ),
		mDataTypes( pDataTypes )
	{
		K_ASSERT( pDataTypes.GetCount() == pDataNames.GetCount() );
		for( int i = 0; i < pDataNames.GetCount(); ++i )
		{
			mDataNames.Add( FbxSdkNew< KString >( pDataNames[i] ) );
		}
	}

    /** Copy constructor. A deep copy is made.
    * \param pOther Another KFbxLayerElementUserData object to be copied.
    */
    KFbxLayerElementUserData( KFbxLayerElementUserData const& pOther ) : mId(pOther.mId), mDataTypes(pOther.mDataTypes)
    {
        for (int lIndex = 0; lIndex < pOther.mDataNames.GetCount(); ++lIndex)
        {
            mDataNames.Add(FbxSdkNew<KString>(*(pOther.mDataNames[lIndex])));
        }

        SetType(&DTLayerElementUserData);
        AllocateArrays();

        for(int i = 0; i < pOther.GetDirectArrayCount(); ++i)
        {
            switch (mDataTypes[i].GetType())
            {
            case eBOOL1:
                KFbxGetDirectArray<bool>(this, i) = KFbxGetDirectArray<bool>(&pOther, i);
                break;

            case eINTEGER1:
                KFbxGetDirectArray<int>(this, i) = KFbxGetDirectArray<int>(&pOther, i);
                break;

            case eFLOAT1:
                KFbxGetDirectArray<float>(this, i) = KFbxGetDirectArray<float>(&pOther, i);
                break;

            case eDOUBLE1:
                KFbxGetDirectArray<double>(this, i) = KFbxGetDirectArray<double>(&pOther, i);
                break;

            default:
                K_ASSERT_MSG_NOW("Unknown type" );
                break;
            }
        }

        if ( ( mReferenceMode == KFbxLayerElement::eINDEX || 
            mReferenceMode == KFbxLayerElement::eINDEX_TO_DIRECT) &&
            ( pOther.GetReferenceMode() == KFbxLayerElement::eINDEX || 
            pOther.GetReferenceMode()  == KFbxLayerElement::eINDEX_TO_DIRECT))
        {
            GetIndexArray() = pOther.GetIndexArray();
        }
    }

    //!Destructor.
	~KFbxLayerElementUserData()
	{
		Clear();
	}

	//@}
	virtual void AllocateArrays()
	{
		KFbxLayerElementTemplate<void*>::AllocateArrays();
		Init();
	}


private:

	void Init()
	{
	    int i;
		GetDirectArray().Resize( mDataTypes.GetCount() );

		// initialize arrays
		for( i = 0; i < mDataTypes.GetCount(); ++i )
		{
			kReference** dst = NULL;
			dst = GetDirectArray().GetLocked(dst);
			if (dst)
			{
				switch( mDataTypes[i].GetType() )
				{
					case eBOOL1:	dst[i] = (kReference*)FbxSdkNew< KFbxLayerElementArrayTemplate<bool> >(mDataTypes[i].GetType());	break;
					case eINTEGER1:	dst[i] = (kReference*)FbxSdkNew< KFbxLayerElementArrayTemplate<int> >(mDataTypes[i].GetType());	break;
					case eFLOAT1:	dst[i] = (kReference*)FbxSdkNew< KFbxLayerElementArrayTemplate<float> >(mDataTypes[i].GetType());	break;
					case eDOUBLE1:	dst[i] = (kReference*)FbxSdkNew< KFbxLayerElementArrayTemplate<double> >(mDataTypes[i].GetType());	break;
					//case eDOUBLE3:	mDirectArray[i] = new KArrayTemplate< fbxDouble3 >();	break;  
					//case eDOUBLE4:	mDirectArray[i] = new KArrayTemplate< fbxDouble4 >();	break;
					//case eDOUBLE44:	mDirectArray[i] = new KArrayTemplate< fbxDouble44 >();	break;  
					default:
						K_ASSERT_MSG_NOW("Trying to assign an unknown type" ); break;
				}
				GetDirectArray().Release((void**)&dst);
			}
		}
	}

	int mId;
	KArrayTemplate<KFbxDataType> mDataTypes;
	KArrayTemplate<KString*> mDataNames;
  #endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

/** Returns the direct array with the given attribute index. The template type must match the attribute type at pIndex. 
 * \param pLayerElement     The layer element whose direct array to return.
 * \param pIndex            The direct array index
 * \param pStatus           Will be set to \c false if accessing the direct array encounters an error.
 * \return                  If pStatus receives \c true, the direct array at the given index is
 *                          returned. Otherwise the return value is \c undefined.
 */
template <class T>
inline KFbxLayerElementArrayTemplate<T>& KFbxGetDirectArray( KFbxLayerElementUserData *pLayerElement,int pIndex, bool* pStatus)
{
	return *(KFbxLayerElementArrayTemplate<T>*)pLayerElement->GetDirectArrayVoid(pIndex,pStatus);
}

/** Returns the direct array with the given attribute index. The template type must match the attribute type at pIndex. 
 * \param pLayerElement     The layer element whose direct array to return.
 * \param pIndex            The direct array index
 * \param pStatus           Will be set to \c false if accessing the direct array encounters an error.
 * \return                  If pStatus receives \c true, the direct array at the given index is
 *                          returned. Otherwise the return value is \c undefined.
 */
template <class T>
inline KFbxLayerElementArrayTemplate<T> const& KFbxGetDirectArray(KFbxLayerElementUserData const *pLayerElement, int pIndex, bool* pStatus)
{
	return *(KFbxLayerElementArrayTemplate<T> const*)pLayerElement->GetDirectArrayVoid(pIndex,pStatus);
}


/** Returns the direct array with the given attribute name.The template type must match the attribute type with pName. 
 * \param pLayerElement     The layer element whose direct array to return.
 * \param pName             The given attribute name.
 * \param pStatus           Will be set to false if accessing the direct array encounters an error.
 * \return                  If pStatus receives \c true, the direct array at the given index is
 *                          returned. Otherwise the return value is \c undefined. 
 */ 
template <class T>
inline KFbxLayerElementArrayTemplate<T>& KFbxGetDirectArray( KFbxLayerElementUserData *pLayerElement,const char* pName, bool* pStatus )
{
	return *(KFbxLayerElementArrayTemplate<T>*)pLayerElement->GetDirectArrayVoid(pName,pStatus);
}

/** Returns the direct array with the given attribute name.The template type must match the attribute type with pName. 
 * \param pLayerElement     The layer element whose direct array to return.
 * \param pName             The given attribute name.
 * \param pStatus           Will be set to false if accessing the direct array encounters an error.
 * \return                  If pStatus receives \c true, the direct array at the given index is
 *                          returned. Otherwise the return value is \c undefined. 
 */ 
template <class T>
inline KFbxLayerElementArrayTemplate<T> const& KFbxGetDirectArray(KFbxLayerElementUserData const *pLayerElement, const char* pName, bool* pStatus )
{
	return *(KFbxLayerElementArrayTemplate<T> const*)pLayerElement->GetDirectArrayVoid(pName,pStatus);
}


/** Layer element for indicating smoothness of components of a geometry.
  * \nosubgrouping
  */
class KFBX_DLL KFbxLayerElementSmoothing : public KFbxLayerElementTemplate<int>
{
public:
    KFBXNEW_DECLARE_FRIEND

    /** Allocation method.
      * \param pOwner       The owner of this layer element.
      * \param pName        The name of this layer element.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	static KFbxLayerElementSmoothing* Create(KFbxLayerContainer* pOwner, char const *pName);

	/** Sets the Reference Mode.
	  * \param pMode       Specifies the reference mode.
      * \remarks                    Only support eDIRECT.                                         
	  */
	void SetReferenceMode( KFbxLayerElement::EReferenceMode pMode )
	{
		if( pMode != KFbxLayerElement::eDIRECT )
		{
			K_ASSERT_MSG_NOW( "Smoothing layer elements must be direct mapped" );
			return;
		}
	}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

protected:
	KFbxLayerElementSmoothing()
	{
		mReferenceMode = KFbxLayerElement::eDIRECT;
	}
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

/** Layer element for indicating crease of components of a geometry.
  * \nosubgrouping
  */
class KFBX_DLL KFbxLayerElementCrease : public KFbxLayerElementTemplate<double>
{
public:
    KFBXNEW_DECLARE_FRIEND

    /** Allocation method.
      * \param pOwner       The owner of this layer element. 
      * \param pName        The name of this layer element.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	static KFbxLayerElementCrease* Create(KFbxLayerContainer* pOwner, char const *pName);

	/** Sets the Reference Mode.
	  * \param pMode       Specifies the reference mode.
      * \remarks                    Only support eDIRECT.
	  */
	void SetReferenceMode( KFbxLayerElement::EReferenceMode pMode )
	{
		if( pMode != KFbxLayerElement::eDIRECT )
		{
			K_ASSERT_MSG_NOW( "Crease layer elements must be direct mapped" );
			return;
		}
	}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

protected:
	KFbxLayerElementCrease()
	{
		mReferenceMode = KFbxLayerElement::eDIRECT;
	}
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

/** Layer element for indicating hole of polygon of a geometry.
* \nosubgrouping
*/
class KFBX_DLL KFbxLayerElementHole : public KFbxLayerElementTemplate<bool>
{
public:
    KFBXNEW_DECLARE_FRIEND

        /** Allocation method.
        * \param pOwner       The owner of this layer element. 
        * \param pName        The name of this layer element.
        * \return             A pointer to the layer element or \c NULL if creation fails. 
        */
        static KFbxLayerElementHole* Create(KFbxLayerContainer* pOwner, char const *pName);

    /** Sets the Reference Mode.
    * \param pMode       Specifies the reference mode.
    * \remarks                    Only support eDIRECT.
    */
    void SetReferenceMode( KFbxLayerElement::EReferenceMode pMode )
    {
        if( pMode != KFbxLayerElement::eDIRECT )
        {
            K_ASSERT_MSG_NOW( "hole layer elements must be direct mapped" );
            return;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  WARNING!
    //
    //	Anything beyond these lines may not be documented accurately and is 
    // 	subject to change without notice.
    //
    ///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

protected:
    KFbxLayerElementHole()
    {
        mReferenceMode = KFbxLayerElement::eDIRECT;
    }
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

/** Layer element for indicating if specified components are shown/hidden 
 */
class KFBX_DLL KFbxLayerElementVisibility : public KFbxLayerElementTemplate<bool>
{
public:

    /** Allocation method.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	CREATE_DECLARE(LayerElementVisibility);

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
	KFbxLayerElementVisibility();
	~KFbxLayerElementVisibility();
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

/** This class is deprecated.
  * \see KFbxLayerElementTexture
  */
typedef class KFBX_DLL KFbxLayerElementTemplate<KFbxTexture*>  KFbxLayerElementTextureBase;

/** \brief Layer element for mapping Textures to a geometry. This class is deprecated.
  *
  * Deprecated since FBX SDK 2011. Textures (KFbxTexture derived classes) should be connected
  * to material properties.
  *
  * For example:
  *
  * \code
  * KFbxFileTexture*  file;
  * KFbxSurfacePhong* phong;
  * phong->Diffuse.ConnectSrcObject(file);
  * \endcode
  * \see KFbxSurfaceMaterial
  * \nosubgrouping
  */
class KFBX_DLL KFbxLayerElementTexture : public KFbxLayerElementTextureBase
{

public:

    /** Allocation method.
      * \return             A pointer to the layer element or \c NULL if creation fails. 
      */
	CREATE_DECLARE(LayerElementTexture);

	/** \enum EBlendMode      Lets you control how textures are combined when you apply multiple layers of texture to a surface.
	  * - \e eTRANSLUCENT     The new texture layer is transparent (depending on the Alpha value).
	  * - \e eADD             Add the color of the new texture to the previous texture.
	  * - \e eMODULATE        Multiples the color value of the new texture by the color values of all previous layers of texture.
	  * - \e eMODULATE2       Multiples the color value of the new texture by two and then by the color values of all previous layers of texture.
	  * - \e eOVER            Blends the new texture over top of the old texture, according to the new texture's alpha channel.
      * - \e eNORMAL,         The colors of the two layers will not interact in any way, and it will display the full value of the colors in layer 1.
      * - \e eDISSOLVE,       Dissolve makes the lower layer take on the colors of the top layer, and how much depends on the opacity of the upper layer. 
      * - \e eDARKEN,		  Darken compares each pixel value of the upper layer to its counterpart's pixel value of the lower layer and chooses the darker of the two to display.
      * - \e eCOLORBURN,      Color Burn burns in the color of the upper layer with the lower layer. No part of the image will get lighter.
      * - \e eLINEARBURN, 	  Linear Burn works like multiply but the results are more intense.
      * - \e eDARKERCOLOR,    This blend mode simply divides pixel values of one layer with the other.
      * - \e eLIGHTEN,		  Lighten compares the two layers pixel for pixel and uses the lightest pixel value. No part of the image gets darker. 
      * - \e eSCREEN,		  Screen brightens by lightning the lower layer based on the lightness of the upper layer
      * - \e eCOLORDODGE,	  Color Dodge dodges the lower layer with the upper layer, resulting in a lighter image. No part of the image will be darkened.
      * - \e eLINEARDODGE,    Linear Dodge works like screen but with more intense results.
      * - \e eLIGHTERCOLOR,   This blend mode has the opposite effect of the Darker Color mode. It compares all the values in both layers, then displays the lightest values.
      * - \e eSOFTLIGHT,      Soft Light will multiply the dark tones and screen the light tones.
      * - \e eHARDLIGHT,      Hard Light multiplies the dark colors and screens the light colors.
      * - \e eVIVIDLIGHT,     Vivid Light will dodges or burn the lower layer pixels depending on whether the upper layer pixels are brighter or darker than neutral gray. It works on the contrast of the lower layer.
      * - \e eLINEARLIGHT,    Linear Light is the same as Vivid light but it works on the brightness of the lower layer.
      * - \e ePINLIGHT, 	  Pin Light changes the lower layer pixels depending on how bright the pixels are in the upper layer.
      * - \e eHARDMIX,		  Produces either white or black, depending on similarities between A and B.
      * - \e eDIFFERENCE, 	  Difference reacts to the differences between the upper and lower layer pixels.
      * - \e eEXCLUSION, 	  Exclusion uses the darkness of the lower layer to mask the difference between upper and lower layers.
      * - \e eSUBTRACT,       The result color is the foreground color subtracted from the background color. The result color is then applied over the background color using the foreground alpha to define the opacity of the result.
      * - \e eDIVIDE,         This blend mode simply divides pixel values of one layer with the other.
      * - \e eHUE, 			  Hue changes the hue of the lower layer to the hue of the upper layer but leaves brightness and saturation alone.
      * - \e eSATURATION,	  Saturation changes the saturation of the lower layer to the hue of the upper layer but leaves brightness and hue alone.
      * - \e eCOLOR,		  Color changes the hue and saturation of the lower layer to the hue and saturation of the upper layer but leaves luminosity alone.
      * - \e eLUMINOSITY      Luminosity changes the luminosity of the lower layer to the luminosity of the upper layer while leaving hue and saturation the same.
	  * - \e eMAXBLEND        Marks the end of the blend mode enum.
	  */
	typedef enum 
	{
		eTRANSLUCENT,
		eADD,
		eMODULATE,
		eMODULATE2,
        eOVER,
        eNORMAL,		
        eDISSOLVE,
        eDARKEN,			
        eCOLORBURN,
        eLINEARBURN, 	
        eDARKERCOLOR,
        eLIGHTEN,			
        eSCREEN,		
        eCOLORDODGE,
        eLINEARDODGE,
        eLIGHTERCOLOR,
        eSOFTLIGHT,		
        eHARDLIGHT,		
        eVIVIDLIGHT,
        eLINEARLIGHT,
        ePINLIGHT, 		
        eHARDMIX,		
        eDIFFERENCE, 		
        eEXCLUSION, 		
        eSUBTRACT,
        eDIVIDE,
        eHUE, 			
        eSATURATION,		
        eCOLOR,		
        eLUMINOSITY,
		eMAXBLEND
	} EBlendMode;

	/** Sets the way Textures blend between layers.
	  * \param pBlendMode     A valid blend mode.
	  */
	void       SetBlendMode(EBlendMode pBlendMode) { mBlendMode = pBlendMode; }

	/** Sets the transparency level between multiple texture levels.
	  * \param pAlpha     Set to a value between 0.0 and 1.0, where 0.0 is totally transparent and 1.0 is totally opaque.
	  * \remarks          Values smaller than 0.0 are clipped to 0.0, while values greater than 1.0 are clipped to 1.0.
	  */
    void       SetAlpha(double pAlpha)
    {
        if (pAlpha > 1.0)
            mAlpha = 1.0;
        else if (pAlpha < 0.0)
            mAlpha = 0.0;
        else
            mAlpha = pAlpha;
    }

	/** Returns the way Textures blend between layers.
	  * \return     The current Blend Mode.
	  */
	EBlendMode GetBlendMode() const                      { return mBlendMode; } 

	/** Returns the transparency level between multiple levels of textures.
	  * \return     An alpha value between 0.0 and 1.0, where 0.0 is totally transparent and 1.0 is totally opaque.
	  */
	double     GetAlpha() const                          { return mAlpha; }

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

	virtual int MemorySize() const
	{
		int size = KFbxLayerElementTextureBase::MemorySize();
		size += sizeof(mBlendMode);
		size += sizeof(mAlpha);
		return size;
	}

protected:
	/** Constructor
	* By default, textures have a Blend Mode of eTRANSLUCENT,
	* a Reference Mode of eINDEX_TO_DIRECT, and an Alpha value of 1.0.
	*/
	KFbxLayerElementTexture() : mBlendMode(eTRANSLUCENT)
	{
		mReferenceMode = eINDEX_TO_DIRECT;
		mAlpha         = 1.0;
	}

private:
	EBlendMode mBlendMode;
	double     mAlpha;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};


/** KFbxLayer class provides a base for the layering mechanism.
  * 
  * A layer can contain one or more of the following layer elements:
  *      \li Normals
  *      \li Binormals
  *      \li Tangents
  *      \li Materials
  *      \li Polygon Groups
  *      \li UVs
  *      \li Vertex Colors
  *      \li Smoothing informations
  *      \li Vertex Creases
  *      \li Edge Creases
  *      \li Custom User Data
  *      \li Visibilities 
  *      \li Textures (diffuse, ambient, specular, etc.) (deprecated)
  * 
  * A typical layer for a Mesh contains Normals, UVs and Materials. A typical layer for NURBS contains only Materials. 
  * In the case of the NURBS, the NURBS' parameterization is used for the UVs; no UVs should be specified.
  *
  * In most cases, you only need a single layer to describe a geometry. Many applications only support what is defined on the first layer. 
  * Take this into account when you fill the layer. For example, it is legal to define the Layer 0 with the UVs and then
  * define the model's Normals on layer 1. However if you construct a file this way, it may not be imported correctly in other applications.
  * Store the Normals in Layer 0 to avoid problems.
  *
  * Since FBX SDK 2011, Textures are connected to the properties of KFbxSurfaceMaterial derived classes.
  * KFbxLayerElementTexture is no longer used. See the code example in KFbxLayerElementTexture for how to connect a texture.
  *
  * Since FBX SDK 2011, texture layering is achieved by KFbxLayeredTexture. See the code example in KFbxLayeredTexture for how to blend textures.
  *
  * Normally, you can access layer from KFbxLayerContainer like KFbxGeometry.
  * For example,
  * \code
  * KFbxMesh* mesh;
  * KFbxLayer* layer0 = mesh->GetLayer(0);
  * KFbxLayerElementNormal* normals = layer0->GetNormals();
  * \endcode
  *
  * \nosubgrouping
  * \see KFbxLayerElement
  * \see KFbxLayerElementNormal
  * \see KFbxLayerElementBinormal
  * \see KFbxLayerElementTangent
  * \see KFbxLayerElementMaterial
  * \see KFbxLayerElementPolygonGroup
  * \see KFbxLayerElementUV
  * \see KFbxLayerElementVertexColor
  * \see KFbxLayerElementSmoothing
  * \see KFbxLayerElementCrease
  * \see KFbxLayerElementUserData
  * \see KFbxLayerElementHole
  * \see KFbxLayerElementVisibility
  */
class KFBX_DLL KFbxLayer
{

public:
    KFBXNEW_DECLARE_FRIEND

	/**
	  * \name Layer Element Management
	  */
	//@{

	/** Returns this layer's Normals description .
	  * \return      A pointer to the Normals layer element, or \c NULL if no Normals layer element is defined in this layer.
	  * \remarks     KFbxNurb or KFbxPatch geometry should not have Normals defined.
	  */
	KFbxLayerElementNormal* GetNormals();	

	/** Returns this layer's Normals description .
	  * \return      A pointer to the Normals layer element, or \c NULL if no Normals layer element is defined in this layer.
	  * \remarks     KFbxNurb or KFbxPatch geometry should not have Normals defined.
	  */
	KFbxLayerElementNormal const* GetNormals() const;

    /** Returns this layer's Tangents description.
      * \return      A pointer to the Tangents layer element, or \c NULL if no Tangents layer element is defined in this layer.
      * \remarks     KFbxNurb or KFbxPatch geometry should not have Tangents defined.
      */
    KFbxLayerElementTangent* GetTangents();	

    /** Returns this layer's Tangents description.
      * \return      A pointer to the Tangents layer element, or \c NULL if no Tangents layer element is defined in this layer.
      * \remarks     KFbxNurb or KFbxPatch geometry should not have Tangents defined.
      */
    KFbxLayerElementTangent const* GetTangents() const;

    /** Returns this layer's Binormals description.
      * \return      A pointer to the Binormals layer element, or \c NULL if no Binormals layer element is defined in this layer.
      * \remarks     KFbxNurb or KFbxPatch geometry should not have Binormals defined.
      */
    KFbxLayerElementBinormal* GetBinormals();	

    /** Returns this layer's Binormals description.
      * \return      A pointer to the Binormals layer element, or \c NULL if no Binormals layer element is defined in this layer.
      * \remarks     KFbxNurb or KFbxPatch geometry should not have Binormals defined.
      */
    KFbxLayerElementBinormal const* GetBinormals() const;

	/** Returns this layer's Materials description.
	  * \return     A pointer to the Materials layer element, or \c NULL if no Materials layer element is defined in this layer.
	  */
	KFbxLayerElementMaterial* GetMaterials();

	/** Returns this layer's Materials description.
	  * \return     A pointer to the Materials layer element, or \c NULL if no Materials layer element is defined in this layer.
	  */
	KFbxLayerElementMaterial const* GetMaterials() const;

	/** Returns this layer's Polygon Groups description.
	  * \return     A pointer to the Polygon Groups layer element, or \c NULL if no Polygon Groups layer element is defined in this layer.
	  */
	KFbxLayerElementPolygonGroup* GetPolygonGroups();

	/** Returns this layer's Polygon Groups description.
	  * \return     A pointer to the Polygon Groups layer element, or \c NULL if no Polygon Groups layer element is defined in this layer.
	  */
	KFbxLayerElementPolygonGroup const* GetPolygonGroups() const;

	/** Returns this layer's UV description.
	  * \param pTypeIdentifier          Layer element type identifier, should be a texture type identifier.  
	  * \return                         A pointer to the UVs layer element, or \c NULL if no UV is defined in this layer.
	  * \remarks                        KFbxNurb or KFbxPatch geometry should not have UVs defined. 
	  *                                 The NURBS/Patch parameterization is used as UV parameters to map a texture.
	  */
	KFbxLayerElementUV* GetUVs(KFbxLayerElement::ELayerElementType pTypeIdentifier=KFbxLayerElement::eDIFFUSE_TEXTURES);

	/** Returns this layer's UV description.
	  * \param pTypeIdentifier          Layer element type identifier, should be a texture type identifier.  
	  * \return                         A pointer to the UVs layer element, or \c NULL if no UV is defined in this layer.
	  * \remarks                        KFbxNurb or KFbxPatch geometry should not have UVs defined. 
	  *                                 The NURBS/Patch parameterization is used as UV parameters to map a texture.
	  */
	KFbxLayerElementUV const* GetUVs(KFbxLayerElement::ELayerElementType pTypeIdentifier=KFbxLayerElement::eDIFFUSE_TEXTURES) const;


	/** Returns the number of different UV sets in this layer.
	  */
	int GetUVSetCount() const;
	
	/** Returns an array that describes which UV sets are in this layer.
	  */
	KArrayTemplate<KFbxLayerElement::ELayerElementType> GetUVSetChannels() const;

	/** Returns an array of UV sets in this layer.
	  */
	KArrayTemplate<KFbxLayerElementUV const*> GetUVSets() const;

	/** Returns this layer's Vertex Colors description.
	  * \return      A pointer to the Vertex Colors layer element, or \c NULL if no Vertex Color layer element is defined in this layer.
	  * \remarks     KFbxNurb or KFbxPatch geometry should not have Vertex Colors defined, since no vertex exists.
	  */
	KFbxLayerElementVertexColor* GetVertexColors();

	/** Returns this layer's Vertex Colors description.
	  * \return      A pointer to the Vertex Colors layer element, or \c NULL if no Vertex Color layer element is defined in this layer.
	  * \remarks     KFbxNurb or KFbxPatch geometry should not have Vertex Colors defined, since no vertex exists.
	  */
	KFbxLayerElementVertexColor const* GetVertexColors() const;

	/** Returns this layer's Smoothing description.
	  * \return      A pointer to the Smoothing layer element, or \c NULL if no Smoothing layer element is defined in this layer.
	  * \remarks     KFbxNurb or KFbxPatch geometry should not have Smoothing defined.
	  */
	KFbxLayerElementSmoothing* GetSmoothing();

	/** Returns this layer's Smoothing description.
	  * \return      A pointer to the Smoothing layer element, or \c NULL if no Smoothing layer element is defined in this layer.
	  * \remarks     KFbxNurb or KFbxPatch geometry should not have Smoothing defined.
	  */
	KFbxLayerElementSmoothing const* GetSmoothing() const;

	/** Returns this layer's vertex crease description.
	  * \return      A pointer to the Crease layer element, or \c NULL if no Crease layer element is defined in this layer.
	  * \remarks     Crease info should only be defined when the geometry is KFbxSubdiv.
	  */
	KFbxLayerElementCrease* GetVertexCrease();

	/** Returns this layer's vertex crease description.
	  * \return      A pointer to the Crease layer element, or \c NULL if no Crease layer element is defined in this layer.
	  * \remarks     Crease info should only be defined when the geometry is KFbxSubdiv.
	  */
	KFbxLayerElementCrease const* GetVertexCrease() const;

    /** Returns this layer's edge crease description.
      * \return      A pointer to the Crease layer element, or \c NULL if no Crease layer element is defined in this layer.
      * \remarks     Crease info should only be defined when the geometry is KFbxSubdiv.
      */
    KFbxLayerElementCrease* GetEdgeCrease();

    /** Returns this layer's edge crease description.
      * \return      A pointer to the Crease layer element, or \c NULL if no Crease layer element is defined in this layer.
      * \remarks     Crease info should only be defined when the geometry is KFbxSubdiv.
      */
    KFbxLayerElementCrease const* GetEdgeCrease() const;

    /** Returns this layer's Hole description.
    * \return      A pointer to the Hole layer element, or \c NULL if no Hole layer element is defined in this layer.
    * \remarks     Hole info should only be defined when the geometry is KFbxMesh.
    */
    KFbxLayerElementHole* GetHole();

    /** Returns this layer's Hole description.
    * \return      A pointer to the Hole layer element, or \c NULL if no Hole layer element is defined in this layer.
    * \remarks     Hole info should only be defined when the geometry is KFbxMesh.
    */
    KFbxLayerElementHole const* GetHole() const;

	/** Returns this layer's User Data.
	  * \return     A pointer to the User Data layer element, or \c NULL if no User Data layer element is defined in this layer.
	  */
	KFbxLayerElementUserData* GetUserData();

	/** Returns this layer's User Data.
	  * \return     A pointer to the User Data layer element, or \c NULL if no User Data layer element is defined in this layer.
	  */
	KFbxLayerElementUserData const* GetUserData() const;

	/** Returns this layer's visibility.
	  * \return     A pointer to the visibility layer element, or \c NULL if no visibility layer element is defined in this layer.
	  */
	KFbxLayerElementVisibility* GetVisibility();

	/** Returns this layer's visibility.
	  * \return     A pointer to the visibility layer element, or \c NULL if no visibility layer element is defined in this layer.
	  */
	KFbxLayerElementVisibility const* GetVisibility() const;

	/** Returns this layer's Textures description.
	  * \param pType            Layer element type, should be a texture type identifier.
	  * \return                 A pointer to the Textures layer element, or \c NULL if no Textures layer element is defined in this layer.
	  */
	KFbxLayerElementTexture* GetTextures(KFbxLayerElement::ELayerElementType pType);

	/** Returns this layer's Textures description.
	  * \param pType            Layer element type, should be a texture type identifier.
	  * \return                 A pointer to the Textures layer element, or \c NULL if no Textures layer element is defined in this layer.
	  */
	KFbxLayerElementTexture const* GetTextures(KFbxLayerElement::ELayerElementType pType) const;

	/** Sets this layer's Textures description.
	  * \param pType            Texture type identifier.
	  * \param pTextures        A pointer to the Textures layer element, or \c NULL to remove the Textures definition.
	  */
    void SetTextures(KFbxLayerElement::ELayerElementType pType, KFbxLayerElementTexture* pTextures);

	/** Returns the specified type of layer element description for this layer.
	  * \param pType            The required Layer element type. 
	  *                             - Calling with eNORMAL is the equivalent of calling GetNormals().
      *                             - Calling with eBINORMAL is the equivalent of calling GetBinormals().
      *                             - Calling with eTANGENT is the equivalent of calling GetTangents().
	  *                             - Calling with eMATERIAL is the equivalent of calling GetMaterials().
	  *                             - Calling with ePOLYGON_GROUP is the equivalent of calling GetPolygonGroups().
	  *                             - Calling with eUV is the equivalent of calling GetUVs().
	  *                             - Calling with eVERTEX_COLOR is the equivalent of calling GetVertexColors().
	  *                             - Calling with eSMOOTHING is the equivalent of calling GetSmoothing().
      *                             - Calling with eVERTEX_CREASE is the equivalent of calling GetVertexCrease().
      *                             - Calling with eEDGE_CREASE is the equivalent of calling GetEdgeCrease().
	  *                             - Calling with eUSER_DATA is the equivalent of calling GetUserData().
      *                             - Calling with eVISIBILITY is the equivalent of calling GetVisibility().
      *                             - Calling with texture type is the equivalent of calling GetTextures(KFbxLayerElement::ELayerElementType pType).
      * \param pIsUV            If \c true, requests the UV layer element that corresponds with the specified texture type.
	  * \return                 A pointer to the requested layer element, or \e NULL if the layer element is not defined in this layer.
	  */
	KFbxLayerElement* GetLayerElementOfType(KFbxLayerElement::ELayerElementType pType, bool pIsUV=false);

	/** Returns the specified type of layer element description for this layer.
	  * \param pType            The required Layer element type. 
	  *                             - Calling with eNORMAL is the equivalent of calling GetNormals().
      *                             - Calling with eBINORMAL is the equivalent of calling GetBinormals().
      *                             - Calling with eTANGENT is the equivalent of calling GetTangents().
	  *                             - Calling with eMATERIAL is the equivalent of calling GetMaterials().
	  *                             - Calling with ePOLYGON_GROUP is the equivalent of calling GetPolygonGroups().
	  *                             - Calling with eUV is the equivalent of calling GetUVs().
	  *                             - Calling with eVERTEX_COLOR is the equivalent of calling GetVertexColors().
	  *                             - Calling with eSMOOTHING is the equivalent of calling GetSmoothing().
      *                             - Calling with eVERTEX_CREASE is the equivalent of calling GetVertexCrease().
      *                             - Calling with eEDGE_CREASE is the equivalent of calling GetEdgeCrease().
	  *                             - Calling with eUSER_DATA is the equivalent of calling GetUserData().
      *                             - Calling with eVISIBILITY is the equivalent of calling GetVisibility().
      *                             - Calling with texture type is the equivalent of calling GetTextures(KFbxLayerElement::ELayerElementType pType).
      * \param pIsUV            If \c true, requests the UV layer element that corresponds with the specified texture type.
	  * \return                 A pointer to the requested layer element, or \e NULL if the layer element is not defined in this layer.
	  */
	KFbxLayerElement const* GetLayerElementOfType(KFbxLayerElement::ELayerElementType pType, bool pIsUV=false) const;

	/** Sets this layer's Normals description.
	  * \param pNormals         A pointer to the Normals layer element, or \c NULL to remove the Normals definition.
	  * \remarks                KFbxNurb or KFbxPatch geometry should not have Normals defined.
	  */
	void SetNormals(KFbxLayerElementNormal* pNormals);

    /** Sets this layer's Binormals description.
      * \param pBinormals       A pointer to the Binormals layer element, or \c NULL to remove the Binormals definition.
      * \remarks                KFbxNurb or KFbxPatch geometry should not have Binormals defined.
      */
    void SetBinormals(KFbxLayerElementBinormal* pBinormals);

    /** Sets this layer's Tangents description.
      * \param pTangents        A pointer to the Tangents layer element, or \c NULL to remove the Tangents definition.
      * \remarks                KFbxNurb or KFbxPatch geometry should not have Tangents defined.
      */
    void SetTangents(KFbxLayerElementTangent* pTangents);

	/** Sets this layer's Materials description.
	  * \param pMaterials       A pointer to the Materials layer element, or \c NULL to remove the Material definition.
	  */
	void SetMaterials(KFbxLayerElementMaterial* pMaterials);

	/** Sets this layer's Polygon Groups description.
	  * \param pPolygonGroups     A pointer to the Polygon Groups layer element, or \c NULL to remove the Polygon Group definition.
	  */
	void SetPolygonGroups(KFbxLayerElementPolygonGroup* pPolygonGroups);

	/** Sets this layer's UVs description.
	  * \param pUVs             A pointer to the UVs layer element, or \c NULL to remove the UV definition.
	  * \param pTypeIdentifier  Layer element type, should be texture type.
	  * \remarks                KFbxNurb or KFbxPatch geometry should not have UVs defined.
	  *                         The NURBS/Patch parameterization is used as UV parameters to map a texture.
	  */
	void SetUVs(KFbxLayerElementUV* pUVs, KFbxLayerElement::ELayerElementType pTypeIdentifier=KFbxLayerElement::eDIFFUSE_TEXTURES);

	/** Sets this layer's Vertex Colors description.
	  * \param pVertexColors     A pointer to the Vertex Colors layer element, or \c NULL to remove the Vertex Color definition.
	  * \remarks                 KFbxNurb or KFbxPatch geometry should not have Vertex Colors defined, since no vertex exists.
	  */
	void SetVertexColors (KFbxLayerElementVertexColor* pVertexColors);

	/** Sets this layer's Smoothing description.
	  * \param pSmoothing       A pointer to the Smoothing layer element, or \c NULL to remove the Smoothing definition.
	  * \remarks                KFbxNurb or KFbxPatch geometry should not have Smoothing defined.
	  */
	void SetSmoothing (KFbxLayerElementSmoothing* pSmoothing);

	/** Sets this layer's Vertex Crease description.
	  * \param pCrease          A pointer to the Vertex Crease layer element, or \c NULL to remove the Crease definition.
	  * \remarks                Crease should only be defined when the geometry is KFbxSubdiv.
	  */
	void SetVertexCrease (KFbxLayerElementCrease* pCrease);

    /** Sets this layer's Edge Crease description.
      * \param pCrease          A pointer to the Edge Crease layer element, or \c NULL to remove the Crease definition.
      * \remarks                Crease should only be defined when the geometry is KFbxSubdiv.
      */
    void SetEdgeCrease (KFbxLayerElementCrease* pCrease);

    /** Sets this layer's Hole description.
    * \param pHole          A pointer to the Hole layer element, or \c NULL to remove the Hole definition.
    * \remarks                Hole should only be defined when the geometry is KFbxMesh.
    */
    void SetHole (KFbxLayerElementHole* pHole);

	/** Sets this layer's User Data.
	  * \param pUserData        A pointer to the User Data layer element, or \c NULL to remove the User Data.
	  */
	void SetUserData (KFbxLayerElementUserData* pUserData);

	/** Sets  this layer's the visibility.
	  * \param pVisibility      A pointer to the visibility layer element, or \c NULL to remove the visibility.
	  */
	void SetVisibility( KFbxLayerElementVisibility* pVisibility );

    /** Sets the specified type of layer element description for this layer.
      * \param pLayerElement    A pointer to the layer element, or \c NULL to remove the layer element.
	  * \param pType            The required Layer element type. 
	  *                             - Calling with eNORMAL is the equivalent of calling GetNormals().
      *                             - Calling with eBINORMAL is the equivalent of calling GetBinormals().
      *                             - Calling with eTANGENT is the equivalent of calling GetTangents().
	  *                             - Calling with eMATERIAL is the equivalent of calling GetMaterials().
	  *                             - Calling with ePOLYGON_GROUP is the equivalent of calling GetPolygonGroups().
	  *                             - Calling with eUV is the equivalent of calling GetUVs().
	  *                             - Calling with eVERTEX_COLOR is the equivalent of calling GetVertexColors().
	  *                             - Calling with eSMOOTHING is the equivalent of calling GetSmoothing().
      *                             - Calling with eVERTEX_CREASE is the equivalent of calling GetVertexCrease().
      *                             - Calling with eEDGE_CREASE is the equivalent of calling GetEdgeCrease().
	  *                             - Calling with eUSER_DATA is the equivalent of calling GetUserData().
      *                             - Calling with eVISIBILITY is the equivalent of calling GetVisibility().
      *                             - Calling with texture type is the equivalent of calling GetTextures(KFbxLayerElement::ELayerElementType pType).
      * \param pIsUV            If \c true, requests the UV layer element that corresponds with the specified texture type.
	  */
	void SetLayerElementOfType(KFbxLayerElement* pLayerElement, KFbxLayerElement::ELayerElementType pType, bool pIsUV=false);

	/** Creates the specified type of layer element description for this layer.
	  * \param pType            The required Layer element type. 
      * \param pIsUV            When \c true, requests the UV LayerElement that corresponds with the specified Layer Element type (only applies to
	  *                         TEXTURE type layer elements).
	  * \return                 A pointer to the newly created layer element, or \e NULL if the layer element has not been created for this layer.
	  */
	KFbxLayerElement* CreateLayerElementOfType(KFbxLayerElement::ELayerElementType pType, bool pIsUV=false);

    /** Clone function.
	  * \param pSrcLayer        The source layer to be cloned.
	  * \param pSdkManager
	  */
	void Clone(KFbxLayer const& pSrcLayer, KFbxSdkManager* pSdkManager);	
	
	
///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
	//! Assignment operator.
	KFbxLayer& operator=(KFbxLayer const& pSrcLayer);
	//@}	
private:

	KFbxLayer(KFbxLayerContainer& pOwner);
	virtual ~KFbxLayer();

	void Clear();

	KFbxLayerContainer& mOwner;

    KFbxLayerElement*             mNonTexturesArray[KFbxLayerElement::LAYERELEMENT_TYPE_NON_TEXTURE_COUNT];
    KFbxLayerElementUV*           mUVsArray[KFbxLayerElement::LAYERELEMENT_TYPE_TEXTURE_COUNT];
    KFbxLayerElementTexture*      mTexturesArray[KFbxLayerElement::LAYERELEMENT_TYPE_TEXTURE_COUNT];


	friend class KFbxLayerContainer;

public:
	/**
	  * \name Serialization section
	  */
	//@{
		bool ContentWriteTo(KFbxStream& pStream) const;
		bool ContentReadFrom(const KFbxStream& pStream);
	//@}
	virtual int MemoryUsage() const;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

/**
  *  Utility macro for iterating over texture layer elements
  */
#define FOR_EACH_TEXTURE(lLayerIndex) for((lLayerIndex) = 0; (lLayerIndex) < KFbxLayerElement::LAYERELEMENT_TYPE_TEXTURE_COUNT; (lLayerIndex)++)

/**
  *  Utility macro for iterating over non-texture layer elements
  */
#define FOR_EACH_NON_TEXTURE(lLayerIndex) for((lLayerIndex) = 0; (lLayerIndex) < KFbxLayerElement::LAYERELEMENT_TYPE_NON_TEXTURE_COUNT; (lLayerIndex)++)

/**
  *  Utility macro for getting texture layer element index by type
  */
#define TEXTURE_INDEX(ElementType) (int(ElementType) - KFbxLayerElement::LAYERELEMENT_TYPE_TEXTURE_START_INDEX)

/**
  *  Utility macro for getting texture layer element type by index
  */
#define TEXTURE_TYPE(TextureIndex) (KFbxLayerElement::ELayerElementType((TextureIndex) + KFbxLayerElement::LAYERELEMENT_TYPE_TEXTURE_START_INDEX))

/**
  *  Utility macro for getting non-texture layer element index by type
  */
#define NON_TEXTURE_INDEX(ElementType) (int(ElementType) - KFbxLayerElement::LAYERELEMENT_TYPE_NON_TEXTURE_START_INDEX)

/**
  *  Utility macro for getting non-texture layer element type by index
  */
#define NON_TEXTURE_TYPE(Index) (KFbxLayerElement::ELayerElementType((Index) + KFbxLayerElement::LAYERELEMENT_TYPE_NON_TEXTURE_START_INDEX))


/**  Defines geometry element classes.
  *  A geometry element describes how the geometry element is mapped to a geometry surface
  *  and how the mapping information is arranged in memory.
  *  \remarks Geometry elements are independent of layer elements and hide the complexity of layers.
  *  \code
  *  KFbxGeometryElementUV* lUVs = lMesh->GetElementUV("map1");
  *  KFbxGeometryElementUV::DirectArrayType lDirectArray = lUVs->GetDirectArray();
  *  int lDirectUVCount = lDirectArray.GetCount();
  *  KFbxVector2 lFirstUV = lDirectArray[0];
  *  \endcode
  *  \see KFbxGeometryBase
  */
typedef KFbxLayerElement KFbxGeometryElement;
typedef KFbxLayerElementNormal KFbxGeometryElementNormal;
typedef KFbxLayerElementBinormal KFbxGeometryElementBinormal;
typedef KFbxLayerElementTangent KFbxGeometryElementTangent;
typedef KFbxLayerElementMaterial KFbxGeometryElementMaterial;
typedef KFbxLayerElementPolygonGroup KFbxGeometryElementPolygonGroup;
typedef KFbxLayerElementUV KFbxGeometryElementUV;
typedef KFbxLayerElementVertexColor KFbxGeometryElementVertexColor;
typedef KFbxLayerElementUserData KFbxGeometryElementUserData;
typedef KFbxLayerElementSmoothing KFbxGeometryElementSmoothing;
typedef KFbxLayerElementCrease KFbxGeometryElementCrease;
typedef KFbxLayerElementHole KFbxGeometryElementHole;
typedef KFbxLayerElementVisibility KFbxGeometryElementVisibility;


#undef CREATE_DECLARE
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXLAYER_H

