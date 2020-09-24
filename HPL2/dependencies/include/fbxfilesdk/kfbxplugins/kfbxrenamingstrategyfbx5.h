/*!  \file kfbxrenamingstrategyfbx5.h
*/
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXRENAMINGSTRATEGYFBX5_H
#define FBXFILESDK_KFBXPLUGINS_KFBXRENAMINGSTRATEGYFBX5_H
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
#include <fbxfilesdk/kfbxplugins/kfbxrenamingstrategybase.h>
#include <fbxfilesdk/kfbxplugins/kfbxrenamingstrategyutilities.h>
#include <fbxfilesdk/kfbxplugins/kfbxscene.h>
#include <fbxfilesdk/components/kbaselib/klib/kname.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** \brief This class contains the description of the FBX renaming strategy for fbx file format 5.
* \nosubgrouping
*/
class KFBX_DLL KFbxRenamingStrategyFbx5: public KFbxRenamingStrategyBase
{
public:

    //! Default constructor
    KFbxRenamingStrategyFbx5();

    //! Destructor
    virtual ~KFbxRenamingStrategyFbx5();

    /** This method put all the names in the scene back to the original values
    * \param pScene
    * \return Returns true if some names have been modified.
    */
    virtual bool DecodeScene(KFbxScene* pScene);

    /** This method renames all the names in the scene
    * \param pScene
    * \return Returns true if some names have been modified.
    */
    virtual bool EncodeScene(KFbxScene* pScene);

    /** This method find the original name of a given string
    * \param pName
    * \return Returns true if the name has been modified.
    */
    virtual bool DecodeString(KName& pName);

    /** This method find the renaming name of a given string
    * \param pName
    * \param pIsPropertyName
    * \return Returns true if the name has been modified.
    */
    virtual bool EncodeString(KName& pName, bool pIsPropertyName=false);

    //! clean up the name cells.
    virtual void CleanUp();
};


#include <fbxfilesdk/fbxfilesdk_nsend.h>
#endif
