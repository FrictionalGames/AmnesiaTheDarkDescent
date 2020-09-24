/*!  \file kfbxrenamingstrategy.h
 */
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXRENAMINGSTRATEGY_H
#define FBXFILESDK_KFBXPLUGINS_KFBXRENAMINGSTRATEGY_H

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
#include <fbxfilesdk/components/kbaselib/klib/kstringlist.h>
#include <fbxfilesdk/components/kbaselib/klib/krenamingstrategy.h>
#include <fbxfilesdk/components/kbaselib/klib/kcharptrset.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxNode;
class KFbxTexture;
class KFbxVideo;
class KFbxScene;
class KFbxNodeAttribute;

/** The KFbxRenamingStrategy object can be set to rename all the objects in a scene.
  * It can remove name clashing, remove illegal characters, manage namespaces and manage backward compatibility.
  * It is better to choose KFbxSceneRenamer instead of this class to simplify the usage.
  * \nosubgrouping
  * \see KFbxSceneRenamer
  */
class KFBX_DLL KFbxRenamingStrategy : public KRenamingStrategy
{
public:
    /** \enum EMode The mode describing the convention direction, from FBX format or to FBX format.
      * - \e eTO_FBX        Convert to FBX format from another format.
      * - \e eFROM_FBX      Convert from FBX format to another format.
      * - \e eMODE_COUNT
      */
    enum EMode
    {
        eTO_FBX,
        eFROM_FBX,
        eMODE_COUNT
    };

    /** Constructor.
      * \param pMod The mode describing the convention direction, from FBX format or to FBX format.
      * \param pOnCreationRun
      */
    KFbxRenamingStrategy(EMode pMod, bool pOnCreationRun = false);

    //! Destructor.
    virtual ~KFbxRenamingStrategy();
    
    /** Rename a name if necessary.
      * \param pName The name to be renamed.
      * \return Return \c true on success, \c false otherwise.
      */
    virtual bool Rename(KName& pName);

    //! Resets internal state regarding assigned names.
    virtual void Clear();
    
    /** Create a dynamic renaming strategy instance of the same type as the child class.
      * \return New instance.
      */
    virtual KRenamingStrategy* Clone();

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  WARNING!
    //
    //  Anything beyond these lines may not be documented accurately and is
    //  subject to change without notice.
    //
    ///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

        /** \enum EClashType
      * - \e eNAMECLASH_AUTO
      * - \e eNAMECLASH_TYPE1
      * - \e eNAMECLASH_TYPE2
      */
    enum EClashType
    {
        eNAMECLASH_AUTO,
        eNAMECLASH_TYPE1,
        eNAMECLASH_TYPE2
    };

    /** Setup the strategy to perform this algorithm
      * \param pType 
      */
    void SetClashSoverType(EClashType pType);

    /** Returns a name with its prefix removed.
     * \param pName    A name containing a prefix.
     * \return         The part of pName following the "::"
     */
    static char* NoPrefixName (char const* pName);

    /** Returns a name with its prefix removed.
    * \param pName    A name containing a prefix.
    * \return         The part of pName following the "::"
    */
    static char* NoPrefixName (KString& pName);

    /** Get the namespace of the last renamed object.
     * \return     Char pointer to the namespace.
     */
    virtual char* GetNameSpace() { return mNameSpace.Buffer(); } 

    /** Sets the current scene namespace symbol.
     * \param pNameSpaceSymbol     namespace symbol.
     */
    virtual void SetInNameSpaceSymbol(KString pNameSpaceSymbol){mInNameSpaceSymbol = pNameSpaceSymbol;}
    
    /** Sets the wanted scene namespace symbol.
     * \param pNameSpaceSymbol     namespace symbol.
     */
    virtual void SetOutNameSpaceSymbol(KString pNameSpaceSymbol){mOutNameSpaceSymbol = pNameSpaceSymbol;}    

    /** Sets case sensitivity for name clashing.
     * \param pIsCaseSensitive     Set to \c true to make the name clashing case sensitive.
     */
    virtual void SetCaseSensibility(bool pIsCaseSensitive){mCaseSensitive = pIsCaseSensitive ;}

    /** Sets the flag for character acceptance during renaming.
     * \param pReplaceNonAlphaNum     Set to \c true to replace illegal characters with an underscore ("_").  
     */
    virtual void SetReplaceNonAlphaNum(bool pReplaceNonAlphaNum){mReplaceNonAlphaNum = pReplaceNonAlphaNum;}

    /** Sets the flag for first character acceptance during renaming.
     * \param pFirstNotNum     Set to \c true to add an underscore to the name if the first character is a number.
     */
    virtual void SetFirstNotNum(bool pFirstNotNum){mFirstNotNum = pFirstNotNum;}

    /** Recursively renames all the unparented namespaced objects (Prefix mode) starting from this node.
     * \param pNode       Parent node.
     * \param pIsRoot     The root node.
     * \remarks           This function adds "_NSclash" when it encounters an unparented namespaced object.
     */
    virtual bool RenameUnparentNameSpace(KFbxNode* pNode, bool pIsRoot = false);

    /** Recursively removes all the unparented namespaced "key" starting from this node.
     * \param pNode     Parent node.
     * \remarks         This function removes "_NSclash" when encountered. This is the opposite from RenameUnparentNameSpace.
     */
    virtual bool RemoveImportNameSpaceClash(KFbxNode* pNode);

