/*!  \file kfbxprocessor.h
 */

#ifndef FBXFILESDK_FBXPROCESSORS_KFBXPROCESSOR_H
#define FBXFILESDK_FBXPROCESSORS_KFBXPROCESSOR_H

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

#include <fbxfilesdk/fbxprocessors/fbxprocessors_def.h>

#include <fbxfilesdk/fbxcore/fbxcollection/kfbxcollection.h>

namespace FBXPROCESSORS_NAMESPACE
{
	/** The class and its derived classes(e.g. KFbxProcessorXRefCopy, KFbxProcessorXRefCopyUserLibrary, etc.) are used to process shader, library, asset, etc.
      * For example, you could update object property and its value via KFbxProcessor::ProcessObject(), KFbxProcessor::internal_ProcessObject(), etc.
	  */
	class KFBX_DLL KFbxProcessor : public KFbxObject
	{
		KFBXOBJECT_DECLARE(KFbxProcessor,KFbxObject);

	public:
		/**
		* \name Processor management
		*/
		//@{
            /** Process the specified collection.
            * \param pCollection
            */
			bool					ProcessCollection(KFbxCollection *pCollection=0);

            /** Process the specified object.
            * \param pCollection KFbxObject to process
            */
			bool					ProcessObject	 (KFbxObject *pCollection=0);
		//@}

	///////////////////////////////////////////////////////////////////////////////
	//  WARNING!
	//	Anything beyond these lines may not be documented accurately and is 
	// 	subject to change without notice.
	///////////////////////////////////////////////////////////////////////////////
	#ifndef DOXYGEN_SHOULD_SKIP_THIS
    protected:
		KFbxProcessor(KFbxSdkManager& pManager, char const* pName);

        virtual bool				internal_ProcessCollectionBegin (KFbxCollection *pCollection);
        virtual bool				internal_ProcessCollectionEnd 	(KFbxCollection *pCollection);
        virtual bool				internal_ProcessObject  		(KFbxObject*	 pObject);
        virtual bool				internal_ProcessCollection		(KFbxCollection* pCollection);
	#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 
	};
}

#endif // FBXFILESDK_FBXPROCESSORS_KFBXPROCESSOR_H

