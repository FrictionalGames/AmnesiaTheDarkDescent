/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxreader.h
 */

#ifndef FBXFILESDK_KFBXIO_KFBXREADER_H
#define FBXFILESDK_KFBXIO_KFBXREADER_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>
#include <fbxfilesdk/components/kbaselib/kfbx/kfbx.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxDocument;
class KFbxDocumentInfo;
class KFbxNode;
class KFbxSdkManager;
class KFbxImporter;
class KFbxIOSettings;
class KFbxAxisSystem;
class KFbxSystemUnit;
class KFbxStatistics;
class KFbxTakeInfo;
class KFile;
class KFbxObject;
class KFbxScene;
class KFbxProgress;

 /** Base class of other readers used internally.
   * This class provides the interfaces for reading files.
   *
   * The role of the reader is to effectively "read" specific file data
   * vs the role of the importer is to select a specific reader
   * and launch the reading of a file through that reader.
   * \see KFbxImporter 
   *
   * ex:
   * - KFbxReaderFbx  can read FBX 5 format files
   * - KFbxReaderFbx6 can read FBX 6 format files
   * - KFbxReaderFbx7 can read FBX 7 format files
   * - KFbxReaderCollada can read Collada files
   * - KFbxReaderDxf can read Dxf files
   * - ... etc.
   *
   * A SDK user should - normally - not use this class,
   * except if a custom reader must be created for plug-in extension,
   * then KFbxReader must be the base class for 
   * the new custom reader in that particular situation.
   * \nosubgrouping
   */

class KFBX_DLL KFbxReader
{
public:
	/** Constructor.
      *	\param pManager        The KFbxSdkManager Object.
      * \param pID             Id for current reader.
      */
    KFbxReader(KFbxSdkManager& pManager, int pID);

	 /** Destructor.
      */
    virtual ~KFbxReader();
    
	/** Error identifiers.
     */
    typedef enum
    {
        eFILE_CORRUPTED,
        eFILE_VERSION_NOT_SUPPORTED_YET,
        eFILE_VERSION_NOT_SUPPORTED_ANYMORE,
        eFILE_NOT_OPENED,
        eFILE_NOT_CREATED,
        eWRONG_PASSWORD,
        eINVALID_DOCUMENT_HANDLE,
        eDOCUMENT_NOT_SUPPORTED,
        eUNRESOLVED_EXTERNAL_REFERENCES,
        eUNIDENTIFIED_ERROR,
        eERROR_COUNT
    } EError;

	/** Information type to request.
     * \remarks Used internally to get reader file information.
     */
    enum KInfoRequest {
        eInfoExtension,     //!< To get the file ext for a reader ex: "FBX".		
        eInfoDescriptions,	//!< To get the file description for a reader ex: "Autodesk FBX (*.fbx)".
        eReserved1 = 0xFBFB,
    };

	/** Flags for reading parts of file.
     * \remarks Used internally when an importer is initialized to get some information very fast.
     */
    enum EFileOpenSpecialFlags {
		ParseForGlobalSettings = 1, //!< Used for reading the Global settings section when an importer is initialized.
		ParseForStatistics = 2      //!< Used for reading a group of statistics when an importer is initialized.     
	};

	/** \internal Helper typedef for passing KFbxReader creator function as argument (used internally) */
    typedef KFbxReader*			(*CreateFuncType)(KFbxSdkManager& pManager, KFbxImporter& pImporter, int pSubID, int pPluginID);

	/** \internal Helper typedef for passing KFbxIOSettings creator function as argument (used internally) */
    typedef void				(*IOSettingsFillerFuncType)(KFbxIOSettings& pIOS);

	/** \internal Helper typedef for passing KInfoRequest function as argument (used internally) */
    typedef void*				(*GetInfoFuncType)(KInfoRequest pRequest, int pReaderTypeId);

	/** Returns the file version.
      *	\param pMajor       Major version.
      *	\param pMinor       Minor version.
      *	\param pRevision    Revision version.
      */
	virtual void				GetVersion(int& pMajor, int& pMinor, int& pRevision){ pMajor = pMinor = pRevision = 0; }

	/** Opens the file with default flag
      *	\param pFileName     Name of the File to open
      * \return				 If the file opens successfully return \c true, otherwise return \c false.
      */
    virtual bool				FileOpen(char* pFileName) = 0;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
	 /** Opens the file with KFile handle
      *	\param pFile     The KFile handle
      * \return          If the file opens successfully return \c true, otherwise return \c false.
      */
    virtual bool				FileOpen(KFile * pFile);
#endif 

	/** Closes the file stream
      * \return  \c false
      */
    virtual bool				FileClose() = 0;

	 /** Checks if the file stream is open.
      *	\return  \c false.
      */
    virtual bool				IsFileOpen() = 0;

	/** Returns file stream options
      *	\param pParseFileAsNeeded       Sets whether to parse file as read options
      * \return                         true on success, otherwise return false. 
      */
    virtual bool                GetReadOptions(bool pParseFileAsNeeded = true) = 0;

	/** Reads file with stream options
      *	\param pDocument        KFbxDocument to store the file data
      *	\return \c false.
      */
    virtual bool				Read(KFbxDocument* pDocument) = 0;

	/** Reads extension plug-ins name, version and parameters, so that we can remember if a plug-in was used during export.
	  * This is especially useful for extension plug-ins that modify the scene and also to warn users during import if an
	  * extension plug-in was used that could be missing.
      * \param pParams The parameters of the extension plug-in. The properties of the objects are used
	  * as the parameters of the extension plug-in.
	  * \remark This function has no implementation in this class. Only sub-class should implement it as needed. For example,
	  * FBX 6 and FBX 7 does implement it.
      */
	virtual void				PluginReadParameters(KFbxObject& pParams);

	/** Opens the file with specific EFileOpenSpecialFlags
      * \param pFileName     Name of the File to open.
      * \param pFlags        The EFileOpenSpecialFlags to open with
      * \return If the file opens successfully return true, otherwise return false.
      */
    virtual bool				FileOpen(char* pFileName, EFileOpenSpecialFlags pFlags){ return FileOpen(pFileName); }

	/** Returns the system axis information and file system units from the file
      *	\param pAxisSystem      Axis system in file
      * \param pSystemUnits     System unit in file
      * \return \c false.
      */
    virtual bool				GetAxisInfo(KFbxAxisSystem* pAxisSystem, KFbxSystemUnit* pSystemUnits){ return false; }

	/** Returns statistics from the file 
      *	\param pStats			Statistics in the file.
      *	\return \c false.
      */
    virtual bool				GetStatistics(KFbxStatistics* pStats){ return false; }

	/** Get FBX file time mode read from GlobalSettings in FBX 6.n and FBX 7.n
	  *	\param pTimeMode  ref to a KTime::ETimeMode enum	
	  *	\return     \c true on success, \c false otherwise.
	  *	\remarks    This function must be called after KFbxImporter::Initialize().
	  *             Can be used for statistics (via GlobalSettings) before loading the whole scene from the file.
	  */
	virtual bool                GetFrameRate(KTime::ETimeMode &pTimeMode) { pTimeMode = KTime::eDEFAULT_MODE; return false; }


    /** Returns the scene info from the file.
      * \return NULL.
      */
    virtual KFbxDocumentInfo*   GetSceneInfo(){return NULL;}

    /** Returns the list of take infos from the file.	
      * \return NULL
      */
    virtual KArrayTemplate<KFbxTakeInfo*>* GetTakeInfo(){return NULL;}

	/** If default camera resolution is OK, returns information about the resolution of the render.
      *	\param pCamName			Default camera name.
	  *	\param pResolutionMode	Default resolution mode.
	  *	\param pW				Default resolution width.
	  *	\param pH				Default resolution height.
      *	\return \c true If default camera resolution is OK, \c false  Otherwise.
      */
	virtual bool	  		    GetDefaultRenderResolution(KString& pCamName, KString& pResolutionMode, double& pW, double& pH);

	/** Judges if the format of the file is was created by an Autodesk plug-in.
	  * An internal (genuine) plug-in is one created by the Autodesk FBX product team.
      *	\return \c true If the file format is internal plug-in , \c false Otherwise.
      */
	bool						IsGenuine();

	/** Retrieves error objects.
      * \return Reference to error objects.
      */
    KError&						GetError();

	/** Returns the ID of the last error that occurred when the file was read.
      * \return Last error ID.
      */
    EError						GetLastErrorID() const;

	/** Returns the error string that describes the last error that occurred when the file was read.
      * \return A text description of the last error.
      */
    const char*					GetLastErrorString() const;

	/** Returns a warning message that describes what occurred when the file was read.
      *	\param pMessage			The returned warning message.
      */
    void						GetMessage(KString& pMessage) const;

	/** Returns a warning message that describes what occurred when the file was read.
      *	\return The warning message.
      */
    KString&					GetMessage();

	/** Clears the warning message string.      */
    void						ClearMessage();

    /** Access to a IOSettings object.
      * \return A pointer to IOSettings used for this reader or NULL if the object
      * has not been allocated.
    */
	virtual KFbxIOSettings * GetIOSettings();

	/** Set the IOSettings pointer to be used for this reader instance.
	  * \param pIOSettings  
	  */
	virtual void SetIOSettings(KFbxIOSettings * pIOSettings);

    /** Pass a progress handler to the reader.
      * \param pProgress     KFbxProgress to store the progress information.
      */
    virtual void SetProgressHandler(KFbxProgress *pProgress){}

	virtual void SetEmbeddingExtractionFolder(const char* pExtractFolder){}

protected:
    /**
     * Set resolution information for default camera.
     * \param pCamName          The default camera name.
     * \param pResolutionMode   Resolution mode. ex: "Fixed Resolution","Fixed Ratio","Fixed Width","Fixed Height","Window Size".
     * \param pW                Resolution width.
     * \param pH                Resolution height.
     */
	void						SetDefaultRenderResolution(const char* pCamName, const char* pResolutionMode, double pW, double pH);
	
	//! A callback function called by FBX before the FBX scene is read.
    void						PluginsReadBegin(KFbxScene& pScene);
	/**
     * A callback function before read any FBX object.
     * \param pName     Object name.
     * \param pVersion  Version of object. 
     */
    void						PluginsRead(const char* pName, const char* pVersion);
	//! A callback function called by FBX after read the FBX scene.
    void						PluginsReadEnd(KFbxScene& pScene);

    KFbxReader&					operator=(KFbxReader const&) { return *this; }
    /**
     * Check if there is any node under pRootNode that have the same name in pDuplicateNodeNameList
     * \param pRootNode                 Root node. The node and its children will be checked recursively.
     * \param pDuplicatedNodeNameList  Name list.
     */
    virtual bool				CheckDuplicateNodeNames(KFbxNode* pRootNode, KString& pDuplicateNodeNameList);

    //! FBX SDK manager.
    KFbxSdkManager&				mManager;
    //! Resolution data for the active camera
    KDefaultRenderResolution*	mData;

private:
    KError						mError;
    KString						mMessage;
	int							mInternalID;
	KFbxIOSettings *            mIOSettings;

	friend struct KFbxReaderFbx7Impl;
};

//! Helper to access the IOSetting object pointer as a ref ex: IOS_REF.GetBoolProp( ... );
#define IOS_REF (*GetIOSettings())

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXIO_KFBXREADER_H

