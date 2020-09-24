/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#ifndef FBXFILESDK_FBXFILESDK_VERSION_H
#define FBXFILESDK_FBXFILESDK_VERSION_H

//FBX SDK version defines
#define FBXSDK_VERSION_MAJOR	2012
#define FBXSDK_VERSION_MINOR	2
#define FBXSDK_VERSION_POINT	0

#define FBXSDK_VERSION_REVISION	20110927

//FBX SDK version string macros
#define FBXSDK_DEF_TO_STR(x) #x

#if FBXSDK_VERSION_POINT == 0
	#define FBXSDK_VER_TO_STR(a, b, c)	FBXSDK_DEF_TO_STR(a.b)
#else
	#define FBXSDK_VER_TO_STR(a, b, c)	FBXSDK_DEF_TO_STR(a.b.c)
#endif

//FBX SDK version string
#define FBXSDK_VERSION_STRING	FBXSDK_VER_TO_STR(FBXSDK_VERSION_MAJOR, FBXSDK_VERSION_MINOR, FBXSDK_VERSION_POINT)

//FBX SDK namespace definition
#define FBXSDK_NAMESPACE		fbxsdk_2012_2

#endif /* FBXFILESDK_FBXFILESDK_VERSION_H */
