/*!  \file kfbxlodgroup.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXLODGROUP_H
#define FBXFILESDK_KFBXPLUGINS_KFBXLODGROUP_H

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

/** Defines a LOD (Level of Detail) group.
  * This LodGroup node is a group node that can be used to detect how
  * close a group of objects is to a camera. Typically this node is
  * used for controlling "Level of Detail" visibility.
  *
  * Properties in the class are designed according to Maya implementation.
  * So these properties may be incompatible with other software, like 3ds Max.
  *
  * In Maya, with "Level of Detail",the visibility of the children of this transform are controlled
  * by the distance of a group to a camera and the threshold values.
  * For example, under a LOD group node, there are three children:
  * ship_detailed, ship_medium, and ship_rough.  There are three
  * threshold values: 5, 10 and 15.  When the camera is within
  * 5 units of the group bounding box, only ship_detailed is visible.
  * When the view is zoomed out and the camera is 9 units away from
  * the group, only ship_medium is visible.  When the view is
  * zoomed out to 30 units away, only ship_rough is visible.
  *
  * This node attribute contains the properties of a null node.
  *
  * Example code to create LODGroup:
  *	\code
  * KFbxNode *lLodGroup = KFbxNode::Create(pScene, "LODNode");
  * KFbxLodGroup *lLodGroupAttr = KFbxLodGroup::Create(pScene, "LODGroup1");
  * // Array lChildNodes contains geometries of all LOD levels
  * for (int j = 0; j < lChildNodes.GetCount(); j++)
  * {
  *    lLodGroup->AddChild(lChildNodes.GetAt(j));
  * }
  * \endcode
  * \nosubgrouping
  */
class KFBX_DLL KFbxLodGroup : public KFbxNodeAttribute
{
    KFBXOBJECT_DECLARE(KFbxLodGroup,KFbxNodeAttribute);

public:
    //! Return the type of node attribute which is EAttributeType::eLODGROUP.
    virtual EAttributeType GetAttributeType() const;

    /** \enum EDisplayLevel types to determine how to display nodes in LODGroup.
      * - \e eUseLOD Display the node according LOD threshold
      * - \e eShow Always show this node
	  * - \e eHide Always hide this node
      */
	typedef enum {
		eUseLOD,
		eShow,
		eHide
	} EDisplayLevel;

    //////////////////////////////////////////////////////////////////////////
    //
    // Properties
    //
    //////////////////////////////////////////////////////////////////////////

    /** This property handles the use of the Min/Max distances.
      * Enables the minimum and maximum distance to take effect.
      * For example, if the distance between the group and the camera is smaller 
      * than the minimum distance, then the whole group disappears.
      *
      * To access this property do: MinMaxDistance.Get().
      * To set this property do: MinMaxDistance.Set(bool).
      *
      * Default value is false.
      */
    KFbxTypedProperty<fbxBool1>       MinMaxDistance;

    /** The minimum distance at which the group is displayed.
      *
      * To access this property do: MinDistance.Get().
      * To set this property do: MinDistance.Set(double).
      *
      * Default value is -100
      */
    KFbxTypedProperty<fbxDouble1>     MinDistance;

    /** The maximum distance at which the group is displayed. 
      *
      * To access this property do: MaxDistance.Get().
      * To set this property do: MaxDistance.Set(double).
      *
      * Default value is 100
      */
    KFbxTypedProperty<fbxDouble1>     MaxDistance;

    /** Work in world space of transform or local space If true, 
      * the camera distance to the LOD group will be computed in world space.
      * This means it is possible to parent the LOD transform below other transforms 
      * and still have it work as expected. If this attribute is set to false,
      * the distance computation ignores any parent transforms of the LOD transform. 
      *
      * To access this property do: WorldSpace.Get().
      * To set this property do: WorldSpace.Set(bool).
      *
      * Default value is false
      */
	KFbxTypedProperty<fbxBool1>       WorldSpace;


