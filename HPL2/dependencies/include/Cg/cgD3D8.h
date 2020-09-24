/*
 *
 * Copyright (c) 2002-2006, NVIDIA Corporation.
 * 
 *  
 * 
 * NVIDIA Corporation("NVIDIA") supplies this software to you in consideration 
 * of your agreement to the following terms, and your use, installation, 
 * modification or redistribution of this NVIDIA software constitutes 
 * acceptance of these terms.  If you do not agree with these terms, please do 
 * not use, install, modify or redistribute this NVIDIA software.
 * 
 *  
 * 
 * In consideration of your agreement to abide by the following terms, and 
 * subject to these terms, NVIDIA grants you a personal, non-exclusive license,
 * under NVIDIA's copyrights in this original NVIDIA software (the "NVIDIA 
 * Software"), to use, reproduce, modify and redistribute the NVIDIA 
 * Software, with or without modifications, in source and/or binary forms; 
 * provided that if you redistribute the NVIDIA Software, you must retain the 
 * copyright notice of NVIDIA, this notice and the following text and 
 * disclaimers in all such redistributions of the NVIDIA Software. Neither the 
 * name, trademarks, service marks nor logos of NVIDIA Corporation may be used 
 * to endorse or promote products derived from the NVIDIA Software without 
 * specific prior written permission from NVIDIA.  Except as expressly stated 
 * in this notice, no other rights or licenses express or implied, are granted 
 * by NVIDIA herein, including but not limited to any patent rights that may be 
 * infringed by your derivative works or by other works in which the NVIDIA 
 * Software may be incorporated. No hardware is licensed hereunder. 
 * 
 *  
 * 
 * THE NVIDIA SOFTWARE IS BEING PROVIDED ON AN "AS IS" BASIS, WITHOUT 
 * WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING 
 * WITHOUT LIMITATION, WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR ITS USE AND OPERATION 
 * EITHER ALONE OR IN COMBINATION WITH OTHER PRODUCTS.
 * 
 *  
 * 
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL, 
 * EXEMPLARY, CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, LOST 
 * PROFITS; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) OR ARISING IN ANY WAY OUT OF THE USE, 
 * REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION OF THE NVIDIA SOFTWARE, 
 * HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING 
 * NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF NVIDIA HAS BEEN ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */ 
#ifndef CGD3D8_INCLUDED
#define CGD3D8_INCLUDED

#ifdef _WIN32

#pragma once

#include "cg.h"
#include <d3d8.h>
#include <d3dx8.h>

// Set up for either Win32 import/export/lib.
#include <windows.h>
#ifdef CGD3D8DLL_EXPORTS
#define CGD3D8DLL_API __declspec(dllexport)
#elif defined (CG_LIB)
#define CGD3D8DLL_API
#else
#define CGD3D8DLL_API __declspec(dllimport)
#endif

#ifndef CGD3D8ENTRY
# ifdef _WIN32
#  define CGD3D8ENTRY __cdecl
# else
#  define CGD3D8ENTRY
# endif
#endif

/*---------------------------------------------------------------------------
// CGerrors that will be fed to cgSetError
// Use cgD3D8TranslateCGerror() to translate these errors into strings.
---------------------------------------------------------------------------*/
enum cgD3D8Errors
{
    cgD3D8Failed = 1000,
    cgD3D8DebugTrace = 1001,
};

/*---------------------------------------------------------------------------
// HRESULTs specific to cgD3D8. When the CGerror is set to cgD3D8Failed
// cgD3D8GetLastError will return an HRESULT that could be one these.
// Use cgD3D8TranslateHRESULT() to translate these errors into strings.
---------------------------------------------------------------------------*/
static const HRESULT CGD3D8ERR_NOTLOADED       = MAKE_HRESULT(1, 0x877,  1);
static const HRESULT CGD3D8ERR_NODEVICE        = MAKE_HRESULT(1, 0x877,  2);
static const HRESULT CGD3D8ERR_NOTSAMPLER      = MAKE_HRESULT(1, 0x877,  3);
static const HRESULT CGD3D8ERR_INVALIDPROFILE  = MAKE_HRESULT(1, 0x877,  4);
static const HRESULT CGD3D8ERR_NULLVALUE       = MAKE_HRESULT(1, 0x877,  5);
static const HRESULT CGD3D8ERR_OUTOFRANGE      = MAKE_HRESULT(1, 0x877,  6);
static const HRESULT CGD3D8ERR_NOTUNIFORM      = MAKE_HRESULT(1, 0x877,  7);
static const HRESULT CGD3D8ERR_NOTMATRIX       = MAKE_HRESULT(1, 0x877,  8);
static const HRESULT CGD3D8ERR_INVALIDPARAM    = MAKE_HRESULT(1, 0x877,  9);

static const HRESULT CGD3D8ERR_INVALIDSAMPLERSTATE       = MAKE_HRESULT(1, 0x877, 100);
static const HRESULT CGD3D8ERR_INVALIDVEREXDECL          = MAKE_HRESULT(1, 0x877, 101);

/*---------------------------------------------------------------------------
// Other error return values
---------------------------------------------------------------------------*/
static const DWORD CGD3D8_INVALID_REG = 0xFFFFFFFF;

#ifdef __cplusplus
extern "C"
{
#endif

/*---------------------------------------------------------------------------
// Minimal Interface
---------------------------------------------------------------------------*/

CGD3D8DLL_API DWORD CGD3D8ENTRY
cgD3D8TypeToSize(
  CGtype type
);

CGD3D8DLL_API DWORD CGD3D8ENTRY
cgD3D8ResourceToInputRegister(
  CGresource resource
);

CGD3D8DLL_API CGbool CGD3D8ENTRY
cgD3D8GetVertexDeclaration(
  CGprogram prog,
  DWORD     decl[MAX_FVF_DECL_SIZE]
);

CGD3D8DLL_API CGbool CGD3D8ENTRY
cgD3D8ValidateVertexDeclaration(
  CGprogram    prog,
  const DWORD* decl
);

/*---------------------------------------------------------------------------
// Expanded Interface
---------------------------------------------------------------------------*/

/* ----- D3D Device Control ----------- */
CGD3D8DLL_API IDirect3DDevice8 * CGD3D8ENTRY
cgD3D8GetDevice();

CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8SetDevice(
  IDirect3DDevice8* pDevice
);

/* ----- Shader Management ----------- */
CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8LoadProgram(
  CGprogram    prog,
  CGbool       paramShadowing,
  DWORD        assemFlags,
  DWORD        vshaderUsage,
  const DWORD* vertexDecl
);

CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8UnloadProgram(
  CGprogram prog
);

CGD3D8DLL_API CGbool CGD3D8ENTRY
cgD3D8IsProgramLoaded(
  CGprogram prog
);

CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8BindProgram(
  CGprogram prog
);

/* ----- Parameter Management ----------- */
CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8SetUniform(
  CGparameter param,
  const void* floats
);

CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8SetUniformArray(
  CGparameter param,
  DWORD       offset,
  DWORD       numItems,
  const void* values
);

CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8SetUniformMatrix(
  CGparameter      param,
  const D3DMATRIX* matrix
);

CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8SetUniformMatrixArray(
  CGparameter      param,
  DWORD            offset,
  DWORD            numItems,
  const D3DMATRIX* matrices
);

CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8SetTexture(
  CGparameter            param,
  IDirect3DBaseTexture8* tex
);

CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8SetTextureStageState(
  CGparameter              param,
  D3DTEXTURESTAGESTATETYPE type,
  DWORD                    value 
);

CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8SetTextureWrapMode(
  CGparameter param,
  DWORD       value 
);

/* ----- Parameter Management (Shadowing) ----------- */
CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8EnableParameterShadowing(
  CGprogram prog,
  CGbool enable
);

CGD3D8DLL_API CGbool CGD3D8ENTRY
cgD3D8IsParameterShadowingEnabled(
  CGprogram prog
);

/* --------- Profile Options ----------------- */
CGD3D8DLL_API CGprofile CGD3D8ENTRY
cgD3D8GetLatestVertexProfile();

CGD3D8DLL_API CGprofile CGD3D8ENTRY
cgD3D8GetLatestPixelProfile();

CGD3D8DLL_API const char * CGD3D8ENTRY
cgD3D8GetOptimalOptions(
  CGprofile profile
);

/* --------- Error reporting ----------------- */
CGD3D8DLL_API HRESULT CGD3D8ENTRY
cgD3D8GetLastError();

CGD3D8DLL_API const char * CGD3D8ENTRY
cgD3D8TranslateCGerror(
  CGerror error
);

CGD3D8DLL_API const char * CGD3D8ENTRY
cgD3D8TranslateHRESULT(
  HRESULT hr
);

CGD3D8DLL_API void CGD3D8ENTRY
cgD3D8EnableDebugTracing(
  CGbool enable
);

#ifdef __cplusplus
};
#endif

#endif // _WIN32

#endif
