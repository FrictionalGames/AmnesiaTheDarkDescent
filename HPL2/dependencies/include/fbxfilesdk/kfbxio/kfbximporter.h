/*!  \file kfbximporter.h
 */

#ifndef FBXFILESDK_KFBXIO_KFBXIMPORTER_H
#define FBXFILESDK_KFBXIO_KFBXIMPORTER_H

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

#include <fbxfilesdk/kfbxio/kfbxio.h>
#include <fbxfilesdk/kfbxevents/kfbxevents.h>

#include <fbxfilesdk/kfbxio/kfbxiosettings.h>
#include <fbxfilesdk/kfbxio/kfbxprogress.h>

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxDocument;
class KFbxImporterImp;
class KFbxTakeInfo;
class KFbxDocumentInfo;
class KFbxAxisSystem;
class KFbxSystemUnit;
class KFbxStatistics;
class KFile;
class KFbxFileHeaderInfo;

//! Event that is emitted to plugins before a FBX file has been imported.
class KFBX_DLL KFbxEventPreImport : public KFbxEvent<KFbxEventPreImport>
{
    KFBXEVENT_DECLARE(KFbxEventPreImport);
public:
    inline KFbxEventPreImport( KFbxDocument* pDocument ) : mDocument(pDocument) {};

    //! The document the FBX file is to be imported into.
    KFbxDocument* mDocument; 
};

//! Event that is emitted to plugins after a FBX file has been imported.
class KFBX_DLL KFbxEventPostImport : public KFbxEvent<KFbxEventPostImport>
{
    KFBXEVENT_DECLARE(KFbxEventPostImport);
public:
    inline KFbxEventPostImport( KFbxDocument* pDocument ) : mDocument(pDocument) {};

    //! The imported document
    KFbxDocument* mDocument; 
};

  /** Class to import an FBX file into SDK objects.
    * Normally this class is used as is. But for very special needs
	* a user can override Initialize() for special purpose.
	*
	* An importer will select the appropriate reader to a particular file.
	* Ex: When an importer must import an FBX 7 file, 
	* the importer will ask for all registered readers if an FBX 7 file reader is available,
	* then if a reader is found, the importer will create 
	* the specialized FBX 7 reader and read the file.
	* This way, an importer can "read" many different type of files like FBX 5/6/7, 3DS, Obj, Dxf, Collada, etc.
	* \see KFbxReader
	*
	* Typical workflow for using the KFbxImporter class:
	* -# create a SDKManager
	* -# create an IOSettings object
	* -# create an empty scene
	* -# create an importer
	* -# initialize the importer with a file name and IOSettings
	* -# set numerous states, take information, defining how the importer will behave
	* -# call KFbxImporter::Import() with an empty scene
	* \code
	* ex:
	* // create a SdkManager
	* KFbxSdkManager *lSdkManager = KFbxSdkManager::Create();
	*
	* // create an IOSettings object
	* KFbxIOSettings * ios = KFbxIOSettings::Create(lSdkManager, IOSROOT );
	*
	* // set some IOSettings options 
	* ios->SetBoolProp(IMP_FBX_MATERIAL, true);
	* ios->SetBoolProp(IMP_FBX_TEXTURE,  true);
	*
	* // create an empty scene
	* KFbxScene* lScene = KFbxScene::Create(lSdkManager,"");
	*
	* // Create an importer.
	* KFbxImporter* lImporter = KFbxImporter::Create(lSdkManager, "");
	*
	* // Initialize the importer by providing a filename and the IOSettings to use
	* lImporter->Initialize("C:\\myfile.fbx", -1, ios);
	*
	* // Import the scene.
	* lImporter->Import(lScene); 
	*
	* // Destroy the importer.
	* lImporter->Destroy(); 
	* \endcode
	*
	* \remarks According to the file suffix, a specialized reader will be created internally.
	*          Ex: for .fbx files a FBX Reader, for .3ds files, a 3ds reader, etc.
	*          Supported files formats: FBX 5/6/7 Binary & ASCII, Collada, DXF, OBJ, 3DS
	* \nosubgrouping
	*/
class KFBX_DLL KFbxImporter : public KFbxIO
{
	KFBXOBJECT_DECLARE(KFbxImporter,KFbxIO);
public:
	/** 
	  * \name Import Functions
	  */
	//@{

	/** Initialize object.
	  * \param pFileName          Name of file to access.
      * \param pFileFormat        file format identifier User does not need to specify it by default.
                                  if not specified, plugin will detect the file format according to file suffix automatically.
	  * \param pIOSettings        client IOSettings, if not specified, a default IOSettings will be created
	  * \return                   \c true on success, \c false otherwise.
	  * \remarks                  To identify the error that occurred, call KFbxIO::GetLastErrorID().	
      * \remarks                  You do not need to give the pFileFormat if the suffix of pFileName is recognized
	  */
	virtual bool Initialize(const char* pFileName, const int pFileFormat=-1, KFbxIOSettings * pIOSettings=NULL);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
	/** Initialize object.
	  *	\param pFile              file to access. Ownership is transfered to this object.
      * \param pFileFormat        file format identifier User does not need to specify it by default.
                                  if not specified, plugin will detect the file format according to file suffix automatically.
	  * \param pIOSettings        client IOSettings, if not specified, a default IOSettings will be created
      *	\return                   \c true on success, \c false otherwise.
	  * \remarks                  To identify the error that occurred, call KFbxIO::GetLastErrorID().	  
      * \remarks                  You do not need to give the pFileFormat if the suffix of pFileName is recognized
      */
	virtual bool Initialize(KFile* pFile, const int pFileFormat=-1, KFbxIOSettings* pIOSettings=NULL);

	/** Get the file, if any.
	   * \return The file or NULL if the file has not been set.
	   */
	virtual KFile * GetFile();
#endif 

	/** Get the FBX version number of the FBX file.
	  * FBX version numbers start at 5.0.0.
	  * \param pMajor        Version major number.
	  * \param pMinor        Version minor number.
	  * \param pRevision     Version revision number.
	  *	\remarks             This function must be called after KFbxImporter::Initialize().	  
	  */
	void GetFileVersion(int& pMajor, int& pMinor, int& pRevision);

	/**	Get the default rendering resolution if present in the file header.
	  * \param pCamName            Returned name of the camera.
	  * \param pResolutionMode     Returned resolution mode.
	  * \param pW                  Returned width.
	  * \param pH                  Returned height.
	  * \return                    \c true if the default rendering settings are defined in the file, otherwise
	  *                            returns \c false with empty parameters.
	  */
	bool GetDefaultRenderResolution(KString& pCamName, KString& pResolutionMode, double& pW, double& pH);

	/**	Get the complete file header information.
	  * \return		valid pointer to the complete header information
	  */
	KFbxFileHeaderInfo* GetFileHeaderInfo();

	/** \enum EStreamOptionsGeneration Stream options identifiers.
	  * - \e eSTREAMOPTIONS_PARSE_FILE  Parse the file
	  * - \e eSTREAMOPTIONS_FAST		Do not parse the file.
	  * - \e eSTREAMOPTIONS_COUNT
	  */
	typedef enum 
	{
		eSTREAMOPTIONS_PARSE_FILE,  // Parse the file
		eSTREAMOPTIONS_FAST,        // Do not parse the file
		eSTREAMOPTIONS_COUNT
	} EStreamOptionsGeneration;

    /** Read the currently opened file header to retrieve information related to takes.
	  * \param pStreamOptionsGeneration     Stream options identifier.
	  *	\return                \c true on success, \c false otherwise.
	  * \remarks                            Caller gets ownership of the returned structure.
      */
 	  bool GetImportOptions(EStreamOptionsGeneration pStreamOptionsGeneration = eSTREAMOPTIONS_PARSE_FILE);	

    /** Read the currently opened file header to retrieve information related to takes.
	  * \param pFbxObject     Target FBX file.
	  *	\return                \c true on success, \c false otherwise.
	  * \remarks              Caller gets ownership of the returned structure.
      */

	  bool GetImportOptions(KFbx* pFbxObject);

    /** Import the currently opened file into a scene. 
      * \param pDocument          Document to fill with file content.
	  * \param pNonBlocking       If true, the import process will be executed in a new thread, allowing it to be non-blocking.
	                              To determine if the import finished, refer to the function IsImporting().
	  *	\return                   \c true on success, \c false otherwise.
	  * \remarks                  To identify the error that occurred, call KFbxIO::GetLastErrorID().
	  *                           If the imported file is password protected and the password is not
	  *                           set or wrong, function KFbxIO::GetLastErrorID() returns 
	  *                           KFbxIO::ePASSWORD_ERROR.
      */
	  bool Import(KFbxDocument* pDocument, bool pNonBlocking=false);

    /** Check if the importer is currently importing.
	  * \param pImportResult  This parameter, after the import finished, will contain the result of the import success or failure.
      * \return               Return true if the importer is currently importing.
	  * \remarks              This function will always return false if Import() was called with pNonBlocking set to false.
	  *                       This function should be used only in the context of pNonBlocking set to true.
	  *                       It is very important to periodically check if the import finished using this function,
      *                       since it will also free up the thread's allocations when its done.
      */
	  bool IsImporting(bool& pImportResult);

	/** Get the progress status in non-blocking mode.
	  *	\param pStatus       KString reference to retrieve current status
      *	\return              Percentage of the finished workload
	  */
      float GetProgress(KString &pStatus);

    /** Register a callback function for progress reporting in single thread mode.
      *	\param pHandler      Pointer of the callback function
      */
      void SetProgressCallback(KFbxProgressCallback pHandler);

    /** Explicitly set the embedding extraction folder. If this is never called, the FBX SDK will determine the best folder to extract embedded files.
      * \param pExtractionFolder The file path name where the embedded files should be extracted.
      */
	void SetEmbeddingExtractionFolder(const char* pExtractFolder);

	/** Retrieve the current folder destination where the embedded files will be extracted. This might not be initialized until file I/O is performed.
	  */
	const char* GetEmbeddingExtractionFolder();

	/** Access to a IOSettings object.
      * \return The pointer to IOSettings or \c NULL \c if the object has not been allocated.
      */
 	  KFbxIOSettings* GetIOSettings();

	/** Set the IOSettings pointer
	  * \param pIOSettings Point to a KFbxIOSettings object.
	  */
	  void SetIOSettings(KFbxIOSettings* pIOSettings);

	/** Set the password.
	  * All subsequently imported files are opened with the given password.
      * \param pPassword     Password string.
      */
      void SetPassword(char* pPassword);

	/** 
	  * \name Animation Stack Description Access
	  * \see KFbxAnimStack
	  */
	//@{
	
    /** Get the number of available animation stacks in the file.
      * \return      Number of animation stacks.
      *	\remarks     This function must be called after KFbxImporter::Initialize().
      */
      int GetAnimStackCount();

	/** Get the take information about an available take.
	  * Use the returned reference to a KFbxTakeInfo object to set whether the indexed take is imported.
	  *	\param pIndex     Index of the requested take.
	  *	\return           Take information or \c NULL if function failed.
	  *	\remarks          This function must be called after KFbxImporter::Initialize().
	  */
	  KFbxTakeInfo* GetTakeInfo(int pIndex);

    /** Return the active animation stack name.
      * \return     Active animation stack name if there is one, otherwise returns an empty string.
      * \remarks    This function must be called after KFbxImporter::Initialize().
      */
      KString GetActiveAnimStackName();

	//@}

	/** 
	  * \name Scene Description Access
	  */
	//@{

	/** Get the scene info.
	  * \return     Pointer to the scene info or \c NULL if no scene information
	  *             is available in the file.
	  */
	KFbxDocumentInfo* GetSceneInfo();

	//@}
	/** 
	  * \name File Format
	  */
	//@{

	/** Returns the index of the reader (FbxReader) associated with the file format. 
	    This index is considered the identifier of the file format. 
		The array of registered readers can't be retrieved. 
		\return Index of the registered FbxReader associated with the file format. 
		        If no reader found return -1.
		\remarks According to the number of readers registered this value can change 
		         for the same reader between SDK Manager instantiations.
	*/
	int GetFileFormat ();

	/** \return     \c true if the file format is a recognized FBX format.
	  */
	bool IsFBX();
	//@}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

	/** Set a flag to parse or not the GlobalSettings data section
	  * when the importer is initialized.
	  *	\param pState    true or false
	  *	\remarks		 This function is used internally by the FBX readers
	  */
	void ParseForGlobalSettings(bool pState);

	/** Set a flag to parse or not in a FBX file the Summary data section 
	  * or the Definition data section when the importer is initialized.
	  *	\param pState    true or false
	  *	\remarks		 This function is used internally by the FBX readers
	  */
    void ParseForStatistics(bool pState);

	/** Get FBX file Axis information and FBX file System units
	  *	\param pAxisSystem    pointer to a KFbxAxisSystem object
	  *	\param pSystemUnits    pointer to a KFbxSystemUnit object
	  *	\return     \c true on success, \c false otherwise.
	  *	\remarks    This function must be called after KFbxImporter::Initialize().
	  */
	bool GetAxisInfo(KFbxAxisSystem* pAxisSystem, KFbxSystemUnit* pSystemUnits);

	/** Get FBX file Statistics informations.
	  * Ex: Number of Models, Characters, Textures, Materials, etc.
	  *	\param pStatistics    pointer to a KFbxStatistics object
	  *	\return     \c true on success, \c false otherwise.
	  *	\remarks    This function must be called after KFbxImporter::Initialize().
	  */
    bool GetStatistics(KFbxStatistics* pStatistics);

	/** Get FBX file time mode read from GlobalSettings in FBX 6.n and FBX 7.n
	  *	\param pTimeMode  ref to a KTime::ETimeMode enum
	  *	\return     \c true on success, \c false otherwise.
	  *	\remarks    This function must be called after KFbxImporter::Initialize().
	  *             Can be used for statistics (via GlobalSettings) before loading the whole scene from the file.
	  */
	bool GetFrameRate(KTime::ETimeMode &pTimeMode);


#ifndef DOXYGEN_SHOULD_SKIP_THIS
	//Internal use only, please use the other Import function to do your imports
	bool Import(KFbxDocument* pDocument, KFbx* pFbxObject);

private:
	friend void ImportThread(void*);
	bool ImportProcess(KFbxDocument* pDocument);

protected:
	void GetImportOptionsInfo();
    
	KFbxImporter(KFbxSdkManager& pManager,char const* pName);
	virtual void Destruct(bool pRecursive, bool pDependents);

	void Reset();

    bool FileOpen(KFile* pFile = NULL);
	void FileClose();
    
    bool IsNativeExtension ();

	KFbxImporterImp*				mImp;
    KFile*                          mFile;
	bool							mImportOptionsDone;
	KArrayTemplate<KFbxTakeInfo*>	mTakeInfo;
	KFbxDocumentInfo*				mSceneInfo;
	KString							mActiveAnimStackName;
	int								mMajorVersion;
	int								mMinorVersion;
	int								mRevisionVersion;
	KFbxFileHeaderInfo				*mHeaderInfo;
	KFbxIOSettings*                 mIOSettings;
	bool                            mClientIOSettings;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXIO_KFBXIMPORTER_H

