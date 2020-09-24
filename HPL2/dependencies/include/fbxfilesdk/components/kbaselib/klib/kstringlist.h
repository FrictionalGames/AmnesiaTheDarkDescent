/*!  \file kstringlist.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSTRINGLIST_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSTRINGLIST_H

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

#include <fbxfilesdk/components/kbaselib/isocpp_conformant.h> // RR: for new deprecated functions introduced by VC++ 2005
// We include the file here because we want to make sure that the symbols we have re-defined
// in isocpp_conformat.h correctly overwrite the respective functions.

#include <fbxfilesdk/fbxfilesdk_new.h>

#include <fbxfilesdk/components/kbaselib/klib/t-str.h>
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

    class KFBX_DLL KStringListItem {
    public:
	    KString		mString; 
	    kReference	mReference;
	    KStringListItem( ) { mReference = 0;} 
	    KStringListItem( const char* pString, kReference pRef=0 ) { mString = pString; mReference = pRef;} 
    };

    inline int CompareKStringListSort(const void *E1,const void *E2)
    {
		return t_stricmp((*(KStringListItem **)E1)->mString.Buffer(),(*(KStringListItem **)E2)->mString.Buffer());
    }

	inline int CompareKStringListFindEqual(const void *E1,const void *E2)
    {
		return t_stricmp((*(KStringListItem *)E1).mString.Buffer(),(*(KStringListItem **)E2)->mString.Buffer());
    }

	inline int CompareCaseSensitiveKStringList(const void *E1,const void *E2)
    {
		return strcmp((*(KStringListItem *)E1).mString.Buffer() ,(*(KStringListItem **)E2)->mString.Buffer());
    }

	typedef class KArrayTemplate<KStringListItem *> KArrayStringListItem;

	 
    template <class Type> class KStringListTemplate {
    protected:
	    KArrayTemplate<Type *> mList;

    public:
		/**
         * \name Operation With The Array 
         */
       //@{

		 /** Append a item at the end of the array.
        * \return Index of appended pointer.
        */
	    int		AddItem( Type *pItem )		{ return mList.Add( pItem ); }

	    /** Insert a item in the array.
        * \param pIndex Position where to insert the item.
        * \param pItem  Item to insert.
        * \return Position of the inserted item in the array.
        * \remarks If the given index is out of range, the pointer is appended at the end of the array.
        */
		int		InsertItemAt( int pIndex, Type *pItem )	{ return mList.InsertAt( pIndex, pItem ); }

		//! Access item at given index.
	    Type   *GetItemAt( int pIndex )	const	{ return mList[pIndex]; }

		/** Find first matching item.
        * \return Index of first matching item found or -1 if there is no matching element.
        */
	    int		FindItem( Type *pItem )	const	{ return mList.Find( pItem ); }
		//}@

    public : 
        /**
         * \name Constructor and Destructor
         */
       //@{

		//! Default constructor.
	    KStringListTemplate()
	    {
	    }

		//! Destructor.
	    virtual ~KStringListTemplate() { Clear(); }
		//}@

		//!Remove the item at the end of the array and delete the associated object.
        void RemoveLast() { RemoveAt( mList.GetCount()-1 ); }

		/** Get number of items in the array.
         * \return The number of items in the array.
         */
	    inline int		GetCount() const { return mList.GetCount(); }

		//! Access the string in the item at given index.
        KString   &operator[](int pIndex) { return mList[pIndex]->mString; }

        //! Access the value of reference in the item at given index.
        kReference		GetReferenceAt(int pIndex) const { return mList[pIndex]->mReference; }

        //! Set the value of reference at given index.        
        void			SetReferenceAt(int pIndex, kReference pRef) { mList[pIndex]->mReference = pRef; }

		//! Access the pointer of string at given index.
        char*		GetStringAt(int pIndex) const { if (pIndex<mList.GetCount()) return mList[pIndex]->mString.Buffer(); else return NULL; }
        
		//! Set string at given index.
		virtual bool	SetStringAt(int pIndex, const char* pString) 
	    { 
		    if (pIndex<mList.GetCount()) 
		    {
			    mList[pIndex]->mString = pString; 
			    return true;
		    } else return false; 
	    }
       
	    /** Find first matching item.
        * \return Index of first matching item found or -1 if  there is no
        * matching element.
        */
	    int Find( Type &pItem ) const
	    { 
		    for (int Count=0; Count<mList.GetCount(); Count++) {
			    if (mList[Count]==&pItem) {
				    return Count;
			    }
		    }
		    return -1;
	    }

		/** Find first matching item which has the same reference as given parameter.
        * \return Index of first matching item found or -1 if  there is no
        * matching element.
        */
	    int FindIndex( kReference pReference ) const
	    { 
		    for (int Count=0; Count<mList.GetCount(); Count++) {
			    if (mList[Count]->mReference==pReference) {
				    return Count;
			    }
		    }
		    return -1;
	    }

		/** Find first matching item in array whose string address is the same as given pointer.
        * \return Index of first matching item found or -1 if  there is no
        * matching element.
        */
	    int FindIndex( const char* pString ) const
	    { 
		    for (int Count=0; Count<mList.GetCount(); Count++) {
			    if (mList[Count]->mString==pString) {
				    return Count;
			    }
		    }
		    return -1;
	    }

		/** Access the value of reference of the first matching item in array 
		* whose string address is the same as given pointer.
        * \return The value of reference of the first matching item found or NULL if  there is no
        * matching element.
        */
	    kReference FindReference(const char* pString ) const
	    {
	    int Index = FindIndex( pString );
		    if (Index!=-1) {
			    return mList[Index]->mReference;
		    }
		    return 0; // NULL
	    }

		//! Remove first matching item.
        bool Remove ( Type &pItem )
	    {
	    int Index = Find( pItem );
	        if (Index>=0) {
			    RemoveAt( Index );
			    return true;
		    }
		    return false;
	    }

		//! Remove first matching item in array whose string address is the same as given pointer.
        bool Remove (const char* pString )
	    {
	    int Index = FindIndex( pString );
	        if (Index>=0) {
			    RemoveAt( Index );
			    return true;
		    }
		    return false;
	    }

		//! Remove first matching item.
        bool RemoveIt ( Type &pItem )
	    {
	    int Index = Find( pItem );
	        if (Index>=0) {
			    RemoveAt( Index );
			    return true;
		    }
		    return false;
	    }

		//! Sort the array by the string of every item,not case sensitive.
	    void Sort( )
	    {
		    qsort( &(mList.GetArray()[0]),mList.GetCount(),sizeof(KStringListItem *),CompareKStringListSort );
	    }

	    /** Find first matching item which has the same string as given parameter,not case sensitive.
        * \return the pointer of matching item found or NULL if  there is no
        * matching element.
        */
		void* FindEqual(const char* pString) const
	    {
	    KStringListItem Key(pString);  
        
		    if (mList.GetCount() != 0)
		    {
			    return bsearch ( &Key, &(mList.GetArray()[0]),mList.GetCount(),sizeof(KStringListItem *),CompareKStringListFindEqual );
		    }
		    else
		    {
			    return NULL ;
		    }
	    }

		/** Find first matching item which has the same string as given parameter, case sensitive.
        * \return the pointer of matching item found or NULL if  there is no
        * matching element.
        */
		void* FindCaseSensitive(const char * pString) const
		{
	    KStringListItem Key(pString);  
        
		    if (mList.GetCount() != 0)
		    {
			    return bsearch ( &Key, &(mList.GetArray()[0]),mList.GetCount(),sizeof(KStringListItem *), CompareCaseSensitiveKStringList);
		    }
		    else
		    {
			    return NULL ;
		    }
		
		}


		//! Add a new item at the end of array.
	    int Add( const char* pString, kReference pItem=0 ) 
	    { 
		    return InsertAt( mList.GetCount(),pString,pItem ); 
	    }

	    virtual int InsertAt( int pIndex, const char* pString, kReference pItem=0 ) 
	    { 
		    return mList.InsertAt( pIndex,FbxSdkNew< Type >( pString,(kReference)pItem )); 
	    }

	    /** Remove the item at the given position in the array and delete the associated object.
        * \param pIndex Position of the item to remove.
        * \remarks If the index is not valid, nothing is performed. Otherwise,
        * the item is removed from the array and the items are shifted to fill the
        * empty slot.
        */
		virtual void RemoveAt( int pIndex ) 
	    { 
		    FbxSdkDelete(mList[pIndex]); mList.RemoveAt( pIndex ); 
	    }

		//! Delete the array.
        virtual void Clear()
	    {
		    for (int Count=0; Count<mList.GetCount(); Count++ ) {
			    FbxSdkDelete(mList[Count]);
		    }
		    mList.Clear();
	    }

		/** Get the string of all the item.
        * \return The text of string, each item's string separated by '~'.
        */
       virtual void GetText(KString& pText) const
	    {
		    int		Count;
		    for (Count=0; Count<mList.GetCount(); Count++) 
            {
                pText += mList[Count]->mString;
                if (Count<mList.GetCount()-1) 
                {
                   // HACK: Fix this
                    pText += "~";
                   //------- END HACK
                }
		    }
	    }

        /** Clear the array and set the array's new items with the substring separated by '~' from the given string.
        * \param pList The string which used to generate the new items.
		* \return The last index of the item in the new array.
        * \remarks The number of items in the new array is the same as the number of substrings, 
		* and the string of each item is decided by the content of each substring.
        */ 
	    virtual int SetText(const char* pList)
	    {
  	    int		Pos=0, OldPos = 0;
	    int		LastIndex=0;
  	    KString	Name=pList;

  		    Clear();
    	    for (Pos=0; Name.Buffer()[Pos]!=0; Pos++) {
        	    if (Name.Buffer()[Pos]==_T('~')) {
            	    Name.Buffer()[Pos]=0;
            	    LastIndex = Add(Name.Buffer()+OldPos);
            	    OldPos=Pos+1;
        	    }
		    }

		    if(OldPos != Pos)
		    {
		        LastIndex = Add(Name.Buffer()+OldPos);
		    }
		    return LastIndex;
	    } 


    };

    typedef class KFBX_DLL KStringListTemplate< KStringListItem > KStringListTemplateItem;

    class KFBX_DLL KStringList : public KStringListTemplate< KStringListItem >
    {
    public : 
        /**
         * \name Constructors
         */
       //@{

		//! Default constructor.
	    KStringList(); 

		//! Copy constructor.
	    KStringList( const KStringList &pOriginal );
	  //@}

		/**
         * \name Assignment Operators
         */
        //@{

		//! KStringList assignment function.
	    void CopyFrom( const KStringList *pOriginal  );
    	
	    //! KStringList assignment operator.
		KStringList& operator=(const KStringList& pOriginal);
         //@}
    };
	  

    typedef KStringList *HKStringList;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSTRINGLIST_H

