/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HPL_BINARY_BUFFER_H
#define HPL_BINARY_BUFFER_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {
	class cBinaryBuffer
	{
	public:
		cBinaryBuffer();
		cBinaryBuffer(const tWString& asFile);
		~cBinaryBuffer();
		

		////////////////////////////////
		// GENERAL
		////////////////////////////////

		/**
		 * Loads data from set file location. Will not work is location is not set! Loading clears any data set.
		 * \return true if loading ws ok, else false
		 */
		bool Load();
		/**
		 * Loads data from file.
		 * \return 
		 */
		bool Load(const tWString& asFile);
		/**
		 * Saves the data to set file location. Will not work is location is not set! Loading clears any data set.
		 * \return true if loading was ok, else false
		 */
		bool Save();
		/**
		 * Save data to file.
		 */
		bool Save(const tWString& asFile);

		/**
		 * Loads data from a readable text encoded data string.
		 */
		void LoadFromCharEncode(const tString& asInputData, size_t alSize);

		/**
		 * Saves current data to a readable text encoded string
		 */
		void SaveToCharEncode(tString& asOutputData);


		void SetFileLocation(const tWString& asFile){ msFile = asFile;}
		const tWString& GetFileLocation(){ return msFile;}

		/**
		* Reserves memory space, if reserving less than what the file already contains, then nothing happens.'
		* \return true if memory was found, else false
		*/
		bool Reserve(size_t alSize);
		
		/**
		 * This deletes all data and resets size and pos.
		 */
		void Clear();

		size_t GetSize(){ return mlDataSize; }
		size_t GetReservedSize(){ return mlReservedDataSize; }

		char* GetDataPointer(){ return mpData; }
		char* GetDataPointerAtPos(size_t alPos){ return &mpData[alPos]; }
		char* GetDataPointerAtCurrentPos(){ return &mpData[mlDataPos]; }

		////////////////////////////////
		// FILE SEARCHING
		////////////////////////////////

		size_t GetPos()const { return mlDataPos; }
		bool SetPos(size_t alPos);
		bool AddPos(size_t alSize);

		/**
		 * If data position has reached end of file.
		 */
		bool IsEOF(){ return mlDataPos >= mlDataSize; }

		////////////////////////////////
		// COMPRESSION
		////////////////////////////////

		/**
		* Compresses the data from apSrcData to current pos. Compression level is 0 - 9, where 0 is no compression. -1 = default (recommended basically) compression.
		* If abWriteDataSize is true, then the first 4 bytes, will be a 32bit int with the size of the compressed data.
		*/
		bool CompressAndAdd(char *apSrcData, size_t alSize, int alCompressionLevel=-1, bool abWriteDataSize=false);

		/**
		* Decompresses the data from apSrcData to current pos.
		*/
		bool DecompressAndAdd(char *apSrcData, size_t alSize);

		/**
		* Decompresses the data from a source buffer, beginning at the current position. If abSizeDataWritten is false, it stops at EOF and assumes size has not been writen. If true, it will assume that the first 4 bytes is the size of the data.
		* It also updates the current postion in the data for SrcBuffer
		*/
		bool DecompressAndAddFromBuffer(cBinaryBuffer *apSrcBuffer, bool abSizeDataWritten);

		////////////////////////////////
		// ENCRYPTION
		////////////////////////////////

		/**
		* Runs an XOR the entire buffer using simple XOR
		*/
		void XorTransform(const char* apKeyData, size_t alKeySize);
		
		////////////////////////////////
		// TESTS
		////////////////////////////////

		/**
		* Do this when the data position is where the CRC should be stored and then any subsequent data (until AddCRC_End is called) will be part of the CRC calculation.
		* This will add a 32 bit int with the CRC that is then set when AddCRC_End is called.
		*/
		void AddCRC_Begin();
		
		/**
		* This must be called after AddCRC_Begin, and after some data has been added.
		*/
		unsigned int AddCRC_End(unsigned int alKey);
				
		/**
		* Calculates the CRC from current position (if alDataPos<0) or alDataPos, to end (if alCount < 0) or according to alCount
		*/
		unsigned int GetCRC(unsigned int alKey, int alDataPos=-1, int alCount=-1);

		/**
		* This checks CRC from current position to end (if alCount<0) else according to alCount (which includes the the first 32bit CRC)
		* It assumes that the first the 32bit int is the CRC and will increase the data position to be past these.
		*/
		bool CheckInternalCRC(unsigned int alKey, int alCount=-1);

		/**
		* This checks CRC from current position to end (if alCount<0) else according to alCount
		*/
		bool CheckCRC(unsigned int alCRC, unsigned int alKey, int alCount=-1);
		
		////////////////////////////////
		// DATA INPUT
		////////////////////////////////
		void AddChar(char alX);
		void AddUnsignedChar(unsigned char alX);
		void AddBool(bool abX);
		void AddShort16(short alX);
		void AddUnsignedShort16(unsigned short alX);
		void AddInt32(int alX);
		void AddFloat32(float afX);
		void AddVector2f(const cVector2f& avX);
		void AddVector3f(const cVector3f& avX);
		void AddVector2l(const cVector2l& avX);
		void AddVector3l(const cVector3l& avX);
		void AddMatrixf(const cMatrixf& a_mtxX);
		void AddQuaternion(const cQuaternion& aqX);
		void AddColor(const cColor& avX);
		void AddString(const tString& asStr);
//      Must rethink how to store, as Mac/Linux is UTF-32 and Windows is UTF-16.  Better to store as UTF-8 on all platforms.
//      Also, must take care of Endianess for PPC
//		void AddStringW(const tWString& asStr);

		void AddCharArray(const char* apData, size_t alSize);
		void AddShort16Array(const short* apData, size_t alSize);
		void AddInt32Array(const int* apData, size_t alSize);
		void AddFloat32Array(const float* apData, size_t alSize);
		
        ////////////////////////////////
		// DATA UPDATE
		////////////////////////////////
        void SetInt32(int alX, size_t alPos);

		////////////////////////////////
		// DATA OUTPUT
		////////////////////////////////
		char GetChar();
		unsigned char GetUnsignedChar();
		bool GetBool();
		short GetShort16();
		unsigned short GetUnsignedShort16();
		int GetInt32();
		float GetFloat32();
		void GetVector2f(cVector2f *apX);
		void GetVector3f(cVector3f *apX);
		void GetVector2l(cVector2l *apX);
		void GetVector3l(cVector3l *apX);
		void GetMatrixf(cMatrixf *apX);
		void GetQuaternion(cQuaternion* apX);
		void GetColor(cColor *apX);
		void GetString( tString *apStr);
//		void GetStringW(tWString *apStr);

		void GetCharArray(char* apData, size_t alSize);
		void GetShort16Array(short* apData, size_t alSize);
		void GetInt32Array(int* apData, size_t alSize);
		void GetFloat32Array(float* apData, size_t alSize);
					        		
	private:
		/**
		 * Adds data to the buffer and will increase the pos to point at byte after written data (usually EOF). Will increase reserved size when it runs out of space.
		 * Note that this function does _not_ handle endian issues!
		 */
		void AddData(const void *apData, size_t alSize);
		/**
		* Gets data from buffer and puts in pointer, that needs to have memory allocated. Also moves forward the position in data to after the chunk of data.
		* Note that this function does _not_ handle endian issues!
		* \param *apData Pointer to data destination. Needs to be allocated in memory.
		* \param alSize Size of the data to be transfered to destination.
		* \return bool if transfer was successful, false if EOF was found before entire size of data could be read! If size is too large the position in data will move to EOF.
		*/
		bool GetData(void *apData, size_t alSize);

		void InitAndAllocData();

		tWString msFile;

		char *mpData;
		size_t mlDataPos;
		size_t mlDataSize;
		size_t mlReservedDataSize;

		size_t mlCRCStartPos;
	};

};
#endif // HPL_BINARY_BUFFER_H
