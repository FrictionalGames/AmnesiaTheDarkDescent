/*!  \file arch.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KARCH_ARCH_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KARCH_ARCH_H

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

/*
 *	Intel IA32 Architecture
 */
#define KARCH_ARCH_80286			200
#define KARCH_ARCH_I386				300
#define KARCH_ARCH_I486				400
#define KARCH_ARCH_PENTIUM			500
#define KARCH_ARCH_PENTIUMPRO		600
#define KARCH_ARCH_PENTIUMII		630
#define KARCH_ARCH_PENTIUMIII		670
#define KARCH_ARCH_PENTIUMIV		700

/*
 *	AMD x64 Architecture
 */
#define KARCH_ARCH_AMD64_GENERIC	800

/*
 *	Intel IA64 Architecture
 */
#define KARCH_ARCH_ITANIUM			1000


#if defined(__linux__) || defined(__CYGWIN32__) //////////////////////////////////////////////// LINUX
	#define KARCH_DEV_GNUC			__GNUC__
	#define KARCH_ENV_POSIX			1

	#if defined(__linux__)
		#define KARCH_ENV_LINUX			1
	#endif

	#if defined(__GNUC__)
		#ifndef _GNU_SOURCE
		#define _GNU_SOURCE
		#endif
		#define KARCH_DEV_GNUC			__GNUC__

		// CPU Determination.
		#if defined(__ia64__) && !defined(KARCH_CPU_IA64)
			#define KARCH_CPU_IA64		KARCH_ARCH_ITANIUM
		#elif defined(__x86_64) && !defined(KARCH_CPU_AMD64)
			#define KARCH_CPU_AMD64 	KARCH_ARCH_AMD64_GENERIC
		#elif !defined(KARCH_CPU_IA32)
			#if defined(__pentiumiii__)
				#define KARCH_CPU_IA32		KARCH_ARCH_PENTIUMIII
			#elif defined(__pentiumii__)
				#define KARCH_CPU_IA32		KARCH_ARCH_PENTIUMII
			#elif defined(__i686__) || defined(__pentiumpro__)
				#define KARCH_CPU_IA32		KARCH_ARCH_PENTIUMPRO
			#elif defined(__i586__) || defined(__pentium__) || defined(__k6__)
				#define KARCH_CPU_IA32		KARCH_ARCH_PENTIUM
			#elif defined(__i486__)
				#define KARCH_CPU_IA32		KARCH_ARCH_I486
			#elif defined(__i386__)
				#define KARCH_CPU_IA32		KARCH_ARCH_I386
			#else
				#define KARCH_CPU_IA32		KARCH_ARCH_I286
			#endif
		#endif

		// ARCH Determination.
		#if defined(KARCH_CPU_IA64)
			#define KARCH_ARCH_IA64			KARCH_CPU_IA64
			#define KARCH_ARCH_64			1
		#elif defined(KARCH_CPU_AMD64)
			#define KARCH_ARCH_AMD64		KARCH_CPU_AMD64
			#define KARCH_ARCH_64			1
		#elif defined(KARCH_CPU_IA32)
			#define KARCH_ARCH_IA32			KARCH_CPU_IA32
			
			#if (KARCH_CPU_IA32 >= KARCH_ARCH_PENTIUMII)
				#define KARCH_CPU_IA32_MMX
			#endif
			#if (KARCH_CPU_IA32 >= KARCH_ARCH_PENTIUMIII)
				#define KARCH_CPU_IA32_SIMD
			#endif
		#endif
	#else
		#error "Compiler not supported"
	#endif
#elif defined(_XBOX)
	#define KARCH_ENV_WIN				1
	#define KARCH_ENV_WIN32				_WIN32
	#define KARCH_ENV_XBOX				_XBOX
	#define KARCH_DEV_MSC				_MSC_VER
	#define KARCH_ARCH_IA32				_M_IX86	
	#define KARCH_CPU_IA32				KARCH_ARCH_PENTIUM	
	//#define KARCH_CPU_IA32_MMX
	//#define KARCH_CPU_IA32_SIMD

#elif defined(_WIN32) || defined(_WIN64) //////////////////////////////////////////////// WIN32/WIN64
	#define KARCH_ENV_WIN				1
	#ifdef KARCH_ENV_WIN32
	#undef KARCH_ENV_WIN32
	#endif
	#define KARCH_ENV_WIN32				_WIN32
	#if defined(_WINNT)
		#define KARCH_ENV_WINNT			_WINNT
	#endif

	#if defined(__MINGW32__)
		#define KARCH_ENV_MING32	__MINGW32__
	#endif
	#if defined(__ICL)
		#define KARCH_DEV_INTEL		__ICL
	#endif
	#if defined(_MSC_VER)
		#define KARCH_DEV_MSC		_MSC_VER
	#elif defined(__GNUC__)
		#define KARCH_DEV_GNUC			__GNUC__
		#if !defined(KARCH_CPU_IA32)
			#if defined(__pentiumiii__)
				#define KARCH_CPU_IA32		KARCH_ARCH_PENTIUMIII
			#elif defined(__pentiumii__)
				#define KARCH_CPU_IA32		KARCH_ARCH_PENTIUMII
			#elif defined(__i686__) || defined(__pentiumpro__)
				#define KARCH_CPU_IA32		KARCH_ARCH_PENTIUMPRO
			#elif defined(__i586__) || defined(__pentium__) || defined(__k6__)
				#define KARCH_CPU_IA32		KARCH_ARCH_PENTIUM
			#elif defined(__i486__)
				#define KARCH_CPU_IA32		KARCH_ARCH_I486
			#elif defined(__i386__)
				#define KARCH_CPU_IA32		KARCH_ARCH_I386
			#else
				#define KARCH_CPU_IA32		KARCH_ARCH_I286
			#endif
		#endif
		#if !defined(KARCH_ARCH_IA32)
			#define KARCH_ARCH_IA32			KARCH_CPU_IA32
		#elif (KARCH_ARCH_IA32 < KARCH_CPU_IA32)
			#error "Selected architecture doesn't support selected instruction set"
		#endif
		#if (KARCH_CPU_IA32 >= KARCH_ARCH_PENTIUMII)
			#define KARCH_CPU_IA32_MMX
		#endif
		#if (KARCH_CPU_IA32 >= KARCH_ARCH_PENTIUMIII)
			#define KARCH_CPU_IA32_SIMD
		#endif
	#else
		#error "Compiler not supported"
	#endif
	#if defined(_M_IX86)
		#define KARCH_ARCH_IA32			_M_IX86
		#define KARCH_CPU_IA32			KARCH_ARCH_PENTIUMIII // to force a minimum of PENTIUM II
		#if !defined(KARCH_CPU_IA32)
			#if (KARCH_ARCH_IA32 >= KARCH_ARCH_PENTIUM)
				#define KARCH_CPU_IA32	KARCH_ARCH_PENTIUM
			#else
				#define KARCH_CPU_IA32	KARCH_ARCH_IA32
			#endif
		#endif
		#if (KARCH_CPU_IA32 >= KARCH_ARCH_PENTIUMII)
			#define KARCH_CPU_IA32_MMX
		#endif
		#if (KARCH_CPU_IA32 >= KARCH_ARCH_PENTIUMIII)
			#define KARCH_CPU_IA32_SIMD
		#endif
	#elif defined(_M_X64)
		#define KARCH_ARCH_X64			_M_X64
		#define KARCH_CPU_X64			KARCH_ARCH_AMD64
		#define KARCH_ENV_WIN64			_WIN64
		#define KARCH_ARCH_64			1
	#elif defined(_M_IA64)
		#define KARCH_ARCH_IA64			_M_IA64
		#define KARCH_CPU_IA64			KARCH_ARCH_ITANIUM
		#define KARCH_ENV_WIN64			_WIN64
		#define KARCH_ARCH_64		1
	#else
		#error "Architecture not supported"
	#endif

#elif defined(__MACH__)
	#if __LP64__ && !defined(__x86_64__)
		#define __x86_64__
	#endif
	#if defined(__i386__)
		#define KARCH_CPU_IA32		KARCH_ARCH_PENTIUM
	 	#define KARCH_ARCH_IA32		KARCH_CPU_IA32
	#elif defined(__x86_64__)
	 	#define KARCH_ARCH_X64		_M_X64
		#define KARCH_ARCH_64		1
	#else
	    #define KARCH_ARCH_POWERPC
		#define KARCH_CPU_POWERPC
	#endif
	
    #define KARCH_DEV_MACOSX
    #define KARCH_DEV_GNUC_MACOSX
    #define KARCH_ENV_MACOSX
    #define KARCH_DEV_GNUC			__GNUC__
	#define KARCH_ENV_POSIX			1

#else	//////////////////////////////////////////////// GENERIC
	#error "Environment not supported"
#endif

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KARCH_ARCH_H

