/*!  \file kfbxcontrolset.h
 */

#ifndef FBXFILESDK_KFBXCHARACTER_KFBXCONTROLSET_H
#define FBXFILESDK_KFBXCHARACTER_KFBXCONTROLSET_H

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

#include <fbxfilesdk/kfbxcharacter/kfbxcharacter.h>
#include <fbxfilesdk/kfbxmath/kfbxxmatrix.h>
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxNode;
class KFbxSdkManager;
class KFbxControlSetPlug;
class KFbxControlSetPlug_internal;

typedef enum
{
    eEffectorHips = 0,
    eEffectorLeftAnkle,
    eEffectorRightAnkle,
    eEffectorLeftWrist,
    eEffectorRightWrist,
    eEffectorLeftKnee,
    eEffectorRightKnee,
    eEffectorLeftElbow,
    eEffectorRightElbow,
    eEffectorChestOrigin,
    eEffectorChestEnd,
    eEffectorLeftFoot,
    eEffectorRightFoot,
    eEffectorLeftShoulder,
    eEffectorRightShoulder,
    eEffectorHead,
    eEffectorLeftHip,
    eEffectorRightHip,

    // Added for 4.5

    eEffectorLeftHand,
    eEffectorRightHand,

    eEffectorLeftHandThumb,
    eEffectorLeftHandIndex,
    eEffectorLeftHandMiddle,
    eEffectorLeftHandRing,
    eEffectorLeftHandPinky,
    eEffectorLeftHandExtraFinger,

    eEffectorRightHandThumb,
    eEffectorRightHandIndex,
    eEffectorRightHandMiddle,
    eEffectorRightHandRing,
    eEffectorRightHandPinky,
    eEffectorRightHandExtraFinger,

    eEffectorLeftFootThumb,
    eEffectorLeftFootIndex,
    eEffectorLeftFootMiddle,
    eEffectorLeftFootRing,
    eEffectorLeftFootPinky,
    eEffectorLeftFootExtraFinger,

    eEffectorRightFootThumb,
    eEffectorRightFootIndex,
    eEffectorRightFootMiddle,
    eEffectorRightFootRing,
    eEffectorRightFootPinky,
    eEffectorRightFootExtraFinger,

    eEffectorLastNodeId
} EEffectorNodeId;

typedef enum
{
    eEffectorSetDefault = 0,
    eEffectorSetAux1,
    eEffectorSetAux2,
    eEffectorSetAux3,
    eEffectorSetAux4,
    eEffectorSetAux5,
    eEffectorSetAux6,
    eEffectorSetAux7,
    eEffectorSetAux8,
    eEffectorSetAux9,
    eEffectorSetAux10,
    eEffectorSetAux11,
    eEffectorSetAux12,
    eEffectorSetAux13,
    eEffectorSetAux14,
    eLastEffectorSetId
} EEffectorSetId;


/** \class KFbxControlSetLink
  *
  * \brief This class represents a link between a given character's FK node and the associated node in the character hierarchy.
  *
  */
class KFBX_DLL KFbxControlSetLink
{

public:

	//! Default constructor.
    KFbxControlSetLink();

	/** Copy constructor.
	* \param pControlSetLink Given object.
	*/
    KFbxControlSetLink(const KFbxControlSetLink& pControlSetLink);
	/** Assignment operator.
	  * \param pControlSetLink Another KFbxControlSetLink object assigned to this one.
	  */
    KFbxControlSetLink& operator=(const KFbxControlSetLink& pControlSetLink);

    /** Reset to default values.
      *
      * Member mNode is set to \c NULL and member mTemplateName is cleared.
      */
    void Reset();

    //! The character's node in a hierarchy linked to this control set link.
    KFbxNode* mNode;

    //! A template name is a naming convention that is used to automatically map
    //! the nodes of other skeletons that use the same naming convention for automatic characterization.
    KString mTemplateName;
};

/** 
  * An effector wraps a character node (KFbxNode) used to animate its control rig (KFbxControlSet) via inverse kinematics.
  */
class KFBX_DLL KFbxEffector
{

public:

	//! Default constructor with uninitialized character node.
    KFbxEffector();
	/** Assignment operator.
	  * \param pEffector Another KFbxEffector assigned to this one.
	  */
    KFbxEffector& operator=(const KFbxEffector& pEffector);

    /** Reset to default values.
      *     - mNode is set to NULL.
      *     - mShow is set to true.
      */
    void Reset();

    //! The character's node in a hierarchy linked to this effector.
    KFbxNode* mNode;

    //! \c true if the effector is visible, \c false if hidden
    bool mShow;

    // These members are for backward compatibility and should not be used. These properties are now published through class KFbxControlSetPlug.
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    bool mTActive;
    bool mRActive;
    bool mCandidateTActive;
    bool mCandidateRActive;
#endif
};

/** \class KFbxControlSet
  *
  * This class contains all methods to either set-up an exported control rig or query information on an imported control rig.
  * A Control rig is a character manipulation tool that lets you change the position and orientation
  * of a character to create or alter animation.
  *
  * This class also contains some methods to manipulate the KFbxEffector and KFbxControlSetLink.
  *
  * The KFbxControlSet class contains FK rig (Forward Kinematics) and IK rig (Inverse Kinematics) animation. The FK rig is represented
  * by a list of nodes while the IK rig is represented by a list of effectors.
  *
  * You can access the FK rig with the KFbxControlSetLink class, using the functions KFbxControlSet::SetControlSetLink() and KFbxControlSet::GetControlSetLink().
  *
  * You can access the IK rig with the KFbxEffector class, using the functions KFbxControlSet::SetEffector() and KFbxControlSet::GetEffector().
  *
  * \see KFbxEffector, KFbxControlSetLink
  */
class KFBX_DLL KFbxControlSet
{

public:
    KFBXNEW_DECLARE_FRIEND
    /** Reset to default values.
      * Reset all effector and control set links.
      */
    void Reset();

    /** \enum EType      Control rig type.
      * - \e eNone       No Control rig.
      * - \e eFkIk       Both an FK rig and IK rig.
      * - \e eIkOnly     Only an IK rig.
      */
    enum EType
    {
        eNone,
        eFkIk,
        eIkOnly
    };

    /** Set type as given.
	* \param pType The given type.
	*/
    void SetType(EType pType);

   	/** Get type.
	* \return The gotten type.
	*/
    EType GetType() const;

    /** Set use axis flag as given.
	* \param pUseAxis The given use axis flag.
	*/
    void SetUseAxis(bool pUseAxis);

    /** Get use axis flag.
	* \return The gotten use axis flag.
	*/
    bool GetUseAxis() const;

   	/** Set lock transform flag as given.
	* \param pLockTransform The given lock transform flag.
	*/
    void SetLockTransform(bool pLockTransform);

    /** Get lock transform flag.
	* \return The gotten lock transform flag.
	*/
    bool GetLockTransform()const;

   /** Set lock 3D pick flag as given.
	* \param pLock3DPick The given lock 3D pick flag.
	*/
    void SetLock3DPick(bool pLock3DPick);

   	/** Get lock 3D pick flag.
	* \return The gotten lock 3D pick flag.
	*/
    bool GetLock3DPick() const;

    /** Set a control set link for a character node ID.
      * \param pCharacterNodeId     Character node ID.
      * \param pControlSetLink      Control set link to be associated with the Character node ID.
      * \return                     \c true if successful, \c false otherwise.
      * \remarks                    You should avoid setting a control set link for
      *                             eCharacterLeftFloor, eCharacterRightFloor, eCharacterLeftHandFloor, eCharacterRightHandFloor,
      *                             eCharacterProps0, eCharacterProps1, eCharacterProps2, eCharacterProps3 or eCharacterProps4.
      *                             None of these nodes are part of a control set.
      */
    bool SetControlSetLink(ECharacterNodeId pCharacterNodeId, const KFbxControlSetLink& pControlSetLink);

    /** Get the control set link associated with a character node ID.
      * \param pCharacterNodeId     Requested character node ID.
      * \param pControlSetLink      Optional pointer that returns the control set link if the function succeeds.
      * \return                     \c true if successful, \c false otherwise.
      * \remarks                    You should avoid getting a control set link for
      *                             eCharacterLeftFloor, eCharacterRightFloor, eCharacterLeftHandFloor, eCharacterRightHandFloor,
      *                             eCharacterProps0, eCharacterProps1, eCharacterProps2, eCharacterProps3 or eCharacterProps4.
      *                             None of these nodes are part of a control set.
      */
    bool GetControlSetLink(ECharacterNodeId pCharacterNodeId, KFbxControlSetLink* pControlSetLink = NULL) const;

    /** Set an effector node for an effector node ID.
      * \param pEffectorNodeId     Effector node ID.
      * \param pEffector           Effector to be associated with the effector node ID.
      * \return                    \c true if successful, \c false otherwise.
      */
    bool SetEffector(EEffectorNodeId pEffectorNodeId, KFbxEffector pEffector);

    /** Get the effector associated with an effector node ID.
      * \param pEffectorNodeId     ID of requested effector node.
      * \param pEffector           Optional pointer that returns the effector if the function succeeds.
      * \return                    \c true if successful, \c false otherwise.
      */
    bool GetEffector(EEffectorNodeId pEffectorNodeId, KFbxEffector* pEffector = NULL);

    /** Set an auxiliary effector node for an effector node ID.
      * \param pEffectorNodeId     Effector node ID.
      * \param pNode               Auxiliary effector node to be associated with the effector node ID.
      * \param pEffectorSetId      Effector set ID. Set to eEffectorSetAux1 by default.
      * \return                    \c true if successful, \c false otherwise.
      */
    bool SetEffectorAux(EEffectorNodeId pEffectorNodeId, KFbxNode* pNode,EEffectorSetId pEffectorSetId = eEffectorSetAux1);

    /** Get the auxiliary effector associated with an effector node ID.
      * \param pEffectorNodeId     ID of requested auxiliary effector node.
      * \param pNode               Optional pointer that returns the auxiliary effector node if the function succeeds.
      * \param pEffectorSetId      Effector set ID. Set to eEffectorSetAux1 by default.
      * \return                    \c true if successful, \c false otherwise.
      */
    bool GetEffectorAux(EEffectorNodeId pEffectorNodeId, KFbxNode** pNode = NULL,EEffectorSetId pEffectorSetId = eEffectorSetAux1) const;

    /** Get the name associated with an effector node ID.
      * \param pEffectorNodeId     Effector node ID.
      * \return                    Name associated with the effector node ID.
      */
    static char* GetEffectorNodeName(EEffectorNodeId pEffectorNodeId);

    /** Get ID associated with an effector node name.
      * \param pEffectorNodeName     Effector node name.
      * \return                      Effector node ID associated with the given effector node name, or -1 if
      *                              no effector node with pEffectorNodeName exists.
      */
    static EEffectorNodeId GetEffectorNodeId(char* pEffectorNodeName);


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

    KFbxControlSet();
    ~KFbxControlSet();

    KFbxCharacter* mCharacter;
    EType mType;
    bool mUseAxis;
    bool mLockTransform;
    bool mLock3DPick;
    KFbxControlSetLink mControlSetLink[eCharacterLastNodeId]; // Except floor node IDs!
    KFbxEffector mEffector[eEffectorLastNodeId];
    KFbxNode* mEffectorAux[eEffectorLastNodeId][eLastEffectorSetId-1];

    friend class KFbxCharacter;
    friend class KFbxNode;

public:

    void FromPlug(KFbxControlSetPlug *pPlug);
    void ToPlug(KFbxControlSetPlug *pPlug);

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

/** Plug class for control set.
  * \nosubgrouping
  */
class KFBX_DLL KFbxControlSetPlug : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxControlSetPlug,KFbxObject);
public:

	    //! Type property of control set.
        KFbxTypedProperty<KFbxControlSet::EType>    ControlSetType;
	    //! Use axis flag.
        KFbxTypedProperty<bool>                     UseAxis;
		//! Reference character.
        KFbxTypedProperty<fbxReference*>            Character;

        /**
          * \name Error Management
          * The same error object is shared among instances of this class.
          */
        //@{

        /** Retrieve error object.
          * \return Reference to error object.
          */
        KError& GetError();

        //! Error identifiers.
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

    protected:
        KFbxControlSetPlug(KFbxSdkManager& pManager, char const* pName);

        virtual void Construct(const KFbxControlSetPlug* pFrom);

	public:
        virtual KStringList GetTypeFlags() const;

	protected:
        KArrayTemplate<KFbxProperty>    mFKBuf;
        KArrayTemplate<KFbxProperty>    mIKBuf;

        KError mError;

        friend class KFbxScene;
        friend class KFbxControlSet;
};

typedef KFbxControlSetPlug* HKFbxControlSetPlug;
inline EFbxType FbxTypeOf( KFbxControlSet::EType const &pItem ) { return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXCHARACTER_KFBXCONTROLSET_H

