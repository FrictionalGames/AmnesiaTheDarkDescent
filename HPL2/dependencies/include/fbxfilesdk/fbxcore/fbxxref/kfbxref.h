/*!  \file kfbxref.h
 */

#ifndef FBXFILESDK_FBXCORE_FBXXREF_KFBXREF_H
#define FBXFILESDK_FBXCORE_FBXXREF_KFBXREF_H

/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
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

#include <fbxfilesdk/fbxcore/fbxxref/fbxxref_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxsdkmanager.h>

namespace FBXXREF_NAMESPACE {

    class KXRefManagerProject;
    class KFbxDocument;


    /** This class manages external references to files.
      * \nosubgrouping
      */
    class KFBX_DLL KFbxXRefManager {
        public:

            //! Default constructor.
            KFbxXRefManager();

            //! Destructor.
            virtual ~KFbxXRefManager();

            /**
              * \name Predefined Project Types
              */
            //@{

                //! This project represents an URL for storing temporary files.
                static const char* sTemporaryFileProject;

                //! This project represents an URL for configuration files.
                static const char* sConfigurationProject;

                //! This project represents an URL for storing localization files (that is not part of the asset library).
                static const char* sLocalizationProject;

                /** This project is used for creating the ".fbm" folders that are used for
                  * storing embedded resources in FBX files.
                  *  
                  * When not set, or if the folder is not writable, the ".fbm"
                  * folder is created alongside the FBX file.
                  *  
                  * If we cannot write in that folder, we look at the sTemporaryFileProject location.
                  * If no folder is set in the sTemporaryFileProject location, or it is not
                  * writable, the operating system's Temp folder becomes the location.
                  */
                static const char* sEmbeddedFileProject;
            //@}

        public:

            /**
              * \name XRef URL properties
              */
            //@{
                /** Returns the number of URLs that are stored in a property.
				  * \param pProperty                The property. 
                  * \return                         The URL count.
                  */
                static int     GetUrlCount(KFbxProperty const &pProperty);

                /** Returns the number of URLs that are stored in a string.
				  * \param pUrl                     The string.
				  * \return                         The URL count.
				  */
				 
                static int     GetUrlCount(fbxString const& pUrl);

                /** Checks whether the URL at the given index stored in the property is relative or not.
				  * \param pProperty                The property.
				  * \param pIndex                   The URL index.
                  * \return                         \c True if the URL is relative, \c false if the URL is not relative.
                  */
                static bool IsRelativeUrl  (KFbxProperty const &pProperty,int pIndex);

                /** Returns the URL stored in the property at the given index.
				  * \param pProperty                The property.
				  * \param pIndex                   The URL index.
                  * \return The URL
                  */
                static KString GetUrl(KFbxProperty const &pProperty,int pIndex);

                /** Tries to resolve the URL stored in the property at the given index.
				  * \param pProperty                The property.
				  * \param pIndex                   The URL index.
				  * \param pResolvedPath            Filled with the resolved path.
                  * \return                         \c True if the URL is resolved, return \c false if the URL is not resolved.
                  */
                bool GetResolvedUrl (KFbxProperty const &pProperty,int pIndex,KString & pResolvedPath) const;
            
                /** Tries to resolve the specified URL.
				  * \param pUrl                     The specified URL.
				  * \param pDoc                     The document whose ".fbm" folder is used to resolve the URL.
				  * \param pResolvedPath            Filled with the resolved path.
                  * \return                         \c True if the URL is resolved, return \c false if the URL is not resolved.
                  */
                bool GetResolvedUrl (char const* pUrl, KFbxDocument* pDoc, KString& pResolvedPath) const;
            //@}

                /** Looks for the first file that matches a specified "pattern",
                  * which is built as:
                  *
                  * if pOptExt is given:         prefix*.ext
                  * If pOptExt is NULL:          prefix*
                  * if pOptExt is "" or ".":     prefix*.
                  *
                  * Returns the URL of the first matching files. This function cannot be
                  * used to resolve folders, only files.
                  *
                  * If a document is given, we start by looking at the document's ".fbm" folder.
			      * \param pPrefix                  The prefix of the pattern.
			      * \param pOptExt                  The extension of the pattern.
			      * \param pDoc                     The given document.
			      * \param pResolvedPath            Filled with the first matching URL.
			      * \return                         \c True if one matching file is found, returns \c false if no matching file is found.
                  */
                bool GetFirstMatchingUrl(char const* pPrefix, char const* pOptExt, const KFbxDocument* pDoc, KString& pResolvedPath) const;

            /**
              * \name XRef Resolve URL and Projects
              */
            //@{

                /** Adds an XRef Project.
                  * Note:Only one URL is associated with a project. Calling 
                  * this on an existing project replaces the project's existing URL.
                  * \param pName                    The name of the project
                  * \param pUrl                     The URL to be associated with the project.
                  * \return                         \c True if the project is added successfully, \c false if no project is added.
                 */
                bool        AddXRefProject   (const char *pName,const char *pUrl);

                /** Adds an XRef Project.
                  * Note:Only one URL is associated with a project. Calling 
                  * this on an existing project replaces the project's existing URL.
                  * \param pName                    The name of the project
				  * \param pExtension               The extension of the project.
                  * \param pUrl                     The URL to be associated with the project.
                  * \return                         \c True if the project is added successfully, returns \c false if no project is added.
                 */
                bool        AddXRefProject   (const char *pName,const char *pExtension,const char *pUrl);

                /** Adds an XRef project based on the document's EmbeddedUrl 
                  * property if set, if EmbeddedUrl is not set, based on its current URL property. 
                  * \param pDoc                     The document used to name the project and to specify the URL.
                  * \return                         \c True if the project is added successfully, returns \c false if no project is added.
                  * \remarks                        The project name is set as the document name and the URL is set as EmbeddedUrl or URL of the document.
                  */
                bool        AddXRefProject   (KFbxDocument* pDoc);

				/** Removes an XRef Projects.
				  * \param pName                    The name of the project to be removed.
				  * \return                         \c True if the project is removed successfully, returns \c false if the project with the name does not exist.
				  */
                bool        RemoveXRefProject(const char *pName);

				/** Removes all XRef Projects. 
                  * \return                         \c True always.
                  */
                bool        RemoveAllXRefProjects();

                /** Returns the number of XRef Projects.
				  * \return                         The number of XRef Projects.
				  */
                int         GetXRefProjectCount() const;

				/** Returns the name of the XRef project at the specified index.
				  * \param pIndex                   The XRef project index.
				  * \return                         The XRef project name.
				  */
                const char *GetXRefProjectName(int pIndex) const;

                /** Returns the base URL for the given project.
                  * \param pName                    The name of the given project
                  * \return                         The base URL of the project or returns NULL if the project with the name is not found.
                  */
                const char* GetXRefProjectUrl(const char* pName);   // FIXME: Should be const, will break AV.

                /** Returns the base URL for the given project.
                  * \param pName                    The name of the given project
                  * \return                         The base URL of the project or returns NULL if the project with the name is not found.
                  */
                const char* GetXRefProjectUrl(const char* pName) const;

                /** Returns the base URL for the given project.
                  * \param pIndex                   The index of the project.
                  * \return                         The base URL of the project or NULL if the index is out of bounds.
                  */
                const char* GetXRefProjectUrl(int pIndex) const;

                /** Checks if a project with the given name is defined in this manager.
				  * \param pName                    The name of the project.
				  * \return                         \c True if the project is defined in this manager, returns \c false if it isn't defined in this manager.
				  */
                inline bool HasXRefProject( const char* pName ) { return GetXRefProjectUrl(pName) != NULL; }

                /** Tries to resolve an relative URL
				  * \param pUrl                     The relative URL to be resolved.
				  * \param pResolvePath             Filled with the resolved path.
                  * \return                         \c True if the URL is resolved, returns \c false if the URL is not resolved.
                  */
                bool GetResolvedUrl (char const *pUrl,KString & pResolvePath) const;

            //@}
        private:
            KArrayTemplate<KXRefManagerProject*>    mProjects;

            static bool UrlExist(const char* pUrl);
    };

}

#endif // FBXFILESDK_FBXCORE_FBXXREF_KFBXREF_H