    virtual void GetParentsNameSpaceList(KFbxNode* pNode, KArrayTemplate<KString*> &pNameSpaceList);
    virtual bool PropagateNameSpaceChange(KFbxNode* pNode, KString OldNS, KString NewNS);

protected:

    virtual bool RenameToFBX(KName& pName);
    virtual bool RenameFromFBX(KName& pName);
    virtual KString& ReplaceNonAlphaNum(KString& pName,    char* pReplace, bool pIgnoreNameSpace);

    EMode mMode;
    EClashType mType;

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


    KCharPtrSet                    mStringNameArray;
    KArrayTemplate<NameCell*>    mExistingNsList;
    bool                        mOnCreationRun;
    bool                        mCaseSensitive;
    bool                        mReplaceNonAlphaNum;
    bool                        mFirstNotNum;
    KString                        mNameSpace;
    KString                        mInNameSpaceSymbol; //symbol identifying a name space
    KString                        mOutNameSpaceSymbol; 

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

/** The KFbxSceneRenamer provides a way to easily rename objects in a scene without using the KFbxRenamingStrategy class.
  * KFbxSceneRenamer can remove name clashing and illegal characters. It also manages namespaces.
  *
  * Example:
  * Maya only accepts names with letters, digits, or underscores, and we want to convert
  * all the names of a scene from FBX format to Maya format.
  * \code
  * KFbxSceneRenamer lSceneRenamer(pScene);
  * lSceneRenamer.RenameFor(KFbxSceneRenamer::eFBX_TO_MAYA);
  * \endcode
  * \nosubgrouping
  * \see KFbxRenamingStrategy
  */
class KFBX_DLL KFbxSceneRenamer
{
public:
    /** Constructor
      * \param pScene A scene which contains objects to be renamed.
      */
    KFbxSceneRenamer(KFbxScene* pScene) {mScene = pScene;};

    //! Destructor.
    virtual ~KFbxSceneRenamer(){};

     /** \enum ERenamingMode The Mode describing from which format to which format.
       * - \e eNONE
       * - \e eMAYA_TO_FBX5
       * - \e eMAYA_TO_FBX_MB75
       * - \e eMAYA_TO_FBX_MB70
       * - \e eFBXMB75_TO_FBXMB70
       * - \e eFBX_TO_FBX
       * - \e eMAYA_TO_FBX
       * - \e eFBX_TO_MAYA
       * - \e eLW_TO_FBX
       * - \e eFBX_TO_LW
       * - \e eXSI_TO_FBX
       * - \e eFBX_TO_XSI
       * - \e eMAX_TO_FBX
       * - \e eFBX_TO_MAX
       * - \e eMB_TO_FBX
       * - \e eFBX_TO_MB
       * - \e eDAE_TO_FBX
       * - \e eFBX_TO_DAE
       */
    enum ERenamingMode
    { 
        eNONE,
        eMAYA_TO_FBX5,
        eMAYA_TO_FBX_MB75,
        eMAYA_TO_FBX_MB70,
        eFBXMB75_TO_FBXMB70,
        eFBX_TO_FBX,
        eMAYA_TO_FBX,
        eFBX_TO_MAYA,
        eLW_TO_FBX,
        eFBX_TO_LW,
        eXSI_TO_FBX,
        eFBX_TO_XSI,
        eMAX_TO_FBX,
        eFBX_TO_MAX,
        eMB_TO_FBX,
        eFBX_TO_MB,
        eDAE_TO_FBX,
        eFBX_TO_DAE
    };

    /** Rename the objects of the scene according the specific mode.
      * \param pMode A mode describing from which format to which format.
      */
    void RenameFor(ERenamingMode pMode);

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

private:
    
     /** Rename all object to remove name clashing.
     * \param pFromFbx                  Set to \c true to enable this flag.
     * \param pIgnoreNS                 Set to \c true to enable this flag.
     * \param pIsCaseSensitive          Set to \c true to enable case sensitive renaming.
     * \param pReplaceNonAlphaNum       Set to \c true to replace non-alphanumeric characters with underscores ("_").
     * \param pFirstNotNum              Set to \c true to add a leading _ if first char is a number (for xs:NCName).
     * \param pInNameSpaceSymbol        Identifier of a namespace.
     * \param pOutNameSpaceSymbol       Identifier of a namespace.
     * \param pNoUnparentNS             Set to \c true to not not allow unparent namespace.
     * \param pRemoveNameSpaceClash     Set to \c true to remove NameSpaceClash token.
     * \return void.
     */
    void ResolveNameClashing(    bool pFromFbx, bool pIgnoreNS, bool pIsCaseSensitive,
                                bool pReplaceNonAlphaNum, bool pFirstNotNum,
                                KString pInNameSpaceSymbol, KString pOutNameSpaceSymbol,
                                bool pNoUnparentNS/*for MB < 7.5*/, bool pRemoveNameSpaceClash);

    KRenamingStrategy* mNodeRenamingStrategy;
    KFbxScene* mScene;

#endif // DOXYGEN_SHOULD_SKIP_THIS
};


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXRENAMINGSTRATEGY_H

