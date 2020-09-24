/*!  \file kfbxconstraintparent.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTPARENT_H
#define FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTPARENT_H

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

#include <fbxfilesdk/kfbxplugins/kfbxconstraint.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;


/** The parent constraint creates a parent-to-child relationship between any two objects, from any two hierarchies.
  * It creates the same relationship as the parent-to-child relationships found in hierarchies.
  * You can use this constraint to connect objects without changing hierarchies.
  * \nosubgrouping
  */
class KFBX_DLL KFbxConstraintParent : public KFbxConstraint
{
    KFBXOBJECT_DECLARE(KFbxConstraintParent,KFbxConstraint);

public:
    /**
      * \name Properties
      */
    //@{
        /** This property handles whether to affect the translation of the constrained object along X axis.
          *
          * Default value is true.
          */
        KFbxTypedProperty<fbxBool1> AffectTranslationX;

        /** This property handles whether to affect the translation of the constrained object along Y axis.
          *
          * Default value is true.
          */
        KFbxTypedProperty<fbxBool1> AffectTranslationY;

        /** This property handles whether to affect the translation of the constrained object along Z axis.
          *
          * Default value is true.
          */
        KFbxTypedProperty<fbxBool1> AffectTranslationZ;

        /** This property handles whether to affect the rotation of the constrained object around X axis.
          *
          * Default value is true.
          */
        KFbxTypedProperty<fbxBool1> AffectRotationX;

        /** This property handles whether to affect the rotation of the constrained object around Y axis.
          *
          * Default value is true.
          */
        KFbxTypedProperty<fbxBool1> AffectRotationY;

        /** This property handles whether to affect the rotation of the constrained object around Z axis.
          *
          * Default value is true.
          */
        KFbxTypedProperty<fbxBool1> AffectRotationZ;

        /** This property handles whether to affect the scaling of the constrained object along X axis.
          *
          * Default value is true.
          */
        KFbxTypedProperty<fbxBool1> AffectScalingX;

        /** This property handles whether to affect the scaling of the constrained object along Y axis.
          *
          * Default value is true.
          */
        KFbxTypedProperty<fbxBool1> AffectScalingY;

        /** This property handles whether to affect the scaling of the constrained object along Z axis.
          *
          * Default value is true.
          */
        KFbxTypedProperty<fbxBool1> AffectScalingZ;

        /** This property used to access constraint sources.
          * A constrained object is an object whose position, orientation, and so on is driven by one or more constraint sources.
          */
        KFbxTypedProperty<fbxReference> ConstraintSources;

        /** This property used to access constrained object.
          * A constrained object is an object whose position, orientation, and so on is driven by one or more constraint sources.
          */
        KFbxTypedProperty<fbxReference> ConstrainedObject;
    //@}

    /** Set the constraint lock state.
      * \param pLock The new state of the lock flag.
      */
    void SetLock(bool pLock);

    /** Retrieve the constraint lock state.
      * \return The current value of the lock flag.
      */
    bool GetLock() const;

    /** Set the translation offset of the specified constraint source.
      * \param pObject The specified constraint source.
      * \param pTranslation The new offset vector.
      */
    void SetTranslationOffset(KFbxObject* pObject, KFbxVector4 pTranslation);

    /** Retrieve the translation offset of the specified constraint source.
      * \param pObject The specified constraint source.
      * \return The current translation offset.
      */
    KFbxVector4 GetTranslationOffset(const KFbxObject* pObject) const;

    /** Set the rotation offset of the specified constraint source.
      * \param pObject The specified constraint source.
      * \param pRotation The new offset vector.
      */
    virtual void SetRotationOffset(const KFbxObject* pObject, KFbxVector4 pRotation);

    /** Retrieve the rotation offset of the specified constraint source.
      * \param pObject The specified constraint source.
      * \return The current translation offset.
      */
    KFbxVector4 GetRotationOffset(const KFbxObject* pObject) const;

    /** Set the weight of the constraint.
      * \param pWeight The new weight value.
      */
    void SetWeight(double pWeight);

    /** Add a constraint source to the constraint.
      * \param pObject New constraint source.
      * \param pWeight Weight of the constraint source.
      */
    void AddConstraintSource(KFbxObject* pObject, double pWeight = 100);

    /** Retrieve the constraint source count.
      * \return Current constraint source count.
      */
    int GetConstraintSourceCount() const;

    /** Retrieve a constraint source object.
      * \param pIndex Index of the constraint source.
      * \return The constraint source at the specified index.
      */
    KFbxObject* GetConstraintSource(int pIndex) const;

    /** Set the constrained object.
      * \param pObject The constrained object.
      */
    void SetConstrainedObject(KFbxObject* pObject);

    /** Retrieve the constrained object.
      * \return Current constrained object.
      */
    KFbxObject* GetConstrainedObject() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    KFbxConstraintParent(KFbxSdkManager& pManager, char const* pName);

    virtual bool ConstructProperties( bool pForceSet );

    virtual EConstraintType GetConstraintType() const;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTPARENT_H

