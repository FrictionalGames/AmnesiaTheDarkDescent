/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxnode.h
 */
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXNODE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXNODE_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxplugins/kfbxnodelimits.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>

#include <fbxfilesdk/kfbxmath/kfbxtransformation.h>
#include <fbxfilesdk/kfbxmath/kfbxvector4.h>
#include <fbxfilesdk/kfbxmath/kfbxmatrix.h>
#include <fbxfilesdk/kfbxmath/kfbxxmatrix.h>

#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/kerror.h>
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/ktime.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

    class KFbxNodeAttribute;
    class KFbxScene;
    class KFbxAnimStack;
    class KFbxAnimLayer;
    class KFbxSdkManager;
    class KFbxCachedEffect;
	class KFbxLodGroup;
    class KFbxNull;
    class KFbxMarker;
    class KFbxSkeleton;
    class KFbxGeometry;
    class KFbxMesh;
    class KFbxNurb;
    class KFbxNurbsCurve;
    class KFbxLine;
    class KFbxNurbsSurface;
    class KFbxTrimNurbsSurface;
    class KFbxPatch;
    class KFbxCamera;
    class KFbxCameraStereo;
    class KFbxCameraSwitcher;
    class KFbxLight;
    class KFbxOpticalReference;
    class KFbxCharacter;
    class KFbxNode_internal;
    class KFbxSurfaceMaterial;
    class KFbxSubdiv;
	class KFbxNodeEvalState;
	class KMBTransform;

    /** Represents an element in the scene graph. A scene graph is a tree of KFbxNodes 
	  * objects. The tree management services are self contained in this class.
	  *
	  * \note The FBX SDK does not test the validity of the constructed scene graph. It 
	  * is the responsibility of the caller to make sure that it does not generate cyclic 
	  * graphs in a node hierarchy.
      *
	  * Besides the tree management, this class defines all the properties required to describe
	  * the position of the object in the scene. This information include the basic Translation,
	  * Rotation and Scaling properties and the more advanced options for pivots, limits, and IK joints
	  * attributes such the stiffness and dampening.
	  *
	  * When it is first created, the KFbxNode object is "empty" (i.e: it is an object without any 
	  * graphical representation that only contains the position information). In this state, it can
	  * be used to represent parents in the node tree structure but not much more. The normal use of
	  * this type of objects is to add them an attribute that will specialize the node (see the 
	  * "Node Attribute Management" section). 
	  *
	  * The node attribute is an object in itself and is connected to the the KFbxNode. This also 
	  * means that the same node attribute can be shared among multiple nodes. KFbxCamera, KFbxLight,
	  * KFbxMesh, etc... are all node attributes and they all derive from the base class KFbxNodeAttribute. 
	  *
      */
    class KFBX_DLL KFbxNode : public KFbxObject
    {
        KFBXOBJECT_DECLARE(KFbxNode,KFbxObject);
        public:
        /**
          * \name Node Tree Management
          */
        //@{

            /** Get the parent node.
              * \return Pointer to parent node or \c NULL if the current node has no parent.
              */
            KFbxNode* GetParent();
            KFbxNode const* GetParent() const;

            /** Add a child node and its underlying node tree.
              * \param pNode Node we want to make child of \c this.
              * \return \c true on success, \c false if \e pNode is \c NULL or the system is 
			  *            unable to make the connection.
              * \remarks If \e pNode already has a parent, first it is removed from current parent and then
			  *          added to this one.
              */
            bool AddChild(KFbxNode* pNode);

            /** Remove the child node.
              * \param pNode The child node to be removed.
              * \return The removed child node.
              */
            KFbxNode* RemoveChild(KFbxNode* pNode);

            /** Get the number of children nodes.
              * \param pRecursive If \c true the method will also count all the descendant children.
              * \return Total number of children for this node.
              */
            int GetChildCount(bool pRecursive = false) const;

            /** Get child by index.
			  * \param pIndex The child index.
              * \return Child node or \c NULL if \e pIndex is out of range (i.e: < 0 or > GetChildCount()).
              */
            KFbxNode* GetChild(int pIndex);

            /** Get child by index.
			  * \param pIndex The child index.
              * \return Child node or \c NULL if \e pIndex is out of range (i.e: < 0 or > GetChildCount()).
              */
            KFbxNode const* GetChild(int pIndex) const;

            /** Finds a child node by name.
              * \param pName Name of the searched child node.
              * \param pRecursive Flag to request recursive calls.
              * \param pInitial If set to \c true, the search compares the initial name of 
			  *                 the node (see the KFbxObject class)
              * \return Found child node or \c NULL if no child node with this name exists.
              */
            KFbxNode* FindChild(char const* pName, bool pRecursive = true, bool pInitial = false);

        //@}

        /**
          * \name Node Target Management
		  * The KFbxNode class allows the client to set a "follow" target node. This target 
		  * forces the node to re-align itself so it points to the target. By default, the node
		  * uses its X axis as the aiming constraint. A rotation offset can be added to change 
		  * this behavior. While the default relative orientation to the target (the X axis) is 
		  * sufficient for the FBX cameras (with a (0,0,0) rotation vector, they are aiming
		  * along the X axis), this rotation offset becomes particularly useful with the lights
		  * objects because their default orientation (when they have a 0,0,0 rotation vector) is to
		  * point along the -Y axis and they need to be adjusted with a 90-degree offset on the Z axis.
		  *
          * The KFbxNode class also permits the use of node to define an Up-vector. By default, 
		  * the node's up vector points towards the Up node. If the Up node is not specified, 
		  * then the node's Up vector points towards the Y axis. Here too, a rotation offset can be 
		  * added to change the default behavior. 
		  *
		  * Of course, these offsets can be applied to anything, not only the cameras and lights.
		  *
		  * \note Objects in the FBX SDK are always created in the right handed, Y-Up system and need 
		  *       to be adjusted for any other axis system by explicitly convert them (the class 
		  *       KFbxAxisSystem can help in that process).
		  *       
          */
        //@{

            /** The target must be part of the same scene and it cannot be itself.
              * \param pNode The target.
              */
            void SetTarget(KFbxNode* pNode);

            /** Get the target for this node.
              * \returns \c NULL if target isn't set.
              */
            KFbxNode* GetTarget() const;

            /** Set rotation offset from default relative orientation to target.
              * \param pVector The rotation offset.
              */
            void SetPostTargetRotation(KFbxVector4 pVector);

            /** Get rotation offset from default relative orientation to target.
              * \return The rotation offset.
              */
            KFbxVector4 GetPostTargetRotation() const;

            /** The target up node must be part of the same scene and it cannot be itself.
              * \param pNode The target.
              */
            void SetTargetUp(KFbxNode* pNode);

            /** Get the target up node.
              * \return \c NULL if the target up model isn't set.
              */
            KFbxNode* GetTargetUp() const;

            /** Set up vector offset from default relative target up vector.
              * \param pVector The rotation offset.
              */
            void SetTargetUpVector(KFbxVector4 pVector);

            /** Get up vector offset from default relative target up vector.
              * \return The up vector offset.
              */
            KFbxVector4 GetTargetUpVector() const;

        //@}


        /**
          * \name UpdateId Management
          */
        //@{
        public:
            virtual kFbxUpdateId GetUpdateId(eFbxUpdateIdType pUpdateId=eUpdateId_Object) const;
        //@}

        /**
          * \name Node Display Parameters
          */
        //@{
            /** This is a DEPRECATED utility method that can be used to set the value of the Visibility property. 
              * Checks for the validity of the property before attempting to set its value. 
              * \param pVisibility Although the argument is a double number and its value is not checked, the 
              *                    FBX SDK assumes that this argument value is in the range [0.0 - 1.0]. 
			  *                    The value 0.0 meaning totally invisible and anything else meaning visible.
              *                    Applications can also use this value as a percentage of the visibility.
              * \remarks This method has been deprecated since the FBX SDK promotes manipulating the properties
              *          directly. The exact same result can be achieved using the following code:
			  * \code
			  * KFbxNode* node; // we suppose this is a valid pointer
			  * if (node->Visibility.IsValid())
			  *     KFbxSet <double> (node->Visibility, pVisibility);
			  * \endcode
              * \see Visibility property.
              */
            K_DEPRECATED void SetDefaultVisibility(double pVisibility);

            /** Set the node Visibility value from the boolean parameter. 
              * \param pIsVisible Node is visible in the scene if set to \c true.
              * \remarks This method checks for the validity of the property before attempting to 
			  * set its value. In fact, the exact same result can be achieved by the following code:
              * \code
			  * if (Visibility.IsValid())
              * {
              *     fbxDouble1 val(pIsVisible);
              *     KFbxSet <fbxDouble1> (Visibility, val);
			  * }
			  * \endcode
              *
              * \see Visibility property.
              */
            void SetVisibility(bool pIsVisible);

            /** This is a DEPRECATED utility method that can be used to get the value of the Visibility property.
              * Checks for the validity of the property before attempting to set its value. 
              * \remarks This method has been deprecated since the FBX SDK promotes manipulating the properties
              *          directly. The exact same result can be achieved using the following code:
			  * \code
			  * KFbxNode* node; // we suppose this is a valid pointer
			  * double lVisibility = 0.0;
			  * if (node->Visibility.IsValid())
              *     lVisibility = KFbxGet<double>(node->Visibility);
			  * \endcode
              * \return The value stored in the Visibility property or 0.0 if the property is invalid. 
              */
            K_DEPRECATED double GetDefaultVisibility() const;

            /** Get the current value of the Visibility property.
              * \return \c false if the Visibility property value is 0.0 and \c true for any other value.
			  * \remarks This method expects the Visibility property to exist and to be valid. If this 
			  *          condition is not met, the returned value will be \c false.
              */
            bool GetVisibility() const;

            /** \enum EShadingMode Shading modes.
			  * These shading modes are not directly used by the FBX SDK but it is guaranteed that the information is
			  * carried to and from the FBX files. The typical context of using these modes is to affect the rendering of 
			  * geometric objects (this is, of course, performed at the application level) and the possible definition
			  * for each mode is:
              */
            typedef enum
            {
                eHARD_SHADING,				/*!<	Solid geometries rendered with smooth surfaces - using the system light. */
                eWIRE_FRAME,				/*!<	Geometries displayed in wire frame. */
                eFLAT_SHADING,				/*!<	Solid geometries rendered faceted - using the system light. */
                eLIGHT_SHADING,				/*!<	Solid geometries rendered with the scene lights. */
                eTEXTURE_SHADING,			/*!<	Solid geometries rendered with smooth textured surfaces - using system light. */
                eLIGHT_TEXTURE_SHADING		/*!<	Solid geometries rendered with smooth textured surfaces and scene lights. */
            } EShadingMode;

            /** Set the shading mode.
              * \param pShadingMode The shading mode.
              */
            void SetShadingMode(EShadingMode pShadingMode);

            /** Get the shading mode.
              * \return The currently set shading mode.
              */
            EShadingMode GetShadingMode() const;

        //@}

        /**
          * \name Node Attribute Management
          */
        //@{

            /** Set the node attribute.
              * \param pNodeAttribute Node attribute object
              * \return Pointer to previous node attribute object.
              * \c NULL if the node didn't have a node attribute or if
              * the new node attribute is equal to the one currently set.
              * \remarks A node attribute can be shared between nodes.
              * \remarks If this node has more than one attribute (added via the AddAttribute() method), this call
			  * will destroy all, but the default node attribute.
              */
            KFbxNodeAttribute* SetNodeAttribute(KFbxNodeAttribute* pNodeAttribute);

            /** Get the default node attribute.
			  * The default node attribute is the attribute that has been set by the call to SetNodeAttribute().
              * \return Pointer to the default node attribute or \c NULL if the node doesn't
              * have a node attribute.
              */
            KFbxNodeAttribute* GetNodeAttribute();

            /** Get the default node attribute.
			  * The default node attribute is the attribute that has been set by the call to SetNodeAttribute(...).
              * \return Pointer to the default node attribute or \c NULL if the node doesn't
              * have a node attribute.
              */
            KFbxNodeAttribute const* GetNodeAttribute() const;

            //! Get the number of node attribute(s) connected to this node.
            int GetNodeAttributeCount() const;

            /** Get the index, in the list of connected node attributes, of the node attribute that is set
			  * to be the default one.
              * \return Index of the default node attribute or \c -1 if there is no default node attribute set.
              */
            int GetDefaultNodeAttributeIndex() const;

            /** Set index of the default node attribute.
			  * \param pIndex Identifies which of the connected node attributes is becoming the default one.
			  *               This value represent the connection number of the node.
              * \return \c true if the operation succeeds or \c false if the passed index is invalid.
              * In this case, KFbxNode::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
              */
            bool SetDefaultNodeAttributeIndex(int pIndex);

            /** Get the connected node attribute by specifying its index in the connection list.
			  * \param pIndex The connection number of the node.
              * \return Pointer to corresponding node attribute or \c NULL if the index is out of range.
              * In this case, KFbxNode::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
              */
            KFbxNodeAttribute* GetNodeAttributeByIndex(int pIndex);

            /** Get the connected node attribute by specifying its index in the connection list.
			  * \param pIndex The connection number of the node.
              * \return Pointer to corresponding node attribute or \c NULL if the index is out of range.
              * In this case, KFbxNode::GetLastErrorID() returns eINDEX_OUT_OF_RANGE.
              */
            KFbxNodeAttribute const* GetNodeAttributeByIndex(int pIndex) const;

            /** Get the connection index of the specified node attribute.
			  * This method will do a linear search of all the connected node attributes (from the last to
			  * the first connection) until it finds \e pNodeAttribue.
              * \param pNodeAttribute The pointer to the node attribute.
              * \return The connection number of the node attribute or \c -1 if pNodeAttribute is \c NULL 
			  * or not connected to this node. For both failing conditions, KFbxNode::GetLastErrorID() 
			  * returns eATTRIBUTE_NOT_CONNECTED.
              */
            int GetNodeAttributeIndex(KFbxNodeAttribute* pNodeAttribute) const;

            /** Add the new node attribute to this node.
			  * If no other node attribute is already set as the default one, this new node attribute is
			  * automatically set as the default one.
              * \param pNodeAttribute The pointer to a node attribute.
              * \return \c true if the operation succeeded or \c false if the operation failed.
              * \remarks The failing conditions for this methods are:
			  *      - The received object pointer is \c NULL.
			  *      - The received object is already connected to this node.
			  *      - An internal error prevented the connection to successfully complete.
              */
            bool AddNodeAttribute(KFbxNodeAttribute* pNodeAttribute);

            /** Remove the node attribute from the connection list of this node.
              * \param pNodeAttribute The pointer to a node attribute.
              * \return Pointer to the removed node attribute or \c NULL if the operation failed.
              * In this case, KFbxNode::GetLastErrorID() returns eATTRIBUTE_NOT_CONNECTED.
              */
            KFbxNodeAttribute* RemoveNodeAttribute(KFbxNodeAttribute* pNodeAttribute);

            /** Remove the node attribute, specified by the connection index, from the connection 
			  * list of this node.
              * \param pIndex Index of the node attribute.
              * \return Pointer to the removed node attribute or \c NULL if the operation failed.
			  * \remarks If the specified node attribute is also the default one, its predecessor in
			  *          the connection list will become the new default node attribute. And if there 
			  *          are no more predecessors, the node DefaultNodeAttributeIndex is reset to -1.
              */
            KFbxNodeAttribute* RemoveNodeAttributeByIndex(int pIndex);

            /** Get the default node attribute casted to a KFbxCachedEffect pointer.
              * \return Pointer to the cached effect object. 
			  * \remarks If the type cast failed because there is not default node attribute set or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxCachedEffect* GetCachedEffect();

            /** Get the default node attribute casted to a KFbxLodGroup pointer.
              * \return Pointer to the lod group object. 
			  * \remarks If the type cast failed because there is not default node attribute set or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxLodGroup* GetLodGroup();

			/** Get the default node attribute casted to a KFbxNull pointer.
              * \return Pointer to the null object.
			  * \remarks If the type cast failed because there is not default node attribute set or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxNull* GetNull();

            /** Get the node attribute casted to a KFbxMarker pointer.
              * \return Pointer to the marker object.
			  * \remarks If the type cast failed because there is not default node attribute set or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxMarker* GetMarker();

            /** Get the node attribute casted to a KFbxSkeleton pointer.
              * \return Pointer to the skeleton object.
			  * \remarks If the type cast failed because there is not default node attribute set or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxSkeleton* GetSkeleton();

            /** Get the node attribute casted to a KFbxGeometry pointer.
              * \return Pointer to the geometry object.
			  * \remarks If the type cast failed because there is not default node attribute set or it cannot
			  *          be successfully casted, this method will return \c NULL.
			  * \remarks For this method to succeed, the node attribute's GetAttributeType() must returns one of the 
			  *          following:
			  *          - KFbxNodeAttribute::eMESH
              *          - KFbxNodeAttribute::eNURB
              *          - KFbxNodeAttribute::eNURBS_SURFACE
              *          - KFbxNodeAttribute::ePATCH
              *          - KFbxNodeAttribute::eNURBS_CURVE
              *          - KFbxNodeAttribute::eBOUNDARY
              *          - KFbxNodeAttribute::eTRIM_NURBS_SURFACE
              *          - KFbxNodeAttribute::eSUBDIV
              *          - KFbxNodeAttribute::eLINE
              */
            KFbxGeometry* GetGeometry();

            /** Get the node attribute casted to a KFbxMesh pointer.
              * \return Pointer to the mesh object.
			  * \remarks This method will try to process the default node attribute first. If it cannot
			  *          find it, it will scan the list of connected node attributes and get the first
			  *          object that is a KFbxNodeAttribute::eMESH.
			  * \remarks If the above search failed to get a valid pointer or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxMesh* GetMesh();

            /** Get the node attribute casted to a KFbxNurb pointer.
              * \return Pointer to the nurb object.
			  * \remarks This method will try to process the default node attribute first. If it cannot
			  *          find it, it will scan the list of connected node attributes and get the first
			  *          object that is a KFbxNodeAttribute::eNURB.
			  * \remarks If the above search failed to get a valid pointer or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxNurb* GetNurb();

            /** Get the node attribute casted to a KFbxNurbsSurface pointer.
              * \return Pointer to the nurbs surface object.
			  * \remarks This method will try to process the default node attribute first. If it cannot
			  *          find it, it will scan the list of connected node attributes and get the first
			  *          object that is a KFbxNodeAttribute::eNURBS_SURFACE.
			  * \remarks If the above search failed to get a valid pointer or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxNurbsSurface* GetNurbsSurface();

            /** Get the node attribute casted to a KFbxNurbsCurve pointer.
              * \return Pointer to the nurbs curve object.
			  * \remarks This method will try to process the default node attribute first. If it cannot
			  *          find it, it will scan the list of connected node attributes and get the first
			  *          object that is a KFbxNodeAttribute::eNURBS_CURVE.
			  * \remarks If the above search failed to get a valid pointer or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxNurbsCurve* GetNurbsCurve();

            /** Get the node attribute casted to a KFbxLine pointer.
            * \return Pointer to the line object.
            * \remarks This method will try to process the default node attribute first. If it cannot
            *          find it, it will scan the list of connected node attributes and get the first
            *          object that is a KFbxNodeAttribute::eLINE.
            * \remarks If the above search failed to get a valid pointer or it cannot
            *          be successfully casted, this method will return \c NULL.
            */
            KFbxLine* GetLine();

            /** Get the node attribute casted to a KFbxTrimNurbsSurface pointer.
              * \return Pointer to the trim nurbs surface object.
			  * \remarks This method will try to process the default node attribute first. If it cannot
			  *          find it, it will scan the list of connected node attributes and get the first
			  *          object that is a KFbxNodeAttribute::eTRIM_NURBS_SURFACE.
			  * \remarks If the above search failed to get a valid pointer or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxTrimNurbsSurface* GetTrimNurbsSurface();

            /** Get the node attribute casted to a KFbxSubdiv pointer.
              * \return Pointer to the subdivision surface object.
			  * \remarks This method will try to process the default node attribute first. If it cannot
			  *          find it, it will scan the list of connected node attributes and get the first
			  *          object that is a KFbxNodeAttribute::eSUBDIV.
			  * \remarks If the above search failed to get a valid pointer or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxSubdiv* GetSubdiv();

            /** Get the node attribute casted to a KFbxPatch pointer.
              * \return Pointer to the patch object.
			  * \remarks This method will try to process the default node attribute first. If it cannot
			  *          find it, it will scan the list of connected node attributes and get the first
			  *          object that is a KFbxNodeAttribute::ePATCH.
			  * \remarks If the above search failed to get a valid pointer or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxPatch* GetPatch();

            /** Get the node attribute casted to a KFbxCamera pointer.
              * \return Pointer to the camera object.
			  * \remarks If the type cast failed because there is not default node attribute set or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxCamera* GetCamera();
            const KFbxCamera* GetCamera() const;

            /** Get the node attribute casted to a KFbxCameraStereo pointer.
            * \return Pointer to the stereo camera object.
            * \remarks If the type cast failed because there is not default node attribute set or it cannot
            *          be successfully casted, this method will return \c NULL.
            */
            KFbxCameraStereo* GetCameraStereo();

            /** Get the node attribute casted to a KFbxCameraSwitcher pointer.
              * \return Pointer to the camera switcher object.
			  * \remarks If the type cast failed because there is not default node attribute set or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxCameraSwitcher* GetCameraSwitcher();

            /** Get the node attribute casted to a KFbxLight pointer.
              * \return Pointer to the light object.
			  * \remarks If the type cast failed because there is not default node attribute set or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxLight* GetLight();
            const KFbxLight * GetLight() const;

            /** Get the node attribute casted to a KFbxOpticalReference pointer.
              * \return Pointer to the optical reference object.
			  * \remarks If the type cast failed because there is not default node attribute set or it cannot
			  *          be successfully casted, this method will return \c NULL.
              */
            KFbxOpticalReference* GetOpticalReference();
        //@}

        /**
          * \name Transformation propagation
          * This set of functions provides direct access to the transformation propagations settings 
		  * of the KFbxNode. These settings determine how transformations must be applied when 
		  * evaluating a node's transformation matrix. The possible values are:
		  *     - eINHERIT_RrSs : Scaling of parent is applied in the child world after the local child rotation.
		  *     - eINHERIT_RSrs : Scaling of parent is applied in the parent world.
		  *     - eINHERIT_Rrs  : Scaling of parent does not affect the scaling of children.
          */
        //@{
            /** Sets how child transforms are inherited from parent transforms.
              * \param pInheritType One of the following values eINHERIT_RrSs, eINHERIT_RSrs or eINHERIT_Rrs
              */
            void SetTransformationInheritType(ETransformInheritType pInheritType);

            //! Get transformation inherit type.
            void GetTransformationInheritType(ETransformInheritType& pInheritType) const;
        //@}

        /**
          * \name Pivot Management
          * Pivots are used to specify translation, rotation and scaling centers in coordinates 
		  * relative to a node's origin. 
		  * A node has two pivot contexts defined by the EPivotSet enumeration. The node's animation 
		  * data can be converted from one pivot context to the other. Each context can be set to be
		  * either active or passive (reference). By default the two pivot contexts are passive. They
		  * need to be active to be processed during the evaluation of the node final transformation
		  * matrix. In its passive state, a pivot context can still be accessed to retrieve its content
		  * for any other required purpose. Each pivot context stores values (as KFbxVector4) for:
          * \code
		  *     - Rotation offset (Roff)
          *     - Rotation pivot (Rp)
		  *     - Pre-rotation (Rpre)
		  *     - Post-rotation (Rpost)
		  *     - Scaling offset (Soff)
		  *     - Scaling pivot (Sp)
		  *     - Geometric translation (Gt)
		  *     - Geometric rotation (Gr)
		  *     - Geometric scaling (Gs)
	      * 
		  * These values combine in the matrix form to compute the World transform of the node 
		  * using the formula:
		  *
          * 	World = ParentWorld * T * Roff * Rp * Rpre * R * Rpost * Rp-1 * Soff * Sp * S * Sp-1
          * \endcode
		  *
		  * The geometric transformation (Gt * Gr * Gs) is applied only to the node attribute and after 
		  * the node transformations. This transformation is not inherited across the node hierarchy.
          *
		  * \note Please refer to the FBX SDK programmers guide for more details.
		  * 
		  * The application of the pivots is performed by calling the method ConvertPivotAnimation(). Typically,
		  * you set-up the eDESTINATION_SET context to match what your system can directly support and leave at (0,0,0) the
		  * attributes that are not supported by your system. When the values of a specific attribute in the 
		  * two contexts (source and destination) are identical, the system considers that no adjustment is 
		  * required because the attribute is directly supported in the destination world.
		  *
		  * Below is an example of code that shows how the pivot information could be setup before calling ConvertPivotAnimation(). 
		  * \code
		  * KFbxVector4 lZero(0,0,0);
		  * pNode->SetPivotState(KFbxNode::eSOURCE_SET, KFbxNode::ePIVOT_STATE_ACTIVE);
		  * pNode->SetPivotState(KFbxNode::eDESTINATION_SET, KFbxNode::ePIVOT_STATE_ACTIVE);
		  *	
		  * ERotationOrder lRotationOrder;
		  * pNode->GetRotationOrder(KFbxNode::eSOURCE_SET , lRotationOrder);
		  * pNode->SetRotationOrder(KFbxNode::eDESTINATION_SET , lRotationOrder);
          *
		  * // For cameras and lights (without targets) let's compensate the postrotation.
		  * if (pNode->GetCamera() || pNode->GetLight())
		  * {
		  *    if (!pNode->GetTarget())
		  *    {
		  *        KFbxVector4 lRV(90, 0, 0);
		  *        if (pNode->GetCamera())    
		  *           lRV.Set(0, 90, 0);
          *
		  *        KFbxVector4 prV = pNode->GetPostRotation(KFbxNode::eSOURCE_SET);
		  *        KgeRMatrix lSourceR;
		  *        KgeRMatrix lR(lRV.mData[0], lRV.mData[1], lRV.mData[2]);
		  *        KgeVector res(prV.mData[0], prV.mData[1], prV.mData[2]);
          *
		  *        // Rotation order don't affect post rotation, so just use the default XYZ order
		  *        KMBRotationOrder rOrder;
		  *        rOrder.V2M(lSourceR, *((KgeRVector*)res.mData));
          *
		  *        KgeMult(lR, lSourceR, lR);
  		  *        rOrder.M2V(*((KgeRVector*)res.mData), lR);
		  *        prV.mData[0] = res.mData[0];
		  *        prV.mData[1] = res.mData[1];
		  *        prV.mData[2] = res.mData[2];
		  *        pNode->SetPostRotation(KFbxNode::eSOURCE_SET, prV);
		  *        pNode->GetLimits().SetRotationLimitActive(true);
          *    }
          *
		  *    // Point light do not need to be adjusted (since they radiate in all the directions).
		  *    if (pNode->GetLight() && pNode->GetLight()->LightType.Get() == KFbxLight::ePOINT)
		  *    {
		  *        pNode->SetPostRotation(KFbxNode::eSOURCE_SET, KFbxVector4(0,0,0,0));
		  *    }
          *
		  *    // apply Pre rotations only on bones / end of chains
		  *    if(pNode->GetNodeAttribute() && pNode->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eSKELETON
		  *       || (pNode->GetMarker() && pNode->GetMarker()->GetType() == KFbxMarker::eFK_EFFECTOR)
		  *       || (pNode->GetMarker() && pNode->GetMarker()->GetType() == KFbxMarker::eIK_EFFECTOR))
		  *    {
		  *        if(pNode->GetLimits().GetRotationLimitActive())
		  *        {
 		  *            pNode->SetPreRotation(KFbxNode::eDESTINATION_SET, pNode->GetPreRotation(KFbxNode::eSOURCE_SET));
		  *        }
          *			            
		  *  	   // No pivots on bones
		  *        pNode->SetRotationPivot(KFbxNode::eDESTINATION_SET, lZero);    
		  *        pNode->SetScalingPivot(KFbxNode::eDESTINATION_SET,    lZero);    
		  *        pNode->SetRotationOffset(KFbxNode::eDESTINATION_SET,lZero);    
		  *        pNode->SetScalingOffset(KFbxNode::eDESTINATION_SET, lZero);
		  *    }
		  *    else
		  *    {
		  *        // any other type: no pre-rotation support but...
		  *        pNode->SetPreRotation(KFbxNode::eDESTINATION_SET, lZero);
          *       
		  *        // support for rotation and scaling pivots.
 		  *        pNode->SetRotationPivot(KFbxNode::eDESTINATION_SET, pNode->GetRotationPivot(KFbxNode::eSOURCE_SET));    
		  *        pNode->SetScalingPivot(KFbxNode::eDESTINATION_SET, pNode->GetScalingPivot(KFbxNode::eSOURCE_SET));    
		  *        // Rotation and scaling offset are supported
		  *        pNode->SetRotationOffset(KFbxNode::eDESTINATION_SET, pNode->GetRotationOffset(KFbxNode::eSOURCE_SET));    
		  *        pNode->SetScalingOffset(KFbxNode::eDESTINATION_SET, pNode->GetScalingOffset(KFbxNode::eSOURCE_SET));
		  *        //
		  *        // If we don't "support" scaling pivots, we can simply do:
		  *        // pNode->SetRotationPivot(KFbxNode::eDESTINATION_SET, lZero);
	      *        // pNode->SetScalingPivot(KFbxNode::eDESTINATION_SET, lZero);
		  *     }
		  * \endcode
		  *
          */
        //@{

            /** \enum EPivotSet  Pivot context identifier.
              */
            typedef enum
            {
                eSOURCE_SET,			/*!<	The source pivot context. */
                eDESTINATION_SET		/*!<	The destination pivot context. */
            } EPivotSet;

            /** \enum EPivotState  Pivot context state.
              */
            typedef enum
            {
                ePIVOT_STATE_ACTIVE,		/*!< The pivot context with this state is affecting the 
											     node's transform computation. */
                ePIVOT_STATE_REFERENCE		/*!< The pivot context with this state is not used during 
											     the node transform computation but can be accessed for reference
												 purposes. */
            } EPivotState;

            /** Change the state of the pivot context.
              * \param pPivotSet Specify which pivot context is manipulated.
              * \param pPivotState The new state of the pivot context.
              */
            void SetPivotState(EPivotSet pPivotSet, EPivotState pPivotState);

            /** Get the pivot context state.
              * The returned value tells if this pivot context is used in the
			  * evaluation of the node transform or not. 
              * \param pPivotSet Specify which pivot context is queried.
              * \param pPivotState The current state of the pivot set.
              */
            void GetPivotState(EPivotSet pPivotSet, EPivotState& pPivotState) const;

            /** Set rotation space
              * Determine the rotation space (Euler or Spheric) and the rotation order.
              * \param pPivotSet Specify which pivot context is manipulated.
              * \param pRotationOrder The new value for the pivot rotation order.
              */
            void SetRotationOrder(EPivotSet pPivotSet, ERotationOrder pRotationOrder);

            /** Get rotation order
              * \param pPivotSet Specify which pivot context is queried.
              * \param pRotationOrder The current value of the pivot rotation order.
              */
            void GetRotationOrder(EPivotSet pPivotSet, ERotationOrder& pRotationOrder) const;

            /** Set rotation space for limit only.
              * \param pPivotSet Specify which pivot context is manipulated.
              * \param pUseForLimitOnly When set to \c true, the current rotation space 
			  *                         (set with SetRotationOrder) define the rotation space for 
			  *                         the limit only; leaving the rotation animation in 
			  *                         Euler XYZ space. When set to \c false, the current rotation
			  *                         space defines the rotation space for both the limits and the 
			  *                         rotation animation data.
              */
            void SetUseRotationSpaceForLimitOnly(EPivotSet pPivotSet, bool pUseForLimitOnly);

            /** Get rotation space for limit only.
              * \param pPivotSet Specify which pivot context is queried.
              * \return The current rotation space limit flag value.
              */
            bool GetUseRotationSpaceForLimitOnly(EPivotSet pPivotSet) const;

            /** Set the RotationActive state.
              * \param pVal The new state of the property.
              * \remarks When this flag is set to false, the RotationOrder, the Pre/Post rotation values
              *          and the rotation limits should be ignored.
              */
            void SetRotationActive(bool pVal);

            /** Get the RotationActive state.
              * \return The value of the RotationActive flag.
              */
            bool GetRotationActive() const;

            /** Specify which Quaternion interpolation mode is used on the pivot context.
              * \param pPivotSet Specify which pivot context is manipulated.
              * \param pQuatIterp  The new value.
              * \remarks When the \e pPivotSet is eSOURCE_SET, this method also updates the value of the 
              *          QuaternionInterpolate property.
              */
            void SetQuaternionInterpolation(EPivotSet pPivotSet, EQuaternionInterpolation pQuatIterp);

            /** Get the Quaternion interpolation mode of the pivot context.
              * \param pPivotSet Specify which pivot context is queried.
              * \return The current mode set on the pivot context.
              */
            EQuaternionInterpolation GetQuaternionInterpolation(EPivotSet pPivotSet) const;

            /** Set the Quaternion interpolation mode on the pivot context.
              * \param pPivotSet Specify which pivot context is manipulated.
              * \param pUseQuaternion The new state.
              * \remarks When the \e pUseQuaternion = \c False, this method is equivalent to calling 
              *          SetQuaternionInterpolation(pPivotSet, eQUATINTERP_OFF). Similarly, when the
              *          \e pUseQuaternion = \c True, the equivalent call is 
              *          SetQuaternionInterpolation(pPivotSet, eQUATINTERP_CLASSIC).
              */
            K_DEPRECATED void SetUseQuaternionForInterpolation(EPivotSet pPivotSet, bool pUseQuaternion);

            /** Get the Quaternion interpolation state on the pivot context.
              * \param pPivotSet Specify which pivot context is queried.
              * \return \c False if the Quaternion Interpolation mode on the pivot context is
              *          eQUATINTERP_OFF and \c True otherwise.
              * \remarks When the return value of this method is \c True, applications that only
              *          support the eQUATINTERP_CLASSIC mode, should call the GetQuaternionInterpolation() method
              *          to make sure that they are manipulating the supported mode.
              */
            K_DEPRECATED bool GetUseQuaternionForInterpolation(EPivotSet pPivotSet) const;

            /** Set the rotation stiffness.
              * The stiffness attribute is used by IK solvers to generate a resistance
              * to a joint motion. The higher the stiffness the less it will rotate.
              * Stiffness works in a relative sense: it determines the willingness of
              * this joint to rotate with respect to the other joint in the IK chain.
              * \param pRotationStiffness The rotation stiffness values are limited to
              * the range [0, 100].
              */
            void SetRotationStiffness(KFbxVector4 pRotationStiffness);

            /** Get the rotation stiffness
              * \return The currently set rotation stiffness values.
              */
            KFbxVector4 GetRotationStiffness() const;

            /** Set the minimum damp range angles.
              * This attributes apply resistance to a joint rotation as it approaches the
              * lower boundary of its rotation limits. This functionality allows joint
              * motion to slow down smoothly until the joint reaches its rotation limits
              * instead of stopping abruptly. The MinDampRange specifies when the
              * deceleration should start.
              * \param pMinDampRange Angle, in degrees, where deceleration should start
              */
            void SetMinDampRange(KFbxVector4 pMinDampRange);

            /** Get the minimum damp range angles
              * \return The currently set minimum damp range angles.
              */
            KFbxVector4 GetMinDampRange() const;


            /** Set the maximum damp range angles.
              * This attributes apply resistance to a joint rotation as it approaches the
              * upper boundary of its rotation limits. This functionality allows joint
              * motion to slow down smoothly until the joint reaches its rotation limits
              * instead of stopping abruptly. The MaxDampRange specifies when the
              * deceleration should start.
              * \param pMaxDampRange Angle, in degrees, where deceleration should start
              */
            void SetMaxDampRange(KFbxVector4 pMaxDampRange);

            /** Get the maximum damp range angles
              * \return The currently set maximum damp range angles.
              */
            KFbxVector4 GetMaxDampRange() const;


            /** Set the minimum damp strength.
              * This attributes apply resistance to a joint rotation as it approaches the
              * lower boundary of its rotation limits. This functionality allows joint
              * motion to slow down smoothly until the joint reaches its rotation limits
              * instead of stopping abruptly. The MinDampStrength defines the
              * rate of deceleration.
              * \param pMinDampStrength Values are limited to the range [0, 100].
              */
            void SetMinDampStrength(KFbxVector4 pMinDampStrength);

            /** Get the minimum damp strength
              * \return The currently set minimum damp strength values.
              */
            KFbxVector4 GetMinDampStrength() const;


            /** Set the maximum damp strength.
              * This attributes apply resistance to a joint rotation as it approaches the
              * upper boundary of its rotation limits. This functionality allows joint
              * motion to slow down smoothly until the joint reaches its rotation limits
              * instead of stopping abruptly. The MaxDampStrength defines the
              * rate of deceleration.
              * \param pMaxDampStrength Values are limited to the range [0, 100].
              */
            void SetMaxDampStrength(KFbxVector4 pMaxDampStrength);

            /** Get the maximum damp strength
              * \return The currently set maximum damp strength values.
              */
            KFbxVector4 GetMaxDampStrength() const;

            /** Set the preferred angle.
              * The preferredAngle attribute defines the initial joint configuration used
              * by a single chain IK solver to calculate the inverse kinematic solution.
              * \param pPreferedAngle Angle in degrees
              */
            void SetPreferedAngle(KFbxVector4 pPreferedAngle);

            /** Get the preferred angle
              * \return The currently set preferred angle.
              */
            KFbxVector4 GetPreferedAngle() const;

            /** Set a translation offset for the rotation pivot.
              * The translation offset is in coordinates relative to the node's origin.
              * \param pPivotSet Specify which pivot set to modify.
              * \param pVector The X,Y and Z translation values (the 4th component of the KFbxVector4 is ignored).
              */
            void SetRotationOffset(EPivotSet pPivotSet, KFbxVector4 pVector);

            /** Get the translation offset for the rotation pivot.
              * The translation offset is in coordinates relative to the node's origin.
              * \param pPivotSet Specify which pivot set to to query the value.
              * \return The X, Y and Z translation offset values (the 4th component of the KFbxVector4 is always 1).
              */
            const KFbxVector4& GetRotationOffset(EPivotSet pPivotSet) const;

            /** Set rotation pivot.
              * The rotation pivot is the center of rotation in coordinates relative to
              * the node's origin.
              * \param pPivotSet Specify which pivot set to modify.
              * \param pVector The new position of the rotation pivot (the 4th component of the KFbxVector4 is ignored).
              */
            void SetRotationPivot(EPivotSet pPivotSet, KFbxVector4 pVector);

            /** Get rotation pivot.
              * The rotation pivot is the center of rotation in coordinates relative to
              * the node's origin.
              * \param pPivotSet Specify which pivot set to query.
              * \return The current position of the rotation pivot (the 4th component of the KFbxVector4 is always 1).
              */
            const KFbxVector4& GetRotationPivot(EPivotSet pPivotSet) const;

            /** Set pre-rotation in Euler angles.
              * The pre-rotation is the rotation applied to the node before
              * rotation animation data.
              * \param pPivotSet Specify which pivot set to modify.
              * \param pVector The X,Y,Z rotation values to set (the 4th component of the KFbxVector4 is ignored).
              */
            void SetPreRotation(EPivotSet pPivotSet, KFbxVector4 pVector);

            /** Get pre-rotation in Euler angles.
              * The pre-rotation is the rotation applied to the node before
              * rotation animation data.
              * \param pPivotSet Specify which pivot set to query.
              * \return The X,Y and Z rotation values (the 4th component of the KFbxVector4 is always 1).
              */
            const KFbxVector4& GetPreRotation(EPivotSet pPivotSet) const;

            /** Set post-rotation in Euler angles.
              * The post-rotation is the rotation applied to the node after the
              * rotation animation data.
              * \param pPivotSet Specify which pivot set to modify.
              * \param pVector The X,Y,Z rotation values to set (the 4th component of the KFbxVector4 is ignored).
              */
            void SetPostRotation(EPivotSet pPivotSet, KFbxVector4 pVector);

            /** Get post-rotation in Euler angles.
              * The post-rotation is the rotation applied to the node after the
              * rotation animation data.
              * \param pPivotSet Specify which pivot set to query.
              * \return The X,Y and Z rotation values (the 4th component of the KFbxVector4 is always 1).
              */
            const KFbxVector4& GetPostRotation(EPivotSet pPivotSet) const;

            /** Set a translation offset for the scaling pivot.
              * The translation offset is in coordinates relative to the node's origin.
              * \param pPivotSet Specify which pivot set to modify.
              * \param pVector The X,Y and Z translation values (the 4th component of the KFbxVector4 is ignored).
              */
            void SetScalingOffset(EPivotSet pPivotSet, KFbxVector4 pVector);

            /** Get the translation offset for the scaling pivot.
              * The translation offset is in coordinates relative to the node's origin.
              * \param pPivotSet Specify which pivot set to query the value.
              * \return The X, Y and Z translation offset values (the 4th component of the KFbxVector4 is always 1).
              */
            const KFbxVector4& GetScalingOffset(EPivotSet pPivotSet) const;

            /** Set scaling pivot.
              * The scaling pivot is the center of scaling in coordinates relative to
              * the node's origin.
              * \param pPivotSet Specify which pivot set to modify.
			  * \param pVector The new position of the scaling pivot (the 4th component of the KFbxVector4 is ignored).
              */
            void SetScalingPivot(EPivotSet pPivotSet, KFbxVector4 pVector);

            /** Get scaling pivot.
              * The scaling pivot is the center of scaling in coordinates relative to
              * the node's origin.
              * \param pPivotSet Specify which pivot set to query.
              * \return The current position of the rotation pivot (the 4th component of the KFbxVector4 is always 1).
              */
            const KFbxVector4& GetScalingPivot(EPivotSet pPivotSet) const;

            /** Set geometric translation
              * The geometric translation is a local translation that is applied
              * to a node attribute only. This translation is applied to the node attribute
              * after the node transformations. This translation is not inherited across the
              * node hierarchy.
              * \param pPivotSet Specify which pivot set to modify.
              * \param pVector The X, Y, and Z translation values (the 4th component of the KFbxVector4 is ignored).
              */
            void SetGeometricTranslation(EPivotSet pPivotSet, KFbxVector4 pVector);

            /** Get geometric translation
              * \param pPivotSet Specify which pivot set to query.
              * \return The current geometric translation (the 4th component of the KFbxVector4 is always 1).
              */
            KFbxVector4 GetGeometricTranslation(EPivotSet pPivotSet) const;

            /** Set geometric rotation
              * The geometric rotation is a local rotation that is applied
              * to a node attribute only. This rotation is applied to the node attribute
              * after the node transformations. This rotation is not inherited across the
              * node hierarchy.
              * \param pPivotSet Specify which pivot set to modify.
              * \param pVector The X,Y and Z rotation values (the 4th component of the KFbxVector4 is ignored).
              */
            void SetGeometricRotation(EPivotSet pPivotSet, KFbxVector4 pVector);

            /** Get geometric rotation
              * \param pPivotSet Specify which pivot set to query.
              * \return The current geometric rotation (the 4th component of the KFbxVector4 is always 1).
              */
            KFbxVector4 GetGeometricRotation(EPivotSet pPivotSet) const;

            /** Set geometric scaling
              * The geometric scaling is a local scaling that is applied
              * to a node attribute only. This scaling is applied to the node attribute
              * after the node transformations. This scaling is not inherited across the
              * node hierarchy.
              * \param pPivotSet Specify which pivot set to modify.
              * \param pVector The X,Y and Z scale values (the 4th component of the KFbxVector4 is ignored).
              */
            void SetGeometricScaling(EPivotSet pPivotSet, KFbxVector4 pVector);

            /** Get geometric scaling
			  * \param pPivotSet Specify which pivot set to query.
              * \return The current geometric scaling (the 4th component of the KFbxVector4 is always 1).
              */
            KFbxVector4 GetGeometricScaling(EPivotSet pPivotSet) const;

            /** Reset a pivot set to the default pivot context.
			  * If the node has a geometry, reset the geometry's pivot to the identity matrix.
              * \param pPivotSet Pivot set to reset.
              * \remarks The default pivot context is a context with all the vector attributes
			  *          set to (0,0,0) except the GeometricScaling attribute that is reset to (1,1,1).
              */
            void ResetPivotSet( KFbxNode::EPivotSet pPivotSet );

			/** Recursively convert the animation data according to pivot settings.
			  * This method is still available for legacy reasons. Its use is limited to
			  * the processing of very old data coming from FBX v5 files and should not be
			  * used in any other case. Instead call the ConvertPivotAnimationRecursive().
			  *
              * \param pConversionTarget If set to EPivotSet::eDESTINATION_SET,
              *                          convert animation data from the EPivotSet::eSOURCE_SET pivot context
              *                          to the EPivotSet::eDESTINATION_SET pivot context. Otherwise, the
              *                          conversion is computed the other way around.
              * \param pFrameRate Resampling frame rate in frames per second.
              * \param pKeyReduce Apply or skip key reducing filter.
			  * \remarks Due to the intrinsic properties of the mathematical operations performed,
			  *          sometimes, it is necessary to resample animation curves to maintain the accurate
			  *          conversion. When this resampling is required, the method will use the \e pFrameRate
			  *          value to specify the number of samples. To avoid a huge number of keys in the animation
			  *          curves, a constant key reducer filter (KFbxKFCurveFilterConstantKeyReducer) is 
			  *          automatically applied to all the affected curves to remove as much consecutive keys 
			  *          that have the same value. This filter is private and its settings cannot be changed.
			  *          It is possible that, after the filtering pass, the animations curves do not contain keys
			  *          anymore. This is a normal result and does not affect the overall results.
			  *
              */
            void ConvertPivotAnimation(EPivotSet pConversionTarget, double pFrameRate, bool pKeyReduce=true);
            
            /** This version is an improved version of the ConvertPivotAnimation(). It fully supports all the
			  * attributes defined in the pivot sets and can process animation data defined on different animation
			  * stack. 
              * \param pAnimStackName The name of animation stack on which the conversion will take place.
              *                       If equals \c NULL or is an empty string, the first animation stack 
			  *                       will be used.
              * \param pConversionTarget If set to EPivotSet::eDESTINATION_SET,
              *                          convert animation data from the EPivotSet::eSOURCE_SET pivot context
              *                          to the EPivotSet::eDESTINATION_SET pivot context. Otherwise, the
              *                          conversion is computed the other way around.
              * \param pFrameRate Resampling frame rate in frames per second.
              * \param pKeyReduce Apply or skip key reducing filter.
			  * \remarks Due to the intrinsic properties of the mathematical operations performed,
			  *          sometimes, it is necessary to resample animation curves to maintain the accurate
			  *          conversion. When this resampling is required, the method will use the \e pFrameRate
			  *          value to specify the number of samples. To avoid a huge number of keys in the animation
			  *          curves, a constant key reducer filter (KFbxKFCurveFilterConstantKeyReducer) is 
			  *          automatically applied to all the affected curves to remove as much consecutive keys 
			  *          that have the same value. This filter is private and its settings cannot be changed.
			  *          It is possible that, after the filtering pass, the animations curves do not contain keys
			  *          anymore. This is a normal result and does not affect the overall results.
             */
            void ConvertPivotAnimationRecursive(const char * pAnimStackName,
                EPivotSet pConversionTarget, double pFrameRate, bool pKeyReduce=true);

            /** Reset all the pivot sets to the default pivot context and convert the animation.
              * \param pFrameRate Resampling frame rate in frames per second.
              * \param pKeyReduce Apply or skip key reducing filter.
              * \param pToNodeCenter: Reset pivots to node center if \c true, or retain pivot places if \c false.
              * \param pForceResetLimits  If \c true, this flag will reset all the Translation, Rotation and Scaling 
			  *                           limits and clears the enabled flags.
              * \remarks The resulting animation will be visually equivalent and all the pivots will be cleared.
              *          The conversion is performed on all animation stacks.
              * \remarks Will recursively convert the animation of all the children nodes.
              * \remarks The \e pForceResetLimits flag has a destructive behavior and should be used only in very 
			  *          limited cases where the values of the limits are not required after the call to this method.
              */
            void ResetPivotSetAndConvertAnimation( double pFrameRate=30., bool pKeyReduce=false, bool pToNodeCenter=true, bool pForceResetLimits = false );

            /** Set rotation pivot as node center recursively
              * \param pParentGeometricOffset Offset vector to be applied.
              */
            void SetRotationPivotAsCenterRecursive(KFbxVector4 pParentGeometricOffset = KFbxVector4());
        //@}

        /**
          * \name Node Transform Evaluation
          */
			/** Returns this node's global transformation matrix at the specified time. The node's translation, rotation and scaling limits are taken into consideration.
			  * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
			  * \param pPivotSet The pivot set to take into account
			  * \param pApplyTarget Applies the necessary transform to align into the target node
			  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
			  * \return The resulting global transform of the specified node at the specified time.
			  * \remarks This function is the equivalent of calling Scene->GetEvaluator()->GetNodeGlobalTransform().
			  */
			KFbxXMatrix& EvaluateGlobalTransform(KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);

			/** Returns this node's local transformation matrix at the specified time. The node's translation, rotation and scaling limits are taken into consideration.
			  * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
			  * \param pPivotSet The pivot set to take into account
			  * \param pApplyTarget Applies the necessary transform to align into the target node
			  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
			  * \return The resulting local transform of the specified node for the specified time.
			  * \remarks The local transform matrix is calculated in this way: ParentGlobal.Inverse * Global, all transforms such as pre/post rotation are taken into consideration.
			  * This will return a different value than LclTranslation, LclRotation and LclScaling at the specified time. To evaluate these properties separately
			  * without taking pre/post rotation, pivots and offsets into consideration, please use GetNodeLocalTranslation(), GetNodeLocalRotation() and GetNodeLocalScaling().
			  * This function is the equivalent of calling Scene->GetEvaluator()->GetNodeLocalTransform().
			  */
			KFbxXMatrix& EvaluateLocalTransform(KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);

			/** Returns this node's LclTranslation property at the specified time.
			  * No pivot, offsets, or any other transform is taken into consideration. The translation limit is applied.
			  * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
			  * \param pPivotSet The pivot set to take into account
			  * \param pApplyTarget Applies the necessary transform to align into the target node
			  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
			  * \return The resulting value of LclTranslation property of the specified node at the specified time.
			  * \remarks This function is the equivalent of calling Scene->GetEvaluator()->GetNodeLocalTranslation().
			  */
			KFbxVector4& EvaluateLocalTranslation(KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);

			/** Returns this node's LclRotation property at the specified time.
			  * No pre/post rotation, rotation pivot, rotation offset or any other transform is taken into consideration. The rotation limit is applied.
			  * \param pNode The transform node to evaluate.
			  * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
			  * \param pPivotSet The pivot set to take into account
			  * \param pApplyTarget Applies the necessary transform to align into the target node
			  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
			  * \return The resulting value of LclRotation property of the specified node at the specified time.
			  * \remarks This function is the equivalent of calling Scene->GetEvaluator()->GetNodeLocalRotation().
			  */
			KFbxVector4& EvaluateLocalRotation(KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);

			/** Returns this node's LclScaling property at the specified time.
			  * No scaling pivot, scaling offset or any other transform is taken into consideration. The scaling limit is applied.
			  * \param pTime The time used for evaluate. If KTIME_INFINITE is used, this returns the default value, without animation curves evaluation.
			  * \param pPivotSet The pivot set to take into account
			  * \param pApplyTarget Applies the necessary transform to align into the target node
			  * \param pForceEval Force the evaluator to refresh the evaluation state cache even if its already up-to-date.
			  * \return The resulting value of LclScaling property of the specified node at the specified time.
			  * \remarks This function is the equivalent of calling Scene->GetEvaluator()->GetNodeLocalScaling().
			  */
			KFbxVector4& EvaluateLocalScaling(KTime pTime=KTIME_INFINITE, KFbxNode::EPivotSet pPivotSet=KFbxNode::eSOURCE_SET, bool pApplyTarget=false, bool pForceEval=false);
        //@{
        //@}

        /**
          * \name Character Link
          */
        //@{

            /** Get number of character links.
              * \return The number of character links.
              */
            int GetCharacterLinkCount() const;

            /** Get character link at given index.
              * \param pIndex Index of character link.
              * \param pCharacter Pointer to receive linked character if function succeeds.
              * \param pCharacterLinkType Pointer to receive character link type if function succeeds,
              *                           cast to \c ECharacterLinkType.
              * \param pNodeId Pointer to receive the node ID if function succeeds. This ID should be casted 
			  *                to \c ECharacterNodeId type when the character link type is \c eCharacterLink or
			  *                \c eControlSetLink  else to the \c EEffectorNodeId type if the character link type is 
			  *                \c eControlSetEffector or \c eControlSetEffectorAux.
			  * \param pNodeSubId For internal use.
              * \return \c false if the index is out of range or any of the pointer arguments is NULL.
              */
            bool GetCharacterLink(int pIndex, KFbxCharacter** pCharacter, int* pCharacterLinkType, int* pNodeId, int *pNodeSubId);

            /** Looks if the given character link exists on this node.
              * \param pCharacter Character searched.
              * \param pCharacterLinkType Character link type searched. Its value must be one of 
			  *                           the \c ECharacterLinkType symbols..
              * \param pNodeId Node ID searched. If \e pCharacterLinkType is \c eCharacterLink or \c eControlSetLink
			  *                the \e pNodeId value is casted to the \c ECharacterNodeId type. If the \e pCharacterLinkType
              *			       is \c eControlSetEffector or \c eControlSetEffectorAux then the \e pNodeId is casted to the
			  *                \c EEffectorNodeId type.
			  * \param pNodeSubId For internal use.
              * \return Index of found character link if it exists, -1 otherwise.
              */
            int FindCharacterLink(KFbxCharacter* pCharacter, int pCharacterLinkType, int pNodeId, int pNodeSubId) const;
        //@}

        /** Find out start and end time of the animation curves for this node (and its children).
          * \param pStart Reference to store the start time.
          *               \c pStart is overwritten with a new value only if the found start time is 
		  *               lower than \c pStart current value. Therefore, it is important to initialize
		  *               \e pStart with KTIME_INFINITE.
          * \param pStop Reference to store end time.
          *               \c pStop is overwritten with a new value only if the found stop time is 
		  *               higher than \c pStop current value. Therefore, it is important to initialize
		  *               \c pStop with KTIME_MINUS_INFINITE.
          * \param pAnimStack   Animation stack where to retrieve animation curves. 
          * \param pAnimLayerId Specific animation layer on the animStack to use. 
          * \return \c true if the node (or its children) is animated, \c false otherwise.
          * \remarks If pAnimStack is left NULL, the function will try to get the first AnimStack that is connected
		  *          to the scene. \e pAnimLayerId represent the index of the connection. For example, the call:
          * \remarks This function is deprecated, please use GetAnimationInterval(KTimeSpan&, KFbxAnimStack*, int) instead.
		  * \code
		  *	lNode->GetAnimationInterval(start, stop, myStack, 3);
		  * \endcode
		  * will scan all the animation curves of this node, and it's children, that are defined on the third 
		  * animation layer of \c myStack.
		  *
          */
        K_DEPRECATED virtual bool GetAnimationInterval(KTime& pStart, KTime& pStop, KFbxAnimStack* pAnimStack = NULL, int pAnimLayerId = 0);

        /** Find out start and end time of the animation curves for this node (and its children).
          * \param pSpan Reference to store the start time and end time.
          *               The start time is overwritten with a new value only if the found start time is 
		  *               lower than current value. The end time is overwritten with a new value only if the
          *               found end time is higher than current value. Therefore, it is important
          *               to initialize pSpan with KTimeSpan(KTIME_INFINITE, KTIME_MINUS_INFINITE).
          * \param pAnimStack   Animation stack where to retrieve animation curves. 
          * \param pAnimLayerId Specific animation layer on the animStack to use. 
          * \return \c true if the node (or its children) is animated, \c false otherwise.
          * \remarks If pAnimStack is left NULL, the function will try to get the first AnimStack that is connected
		  *          to the scene. \e pAnimLayerId represent the index of the connection. For example, the call:
		  * \code
		  *	lNode->GetAnimationInterval(span, myStack, 3);
		  * \endcode
		  * will scan all the animation curves of this node, and it's children, that are defined on the third 
		  * animation layer of \c myStack.
		  *
          */
        virtual bool GetAnimationInterval(KTimeSpan& pSpan, KFbxAnimStack* pAnimStack = NULL, int pAnimLayerId = 0);


        /**
          * \name Material Management
          */
        //@{

            /** Add a material to this node.
              * \param pMaterial The material to add.
              * \return non-negative index of added material, or -1 on error.
              */
            int AddMaterial( KFbxSurfaceMaterial* pMaterial );

            /** Remove a material from this node.
              * \param pMaterial The material to remove.
              * \return true on success, false otherwise
              */
            bool RemoveMaterial( KFbxSurfaceMaterial* pMaterial );

            /**
              * \return The number of materials applied to this node
              */
            int GetMaterialCount() const;

            /** Access a material on this node.
              * \param pIndex Valid range is [0, GetMaterialCount() - 1]
              * \return The pIndex-th material, or NULL if pIndex is invalid.
              */
            KFbxSurfaceMaterial* GetMaterial( int pIndex ) const;

            /** Remove all materials applied to this node.
              */
            void RemoveAllMaterials();

            /** Find an applied material with the given name.
              * \param pName The requested name
              * \return an index to a material, or -1 if no applied material
              * has the requested name.
              */
            int GetMaterialIndex( char const* pName ) const;

        //@}

        /**
          * \name Error Management
          * The same error object is shared among instances of this class.
          */
        //@{

            /** Retrieve error object.
              * \return Reference to error object.
              */
            KError& GetError();

            /** \enum EError  Error identifiers.
              */
            typedef enum
            {
                eINDEX_OUT_OF_RANGE,		/*!<	The index argument is outside the allowed values. */
                eATTRIBUTE_NOT_CONNECTED,	/*!<	The requested node attribute is invalid or not connected to this node. */
                eERROR_COUNT
            } EError;

            /** Get last error code.
              * \return Last error code.
              */
            EError GetLastErrorID() const;

            /** Get last error string.
              * \return Textual description of the last error.
              */
            const char* GetLastErrorString() const;

        //@}


        /**
          * \name Public and fast access Properties
          */
        //@{

            /** This property contains the translation information of the node
            *
            * To access this property do: LclTranslation.Get().
            * To set this property do: LclTranslation.Set(fbxDouble3).
            *
            * Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               LclTranslation;

            /** This property contains the rotation information of the node
            *
            * To access this property do: LclRotation.Get().
            * To set this property do: LclRotation.Set(fbxDouble3).
            *
            * Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               LclRotation;

            /** This property contains the scaling information of the node
            *
            * To access this property do: LclScaling.Get().
            * To set this property do: LclScaling.Set(fbxDouble3).
            *
            * Default value is 1.,1.,1.
            */
            KFbxTypedProperty<fbxDouble3>               LclScaling;

            /** This property contains the visibility information of the node.
            * The assumed behavior of this property is to affect the visibility of the node, all the 
            * nodes attributes connected to it as well as all its descendants. This property can be
            * animated.
            *
            * To access this property do: Visibility.Get().
            * To set this property do: Visibility.Set(fbxDouble1).
            *
            * Default value is 1.
            * \remarks  \li This property holds values ranging from 0.0 to 1.0 where the value 0.0 means
            *           a totally invisible object, the value 1.0, a full visible object and anything inbetween, a
            *           percentage degree of visibility.\n
            *
            *          \li Since not all the applications may support a degree of visibility, it is agreed that
            *          a value of 0.0 means invisible and anything else means visible.
            *
            * \see Show property.
            */
            KFbxTypedProperty<fbxDouble1>               Visibility;

            /** This property contains the visibility inheritance flag that allow applications to modify
            * the Visibility property interpretation. By default, this value is set to \c true because it is
			* assumed (as explained in the Visibility property description) that the node visibility is inherited
			* from its parent. In other words, applications should always process the Visibility property of the 
			* node and, depending on its value, decide whether or not the node has to be displayed. After
			* this first assessment, check the node VisibilityInheritance flag. If its value is set to \c false then
			* move to the next object, else use the parent's Visibility value and modify this node display state 
			* by performing the logical AND operation between this node Visibility property and its parent's.
            *
            * To access this property do: VisibilityInheritance.Get().
            * To set this property do: VisibilityInheritance.Set(fbxBool1).
            *
            * Default value is \c true.
            * \remarks This property is non-animatable and is not used inside the FBX SDK but it is guaranteed
			*          to exist in FBX files with version 7.2 and above.
            * \see Visibility property.
            */
			KFbxTypedProperty<fbxBool1>					VisibilityInheritance;


            /** This property contains the quaternion interpolate flag of the node
            *
            * To access this property do: QuaternionInterpolate.Get().
            * To set this property do: QuaternionInterpolate.Set(EQuaternionInterpolation).
			*
            * Default value is eQUATINTERP_OFF.
            */
            KFbxTypedProperty<EQuaternionInterpolation>  QuaternionInterpolate;

            /** This property contains the rotation offset information of the node
            *
            * To access this property do: RotationOffset.Get().
            * To set this property do: RotationOffset.Set(fbxDouble3).
			*
            * Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               RotationOffset;

            /** This property contains the rotation pivot information of the node
            *
            * To access this property do: RotationPivot.Get().
            * To set this property do: RotationPivot.Set(fbxDouble3).
			*
            * Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               RotationPivot;
 
            /** This property contains the scaling offset information of the node
            *
            * To access this property do: ScalingOffset.Get().
            * To set this property do: ScalingOffset.Set(fbxDouble3).
			*
            * Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               ScalingOffset;

            /** This property contains the scaling pivot information of the node
            *
            * To access this property do: ScalingPivot.Get().
            * To set this property do: ScalingPivot.Set(fbxDouble3).
			*
            * Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               ScalingPivot;

            /** This property contains the translation active information of the node
            *
            * To access this property do: TranslationActive.Get().
            * To set this property do: TranslationActive.Set(fbxBool1).
			*
            * Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 TranslationActive;

            /** This property contains the translation information of the node
            *
            * To access this property do: Translation.Get().
            * To set this property do: Translation.Set(fbxDouble3).
			*
            */
            KFbxTypedProperty<fbxDouble3>               Translation;

            /** This property contains the min translation limit information of the node
            *
            * To access this property do: TranslationMin.Get().
            * To set this property do: TranslationMin.Set(fbxDouble3).
			* Default value is 0.,0.,0.
			*
            */
            KFbxTypedProperty<fbxDouble3>               TranslationMin;

            /** This property contains the max translation limit information of the node
            *
            * To access this property do: TranslationMax.Get().
            * To set this property do: TranslationMax.Set(fbxDouble3).
			* Default value is 0.,0.,0.
			*
            */
            KFbxTypedProperty<fbxDouble3>               TranslationMax;

            /** This property contains the flag which actives the x component of min translation limit of the node
            *
            * To access this property do: TranslationMinX.Get().
            * To set this property do: TranslationMinX.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 TranslationMinX;

            /** This property contains the flag which actives the y component of min translation limit of the node
            *
            * To access this property do: TranslationMinY.Get().
            * To set this property do: TranslationMinY.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 TranslationMinY;

            /** This property contains the flag which actives the z component of min translation limit of the node
            *
            * To access this property do: TranslationMinZ.Get().
            * To set this property do: TranslationMinZ.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 TranslationMinZ;

            /** This property contains the flag which actives the x component of max translation limit of the node
            *
            * To access this property do: TranslationMaxX.Get().
            * To set this property do: TranslationMaxX.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 TranslationMaxX;

            /** This property contains the flag which actives the y component of max translation limit of the node
            *
            * To access this property do: TranslationMaxY.Get().
            * To set this property do: TranslationMaxY.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 TranslationMaxY;

            /** This property contains the flag which actives the z component of max translation limit of the node
            *
            * To access this property do: TranslationMaxZ.Get().
            * To set this property do: TranslationMaxZ.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 TranslationMaxZ;

			/** This property contains the rotation order information of the node
            *
            * To access this property do: RotationOrder.Get().
            * To set this property do: RotationOrder.Set(ERotationOrder).
			* Default value is eEULER_XYZ.
			*
            */
            KFbxTypedProperty<ERotationOrder>           RotationOrder;

            /** This property contains the rotation space for limit only flag of the node
            *
            * To access this property do: RotationSpaceForLimitOnly.Get().
            * To set this property do: RotationSpaceForLimitOnly.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 RotationSpaceForLimitOnly;

            /** This property contains the x value of the rotation stiffness of the node
            *
            * To access this property do: RotationStiffnessX.Get().
            * To set this property do: RotationStiffnessX.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               RotationStiffnessX;

			/** This property contains the y value of the rotation stiffness of the node
            *
            * To access this property do: RotationStiffnessY.Get().
            * To set this property do: RotationStiffnessY.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               RotationStiffnessY;

			/** This property contains the z value of the rotation stiffness of the node
            *
            * To access this property do: RotationStiffnessZ.Get().
            * To set this property do: RotationStiffnessZ.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               RotationStiffnessZ;

            /** This property contains axis length information of the node
            *
            * To access this property do: AxisLen.Get().
            * To set this property do: AxisLen.Set(fbxDouble1).
			* 
			* Default value is 10.
            */
            KFbxTypedProperty<fbxDouble1>               AxisLen;
         
            /** This property contains pre-rotation information of the node
            *
            * To access this property do: PreRotation.Get().
            * To set this property do: PreRotation.Set(fbxDouble3).
			* 
			* Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               PreRotation;

			/** This property contains post-rotation information of the node
            *
            * To access this property do: PostRotation.Get().
            * To set this property do: PostRotation.Set(fbxDouble3).
			* 
			* Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               PostRotation;

            /** This property contains rotation active information of the node
            *
            * To access this property do: RotationActive.Get().
            * To set this property do: RotationActive.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 RotationActive;

            /** This property contains the min rotation limit information of the node
            *
            * To access this property do: RotationMin.Get().
            * To set this property do: RotationMin.Set(fbxDouble3).
			* 
			* Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               RotationMin;

            /** This property contains the max rotation limit information of the node
            *
            * To access this property do: RotationMax.Get().
            * To set this property do: RotationMax.Set(fbxDouble3).
			* 
			* Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               RotationMax;

            /** This property contains the flag which actives the x component of min rotation limit of the node
            *
            * To access this property do: RotationMinX.Get().
            * To set this property do: RotationMinX.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 RotationMinX;

            /** This property contains the flag which actives the y component of min rotation limit of the node
            *
            * To access this property do: RotationMinY.Get().
            * To set this property do: RotationMinY.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 RotationMinY;

            /** This property contains the flag which actives the z component of min rotation limit of the node
            *
            * To access this property do: RotationMinZ.Get().
            * To set this property do: RotationMinZ.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 RotationMinZ;

			/** This property contains the flag which actives the x component of max rotation limit of the node
            *
            * To access this property do: RotationMaxX.Get().
            * To set this property do: RotationMaxX.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 RotationMaxX;

            /** This property contains the flag which actives the y component of max rotation limit of the node
            *
            * To access this property do: RotationMaxY.Get().
            * To set this property do: RotationMaxY.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 RotationMaxY;

			/** This property contains the flag which actives the z component of max rotation limit of the node
            *
            * To access this property do: RotationMaxZ.Get().
            * To set this property do: RotationMaxZ.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 RotationMaxZ;

            /** This property contains inherit type information of the node
            *
            * To access this property do: InheritType.Get().
            * To set this property do: InheritType.Set(ETransformInheritType).
			* 
			* Default value is eINHERIT_RrSs.
            */
            KFbxTypedProperty<ETransformInheritType>    InheritType;

            /** This property contains scaling active information of the node
            *
            * To access this property do: ScalingActive.Get().
            * To set this property do: ScalingActive.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 ScalingActive;

            /** This property contains scaling information of the node
            *
            * To access this property do: Scaling.Get().
            * To set this property do: Scaling.Set(fbxDouble3).
			* 
            */
            KFbxTypedProperty<fbxDouble3>               Scaling;

            /** This property contains the min scaling limit information of the node
            *
            * To access this property do: ScalingMin.Get().
            * To set this property do: ScalingMin.Set(fbxDouble3).
			* 
			* Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               ScalingMin;

            /** This property contains the max scaling limit information of the node
            *
            * To access this property do: ScalingMax.Get().
            * To set this property do: ScalingMax.Set(fbxDouble3).
			* 
			* Default value is 1.,1.,1.
            */
            KFbxTypedProperty<fbxDouble3>               ScalingMax;

            /** This property contains the flag which actives the x component of min scaling limit of the node
            *
            * To access this property do: ScalingMinX.Get().
            * To set this property do: ScalingMinX.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 ScalingMinX;

            /** This property contains the flag which actives the y component of min scaling limit of the node
            *
            * To access this property do: ScalingMinY.Get().
            * To set this property do: ScalingMinY.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 ScalingMinY;

            /** This property contains the flag which actives the z component of min scaling limit of the node
            *
            * To access this property do: ScalingMinZ.Get().
            * To set this property do: ScalingMinZ.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 ScalingMinZ;

            /** This property contains the flag which actives the x component of max scaling limit of the node
            *
            * To access this property do: ScalingMaxX.Get().
            * To set this property do: ScalingMaxX.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 ScalingMaxX;

            /** This property contains the flag which actives the y component of max scaling limit of the node
            *
            * To access this property do: ScalingMaxY.Get().
            * To set this property do: ScalingMaxY.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 ScalingMaxY;

            /** This property contains the flag which actives the z component of max scaling limit of the node
            *
            * To access this property do: ScalingMaxZ.Get().
            * To set this property do: ScalingMaxZ.Set(fbxBool1).
			* 
			* Default value is false.
            */
            KFbxTypedProperty<fbxBool1>                 ScalingMaxZ;

			/** This property contains geometric translation information of the node
            *
            * To access this property do: GeometricTranslation.Get().
            * To set this property do: GeometricTranslation.Set(fbxDouble3).
			* 
			* Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               GeometricTranslation;

			/** This property contains geometric rotation information of the node
            *
            * To access this property do: GeometricRotation.Get().
            * To set this property do: GeometricRotation.Set(fbxDouble3).
			* 
			* Default value is 0.,0.,0.
            */
            KFbxTypedProperty<fbxDouble3>               GeometricRotation;
         
			/** This property contains geometric scaling information of the node
            *
            * To access this property do: GeometricScaling.Get().
            * To set this property do: GeometricScaling.Set(fbxDouble3).
			* 
			* Default value is 1.,1.,1.
            */
            KFbxTypedProperty<fbxDouble3>               GeometricScaling;

            // IK Settings
            //////////////////////////////////////////////////////////

            /** This property contains the x component of the minimum damp range angles of the node
            *
            * To access this property do: MinDampRangeX.Get().
            * To set this property do: MinDampRangeX.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MinDampRangeX;

			/** This property contains the y component of the minimum damp range angles of the node
            *
            * To access this property do: MinDampRangeY.Get().
            * To set this property do: MinDampRangeY.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MinDampRangeY;

			/** This property contains the z component of the minimum damp range angles of the node
            *
            * To access this property do: MinDampRangeZ.Get().
            * To set this property do: MinDampRangeZ.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MinDampRangeZ;

            /** This property contains the x component of the maximum damp range angles of the node
            *
            * To access this property do: MaxDampRangeX.Get().
            * To set this property do: MaxDampRangeX.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MaxDampRangeX;

            /** This property contains the y component of the maximum damp range angles of the node
            *
            * To access this property do: MaxDampRangeY.Get().
            * To set this property do: MaxDampRangeY.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MaxDampRangeY;

			/** This property contains the z component of the maximum damp range angles of the node
            *
            * To access this property do: MaxDampRangeZ.Get().
            * To set this property do: MaxDampRangeZ.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MaxDampRangeZ;

			/** This property contains the x component of the minimum damp strength of the node
            *
            * To access this property do: MinDampStrengthX.Get().
            * To set this property do: MinDampStrengthX.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MinDampStrengthX;

            /** This property contains the y component of the minimum damp strength of the node
            *
            * To access this property do: MinDampStrengthY.Get().
            * To set this property do: MinDampStrengthY.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MinDampStrengthY;

            /** This property contains the z component of the minimum damp strength of the node
            *
            * To access this property do: MinDampStrengthZ.Get().
            * To set this property do: MinDampStrengthZ.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MinDampStrengthZ;

            /** This property contains the x component of the maximum damp strength of the node
            *
            * To access this property do: MaxDampStrengthX.Get().
            * To set this property do: MaxDampStrengthX.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MaxDampStrengthX;

            /** This property contains the y component of the maximum damp strength of the node
            *
            * To access this property do: MaxDampStrengthY.Get().
            * To set this property do: MaxDampStrengthY.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MaxDampStrengthY;

            /** This property contains the z component of the maximum damp strength of the node
            *
            * To access this property do: MaxDampStrengthZ.Get().
            * To set this property do: MaxDampStrengthZ.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               MaxDampStrengthZ;

            /** This property contains the x component of the preferred angle of the node
            *
            * To access this property do: PreferedAngleX.Get().
            * To set this property do: PreferedAngleX.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               PreferedAngleX;

            /** This property contains the y component of the preferred angle of the node
            *
            * To access this property do: PreferedAngleY.Get().
            * To set this property do: PreferedAngleY.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               PreferedAngleY;

			/** This property contains the z component of the preferred angle of the node
            *
            * To access this property do: PreferedAngleZ.Get().
            * To set this property do: PreferedAngleZ.Set(fbxDouble1).
			* 
			* Default value is 0.
            */
            KFbxTypedProperty<fbxDouble1>               PreferedAngleZ;
            ///////////////////////////////////////////////////////

            /** This property contains lookat property of the node
            *
            * To access this property do: LookAtProperty.Get().
            * To set this property do: LookAtProperty.Set(fbxReference*).
			* 
            */
            KFbxTypedProperty<fbxReference*>            LookAtProperty;

			/** This property contains the up vector property of the node
            *
            * To access this property do: UpVectorProperty.Get().
            * To set this property do: UpVectorProperty.Set(fbxReference*).
			* 
            */
            KFbxTypedProperty<fbxReference*>            UpVectorProperty;

            /** This property contains the show information of the node.
            * As opposed to the Visibility property, this one cannot be animated. The assumed behavior of 
            * this property is to represent the show/hide state of all the nodes attributes connected to this 
            * node only.
            *
            * To access this property do: Show.Get().
            * To set this property do: Show.Set(fbxBool1).
			* 
			* Default value is true.
            *
            * \remarks  \li Because node attributes can be shared by multiple nodes (instances), the FBX SDK provides an utility
            *           function KFbxScene::SyncShowPropertyForInstance() to propagates the same Show value across all the nodes
            *           referencing the node attribute. The applied logic is that as soon as one of these nodes has the Show 
            *           property set to \c false, all will be set to \c false (basically it is an AND operation on all the
            *           Show flags).
            *
            *           \li Depending on the support of the Show and Visibility properties that applications will implement, there 
            *           may be conflicts with these two states. In this case, it is suggested that the Visibility property
            *           always overrides the Show.
            *
            * \see Visibility property.
            */
            KFbxTypedProperty<fbxBool1>                 Show;

            /** This property contains negative percent shape support information of the node
            *
            * To access this property do: NegativePercentShapeSupport.Get().
            * To set this property do: NegativePercentShapeSupport.Set(fbxBool1).
			* 
			* Default value is true.
            */
            KFbxTypedProperty<fbxBool1>                 NegativePercentShapeSupport;

            /** This property contains default attribute index information of the node
            *
            * To access this property do: DefaultAttributeIndex.Get().
            * To set this property do: DefaultAttributeIndex.Set(fbxInteger1).
            * 
            * Default value is -1.
            */
            KFbxTypedProperty<fbxInteger1>              DefaultAttributeIndex;

            /** This property contains manipulation state information of the node
            *
            * To access this property do: Freeze.Get().
            * To set this property do: Freeze.Set(fbxBool1).
            * 
            * Default value is false.
            */
            KFbxTypedProperty<fbxBool1>              Freeze;

            /** This property contains level of detail mode information of the node
            *
            * To access this property do: LODBox.Get().
            * To set this property do: LODBox.Set(fbxBool1).
            * 
            * True: Bounding box
            * False: Geometry object is displayed.
            * Default value is false.
            */
            KFbxTypedProperty<fbxBool1>              LODBox;
        //@}


        #ifndef DOXYGEN_SHOULD_SKIP_THIS
        ///////////////////////////////////////////////////////////////////////////////
        //  WARNING!
        //  Anything beyond these lines may not be documented accurately and is
        //  subject to change without notice.
        ///////////////////////////////////////////////////////////////////////////////
        public:
			/**
			  * This flag is still available for backward compatibility with older 
			  * version of FBX files and should not be used anymore. All the animation layering
			  * system has been moved to the KFbxAnimLayer and KFbxAnimStack classes.
			  */
			//@{
            /** Enable or disable the multilayer state.
              * \param pMultiLayer The new state of the multi-layer flag.
              */
            void SetMultiLayer(bool pMultiLayer);

            /** Get multilayer state.
              * \return The current state of the multi-layer flag.
              */
            bool GetMultiLayer() const;

            typedef enum
            {
                eOLD_MULTI_TAKE,
                eMULTI_TAKE,
                eMONO_TAKE
            } EMultiTakeMode;

            /** Set the multitake mode.
              * \param pMultiTakeMode The multitake mode to set.
              */
            void SetMultiTakeMode(EMultiTakeMode pMultiTakeMode);

            /** Get multitake mode.
              * \return The currently set multitake mode.
              */
            EMultiTakeMode GetMultiTakeMode() const;
			//@}

			/** Get node limits.
              * \return The node limits.
              */
            KFbxNodeLimits& GetLimits();

            /** Notify that a KFbxNode property has changed. 
            *   \param pType Type of property notification
            *   \param pProperty The changed property
            *   \return \c true on success, \c false otherwise.
            */
            virtual bool PropertyNotify(eFbxPropertyNotify pType, KFbxProperty* pProperty);

        protected:
            KFbxNode(KFbxSdkManager& pManager, char const* pName);
            
            virtual void Construct(const KFbxNode* pFrom);
            virtual bool ConstructProperties(bool pForceSet);
            virtual void Destruct(bool pRecursive, bool pDependents);

            void    Reset();
            K_DEPRECATED bool    GetAnimationIntervalRecursive(KTime& pStart, KTime& pStop, KFbxAnimLayer* pAnimLayer);
            bool    GetAnimationIntervalRecursive(KTimeSpan& pTimeInterval, KFbxAnimLayer* pAnimLayer);

        private:
            //! Recursively disable all the TRS limits, reset the Min/Max Limits value, and update the internal Limits cache.
            void    ResetLimitsRecursive(KFbxNode* pNode);

            // a wrapper function for ConvertPivotAnimationRecursive 
            // to avoid stack over flow when exporting scene with deep hierarchy
            void ConvertPivotAnimationRecursiveWrapper(const char * pAnimStackName,
                EPivotSet pConversionTarget, double pFrameRate, bool pKeyReduce );

        public:
            virtual KFbxObject& Copy(const KFbxObject& pObject);

			/** Just add child name to the children name list.
              * \param pChildName The added child name.
              */
            void    AddChildName(char* pChildName);

			 /** According the given index,get child name from the children name list.
               * \param pIndex The given index.
			   * \return The child name.
               */
            char*   GetChildName(kUInt pIndex) const;

			/** Get the count of child names in the children name list.
              * \return The count of child names.
              */
            kUInt   GetChildNameCount() const;

			/** Update pivots and limits from properties.
              */
            void UpdatePivotsAndLimitsFromProperties();
			
			/** Update properties from pivots and limits.
              */
            void UpdatePropertiesFromPivotsAndLimits();

			/** Set rotation active property as given value.
              * \param pVal The given value of rotation active property.
              */
            void SetRotationActiveProperty(bool pVal);

			/** Set MBTransform according the given pivot set.
              * \param pPivotSet Specify according which pivot set to set MBTransform.
              */
            void PivotSetToMBTransform(EPivotSet pPivotSet);

			/** Get type name from the default node attribute.
              * \return The type name.
              */
            virtual const char* GetTypeName() const;

			/** Get type flags from the default node attribute.
              * \return The type flags.
              */
            virtual KStringList GetTypeFlags() const;

        protected:
            KMBTransform*       GetMBTransform();

            // begin character related members
            int AddCharacterLink(KFbxCharacter* pCharacter, int pCharacterLinkType, int pNodeId, int pNodeSubId);
            int RemoveCharacterLink(KFbxCharacter* pCharacter, int pCharacterLinkType, int pNodeId, int pNodeSubId);
            // This class must be declared public, otherwise gcc complains on typedef below.

        private:
            KFbxNode& DeepCopy( KFbxNode const& pNode, bool pCopyNodeAttribute );

        public:
            // Unsupported parameters in the FBX SDK
            // These are declared but not accessible
            // They are used to keep imported and exported data identical
            typedef enum { eCULLING_OFF, eCULLING_ON_CCW, eCULLING_ON_CW } ECullingType;
            ECullingType                mCullingType;

            bool                        mCorrectInheritType;

        protected:
            KFbxNode_internal*          mPH;
            mutable KError              mError;

            friend class KFbxScene;
            friend class KFbxGeometry;
            friend class KFbxLight;
            friend class KFbxNodeFinderDuplicateName;
            friend class KFbxCharacter;
            friend class KFbxControlSet;
            friend class KFbxNode_internal;
            friend class KFbxSurfaceMaterial_internal;
            friend class KFbxTexture_internal;
            friend class KFbxVideo_internal;

            friend class KFbxNodeLimits;
            friend class KFbxLimits;
			friend class KFbxNodeEvalState;
        #endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
    };

    typedef KFbxNode* HKFbxNode;
    typedef class KFBX_DLL KArrayTemplate<KFbxNode*> KArrayKFbxNode;

    inline EFbxType FbxTypeOf( ERotationOrder const &pItem )           { return eENUM; }
    inline EFbxType FbxTypeOf( ETransformInheritType const &pItem )    { return eENUM; }
    inline EFbxType FbxTypeOf( EQuaternionInterpolation const& pItem ) { return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXNODE_H

