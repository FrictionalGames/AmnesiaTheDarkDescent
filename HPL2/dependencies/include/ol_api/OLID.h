/*************************************************************************************
			Copyright(c)   2009 - 2011 OnLive, Inc.  All Rights Reserved.

NOTICE TO USER:

This code contains confidential material proprietary to OnLive, Inc. Your access to
and use of these confidential materials is subject to the terms and conditions of
your confidentiality and SDK license agreements with OnLive. This document and
information and ideas herein may not be disclosed, copied, reproduced or distributed
to anyone outside your company without the prior written consent of OnLive.

You may not modify, reverse engineer, or otherwise attempt to use this code for
purposes not specified in your SDK license agreement with OnLive. This material,
including but not limited to documentation and related code, is protected by U.S.
and international copyright laws and other intellectual property laws worldwide
including, but not limited to, U.S. and international patents and patents pending.
OnLive, MicroConsole, Brag Clips, Mova, Contour, the OnLive logo and the Mova logo
are trademarks or registered trademarks of OnLive, Inc. The OnLive logo and the Mova
logo are copyrights or registered copyrights of OnLive, Inc. All other trademarks
and other intellectual property assets are the property of their respective owners
and are used by permission. The furnishing of these materials does not give you any
license to said intellectual property.

THIS SOFTWARE IS PROVIDED BY ONLIVE "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*************************************************************************************/
//
// OnLive Service Platform API
// OLID.h $Revision: 78145 $
//
/// \file OLID.h

#ifndef	OLID_H
#define	OLID_H

namespace olapi
{
	/// \addtogroup OLID
	/// 
	
	/// \brief Used to indicate an invalid OnLive ID (both the ID and type are invalid).
	#define OLID_INVALID		(olapi::OLID())
	/// \brief Buffer size for encoding an OLID ID as a 22 char URI compliant string (+1 for nul char).
	///			See \ref olapi::OLID::getIDString and \ref olapi::OLID_base::encode22Char.
	#define OLID_STRING_SIZE	(23)


	#pragma warning(push)
	
	
	///\brief nonstandard extension used: specifying underlying type for enum.
	#pragma warning(disable : 4480) 

	///
	///	\brief Identifies the type of an OnLive ID.  See \ref olapi::OLID.
	///
	/// \note OL_XXX_ID_BASE offsets are used to group the ID types, they are not ID types themselves.
	///
	enum OLIDTypes : short
	{
		OL_SERVICE_ID_BASE			= 0,			///< IDs belonging to OLService:\n
		OL_INVALID_ID =	0,							///< Invalid ID.
		OL_CONTAINER_ID,							///< A container ID, see \ref OLSERVICE_API_CONTAINERS.
		OL_CONTAINER_LIST_ID,						///< Represents a list of container IDs.
		OL_SESSION_ID_BASE			= 0x100,		///< IDs belonging to OLAppSession or OLUserSession:\n
		OL_SESSION_ID,								///< A session ID; could be a user session or an application session.
		OL_OBJECT_ID_BASE			= 0x200,		///< IDs belonging to objects:\n
		OL_IDENTITY_ID,								///< References the player object.
		OL_TITLE_ID,								///< References the title or application object.
		OL_FRIEND_ID,								///< References friends.
		OL_PRODUCT_ID,								///< References the product for purchasing.
		OL_PRICING_PLAN_ID,							///< References the pricing plan of a product for purchasing.
		// SDK Version 1.063 addition
		OL_APPLICATION_ID,							///< References the application instance ID.
		OL_ACHIEVEMENT_ID,							///< References Achievements.
		OL_ACHIEVEMENT_ONEOFSET_ID,					///< References OneOfSet items in an Achievement.
		OL_MEDIA_ID_BASE			= 0x300,		///< IDs belonging to Media:\n
		OL_STREAM_ID,								///< ID for a (media) stream. 
		OL_BRAG_CLIP_ID,							///< ID for a Brag Clip.
		OL_NOTE_ID,      							///< ID for a note.
		OL_CONTENT_ID_BASE			= 0x400,		///< IDs belonging to Content:\n
		OL_CONTENT_ID,								///< ID for a Content Add-on
	};

