/*!  \file kfbxstream.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSTREAM_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSTREAM_H

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

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** \internal Generic base class for streamable data.
  * Used for reducing memory usage when loading 
  * and unloading content of peripheral (KFbxPeripheral) objects.
  */
class KFbxStream
{
public:

	/** \enum eStreamState
		* - \e eCLOSED  indicate a closed stream  
		* - \e eOPENED  indicate an opened stream  
		* - \e eEMPTY   indicate an empty stream
		* \remarks Used internally to manage stream states
	    */
	typedef enum {
		eCLOSED = 0,
		eOPENED = 1,
		eEMPTY  = 2
	} eStreamState;

	/** Default constructor.
	  */
	KFbxStream() {};

	/** Destructor
	  */
	virtual ~KFbxStream() {};

	/** Query the current state of the stream.
	  */
	virtual eStreamState State() { return eCLOSED; }

	/** Open the stream.
	  * \return True if successful.
	  */
	virtual bool Open()  { return false; };

	/** Close the stream.
	  * \return True if successful.
	  */
	virtual bool Close() { return false; };

	/** Empties the internal data of the stream.
	  * \return True if successful.
	  */
	virtual bool Flush() { return false; };

	/** Writes a memory block.
	  * \param pData Pointer to the memory block to write.
	  * \param pSize Size (in bytes) of the memory block to write.
	  * \return The number of bytes written in the stream.
	  */
	virtual int Write(const void* pData, int pSize) { return 0; }

	int Write(const char* pData, int pSize) { return Write((void*)pData, pSize); }
	int Write(const int* pData, int pSize) { return Write((void*)pData, pSize); }

	/** Read bytes from the stream and store them in the memory block.
	  * \param pData Pointer to the memory block where the read bytes are stored.
	  * \param pSize Number of bytes read from the stream.
	  * \return The actual number of bytes successfully read from the stream.
	  */
	virtual int Read (void* pData, int pSize) const { return 0; }

	int Read(char* pData, int pSize) const { return Read((void*)pData, pSize); }
	int Read(int* pData, int pSize) const { return Read((void*)pData, pSize); }

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSTREAM_H

