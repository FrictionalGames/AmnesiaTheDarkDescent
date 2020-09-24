/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxscene.h
 */
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSCENE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSCENE_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/fbxcore/fbxcollection/kfbxdocument.h>

#include <fbxfilesdk/kfbxplugins/kfbxdeformer.h>
#include <fbxfilesdk/kfbxplugins/kfbxsubdeformer.h>
#include <fbxfilesdk/kfbxplugins/kfbxlayer.h>
#include <fbxfilesdk/kfbxplugins/kfbxnodeattribute.h>
#include <fbxfilesdk/kfbxplugins/kfbxglobalsettings.h>
#include <fbxfilesdk/kfbxplugins/kfbxanimevaluator.h>

#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/kerror.h>
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/kcharptrset.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;
class KFbxObject;
class KFbxNode;
class KFbxVideo;
class KFbxTexture;
class KFbxSurfaceMaterial;
class KFbxMarkerSet;
class KFbxActor;
class KFbxCharacter;
class KFbxControlSetPlug;
class KFbxCharacterPose;
class KFbxPose;
class KFbxDocumentInfo;
class KFbxTakeInfo;
class KFbxGlobalSettings;
class KFbxGenericNode;
class KFbxConstraint;
class KFbxGeometry;
class KFbxLayerContainer;
class KSet;

class KFbxGlobalLightSettings;
class KFbxGlobalCameraSettings;
class KFbxGlobalTimeSettings;


/** This class contains the description of a 3D scene. It contains the nodes (including the root node) (KFbxNode),
  * materials, textures, videos, gobos, 
  * poses, characters, character poses, control set plugs, 
  * generic nodes, 
  * scene information, global settings,
  * and a global evaluator.
  * The nodes are structured in a tree under the scene's root node.
  *
  * When an object is created using the FBX SDK, a scene is usually passed as argument to the
  * object creation function to specify that the object belongs to this scene.
  * At this point, a connection is made with the object as source and the scene as destination. 
  * 
  * All objects in the scene can be queried by connection index. In addition,
  * generic nodes, materials, and textures can also be queried by name. In this latter case, the
  * first object with the queried name will be returned.
  *
  * The global evaluator (KFbxAnimEvaluator) is used to compute animation values
  * for animated scenes.
  * \nosubgrouping
  */
class KFBX_DLL KFbxScene : public KFbxDocument
{
    KFBXOBJECT_DECLARE(KFbxScene, KFbxDocument);

public:
    /**
      * \name Clear scene
      */
    //@{
    //! Delete the node tree below the root node and restore default settings.
    void Clear();
	//@}

    /**
      * \name Node Tree Access
      */
    //@{

    /** Get the root node of the scene.
      * \return Pointer to the root node.
      * \remarks This node is not saved. Do not use it to apply a global transformation
      * to the node hierarchy. If a global transformation must be applied, insert a
      * new node below this one.
      */
    KFbxNode* GetRootNode() const;
    //@}

    /**
      * \name Texture Material and Video Access
      */
    //@{

    /** Clear, then fill, a texture array with all existing textures included in the scene.
      * \param pTextureArray An array of texture pointers.
      */
    void FillTextureArray(KArrayTemplate<KFbxTexture*>& pTextureArray);

    /** Clear, then fill, a material array with all existing materials included in the scene.
      * \param pMaterialArray An array of material pointers.
      */
    void FillMaterialArray(KArrayTemplate<KFbxSurfaceMaterial*>& pMaterialArray);

    //@}

    /**
      * \name Generic Node Access
      */
    //@{

    /** Get number of generic nodes in the scene.
      * \return Number of Generic Nodes in this scene.
      */
    int             GetGenericNodeCount() const;

    /** Get generic node at given index.
      * \param pIndex Position in the list of the generic nodes.
      * \return Pointer to the generic node or \c NULL if the index is out of bounds.
      */
    KFbxGenericNode* GetGenericNode(int pIndex);

    /** Access a generic node from its name.
    *   \param pName Name of the generic node.
    *   \return found generic node
    */
    KFbxGenericNode* GetGenericNode(char* pName);

    /** Add a generic node to this scene.
      * \param pGenericNode Pointer to the generic node to be added.
      * \return If the passed parameter is \c NULL, this method will return \c false, otherwise \c true.
      */
    bool             AddGenericNode(KFbxGenericNode* pGenericNode);