	#pragma warning(pop)

	#pragma pack(push)
	#pragma pack(1)

	///
	/// \class OLID_base
	/// \brief OnLive ID interface base class.  Do not modify.
	///
	/// The OnLive ID is used throughout the SDK to track various objects sent by the OSP.
	/// Internally, each OLID contains a unique 132 bit ID and a type enum (see \ref olapi::OLIDTypes).
	///
	/// \warning The OLID_base class is not to be modified in any way.
	///
	struct OLID_base
	{
		public:
		
		/// \name low-level OLID_base ID assignment/modification
		/// 

		///
		/// \brief Create a new unique 132 bit ID; the type is not altered.
		///
		__declspec(dllexport) void __cdecl generate();

		/// 
		/// \name low-level OLID_base ID accessors
		/// 

		///
		/// \brief Retrieves the low 64 bits of the 132 bit ID.
		/// \return Returns the low 64 bits of the 132 bit ID.
		///
		__declspec(dllexport) unsigned __int64 __cdecl getLow() const;
		

		///
		/// \brief Retrieves the high 64 bits of the 132 bit ID.  Note: Technically, 
		///		these are the middle 64 bits; see \ref getHigher().
		///
		/// \return Returns the high 64 bits of the 132 bit ID (technically, these are the middle bits).
		///
		__declspec(dllexport) unsigned __int64 __cdecl getHigh() const;
		

		///
		/// \brief Retrieves the highest 4 bits of the 132 bit ID.
		///
		/// Retrieves the highest 4 bits of an ID.  Since IDs are 22 character
		/// URI encoded, each character represents 6 bits of the ID, at 22 characters
		/// the total ID size is 132 bits.
		///
		/// \return Returns the highest 4 bits of the 132 bit ID.
		///
		__declspec(dllexport) unsigned short __cdecl getHigher() const;

		/// 
		/// \name low-level OLID_base methods to encode/decode an ID as a string
		/// 

		///
		/// \brief Set the URI string encoding size of the ID
		///
		/// ID strings are stored in an internal structure, this function allows
		/// us to cache the URI encoded string size (if less than 22 chars).
		///
		/// \param[in] size			The size of the ID string.
		///
		__declspec(dllexport) void __cdecl setSize(unsigned char size);


		///
		/// \brief Get the previously set URI string encoding size of the ID
		///
		/// ID strings are stored in an internal structure, this function allows
		/// us to cache the URI encoded string size (if less than 22 chars).
		///
		/// \return Returns the previously set URI string encoding size of the ID.
		///
		__declspec(dllexport) unsigned char __cdecl getSize() const;

		/// 
		/// \name low-level OLID_base type methods
		/// 

		///
		/// \brief Return the OLID's type enum.
		/// \return Returns the OLID's type enum.
		///
		__declspec(dllexport) OLIDTypes __cdecl getType() const;
		

		///
		/// \brief Set the OLID's type enum.
		///
		///		\param[in] type			The type for this OnLive ID.
		///
		__declspec(dllexport) void __cdecl setType(OLIDTypes type);
		
		/// \name low-level OLID_base ID assignment/modification
		/// 

		///
		/// \brief Reset this OLID's 132 bit ID back to the invalid value (0). 
		///
		__declspec(dllexport) void __cdecl init();


		///
		/// \brief Set this OLID's 132 bit ID to the supplied raw values.
		///
		/// \param[in] lo		The ID's low 64 bits, see \ref getLow().
		///
		/// \param[in] hi		The ID's high 64 bits, see \ref getHigh().
		///
		/// \param[in] higher	The ID's highest 4 bits, see \ref getHigher().
		///
		__declspec(dllexport) void __cdecl assign(unsigned __int64 lo, unsigned __int64 hi, unsigned char higher);


		///
		/// \brief Set this OLID's ID to the supplied OLID's ID.  NOTE: The type is not altered.
		///
		/// \param[in] otherId	This OLID's 132 bit is set to otherId's value.
		///
		__declspec(dllexport) void __cdecl assign(const OLID_base &otherId);
		
		/// 
		/// \name low-level OLAPI_base comparison methods
		/// 

		///
		/// \brief Return true if this OLID's ID is equal to the supplied raw ID bits.
		///
		/// \param[in] lo		An ID's low 64 bits, see \ref getLow().
		///
		/// \param[in] hi		An ID's high 64 bits, see \ref getHigh().
		///
		/// \param[in] higher	An ID's highest 4 bits, see \ref getHigher().
		///
		/// \return Returns true if the OLID's ID is equal to the supplied raw ID bits.
		///
		__declspec(dllexport) bool __cdecl compare(unsigned __int64 lo, unsigned __int64 hi, unsigned char higher) const;


		///
		/// \brief Return true if this OLID's ID is equal to the supplied OLID's ID.  NOTE: The type of the OLIDs are not compared.
		///
		/// \param[in] otherId	The other ID to compare against.
		///
		/// \return Returns true if this OLID's ID is equal to the supplied OLID's ID.  The OLIDs types are not compared.
		///
		__declspec(dllexport) bool __cdecl compare(const OLID_base &otherId) const;


		///
		/// \brief Return true if this OLID's ID is greater than the supplied raw ID bits.
		///
		/// \param[in] lo		An ID's low 64 bits, see \ref getLow().
		///
		/// \param[in] hi		An ID's high 64 bits, see \ref getHigh().
		///
		/// \param[in] higher	An ID's highest 4 bits, see \ref getHigher().
		///
		/// \return Returns true if this OLID's ID is greater than the supplied raw ID bits.
		///
		__declspec(dllexport) bool __cdecl comparegreater(unsigned __int64 lo, unsigned __int64 hi, 
														  unsigned char higher) const;


		///
		/// \brief Return true if this OLID's ID is greater than the supplied OLID's ID.    The OLID types are not compared.
		///
		/// \param[in] otherId	The other ID to compare against.
		///
		/// \return Returns true if this OLID's ID is greater than the other OLID's ID.  The OLID types are not compared.
		///
		__declspec(dllexport) bool __cdecl comparegreater(const OLID_base &otherId) const;


		///
		/// \brief Return true if this OLID's ID is less than the supplied raw ID bits.
		///
		/// \param[in] lo		An ID's low 64 bits, see \ref getLow().
		///
		/// \param[in] hi		An ID's high 64 bits, see \ref getHigh().
		///
		/// \param[in] higher	An ID's highest 4 bits, see \ref getHigher().
		///
		/// \return Returns true if this OLID's ID is less than the supplied raw ID bits.
		///
		__declspec(dllexport) bool __cdecl compareless(unsigned __int64 lo, unsigned __int64 hi, 
													   unsigned char higher) const;


		///
		/// \brief Return true if this OLID's ID is less than the supplied OLID's ID.   The OLID types are not compared.
		///
		/// \param[in] otherId	The other ID to compare against.
		///
		/// \return Returns true if this OLID's ID is less than the supplied OLID's ID.   The OLID types are not compared.
		///
		__declspec(dllexport) bool __cdecl compareless(const OLID_base &otherId) const;

		/// 
		/// \name Low-level OLID_base methods to encode/decode an ID as a string
		/// 

		///
		/// \brief Encode the OLID's ID into the supplied buffer as a nul terminated URI-safe string.
		///			Note: This does not encode the OLID's type enum.
		///
		/// See \ref decode22Char.
		///
		/// \param[out]	buf		An \ref OLID_STRING_SIZE buffer (23 chars) to write the URI-safe ID string into.
		///
		/// \return Returns the supplied buffer for convenience (containing the nul terminated string).
		///
		__declspec(dllexport) char *__cdecl encode22Char(char *buf);
		
		
		///
		/// \brief Set this OLID's ID from the supplied URI-safe string encoding of an ID.
		///			Note: This does not modify the OLID's type.
		///
		/// See \ref encode22Char.
		///
		/// \param[out]	buf		The buffer to read the URI-safe ID string from.
		///
		__declspec(dllexport) void __cdecl decode22Char(const char *buf);

