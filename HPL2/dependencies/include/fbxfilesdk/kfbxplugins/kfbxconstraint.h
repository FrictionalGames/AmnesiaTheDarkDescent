/*!  \file kfbxconstraint.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINT_H
#define FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINT_H

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

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>
#include <fbxfilesdk/components/kbaselib/klib/kstringlist.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;
class KFbxVector4;

/** Base class for weighted animation constraints.
  * Constraints are primarily used to impose limits on properties of objects (e.g. position, orientation, scale)
  * and to automate animation processes.
  * A <b>constrained object</b> is an object with properties constrained by one or more weighted <b>constraint source</b>s.
  * \nosubgrouping
  */
class KFBX_DLL KFbxConstraint : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxConstraint,KFbxObject);

public:
    /**
      * \name Properties
      */
    //@{
        /** This property represents the degree of influence of a constraint from 0.0 (no influence) to 100.0 (full influence).
          *
          * Default value is 100.0.
          */
        KFbxTypedProperty<fbxDouble1> Weight;

        /** This property controls whether the constraint is applied or not.
          * If the value is \c false the constraint will have no effect. The default value is \c true.
          *
          * Default value is true.
          */
        KFbxTypedProperty<fbxBool1> Active;

        /** This property handles the lock state of the constraint.
          *
          * When enabled, the constrained object cannot be moved away from its constrained location when the constraint is active.
          *
          * Default value is false.
          */
        KFbxTypedProperty<fbxBool1> Lock;
    //@}

    /** \enum EConstraintType Constraint attribute types.
      * - \e eUNIDENTIFIED          Invalid constraint.
      * - \e ePOSITION              Position constraint (referred to as a point constraint in Maya).
      * - \e eROTATION              Rotation constraint (referred to as an orient constraint in Maya).
      * - \e eSCALE                 Scale constraint.
      * - \e ePARENT                Parent constraint.
      * - \e eSINGLECHAIN_IK        Single chain IK constraint.
      * - \e eAIM                   Aim constraint.
      * - \e eCHARACTER_CONSTRAINT  Character.
      * - \e eCUSTOM                Constraint defined by customers.
      * - \e eCONSTRAINT_COUNT      The count of constraint type.
      */
    typedef enum
    {
        eUNIDENTIFIED,
        ePOSITION,
        eROTATION,
        eSCALE,
        ePARENT,
        eSINGLECHAIN_IK,
        eAIM,
        eCHARACTER_CONSTRAINT,
        eCUSTOM,
        eCONSTRAINT_COUNT
    } EConstraintType;

    /** Access the type of the constraint.
      * \return This type of the constraint.
      */
    virtual EConstraintType GetConstraintType() const { return eUNIDENTIFIED; }

    /** Retrieve the constrained object.
      * \return The constrained object.
      */
    virtual KFbxObject* GetConstrainedObject() const { return NULL; }

    /** Retrieve the count of constraint source.
      * \return The count of constraint source.
      */
    virtual int GetConstraintSourceCount() const { return 0; }

    /** Retrieve a constraint source with the specified index.
      * \param pIndex The specified index.
      * \return The constraint source at the specified index.
      */
    virtual KFbxObject* GetConstraintSource(int pIndex) const { return NULL; }

    /** Set value of the offset.
      * \param pOffset Offset vector value.
      */
    virtual void SetOffset(KFbxVector4 pOffset);

    /** Get the weight associated with a constraint source.
      * \param pObject The given constraint source.
      * \return The weight of the constraint source.
      */
    double GetSourceWeight(const KFbxObject* pObject) const;

    /**
      * \name Error Management
      */
    //@{

        /** Retrieve error object.
          * \return     Reference to error object.
          */
        KError& GetError();

        /** \enum EError Error identifiers.
          * - \e eERROR
          * - \e eERROR_COUNT
          */
        typedef enum
        {
            eERROR,
            eERROR_COUNT
        } EError;

        /** Get last error code.
          * \return     Last error code.
          */
        EError GetLastErrorID() const;

        /** Get last error string.
          * \return     Textual description of the last error.
          */
        const char* GetLastErrorString() const;

    //@}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

protected:
    KFbxConstraint(KFbxSdkManager& pManager, char const* pName);
    virtual bool ConstructProperties(bool pForceSet);

private:
    KError mError;

protected:

    friend class KFbxScene;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};


///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

const KString GetWeightPropertyName(const KFbxObject * pObject);
void CreateWeightPropertyForSourceObject(KFbxObject * pConstraint,
                                         const KFbxObject * pSourceObject, double pWeightValue);

typedef KFbxConstraint* HKFbxConstraint;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINT_H

