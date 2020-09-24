/*!  \file kfbxnull.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXNULL_H
#define FBXFILESDK_KFBXPLUGINS_KFBXNULL_H

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

#include <fbxfilesdk/kfbxplugins/kfbxnodeattribute.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;

/** \brief This node attribute contains the properties of a null node.
  * \nosubgrouping
  */
class KFBX_DLL KFbxNull : public KFbxNodeAttribute
{
    KFBXOBJECT_DECLARE(KFbxNull,KFbxNodeAttribute);

public:
    //! Returns the EAttributeType::eNULL node attribute type.
    virtual EAttributeType GetAttributeType() const;

    //! Resets the default values.
    void Reset();

    /**
      * \name Null Node Properties
      */
    //@{

    /** \enum ELook         Null node look types.
      * - \e eNONE
      * - \e eCROSS
      */
    typedef enum {
        eNONE,
        eCROSS,
    } ELook;

    /** Returns the default size value.
      * \return             The default size of this object (100).
      */
    double GetSizeDefaultValue() const;

    //@}

    /**
      * \name Property Names
      */
    //@{
    static const char*          sSize;
    static const char*          sLook;
    //@}

    /**
      * \name Property Default Values
      */
    //@{
    static const fbxDouble1     sDefaultSize;
    static const ELook      sDefaultLook;
    //@}


    //////////////////////////////////////////////////////////////////////////
    //
    // Properties
    //
    //////////////////////////////////////////////////////////////////////////

    /** This property handles the null node size.
      *
      * To access this property do: Size.Get().
      * To set this property do: Size.Set(fbxDouble1).
      *
      * The default value is 100.
      */
    KFbxTypedProperty<fbxDouble1>       Size;

    /** This property handles the look of the null node.
      *
      * To access this property do: Look.Get().
      * To set this property do: Look.Set(ELook).
      *
      * The default value is true
      */
    KFbxTypedProperty<ELook>            Look;


///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
    KFbxNull(KFbxSdkManager& pManager, char const* pName);

    virtual void Construct(const KFbxNull* pFrom);
    virtual bool ConstructProperties(bool pForceSet);

public:
    virtual KStringList GetTypeFlags() const;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxNull* HKFbxNull;

inline EFbxType FbxTypeOf( KFbxNull::ELook const &pItem )           { return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXNULL_H