		/// 

		private:

		unsigned __int64 m_hi;			///> Used to store the high 64 bits of the 132 bit ID.
		unsigned __int64 m_lo;			///> Used to store the low 64 bits of the 132 bit ID.
		unsigned char m_hihi;			///> Used to store the highest 4 bits of the 132 bit ID.
		unsigned char m_osize;			///> Used to store the input string size (if less than 22).
		
		OLIDTypes m_type;				///> Used to store the ID Type.
	};

	///
	/// \class OLID
	/// \brief The OLID (OnLive ID) is a generic ID used to represent a variety of objects
	///		in the OnLive SDK and/or OSP. Each OLID consists of a unique 132 bit ID and a type enumeration.
	///
	/// Note: Some ID types only require a 128 bit ID.
	///
	/// An OLID supports the follow operator overloads:
	///
	/// \code
	///		OLID olid_a, olid_b;
	///
	///		// assignment operator
	///		olid_a = olid_b;
	///		
	///		// you can compare an OLID's type against an \ref olapi::OLIDTypes enum value:
	///		olid_b == OL_CONTAINER_ID;
	///		olid_b != OL_SESSION_ID;
	///
	///		// you can compare an OLID's ID against another OLID's ID (ignoring the OLID type values):
	///		olid_a == olid_b; // compares only the 132 bit IDs.
	///		olid_a != olid_b; // compares only the 132 bit IDs.
	///		olid_a < olid_b;  // compares only the 132 bit IDs.
	///		olid_a > olid_b;  // compares only the 132 bit IDs.
	///		!olid_a; // returns true if the 132 bit ID is valid (non-zero).
	/// \endcode
	///
	/// \note The OLID class can be modified as long as no member variables or virtual functions
	/// are added.
	///
	struct OLID : public OLID_base
	{
		public:
		
		/// \name OLID Constructors
		/// 

		///
		/// \brief The default constructor generates an invalid OLID.  The ID is zero and
		///			the type is OL_INVALID_ID.  See the \#define \ref OLID_INVALID.
		///
		inline OLID()								
		{
			init();
		};

		///
		/// \brief Generate an OLID with a zeroed out ID and invalid type.  This allows
		///			"someOLID = 0" to reset the olid's ID and value.
		///
		/// This constructor is used to initialize list items to 0, and for "if(OLID == 0)"
		///
		/// Example: idlist[pos] = 0;
		///
		///		\param[in] value			This value should be 0.
		///
		inline OLID(const int value)
		{
			value;
			init();
		};
		
		///
		/// \brief Create a valid 128-bit ID.
		///
		///		\param[in] lo				This is the low 64 bits of the ID.
		///
		///		\param[in] hi				This is the high 64 bits of the ID.
		///
		///		\param[in] type				ID Type to create.
		///
		inline OLID(unsigned __int64 lo, unsigned __int64 hi, OLIDTypes type)
		{
			assign(lo, hi, 0);
			setType(type);
			setSize(0);
		};

		///
		/// \brief Generate a unique 132 bit ID of the supplied type.
		///
		///		\param[in] type				ID Type to create.
		///
		inline OLID(OLIDTypes type)
		{
			generate();
			setType(type);
		};
		
		///
		/// \brief Create an OLID with the supplied type and a URI-safe string ID value.
		///			See \ref olapi::OLID_base::encode22Char.
		///
		///		\param[in] idString			22 character URI ID String; see \ref olapi::OLID_base::encode22Char.
		///
		///		\param[in] type				ID Type to create.
		///
		inline OLID(char *idString, OLIDTypes type)
		{
			init();

			if (idString)
			{
				decode22Char(idString);
				setType(type);
			}
		};

