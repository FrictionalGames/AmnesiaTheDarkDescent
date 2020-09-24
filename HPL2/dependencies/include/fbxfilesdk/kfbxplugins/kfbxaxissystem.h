/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxaxissystem.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXAXISSYSTEM_H
#define FBXFILESDK_KFBXPLUGINS_KFBXAXISSYSTEM_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>

#include <fbxfilesdk/kfbxmath/kfbxmatrix.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxNode;
class KFbxScene;
class KFbxAnimCurveNode;

/** This class represents the coordinate system of the scene and can convert scenes 
    to other coordinate systems. By default the KFbxScene uses a Y-Up axis
    system. If the calling application wishes to change the default axis it will need to define the new
    axis system and call the convert method with the scene as argument. The appropriate transforms will be
    applied to the first level objects of the scene only (objects whose parent is the scene itself). Child
    objects do not need to be transformed since they inherit from their parents.
    The adjustment will affect the translation animation curves and the objects pivots values (the rotation 
    transformation is applied as a pre-rotation transform therefore the rotation animation curves do not need to
    be transformed). Once converted, the scene will have its axis definition changed to the new system. 

    For example:
    \code
        KFbxScene* lScene = KFbxScene::Create(sdkmanager, "MyScene");
        ...
        // the scene is filled with objects

        int dir;
        lScene->GetGlobalSettings().GetAxisSystem().GetUpVector(dir); // this returns the equivalent of KFbxAxisSystem::YAxis

        KFbxAxisSystem max; // we desire to convert the scene from Y-Up to Z-Up
        max.ConvertScene(lScene);

        lScene->GetGlobalSettings().GetAxisSystem().GetUpVector(dir); // this will now return the equivalent of KFbxAxisSystem::ZAxis
    \endcode

    No conversion will take place if the scene current axis system is equal to the new one.

    The eUpVector specifies which axis has the up and down direction in the system (typically this is the Y or Z axis).
    The sign of the eUpVector is applied to represent the direction (1 is up and -1 is down relative to the observer).
    
    The eFrontVector specifies which axis has the front and back direction in the system. It is not an independent variable, 
	which means it depends on eUpVector. The enum values ParityEven and ParityOdd denote the first one and 
	the second one of the remain two axes in addition to the up axis.
    
    For example if the up axis is X, the remain two axes will be Y And Z, so the ParityEven is Y, and the ParityOdd is Z
    ; If the up axis is Y, the remain two axes will X And Z, so the ParityEven is X, and the ParityOdd is Z; 
    If the up axis is Z, the remain two axes will X And Y, so the ParityEven is X, and the ParityOdd is Y. 
    
    There still needs a parameter to denote the direction of the eFrontVector just as the eUpVector. And the sign of the 
    eFrontVector represents the direction (1 is front and -1 is back relative to observer).
    
    If the front axis and the up axis are determined, the third axis will be automatically determined as the left one. 
    The eCoorSystem enum is a parameter to determine the direction of the third axis just as the eUpVector sign. 
    It determines if the axis system is right-handed or left-handed just as the enum values.

    Some code for reconstructing a KFbxAxisSystem object from reference scene.
	\code
        //the reference scene
        KFbxScene* lSceneReference = KFbxScene::Create(sdkmanager, "ReferenceScene");
        ...
        // the scene is filled with objects

        KFbxAxisSystem lAxisSytemReference = lSceneReference->GetGlobalSettings().GetAxisSystem();

        int lUpVectorSign = 1;
        int lFrontVectorSign = 1;

        //get upVector and its sign.
        eUpVector lUpVector = lAxisSsytemReference.getUpVector( lUpVectorSign );

        //get FrontVector and its sign.
        eFrontVector lFrontVector = lAxisSsytemReference.getFrontVector( lFrontVectorSign );

        //get uCoorSystem. 
        eCoorSystem lCoorSystem = lAxisSsytemReference.GetCoorSystem();

        //The KFbxAxisSystem object to reconstruct back by saved parameter
        KFbxAxisSystem lAxisSytemReconstruct( lUpVectorSign * lUpVector, 
								              lFrontVectorSign * lFrontVector,
								              lCoorSystem);
    \endcode

  * \nosubgrouping
  */
class KFBX_DLL KFbxAxisSystem
{
public:

    /** \enum eUpVector Specifies which canonical axis represents up in the system (typically Y or Z). 
      */
    enum eUpVector {
        XAxis =    1,
        YAxis =    2,
        ZAxis =    3
    };
    
    /** \enum eFrontVector  Vector with origin at the screen pointing toward the camera.
      *                     This is a subset of enum eUpVector because axis cannot be repeated.
	  *                     We use the system of "parity" to define this vector because its value (X,Y or Z axis)
	  *						really depends on the up-vector. The ePreDefinedAxisSystem list the up-vector, parity and
	  *                     coordinate system values for the predefined systems.
      */
    enum eFrontVector {
        ParityEven = 1,
        ParityOdd  = 2
    };

    /** \enum eCoorSystem Specifies the third vector of the system.
	  *                   The KFbxAxisSystem deduces the correct vector and direction based on this flag
	  *                   and the relationship with the up and front vectors. The ePreDefinedAxisSystem list the up-vector, parity and
	  *                   coordinate system values for the predefined systems.
      */
    enum eCoorSystem {
        RightHanded = 0,
        LeftHanded  = 1
    };

    /** \enum ePreDefinedAxisSystem  Enumeration that can be used to initialize a new instance of this class with
	  *                              predefined configurations (see the "Predefined axis systems" section).
      */
    enum ePreDefinedAxisSystem {
        eMayaZUp = 0,		/*!< UpVector = ZAxis, FrontVector = -ParityOdd, CoordSystem = RightHanded */
        eMayaYUp,			/*!< UpVector = YAxis, FrontVector =  ParityOdd, CoordSystem = RightHanded */
        eMax,				/*!< UpVector = ZAxis, FrontVector = -ParityOdd, CoordSystem = RightHanded */
        eMotionBuilder,		/*!< UpVector = YAxis, FrontVector =  ParityOdd, CoordSystem = RightHanded */
        eOpenGL,			/*!< UpVector = YAxis, FrontVector =  ParityOdd, CoordSystem = RightHanded */
        eDirectX,			/*!< UpVector = YAxis, FrontVector =  ParityOdd, CoordSystem = LeftHanded */
        eLightwave			/*!< UpVector = YAxis, FrontVector =  ParityOdd, CoordSystem = LeftHanded */
    };

    /** 
      * \name Constructor and Destructor
      */
    //@{

    /** Constructor!
      * \param pUpVector Specify the up vector.
      * \param pFrontVector Specify the front vector.
      * \param pCoorSystem Specify RightHanded coordinate system or LeftHanded coordinate system.
      */
    KFbxAxisSystem(eUpVector pUpVector, eFrontVector pFrontVector, eCoorSystem pCoorSystem);

    /** Copy constructor!
      * \param pAxisSystem Another KFbxAxisSystem object copied to this one.
      */
    KFbxAxisSystem(const KFbxAxisSystem& pAxisSystem);

    /** Constructor!
      * \param pAxisSystem Specify which predefined axis system to copy.
      */
    KFbxAxisSystem(const ePreDefinedAxisSystem pAxisSystem);

    //! Destructor.
    virtual ~KFbxAxisSystem();
    //@}

    /**
      * \name Boolean operation.
      */
    //@{

    /** Equivalence operator.
      * \param pAxisSystem The axis system to compare against this one.
      * \return \c true if these two axis systems are equal, \c false otherwise.
      */
    bool operator==(const KFbxAxisSystem& pAxisSystem)const;

    /** Non-equivalence operator.
      * \param pAxisSystem The axis system to compare against this one.
      * \return \c true if these two axis systems are unequal, \c false otherwise.
      */
    bool operator!=(const KFbxAxisSystem& pAxisSystem)const;
    //@}

    /** Assignment operation.
      * \param pAxisSystem Axis system assigned to this one.
      */
    KFbxAxisSystem& operator=(const KFbxAxisSystem& pAxisSystem);

	/**
      * \name Predefined axis systems.
	  *	These static members define the axis system of the most popular applications.
      */
    //@{

    //! Predefined axis system: MayaZUp (UpVector = +Z, FrontVector = -Y, CoordSystem = +X (RightHanded))
    static const KFbxAxisSystem MayaZUp;

    //! Predefined axis system: MayaYUp (UpVector = +Y, FrontVector = +Z, CoordSystem = +X (RightHanded))
    static const KFbxAxisSystem MayaYUp;

    //! Predefined axis system: Max (UpVector = +Z, FrontVector = -Y, CoordSystem = +X (RightHanded))
    static const KFbxAxisSystem Max;

    //! Predefined axis system: Motionbuilder (UpVector = +Y, FrontVector = +Z, CoordSystem = +X (RightHanded))
    static const KFbxAxisSystem Motionbuilder;

    //! Predefined axis system: OpenGL (UpVector = +Y, FrontVector = +Z, CoordSystem = +X (RightHanded))
    static const KFbxAxisSystem OpenGL;

    //! Predefined axis system: DirectX (UpVector = +Y, FrontVector = +Z, CoordSystem = -X (LeftHanded))
    static const KFbxAxisSystem DirectX;

    //! Predefined axis system: Lightwave (UpVector = +Y, FrontVector = +Z, CoordSystem = -X (LeftHanded))
    static const KFbxAxisSystem Lightwave;
	//@}

    /** Convert a scene to this axis system. Sets the axis system of the scene to this system unit. 
      * \param pScene     The scene to convert
      */
    void ConvertScene(KFbxScene* pScene) const;

    /** Convert a scene to this axis system by using the specified
      *             node as an Fbx_Root. This is provided for backwards compatibility
      *             only and ConvertScene(KFbxScene* pScene) should be used instead when possible.
      * \param pScene       The scene to convert
      * \param pFbxRoot     The Fbx_Root node that will be transformed.
      */
    void ConvertScene(KFbxScene* pScene, KFbxNode* pFbxRoot) const;
	
    /** Get the eFrontVector and its sign of this axis system.
      * \param pSign The sign of the axis, 1 for front, -1 for back (relative to observer).
      * \return The eFrontVector of this axis system.
      */
    eFrontVector GetFrontVector( int & pSign ) const;

    /** Get the eUpVector and its sign of this axis system.
      * \param pSign The sign of the axis, 1 for up, -1 for down (relative to observer).
      * \return The eUpVector of this axis system.
      */
    eUpVector GetUpVector( int & pSign ) const;

    /** Accessor to the eCoorSystem of this object.
      * \return The current coordinate axis system of this object.
      */
    eCoorSystem GetCoorSystem() const;

    /** Converts the children of the given node to this axis system.
      *             Unlike the ConvertScene() method, this method does not set the axis system 
      *             of the scene that the pRoot node belongs, nor does it adjust KFbxPoses
      *             as they are not stored under the scene, and not under a particular node.
      * \param pRoot The node whose children are converted.
      * \param pSrcSystem The source axis system.
      */
    void ConvertChildren(KFbxNode* pRoot, const KFbxAxisSystem& pSrcSystem) const;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//    Anything beyond these lines may not be documented accurately and is 
//     subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

protected:

    class KFbxAxis
    {
    public:
        enum eAxis {
            eXAxis = 0, eYAxis, eZAxis
        };

        eAxis mAxis;
        int   mSign;

        bool operator==(const KFbxAxis& pAxis)const
        {
            return mAxis == pAxis.mAxis && mSign == pAxis.mSign;
        }
    };

	//! The up vector (and sign) of this object.
    KFbxAxis mUpVector;

	//! The front vector (and sign) of this object.
    KFbxAxis mFrontVector;

	//! The third vector (and sign) of this object.
    KFbxAxis mCoorSystem;

protected:
    /////////////////////////////////////////////////////////////////////////////////
    // Conversion engine, here you will find all the step involved into the conversion

    //! Apply the axis conversion to the translation property of the nodes.
    void ConvertTProperty(KArrayTemplate<KFbxNode*>& pNodes, const KFbxAxisSystem& pFrom) const;

	//! Apply the axis conversion to the translation animation curves.
    void ConvertCurveNodes(KArrayTemplate<KFbxAnimCurveNode*>& pCurveNodes, const KFbxAxisSystem& pFrom) const;

    //! Adjust the Pre rotation to orient the node and children correctly.
    void AdjustPreRotation(KFbxNode* pNode, const KFbxMatrix& pConversionRM) const;

	//! Adjust the pivots of the node for the new axis system.
    void AdjustPivots(KFbxNode* pNode, const KFbxMatrix& pConversionRM) const;

	//! Computes the transformation matrix to pass from the \e pFrom system to this one.
    void GetConversionMatrix(const KFbxAxisSystem& pFrom, KFbxMatrix& pConversionRM) const;

	//! Adjust the node translation limits for the new axis system.
    void AdjustLimits(KFbxNode* pNode, const KFbxMatrix& pConversionRM) const;

	//! Adjust all the poses in the \e pScene for the new axis system.
    void AdjustPoses(KFbxScene* pScene, const KFbxMatrix& pConversionRM) const;

	//! Recompute the camera's up-vector for the new axis system.
    void AdjustCamera(KFbxNode* pNode, const KFbxMatrix& pConversionRM ) const;

	//! Adjust the node cluster's matrices for the new axis system.
    void AdjustCluster(KFbxNode* pNode, const KFbxMatrix& pConversionRM) const;

	//! This is the main function that will dispatch all the calls of the "Adjust" methods.
    void ConvertChildren(KFbxNode* pRoot, const KFbxAxisSystem& pSrcSystem, bool pSubChildrenOnly) const;

    friend class KFbxGlobalSettings;
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>


#endif // FBXFILESDK_KFBXPLUGINS_KFBXAXISSYSTEM_H

