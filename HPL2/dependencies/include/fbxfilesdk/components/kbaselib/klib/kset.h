/*!  \file kset.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSET_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSET_H

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

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

    struct SSet;

    // *************************************************************************
    //
    //  class KSet
    //
    // ************************************************************************* 

	/** Class to manipulate set
	* \nosubgrouping
	*/
    class KFBX_DLL KSet {
    public:
        enum
        {
            KFBX_NOT_FOUND        = 0x0L,
            KFBX_ITEM_PER_BLOCK   = 20
        };

	  /**
       * \name Constructors and Destructor
       */
      //@{

		
		/** Int constructor.
		* \param pItemPerBlock The number of items that every block included
		*/
        KSet( int pItemPerBlock = KFBX_ITEM_PER_BLOCK );

		/** Copy constructor.
		* \param other Given object.
		*/
        KSet(const KSet& other);

		//! Destructor.
        ~KSet();
		//@}

	    // Add and remove
		/** If can't find the matching item,append a item at the end of the array.
		* If find the matching item ,insert the new item before the matching item. 
        * \param pReference The value of Reference in new item, also is the character for matching.
		* \param pItem The value of Item in new item.
		* \return If add successfully return true,otherwise return false.
	    */
	    bool  Add		( kReference pReference, kReference pItem );

		
		/** Remove the first matching item, whose reference is the same as given.
		* \param pReference The given reference.
		* \return If remove successfully return true,otherwise return false.
		*/
	    bool  Remove		( kReference pReference );
		
		/** Remove all the matching item, whose item is the same as given.
		* \param pItem The given item.
		* \return If remove successfully return true,otherwise return false.
		*/
	    bool  RemoveItem	( kReference pItem );

        /** Set first matching item with the given parameter.
        * \param pReference The character for matching.
		* \param pItem  The value of Item that the matching item will be set.
		* \return If set successfully return true,otherwise return false.
        */
	    bool  SetItem	( kReference pReference, kReference pItem ); // Change Item Reference, don't Create if doesn't Exist

        /** Get first matching item with the given parameter.
        * \param pReference The character for matching.
		* \param pIndex The pointer to the index of the matching item.
		* \return The value of Item in the matching item.
        * \remarks If there are multiple elements that match the character, the index returned is unspecified.
        */
        kReference Get ( kReference pReference, int* pIndex = NULL ) const;

		//! Delete the array.
	    void	 Clear();

	    // Index manipulation
		/** Get the item of the given index.
        * \param pIndex The index for matching.
		* \param pReference The pointer to the Reference of the matching item.
		* \return The value of Item in the matching item.
        */
	    kReference GetFromIndex ( int pIndex, kReference* pReference = NULL )const;

		/** Remove the item of the given index
		* \param pIndex The given index.
		* \return If remove successfully return true,otherwise return false.
		*/
	    bool  RemoveFromIndex( int pIndex );

	    // Get The Count
		/** Get number of items in the array.
		* \return The number of items in the array.
		*/
	    int	 GetCount ()const		{ return mSetCount; }

	    // Sorting
		/** Swap the value of Reference and Item in every item of array,
		 *  and sort the new array with the value of Reference. 
		 * \return If swap successfully return true,otherwise return false.
		 */
        bool  Swap()const;

		//The array can be sorted only if the private member:mIsChanged be true.
		/** Sort the array according the value of Reference in each item.
		* \return If sort successfully return true,otherwise return false.
		*/
	    bool  Sort()const;

        //! KString assignment operator.
        const KSet& operator=(const KSet&);

    private:
	    // internal functions for Sets manipulation
	    SSet*	FindEqual( kReference pReference)const;

    private:
 	    SSet*	mSetArray;
	    int		mSetCount;
	    int		mBlockCount;
	    int		mItemPerBlock;
	    mutable bool	mIsChanged;
    };

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSET_H

