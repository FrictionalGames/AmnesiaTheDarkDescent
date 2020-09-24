/*!  \file kname.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KNAME_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KNAME_H

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

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** A name is a case-sensitive string ID of a property, a node, a node attribute, a texture, etc. The characters constituting a name has no specific limitation.
  * An initial name is the original name (from importing a file, for example), which is saved up for reversible renaming.
  * A current name is the name used in FBX.
  * A namespace is a simple grouping of objects under a given name. Namespaces are primarily used to resolve
  * name-clash issues in FBX, where a new object has the same name as an existing object.
  *
  * For example, Maya only accepts names with letters, digits, or underscores. And when a user import FBX into Maya,
  * a node whose name contains whitespace will be renamed. But the connections and references to this node in FBX
  * scene graph still use the original name, so users have to use the initial name to retrieve related information.
  * \nosubgrouping
  */
class KFBX_DLL KName
{
public:
    
    /** Constructor.
      * \param pInitialName Name string used to initialize both members (initialName and currentName)
      * of this class.
      */
    KName(char const* pInitialName = "");

    /** Copy constructor.
      * \param pName A KName copied to this one.
      */
    KName(KName const& pName);

    // !Destructor
    ~KName();

    /** Set the initial name.
      * \param pInitialName New string for the initial name.
      * \remarks The current name will also be changed to this value.
      */
    void SetInitialName(char const* pInitialName);

    /** Get the initial name.
      * \return Pointer to the InitialName string buffer.
      */
    char const* GetInitialName() const;

    /** Set the current name.
      * \param pNewName New string for the current name.
      * \remarks The initial name is not affected.
      */
    void SetCurrentName(char const* pNewName);

    /** Get the current name.
      * \return Pointer to the CurrentName string buffer.
      */
    char const* GetCurrentName() const;

    /** Set the namespace.
      * \param pNameSpace New string for the namespace.
      * \remarks The initial name is not affected.
      */
    void SetNameSpace(char const* pNameSpace);

    /** Get the namespace.
      * \return Pointer to the namespace string buffer.
      */
    char const* GetNameSpace() const;

    /** Check if the current name and initial name match.
      * \return \c true if the current name isn't identical to the initial name.
      */
    bool IsRenamed() const;
    
    /** Assignment operator
      * \param pName KName assigned to this one.
      */
    KName& operator= (KName const& pName);

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//    Anything beyond these lines may not be documented accurately and is 
//     subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    /**
    * \name Private use for the renaming strategies classes.
    *
    * Some renaming strategies classes need to store the parent name to successfully apply the renaming algorithms. 
    * The methods in this section allow them to do so.
    * \remark Because of the very specific use of the mParentName string, 
    * callers of the KName class should never assume that mParentName is correctly initialized 
    * nor contains a meaningful value outside the scope of the renaming strategy class that used it.
    */
    //@{

    /** Set the parent name.
    * \param pParentName New string for the parent name.
    * \remarks The parent name here could combine several hierarchy name.
    * The full name should be "ParentName + CurrentName".
    *   A
    *   |_B
    *     |_C
    * For the above hierarchy, the parent name of C is "AB".
    * The full name of C is "ABC".
    */
    void SetParentName(char const* pParentName);

    /** Get the parent name.
    * \return Pointer to the ParentName string buffer.
    */
    char const* GetParentName() const;

    //@}

    /** Get the namespaces in a string pointer array format.
    * \return KArrayTemplate<KString*> .
    */
    KArrayTemplate<KString*> GetNameSpaceArray(char identifier);

private:
    KString mParentName;
    KString mInitialName;
    KString mCurrentName;
    KString mNameSpace;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KNAME_H

