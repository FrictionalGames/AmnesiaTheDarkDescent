/*!  \file kfbxcontainertemplate.h
 */

#ifndef FBXFILESDK_KFBXCONTAINERTEMPLATE_H
#define FBXFILESDK_KFBXCONTAINERTEMPLATE_H

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
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/globals.h>

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

//Container Template tokens
#define CONTAINER_TEMPLATE  "templates"
#define TEMPLATE            "template"
#define EXTENDS_TEMPLATE    "extends"

class KFbxSdkManager;

/** Class for Container Template files.
* \nosubgrouping
*/
class KFBX_DLL KFbxContainerTemplate : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxContainerTemplate, KFbxObject);

public:

    /** Parse template file to get extend templates.
    * \param  pTemplateFilePath  The template file to be parsed.
    * \param  pExtendTemplateNames  Fill extend templates' names to this array.
    * \remark Call this function to get extend templates' names.
    */
    void ParseTemplateFile(const char* pTemplateFilePath, KArrayTemplate<KString*>& pExtendTemplateNames);

    /** Add extend template path.
    * \param  pExtendTemplatePath  The template file path to be added.
    */
    void AddExtendTemplatePath(const char* pExtendTemplatePath);

    /** Get the (pIndex)th extend template path.
    * \param  pIndex  Index of the queried item.
    * \return The (pIndex)th extend template path.
    */
    char* GetExtendTemplatePathAt(kUInt pIndex) const;

    /** Get the count of extend template path.
    * \return The count of extend template path.
    */
    kUInt GetExtendTemplateCount() const;

    /** Clear the extend template path.
    */
    void ClearExtendTemplatePath();

    /** This property contains the template name.
    *
    * To access this property do: TemplateName.Get().
    * To set this property do: TemplateName.Set(fbxString).
    *
    * Default value is "".
    */
    KFbxTypedProperty<fbxString> ContainerTemplateName;

    /** This property contains the template path.
    *
    * To access this property do: TemplatePath.Get().
    * To set this property do: TemplatePath.Set(fbxString).
    *
    * Default value is "".
    */
    KFbxTypedProperty<fbxString> ContainerTemplatePath;

    /** This property contains the template package name.
    *
    * To access this property do: TemplatePackageName.Get().
    * To set this property do: TemplatePackageName.Set(fbxString).
    *
    * Default value is "".
    */
    KFbxTypedProperty<fbxString> ContainerTemplatePackageName;

    /** This property contains the template version information of the container
    *
    * To access this property do: TemplateVersion.Get().
    * To set this property do: TemplateVersion.Set(fbxString).
    *
    * Default value is "".
    */
    KFbxTypedProperty<fbxString> ContainerTemplateVersion;


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

    //This constructor is mandatory, it must be put in the protected section
    //because all objects MUST be created via the Sdk Manager
    KFbxContainerTemplate(KFbxSdkManager& pManager, char const* pName);
    virtual bool ConstructProperties(bool pForceSet);

    //Destructor
    virtual void Destruct(bool pRecursive, bool pDependents);
    
private:
    KString GetContent(xmlNode* pXmlNode) const;
    KString GetProperty(xmlNode* pXmlNode, const char* pProperty) const;
    void GetExtendTemplates(xmlNode* pXmlNode, KArrayTemplate<KString*>& pExtendTemplateNames) const;

    xmlDocPtr mXmlDoc;

    KArrayTemplate<KString*> mExtendTemplatePaths;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

    friend class KFbxContainer;
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXCONTAINERTEMPLATE_H
