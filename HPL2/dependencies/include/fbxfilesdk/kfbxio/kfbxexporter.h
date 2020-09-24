/*!  \file kfbxexporter.h
 */
 
#ifndef FBXFILESDK_KFBXIO_KFBXEXPORTER_H
#define FBXFILESDK_KFBXIO_KFBXEXPORTER_H

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
#include <fbxfilesdk/kfbxio/kfbxiosettings.h>

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/kfbxio/kfbxprogress.h>

#include <fbxfilesdk/kfbxplugins/kfbxrenamingstrategy.h>
#include <fbxfilesdk/kfbxevents/kfbxevents.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxDocument;
class KFbxExporterImp;
class KFbxFileHeaderInfo;

//! Event that is emitted to plugins before a file is exported to the FBX format.
class KFBX_DLL KFbxEventPreExport : public KFbxEvent<KFbxEventPreExport>
{
    KFBXEVENT_DECLARE(KFbxEventPreExport);
public:
    inline KFbxEventPreExport( KFbxDocument* pDocument ) : mDocument(pDocument) {};

    //! The document to be exported
    KFbxDocument* mDocument;
};

//! Event that is emitted to plugins after a file is exported to the FBX format.
class KFBX_DLL KFbxEventPostExport : public KFbxEvent<KFbxEventPostExport>
{
    KFBXEVENT_DECLARE(KFbxEventPostExport);
public:
    inline KFbxEventPostExport( KFbxDocument* pDocument ) : mDocument(pDocument) {};

    //! The document to be exported
    KFbxDocument* mDocument;
};

  /** Class to export SDK objects into an FBX file.
    * Normally this class is used as is. But for very special needs
	* a user can override Initialize() for special purpose.
	*
	* An exporter will select the appropriate writer to a particular file.
	* Ex: When an exporter must export an FBX 7 file, 
	* the exporter will ask for all registered writers if an FBX 7 file writer is available,
	* then if a writer is found, the exporter will create 
	* the specialized FBX 7 writer and write the file.
	* This way, an exporter can "write" many different type of files like FBX 5/6/7, 3DS, Obj, Dxf, Collada, etc.
	* \see KFbxWriter
	*
	* Typical workflow for using the KFbxExporter class:
	* -# create a SDKManager
	* -# create an IOSettings object
	* -# create an empty scene
	* -# create an exporter
	* -# initialize it with a file name
	* -# set numerous options to control how the exporter will behave.\n
	*    ex: set IOSettings values to export Materials or Textures.
	* -# call KFbxExporter::Export() with the entity to export.
	*
	* \code
	* // ex:
	* // create a SdkManager
	* KFbxSdkManager *lSdkManager = KFbxSdkManager::Create();
    *
	* // create an IOSettings object
	* KFbxIOSettings * ios = KFbxIOSettings::Create(lSdkManager, IOSROOT );
	*
	* // set some IOSettings options 
	* ios->SetBoolProp(EXP_FBX_MATERIAL, true);
	* ios->SetBoolProp(EXP_FBX_TEXTURE,  true);
	*
	* // create an empty scene
	* KFbxScene* lScene = KFbxScene::Create(lSdkManager,"");
	*
	* // create an exporter.
	* KFbxExporter* lExporter = KFbxExporter::Create(lSdkManager, "");
	*
	* // initialize the exporter by providing a filename and the IOSettings to use
	* lExporter->Initialize("C:\\myfile.fbx", -1, ios);
	*
	* // export the scene.
	* lExporter->Export(lScene); 
	*  
	* // destroy the exporter
	* lExporter->Destroy();
	* \endcode
	*
	* \remarks According to the file suffix, a specialized writer will be created internally.\n
	* 		 Ex: for .fbx files a FBX Writer, for .3ds files, a 3ds writer, etc.\n
	*          Supported files formats: FBX 5/6/7 Binary & ASCII, Collada, DXF, OBJ, 3DS
	* \nosubgrouping
	*/
class KFBX_DLL KFbxExporter : public KFbxIO
{
	KFBXOBJECT_DECLARE(KFbxExporter, KFbxIO);

public:

	/** 
	  * \name Export Functions
	  */
	//@{

	/** Initialize object.
	  *	\param pFileName     Name of file to access.
      * \param pFileFormat   file format identifier User does not need to specify it by default.
                             if not specified, plugin will detect the file format according to file suffix automatically.
	  * \param pIOSettings   client IOSettings, if not specified, a default IOSettings will be created
      *	\return              \c true on success, \c false otherwise.
	  * \remarks             To identify the error that occurred, call KFbxIO::GetLastErrorID().
	  */
	virtual bool Initialize(const char *pFileName, int pFileFormat = -1, KFbxIOSettings * pIOSettings = NULL);

