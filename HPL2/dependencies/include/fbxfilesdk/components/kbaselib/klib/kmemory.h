/*!  \file kmemory.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KMEMORY_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KMEMORY_H

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

#include <string.h>

#ifndef KARCH_ENV_MACOSX
	#include <stdlib.h>
#endif

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KMpStack;
class KMpMutex;
#define KMEMORY_DEFAULTALIGNMENT	4

/** Allocation routines
*		Standard allocation
*/
#define kMalloc( pSize )														FbxSdkMalloc( pSize )
#define kRealloc( pOldMemBlock, pNewSize )										FbxSdkRealloc( pOldMemBlock, pNewSize )
#define kFree(pAllocatedMemBlock)												FbxSdkFree(pAllocatedMemBlock)
/** Aligned memory allocation.
*	\param pSize Memory block size.
*	\param pAlignment Alignment window or width.
*	\return the allocated memory block. (Used to free the block).
*/
KFBX_DLL void* kMallocAligned( kSizeT pSize, kUInt pAlignment = KMEMORY_DEFAULTALIGNMENT);

/** Aligned memory reallocation.
*	\param pAlignedPtr Previously aligned block
*	\param pSize Memory block size.
*	\param pAlignment Alignment window or width.
*	\return the reallocated memory block. (Used to free the block).
*/
KFBX_DLL void* kReallocAligned( void* pAlignedPtr, kSizeT pSize, kUInt pAlignment = KMEMORY_DEFAULTALIGNMENT);

/** Aligned memory deallocation. 
  * \param pAlignedPtr
  * \return KFBX_DLL void
  */
KFBX_DLL void kFreeAligned( void* pAlignedPtr );


/** Allocation routines debug
*		Standard allocation
*/
// for debugging portability
#define kMallocDbg( pSize, blockType, filename, linenumber )					FbxSdkMalloc_Debug( pSize, blockType, filename, linenumber )
#define kReallocDbg( pOldMemBlock, pNewSize, blockType, filename, linenumber )	FbxSdkRealloc_Debug( pOldMemBlock, pNewSize, blockType, filename, linenumber )
#define kFreeDbg(pAllocatedMemBlock, blockType, filename, linenumber)			FbxSdkFree_Debug(pAllocatedMemBlock, blockType)

KFBX_DLL void* kMallocAlignedDbg( kSizeT pSize, kUInt pAlignment, int blockType, const char *filename, int linenumber);
KFBX_DLL void* kReallocAlignedDbg( void* pAlignedPtr, kSizeT pSize, kUInt pAlignment, int blockType, const char *filename, int linenumber);
KFBX_DLL void  kFreeAlignedDbg( void* pAlignedPtr, int blockType, const char *filename, int linenumber );

/**Simple Memory Usage Counter
  */
class KFBX_DLL KMemoryUsageCounter {
public:
    KMemoryUsageCounter();
    ~KMemoryUsageCounter();
#ifdef _DEBUG
    kInt64	GetMemoryUsage();
    kInt64	GetMallocCount();
    kInt64	GetReallocCount();
    kInt64	GetFreeCount();
    kInt64	GetMallocMemoryUsage();
    kInt64	GetReallocMemoryUsage();
    kInt64	GetFreeMemoryUsage();

    void CounterFree( void *pData );
    void CounterMalloc( size_t pValue );
    void CounterRealloc( void *pData,size_t pValue );
#else
	// in Release the MemoryUsageCounter is not available
	inline kInt64 GetMemoryUsage()			{ return 0; }
	inline kInt64 GetMallocCount()			{ return 0; }
	inline kInt64 GetReallocCount()			{ return 0; }
	inline kInt64 GetFreeCount()			{ return 0; }
	inline kInt64 GetMallocMemoryUsage()	{ return 0; }
	inline kInt64 GetReallocMemoryUsage()	{ return 0; }
	inline kInt64 GetFreeMemoryUsage()		{ return 0; }

	inline void	CounterFree( void* )				{};
	inline void CounterMalloc( size_t )				{};
	inline void CounterRealloc( void* , size_t )	{};
#endif

    KMemoryUsageCounter operator + (KMemoryUsageCounter& pSrc);
    KMemoryUsageCounter operator - (KMemoryUsageCounter& pSrc);

private:
#ifdef _DEBUG
    kInt64	mMemoryUsage;
    kInt64	mMallocCount;
    kInt64	mMallocMemoryUsage;
    kInt64	mReallocCount;
    kInt64	mReallocMemoryUsage;
    kInt64	mFreeCount;
    kInt64	mFreeMemoryUsage;
#endif
};

/** Simple Memory allocator
  */
class KFBX_DLL KMemoryAllocator : public KMemoryUsageCounter 
{
public:
    explicit KMemoryAllocator(const char *pId){}
    virtual ~KMemoryAllocator();
public:
    virtual	void	Purge() {}

    virtual void *	Malloc	(size_t pSize);
    virtual void *	Calloc	(size_t pCount,size_t pSize);
    virtual void *	Realloc (void* pData,size_t pSize);
    virtual void	Free	(void* pData);
};

#define KMEMORY_DEFAULTALIGNMENT	4

/** A pool of memory blocks of arbitrary size.
  */
class KFBX_DLL KMemoryReusePool : public KMemoryAllocator{

public:
    // Public interface.
    explicit KMemoryReusePool(size_t pSizeRange = 1024 * 128, int pStrideShift = 2, int pAlignmentShift = 0, size_t pTotalAllocatedMax = 0);
    virtual ~KMemoryReusePool();

    // Allocation
public:
    virtual	void	Purge();

    virtual void *	Malloc	(size_t pSize);
    virtual void *	Calloc	(size_t pCount,size_t pSize);
    virtual void *	Realloc (void* pData,size_t pSize);
    virtual void	Free	(void* pData);
protected:
    // Implementation.
    bool		SizeFitsInPool(size_t pSize) const { return (pSize >= GetHeaderSize() && pSize < mSizeRange); }
    bool		PoolIsFull() const { return (0 != mTotalAllocatedMax && mTotalAllocated > mTotalAllocatedMax); }
    kUChar		ComputeAlignmentOffset(void* pBuffer);
    static int	ComputeRightShiftRemainder(int pStrideShift);

protected:
    struct KBlockHeader {
        size_t	mSize;
        kUChar	mAlignmentOffset;
    };
    void*			AssignHeaderAndReturnBlock(void* pBuffer, size_t pBufferSize);
    size_t			GetBufferAndSize(void* pBlock, void* & pBuffer);
    static size_t	GetHeaderSize();

private:
    KMemoryReusePool(KMemoryReusePool const &);
    KMemoryReusePool & operator=(KMemoryReusePool const &);

protected:
    // Fundamental data.
    KMpMutex *		mLock;				//!< Critical section on our pool (maintained by pointer only to not force a dependency on windows.h at large).
    void* *			mBuffer;			//!< Our array of free lists, of size mBufferSize.
    size_t const	mSizeRange;			//!< Maximum alloc size.
    size_t const	mBufferSize;		//!< mSizeRange >> mStrideShift.
    int const		mStrideShift;		//!< Divider for mSizeRange.
    int const		mStrideRemainder;	//!< A mask of the remainder of a right shift done with mStrideShift.
    int const		mAlignmentShift;	//!< Multiplier, to specify an alignment in bytes.
    size_t const	mTotalAllocatedMax; //!< Maximum total amount of memory handed out or managed by this pool (past this point, normal allocs are done).
    size_t			mTotalAllocated;	//!< The total amount of memory handed out or managed by this pool.
    size_t			mTotalBlocks;		//!< The total number of blocks handed out or managed by this pool.
};

/** class KMemory pool
  */
class KFBX_DLL KMemoryPool {
public:
    /** The class constructor.
      * \param pSupportConcurrentAccess Whether the pool supports concurrent allocation and release operations.
      * \param pBlockSize
      * \param pBlockCount
      * \param pAlignment
      * \param pResizable
      */
    explicit KMemoryPool(kSizeT pBlockSize = 0,kUInt pBlockCount = 0,kUInt pAlignment = 0,bool pResizable = true, bool pSupportConcurrentAccess = true);
    ~KMemoryPool();

    /** Initiate the memory pool to the given parameters.
    * \param pBlockSize
    *	\param pBlockCount The blocks size.
    *	\param pAlignment Alignment.
    *	\param pResizable Is it resizable?
    */
    void Init(	kSizeT pBlockSize,kUInt pBlockCount = 0,kUInt pAlignment = 0,bool pResizable = true );

    /** Uninitialized memory pool. */
    void Clear();

    /** Allocate or lock for usage a memory block.
    *	\return a memory block.
    */
    void* Allocate();

    /** Dispose or unlock the usage of a memory block.
    *	Asserting that this pool own this block.
    *	\param pMemBlock 
    */
    void Free( void* pMemBlock );

private:
    void InitVars();
    void* Pop();

private:
    kAtomic		mCountFreeBlock;
    void*   	mFreeBlocks;
    kSizeT		mBlockSize;
    kUInt		mAlignment;
    bool		mResizable;
    bool        mSupportConcurrentAccess;
};



/** Memory leak debugging functions 
  */
#if ( (defined(MEMORY_DEBUG) || defined(MEMORY_DEBUG_FBXFILESDK_MEMORY)) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
KFBX_DLL void KMemoryLeakInit();
KFBX_DLL int KMemoryLeakDumpReport(_CrtMemState*);
#endif

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KMEMORY_H

