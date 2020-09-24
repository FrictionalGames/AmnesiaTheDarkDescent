/*!  \file kfbxconstraintposition.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTPOSITION_H
#define FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTPOSITION_H

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
class KFbxVector4;

/** \brief This constraint class contains methods for accessing the properties of a position constraint.
  * A position constraint lets you constrain the position of an object based on the position of one or more sources.
  * \nosubgrouping
  */
class KFBX_DLL KFbxConstraintPosition : public KFbxConstraint
{
    KFBXOBJECT_DECLARE(KFbxConstraintPosition,KFbxConstraint);

public:

        /**
          * \name Properties
          */
        //@{
            /** This property handles whether to affect x axis.
              *
              * Default value is true.
              */
            KFbxTypedProperty<fbxBool1>        AffectX;

            /** This property handles whether to affect y axis.
              *
              * Default value is true.
              */
            KFbxTypedProperty<fbxBool1>        AffectY;

            /** This property handles whether to affect z axis.
              *
              * Default value is true.
              */
            KFbxTypedProperty<fbxBool1>        AffectZ;

            /** This property handles translation offset.
              *
              * Default value is (0, 0, 0).
              */
            KFbxTypedProperty<fbxDouble3>    Translation;

            /** This property handles constraint source objects.
              */
            KFbxTypedProperty<fbxReference> ConstraintSources;

            /** This property handles constraint target objects.
              */
            KFbxTypedProperty<fbxReference> ConstrainedObject;
        //@}

    /** Set the constraint lock.
      * \param pLock State of the lock flag.
      */
    void SetLock(bool pLock);

    /** Retrieve the constraint lock state.
      * \return Current lock flag.
      */
    bool GetLock() const;

	/** Set the constraint X-axis effectiveness.
	  * \param pAffect State of the effectiveness on the X axis.
	  */
	void SetAffectX(bool pAffect);

	/** Retrieve the constraint X-axis effectiveness.
	  * \return Current state flag.
	  */
	bool GetAffectX() const;

	/** Set the constraint Y-axis effectiveness.
	  * \param pAffect State of the effectiveness on the X axis.
	  */
	void SetAffectY(bool pAffect);

	/** Retrieve the constraint Y-axis effectiveness.
	  * \return Current state flag.
	  */
	bool GetAffectY() const;

	/** Set the constraint Z-axis effectiveness.
	  * \param pAffect State of the effectiveness on the X axis.
	  */
	void SetAffectZ(bool pAffect);

	/** Retrieve the constraint Z-axis effectiveness.
	  * \return Current state flag.
	  */
	bool GetAffectZ() const;

    /** Set the translation offset.
      * \param pTranslation New offset vector.
      */
    virtual void SetOffset(KFbxVector4 pTranslation);

    /** Retrieve the constraint translation offset.
      * \return Current translation offset.
      */
    KFbxVector4 GetOffset() const;

    /** Set the weight of the constraint.
      * \param pWeight New weight value.
      */
    void SetWeight(double pWeight);

    /** Add a source to the constraint.
      * \param pObject New source object.
      * \param pWeight Weight of the source object.
      */
    void AddConstraintSource(KFbxObject* pObject, double pWeight = 100);

    /** Remove a source from the constraint.
      * \param pObject Source object to remove.
      */
    bool RemoveConstraintSource(KFbxObject* pObject);

    /** Retrieve the constraint source count.
      * \return Current constraint source count.
      */
    int GetConstraintSourceCount() const;

    /** Retrieve a constraint source object.
      * \param pIndex Index of the source
      * \return Current source at the specified index.
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
    KFbxConstraintPosition(KFbxSdkManager& pManager, char const* pName);

    virtual bool ConstructProperties(bool pForceSet);

    virtual EConstraintType GetConstraintType() const;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTPOSITION_H

