
//! File: <HXEngine.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_HXENGINE_H_
#define _HAPTX_HXENGINE_H_

#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x05000
#endif

#include <windows.h>

#ifdef HAPTX_ENGINE_NO_DLL
	#define HAPTX_ENGINE_SPEC
#else
	#ifdef HAPTX_ENGINE_DLL_BUILD
		#define HAPTX_ENGINE_SPEC __declspec(dllexport)	
	#else
		#define HAPTX_ENGINE_SPEC __declspec(dllimport)
	#endif
#endif

#endif
