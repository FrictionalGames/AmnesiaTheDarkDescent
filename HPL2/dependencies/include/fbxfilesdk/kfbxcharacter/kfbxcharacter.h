/*!  \file kfbxcharacter.h
 */

#ifndef FBXFILESDK_KFBXCHARACTER_KFBXCHARACTER_H
#define FBXFILESDK_KFBXCHARACTER_KFBXCHARACTER_H

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

#include <fbxfilesdk/kfbxmath/kfbxvector4.h>

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/components/kcharacterdef/kfiltersetenum.h>

#include <fbxfilesdk/kfbxplugins/kfbxconstraint.h>
#include <fbxfilesdk/kfbxplugins/kfbxnodelimits.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxNode;
class KFbxControlSet;
class KFbxSdkManager;

/** \enum ECharacterInputType		Character input type.
  * - \e eCharacterInputActor		Not supported.
  * - \e eCharacterInputCharacter	The character's input is another character.
  * - \e eCharacterInputMarkerSet	The character's input is a control rig.
  * - \e eCharacterOutputMarkerSet	Not supported.
  * - \e eCharacterInputStance		The character's input is the stance pose.
  */
typedef enum 
{ 
	eCharacterInputActor = 0, 
	eCharacterInputCharacter = 1, 
	eCharacterInputMarkerSet = 2, 
	eCharacterOutputMarkerSet = 3, 
	eCharacterInputStance = 4 
} ECharacterInputType;

/** \enum ECharacterNodeId Define ID for each character node.
 */
