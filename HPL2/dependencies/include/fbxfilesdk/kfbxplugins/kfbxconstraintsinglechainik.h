/*!  \file kfbxconstraintsinglechainik.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTSINGLECHAINIK_H
#define FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTSINGLECHAINIK_H

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

#include <fbxfilesdk/kfbxmath/kfbxvector4.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;

/** \brief This constraint class contains methods for accessing the properties of a single chain IK constraint.
  * \nosubgrouping
  */
class KFBX_DLL KFbxConstraintSingleChainIK : public KFbxConstraint
{
    KFBXOBJECT_DECLARE(KFbxConstraintSingleChainIK,KFbxConstraint);

public:
        /**
          * \name Properties
          */
        //@{            
            /** This property handles pole vector type.
              */
            KFbxTypedProperty<fbxEnum>        PoleVectorType;

            /** This property handles solver type.
              */
            KFbxTypedProperty<fbxEnum>        SolverType;

            /** This property handles evaluate TS animation.
              */
            KFbxTypedProperty<fbxEnum>        EvaluateTSAnim;

            //KFbxTypedProperty<fbxReference> PoleVectorObjectWeights;

            /** This property handles pole vector objects.
              */
            KFbxTypedProperty<fbxReference>    PoleVectorObjects;

            /** This property handles pole vector.
              *
              * Default value is (0, 1, 0).
              */
            KFbxTypedProperty<fbxDouble3>    PoleVector;

            /** This property handles twist value.
              *
              * Default value is 0.
              */
            KFbxTypedProperty<fbxDouble1>    Twist;

            /** This property handles first joint object.
              */
            KFbxTypedProperty<fbxReference> FirstJointObject;

            /** This property handles end joint object.
              */
            KFbxTypedProperty<fbxReference> EndJointObject;

            /** This property handles effector object.
              */
            KFbxTypedProperty<fbxReference> EffectorObject;
        //@}

    /** \enum ESolverType Pole vector type.
      * - \e eRP_SOLVER
      * - \e eSC_SOLVER
      */
    typedef enum 
    {
        eRP_SOLVER,
        eSC_SOLVER
    } ESolverType;

    /** \enum EPoleVectorType Pole vector type.
      * - \e ePOLE_VECTOR_TYPE_VECTOR
      * - \e ePOLE_VECTOR_TYPE_OBJECT
      */
    typedef enum 
    {
        ePOLE_VECTOR_TYPE_VECTOR,
        ePOLE_VECTOR_TYPE_OBJECT
    } EPoleVectorType;

	/** \enum EEvalTS If the constraints read its animation on Translation and Scale for the nodes it constraints.
	  * - \e eEVALTS_NEVER
	  * - \e eEVALTS_AUTODETECT
	  * = \e eEVALTS_ALWAYS
	  */
	typedef enum
	{
		eEVAL_TS_NEVER,
		eEVAL_TS_AUTO_DETECT,
		eEVAL_TS_ALWAYS
	} EEvalTS;

    /** Set the constraint lock.
      * \param pLock     State of the lock flag.
      */
    void SetLock(bool pLock);

    /** Retrieve the constraint lock state.
      * \return     Current lock flag.
      */
    bool GetLock() const;

    /** Set the Pole Vector type.
      * \param pType     New type for the pole vector.
      */
    void SetPoleVectorType(EPoleVectorType pType);

    /** Retrieve the pole vector type.
      * \return     Current pole vector type.
      */
    EPoleVectorType GetPoleVectorType() const;

    /** Set the Solver type.
      * \param pType     New type for the solver.
      */
    void SetSolverType(ESolverType pType);

    /** Retrieve the solver type.
      * \return     Current solver type.
      */
    ESolverType GetSolverType() const;

    /** Sets the EvalTS
      * \param pEval     New type of EvalTS 
      */
    void SetEvalTS(EEvalTS pEval);
    
    /** Retrieve the EvalTS
      * \return     The current EvalTS type
      */
    EEvalTS GetEvalTS() const;

    /** Set the weight of the constraint.
      * \param pWeight     New weight value.
      */
    void SetWeight(double pWeight);

    /** Get the weight of the constraint.
      * \return     The current weight value.
      */
    double GetWeight() const;

    
//    void AddPoleVectorObjectWeight(double pWeight);

    /** Get the weight of a source.
      * \param pObject     Source object that we want the weight.
      */
    double GetPoleVectorObjectWeight(const KFbxObject* pObject) const;

    /** Add a source to the constraint.
      * \param pObject     New source object.
      * \param pWeight     Weight value of the source object expressed as a percentage.
      * \remarks           pWeight value is 100 percent by default.
      */
    void AddPoleVectorObject(KFbxObject* pObject, double pWeight = 100);

    /** Retrieve the constraint source count.
      * \return     Current constraint source count.
      */
    int GetConstraintPoleVectorCount() const;

    /** Retrieve a constraint source object.
      * \param pIndex     Index of constraint source object.
      * \return           Current source at the specified index.
      */
    KFbxObject* GetPoleVectorObject(int pIndex) const;

    /** Set the pole vector.
      * \param pVector     New pole vector.
      */
    void SetPoleVector(KFbxVector4 pVector);

    /** Retrieve the pole vector.
      * \return     Current pole vector.
      */
    KFbxVector4 GetPoleVector() const;

    /** Set the twist value.
    * \param pTwist    New twist value.
    */
    void SetTwist(double pTwist);

    /** Retrieve the twist value.
      * \return     Current twist value.
      */
    double GetTwist() const;

    /** Set the first joint object.
      * \param pObject     The first joint object.
      */
    void SetFirstJointObject(KFbxObject* pObject);

    /** Retrieve the first joint object.
      * \return Current first joint object.
      */
    KFbxObject* GetFirstJointObject() const;

    /** Set the end joint object.
      * \param pObject     The end joint object.
      */
    void SetEndJointObject(KFbxObject* pObject);

    /** Retrieve the end joint object.
      * \return     Current end joint object.
      */
    KFbxObject* GetEndJointObject() const;

    /** Set the effector object.
      * \param pObject     The effector object.
      */
    void SetEffectorObject(KFbxObject* pObject);

    /** Retrieve the effector object.
      * \return     Current effector object.
      */
    KFbxObject* GetEffectorObject() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    KFbxConstraintSingleChainIK(KFbxSdkManager& pManager, char const* pName);

    virtual bool ConstructProperties( bool pForceSet );

    virtual EConstraintType GetConstraintType() const;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

inline EFbxType FbxTypeOf( KFbxConstraintSingleChainIK::EPoleVectorType const &pItem )                { return eENUM; }
inline EFbxType FbxTypeOf( KFbxConstraintSingleChainIK::ESolverType const &pItem )                { return eENUM; }
inline EFbxType FbxTypeOf( KFbxConstraintSingleChainIK::EEvalTS const &pItem )                { return eENUM; }


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXCONSTRAINTSINGLECHAINIK_H