    /** Remove the generic node from this scene.
      * \param pGenericNode Pointer to the generic node to be removed.
      * \return If the passed parameter is \c NULL, this method will return \c false, otherwise \c true.
      * \remarks The pointed object is not referenced by the scene anymore but is not deleted.
      */
    bool             RemoveGenericNode(KFbxGenericNode* pGenericNode);

    //@}


    /**
      * \name Character Management
      */
    //@{

    /** Get number of characters.
      * \return Number of characters in this scene.
      */
    int GetCharacterCount() const;

    /** Get character at given index.
      * \param pIndex Position in the list of the characters.
      * \return Pointer to the character or \c NULL if index is out of bounds.
      */
    KFbxCharacter* GetCharacter(int pIndex);

    /** Create a new character.
      * \param pName Name given to character.
      * \return Index of the created character.
      */
    int CreateCharacter(char* pName);

    /** Destroy character.
      * \param pIndex Specify which character to destroy.
      */
    void DestroyCharacter(int pIndex);

    //@}

    /**
      * \name ControlSetPlug Management
      */
    //@{

    /** Get number of ControlSetPlugs.
      * \return Number of ControlSet plugs in this scene.
      */
    int GetControlSetPlugCount() const;

    /** Get ControlSetPlug at given index.
      * \param pIndex Position in the list of the ControlSetPlug
      * \return Pointer to ControlSetPlug or \c NULL if index is out of bounds.
      */
    KFbxControlSetPlug* GetControlSetPlug(int pIndex);

    /** Create a new ControlSetPlug.
      * \param pName Name given to ControlSetPlug.
      * \return Index of created ControlSetPlug.
      */
    int CreateControlSetPlug(char* pName);

    /** Destroy ControlSetPlug.
      * \param pIndex Specify which ControlSetPlug to destroy.
      */
    void DestroyControlSetPlug(int pIndex);

    //@}

    /**
      * \name Character Pose Management
      */
    //@{

    /** Get number of character poses.
      * \return Number of character poses in this scene.
      * \remarks Character Poses and Poses are two distinct entities having their own lists.
      */
    int GetCharacterPoseCount() const;

    /** Get character pose at given index.
      * \param pIndex Position in the list of character poses.
      * \return Pointer to the character pose or \c NULL if index is out of bounds.
      */
    KFbxCharacterPose* GetCharacterPose(int pIndex);

    /** Create a new character pose.
      * \param pName Name given to character pose.
      * \return Index of created character pose.
      */
    int CreateCharacterPose(char* pName);

    /** Destroy character pose.
      * \param pIndex Specify which character pose to destroy.
      */
    void DestroyCharacterPose(int pIndex);

    //@}

    /**
      * \name Pose Management
      */
    //@{

    /** Get number of poses.
      * \return Number of poses in the scene.
      * \remarks Poses and Character Poses are two distinct entities having their own lists.
      */
    int GetPoseCount() const;

    /** Get pose at given index.
      * \param pIndex Position in the list of poses.
      * \return Pointer to the pose or \c NULL if index is out of bounds.
      */
    KFbxPose* GetPose(int pIndex);

    /** Add a pose to this scene.
      * \param pPose The pose (for example: bind pose, rest pose) to be added to the scene.
      * \return If the pose is correctly added to the scene, return \c true. Otherwise, if the pose is
      * already in the scene, return \c false.
      */
    bool AddPose(KFbxPose* pPose);

    /** Remove the specified pose from the scene.
      * \param pPose The pose (for example: bind pose, rest pose) to be removed from the scene.
      * \return If the pose was successfully removed from the scene, return \c true. Otherwise, if the
      * pose could not be found return \c false.
      */
    bool RemovePose(KFbxPose* pPose);

    /** Remove the pose at the given index from the scene.
      * \param pIndex Index of the pose to be removed.
      * \return If the pose was successfully removed from the scene, return \c true. Otherwise, if the
      * pose could not be found return \c false.
      */
    bool RemovePose(int pIndex);


    //@}
    /**
      * \name Scene information
      */
    //@{

    /** Get the scene information.
      * \return Pointer to the scene information object.
      */
    inline KFbxDocumentInfo* GetSceneInfo() { return GetDocumentInfo(); }

    /** Set the scene information.
      * \param pSceneInfo Pointer to the scene information object.
      */
    inline void SetSceneInfo(KFbxDocumentInfo* pSceneInfo) { SetDocumentInfo(pSceneInfo); }

    //@}

    /**
      * \name Global Settings
      */
    //@{

    /** Access global settings.
      * \return Reference to the Global Settings.
      */
    KFbxGlobalSettings& GetGlobalSettings();

    /** Const access to global settings.
      * \return Const reference to the Global Settings.
      */
    const KFbxGlobalSettings& GetGlobalSettings() const;
    //@}

    /**
      * \name Global Evaluator
	  * The global evaluator is used to compute animation values
      * for animated scenes.
	  * A typical usage would be to compute the global transform
	  * matrix of a node \c lNode at a given time \c lTime.
	  * \code
	  KFbxXMatrix& lGlobalMatrix = lNode->GetScene()->GetEvaluator()->GetNodeGlobalTransform(lNode, lTime);

	  or the exact equivalent:

	  KFbxXMatrix& lGlobalMatrix = lNode->EvaluateGlobalTransform(lTime);
	  * \endcode
	  *
	  * The user can create one or more evaluators in the scene.
	  * The default evaluator is set using SetEvaluator.
	  * When GetEvaluator is called, if the scene has no evaluator,
	  * an evaluator is created with default values.
      */
    //@{

		/** Set the global evaluator used by this scene evaluation engine.
		  * \param pEvaluator The evaluator to be used for evaluation processing of this scene.
		  */
		void SetEvaluator(KFbxAnimEvaluator* pEvaluator);

		/** Get the global evaluator used by this scene evaluation engine.
		  * If no evaluator were previously set, this function will return either the
		  * first evaluator found attached to this scene, or a new default evaluator.
		  * \return The evaluator to be used for evaluation processing of this scene.
		  */
		KFbxAnimEvaluator* GetEvaluator();

    //@}

    /** \deprecated Clear then fill a node array with all existing nodes included in the scene.
      * \param pNodeArray An array of node pointers.
      */
    K_DEPRECATED void FillNodeArray(KArrayTemplate<KFbxNode*>& pNodeArray);

    /** Clear then fill a pose array with all existing pose included in the scene.
      * \param pPoseArray An array of pose pointers.
      */
    void FillPoseArray(KArrayTemplate<KFbxPose*>& pPoseArray);


    /**
      * \name Material Access
      */
    //@{

    /** Get number of materials.
      * \return Number of materials in this scene.
      */
    int                     GetMaterialCount            () const;

    /** Get the material at the given index.
      * \param pIndex Position in the list of materials.
      * \return Pointer to the material or \c NULL if the index is out of bounds.
      * \remarks pIndex must be between 0 and GetMaterialCount().
      */
    KFbxSurfaceMaterial*            GetMaterial                 (int pIndex);

    /** Get the material by its name.
      * \param pName Name of the material.
      * \return Pointer to the material or \c NULL if not found.
      */
    KFbxSurfaceMaterial*            GetMaterial                 (char* pName);

    /** Add the material to this scene.
      * \param pMaterial Pointer to the material to be added.
      * \return true on successful addition.
      */
    bool                    AddMaterial                 (KFbxSurfaceMaterial* pMaterial);

    /** Remove the material from this scene.
      * \param pMaterial Pointer to the material to be removed.
      * \return true on successful removal.
      */
    bool                    RemoveMaterial              (KFbxSurfaceMaterial* pMaterial);

    //@}

    /**
      * \name Texture Access
      */
    //@{

    /** Get number of textures (type KFbxTexture).
      * \return Number of textures in this scene. Includes types KFbxFileTexture, KFbxLayeredTexture and KFbxProceduralTexture.
      * \remarks To get the number of textures of a specific type, use GetSrcCount(). For example:
	  * \code
	  * int lNbFileTextures = lScene->GetSrcObjectCount(FBX_TYPE(KFbxFileTexture));
	  * int lNbLayeredTextures = lScene->GetSrcObjectCount(FBX_TYPE(KFbxLayeredTexture));
	  * int lNbProceduralTextures = lScene->GetSrcObjectCount(FBX_TYPE(KFbxProceduralTexture));
	  * \endcode
	  */
    int                     GetTextureCount             () const;

    /** Get the texture at the given index. pIndex must be between 0 and GetTextureCount().
      * \param pIndex Position in the list of textures.
      * \return Pointer to the texture or \c NULL if the index is out of bounds.
      * \remarks To get the texture of a specific texture type, use GetSrcObject(). For example:
	  * \code
	  * KFbxFileTexture* lFileTexture = lScene->GetSrcObject(FBX_TYPE(KFbxFileTexture), i);
	  * KFbxLayeredTexture* lLayeredTexture = lScene->GetSrcObject(FBX_TYPE(KFbxLayeredTexture), i);
	  * KFbxProceduralTexture* lProceduralTexture = lScene->GetSrcObject(FBX_TYPE(KFbxProceduralTexture), i);
	  * \endcode
      */
    KFbxTexture*            GetTexture                  (int pIndex);

    /** Get the texture by its name.
      * \param pName Name of the texture.
      * \return Pointer to the texture or \c NULL if not found.
      */
    KFbxTexture*            GetTexture                  (char* pName);

    /** Add the texture to this scene.
      * \param pTexture Pointer to the texture to be added.
      * \return \c true on successful addition.
      */
    bool                    AddTexture                  (KFbxTexture* pTexture);

    /** Remove the texture from this scene.
      * \param pTexture Pointer to the texture to be removed.
      * \return \c true on successful removal.
      */
    bool                    RemoveTexture               (KFbxTexture* pTexture);

    //@}

    /**
      * \name Node Access
      */
    //@{

    /** Get number of nodes.
      * \return Number of nodes in this scene.
      */
    int                     GetNodeCount                () const;

    /** Get the node at the given index.
      * \param pIndex Position in the list of nodes.
      * \return Pointer to the node or \c NULL if the index is out of bounds.
      * \remarks pIndex must be between 0 and GetNodeCount().
      */
    KFbxNode*               GetNode                     (int pIndex);

    /** Add the node to this scene.
      * \param pNode Pointer to the node to be added.
      * \return true on successful addition.
      */
    bool                    AddNode                     (KFbxNode* pNode);

    /** Remove the node from this scene.
      * \param pNode Pointer to the node to be removed.
      * \return true on successful removal.
      */
    bool                    RemoveNode                  (KFbxNode* pNode);

    /** Helper method for determining the number of nodes that have
      * curves on surface attributes in the scene. Since the curve-on-surface
      * nodes are connected to nurbs geometry and not any KFbxNodes in the
      * scene, they won't normally be picked up in a graph traversal.
      * \return The number of curve-on-surface nodes in the scene
      */
    int                     GetCurveOnSurfaceCount      ();

	/** Get the first node with this name.
	  * \param pName Name of the node.
	  * \return Pointer to the node, or \c NULL if node is not found.
	  */
    KFbxNode*               FindNodeByName              ( const KString& pName );

    //@}

    /**
      * \name Geometry Access
      */
    //@{

    /** Get number of geometries.
      * \return Number of geometries in this scene.
      */
    int                     GetGeometryCount            () const;

    /** Get the geometry at the given index.
      * \param pIndex Position in the list of geometries.
      * \return Pointer to the geometry or \c NULL if the index is out of bounds.
      * \remarks pIndex must be between 0 and GetGeometryCount().
      */
    KFbxGeometry*           GetGeometry                 (int pIndex);

    /** Add the geometry to this scene.
      * \param pGeometry Pointer to the geometry to be added.
      * \return true on successful addition.
      */
    bool                    AddGeometry                 (KFbxGeometry* pGeometry);

    /** Remove the geometry from this scene.
      * \param pGeometry Pointer to the geometry to be removed.
      * \return true on successful removal.
      */
    bool                    RemoveGeometry              (KFbxGeometry* pGeometry);

    //@}

    /**
      * \name Video Access
      */
    //@{

    /** Get number of videos.
      * \return Number of videos in this scene.
      */
    int                     GetVideoCount               () const;

    /** Get the video at the given index.
      * \param pIndex Position in the list of videos.
      * \return Pointer to the video or \c NULL if the index is out of bounds.
      * \remarks pIndex must be between 0 and GetVideoCount().
      */
    KFbxVideo*              GetVideo                    (int pIndex);

    /** Add the video to this scene.
      * \param pVideo Pointer to the video to be added.
      * \return true on successful addition.
      */
    bool                    AddVideo                    (KFbxVideo* pVideo);

    /** Remove the video from this scene.
      * \param pVideo Pointer to the video to be removed.
      * \return true on successful removal.
      */
    bool                    RemoveVideo                 (KFbxVideo* pVideo);

    //@}

    /**
      * \name Utilities
      */
    //@{
    /** Synchronize all the Show properties of node instances.
      * Walks all the node attributes defined in the scene and synchronize the Show property
      * of all the nodes that reference the node attribute so that they all contain the same
      * value. This method should be called after the FBX scene is completely created (typically
      * right after the calls to the KFbxImporter::Import() or just before the calls to the 
      * KFbxExporter::Export().
      *
      * \remarks Applications only need to call this method if their interpretation of the Show
      *          property implies that setting the Show state on one instance affect all of them.
      *         
      * \see KFbxNode::Visibility property, KFbxNode::Show property
      */
    void SyncShowPropertyForInstance();
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
    virtual KFbxObject& Copy(const KFbxObject& pObject);

    void ConnectMaterials();

    void BuildMaterialLayersDirectArray();
    void ReindexMaterialConnections(); // called to make sure that eINDEX is remapped to eINDEX_TO_DIRECT

    KSet* AddTakeTimeWarpSet(char *pTakeName);
    KSet* GetTakeTimeWarpSet(char *pTakeName);

	// This function will destroy the scene (and all the objects directly connected to it) without sending 
	// the Connect notifications nor trying to disconnect the objects first. This is a bypass of the intended
	// workflow and should be used with care.
	void ForceKill();

private:
    KFbxScene(KFbxSdkManager& pManager, char const* pName);

    virtual void Construct(const KFbxScene* pFrom);
    virtual void Destruct(bool pRecursive, bool pDependents);

    // Don't mark it as DEPRECATED to avoid useless warning. This method is only called by
    // FillNodeArray() and the method is already makred as deprecated. When FillNodeArray is going to 
    // be removed, FillNodeArrayRecursive will be too.
    void FillNodeArrayRecursive(KArrayTemplate<KFbxNode*>& pNodeArray, kFbxClassId classId, KFbxNode* pNode);

    void ConnectTextureLayerElement(KFbxLayerContainer* pLayerContainer,
                                    KFbxLayerElement::ELayerElementType pLayerType,
                                    KFbxNode* pParentNode);

    void BuildTextureLayersDirectArrayForLayerType(KFbxLayerContainer* pLayerContainer,
                                                   KFbxLayerElement::ELayerElementType pLayerType);

public:
    void ConvertNurbsSurfaceToNurb();
    void ConvertMeshNormals();
    void ConvertNurbCurvesToNulls();
    void ConnectTextures();
    void BuildTextureLayersDirectArray();
    void FixInheritType(KFbxNode *pNode);

    /**
     * Recursively convert transform inherit type from eINHERIT_Rrs to eINHERIT_RSrs by compensating object scale.
     * \param pNode Root node. Conversion will traverse its children recursively.
     * \param pIOS  The IO options.  
     */
    void UpdateScaleCompensate(KFbxNode *pNode, KFbxIOSettings& pIOS);

    kFbxClassId ConvertAttributeTypeToClassID(KFbxNodeAttribute::EAttributeType pAttributeType);

    /** 
     * Internally used by the FBX version 6.x and earlier readers/writers. 
     * The relevant data that is contained in these objects have been moved into the KFbxGlobalSettings(). 
     * Therefore, any previous calls to these methods should be replaced by the above. 
     */
    KFbxGlobalLightSettings&  GlobalLightSettings()  { return *mGlobalLightSettings; }
     /** Internally used by the FBX version 6.x and earlier readers/writers.  */
    KFbxGlobalCameraSettings& GlobalCameraSettings() { return *mGlobalCameraSettings; }
     /** Internally used by the FBX version 6.x and earlier readers/writers.  */
    KFbxGlobalTimeSettings&   GlobalTimeSettings()   { return *mGlobalTimeSettings; }

private:
    KFbxNode*                           mRootNode;

    KFbxGlobalLightSettings*            mGlobalLightSettings;
    KFbxGlobalCameraSettings*           mGlobalCameraSettings;
    KFbxGlobalTimeSettings*             mGlobalTimeSettings;

	KFbxAnimEvaluator*					mEvaluator;

    KCharPtrSet                         mTakeTimeWarpSet;

    friend class KFbxLayerContainer;
    friend class KFbxNodeFinderDuplicateName;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

typedef  class KFBX_DLL KArrayTemplate<KFbxTakeInfo *>          KArrayKFbxTakeInfo;
typedef KFbxScene* HKFbxScene;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSCENE_H