		///
		/// \brief Create an OLID with the supplied type and a URI-safe string ID value.
		///			See \ref olapi::OLID_base::encode22Char.
		///
		///		\param[in] idString			22 character URI ID String; see \ref olapi::OLID_base::encode22Char.
		///
		///		\param[in] type				ID Type to create.
		///
		inline OLID(const char *idString, OLIDTypes type)
		{
			init();

			if (idString)
			{
				decode22Char(idString);
				setType(type);
			}
		};

		/// 

		///
		/// \brief Returns true if the supplied idString is URI compliant.  See \ref getIDString.
		///
		/// Checks each character in the String ID. The String ID must only use the characters from A to Z,
		///	a to z, 0 to 9, hyphen, and underscore. The length of String ID must not exceed 22 characters. If the String ID
		///	is 22 characters long, then it must start with 'a' or 'b'.
		///
		///
		///		\param[in] idString			The string ID to check for URI compliance.
		///
		/// \return Returns true if the supplied idString is URI compliant.  See \ref getIDString. 
		///
		static bool isValid(const char* idString)
		{
			if (idString)
			{
				int len = 0; while (idString[len]) len++;

				if (len > 22) return false;

				if (len == 22)
				{
					if ( !(idString[0] == 'a' || idString[0] == 'b' ) )
						return false;
				}

				for (unsigned char i = 0; i < len; i++) 
				{
					char bit = idString[i];
					if ( !((bit >= 'a' && bit <= 'z') ||
						(bit >= 'A' && bit <= 'Z') || 
						(bit >= '0' && bit <= '9') || 
						(bit == '-') || 
						(bit == '_')) )
					{
						//bad character 
						return false;
					}
				}

				return true;
			}
			return false;
		}
		
		/// \name OLID assignment operator
		/// 

		///
		/// \brief OLID assignment operator.  This operator allows you to assign one OLID to 
		///		another.
		///
		/// Example:
		///				OLID a, b;
		///				a = OLID(OL_IDENTITY_ID);
		///				b = a;
		///
		///		\param[in] otherOLID	This OLID's type and ID are set to the other OLID's values.
		///
		///		\return	Returns a reference to this, allowing for chained assignments (a = b = c);
		///
		inline OLID& OLID::operator=(const OLID &otherOLID)
		{
			assign(otherOLID);
			setSize(otherOLID.getSize());
			setType(otherOLID.getType());
			return *this;
		};
		
		/// 
		/// \name Operator overloads to compare an OLID's type
		/// 

		///
		/// \brief OLID type equality operator overload. Returns true if this OLID's type
		///		equals the supplied \ref OLIDTypes enum.
		///
		/// Example:
		///				OLID a;
		///				a = OLID(OL_TITLE_ID);	// Create a title ID
		///				if (a == OL_TITLE_ID) printf("TITLE ID"); // Compare against title ID
		///
		///		\param[in] type			ID type to compare this OLID's type against.
		///
		///		\return Returns true if this OLID's type equals the supplied type.
		///
		inline bool OLID::operator==(const OLIDTypes type) const
		{
			return (this->getType() == type);
		}
		
		///
		/// \brief OLID type inequality operator overload.  Returns true if this OLID's type
		///		does not equal the supplied \ref OLIDTypes enum.
		///
		/// Example:
		///				OLID a;
		///				a = OLID(OL_TITLE_ID);	// Create a title ID
		///				if (a != OL_INVALID_ID) printf("VALID ID"); // Is the ID valid...
		///
		///		\param[in] type			ID type to compare against
		///
		///		\return Returns true if this OLID's type does not equal the supplied type.
		///
		inline bool OLID::operator!=(const OLIDTypes type) const
		{
			return (this->getType() != type);
		}
		
		/// 
		/// \name Operator overloads to compare an OLID's 132 bit ID.
		/// 

		///
		/// \brief OLID ID inequality operator.  Returns true if this OLID's ID does not equal
		///		the other OLID's ID value.  The OLID types are not compared.
		///
		/// Example:
		///				OLID a, b;
		///				a = OLID(OL_TITLE_ID);	// Create an ID
		///				b = OLID(OL_TITLE_ID);	// Create an ID
		///				if (a != b) printf("ID's do NOT Match"); // Compare IDs
		///
		///		\param[in] otherOLID		OLID to compare IDs against.
		///
		///		\return Returns true if this OLID's ID does not equal the other OLID's value.  
		///				The OLID types are not compared.
		///
		inline bool OLID::operator!=(const OLID& otherOLID) const
		{
			return !(this->compare(otherOLID));
		}

		///
		/// \brief OLID ID equality operator.  Returns true if this OLID's ID equals
		///		the other OLID's ID value.  The OLID types are not compared.
		///
		/// Example:
		///				OLID a, b;
		///				a = OLID(OL_TITLE_ID);	// Create an ID
		///				b = OLID(OL_TITLE_ID);	// Create an ID
		///				if (a == b) printf("ID's Match"); // Compare IDs
		///
		///		\param[in] otherOLID		OLID to compare IDs against.
		///
		///		\return Returns true if this OLID's ID equals the other OLID's value.  
		///				The OLID types are not compared.
		///
		inline bool OLID::operator==(const OLID& otherOLID) const
		{
			return this->compare(otherOLID);
		}

		///
		/// \brief OLID ID lessThan operator.  Returns true if this OLID's ID is less than
		///		the other OLID's ID value.  The OLID types are not compared.
		///
		///		\param[in] otherOLID		OLID to compare IDs against.
		///
		///		\return Returns true if this OLID's ID is less than the other OLID's value.  
		///				The OLID types are not compared.
		///
		inline bool OLID::operator<(const OLID& otherOLID) const
		{
			return this->compareless(otherOLID);
		}

		///
		/// \brief OLID ID greaterThan operator.  Returns true if this OLID's ID is greater than
		///		the other OLID's ID value.  The OLID types are not compared.
		///
		///		\param[in] otherOLID		OLID to compare IDs against.
		///
		///		\return Returns true if this OLID's ID is greater than the other OLID's value.  
		///				The OLID types are not compared.
		///
		inline bool OLID::operator>(const OLID& otherOLID) const
		{
			return this->comparegreater(otherOLID);
		}
		
		///
		/// \brief OLID ID not operator.  Returns true if this OLID's ID is valid (non-zero).
		///			The OLID's type is not examined.
		///
		///		\return Returns true if this OLID's ID is valid (non-zero).
		///
		inline bool OLID::operator!( ) const
		{
			return this->compare(0, 0, 0);
		}	
		
		/// 
		/// \name OLID encoding/decoding the OLID's 132 bit ID as a URI-compliant string.
		/// 

		///
		///	\brief Encodes the OLID's ID as a 22 character string buffer (URI encoded).  The string
		///			is written into the supplied buffer and also returned.
		///
		///	See \ref setID.
		///
		/// \param[out]	buf		An \ref OLID_STRING_SIZE buffer (23 chars) to write the URI-safe ID string into.
		///
		/// \return Returns the supplied buffer for convenience (containing the nul terminated string).
		///
		char *getIDString(char *buf)
		{
			if (buf)
			{
				encode22Char( buf );
			}
			
			return buf;
		}
		
		///
		/// \brief Set this OLID's ID from the supplied URI-safe string encoding of an ID.
		///			Note: This does not modify the OLID's type.
		///
		/// See \ref getIDString.
		///
		/// \param[out]	idString		The buffer to read the URI-safe ID string from.
		///
		void setID(char *idString)
		{
			if (idString)
			{
				decode22Char( idString );
			}
		}

		/// 

	};

	#pragma pack(pop)
	
	/// 
}

/// \brief This is a compile-time check on the size of an OLID to detect
///		modifications that would render it incompatible with the OnLive SDK DLLs.
///
///	Note this will not catch modifications to the class's vtable.

#define CHECK_OLID_SIZE(expr) typedef char __static_assert_olid_t[ (expr) ]
CHECK_OLID_SIZE(sizeof(olapi::OLID) == 20); ///< Will error with C2466 if expression fails


/*!
\addtogroup OLID
	\brief \copybrief olapi::OLID

*/

#endif // OLID_H
