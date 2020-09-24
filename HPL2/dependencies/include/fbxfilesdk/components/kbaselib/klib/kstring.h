/*!  \file kstring.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSTRING_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSTRING_H

/**************************************************************************************

 Copyright (C) 1994 - 2010 Autodesk, Inc. and/or its licensors.
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

#include <fbxfilesdk/components/kbaselib/klib/kdebug.h>

#if defined(KARCH_DEV_MSC) && (KARCH_DEV_MSC <= 1600)
	#if defined(KARCH_ARCH_IA32)
        #define SIZE_OF_STDSTRING 32
	#else
		#define SIZE_OF_STDSTRING 48
	#endif
#elif defined(KARCH_DEV_GNUC)
	#if defined(KARCH_ARCH_IA32)
		#define SIZE_OF_STDSTRING 4
	#else
		#define SIZE_OF_STDSTRING 8
	#endif
#endif

#if !defined(SIZE_OF_STDSTRING)
    #error Unsupported platform
#endif

#define KFBX_KSTRING_OPAQUE_OBJECT_HEADER void* mOpaqueChunk[SIZE_OF_STDSTRING/sizeof(void*)]
#ifndef KFBX_KSTRING_OPAQUE_OBJECT
    #define KFBX_KSTRING_OPAQUE_OBJECT KFBX_KSTRING_OPAQUE_OBJECT_HEADER
#endif

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

//Conversions between WideChar, UTF8 and ANSI

/** Convert string from UTF8 to wide-char
  * \param inUTF8               input string
  * \param pOutWideChar         output string
  * \remark Output buffer should be release by caller
  */
KFBX_DLL void KFbxUTF8ToWidechar(const char* pInUTF8, wchar_t*& pOutWideChar);

/** Convert string from wide-char to UTF8
  * \param inWideChar            input string
  * \param pOutUTF8              output string
  * \remark Output buffer should be release by caller
  */
KFBX_DLL void KFbxWideCharToUTF8(const wchar_t* pInWideChar, char*& pOutUTF8 );

#if defined(KARCH_ENV_WIN32)
    /** Convert string from wide-char to ANSI
      * \param pInWideChar          input string
      * \param pOutUTF8             output string
      * \remark Output buffer should be release by caller
      */
    KFBX_DLL void KFbxWideCharToAnsi(const wchar_t* pInWideChar, char*& pOutANSI );

    /** Convert string from ANSI to wide-char
      * \param inANSI               input string
      * \param pOutWideChar         output string
      * \remark Output buffer should be release by caller
      */
    KFBX_DLL void KFbxAnsiToWideChar(const char* inANSI, wchar_t*& pOutWideChar );

    /** Convert string from ANSI to UTF8
      * \param inANSI               input string
      * \param outUTF8              output string
      * \remark Output buffer should be release by caller
      */
    KFBX_DLL void KFbxAnsiToUTF8( const char* inANSI, char*& outUTF8 );

    /** Convert string from UTF8 to ANSI
      * \param pInUTF8              input string
      * \param outANSI              output string
      * \remark Output buffer should be release by caller
      */
    KFBX_DLL void KFbxUTF8ToAnsi( const char* pInUTF8, char*& outANSI );
#endif

//Old macros to convert between WideChar, UTF8 and ANSI
#if defined(KARCH_ENV_WIN32)
	// DEPREACTED! Use KFbxUTF8ToWidechar instead
	#define KFBX_UTF8_to_WIDECHAR( inUTF8,out_UTF8_to_WIDECHAR ) \
		wchar_t* out_UTF8_to_WIDECHAR = NULL;\
		KFbxPointerFreeGuard out_UTF8_to_WIDECHAR##PointerFreeGuard; \
		if((char*)inUTF8 != NULL) { \
		out_UTF8_to_WIDECHAR = (wchar_t *)FbxSdkMalloc((strlen((char*)inUTF8) + (size_t)1) * sizeof(wchar_t)); \
		out_UTF8_to_WIDECHAR##PointerFreeGuard.SetPointer((void*)out_UTF8_to_WIDECHAR);\
		MultiByteToWideChar( CP_UTF8, 0, inUTF8, -1, out_UTF8_to_WIDECHAR, (int)((strlen((char*)inUTF8) + 1)) ); \
		}

    // DEPREACTED! Use KFbxWideCharToUTF8 instead
	#define KFBX_WIDECHAR_to_UTF8( inWideChar,out_WIDECHAR_to_UTF8 )  \
		char* out_WIDECHAR_to_UTF8 = NULL; \
		KFbxPointerFreeGuard out_WIDECHAR_to_UTF8##PointerFreeGuard; \
		if((wchar_t*)inWideChar != NULL) { \
		out_WIDECHAR_to_UTF8 = (char *) FbxSdkMalloc((wcslen((wchar_t *)inWideChar)+1) * sizeof(wchar_t)); \
		out_WIDECHAR_to_UTF8##PointerFreeGuard.SetPointer((void*) out_WIDECHAR_to_UTF8);\
		WideCharToMultiByte( CP_UTF8,0,inWideChar,-1,out_WIDECHAR_to_UTF8, (int)((wcslen((wchar_t *)inWideChar)+1) * sizeof(wchar_t)),NULL,NULL); \
		}

    // DEPREACTED! Use KFbxWideCharToAnsi instead
	#define KFBX_WIDECHAR_to_ANSI( inWideChar,out_WIDECHAR_to_ANSI )  \
		char* out_WIDECHAR_to_ANSI = NULL; \
		KFbxPointerFreeGuard out_WIDECHAR_to_ANSI##PointerFreeGuard; \
		if((wchar_t*)inWideChar != NULL) { \
		out_WIDECHAR_to_ANSI = (char *) FbxSdkMalloc((wcslen((wchar_t *)inWideChar)+1) * sizeof(wchar_t)); \
		out_WIDECHAR_to_ANSI##PointerFreeGuard.SetPointer((void*) out_WIDECHAR_to_ANSI); \
		WideCharToMultiByte( CP_ACP,0,inWideChar,-1,out_WIDECHAR_to_ANSI,(int)((wcslen((wchar_t *)inWideChar)+1) * sizeof(wchar_t)),NULL,NULL); \
		}

    // DEPREACTED! Use KFbxAnsiToWideChar instead
	#define KFBX_ANSI_to_WIDECHAR( inANSI,out_ANSI_to_WIDECHAR ) \
		wchar_t* out_ANSI_to_WIDECHAR = NULL; \
		KFbxPointerFreeGuard out_ANSI_to_WIDECHAR##PointerFreeGuard;\
		if((char*)inANSI != NULL) { \
		out_ANSI_to_WIDECHAR = (wchar_t *)FbxSdkMalloc((strlen((char*)inANSI)+1) * sizeof(wchar_t)); \
		out_ANSI_to_WIDECHAR##PointerFreeGuard.SetPointer((void*) out_ANSI_to_WIDECHAR); \
		MultiByteToWideChar( CP_ACP, 0, inANSI, -1, out_ANSI_to_WIDECHAR, (int)((strlen((char*)inANSI) + 1)) ); \
		}

    // DEPREACTED! Use KFbxAnsiToUTF8 instead
	#define KFBX_ANSI_to_UTF8( inANSI,outUTF8 )  \
		char* outUTF8 = NULL; \
		KFbxPointerFreeGuard outUTF8##PointerFreeGuard; \
		if((char*)inANSI != NULL){ \
		size_t temp_len = strlen((char*)inANSI) + (size_t)1;\
		outUTF8 = (char *) FbxSdkMalloc(temp_len); \
		outUTF8##PointerFreeGuard.SetPointer((void*) outUTF8);\
		wchar_t* temp_ANSI_to_UTF8_WideChar = (wchar_t *)FbxSdkMalloc(temp_len * sizeof(wchar_t)); \
		MultiByteToWideChar( CP_ACP, 0, inANSI, -1, temp_ANSI_to_UTF8_WideChar, (int)(temp_len)); \
		WideCharToMultiByte( CP_UTF8, 0, temp_ANSI_to_UTF8_WideChar, -1, outUTF8, (int)(temp_len), NULL, NULL);\
		FbxSdkFree(temp_ANSI_to_UTF8_WideChar);}\

    // DEPREACTED! Use KFbxUTF8ToAnsi instead
	#define KFBX_UTF8_to_ANSI( inUTF8,outANSI )  \
		char* outANSI = NULL; \
		KFbxPointerFreeGuard outANSI##PointerFreeGuard; \
		if((char*)inUTF8 != NULL){ \
		size_t temp_len = strlen((char*)inUTF8) + (size_t)1;\
		outANSI = (char *) FbxSdkMalloc(temp_len); \
		outANSI##PointerFreeGuard.SetPointer((void*) outANSI);\
		wchar_t* temp_UTF8_to_ANSI_WideChar = (wchar_t *)FbxSdkMalloc(temp_len * sizeof(wchar_t));\
		MultiByteToWideChar( CP_UTF8, 0, inUTF8, -1, temp_UTF8_to_ANSI_WideChar, (int)(temp_len)); \
		WideCharToMultiByte( CP_ACP, 0, temp_UTF8_to_ANSI_WideChar, -1, outANSI, (int)(temp_len), NULL, NULL);\
		FbxSdkFree(temp_UTF8_to_ANSI_WideChar);} \

    #pragma deprecated(KFBX_UTF8_to_WIDECHAR, KFBX_WIDECHAR_to_UTF8, KFBX_WIDECHAR_to_ANSI, KFBX_ANSI_to_WIDECHAR, KFBX_ANSI_to_UTF8, KFBX_UTF8_to_ANSI)
#endif

/** Utility class to manipulate strings.
* \nosubgrouping */
class KFBX_DLL KString
{
public:
	/**
	* \name Constructors and Destructor
	*/
	//@{
		//! Default constructor.
		KString();

		/** Copy constructor.
		* \param pStr The KString to be copied. */
		KString(const KString& pStr);

		/** String constructor.
		* \param pStr The string used to construct KString. */
		KString(const char* pStr);

		/** Character constructor.
		* \param pChar The character used to construct KString.
		* \param pNbRepeat The number of times to repeat the character. Default value is 1 */
		KString(char pChar, size_t pNbRepeat=1);

		/** String constructor with maximum length.
		* \param pCharPtr The string used to construct KString. 
		* \param pLength  Maximum length. */
		KString(const char* pCharPtr, size_t pLength);

		/** Integer constructor.
		* \param pValue The int value used to construct KString. */
		KString(const int pValue);

		/** Float constructor.
		* \param pValue The float value used to construct KString. */
		KString(const float pValue);

		/** Double constructor.
		* \param pValue The double value used to construct KString. */
		KString(const double pValue);

		//! Destructor.
		~KString();

		//! Destroy an allocated version of the string
		void Destroy();
	//@}

	/**
	* \name Buffer Access and Validation
	*/
	//@{
		//! Get string length like "C" strlen().
		size_t GetLen() const;

		//! Return \c true if string length equal zero.
		bool IsEmpty() const;

		//! Discard the content of the string.
		KString& Empty();

		/** Access by reference.
		* \param pIndex   The index.
		* \return The reference of the char at pIndex. */
		char& operator[](int pIndex);

		/** Access by copy.
		* \param pIndex   The index.
		* \return The char at pIndex. */
		char operator[](int pIndex) const;

		//! Non-const buffer access.
		char* Buffer();

		//! Const buffer access.
		const char* Buffer()const;
	//@}

	/**
	* \name String Operations
	*/
	//@{
		/** KString assignment operator.
		* \param pStr The KString to be assigned. */
		const KString& operator=(const KString& pStr);

		/** Character assignment operator.
		* \param pChar The character to be assigned. */
		const KString& operator=(char pChar);

		/** String assignment operator.
		* \param pStr The string to be assigned. */
		const KString& operator=(const char* pStr);

		/** Int assignment operator.
		* \param pValue The int value to be assigned. */
		const KString& operator=(int pValue);

		/** Float assignment operator.
		* \param pValue The float value to be assigned. */
		const KString& operator=(float pValue);

		/** Double assignment operator.
		* \param pValue The double value to be assigned. */
		const KString& operator=(double pValue);

		/** KString append.
		* \param pKStr  The KString to be appended. */
		const KString& operator+=(const KString& pKStr);

		/** Character append.
		* \param pChar  The character to be appended. */
		const KString& operator+=(char pChar);

		/** String append.
		* \param pStr    The string to be appended. */
		const KString& operator+=(const char* pStr);

		/** Integer append.
		* \param pValue  The int value to be appended. */
		const KString& operator+=(int pValue);

		/** Float append.
		* \param pValue  The float value to be appended. */
		const KString& operator+=(float pValue);

		/** Double append.
		* \param pValue  The double value to be appended. */
		const KString& operator+=(double pValue);

		/** Equality operator.
		* \param pStr    The KString to be compared. */
		bool operator== (const KString &pStr) const;

		/** Inequality operator.
		* \param pStr    The KString to be compared. */
		bool operator!= (const KString &pStr) const;

		/** Inferior to operator.
		* \param pStr    The KString to be compared. */
		bool operator< (const KString &pStr) const;

		/** Inferior or equal to operator.
		* \param pStr    The KString to be compared. */
		bool operator<= (const KString &pStr) const;

		/** Superior or equal to operator.
		* \param pStr    The KString to be compared. */
		bool operator>= (const KString &pStr) const;

		/** Superior to operator.
		* \param pStr    The KString to be compared. */
		bool operator> (const KString &pStr) const;

		/** Equality operator.
		* \param pStr    The string to be compared. */
		bool operator== (const char *pStr) const;

		/** Inequality operator.
		* \param pStr    The string to be compared. */
		bool operator!= (const char *pStr) const;

		/** Inferior to operator.
		* \param pStr    The string to be compared. */
		bool operator< (const char *pStr) const;

		/** Inferior or equal to operator.
		* \param pStr    The string to be compared. */
		bool operator<= (const char *pStr) const;

		/** Superior or equal to operator.
		* \param pStr    The string to be compared. */
		bool operator>= (const char *pStr) const;

		/** Superior to operator.
		* \param pStr    The string to be compared. */
		bool operator> (const char *pStr) const;

		/** KString concatenation.
		* \param pKStr1  KString 1 to be concatenated to KString 2.
		* \param pKStr2  KString 2 to be concatenated to KString 1 */
		friend KFBX_DLL KString operator+(const KString& pKStr1, const KString& pKStr2);

		/** Character concatenation.
		* \param pKStr  KString to be concatenated to Character.
		* \param pChar  Character to be concatenated to KString */
		friend KFBX_DLL KString operator+(const KString& pKStr, char pChar);

		/** String concatenation.
		* \param pKStr  KString to be concatenated to String.
		* \param pStr  String to be concatenated to KString */
		friend KFBX_DLL KString operator+(const KString& pKStr, const char* pStr);

		/** Integer concatenation.
		* \param pKStr  KString to be concatenated to Integer.
		* \param pValue  Integer to be concatenated to KString */
		friend KFBX_DLL KString operator+(const KString& pKStr, int pValue);

		/** Float concatenation.
		* \param pKStr  KString to be concatenated to Float.
		* \param pValue  Float to be concatenated to KString */
		friend KFBX_DLL KString operator+(const KString& pKStr, float pValue);

		/** Double concatenation.
		* \param pKStr  KString to be concatenated to Double.
		* \param pValue  Double to be concatenated to KString */
		friend KFBX_DLL KString operator+(const KString& pKStr, double pValue);

		//! Cast operator.
		operator const char*() const;

		/** String assignment function with maximum length.
		  * \param pStr The string to be assigned.
		  * \param pLength The maximum length of string to be assigned. */
		const KString& Copy(const char* pStr, size_t pLength);

		/** Append as "C" strncat().
		* \param pStr The string to be appended.
		* \param pLength The length of chars to be appended. */
		const KString& Append(const char* pStr, size_t pLength);

		/** Compare as "C" strcmp().
		* \param pStr    The string to be compared. */
		int Compare(const char* pStr) const;

		/** Compare as "C" stricmp().
		* \param pStr    The string to be compared. */
		int CompareNoCase(const char* pStr) const;

		/** Swap the contents of two strings.
		* \param pStr The KString to be swapped. */
		void Swap(KString& pStr);

		//! Uppercase conversion.
		KString Upper() const;

		//! Lowercase conversion.
		KString Lower() const;
	//@}

    /**
    * \name Substring Extraction
    */
    //@{
		/** Extract middle string for a given length.
		* \param pFirst The start index of KString to be extracted.
		* \param pCount The length of sub-string to be extracted. */
		KString Mid(size_t pFirst, size_t pCount) const;

		/** Extract middle string up to the end.
		* \param pFirst The start index of KString to be extracted. */
		KString Mid(size_t pFirst) const;

		/** Extract left string.
		* \param pCount The length of sub-string to be extracted. */
		KString Left(size_t pCount) const;

		/** Extract right string.
		* \param pCount The length of sub-string to be extracted. */
		KString Right(size_t pCount) const;
	//@}

	/**
	* \name Padding
	*/
	//@{
		/** \enum PaddingType      Padding types.
		* - \e eRight
		* - \e eLeft
		* - \e eBoth */
		enum PaddingType {eRight, eLeft, eBoth};

		/** Add padding characters.
		* \param pPadding The padding type.
		* \param pLen The length limit of KString after padding. 
		* \param pCar The character to be padded. */
		KString Pad(PaddingType pPadding, size_t pLen, char pCar=' ') const;

		/** Remove padding characters.
		* \param pPadding The padding type. */
		KString UnPad(PaddingType pPadding) const;
	//@}

	/**
	* \name Search
	*/
	//@{
		/** Look for a single character match, like "C" strchr().
		* \param pChar The character to look for.
		* \param pStartPosition  Start position to look for.
		* \return Index or -1 if not found. */
		int Find(char pChar, size_t pStartPosition=0) const;

		/** Look for a substring match, like "C" strstr().
		* \param pStrSub The substring to look for.
		* \param pStartPosition  Start position to look for.
		* \return Starting index or -1 if not found. */
		int Find(const char* pStrSub, size_t pStartPosition=0) const;

		/** Look for the last occurrence of character in string, like "C" strrchr().
		* \param pChar The character to look for.
		* \return Index or -1 if not found. */
		int ReverseFind(char pChar) const;

		/** Look for a single character match, like "C" strpbrk().
		* \param pStrCharSet The character set.
		* \param pStartPosition The start position.
		* \return Index or -1 if not found. */
		int FindOneOf(const char * pStrCharSet, size_t pStartPosition=0) const;

		/** Replace a substring.
		* \param pFind The substring to look for.
		* \param pReplaceBy The string to replace by.
		* \param pStartPosition The start position. 
		* \return \c true if substring found and replaced. */
		bool FindAndReplace(const char* pFind, const char* pReplaceBy, size_t pStartPosition=0);

		/** Replace all occurrence of a substring.
		* \param pFind The substring to look for.
		* \param pReplaceBy The string to replace by.
		* \return \c true if something got replaced. */
		bool ReplaceAll(const char* pFind, const char* pReplaceBy);

        /** Replace all occurrence of character to find by replacement character.
		* \param pFind The character to look for.
		* \param pReplaceBy The character to replace by.
		* \return \c true if character found and replaced. */
		bool ReplaceAll(char pFind, char pReplaceBy);
	//@}

	/**
	* \name Token Extraction
	*/
	//@{
		/** Get number of tokens.
		* \param pSpans The span
		* \return The number of tokens. */
		int GetTokenCount(const char* pSpans) const;

		/** Get token at given index.
		* \param pTokenIndex The token index.
		* \param pSpans The span */
		KString GetToken(int pTokenIndex, const char* pSpans) const;
	//@}

private:
    KFBX_KSTRING_OPAQUE_OBJECT;
};

KFBX_INCOMPATIBLE_WITH_KARRAYTEMPLATE(KString);

//! KString concatenation.
KFBX_DLL KString operator+(const KString& pKStr1, const KString& pKStr2);

//! Character concatenation.
KFBX_DLL KString operator+(const KString& pKStr, char pChar);

//! String concatenation.
KFBX_DLL KString operator+(const KString& pKStr, const char* pStr);

//! Integer concatenation.
KFBX_DLL KString operator+(const KString& pKStr, int pValue);

//! Float concatenation.
KFBX_DLL KString operator+(const KString& pKStr, float pValue);

//! Double concatenation.
KFBX_DLL KString operator+(const KString& pKStr, double pValue);

/** Functor class to compare KString, and is suitable for use in KMap. */
class KStringCompare
{
public:
	/** Compare two KStrings.
	* \param pKeyA The first KString to compare.
	* \param pKeyB The second KString to compare.
	* \return -1 indicates pKeyA is greater than pKeyB, 1 indicates pKeyB is greater than pKeyA, zero indicates they are equal.
	*/
	inline int operator()(KString const &pKeyA, KString const &pKeyB) const
	{
		return (pKeyA < pKeyB) ? -1 : ((pKeyB < pKeyA) ? 1 : 0);
	}
};

/** Functor class to compare "C" strings. */
class KCharCompare
{
public:
	//! Compare two strings like "C" strcmp().
	inline int operator()(char const* pKeyA, char const* pKeyB) const
	{
		return strcmp(pKeyA, pKeyB);
	}
};

/** Remove the given char in the given string.
* \param pString The given string.
* \param lToRemove The given char ought to be removed.
* \remarks Strings used in this function are case-sensitive. */
inline void RemoveChar(KString& pString, char lToRemove)
{
    int lPos = pString.ReverseFind(lToRemove);
    while( lPos >= 0 )
    {
        pString = pString.Left(lPos) + pString.Mid(lPos + 1);
        lPos = pString.ReverseFind(lToRemove);
    }
}

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSTRING_H