typedef enum  
{       
	eCharacterHips = 0,             //!< Required
	eCharacterLeftHip,              //!< Required
	eCharacterLeftKnee,             //!< Required
	eCharacterLeftAnkle,            //!< Required
	eCharacterLeftFoot,
	eCharacterRightHip,             //!< Required
	eCharacterRightKnee,            //!< Required
	eCharacterRightAnkle,           //!< Required
	eCharacterRightFoot,
	eCharacterWaist,                //!< Spine0, Required
	eCharacterChest,                //!< Spine1
	eCharacterLeftCollar,
	eCharacterLeftShoulder,         //!< Required
	eCharacterLeftElbow,            //!< Required
	eCharacterLeftWrist,            //!< Required
	eCharacterRightCollar,
	eCharacterRightShoulder,        //!< Required
	eCharacterRightElbow,           //!< Required
	eCharacterRightWrist,           //!< Required
	eCharacterNeck,
	eCharacterHead,                 //!< Required
	eCharacterLeftHipRoll,
	eCharacterLeftKneeRoll,
	eCharacterRightHipRoll,
	eCharacterRightKneeRoll,
	eCharacterLeftShoulderRoll,
	eCharacterLeftElbowRoll,
	eCharacterRightShoulderRoll,
	eCharacterRightElbowRoll,
	eCharacterSpine2,
	eCharacterSpine3,
	eCharacterSpine4,
	eCharacterSpine5,
	eCharacterSpine6,
	eCharacterSpine7,
	eCharacterSpine8,
	eCharacterSpine9,
	eCharacterLeftThumbA,
	eCharacterLeftThumbB,
	eCharacterLeftThumbC,
	eCharacterLeftIndexA,
	eCharacterLeftIndexB,
	eCharacterLeftIndexC,
	eCharacterLeftMiddleA,
	eCharacterLeftMiddleB,
	eCharacterLeftMiddleC,
	eCharacterLeftRingA,
	eCharacterLeftRingB,
	eCharacterLeftRingC,
	eCharacterLeftPinkyA,
	eCharacterLeftPinkyB,
	eCharacterLeftPinkyC,
	eCharacterRightThumbA,
	eCharacterRightThumbB,
	eCharacterRightThumbC,
	eCharacterRightIndexA,
	eCharacterRightIndexB,
	eCharacterRightIndexC,
	eCharacterRightMiddleA,
	eCharacterRightMiddleB,
	eCharacterRightMiddleC,
	eCharacterRightRingA,
	eCharacterRightRingB,
	eCharacterRightRingC,
	eCharacterRightPinkyA,
	eCharacterRightPinkyB,
	eCharacterRightPinkyC,
	eCharacterReference,
	eCharacterLeftFloor,
	eCharacterRightFloor,
	eCharacterHipsTranslation,
	eCharacterProps0,
	eCharacterProps1,
	eCharacterProps2,
	eCharacterProps3,
	eCharacterProps4,

	// Added in 4.0x as patch.

	eCharacterGameModeParentLeftHipRoll,
	eCharacterGameModeParentLeftKnee,
	eCharacterGameModeParentLeftKneeRoll,
	eCharacterGameModeParentRightHipRoll,
	eCharacterGameModeParentRightKnee,
	eCharacterGameModeParentRightKneeRoll,
	eCharacterGameModeParentLeftShoulderRoll,       
	eCharacterGameModeParentLeftElbow,      
	eCharacterGameModeParentLeftElbowRoll,  
	eCharacterGameModeParentRightShoulderRoll,
	eCharacterGameModeParentRightElbow,             
	eCharacterGameModeParentRightElbowRoll, 


	// Elements added for Dominus.
	// From this point, the enum doesn't have to be synchronized with the HumanIK library.

	eCharacterLeftHandFloor,
	eCharacterRightHandFloor,

	eCharacterLeftHand,
	eCharacterRightHand,

	eCharacterNeck1,
	eCharacterNeck2,
	eCharacterNeck3,
	eCharacterNeck4,
	eCharacterNeck5,
	eCharacterNeck6,
	eCharacterNeck7,
	eCharacterNeck8,
	eCharacterNeck9,

	eCharacterLeftInHandThumb,
	eCharacterLeftThumbD,

	eCharacterLeftInHandIndex,
	eCharacterLeftIndexD,

	eCharacterLeftInHandMiddle,
	eCharacterLeftMiddleD,

	eCharacterLeftInHandRing,
	eCharacterLeftRingD,

	eCharacterLeftInHandPinky,
	eCharacterLeftPinkyD,

	eCharacterLeftInHandExtraFinger,
	eCharacterLeftExtraFingerA,
	eCharacterLeftExtraFingerB,
	eCharacterLeftExtraFingerC,
	eCharacterLeftExtraFingerD,

	eCharacterRightInHandThumb,
	eCharacterRightThumbD,

	eCharacterRightInHandIndex,
	eCharacterRightIndexD,

	eCharacterRightInHandMiddle,
	eCharacterRightMiddleD,

	eCharacterRightInHandRing,
	eCharacterRightRingD,

	eCharacterRightInHandPinky,
	eCharacterRightPinkyD,

	eCharacterRightInHandExtraFinger,
	eCharacterRightExtraFingerA,
	eCharacterRightExtraFingerB,
	eCharacterRightExtraFingerC,
	eCharacterRightExtraFingerD,

	eCharacterLeftInFootThumb,
	eCharacterLeftFootThumbA,
	eCharacterLeftFootThumbB,
	eCharacterLeftFootThumbC,
	eCharacterLeftFootThumbD,
	eCharacterLeftInFootIndex,
	eCharacterLeftFootIndexA,
	eCharacterLeftFootIndexB,
	eCharacterLeftFootIndexC,
	eCharacterLeftFootIndexD,
	eCharacterLeftInFootMiddle,
	eCharacterLeftFootMiddleA,
	eCharacterLeftFootMiddleB,
	eCharacterLeftFootMiddleC,
	eCharacterLeftFootMiddleD,
	eCharacterLeftInFootRing,
	eCharacterLeftFootRingA,
	eCharacterLeftFootRingB,
	eCharacterLeftFootRingC,
	eCharacterLeftFootRingD,
	eCharacterLeftInFootPinky,
	eCharacterLeftFootPinkyA,
	eCharacterLeftFootPinkyB,
	eCharacterLeftFootPinkyC,
	eCharacterLeftFootPinkyD,
	eCharacterLeftInFootExtraFinger,
	eCharacterLeftFootExtraFingerA,
	eCharacterLeftFootExtraFingerB,
	eCharacterLeftFootExtraFingerC,
	eCharacterLeftFootExtraFingerD,

	eCharacterRightInFootThumb,
	eCharacterRightFootThumbA,
	eCharacterRightFootThumbB,
	eCharacterRightFootThumbC,
	eCharacterRightFootThumbD,
	eCharacterRightInFootIndex,
	eCharacterRightFootIndexA,
	eCharacterRightFootIndexB,
	eCharacterRightFootIndexC,
	eCharacterRightFootIndexD,
	eCharacterRightInFootMiddle,
	eCharacterRightFootMiddleA,
	eCharacterRightFootMiddleB,
	eCharacterRightFootMiddleC,
	eCharacterRightFootMiddleD,
	eCharacterRightInFootRing,
	eCharacterRightFootRingA,
	eCharacterRightFootRingB,
	eCharacterRightFootRingC,
	eCharacterRightFootRingD,
	eCharacterRightInFootPinky,
	eCharacterRightFootPinkyA,
	eCharacterRightFootPinkyB,
	eCharacterRightFootPinkyC,
	eCharacterRightFootPinkyD,
	eCharacterRightInFootExtraFinger,
	eCharacterRightFootExtraFingerA,
	eCharacterRightFootExtraFingerB,
	eCharacterRightFootExtraFingerC,
	eCharacterRightFootExtraFingerD,
	eCharacterLeftCollarExtra,
	eCharacterRightCollarExtra,

	eCharacterLastNodeId
} ECharacterNodeId;

/** \enum ECharacterGroupId Define ID for character groups that contains multiple character nodes.
 */
typedef enum 
{
	eCharacterGroup_Base,
	eCharacterGroup_Auxiliary,
	eCharacterGroup_Spine,
	eCharacterGroup_Roll,
	eCharacterGroup_Special,
	eCharacterGroup_LeftHand,
	eCharacterGroup_RightHand,
	eCharacterGroup_Props,
	eCharacterGroup_GameModeParent,

	// Added for 4.5 

	eCharacterGroup_Neck,
	eCharacterGroup_LeftFoot,
	eCharacterGroup_RightFoot,

	// Tempo Bug #266854 >>>>>
	eCharacterGroup_FloorContact,
	// Tempo Bug #266854 <<<<<

	eLastCharacterGroupId
} ECharacterGroupId;

/** \enum ECharacterLinkType 
*/
typedef enum 
{
	eCharacterLink,
	eControlSetLink,
	eControlSetEffector,
	eControlSetEffectorAux
} ECharacterLinkType;

/** \class KFbxCharacterLink
  *
  * \brief This class represents a link between a given FBX node and the associated node in the character hierarchy. It also contains
  *  the transform matrix (offset) for the linked character's node.
  */
class KFBX_DLL KFbxCharacterLink
{
public:
    /** Default Constructor.
     */
    KFbxCharacterLink();
    /** Copy Constructor.
     */
    KFbxCharacterLink(const KFbxCharacterLink& pCharacterLink);
    
    /** Assignment operation
     * \param pCharacterLink Another KFbxCharacterLink object assigned to this one.
     */
    KFbxCharacterLink& operator=(const KFbxCharacterLink& pCharacterLink);
    
    /** Reset to default values.
     */
    void Reset();

	//! The character's node in hierarchy linked to this character link.
    KFbxNode* mNode;

	//! A template name is a naming convention that is used to automatically map
	//! the nodes of other skeletons that use the same naming convention.
    KString mTemplateName;

	//! Get offset position of this character link.
    KFbxVector4 mOffsetT;
	//! Get offset rotation of this character link.
    KFbxVector4 mOffsetR;
	//! Get offset scale of this character link.
    KFbxVector4 mOffsetS;
    //! Get the parent offset rotation of this character link
    KFbxVector4 mParentROffset;

    //! \c true if this character link has a defined rotation space
    bool            mHasRotSpace;
    //! Get the rotation limits of this character link
    KFbxLimits  mRLimits;
    //! Get the PreRotation of this character link
    KFbxVector4 mPreRotation;
    //! Get the PostRotation of this character link
    KFbxVector4 mPostRotation;
    //! Get the rotation order of this character link
    int                     mRotOrder;
    //! Get the axis length of this character link
    double          mAxisLen;


///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//      Anything beyond these lines may not be documented accurately and is 
//      subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS        
    KFbxProperty mPropertyLink;
    KFbxProperty mPropertyOffsetT;
    KFbxProperty mPropertyOffsetR;
    KFbxProperty mPropertyOffsetS;
    KFbxProperty mPropertyParentOffsetR;
    KFbxProperty mPropertyTemplateName;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

/** 
  * Character is person or animal with pre-defined skeleton system. The skeleton system is composed of multiple named node (skeleton).
  * This class contains all methods to setup an exported character or query information on an imported character.
  * This class also contains some methods for manipulating the KFbxCharacterLink, KFbxControlSet
  * 
  * The most important part of a KFbxCharacter is the KFbxCharacterLink. There is one KFbxCharacterLink for each characterized node.
  * For more information see KFbxCharacterLink class documentation.
  *
  * \see KFbxCharacterLink, KFbxControlSet
  */
class KFBX_DLL KFbxCharacter : public KFbxConstraint
{
        KFBXOBJECT_DECLARE(KFbxCharacter,KFbxConstraint);

public:

        /** Reset to default values. 
         *     - Input type will be set to eCharacterInputStance.
         *     - Input object will be set to NULL.
         *     - Each Character link will be reset.
         *     - The control set will be reset.
         */
        void Reset();

        /** Set input type and index.
          * \param pInputType       Input type.
          * \param pInputObject     Pointer to input character if input type equals eCharacterInputCharacter, otherwise \c NULL.
          */
        void SetInput(ECharacterInputType pInputType, KFbxObject* pInputObject = NULL);

        //! Get input type.
        ECharacterInputType GetInputType() const;

        /** Get input actor or character.
          * \return     Pointer or \c Null, depending on the input type.
          *                  - If the input type is set to eCharacterInputCharacter. The returned pointer can be casted to a pointer of type KFbxCharacter.
          *                  - \c Null pointer if the input object has not been set, or if the input type is not set to eCharacterInputCharacter.
          */
        KFbxObject* GetInputObject() const;

        /** Associate a character link to a given character node ID. If a character link already exists for this character node ID,
          * the character link will be removed.
          * \param pCharacterNodeId      Character node ID.
          * \param pCharacterLink        Character link.
          * \param pUpdateObjectList     Set to \c true to update the object list (default value).
          * \return                      \c true if successful, \c false otherwise.
          */
        bool SetCharacterLink(ECharacterNodeId pCharacterNodeId, const KFbxCharacterLink& pCharacterLink, bool pUpdateObjectList = true);

        /** Get a character link associated with a given character node ID.
          * \param pCharacterNodeId     ID of character node requested.
          * \param pCharacterLink       Optional pointer to receive the character link if function succeeds.
          * \return                     \c true if successful, \c false otherwise.
          */
        bool GetCharacterLink(ECharacterNodeId pCharacterNodeId, KFbxCharacterLink* pCharacterLink = NULL) const;

        /** Get control set associated with the character.
         * \return     Return the control set associated with the character.
         */
        KFbxControlSet& GetControlSet() const;

        /** Get number of elements in a given character group.
          * \param pCharacterGroupId     Character group ID.
          * \return                      The number of elements in the pCharacterGroupId character group.
          */
        static int GetCharacterGroupCount(ECharacterGroupId pCharacterGroupId);

        /** Get character node ID of an element in a given character group.
          * \param pCharacterGroupId     Character group ID.
          * \param pIndex                Character index ID.
          * \return                      Character node ID.
          */
        static ECharacterNodeId GetCharacterGroupElementByIndex(ECharacterGroupId pCharacterGroupId, int pIndex);

    
        /** Get character node name of an element in a given character group.
          * \param pCharacterGroupId     Character group ID.
          * \param pIndex                Character index ID.
          * \return                      Character node name.
          */
        static char* GetCharacterGroupNameByIndex(ECharacterGroupId pCharacterGroupId, int pIndex);

        /** Get character node version of an element in a given character group.
          * \param pCharacterGroupId     Character group ID.
          * \param pIndex                Character index ID.
          * \return                      Character node version.
          */
        static int GetCharacterGroupVersionByIndex(ECharacterGroupId pCharacterGroupId, int pIndex);

        /** Find the character group index associated with a given character node name.
          * \param pName                 Character node name.
          * \param pForceGroupId         Set to \c true to force the character group ID.
          * \param pCharacterGroupId     Receives character group ID.
          * \param pIndex                Receives character index ID.
          * \return                      \c true if successful, otherwise \c false.
          */
        static bool FindCharacterGroupIndexByName(const char* pName, bool pForceGroupId, ECharacterGroupId& pCharacterGroupId, int& pIndex);

        /** Get character node group and index of a given character node ID.
          * \param pCharacterNodeId     Character node ID.
          * \param pCharacterGroupId    if the Character node ID is found, the method returns the group ID through this parameter
          * \param pIndex               if the Character node ID is found, the method returns the index through this parameter
          * \remarks                    Only works for a character node ID that is part of a group.
          * \return                     \c true if successful, \c false otherwise.
          */
        static bool GetCharacterGroupIndexByElement(ECharacterNodeId pCharacterNodeId, ECharacterGroupId& pCharacterGroupId, int& pIndex);

        /** Get character node version of a given character node ID.
          * \param pCharacterNodeId     Character node ID to get version.
          * \param pVersion             if the node ID is found, the method returns the version through this parameter
          * \remarks                    Only works for a character node ID is part of a group.
          * \return                     \c true if successful, \c false otherwise.
          */
        static bool GetCharacterGroupVersionByElement(ECharacterNodeId pCharacterNodeId, int& pVersion);

        /** Get character node name associated with a given character node ID.
          * \param pCharacterNodeId     Character node ID to get name.
          * \param pName                if the node ID is found, the method returns the node name through this parameter
          *                             Since the Pointer points to internal data, it is not necessary to allocate a string buffer 
          *                             before calling this function.
          * \return                     \c true if a name exists for the given node ID.
          */
        static bool GetCharacterNodeNameFromNodeId(ECharacterNodeId pCharacterNodeId, char*& pName);

