/*!  \file kfbxrenamingstrategybase.h
*/
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXRENAMINGSTRATEGYBASE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXRENAMINGSTRATEGYBASE_H

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
#include <fbxfilesdk/kfbxplugins/kfbxscene.h>
#include <fbxfilesdk/components/kbaselib/klib/kname.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** \brief Base class for renaming strategy.
* \nosubgrouping
*/
class KFBX_DLL KFbxRenamingStrategyBase
{
public:

    //! Default constructor
    KFbxRenamingStrategyBase();

    /** Constructor.
    * \param pNameSpaceSymbol
    */
    KFbxRenamingStrategyBase(char pNameSpaceSymbol);

    //! Destructor.
    virtual ~KFbxRenamingStrategyBase();

    /** This method put all the names in the scene back to the original values
    * \param pScene
    * \return Returns true if some names have been modified.
    */
    virtual bool DecodeScene(KFbxScene* pScene)=0;

    /** This method renames all the names in the scene
    * \param pScene
    * \return Returns true if some names have been modified.
    */
    virtual bool EncodeScene(KFbxScene* pScene)=0;

    /** This method find the original name of a given string
    * \param pString
    * \return Returns true if the name has been modified.
    */
    virtual bool DecodeString(KName& pString)=0;

    /** This method find the renaming name of a given string
    * \param pString
    * \param pIsPropertyName
    * \return Returns true if the name has been modified.
    */
    virtual bool EncodeString(KName& pString, bool pIsPropertyName=false)=0;

    //! clean up the name cells.
    virtual void CleanUp();

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
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

    char                     mNamespaceSymbol;
    KCharPtrSet					mStringNameArray;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif
