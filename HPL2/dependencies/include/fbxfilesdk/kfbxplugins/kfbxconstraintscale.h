/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxconstraintscale.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTSCALE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTSCALE_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxconstraint.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;
class KFbxVector4;

/**\brief This constraint class contains methods for accessing the properties of a scale constraint.
  * A scale constraint lets you constrain the scale of an object based on the scale of one or more sources.
  * \nosubgrouping
  */
class KFBX_DLL KFbxConstraintScale : public KFbxConstraint
{
	KFBXOBJECT_DECLARE(KFbxConstraintScale,KFbxConstraint);

public:
		/**
		  * \name Properties
		  */
		//@{
			/** This property handles whether to affect x axis.
              * Default value is true.
              */
			KFbxTypedProperty<fbxBool1>		AffectX;
			
			/** This property handles whether to affect y axis.
              * Default value is true.
              */
			KFbxTypedProperty<fbxBool1>		AffectY;

			/** This property handles whether to affect z axis.
              * Default value is true.
              */
			KFbxTypedProperty<fbxBool1>		AffectZ;
           
			/** This property handles scaling offset.
              * Default value is (0, 0, 0).
              */
			KFbxTypedProperty<fbxDouble3>	Scaling;

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

	/** Set the Scale offset.
	  * \param pScale New offset vector.
	  */
	virtual void SetOffset(KFbxVector4 pScale);

	/** Retrieve the constraint scale offset.
	  * \return Current scale offset.
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
	KFbxConstraintScale(KFbxSdkManager& pManager, char const* pName);

	virtual bool ConstructProperties(bool pForceSet);

	virtual EConstraintType GetConstraintType() const;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTSCALE_H