    /** Setup file export options settings.
	  *	\return              \c true on success, \c false otherwise.
      */	
	bool GetExportOptions();

	/** Access to a IOSettings object.
      * \return The pointer to IOSettings or \c NULL \c if the object has not been allocated.
      */
    KFbxIOSettings * GetIOSettings();

	/** Set the IOSettings pointer
	  * \param pIOSettings  Pointer on a KFbxIOSettings object.  
	  */
    void SetIOSettings(KFbxIOSettings * pIOSettings);

    
	/** Export the document to the currently created file.
      * \param pDocument          Document to export.
      * \param pNonBlocking       If true, the export process will be executed in a new thread, allowing it to be non-blocking.
                                  To determine if the export finished, refer to the function IsExporting().
	  *	\return                   \c true on success, \c false otherwise.
	  * \remarks                  To identify the error, call KFbxIO::GetLastErrorID().
      */
	bool Export(KFbxDocument* pDocument, bool pNonBlocking=false);

    /** Check if the exporter is currently exporting.
    * \param pExportResult  This parameter, after the export finished, will contain the result of the export success or failure.
    * \return               Return true if the exporter is currently exporting.
    * \remarks              This function will always return false if Export() was called with pNonBlocking set to false.
    *                       This function should be used only in the context of pNonBlocking set to true.
    *                       It is very important to periodically check if the export finished using this function,
    *                       since it will also free up the thread's allocations when its done.
    */
    bool IsExporting(bool& pExportResult);

	/** Get the progress status in non-blocking mode.
	  *	\param pStatus       KString reference to retrieve current status
      *	\return              Percentage of the finished workload
	  */
    float GetProgress(KString &pStatus);

    /** Register a callback function for progress reporting in single thread mode.
	  *	\param pHandler      Pointer of the callback function.
	  */
    void SetProgressCallback(KFbxProgressCallback pHandler);
    
    //@}

	/** 
	  * \name File Format
	  */
	//@{

	/** Get the format of the exported file.
	  *	\return     File format identifier.
	  */
	int GetFileFormat();

	/** Return     \c true if the file format is a recognized FBX format.
	  */
	bool IsFBX();

	/** Get writable version for the current file format.
	  * \return     \c char** string array of writable versions
	  * \remarks the strings returned match the writers registered for the current format
	  * ex: "FBX" for format 0, "DXF" for format 1, etc
	  */
	char const* const* GetCurrentWritableVersions();

	/** Set file version for a given file format.
	  * \param pVersion        String description of the file format.
	  * \param pRenamingMode   Renaming mode.
	  * \return                \c true if mode is set correctly
	  */
	bool SetFileExportVersion(KString pVersion, KFbxSceneRenamer::ERenamingMode pRenamingMode);

	/** Set the resampling rate (only used when exporting to FBX 5.3 and lower)
	  * \param     pResamplingRate resampling rate
	  */
	inline void SetResamplingRate(double pResamplingRate){mResamplingRate = pResamplingRate;}

    /** Set the default rendering resolution.
    * \param pCamName            name of the camera.
    * \param pResolutionMode     resolution mode.
    * \param pW                  width.
    * \param pH                  height.
    */
    void SetDefaultRenderResolution(KString pCamName, KString pResolutionMode, double pW, double pH);

    /**	Get the complete file header information.
    * \return		valid pointer to the complete header information
    */
    KFbxFileHeaderInfo* GetFileHeaderInfo();

	//@}


///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    /** Get file export option settings.
      * \param pFbxObject     Target FBX file.
	  *	\return              \c true on success, \c false otherwise.
      */	
	bool GetExportOptions(KFbx* pFbxObject);

	//Internal use only, please use the other Export function to do your exports
	bool Export(KFbxDocument* pDocument, KFbx* pFbxObject);
private:
    friend void ExportThread(void*);
    bool ExportProcess(KFbxDocument* pDocument);

protected:
	KFbxExporter(KFbxSdkManager& pManager,char const *pName);
	virtual void Destruct(bool pRecursive, bool pDependents);

	void Reset();

	bool FileCreate();
	void FileClose();

	KFbxExporterImp* mImp;

	KString mStrFileVersion;
	double  mResamplingRate;
	KFbxSceneRenamer::ERenamingMode mRenamingMode;
    KFbxFileHeaderInfo*				mHeaderInfo;
	KFbxIOSettings*                 mIOSettings;
	bool                            mClientIOSettings;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXIO_KFBXEXPORTER_H

