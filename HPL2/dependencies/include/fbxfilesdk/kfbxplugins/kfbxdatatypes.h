/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxdatatypes.h
 */
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXDATATYPES_H
#define FBXFILESDK_KFBXPLUGINS_KFBXDATATYPES_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/components/kbaselib/klib/kdebug.h>
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>

#include <fbxfilesdk/kfbxplugins/kfbxtypes.h>
#include <fbxfilesdk/kfbxplugins/kfbxcolor.h>
#include <fbxfilesdk/kfbxmath/kfbxvector4.h>

#include <fbxfilesdk/fbxcore/kfbxpropertyhandle.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

    class   KFbxSdkManager;
    class   KFbxTypeInfo_internal;

	/**FBX SDK data type class
	  *\nosubgrouping
	  */
    class KFBX_DLL KFbxDataType {

        public:
            static KFbxDataType Create(const char *pName,EFbxType pType);
            static KFbxDataType Create(const char *pName,KFbxDataType const &pDataType);
    /**
	  *\name Constructor and Destructor.
	  */
	//@{
			//!Constructor.
            KFbxDataType();

			/**Copy constructor.
			  * \param pDataType Another KFbxDataType object copied to this one.
			  */
            KFbxDataType( KFbxDataType const &pDataType );

            //!Destroy this datatype.
            void Destroy();

            /**Constructor.
			  *\param pTypeInfoHandle                Type information handle
			  */
            KFbxDataType( KFbxTypeInfoHandle const &pTypeInfoHandle );

			//!Destructor.
            ~KFbxDataType();
	//@}


        public:
			/**Assignment operator
			  *\param pDataType               Datatype whose value is assigned to this datatype.
			  *\return                        this datatype
			  */
            inline KFbxDataType& operator=(const KFbxDataType &pDataType) { mTypeInfoHandle=pDataType.mTypeInfoHandle; return *this;  }

			/**
			  * \name boolean operation
			  */
			//@{

			/**Equality operator
			  *\param pDataType                Datatype to compare to.
			  *\return                         \c true if equal,\c false otherwise.
			  */
            inline bool operator==(const KFbxDataType& pDataType) const   { return mTypeInfoHandle==pDataType.mTypeInfoHandle;        }

            /**Non-equality operator
			  *\param pDataType                Datatype to compare to.
			  *\return                         \c true if unequal,\c false otherwise.
			  */
            inline bool operator!=(const KFbxDataType& pDataType) const   { return mTypeInfoHandle!=pDataType.mTypeInfoHandle;        }
            //@}
        public:

			/**Test whether this datatype is a valid datatype.
			  *\return         \c true if valid, \c false otherwise.
			  */
            inline bool     Valid() const { return mTypeInfoHandle.Valid(); }

			/** Test if this datatype is the specified datatype. 
			  * \param pDataType               Datatype to compare to.
			  * \return                        \c true if this datatype is the specified datatype, \c false otherwise. 
			  */
            inline bool     Is(const KFbxDataType& pDataType) const { return mTypeInfoHandle.Is(pDataType.mTypeInfoHandle); }

			/** Retrieve this data type.
			  * \return     this data type.
			  */
            EFbxType        GetType() const;

			/** Retrieve data type name.
              * \return     data type name.
			  */
            const char*     GetName() const;

        private:
            KFbxTypeInfoHandle      mTypeInfoHandle;
        public:
			/** Retrieve the information handle of this data type.
			  * \return       information handle of this data type.
			  */
            inline KFbxTypeInfoHandle const &GetTypeInfoHandle() const  { return mTypeInfoHandle; }

        friend class KFbxSdkManager;
    };

    // Default Basic Types
	extern KFBX_DLL	 KFbxDataType   DTByte;
	extern KFBX_DLL	 KFbxDataType   DTUByte;
	extern KFBX_DLL	 KFbxDataType   DTShort;
	extern KFBX_DLL	 KFbxDataType   DTUShort;
	extern KFBX_DLL	 KFbxDataType   DTUInteger;
	extern KFBX_DLL	 KFbxDataType   DTLongLong;
	extern KFBX_DLL	 KFbxDataType   DTULongLong;
	extern KFBX_DLL  KFbxDataType   DTHalfFloat;

    extern KFBX_DLL  KFbxDataType   DTNone;
    extern KFBX_DLL  KFbxDataType   DTBool;
    extern KFBX_DLL  KFbxDataType   DTInteger;
    extern KFBX_DLL  KFbxDataType   DTFloat;
    extern KFBX_DLL  KFbxDataType   DTDouble;
    extern KFBX_DLL  KFbxDataType   DTDouble2;
    extern KFBX_DLL  KFbxDataType   DTDouble3;
    extern KFBX_DLL  KFbxDataType   DTDouble4;
    extern KFBX_DLL  KFbxDataType   DTDouble44;
    extern KFBX_DLL  KFbxDataType   DTEnum;
    extern KFBX_DLL  KFbxDataType   DTStringList;   // ?
    extern KFBX_DLL  KFbxDataType   DTString;
    extern KFBX_DLL  KFbxDataType   DTCharPtr;      // ?
    extern KFBX_DLL  KFbxDataType   DTTime;
    extern KFBX_DLL  KFbxDataType   DTTimeCode;
    extern KFBX_DLL  KFbxDataType   DTReference;
    extern KFBX_DLL  KFbxDataType   DTCompound;
    extern KFBX_DLL  KFbxDataType   DTBlob;
	extern KFBX_DLL	 KFbxDataType	DTDistance;
    extern KFBX_DLL	 KFbxDataType	DTDateTime;

    // MB Specific datatypes
    extern KFBX_DLL  KFbxDataType   DTAction;
    extern KFBX_DLL  KFbxDataType   DTEvent;

    // Specialised reference Properties
    extern KFBX_DLL  KFbxDataType   DTReferenceObject;
    extern KFBX_DLL  KFbxDataType   DTReferenceProperty;

    // Extended sub types
    extern KFBX_DLL  KFbxDataType   DTColor3;
    extern KFBX_DLL  KFbxDataType   DTColor4;

    // Support for older datatypes
    extern KFBX_DLL  KFbxDataType   DTReal;
    extern KFBX_DLL  KFbxDataType   DTVector3D;
    extern KFBX_DLL  KFbxDataType   DTVector4D;

    // Tranforms Types
    extern KFBX_DLL  KFbxDataType   DTTranslation;
    extern KFBX_DLL  KFbxDataType   DTRotation;
    extern KFBX_DLL  KFbxDataType   DTScaling;
    extern KFBX_DLL  KFbxDataType   DTQuaternion;

    extern KFBX_DLL  KFbxDataType   DTLocalTranslation;
    extern KFBX_DLL  KFbxDataType   DTLocalRotation;
    extern KFBX_DLL  KFbxDataType   DTLocalScaling;
    extern KFBX_DLL  KFbxDataType   DTLocalQuaternion;

    extern KFBX_DLL  KFbxDataType   DTTransformMatrix;
    extern KFBX_DLL  KFbxDataType   DTTranslationMatrix;
    extern KFBX_DLL  KFbxDataType   DTRotationMatrix;
    extern KFBX_DLL  KFbxDataType   DTScalingMatrix;

    // Material-related types (also used as DataType for Texture layer elements)
    extern KFBX_DLL  KFbxDataType DTMaterialEmissive;
    extern KFBX_DLL  KFbxDataType DTMaterialEmissiveFactor;
    extern KFBX_DLL  KFbxDataType DTMaterialAmbient;
    extern KFBX_DLL  KFbxDataType DTMaterialAmbientFactor;
    extern KFBX_DLL  KFbxDataType DTMaterialDiffuse;
    extern KFBX_DLL  KFbxDataType DTMaterialDiffuseFactor;
    extern KFBX_DLL  KFbxDataType DTMaterialBump;
    extern KFBX_DLL  KFbxDataType DTMaterialNormalMap;
    extern KFBX_DLL  KFbxDataType DTMaterialTransparentColor;
    extern KFBX_DLL  KFbxDataType DTMaterialTransparencyFactor;
    extern KFBX_DLL  KFbxDataType DTMaterialSpecular;
    extern KFBX_DLL  KFbxDataType DTMaterialSpecularFactor;
    extern KFBX_DLL  KFbxDataType DTMaterialShininess;
    extern KFBX_DLL  KFbxDataType DTMaterialReflection;
    extern KFBX_DLL  KFbxDataType DTMaterialReflectionFactor;
    extern KFBX_DLL  KFbxDataType DTMaterialDisplacement;
    extern KFBX_DLL  KFbxDataType DTMaterialVectorDisplacement;
    extern KFBX_DLL  KFbxDataType DTMaterialCommonFactor;
    extern KFBX_DLL  KFbxDataType DTMaterialCommonTexture;

    // LayerElement
    extern KFBX_DLL  KFbxDataType DTLayerElementUndefined;
    extern KFBX_DLL  KFbxDataType DTLayerElementNormal;
    extern KFBX_DLL  KFbxDataType DTLayerElementBinormal;
    extern KFBX_DLL  KFbxDataType DTLayerElementTangent;
    extern KFBX_DLL  KFbxDataType DTLayerElementMaterial;
    extern KFBX_DLL  KFbxDataType DTLayerElementTexture;
    extern KFBX_DLL  KFbxDataType DTLayerElementPolygonGroup;
    extern KFBX_DLL  KFbxDataType DTLayerElementUV;
    extern KFBX_DLL  KFbxDataType DTLayerElementVertexColor;
    extern KFBX_DLL  KFbxDataType DTLayerElementSmoothing;
    extern KFBX_DLL  KFbxDataType DTLayerElementCrease;
    extern KFBX_DLL  KFbxDataType DTLayerElementHole;
    extern KFBX_DLL  KFbxDataType DTLayerElementUserData;
    extern KFBX_DLL  KFbxDataType DTLayerElementVisibility;

    // File references / External file references
    extern KFBX_DLL  KFbxDataType   DTUrl;
    extern KFBX_DLL  KFbxDataType   DTXRefUrl;

    // IO Specialised Types
	extern KFBX_DLL  KFbxDataType   DTPresets;
	extern KFBX_DLL  KFbxDataType   DTStatistics;
	extern KFBX_DLL  KFbxDataType   DTUnits;
	extern KFBX_DLL  KFbxDataType   DTWarning;
	extern KFBX_DLL  KFbxDataType   DTWeb;
	extern KFBX_DLL  KFbxDataType   DTTextLine;
	extern KFBX_DLL  KFbxDataType   DTAlias;


    // Motion Builder Specialised Types
    extern KFBX_DLL  KFbxDataType   DTIntensity;
    extern KFBX_DLL  KFbxDataType   DTConeAngle;
    extern KFBX_DLL  KFbxDataType   DTFog;
    extern KFBX_DLL  KFbxDataType   DTShape;
    extern KFBX_DLL  KFbxDataType   DTFieldOfView;
    extern KFBX_DLL  KFbxDataType   DTFieldOfViewX;
    extern KFBX_DLL  KFbxDataType   DTFieldOfViewY;
    extern KFBX_DLL  KFbxDataType   DTOpticalCenterX;
    extern KFBX_DLL  KFbxDataType   DTOpticalCenterY;

    extern KFBX_DLL  KFbxDataType   DTRoll;
    extern KFBX_DLL  KFbxDataType   DTCameraIndex;
    extern KFBX_DLL  KFbxDataType   DTTimeWarp;
    extern KFBX_DLL  KFbxDataType   DTVisibility;
    extern KFBX_DLL  KFbxDataType   DTVisibilityInheritance;

    extern KFBX_DLL  KFbxDataType   DTTranslationUV;
    extern KFBX_DLL  KFbxDataType   DTScalingUV;
    extern KFBX_DLL  KFbxDataType   DTTextureRotation;//MB DT
    extern KFBX_DLL  KFbxDataType   DTHSB;

    extern KFBX_DLL  KFbxDataType   DTOrientation;
    extern KFBX_DLL  KFbxDataType   DTLookAt;

    extern KFBX_DLL  KFbxDataType   DTOcclusion;
    extern KFBX_DLL  KFbxDataType   DTWeight;

    extern KFBX_DLL  KFbxDataType   DTIKReachTranslation;
    extern KFBX_DLL  KFbxDataType   DTIKReachRotation;

    KFBX_DLL KFbxDataType const & GetFbxDataType(EFbxType pType);

    // Internal use
    KFBX_DLL bool               KFbxTypesInit   (KFbxSdkManager *pManager);
    KFBX_DLL void               KFbxTypesRelease(KFbxSdkManager *pManager);

    KFBX_DLL char*              FbxDataTypeToFormatPropertyType( KFbxDataType const &pDataType );
    KFBX_DLL KFbxDataType const &FormatPropertyTypeToFbxDataType( const char *pDataTypeName );



#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXDATATYPES_H

