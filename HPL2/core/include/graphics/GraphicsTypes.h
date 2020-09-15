/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HPL_GRAPHICS_TYPES_H
#define HPL_GRAPHICS_TYPES_H

#pragma warning( disable : 4996 ) //disable unsafe warning!

#include <list>
#include <vector>
#include "graphics/Color.h"
#include "math/MathTypes.h"

namespace hpl {

	//-----------------------------------------

	#define kMaxTextureUnits (8)
	#define kMaxNumOfLights (30)
	#define kMaxClipPlanes (6)
	#define kMaxDrawColorBuffers (4)

	//-----------------------------------------

	class cVideoMode
	{
	public:
		// Old style add a resolution
		cVideoMode(const cVector2l& avScreenSize, int alBitsPerPixel, int alRefreshRate);
		// add a resolution for a specific monitor
		cVideoMode(int alDisplay, const cVector2l& avScreenSize, int alBitsPerPixel, int alRefreshRate);
		// Add a custom resolution
		cVideoMode(const cVector2l& avScreenSize);
		cVideoMode();

		bool isFullScreenDesktop() const { return mvScreenSize.x == 0 && mvScreenSize.y == 0; }

		bool operator== (const cVideoMode& o) const;

		cVector2l mvScreenSize;
		int mlBitsPerPixel;
		int mlRefreshRate;
		int mlDisplay;
		bool mbCustom;
	};

	//-----------------------------------------
	
	enum eRenderer
	{
		eRenderer_Main,
		eRenderer_WireFrame,
		eRenderer_Simple,
		eRenderer_LastEnum,
	};
	
	//-----------------------------------------

	enum eMatrix
	{
		eMatrix_ModelView,
		eMatrix_Projection,
		eMatrix_Texture,
		eMatrix_LastEnum
	};
	//-----------------------------------------


	enum ePrimitiveType
	{
		ePrimitiveType_Tri,
		ePrimitiveType_Quad,
		ePrimitiveType_LastEnum
	};

	//-----------------------------------------

	
	typedef tFlag tAnimTransformFlag;

	#define eAnimTransformFlag_Translate	(0x00000001)
	#define eAnimTransformFlag_Rotate		(0x00000002)
	#define eAnimTransformFlag_Scale		(0x00000004)
	
	#define klNumOfAnimTransformFlags (2)

	const tAnimTransformFlag kvAnimTransformFlags[] = {eAnimTransformFlag_Translate, eAnimTransformFlag_Rotate};

	//-----------------------------------------

	enum eBlendFunc
	{
		eBlendFunc_Zero,
		eBlendFunc_One,
		eBlendFunc_SrcColor,
		eBlendFunc_OneMinusSrcColor,
		eBlendFunc_DestColor,
		eBlendFunc_OneMinusDestColor,
		eBlendFunc_SrcAlpha,
		eBlendFunc_OneMinusSrcAlpha,
		eBlendFunc_DestAlpha,
		eBlendFunc_OneMinusDestAlpha,
		eBlendFunc_SrcAlphaSaturate,
		eBlendFunc_LastEnum
	};

	//-----------------------------------------


	enum eTextureParam
	{
		eTextureParam_ColorFunc,
		eTextureParam_AlphaFunc,
		eTextureParam_ColorSource0,
		eTextureParam_ColorSource1,
		eTextureParam_ColorSource2,
		eTextureParam_AlphaSource0,
		eTextureParam_AlphaSource1,
		eTextureParam_AlphaSource2,
		eTextureParam_ColorOp0,
		eTextureParam_ColorOp1,
		eTextureParam_ColorOp2,
		eTextureParam_AlphaOp0,
		eTextureParam_AlphaOp1,
		eTextureParam_AlphaOp2,
		eTextureParam_ColorScale,
		eTextureParam_AlphaScale,
		eTextureParam_LastEnum
	};

	enum eTextureOp
	{
		eTextureOp_Color,
		eTextureOp_OneMinusColor,
		eTextureOp_Alpha,
		eTextureOp_OneMinusAlpha,
		eTextureOp_LasEnum
	};

	enum eTextureSource
	{
		eTextureSource_Texture,
		eTextureSource_Constant,
		eTextureSource_Primary,
		eTextureSource_Previous,
		eTextureSource_LastEnum
	};

	enum eTextureFunc
	{
		eTextureFunc_Modulate,
		eTextureFunc_Replace,
		eTextureFunc_Add,
		eTextureFunc_Substract,
		eTextureFunc_AddSigned,
		eTextureFunc_Interpolate, 
		eTextureFunc_Dot3RGB,
		eTextureFunc_Dot3RGBA,
		eTextureFunc_LastEnum
	};

	//-----------------------------------------


	enum eStencilFunc
	{
		eStencilFunc_Never,
		eStencilFunc_Less,
		eStencilFunc_LessOrEqual,
		eStencilFunc_Greater,
		eStencilFunc_GreaterOrEqual, 
		eStencilFunc_Equal, 
		eStencilFunc_NotEqual, 
		eStencilFunc_Always, 
		eStencilFunc_LastEnum
	};

	enum eStencilFace
	{
		eStencilFace_Front,
		eStencilFace_Back,
		eStencilFace_LastEnum,
	};

	enum eStencilOp
	{
		eStencilOp_Keep, 
		eStencilOp_Zero, 
		eStencilOp_Replace, 
		eStencilOp_Increment, 
		eStencilOp_Decrement, 
		eStencilOp_Invert,
		eStencilOp_IncrementWrap,
		eStencilOp_DecrementWrap,
		eStencilOp_LastEnum
	};

	//-----------------------------------------

	enum eDepthTestFunc
	{
		eDepthTestFunc_Never,
		eDepthTestFunc_Less,
		eDepthTestFunc_LessOrEqual,
		eDepthTestFunc_Greater,
		eDepthTestFunc_GreaterOrEqual, 
		eDepthTestFunc_Equal, 
		eDepthTestFunc_NotEqual, 
		eDepthTestFunc_Always, 
		eDepthTestFunc_LastEnum
	};

	//-----------------------------------------

	enum eAlphaTestFunc
	{
		eAlphaTestFunc_Never,
		eAlphaTestFunc_Less,
		eAlphaTestFunc_LessOrEqual,
		eAlphaTestFunc_Greater,
		eAlphaTestFunc_GreaterOrEqual, 
		eAlphaTestFunc_Equal, 
		eAlphaTestFunc_NotEqual, 
		eAlphaTestFunc_Always, 
		eAlphaTestFunc_LastEnum
	};

	//-----------------------------------------

	enum eCullMode
	{
		eCullMode_Clockwise,
		eCullMode_CounterClockwise,
		eCullMode_LastEnum
	};

	//-----------------------------------------


	enum eGraphicCaps
	{
		eGraphicCaps_TextureTargetRectangle,
		eGraphicCaps_VertexBufferObject,
		eGraphicCaps_TwoSideStencil,

		eGraphicCaps_MaxTextureImageUnits,
		eGraphicCaps_MaxTextureCoordUnits,
		eGraphicCaps_MaxUserClipPlanes,

		eGraphicCaps_AnisotropicFiltering,
		eGraphicCaps_MaxAnisotropicFiltering,

		eGraphicCaps_Multisampling,

		eGraphicCaps_TextureCompression,
		eGraphicCaps_TextureCompression_DXTC,

		eGraphicCaps_AutoGenerateMipMaps,

		eGraphicCaps_RenderToTexture,
		eGraphicCaps_MaxDrawBuffers,

		eGraphicCaps_PackedDepthStencil,
		eGraphicCaps_TextureFloat,

		eGraphicCaps_PolygonOffset,

		eGraphicCaps_ShaderModel_2,
		eGraphicCaps_ShaderModel_3,
		eGraphicCaps_ShaderModel_4,

		eGraphicCaps_OGL_ATIFragmentShader,

		eGraphicCaps_MaxColorRenderTargets,

		eGraphicCaps_LastEnum
	};
	
	//-----------------------------------------


	typedef tFlag tClearFrameBufferFlag;

	#define eClearFrameBufferFlag_Color		(0x00000001)
	#define eClearFrameBufferFlag_Depth		(0x00000002)
	#define eClearFrameBufferFlag_Stencil	(0x00000004)

	typedef tFlag tVtxBatchFlag;

	#define eVtxBatchFlag_Normal		(0x00000001)
	#define eVtxBatchFlag_Position		(0x00000002)
	#define eVtxBatchFlag_Color0		(0x00000004)
	#define eVtxBatchFlag_Texture0		(0x00000008)
	#define eVtxBatchFlag_Texture1		(0x00000010)
	#define eVtxBatchFlag_Texture2		(0x00000020)

	enum eVertexBufferType
	{
		eVertexBufferType_Software,
		eVertexBufferType_Hardware,
		eVertexBufferType_LastEnum,
	};

	enum eVertexBufferDrawType
	{
		eVertexBufferDrawType_Tri,
		eVertexBufferDrawType_TriStrip,
		eVertexBufferDrawType_TriFan,
		eVertexBufferDrawType_Quad,
		eVertexBufferDrawType_QuadStrip,
		eVertexBufferDrawType_Line,
		eVertexBufferDrawType_LineLoop,
		eVertexBufferDrawType_LineStrip,
		eVertexBufferDrawType_LastEnum
	};

	enum eVertexBufferUsageType
	{
		eVertexBufferUsageType_Static,
		eVertexBufferUsageType_Dynamic,
		eVertexBufferUsageType_Stream,
		eVertexBufferUsageType_LastEnum
	};

	enum eVertexBufferElement
	{
		eVertexBufferElement_Normal,
		eVertexBufferElement_Position,
		eVertexBufferElement_Color0,
		eVertexBufferElement_Color1,
		eVertexBufferElement_Texture1Tangent,
		eVertexBufferElement_Texture0,
		eVertexBufferElement_Texture1,
		eVertexBufferElement_Texture2,
		eVertexBufferElement_Texture3,
		eVertexBufferElement_Texture4,
		eVertexBufferElement_User0,
		eVertexBufferElement_User1,
		eVertexBufferElement_User2,
		eVertexBufferElement_User3,
		eVertexBufferElement_LastEnum,
	};

	enum eVertexBufferElementFormat
	{
		eVertexBufferElementFormat_Float,
		eVertexBufferElementFormat_Int,
		eVertexBufferElementFormat_Byte,
		eVertexBufferElementFormat_LastEnum
	};

	typedef tFlag tVertexElementFlag;

	#define eVertexElementFlag_Normal		(0x00000001)
	#define eVertexElementFlag_Position		(0x00000002)
	#define eVertexElementFlag_Color0		(0x00000004)
	#define eVertexElementFlag_Color1		(0x00000008)
	#define eVertexElementFlag_Texture0		(0x00000010)
	#define eVertexElementFlag_Texture1		(0x00000020)
	#define eVertexElementFlag_Texture2		(0x00000040)
	#define eVertexElementFlag_Texture3		(0x00000080)
	#define eVertexElementFlag_Texture4		(0x00000100)
	#define eVertexElementFlag_User0		(0x00000200)
	#define eVertexElementFlag_User1		(0x00000400)
	#define eVertexElementFlag_User2		(0x00000800)
	#define eVertexElementFlag_User3		(0x00001000)

	typedef tFlag tVertexCompileFlag;

	#define eVertexCompileFlag_CreateTangents	(0x00000001)

	//-----------------------------------------

	enum eTextureUsage
	{
		eTextureUsage_Normal,
		eTextureUsage_RenderTarget,
		eTextureUsage_LastEnum
	};

	//-----------------------------------------

	enum eTextureType
	{
		eTextureType_1D,
		eTextureType_2D,
		eTextureType_Rect,
		eTextureType_CubeMap,
		eTextureType_3D,
		eTextureType_LastEnum
	};

	//-----------------------------------------


	enum eTextureWrap
	{
		eTextureWrap_Repeat,
		eTextureWrap_Clamp,
		eTextureWrap_ClampToEdge,
		eTextureWrap_ClampToBorder,
		eTextureWrap_LastEnum
	};

	//-----------------------------------------

	enum eTextureFilter
	{
		eTextureFilter_Nearest,
		eTextureFilter_Bilinear,
		eTextureFilter_Trilinear,
		eTextureFilter_LastEnum
	};

	//-----------------------------------------

	enum eTextureCompareMode
	{
		eTextureCompareMode_None,
		eTextureCompareMode_RToTexture,
		eTextureCompareMode_LastEnum
	};

	enum eTextureCompareFunc
	{
		eTextureCompareFunc_LessOrEqual,
		eTextureCompareFunc_GreaterOrEqual,
		eTextureCompareFunc_LastEnum
	};

	//-----------------------------------------

	enum eTextureAnimMode
	{
		eTextureAnimMode_None,
		eTextureAnimMode_Loop,
		eTextureAnimMode_Oscillate,
		eTextureAnimMode_LastEnum
	};

	//-----------------------------------------

	enum eGpuProgramFormat
	{
		eGpuProgramFormat_CG,
		eGpuProgramFormat_GLSL,
		eGpuProgramFormat_LastEnum
	};

	//-----------------------------------------

	enum eGpuShaderType
	{
		eGpuShaderType_Vertex,
		eGpuShaderType_Fragment,
		eGpuShaderType_LastEnum
	};
	
	enum eGpuShaderMatrix
	{
		eGpuShaderMatrix_View,
		eGpuShaderMatrix_Projection,
		eGpuShaderMatrix_Texture,
		eGpuShaderMatrix_ViewProjection,
		eGpuShaderMatrix_LastEnum
	};

	enum eGpuShaderMatrixOp
	{
		eGpuShaderMatrixOp_Identity,
		eGpuShaderMatrixOp_Inverse,
		eGpuShaderMatrixOp_Transpose,
		eGpuShaderMatrixOp_InverseTranspose,
		eGpuShaderMatrixOp_LastEnum
	};

	//-----------------------------------------

	
	enum ePixelFormat
	{
			ePixelFormat_Unknown,
			ePixelFormat_Alpha,
			ePixelFormat_Luminance,
			ePixelFormat_LuminanceAlpha,
			ePixelFormat_RGB,
			ePixelFormat_RGBA,
			ePixelFormat_BGR,
			ePixelFormat_BGRA,
			ePixelFormat_DXT1,
			ePixelFormat_DXT2,
			ePixelFormat_DXT3,
			ePixelFormat_DXT4,
			ePixelFormat_DXT5,
			ePixelFormat_Depth16,
			ePixelFormat_Depth24,
			ePixelFormat_Depth32,
			ePixelFormat_Alpha16,
			ePixelFormat_Luminance16,
			ePixelFormat_LuminanceAlpha16,
			ePixelFormat_RGB16,
			ePixelFormat_RGBA16,
			ePixelFormat_Alpha32,
			ePixelFormat_Luminance32,
			ePixelFormat_LuminanceAlpha32,
			ePixelFormat_RGB32,
			ePixelFormat_RGBA32,
			ePixelFormat_LastEnum,
	};

	//-----------------------------------------
	
	enum eFontAlign
	{
		eFontAlign_Left,
		eFontAlign_Right,
		eFontAlign_Center,
		eFontAlign_LastEnum
	};

	//---------------------------------------

	enum eFrameBufferAttachment
	{	
		eFrameBufferAttachment_RenderBuffer,	
		eFrameBufferAttachment_Texture,
		eFrameBufferAttachment_LastEnum
	};

	//---------------------------------------
	
	enum eMaterialTexture
	{
		eMaterialTexture_Diffuse,
		eMaterialTexture_NMap,
		eMaterialTexture_Specular,
		eMaterialTexture_Alpha,
		eMaterialTexture_Height,
		eMaterialTexture_Illumination,
		eMaterialTexture_CubeMap,
		eMaterialTexture_DissolveAlpha,
		eMaterialTexture_CubeMapAlpha,
		eMaterialTexture_Special,	//This means that the texture is not data in the material, but retrieved else where. Such as a reflection texture.
		eMaterialTexture_LastEnum
	};

	enum eMaterialChannelMode
	{
		eMaterialChannelMode_RGBA,
		eMaterialChannelMode_RGB,
		eMaterialChannelMode_A,
		eMaterialChannelMode_None,
		eMaterialChannelMode_LastEnum,
	};

	enum eMaterialBlendMode
	{
		eMaterialBlendMode_None, 
		eMaterialBlendMode_Add,
		eMaterialBlendMode_Mul,
		eMaterialBlendMode_MulX2,
		eMaterialBlendMode_Alpha,
		eMaterialBlendMode_PremulAlpha,
		eMaterialBlendMode_LastEnum
	};

	enum eMaterialAlphaMode
	{
		eMaterialAlphaMode_Solid,
		eMaterialAlphaMode_Trans,
		eMaterialAlphaMode_LastEnum,
	};


	enum eMaterialRenderMode
	{
		eMaterialRenderMode_Z,				//Early Z test. Shall always return diffuse texture as unit 0, allthough it might not always be used.
		eMaterialRenderMode_Z_Dissolve,		//Like early Z, but with a dissolve effect
		eMaterialRenderMode_Diffuse,		//G-buffer pass in deferred, transparent pass for transparent objects.
		eMaterialRenderMode_DiffuseFog,		//Transparent pass with fog.
		eMaterialRenderMode_Light,			//Not used by deferred shader.
		eMaterialRenderMode_Illumination,	//Illumination textures.
		eMaterialRenderMode_IlluminationFog,//Illumination with fog (used by transperant textures)
		eMaterialRenderMode_LastEnum,
	};

	//---------------------------------------

	enum eMaterialUvAnimation
	{
		eMaterialUvAnimation_Translate,
		eMaterialUvAnimation_Sin,
		eMaterialUvAnimation_Rotate,
		eMaterialUvAnimation_LastEnum
	};

	enum eMaterialAnimationAxis
	{
		eMaterialAnimationAxis_X,
		eMaterialAnimationAxis_Y,
		eMaterialAnimationAxis_Z,
		eMaterialAnimationAxis_LastEnum
	};

	//---------------------------------------
	
	enum eShadowMapResolution
	{	
		eShadowMapResolution_Low,
		eShadowMapResolution_Medium,
		eShadowMapResolution_High,
		eShadowMapResolution_LastEnum,
	};

	//---------------------------------------

	enum eShadowMapQuality
	{	
		eShadowMapQuality_Low,
		eShadowMapQuality_Medium,
		eShadowMapQuality_High,
		eShadowMapQuality_LastEnum,
	};
	
	//---------------------------------------

	enum eParallaxQuality
	{
		eParallaxQuality_Low,
		eParallaxQuality_Medium,
		eParallaxQuality_High,
		eParallaxQuality_LastEnum,
	};

	//---------------------------------------

	struct cProgramComboFeature
	{
		cProgramComboFeature(){}
		cProgramComboFeature(const tString& asVar, tFlag alShaders) : msVariable(asVar), mlShaders(alShaders), mlNeededFlags(0){}
		cProgramComboFeature(const tString& asVar, tFlag alShaders, tFlag alNeededFlags) : msVariable(asVar), mlShaders(alShaders),
																						   mlNeededFlags(alNeededFlags){}

		tString msVariable;
		tFlag mlShaders;
		tFlag mlNeededFlags;
	};


	//---------------------------------------

	enum eRenderableType
	{
		eRenderableType_ParticleEmitter,
		eRenderableType_SubMesh,
		eRenderableType_Light,
		eRenderableType_Billboard,
		eRenderableType_Beam,
		eRenderableType_GuiSet,
		eRenderableType_Rope,
		eRenderableType_FogArea,
		eRenderableType_Dummy,
		eRenderableType_LastEnum
	};

	//---------------------------------------

	typedef tFlag tRenderableFlag;

	#define eRenderableFlag_ShadowCaster			(0x00000001)
	#define eRenderableFlag_VisibleInReflection		(0x00000002)
	#define eRenderableFlag_VisibleInNonReflection	(0x00000004)
	#define eRenderableFlag_ContainerDebug			(0x00000008)
	
	//---------------------------------------

	typedef tFlag tRenderListCompileFlag;

	#define eRenderListCompileFlag_Z			(0x00000001)
	#define eRenderListCompileFlag_Diffuse		(0x00000002)
	#define eRenderListCompileFlag_Translucent	(0x00000004)
	#define eRenderListCompileFlag_Decal		(0x00000008)
	#define eRenderListCompileFlag_Illumination	(0x00000010)

	enum eRenderListType
	{
		eRenderListType_Z,	
		eRenderListType_Diffuse,
		eRenderListType_Translucent,
		eRenderListType_Decal,
		eRenderListType_Illumination,
		eRenderListType_LastEnum
	};

	//---------------------------------------

	class cMaterial;
	
	typedef std::vector<cMaterial*> tMaterialVec;
	typedef tMaterialVec::iterator tMaterialVecIt;

	//---------------------------------------

	class cKeyFrame
	{
	public:
		cVector3f trans;
		cQuaternion rotation;
		float time;
	};

	typedef std::vector<cKeyFrame*> tKeyFramePtrVec;
	typedef tKeyFramePtrVec::iterator tKeyFramePtrVecIt;

	typedef std::vector<cKeyFrame> tKeyFrameVec;
	typedef tKeyFrameVec::iterator tKeyFrameVecIt;

	//---------------------------------------

	enum eAnimationEventType
	{
		eAnimationEventType_PlaySound,
		eAnimationEventType_Step,
		eAnimationEventType_LastEnum
	};

	//---------------------------------------
	
	class iRenderable;

	typedef std::vector<iRenderable*> tRenderableVec;
	typedef tRenderableVec::iterator tRenderableVecIt;

	typedef std::list<iRenderable*> tRenderableList;
	typedef tRenderableList::iterator tRenderableListIt;

	typedef std::set<iRenderable*> tRenderableSet;
	typedef tRenderableSet::iterator tRenderableSetIt;

	//---------------------------------------

	class cVertexBonePair
	{
	public:
		cVertexBonePair(unsigned int aVtx, unsigned int aBone, float aW)
		{
			vtxIdx = aVtx;
			boneIdx = aBone;
			weight = aW;
		}
		cVertexBonePair(){}


		unsigned int vtxIdx;
		unsigned int boneIdx;
		float weight;
	};


	typedef std::vector<cVertexBonePair> tVertexBonePairVec;
	typedef tVertexBonePairVec::iterator tVertexBonePairVecIt;
	
	//---------------------------------------

	class cVertex
	{
	public:
		cVertex():pos(0),tex(0),col(0){}
		cVertex(const cVector3f &avPos,const cVector3f &avTex,const cColor &aCol )
		{
			pos = avPos;
			tex = avTex;
			col = aCol;
		}
		
		cVertex(const cVector3f &avPos,const cColor &aCol )
		{
			pos = avPos;
			col = aCol;
		}

		cVector3f pos;
		cVector3f tex;
		cVector3f tan;
		cVector3f norm;
        cColor col;
	};
	
	typedef std::list<cVertex> tVertexList;
	typedef tVertexList::iterator tVertexListIt;
	
	typedef std::vector<cVertex> tVertexVec;
	typedef tVertexVec::iterator tVertexVecIt;

	//---------------------------------------

	class iOcclusionQuery;
	class iVertexBuffer;
	
	class cOcclusionQueryObject
	{
	public:
		cOcclusionQueryObject() : mpQuery(NULL), mpVtxBuffer(NULL),mpMatrix(NULL), mbDepthTest(false), mlSampleResults(0) {}

		int mlCustomID;
		iOcclusionQuery *mpQuery;
		iVertexBuffer *mpVtxBuffer;
		cMatrixf *mpMatrix;
		bool mbDepthTest;
		int mlSampleResults;
	};

	//---------------------------------------
	class iTexture;
	class iDepthStencilBuffer;

	class iFrameBufferAttachment
	{
	public:
		virtual eFrameBufferAttachment GetFrameBufferAttachmentType()=0;

		iTexture* ToTexture();
		iDepthStencilBuffer* ToDepthStencilBuffer();
	};

	//---------------------------------------

	class iRenderableCallback
	{
	public:
		virtual void OnVisibleChange(iRenderable *apObject)=0;
		virtual void OnRenderFlagsChange(iRenderable *apObject)=0;
	};

	//---------------------------------------

	class iFrameBuffer;

	class cRenderTarget
	{
	public:
		cRenderTarget() : mpFrameBuffer(NULL), mvPos(0), mvSize(-1){}

		iFrameBuffer *mpFrameBuffer;
		cVector2l mvPos;
		cVector2l mvSize;
	};

	//---------------------------------------
	
	class iGpuShader;
	class iGpuProgram;

	typedef std::list<iGpuShader*> tGpuShaderList;
	typedef tGpuShaderList::iterator tGpuShaderListIt;

	typedef std::list<iGpuProgram*> tGpuProgramList;
	typedef tGpuProgramList::iterator tGpuProgramListIt;

	//---------------------------------------


	class cBoneState;

	typedef std::vector<cBoneState*> tNodeStateVec;
	typedef tNodeStateVec::iterator tNodeStateVecIt;

	typedef std::list<cBoneState*> tNodeStateList;
	typedef tNodeStateList::iterator tNodeStateListIt;

	typedef std::map<int, cBoneState*> tNodeStateMap;
	typedef tNodeStateMap::iterator tNodeStateMapIt;

	typedef std::map<tString, int> tNodeStateIndexMap;
	typedef tNodeStateIndexMap::iterator tNodeStateIndexMapIt;

	//---------------------------------------

	extern tVertexElementFlag GetVertexElementFlagFromEnum(eVertexBufferElement aElement);
	extern int GetVertexFormatByteSize(eVertexBufferElementFormat aFormat);
	extern int GetVertexElementTextureUnit(eVertexBufferElement aElement);
	
	extern int GetChannelsInPixelFormat(ePixelFormat aFormat);
	extern int GetBytesPerPixel(ePixelFormat aFormat);
	extern bool PixelFormatIsCompressed(ePixelFormat aFormat);
	extern bool PixelFormatIsDepth(ePixelFormat aFormat);
	extern bool PixelFormatIsFloatingPoint(ePixelFormat aFormat);
	inline char FloatColorToUChar(float afColor)
	{
		return (unsigned char)(afColor * 255.0f + 0.5f); // Round up
	}
	inline float UCharColorToFloat(unsigned char alColor)
	{
		return (float)(alColor) / 255.0f;
	}


	//---------------------------------------

}

#endif // HPL_GRAPHICS_TYPES_H
