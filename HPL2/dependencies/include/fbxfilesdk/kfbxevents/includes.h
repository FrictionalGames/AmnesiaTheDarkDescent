/*!  \file
 */

#ifndef FBXFILESDK_KFBXEVENTS_INCLUDES_H
#define FBXFILESDK_KFBXEVENTS_INCLUDES_H

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

// Local includes
#include <fbxfilesdk/kfbxevents/kfbxemitter.h>
#include <fbxfilesdk/kfbxevents/kfbxevents.h>
#include <fbxfilesdk/kfbxevents/kfbxlistener.h>
#include <fbxfilesdk/kfbxevents/kfbxeventhandler.h>

// FBX namespace begin
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

// FBX users must call this macro in synchronize event 
// type ids between DLLs and applications.
#define INITIALZE_FBX_PUBLIC_EVENTS \
    int ____FBXReservedIndex = 0x4000000;\
    KFbxQueryEvent<int>*_01_(0);_01_=_01_;KFbxQueryEvent<int>::ForceTypeId(++____FBXReservedIndex); \
    KFbxQueryEvent<float>*_02_(0);_02_=_02_;KFbxQueryEvent<float>::ForceTypeId(++____FBXReservedIndex); \
    KFbxQueryEvent<double>*_03_(0);_03_=_03_;KFbxQueryEvent<double>::ForceTypeId(++____FBXReservedIndex); \
    KFbxQueryEvent<KFbxSdkManager>*_04_(0);_04_=_04_;KFbxQueryEvent<KFbxSdkManager>::ForceTypeId(++____FBXReservedIndex); \
    KFbxQueryEvent<KFbxObject>*_05_(0);_05_=_05_;KFbxQueryEvent<KFbxObject>::ForceTypeId(++____FBXReservedIndex); \
    KFbxQueryEvent<KFbxDocument>*_06_(0);_06_=_06_;KFbxQueryEvent<KFbxDocument>::ForceTypeId(++____FBXReservedIndex); \
    KFbxQueryEvent<KFbxLibrary>*_07_(0);_07_=_07_;KFbxQueryEvent<KFbxLibrary>::ForceTypeId(++____FBXReservedIndex); \
    KFbxQueryEvent<KFbxImporter>*_08_(0);_08_=_08_;KFbxQueryEvent<KFbxImporter>::ForceTypeId(++____FBXReservedIndex); \
    KFbxQueryEvent<KFbxExporter>*_09_(0);_09_=_09_;KFbxQueryEvent<KFbxExporter>::ForceTypeId(++____FBXReservedIndex); 
    // MUST add new public event types HERE  

// FBX namespace end
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXEVENTS_INCLUDES_H


