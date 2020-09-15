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

#ifndef HPL_STRING_H
#define HPL_STRING_H

#include "system/SystemTypes.h"

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {

	class cString
	{
	public:
		
		/**
		 * Converts local encoding (ascii, utf-8, etc, what ever printf and the like uses) to unicode. 
		 */
		static tWString To16Char(const tString &asString);

		/**
		 * Converts unicode to local encoding. (This can be ascii, utf-8 or strange things. This depends on platform and system setup. Keep in mind that that output might be utf-8 encoded.
		 * In any case, it is a format that things like printf, Log, etc likes, so okay to output.
		 */
		static tString To8Char(const tWString &awsString);

        /**
         *
         */
        static tWString UTF8ToWChar(const tString &asString);

		/**
		 *
		 */
		static tString S16BitToUTF8(const tWString& awsString);

		/**
		 * Gets a 16 bit string from an string of numbers
		 */
		static tWString Get16BitFromArray(const tString &asArray);

		/**
		* Get the sub string
		* \param asString The string to get the sub from. The method error check so the params are correct.
		* \param alStart The character to start at.
		* \param alCount The number of character to copy to the sub. -1 = all til end.
		* \return
		*/
		static tString Sub(const tString& asString,int alStart,int alCount=-1);
		static tWString SubW(const tWString& asString,int alStart,int alCount=-1);
		/**
		 * Get the file extension of a string
		 * \param aString extension (for example ".exe"). If none "" is returned.
		 * \return
		 */
		static tString GetFileExt(const tString& aString);
		static tWString GetFileExtW(const tWString& aString);
		/**
		 * Sets the file extension. "" removes extension, for example "test.exe" -> "test"
		 * \param aString New string
		 * \param aExt Extension, both ".exe" and "exe" works
		 * \return
		 */
		static tString SetFileExt(const tString&  aString,const tString&  aExt);
		static tWString SetFileExtW(const tWString&  aString,const tWString&  aExt);
		
		/**
		 * Gets the file filename in for example: "/files/test/this.txt"
		 * \param aString The Filename
		 * \return
		 */
		static tString GetFileName(const tString&  aString);
		static tWString GetFileNameW(const tWString&  aString);
		/**
		 * Removes the filename from a path
		 * \param aString
		 * \return
		 */
		static tString GetFilePath(const tString& aString);
		static tWString GetFilePathW(const tWString& aString);
		/**
		 * Sets the path for a file.
		 * \param aString
		 * \param aPath New path
		 * \return
		 */
		static tString SetFilePath(const tString& aString,const tString& aPath);
		static tWString SetFilePathW(const tWString& aString,const tWString& aPath);
		/**
		 * Gets the relative path from the base path to the target path
		 * \param aPath Target path
		 * \param aRelativeToPath Base path to be relative to
		 * \return
		 */
		static tString GetRelativePath(const tString& aPath, const tString& aRelativeToPath);
		static tWString GetRelativePathW(const tWString& aPath, const tWString& aRelativeToPath);

		/**
		* If string does not have a slash at the end. Add one.
		*/
		static tString AddSlashAtEnd(const tString& asPath,char alSlash='/');
		static tWString AddSlashAtEndW(const tWString& asPath,wchar_t alSlash=_W('/'));

		/**
		* Remove slash from end of string if any.
		*/
		static tString RemoveSlashAtEnd(const tString& asPath);
		static tWString RemoveSlashAtEndW(const tWString& asPath);
				
		/**
		 * Converts a string to lower case.
		 * \param aString
		 * \return
		 */
		static tString ToLowerCase(const tString& aString);
		static tWString ToLowerCaseW(const tWString& aString);

		/**
		 * Converts a string to upper case.
		 * \param aString
		 * \return
		 */
		static tString ToUpperCase(const tString& aString);
		static tWString ToUpperCaseW(const tWString& aString);

		/**
		 *
		 * \param aString The string to do the replacement on
		 * \param asOldChar The char to replace (one character only!)
		 * \param asNewChar The char to replace with (one character only!)
		 * \return
		 */
		static tString ReplaceCharTo(const tString& aString, const tString& asOldChar,const tString& asNewChar);
		static tWString ReplaceCharToW(const tWString& aString, const tWString& asOldChar,const tWString& asNewChar);

		/**
		*
		* \param aString The string to do the replacement on
		* \param asOldString The char to replace
		* \param asNewString The char to replace with
		* \return
		*/
		static tString ReplaceStringTo(const tString& aString, const tString& asOldString,const tString& asNewString);

		static tString ToString(const char* asString,tString asDefault);
		static int ToInt(const char* asString,int alDefault);
		static bool ToBool(const char* asString, bool abDefault);
		static float ToFloat(const char* asString, float afDefault);
		static cColor ToColor(const char* asString, const cColor& aDefault);
		static cVector2f ToVector2f(const char* asString, const cVector2f& avDefault);
		static cVector3f ToVector3f(const char* asString, const cVector3f& avDefault);
		static cVector2l ToVector2l(const char* asString, const cVector2l& avDefault);
		static cVector3l ToVector3l(const char* asString, const cVector3l& avDefault);
		static cMatrixf ToMatrixf(const char* asString, const cMatrixf& a_mtxDefault);


		static tString ToString(int alX, int alPaddingZeros=0);
		static tString ToString(unsigned int alX, int alPaddingZeros=0);
		static tString ToString(unsigned long alX, int alPaddingZeros=0);
		static tString ToString(float afX, int alNumDecimals=20, bool abRemoveZeroes=false);

		static tWString ToStringW(int alX, int alPaddingZeros=0);
		static tWString ToStringW(unsigned long alX, int alPaddingZeros=0);
		static tWString ToStringW(float afX, int alNumDecimals=20, bool abRemoveZeroes=false);

		/**
		 * Get a vector of ints from a string such as "1, 2, 3".
		 * Valid separators are ' ', '\n', '\t' and ','
		 * \param &asData The string
		 * \param avVec a vector the values will be appended to.
		 * \param apSeparators a pointer to a string with chars to override the default separators
		 */
		static tIntVec& GetIntVec(const tString &asData, tIntVec& avVec, tString *apSeparators=NULL);

		/**
		* Get a vector of ints from a string such as "1, 2, 3".
		* Valid separators are ' ', '\n', '\t' and ','
		* \param &asData The string
		* \param avVec a vector the values will be appended to.
		* \param apSeparators a pointer to a string with chars to override the default separators
		*/
		static tUIntVec& GetUIntVec(const tString &asData, tUIntVec& avVec, tString *apSeparators=NULL);

		/**
		* Get a vector of floats from a string such as "1, 2, 3".
		* Valid separators are ' ', '\n', '\t' and ','
		* \param &asData The string
		* \param avVec a vector the values will be appended to.
		* \param apSeparators a pointer to a string with chars to override the default separators
		*/
		static tFloatVec& GetFloatVec(const tString &asData, tFloatVec& avVec,tString *apSeparators=NULL);

		/**
		 * Get a vector of strings from a string such as "one, two, three".
		 * Valid separators are ' ', '\n', '\t' and ','
		 * \param &asData
		 * \param avVec
		 * \param apSeparators a pointer to a string with chars to override the default separators
		 */
		static tStringVec& GetStringVec(const tString &asData, tStringVec& avVec,tString *apSeparators=NULL);
		static tWStringVec& GetStringVecW(const tWString &asData, tWStringVec& avVec,tWString *apSeparators=NULL);

		/**
		 * Get a vector of Wstrings from a string such as "one, two, three".
		 * Valid separators are ' ', '\n', '\t' and ','
		 * \param &asData
		 * \param avVec
		 * \param apSeparators a pointer to a string with chars to override the default separators
		 */
		static tWStringVec& GetStringWVec(const tWString &asData, tWStringVec& avVec,tWString *apSeparators=NULL);

		/**
		 * Gets the last character of the string.
		 * \param aString
		 * \return
		 */
		static tString GetLastChar(const tString& aString);
		static tWString GetLastCharW(const tWString& aString);

		/**
		 * Get the first pos where aChar is found.
		 * \param aString
		 * \param aChar
		 * \return >=0 if string is found else -1
		 */
		static int GetFirstStringPos(const tString& aString, const tString& aChar);
		static int GetFirstStringPosW(const tWString& aString, const tWString& aChar);
		/**
		 * Get the last pos where aChar is found.
		 * \param aString
		 * \param aChar
		 * \return >=0 if string is found else -1
		 */
		static int GetLastStringPos(const tString& aString, const tString&  aChar);
		static int GetLastStringPosW(const tWString& aString, const tWString&  aChar);

		static void UIntStringToArray(unsigned int *apArray, const char* apString,int alSize);
		static void FloatStringToArray(float *apArray, const char* apString,int alSize);

		static int CountCharsInString(const tString& aString, const tString& aChar);
		static int CountCharsInStringW(const tWString& aString, const tWString& aChar);
		
		static unsigned int GetHash(const tString& asStr);
		static unsigned int GetHashW(const tWString& asStr);

		static tString GetNumericSuffix(const tString& aString, int* apIndex);
		static tWString GetNumericSuffixW(const tWString& aString, int* apIndex);

		/**
		 * Encode arbitrary data to special readable text string format (that can be used as text in XML files, etc). Size must be multiple of 4
		 */
		static void EncodeDataToTextString(const void *apData, size_t alSize, tString &asOutput);
		/**
		* Decode arbitrary data from special readable text string format (that can be used as text in XML files, etc). Size must be multiple of 4. String must be 150% of alSize
		*/
		static void DecodeDataFromTextString(const tString &asStr, void *apDest, size_t alSize);


	private:
		static tString GetFormatOptions(const tString& asDataType,int alZeroesOnLeft=0, int alNumDecimals=-1);
		static tWString GetFormatOptionsW(const tString& asDataType, int alZeroesOnLeft=0, int alNumDecimals=-1);

	};

};
#endif // HPL_STRING_H
