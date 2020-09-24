/****************************************************************************************
 
   Copyright (C) 2013 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxpropertydef.h
#ifndef _FBXSDK_CORE_PROPERTY_DEFINITION_H_
#define _FBXSDK_CORE_PROPERTY_DEFINITION_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/core/fbxpropertytypes.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

#define FBXSDK_PROPERTY_ID_NULL	-1
#define FBXSDK_PROPERTY_ID_ROOT 0

class FbxPropertyPage;

class FBXSDK_DLL FbxPropertyAttr
{
public:
	enum EInheritType
	{
		eOverride,	//!< Property override this flag from its reference property.
		eInherit,	//!< Property inherit this flag from its reference property.
		eDeleted	//!< Property has been deleted, so inheritance is invalid.
	};

	enum EFlags
	{
		eNone = 0,					//!< No flag.
		eStatic = 1 << 0,			//!< Property is defined in the class declaration, so it wasn't created dynamically.
		eAnimatable = 1 << 1,		//!< Property can be animated, thus is can have am animation curve node connected.
		eAnimated = 1 << 2,			//!< Property is animated, so it also has an animation curve node connected.
		eImported = 1 << 3,			//!< Property has been created during import process when reading FBX file.
		eUserDefined = 1 << 4,		//!< Property has been defined by user, not by the FBX SDK.
		eHidden = 1 << 5,			//!< Property should not be displayed on user interface.
		eNotSavable = 1 << 6,		//!< Property value must not be exported when writing FBX files.

        eLockedMember0 = 1 << 7,	//!< This property has its member #0 locked.
        eLockedMember1 = 1 << 8,	//!< This property has its member #1 locked.
        eLockedMember2 = 1 << 9,	//!< This property has its member #2 locked.
        eLockedMember3 = 1 << 10,	//!< This property has its member #3 locked.
        eLockedAll = eLockedMember0 | eLockedMember1 | eLockedMember2 | eLockedMember3,
        eMutedMember0 = 1 << 11,	//!< This property has its member #0 muted.
        eMutedMember1 = 1 << 12,	//!< This property has its member #1 muted.
        eMutedMember2 = 1 << 13,	//!< This property has its member #2 muted.
        eMutedMember3 = 1 << 14,	//!< This property has its member #3 muted.
        eMutedAll = eMutedMember0 | eMutedMember1 | eMutedMember2 | eMutedMember3,

		//Private flags
		eUIDisabled = 1 << 15,		//!< Private flag for dynamic UI in FBX plug-ins.
		eUIGroup = 1 << 16,			//!< Private flag for dynamic UI in FBX plug-ins.
		eUIBoolGroup = 1 << 17,		//!< Private flag for dynamic UI in FBX plug-ins.
		eUIExpanded = 1 << 18,		//!< Private flag for dynamic UI in FBX plug-ins.
		eUINoCaption = 1 << 19,		//!< Private flag for dynamic UI in FBX plug-ins.
		eUIPanel = 1 << 20,			//!< Private flag for dynamic UI in FBX plug-ins.
		eUILeftLabel = 1 << 21,		//!< Private flag for dynamic UI in FBX plug-ins.
		eUIHidden = 1 << 22,		//!< Private flag for dynamic UI in FBX plug-ins.

		eCtrlFlags = eStatic | eAnimatable | eAnimated | eImported | eUserDefined | eHidden | eNotSavable | eLockedAll | eMutedAll,
		eUIFlags = eUIDisabled | eUIGroup | eUIBoolGroup | eUIExpanded | eUINoCaption | eUIPanel | eUILeftLabel | eUIHidden,
		eAllFlags = eCtrlFlags | eUIFlags,

		eFlagCount = 23,
	};

    static const int sLockedMembersMax       = 4;	//!< Maximum number of property sub-member that can be locked.
    static const int sLockedMembersBitOffset = 7;	//!< Number of bits to shift to get to the first locked member flag.
    static const int sMutedMembersMax        = 4;	//!< Maximum number of property sub-member that can be muted.
    static const int sMutedMembersBitOffset  = 11;	//!< Number of bits to shift to get to the first muted member flag.
};

class FBXSDK_DLL FbxPropertyFlags
{
/*****************************************************************************************************************************
** WARNING! Anything beyond these lines is for internal use, may not be documented and is subject to change without notice! **
*****************************************************************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
public:
	FBX_ASSERT_STATIC(sizeof(FbxUInt32) * 8 >= FbxPropertyAttr::eFlagCount);

	FbxPropertyFlags() : mFlagData(0), mMaskData(0) {}
	explicit FbxPropertyFlags(FbxPropertyAttr::EFlags pFlags) : mFlagData(0), mMaskData(0) { ModifyFlags(pFlags, true); }

	inline FbxPropertyFlags Clone(FbxPropertyPage* /*pPage*/) { return FbxPropertyFlags(); }
	inline bool SetMask(FbxPropertyAttr::EFlags pFlags) { mMaskData |= pFlags; return true; }
	inline bool UnsetMask(FbxPropertyAttr::EFlags pFlags) { mMaskData &= ~pFlags; return true; }
	inline FbxPropertyAttr::EFlags GetMask() const { return (FbxPropertyAttr::EFlags) (mMaskData); }
	inline FbxPropertyAttr::EFlags GetFlags() const { return (FbxPropertyAttr::EFlags) (mFlagData); }

	inline bool ModifyFlags( FbxPropertyAttr::EFlags pFlags, bool pValue )
	{
		SetMask(pFlags);
		mFlagData = pValue ? mFlagData | pFlags : mFlagData & ~pFlags;
		return true;
	}

	inline FbxPropertyAttr::EFlags GetMergedFlags(FbxPropertyAttr::EFlags pFlags) const
	{
		FbxUInt32 lTemp = (FbxUInt32)pFlags & ~(GetMask());
		return (FbxPropertyAttr::EFlags)(lTemp | GetFlags());
	}

	inline FbxPropertyAttr::EInheritType GetFlagsInheritType(FbxPropertyAttr::EFlags pFlags) const
	{
		return ( GetMask() & pFlags ) != 0 ? FbxPropertyAttr::eOverride : FbxPropertyAttr::eInherit;
	}

	inline bool SetFlags(FbxPropertyAttr::EFlags pMask, FbxPropertyAttr::EFlags pFlags)
	{
		FbxUInt32 lFlags = GetFlags() & ~pMask;
		lFlags |= pFlags;
		mFlagData = lFlags;
		return SetMask(pMask);
	}

	inline bool Equal(const FbxPropertyFlags& pOther, FbxPropertyAttr::EFlags pFlags) const
	{
		FbxUInt32 lMyFlags = GetFlags() & pFlags;
		FbxUInt32 lOthersFlags = pOther.GetFlags() & pFlags;
		return lMyFlags == lOthersFlags;
	}

private:
    FbxUInt32 mFlagData, mMaskData;
#endif /* !DOXYGEN_SHOULD_SKIP_THIS *****************************************************************************************/
};

class FBXSDK_DLL FbxPropertyValue
{
/*****************************************************************************************************************************
** WARNING! Anything beyond these lines is for internal use, may not be documented and is subject to change without notice! **
*****************************************************************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
public:
	FbxPropertyValue() : mRef(0), mType(eFbxUndefined), mValue(NULL){}

	static FbxPropertyValue*	Create(void* pData, EFbxType pType){ return FbxNew<FbxPropertyValue>(pData, pType); }
	void						Destroy(){ FbxDelete(this); }
	FbxPropertyValue*			Clone(FbxPropertyPage*){ return FbxNew<FbxPropertyValue>(mValue, mType); }

	inline bool					Get(void* pValue, EFbxType pValueType){ return mValue ? FbxTypeCopy(pValue, pValueType, mValue, mType) : false; }
	inline bool					Set(const void* pValue, EFbxType pValueType){ return mValue ? FbxTypeCopy(mValue, mType, pValue, pValueType) : false; }
	inline size_t				GetSizeOf() const { return FbxTypeSizeOf(mType); }
	inline size_t				GetComponentCount() const { return FbxTypeComponentCount(mType); }

	inline void					IncRef(){ mRef++; }
	inline void					DecRef(){ mRef--; if( mRef == 0 ) FbxDelete(this); }
	inline int					GetRef(){ return mRef; }

private:
	FbxPropertyValue(void* pValue, EFbxType pType) :
		mRef(0),
		mType(pType),
		mValue(0)
	{
		size_t lSize = FbxTypeSizeOf(mType);
		if( lSize )
		{
			mValue = FbxTypeAllocate(mType);
			if( pValue && mType != eFbxReference )
			{
				FbxTypeCopy(mValue, mType, pValue, mType);
			}
		}
	}

	~FbxPropertyValue(){ FbxTypeDeallocate(mType, mValue); }

	int			mRef;
	EFbxType	mType;
	void*		mValue;

	FBXSDK_FRIEND_NEW();
#endif /* !DOXYGEN_SHOULD_SKIP_THIS *****************************************************************************************/
};

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_CORE_PROPERTY_DEFINITION_H_ */