    //////////////////////////////////////////////////////////////////////////
    //
    // Methods
    //
    //////////////////////////////////////////////////////////////////////////

	/** Get the size of the threshold list.
	  * In correct situation, the size is less one than LOD nodes number.
	  * \return The current size of the threshold list.
	  */
	int GetNumThresholds() const;

	/** Add a new threshold value to the current list. 
	  * \param pThreshValue Threshold distance from the previous entry in the threshold list
	  * \remarks This list can only expand so the only way to shrink it, is
	  * to completely destroy this object.
	  * \remarks This method does not check the received values and blindly add them
	  * to the list. Therefore duplicated values can exist in different positions in
	  * the list.
	  * \return true if successful and false if any error occurred.
	  */
	bool AddThreshold(fbxDistance pThreshValue);

	/** Set the threshold value for the specified object.
	  * \param pEl The index of the object we want to set the threshold.
	  * \param pThreshValue Threshold distance from the previous entry in the threshold list
	  * \return true if successful and false if the specified index is invalid.
	  */
	bool SetThreshold(int pEl, fbxDistance pThreshValue);

	/** Get the threshold value for the specified object.
      * \param pEl The index of the object we want to get the threshold.
	  * \param pThreshValue the current threshold value.
	  * \return true if successful and false if the specified index is invalid.
	  * \remarks in case of a failure, the pThreshValue is left unchanged.
	  */
	bool GetThreshold(int pEl, fbxDistance& pThreshValue) const;


	/** Get the size of the displayLevel list.
	  * In correct situation, the size is equal to LOD nodes number.
	  * \return The current size of the displayLevel list.
	  */
	int GetNumDisplayLevels() const;

	/** Add a new displayLevel value to the current list.
	  *
	  * The value overrides the display of any level and can force it to hide or show the object at that level.
	  * For example, if the distance between the group and the camera is smaller than the first threshold, 
	  * then the object at level 0 is visible. If the display level for the object at level 2 is changed to eShow,
	  * ie. if the attribute displayLevel[2] is set to eShow, then the object at level 2 will show, regardless of the current active level. 
	  *
	  * \param pValue Display level value
	  * \remarks This list can only expand so the only way to shrink it, is
	  * to completely destroy this object.
	  * \remarks This method does not check the received values and blindly add them
	  * to the list. Therefore duplicated values can exist in different positions in
	  * the list.
	  * \return true if successful and false if any error occurred.
	  */
	bool AddDisplayLevel(KFbxLodGroup::EDisplayLevel pValue);

	/** Set the display level value for the specified object.
	  * \param pEl The index of the object we want to set the display level.
	  * \param pValue New display level value
	  * \return true if successful and false if the specified index is invalid.
	  */
	bool SetDisplayLevel(int pEl, KFbxLodGroup::EDisplayLevel pValue);

	/** Get the display level value for the specified object.
      * \param pEl The index of the object we want to get the display level value.
	  * \param pValue the current display level value.
	  * \return true if successful and false if the specified index is invalid.
	  * \remarks in case of a failure, the pValue is left unchanged.
	  */
	bool GetDisplayLevel(int pEl, KFbxLodGroup::EDisplayLevel& pValue) const;
	
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
    KFbxLodGroup(KFbxSdkManager& pManager, char const* pName);
    virtual bool ConstructProperties(bool pForceSet);

private:
	int mNbThresholds;
	KFbxProperty mThresholds;	
	bool Threshold(int pEl, fbxDistance pThreshValue, bool pCreateNew);

	int mNbDisplayLevels;
	KFbxProperty mDisplayLevels;
	bool DisplayLevel(int pEl, KFbxLodGroup::EDisplayLevel pValue, bool pCreateNew);

public:
    virtual KStringList GetTypeFlags() const;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef KFbxLodGroup* HKFbxLodGroup;
inline EFbxType FbxTypeOf( KFbxLodGroup::EDisplayLevel const &pItem )           { return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXLODGROUP_H

