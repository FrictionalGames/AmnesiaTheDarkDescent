/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxiopluginregistry.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXIOPLUGINREGISTRY_H
#define FBXFILESDK_KFBXPLUGINS_KFBXIOPLUGINREGISTRY_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxio/kfbxreader.h>
#include <fbxfilesdk/kfbxio/kfbxwriter.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;
class KFbxIOPluginRegistry;
class KFbxExporter;
class KFbxImporter;
class KFbxIOSettings;

/**	\brief This class serves as the registrar for file formats.
  * A file format must be registered when it is used by the FBX SDK.
  *
  * This class also lets you create and read formats other than FBX SDK native formats.
  * Users of FBX SDK can write their own plug-ins to read or write arbitrary file formats.
  * Once their plug-ins are registered in this class, FBX SDK is able to read or write
  * these file formats.
  *
  * Each KFbxSdkManager has a unique KFbxIOPluginRegistry. To get an instance of this class:
  * \code
  * KFbxIOPluginRegistry* registry = manager->GetIOPluginRegistry();
  * \endcode
  * \see KFbxSdkManager::GetIOPluginRegistry()
  */
class KFBX_DLL KFbxIOPluginRegistry
{
public:

    /** Constructor.
      */
    KFbxIOPluginRegistry();

    /** Destructor.
      */
	virtual ~KFbxIOPluginRegistry();

	/** Registers a Reader from a plug-in path.
	  *	\param pPluginPath          The plug-in path.
	  * \param pFirstPluginID       Contains the ID of the first plug-in found.
	  * \param pRegisteredCount     Contains the number of registered Readers.
	  */
	void RegisterReader(char const* pPluginPath,
						int& pFirstPluginID,
						int& pRegisteredCount);

	/** Registers a Reader.
	  *	\param pCreateF             The function that creates the Reader to be registered.
	  * \param pInfoF               The function that provides information about the Reader file format, such as the file extension and description. 
	  * \param pFirstPluginID       Contains the ID of the first plug-in found.
	  * \param pRegisteredCount     Contains the number of registered Readers.
	  * \param pIOSettingsFillerF   The function that fills the IO settings for the Reader. 
	  */
	void RegisterReader(KFbxReader::CreateFuncType pCreateF, 
						KFbxReader::GetInfoFuncType pInfoF,
						int& pFirstPluginID,
						int& pRegisteredCount,
						KFbxReader::IOSettingsFillerFuncType pIOSettingsFillerF = NULL
					   );


	/** Registers a Writer from a plug-in path
	  *	\param pPluginPath          The plug-in path.
	  * \param pFirstPluginID       Contains the ID of the first plug-in found.
	  * \param pRegisteredCount     Contains the number of registered Writers.
	  */
	void RegisterWriter(char const* pPluginPath,
						int& pFirstPluginID,
						int& pRegisteredCount);

	/** Registers a Writer.
	  *	\param pCreateF             The function that creates the Writer to be registered. 
	  * \param pInfoF               The function that provides information about the Writer file format, such as the file extension, description and version. 
	  * \param pFirstPluginID       Contains the ID of the first plug-in found.
	  * \param pRegisteredCount     Contains the number of registered Writers.
	  * \param pIOSettingsFillerF   The function that fills the IO settings for the Writer. 
	  */
	void RegisterWriter(KFbxWriter::CreateFuncType pCreateF, 
						KFbxWriter::GetInfoFuncType pInfoF,
						int& pFirstPluginID,
						int& pRegisteredCount,
						KFbxWriter::IOSettingsFillerFuncType pIOSettingsFillerF = NULL);

	/** Creates a Reader.
	*	\param pManager         The SDK Manager where the Reader is created.
	  *	\param pImporter        The importer that holds the created Reader.
	  * \param pPluginID        The Reader ID.
	  */
	KFbxReader* CreateReader(KFbxSdkManager& pManager, 
							 KFbxImporter& pImporter, 
							 int pPluginID) const;

	/** Creates a Writer.
	  * \param pManager         The SDK Manager where the Writer is created.
	  *	\param pExporter        The exporter that holds the created Writer.
	  * \param pPluginID        The Writer ID.
	  */
	KFbxWriter* CreateWriter(KFbxSdkManager& pManager, 
							 KFbxExporter& pExporter,
							 int pPluginID) const;

	/** Searches for the Reader ID by the file extension.
	  * \param pExt             The file extension.
	  *	\return                 The Reader ID if found, if not found, returns -1
	  */
	int FindReaderIDByExtension(char const* pExt) const;

	/** Searches for the Writer ID by the file extension.
	  * \param pExt             The file extension.	  
	  *	\return                 The Writer ID if found, if not found, returns -1
	  */
	int FindWriterIDByExtension(char const* pExt) const;
	
	/** Searches for the Reader ID by the file format description.
	  * \param pDesc            The file format description.	  
	  *	\return                 The Reader ID if found, if not found, returns -1
	  */
	int FindReaderIDByDescription(char const* pDesc) const;

	/** Searches for the Writer ID by the file format description.
	  * \param pDesc            The file format description.	  
	  *	\return                 The Writer ID if found, if not found, returns -1.
	  */
	int FindWriterIDByDescription(char const* pDesc) const;
	
	/** Verifies if the file format of the Reader is FBX.
	  * \param pFileFormat      The file format identifier.
	  *	\return                 \c True if the file format of the Reader is FBX, return \c false otherwise..
	  */
	bool ReaderIsFBX(int pFileFormat) const;

	/** Verifies if the file format of the Writer is FBX.
	  * \param pFileFormat      The file format identifier.
	  *	\return                 \c True if the file format of the Writer is FBX, return \c false otherwise.
	  */
	bool WriterIsFBX(int pFileFormat) const;

	/** Verifies if the file format of the Reader is genuine (internal).
      * \param pFileFormat      The file format identifier.
	  *	\return                 \c True if the file format of the Reader is FBX, DXF, 3DS, OBJ and DAE, return \c false otherwise.
	  */
	bool ReaderIsGenuine(int pFileFormat) const;

	/** Verifies if the file format of the Writer is genuine (internal).
      * \param pFileFormat      The file format identifier.
	  *	\return                 \c True if the file format of the Writer is FBX, DXF, 3DS, OBJ and DAE, return \c false otherwise.
	  */
	bool WriterIsGenuine(int pFileFormat) const;

	/** Returns the number of file formats that can be imported. 
	  *	\return     The number of importable formats.
	  */
	int GetReaderFormatCount() const;

	/** Returns the number of file formats that can be exported.
	  *	\return      The number of exportable formats.
	  * \remarks     Multiple identifiers for the same format count as 
	  *              different file formats. For example, eFBX_BINARY, eFBX_ASCII and eFBX_ENCRYPTED
	  *              are counted as three separate file formats.
	  */
	int GetWriterFormatCount() const;

	/** Returns the description of an importable file format.
	  *	\param pFileFormat     The file format identifier.
	  *	\return                A pointer to the character representation of the description.
	  */
	char const* GetReaderFormatDescription(int pFileFormat) const;

	/** Returns the description of an exportable file format.
	  *	\param pFileFormat     The file format identifier.
	  *	\return                A pointer to the character representation of the description.
	  */
	char const* GetWriterFormatDescription(int pFileFormat) const;

	/** Returns an importable file format's file extension.
	  *	\param pFileFormat     The file format identifier.
	  *	\return                A pointer to the character representation of the file extension.
	  */
	char const* GetReaderFormatExtension(int pFileFormat) const;
	
	/** Returns an exportable file format's file extension.
	  *	\param pFileFormat     The file format identifier.
	  *	\return                A pointer to the character representation of the file extension.
	  */
	char const* GetWriterFormatExtension(int pFileFormat) const;

	/** Returns a list of the writable file format versions.
	  *	\param pFileFormat     The file format identifier.
	  *	\return                A pointer to a list of user-readable strings that represent the versions.
	  */
	char const* const* GetWritableVersions(int pFileFormat) const;

	/** Detects the import (reader) file format specified for the file.
	  * \param pFileName       The file whose file format is to be determined.
	  * \param pFileFormat     It equals the file format identifier if this function returns \c true. If this function returns \c false, it is unmodified.
	  * \return                \c True if the file has been determined successfully, 
	  *                        returns \c false otherwise.
	  * \remarks               This function attempts to detect the specified file's file format based on the file extension and, 
	  *                        in some cases, its content. This function may not be able to determine all file formats.
	  *                        Use this function as a helper before calling \c SetFileFormat().
	  * \note                  The file must be unlocked (already open) for this function to succeed.
	  */
	bool DetectReaderFileFormat(const char* pFileName, int& pFileFormat) const;

	/** Detects the export (writer) file format specified for the file.
	  * \param pFileName       The file whose file format is to be determined.
	  * \param pFileFormat     It equals the file format identifier if this function returns \c true. If this function returns \c false, it is unmodified.
	  * \return                \c True if the file has been determined successfully, 
	  *                        returns \c false otherwise.
	  * \remarks               This function attempts to detect the specified file's file format based on the file extension and, 
	  *                        in some cases, its content. This function may not be able to determine all file formats.
	  *                        Use this function as a helper before calling \c SetFileFormat().
	  * \note                  The file must be unlocked (already open) for this function to succeed.
	  */
	bool DetectWriterFileFormat(const char* pFileName, int& pFileFormat) const;
	
	/** Returns the file format of the native Reader.
	  *	\return     The ID of the native Reader's file format.
	  */
	int GetNativeReaderFormat();

	/** Returns the file format of the native Writer.
	  *	\return     The ID of the native Writer's file format.
	  */
	int GetNativeWriterFormat();

	/** Fills the IO Settings for all registered readers.
	  *	\param pIOS			   The IO settings to be filled.
	  */
	void FillIOSettingsForReadersRegistered(KFbxIOSettings & pIOS);

    /** Fills the IO Settings for all registered writers.
	  *	\param pIOS			   The IO settings to be filled.
	  */
	void FillIOSettingsForWritersRegistered(KFbxIOSettings & pIOS);


///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

private:
	void RegisterInternalIOPlugins();

	struct ReaderPluginEntry
	{
		ReaderPluginEntry(char const* pExtension, char const* pDescription, KFbxReader::CreateFuncType pCreatorFunction, int pBaseID,
			KFbxReader::IOSettingsFillerFuncType pIOSettingsFillerFunction=NULL);
		
		char const*								mExtension;
		char const*								mDescription;
		KFbxReader::CreateFuncType				mCreatorFunction;
		KFbxReader::IOSettingsFillerFuncType	mIOSettingsFillerFunction;
		int										mBaseID;
		bool									mIsFBX;
		bool									mIsInternalPlugin;
	};
	
	struct WriterPluginEntry
	{
		WriterPluginEntry(char const* pExtension, char const* pDescription, char const* const* pVersions, KFbxWriter::CreateFuncType pCreatorFunction, int pBaseID,
			KFbxWriter::IOSettingsFillerFuncType pIOSettingsFillerFunction=NULL);
		
		char const*								mExtension;
		char const*								mDescription;
		char const* const*						mVersions;
		KFbxWriter::CreateFuncType				mCreatorFunction;
		KFbxWriter::IOSettingsFillerFuncType	mIOSettingsFillerFunction;
		int										mBaseID;
		bool									mIsFBX;
		bool									mIsInternalPlugin;
	};

	KArrayTemplate<ReaderPluginEntry*>	mReaders;
	KArrayTemplate<WriterPluginEntry*>	mWriters;
	int									mNativeReaderFormat;
	int									mNativeWriterFormat;
	bool								mInternalPluginMode;

#endif //DOXYGEN
};


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXIOPLUGINREGISTRY_H

