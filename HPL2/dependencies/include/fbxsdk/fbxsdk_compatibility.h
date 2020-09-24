/****************************************************************************************
 
   Copyright (C) 2012 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxsdk_compatibility.h
#ifndef _FBXSDK_COMPATIBILITY_H_
#define _FBXSDK_COMPATIBILITY_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

#pragma message("IMPORTANT NOTE: The FBX SDK API changed substantially. Please define FBXSDK_NEW_API in your project and fix compilation errors as instructed in fbxsdk_compatibility.h.")

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Here is a list of types that cannot be typedef'd here, please search and replace your code with these:
//
//	KFbxCast							->	FbxCast
//	KFbxIteratorSrc						->	FbxIteratorSrc
//	KFbxIteratorDst						->	FbxIteratorDst
//	KFbxForEach							->	FbxForEach
//	KFbxConnectSrc						->	Dst->ConnectSrcObject(Src)
//	KFbxConnectDst						->	Src->ConnectDstObject(Dst)
//	FbxConnectSrc(Dst, Src)				->	Dst->ConnectSrcObject(Src)
//	FbxConnectDst(Src, Dst)				->	Src->ConnectDstObject(Dst)
//	FbxGetSrcCount<Type>(...)			->	GetSrcObjectCount<Type>(...)
//	FbxGetSrc<Type>(...)				->	GetSrcObject<Type>(...)
//	FbxFindSrc<Type>(...)				->	FindSrcObject<Type>(...)
//	FbxDisconnectAllSrc<Type>(...)		->	DisconnectAllSrcObject<Type>(...)
//	FbxGetDstCount<Type>(...)			->	GetDstObjectCount<Type>(...)
//	FbxGetDst<Type>(...)				->	GetDstObject<Type>(...)
//	FbxFindDst<Type>(...)				->	FindDstObject<Type>(...)
//	FbxDisconnectAllDst<Type>(...)		->	DisconnectAllDstObject<Type>(...)
//	KFbxTypedProperty					->	FbxPropertyT
//	KFbxGet								->	Property.Get<Type>()
//	KFbxSet								->	Property.Set(Value)
//	KFbxPropertyGet						->	Property.Get<Type>()
//	KFbxPropertySet						->	Property.Set(Value)
//	KFbxProperty.Get(Type)				->	Property.Get<Type>()
//	KFbxProperty.Set(Value, Type)		->	Property.Set(Value)
//	KArrayTemplate						->	FbxArray
//	FbxSdkDeleteAndClear				->	FbxArrayDelete
//	DeleteAndClear						->	FbxArrayDelete
//	KFbxLayerElementArrayTemplate		->	FbxLayerElementArrayTemplate
//	KFbxUnitConversionOptions			->	FbxSystemUnit::ConversionOptions
//	KFbxIOSettings::GetKTimeProp		->	FbxIOSettings::GetTimeProp
//	KFbxIOSettings::SetKTimeProp		->	FbxIOSettings::SetTimeProp
//	KFbxSdkManager::GetSrcObjectCount	->	FbxScene::GetSrcObjectCount
//	KFbxSdkManager::GetSrcObject		->	FbxScene::GetSrcObject
//	FbxManagerGetSrcCount				->	FbxScene::GetSrcObjectCount
//	FbxManagerGetSrc					->	FbxScene::GetSrcObject
//	KFbxObject::GetUpdateId				->	no replacement, function removed
//	KFbxNode::GetMultiLayer				->	no replacement, function removed
//	KFbxNode::SetMultiLayer				->	no replacement, function removed
//	KFbxNode::GetMultiTakeMode			->	no replacement, function removed
//	KFbxNode::SetMultiTakeMode			->	no replacement, function removed
//	KFbxUTF8ToWidechar					->	FbxUTF8ToWC
//	KFbxWideCharToUTF8					->	FbxWCToUTF8
//	KFbxWideCharToAnsi					->	FbxWCToAnsi
//	KFbxAnsiToWideChar					->	FbxAnsiToWC
//	KFbxAnsiToUTF8						->	FbxAnsiToUTF8
//	KFbxUTF8ToAnsi						->	FbxUTF8ToAnsi
//	RemoveChar							->	FbxRemoveChar
//	KRegisterAssertStubFct				->	FbxAssertSetProc
//	kGetTempPath						->	FbxGetSystemTempPath
//	KFbxExtractFileName					->	FbxPathUtils::GetFileName
//	KFbxCleanPath						->	FbxPathUtils::Clean
//	KFbxExtractFileExtension			->	FbxPathUtils::GetExtensionName
//	KFbxExtractDirectory				->	FbxPathUtils::GetFolderName
//	KTime_SetGlobalTimeMode				->	FbxSetGlobalTimeMode
//	KTime_GetFrameRate					->	FbxGetFrameRate
//	KLibraryGetProc						->	FbxModuleGetProc
//
//	Most KFbxConstraint Set and Get function calls should be replaced with their property access directly.
//
//  For all the other errors, please refer to the convertion typedefs below for search and replace in your code.
//


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Now we redefine previous defines using new ones
#if defined(FBXSDK_ENV_WIN)
	#define KARCH_ENV_WIN				1
	#define KARCH_ENV_WIN32				1
#elif defined(FBXSDK_ENV_MAC)
	#define KARCH_ENV_MACOSX			1
	#define KARCH_ENV_POSIX				1
#elif defined(FBXSDK_ENV_LINUX)
	#define KARCH_ENV_LINUX				1
	#define KARCH_ENV_POSIX				1
#endif

#if defined(FBXSDK_ARCH_32)
	#define KARCH_ARCH_IA32				1
#elif defined(FBXSDK_ARCH_64)
	#define KARCH_ARCH_64				1
#endif

#if defined(FBXSDK_COMPILER_MSC)
	#define KARCH_DEV_MSC				_MSC_VER
#elif defined(FBXSDK_COMPILER_GNU)
	#define KARCH_DEV_GNUC				__GNUC__
#endif

#define KFBX_DLL						FBXSDK_DLL
#define KFBX_DLLEXPORT					FBXSDK_DLLEXPORT
#define KFBX_DLLIMPORT					FBXSDK_DLLIMPORT

#define KFBX_SAFE_DELETE(p)				FBX_SAFE_DELETE(p)
#define KFBX_SAFE_DELETE_ARRAY(p)		FBX_SAFE_DELETE_ARRAY(p)
#define KFBX_SAFE_DESTROY_OBJECT(p)		FBX_SAFE_DESTROY(p)

#define K_ASSERT(c)						FBX_ASSERT(c)
#define K_ASSERT_MSG(c,m)				FBX_ASSERT_MSG(c,m)
#define K_ASSERT_MSG_NOW(m)				FBX_ASSERT_NOW(m)
#define K_CHECKRTN(c)					FBX_ASSERT_RETURN(c)
#define K_CHECKRTNV(c,v)				FBX_ASSERT_RETURN_VALUE(c,v)
#define K_STATIC_ASSERT(c)				FBX_ASSERT_STATIC(c)

#define K_FLOAT_EPSILON					FBXSDK_FLOAT_EPSILON
#define K_DOUBLE_EPSILON				FBXSDK_DOUBLE_EPSILON

#define KFBXOBJECT_DECLARE(c,p)			FBXSDK_OBJECT_DECLARE(c,p)
#define KFBXOBJECT_IMPLEMENT(c)			FBXSDK_OBJECT_IMPLEMENT(c)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Now let's try to typedef as much as we can
typedef void*							kPtr;
typedef FbxBool							kBool;
typedef FbxChar							kByte;
typedef FbxUChar						kUByte;
typedef FbxShort						kShort;
typedef FbxUShort						kUShort;
typedef FbxInt							kInt;
typedef FbxUInt							kUInt;
typedef FbxLong							kLong;
typedef FbxULong						kULong;
typedef FbxLongLong						kLongLong;
typedef FbxULongLong					kULongLong;
typedef FbxFloat						kFloat;
typedef FbxDouble						kDouble;
typedef FbxHandle						kReference;
typedef FbxDouble3						fbxDouble3;
typedef FbxInt16						kInt16;
typedef FbxUInt16						kUInt16;
typedef FbxInt32						kInt32;
typedef FbxUInt32						kUInt32;
typedef FbxInt64						kInt64;
typedef FbxUInt64						kUInt64;
typedef FbxCharPtr						kTCharPtr;

typedef FbxModule						kLibHandle;

//typedef FbxError						KError;
typedef FbxString						KString;
typedef FbxStringList					KStringList;
typedef FbxStringCompare				KStringCompare;
typedef FbxCharCompare					KCharCompare;
typedef FbxCharPtrSet					KCharPtrSet;
typedef FbxArray<FbxHandle>				KArrayUL;
typedef FbxFolder						KDir;
typedef FbxFile							KFile;
typedef FbxTime							KTime;
typedef FbxTimeSpan						KTimeSpan;
typedef FbxSet							KSet;
typedef FbxNameHandler					KName;
typedef FbxColor						KFbxColor;
typedef FbxVector2						KFbxVector2;
typedef FbxVector4						KFbxVector4;
typedef FbxMatrix						KFbxMatrix;
typedef FbxAMatrix						KFbxXMatrix;

typedef FbxBlob							fbxBlob;
typedef FbxString						fbxString;
typedef FbxDateTime						fbxDateTime;

typedef FbxClassId						kFbxClassId;
typedef FbxObject						KFbxObject;
typedef FbxCollection					KFbxCollection;
typedef FbxCollectionExclusive			KFbxCollectionExclusive;
typedef FbxDataType						KFbxDataType;
typedef FbxProperty						KFbxProperty;
typedef FbxManager						KFbxSdkManager;
typedef FbxNode							KFbxNode;
typedef FbxNodeAttribute				KFbxNodeAttribute;
typedef FbxCamera						KFbxCamera;
typedef FbxCameraSwitcher				KFbxCameraSwitcher;
typedef FbxCameraStereo					KFbxCameraStereo;
typedef FbxGeometry						KFbxGeometry;
typedef FbxGeometryBase					KFbxGeometryBase;
typedef FbxLayer						KFbxLayer;
typedef FbxLayerElement					KFbxLayerElement;
typedef FbxLayerElementArray			KFbxLayerElementArray;
typedef FbxLayerElementNormal			KFbxLayerElementNormal;
typedef FbxLayerElementMaterial			KFbxLayerElementMaterial;
typedef FbxLayerElementVisibility		KFbxLayerElementVisibility;
typedef	FbxLayerElementVertexColor		KFbxLayerElementVertexColor;
typedef FbxLayerElementSmoothing		KFbxLayerElementSmoothing;
typedef FbxLayerElementUV				KFbxLayerElementUV;
typedef FbxLayerElementPolygonGroup		KFbxLayerElementPolygonGroup;
typedef FbxLayerElementTexture			KFbxLayerElementTexture;
typedef FbxMesh							KFbxMesh;
typedef FbxMarker						KFbxMarker;
typedef FbxSkeleton						KFbxSkeleton;
typedef FbxNull							KFbxNull;
typedef FbxOpticalReference				KFbxOpticalReference;
typedef FbxLight						KFbxLight;
typedef FbxLine							KFbxLine;
typedef FbxNurbs						KFbxNurb;
typedef FbxNurbsSurface					KFbxNurbsSurface;
typedef FbxNurbsCurve					KFbxNurbsCurve;
typedef FbxTrimNurbsSurface				KFbxTrimNurbSurface;
typedef FbxPatch						KFbxPatch;
typedef FbxTexture						KFbxTexture;
typedef FbxFileTexture					KFbxFileTexture;
typedef FbxLayeredTexture				KFbxLayeredTexture;
typedef FbxProceduralTexture			KFbxProceduralTexture;
typedef FbxSurfaceMaterial				KFbxSurfaceMaterial;
typedef FbxSurfaceLambert				KFbxSurfaceLambert;
typedef FbxSurfacePhong					KFbxSurfacePhong;
typedef FbxVideo						KFbxVideo;
typedef FbxCache						KFbxCache;
typedef FbxVertexCacheDeformer			KFbxVertexCacheDeformer;
typedef FbxDeformer						KFbxDeformer;
typedef FbxCluster						KFbxCluster;
typedef FbxSkin							KFbxSkin;
typedef FbxScene						KFbxScene;
typedef FbxDocument						KFbxDocument;
typedef FbxDocumentInfo					KFbxDocumentInfo;
typedef FbxAnimCurve					KFbxAnimCurve;
typedef FbxAnimCurveBase				KFbxAnimCurveBase;
typedef FbxAnimCurveKey					KFbxAnimCurveKey;
typedef FbxAnimCurveNode				KFbxAnimCurveNode;
typedef FbxAnimCurveDef					KFbxAnimCurveDef;
typedef FbxAnimStack					KFbxAnimStack;
typedef FbxAnimLayer					KFbxAnimLayer;
typedef FbxAnimEvaluator				KFbxAnimEvaluator;
typedef FbxSurfaceMaterial				KFbxSurfaceMaterial;
typedef FbxConstraint					KFbxConstraint;
typedef FbxConstraintAim				KFbxConstraintAim;
typedef FbxConstraintCustom				KFbxConstraintCustom;
typedef FbxConstraintParent				KFbxConstraintParent;
typedef FbxConstraintPosition			KFbxConstraintPosition;
typedef FbxConstraintRotation			KFbxConstraintRotation;
typedef FbxConstraintScale				KFbxConstraintScale;
typedef FbxConstraintSingleChainIK		KFbxConstraintSingleChainIK;
typedef FbxCharacter					KFbxCharacter;
typedef FbxCharacterLink				KFbxCharacterLink;
typedef FbxCharacterPose				KFbxCharacterPose;
typedef FbxControlSet					KFbxControlSet;
typedef FbxControlSetPlug				KFbxControlSetPlug;
typedef FbxControlSetLink				KFbxControlSetLink;
typedef FbxEffector						KFbxEffector;
typedef FbxPose							KFbxPose;
typedef FbxShape						KFbxShape;
typedef FbxBlendShape					KFbxBlendShape;
typedef FbxBlendShapeChannel			KFbxBlendShapeChannel;
typedef FbxImporter						KFbxImporter;
typedef FbxExporter						KFbxExporter;
typedef FbxIOSettings					KFbxIOSettings;
typedef FbxGlobalSettings				KFbxGlobalSettings;
typedef FbxGlobalCameraSettings			KFbxGlobalCameraSettings;
typedef FbxGlobalLightSettings			KFbxGlobalLightSettings;
//typedef FbxGlobalTimeSettings			KFbxGlobalTimeSettings;
typedef FbxTakeInfo						KFbxTakeInfo;
typedef FbxTakeLayerInfo				KLayerInfo;
typedef FbxThumbnail					KFbxThumbnail;
typedef FbxPlugin						KFbxPlugin;
typedef FbxAxisSystem					KFbxAxisSystem;
typedef FbxSystemUnit					KFbxSystemUnit;
typedef FbxStatistics					KFbxStatistics;
typedef FbxStatisticsFbx				KFbxStatisticsFbx;

typedef EFbxRotationOrder				ERotationOrder;
typedef EFbxQuatInterpMode				EQuaternionInterpolation;

typedef FbxIOFileHeaderInfo				KFbxFileHeaderInfo;

//#define eUNIDENTIFIED					eFbxUndefined
#define eBYTE1							eFbxChar
#define eUBYTE1							eFbxUChar
#define eSHORT1							eFbxShort
#define eUSHORT1						eFbxUShort
#define eUINTEGER1						eFbxUInt
#define eLONGLONG1						eFbxLongLong
#define eULONGLONG1						eFbxULongLong
#define eHALFFLOAT						eFbxHalfFloat
#define eBOOL1							eFbxBool
#define eINTEGER1						eFbxInt
#define eFLOAT1							eFbxFloat
#define eDOUBLE1						eFbxDouble
#define eDOUBLE2						eFbxDouble2
#define eDOUBLE3						eFbxDouble3
#define eDOUBLE4						eFbxDouble4
#define eDOUBLE44						eFbxDouble4x4
#define eENUM							eFbxEnum
#define eSTRING							eFbxString
#define eTIME							eFbxTime
#define eREFERENCE						eFbxReference
#define eBLOB							eFbxBlob
#define eDISTANCE						eFbxDistance
#define eDATETIME						eFbxDateTime
#define eMAX_TYPES						eFbxTypeCount

#define eEULER_XYZ						eEulerXYZ
#define eEULER_XZY						eEulerXZY
#define eEULER_YZX						eEulerYZX
#define eEULER_YXZ						eEulerYXZ
#define eEULER_ZXY						eEulerZXY
#define eEULER_ZYX						eEulerZYX
#define eSPHERIC_XYZ					eSphericXYZ

#define eINHERIT_RrSs					FbxTransform::eInheritRrSs
#define eINHERIT_RSrs					FbxTransform::eInheritRSrs
#define eINHERIT_Rrs					FbxTransform::eInheritRrs

#define eQUATINTERP_OFF					eQuatInterpOff
#define eQUATINTERP_CLASSIC				eQuatInterpClassic
#define eQUATINTERP_SLERP				eQuatInterpSlerp
#define eQUATINTERP_CUBIC				eQuatInterpCubic
#define eQUATINTERP_TANGENTDEPENDENT	eQuatInterpTangentDependent
#define eQUATINTERP_NBMODES				eQuatInterpCount

#define DTNone							FbxUndefinedDT
#define DTBool							FbxBoolDT
#define DTInteger						FbxIntDT
#define DTLongLong						FbxLongLongDT
#define DTULongLong						FbxULongLongDT
#define DTFloat							FbxFloatDT
#define DTDouble						FbxDoubleDT
#define DTEnum							FbxEnumDT
#define DTString						FbxStringDT
#define DTStringList					FbxStringListDT
#define DTCharPtr						FbxCharPtrDT
#define DTTime							FbxTimeDT
#define DTTimeCode						FbxTimeCodeDT
#define DTCompound						FbxCompoundDT
#define DTAction						FbxActionDT
#define DTEvent							FbxEventDT
#define DTReferenceObject				FbxReferenceObjectDT
#define DTReferenceProperty				FbxReferencePropertyDT
#define DTColor3						FbxColor3DT
#define DTColor4						FbxColor4DT
#define DTReal							FbxRealDT
#define DTDouble2						FbxDouble2DT
#define DTLocalTranslation				FbxLocalTranslationDT
#define DTLocalRotation					FbxLocalRotationDT
#define DTLocalScaling					FbxLocalScalingDT
#define DTTranslation					FbxTranslationDT
#define DTRotation						FbxRotationDT
#define DTScaling						FbxScalingDT
#define DTVector3D						FbxDouble3DT
#define DTDouble3						FbxDouble3DT
#define DTQuaternion					FbxQuaternionDT
#define DTLocalQuaternion				FbxLocalQuaternionDT
#define DTVisibility					FbxVisibilityDT
#define DTVector4D						FbxDouble4DT
#define DTDouble4						FbxDouble4DT
#define DTIntensity						FbxIntensityDT
#define DTConeAngle						FbxConeAngleDT
#define DTFog							FbxFogDT
#define DTShape							FbxShapeDT
#define DTFieldOfView					FbxFieldOfViewDT
#define DTFieldOfViewX					FbxFieldOfViewXDT
#define DTFieldOfViewY					FbxFieldOfViewYDT
#define DTOpticalCenterX				FbxOpticalCenterXDT
#define DTOpticalCenterY				FbxOpticalCenterYDT
#define DTRoll							FbxRollDT
#define DTCameraIndex					FbxCameraIndexDT
#define DTTimeWarp						FbxTimeWarpDT
#define DTTranslationUV					FbxTranslationUVDT
#define DTScalingUV						FbxScalingUVDT
#define DTOrientation					FbxOrientationDT
#define DTLookAt						FbxLookAtDT
#define DTOcclusion						FbxOcclusionDT
#define DTWeight						FbxWeightDT
#define DTIKReachTranslation			FbxIKReachTranslationDT
#define DTIKReachRotation				FbxIKReachRotationDT
#define DTTextureRotation				FbxTextureRotationDT
#define DTHSB							FbxHSBDT
#define DTTranslationMatrix				FbxTranslationMatrixDT
#define DTScalingMatrix					FbxScalingMatrixDT
#define DTRotationMatrix				FbxRotationMatrixDT
#define DTTransformMatrix				FbxTransformMatrixDT
#define DTBlob							FbxBlobDT

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_COMPATIBILITY_H_ */
