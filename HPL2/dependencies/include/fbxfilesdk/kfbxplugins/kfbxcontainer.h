/*!  \file kfbxcontainer.h
 */

#ifndef FBXFILESDK_KFBXCONTAINER_H
#define FBXFILESDK_KFBXCONTAINER_H

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

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxplugins/kfbxcontainertemplate.h>
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;

/** Generic container for object grouping and encapsulation.
  * \nosubgrouping
  */
class KFBX_DLL KFbxContainer : public KFbxObject
{
	KFBXOBJECT_DECLARE(KFbxContainer, KFbxObject);

public:

	/**
	  * \name Container dynamic attributes
     */
    //@{
		/** Create a new property.
		  * \param pName Name of the property
		  * \param pType Type of the property
		  * \param pLabel Label of the property
		  * \return the newly created property
		 */
		KFbxProperty CreateProperty(KString pName, KFbxDataType & pType, KString pLabel);
	//@}

		/**
          * \name Public and fast access Properties
          */
        //@{

            /** This property contains the template name information of the container
            *
            * To access this property do: TemplateName.Get().
            * To set this property do: TemplateName.Set(fbxString).
            *
            * Default value is "".
            */
			KFbxTypedProperty<fbxString> TemplateName;

            /** This property contains the template path information of the container
            *
            * To access this property do: TemplatePath.Get().
            * To set this property do: TemplatePath.Set(fbxString).
            *
            * Default value is "".
            */
            KFbxTypedProperty<fbxString> TemplatePath;

            /** This property contains the template version information of the container
            *
            * To access this property do: TemplateVersion.Get().
            * To set this property do: TemplateVersion.Set(fbxString).
            *
            * Default value is "".
            */
			KFbxTypedProperty<fbxString> TemplateVersion;

            /** This property contains the view name information of the container
            *
            * To access this property do: ViewName.Get().
            * To set this property do: ViewName.Set(fbxString).
            *
            * Default value is "".
            */
			KFbxTypedProperty<fbxString> ViewName;
		//@}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
    KFbxContainerTemplate* mContainerTemplate;

protected:

	//This constructor is mandatory, it must be put in the protected section
	//because all objects MUST be created via the Sdk Manager
	KFbxContainer(KFbxSdkManager& pManager, char const* pName);
	virtual bool ConstructProperties(bool pForceSet);

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif
