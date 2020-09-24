/*!  \file conventions.h
 */

#ifndef _ADSK_CONVENTIONS_H_
#define _ADSK_CONVENTIONS_H_
/**************************************************************************************

Copyright (C) 2001 - 2008 Autodesk, Inc. and/or its licensors.
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

//
// Predefined shader languages
//
#define sHLSL  "HLSL"
#define sGLSL  "GLSL"
#define sCGFX  "CGFX"
#define sMentalRaySL "MentalRaySL"

//
// Predefined rendering apis
//
#define sDirectX  "DirectX"
#define sOpenGL  "OpenGL"
#define sMentalRay "MentalRay"
#define sPreview  "PreviewColorAPI"


/*
enum ImplementationTarget
{
    eImplPreview,
    eImplMentalRay,
    eImplCGFX,
    eImplHLSL,
    eImplNone,
    eImplCount
};
*/

#define ImplementationPreview "ImplementationPreview"
#define ImplementationMentalRay "ImplementationMentalRay"
#define ImplementationCGFX "ImplementationCGFX"
#define ImplementationHLSL "ImplementationHLSL"
#define ImplementaitonOGS  "ImplementationOGS"
#define ImplementationNone "ImplementationNone"

#define ImplementationAdditionalFiles "AdditionalFiles"

// PROTEIN 1.0 conventions
#define FBXSDK_TYPE_ENVIRONMENT         "KFbxEnvironment"
#define FBXSDK_TYPE_LIGHT               "KFbxLight"
#define FBXSDK_TYPE_PROCEDURALGEOMETRY  "KFbxProceduralGeometry"
#define FBXSDK_TYPE_SURFACEMATERIAL     "KFbxSurfaceMaterial"
#define FBXSDK_TYPE_TEXTURE             "KFbxTexture"
#define FBXSDK_TYPE_SWATCHSCENE         "KFbxSwatchScene"

// PROTEIN 2.0 conventions
#define ADSK_TYPE_ENVIRONMENT           "ADSKEnvironmentDefinition"
#define ADSK_TYPE_LIGHT                 "ADSKLightDefinition"
#define ADSK_TYPE_PROCEDURALGEOMETRY    "ADSKProceduralGeometryDefinition"
#define ADSK_TYPE_SURFACEMATERIAL       "ADSKSurfaceMaterialDefinition"
#define ADSK_TYPE_TEXTURE               "ADSKTextureDefinition"
#define ADSK_TYPE_SWATCHSCENE           "ADSKSwatchSceneDefinition"

// ASSET Definition conventions
#define ADSK_UI_DEFINITION_URL          "UIDefinition"


#endif

