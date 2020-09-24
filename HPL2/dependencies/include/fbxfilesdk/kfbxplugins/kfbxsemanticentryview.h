#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSEMANTICENTRYVIEW_H 
#define FBXFILESDK_KFBXPLUGINS_KFBXSEMANTICENTRYVIEW_H

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
#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxentryview.h>
#include <fbxfilesdk/kfbxplugins/kfbxbindingtableentry.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/* This binding table entry type stores a binding between a 
 * KFbxProperty and a semantic within a shader file.
 */
class KFBX_DLL KFbxSemanticEntryView : public KFbxEntryView
{
public:

	static const char* sEntryType;

	KFbxSemanticEntryView (KFbxBindingTableEntry* pEntry, bool pAsSource, bool pCreate = false);
	virtual ~KFbxSemanticEntryView();
	
	void SetSemantic( const char* pSemantic );
	KString GetSemantic(bool pAppendIndex = true) const;

	int GetIndex() const;

	virtual const char* EntryType() const;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

protected:

private:

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSEMANTICENTRYVIEW_H

