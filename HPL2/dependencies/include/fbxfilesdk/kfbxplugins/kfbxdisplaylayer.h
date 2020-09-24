/*!  \file kfbxdisplaylayer.h
 */
#ifndef FBXFILESDK_KFBXDISPLAYLAYER_H
#define FBXFILESDK_KFBXDISPLAYLAYER_H

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

#include <fbxfilesdk/fbxcore/fbxcollection/kfbxcollectionexclusive.h>
#include <fbxfilesdk/kfbxplugins/kfbxsdkmanager.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** Class for display layers.
* \nosubgrouping
* Display layers are overlapping views of your scene that contain a list of members. 
* The members are exclusive. Members cannot be part of multiple display layers. 
* Display layers enables user to organize elements of scene and affect visibility and manipulation attributes of multiple objects at once. 
*/
class KFBX_DLL KFbxDisplayLayer : public KFbxCollectionExclusive
{
    KFBXOBJECT_DECLARE(KFbxDisplayLayer, KFbxCollectionExclusive);

public:
    //////////////////////////////////////////////////////////////////////////
    //
    // Properties
    //
    //////////////////////////////////////////////////////////////////////////
    /** This property stores the color of this display layer.
    *
    * Default value is fbxDouble3(0.8,0.8,0.8).
    */
    KFbxTypedProperty<fbxDouble3>       Color;
    /** This property stores the visibility of this display layer.
    *
    * Default value is true.
    */
    KFbxTypedProperty<fbxBool1>         Show;
    /** This property stores the manipulation state of this display layer.
    *
    * Default value is false.
    */
    KFbxTypedProperty<fbxBool1>         Freeze;
    /** This property stores the level of detail mode of this display layer.
    *
    * Default value is false.
    */
    KFbxTypedProperty<fbxBool1>         LODBox;

    //////////////////////////////////////////////////////////////////////////
    // Static values
    //////////////////////////////////////////////////////////////////////////

    // Default property values
    static const fbxDouble3 sColorDefault;

private:

#ifndef DOXYGEN_SHOULD_SKIP_THIS

protected:
    KFbxDisplayLayer(KFbxSdkManager& pManager, char const* pName);
    
    bool ConstructProperties(bool pForceSet);  

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXDISPLAYLAYER_H
