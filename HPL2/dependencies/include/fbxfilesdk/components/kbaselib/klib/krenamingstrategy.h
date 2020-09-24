/*!  \file krenamingstrategy.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KRENAMINGSTRATEGY_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KRENAMINGSTRATEGY_H

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
#include <fbxfilesdk/components/kbaselib/klib/kname.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** This base class is an abstract implementation of a renaming strategy for avoiding name clashes.
  * An implementation of a reader (KFbxReader) or writer (KFbxWriter) class must call a concrete implementation
  * of "KRenamingStrategy::Rename()" every time a name is imported or exported to avoid name clashes.
  * Any class deriving from KRenamingStrategy must implement KRenamingStrategy::Clear(),
  * KRenamingStrategy::Rename(), and KRenamingStrategy::Clone().
  * \nosubgrouping
  * \see KName KNumberRenamingStrategy
  */
class KFBX_DLL KRenamingStrategy
{
public:
    //! Constructor.
    KRenamingStrategy();

    //! Destructor.
    virtual ~KRenamingStrategy ();

    //! Resets internal state regarding assigned names.
    virtual void Clear() = 0;

    /** Rename a name if necessary to avoid name-clash issues.
      * \param pName The name to be renamed.
      * \return Return \c true on success, \c false otherwise.
      */
    virtual bool Rename(KName& pName) = 0;

    /** Create a dynamic renaming strategy instance of the same type as the child class.
      * \return New instance.
      */
    virtual KRenamingStrategy* Clone() = 0;
};

/** Implements a renaming strategy that resolves name clashes by adding number postfixes.
  * For example, when there are three objects with the same names "MyObject",
  * and they will be renamed to "MyObject", "MyObject1" and "MyObject2".
  * \nosubgrouping
  * \see KName KRenamingStrategy
  */
class KFBX_DLL KNumberRenamingStrategy : public KRenamingStrategy
{
public:
    //! Constructor.
    KNumberRenamingStrategy();

    //! Destructor.
    virtual ~KNumberRenamingStrategy ();

    //! Resets internal state regarding assigned names.
    virtual void Clear();

    /** Rename a name if necessary to avoid name-clash issues.
      * \param pName The name to be renamed.
      * \return Return \c true on success, \c false otherwise.
      */
    virtual bool Rename(KName& pName);

    /** Create a dynamic renaming strategy instance of the same type as the child class.
      * \return New instance.
      */
    virtual KRenamingStrategy* Clone();

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  WARNING!
    //
    //    Anything beyond these lines may not be documented accurately and is 
    //     subject to change without notice.
    //
    ///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

private:
    
    struct NameCell
    {
        NameCell(char const* pName) :
            mName(pName),
            mInstanceCount(0)
        {
        }
            
        KString mName;
        int mInstanceCount;
    };

    KArrayTemplate<NameCell*> mNameArray;

#endif // DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KRENAMINGSTRATEGY_H