        /** Get the character node ID associated with a given character node name.
      * \param pName                Character node name to get node ID.
      * \param pCharacterNodeId     if the node name is found, this method returns the node ID through this parameter
      * \return                     \c true if a node ID exists for the given node name.
          */
        static bool GetCharacterNodeIdFromNodeName(const char* pName, ECharacterNodeId& pCharacterNodeId);

        typedef enum 
        {
                eINTERNAL_ERROR,
                eERROR_COUNT
        } EError; 
        
        // KFbxCharacter Properties
        KFbxTypedProperty<fbxInteger1>                          PullIterationCount;
        KFbxTypedProperty<kCharacterPosture>                    Posture;
        KFbxTypedProperty<fbxBool1>                             ForceActorSpace;
        KFbxTypedProperty<fbxDouble1>                           ScaleCompensation;
        KFbxTypedProperty<kCharacterOffAutoUser>                ScaleCompensationMode;
        KFbxTypedProperty<fbxDouble1>                           HipsHeightCompensation;
        KFbxTypedProperty<kCharacterOffAutoUser>                HipsHeightCompensationMode;
        KFbxTypedProperty<fbxDouble1>                           AnkleHeightCompensation;
        KFbxTypedProperty<kCharacterOffAutoUser>                AnkleHeightCompensationMode;
        KFbxTypedProperty<fbxDouble1>                           AnkleProximityCompensation;
        KFbxTypedProperty<kCharacterOffAutoUser>                AnkleProximityCompensationMode;
        KFbxTypedProperty<fbxDouble1>                           MassCenterCompensation;
        KFbxTypedProperty<fbxBool1>                             ApplyLimits;
        KFbxTypedProperty<fbxDouble1>                           ChestReduction;
        KFbxTypedProperty<fbxDouble1>                           CollarReduction;
        KFbxTypedProperty<fbxDouble1>                           NeckReduction;
        KFbxTypedProperty<fbxDouble1>                           HeadReduction;
        KFbxTypedProperty<fbxDouble1>                           ReachActorLeftAnkle;
        KFbxTypedProperty<fbxDouble1>                           ReachActorRightAnkle;
        KFbxTypedProperty<fbxDouble1>                           ReachActorLeftKnee;
        KFbxTypedProperty<fbxDouble1>                           ReachActorRightKnee;
        KFbxTypedProperty<fbxDouble1>                           ReachActorChest;
        KFbxTypedProperty<fbxDouble1>                           ReachActorHead;
        KFbxTypedProperty<fbxDouble1>                           ReachActorLeftWrist;
        KFbxTypedProperty<fbxDouble1>                           ReachActorRightWrist;
        KFbxTypedProperty<fbxDouble1>                           ReachActorLeftElbow;
        KFbxTypedProperty<fbxDouble1>                           ReachActorRightElbow;
        KFbxTypedProperty<fbxDouble1>                           ReachActorLeftFingerBase;
        KFbxTypedProperty<fbxDouble1>                           ReachActorRightFingerBase;
        KFbxTypedProperty<fbxDouble1>                           ReachActorLeftToesBase;
        KFbxTypedProperty<fbxDouble1>                           ReachActorRightToesBase;
        KFbxTypedProperty<fbxDouble1>                           ReachActorLeftFingerBaseRotation;
        KFbxTypedProperty<fbxDouble1>                           ReachActorRightFingerBaseRotation;
        KFbxTypedProperty<fbxDouble1>                           ReachActorLeftToesBaseRotation;
        KFbxTypedProperty<fbxDouble1>                           ReachActorRightToesBaseRotation;
        KFbxTypedProperty<fbxDouble1>                           ReachActorLeftAnkleRotation;
        KFbxTypedProperty<fbxDouble1>                           ReachActorRightAnkleRotation;
        KFbxTypedProperty<fbxDouble1>                           ReachActorHeadRotation;
        KFbxTypedProperty<fbxDouble1>                           ReachActorLeftWristRotation;
        KFbxTypedProperty<fbxDouble1>                           ReachActorRightWristRotation;
        KFbxTypedProperty<fbxDouble1>                           ReachActorChestRotation;
        KFbxTypedProperty<fbxDouble1>                           ReachActorLowerChestRotation;
        KFbxTypedProperty<fbxDouble3>                           HipsTOffset;
        KFbxTypedProperty<fbxDouble3>                           ChestTOffset;
        KFbxTypedProperty<kCharacterRollExtractionMode>         RollExtractionMode;
        KFbxTypedProperty<fbxDouble1>                           LeftUpLegRoll;
        KFbxTypedProperty<fbxBool1>                             LeftUpLegRollMode;
        KFbxTypedProperty<fbxDouble1>                           LeftLegRoll;
        KFbxTypedProperty<fbxBool1>                             LeftLegRollMode;
        KFbxTypedProperty<fbxDouble1>                           RightUpLegRoll;
        KFbxTypedProperty<fbxBool1>                             RightUpLegRollMode;
        KFbxTypedProperty<fbxDouble1>                           RightLegRoll;
        KFbxTypedProperty<fbxBool1>                             RightLegRollMode;
        KFbxTypedProperty<fbxDouble1>                           LeftArmRoll;
        KFbxTypedProperty<fbxBool1>                             LeftArmRollMode;
        KFbxTypedProperty<fbxDouble1>                           LeftForeArmRoll;
        KFbxTypedProperty<fbxBool1>                             LeftForeArmRollMode;
        KFbxTypedProperty<fbxDouble1>                           RightArmRoll;
        KFbxTypedProperty<fbxBool1>                             RightArmRollMode;
        KFbxTypedProperty<fbxDouble1>                           RightForeArmRoll;
        KFbxTypedProperty<fbxBool1>                             RightForeArmRollMode;
        KFbxTypedProperty<kCharacterContactBehaviour>           ContactBehaviour;
        KFbxTypedProperty<fbxBool1>                             FootFloorContact;
        KFbxTypedProperty<fbxBool1>                             FootAutomaticToes;
        KFbxTypedProperty<kCharacterFloorPivot>                 FootFloorPivot;
        KFbxTypedProperty<fbxDouble1>                           FootBottomToAnkle;
        KFbxTypedProperty<fbxDouble1>                           FootBackToAnkle;
        KFbxTypedProperty<fbxDouble1>                           FootMiddleToAnkle;
        KFbxTypedProperty<fbxDouble1>                           FootFrontToMiddle;
        KFbxTypedProperty<fbxDouble1>                           FootInToAnkle;
        KFbxTypedProperty<fbxDouble1>                           FootOutToAnkle;
        KFbxTypedProperty<fbxDouble1>                           FootContactSize;
        KFbxTypedProperty<fbxBool1>                             FootFingerContact;
        KFbxTypedProperty<kCharacterFootContactType>            FootContactType;
        KFbxTypedProperty<kCharacterFingerContactMode>          FootFingerContactMode;
        KFbxTypedProperty<fbxDouble1>                           FootContactStiffness;
        KFbxTypedProperty<fbxDouble1>                           FootFingerContactRollStiffness;
        KFbxTypedProperty<fbxBool1>                             HandFloorContact;
        KFbxTypedProperty<fbxBool1>                             HandAutomaticFingers;
        KFbxTypedProperty<kCharacterFloorPivot>                 HandFloorPivot;
        KFbxTypedProperty<fbxDouble1>                           HandBottomToWrist;
        KFbxTypedProperty<fbxDouble1>                           HandBackToWrist;
        KFbxTypedProperty<fbxDouble1>                           HandMiddleToWrist;
        KFbxTypedProperty<fbxDouble1>                           HandFrontToMiddle;
        KFbxTypedProperty<fbxDouble1>                           HandInToWrist;
        KFbxTypedProperty<fbxDouble1>                           HandOutToWrist;
        KFbxTypedProperty<fbxDouble1>                           HandContactSize;
        KFbxTypedProperty<fbxBool1>                             HandFingerContact;
        KFbxTypedProperty<kCharacterHandContactType>            HandContactType;
        KFbxTypedProperty<kCharacterFingerContactMode>          HandFingerContactMode;
        KFbxTypedProperty<fbxDouble1>                           HandContactStiffness;
        KFbxTypedProperty<fbxDouble1>                           HandFingerContactRollStiffness;
        KFbxTypedProperty<fbxDouble1>                           LeftHandThumbTip;
        KFbxTypedProperty<fbxDouble1>                           LeftHandIndexTip;
        KFbxTypedProperty<fbxDouble1>                           LeftHandMiddleTip;
        KFbxTypedProperty<fbxDouble1>                           LeftHandRingTip;
        KFbxTypedProperty<fbxDouble1>                           LeftHandPinkyTip;
        KFbxTypedProperty<fbxDouble1>                           LeftHandExtraFingerTip;
        KFbxTypedProperty<fbxDouble1>                           RightHandThumbTip;
        KFbxTypedProperty<fbxDouble1>                           RightHandIndexTip;
        KFbxTypedProperty<fbxDouble1>                           RightHandMiddleTip;
        KFbxTypedProperty<fbxDouble1>                           RightHandRingTip;
        KFbxTypedProperty<fbxDouble1>                           RightHandPinkyTip;
        KFbxTypedProperty<fbxDouble1>                           RightHandExtraFingerTip;
        KFbxTypedProperty<fbxDouble1>                           LeftFootThumbTip;
        KFbxTypedProperty<fbxDouble1>                           LeftFootIndexTip;
        KFbxTypedProperty<fbxDouble1>                           LeftFootMiddleTip;
        KFbxTypedProperty<fbxDouble1>                           LeftFootRingTip;
        KFbxTypedProperty<fbxDouble1>                           LeftFootPinkyTip;
        KFbxTypedProperty<fbxDouble1>                           LeftFootExtraFingerTip;
        KFbxTypedProperty<fbxDouble1>                           RightFootThumbTip;
        KFbxTypedProperty<fbxDouble1>                           RightFootIndexTip;
        KFbxTypedProperty<fbxDouble1>                           RightFootMiddleTip;
        KFbxTypedProperty<fbxDouble1>                           RightFootRingTip;
        KFbxTypedProperty<fbxDouble1>                           RightFootPinkyTip;
        KFbxTypedProperty<fbxDouble1>                           RightFootExtraFingerTip;
        KFbxTypedProperty<fbxBool1>                             FingerSolving;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullLeftToeBase;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullLeftFoot;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullLeftKnee;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullRightToeBase;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullRightFoot;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullRightKnee;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullLeftFingerBase;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullLeftHand;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullLeftElbow;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullRightFingerBase;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullRightHand;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullRightElbow;
        KFbxTypedProperty<fbxDouble1>                           CtrlChestPullLeftHand;
        KFbxTypedProperty<fbxDouble1>                           CtrlChestPullRightHand;
        KFbxTypedProperty<fbxDouble1>                           CtrlPullHead;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistHipsPosition;
        KFbxTypedProperty<fbxDouble1>                           CtrlEnforceGravity;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistHipsOrientation;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistChestPosition;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistChestOrientation;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistLeftCollar;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistRightCollar;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistLeftKnee;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistMaximumExtensionLeftKnee;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistCompressionFactorLeftKnee;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistRightKnee;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistMaximumExtensionRightKnee;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistCompressionFactorRightKnee;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistLeftElbow;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistMaximumExtensionLeftElbow;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistCompressionFactorLeftElbow;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistRightElbow;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistMaximumExtensionRightElbow;
        KFbxTypedProperty<fbxDouble1>                           CtrlResistCompressionFactorRightElbow;
        KFbxTypedProperty<fbxDouble1>                           CtrlSpineStiffness;
        KFbxTypedProperty<fbxDouble1>                           CtrlNeckStiffness;
        KFbxTypedProperty<fbxBool1>                             MirrorMode;
        KFbxTypedProperty<fbxDouble1>                           ShoulderCorrection;
        KFbxTypedProperty<fbxBool1>                             LeftKneeKillPitch;
        KFbxTypedProperty<fbxBool1>                             RightKneeKillPitch;
        KFbxTypedProperty<fbxBool1>                             LeftElbowKillPitch;
        KFbxTypedProperty<fbxBool1>                             RightElbowKillPitch;
        KFbxTypedProperty<kCharacterHipsTranslationMode>        HipsTranslationMode;
        KFbxTypedProperty<fbxBool1>                             WriteReference;
        KFbxTypedProperty<fbxBool1>                             SyncMode;
        KFbxTypedProperty<fbxDouble1>                           Damping;
        KFbxTypedProperty<fbxDouble1>                           OrientationDamping;
        KFbxTypedProperty<kCharacterOffAutoUser>                OrientationDampingMode;
        KFbxTypedProperty<fbxDouble1>                           DisplacementDamping;
        KFbxTypedProperty<kCharacterOffAutoUser>                DisplacementDampingMode;
        KFbxTypedProperty<fbxDouble1>                           DisplacementMemory;
        KFbxTypedProperty<kCharacterAutoUser>                   DisplacementMemoryMode;
        KFbxTypedProperty<fbxDouble1>                           HipsDisplacementDamping;
        KFbxTypedProperty<kCharacterAutoUser>                   HipsDisplacementDampingMode;
        KFbxTypedProperty<fbxDouble1>                           AnkleDisplacementDamping;
        KFbxTypedProperty<kCharacterAutoUser>                   AnkleDisplacementDampingMode;
        KFbxTypedProperty<fbxDouble1>                           WristDisplacementDamping;
        KFbxTypedProperty<kCharacterAutoUser>                   WristDisplacementDampingMode;
        KFbxTypedProperty<fbxDouble1>                           Stabilization;
        KFbxTypedProperty<fbxDouble1>                           AnkleStabilizationTime;
        KFbxTypedProperty<kCharacterAutoUser>                   AnkleStabilizationTimeMode;
        KFbxTypedProperty<fbxDouble1>                           AnkleStabilizationPerimeter;
        KFbxTypedProperty<kCharacterAutoUser>                   AnkleStabilizationPerimeterMode;
        KFbxTypedProperty<fbxDouble1>                           AnkleStabilizationAngularPerimeter;
        KFbxTypedProperty<kCharacterOffAutoUser>                AnkleStabilizationAngularPerimeterMode;
        KFbxTypedProperty<fbxDouble1>                           AnkleStabilizationFloorProximity;
        KFbxTypedProperty<kCharacterOffAutoUser>                AnkleStabilizationFloorProximityMode;
        KFbxTypedProperty<fbxDouble1>                           AnkleStabilizationDamping;
        KFbxTypedProperty<kCharacterOffAutoUser>                AnkleStabilizationDampingMode;
        KFbxTypedProperty<fbxDouble1>                           AnkleStabilizationRecoveryTime;
        KFbxTypedProperty<kCharacterOffAutoUser>                AnkleStabilizationRecoveryTimeMode;

    KFbxTypedProperty<fbxReference*>                    SourceObject;
    KFbxTypedProperty<fbxReference*>                    DestinationObject;
    KFbxTypedProperty<fbxReference*>                    Actor;
    KFbxTypedProperty<fbxReference*>                    Character;
    KFbxTypedProperty<fbxReference*>                    ControlSet;

    KFbxTypedProperty<fbxDouble1>                          HikVersion;
    KFbxTypedProperty<fbxBool1>                            Characterize;
    KFbxTypedProperty<fbxBool1>                            LockXForm;
    KFbxTypedProperty<fbxBool1>                            LockPick;
    
///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//      Anything beyond these lines may not be documented accurately and is 
//      subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS        

        virtual KFbxObject& Copy(const KFbxObject& pObject);

        KFbxCharacter(KFbxSdkManager& pManager, char const* pName);

        virtual void Construct(const KFbxCharacter* pFrom);
        virtual void Destruct(bool pRecursive, bool pDependents);

        virtual EConstraintType GetConstraintType() const;

        virtual KStringList     GetTypeFlags() const;

        virtual bool    ConnecNotify (KFbxConnectEvent const &pEvent);

        // Used when bridging to HumanIK Middleware
        const void GetKFbxCharacterPropertyFromHIKProperty(     char* &pKFbxCharacterPropertyName, 
                                                                char* &pKFbxCharacterPropertyModeName,  
                                                                kCharacterPropertyUnit &pUnit,
                                                                int &pPropertyIndex,
                                                                const char* pHIKPropertyName) const;
        
        const void GetHIKPropertyFromKFbxCharacterProperty(     char* &pHIKPropertyName,                                                                                                                
                                                                kCharacterPropertyUnit &pUnit,
                                                                const char* pKFbxCharacterPropertyName) const ;

        const void GetPropertyInfo(     char* &pKFbxCharacterPropertyName, 
                                                                char* &pKFbxCharacterPropertyModeName,  
                                                                kCharacterPropertyUnit &pUnit,
                                                                int &pPropertyIndex,
                                                                char* &pHIKPropertyName,
                                                                int pIndex) const;
        int GetPropertyInfoCount();

        KError                  mError;

        KString                 mName;  
        KFbxCharacterLink       mCharacterLink[eCharacterLastNodeId];
        KFbxControlSet*         mControlSet;
        
        friend class KFbxScene;
        friend class KFbxNode;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

protected:
        
};

typedef KFbxCharacter* HKFbxCharacter;

inline EFbxType FbxTypeOf( kCharacterOffAutoUser const &pItem )             { return eENUM; }
inline EFbxType FbxTypeOf( kCharacterAutoUser const &pItem )                { return eENUM; }
inline EFbxType FbxTypeOf( kCharacterPosture const &pItem )                 { return eENUM; }
inline EFbxType FbxTypeOf( kCharacterFloorPivot const &pItem )              { return eENUM; }
inline EFbxType FbxTypeOf( kCharacterRollExtractionMode const &pItem )      { return eENUM; }
inline EFbxType FbxTypeOf( kCharacterHipsTranslationMode const &pItem )     { return eENUM; }
inline EFbxType FbxTypeOf( kCharacterFootContactType const &pItem )         { return eENUM; }
inline EFbxType FbxTypeOf( kCharacterHandContactType const &pItem )         { return eENUM; }
inline EFbxType FbxTypeOf( kCharacterFingerContactMode const &pItem )       { return eENUM; }
inline EFbxType FbxTypeOf( kCharacterContactBehaviour const &pItem )        { return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXCHARACTER_KFBXCHARACTER_H

